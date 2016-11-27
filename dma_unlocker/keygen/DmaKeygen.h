#pragma once

#include "../types.h"
#include <string>
#include <vector>

#define KEY_SIZE 32

class DmaKeygen
{
public:
    DmaKeygen(uint32_t seed)
    {
        reset(seed);
    }

    void reset(uint32_t seed)
    {
        this->seed = seed;
        reset();
    }

    void reset()
    {
        srand(seed);
        this->keyNumber = 0;
        this->key = "";
    }

    std::string makeNextKey();

    //make and store:
    std::vector<std::string> makeKeys(size_t key_number)
    {
        std::vector<std::string> keys;
        for (size_t i = 0; i < key_number; i++) {
            keys.push_back(makeNextKey());
        }
        return keys;
    }

    //make but not store
    std::string makeKeyNumber(size_t key_number)
    {
        std::vector<std::string> keys;
        for (size_t i = 0; i < key_number; i++) {
            makeNextKey();
        }
        return makeNextKey();
    }

    size_t searchNumberOfKey(std::string searched_key, size_t max_search)
    {
        std::vector<std::string> keys;
        for (size_t i = 0; i < max_search; i++) {
            makeNextKey();
            if (searched_key == this->key) {
                return i;
            }
        }
        return -1;
    }

    uint32_t seed;
    std::string key;
    size_t keyNumber;
};

