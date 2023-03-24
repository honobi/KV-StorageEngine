// 本项目参考：Pugh, W. (1990). Skip Lists: A Probabilistic Alternative to Balanced Trees. Communications of the ACM, 33(6), 668-676. doi: 10.1145/78973.78977
// pdf：https://epaperpress.com/sortsearch/download/skiplist.pdf

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>
#include <mutex>
#include <cstring>
#include <fstream>

template<typename K, typename V>
class SkipListNode {
 public:
    SkipListNode() {}
    SkipListNode(int level);
    SkipListNode(K key, V val, int level);
    ~SkipListNode();
    K key() const;
    V val() const;
    void set_val(V val);

    int level_;
    SkipListNode<K, V>** next_;  // 保存该节点在不同level中的下一个节点，例如，next[i]代表该节点在i级的下一个节点。论文中这个数组名为forward
 private:
    K key_;
    V val_;
};

template<typename K, typename V>
class SkipList {
 public:
    SkipList(int max_level);  // 跳表的最大高度一般设置为log2(n)
    ~SkipList();
    int get_random_level();
    int insert(K, V);
    SkipListNode<K, V>* search(K);
    void erase(K);
    void print();
    int size();
    void read_file(const char* file_name = "data");
    void write_file(const char* file_name = "data"); 

    std::mutex mutex_;
 private:
    int size_;
    int max_level_;
    int skip_list_level_;
    SkipListNode<K, V>* header_;  // 虚拟头结点
};


template<typename K, typename V>
SkipListNode<K, V>::SkipListNode(int level) {
    level_ = level;
    next_ = new SkipListNode<K, V>*[level + 1];  // level是从0级开始
    memset(next_, 0, sizeof(SkipListNode<K, V>*) * (level + 1));
}

template<typename K, typename V>
SkipListNode<K, V>::~SkipListNode() {
    delete[] next_;  // delete一个元素为指针的动态数组，只会把这块动态数组delete掉，并不会delete数组元素指向的内存
}

template<typename K, typename V>
SkipListNode<K, V>::SkipListNode(K key, V val, int level) {
    key_ = key;
    val_ = val;
    level_ = level;
    next_ = new SkipListNode<K, V>*[level + 1];  
    memset(next_, 0, sizeof(SkipListNode<K, V>*) * (level + 1));
}

template<typename K, typename V>
K SkipListNode<K, V>::key() const {
    return key_;
}

template<typename K, typename V>
V SkipListNode<K, V>::val() const {
    return val_;
}

template<typename K, typename V>
void SkipListNode<K, V>::set_val(V val) {
    val_ = val;
}

template<typename K, typename V> 
SkipList<K, V>::SkipList(int max_level) {
    max_level_ = max_level;
    skip_list_level_ = 0;
    size_ = 0;
    header_ = new SkipListNode<K, V>(max_level_);   // 每一级都要有头结点
};

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
   SkipListNode<K, V>* cur = header_;
   while(cur) {
      SkipListNode<K, V>* tmp = cur->next_[0];
      delete cur;
      cur = tmp;
   }
}

// 成功返回0，失败返回-1（key已存在）
template<typename K, typename V>
int SkipList<K, V>::insert(K key, V val) {
    mutex_.lock();
    SkipListNode<K, V>* cur = header_;
    SkipListNode<K, V>* last_less[max_level_ + 1];  // 保存每一级中最后一个小于key的节点。这个数组是论文中的update数组
    memset(last_less, 0, sizeof(SkipListNode<K, V>*) * (max_level_ + 1));
    // 从最高级往下查找，就可以做到O(logn)的时间复杂度
    for(int i = skip_list_level_; i >= 0; --i) {
        while(cur->next_[i] && cur->next_[i]->key() < key) {
            cur = cur->next_[i];
        }
        last_less[i] = cur;
    }  // 此时cur是level 0 中最后小于实参key的节点
    cur = cur->next_[0];

    // 如果key已存在
    if(cur && cur->key() == key) {
        mutex_.unlock();
        return -1;
    }
    // 如果插入位置为链表尾，或者第一个不小于key的元素与key不相等，那么key可以插入
    if(cur == NULL || cur->key() != key){
        int random_level = get_random_level();
        // 如果该节点的随机level大于跳表当前level，则么将高于当前level的那些级别的last_less指向header_
        if(random_level > skip_list_level_) {
            for(int i = skip_list_level_ + 1; i <= random_level; ++i){
                last_less[i] = header_;
            }
            skip_list_level_ = random_level;
        }
        SkipListNode<K, V>* new_node = new SkipListNode<K, V>(key, val, random_level);
        // 在[0, random + 1]级插入节点
        for(int i = 0; i <= random_level; ++i){
            new_node->next_[i] = last_less[i]->next_[i];
            last_less[i]->next_[i] = new_node;
        }
        ++size_;
    }
    mutex_.unlock();
    return 0;
}

template<typename K, typename V>
void SkipList<K, V>::erase(K key) {
    mutex_.lock();
    SkipListNode<K, V>* cur = header_;
    SkipListNode<K, V>* last_less[max_level_ + 1];
    memset(last_less, 0, sizeof(SkipListNode<K, V>*) * (max_level_ + 1));
    // 获取last_less数组
    for(int i = skip_list_level_; i >= 0; --i) {
        while(cur->next_[i] && cur->next_[i]->key() < key){
            cur = cur->next_[i];
        }
        last_less[i] = cur;
    }
    cur = cur->next_[0];
    // 如果找到该节点
    if(cur && cur->key() == key){
        // 将该节点从每一级中删除
        for(int i = 0; i <= cur->level_; ++i){
            last_less[i]->next_[i] = cur->next_[i];
        }
        // 删除没有元素的level
        while(skip_list_level_ > 0 && header_->next_[skip_list_level_] == NULL){
            skip_list_level_--;
        }
        --size_;
        delete cur;
    }
    mutex_.unlock();
    return;
}

template<typename K, typename V> 
SkipListNode<K, V>* SkipList<K, V>::search(K key) {
    SkipListNode<K, V>* cur = header_;
    for(int i = skip_list_level_; i >= 0; --i){
        while(cur->next_[i] && cur->next_[i]->key() < key){
            cur = cur->next_[i];
        }
    }
    cur = cur->next_[0];
    if(cur && cur->key() == key)
        return cur;
    return NULL;
}

template<typename K, typename V>
int SkipList<K, V>::get_random_level() {
    int level = 0;  // 一个节点在level 0必定存在，所以从level 1开始随机
    while(level < max_level_ && random() % 2 == 1){
        ++level;
    }
    return level;
}

template<typename K, typename V>
void SkipList<K, V>::print() {
    for(int i = 0; i <= skip_list_level_; ++i) {
        std::cout << "level " << i << ": ";
        SkipListNode<K, V>* cur = header_->next_[i];
        while(cur){
            std::cout << cur->key() << ":" << cur->val() << " ";
            cur = cur->next_[i];
        }
        std::cout << std::endl;
    }
}

template<typename K, typename V> 
int SkipList<K, V>::size() { 
    return size_;
}

template<typename K, typename V>
void SkipList<K, V>::read_file(const char* file_name) {
    std::ifstream in(file_name);
    if(in.is_open()) {
        K key;
        V val;
        while(in >> key >> val){
            insert(key, val);
        }
    }
    in.close();
}

template<typename K, typename V>
void SkipList<K, V>::write_file(const char* file_name) {
    std::ofstream out(file_name);
    if(out.is_open()) {
        SkipListNode<K, V>* cur = header_;
        while(cur){
            out << cur->key() << cur->val();
            cur = cur->next_[0];
        }
    }
   out.close();
}

#endif