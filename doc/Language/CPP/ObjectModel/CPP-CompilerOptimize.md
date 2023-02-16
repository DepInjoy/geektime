# NRV优化

也就是Named Return Value,

```C++
#include <iostream>
 
class MyClass {
public:
    MyClass() {
        m_i = 0;
        std::cout << "this is default constructor!" << endl;
    }
	
    MyClass(const MyClass& that) {
		std::cout << "this is copy constructor!" << endl;
		this->m_i = that.m_i;
	}
    
    MyClass& operator=(const MyClass& that) {
		std::cout << "this is assignment constructor!" << endl;
		if (this != &that) {
			return *this;
		} else {
			this->m_i = that.m_i;
			return *this;
		}
}
private:
	int m_i;
};
 
MyClass::
 

 
MyClass gfunc() {
	MyClass obj;
	return obj;
}
 
MyClass func() {
	return MyClass();
}
 
int main() {
	MyClass myObj;
	std::cout << "----------------------- 1 -----------------" << std::endl;
 
	myObj = gfunc();
	std::cout << "----------------------- 1 -----------------" << std::endl;
 
	MyClass myObj2 = gfunc();
	std::cout << "----------------------- 1 -----------------" << std::endl;
 
	MyClass myObj3 = func();   //RVO优化
    std::cout << "----------------------- 1 -----------------" << std::endl;
 
    myObj3 = func();           //RVO优化 
	return 0;
}
```



# ROV优化

