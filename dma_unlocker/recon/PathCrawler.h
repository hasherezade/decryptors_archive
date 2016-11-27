#pragma once

#include "../types.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include "FileSet.h"

class PathCrawler
{
public:
    static std::string getLocalDir();
        
    void listDir(std::string path, FileSet *fileSet, bool skip_decrypted);
    bool isExtensionAttacked(char *filename);
    bool isInfected(std::string filename);
    bool isDecrypted(std::string filename);
};