#include <windows.h>

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>
#include <stdio.h>

#include "FileTypes.h"

//----

int main (int argc, char *argv[])
{
    printf("---------------------------\n");
    printf("DMA filetypes v0.1\n");
    printf ("params: [hdrs_dir] - default: '%s'\n", HDRS_DIR);
    printf("---------------------------\n");

    FileTypesSet typesSet;
    typesSet.listDir(HDRS_DIR);
    printf("Registered %d filetypes!\n", typesSet.countTypes());
    printf ("ok!\n");
    system("pause");
    return 0;
}