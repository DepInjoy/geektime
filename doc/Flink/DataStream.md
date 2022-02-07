# DataStream

DataStream的map方法实现:
```
    public <R> SingleOutputStreamOperator<R> map(
            MapFunction<T, R> mapper, TypeInformation<R> outputType) {
        return transform("Map", outputType, new StreamMap<>(clean(mapper)));
    }
```
继续跟进`transform`方法
```
    @PublicEvolving
    public <R> SingleOutputStreamOperator<R> transform(
            String operatorName,
            TypeInformation<R> outTypeInfo,
            OneInputStreamOperator<T, R> operator) {

        return doTransform(operatorName, outTypeInfo, SimpleOperatorFactory.of(operator));
    }
```
```Java
   protected <R> SingleOutputStreamOperator<R> doTransform(
            String operatorName,
            TypeInformation<R> outTypeInfo,
            StreamOperatorFactory<R> operatorFactory) {

        // 1.   获取上一次操作的TypeInformation信息
        //      确定没有MissingTypeInfo错误，确保下游算子转换不会出现问题
        transformation.getOutputType();

        // 2.   创建OneInputTransformation，其中包含当前DataStream对应的上一次操作(this.transformation)
        OneInputTransformation<T, R> resultTransform =
                new OneInputTransformation<>(
                        this.transformation,
                        operatorName,
                        operatorFactory,
                        outTypeInfo,
                        environment.getParallelism());

        @SuppressWarnings({"unchecked", "rawtypes"})
        SingleOutputStreamOperator<R> returnStream =
                new SingleOutputStreamOperator(environment, resultTransform);

        // 3.   将创建的OneInputTransformation添加到StreamExecutionEnvironment的Transformation集合中
        //      便于生成StreamGraph对象(即DataStream间转换操作形成的Pipeline拓扑)
        getExecutionEnvironment().addOperator(resultTransform);

        return returnStream;
    }
```

## Transformation转换
Transformation负责描述DataStream之间的转换信息。在Transformation的基础上又抽象出了`PhysicalTransformation`类。`PhysicalTransformation`中提供了`setChainingStrategy()`方法，控制算子之间的连接，将上下游算子按照指定的策略连接。`ChainingStrategy`支持如下三种策略。
- ALWAYS：代表该`Transformation`中的算子会和上游算子尽可能地链化，最终将多个Operator组合成`OperatorChain`。`OperatorChain`中的`Operator`会运行在同一个SubTask实例中，这样做的目的主要是优化性能，减少`Operator`之间的网络传输。
- NEVER：代表该Transformation中的`Operator`永远不会和上下游算子之间链化，因此对应的`Operator`会运行在独立的`SubTask`实例中。
- HEAD：代表该`Transformation`对应的`Operator`为头部算子，不支持上游算子链化，但可以和下游算子链化，实际就是`OperatorChain`中`HeaderOperator`。
用户也可以显性调用`disableChaining()`等方法，设定不同的`ChainingStrategy`，实现对O`perator`之间物理连接的控制。

支持设定`ChainingStrategy`的`PhysicalTransformation`操作类型，也就是继承了PhysicalTransformation抽象的实现类:
- OneInputTransformation：单进单出的数据集转换操作，例如DataStream.map()转换。
- TwoInputTransformation：双进单出的数据集转换操作，例如在DataStream与DataStream之间进行Join操作，且该转换操作中的Operator类型为TwoInputStreamOperator。
- SinkTransformation：数据集输出操作，当用户调用DataStream.addSink()方法时，会同步创建SinkTransformation操作，将DataStream中的数据输出到外部系统中。
- SourceTransformation：数据集输入操作，调用DataStream.addSource()方法时，会创建SourceTransformation操作，用于从外部系统中读取数据并转换成DataStream数据集。
- SplitTransformation：数据集切分操作，用于将DataStream数据集根据指定字段进行切分，调用DataStream.split()方法时会创建SplitTransformation。

除了PhysicalTransformation之外，还有一部分转换操作直接继承自Transformation抽象类，它们本身就是物理转换操作，不支持链化操作，因此不会将其与其他算子放置在同一个SubTask中运行。例如PartitionTransformation和SelectTransformation等转换操作，这类转换操作不涉及具体的数据处理过程，仅描述上下游算子之间的数据分区。
- SelectTransformation：根据用户提供的selectedName从上游DataStream中选择需要输出到下游的数据。
- PartitionTransformation：支持对上游DataStream中的数据进行分区，分区策略通过指定的StreamPartitioner决定，例如当用户执行DataStream.rebalance()方法时，就会创建StreamPartitioner实现类RebalancePartitioner实现上下游数据的路由操作。
- UnionTransformation：用于对多个输入Transformation进行合并，最终将上游DataStream数据集中的数据合并为一个DataStream。
- SideOutputTransformation：用于根据OutputTag筛选上游DataStream中的数据并下发到下游的算子中继续处理。
- CoFeedbackTransformation：用于迭代计算中单输入反馈数据流节点的转换操作。
- FeedbackTransformation：用于迭代计算中双输入反馈数据流节点的转换操作。

## StreamOperator
Transformation结构中最主要的组成部分就是StreamOperator。

不管是OneInputStreamOperator还是TwoInputStreamOperator类型的算子，最终都会继承AbstractStreamOperator基本实现类。在调度和执行Task实例时，会通过AbstractStreamOperator提供的入口方法触发和执行Operator。同时在AbstractStreamOperator中也定义了所有算子中公共的组成部分，如StreamingRuntimeContext、OperatorStateBackend等。

StreamOperator的接口定义：
```Java
public interface StreamOperator<OUT> extends CheckpointListener, KeyContext, Serializable {

    /** ------------------------------------------------------------------------
				life cycle
    	------------------------------------------------------------------------- */
    // 当前Operator的初始化方法,在数据元素正式接入Operator运算前，Task会调用StreamOperator.open()方法对该算子初始化
    // 具体open()方法的定义由子类实现，常见的用法如调用RichFunction中的open()方法创建相应的状态变量。
    void open() throws Exception;

    // 当所有的数据元素都添加到当前Operator调用该方法
    // 刷新所有剩余的缓冲数据，保证算子中所有数据被正确处理。
    void finish() throws Exception;


    // operator生命周期结束时调用，无论算子执行成功、失败或取消，确保该算子所申请的资源都释放
    void close() throws Exception;

    /** ------------------------state snapshots------------------------------ */

    // 在StreamOperator执行checkpoint是调用，checkpoint barrier操作之前
    void prepareSnapshotPreBarrier(long checkpointId) throws Exception;

    // 当SubTask执行checkpoint操作时会调用该方法，用于触发该Operator中状态数据的快照操作。
    OperatorSnapshotFutures snapshotState(long checkpointId, long timestamp,
    	CheckpointOptions checkpointOptions, CheckpointStreamFactory storageLocation) throws Exception;

    // 当算子启动或重启时，调用该方法初始化状态数据
    void initializeState(StreamTaskStateInitializer streamTaskStateManager) throws Exception;
    

    /** ------------------------------------------------------------------------
				miscellaneous
    	------------------------------------------------------------------------ */

    void setKeyContextElement1(StreamRecord<?> record) throws Exception;

    void setKeyContextElement2(StreamRecord<?> record) throws Exception;

    OperatorMetricGroup getMetricGroup();

    OperatorID getOperatorID();
}
```

### AbstractStreamOperator实现
AbstractStreamOperator作为StreamOperator的基本实现类，所有的Operator都会继承和实现该抽象实现类。在AbstractStreamOperator中定义了Operator用到的基础方法和成员信息。


```java
public abstract class AbstractStreamOperator<OUT> implements StreamOperator<OUT>,
	SetupableStreamOperator<OUT>, CheckpointedStreamOperator, Serializable {
  		......

    /** ----------- configuration properties ------------- */

    // 用于指定Operator的上下游算子链接策略，其中ChainStrategy可以是ALWAYS、NEVER或HEAD类型
    protected ChainingStrategy chainingStrategy = ChainingStrategy.HEAD;

    /** ---------------- runtime fields ------------------ */

    // 表示当前Operator所属的StreamTask，最终会通过StreamTask中的invoke()方法执行当前StreamTask中的所有Operator
    private transient StreamTask<?, ?> container;
    // 存储了该StreamOperator的配置信息，实际上是对Configuration参数进行了封装
    protected transient StreamConfig config;
    // 定义了当前StreamOperator输出操作，执行完该算子的所有转换操作后，会通过Output组件将数据推送到下游算子继续执行
    protected transient Output<StreamRecord<OUT>> output;

    private transient IndexedCombinedWatermarkStatus combinedWatermark;
    // 主要定义了UDF执行过程中的上下文信息，例如获取累加器、状态数据。
    private transient StreamingRuntimeContext runtimeContext;

    /** ---------------- key/value state ------------------*/
    
    // 只有DataStream经过keyBy()转换操作生成KeyedStream后，才会设定该变量信息,否则为null
    private transient KeySelector<?, ?> stateKeySelector1;
    // 只在执行两个KeyedStream关联操作时使用，例如Join操作，在AbstractStreamOperator中会保存stateKeySelector2的信息。
    private transient KeySelector<?, ?> stateKeySelector2;
    // 状态后端存储
    private transient StreamOperatorStateHandler stateHandler;
    // Flink内部时间服务，和processingTimeService相似，但支持基于事件时间的时间域处理数据,还可以同时注册基于事件时间和处理时间的定时器
    // 例如在窗口、CEP等高级类型的算子中，会在ProcessFunction中通过timeServiceManager注册Timer定时器
    // 当事件时间或处理时间到达指定时间后执行Timer定时器，以实现复杂的函数计算。
    private transient InternalTimeServiceManager<?> timeServiceManager;

    /** --------------- Metrics --------------------------- */

    // 记录当前算子层面的监控指标，包括numRecordsIn、numRecordsOut、numRecordsInRate、numRecordsOutRate等
    protected transient InternalOperatorMetricGroup metrics;
    // 采集和汇报当前Operator的延时状况
    protected transient LatencyStats latencyStats;

    /** ---------------- time handler ------------------ */
    // 基于ProcessingTime的时间服务，实现ProcessingTime时间域操作，例如获取当前ProcessingTime，然后创建定时器回调等
    protected transient ProcessingTimeService processingTimeService;



    // 用于处理在SourceOperator中产生的LatencyMarker信息
    // 在当前Operator中会计算事件和LatencyMarker之间的差值，用于评估当前算子的延时程度
    public void processLatencyMarker(LatencyMarker latencyMarker) throws Exception {
        reportOrForwardLatencyMarker(latencyMarker);
    }
    
    // 用于处理接入的Watermark时间戳信息，并用最新的Watermark更新当前算子内部的时钟
    public void processWatermark(Watermark mark) throws Exception {
        if (timeServiceManager != null) {
            timeServiceManager.advanceWatermark(mark);
        }
        output.emitWatermark(mark);
    }
    
    // 提供子类获取InternalTimerService的方法，以实现不同类型的Timer注册操作
    public <K, N> InternalTimerService<N> getInternalTimerService(
            String name, TypeSerializer<N> namespaceSerializer, Triggerable<K, N> triggerable)
}
```
状态后端管理`StreamOperatorStateHandler`实现
```java
public class StreamOperatorStateHandler {
    // 存储KeyedState的状态管理后端，默认为HeapKeyedStateBackend。如果配置RocksDB作为状态存储后端，为RocksDBKeyedStateBackend。
    @Nullable private final CheckpointableKeyedStateBackend<?> keyedStateBackend;

    private final CloseableRegistry closeableRegistry;
    // 主要提供KeyedState的状态存储服务，实际上是对KeyedStateBackend进行封装并提供了不同类型的KeyedState获取方法
    @Nullable private final DefaultKeyedStateStore keyedStateStore;
    // 主要提供OperatorState对应的状态后端存储，默认OperatorStateBackend只有DefaultOperatorStateBackend实现
    private final OperatorStateBackend operatorStateBackend;
    private final StreamOperatorStateContext context;
}
```

### AbstractUdfStreamOperator基本实现
当StreamOperator涉及自定义用户函数数据转换处理时，对应的Operator会继承AbstractUdfStreamOperator抽象实现类，常见的有StreamMap、CoProcessOperator等算子。当然，并不是所有的Operator都继承自AbstractUdfStreamOperator。在Flink Table API模块实现的算子中，都会直接继承和实现AbstractStreamOperator抽象实现类。有状态查询的AbstractQueryableStateOperator也不需要使用用户自定义函数处理数据。
AbstractUdfStreamOperator继承自AbstractStreamOperator抽象类，对于AbstractUdfStreamOperator抽象类来讲，最重要的拓展就是增加了成员变量userFunction，且提供了userFunction初始化以及状态持久化的抽象方法。
```
ublic abstract class AbstractUdfStreamOperator<OUT, F extends Function>
        extends AbstractStreamOperator<OUT> implements OutputTypeConfigurable<OUT> {
	......
    
    // 用户自定义函数
    protected final F userFunction; 
	
    /** ----------------------------- operator life cycle -------------------------------- */
    @Override
    public void setup(
            StreamTask<?, ?> containingTask,
            StreamConfig config,
            Output<StreamRecord<OUT>> output) {
        super.setup(containingTask, config, output);
        // 为userFunction设定RuntimeContext
        FunctionUtils.setFunctionRuntimeContext(userFunction, getRuntimeContext());
    }

    @Override
    public void snapshotState(StateSnapshotContext context) throws Exception {
        super.snapshotState(context);
        // 对userFunction中的状态进行快照操作
        StreamingFunctionUtils.snapshotFunctionState(
                context, getOperatorStateBackend(), userFunction);
    }

    @Override
    public void initializeState(StateInitializationContext context) throws Exception {
        super.initializeState(context);
        // 初始化userFunction的状态值
        StreamingFunctionUtils.restoreFunctionState(context, userFunction);
    }

    @Override
    public void open() throws Exception {
        super.open();
        // 当用户自定义并实现RichFunction时，FunctionUtils.openFunction()方法会调用
        // RichFunction.open()方法，完成用户自定义状态的创建和初始化
        FunctionUtils.openFunction(userFunction, new Configuration());
    }

    @Override
    public void finish() throws Exception {
        super.finish();
        if (userFunction instanceof SinkFunction) {
            ((SinkFunction<?>) userFunction).finish();
        }
    }

    @Override
    public void close() throws Exception {
        super.close();
        FunctionUtils.closeFunction(userFunction);
    } 
           
}
```
当用户自定义实现Function时，在AbstractUdfStreamOperator抽象类中提供了对这些Function的初始化操作，也就实现了Operator和Function之间的关联。Operator也是Function的载体，具体数据处理操作借助Operator中的Function进行。StreamOperator提供了执行Function的环境，包括状态数据管理和处理Watermark、LatencyMarker等信息。

### StreamOperatorFactory
StreamOperator最终会通过`StreamOperatorFactory`封装在Transformation结构中，并存储在`StreamGraph`和`JobGraph`结构中，直到运行时执行StreamTask时，才会调用`StreamOperatorFactory.createStreamOperator()`方法在`StreamOperatorFactory`中创建StreamOperator实例。在DataStreamAPI中主要通过`SimpleStreamOperatorFactory`创建已经定义的Operator，而在Table API模块中主要通过`CodeGenOperatorFactory`从代码中动态编译并创建Operator实例。
DataStream API中大部分转换操作都是通过`SimpleOperatorFactory`进行封装和创建的。`SimpleStreamOperatorFactory`根据算子类型的不同，拓展出了`InputFormatOperatorFactory`、`UdfStreamOperatorFactory`和`OutputFormatOperatorFactory`三种接口实现。
- InputFormatOperatorFactory：支持创建InputFormat类型输入的StreamSource算子，即SourceFunction为InputFormatSourceFunction类型，并提供getInputFormat()方法生成StreamGraph。
- UdfStreamOperatorFactory：支持AbstractUdfStreamOperator类型的Operator创建，并且在UdfStreamOperatorFactory中提供了获取UserFunction的方法。
- OutputFormatOperatorFactory：支持创建OutputFormat类型输出的StreamSink算子，即SinkFunction为OutputFormatSinkFunction类型，并提供getOutputFormat()方法生成StreamGraph。


`SimpleOperatorFactory.of()`根据Operator类型的不同，创建不同的`SimpleOperatorFactory`实现类
```
public class SimpleOperatorFactory<OUT> extends AbstractStreamOperatorFactory<OUT> {
	
	......

    @SuppressWarnings("unchecked")
    // 根据StreamOperator创建SimpleOperatorFactory
    public static <OUT> SimpleOperatorFactory<OUT> of(StreamOperator<OUT> operator) {
        if (operator == null) {
            return null;
        } else if (operator instanceof StreamSource
                && ((StreamSource) operator).getUserFunction()
                        instanceof InputFormatSourceFunction) {
            // 如果Operator是StreamSource类型，且UserFunction类型为InputFormatSourceFunction
            return new SimpleInputFormatOperatorFactory<OUT>((StreamSource) operator);
        } else if (operator instanceof StreamSink
                && ((StreamSink) operator).getUserFunction() instanceof OutputFormatSinkFunction) {
            // 如果Operator是StreamSink类型，且UserFunction类型为OutputFormatSinkFunction
            return new SimpleOutputFormatOperatorFactory<>((StreamSink) operator);
        } else if (operator instanceof AbstractUdfStreamOperator) {
            // Operator是AbstractUdfStreamOperator类型
            return new SimpleUdfStreamOperatorFactory<OUT>((AbstractUdfStreamOperator) operator);
        } else {  // 其他情况
            return new SimpleOperatorFactory<>(operator);
        }
    }
}
```
SimpleOperatorFactory.createStreamOperator创建StreamOperator实例
```java
    @Override
    // 创建StreamOperator实例
    public <T extends StreamOperator<OUT>> T createStreamOperator(
            StreamOperatorParameters<OUT> parameters) {
        if (operator instanceof AbstractStreamOperator) {
            ((AbstractStreamOperator) operator).setProcessingTimeService(processingTimeService);
        }
        // 算子实现了SetupableStreamOperator接口，调用setup()方法对算子进行基本的设置
        if (operator instanceof SetupableStreamOperator) {
            ((SetupableStreamOperator) operator)
                    .setup(
                            parameters.getContainingTask(),
                            parameters.getStreamConfig(),
                            parameters.getOutput());
        }
        return (T) operator;
    }
```
### OneInputStreamOperator与TwoInputStreamOperator
StreamOperator根据输入流的数量分为两种类型，即支持单输入流的OneInputStreamOperator以及支持双输入流的TwoInputStreamOperator，我们可以将其称为一元输入算子和二元输入算子。
#### OneInputStreamOperator的实现
OneInputStreamOperator定义了单输入流的StreamOperator，常见的实现类有StreamMap、StreamFilter等算子。OneInputStreamOperator接口主要包含以下方法，专门用于处理接入的单输入数据流.
```java
public interface OneInputStreamOperator<IN, OUT> extends StreamOperator<OUT>, Input<IN> {
    @Override
    default void setKeyContextElement(StreamRecord<IN> record) throws Exception {
        setKeyContextElement1(record);
    }
}
```

#### TwoInputStreamOperator的实现
TwoInputStreamOperator定义了双输入流类型的StreamOperator接口实现，常见的实现类有CoStreamMap、HashJoinOperator等算子。
```java
public interface TwoInputStreamOperator<IN1, IN2, OUT> extends StreamOperator<OUT> {
    // 处理输入源1的数据元素方法
    void processElement1(StreamRecord<IN1> element) throws Exception;

    // 处理输入源2的数据元素方法
    void processElement2(StreamRecord<IN2> element) throws Exception;

    // 处理输入源1的Watermark方法
    void processWatermark1(Watermark mark) throws Exception;

    // 处理输入源2的Watermark方法
    void processWatermark2(Watermark mark) throws Exception;

    // 处理输入源1的LatencyMarker方法
    void processLatencyMarker1(LatencyMarker latencyMarker) throws Exception;

    // 处理输入源2的LatencyMarker方法
    void processLatencyMarker2(LatencyMarker latencyMarker) throws Exception;

    void processWatermarkStatus1(WatermarkStatus watermarkStatus) throws Exception;

    void processWatermarkStatus2(WatermarkStatus watermarkStatus) throws Exception;
}

```

|算子|连接策略|
|:--:|:--:|
|StreamMap|ALWAYS|

## Function的定义与实现
Function是Flink中最小的数据处理单元，在Flink中占据非常重要的地位，Flink实现的Function接口专门用于处理接入的数据元素。StreamOperator负责对内部Function的调用和执行，当StreamOperator被Task调用和执行时，StreamOperator会将接入的数据元素传递给内部Function进行处理，然后将Function处理后的结果推送给下游的算子继续处理。
Function接口是所有自定义函数父类，`MapFunction`和`FlatMapFunction`直接继承自Function接口，并提供各自数据处理方法。Flink还提供`RichFunction`接口实现对有状态计算的支持，RichFunction接口除了包含open()和close()方法之外，还提供了获取RuntimeContext的方法，并在AbstractRichFunction抽象类类中提供了对RichFunction接口的基本实现。RichMapFunction和RichFlatMapFunction接口实现类最终通过AbstractRichFunction提供的getRuntimeContext()方法获取RuntimeContext对象，进而操作状态数据。

### RichFunction
RichFunction接口实际上对Function进行了补充和拓展，提供了控制函数生命周期的open()和close()方法，所有实现了RichFunction的子类都能够获取RuntimeContext对象。而RuntimeContext包含了算子执行过程中所有运行时的上下文信息，例如Accumulator、BroadcastVariable和DistributedCache等变量。

#### RuntimeContext上下文

### SourceFunction和SinkFunction
有两种比较特殊的Function接口：SourceFunction和SinkFunction。SourceFunction没有具体的数据元素输入，而是通过在SourceFunction实现中与具体数据源建立连接，并读取指定数据源中的数据，然后转换成StreamRecord数据结构发送到下游的Operator中。SinkFunction接口的主要作用是将上游的数据元素输出到外部数据源中。两种函数都具有比较独立的实现逻辑。
#### SourceFunction
在默认情况下，SourceFunction不支持并行读取数据，因此SourceFunction被ParallelSourceFunction接口继承，以支持对外部数据源中数据的并行读取操作，比较典型的ParallelSourceFunction实例就是FlinkKafkaConsumer。

在SourceFunction的基础上拓展了RichParallelSourceFunction和RichSourceFunction抽象实现类，这使得SourceFunction可以在数据接入的过程中获取RuntimeContext信息，从而实现更加复杂的操作，例如使用OperatorState保存Kafka中数据消费的偏移量，从而实现端到端当且仅被处理一次的语义保障。
```
@Public
public interface SourceFunction<T> extends Function, Serializable {
    // 与外部数据源的交互，以实现外部数据的读取
    // 并将读取到的数据通过SourceContext提供的collect()方法发送给DataStream后续的算子进行处理
    void run(SourceContext<T> ctx) throws Exception;

    void cancel();
}
```
SourceContext主要用于收集SourceFunction中的上下文信息
```
    @Public
    interface SourceContext<T> {
        // 收集从外部数据源读取的数据并下发到下游算子
        void collect(T element);

        @PublicEvolving
        // 支持直接收集数据元素以及EventTime时间戳
        void collectWithTimestamp(T element, long timestamp);

        @PublicEvolving
        // 在SourceFunction中生成Watermark并发送到下游算子进行处理
        void emitWatermark(Watermark mark);

        @PublicEvolving
        void markAsTemporarilyIdle();

        // 用于获取检查点锁(Checkpoint Lock)
        // 例如使用KafkaConsumer读取数据时，可以使用检查点锁，确保记录发出的原子性和偏移状态更新。
        Object getCheckpointLock();


        void close();
    }
```
SourceContext类型与TimeCharacteristic的对应关系
|TimeCharactcristic | SourccContcxt|
|:--:|:--:|
|IngcstionTimc（接人时间）| AutomaticWatcrmarkContcxt|
|ProcessingTimc（处理时间）| NonTimcstampContcxt|
|EventTime（事件时间）| ManualWatcrmarkContcxt|

EventsGeneratorSource
```
    @Override
    // 实现事件的创建和采集
    public void run(SourceContext<Event> sourceContext) throws Exception {
        final EventsGenerator generator = new EventsGenerator(errorProbability);

        final int range = Integer.MAX_VALUE / getRuntimeContext().getNumberOfParallelSubtasks();
        final int min = range * getRuntimeContext().getIndexOfThisSubtask();
        final int max = min + range;

        while (running) {
            // 创建新的Event数据并对数据元素进行收集和下发
            sourceContext.collect(generator.next(min, max));

            if (delayPerRecordMillis > 0) {
                Thread.sleep(delayPerRecordMillis);
            }
        }
    }

```
`SourceFunction`会被封装在`StreamSource`算子中。在`StreamSource`算子中提供`run()`方法实现`SourceStreamTask`实例调用和执行,`SourceStreamTask`实际上是针对Source类型算子实现的`StreamTask`实现类。
```java
/**
 * for streaming sources.
 *
 * @param <OUT> Type of the output elements
 * @param <SRC> Type of the source function of this stream source operator
 */

// chainingStrategy = ChainingStrategy.HEAD
public class StreamSource<OUT, SRC extends SourceFunction<OUT>>
	extends AbstractUdfStreamOperator<OUT, SRC> {

    public void run(
            final Object lockingObject,
            final Output<StreamRecord<OUT>> collector,
            final OperatorChain<?, ?> operatorChain)
            throws Exception {
        // 获取TimeCharacteristic
        final TimeCharacteristic timeCharacteristic = getOperatorConfig().getTimeCharacteristic();
	// 从Task的环境信息Environment中获取Configuration配置信息
        final Configuration configuration =
                this.getContainingTask().getEnvironment().getTaskManagerInfo().getConfiguration();
        final long latencyTrackingInterval =
                getExecutionConfig().isLatencyTrackingConfigured()
                        ? getExecutionConfig().getLatencyTrackingInterval()
                        : configuration.getLong(MetricOptions.LATENCY_INTERVAL);
        // 创建LatencyMarksEmitter实例，主要用于在SourceFunction中输出Latency标记，也就是周期性地生成时间戳
        // 当下游算子接收到SourceOperator发送的LatencyMark后，会使用当前的时间减去LatencyMark中的时间戳
        // 以此确认该算子数据处理的延迟情况
        // 最后算子会将LatencyMark监控指标以Metric的形式发送到外部的监控系统中
        LatencyMarksEmitter<OUT> latencyEmitter = null;
        if (latencyTrackingInterval > 0) {
            latencyEmitter =
                    new LatencyMarksEmitter<>(
                            getProcessingTimeService(),
                            collector,
                            latencyTrackingInterval,
                            this.getOperatorID(),
                            getRuntimeContext().getIndexOfThisSubtask());
        }

        final long watermarkInterval =
                getRuntimeContext().getExecutionConfig().getAutoWatermarkInterval();
        // 获取SourceContext
        this.ctx =
                // 根据TimeCharacteristic参数创建对应类型的SourceContext
                StreamSourceContexts.getSourceContext(
                        timeCharacteristic,
                        getProcessingTimeService(),
                        lockingObject,
                        collector,
                        watermarkInterval,
                        -1,
                        emitProgressiveWatermarks);
        // 运行SourceFunction
        try {
            userFunction.run(ctx);  // 调用和执行SourceFunction实例
        } finally {
            if (latencyEmitter != null) {
                latencyEmitter.close();
            }
        }
    }
}
```

#### SinkFunction
在StreamSink Operator中提供了默认SinkContext实现，通过SimpleContext可以从ProcessingTimeservice中获取当前的处理时间、当前最大的Watermark和事件中的Timestamp等信息
```java
@Internal
public class StreamSink<IN> extends AbstractUdfStreamOperator<Object, SinkFunction<IN>>
        implements OneInputStreamOperator<IN, Object> {
	......
    private transient SimpleContext sinkContext;

    private long currentWatermark = Long.MIN_VALUE;

    public StreamSink(SinkFunction<IN> sinkFunction) {
        super(sinkFunction);
        chainingStrategy = ChainingStrategy.ALWAYS;
    }

    @Override
    public void open() throws Exception {
        super.open();
        this.sinkContext = new SimpleContext<>(getProcessingTimeService());
    }

    @Override
    public void processElement(StreamRecord<IN> element) throws Exception {
        sinkContext.element = element;
        // 触发自定义函数的计算，并将sinkContext作为参数传递到userFunction中使用
        // SinkFunction通过SinkContext提供的方法获取相应的时间信息并进行数据处理，实现将数据发送至外部系统
        userFunction.invoke(element.getValue(), sinkContext);
    }

    @Override
    protected void reportOrForwardLatencyMarker(LatencyMarker marker) {
        // all operators are tracking latencies
        this.latencyStats.reportLatency(marker);

        // sinks don't forward latency markers
    }

    @Override
    public void processWatermark(Watermark mark) throws Exception {
        super.processWatermark(mark);
        this.currentWatermark = mark.getTimestamp();
        userFunction.writeWatermark(
                new org.apache.flink.api.common.eventtime.Watermark(mark.getTimestamp()));
    }

    private class SimpleContext<IN> implements SinkFunction.Context {
        // 处理数据
        private StreamRecord<IN> element;
        // 时间服务
        private final ProcessingTimeService processingTimeService;

        public SimpleContext(ProcessingTimeService processingTimeService) {
            this.processingTimeService = processingTimeService;
        }

        @Override
        // 获取当前的处理时间
        public long currentProcessingTime() {
            return processingTimeService.getCurrentProcessingTime();
        }

        @Override
        // 获取当前的Watermark
        public long currentWatermark() {
            return currentWatermark;
        }

        @Override
        // 获取数据中的Timestamp
        public Long timestamp() {
            if (element.hasTimestamp()) {
                return element.getTimestamp();
            }
            return null;
        }
    }
}
```

#### TwoPhaseCommitSinkFunction
主要用于保证数据当且仅被输出一条的语义保障的场景。在TwoPhaseCommitSinkFunction中实现了和外围数据交互过程的Transaction逻辑，也就是只有当数据真正下发到外围存储介质时，才会认为Sink中的数据输出成功，其他任何因素导致写入过程失败，都会对输出操作进行回退并重新发送数据。

### ProcessFunction
在Flink API抽象栈中，最底层的是Stateful Function Process接口，代码实现对应的是ProcessFunction接口。通过实现ProcessFunction接口，能够灵活地获取底层处理数据和信息，例如状态数据的操作、定时器的注册以及事件触发周期的控制等。
根据数据元素是否进行了KeyBy操作，可以将ProcessFunction分为KeyedProcessFunction和ProcessFunction两种类型
- KeyedProcessFunction使用相对较多，常见的实现类有TopNFunction、GroupAggFunction等函数
- ProcessFunction的主要实现类是LookupJoinRunner，主要用于实现维表的关联等操作。
Table API模块相关的Operator直接实现自ProcessFunction接口。

```
public abstract class KeyedProcessFunction<K, I, O> extends AbstractRichFunction {
			......
			
    // 读取数据元素并处理，会在processElement()方法中根据实际情况创建定时器
    // 定时器会被注册到Context的TimerService定时器队列中
    // 当满足定时器触发的时间条件后，调用OnTimer()方法执行定时器中的计算逻辑，例如对状态数据的异步清理操作
    public abstract void processElement(I value, Context ctx, Collector<O> out) throws Exception;
    
    public void onTimer(long timestamp, OnTimerContext ctx, Collector<O> out) throws Exception {}
    
    public abstract class Context {
        public abstract Long timestamp();
        public abstract TimerService timerService();
        public abstract <X> void output(OutputTag<X> outputTag, X value);
        public abstract K getCurrentKey();
    }

    public abstract class OnTimerContext extends Context {
        public abstract TimeDomain timeDomain();
        public abstract K getCurrentKey();
    }
}
```
