#include <cstdint>
#include <cstring>

// 定义常量参数
constexpr uint32_t c1 = 0xcc9e2d51;
constexpr uint32_t c2 = 0x1b873593;
constexpr uint32_t r1 = 15;
constexpr uint32_t r2 = 13;
constexpr uint32_t m = 5;
constexpr uint32_t n = 0xe6546b64;

uint32_t murmurhash3_x86_32(const void* key, size_t len, uint32_t seed) {

    // 定义哈希值和数据块指针
    uint32_t h = seed;
    const uint8_t* data = (const uint8_t*)key;

    // 处理数据块
    while (len >= 4) {
        // 获取一个32位数据块
        uint32_t k = *(uint32_t*)data;

        // 计算哈希值
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        h ^= k;
        h = (h << r2) | (h >> (32 - r2));
        h = h * m + n;

        data += 4;
        len -= 4;
    }

    // 处理剩余字节
    uint32_t k = 0;
    switch (len) {
        case 3: k ^= data[2] << 16;
        case 2: k ^= data[1] << 8;
        case 1: k ^= data[0];
                k *= c1;
                k = (k << r1) | (k >> (32 - r1));
                k *= c2;
                h ^= k;
    }

    // 最后处理
    h ^= (uint32_t)len;
    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);

    return h;
}
