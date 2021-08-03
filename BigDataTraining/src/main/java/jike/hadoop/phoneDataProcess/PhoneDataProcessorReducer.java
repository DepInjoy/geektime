package jike.hadoop.phoneDataProcess;

import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;
import java.io.IOException;

public class PhoneDataProcessorReducer extends Reducer<Text,FlowBean,Text,FlowBean> {
    @Override
    protected void reduce(Text key, Iterable<FlowBean> values,Context context)
            throws IOException,InterruptedException {
        FlowBean reduceBean = new FlowBean();
        long sumUpFlow = 0, sumDownFlow = 0;
        for (FlowBean value : values) {
            sumUpFlow += value.getUpFlow();
            sumDownFlow += value.getDownFlow();
        }
        reduceBean.set(sumUpFlow, sumDownFlow);
        context.write(key, reduceBean);
    }
}
