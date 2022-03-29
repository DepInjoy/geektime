#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

/**
 *  @brief:     序列迭代器失效处理
 *              将erase返回的下一个有效的迭代器赋值给iter，来避免迭代器失效
 *
 *              操作示例：将数据是奇数的迭代器删除
*/
void vector_iter_erase() {
    std::vector<int> vect {0, 1, 2, 3, 4, 5};
    for (auto iter = vect.begin(); iter != vect.end();) {
        if (*iter & 1) {
            // 

            iter = vect.erase(iter);
        } else {
            iter++;
        }
    }

    // 展示所有的有效数据
    std::cout << "The vector item's ";
    for (auto iter = vect.begin(); iter != vect.end(); iter++) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

/**
 *  @brief:     链表迭代器失效处理，有两种方式：
 *                  1. 将erase返回的下一个有效的迭代器赋值给iter，来避免迭代器失效
 *                  2. 将当前的迭代器iter++,指向下一个有效迭代器
 *
 *              操作示例：将数据是奇数的迭代器删除
*/
void list_iter_erase() {
    std::list<int> lst {0, 1, 2, 3, 4, 5};
#if 0
    for (auto iter = lst.begin(); iter != lst.end();) {
        if (*iter & 1) {
            lst.erase(iter++);
        } else {
            iter++;
        }
    }
#else
    for (auto iter = lst.begin(); iter != lst.end();) {
        if (*iter & 1) {
            iter = lst.erase(iter);
        } else {
            iter++;
        }
    }
#endif

    // 展示所有有效数据
    std::cout << "The list item's ";
    for (auto iter = lst.begin(); iter != lst.end(); iter++) {
        std::cout << *iter << " ";
    }
    std::cout << std::endl;
}

/**
 *  @brief:     树形迭代器失效处理，erase接口返回值时void
 *              指向下一个有效迭代器来避免迭代器失效
 *
 *              操作示例：将数据是key是奇数的迭代器删除
*/
void map_iter_erase() {
    std::unordered_map<int, int> umap {
        {0, 1}, {1, 6}, {2, 6}, {3, 6}, {4, 8}, {5, 8}
    };
    for (auto iter = umap.begin(); iter != umap.end();) {
        if (iter->first & 1) {
            umap.erase(iter++);
        } else {
            iter++;
        }
    }

    std::cout << "The unordered_map item's " << std::endl;
    for (auto iter = umap.begin(); iter != umap.end(); iter++) {
        std::cout << "\t" << iter->first << " : " << iter->second << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    vector_iter_erase();
    list_iter_erase();
    map_iter_erase();
    return 0;
}