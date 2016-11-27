#pragma once

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "aes256.h"

class DmaDecryptor {

public:
    static const size_t OFFSET = 0x88;
    static const size_t KEY_LEN = 32;
    static const size_t UNIT_LEN = 16;

    static bool isInfected(FILE *f);
    static std::string makeOutName(const char *inFile);
    
    static void decryptChunk(unsigned char enc_buf[], unsigned char key[]);

//---
    DmaDecryptor(std::string filename, std::string key)
    {
        this->filename = filename;
        this->key = key;

        this->encryptedSize = this->fileSize = 0;
        this->buffer = NULL;
        this->chunkNum = 0;
        this->lastChunkSize = 0;
    }

    ~DmaDecryptor()
    {
        if (buffer) free(buffer);
    }
    
    bool init();
    bool decrypt(std::string outFilename);

protected:
    bool _decrypt(unsigned char buf[], size_t bufSize, unsigned char key[]);

    size_t chunkSize;
    size_t chunkNum;
    size_t lastChunkSize;

    size_t encryptedSize, fileSize;
    std::string filename;
    std::string key;

    aes256_context ctx;

    unsigned char aesKey[KEY_LEN + 1];    
    unsigned char *buffer;
};