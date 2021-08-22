package jike.spark.practice

import org.apache.spark.sql.SparkSession;
import org.apache.spark.rdd.RDD;

/*
  功能：利用Spark RDD实现倒排索引和词频统计，打印最终的统计结果 */

/*
*   @func : 定义特质统一倒排索引和词频统计接口
* */
trait Transformer[S, T] {
  def transform(rdd: RDD[S]): RDD[T]
}

/*
  @func：利用Spark RDD实现倒排索引
* */
class ReverseIndex extends Transformer[String, (String, String)] {
  override def transform(rdd: RDD[String]): RDD[(String, String)] = {
    rdd
      .map(x => ("1", x))
      .map(x => x._2.split(" ")
        .map(words => (words, x._1)))
      .flatMap(file_word => file_word)
      .reduceByKey((word, file) => (word + "," + file))
  }
}
/*
*   功能：利用Spark RDD实现词频统计
* */
class WordCount extends Transformer[String, (String, Int)] {
  override def transform(rdd: RDD[String]): RDD[(String, Int)] = {
    rdd
      .flatMap(line => line.split(" "))
      .map(word => (word, 1))
      .reduceByKey(_ + _)
      .sortByKey()
  }
}

object ReverseIndex {
  def main(args : Array[String]): Unit = {
    val spark_session = SparkSession
      .builder.master("local")
      .appName("ReverseIndex")
      .getOrCreate()

    // 定义文件路径
    val file_path : String = "D:\\share\\project\\data\\3.txt"

    val word_count = new WordCount().transform(spark_session.read.textFile(file_path).rdd)
    println("-------------------------- 倒排索引结果 --------------------------------")
    val reverse_index = new ReverseIndex().transform(spark_session.read.textFile(file_path).rdd)
    reverse_index.collect().foreach(println)
    println()

    println("-------------------------- 词频统计结果 --------------------------------")
    word_count.collect().foreach(println)
    // 停止SparkSession
    spark_session.stop()
  }
}
