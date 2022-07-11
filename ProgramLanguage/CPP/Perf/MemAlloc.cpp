#include "TimeCost.h"

/********** 采用C++标准的全局 new/delete operator Start ******/
class Rational {
public:
    explicit Rational(int a = 0, int b = 1) : n(a), d(b) {}

private:
    int n;  // 分子
    int d;  // 分母
};
/********** 采用C++标准的全局 new/delete operator End ******/

class NextOnFreeList {
public:
    NextOnFreeList *next{nullptr};
};

/********** 单线程专用Rational2 new/delete operator Start ******/
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
template <class T>
class MemoryPool {
public:
    explicit MemoryPool(size_t size = EXPANSION_SIZE);
    ~MemoryPool();
    //  从空闲列表中分配T元素
    inline void* alloc(size_t size);
    //  返回T元素到空闲列表中
    inline void free(void *someElement);

private:
    // 空闲列表的下一元素
    MemoryPool<T>*  next{ nullptr };
    // 如果空闲列表为空，按该大小扩展它
    enum { EXPANSION_SIZE = 32};
    // 添加空闲元素至空闲列表
    void expandTheFreeList(int howMany = EXPANSION_SIZE);
};

template <class T>
MemoryPool<T>::MemoryPool(size_t size) {
    expandTheFreeList(size);
}

template <class T>
MemoryPool<T>::~MemoryPool() {
    MemoryPool<T> *nextPtr = next;
    for (nextPtr = next; nextPtr; nextPtr = next) {
        next = next->next;
        delete [] reinterpret_cast<char*>(nextPtr);
    }
}

template <class T>
inline void* MemoryPool<T>::alloc(size_t) {
    if (!next) {
        expandTheFreeList();
    }
    static int cnt = 0;
    MemoryPool<T>* head = next;
    next = head->next;
    return head;
}

template<class T>
inline void MemoryPool<T>::free(void* doomed) {
    MemoryPool<T> *head = reinterpret_cast<MemoryPool<T>*>(doomed);
    head->next = next;
    next = head;
}

template <class T>
void MemoryPool<T>::expandTheFreeList(int howMany) {
    // 必须分配足够大的对象以包含下一个指针
    size_t size = (sizeof(T) > sizeof(MemoryPool<T>*)) ? sizeof(T) : sizeof(MemoryPool<T>*);
    MemoryPool<T>* runner = reinterpret_cast<MemoryPool<T>*>(new char[size]);
    next = runner;
    for (int i = 0; i < howMany; ++i) {
        runner->next = reinterpret_cast<MemoryPool<T>*>(new char[size]);
        runner = runner->next;
    }
    runner->next = nullptr;
}

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
class MemoryChunk {
public:
    MemoryChunk(MemoryChunk *nextChunk, size_t chunkSize);
    ~MemoryChunk();
    inline void *alloc(size_t size);
    inline void free(void* someElement, const size_t size);
    // 指向列表下一内存块的指针
    MemoryChunk *nextMemChunk() { return next; }
    // 当前内存块剩余空间大小
    size_t spaceAvailable() { return chunkSize - bytesAlreadyAllocated; }
    // 这是一个内存块的默认大小
    enum { DEFAULT_CHUNK_SIZE = 4096 };

private:
    MemoryChunk *next;
    void *mem;                      // 一个内存块的默认大小
    size_t chunkSize;               // 当前内存块中已分配的字节数
    size_t bytesAlreadyAllocated;
};

MemoryChunk::MemoryChunk(MemoryChunk *nextChunk, size_t reqSize) {
    chunkSize = (reqSize > DEFAULT_CHUNK_SIZE) ? reqSize : DEFAULT_CHUNK_SIZE;
    next = nextChunk;
    bytesAlreadyAllocated = 0;
    mem = reinterpret_cast<void*>(new char[chunkSize]);
}

MemoryChunk::~MemoryChunk() { delete [] reinterpret_cast<char*>(mem); }

void* MemoryChunk::alloc(size_t requestSize) {
    void *addr = reinterpret_cast<void*>(reinterpret_cast<char*>(mem) + bytesAlreadyAllocated);
    bytesAlreadyAllocated += requestSize;
    return addr;
}

inline void MemoryChunk::free(void *doomed, const size_t size) {
    bytesAlreadyAllocated -= size;
}

class ByteMemoryPool {
public:
    explicit ByteMemoryPool(size_t initSize = MemoryChunk::DEFAULT_CHUNK_SIZE);
    ~ByteMemoryPool();
    // 从私有内存池分配内存
    inline void *alloc(size_t size);
    // 释放先前从内存池中分配的内存
    inline void free(void* someElement, const size_t size);

private:
    // 内存块列表。它是我们的私有存储空间
    MemoryChunk *listOfMemoryChunks{nullptr};
    // 向我们的私有存储空间添加一个内存块
    void expandStorage(size_t reqSize);
};

// 创建ByteMemoryPool对象。生成私有存储空间
ByteMemoryPool::ByteMemoryPool(size_t initSize) {
    expandStorage(initSize);
}

ByteMemoryPool::~ByteMemoryPool() {
    MemoryChunk *memChunk = listOfMemoryChunks;
    while (memChunk) {
        listOfMemoryChunks = memChunk->nextMemChunk();
        delete [] reinterpret_cast<char*>(memChunk);
        memChunk = listOfMemoryChunks;
    }
}

void ByteMemoryPool::expandStorage(size_t reqSize) {
    listOfMemoryChunks = new MemoryChunk(listOfMemoryChunks, reqSize);
}

inline void* ByteMemoryPool::alloc(size_t requestSize) {
    size_t space = listOfMemoryChunks->spaceAvailable();
    if (space <requestSize) {
        expandStorage(requestSize);
    }
    return listOfMemoryChunks->alloc(requestSize);
}

inline void ByteMemoryPool::free(void *doomed, const size_t size) {
    listOfMemoryChunks->free(doomed, size);
}

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
template<class T>
void oneRoundCost(std::string uniqueFlag) {
    T* array[1000];
    TimeCost cost(uniqueFlag);
    for (int i = 0; i < 5000; ++i) {
        for (int j = 0; j < 1000; ++j) {
            array[j] = new T(i);
        }
        for (int j = 0; j < 1000; ++j) {
            delete array[j];
        }
    }
}

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
