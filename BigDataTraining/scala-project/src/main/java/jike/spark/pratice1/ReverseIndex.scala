package jike.spark.pratice1

import org.apache.spark.sql.SparkSession;
import org.apache.spark.rdd.RDD;

/*
*   @func : 定义特质统一倒排索引和词频统计接口
* */
trait Transformer[S, T] {
  def transform(rdd: RDD[S]): RDD[T]
}

/*
* @func：    利用Spark RDD实现倒排索引
*
* @param：   RDD with (file name, file data) format
* @return：  RDD with ((word, file name) word count) format
* */
class ReverseIndex extends Transformer[(String, String), ((String, String), Int)] {
  override def transform(rdd: RDD[(String, String)]): RDD[((String, String), Int)] = {
    rdd
      .map(x => x._2.split(" ")
        .map(words => ((words, x._1), 1)))
      .flatMap(wordFileMap => wordFileMap)
      .reduceByKey(_ + _)
  }
}

/*
* @func:    格式化倒排索引和词频统计的结果
*
* @param:   RDD with ((word, file name), word count) format
* @return:  RDD with (word, (file name, word count)) format
* */
class FormatOutput extends Transformer[((String, String), Int), (String, Iterable[(String, Int)])] {
  override def transform(rdd: RDD[((String, String), Int)]): RDD[(String, Iterable[(String, Int)])] = {
    rdd.map {
      wordFileMap =>
        val word = wordFileMap._1._1
        val fileName = wordFileMap._1._2
        (word, (fileName, wordFileMap._2))
    }.groupByKey()
  }
}

object ReverseIndex {
  /* 文件实现功能： 利用Spark RDD实现倒排索引，打印最终的统计结果 */
  def main(args : Array[String]): Unit = {
    val spark_session = SparkSession
      .builder.master("local")
      .appName("ReverseIndex")
      .getOrCreate()

    // 定义文件路径
    val filePaths : Array[String] = Array(
      "data/ReverseIndex/0.txt",
      "data/ReverseIndex/1.txt",
      "data/ReverseIndex/2.txt"
    )

    // 构造输入RDD,(文件名, 文件内容)
    var inputRDD : RDD[(String, String)] = null;
    for(filePath <- filePaths) {
      def curRDD(filePath : String) : RDD[(String, String)] = {
        return spark_session.read.textFile(filePath).rdd.map {
          fileData =>
            val splits = filePath.split("/");
            val fileName: String = splits(splits.length - 1)
            (fileName, fileData)
        }
      }
      if (inputRDD == null) {
        inputRDD = curRDD(filePath);
      } else {
        inputRDD = inputRDD.union(curRDD(filePath))
      }
    }

    // 倒排索引->词频统计->结果格式化
    val finalReverseIndex = new FormatOutput().transform(
        new ReverseIndex().transform(inputRDD)
    )

    println("-------------------------- 倒排索引结果开始 --------------------------------")
    finalReverseIndex.collect().foreach(println)
    println("-------------------------- 倒排索引结果结束 --------------------------------")

    // 停止SparkSession
    spark_session.stop()
  }
}
