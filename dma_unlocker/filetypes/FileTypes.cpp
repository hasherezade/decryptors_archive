#include "FileTypes.h"
#include <Windows.h>
#include "../util.h"

bool FileType::parseName(std::string fname)
{
    char *filename = const_cast<char*>(fname.c_str());
    char* ext = strrchr(filename, '.');
    if (ext == NULL) return false;

    char* separator = strrchr(filename, '//');
    if (separator == NULL) return false;

    separator++;
    sscanf(separator, "%d", &accuracy); 

    char* separator2 = strrchr(filename, '_');
    if (separator2 != NULL) {
        separator2++;
        sscanf(separator2, "%d", &offset); 
    }
    if (accuracy < MIN_ACCURACY) {
        printf("[ERROR] Cannot register filetype %s! Minimal accuracy for match is %d\n", ext, MIN_ACCURACY);
        return false;
    }
    init_params(offset, accuracy);
    return true;
}

bool FileType::readHeader(std::string fname, size_t offset)
{
    char *filename = const_cast<char*>(fname.c_str());
    FILE *fp = fopen(filename,"rb");
    if (fp == NULL) {
        printf("Cannot open: %s\n", filename);
        return false;
    }
    memset(this->buffer, 0, sizeof(buffer));
    fseek(fp, offset, SEEK_SET);
    fread(this->buffer, 1, FileType::CHUNK_SIZE, fp);
    fclose(fp);
    buffer[FileType::CHUNK_SIZE] = '\0';
    return true;
}

FileType* FileTypesSet::getType(std::string ext)
{
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    std::map<std::string, FileType*>::iterator found = types.find(ext);
    if (found == types.end()) return NULL;
    return found->second;
}

size_t FileTypesSet::listDir(std::string path)
{
    const std::string fileName = path + "\\*.*";
    WIN32_FIND_DATA foundFileData;
    HANDLE hFind = FindFirstFile(fileName.c_str(), &foundFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No such dir: %s\n", path.c_str());
        return 0;
    }

    while (FindNextFile(hFind, &foundFileData)) {
        if (foundFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(foundFileData.cFileName, ".") == 0) continue;
            if (strcmp(foundFileData.cFileName, "..") == 0) continue;

            std::string ext = get_extension(foundFileData.cFileName);
            if (ext == "") continue; // skip

            std::string fullPath = path + "//" + foundFileData.cFileName;

            MultiFileType *multiType = new MultiFileType();
            if (multiType->init(fullPath, ext) == false) {
                delete multiType;
                continue;
            }
            this->types[ext] = multiType;
            continue;            
        }

        std::string ext = get_extension(foundFileData.cFileName);
        if (ext == "") {
            printf("skipping file without extension!\n");
            continue; //skipping file
        }

        std::string full_path = path + "//" + foundFileData.cFileName;
        FileType* type = NULL;

        if (strstr(foundFileData.cFileName, SPECIAL_TITLE)) {
            printf("Special type: %s\n", ext.c_str());
            type = new SpecialFileType();
        } else {
            //normal, header type
            type = new FileType();
        }

        if (type->init(full_path, ext) == false) {
            delete type;
            type = NULL;
            continue;
        }
        this->types[ext] = type;
    }
    FindClose(hFind);
    return types.size();
}

size_t MultiFileType::listDir(std::string path, std::string dir_extension)
{
    const std::string fileName = path + "\\*.*";
    WIN32_FIND_DATA foundFileData;

    HANDLE hFind = FindFirstFile(fileName.c_str(), &foundFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No such dir: %s\n", path.c_str());
        return 0;
    }

    FileType* type = NULL;

    while (FindNextFile(hFind, &foundFileData)) {
        if (foundFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            continue; //skip nested dir
        }
        std::string ext = get_extension(foundFileData.cFileName);
        if (ext != dir_extension) {
            printf("Wrong extension: %s dir: %s\n", ext.c_str(), dir_extension.c_str());
            continue;
        }

        std::string fullPath = path + "//" + foundFileData.cFileName;
        FileType* type = new FileType();
#ifdef DEBUG
        printf("Adding to MULTI: %s\n", foundFileData.cFileName);
#endif
        if (type->init(fullPath, ext, this->offset, this->accuracy) == false) {
            delete type;
            type = NULL;
            continue;
        }
        this->defs.insert(type);
    }

    FindClose(hFind);
    return defs.size();
}
