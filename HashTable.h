#pragma once
#include <array>
#include <vector>
#include <tuple>
#include <stddef.h>
#include <cstdint>
#include <iostream>
#include <cassert>
#include <math.h>

template <class Key, class Value>
class HashTable
{
protected:
    
    inline uint64_t hash(Key key)
    {
        // return key;
        key ^= key >> 33;
        key *= 0xff51afd7ed558ccd;
        key ^= key >> 33;
        key *= 0xc4ceb9fe1a85ec53;
        key ^= key >> 33;
        return key;
    }
public:
    virtual void insert(Key key, Value value) = 0;
    virtual bool lookup(Key key, Value &value_ret)  = 0;

    bool test() {
        bool success = true;
        size_t size = 1234;
        for (size_t i = 0; i < size; i++)
        {
            this->insert(i, i);        
            uint64_t val;
            success &= this->lookup(i, val);
            if (val != i | !success) {
                std::cout << "didn't find " << i << " but got " << val << std::endl;
                exit(-1);
            }
        }

        for (size_t i = 0; i < size; i++)
        {
            uint64_t val;
            this->lookup(i, val);
            if (val != i | !success) { 
                std::cout << "didn't find " << i << " but got " << val << std::endl;
                exit(-1);
            }
        }
        return success;
    };
};



template <class Key, class Value>
class HTOpenAddr : public HashTable<Key, Value>
{
private:
    static_assert(std::is_same<Key, uint64_t>::value);
    enum State {
        FREE,
        OCCUPIED,
        TOMBSTONE,
    };
    struct Entry {
        State state;
        Key key;
        Value value;
    };



    inline uint64_t linear_probing(Key key) { return (key+1); }

    std::vector<Entry> hasharray;
    size_t ht_size;
    size_t filled = 0;
public:
    HTOpenAddr(size_t size) : HashTable<Key, Value>(), hasharray(size, {State::FREE}), ht_size(size) {
        // std::fill(hasharray.begin(), hasharray.end(), {State::FREE});
    };

    ~HTOpenAddr() {};

    void insert(Key key, Value value) override {
        auto hashkey = this->hash(key) % ht_size;
        while (hasharray[hashkey].state != FREE) {
            hashkey = linear_probing(hashkey) % ht_size;
        }
        hasharray[hashkey] = Entry{OCCUPIED, key, value};
        filled++;

        //Grow hash table
        //Optimization ideas: use radix hashing and take one more bit for hashing - use set of hash tables?
        if (1.0*filled / ht_size > 0.5) { 
            auto new_ht_size = ht_size*2;
            // std::cout << "Growing ht to size: " << new_ht_size << std::endl;
            auto new_hasharray = std::vector<Entry>(new_ht_size, {State::FREE});
            for (size_t i = 0; i < ht_size; i++)
            {
                if (hasharray[i].state == OCCUPIED) { 
                    auto new_hashkey = this->hash(hasharray[i].key) % new_ht_size;

                    while (new_hasharray[new_hashkey].state != FREE) {
                        new_hashkey = linear_probing(new_hashkey) % new_ht_size;
                    }
                    new_hasharray[new_hashkey] = hasharray[i];
                    // std::cout << "from " << i << " to " << new_hashkey << std::endl;
                }
            }
            ht_size = new_ht_size;
            hasharray = new_hasharray;
        }
    };

    bool lookup(Key key, Value &value_ret) override {
        auto hashkey = this->hash(key) % ht_size;
        while (hasharray[hashkey].state != FREE) {
            if (hasharray[hashkey].state == OCCUPIED && hasharray[hashkey].key == key) {
                value_ret = hasharray[hashkey].value;
                // std::cout << " ------------ " << value_ret << " pos: " << hashkey << std::endl;
                return true;
            }
            hashkey = linear_probing(hashkey) % ht_size;
            // std::cout << hashkey << " ... ";
        }
        // std::cout << " ------------ fail pos: " << hashkey << std::endl;
        return false;
    };

    void print() {
        for (size_t i = 0; i < ht_size; i++) {
            const auto &entry = hasharray[i];
            if (entry.state == OCCUPIED) { 
                std::cout << i << " - " << entry.key << " - " << entry.value << std::endl;
                // Value val;
                // if (!lookup(entry.key, val)) {
                //     std::cout << i << " - " << entry.key << " - " << entry.value << std::endl;
                //     std::cout << "DIDNT FIND KEY!!!" << std::endl;
                // }
                // if (val != entry.key) {
                //     std::cout << " GOT MISMATCH!!" << std::endl;
                // }
            }
        }

    }

    
};





