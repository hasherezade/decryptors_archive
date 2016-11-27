#pragma once

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <string>
#include "filetypes/FileTypes.h"
#include "filetypes/TypeValidator.h"
#include "Params.h"

std::string search_key(Params &params,
                TypeValidator *validator, 
                size_t series_min, 
                size_t series_max
                );

std::string search_key_in_series(uint32_t seed, 
                TypeValidator *validator, 
                unsigned char *enc_buf
                );


std::string get_extension(std::string filename);
bool read_header(std::string filename, unsigned char buf[], size_t offset);

bool decrypt_file(FileTypesSet &fileTypes, Params &params,
                  std::string enc_filename);

bool decrypt_file(FileTypesSet &fileTypes, Params &params,
                  std::string enc_filename, std::string &key);