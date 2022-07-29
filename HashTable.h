
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
            std::cout << "Growing ht to size: " << new_ht_size << std::endl;
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
        std::cout << "splitting on bucket " << hashkey << " local depth: " << bucket->local_depth << " global depth: " << global_depth << std::endl;
        //determine "sibling" directory location for new bucket (i.e., next directory location that points to same bucket)
        auto sibling_key = hashkey | (1 << bucket->local_depth);
        std::cout << "creating new bucket on dir offset: " << sibling_key << std::endl;

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
            std::cout << "\tinserting key " << entry.key << " into bucket: " << new_bucket_ptr->key << " dir offset: " << new_hashkey <<  std::endl;
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