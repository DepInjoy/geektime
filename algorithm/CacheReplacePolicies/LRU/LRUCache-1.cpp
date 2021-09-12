class LRUCache {
public:
    LRUCache(int capacity) : capacity_(capacity) {
    }
    
    int get(int key) {
        auto iter = hash_map_.find(key);
        // 如果已存在，由于此次访问了数据，需要调整数据的位置
        if (iter != hash_map_.end()) {
            // 根据双向链表中的位置进行删除，降低操作复杂度
            key_list_.erase(iter->second.second);
            key_list_.push_front(key);
            hash_map_[key] = std::make_pair(iter->second.first, key_list_.begin());
            return iter->second.first;
        }
        return -1;
    }

    void put(int key, int value) {
        // 如果数据存在，则更新value
        if (get(key) != -1) {
            hash_map_.find(key)->second.first = value;
            return;
        }

        // 数据量超过capacity，则删除数据
        while(key_list_.size() >= capacity_) {
            int last_key = key_list_.back();
            key_list_.pop_back();
            hash_map_.erase(last_key);
        }

        // key不存在且没超过数据容量则直接插入数据
        insert(key, value);
    }

private:
    void insert(int key, int value) {
        key_list_.push_front(key);
        hash_map_.insert(std::make_pair(key,
                std::make_pair(value, key_list_.begin())));
    }

    int                                         capacity_;
    std::list<int>                              key_list_;
    // <key, <key, double-list location> >
    std::unordered_map< int, std::pair<int,
            std::list<int>::iterator > >        hash_map_;
};
