#include <iostream>
#include <string>
#include <list>
#include <set>

extern "C"

typedef struct Row {
    int a;
    int b;
} Row;

// 符合检索条件的行信息
struct RowInfo {
    int minRowNum;
    int maxRowNum;
};

// 将数据划分一系列的Batch,然后按照一批批进行数据处理,本测试的数据量较小，设置为2
const uint64_t NUM_PER_BATCH = 2;

void task1(const Row* rows, int nrows) {
    for (int i = 0; i < nrows; ++i) {
        if ((rows[i].a == 1000 || rows[i].a == 2000 || rows[i].a == 3000) &&
            (rows[i].b >= 10 && rows[i].b < 50)) {
                std::cout << rows[i].a << " " << rows[i].b << std::endl;
            }
    }
}

int structToCol(const Row& row, int colNum) {
    switch (colNum) {
    case 0:
        return row.a;
    case 1:
        return row.b;
    default:
        std::cout << "Invalid column number!" << std::endl;
    }
    return -1;
}

/*  @param:
        colNum：    将Row的a和b映射为列数，其中0代表a, 1代表b    
    @return:        搜索数值所在的行数， -1代表未搜索到
*/
bool binarySearch(const Row* rows, const int startRow, const int endRow,
        const int colNum, const int searchNum, RowInfo* rowInfo) {
    if (structToCol(rows[startRow], colNum) < searchNum && structToCol(rows[startRow], colNum) > searchNum)
        return false;

    int left = startRow;
    int right = endRow;
    while (left <= right) {
        int middle = left + (right - left) / 2;
        if (structToCol(rows[middle], colNum) == searchNum) {  // found
            rowInfo->minRowNum = middle;
            rowInfo->maxRowNum = middle;
            // find min
            for (int i = middle - 1; i >= left; --i) {
                if (structToCol(rows[i], colNum) == searchNum) {
                    rowInfo->minRowNum = i;
                } else {
                    break;
                }
            }

            // find max
            for (int i = middle + 1; i <= right; ++i) {
                if (structToCol(rows[i], colNum) == searchNum) {
                    rowInfo->maxRowNum = i;
                } else {
                    break;
                }
            }
            return true;
        } else if (structToCol(rows[middle], colNum) > searchNum) {
            right = middle - 1;
        } else {
            left = middle + 1;
        }
    }
    return false;
}

void task2(const Row* rows, int nrows) {
    int batchNum = nrows % NUM_PER_BATCH ? nrows / NUM_PER_BATCH + 1 : nrows / NUM_PER_BATCH;
    for (int i = 0; i < batchNum; ++i) {
        int startRow = i * NUM_PER_BATCH;
        int endRow = (i + 1) * NUM_PER_BATCH - 1;
        endRow = endRow < nrows ? endRow : nrows;

        std::set<int> condition{1000, 2000, 3000};  // no-repeated and sorted
        RowInfo row_info;
        for (auto iter = condition.begin(); iter != condition.end(); ++iter) {
            if (binarySearch(rows, startRow, endRow, 0, *iter, &row_info)) {
                for (int index = row_info.minRowNum; index <= row_info.maxRowNum; ++index) {
                    if (rows[index].b >= 10 && rows[index].b < 50) {
                        std::cout << rows[index].a << " " << rows[index].b << std::endl;
                    }
                }
                // 继续向下寻找
                startRow = row_info.maxRowNum;
            }
        }
    }
}

void putValueToOrderList(std::list<Row>& sortedRow, int& curMinPos, const Row& newVal) {
    auto iter = sortedRow.begin();
    std::advance(iter, curMinPos);
    for (; iter != sortedRow.end(); ++iter) {
        if (iter->b < newVal.b) {
            ++curMinPos;
        } else {
            sortedRow.insert(iter, newVal);
            return;
        }
    }
}

void task3(const Row* rows, int nrows) {
    int batchNum = nrows % NUM_PER_BATCH ? nrows / NUM_PER_BATCH + 1 : nrows / NUM_PER_BATCH;
    std::list<Row> sortedRes;
    for (int i = 0; i < batchNum; ++i) {
        int startRow = i * NUM_PER_BATCH;
        int endRow = (i + 1) * NUM_PER_BATCH - 1;
        endRow = endRow < nrows ? endRow : nrows;

        bool inited = false;
        RowInfo row_info;
        std::set<int> condition{1000, 2000, 3000};  // no-repeated and sorted
        for (auto iter = condition.begin(); iter != condition.end(); ++iter) {
            if (binarySearch(rows, startRow, endRow, 0, *iter, &row_info)) {
                int curMinPos = 0;
                for (int index = row_info.minRowNum; index <= row_info.maxRowNum; ++index) {
                    if (rows[index].b >= 10 && rows[index].b < 50) {
                        if (!inited) {  // be sorted, keep
                            sortedRes.push_back(rows[index]);
                        } else {  // maybe unsorted, put it in order
                            if (sortedRes.front().b > rows[index].b) {
                                sortedRes.push_front(rows[index]);
                            } else if (sortedRes.back().b < rows[index].b) {
                                sortedRes.push_back(rows[index]);
                            } else {  // middle
                                putValueToOrderList(sortedRes, curMinPos, rows[index]);
                            }
                        }
                    }
                }
                // 继续向下寻找
                startRow = row_info.maxRowNum;
                inited = true;
            }
        }
    }

    // formated output
    for (auto iter = sortedRes.begin(); iter != sortedRes.end(); ++iter) {
        std::cout << iter->a << " " << iter->b << std::endl;
    }
}

void task4(const Row* rows, int nrows) {

}

int main (int argc, char* argv[]) {
    std::cout << "******** task1 start ********" << std::endl;
    Row normalRows[] = {
        {2000, 10},
        {2000, 1},
        {1000, 30},
        {1000, 31},
        {1000, 80},
        {3000, 30},
        {5000, 30},
    };
    task1(normalRows, 6);
    std::cout << "******** task1 end ********" << std::endl << std::endl;

    std::cout << "******** task2 start ********" << std::endl;
    Row SortedRows[] = {
        {1000, 30},
        {1000, 31},
        {1000, 72},
        {2000, 12},
        {2000, 33},
        {3000, 30},
        {5000, 50},
    };
    task2(SortedRows, 6);
    std::cout << "******** task2  end********" << std::endl << std::endl;

    std::cout << "******** task3 start ********" << std::endl;
    task3(SortedRows, 6);
    std::cout << "******** task3 end ********" << std::endl << std::endl;

    std::cout << "******** task4 start ********" << std::endl;
    task4(SortedRows, 6);
    std::cout << "******** task4 end ********" << std::endl << std::endl;
    return 0;
}
