/**
 *  146. LRU 缓存
 *  https://leetcode.cn/problems/lru-cache/description/ 
*/
#include <unordered_map>

struct DLinkList {
    int key, value;
    DLinkList*  prev{nullptr};
    DLinkList*  next{nullptr};
    DLinkList() : key(0), value(0) {}
    DLinkList(int _key, int _value) :
        key(_key), value(_value) {}
};

class LRUCache {
public:
    LRUCache(int capacity) : capacity_(capacity), size_(0) {
        head = new DLinkList();
        tail = new DLinkList();
        head->next = tail;
        tail->prev = head;
    }
    
    int get(int key) {
        if (!cache.count(key)) {
            return -1;
        }

        DLinkList* node = cache[key];
        moveToHead(node);
        return node->value;
    }
    
    void put(int key, int value) {
        if (!cache.count(key)) {
            DLinkList* newNode = new DLinkList(key, value);
            cache[key] = newNode;
            addToHead(newNode);
            ++size_;
            if (size_ > capacity_) {
                DLinkList* rmNode = removeTail();
                cache.erase(rmNode->key);
                size_--;
                delete rmNode;
            }
        } else {
            DLinkList* node = cache[key];
            node->value = value;
            moveToHead(node);
        }
    }

private:
    void addToHead(DLinkList* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

    void removeNode(DLinkList* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void moveToHead(DLinkList* node) {
        removeNode(node);
        addToHead(node);
    }

    DLinkList* removeTail() {
        DLinkList* node = tail->prev;
        removeNode(node);
        return node;
    }
private:
    std::unordered_map<int, DLinkList*> cache;
    DLinkList*  head;
    DLinkList*  tail;
    int size_;
    int capacity_;
};