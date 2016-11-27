#include "Unlocker.h"
#include "../util.h"

#define CHECK_MS 5

bool Unlocker::tryKey(FileStoringSet& filesInput, std::string key)
{
    std::map<uint32_t, std::vector<std::string> >::iterator itr;
    size_t checked_ms = 0;
    for (itr = filesInput.filesMap.begin(); itr != filesInput.filesMap.end() && checked_ms < CHECK_MS; itr++, checked_ms++) {
         std::vector<std::string>&vec = itr->second;
         uint32_t infection_time = itr->first;

         std::vector<std::string>::iterator paths_itr;
         for (paths_itr = vec.begin();  paths_itr != vec.end(); paths_itr++) {
             std::string &full_path = *paths_itr;

             if (decrypt_file(fileTypes, params, full_path, key)) {
                printf ("[KEY OK]\n");
                return true;
             }
         }
    }
    return false;
}
