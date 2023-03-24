#include "storageEngine.h"

using namespace std;

int main() {
    // 示例
    int data_count = 10;  
    double false_positive_rate = 0.03;
    StorageEngine<string, string> se(data_count, false_positive_rate, log(data_count) / log(2));
    for(int i = 0; i < 10; ++i) {
       se.insert(to_string(i), to_string(i));
    }
    se.skip_list_->print();
    return 0;
}