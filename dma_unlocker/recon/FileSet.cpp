#include "FileSet.h"

//virtual 
bool FileSet::processFile(std::string path, std::string ext, uint32_t writeTime)
{
    return logToFile(path, writeTime);
}

bool FileSet::logToFile(std::string path, uint32_t writeTime)
{
    if (logFile) {
        fprintf(logFile, "%d,%s\n", writeTime, path.c_str());
        fflush(logFile);
    }
    return true;
}

void FileSet::addPivot(std::string path, std::string ext, uint32_t writeTime)
{
    if (this->knownTypes == NULL) return;
    FileType *type = this->knownTypes->getType(ext);
    if (type != NULL && type->accuracy >= PIVOT_MIN) {
        this->pivotFile = path;
    }
}

void FileSet::addFile(std::string path, std::string ext, uint32_t writeTime)
{
    if (path.length() == 0) return;
    if (this->knownTypes != NULL && knownTypes->getType(ext) == NULL) {
        return; //skip unknown extension
    }

    if (processFile(path, ext, writeTime) == false) {
        return;
    }

    filesCounter++;
    extensions.insert(ext);
    
    if (this->minTimestamp == 0) {
        minTimestamp = maxTimestamp  = writeTime;
        minTimestampFile = maxTimestampFile = pivotFile = path;
        return;
    }

    if (writeTime < minTimestamp) {
        writeTime = writeTime;
        minTimestampFile = path;
        addPivot(path, ext, writeTime);
    }

    if (writeTime > maxTimestamp) {
        maxTimestamp = writeTime;
        maxTimestampFile = path;
    }
}
