#include <stdint.h>
#include "HashTable.h"
#include "HashTableExtendibleHashing.h"
#include "HashTableLinearHashing.h"
#include <iostream>

int main(int argc, char const *argv[])
{


    {
        HTOpenAddr<uint64_t, uint64_t> ht(4);
        if (!ht.test()) std::cout << "Failed HTOpenAddr test" << std::endl;
    }
    {
        HTExtendibleHashing<uint64_t, uint64_t> ht;
        if (!ht.test()) std::cout << "Failed HTExtendibleHashing test" << std::endl;
    }
    {
        HTLinearHashing<uint64_t, uint64_t> ht;
        if (!ht.test()) std::cout << "Failed HTLinearHashing test" << std::endl;
    }
    std::cout << "Done..." << std::endl;
    return 0;
}
