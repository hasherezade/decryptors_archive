#include "CrackedSet.h"
#define INFECTED_PER_MS 50
#define UPPER_SERIES_BOUND 2000000

#include "../timeconv.h"


//virtual 
bool CrackedSet::processFile(std::string path, std::string ext, uint32_t writeTime)
{
    if (params.fileEncryptionTime == 0) {
        params.fileEncryptionTime = writeTime;
        params.init(path, writeTime);
    }
    long dif = 0;
    if (this->lastDecryptedTimestamp != 0) {
        dif = abs(long(writeTime - lastDecryptedTimestamp));
    }

    if (params.seed == 0 || writeTime < params.seed || dif > HOUR_LEN) {
#ifdef DEBUG
        printf ("Setting seed to the file encryption time!\n");
#endif     
        params.init(path, writeTime);
        params.seed = params.fileEncryptionTime;
        params.autosetMode();
    }

    if (params.fileEncryptionTime == params.seed) {
#ifdef DEBUG
        printf("Smart timestamp enabled!\n");
#endif
        params.series_min = INFECTED_PER_MS; //start from small series
        params.series_max = UPPER_SERIES_BOUND;
    } else {
        params.series_min = params.series_max;
    }

    params.filename = path;
    if (decrypt_file(fileTypes, params, path)) {
        printf("[SUCCESS]\n");
        this->lastDecryptedTimestamp = writeTime;
        logToFile(path, writeTime);
        return true;
    }
    return false;
}
