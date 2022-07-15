#include <atomic>
#include "MemAlloc.h"
#include "MemoryPool.h"
#include "MTMemPool.h"

class MutexLock : public ABCLock {
public:
    MutexLock() { pthread_mutex_init(&lock_, NULL); }
    ~MutexLock() { pthread_mutex_destroy(&lock_); }
    inline void lock() { pthread_mutex_lock(&lock_); }
    inline void unlock() { pthread_mutex_unlock(&lock_); }

private:
    pthread_mutex_t lock_;
};

class PrimitiveLock : public ABCLock {
public:
    ~PrimitiveLock() {}
    inline void lock() {  // 旋转锁
        int expected = LOCK_FREE;
        while (!lock_.compare_exchange_weak(expected, LOCK_BUSY)) {}
    }
    inline void unlock() {
        lock_.store(LOCK_FREE);
    }

private:
    std::atomic_int lock_{LOCK_FREE};
    enum {
        LOCK_FREE = 0,
        LOCK_BUSY = 1
    };
};

class MTRational {
public:
    explicit MTRational(int a = 0, int b = 1) : n(a), d(b) {}
    void *operator new(size_t size) { return memPool->alloc(size); }
    void operator delete(void *doomed, size_t size) { memPool->free(doomed); }
    static void newMemPool() { memPool = new MTMemoryPool<MemoryPool<MTRational>, MutexLock>(); }
    static void deleteMemPool() { delete memPool; }

private:
    int n;  // 分子
    int d;  // 分母
    static MTMemoryPool<MemoryPool<MTRational>, MutexLock> *memPool;
};
MTMemoryPool<MemoryPool<MTRational>, MutexLock>* MTRational::memPool = nullptr;

class MTRational2 {
public:
    explicit MTRational2(int a = 0, int b = 1) : n(a), d(b) {}
    void *operator new(size_t size) { return memPool->alloc(size); }
    void operator delete(void *doomed, size_t size) { memPool->free(doomed); }
    static void newMemPool() { memPool = new MTMemoryPool<MemoryPool<MTRational2>, PrimitiveLock>(); }
    static void deleteMemPool() { delete memPool; }

private:
    int n;  // 分子
    int d;  // 分母
    static MTMemoryPool<MemoryPool<MTRational2>, PrimitiveLock> *memPool;
};
MTMemoryPool<MemoryPool<MTRational2>, PrimitiveLock>* MTRational2::memPool = nullptr;

int main(int argc, char* argv[]) {
    MTRational::newMemPool();
    oneRoundCost<MTRational>("Multithread Memory Pool");
    MTRational::deleteMemPool();

    MTRational2::newMemPool();
    oneRoundCost<MTRational2>("Multithread Memory Pool V2");
    MTRational2::deleteMemPool();
    return 0;
}
