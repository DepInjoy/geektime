#include <iostream>
template<typename Derived>
class Base {
public:
    void name() {
        (static_cast<Derived*>(this))->impl();
    }
};

class Derived1 : public Base<Derived1>{
public:
    void impl() {
        std::cout << "Derived1::impl" << std::endl;
    }
};

class Derived2 : public Base<Derived2>{
public:
    void impl() {
        std::cout << "Derived2::impl" << std::endl;
    }
};


int main(int argc, char* argv[]) {
    Derived1* d1 = new Derived1();
    Derived2* d2 = new Derived2();
    d1->name();
    d2->name();
    return 0;
}