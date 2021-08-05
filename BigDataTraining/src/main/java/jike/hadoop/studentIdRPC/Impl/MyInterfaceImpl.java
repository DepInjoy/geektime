package jike.hadoop.studentIdRPC.Impl;

import jike.hadoop.studentIdRPC.MyInterface;

public class MyInterfaceImpl implements MyInterface {
    /**
     *  实现加法
     * @param number1
     * @param nunmber2
     * @return
     */
    @Override
    public int add(int number1, int nunmber2) {
        System.out.println("number1 = "+number1+"number2 = "+nunmber2);
        return number1+nunmber2;
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
}
