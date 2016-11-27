#include "DmaKeygen.h"
#include "keygen.h"

    std::string DmaKeygen::makeNextKey()
    {
        char buffer[KEY_SIZE + 1];
        make_random_key(buffer, KEY_SIZE);
        buffer[KEY_SIZE] = '\0';

        this->key = buffer;
        this->keyNumber++;
        return this->key;
    }