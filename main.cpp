#include "skiplist.h"

using namespace std;

int main() {
    SkipList<int, int> sl(5);
    sl.read_file();
    for(int i = 0; i < 10; ++i){
        sl.insert(i, i);
    }
    sl.print();
    sl.write_file();
    return 0;
}