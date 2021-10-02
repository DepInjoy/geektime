package geek.flink.base;

import akka.stream.impl.fusing.Collect;
import org.apache.flink.api.common.functions.FlatMapFunction;
import org.apache.flink.api.java.DataSet;
import org.apache.flink.api.java.ExecutionEnvironment;
import org.apache.flink.api.java.utils.MultipleParameterTool;
import org.apache.flink.streaming.api.datastream.DataStream;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;
import org.apache.flink.util.Collector;
import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.util.Preconditions;

/**
 * 以批处理和流处理两种方式实现WordCout,需要注意的是两种实现的结果的不同
 * 运行：
 *  -input src/main/resources/WordCountData.txt -output /mnt/output
 *  -input src/main/resources/WordCountData.txt
 *
 * 批处理的结果
 * (is,1)
 * (a,2)
 * (or,1)
 * (not,1)
 * (hello,1)
 * (to,2)
 * (be,2)
 * (this,2)
 * (banana,1)
 * (word,1)
 * (question,1)
 *
 * 流处理的结果：
 * (this,1)
 * (this,2)
 * (hello,1)
 * (not,1)
 * (banana,1)
 * (question,1)
 * (a,1)
 * (to,1)
 * (be,1)
 * (or,1)
 * (to,2)
 * (be,2)
 * (a,2)
 * (word,1)
 * (is,1)
 */

public class WindowWordCount {
    private enum ProcessMethod {
        BATCH_PROCESS,
        STREAMING_PROCESS
    }

    /**
     * 将每一行数据进行切分转换为(word, 1)的形式
     */
    public static class TextSpliter implements FlatMapFunction<String, Tuple2<String, Integer> > {
        public void flatMap(String sentence, Collector< Tuple2<String, Integer> > out) {
            for (String word : sentence.toLowerCase().split("\\W+")) {
                out.collect(new Tuple2<>(word, 1));
            }
        }
    }

    public  static void main(String[] args) {
        ProcessMethod method = ProcessMethod.STREAMING_PROCESS;
        try {
            // 读取输入参数
            MultipleParameterTool inputParams = MultipleParameterTool.fromArgs(args);
            if (!inputParams.has("input")) {
                System.out.println("Please make your input!");
                return;
            }

            switch (method) {
                case BATCH_PROCESS:
                    ExecutionEnvironment batchEnv = ExecutionEnvironment.getExecutionEnvironment();
                    batchEnv.getConfig().setGlobalJobParameters(inputParams);
                    // 获取数据源并进行一些转换操作
                    DataSet<String> batchText = null;
                    for (String in : inputParams.getMultiParameter("input")) {
                        if (batchText == null) {
                            batchText = batchEnv.readTextFile(in);
                        } else {
                            batchText.union(batchEnv.readTextFile(in));
                        }
                    }
                    Preconditions.checkNotNull(batchText, "Input DataStream should not be null.");
                    DataSet<Tuple2<String, Integer>> dataSet = batchText
                            .flatMap(new TextSpliter())
                            .groupBy(0)
                            .sum(1);
                    // Sink操作
                    if (inputParams.has("output")) {
                        dataSet.writeAsText(inputParams.get("output"));
                        // 执行应用程序
                        batchEnv.execute("Flink Batch WordCount");
                    } else {
                        // WARNING: 如果直接在终端打印，则无需执行execute，否则会有异常抛出
                        dataSet.print();
                    }
                    break;
                case STREAMING_PROCESS:
                    // 设置运行环境
                    StreamExecutionEnvironment env = StreamExecutionEnvironment.getExecutionEnvironment();
                    // 使Web接口也使用该参数
                    env.getConfig().setGlobalJobParameters(inputParams);
                    // 获取数据源并进行一些转换操作
                    DataStream<String> text = null;
                    for (String in : inputParams.getMultiParameter("input")) {
                        if (text == null) {
                                text = env.readTextFile(in);
                        } else {
                            text.union(env.readTextFile(in));
                        }
                    }
                    Preconditions.checkNotNull(text, "Input DataStream should not be null.");
                    DataStream<Tuple2<String, Integer>> dataStream = text
                            .flatMap(new TextSpliter())
                            // 根据Tuple中的0字段进行分组并将第1个字段相加
                            .keyBy(value -> value.f0)
                            .sum(1);

                    // Sink操作
                    if (inputParams.has("output")) {
                        dataStream.writeAsText(inputParams.get("output"));
                    } else {
                        dataStream.print();
                    }
                    // 执行应用程序
                    env.execute("Flink Stream WordCount");
                    break;
                default:
                    System.out.println("Unknown Process method.");
                    break;
            }
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Meet exception: " + e.getMessage());
        }
    }
}
