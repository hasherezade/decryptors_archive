#include "timeconv.h"
//#define DEBUG 1
//----

time_t FileTime_to_POSIX(FILETIME ft)
{
    // takes the last modified date
    LARGE_INTEGER date, adjust;
    date.HighPart = ft.dwHighDateTime;
    date.LowPart = ft.dwLowDateTime;
    // 100-nanoseconds = milliseconds * 10000
    adjust.QuadPart = 11644473600000 * 10000;
    // removes the diff between 1970 and 1601
    date.QuadPart -= adjust.QuadPart;
    // converts back from 100-nanoseconds to seconds
    return date.QuadPart / 10000000;
}

void print_time_str(time_t rawtime)
{
    struct tm* timeinfo = localtime (&rawtime);
    char s[100]; /* strlen("2009-08-10 18:17:54") + 1 */
    strftime (s, 100, "%Y-%m-%d %H:%M:%S", timeinfo);
    printf("%s\n", s);
}

time_t get_file_modification(std::string filename)
{
    HANDLE hFile =  CreateFile(filename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
    if (hFile == NULL) {
        printf("Cannot open file: %s\n", filename.c_str());
        return 0;
    }
    FILETIME lastWriteTime;
    BOOL isOk = GetFileTime(hFile, NULL, NULL, &lastWriteTime);
    
    CloseHandle(hFile);

    return FileTime_to_POSIX(lastWriteTime);
}

time_t get_file_creation(std::string filename)
{
    
    HANDLE hFile =  CreateFile(filename.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, 0);
    if (!hFile) {
        printf("Cannot open file: %s\n", filename.c_str());
        return 0;
    }
    FILETIME creationTime;
    BOOL isOk = GetFileTime(hFile, &creationTime, NULL, NULL);
    CloseHandle(hFile);

    if (isOk == FALSE) {
#ifdef DEBUG
        printf("Cannot get creation of file: %s\n", filename);
#endif
        return 0;
    }
    return FileTime_to_POSIX(creationTime);
}

uint32_t print_time_info(std::string path, uint32_t fileModTime)
{
#ifdef DEBUG
    printf("***\n");
    printf("Detected encryption time for the file:\n[%s]\n", path);
    printf("TIMESTAMP: %d = %x\n", fileModTime, fileModTime);
    print_time_str(fileModTime);
    printf("Days ago:\t%d\n", (time(NULL) - fileModTime) / DAY_LEN);
    printf("***\n");
#endif
    return fileModTime;
}
