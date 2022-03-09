package algorithm.practice.dc.week1;

import java.io.File;
import java.util.LinkedList;

/**
 * @brief:      给定一个文件目录的路径，统计这个目录下所有的文件数量并返回
 *              隐藏文件也算，但是文件夹不算
 *
 *              实现思想:宽度/深度优先遍历
 * */
public class DC_01_02_CountFiles {
    public static int getFileNumber(String folderPath) {
        File root = new File(folderPath);
        if (!root.isFile() && !root.isDirectory()) return 0;
        if (root.isFile()) return 1;

        LinkedList<File> queue = new LinkedList<>();
        queue.add(root);
        int fileNum = 0;
        while (!queue.isEmpty()) {
            File folder = queue.poll();
            for (File file : folder.listFiles()) {
                if (file.isFile()) fileNum++;
                if (file.isDirectory()) queue.add(file);
            }
        }
        return fileNum;
    }

    public static void main(String[] args) {
        System.out.println(getFileNumber("D:\\project\\code\\tmp"));
    }
}
