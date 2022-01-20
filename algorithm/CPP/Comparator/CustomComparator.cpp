#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

/**
 * @brief       自定义结构体student
 *              用比较器实现先按照id升序排序,再按照age降序排序,最后按照name降序排序
 *              通过sort进行排序, 打印出排序后的结果验证
 */
struct student {
    int             id;
    int             age;
    std::string     name;

    student(const int id, const int age, const std::string& name) {
        this->id = id;
        this->age = age;
        this->name = name;
    }

    /**
     * @brief               通过自定义<运算符来实现
     */
    bool operator < (const student& s) {
        return id != s.id ? id < s.id : (age != s.age ? s.age < age : name < s.name);
    }
};

/**
 * @brief       通过自定义一个外部比较函数来实现<操作 
 */
bool StudentComparator(const student& lst, const student& rst) {
    return lst.id != rst.id ? lst.id < rst.id :  // id降序
        (lst.age != rst.age ? rst.age < lst.age :  // age升序
            lst.name < rst.name);  // name降序
}

/**
 * @brief       用函数对象结构体作为比较器
 */
struct CustomComparator {
    bool operator()(const student& lst, const student& rst) {
        return lst.id != rst.id ? lst.id < rst.id :
            (lst.age != rst.age ? rst.age < lst.age :
            lst.name < rst.name);
    }
};


void showInfo(const std::vector<student>& students) {
    for (auto iter = students.begin(); iter != students.end(); ++iter) {
        std::cout << iter->id << " " << iter->age << " " << iter->name << std::endl;
    }
}

int main(int argc, char*argv[]) {
    std::vector<student> students {
        student(21, 18, "C"), student(10, 20, "A"), student(1, 16, "B"),
        student(18, 21, "BK"), student(18, 21, "MK"), student(18, 22, "AK")
    };

    // std::sort(students.begin(), students.end());
    // std::sort(students.begin(), students.end(), StudentComparator);
    std::sort(students.begin(), students.end(), CustomComparator());
    showInfo(students);

    return 0;
}

