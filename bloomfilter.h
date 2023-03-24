#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <math.h>
#include <iostream>

class Bitset {
 public:
    Bitset(size_t size) : vec_((size + 31) >> 5) {}  // +31是为了向上取整，>>5实际是/32 
    void set(size_t index, bool val);
    bool get(size_t index);
 private:
    std::vector<uint32_t> vec_;
};


class BloomFilter{
 public:
    BloomFilter(int expect_data_count, double false_positive_rate);
    ~BloomFilter();
    void insert(const std::string& key);
    bool contain(const std::string& key);

 private:
    size_t computeBitsetSize();
    int computeHashCount();
    size_t hash_function(int i, const std::string& key);

    size_t bitset_size_;
    int hash_count_;
    int expect_data_count_;
    double false_positive_rate_;
    Bitset* bitset_;
};

#endif