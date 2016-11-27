#include "types.h"
#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "main.h"

#define ARG_MIN 2
#define SERIES_MAX  1000000
#define INFECTED_PER_MS 65

#define VERSION "0.0.7.1"
//----
void version_banner()
{
    printf("---------------------------\n");
    printf("DMA UNLOCKER v%s - alpha, by hasherezade\n", VERSION);
#ifdef VARIANT_2
    printf("for VARIANT 2, i.e:\nDMALOCK 41:11:11:84:32:13:64:68\n\n");
#endif
    printf("This is experimental version, in case of any problems please contact:\n");
    printf("hasherezade@gmail.com\n");
    printf("---------------------------\n");
}

void banner()
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

int main (int argc, char *argv[])
{
    version_banner();
    if (argc < ARG_MIN) {
        printf("Params: [start_dir] [skip_decrypted?]\n");
        printf(" start_dir (optional) : directory with infected files (default is current)\n");
        printf(" overwrite_decrypted? (optional) : 1 - yes, 0 - no; (default=0)\n");
    }
    banner();

    std::string start_dir = "";
    
    FileTypesSet fileTypes;
    if (fileTypes.listDir(HDRS_DIR) == 0) {
        printf("[ERROR] Cannot load supported extensions!\n");
    }

    size_t arg_num = ARG_MIN;
    if (argc >= arg_num) {
        start_dir = argv[arg_num - 1];
    } else {
        start_dir = PathCrawler::getLocalDir();
    }
    arg_num++;


    FileSet fileSet;
    PathCrawler crawler;
    fileSet.init();

    bool skip_decrypted = true;
    if (argc >= arg_num) {
        int overwrite = 0;
        sscanf(argv[arg_num - 1],"%d", &overwrite);
        if (overwrite > 0) {
            skip_decrypted = false;
            printf("MODE: Overwrite decrypted!\n");
        }
    }

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
#ifdef DEBUG
    printf("MOD TIME   = %d\n", params.fileEncryptionTime, params.fileEncryptionTime);
    printf("SEED   = %d = %#x\n", params.seed, params.seed);
#endif
    printf("Days ago:\t%d\n", (params.timeNow - params.seed) / DAY_LEN);

    //max number of encrypted files per milisecons
    int infected_per_ms = INFECTED_PER_MS;
    params.series_min = params.series_max = SERIES_MAX;

    if (params.fileEncryptionTime == params.seed) {
        params.series_min = infected_per_ms; //start from small series
        params.series_max = SERIES_MAX;
    }

    if (decrypt_file(fileTypes, params, pivot_file) == false) {
        printf("Could not crack the pivot file!\n%s\n", pivot_file.c_str());
        system("pause");
        return -1;
    }
    printf("[SUCCESS]\n");

    //added part from the crawler:
    CrackedSet crackedSet(fileTypes, params);
    crackedSet.init();
    PathCrawler crackingCrawler;

    crackingCrawler.listDir(start_dir, &crackedSet, skip_decrypted);
    crackedSet.printSummary();
    printf("FINISHED!\n");

    system("pause");
    return 0;
}
