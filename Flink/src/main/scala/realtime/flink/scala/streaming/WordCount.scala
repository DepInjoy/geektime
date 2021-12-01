package realtime.flink.scala.streaming

import org.apache.flink.streaming.api.scala._
import org.apache.flink.util.Collector
import org.apache.flink.api.common.functions.{FlatMapFunction, RichFlatMapFunction}
import org.apache.flink.api.java.utils.ParameterTool;
/**
 *    本程序重点是了解Flink的编程结构以及Scala编程实现的简便之处
 *    执行需要添加分析文件信息，--input src\main\resources\WordCountData.txt
 */

class MyFlatMapFunction extends FlatMapFunction[String, String] {
  override def flatMap(t : String, out : Collector[String]) : Unit = {
    t.toLowerCase.split(" ").foreach(out.collect)
  }
}

class MyRichFlatMapFunction extends RichFlatMapFunction[String, String] {
  override def flatMap(in: String, out: Collector[String]): Unit = {
    in.toLowerCase.split(" ").foreach(out.collect)
  }
}

object WodCount {
  def main(args: Array[String]): Unit = {
    val inputParam = ParameterTool.fromArgs(args)
    if (!inputParam.has("input")) {
      System.out.println("Please make sure your input!")
      return;
    }

    // 1. 创建执行环境
    val env = StreamExecutionEnvironment.getExecutionEnvironment;
    // 2. 获取输入
    val text = env.readTextFile(inputParam.get("input"))

    // 3. 进行数据转换操作
    val counts: DataStream[(String, Int)] = text
      //      .flatMap(_.toLowerCase.split(" "))                // 实现1：FlapMap算子
      //       .flatMap(new MyFlatMapFunction)                  // 实现2：采用创建Class实现Funciton接口
      //        .flatMap(new FlatMapFunction[String, String] {
      //          override def flatMap(t : String, out : Collector[String]) : Unit = {
      //            t.toLowerCase.split(" ").foreach(out.collect)
      //          }
      //        })                                            // 实现3: 采用创建匿名类实现Funciton接口
      .flatMap(new MyRichFlatMapFunction)               // 实现4：通过RichFunciton实现接口
      .filter(_.nonEmpty)
      .map((_, 1))
      .keyBy(value => value._1)
      .sum(1)

    // 4. 指定计算结果输出位置
    if (inputParam.has("output")) {
      counts.writeAsText(inputParam.get("output"))
    } else {
      println("Printing result to stdout. Use --output to specify output path.")
      counts.print()
    }

    // 5. 指定名称并触发流式任务
    env.execute("Streaming WordCount")
  }
}
