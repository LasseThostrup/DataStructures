#include <stdint.h>
#include "HashTable.h"
#include "HashTableExtendibleHashing.h"
#include "HashTableLinearHashing.h"
#include <iostream>

void testExtendibleHashing() {
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
            exit(-1);
        }
    }
    ht.print_dir();
}


void testLinearHashing() {
    HTLinearHashing<uint64_t, uint64_t> ht;

    for (size_t i = 0; i < 128; i++)
    {
        ht.insert(i, i);        
        uint64_t val;
        ht.lookup(i, val);
        if (val != i) { 
            std::cout << "didn't find " << i << " but got " << val << std::endl;
            exit(-1);
        }
    }
    ht.print();
    // std::cout << val << std::endl;

    // ht.print();
    for (size_t i = 0; i < 128; i++)
    {
        uint64_t val;
        ht.lookup(i, val);
        if (val != i) { 
            std::cout << "didn't find " << i << " but got " << val << std::endl;
            exit(-1);
        }
    }
    // ht.print_dir();
}

int main(int argc, char const *argv[])
{
    // testExtendibleHashing();
    testLinearHashing();
    return 0;
}
