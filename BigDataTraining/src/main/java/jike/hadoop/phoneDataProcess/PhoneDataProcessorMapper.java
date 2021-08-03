package jike.hadoop.phoneDataProcess;

import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;
import java.io.IOException;

public class PhoneDataProcessorMapper extends Mapper<LongWritable, Text, Text, FlowBean> {
    @Override
    public void map(LongWritable key, Text value, Context context)
            throws IOException,InterruptedException{
        FlowBean valueBean = new FlowBean();
        Text phoneNumber = new Text();
        String[] splitText = value.toString().split("\t");
        phoneNumber.set(splitText[1]);
        int textLen = splitText.length;
        long upFlow = Long.parseLong(splitText[textLen - 3]);
        long downFlow = Long.parseLong(splitText[textLen - 2]);
        valueBean.set(upFlow, downFlow);
        context.write(phoneNumber, valueBean);
    }
}
