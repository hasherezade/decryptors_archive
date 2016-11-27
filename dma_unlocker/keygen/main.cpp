#include "DmaKeygen.h"
#define MIN_ARG 3

int main (int argc, char *argv[])
{
    if (argc < MIN_ARG) {
        printf("Usage: <seed> <key_number>");
    }
    size_t arg_num = 2;

    uint32_t seed = 0;
    if (argc >= arg_num) {
        sscanf(argv[arg_num - 1],"%d", &seed);
        printf("SEED   = %d = %#x\n", seed, seed);
    }
    arg_num++;

    uint32_t key_number = 0;
    if (argc >= arg_num) {
        sscanf(argv[arg_num - 1],"%d", &key_number);
        printf("KEY_NUMBER   = %d = %#x\n", key_number, key_number);
    }
    arg_num++;

    DmaKeygen keygen(seed);
    std::string key = keygen.makeKeyNumber(key_number);
    printf("%s\n", key.c_str());

    key = keygen.makeKeyNumber(key_number + 1);
    printf("%s\n", key.c_str());

    system("pause");
    return 0;
}