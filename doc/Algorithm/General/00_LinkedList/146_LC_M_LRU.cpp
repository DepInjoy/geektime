#include <unordered_map>

struct DoubleLinkedNode {
    int key;
    int value;
    DoubleLinkedNode* pre;
    DoubleLinkedNode* next;
    DoubleLinkedNode() : key(0), value(0), pre(nullptr), next(nullptr) {}
    DoubleLinkedNode(int _key, int _value)
        : key(_key), value(_value), pre(nullptr), next(nullptr) {}
};

class LRUCache {
public:
    LRUCache(int capacity) : capacity_(capacity),
        head_(new DoubleLinkedNode()), tail_(new DoubleLinkedNode()) {
        head_->next = tail_;
        tail_->pre = head_;
    }
    
    int get(int key) {
        auto iter = cache_.find(key);
        if (iter == cache_.end()) {
            return -1;
        }
        
        DoubleLinkedNode* node = iter->second;
        moveToHead(node);
        return node->value;
    }
    
    void put(int key, int value) {
        if (cache_.count(key)) {
            DoubleLinkedNode* node = cache_[key];
            node->value = value;
            moveToHead(node);
        } else {
            DoubleLinkedNode* node = new DoubleLinkedNode(key, value);
            addToHead(node);
            cache_[key] = node;
            if (cache_.size() > capacity_) {
                DoubleLinkedNode* node = tail_->pre;
                removeNode(node);
                cache_.erase(node->key);
                delete node;
            }
        }
    }
private:
    void moveToHead(DoubleLinkedNode* node) {
        removeNode(node);
        addToHead(node);
    }

    void addToHead(DoubleLinkedNode* node) {
        node->next = head_->next;
        node->pre = head_;
        head_->next->pre = node;
        head_->next = node;
    }
    
    void removeNode(DoubleLinkedNode *node) {
        node->pre->next = node->next;
        node->next->pre = node->pre;
    }
private:
    int capacity_;
    std::unordered_map<int, DoubleLinkedNode*> cache_;
    DoubleLinkedNode* head_;
    DoubleLinkedNode* tail_;
};