#pragma once

#include "../recon/FileSet.h"
#include "../dma_crack.h"

class CrackedSet : public FileSet
{
public:

    CrackedSet(FileTypesSet &fTypes, Params &p)
        : fileTypes(fTypes), params(p) { lastDecryptedTimestamp = 0; }

    virtual void init()
    {
        logFilename = "cracked_log.txt";
        FILE *logFile = fopen(logFilename.c_str(), "w");
        if (!logFile) {
            printf ("Open log file failed\n");
            return;
        }
        fclose(logFile);
    }

protected:
    virtual bool processFile(std::string path, std::string ext, uint32_t writeTime);
    void autoinitParams(std::string path, std::string ext, uint32_t writeTime);

    FileTypesSet &fileTypes;
    Params &params;
    uint32_t lastDecryptedTimestamp;
};