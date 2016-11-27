#pragma once

#include "types.h"

#include <windows.h>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <time.h>       /* time */
#include <string>

#define DAY_LEN 86400
#define HOUR_LEN 3600

//----
time_t FileTime_to_POSIX(FILETIME ft);
void print_time_str(time_t rawtime);
time_t get_file_modification(std::string filename);
time_t get_file_creation(std::string filename);

uint32_t print_time_info(std::string path, uint32_t fileModTime);