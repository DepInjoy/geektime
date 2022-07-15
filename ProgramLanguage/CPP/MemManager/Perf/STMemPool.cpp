#include "MemAlloc.h"
#include "MemoryPool.h"
#include "ByteMemoryPool.h"

/********** 采用C++标准的全局 new/delete operator Start ******/
class Rational {
public:
    explicit Rational(int a = 0, int b = 1) : n(a), d(b) {}

private:
    int n;  // 分子
    int d;  // 分母
};
/********** 采用C++标准的全局 new/delete operator End ******/

/********** 单线程专用Rational2 new/delete operator Start ******/
class NextOnFreeList {
public:
    NextOnFreeList *next{nullptr};
};

class Rational2 {
public:
    explicit Rational2(int a = 0, int b = 1) : n(a), d(b) {}
    inline void *operator new(size_t size);
    inline void operator delete(void *doomed, size_t size);
    static void newMemPool() { expandTheFreeList(); }
    static void deleteMemPool();

private:
    static NextOnFreeList *freeList;  // Rational对象的空闲列表
    static void expandTheFreeList();
    enum { EXPANSION_SIZE = 32 };

    int n;  // 分子
    int d;  // 分母
};

NextOnFreeList *Rational2::freeList = nullptr;

inline void *Rational2::operator new(size_t size) {
    if (!freeList) {  // 如果列表为空，则将其填满
        expandTheFreeList();
    }
    NextOnFreeList *head = freeList;
    freeList = head->next;
    return head;
}

inline void Rational2::operator delete(void *doomed, size_t size) {
    NextOnFreeList *head = static_cast<NextOnFreeList *>(doomed);
    head->next = freeList;
    freeList = head;
}

void Rational2::expandTheFreeList() {
    //  我们必须分配足够大的对象以包含下一个指针
    size_t size = (sizeof(Rational) > sizeof(NextOnFreeList *)) ? sizeof(Rational) : sizeof(NextOnFreeList *);
    NextOnFreeList *runner = reinterpret_cast<NextOnFreeList*>(new char[size]);
    freeList = runner;
    for (int i = 0; i < EXPANSION_SIZE; i++) {
        runner->next = reinterpret_cast<NextOnFreeList *>(new char[size]);
        runner = runner->next;
    }
    runner->next = nullptr;
}

void Rational2::deleteMemPool() {
    NextOnFreeList *nextPtr;
    for (nextPtr = freeList; nextPtr != NULL; nextPtr = freeList) {
        freeList = freeList->next;
        delete [] reinterpret_cast<char*>(nextPtr);
    }
}
/********** 单线程专用Rational2 new/delete operator End ******/


/********** 固定大小的内存池 Start ********************/
class Rational3 {
 public:
    explicit Rational3(int a = 0, int b = 1) : n(a), d(b) {}
    void *operator new(size_t size) {
        return reinterpret_cast<void*>(memPool->alloc(size));
    }
    void operator delete(void *doomed, size_t size) {
        memPool->free(doomed);
    }
    static void newMemPool() { memPool = new MemoryPool<Rational3>; }
    static void deleteMemPool() { delete memPool; }

 private:
    int n;  // 分子
    int d;  // 分母
    static MemoryPool<Rational3>* memPool;
};
MemoryPool<Rational3>* Rational3::memPool = nullptr;
/********** 固定大小的内存池 End ********************/


/********** 可变大小的内存管理 Start *******************/
class Rational4 {
public:
    explicit Rational4(int a = 0, int b = 1) : n(a), d(b) {}
    void *operator new(size_t size) { return memPool->alloc(size); }
    void operator delete(void *doomed, size_t size) { memPool->free(doomed, size); }
    static void newMemPool() { memPool = new ByteMemoryPool; }
    static void deleteMemPool() { delete memPool; }

private:
    int n;  // 分子
    int d;  // 分母
    static ByteMemoryPool *memPool;
};
ByteMemoryPool* Rational4::memPool = nullptr;
/********** 可变大小的内存管理 End *******************/

int main() {
    oneRoundCost<Rational>("Stannd CPP Global new/delete operator");
    oneRoundCost<Rational2>("Stannd CPP Global new/delete operator");
    Rational3::newMemPool();
    oneRoundCost<Rational3>("Template MemPool new/delete operator");
    Rational3::deleteMemPool(); // 内存释放存在一些问题

    Rational4::newMemPool();
    oneRoundCost<Rational3>("Unfixed size MemPool new/delete operator");
    Rational4::deleteMemPool();
    return 0;
}
