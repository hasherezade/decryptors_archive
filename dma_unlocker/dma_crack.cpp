#include "dma_crack.h"
#include "keygen/keygen.h"
#include "decryptor/aes256.h"
#include "decryptor/decryptor.h"
#include "util.h"
#include "filetypes/TypeValidator.h"

//#define DEBUG 0

void log_key(uint32_t seed, char *key, int key_num, std::string filename)
{
    FILE* f_out = fopen("key_out.txt", "a");
    if (!f_out) return;
    fprintf (f_out, "File: %s\n", filename.c_str());
    fprintf (f_out, "KEY: %s\n", key);
    fprintf (f_out, "INIT %x = %d (num: %d)\n", seed, seed, key_num);
    fprintf (f_out, "---\n");
    fflush(f_out);
    fclose(f_out);
}

std::string search_key(Params &params, 
                TypeValidator *validator, 
                size_t series_min, 
                size_t series_max
                )
{
    if (validator == NULL) {
        printf("Validator not found!\n");
        return "";
    }

    size_t days = 0;
    int day_start = params.seed;
    int deadline = 2; //2 days max
    //---
    int seed = params.seed;
    char key[0x100];

    printf("Searching key started...\n");
#ifdef DEBUG
    printf("Start seed: %d = %#x\n----------\n", seed, seed);
    print_time_str(seed);
#endif
    
    if (series_min != series_max) {
#ifdef DEBUG
        printf("Smart search mode: ON!\nWarning: it works only if the file have a valid modification timestamp!\n");
        printf("Series min = %d , max = %d\n----------\n", series_min, series_max);
#endif
    } else {
#ifdef DEBUG
        printf("Smart search mode: OFF!\n");
        printf("Series min = %d , max = %d\n----------\n", series_min, series_max);
#endif
    }
    size_t series = series_min;

    while (deadline > 0) {
        srand (seed);
        for (size_t key_num = 0; key_num < series; key_num++) {
            make_random_key(key, sizeof(key));
            if (validator->testKey(key)) {
                if (validator->getAccuracy() >= PIVOT_MIN) {
#ifdef DEBUG
                    printf("Adjusting seed to to found one!\n");
#endif
                    params.seed = seed;
                    params.key_num = key_num;
                }
                printf(">> KEY FOUND: %s\n", key);
                printf("[SUCCESS]\n");
#ifdef DEBUG
                printf ("KEY: %s\nSEED %x = %d\nkey number in series: %d\n", key, seed, seed, key_num);
#endif
                log_key(seed, key, key_num, params.filename);
                return key;
            }
        }
        if (params.incrementalMode) {
            seed++;
        } else {
            seed--;
            if (series < series_max) {
                //max number of encrypted files per milisecons
                series += series_min;
            }
        }
        if (abs(day_start - seed) > DAY_LEN) {
            day_start = seed;
            days++;
            deadline--;
            printf("%d day passed!\n", days);
        }
    }
    return "";
}

std::string search_key_in_series(uint32_t seed, TypeValidator *validator,  unsigned char *enc_buf)
{
    if (validator == NULL) {
        printf("Validator not found!\n");
        return "";
    }
    //single series without incrementing seed
    char key[0x100];

    printf("Searching key started...\n");
#ifdef DEBUG
    printf("Start seed: %d = %#x\n----------\n", seed, seed);
    print_time_str(seed);
#endif
    srand (seed);
    
    for (size_t key_num = 0; key_num < 10000000; key_num++) {
        make_random_key(key, sizeof(key));

        if (validator->testKey(key)) {
            printf("> KEY FOUND: %s\n", key);
            printf("[SUCCESS]\n");
#ifdef DEBUG
            printf ("KEY: %s\nSEED %x = %d\nkey number in series: %d\n", key, seed, seed, key_num);
#endif
            //log_key(seed, key, key_num, params.filename);
            return key;
        }
    }
    return "";
}

TypeValidator* makeValidator(FileType *my_type)
{
    if (my_type == NULL) return NULL;

    if (my_type->group == T_GROUP::HEADER) {
#ifdef DEBUG
        printf("Making Header validator\n");
#endif
        return new HeaderValidator();
    }

    if (my_type->group == T_GROUP::MULTI) {
#ifdef DEBUG
        printf("Making MULTI validator\n");
#endif
        return new MultiValidator();
    }

    if (my_type->extension == ".bmp") {
#ifdef DEBUG
        printf ("BMP: Special validator required!\n");
#endif
        return new BMPValidator();
    }
    return NULL;
}

bool decrypt_file(FileTypesSet &fileTypes, Params &params, std::string enc_filename)
{
    //get extension:
    std::string extension = get_extension(enc_filename);
    FileType *my_type =  fileTypes.getType(extension);

    if (my_type == NULL) {
        printf("[ERROR] Not supported file type! Add a header for %s into '%s' folder!\n", extension.c_str(), HDRS_DIR);
        return false;
    }
    TypeValidator *validator = makeValidator(my_type);
    if (validator == NULL) {
        printf("[ERROR] Not suppeoted File Type: %s\n", extension.c_str());
        return false;
    }
    validator->init(enc_filename, my_type);

    printf("Extension: %s\n", extension.c_str());
    //crack it!
    bool isDecrypted = false;
    std::string key = search_key(params, validator, params.series_min, params.series_max);
    if (key.length() == DmaDecryptor::KEY_LEN ) {
        DmaDecryptor decr(enc_filename, key);
        decr.init();
        isDecrypted = decr.decrypt(DmaDecryptor::makeOutName(enc_filename.c_str()));
    }
    delete validator;
    return isDecrypted;
}

bool decrypt_file(FileTypesSet &fileTypes, Params &params, std::string enc_filename, std::string &key)
{
    //get extension:
    std::string extension = get_extension(enc_filename);
    FileType *my_type =  fileTypes.getType(extension);
    if (my_type == NULL) {
        printf("[ERROR] Not supported file type! Add a header for %s into '%s' folder!\n", extension.c_str(), HDRS_DIR);
        return false;
    }
    TypeValidator *validator = makeValidator(my_type);
    if (validator == NULL) {
        printf("[ERROR] Not suppeoted File Type\n");
        return false;
    }
    validator->init(enc_filename, my_type);

    //crack it!
    bool isDecrypted = false;
    if (validator->testKey(key)) {
        DmaDecryptor decr(enc_filename, key);
        decr.init();
        isDecrypted = decr.decrypt(DmaDecryptor::makeOutName(enc_filename.c_str()));
    }
    return isDecrypted;
}
