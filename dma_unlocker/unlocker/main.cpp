#include "../types.h"
#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "Unlocker.h"

#define ARG_MIN 2
#define SERIES_MAX  1000000
#define INFECTED_PER_MS 65

#define VERSION "0.0.6.5"
//----

void disclaimer_banner()
{
    printf("-----------------------------------------------------------------\n");
    printf("!!!!DISCLAIMER!!!!\n\
This tool is an experiment in unlocking a particular kind of Ransomware,\n\
neither Malwarebytes or Hasherezade promise this tool will unlock or decrypt\n\
your files.\n\
This tool should not be considered an official solution to the DMA Locker \nproblem.\n\
Any files destroyed, further encrypted or otherwise tampered with against\n\
the desire of the user are not the responsibility of the developers.\n\
Please use at your own risk.\n\n");
    printf("If you cannot accept this, please exit the program now.\n");
    printf("If you accept the above, press any button to start the program\n");
    printf("-----------------------------------------------------------------\n");
    system("pause");
}

void print_infected_info(FileSet &fileSet)
{
    FILE *logFile = fopen("infected_summary.txt", "w");
    fileSet.printSummary(logFile);
    fclose(logFile);

    printf("Infected files: %d\n", fileSet.countFiles());
    printf("\nDETECTED PIVOT FILE:\n [%s]\n", fileSet.minTimestampFile.c_str()); 
}

void version_banner()
{
    printf("---------------------------\n");
    printf("!DMALOCK UNLOCKER v%s - alpha\n", VERSION);
    printf("This is experimental version, in case of any problems please contact:\n");
    printf("hasherezade@gmail.com\n");
    printf("---------------------------\n\n");
}

int main (size_t argc, char *argv[])
{
    bool skip_decrypted = true;
    version_banner();
    if (argc < ARG_MIN) {
        printf("Params: [start_dir] [skip_decrypted]\n");
        printf(" start_dir (optional) : directory with infected files (default is current)\n");
        printf(" skip_decrypted : do not try to decrypt already decrypted files\n\n");
    }
    disclaimer_banner();

    std::string start_dir = "";
        
    FileTypesSet fileTypes;
    if (fileTypes.listDir(HDRS_DIR) == 0) {
        printf("[ERROR] Cannot load supported extensions!\n");
        system("pause");
        return -1;
    }

    size_t arg_num = ARG_MIN;
    if (argc >= ARG_MIN) {
        start_dir = argv[ARG_MIN - 1];
    } else {
        start_dir = PathCrawler::getLocalDir();
    }
    arg_num++;

    if (argc >= arg_num) {
        int mode = 0;
        sscanf(argv[arg_num - 1],"%d", &mode);
        skip_decrypted = (mode > 0);
    }

    FileStoringSet fileSet;
    PathCrawler crawler;
    fileSet.init();

    printf("\n>>Start path:\n%s\n", start_dir.c_str());

    Params::encryptionStartTime();
    fileSet.setKnownTypes(&fileTypes);
    crawler.listDir(start_dir, &fileSet, skip_decrypted);

    if (fileSet.countFiles() == 0) {
        printf("No files with recognized headers!\n");
        system("pause");
        return -1;
    }
    print_infected_info(fileSet);

    std::string pivot_file = fileSet.getPivotFile();
    
    uint32_t fileModTime = static_cast<uint32_t>(get_file_modification(pivot_file));
    if (fileModTime == 0) {
        system("pause");
        return -1;
    }
    
    Params params;
    params.init(pivot_file, fileModTime);
    params.seed = fileModTime;
    params.autosetMode();
    
    printf("Days ago:\t%d\n", (params.timeNow - params.seed) / DAY_LEN);

    //max number of encrypted files per milisecons
    int infected_per_ms = INFECTED_PER_MS;

    if (params.fileEncryptionTime == params.seed) {
        params.series_min = infected_per_ms; //start from small series
        params.series_max = SERIES_MAX;
    }

    if (decrypt_file(fileTypes, params, pivot_file) == false) {
        printf("Could not crack the pivot file!\n%s", pivot_file.c_str());
        system("pause");
        return -1;
    }
    printf("[SUCCESS]\n");
    DmaKeygen keygen(params.seed);
    Unlocker unlocker (fileTypes, params, keygen);
    std::vector<std::string> keys = keygen.makeKeys(params.key_num + SERIES_MAX);
    std::vector<std::string>::iterator key_itr;
    size_t unlocked = 0;
    size_t to_unlock = fileSet.countFiles() - 1;

    for (key_itr = keys.begin(); key_itr != keys.end(); key_itr++) {
        if (unlocker.tryKey(fileSet, *key_itr)) {
            unlocked++;
            //this key fits to no file - file could be removed?
            printf("Unlocked %d remaining %d\n", unlocked, to_unlock);
        }
        if (unlocked == to_unlock) break;
    }

    if (unlocked == fileSet.countFiles()) {
        printf("FINISHED!\n");
    } else {
        //TODO: find another pivot and repeat...
    }


    //keygen.reset(params.seed);
    //added part from the crawler:
    /*
    crackedSet.init();
    PathCrawler crackingCrawler;

    crackingCrawler.listDir(start_dir, &crackedSet, skip_decrypted);
    crackedSet.printSummary();
    printf("FINISHED!\n");*/

    system("pause");
    return 0;
}