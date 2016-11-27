#pragma once

#include "types.h"

#include <windows.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <string>
#include "types.h"
#include "timeconv.h"

//#define DEBUG 1

class Params {

public:
    static uint32_t encryptionStartTime();

    Params()
    {
        this->timeNow = static_cast<uint32_t> (time(NULL));
        this->startTime = encryptionStartTime();
        this->seed = timeNow;
        incrementalMode = false;
        filename = ""; //not known yet
        fileEncryptionTime = 0;
        key_num = 0;
    }

    void init(std::string enc_filename, uint32_t encryptionTime)
    {
        if (enc_filename.length() == 0) return;
        this->filename = enc_filename;
        this->fileEncryptionTime = encryptionTime;
        if (seed == 0 || seed > fileEncryptionTime) {
            seed = encryptionTime;
        }
        autosetMode();
    }

    void autosetMode() {
        this->incrementalMode = (seed < this->fileEncryptionTime);
    }

//----
    std::string filename;
    uint32_t startTime; //optional
    uint32_t fileEncryptionTime; //optional

    uint32_t timeNow;
    uint32_t seed;
    size_t key_num;
    bool incrementalMode;
    size_t series_min, series_max;
    static const char id_hash[];

protected:    
    void setIncrementalMode(bool val);
};