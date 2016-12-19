#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include <wchar.h>

#include "aes_crypt.h"

#define VERSION "0.2"

#define CHARSET "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define MIN_KEY 0x15
#define MAX_KEY 0x20

wchar_t* make_key(size_t len, size_t seed, bool is_key = true)
{
	wchar_t *key_buf = (wchar_t*) calloc(len + 1, sizeof(wchar_t));
	if (key_buf == NULL) return NULL;

	char charset[] = CHARSET;
	size_t charset_len = strlen(charset);

	srand(seed);
	for (int i = 0; i < len; i++) {
		size_t index = rand() % charset_len;
		key_buf[i] = charset[index];
        if (!is_key) {
            key_buf[i] = tolower(charset[index]);
        }
	}
	return key_buf;
}

DWORD find_seed(wchar_t* uid, DWORD start_seed, bool increment = true, bool is_key = true)
{
    size_t uid_len = lstrlenW(uid);

	const char charset[] = CHARSET;
	const size_t charset_len = strlen(charset);
    DWORD seed = start_seed;

    size_t pos = 0;
    while (true) {
        srand(seed);

        for (pos = 0;  pos < uid_len; pos++) {
            size_t index = rand() % charset_len;
            char rand_char = charset[index];
            if (!is_key) {
                rand_char = tolower(rand_char);
            }
            if (rand_char != uid[pos]) {
                break;
            }
        }
        if (pos < uid_len) {
            pos = 0;
            seed = (increment) ? seed + 1 : seed - 1;
        } else {
            return seed;
        }
    }
    return -1;
}

DWORD find_key_seed(wchar_t* uid, DWORD start_seed, bool increment = true)
{
    return find_seed(uid, start_seed, increment, true);
}

DWORD find_uid_seed(wchar_t* uid, DWORD start_seed, bool increment = true)
{
    return find_seed(uid, start_seed, increment, false);
}

DWORD find_start_seed(wchar_t *uid, wchar_t *ext, DWORD init_seed)
{
    DWORD seed = init_seed;
    DWORD uid_seed = 0;
    DWORD ext_seed = 0;

    //printf ("i: %#x\n", init_seed);
    if (uid) {
        uid_seed = find_uid_seed(uid, seed, false);
        //printf ("u: %#x\n", uid_seed);
    }
    if (!ext) return uid_seed;

    if (uid) {
        ext_seed = find_uid_seed(ext, uid_seed, true);
    } else {
        ext_seed = find_uid_seed(ext, seed, false);
    }
    //printf ("e: %#x\n", ext_seed);
    if (uid && ext_seed - uid_seed > 100) {
        printf("[WARNING] Inconsistency detected!\n");
    }
    if (uid) return uid_seed;
    return ext_seed; //lower quality recognition!
}

bool check_key(BYTE* inbuf, BYTE *expbuf, size_t check_size, wchar_t* key_str, size_t key_len)
{
    BYTE outbuf[BLOCK_LEN];

    if (!aes_decrypt(inbuf, outbuf, BLOCK_LEN, key_str, key_len)) {
        return false;
    }
    if (memcmp(outbuf, expbuf, check_size) == 0) {
        return true;
    }
    return false;
}

size_t read_buffer(LPWSTR filename, BYTE *buffer, size_t buffer_size)
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

bool is_existing(IN const wchar_t *filename)
{
    if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(filename) && GetLastError()==ERROR_FILE_NOT_FOUND) {
        return false;
    }
    return true;
}

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

bool make_outfilename(IN const wchar_t *filename1, IN const wchar_t *extension, OUT wchar_t *out_filename)
{
    if (filename1 == NULL || extension == NULL || out_filename == NULL) {
        return false;
    }
    if (!get_original_name(filename1, extension, out_filename)) {
        return false;
    }
    if (is_existing(out_filename)) {
        size_t len =  lstrlenW(out_filename);
        if (len < MAX_PATH) {
            out_filename[len] = '_';
            out_filename[len+1] = 0;
        }
    }
    return true;
}

bool dump_key(char* filename, wchar_t* key)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) return false;
    fprintf(fp,"%S\n", key);
    fclose(fp);
    return true;
}

size_t get_check_size(IN const wchar_t *filename1, IN const wchar_t *extension, IN const wchar_t *filename2)
{
    wchar_t orig_name[MAX_PATH];
    if (!get_original_name(filename1, extension, orig_name)) {
        printf("-");
        return false;
    }
    wchar_t *ext = get_extension_ptr(orig_name);
    if (ext == NULL) return false;

    if (wcscmp(filename2, orig_name) == 0) {
        return BLOCK_LEN;
    }
    printf("File used for validation: %S\n", filename2);
    DWORD len = 0;
    swscanf(filename2, L"%d", &len);
    return len;
}

wchar_t* find_key(IN wchar_t *filename1, IN wchar_t *filename2, size_t check_size, DWORD uid_seed, DWORD limit=100)
{
    bool increment = false;
    bool found = false;

    BYTE in_buf[BLOCK_LEN+1];
    memset(in_buf, 0, BLOCK_LEN+1);

    BYTE expected_buf[BLOCK_LEN+1];
    memset(expected_buf, 0, BLOCK_LEN+1);

    if (read_buffer(filename1, in_buf, BLOCK_LEN) == 0) {
        printf("Cannot read the file: %S\n", filename1);
        system("pause");
        return 0;
    }
    if (read_buffer(filename2, expected_buf, BLOCK_LEN) == 0) {
        printf("Cannot read the file: %S\n", filename2);;
        system("pause");
        return 0;
    }
    DWORD key_seed = uid_seed;
    size_t key_len = 0;
    wchar_t *key = NULL;
    printf(".");
    do {
        key = make_key(MAX_KEY, key_seed, true);
        for (key_len = MIN_KEY; key_len <= MAX_KEY; key_len++) {
            if (check_key(in_buf, expected_buf, check_size, key, key_len)) {
                printf("\nMatch found, accuracy %d/%d\n", check_size, BLOCK_LEN);
                key[key_len] = 0;
                found = true;
                break;
            }
        }
        if (!found) {
            limit--;
            key_seed = (increment) ? key_seed + 1 : key_seed - 1;
            free(key);
            key = NULL;
        }
    } while (!found && limit > 0);

 //---
    if (key) {
        printf("---\n");
        printf("%S\n", key);
        printf("---\n");
    }
    return key;
}

void banner()
{
    printf("-------------------------------------------------------------------\n");
    printf("PrincessLocker Keygen v%s\n", VERSION);
    printf("Questions? Remarks? Contact the author: hasherezade@gmail.com\n");
    printf("-------------------------------------------------------------------\n");
}

int wmain(int argc, wchar_t *argv[])
{
    wchar_t *filename1 = NULL;
    wchar_t *extension = NULL;
    wchar_t *unique_id = NULL;
    wchar_t *filename2 = NULL;
    banner();
    if (argc < 4) {
        printf("Expected params: <encrypted file> <decrypted file> <added extension> [unique id]\n");
        system("pause");
		return (-1);
    } else {
        filename1 = argv[1];
        filename2 = argv[2];
        extension = argv[3];
        if (argc < 5) {
            printf("[WARNING] If you have the ransom note, please supply your unique ID!\n");
        } else {
            unique_id = argv[4];
        }
    }

    wchar_t out_filename[MAX_PATH];
    memset(out_filename, 0, MAX_PATH);
    if (!make_outfilename(filename1, extension, out_filename)) {
        printf("Input file not matching the extension!\n");
        system("pause");
        return (-1);
    }

    size_t check_size = get_check_size(filename1, extension, filename2);
    printf("Setting check size: %d\n", check_size);
    if (check_size < 4) {
        printf("---ERROR---\n");
        printf("Invalid parameters:\n");
        printf("<decrypted file> must have a name in the following format <check_size>.<extension>\n");
        printf("Check size is the lenght of the header for particular format.\n Choose the format that has a header not shorter than 4 bytes.\n");
        system("pause");
        return (-1);
    }

    printf("Searching the key...");
    DWORD init_seed = time(NULL);
    wchar_t *key = NULL;
    size_t limit = 100;
    do {
        DWORD uid_seed = find_start_seed(unique_id, extension, init_seed);
        key = find_key(filename1, filename2, check_size, uid_seed, limit);
        init_seed = uid_seed - 1;
    } while (!key);

    size_t key_len = lstrlenW(key);
    BOOL res = aes_decrypt_file(filename1, out_filename, key,  key_len);
    if (res) {
        printf("[+] Finished successfuly.\n");
        printf("[+] Check if the output file is valid: \"%S\"\n", out_filename);
        printf("[+] Save your key: %S\n", key);

        char* key_filename = "princess_key.txt";
        dump_key(key_filename, key);
        printf("[+] Key dumped to: %s\n", key_filename);

    } else {
        printf("[ERROR] Decrypting your file failed!\n");
    }
    system("pause");
    return 0;
}
