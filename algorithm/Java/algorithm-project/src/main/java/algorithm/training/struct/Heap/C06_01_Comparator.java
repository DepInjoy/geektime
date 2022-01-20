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

 * */
public class C06_01_Comparator {
    /**
     * @brief:      通过继承Comparable重写compareTo接口实现比较器
     * */
    static class ComparableStudent implements Comparable<ComparableStudent>{
        public Integer id;
        public Integer age;
        public String name;

        public ComparableStudent(String name, Integer id, Integer age) {
            this.id = id;
            this.age = age;
            this.name = name;
        }

        @Override
        public int compareTo(ComparableStudent st) {
            return (id != st.id) ? id - st.id :
                    (age != st.age) ? st.age - age :
                            name.compareTo(st.name);
        }
    };

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

    /**
     * @brief:      通过Comparator函数式接口实现比较器
     * @return:     返回负数, o1 < o2, o1优先
     *              返回正数, o2 < o1, o2优先
     *              返回0, o1与o2相等, 同等优先
     * */
    static class ComparableStudentOrder implements Comparator<Student> {
        @Override
        public int compare(Student s1, Student s2) {
            return (s1.id != s2.id) ? s1.id - s2.id :
                    (s1.age != s2.age) ? s2.age - s1.age :
                            s1.name.compareTo(s2.name);
        }
    }

    public static void main(String[] args) {
        ComparableStudent cst0 = new ComparableStudent("A", 4, 41);
        ComparableStudent cst1 = new ComparableStudent("B", 4, 41);
        ComparableStudent cst2 = new ComparableStudent("C", 3, 42);
        ComparableStudent cst3 = new ComparableStudent("C", 3, 41);
        ComparableStudent cst4 = new ComparableStudent("B", 3, 42);
        ComparableStudent cst5 = new ComparableStudent("A", 3, 41);

        ComparableStudent[] csts1 = new ComparableStudent[]{cst0, cst1, cst2, cst3, cst4, cst5};
        Arrays.sort(csts1);
        System.out.println("Comparable Student 先ID升序Age降序Name升序:");
        for (int i = 0; i < csts1.length; i++) {
            ComparableStudent cst = csts1[i];
            System.out.println(cst.id + " " + cst.age + " " + cst.name);
        }

        Student student1 = new Student("A", 4, 41);
        Student student2 = new Student("B", 4, 41);
        Student student3 = new Student("C", 3, 42);
        Student student4 = new Student("C", 3, 41);
        Student student5 = new Student("B", 3, 42);
        Student student6 = new Student("A", 3, 41);

        System.out.println("Comparator Student 先ID升序Age降序Name升序:");
        Student[] sts = new Student[]{student1, student2, student3, student4, student5, student6};
        Arrays.sort(sts, new ComparableStudentOrder());
        for (int i = 0; i < sts.length; i++) {
            Student st = sts[i];
            System.out.println(st.id + " " + st.age + " " + st.name);
        }
    }
}
