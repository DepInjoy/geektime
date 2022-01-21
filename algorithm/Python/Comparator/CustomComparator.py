from functools import cmp_to_key

'''
@brief:     1. 重载<运算符实现比较器
'''
class Student:
    def __init__(self, name, Id, age):
        self.Id = Id
        self.age = age
        self.name = name

    def __lt__(self, other):
        return self.Id < other.Id if self.Id != other.Id else \
            other.age < self.age if self.age != other.age else \
                self.name < other.name
'''
@brief:     2. 自定义比较函数,通过functools的cmp_to_key重写比较方法

@return:    返回正数,如果st1>st2
            返回负数,如果st1<st2
            返回0,如果st1=st2
'''
def student_comparator(st1, st2):
    return st1.Id - st2.Id if st1.Id != st2.Id else \
        st2.age - st1.age if st1.age != st2.age else \
            0 if st1.name == st2.name else -1 if st1.name < st2.name else 1

if __name__ == "__main__":
    sts = [Student("A", 4, 41), Student("B", 4, 41), Student("C", 3, 42),
            Student("C", 3, 41), Student("B", 3, 42), Student("A", 3, 41)]
    print("通过自定义<运算符实现比较器:")
    for st in sorted(sts):
        print("{} {} {}".format(st.Id, st.age, st.name))

    print("自定义比较方法实现比较器:")
    for st in sorted(sts, key = cmp_to_key(student_comparator)):
        print("{} {} {}".format(st.Id, st.age, st.name))

    print("lambda函数实现自定义比较方法")
    for st in sorted(sts, key = cmp_to_key(lambda st1, st2:
                                           st1.Id - st2.Id if st1.Id != st2.Id else
                                           st2.age - st1.age if st1.age != st2.age else
                                           0 if st1.name == st2.name else -1 if st1.name < st2.name else 1)):
        print("{} {} {}".format(st.Id, st.age, st.name))