#ifndef FILEOPS_HPP
#define FILEOPS_HPP

#include <cstdint>
#include <cstdlib>
#include "parser.hpp"

FILE* openfile(const char* fn, const char* mode);
int readfile(smake_file& file, FILE* fd);
uint64_t filesize(FILE* fd);

#endif
