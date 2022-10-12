#include <new>
#include <iostream>
/**
 * @brief   条款8:了解各种不同意义的new和delete
 * 
 *          new operator在堆上产生对象，它进行两个动作：
 *              1. 分配足够的内存，用来放置某类型的对象。
 *              2. 调用constructor，为刚分配的内存的对象设定初值
 *          operetor new只是分配内存，不调用构造函数
 * 
 *          delete operator调用对象析构函数，还可以释放被该对象占用内存
 *          operator delete只是释放内存
 */
class Complex {
public:
    explicit Complex(double re = 0, double im = 0) : re_(re), im_(im) {
        std::cout << "Constructor : (" << re_ << ", " << im_ << ")" << std::endl;
    }

    ~Complex() {
        std::cout << "Destructor : (" << re_ << ", " << im_ << ")" << std::endl;
    }
  
    void print() {
        std::cout << "(" << re_ <<", " << im_  << ")" << std::endl;
    }
private:
    double re_;
    double im_;
};


int main(int argc, char*argv[]) {
    char buf[2 * sizeof(int)];
    // placement new
    int* i0 = new (buf)int(6);
    int* i1 = new(buf + sizeof(int)) int(18);

    std::cout << "The Address of buf:" << std::endl;
    std::cout << reinterpret_cast<int*>(buf)<< "\t" << reinterpret_cast<int*>(buf + sizeof(int)) << std::endl;
    std::cout << "The Address of i:" << std::endl;
    std::cout << i0 << "\t" << i1 << std::endl;
    std::cout << "The Value of i:" << std::endl;
    std::cout << *i0 << "\t" << *i1 << std::endl;

    // Cbuf在栈上,通过placement new分配内存构造的c1的内存不需要释放
    std::cout << "\n\n\noperator new just allocate memory" << std::endl;
    char Cbuf[32];
    Complex* c1 = new(Cbuf)Complex(6, 8);
    c1->print();
    c1->~Complex();  // 调用析构函数

    std::cout << "\nnew operator create an object in heap, alloc mem and init" << std::endl;
    Complex* c2 = new Complex(18, 16);
    c2->print();
    delete c2;

    std::cout << "\noperator delete just release resource" << std::endl;
    Complex* c3 = new Complex(28, 26);
    c3->print();
    operator delete(c3);

    return 0;
}