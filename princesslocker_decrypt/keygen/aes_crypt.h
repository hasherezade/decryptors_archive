#pragma once

#include <Windows.h>
#include <wincrypt.h>
#include <stdio.h>
#pragma comment(lib, "crypt32.lib")

#define BLOCK_LEN 128


size_t read_from_file(LPWSTR filename, BYTE *buffer, size_t buffer_size);
BOOL aes_decrypt(BYTE *inbuf, BYTE *outbuf, size_t buf_size, LPCWSTR key_str, size_t key_len);
BOOL aes_decrypt_file(LPWSTR filename, LPWSTR filename2, LPCWSTR key_str, size_t key_len);