#include <stdint.h>
#include "HashTable.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    HTOpenAddr<uint64_t, uint64_t> ht(1024);

    for (size_t i = 0; i < 512; i++)
    {
        ht.insert(i, i);        
    }
    
    ht.insert(1234, 1234);
    uint64_t val;
    ht.lookup(1234, val);

    std::cout << val << std::endl;

    // ht.print();
    for (size_t i = 0; i < 512; i++)
    {
        uint64_t val;
        ht.lookup(i, val);
        if (val != i) { 
            std::cout << "didn't find " << i << " but got " << val << std::endl;
        }
    }

    return 0;
}
