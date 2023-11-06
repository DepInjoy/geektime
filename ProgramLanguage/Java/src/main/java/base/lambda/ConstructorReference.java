package base.lambda;

class Person {
    private String  name_;
    private Integer age_;

    Person(String name, Integer age) {
        name_ = name;
        age_ = age;
    }

    String getName() {
        return name_;
    }

    Integer getAge() {
        return age_;
    }
}

// 函数式接口
@FunctionalInterface
interface PersonBuilder {
    Person build(String name, Integer age);
}

public class ConstructorReference {
    public static void printPersonInfo(String name, Integer age, PersonBuilder builder) {
        Person person = builder.build(name, age);
        System.out.println("Person with name: " + person.getName() +
                "age: " + person.getAge());
    }

    public static void main(String[] args) {
        // 采用构造器引用方式
        printPersonInfo("Lucy", 18, Person::new);
        // 采用Lambda表达式方式
        printPersonInfo("Jack", 25,
                (name, age) -> new Person(name, age));
    }
}