#include "decryptor.h"

wchar_t* get_extension_ptr(IN wchar_t *filename)
{
    size_t len = lstrlenW(filename);
    for (int i = len - 1; i > 0; i--) {
        if (filename[i] == '.') {
            return &filename[i];
        }        
    }
    return NULL;
}

bool is_infected(wchar_t* filename, wchar_t* extension)
{
    wchar_t* ptr = get_extension_ptr(filename);
    if (ptr == NULL) return false;
    if (*ptr == L'.') ptr++;

    if (wcscmp(ptr, extension) == 0) {
        return true; // file have the defined extension
    }
    return false;
}

bool is_existing(IN const wchar_t *filename)
{
    if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(filename) && GetLastError()==ERROR_FILE_NOT_FOUND) {
        return false;
    }
    return true;
}

bool is_directory(IN const wchar_t *filename)
{
    DWORD attributes = GetFileAttributes(filename);
    if(INVALID_FILE_ATTRIBUTES == attributes && GetLastError()==ERROR_FILE_NOT_FOUND) {
        return false;
    }
    if (attributes == FILE_ATTRIBUTE_DIRECTORY) {
        return true;
    }
    return false;
}

bool get_original_name(IN const wchar_t *filename1, IN const wchar_t *extension, OUT wchar_t *out_filename)
{
    if (filename1 == NULL || extension == NULL || out_filename == NULL) {
        return false;
    }
    wcscpy(out_filename, filename1);

    wchar_t *ptr = wcsstr(out_filename, extension);
    if (ptr == NULL) {
        return false;
    }
    ptr--;
    if (*(ptr) != '.') {
        return false;
    }
    (*ptr) = 0;
    return true;
}

BOOL aes_decrypt_file(const wchar_t* filename, const wchar_t* filename2, const wchar_t* key_str)
{
    if (filename == NULL || filename2 == NULL || key_str == NULL) return FALSE;

    size_t key_len = lstrlenW(key_str);
    BOOL dwStatus = FALSE;

    wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
    HCRYPTPROV hProv;
    if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }
    HCRYPTHASH hHash;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
        dwStatus = GetLastError();
        printf("CryptCreateHash failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }

    if (!CryptHashData(hHash, (BYTE*)key_str, key_len, 0)) {
        DWORD err = GetLastError();
        printf ("CryptHashData Failed : %#x\n", err);
        return dwStatus;
    }
    HCRYPTKEY hKey;
    if (!CryptDeriveKey(hProv, CALG_AES_128, hHash, 0,&hKey)){
        dwStatus = GetLastError();
        printf("CryptDeriveKey failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }
    
    const size_t chunk_size = BLOCK_LEN;
    BYTE chunk[chunk_size];
    DWORD read = 0;
    DWORD written = 0;

    HANDLE hInpFile = CreateFileW(filename, GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);
    HANDLE hOutFile = CreateFileW(filename2, GENERIC_WRITE, 0,  NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);          

    if (hInpFile == INVALID_HANDLE_VALUE) {
        printf("Cannot open input file!\n");
        return dwStatus;
    }
    if (hOutFile == INVALID_HANDLE_VALUE) {
        printf("Cannot open output file!\n");
        return dwStatus;
    }
    while (ReadFile(hInpFile, chunk, chunk_size, &read, NULL)) {
        if (0 == read){
            break;
        }
        DWORD ciphertextLen = BLOCK_LEN;
        if (!CryptDecrypt(hKey, NULL, FALSE, 0,chunk, &ciphertextLen)) {
            printf("failed!\n");
            dwStatus = FALSE;
            break;
        } else {
            dwStatus = TRUE;
        }
        if (!WriteFile(hOutFile, chunk, ciphertextLen, &written, NULL)) {
            printf("writing failed!\n");
            break;
        }
        memset(chunk, 0, chunk_size);
    }

    CryptReleaseContext(hProv, 0);
    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);

    CloseHandle(hInpFile);
    CloseHandle(hOutFile);
    return dwStatus;
}
