#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>

#include <map>
#include <vector>
#include <set>

#include "../types.h"
#include "../filetypes/FileTypes.h"

class FileSet {

public:
    FileSet()
    {
        minTimestamp = maxTimestamp = 0;
        filesCounter = 0;
        knownTypes = NULL;
        this->logFile = NULL;
    }

    ~FileSet()
    {
        if (!logFile) return;
        fclose(logFile);
    }

    virtual void init()
    {
        logFilename = "infected_log.txt";
        this->logFile = fopen(logFilename.c_str(), "w");
        if (!logFile) {
            printf ("Open log file failed\n");
            return;
        }
    }

    void setKnownTypes(FileTypesSet *types)
    {
        knownTypes = types;
    }

    virtual std::string getPivotFile()
    {
        return this->minTimestampFile;
    }

    void addFile(std::string path, std::string ext, uint32_t writeTime);


    size_t countFiles()
    {
        return filesCounter;
    }

    void printSummary()
    {
        printf("INFECTED: %ld\n", countFiles());
        if (countFiles() == 0) return;

        printf("Files:\n");
        printf("1) %s\n", minTimestampFile.c_str());
        printf("2) %s\n", maxTimestampFile.c_str());
    }

    void printSummary(FILE* logFile)
    {
        if (!logFile) return;
        if (countFiles() == 0) return;

        uint32_t dif = maxTimestamp - minTimestamp;
        fprintf(logFile, "TIMESTAMPS:\nMin: %d\nMax: %d\nDif: %d\n", minTimestamp, maxTimestamp, dif);
        fprintf(logFile, "1) %s\n", minTimestampFile.c_str());
        fprintf(logFile, "2) %s\n", maxTimestampFile.c_str());

        fprintf(logFile, "INFECTED: %ld\n", countFiles());
        fprintf(logFile, "EXTENSIONS:\n");
        std::set<std::string>::iterator itr;
        for (itr = extensions.begin(); itr != extensions.end(); itr++) {
            fprintf(logFile, "%s\n", itr->c_str());
        }
    }

    std::string minTimestampFile, maxTimestampFile, pivotFile;
    uint32_t minTimestamp, maxTimestamp;

protected:
    virtual void addPivot(std::string path, std::string ext, uint32_t writeTime);
    virtual bool processFile(std::string path, std::string ext, uint32_t writeTime);

    bool logToFile(std::string path, uint32_t writeTime);
    std::string logFilename;
    size_t filesCounter;
    std::set<std::string> extensions;

    FileTypesSet* knownTypes;
    FILE *logFile;
};

class FileStoringSet : public FileSet
{
public:
    FileStoringSet()
    {
        lastWriteTime = 0;
    }
    //---

    std::map<uint32_t, std::vector<std::string> > filesMap;
    std::map<uint32_t, std::string> pivotsMap;

protected:
    virtual bool processFile(std::string path, std::string ext, uint32_t writeTime)
    {
        FileType *type = this->knownTypes->getType(ext);
        if (type != NULL && type->accuracy >= PIVOT_MIN) {
            if (abs(long(lastWriteTime - writeTime)) > 3) {
                pivotsMap[writeTime] = path;
            }
        }
        lastWriteTime = writeTime;

        insertFile(path, writeTime);
        logToFile(path, writeTime);
        return true;
    }

    void insertFile(std::string path, uint32_t writeTime)
    {
        filesMap[writeTime].push_back(path);   
    }
    //---

    uint32_t lastWriteTime;
};