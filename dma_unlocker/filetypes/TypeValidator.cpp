#include "TypeValidator.h"


bool TypeValidator::read_header(std::string filename, unsigned char buf[], size_t offset)
{
    FILE *fp = fopen(filename.c_str(),"rb");
    if (!fp) {
        printf("Cannot open");
        return false;
    }
    fseek(fp, offset, SEEK_SET);
    fread(buf, 1, DmaDecryptor::UNIT_LEN, fp);
    fclose(fp);
#ifdef DEBUG
    printf("File:\t%s\n", filename);
#endif
    return true;
}

size_t TypeValidator::get_original_file_size(std::string filename, size_t offset)
{
    FILE *fp = fopen(filename.c_str(),"rb");
    if (!fp) {
        printf("Cannot open");
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fclose(fp);
    if (size < offset) return 0;
    return size - offset;
}
//---
bool TypeValidator::init(std::string enc_filename, FileType* my_type)
{
    if (my_type == NULL) return false;
    this->my_type = my_type;

    memset(enc_buf, 0, sizeof(enc_buf));
    //read header:
    
    size_t read_offset = my_type->offset / FileType::CHUNK_SIZE;
    const size_t total_offset = DmaDecryptor::OFFSET + read_offset;

    if (read_header(enc_filename, enc_buf, total_offset) == false) {
        printf("Opening encrypted file failed!\n");
        return false;
    }
    enc_buf[DmaDecryptor::UNIT_LEN] = '\0';
    isInit = true;
    return true;
}

bool TypeValidator::testKey(std::string key)
{
    if (!isInit) return false;
    if (key == "" || enc_buf == NULL || my_type == NULL) {
        printf("[ERROR] NULL buffer supplied to test_key!!!\n");
        return false;
    }

    unsigned char aesKey[DmaDecryptor::KEY_LEN + 1];
    memcpy(aesKey, key.c_str(), DmaDecryptor::KEY_LEN);
    aesKey[DmaDecryptor::KEY_LEN] ='\0';

    memcpy(buf, enc_buf, DmaDecryptor::UNIT_LEN);
    DmaDecryptor::decryptChunk(buf, aesKey);
    return isDecodedValid(buf);
}

///-----

bool HeaderValidator::isDecodedValid(unsigned char *enc_buf)
{
    return isDecodedValid(enc_buf, this->my_type);
}

bool HeaderValidator::isDecodedValid(unsigned char *enc_buf, FileType *my_type)
{
    if (enc_buf == NULL) {
        printf("[ERROR] NULL buffer supplied to test_key!!!\n");
        return false;
    }    
    size_t accuracy = my_type->getAccuracy();
    size_t match_offset = my_type->getMatchOffset();
    if (accuracy == 0 || accuracy > FileType::CHUNK_SIZE) {
        accuracy = FileType::CHUNK_SIZE;
    }
    if (memcmp(enc_buf + match_offset, my_type->buffer, accuracy) == 0) {
        return true;
    }
    return false;
}

//----
bool BMPValidator::init(std::string enc_filename, FileType* my_type)
{
    if (!TypeValidator::init(enc_filename, my_type)) return false;
    this->original_size = get_original_file_size(enc_filename, DmaDecryptor::OFFSET);
    printf("\nBMP: original file size: %x = %d\n", original_size, original_size);
    return true;
}

bool BMPValidator::isDecodedValid(unsigned char *enc_buf)
{
    if (enc_buf == NULL) {
        printf("[ERROR] NULL buffer supplied to test_key!!!\n");
        return false;
    }    
    size_t size_offset = 2;
    if (memcmp(enc_buf, "BM", size_offset) != 0) {
        return false;
    }
    uint32_t size;
    memcpy(&size, enc_buf + size_offset, sizeof(uint32_t));
    if (size == this->original_size) return true;

    return false;
}

bool MultiValidator::isDecodedValid(unsigned char *enc_buf)
{
    if (my_type == NULL) {
        printf("Wrong type definition!\n");
        return false;
    }
    std::set<FileType*>::iterator itr;
    for (itr = my_type->defs.begin(); itr != my_type->defs.end(); itr++) {
        FileType* def = *itr;
        
        if (HeaderValidator::isDecodedValid(enc_buf, def)) return true;
    }
    return false;
}