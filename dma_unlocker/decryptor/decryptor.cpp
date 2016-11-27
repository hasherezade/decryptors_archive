#include "decryptor.h"

#define CHUNK_NUM 45
#define MAX_ENCRYPTED 1000000000

void DmaDecryptor::decryptChunk(unsigned char enc_buf[], unsigned char key[])
{
    aes256_context ctx;
    memset(&ctx, 0, sizeof(ctx));

    aes256_init(&ctx, key);
    aes256_decrypt_ecb(&ctx, enc_buf);
    aes256_done(&ctx);
}

bool DmaDecryptor::_decrypt(unsigned char buf[], size_t bufSize, unsigned char key[])
{
    aes256_context ctx;
    memset(&ctx,0,sizeof(ctx));
    aes256_init(&ctx, key);

    size_t iters = bufSize / UNIT_LEN;
    size_t i = 0;

    unsigned char* buf_ptr = this->buffer;

    for (i = 0 ; i < bufSize / UNIT_LEN ; i++) {
        aes256_decrypt_ecb(&ctx, buf_ptr);
        buf_ptr += UNIT_LEN;
    }
    size_t rem = (buffer + bufSize) - buf_ptr;

    aes256_done(&ctx);
    return true;
}

//-------

bool DmaDecryptor::init()
{
    FILE *pFile = fopen(this->filename.c_str(), "rb");
    if (!pFile) return false;

    if (DmaDecryptor::isInfected(pFile) == false) {
        printf("It is not infected file!\n");
        fclose(pFile);
        return false;
    }

    fseek(pFile, 0, SEEK_END);
    this->fileSize = ftell(pFile);
    fclose(pFile);

    if (this->fileSize <= OFFSET) {
        printf("Invalid File Size\n");
        return false;
    }
    this->fileSize -= OFFSET;
    this->encryptedSize = (this->fileSize > MAX_ENCRYPTED) ? MAX_ENCRYPTED : this->fileSize;

    this->chunkSize = (this->encryptedSize / CHUNK_NUM);
    this->lastChunkSize = this->encryptedSize - (chunkSize * CHUNK_NUM);
    this->chunkNum = (chunkSize == 0) ? 0 : CHUNK_NUM;
 
    this->buffer = static_cast<unsigned char*> (calloc(this->chunkSize, 1));

    memcpy(aesKey,this->key.c_str(), KEY_LEN);
    aesKey[KEY_LEN] ='\0';

    return true;
}

bool DmaDecryptor::isInfected(FILE *fp)
{
    char buf[10];
    fread(buf, 1, 8, fp);

    if (memcmp(buf, "!DMALOCK", 8) == 0) {
        return true;
    }
    return false;
}

std::string DmaDecryptor::makeOutName(const char *inFile)
{
    std::string ext = "";
    char *pos = strrchr(const_cast<char*>(inFile), '.');
    if (pos) {
        ext = pos;
        *(pos) = 0;
    }
    return std::string(inFile) + "_decrypted" + ext;
}

bool DmaDecryptor::decrypt(std::string outFilename)
{
    FILE *inFile = fopen(this->filename.c_str(), "rb");
    if (!inFile)  {
        printf("Cannot open IN file: %s\n", filename.c_str());
        return false;
    }

    fseek(inFile, OFFSET, SEEK_SET);

    FILE *outFile = fopen(outFilename.c_str(), "wb");
    if (!outFile) {
        printf("Cannot open OUT file\n");
        fclose(inFile);
        return false;
    }

    for (size_t i = 0; i < chunkNum; i++) {

        if (fread(this->buffer, 1, this->chunkSize, inFile) == 0) {
            return false;
        }
        if (_decrypt(this->buffer, this->chunkSize, this->aesKey) == false) {
            return false;
        }
        fwrite(this->buffer, 1, this->chunkSize, outFile);
        fflush(outFile);
    }

    if (this->lastChunkSize > 0) {
        if (fread(this->buffer, 1, this->lastChunkSize, inFile) == 0) {
            return false;
        }
        if (_decrypt(this->buffer, this->lastChunkSize, this->aesKey) == false) {
            return false;
        }
        fwrite(this->buffer, 1, this->lastChunkSize, outFile);
        fflush(outFile);
    }
    //copy the remaining, unencrypted content:
    int dif = this->fileSize - this->encryptedSize;
    while (dif > 0) {
        size_t count = fread(this->buffer, 1, this->chunkSize, inFile);
        if (count == 0) break;
        fwrite(this->buffer, 1, count, outFile);
        fflush(outFile);
        dif -= count;
    }

    fclose(inFile);
    fclose(outFile);

    printf("Saved to file: %s\n", outFilename.c_str());
    return true;
}
