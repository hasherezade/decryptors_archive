#include "decryptor.h"
#include "PathCrawler.h"

#define VERSION "0.2.5"

bool make_outfilename(IN const wchar_t *filename1, IN const wchar_t *extension,  OUT wchar_t *out_filename)
{
    if (filename1 == NULL || out_filename == NULL) {
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

void banner()
{
    printf("-------------------------------------------------------------------\n");
    printf("PrincessLocker Decryptor v%s\n", VERSION);
    printf("Questions? Remarks? Contact the author: hasherezade@gmail.com\n");
    printf("-------------------------------------------------------------------\n");
}

//params: <key> <extension> [directory/file]
int wmain(int argc, wchar_t *argv[])
{
    wchar_t *dirname = NULL; //input file
    wchar_t *key_str = NULL;
    wchar_t *extension = NULL;

    banner();

    if (argc < 3) {
        printf("Expected params: <key> <extension> [file/directory name] \n");
        system("pause");
		return (-1);
    } else {
        key_str = argv[1];
        extension = argv[2];
        if (argc >= 4) {
            dirname = argv[3];
        }
    }
    printf("Key: %S\n", key_str);
    printf("Ransom extension: %S\n", extension);
    printf("----\n");

    if (dirname == NULL) {
        PathCrawler::listDir(L"", extension, key_str, true);
        system("pause");
        return 0;
    }
    
    if (is_directory(dirname)) {
        PathCrawler::listDir(dirname, extension, key_str, true);
        system("pause");
        return 0;
    }

    wchar_t filename2[MAX_PATH];
    if (!make_outfilename(dirname, extension, filename2)) {
        printf("Coud not make output filename\n");
        system("pause");
        return -1;
    }
    printf("Input File: %S\n", dirname);
    printf("Output File: %S\n", filename2);
    printf("----\n");

    if (aes_decrypt_file(dirname, filename2, key_str)) {
        printf("[+] Decrypted! Check the output: %S\n", filename2);
    } else {
        printf("[-] Decrypting failed!\n");
    }
    system("pause");
    return 0;
}