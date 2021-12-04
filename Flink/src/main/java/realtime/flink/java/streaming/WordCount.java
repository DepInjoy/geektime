package realtime.flink.java.streaming;

import geek.flink.base.WindowWordCount;
import org.apache.flink.api.common.functions.FlatMapFunction;
import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.api.java.utils.MultipleParameterTool;
import org.apache.flink.streaming.api.datastream.DataStream;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;
import org.apache.flink.util.Collector;
import org.apache.flink.util.Preconditions;

public class WordCount {

    /**
     * 将每一行数据进行切分转换为(word, 1)的形式
     */
    public static class TextSpliter implements FlatMapFunction<String, Tuple2<String, Integer>> {
        public void flatMap(String sentence, Collector< Tuple2<String, Integer> > out) {
            for (String word : sentence.toLowerCase().split("\\W+")) {
                out.collect(new Tuple2<>(word, 1));
            }
        }
    }

    public  static void main(String[] args) {
        try {
            // 读取输入参数
            MultipleParameterTool inputParams = MultipleParameterTool.fromArgs(args);
            if (!inputParams.has("input")) {
                System.out.println("Please make your input!");
                return;
            }

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
                    .flatMap(new WindowWordCount.TextSpliter())
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
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Meet exception: " + e.getMessage());
        }
    }
}
