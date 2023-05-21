DataStream API主要用于构建流式类型的Flink应用，处理实时无界数据流。和Storm组合式编程接口不同，DataStream API属于定义式编程接口，具有强大的表达力，可以构建复杂的流式应用，例如对状态数据的操作、窗口的定义等。

`DataStream`代表一系列同类型数据的集合，用于表达业务转换逻辑，实际上并没有存储真实数据，可以通过转换操作生成新的`DataStream`。DataStream数据结构包含两个主要成员：

1. `StreamExecutionEnvironment`: 用户在构建Flink作业时，`DataStream`之间的变换操作存储在`StreamExecutionEnvironment`的`List<Transformation<?>> transformations`集合中，之后通过`StreamExecutionEnvironment.execute()`接口生成流式作业Pipeline拓扑结构`StreamGraph`并将`StreamGraph`结构转换为`JobGraph`。
2. `Transformation<T> transformation`：当前`DataStream`对应的上一次的转换操作，也就是通过`transformation`生成当前的`DataStream`。

```java
public class DataStream<T> {
    // 提供通过DataStream API构建Flink作业需要的执行环境
    // 包括设定ExecutionConfig, CheckpointConfig等配置信息,
    // StateBackend和TimeCharacteristic等变量
    protected final StreamExecutionEnvironment environment;
    // transformation是当前DataStream对应的上一次的转换操作
    // 也就是通过transformation生成当前的DataStream
    protected final Transformation<T> transformation;
}

public class StreamExecutionEnvironment implements AutoCloseable {
    // 基于DataStream之间的转换操作生成StreamGraph
    // 并将StreamGraph结构转换为JobGraph,
    // 最终将JobGraph提交到指定的Session集群中运行
    public JobExecutionResult execute(String jobName) throws Exception {
        ......
    }
    
    // 存储构建Flink作业时,DataStream间的转换操作
    protected final List<Transformation<?>> transformations = new ArrayList<>();
}
```

每个Stream Transformation都包含相应的`StreamOperator`，`StreamOperator`涵盖了用户自定义函数的信息。例如执行`DataStream.map(new MapFunction(...))`转换之后，内部生成的便是`StreamMap`算子，`MapFunction`是用户自定义的map转换函数。



`DataStream.map()`方法为例了解`transform`的转换流程，主要的实现逻辑是`DataStream.transform()`调用`DataStream.doTransform`。

```java
/**
 * @param   MapFunction是用户自定义函数实现数据处理逻辑
 * 
 * @return 	SingleOutputStreamOperator继承了DataStream类,属于特殊的DataStream
 * 		    主要用于每次转换操作后返回给用户继续操作的数据结构
 */
public <R> SingleOutputStreamOperator<R> map(MapFunction<T, R> mapper) {
    TypeInformation<R> outType = TypeExtractor.getMapReturnTypes(
            clean(mapper), getType(), Utils.getCallLocationName(), true);
    return map(mapper, outType);
}

public <R> SingleOutputStreamOperator<R> map(
    	MapFunction<T, R> mapper, TypeInformation<R> outputType){
    return transform("Map", outputType, new StreamMap<>(clean(mapper)));
}

public <R> SingleOutputStreamOperator<R> transform(String operatorName,
		TypeInformation<R> outTypeInfo, OneInputStreamOperator<T, R> operator) {
    return doTransform(operatorName, outTypeInfo, SimpleOperatorFactory.of(operator));
}

protected <R> SingleOutputStreamOperator<R> doTransform(String operatorName,
        TypeInformation<R> outTypeInfo, StreamOperatorFactory<R> operatorFactory) {
    // 1. 获取输入Transformation的输出类型
    // 	  确定没有出现MissingTypeInfo错误,以确保下游算子转换不会出现问题
    transformation.getOutputType();

    // 2. 创建Transformation
    //    根据operatorName, outTypeInfo等创建OneInputTransformation
    OneInputTransformation<T, R> resultTransform =new OneInputTransformation<>(
        this.transformation, operatorName, operatorFactory,
        outTypeInfo, environment.getParallelism(), false);

    @SuppressWarnings({"unchecked", "rawtypes"})
    // 创建继承自DataStream的SingleOutputStreamOperator
    SingleOutputStreamOperator<R> returnStream =
            new SingleOutputStreamOperator(environment, resultTransform);
	// 3. 将Transformation添加到StreamExecutionEnvironment中的transformations
    getExecutionEnvironment().addOperator(resultTransform);

	// 4. 将SingleOutputStreamOperator返回给用户继续操作
    return returnStream;
}
```

`DataStream`在转换的过程中，都是按照同样的方式进行：

1. 将用户自定义的函数封装在`StreamOperator`中
2. 将`StreamOperator`封装在`Transformation`中，其中`Transformation`封装了`StreamOperatorFactory`并存储在`StreamGraph`和`JobGraph`结构中，直到执行`StreamTask`时，才调用`StreamOperatorFactory.createStreamOperator()`方法创建`StreamOperator`实例。
3. 将`Transformation`添加到`StreamExecutionEnvironment`中的`transformations`，用于将DataStream间的变换操作转换成`StreamGraph`,再将`StreamGraph`转换成`JobGraph`，`JobGraph`最终会被提交到集群中执行。

# Transformation

Transformation的实现子类涵盖了所有的`DataStream`转换操作，在`Transformation`的基础上又抽象出了`PhysicalTransformation`类，它提供了`setChainingStrategy()`方法，可以将上下游算子按照指定的链化策略连接。`ChainingStrategy`支持如下几种策略：

1. `ALWAYS`：代表该Transformation中的算子会和上游算子尽可能地链化，最终将多个Operator组合成`OperatorChain`。`OperatorChain`中的Operator会运行在同一个`SubTask`实例中，这样做的目的主要是优化性能，减少Operator之间的网络传输。
2. `NEVER`：代表该`Transformation`中的`Operator`永远不会和上下游算子之间链化，对应的`Operator`会运行在独立的`SubTask`实例中。
3. `HEAD`：代表该`Transformation`对应的`Operator`为头部算子，不支持上游算子链化，但是可以和下游算子链化，实际上就是`OperatorChain`中的`HeaderOperator`。
4. `HEAD_WITH_SOURCES`:

还有一些操作直接继承自`Transformation`，它们本身就是物理操作，不支持链化。

```java
/**
 *	1. 在Transformation的基础上抽象出了PhysicalTransformation类
 *     提供了setChainingStrategy()方法，可以将上下游算子按照指定的策略连接
 *     链化将多个Operator组合成OperatorChin,在同一个SubTask实例中运行,减少网络传输
 */
public abstract class PhysicalTransformation<T> extends Transformation<T> {
    // 设置上下游算子的链化策略(ChainingStrategy)
    public abstract void setChainingStrategy(ChainingStrategy strategy);
}
// 单进单出的数据集转换操作, 例如DataStream.map()转换
public class OneInputTransformation<IN, OUT> extends PhysicalTransformation<OUT>;
// 双进单出的数据集转换操作，例如在DataStream与DataStream之间进行Join操作
public class TwoInputTransformation<IN1, IN2, OUT> extends PhysicalTransformation<OUT>;
// 数据集输出操作,例如DataStream.addSource()
public class SinkTransformation<InputT, CommT, WriterStateT, GlobalCommT>
    	extends PhysicalTransformation<Object>;
// 数据集输入操作,例如DataStream.addSource()
public class SourceTransformation<OUT, SplitT extends SourceSplit, EnumChkT>
        extends PhysicalTransformation<OUT> implements WithBoundedness;
public final class ReduceTransformation<IN, K> extends PhysicalTransformation<IN>;
public abstract class AbstractMultipleInputTransformation<OUT> extends PhysicalTransformation<OUT>;
public class MultipleInputTransformation<OUT> extends AbstractMultipleInputTransformation<OUT>;

public class AbstractBroadcastStateTransformation<IN1, IN2, OUT>
        extends PhysicalTransformation<OUT>;
public class BroadcastStateTransformation<IN1, IN2, OUT>
    	extends AbstractBroadcastStateTransformation<IN1, IN2, OUT>;

/**
 *  2. 下面这些操作本身就是物理转换操作,不支持链化,不会将其和其他的算子放在同一subTask中运行
 */
// 根据OutputTag筛选上游DataStream中的数据并下发到下游的算子中继续处理
public class SideOutputTransformation<T> extends Transformation<T>;
// 对多个输入Transformation进行合并, 最终将上游DataStream数据集中的数据合并为一个DataStream
public class UnionTransformation<T> extends Transformation<T>;
// 支持对上游DataStream中的数据进行分区, 分区策略通过指定的StreamPartitioner决定
public class PartitionTransformation<T> extends Transformation<T>;
// 用于迭代计算中单输入反馈数据流节点的转换操作
public class CoFeedbackTransformation<F> extends Transformation<F>;
// 用于迭代计算中双输入反馈数据流节点的转换操作。
public class FeedbackTransformation<T> extends Transformation<T>;
```



```C++
@Internal
public class OneInputTransformation<IN, OUT> extends PhysicalTransformation<OUT> {
    private final Transformation<IN> input;
    // Transformation中封装StreamOperatorFactory
    // 运行时，执行StreamTask时调用StreamOperatorFactory.createStreamOperator()
    // 创建StreamOperator实例
    private final StreamOperatorFactory<OUT> operatorFactory;
    private KeySelector<IN, ?> stateKeySelector;
    private TypeInformation<?> stateKeyType;
}

public interface StreamOperatorFactory<OUT> extends Serializable {
    // 创建和获取streamoperator
    <T extends StreamOperator<OUT>> T createStreamOperator(
        	StreamOperatorParameters<OUT> parameters);
    Class<? extends StreamOperator> getStreamOperatorClass(ClassLoader classLoader);

    // 设置和获取上下游算子的链化策略(ChainingStrategy)
    void setChainingStrategy(ChainingStrategy strategy);
    ChainingStrategy getChainingStrategy();
}
```

# StreamOperator

```java
// 内含UserFunction,允许当前算子通过自定义UserFunction实现计算逻辑
public abstract class AbstractUdfStreamOperator<OUT, F extends Function>
        extends AbstractStreamOperator<OUT> implements OutputTypeConfigurable<OUT> {
    protected final F userFunction; // 用户自定义函数
}
public abstract class AbstractStreamOperator<OUT> implements StreamOperator<OUT>,
		SetupableStreamOperator<OUT>, CheckpointedStreamOperator,
		KeyContextHandler, Serializable{};

// 单输入类型算子
public interface OneInputStreamOperator<IN, OUT> extends StreamOperator<OUT>, Input<IN>;
// 双输入类型算子
public interface TwoInputStreamOperator<IN1, IN2, OUT> extends StreamOperator<OUT>;



public class StreamSource<OUT, SRC extends SourceFunction<OUT>>
    	extends AbstractUdfStreamOperator<OUT, SRC>;
public class StreamMap<IN, OUT> extends AbstractUdfStreamOperator<OUT, MapFunction<IN, OUT>>
        implements OneInputStreamOperator<IN, OUT> {}
public class StreamSink<IN> extends AbstractUdfStreamOperator<Object, SinkFunction<IN>>
        implements OneInputStreamOperator<IN, Object>;

```

`StreamOperator`接口实现的方法主要供Task调用和执行，其核心方法：

```java
public interface StreamOperator<OUT> extends CheckpointListener, KeyContext, Serializable {
    /* ------------------------ life cycle -------------------------- */
    // 当前Operator的初始化方法，在数据元素正式接入Operator运算之前
    // Task调用该方法进行初始化
    void open() throws Exception;
    // 当所有的数据元素都添加到当前Operator时调用
    // 刷新所有剩余的缓冲数据，保证算子中所有数据被正确处理
    void finish() throws Exception;
    // 算子生命周期结束(包括算子操作执行成功、失败或者取消)时调用
    void close() throws Exception;

    /** ------------------------state snapshots ------------------------ */
    // StreamOperator正式执行checkpoint操作之前调用
    void prepareSnapshotPreBarrier(long checkpointId) throws Exception;
    // SubTask执行checkpoint操作时调用, 用于触发该Operator中状态数据的快照操作
    OperatorSnapshotFutures snapshotState(long checkpointId, long timestamp,
        CheckpointOptions checkpointOptions, CheckpointStreamFactory storageLocation)
        throws Exception;
    // 当算子启动或重启时，调用该方法初始化状态数据
    // 当恢复作业任务时，算子从checkpoint持久化的数据中恢复状态数据
    void initializeState(StreamTaskStateInitializer streamTaskStateManager) throws Exception;
}
```



调度和执行`Task`实例时，会通过`AbstractStreamOperator`提供的入口方法触发和执行`Operator`。`AbstractStreamOperator`作为`StreamOperator`的基本实现类，所有的`Operator`都会继承和实现该抽象实现类。在`AbstractStreamOperator`中定义了`Operator`用到的基础方法和成员信息。

```java
public abstract class AbstractStreamOperator<OUT> implements StreamOperator<OUT>,
    SetupableStreamOperator<OUT>, CheckpointedStreamOperator, Serializable {
    // ----------- configuration properties -------------  
    // 上下游算子的链化策略，实际上就是Transform过程中配置的链接策略
    protected ChainingStrategy chainingStrategy = ChainingStrategy.HEAD;
    
    // ---------------- runtime fields ------------------
    // 当前Operator所属的StreamTask
    // 最终会通过StreamTask中的invoke()方法执行当前StreamTask中的所有Operator
    private transient StreamTask<?, ?> container;
    // StreamOperator的配置信息，实际上是Configuration参数的封装
    protected transient StreamConfig config;
    // 定义了当前StreamOperator的输出操作
    // 执行完该算子的所有转换操作后,会通过Output组件将数据推送到下游算子继续执行
    protected transient Output<StreamRecord<OUT>> output;
    // 主要定义了UDF执行过程中的上下文信息，例如获取累加器、状态数据
	private transient StreamingRuntimeContext runtimeContext;
     // 
    private transient IndexedCombinedWatermarkStatus combinedWatermark;

    // ---------------- key/value state ------------------
    // 只有DataStream经过keyBy()转换操作生成KeyedStream后
    // 才设定该算子的stateKeySelector1变量信息
    private transient KeySelector<?, ?> stateKeySelector1;
	// 只在执行两个KeyedStream关联操作时使用
    // 例如Join操作，在AbstractStreamOperator中会保存stateKeySelector2的信息
    private transient KeySelector<?, ?> stateKeySelector2;
	// 状态管理后端
    private transient StreamOperatorStateHandler stateHandler;
    
    // Flink内部时间服务，和processingTimeService相似
    // 但支持基于事件时间的时间域处理数据，还可以同时注册基于事件时间和处理时间的定时器
    // 例如在窗口、CEP等高级类型的算子中，会在ProcessFunction中通过timeServiceManager
    // 注册Timer定时器，当事件时间或处理时间到达指定时间后执行Timer定时器，以实现复杂的函数计算
    private transient InternalTimeServiceManager<?> timeServiceManager;


    // 用于记录当前算子层面的监控指标，包括numRecordsIn、numRecordsOut
    // numRecordsInRate、numRecordsOutRate等
    protected transient InternalOperatorMetricGroup metrics;
    // 采集和汇报当前Operator的延时状况
    protected transient LatencyStats latencyStats;

    // ---------------- time handler ------------------
    // 基于ProcessingTime的时间服务，实现ProcessingTime时间域操作
    protected transient ProcessingTimeService processingTimeService;
        
	// 用于处理在SourceOperator中产生的LatencyMarker信息。
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
}
```



当`StreamOperator`涉及自定义用户函数数据转换处理时，对应的Operator会继承`AbstractUdfStreamOperator`抽象实现类(内部增加了`UserFunction`，提供了`UserFunction`的初始化和状态持久化方法)，常见有`StreamMap`、`CoProcessOperator`等算子。`AbstractUdfStreamOperator`抽象实现`Operator`和`Function`间关联，`Operator`也是`Function`的载体，具体数据处理操作借助Operator中的Function进行。`StreamOperator`提供了执行Function的环境，包括状态数据管理和处理`Watermark`、`LatencyMarker`等信息。

并不是所有Operator都继承自`AbstractUdfStreamOperator`。在`Flink Table API`模块实现的算子中，都会直接继承和实现`AbstractStreamOperator`抽象实现类。有状态查询的`AbstractQueryableStateOperator`也不需要使用用户自定义函数处理数据。

```java
public abstract class AbstractUdfStreamOperator<OUT, F extends Function>
        extends AbstractStreamOperator<OUT> implements OutputTypeConfigurable<OUT> {
    // 用户自定义函数，允许当前算子通过自定义UserFunction实现计算逻辑
    protected final F userFunction;
    public F getUserFunction() { return userFunction; }
    
    
    @Override
    // 为UserFunction设置RuntimeContext,实现可以获取上下文信息
    public void setup(StreamTask<?, ?> containingTask,
                      StreamConfig config, Output<StreamRecord<OUT>> output) {
        super.setup(containingTask, config, output);
        // AbstractStreamOperator拥有runtimeContext(拥有UDF执行过程中的上下文信息)
        // 并封装了getRuntimeContext()接口可以被父类获取
        FunctionUtils.setFunctionRuntimeContext(userFunction, getRuntimeContext());
    }

    @Override
    // 实现对userFunction中的状态进行快照操作
    public void snapshotState(StateSnapshotContext context) throws Exception {
        super.snapshotState(context);
        StreamingFunctionUtils.snapshotFunctionState(
            context, getOperatorStateBackend(), userFunction);
    }

    @Override
    // 初始化userFunction的状态值
    public void initializeState(StateInitializationContext context) throws Exception {
        super.initializeState(context);
        StreamingFunctionUtils.restoreFunctionState(context, userFunction);
    }
    
    
    @Override
    public void open() throws Exception {
        super.open();
        // 这里确保当用户自定义并实现RichFunction时
        // FunctionUtils.openFunction()方法会调用RichFunction.open()方法
        // 完成用户自定义状态的创建和初始化
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


## 单/双输入流StreamOperator

`StreamOperator`根据输入流的数量分为两种类型，即支持单输入流的`OneInputStreamOperator`以及支持双输入流的`TwoInputStreamOperator`，可以将其称为一元输入算子和二元输入算子。



`OneInputStreamOperator`定义了单输入流的`StreamOperator`，常见的有`StreamMap`、`StreamFilter`等算子。

```java
@PublicEvolving
public interface OneInputStreamOperator<IN, OUT> extends StreamOperator<OUT>, Input<IN> {
    @Override
    default void setKeyContextElement(StreamRecord<IN> record) throws Exception {
        setKeyContextElement1(record);
    }
}
```



```java
@Internal
public class StreamFilter<IN> extends AbstractUdfStreamOperator<IN, FilterFunction<IN>>
        implements OneInputStreamOperator<IN, IN> {
    private static final long serialVersionUID = 1L;
    public StreamFilter(FilterFunction<IN> filterFunction) {
        // 初始化化用户自定义函数(FilterFunction)
        super(filterFunction);
        // 上下游的链化策略是ALWAYS
        // 通常会与上下游的Operator连接在一起形成OperatorChain
        chainingStrategy = ChainingStrategy.ALWAYS;
    }

    @Override
    public void processElement(StreamRecord<IN> element) throws Exception {
        // 执行用户自定函数UserFunction的filter()方法
        if (userFunction.filter(element.getValue())) {
            output.collect(element);
        }
    }
}
```



`TwoInputStreamOperator`定义了双输入流类型的`StreamOperator`接口实现，常见的实现类有`CoStreamMap`、`HashJoinOperator`等算子。`TwoInputStreamOperator`接口定义的主要方法，实现对两个数据流转换操作，还定义了两条数据流中`Watermark`和`LatencyMarker`的处理逻辑。

```java
@PublicEvolving
public interface TwoInputStreamOperator<IN1, IN2, OUT> extends StreamOperator<OUT> {
    // 处理输入源1和输入源2的数据元素方法
    void processElement1(StreamRecord<IN1> element) throws Exception;
    void processElement2(StreamRecord<IN2> element) throws Exception;

    // 处理输入源1和输入源2的WaterMark
    void processWatermark1(Watermark mark) throws Exception;
    void processWatermark2(Watermark mark) throws Exception;

	// 处理输入源1和输入源2的LatencyMarker
    void processLatencyMarker1(LatencyMarker latencyMarker) throws Exception;
    void processLatencyMarker2(LatencyMarker latencyMarker) throws Exception;

	// 处理输入源1和输入源2的WatermarkStatus
    void processWatermarkStatus1(WatermarkStatus watermarkStatus) throws Exception;
    void processWatermarkStatus2(WatermarkStatus watermarkStatus) throws Exception;
}
```



从`CoStreamMap`算子定义中可以看出，`CoStreamMap`继承`AbstractUdfStreamOperator`，实现了`TwoInputStreamOperator`接口。其中在`processElement1()`和`processElement2()`两个方法的实现中，分别调用了用户定义的`CoMapFunction`的`map1()`和`map2()`方法对输入的数据元素`Input1`和`Input2`进行处理。经过函数处理后的结果会通过`output.collect()`接口推送到下游的`Operator`中。

```java
@Internal
public class CoStreamMap<IN1, IN2, OUT>
        extends AbstractUdfStreamOperator<OUT, CoMapFunction<IN1, IN2, OUT>>
        implements TwoInputStreamOperator<IN1, IN2, OUT> {
		......
    @Override
    // 调用用户自定义CoMapFunction函数的map1()方法对输入数据元素IN1处理
    public void processElement1(StreamRecord<IN1> element) throws Exception {
        // 通过output.collect将函数处理后的结果推送到下游的Operator中
        output.collect(element.replace(userFunction.map1(element.getValue())));
    }

    @Override
    // 调用用户自定义CoMapFunction函数的map1()方法对输入数据元素IN2处理
    public void processElement2(StreamRecord<IN2> element) throws Exception {
        output.collect(element.replace(userFunction.map2(element.getValue())));
    }
}
```



## StreamOperatorFactory

`StreamOperator`最终会通过`StreamOperatorFactory`封装在Transformation结构中，并存储在`StreamGraph`和`JobGraph`结构中，直到运行时执行StreamTask时，才会调用`StreamOperatorFactory.createStreamOperator()`方法在`StreamOperatorFactory`中创建StreamOperator实例。

通过`StreamOperatorFactory`封装创建`StreamOperator`的操作

- `DataStream` API中主要通过`SimpleStreamOperatorFactory`创建已经定义的`Operator`
- 在Table API模块中主要通过`CodeGenOperatorFactory`从代码中动态编译并创建Operator实例。

`SimpleOperatorFactory`和`CodeGenOperatorFactory`都是`StreamOperatorFactory`的实现类。

```java
public interface StreamOperatorFactory<OUT> extends Serializable {
    // 创建Operator
    <T extends StreamOperator<OUT>> T createStreamOperator(
            StreamOperatorParameters<OUT> parameters);

    // 设置和获取operator factory的链化策略
    void setChainingStrategy(ChainingStrategy strategy);
    ChainingStrategy getChainingStrategy();
    		......
}
```

在`AbstractStreamOperatorFactory`实现了`StreamOperatorFactory`，在其基础之上派生出`SimpleOperatorFactory`。

DataStream API中大部分转换操作都是通过`SimpleOperatorFactory`进行封装和创建的。`SimpleStreamOperatorFactory`根据算子类型的不同，拓展出了`InputFormatOperatorFactory`、`UdfStreamOperatorFactory`和`OutputFormatOperatorFactory`三种接口实现。

- `SimpleInputFormatOperatorFactory`：支持创建`InputFormat`类型输入的`StreamSource`算子，即`SourceFunction`为`InputFormatSourceFunction`类型，并提供`getInputFormat()`方法。
- `SimpleUdfStreamOperatorFactory`：支持`AbstractUdfStreamOperator`类型的`Operator`创建，并且在其中提供了获取`UserFunction`的方法。
- `SimpleOutputFormatOperatorFactory`：支持创建`OutputFormat`类型输出的`StreamSink`算子，即`SinkFunction`为`OutputFormatSinkFunction`类型，并提供`getOutputFormat()`方法。

```java

public abstract class AbstractStreamOperatorFactory<OUT>
        implements StreamOperatorFactory<OUT>, ProcessingTimeServiceAware {
    protected ChainingStrategy chainingStrategy =
        	ChainingStrategy.DEFAULT_CHAINING_STRATEGY;
    protected transient ProcessingTimeService processingTimeService;
    @Nullable private transient MailboxExecutor mailboxExecutor;
        		......
}

@Internal
public class SimpleOperatorFactory<OUT> extends AbstractStreamOperatorFactory<OUT> {
    private final StreamOperator<OUT> operator;
    
    // 根据Operator类型的不同，创建不同的SimpleOperatorFactory实现类
    public static <OUT> SimpleOperatorFactory<OUT> of(StreamOperator<OUT> operator) {
        if (operator == null) {
            return null;
        } else if (operator instanceof StreamSource && ((StreamSource) operator).getUserFunction()
                   instanceof InputFormatSourceFunction) {
            // 如果Operator是StreamSource类型，且UserFunction类型为InputFormatSourceFunction
            return new SimpleInputFormatOperatorFactory<OUT>((StreamSource) operator);
        } else if (operator instanceof UserFunctionProvider && (((
            		UserFunctionProvider<Function>) operator).getUserFunction()
					instanceof OutputFormatSinkFunction)) {
            // 如果Operator是StreamSink类型，且UserFunction类型为OutputFormatSinkFunction
            return new SimpleOutputFormatOperatorFactory<>((((
                	OutputFormatSinkFunction<?>)((UserFunctionProvider<
						Function>) operator).getUserFunction()).getFormat()), operator);
        } else if (operator instanceof AbstractUdfStreamOperator) {
           // Operator是AbstractUdfStreamOperator类型
           return new SimpleUdfStreamOperatorFactory<OUT>((AbstractUdfStreamOperator) operator);
        } else {
            // 其他情况
            return new SimpleOperatorFactory<>(operator);
        }
    }
    
    @Override // 创建StreamOperator实例
    public <T extends StreamOperator<OUT>> T createStreamOperator(
            StreamOperatorParameters<OUT> parameters) {
        if (operator instanceof AbstractStreamOperator) {
            ((AbstractStreamOperator) operator).setProcessingTimeService(processingTimeService);
        }
        // 算子实现了SetupableStreamOperator接口，调用setup()方法对算子进行基本的设置
        if (operator instanceof SetupableStreamOperator) {
            ((SetupableStreamOperator) operator)
                    .setup(parameters.getContainingTask(),
                            parameters.getStreamConfig(),
                            parameters.getOutput());
        }
        return (T) operator;
    }
    			......
}
```



```java
public class SimpleInputFormatOperatorFactory<OUT> extends SimpleOperatorFactory<OUT>
        implements InputFormatOperatorFactory<OUT> {
    private final StreamSource<OUT, InputFormatSourceFunction<OUT>> operator;

    @Override
    public OutputFormat<IN> getOutputFormat() { return outputFormat; }
}

public class SimpleOutputFormatOperatorFactory<IN, OUT> extends SimpleOperatorFactory<OUT>
        implements OutputFormatOperatorFactory<IN, OUT> {
    private final OutputFormat<IN> outputFormat;
    
    public InputFormat<OUT, InputSplit> getInputFormat() {
        return operator.getUserFunction().getFormat();
    }
}
public class SimpleUdfStreamOperatorFactory<OUT> extends SimpleOperatorFactory<OUT>
        implements UdfStreamOperatorFactory<OUT> {
    private final AbstractUdfStreamOperator<OUT, ?> operator;
    
    @Override
    public Function getUserFunction() { return operator.getUserFunction(); }
}

@Internal
public interface InputFormatOperatorFactory<OUT> extends StreamOperatorFactory<OUT> {
    InputFormat<OUT, InputSplit> getInputFormat();
}

@Internal
public interface OutputFormatOperatorFactory<IN, OUT> extends StreamOperatorFactory<OUT> {
    OutputFormat<IN> getOutputFormat();
}

@Internal
public interface UdfStreamOperatorFactory<OUT> extends StreamOperatorFactory<OUT> {
    // 获取用户自定义函数
    Function getUserFunction();
    String getUserFunctionClassName();
}
```



# Function

Function是Flink中最小的数据处理单元，在Flink中占据非常重要的地位，Flink实现的Function接口专门用于处理接入的数据元素。`StreamOperator`负责对内部`Function`调用和执行，当`StreamOperator`被Task调用和执行时，`StreamOperator`会将接入的数据元素传递给内部Function进行处理，然后将Function处理后的结果推送给下游的算子继续处理。

`Function`接口是所有自定义函数父类，`MapFunction`和`FlatMapFunction`直接继承自`Function`接口，并提供各自数据处理方法。

```java
public interface MapFunction<T, O> extends Function, Serializable {
    O map(T value) throws Exception;
}

public interface FlatMapFunction<T, O> extends Function, Serializable {
    void flatMap(T value, Collector<O> out) throws Exception;
}
```

Flink还提供`RichFunction`接口实现对有状态计算的支持，`RichFunction`接口除了包含`open()`和`close()`方法之外，还提供了获取`RuntimeContext`的方法，`RuntimeContext`包含算子执行过程中所有运行时上下文信息，例如`Accumulator`、`BroadcastVariable`和`DistributedCache`等变量。并在`AbstractRichFunction`抽象类类中提供了对`RichFunction`接口的基本实现。`RichMapFunction`和`RichFlatMapFunction`接口实现类通过`AbstractRichFunction`提供的`getRuntimeContext()`方法获取`RuntimeContext`对象，进而操作状态数据。

```java
public interface RichFunction extends Function {
    void open(Configuration parameters) throws Exception;
    void close() throws Exception;

    // 获RuntimeContext对象操作状态数据
	RuntimeContext getRuntimeContext();
    IterationRuntimeContext getIterationRuntimeContext();
    void setRuntimeContext(RuntimeContext t);
}

public abstract class AbstractRichFunction implements RichFunction, Serializable {
    private transient RuntimeContext runtimeContext;
}

public abstract class RichFlatMapFunction<IN, OUT> extends AbstractRichFunction
        implements FlatMapFunction<IN, OUT> {
    @Override
    public abstract void flatMap(IN value, Collector<OUT> out) throws Exception;
}

public abstract class RichMapFunction<IN, OUT> extends AbstractRichFunction
        implements MapFunction<IN, OUT> {
    @Override
    public abstract OUT map(IN value) throws Exception;
}
```

`RuntimeContext`使得`Function`接口实现类可以获取运行时执行过程中的上下文信息，从而实现了更加复杂的统计运算。



不同类型的Operator创建的`RuntimeContext`也有一定区别，因此在Flink中提供了不同的`RuntimeContext`实现类，以满足不同Operator对运行时上下文信息的获取。其中`AbstractRuntimeUDFContext`主要用于获取提供UDF函数的相关运行时上下文信息，且`AbstractRuntimeUDFContext`又分别被下面三个子类继承实现：

1. `RuntimeUDFContext`：主要用于`CollectionExecutor`。
2. `DistributedRuntimeUDFContext`：主要用于`BatchTask`、`DataSinkTask`以及`DataSourceTask`等离线场景。
3. `StreamingRuntimeContext`：流式数据处理中使用最多的是`StreamingRuntimeContext`。

还有其他场景使用到的RuntimeContext实现类：

1. `CepRuntimeContext`
2. `SavepointRuntimeContext`
3. `IterationRuntimeContext`

```java
@Internal
public abstract class AbstractRuntimeUDFContext implements RuntimeContext {
    private final TaskInfo taskInfo;
    private final UserCodeClassLoader userCodeClassLoader;
    private final ExecutionConfig executionConfig;
    private final Map<String, Accumulator<?, ?>> accumulators;
    private final DistributedCache distributedCache;
    private final OperatorMetricGroup metrics;
}

@Internal
public class RuntimeUDFContext extends AbstractRuntimeUDFContext {
    private final HashMap<String, Object> initializedBroadcastVars = new HashMap<>();
    private final HashMap<String, List<?>> uninitializedBroadcastVars = new HashMap<>();
    private final JobID jobID;
}
public class StreamingRuntimeContext extends AbstractRuntimeUDFContext;
public class DistributedRuntimeUDFContext extends AbstractRuntimeUDFContext;
```

```java
@Public
public interface IterationRuntimeContext extends RuntimeContext {
    int getSuperstepNumber();
   
    @PublicEvolving
    <T extends Aggregator<?>> T getIterationAggregator(String name);
    <T extends Value> T getPreviousIterationAggregate(String name);
}

class CepRuntimeContext implements RuntimeContext;
public final class SavepointRuntimeContext implements RuntimeContext;
private static class RichAsyncFunctionRuntimeContext implements RuntimeContext;
```



自定义`RichMapFunction`实现

```java
public class CustomMapper extends RichMapFunction<String, String> {
private transient Counter counter;
private ValueState<Long> state;

@Override
public void open(Configuration config) {
  // 获取MetricGroup创建Counter指标累加器
  this.counter = getRuntimeContext()
      .getMetricGroup()
      .counter("myCounter");
  // 调用getState()方法创建ValueState
  state = getRuntimeContext().getState(
      new ValueStateDescriptor<Long>(
          "count", LongSerializer.INSTANCE, 0L));
}

@Override
// 创建好的Metric和ValueState应用在map()转换中
public String map(String value) throws Exception {
    this.counter.inc();
    long count = state.value() + 1;
    state.update(count);
    return value;
}
}
```



## SourceFunction和SinkFunction
有两种比较特殊的Function接口：`SourceFunction`和`SinkFunction`。`SourceFunction`没有具体的数据元素输入，而是通过在`SourceFunction`实现中与具体数据源建立连接，并读取指定数据源中的数据，然后转换成`StreamRecord`数据结构发送到下游的Operator中。`SinkFunction`接口的主要作用是将上游的数据元素输出到外部数据源中。两种函数都具有比较独立的实现逻辑。



`SourceFunction`接口继承了`Function`接口，并在内部定义了数据读取使用的`run()`方法和定义数据接入过程用到的上下文信息的`SourceContext`内部类。在默认情况下，`SourceFunction`不支持并行读取数据，因此`SourceFunction`被`ParallelSourceFunction`接口继承，以支持对外部数据源中数据的并行读取操作，比较典型的`ParallelSourceFunction`实例就是`FlinkKafkaConsumer`。其内部类`SourceContext`主要用于收集`SourceFunction`中的上下文信息

```java
@Public
public interface SourceFunction<T> extends Function, Serializable {
    // 与外部数据源的交互，以实现外部数据的读取
    // 并将读取到的数据通过SourceContext提供的collect()方法发送给DataStream后续的算子进行处理
    void run(SourceContext<T> ctx) throws Exception;

    void cancel();
    interface SourceContext<T> {
        // 收集从外部数据源读取的数据并下发到下游算子中
        void collect(T element);
        // 支持直接收集数据元素以及EventTime时间戳
        void collectWithTimestamp(T element, long timestamp);
        // 在SourceFunction中生成Watermark并发送到下游算子进行处理
        void emitWatermark(Watermark mark);
        // 
        void markAsTemporarilyIdle();
        // 获取检查点锁(Checkpoint Lock)
        // 例如使用KafkaConsumer读取数据时，使用检查点锁，确保记录发出的原子性和偏移状态更新
        Object getCheckpointLock();
        void close();
    }
}

// 支持数据并行读
public interface ParallelSourceFunction<OUT> extends SourceFunction<OUT> {}
```


在`SourceFunction`的基础上拓展了`RichParallelSourceFunction`和`RichSourceFunction`抽象实现类，这使得`SourceFunction`可以在数据接入的过程中获取`RuntimeContext`信息，从而实现更加复杂的操作，例如使用`OperatorState`保存Kafka中数据消费的偏移量，从而实现端到端当且仅被处理一次的语义保障。

```java
public abstract class RichParallelSourceFunction<OUT> extends AbstractRichFunction
        implements ParallelSourceFunction<OUT> {}
public abstract class RichSourceFunction<OUT> extends AbstractRichFunction
        implements SourceFunction<OUT>{}
```

`EventsGeneratorSource`通过`SourceFunction.run()`方法实现了事件的创建(由`EventsGenerator`实现)和采集。

```java
public class EventsGeneratorSource extends RichParallelSourceFunction<Event> {
    @Override
    // 实现事件的创建和采集，具体创建过程通过EventsGenerator实现
    public void run(SourceContext<Event> sourceContext) throws Exception {
        final EventsGenerator generator = new EventsGenerator(errorProbability);
        final int range = Integer.MAX_VALUE / getRuntimeContext().getNumberOfParallelSubtasks();
        final int min = range * getRuntimeContext().getIndexOfThisSubtask();
        final int max = min + range;
	    // while循环不断调用EventsGenerator创建新的event数据
        // sourceContext.collect()对数据元素收集和下发
        // 下游算子收到Event数据进行处理
        while (running) {
            sourceContext.collect(generator.next(min, max));
            if (delayPerRecordMillis > 0) {
                Thread.sleep(delayPerRecordMillis);
            }
        }
    }
}
```

`SourceFunction`定义完毕后，会被封装在`StreamSource`算子中。在`StreamSource`算子中提供了`run()`方法实现`SourceStreamTask`实例的调用和执行，`SourceStreamTask`实际上是针对`Source`类型算子实现的`StreamTask`实现类。

```java
@Internal
// SourceFunction封装在StreamSource中
public class StreamSource<OUT, SRC extends SourceFunction<OUT>>
        extends AbstractUdfStreamOperator<OUT, SRC> {
    public void run(final Object lockingObject, final OperatorChain<?, ?> operatorChain)
        	throws Exception {
        run(lockingObject, output, operatorChain);
    }
    
    // chainingStrategy = ChainingStrategy.HEAD
    public class StreamSource<OUT, SRC extends SourceFunction<OUT>>
        extends AbstractUdfStreamOperator<OUT, SRC> {

        public void run(final Object lockingObject, final Output<StreamRecord<OUT>> collector,
                final OperatorChain<?, ?> operatorChain) throws Exception {
            // 从OperatorConfig中获取TimeCharacteristic
            final TimeCharacteristic timeCharacteristic =getOperatorConfig().getTimeCharacteristic();
            // 从Task的环境信息Environment中获取Configuration配置信息
            final Configuration configuration = this.getContainingTask().getEnvironment()
                	.getTaskManagerInfo().getConfiguration();
            final long latencyTrackingInterval = getExecutionConfig().isLatencyTrackingConfigured()
                    ? getExecutionConfig().getLatencyTrackingInterval()
                    : configuration.getLong(MetricOptions.LATENCY_INTERVAL);
            // 创建LatencyMarksEmitter实例
            // 主要用于在SourceFunction中输出Latency标记，也就是周期性地生成时间戳
            // 当下游算子接收到SourceOperator发送的LatencyMark，会使用当前时间减去LatencyMark中的时间戳
            // 以此确认该算子数据处理的延迟情况
            // 最后算子会将LatencyMark监控指标以Metric的形式发送到外部的监控系统中
            LatencyMarksEmitter<OUT> latencyEmitter = null;
            if (latencyTrackingInterval > 0) {
                latencyEmitter = new LatencyMarksEmitter<>( getProcessingTimeService(),
                                collector, latencyTrackingInterval, this.getOperatorID(),
                                getRuntimeContext().getIndexOfThisSubtask());
            }

            final long watermarkInterval =
                    getRuntimeContext().getExecutionConfig().getAutoWatermarkInterval();
            // 创建SourceContext,根据TimeCharacteristic参数创建对应类型的SourceContext
            this.ctx = StreamSourceContexts.getSourceContext( timeCharacteristic,
                    getProcessingTimeService(), lockingObject, collector, watermarkInterval,
                      -1, emitProgressiveWatermarks);

            try {
                // 调用SourceFunction的run()方法
                userFunction.run(ctx);
            } finally {
                if (latencyEmitter != null) {
                    latencyEmitter.close();
                }
            }
        }
    }
}
```



`SourceContext`有下面的实现子类：

```java
// 支持事件时间抽取和生成Watermark，最终用于处理乱序事件
private abstract static class WatermarkContext<T> implements SourceFunction.SourceContext<T>;
// 不支持基于事件时间的操作，仅实现了从外部数据源中读取数据并处理的逻辑
// 主要对应TimeCharacteristic为ProcessingTime的情况
private static class NonTimestampContext<T> implements SourceFunction.SourceContext<T> ;
private static class ClosedContext<T> implements SourceFunction.SourceContext<T>;
private static class SwitchingOnClose<T> implements SourceFunction.SourceContext<T>;

private static class AutomaticWatermarkContext<T> extends WatermarkContext<T>;
private static class ManualWatermarkContext<T> extends WatermarkContext<T>;
```

用户设定不同的`TimeCharacteristic`，就会创建不同类型的`SourceContext`。

```java
    public static <OUT> SourceFunction.SourceContext<OUT> getSourceContext(
            TimeCharacteristic timeCharacteristic, ...) {
        final SourceFunction.SourceContext<OUT> ctx;
        switch (timeCharacteristic) {
            case EventTime:
                ctx = new ManualWatermarkContext<>();
                break;
            case IngestionTime:
                ctx = new AutomaticWatermarkContext<>();
                break;
            case ProcessingTime:
                ctx = new NonTimestampContext<>(checkpointLock, output);
                break;
        }
        return new SwitchingOnClose<>(ctx);
    }
```

SourceContext类型与TimeCharacteristic的对应关系
|TimeCharactcristic | SourccContcxt|
|:--:|:--:|
|IngcstionTimc（接人时间）| `AutomaticWatcrmarkContcxt` |
|ProcessingTimc（处理时间）| `NonTimcstampContcxt` |
|EventTime（事件时间）| `ManualWatcrmarkContcxt` |

其中`AutomaticWatermarkContex`t和`ManualWatermarkContext`都继承自`WatermarkContext`抽象类，分别对应接入时间和事件时间。由此可以看出，接入时间对应的`Timestamp`和`Watermark`都是通过`Source`算子自动生成的。事件时间的实现则相对复杂，需要用户自定义`SourceContext.emitWatermark()`方法来实现。



在`SinkFunction`中同样需要关注和外部介质的交互，尤其对于支持两阶段提交的数据源来讲，此时需要使用`TwoPhaseCommitSinkFunction`实现端到端的数据一致性。在`SinkFunction`中也会通过SinkContext获取与Sink操作相关的上下文信息。

`SinkFunction`继承自`Function`接口，且`SinkFunciton`分为`WriteSinkFunction`和`RichSinkFunction`两种类型的子类。常见的`RichSinkFunction`实现类有`SocketClientSink`和`StreamingFileSink`，支持两阶段提交的`TwoPhaseCommitSinkFunction`，其实现类主要有`FlinkKafkaProducer`。

```java
public interface SinkFunction<IN> extends Function, Serializable {}

public abstract class RichSinkFunction<IN> extends AbstractRichFunction
        implements SinkFunction<IN>;
public abstract class WriteSinkFunction<IN> implements SinkFunction<IN>;

// 支持两阶段提交
public abstract class TwoPhaseCommitSinkFunction<IN, TXN, CONTEXT> extends RichSinkFunction<IN>
        implements CheckpointedFunction, CheckpointListener {}
public class SocketClientSink<IN> extends RichSinkFunction<IN>;
public class StreamingFileSink<IN> extends RichSinkFunction<IN>
        implements CheckpointedFunction, CheckpointListener{}

// 在两阶段提交实现类TwoPhaseCommitSinkFunction派生出FlinkKafkaProducer实现类
public class FlinkKafkaProducer<IN> extends TwoPhaseCommitSinkFunction<IN,
                FlinkKafkaProducer.KafkaTransactionState,
                FlinkKafkaProducer.KafkaTransactionContext>{};
```



`SinkFuntion`中也会创建和使用`SinkContext`，以获取Sink操作过程需要的上下文信息。但相比于`SourceContext`，`SinkFuntion`中的`SinkContext`仅包含一些基本方法，例如获取`currentProcessingTime`、`currentWatermark`以及`Timestamp`等变量。



在StreamSink Operator中提供了默认`SinkContext`实现，通过`SimpleContext`可以从`ProcessingTimeservice`中获取当前的处理时间、当前最大的`Watermark`和事件中的`Timestamp`等信息
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
        // SinkFunction通过SinkContext提供的方法获取相应的时间信息并进行数据处理
        // 实现将数据发送至外部系统
        userFunction.invoke(element.getValue(), sinkContext);
    }

    @Override
    protected void reportOrForwardLatencyMarker(LatencyMarker marker) {
        this.latencyStats.reportLatency(marker);
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

`TwoPhaseCommitSinkFunction`主要用于保证数据当且仅被输出一条的语义保障的场景，实现了和外围数据交互过程的Transaction逻辑，也就是只有当数据真正下发到外围存储介质时，才会认为Sink中的数据输出成功，其他任何因素导致写入过程失败，都会对输出操作进行回退并重新发送数据。

## ProcessFunction
在Flink API抽象栈中，最底层的是Stateful Function Process接口，代码实现对应的是ProcessFunction接口。通过实现ProcessFunction接口，能够灵活地获取底层处理数据和信息，例如状态数据的操作、定时器的注册以及事件触发周期的控制等。根据数据元素是否进行了KeyBy操作，可以将ProcessFunction分为

1. `KeyedProcessFunction`：KeyedProcessFunction使用相对较多，常见的实现类有TopNFunction、GroupAggFunction等函数

2. `ProcessFunction`：主要实现类是`LookupJoinRunner`，主要用于实现维表的关联等操作。

Table API模块相关的Operator直接实现自ProcessFunction接口。

```java
public abstract class KeyedProcessFunction<K, I, O> extends AbstractRichFunction {
			......
			
    // 读取数据元素并处理，根据实际情况创建定时器
    // 定时器会被注册到Context的TimerService定时器队列中
    // 当满足定时器触发的时间条件后，调用OnTimer()方法执行定时器中的计算逻辑，例如对状态数据的异步清理操作
    public abstract void processElement(I value, Context ctx, Collector<O> out) throws Exception;
    public void onTimer(long timestamp, OnTimerContext ctx, Collector<O> out) throws Exception {}
    
    // 主要定义了从数据元素中获取Timestamp和从运行时中获取TimerService等信息的方法
    // 还有用于旁路输出的output()方法
    public abstract class Context {
        public abstract Long timestamp();
        public abstract TimerService timerService();
        public abstract <X> void output(OutputTag<X> outputTag, X value);
        public abstract K getCurrentKey();
    }

    // 继承自Context抽象类，主要应用在KeyedProcessFunction的OnTimer()方法中
    public abstract class OnTimerContext extends Context {
        public abstract TimeDomain timeDomain();
        public abstract K getCurrentKey();
    }
}
```



```java
// package org.apache.flink.table.runtime.operators.deduplicate;
abstract class DeduplicateFunctionBase<T, K, IN, OUT> extends KeyedProcessFunction<K, IN, OUT> ;
public class ProcTimeDeduplicateKeepLastRowFunction
        extends DeduplicateFunctionBase<RowData, RowData, RowData, RowData> {}
```

# TimeService

在Operator中如何对时间信息进行有效的协调和管理呢？在每个Operator内部都维系了一个TimerService，专门用于处理与时间相关的操作。例如获取当前算子中最新的处理时间以及Watermark、注册不同时间类型的定时器等。

在Flink中，时间概念主要分为三种类型，即事件时间、处理时间以及接入时间，每种时间的定义和使用范围如下所示：

| 概念类型      | 事件时间                                         | 处理时间                                   | 接入时间                                |
| ------------- | ------------------------------------------------ | ------------------------------------------ | --------------------------------------- |
| 产生时间      | 事件产生的时间，通过数据中的某个时间字段抽取获得 | 数据在流系统中处理所在算子的计算机系统时间 | 数据在接入Flink过程中接入算子产生的时间 |
| WaterMark支持 | 基于事件时间生成Watermark                        | 不支持生成WaterMark                        | 支持自动生成Watermark                   |
| 时间特性      | 能够反映数据产生的先后顺序                       | 仅表示数据在处理过程中的先后关系           | 表示数据接入过程的先后关系              |
| 应用范围      | 结果确定，可以复现每次数据处理的结果             | 无法复现每次数据处理的结果                 | 无法复现每次数据处理的结果              |

通过如下三种方式可以抽获和生成Timestamp和Watermark：

1. 在SourceFunction中抽取Timestamp和生成Watermark。

   ```java
   public interface SourceFunction<T> extends Function, Serializable {
       @Public // Interface might be extended in the future with additional methods.
       interface SourceContext<T> {
           // 抽取Timestamp
           void collectWithTimestamp(T element, long timestamp);
           // 生成Watermark
           void emitWatermark(Watermark mark);
       }
   }
   ```

   如果Flink作业基于事件时间的概念，就会使用`StreamSourceContexts.ManualWatermarkContext`处理Watermark信息，而`ManualWatermarkContext`继承自`WatermarkContext`。`WatermarkContext.collectWithTimestamp`方法直接从Source算子接入的数据中抽取事件时间的时间戳信息。生成Watermark主要是调用`WatermarkContext.emitWatermark()`方法进行的。生成的Watermark首先会更新当前Source算子中的CurrentWatermark，然后将Watermark传递给下游的算子继续处理。当下游算子接收到Watermark事件后，也会更新当前算子内部的CurrentWatermark。

   ```java
   private abstract static class WatermarkContext<T> implements SourceFunction.SourceContext<T> {
   @Override
   public final void collectWithTimestamp(T element, long timestamp) {
       synchronized (checkpointLock) {
           processAndEmitWatermarkStatus(WatermarkStatus.ACTIVE);
           if (nextCheck != null) {
               this.failOnNextCheck = false;
           } else {
               scheduleNextIdleDetectionTask();
           }
           // 抽取Timestamp信息
           processAndCollectWithTimestamp(element, timestamp);
       }
   }
   
   @Override
   public final void emitWatermark(Watermark mark) {
       if (allowWatermark(mark)) {
           synchronized (checkpointLock) {
               processAndEmitWatermarkStatus(WatermarkStatus.ACTIVE);
               if (nextCheck != null) {
                   this.failOnNextCheck = false;
               } else {
                   scheduleNextIdleDetectionTask();
               }
   		   // 处理并发送Watermark至下游的算子继续处理
               processAndEmitWatermark(mark);
           }
       }
   }
   }
   ```

2. 通过DataStream中的独立算子抽取Timestamp和生成Watermark

   也可以在DataStream数据转换的过程中进行相应操作，此时转换操作对应的算子就能使用生成的Timestamp和Watermark信息了。在DataStream API中提供了3种与抽取Timestamp和生成Watermark相关的Function接口，分别为

   1. `TimestampExtractor`：定义抽取Timestamp的方法，在`AssignerWithPeriodicWatermarks`和`AssignerWithPunctuatedWatermarks`接口中定义生成Watermark的方法。

   2. `AssignerWithPeriodicWatermarks`:事件时间驱动，会周期性地根据事件时间与当前算子中最大的Watermark进行对比，如果当前的EventTime大于Watermark，则触发Watermark更新逻辑，将最新的EventTime赋予CurrentWatermark，并将新生成的Watermark推送至下游算子。AssignerWithPeriodicWatermarks中生成Watermark的默认周期为0，用户可以根据具体情况对周期进行调整，但周期过大会增加数据处理的时延。
   3. `AssignerWithPunctuatedWatermarks`。特殊事件驱动，主要根据数据元素中的特殊事件生成Watermark。例如数据中有产生Watermark的标记，接入数据元素时就会根据该标记调用相关方法生成Watermark。

   

   ```java
   // package org.apache.flink.api.common.eventtime;
   public interface TimestampAssigner<T> {
       long NO_TIMESTAMP = Long.MIN_VALUE;
       long extractTimestamp(T element, long recordTimestamp);
   }
   
   public interface AssignerWithPunctuatedWatermarks<T> extends TimestampAssigner<T> {
       @Nullable
       Watermark checkAndGetNextWatermark(T lastElement, long extractedTimestamp);
   }
   
   public interface AssignerWithPeriodicWatermarks<T> extends TimestampAssigner<T> {
       @Nullable
       Watermark getCurrentWatermark();
   }
   
   // 如果接入事件中的Timestamp是单调递增的，即不会出现乱序的情况
   // 可以直接使用AssignerWithPeriodicWatermarks接口的默认抽象实现类
   // AscendingTimestampExtractor自动生成Watermark
   public abstract class AscendingTimestampExtractor<T>
       	implements AssignerWithPeriodicWatermarks<T> {
       // 需要用户自定义实现该方法来获取EventTime信息
       public abstract long extractAscendingTimestamp(T element);
   }
   
   // 接入数据是有界乱序的情况
   // 可以使用BoundedOutOfOrdernessTimestampExtractor实现类生成Watermark事件
   public abstract class BoundedOutOfOrdernessTimestampExtractor<T>
           implements AssignerWithPeriodicWatermarks<T> {
       // 需要用户自定义实现该方法来获取EventTime信息
       public abstract long extractTimestamp(T element);
   }
   
   @Internal
   public class PeriodicWatermarkAssignerWrapper implements AssignerWithPeriodicWatermarks<RowData>;
   ```

   ```java
   public final class IngestionTimeAssigner<T> implements TimestampAssigner<T> {
       private long maxTimestamp;
   
       @Override
       public long extractTimestamp(T element, long recordTimestamp) {
           // make sure timestamps are monotonously increasing, even when the system clock re-syncs
           final long now = Math.max(System.currentTimeMillis(), maxTimestamp);
           maxTimestamp = now;
           return now;
       }
   }
   ```

   

3. 通过Connector提供的接口抽取Timestamp和生成Watermark



