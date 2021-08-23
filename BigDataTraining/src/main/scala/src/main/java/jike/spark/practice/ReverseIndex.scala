package jike.spark.practice

import org.apache.spark.sql.SparkSession;
import org.apache.spark.rdd.RDD;

/*文件实现功能：利用Spark RDD实现倒排索引，打印最终的统计结果 */

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
* @return：  RDD with (word, file name) format
* */
class ReverseIndex extends Transformer[(String, String), (String, String)] {
  override def transform(rdd: RDD[(String, String)]): RDD[(String, String)] = {
    rdd
      .map(x => x._2.split(" ")
        .map(words => (words, x._1)))
      .flatMap(wordFileMap => wordFileMap)
  }
}


/*
*   @func:  利用Spark RDD实现词频统计
*
*   @param: RDD with (word, file name) format
*   @return:  RDD with ((word, file name), word count) format
* */
class WordCount extends Transformer[(String, String), ((String, String), Int)] {
  override def transform(rdd: RDD[(String, String)]): RDD[((String, String), Int)] = {
    rdd
      .map(word => (word, 1))
      .reduceByKey(_ + _)
  }
}

/*
* @func:    格式化倒排索引和词频统计的结果
*
* @param:   RDD with ((word, file name), word count) format
* @return:  RDD with (word, (file name, word count)) format
* */
class FormatOutput extends Transformer[((String, String), Int), (String, (String, Int))] {
  override def transform(rdd: RDD[((String, String), Int)]): RDD[(String, (String, Int))] = {
    rdd.map {
      wordFileMap =>
        val word = wordFileMap._1._1
        val fileName = wordFileMap._1._2
        (word, (fileName, wordFileMap._2))
    }
  }
}

//def getFiles(val path : String) : Array[String]{
//  val configuration:Configuration = new Configuration()
////    configuration.set("fs.defaultFS", hdfsFileName)
//val fileSystem:FileSystem = FileSystem.get(configuration)
//val fsPath: Path = new Path(hdfsDirectory)
//val iterator = fileSystem.listFiles(fsPath, true)
//val list = new ListBuffer[String]
//while (iterator.hasNext) {
//val pathStatus = iterator.next()
//val hdfsPath = pathStatus.getPath
//val fileName = hdfsPath.getName
//list += fileName // list.append(fileName)
//}
//fileSystem.close()
//list.toArray
//}
object ReverseIndex {
  def main(args : Array[String]): Unit = {
    val spark_session = SparkSession
      .builder.master("local")
      .appName("ReverseIndex")
      .getOrCreate()

    // 定义文件路径
    val filePaths : Array[String] = Array(
      "D:/share/project/data/1.txt",
      "D:/share/project/data/2.txt",
      "D:/share/project/data/3.txt"
    )


    // 构造输入RDD,(文件名, 文件内容)
//    val filePath = "D:/share/project/data/1.txt"
//    var inputRDD = spark_session.read.textFile(filePath).rdd.map {
//      fileData =>
//        val splits = filePath.split("/");
//        val fileName: String = splits(splits.length - 1)
//        (fileName, fileData)
//    }
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
    inputRDD.foreach(println)
    // 倒排索引->词频统计->结果格式化
    val finalReverseIndex = new FormatOutput().transform(
      new WordCount().transform(
        new ReverseIndex().transform(inputRDD)
      )
    )

    println("-------------------------- 倒排索引结果开始 --------------------------------")
    finalReverseIndex.collect().foreach(println)
    println("-------------------------- 倒排索引结果结束 --------------------------------")

    // 停止SparkSession
    spark_session.stop()
  }
}
