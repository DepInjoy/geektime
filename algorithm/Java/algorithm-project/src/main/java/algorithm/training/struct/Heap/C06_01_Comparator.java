package algorithm.training.struct.Heap;

import java.util.Arrays;
import java.util.Calendar;
import java.util.Comparator;

/**
 * @brief:      比较器，实质就是重载比较运算符
 *                  1. 比较器可以很好的应用在特殊标准的排序上
 *                  2. 比较器可以很好的应用在根据特殊标准排序的结构上
 *                  3. 写代码变得异常容易，还用于范型编程
 *
 *              比较器的统一约定：
 *                  @Override
 *                  public int compare(T o1, T o2)
 *                      返回负数的情况，就是o1比o2优先的情况
 *                      返回正数的情况，就是o2比o1优先的情况
 *                      返回0的情况，就是o1与o2同样优先的情况
 * */
public class C06_01_Comparator {
    static class Student {
        public Integer id;
        public Integer age;
        public String name;

        public Student(String name, Integer id, Integer age) {
            this.id = id;
            this.age = age;
            this.name = name;
        }
    };

    static class IdAscendingOrder implements Comparator<Student> {
        @Override
        public int compare(Student s1, Student s2) {
            return s1.id - s2.id;
        }
    }

    static class IdAndAgeAscendingOrder implements Comparator<Student> {
        @Override
        public int compare(Student s1, Student s2) {
            return (s1.id - s2.id != 0) ? s1.id - s2.id : s1.age - s2.age;
        }
    }

    public static void main(String[] args) {
        Student student1 = new Student("A", 4, 40);
        Student student2 = new Student("B", 4, 21);
        Student student3 = new Student("C", 3, 12);
        Student student4 = new Student("D", 3, 62);
        Student student5 = new Student("E", 3, 42);

        System.out.println("按照ID排序:");
        Student[] students1 = new Student[]{student1, student2, student3, student4, student5 };
        Arrays.sort(students1, new IdAscendingOrder());
        for (int i = 0; i < students1.length; i++) {
            Student s = students1[i];
            System.out.println(s.name + "," + s.id + "," + s.age);
        }

        System.out.println("先按照ID排序，如果ID相同则按照Age排序:");
        Student[] students2 = new Student[]{student1, student2, student3, student4, student5 };
        Arrays.sort(students2, new IdAndAgeAscendingOrder());
        for (int i = 0; i < students2.length; i++) {
            Student s = students2[i];
            System.out.println(s.name + "," + s.id + "," + s.age);
        }
    }

}
