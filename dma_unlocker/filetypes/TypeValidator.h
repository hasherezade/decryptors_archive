#pragma once

#include "../types.h"
#include "../util.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>

#include "FileTypes.h"
#include "../decryptor/decryptor.h"

class TypeValidator
{
public:

    static bool read_header(std::string filename, unsigned char buf[], size_t offset);
    static size_t get_original_file_size(std::string filename, size_t offset);
//------------------
    TypeValidator()
    {
        isInit = false;
        my_type = NULL;
    }
    
    virtual size_t getAccuracy()
    {
        if (my_type == NULL) return 0;
        return my_type->getAccuracy();
    }

    virtual bool init(std::string enc_filename, FileType* my_type);
    virtual bool testKey(std::string key);

protected:
    virtual bool isDecodedValid(unsigned char *buf) = 0;
    //------
    FileType *my_type;
    //base, copied from the file:
    unsigned char enc_buf[DmaDecryptor::UNIT_LEN + 1];

    //verification buffer
    unsigned char buf[DmaDecryptor::UNIT_LEN + 1];

    bool isInit;
};

class HeaderValidator : public TypeValidator
{
public:
    static bool isDecodedValid(unsigned char *buf, FileType *my_type);
protected:
    virtual bool isDecodedValid(unsigned char *buf);

};

class BMPValidator : public TypeValidator
{
public:
    virtual bool init(std::string enc_filename, FileType* my_type);
    virtual size_t getAccuracy() { return 6; } // 2 for header + 4 for size

protected:
    virtual bool isDecodedValid(unsigned char *buf);
    uint32_t original_size;
};

class MultiValidator : public TypeValidator
{
public:
    MultiValidator() { my_type = NULL; }

    virtual bool init(std::string enc_filename, FileType* type)
    {
        this->my_type = dynamic_cast<MultiFileType*> (type);
        if (my_type == NULL) return false;
        return TypeValidator::init(enc_filename, my_type);
    }

    virtual size_t getAccuracy() { return 4; }

protected:
    virtual bool isDecodedValid(unsigned char *buf);
    MultiFileType *my_type;
};