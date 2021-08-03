package jike.hadoop.phoneDataProcess;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;
import org.apache.hadoop.io.Text;


public class PhoneDataProcessor {
    public static void main(String[] args) throws Exception {
        Configuration conf = new Configuration();
        //获取运行时输入的参数
        String [] otherArgs = new GenericOptionsParser(conf, args)
                .getRemainingArgs();
        if(otherArgs.length < 2){
            System.err.println("必须输入读取文件路径和输出路径");
            System.exit(2);
        }

        // 获取Job实例
        Job job = Job.getInstance(conf);
        // 设置类路径Classpath
        job.setJarByClass(PhoneDataProcessor.class);
        job.setJobName("xieyue");
        //设置mapper
        job.setMapperClass(PhoneDataProcessorMapper.class);
        //设置reducer
        job.setReducerClass(PhoneDataProcessorReducer.class);
        //设置reduce函数的key值
        job.setOutputKeyClass(Text.class);
        //设置reduce函数的value值
        job.setOutputValueClass(FlowBean.class);

        //设置读取文件的路径
        FileInputFormat.addInputPath(job, new Path(args[0]));
        //设置输出路径
        FileOutputFormat.setOutputPath(job, new Path(args[1]));

        // 提交Job
        System.exit(job.waitForCompletion(true) ? 0 : 1);
    }
}
