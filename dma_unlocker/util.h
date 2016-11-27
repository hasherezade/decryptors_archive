#pragma once
#include <string>
#include <algorithm>

inline std::string get_extension(std::string filename)
{
    char* extension = strrchr(const_cast<char*>(filename.c_str()), '.');
    if (extension == NULL) return "";
    std::string ext = extension;
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}
/*
void hex_dump(unsigned char buffer[], size_t buf_size)
{
    for (size_t i = 0; i < buf_size; i++) {
        printf("%02x ", buffer[i]);
    }
    printf("\n");
}
*/