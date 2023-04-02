#include <thread>

template<typename Iterator,typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T& result) {
        result = std::accumulate(first,last,result);
    }
};

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first,last);
    // 传入区间为空，直接返回初始值(init)
    if(!length) {
        return init;
    }

    unsigned long const min_per_thread = 25;
    // 元素总量除以每个线程处理元素的最低限量, 得到线程最大数量,防止发起过多线程
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    // 硬件线程数量，表示程序在歌词运行中可真正并发的线程数量
    unsigned long const hardware_threads= std::thread::hardware_concurrency();
    // 需要运行的线程数量
    unsigned long const num_threads=
        std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    // 将目标区间长度除以线程数量, 得出各线程需分担的元素数量
    unsigned long const block_size = length / num_threads;

    // 线程参数确定完成，开始准备装载线程
    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);
    Iterator block_start = first;
    for(unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        threads[i]=std::thread(
            accumulate_block<Iterator, T>(),
            block_start, block_end, std::ref(results[i]));
        block_start = block_end;
    }
    // 处理最后的一小块(由于计算block_size=length/num_threads可能不能整除, 导致有部分数据未处理)
    accumulate_block<Iterator,T>()(block_start, last, results[num_threads - 1]);

    // 等待所有的线程    
    for(auto& entry: threads) {
        entry.join();
    }

    // 累加中间结果
    return std::accumulate(results.begin(), results.end(), init);
}