#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum ParseMode{
	CURPARSE_COMPILER,
	CURPARSE_FILES,
	CURPARSE_COMPILERFLAGS,
	CURPARSE_ASSEMBLERFLAGS,
	CURPARSE_LINKERFLAGS
};

enum Error{
	InvalidSOF,
	NoSMakeFile,
	UserDirRetrievalFailure,
	NoConfigEntryName,
	TooLongConfigEntryName
};

struct SMakeConfig{
	int8_t compiler;
	char* files;
	char* compilerflags;
	char* assemblerflags;
	char* linkerflags;
};

char *filename = "build.smake";

void ErrExit(enum Error err){
	switch(err){
		case InvalidSOF:
			printf(ANSI_COLOR_RED "Invalid start of file! Cannot continue.\n" ANSI_COLOR_RESET);
			exit(1);
		case NoSMakeFile:
			printf(ANSI_COLOR_RED "No SMake file found! Cannot continue.\n" ANSI_COLOR_RESET);
			exit(1);
		case UserDirRetrievalFailure:
			printf(ANSI_COLOR_RED "Cannot get current user directory! Cannot continue.\n" ANSI_COLOR_RESET);
			exit(1);
		case NoConfigEntryName:
			printf(ANSI_COLOR_RED "No config entry name! Cannot continue.\n" ANSI_COLOR_RESET);
			exit(1);
		case TooLongConfigEntryName:
			printf(ANSI_COLOR_RED "Too long config entry name! May be caused by incorrect syntax. Cannot continue.\n");
			exit(1);
	}
}

void CombinePaths(char* destination, const char* path1, const char* path2)
{
    if(path1 == NULL && path2 == NULL) {
        strcpy(destination, "");;
    }
    else if(path2 == NULL || strlen(path2) == 0) {
        strcpy(destination, path1);
    }
    else if(path1 == NULL || strlen(path1) == 0) {
        strcpy(destination, path2);
    } 
    else {
        char directory_separator[] = "/";
        const char *last_char = path1;
        while(*last_char != '\0')
            last_char++;        
        int append_directory_separator = 0;
        if(strcmp(last_char, directory_separator) != 0) {
            append_directory_separator = 1;
        }
        strcpy(destination, path1);
        if(append_directory_separator)
            strcat(destination, directory_separator);
        strcat(destination, path2);
    }
}

void ParseSMakeConf(char *path){
	FILE* fd;
	if (!(fd = fopen(path, "r"))) ErrExit(NoSMakeFile);
	fseek(fd, 0L, SEEK_END);
	uint64_t len = ftell(fd);
	rewind(fd);
	char buf[len];
	fread(&buf, sizeof(char), len, fd);
	fclose(fd);
	struct SMakeConfig config = {0};
	char compiler[4] = {0};
	char *files = (char*)calloc(sizeof(char), 4096);
	char *compilerflags = (char*)calloc(sizeof(char), 512);
	char *assemblerflags = (char*)calloc(sizeof(char), 512);
	char *linkerflags = (char*)calloc(sizeof(char), 512);
	enum ParseMode currentmode = CURPARSE_COMPILER;
	uint8_t parsing_str = 0;
	char tmpbuf[33] = {0};
	int16_t tmpbuf_offset = 0;
	for (uint64_t i = 0; i < len; i++){
		if (buf[i] != '=' && strlen(tmpbuf) == 32) ErrExit(TooLongConfigEntryName);
		if (buf[i] == '='){
			if (strlen(tmpbuf) == 0) ErrExit(NoConfigEntryName);
			switch(currentmode){
				case CURPARSE_COMPILER:
					currentmode = CURPARSE_FILES;
					break;
				case CURPARSE_FILES:
					currentmode = CURPARSE_COMPILERFLAGS;
					break;
				case CURPARSE_COMPILERFLAGS:
					currentmode = CURPARSE_ASSEMBLERFLAGS;
					break;
				case CURPARSE_ASSEMBLERFLAGS:
					currentmode = CURPARSE_LINKERFLAGS;
					break;
			}
			continue;
		}
		if (buf[i] == '"'){
			
		}
		if (currentmode == CURPARSE_COMPILER){
			if (buf[i] != '#') ErrExit(InvalidSOF);
			if (buf[i] == '\n') currentmode = CURPARSE_FILES;
		}
	}
}

void RunBuild(void){
	char *buf = (char*)calloc(sizeof(char), PATH_MAX + 64);
	char *curpath;
	if (getcwd(curpath, sizeof(curpath)) == NULL){
		printf(ANSI_COLOR_RED "Failed to retrieve current user directory!\n" ANSI_COLOR_RESET);
		exit(1);
	}
	CombinePaths(curpath, buf, filename);
	
}

int main(int argc, char *argv[]){
	if (argc == 1) RunBuild();
}
