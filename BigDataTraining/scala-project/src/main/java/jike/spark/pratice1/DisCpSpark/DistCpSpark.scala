package jike.spark.pratice1.DisCpSpark

import org.apache.spark.sql.SparkSession;
import org.apache.spark.rdd.RDD;

object DistCpSpark {
  /*
  * 功能：   在Spark中应使用RDD实现HadoopDistCp的copy功能
  *
  * HadoopDistCp的功能与实现相关资料：
  *   https://hadoop.apache.org/docs/current/hadoop-distcp/DistCp.html
  *   https://github.com/apache/hadoop/tree/release-2.7.1/hadoop-tools/hadoop-distcp
  * */
  def main(args: Array[String]): Unit = {
    val spark_session = SparkSession
      .builder.master("local")
      .appName("ReverseIndex")
      .getOrCreate()

  }
}
