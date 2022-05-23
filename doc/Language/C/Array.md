## 弹性数组
> 定义：定义数组时，没有指明其长度，此为弹性数组。
> 
> 使用条件，弹性数组只能存在于结构体中，并且必须满足如下条件：
> - 弹性数组必须为结构体的最后一个成员；
> - 该结构体必须包含一个非弹性数组的成员；
> - 编译器需要支持 C99 标准。
> 
> 主要用于存储不定长数据。

使用示例：
```c
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <stdint.h>

typedef struct StudentInfo {
    uint32_t    id;
    uint32_t    grade;
    char        name[];
} StudentInfo;

int main(int argc, char* argv[]) {
    // 输出为8, 弹性数组在结构体中不占用空间
    printf("sizeof(StudentInfo) = %d\n", sizeof(StudentInfo));

    char* name = "Lucy";
    StudentInfo* student = (StudentInfo*)malloc(sizeof(StudentInfo)+strlen(name)+1);
    if (student == NULL) {
        printf("malloc failed\n");
        return -1;
    }
    student->id = 123;
    student->grade = 6;
    memcpy((void *)student->name, name, strlen(name));
    printf("info:\n");
    printf("\tstudent->name: %s\n", student->name);
    printf("\naddress:\n");
    printf("\tstudent: 0x%p\n", student);
    printf("\tstudent->grade: 0x%p\n", &student->grade);
    printf("\tstudent->name: 0x%p\n", &student->name);
    free(student);
    return 0;
}
```