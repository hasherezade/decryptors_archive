#pragma once

#include <windows.h>
#include <stdio.h>
#include <string>

namespace PathCrawler
{
    std::wstring getLocalDir();
        
    void listDir(std::wstring path, wchar_t* extension, wchar_t* key_str, bool skip_decrypted);
    bool isInfected(std::wstring filename, wchar_t* extension);
    bool isDecrypted(std::wstring filename1, wchar_t *extension);

    bool decryptFile();
};