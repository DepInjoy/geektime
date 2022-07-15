/**
 * @brief   实现单线程的申请可变大小的内存池
 */
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
    // 内存块的默认大小
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
    // 内存块列表, 私有存储空间
    MemoryChunk *listOfMemoryChunks{nullptr};
    // 向私有存储空间添加一个内存块
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
