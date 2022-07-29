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


template <class Key, class Value>
class HTExtendibleHashing : public HashTable<Key, Value> {

    static const uint32_t MAX_BUCKET_SIZE = 4;

    struct Entry {
        Key key;
        Value value;
    };
    struct Bucket {
        uint64_t key;
        uint32_t local_depth;
        uint32_t size;
        Entry entries[MAX_BUCKET_SIZE];
    };

    std::vector<Bucket*> directory;
    uint32_t global_depth = 1;

    void split(Bucket* bucket, uint64_t hashkey) {
        // print_dir();
        hashkey = bucket->key;
        // std::cout << "splitting on bucket " << hashkey << " local depth: " << bucket->local_depth << " global depth: " << global_depth << std::endl;
        //determine "sibling" directory location for new bucket (i.e., next directory location that points to same bucket)
        auto sibling_key = hashkey | (1 << bucket->local_depth);
        // std::cout << "creating new bucket on dir offset: " << sibling_key << std::endl;

        directory[hashkey] = new Bucket{bucket->key, bucket->local_depth+1,0};
        directory[sibling_key] = new Bucket{sibling_key, bucket->local_depth+1,0};

        //Update all pointers pointing to the bucket we are splitting
        uint64_t i = sibling_key + (sibling_key-hashkey);
        while (i < directory.size()) {
            directory[i] = directory[hashkey];
            i += (sibling_key-hashkey);
            directory[i] = directory[sibling_key];
            i += (sibling_key-hashkey);
        }


        for (size_t i = 0; i < MAX_BUCKET_SIZE; i++)
        {
            auto &entry = bucket->entries[i];
            auto new_hashkey = this->hash(entry.key) & ((1 << (global_depth))-1);
            auto new_bucket_ptr = directory[new_hashkey];
            new_bucket_ptr->entries[new_bucket_ptr->size++] = entry;
            // std::cout << "\tinserting key " << entry.key << " into bucket: " << new_bucket_ptr->key << " dir offset: " << new_hashkey <<  std::endl;
            // if (!(new_hashkey == sibling_key || new_hashkey == hashkey)) {
            //     std::cout << " new key " << new_hashkey << " didn't match sibling " << sibling_key << " or old key " << hashkey << std::endl;
            // }
            // assert(new_hashkey == sibling_key || new_hashkey == hashkey);
        }
        delete bucket;
    }

public:
    HTExtendibleHashing() : directory(2) {
        directory[0] = new Bucket{0,1,0};
        directory[1] = new Bucket{1,1,0};
    }
    ~HTExtendibleHashing() {}


    virtual void insert(Key key, Value value) override {
        auto hashkey = this->hash(key) & ((1 << global_depth)-1);

        // std::cout << "inserting key " << key << " in bucket " << hashkey << std::endl;

        auto bucket_ptr = directory[hashkey];

        if (bucket_ptr->size < MAX_BUCKET_SIZE) {
            bucket_ptr->entries[bucket_ptr->size++] = Entry{key, value};
            return;
        }

        if (bucket_ptr->local_depth < global_depth) { //Add new bucket without increasing global size
            // if (key == 14) 
            //     print_dir();
            split(bucket_ptr, hashkey);
            // if (key == 14) {
            //     print_dir();
            //     exit(-1);
            // }
            return insert(key, value);
        }

        //increase global size
        assert(bucket_ptr->local_depth == global_depth);  
        // std::cout << "growing to size: " << directory.size()*2 << std::endl;
        // print_dir();

        //Duplicate directory
        directory.reserve(directory.size()*2);
        directory.insert(directory.end(), directory.begin(), directory.end());

        global_depth += 1;
        split(bucket_ptr, hashkey);
        return insert(key, value);
    }

    virtual bool lookup(Key key, Value &value_ret) override {
        auto hashkey = this->hash(key) & ((1 << global_depth)-1);
        auto bucket_ptr = directory[hashkey];
        for (size_t i = 0; i < bucket_ptr->size; i++)
        {
            if (key == bucket_ptr->entries[i].key) { 
                value_ret = bucket_ptr->entries[i].value;
                return true;
            }
        }
        return false;
    }

    void print_dir() {
        std::cout << " ------------ directory size: " << directory.size() << std::endl;
        for (size_t i = 0; i < directory.size(); i++)
        {
            if (i != directory[i]->key)
            {
                continue;
            }
            std::cout << " i: " << i << " - key: " << directory[i]->key << " - ptr: " << reinterpret_cast<uintptr_t>(directory[i]) << std::endl;
            
            for (size_t j = 0; j < directory[i]->size; j++)
            {
                std::cout << "\t\t" << directory[i]->entries[j].key << std::endl;
            }
            
        }
        std::cout << " ------------ " << std::endl;
    }

};