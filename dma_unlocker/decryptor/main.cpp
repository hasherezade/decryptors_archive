#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "decryptor.h"
//----

bool decryptChunk(unsigned char *key, unsigned char *enc_buf)
{
    size_t chunk_len = 16;
    size_t key_len = 32;
    aes256_context ctx;
    memset(&ctx,0,sizeof(ctx));

    aes256_init(&ctx, key);
    aes256_decrypt_ecb(&ctx, enc_buf);
    aes256_done(&ctx);
   
    return false;
}

bool read_header(char *filename, unsigned char buf[], size_t offset)
{
    FILE *fp = fopen(filename,"rb");
    if (!fp) {
        printf("Cannot open");
        return false;
    }
    fseek(fp, offset, SEEK_SET);
    fread(buf, 1, 16, fp);
    fclose(fp);
    
    printf("File:\t%s\n", filename);

    return true;
}

#define DAY_LEN 86400

int main (int argc, char *argv[])
{
    printf("---------------------------\n");
    printf("DMA cracker v0.1\n");
    printf("---------------------------\n");
    if (argc < 3) {
        printf ("Expected: <filename> <AES_key>\n");
        system("pause");
        return -1;
    }
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    DmaDecryptor decr(argv[1], argv[2]);
    printf ("Init...\n");
    if (decr.init() == false) {
        printf ("Init failed\n");
        system("pause");
        return -1;
    }
    
    std::string ext = "";
    char *pos = strrchr(argv[1], '.');
    if (pos) {
        ext = pos;
        *(pos) = 0;
    }
    
    std::string outName = std::string(argv[1]) + "_dec" + ext;
    printf ("Decrypting...\n");
    if (decr.decrypt(outName) == false) {
        printf ("Init failed\n");
        system("pause");
        return -1;
    }
    printf ("Decrypted\n");
    printf ("ok!\n");
    system("pause");
    return 0;
}