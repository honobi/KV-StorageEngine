#include "bloomfilter.h"
#include "murmurhash3.h"

void Bitset::set(size_t index, bool val) {
    if(val)
        vec_[index >> 5] |= (1 << (index % 32));  // TODO: index % 32也可以改为位运算
    else    
        vec_[index >> 5] &= ~(1 << (index % 32));
}

bool Bitset::get(size_t index) {
    if(vec_[index >> 5] & (1 << (index % 32))) {
        return true;
    }
    return false;
}

BloomFilter::BloomFilter(int expect_data_count, double false_positive_rate) 
    : expect_data_count_(expect_data_count), false_positive_rate_(false_positive_rate) {
    bitset_size_ = computeBitsetSize();
    bitset_ = new Bitset(bitset_size_);
    hash_count_ = computeHashCount();
    std::cout << "bitset_size: " << bitset_size_ << std::endl;
    std::cout << "hash_count" << hash_count_ << std::endl;
}

BloomFilter::~BloomFilter() {
    delete bitset_;
}

size_t BloomFilter::computeBitsetSize() {
    return ceil( -((double)expect_data_count_ * log(false_positive_rate_)) / (log(2.0) * log(2.0)));  // 向上取整
}

int BloomFilter::computeHashCount() {
    return ceil( ((double)bitset_size_ / (double)expect_data_count_) * log(2.0));  // 向上取整
}

void BloomFilter::insert(const std::string& key) {
    for(int i = 0; i < hash_count_; ++i) {
        size_t index = hash_function(i, key) % bitset_size_;
        bitset_->set(index, 1);
    }
}

bool BloomFilter::contain(const std::string& key) {
    for(int i = 0; i < hash_count_; ++i) {
        size_t index = hash_function(i, key) % bitset_size_;
        if(bitset_->get(index) == false){
            return false;
        }
    }
    return true;
}

size_t BloomFilter::hash_function(int i, const std::string& key) {
    return murmurhash3_x86_32(key.c_str(), key.size(), i);
}