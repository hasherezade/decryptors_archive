#pragma once

#include "../types.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string.h>     /* memset */

//inline unsigned int random_chunk(int div, int add, char* buf);
int make_random_key(char *buffer, size_t buffer_size);