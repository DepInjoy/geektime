package realtime.flink.java.batch;

import geek.flink.base.WindowWordCount;
import org.apache.flink.api.common.functions.FlatMapFunction;
import org.apache.flink.api.java.DataSet;
import org.apache.flink.api.java.ExecutionEnvironment;
import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.api.java.utils.MultipleParameterTool;
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
                    .flatMap(new WindowWordCount.TextSpliter())
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
        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("Meet exception: " + e.getMessage());
        }
    }
}
