package jike.hadoop.studentIdRPC.Impl;

import jike.hadoop.studentIdRPC.MyInterface;

import org.apache.hadoop.ipc.ProtocolSignature;

import java.io.IOException;
import java.util.HashMap;

public class MyInterfaceImpl implements MyInterface {
    /**
     *  实现加法
     * @param number1
     * @param number2
     * @return
     */
    @Override
    public int add(int number1, int number2) {
        System.out.println("number1 = " + number1 + "number2 = " + number2);
        return number1+number2;
    }

    /**
     * 返回版本号
     * @param protocol
     * @param clientVersion
     * @return
     * @throws IOException
     */
    @Override
    public long getProtocolVersion(String protocol, long clientVersion) throws IOException {
        return MyInterface.versionID;
    }

    /**
     * 用于获取协议签名
     * */
    @Override
    public ProtocolSignature getProtocolSignature(String s, long l, int i) throws IOException {
        return new ProtocolSignature(MyInterface.versionID, null);
    }

    @Override
    public String findName(String studentId) throws IOException {
        HashMap<String, String> numNameMap = new HashMap<String, String>();
        numNameMap.put("20210000000000", "null");
        numNameMap.put("20210123456789", "心心");
        String name = null;
        for (String key : numNameMap.keySet()) {
            if (key.equals(studentId)) {
                name = numNameMap.get(key);
                break;
            }
        }
        return name;
    }
}
