#include "../types.h"
#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "../Params.h"
#include "../recon/PathCrawler.h"
#include "../decryptor/decryptor.h"
#include "../filetypes/FileTypes.h"

#include "CrackedSet.h"
#define MIN_ARG 2
//----

int main (int argc, char *argv[])
{
    printf("---------------------------\n");
    printf("DMA CRAWLER v0.2, by hasherezade\n");
    printf("---------------------------\n");

    printf("Params:\n <input>\n optional:[start_time] [increment_mode] [series][file_mod_time]\n\n");
    printf(" start_time (optional) : timestamp from which the search should start\n");
    printf(" increment_mode (optional) : 1 - increment start_time ; 0 - decrement start_time\n");
    printf(" series : how many keys to generate per single timestamp (max)\n");
    printf(" file_mod_time (optional) : file modification timestamp (in case if the saved one is invalid)\n");
    printf("---------------------------\n");

    bool isIncrementalMode = false;
    int seed = 0;
    uint32_t mod_timestamp = 0;
    size_t max_series = 2000000; //TODO: calculate dynamically

    FileTypesSet fileTypes;
    if (fileTypes.listDir(HDRS_DIR) == 0) {
        printf("[ERROR] Cannot load supported extensions!\n");
    }

    size_t arg_num = MIN_ARG;
    if (argc >= arg_num) {
        sscanf(argv[arg_num - 1],"%d", &seed);
        printf("SEED   = %d = %#x\n", seed, seed);
    }
    arg_num++;

    if (argc >= arg_num) {
        int mode = 0;
        sscanf(argv[arg_num - 1],"%d", &mode);
        isIncrementalMode = (mode >= 1);
        printf("is INC MODE_ = %d\n", isIncrementalMode);
    }
    arg_num++;
   
    if (argc >= arg_num) {
        sscanf(argv[arg_num - 1],"%d", &max_series);
        printf("MAX_SERIES   = %d\n", max_series);
    }

    arg_num++;
    if (argc >= arg_num) {
        sscanf(argv[arg_num - 1],"%d", &mod_timestamp);
        printf("overwriting FILEMODTIME   = %d\n", mod_timestamp);
    }

    //max number of encrypted files per milisecons
    int infected_per_ms = 50; //todo: calculate this!
    int upper_search_bound = max_series;

    Params params;
    params.seed = seed;
    params.series_max = max_series;
    params.series_min = infected_per_ms;
    params.fileEncryptionTime = mod_timestamp; //if there is a need to overwrite
    params.incrementalMode = isIncrementalMode;

    CrackedSet fileSet(fileTypes, params);
    fileSet.init();
    PathCrawler crawler;
    std::string start_path = PathCrawler::getLocalDir();
    printf("Start path: %s\n", start_path.c_str());
    crawler.listDir(start_path, &fileSet, true);
    printf("Summary:\n");
    fileSet.printSummary();
    printf("FINISHED!\n");

    system("pause");
    return 0;
}