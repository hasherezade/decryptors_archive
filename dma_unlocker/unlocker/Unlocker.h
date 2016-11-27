#include "../dma_crack.h"

#include "../Params.h"
#include "../recon/PathCrawler.h"
#include "../decryptor/decryptor.h"
#include "../filetypes/FileTypes.h"
#include "../cracking_crawler/CrackedSet.h"
#include "../keygen/DmaKeygen.h"


class Unlocker {
public:
    Unlocker(FileTypesSet &fTypes, Params &p, DmaKeygen &k)
        : fileTypes(fTypes), params(p), keygen(k) {}
        
    static std::string getPivotFile(FileTypesSet &fileTypes, FileStoringSet& filesInput);

    bool tryKey(FileStoringSet& filesInput, std::string key);

    DmaKeygen &keygen;
    FileTypesSet &fileTypes;
    Params &params;
};