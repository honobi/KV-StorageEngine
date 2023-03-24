#include "bloomfilter.h"
#include "skiplist.h"

template<typename K, typename V>
class StorageEngine {
 public:
    StorageEngine(int bloomfilter_expect_data_count, double bf_false_positive_rate, int skiplist_max_level);
    ~StorageEngine();
    int insert(K key, V val);
    void erase(K);
    SkipListNode<K, V>* search(K);

    SkipList<K ,V>* skip_list_;
 private:
    BloomFilter* bloom_filter_;
};

template<typename K, typename V>
StorageEngine<K, V>::StorageEngine(int bloomfilter_expect_data_count, 
    double bf_false_positive_rate, int skiplist_max_level) {
    bloom_filter_ = new BloomFilter(bloomfilter_expect_data_count, bf_false_positive_rate);
    skip_list_ = new SkipList<K, V>(skiplist_max_level);
}

template<typename K, typename V>
StorageEngine<K, V>::~StorageEngine() {
    delete bloom_filter_;
    delete skip_list_;
}

// 成功返回0，失败返回-1
template<typename K, typename V>
int StorageEngine<K, V>::insert(K key, V val) {
    if(bloom_filter_->contain(key) == false) {
        bloom_filter_->insert(key);
    }
    // 无论是否存在于布隆过滤器，都应该插入跳表，因为布隆过滤器可能误认为存在
    return skip_list_->insert(key, val);
}

template<typename K, typename V>
void StorageEngine<K, V>::erase(K key) {
    // 布隆过滤器表示可能包含该元素，但不一定包含
    if(bloom_filter_->contain(key) == true) {
        skip_list_->erase(key);
    }
    // 布隆过滤器表示不包含该元素，那么一定不包含，不需要删除
}

template<typename K, typename V>
SkipListNode<K, V>* StorageEngine<K, V>::search(K key) {
    // 布隆过滤器表示可能包含该元素，但不一定包含
    if(bloom_filter_->contain(key) == true) {
        return skip_list_->search(key);
    }
    // 布隆过滤器表示不包含该元素，那么一定不包含
    return NULL;
}

