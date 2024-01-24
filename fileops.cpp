#include <cstdlib>
#include <cstdint>
#include "parser.hpp"
#include "fileops.hpp"

FILE* openfile(const char* fn, const char* mode){
    FILE* fd = fopen(fn, mode);
	if (!fd) return NULL;
	return fd;
}

int readfile(smake_file& file, FILE* fd){
		uint64_t fsize = filesize(fd);
		void* buf = malloc(fsize);
		if (!buf) return 1;
		file.contents = (char*)buf;
		fread(file.contents, 1, fsize, fd);
		return 0;
}

uint64_t filesize(FILE* fd){
		uint64_t old = ftell(fd);
		fseek(fd, 0, SEEK_END);
		uint64_t size = ftell(fd);
		fseek(fd, old, SEEK_SET);
		return size;
}
