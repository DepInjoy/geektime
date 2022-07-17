#include <iostream>

/**
 * @brief   CRTP的一个简单应用是记录某个类的对象构造的总个数
 */
template <typename CountedType>
class ObjectCount {
public:
    uint64_t live() { return count; }

protected:
    ObjectCount() { ++count; }
    virtual ~ObjectCount() {--count; }
    ObjectCount(const ObjectCount& obj) { ++count; }
    ObjectCount operator = (const ObjectCount& obj) {
        ObjectCount res;
        ++count;
        return res;
    }
private:
    static uint64_t     count;
};

template<typename T>
class MyCRTP : public ObjectCount<MyCRTP<T> > { };
template <typename CountedType>
uint64_t ObjectCount<CountedType>::count = 0;

int main(int argc, char* argv[]) {
    MyCRTP<char> *m1 = new MyCRTP<char>();
    std::cout << "构造MyCRTP<char>的个数: "<< m1->live() << std::endl;
    delete m1;
    std::cout << "构造MyCRTP<char>的个数: " << m1->live() << std::endl;

    MyCRTP<int> m2, m3;
    MyCRTP<int> m4 = m2;
    std::cout << "构造MyCRTP<int>的个数: "<< m2.live() << std::endl;
    return 0;
}