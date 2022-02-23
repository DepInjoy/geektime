/**
 * @brief       给定一个文件目录的路径，
 *              写一个函数统计这个目录下所有的文件数量并返回
 *              隐藏文件也算，但是文件夹不算
 * 
 *              实现思想：类似于宽度/深度优先遍历
 * 
 */
#include <string>
#include <iostream>
#include <queue>
#include <list>
#include <sys/stat.h>

int getFileNumber(const std::string& folderPath) {
    int fileNum = 0;
    struct stat fileStat;
    stat(folderPath.c_str(), &fileStat);
    if (fileStat.st_mode && S_IFREG) { // file
        return 0;
    }

    std::queue<std::string> dirQueue;
    dirQueue.push(folderPath);
    while (!dirQueue.empty()) {
        std::string cur = dirQueue.front();
        std::list<std::string> fileList; // cur下所有文件文件
        for (std::string path : fileList) {
            stat(path.c_str(), &fileStat);
            if (fileStat.st_mode && S_IFREG) {
                fileNum++;
            } else if (fileStat.st_mode && S_IFDIR) {
                dirQueue.push(path);
            }
        }
        dirQueue.pop();
    }
    return fileNum;
}

int main(int argc, char* argv[]) {
    std::cout << getFileNumber("./") << std::endl;
    return 0;
}