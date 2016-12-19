#include "PathCrawler.h"
#include "decryptor.h"

#include <stdio.h>

std::wstring PathCrawler::getLocalDir()
{
    wchar_t current_dir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, current_dir);
    return current_dir;
}

bool PathCrawler::isDecrypted(std::wstring filename, wchar_t *extension)
{
    wchar_t orig_name[MAX_PATH];
    wchar_t* filename_str = const_cast<wchar_t*>(filename.c_str());
    get_original_name(filename_str, extension, orig_name);
    if (is_existing(orig_name)) {
        return true; 
    }
    return false; 
}

bool PathCrawler::isInfected(std::wstring filename, wchar_t* extension)
{
    wchar_t* filename_str = const_cast<wchar_t*>(filename.c_str());
    return is_infected(filename_str, extension);
}

void PathCrawler::listDir(std::wstring path, wchar_t* extension, wchar_t* key_str, bool skip_decrypted)
{
    if (path.length() == 0) {
        path = PathCrawler::getLocalDir();
    }
    printf("Dir: %S\n", path.c_str());
    const std::wstring fileName =  path + L"\\*";
    WIN32_FIND_DATA foundFileData;
    HANDLE hFind = FindFirstFile(fileName.c_str(), &foundFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("[ERROR] INVALID_HANDLE_VALUE\n");
        return;
    }
    
    do {
        if (foundFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY) {
            if (wcscmp(foundFileData.cFileName, L".") == 0) continue;
            if (wcscmp(foundFileData.cFileName, L"..") == 0) continue;
            std::wstring full_path = path + L"\\"+ foundFileData.cFileName;
            listDir(full_path, extension, key_str, skip_decrypted);
            continue;
        }
        std::wstring full_path = path + L"\\"+ foundFileData.cFileName;
        const wchar_t* full_path_str = full_path.c_str();

        if (isInfected(full_path_str, extension) == false) {
            //printf("Not infected, skipping: %S\n", full_path_str);
            continue;
        }
        if (skip_decrypted && isDecrypted(full_path_str, extension) == true) {
            printf("Already decrypted, skipping: %S\n", full_path.c_str());
            continue;
        }
        wchar_t orig_name[MAX_PATH];
        get_original_name(full_path.c_str(), extension, orig_name);

        if (!aes_decrypt_file(full_path_str, orig_name, key_str)) {
            printf("[-] Failed to decrypt: %S\n", full_path_str);
        } else {
            printf("[+] %S\n", full_path_str);
        }
    } while (FindNextFile(hFind, &foundFileData));

    FindClose(hFind);
}
