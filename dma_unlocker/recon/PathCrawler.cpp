#include "PathCrawler.h"
#include "../timeconv.h"
#include "../decryptor/decryptor.h"

#include <stdio.h>

std::string PathCrawler::getLocalDir()
{
    DWORD startPathLen = GetCurrentDirectory(0, 0); // determine size needed
    char *cpath = (char*) calloc(1, startPathLen);
    
    if ( GetCurrentDirectory(startPathLen, cpath) == 0 ) 
    { 
        printf("[-] getLocalDir failed!\n");
    }
    std::string path = cpath;
    free(cpath);
    return path;
}

bool PathCrawler::isExtensionAttacked(char *filename)
{
    char *skipped[] = { ".exe", ".msi", ".dll",  ".pif", 
        ".scr", ".sys", ".msp", ".com", 
        ".lnk", ".hta", ".cpl", ".msc"
    };
    size_t skipped_count = 12;

    char *ext = strrchr(filename, '.');
    if (ext == NULL) {
        return true;
    }
    
    for (size_t i = 0; i < skipped_count; i++) {
        if (strcmp(ext, skipped[i]) == 0) {
            //printf("Skipped: %s\n", filename);
            return false;
        }
    }
    return true;
}

bool PathCrawler::isInfected(std::string filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");
    if (fp == NULL) {
        return false;
    }
    char buf[10];
    size_t read_size = fread(buf, 1, 8, fp);
    fclose(fp);
    if (read_size < 8) return false;

    if (memcmp(buf, "!DMALOCK", 8) == 0) {
        return true;
    }
    return false;
}

bool PathCrawler::isDecrypted(std::string filename)
{
    //check if this file have a decrypted version£
    std::string decrypted_fn = DmaDecryptor::makeOutName(filename.c_str());
    if (GetFileAttributes(decrypted_fn.c_str()) == INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND) {
        return false; //no such file
    }
    return true; 
}

void PathCrawler::listDir(std::string path, FileSet *infectedFiles, bool skip_decrypted)
{
    if (infectedFiles == NULL) {
        printf("File set not initialized\n");
        return;
    }
    const std::string fileName = path + "\\*.*";
    WIN32_FIND_DATA foundFileData;
    HANDLE hFind = FindFirstFile(fileName.c_str(), &foundFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        return;
    }
    do {
        if (foundFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(foundFileData.cFileName, ".") == 0) continue;
            if (strcmp(foundFileData.cFileName, "..") == 0) continue;
            std::string full_path = path + "\\"+ foundFileData.cFileName;
            listDir(full_path, infectedFiles, skip_decrypted);
            continue;
        }
        std::string full_path = path +"\\"+ foundFileData.cFileName;
        if (isExtensionAttacked(foundFileData.cFileName) == false) {
            continue;
        }
        if (isInfected(full_path) == false) {
            continue;
        }
        if (skip_decrypted && isDecrypted(full_path) == true) {
            printf("Already decrypted, skipping: %s\n", foundFileData.cFileName);
            continue;
        }
        char* ext_none = "<none>";
        char* ext = strrchr(foundFileData.cFileName, '.');
        if (ext == NULL) {
            ext = ext_none;
        }
        uint32_t infection_time = static_cast<uint32_t> (FileTime_to_POSIX(foundFileData.ftLastWriteTime));
        infectedFiles->addFile(full_path,  ext, infection_time);
    } while (FindNextFile(hFind, &foundFileData));
    FindClose(hFind);
}
