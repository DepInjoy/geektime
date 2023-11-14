package basic.lambda;

// 函数式编程接口,JDK 8专门为函数式接口引入了@FunctionalInterface注解
interface Animal {
    void shout();
}

interface Calculate {
    int sum(int x, int y);
}

public class LambdaExample {
    public static void AnimalShout(Animal animal) {
        animal.shout();
    }

    public static void ShowSum(int x, int y, Calculate cal) {
        System.out.println(x + " + " + y + " = " + cal.sum(x, y));
    }

    public static void main(String[] args) {
        // 1. 通过匿名内部类作为参数传递给AnimalShout
        String name = "cat";
        AnimalShout(new Animal() {
            @Override
            public void shout() {
                System.out.println("匿名内部类： "+ name + " 喵喵...");
            }
        });

        // 2. Lambda表达式作为参数传递给AnimalShout
        AnimalShout(() -> {
            System.out.println("Lambda表达式： " + name + " 喵喵...");
        });

        // 2.2 Lambda表达式入参且有返回值
        ShowSum(3, 5, (x, y) -> x + y);
    }
}

