#include "bloomfilter.h"
#include "skiplist.h"
#include "LRUcache.h"

template<typename K, typename V>
class StorageEngine {
 public:
    StorageEngine(int bloomfilter_expect_data_count, double bf_false_positive_rate, int skiplist_max_level, int lru_capacity);
    ~StorageEngine();
    void insert(K key, V val);
    void erase(K);
    SkipListNode<K, V>* search(K);
    void print_skiplist();
 private:
    SkipList<K ,V>* skip_list_;
    LRUCache<K, V>* lru_cache_;
    BloomFilter* bloom_filter_;
};

template<typename K, typename V>
StorageEngine<K, V>::StorageEngine(int bloomfilter_expect_data_count, 
    double bf_false_positive_rate, int skiplist_max_level, int lru_capacity) {
    bloom_filter_ = new BloomFilter(bloomfilter_expect_data_count, bf_false_positive_rate);
    skip_list_ = new SkipList<K, V>(skiplist_max_level);
    lru_cache_ = new LRUCache<K, V>(lru_capacity);
    skip_list_->read_file();
}

template<typename K, typename V>
StorageEngine<K, V>::~StorageEngine() {
    skip_list_->write_file();
    delete bloom_filter_;
    delete lru_cache_;
    delete skip_list_;
}

template<typename K, typename V>
void StorageEngine<K, V>::insert(K key, V val) {
    // 如果不存在于布隆过滤器，那么插入布隆过滤器。但是布隆过滤器无法优化插入
    if(bloom_filter_->contain(key) == false) {
        bloom_filter_->insert(key);
    }
    // 如果存在于LRUCache，那么说明已经存在，无需插入
    if(lru_cache_->get(key) != NULL) {
        return;
    }
    // 如果key不存在布隆过滤器和LRUCache，那么key可能存在也可能不存在（bf误判和LRU未命中），所以需要插入跳表和LRU
    SkipListNode<K, V>* sk_node = skip_list_->insert(key, val);
    lru_cache_->insert(key, sk_node);
}

template<typename K, typename V>
void StorageEngine<K, V>::erase(K key) {
    // 布隆过滤器表示可能包含该元素，但不一定包含
    if(bloom_filter_->contain(key) == true) {
        // 从跳表删除
        if(skip_list_->erase(key) != false){
            // 只有存在于跳表，才有可能在LRUCache里，LRU才需要执行删除操作
            lru_cache_->remove(key);
        }
    }
    // 布隆过滤器表示不包含该元素，那么一定不包含，不需要删除
}

template<typename K, typename V>
SkipListNode<K, V>* StorageEngine<K, V>::search(K key) {
    // 布隆过滤器表示可能包含该元素，但不一定包含
    if(bloom_filter_->contain(key) == true) {
        // 先去LRUCache找，没有再去跳表找
        SkipListNode<K, V>* lru_res = lru_cache_->get();
        if(lru_res != NULL)
            return lru_res;
        return skip_list_->search(key);
    }
    // 布隆过滤器表示不包含该元素，那么一定不包含
    return NULL;
}

template<typename K, typename V>
void StorageEngine<K, V>::print_skiplist() {
    skip_list_->print();
}