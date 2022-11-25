#pragma once

#include <array>
#include <vector>
#include <tuple>
#include <stddef.h>
#include <cstdint>
#include <iostream>
#include <cassert>
#include <math.h>
#include "HashTable.h"
#include <unordered_set>
#include <memory>

template <class Key, class Value>
class HTLinearHashing : public HashTable<Key, Value> {

    struct Bucket {
        Key key;
        Value value;
        std::unique_ptr<Bucket> next;
    };

    std::vector<std::unique_ptr<Bucket>> buckets;

    uint64_t next_split = 0;
    uint64_t M = 2;
    float split_factor = 0.9;
    uint64_t filled = 0;
public:
    HTLinearHashing() : buckets(4) {

    }
    ~HTLinearHashing() {
        
    }


    virtual void insert(Key key, Value value) override {
        auto prehash = this->hash(key);
        auto hashkey = prehash & (M-1);
        
        // bucket was split if its not between next_split and M;
        if (hashkey < next_split) {
            hashkey = prehash & (M*2-1);
        }

        //insert bucket in linked list
        // auto a = std::make_unique<Bucket>({key, value, std::move(buckets[hashkey])});

        buckets[hashkey] = std::unique_ptr<Bucket>(new Bucket{key, value, std::move(buckets[hashkey])});
        filled++;

        
        if (1.0 * filled / buckets.size() >= split_factor) {
            //Grow and split
            
            auto head = std::move(buckets[next_split]);
            buckets[next_split] = nullptr; //create new chain (to avoid doing a lot of pointer gymnastics)
            while (head) {
                auto new_hashkey = this->hash(head->key) & (M*2-1);
                auto tmp = std::move(head->next);
                head->next = std::move(buckets[new_hashkey]);
                buckets[new_hashkey] = std::move(head);

                head = std::move(tmp);
            }

            // std::cout << "Splitting - next bucket to split: " << next_split << std::endl;

            next_split++;
            if (next_split == M) {
                //Grow hash function

                buckets.resize(buckets.size() * 2);        //ensure that buckets is populated with nullptrs!
            
                M *= 2;
                next_split = 0;
                // std::cout << "Increased M to " << M << " hash table size: " << buckets.size() << std::endl;
            }
        }
    }

    virtual bool lookup(Key key, Value &value_ret) override {
        auto prehash = this->hash(key);
        auto hashkey = prehash & (M-1);

        if (hashkey < next_split) {
            hashkey = prehash & (M*2-1);
        }
        Bucket* head = buckets[hashkey].get();
        while (head) { 
            if (head->key == key) {
                value_ret = head->value;
                return true;
            }
            head = head->next.get();
        }
        return false;
    }

    void print() {
        std::cout << " ----- HashTable size: " << buckets.size() << " ------- " << std::endl; 
        for (size_t i = 0; i < buckets.size(); i++)
        {
            std::cout << "i: " << i << std::endl;
            Bucket* head = buckets[i].get();
            while (head) { 
                std::cout << "\t" << head->key << std::endl;
                head = head->next.get();
            }
        }
        std::cout << " -------------------- " << std::endl; 
    }
};