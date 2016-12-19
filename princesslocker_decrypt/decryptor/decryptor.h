#pragma once

#include <Windows.h>
#include <wincrypt.h>
#include <stdio.h>
#pragma comment(lib, "crypt32.lib")

#define BLOCK_LEN 128

wchar_t* get_extension_ptr(IN wchar_t *filename);
bool is_infected(wchar_t* filename, wchar_t* extension);
bool is_existing(IN const wchar_t *filename);
bool is_directory(IN const wchar_t *filename);
bool get_original_name(IN const wchar_t *filename1, IN const wchar_t *extension, OUT wchar_t *out_filename);

BOOL aes_decrypt_file(const wchar_t* filename, const wchar_t* filename2, const wchar_t* key_str);