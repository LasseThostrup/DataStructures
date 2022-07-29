#include <stdint.h>
#include "HashTable.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    HTExtendibleHashing<uint64_t, uint64_t> ht;

    for (size_t i = 0; i < 512; i++)
    {
        ht.insert(i, i);        
    }
    
    // std::cout << val << std::endl;

    // ht.print();
    for (size_t i = 0; i < 512; i++)
    {
        uint64_t val;
        ht.lookup(i, val);
        if (val != i) { 
            std::cout << "didn't find " << i << " but got " << val << std::endl;
        }
    }
    ht.print_dir();
    return 0;
}
