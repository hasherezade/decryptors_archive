#include "keygen.h"

#include "../types.h"
#include "../Params.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>     /* memset */

const int KEY_LEN = 32;

inline unsigned int random_chunk(int div, int add, char* buf)
{
    int r = rand();
    int b = r;
    int result = r / div;
    (*buf) = (b % div) + add;
    return result;
}

int make_random_key_v1(char *buffer, size_t buffer_size)
{
    signed int index = 0;
    int v2, v3, result;
    v2 = v3 = result = 0;
    
    memset(buffer, 0, sizeof(buffer_size));
    char *pBuf = &buffer[0];
    
    do {
        v2 = rand();
        v3 = v2 % 100 + 1;
        if ((unsigned int) (v2 % 100 - 25) <= 0x17) {
            result = random_chunk(10, 0x30, pBuf++);
            continue;
        }
        if (v3 < 25) {
            result = random_chunk(25, 0x61, pBuf++);
            continue;
        }
        if (v3 <= 50) {
            result = random_chunk(10, 0x30, pBuf++);
        } else {
            result = random_chunk(25, 0x41, pBuf++);
        }
    } while (++index < KEY_LEN);
    
    buffer[KEY_LEN] = '\0';
    return result;
}

int make_random_key_v2(char *buffer, size_t buffer_size)
{
    if (buffer_size < KEY_LEN) {
        printf("Wrong key buffer size!\n");
        return 0;
    }
    int index = 0;
    for (index = 0; index < KEY_LEN; index++) {
        uint8_t key_char = static_cast<uint8_t>((rand() % 254) + 1);
        buffer[index] = key_char;
    }
    buffer[KEY_LEN] = '\0';
    return 1;
}

int make_random_key(char *buffer, size_t buffer_size)
{
#ifdef VARIANT_2
    return make_random_key_v2(buffer, buffer_size);
#else
    return make_random_key_v1(buffer, buffer_size);
#endif  
}