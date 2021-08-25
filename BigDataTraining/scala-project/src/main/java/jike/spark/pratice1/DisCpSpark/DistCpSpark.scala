package jike.spark.pratice1.DisCpSpark

import org.apache.spark.sql.SparkSession;
import org.apache.spark.rdd.RDD;

import org.apache.hadoop.tools.OptionsParser;
import org.apache.hadoop.tools.DistCpConstants
import org.apache.hadoop.tools.DistCpContext;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.fs.FileSystem

/*
* 功能：   在Spark中应使用RDD实现HadoopDistCp的copy功能
*
* HadoopDistCp的功能与实现相关资料：
*   https://hadoop.apache.org/docs/current/hadoop-distcp/DistCp.html
*   https://github.com/apache/hadoop/tree/release-2.7.1/hadoop-tools/hadoop-distcp
* */

object DistCpSpark {
  private var context : DistCpContext = _;
  private var spark_session : SparkSession = _;

  import org.apache.hadoop.tools.DistCpConstants
  import org.apache.hadoop.tools.DistCpOptionSwitch
  import java.io.IOException

  /**
   * Check splitting large files is supported and populate configs.
   */
  @throws[IOException]
  private def checkSplitLargeFile(): Unit = {
    if (!context.splitLargeFile)
      return

    val targetPath = context.getTargetPath
    val targetFS = targetPath.getFileSystem(spark_session.sparkContext.hadoopConfiguration)
    try {
      val src = null
      val tgt = null
      targetFS.concat(tgt, src)
    } catch {
      case use: UnsupportedOperationException =>
        throw new UnsupportedOperationException(DistCpOptionSwitch.BLOCKS_PER_CHUNK.getSwitch
          + " is not supported since the target file system doesn't" + " support concat.", use)
      case e: Exception =>
        // Ignore other exception
    }
    println("Set " + DistCpConstants.CONF_LABEL_SIMPLE_LISTING_RANDOMIZE_FILES
      + " to false since " + DistCpOptionSwitch.BLOCKS_PER_CHUNK.getSwitch + " is passed.")
    spark_session.sparkContext.hadoopConfiguration
      .setBoolean(DistCpConstants.CONF_LABEL_SIMPLE_LISTING_RANDOMIZE_FILES, false)
  }

  /*
  *   @func:  set target path exists in DistCpContext and Configuration
  * */
  def setTargetPathExists(): Unit = {
    val targetPath = context.getTargetPath()
    val targetFS = targetPath.getFileSystem(
        spark_session.sparkContext.hadoopConfiguration)
    val targetExist = targetFS.exists(targetPath)
    context.setTargetPathExists(targetExist)
    spark_session.sparkContext.hadoopConfiguration.setBoolean(
      DistCpConstants.CONF_LABEL_TARGET_PATH_EXISTS, targetExist
    )
  }


  def main(args: Array[String]): Unit = {
    if (args.length < 1) {
      OptionsParser.usage()
      return DistCpConstants.INVALID_ARGUMENT
    }

    try {
      context = new DistCpContext(OptionsParser.parse(args))
      spark_session = SparkSession
        .builder.master("local")
        .appName("DistCpSpark")
        .getOrCreate()
      checkSplitLargeFile();
      setTargetPathExists();
    } catch {
      case e : Throwable => {
        spark_session.stop()
        println( "Invalid arguments: ", e.getMessage())
        OptionsParser.usage()
        return DistCpConstants.INVALID_ARGUMENT
      }
    }

  }
}
