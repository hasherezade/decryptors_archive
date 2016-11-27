#pragma once

#include "../types.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#define HDRS_DIR "headers"
#define SPECIAL_TITLE "special"

#define MIN_ACCURACY 4
#define PIVOT_MIN 5

enum type_group {
    HEADER = 0,
    MULTI = 1,
    SPECIAL
};
typedef type_group  T_GROUP;

class FileType {
public:
    FileType()
    {
        this->offset = this->r_offset = this->m_offset = 0;
        this->accuracy = this->c_accuracy = CHUNK_SIZE;
        this->group = HEADER;
    }

    virtual bool init(std::string filename, std::string extension)
    {
        if (parseName(filename) == false) return false;
        setExtension(extension);

        if (readHeader(filename, offset) == false) return false;
        return true;
    }

    virtual bool init(std::string filename, std::string extension, size_t offset, size_t accuracy)
    {
        setExtension(extension);
        init_params(offset, accuracy);

        if (readHeader(filename, offset) == false) return false;
        return true;
    }

    void setExtension(std::string extension)
    {
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        this->extension = extension;
    }

    static const size_t CHUNK_SIZE = 16;
    unsigned char buffer[CHUNK_SIZE + 1];

    std::string extension;
    T_GROUP group;

    size_t getReadOffset() { return r_offset; }
    size_t getMatchOffset() { return m_offset; }
    size_t getAccuracy() { return c_accuracy; }

    //calculated
    size_t r_offset; //read offset - offset of chunk including header
    size_t m_offset; //match offset - where the matching should start
    size_t c_accuracy; //how long should match 

    //raw:
    size_t offset; //where the header starts
    size_t accuracy; //how long should match

protected:

    void init_params(size_t offset, size_t accuracy)
    {
        this->offset = offset;
        this->accuracy = accuracy;

        r_offset = offset / FileType::CHUNK_SIZE;
        m_offset = offset - r_offset;

        size_t dif = FileType::CHUNK_SIZE - m_offset;
        c_accuracy = accuracy > dif ? dif : accuracy;
#ifdef DEBUG
        if (offset != 0) {
            printf("Raw offset: %d\n", offset);
            printf("Chunk read offset: %d\n", r_offset);
            printf("Chunk match offset: %d\n", m_offset);
            printf("Check accuracy: %d\n", c_accuracy);
        }
#endif
    }

    virtual bool parseName(std::string filename);
    virtual bool readHeader(std::string filename, size_t offset);
};

class FileTypesSet
{
public:
    //destroy all FileType objects
    ~FileTypesSet() { deleteTypes(); } 

    size_t listDir(std::string dirName);

    void deleteTypes()
    {
        if (types.size() == 0) return;
        std::map<std::string, FileType*>::iterator itr;
        for (itr = types.begin(); itr != types.end(); itr++) {
            FileType *type = itr->second;
            delete type;
        }
        types.clear();
    }

    FileType* getType(std::string extension);
    size_t countTypes() { return types.size(); }

protected:
    std::map<std::string, FileType*> types;
};

class SpecialFileType : public FileType
{
public:
    SpecialFileType()
    {
        this->offset = this->r_offset = this->m_offset = 0;
        this->accuracy = 1;
        this->group = SPECIAL;
    }

    virtual bool init(std::string filename, std::string extension)
    {
        setExtension(extension);
        //init_params();
        return true;
    }
};


//type with multiple alternative headers:
class MultiFileType : public FileType
{
public:

    MultiFileType()
    {
        this->offset = this->r_offset = this->m_offset = 0;
        this->accuracy = 1;
        this->group = MULTI;
    }
    ~MultiFileType() { deleteTypes(); } 

    virtual bool init(std::string dirName, std::string extension)
    {
        setExtension(extension);
        parseName(dirName);
        if (listDir(dirName, extension) == 0) return false;
        return true;
    }

    size_t listDir(std::string dirName, std::string extension);

    void deleteTypes()
    {
        if (defs.size() == 0) return;
        std::set<FileType*>::iterator itr;
        for (itr = defs.begin(); itr != defs.end(); itr++) {
            FileType *type = *itr;
            delete type;
        }
        defs.clear();
    }

    std::set<FileType*> defs;
};