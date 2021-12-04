package base.basic;

public class Person {
    public Person() {}
    public Person(String name, Integer age) {
        name_ = name;
        age_ = age;
    }
    public String getName() { return name_; }
    public void setName(String name) { name_ = name; }
    public Integer getAge() { return age_; }
    public void setAge(Integer age) { age_ = age; }
    private void setSecret(String secret) { secret_ = secret; }

    private     String          name_ = "UNKNOWN";
    private     Integer         age_= 0;
    protected   String          secret_ = "";
    public      String          title_ = "";
}
