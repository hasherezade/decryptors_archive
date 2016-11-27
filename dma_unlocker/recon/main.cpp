#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "../Params.h"
#include "PathCrawler.h"
#include "FileSet.h"
#include "../filetypes/FileTypes.h"
//----

#define DAY_LEN 86400

int main (int argc, char *argv[])
{
    bool skip_decrypted = false;

    FileSet fileSet;
    PathCrawler crawler(&fileSet);
    
    printf("---------------------------\n");
    printf("DMA recon v1.4\n");
    printf("---------------------------\n");
    if (argc >= 2 && argv[1][0] == '1') {
        skip_decrypted = true;
        printf("Skipping decrypted!\n");
    }
    
    crawler.listDir(crawler.startPath, skip_decrypted);
    printf("listing finished...\n");
    fileSet.printSummary();

    system("pause");
    return 0;

}