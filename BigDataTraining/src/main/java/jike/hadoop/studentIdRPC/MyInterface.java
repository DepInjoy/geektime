package jike.hadoop.studentIdRPC;

import org.apache.hadoop.ipc.VersionedProtocol;
import java.io.IOException;

public class MyInterface {
    long versionID = 1L;
    int add(int number1,int nunmber2);
    String findName(String studentId) throws IOException;
}
