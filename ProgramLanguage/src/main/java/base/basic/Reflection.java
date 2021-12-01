package base.basic;

import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

/**
 * Java基础系列:反射
 *      本文主要是了解使用Java反射的方式
 *      新建了一个Person类，以该Person类为例，实践获取Class对象的方式和一些接口,其中包括
 *          1. 获取Class对象的方式
 *          2. 通过Class对象创建对象实例的方式
 *          3. 获取Class对象的属性信息
 *          4. 获取Class对象的方法
 */
public class Reflection {
    public static void main(String[] args) {
        try {
            /** 1. 获取Class对象的方式 */
            // 方式一:通过Class.forName静态方法，输入类的全路径来获取Class对象
            Class cls1 = Class.forName("base.basic.Person");
            // 方式二:通过.class方法，该方法仅限于编译前已知类名
            Class cls2 = Person.class;
            // 方式三:通过类实例的getClass
            Person person = new Person("Lily", 18);
            Class cls3 = person.getClass();

            /** 2. 通过Class对象创建对象实例 */
            // 方式一: 通过Class对象的newInstance()方法, 只支持无参构造
            Person person1 = (Person)cls1.newInstance();
            // 方式二: 通过Constructor对象的newInstance()方法,该方式支持有参构造
            Constructor constructor = cls1.getConstructor(String.class, Integer.class);
            Person person2 = (Person)constructor.newInstance("Lucy", 30);

            /** 3. 获取Class对象的属性信息 */
            // 获取Class对象的public属性
            System.out.println("Class对象 getFields");
            for (Field field : cls1.getFields()) {
                System.out.println("\tField: " + field.getName());
            }
            // 获取除了继承属性以外的, 包括public, protected, default, private所有属性
            System.out.println("Class对象 getDeclaredFields");
            for (Field field : cls1.getDeclaredFields()) {
                System.out.println("\tDeclaredField: " + field.getName());
            }

            /** 4. 获取Class对象的方法 */
            // 获取类的public接口，包括当前类的成员函数和接口以及继承的
            System.out.println("\nClass对象 getMethods");
            for (Method method : cls1.getMethods()) {
                System.out.println("\tMethod: " + method.getName() + " default: " + method.getDefaultValue());
            }
            // 获取当前类的所有接口
            System.out.println("Class对象 getDeclaredMethods");
            for (Method method : cls1.getDeclaredMethods()) {
                System.out.println("\tMethod: " + method.getName() + " default: " + method.getDefaultValue());
            }
        } catch (ClassNotFoundException e) {
            System.out.println("OPS! Meet Exception:\n\t" + e.getMessage());
        } catch (InstantiationException e) {
            System.out.println("OPS! Meet Exception:\n\t" + e.getMessage());
        } catch (IllegalAccessException e) {
            System.out.println("OPS! Meet Exception:\n\t" + e.getMessage());
        } catch (NoSuchMethodException e) {
            System.out.println("OPS! Meet Exception:\n\t" + e.getMessage());
        } catch (Exception e) {
            System.out.println("OPS! Meet Exception:\n\t" + e.getMessage());
        }
    }
}
