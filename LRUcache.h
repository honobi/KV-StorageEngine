#include <unordered_map>
#include "skiplist.h"

template<typename K, typename V>
class LruNode{
 public:
   //LruNode() {}
   LruNode(SkipListNode<K, V>* node) : skip_list_node_(node) {};
   SkipListNode<K, V>* skip_list_node_;  // LRU链表的val就是一个跳表节点的指针
   LruNode<K, V>* prev;
   LruNode<K, V>* next;
};

template<typename K, typename V>
class LRUCache {
 public: 
   LRUCache(int capacity);
   ~LRUCache();
   void insert(K key, SkipListNode<K, V>*);
   void remove(K key);
   SkipListNode<K, V>* get(K key);
   void display();
 private:
   void push_front(LruNode<K, V>*);
   void remove(LruNode<K, V>*);
   int size;      // 链表中节点数量
   int capacity;
   LruNode<K, V>* head;    // head和tail是哨兵节点。链表头是最近使用的，尾节点是最近最少使用的
   LruNode<K, V>* tail;    
   std::unordered_map<K, LruNode<K, V>*> umap;  //<key，节点>
};

template<typename K, typename V>
LRUCache<K, V>::LRUCache(int capacity) {
   size = 0;
   this->capacity = capacity;
   head = new LruNode<K, V>(NULL);
   tail = new LruNode<K, V>(NULL);
   head->next = tail;
   head->prev = NULL;
   tail->prev = head;
   tail->next = NULL;
}
template<typename K, typename V>
LRUCache<K, V>::~LRUCache() {
   LruNode<K, V>* cur = head;
   while(cur) {
      LruNode<K, V>* tmp = cur->next;
      delete cur;
      cur = cur->next;
   }
}

template<typename K, typename V>
void LRUCache<K, V>::remove(K key) {
   auto it = umap.find(key);
   if(it != umap.end()){
      remove(it->second);
      umap.erase(key);
   }
}

template<typename K, typename V>
void LRUCache<K, V>::remove(LruNode<K, V>* cur){
   cur->prev->next = cur->next;
   cur->next->prev = cur->prev;
   size--;
}

template<typename K, typename V>
void LRUCache<K, V>::push_front(LruNode<K, V>* cur){
   cur->prev = head;
   cur->next = head->next;
   head->next->prev = cur;
   head->next = cur;
   size++;
}

template<typename K, typename V>
SkipListNode<K, V>* LRUCache<K, V>::get(K key) {
   auto it = umap.find(key);
   // 如果该节点存在，那么将该节点移到链表头
   if(it != umap.end()){
      remove(it->second);
      push_front(it->second);
      return it->second->skip_list_node_;
   }
   return NULL;
}

template<typename K, typename V>
void LRUCache<K, V>::insert(K key, SkipListNode<K, V>* value) {
   auto it = umap.find(key);
   // 如果存在，更新val，然后移动到链表头
   if(it != umap.end()){
      it->second->skip_list_node_ = value;
      remove(it->second);
      push_front(it->second);
      return;
   }
   // 如果不存在，那么构造节点并插入链表和umap
   LruNode<K, V>* cur = new LruNode<K, V>(value);
   push_front(cur);
   umap[key] = cur;

   // 如果插入导致大小超过容量，那么删除尾节点
   if(size > capacity){
      umap.erase(tail->prev->skip_list_node_->key());
      remove(tail->prev);
   }
}

template<typename K, typename V>
void LRUCache<K, V>::display() {
   LruNode<K, V>* cur = head->next;
   while(cur != tail) {
      std::cout << cur->skip_list_node_->key() << ":" << cur->skip_list_node_->val() << " ";
      cur = cur->next;
   }
   std::cout << std::endl;
}