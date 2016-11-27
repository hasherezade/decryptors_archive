#include "Params.h"

const char Params::id_hash[] = "qWyYGuCxIkhne4XjTyli7ApK761u0xfG9exLhPteuj/bwJxWQzcmh9Zn56dcqnLX";

//when it all started?
uint32_t Params::encryptionStartTime()
{
    time_t start = get_file_creation("C:\\ProgramData\\start.txt");
    if (start == 0) {
        start = get_file_creation("C:\\ProgramData\\cryptinfo.txt");
    }
    //---
    if (start > 0) {
        printf("Infections start time:\n");
        print_time_str(start);
    } else {
        printf("Infections start time: UNKNOWN\n");
    }
    return static_cast<uint32_t>(start);
}

