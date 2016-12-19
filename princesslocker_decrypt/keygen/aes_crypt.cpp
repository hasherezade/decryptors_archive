#include "aes_crypt.h"

size_t read_from_file(LPWSTR filename, BYTE *buffer, size_t buffer_size)
{
    HANDLE hInpFile = CreateFileW(filename, GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,NULL);        

    if (hInpFile == NULL) {
        printf("Cannot open input file!\n");
        return false;
    }

    DWORD read = 0;
    ReadFile(hInpFile, buffer, buffer_size, &read, NULL);
    CloseHandle(hInpFile);
    return read;
}

BOOL aes_decrypt(BYTE *inbuf, BYTE *outbuf, size_t buf_size, LPCWSTR key_str, size_t key_len)
{
    if (inbuf == NULL || outbuf == NULL) return FALSE;

    BOOL dwStatus = FALSE;

    BOOL bResult = FALSE;
    wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
    HCRYPTPROV hProv;
    if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }
    HCRYPTHASH hHash;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)){
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

    DWORD read = 0;
    DWORD written = 0;

    DWORD ciphertextLen = BLOCK_LEN;

    memcpy(outbuf, inbuf, chunk_size);

    if (CryptDecrypt(hKey, NULL, FALSE, 0,outbuf, &ciphertextLen)) {
        //printf ("[+] crypt OK!\n");
        dwStatus = TRUE;
    }

    CryptReleaseContext(hProv, 0);
    CryptDestroyKey(hKey);
    CryptDestroyHash(hHash);

    return dwStatus;
}

BOOL aes_decrypt_file(LPWSTR filename, LPWSTR filename2, LPCWSTR key_str, size_t key_len)
{
    if (filename == NULL || filename2 == NULL) return FALSE;

    BOOL dwStatus = FALSE;
    printf("Key: %S\n", key_str);
    printf("Key len: %#x\n", key_len);
    printf("Input File: %S\n", filename);
    printf("Output File: %S\n", filename2);
    printf("----\n");

    wchar_t info[] = L"Microsoft Enhanced RSA and AES Cryptographic Provider";
    HCRYPTPROV hProv;
    if (!CryptAcquireContextW(&hProv, NULL, info, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)){
        dwStatus = GetLastError();
        printf("CryptAcquireContext failed: %x\n", dwStatus);
        CryptReleaseContext(hProv, 0);
        return dwStatus;
    }
    HCRYPTHASH hHash;
    if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)){
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

    if (hInpFile == NULL) {
        printf("Cannot open input file!\n");
        return dwStatus;
    }
    if (hOutFile == NULL) {
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
