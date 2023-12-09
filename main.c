#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

enum ParseMode{
	NULLVAL,
	CURPARSE_COMPILER,
	CURPARSE_FILES,
	CURPARSE_COMPILERFLAGS,
	CURPARSE_ASSEMBLERFLAGS,
	CURPARSE_LINKERFLAGS,
	CURPARSE_OUTPUT,
	CURPARSE_RUN
};

enum Error{
	InvalidSOF,
	NoSMakeFile,
	UserDirRetrievalFailure,
	NoConfigEntryName,
	TooLongConfigEntryName,
	UnknownConfigurationEntry,
	UnknownCompiler,
	UnknownRunValue
};

struct SMakeConfig{
	int8_t compiler;
	int8_t afterbuildrun;
	char* files;
	char* compilerflags;
	char* assemblerflags;
	char* linkerflags;
	char* output;
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
		case UnknownCompiler:
			printf(ANSI_COLOR_RED "Unknown compiler!\n" ANSI_COLOR_RESET);
			exit(1);
		case UnknownRunValue:
			printf(ANSI_COLOR_RED "Unknown value of RUN parameter!\n" ANSI_COLOR_RESET);
			exit(1);
	}
}

void PrintConfig(struct SMakeConfig *config){
	printf("Compiler: %s\nFiles: %s\nCompiler flags: %s\nAssembler flags: %s\nLinker flags: %s\n",
	config->compiler == 0 ? "gcc" : "g++",
	config->files,
	config->compilerflags,
	config->assemblerflags,
	config->linkerflags);
}

void GetDirName(char* filepath){
	int32_t last_index = 0;
	for (int32_t i = 0; i < strlen(filepath); i++) if (filepath[i] == '/') last_index = i;
	if (last_index == 0) return;
	filepath[last_index] = '\0';
}

char* GetFileName(char* path){
    return strrchr(path, '/') + 1;
}

void HandleGetCWDError(void){
	switch (errno){
		case EACCES:
			printf(ANSI_COLOR_RED "Permission denied!\n" ANSI_COLOR_RESET);
			break;
		case EFAULT:
			printf(ANSI_COLOR_RED "Buf pointer points to a bad address!\n" ANSI_COLOR_RESET);
			break;
		case EINVAL:
			printf(ANSI_COLOR_RED "Size argument is zero and buf is not a null pointer" ANSI_COLOR_RESET);
			break;
		case ENAMETOOLONG:
			printf(ANSI_COLOR_RED "The size of null-terminated absolute pathname string exceeds %d bytes!\n", PATH_MAX, ANSI_COLOR_RESET);
			break;
		case ENOENT:
			printf(ANSI_COLOR_RED "The current working directory is unlinked!\n" ANSI_COLOR_RESET);
			break;
		case ENOMEM:
			printf(ANSI_COLOR_RED "Out of memory!\n" ANSI_COLOR_RESET);
			break;
		case ERANGE:
			printf(ANSI_COLOR_RED "The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte!\n" ANSI_COLOR_RESET);	
			break;
	}
	exit(1);
}

void CombinePaths(char* destination, const char* path1, const char* path2)
{
    if(path1 == NULL && path2 == NULL) {
        strcpy(destination, "");
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

struct SMakeConfig *ParseSMakeConf(char *path){
	FILE* fd;
	if (!(fd = fopen(path, "r"))) ErrExit(NoSMakeFile);
	fseek(fd, 0L, SEEK_END);
	uint64_t len = ftell(fd);
	rewind(fd);
	char buf[len];
	fread(&buf, sizeof(char), len, fd);
	fclose(fd);
	char compiler[4] = {0};
	char run[2] = {0};
	char *files = (char*)calloc(sizeof(char), 16384);
	char *compilerflags = (char*)calloc(sizeof(char), 512);
	char *assemblerflags = (char*)calloc(sizeof(char), 512);
	char *linkerflags = (char*)calloc(sizeof(char), 512);
	char *output = (char*)calloc(sizeof(char), 128);
	enum ParseMode currentmode = NULLVAL;
	uint8_t parsing_str = 0;
	char tmpbuf[33] = {0};
	uint64_t tmpbuf_offset = 0;
	char *bufptr = (char*)&tmpbuf;
	for (uint64_t i = 0; i < len; i++){
		if (buf[i] == '\n') continue;
		if (buf[i] != '=' && strlen(tmpbuf) == 32) ErrExit(TooLongConfigEntryName);
		if (buf[i] == '='){
			if (strlen(bufptr) == 0) ErrExit(NoConfigEntryName);
			if (strcmp(bufptr, "compiler") == 0) currentmode = CURPARSE_COMPILER;
			else if (strcmp(bufptr, "files") == 0) currentmode = CURPARSE_FILES;
			else if (strcmp(bufptr, "compilerflags") == 0) currentmode = CURPARSE_COMPILERFLAGS;
			else if (strcmp(bufptr, "assemblerflags") == 0) currentmode = CURPARSE_ASSEMBLERFLAGS;
			else if (strcmp(bufptr, "linkerflags") == 0) currentmode = CURPARSE_LINKERFLAGS;
			else if (strcmp(bufptr, "run") == 0) currentmode = CURPARSE_RUN;
			else if (strcmp(bufptr, "output") == 0) currentmode = CURPARSE_OUTPUT;
			else ErrExit(UnknownConfigurationEntry);
			continue;
		}
		if (buf[i] == '"'){
			if (parsing_str == 0){
				parsing_str = 1;
				switch (currentmode){
					case CURPARSE_COMPILER:
						bufptr = (char*)&compiler;
						break;
					case CURPARSE_FILES:
						bufptr = files;
						break;
					case CURPARSE_COMPILERFLAGS:
						bufptr = compilerflags;
						break;
					case CURPARSE_ASSEMBLERFLAGS:
						bufptr = assemblerflags;
						break;
					case CURPARSE_LINKERFLAGS:
						bufptr = linkerflags;
						break;
					case CURPARSE_RUN:
						bufptr = (char*)&run;
						break;
					case CURPARSE_OUTPUT:
						bufptr = output;
						break;
				}
			}
			else if (parsing_str == 1){
				parsing_str = 0;
				currentmode = NULLVAL;
				bufptr = (char*)&tmpbuf;
				memset(bufptr, 0x00, sizeof(tmpbuf));
			}
			tmpbuf_offset = 0;
			continue;
		}
		bufptr[tmpbuf_offset++] = buf[i];
	}
	struct SMakeConfig* config = calloc(sizeof(struct SMakeConfig), 1);
	if (strcmp(compiler, "gcc") == 0) config->compiler = 0;
	else if (strcmp(compiler, "g++") == 0) config->compiler = 1;
	else ErrExit(UnknownCompiler);
	if (run[0] == '0') config->afterbuildrun = 0;
	else if (run[0] == '1') config->afterbuildrun = 1;
	else ErrExit(UnknownRunValue);
	config->files = files;
	config->compilerflags = compilerflags;
	config->assemblerflags = assemblerflags;
	config->linkerflags = linkerflags;
	config->output = output;
	return config;
}

int ProcessFile(struct SMakeConfig* config, char* directory, char* src_filepath){
	char parent_dir[1024];
	char command[2048];
	memcpy((char*)&parent_dir, directory, 1024);
	GetDirName((char*)&parent_dir);
	snprintf(command, sizeof(command), "mkdir -p %s", parent_dir);
	int32_t retcode = system(command);
	if (retcode != 0) return -1;
	memset((char*)&command, 0x00, sizeof(command));
	snprintf(command, sizeof(command), "cp %s %s", src_filepath, directory);
	retcode = system(command);
	if (retcode != 0) return -1;
	memset((char*)&command, 0x00, sizeof(command));
	char* filename = GetFileName(src_filepath);
	snprintf(command, sizeof(command), "%s %s/%s %s", config->compiler == 0 ? "gcc" : "g++", directory, filename, config->compilerflags);
	snprintf(((char*)&command) + strlen(command), sizeof(command) - strlen(command), " -o %s", config->output);
	system(command);
}

int RunBuild(void){
	char *buf = (char*)calloc(sizeof(char), PATH_MAX + 64);
	char curpath[PATH_MAX];
	errno = 0;
	if (getcwd(curpath, PATH_MAX) == NULL) HandleGetCWDError();
	CombinePaths(buf, curpath, filename);
	struct SMakeConfig *config = ParseSMakeConf(buf);
	free(buf);
	srand(time(NULL));
	int random = rand();
	char command_foldercreation[64] = {0};
	char directory[32] = {0};
	snprintf(directory, sizeof(directory), "/tmp/SMake-%d", random);
	snprintf(command_foldercreation, sizeof(command_foldercreation), "mkdir %s", directory);
	system(command_foldercreation);
	char *filepath = (char*)calloc(sizeof(char), 2048);
	char *src_filepath = (char*)calloc(sizeof(char), 2048);
	char parsed_filename[1024] = {0};
	uint16_t offset = 0;
	for (uint64_t i = 0; i < strlen(config->files); i++){
		if (config->files[i] == ' ' || config->files[i] == '\0'){
			CombinePaths(filepath, directory, parsed_filename);
			CombinePaths(src_filepath, buf, parsed_filename);
			int32_t retcode = ProcessFile(config, filepath, src_filepath);
			if (retcode != 0) return -1;
			memset((char*)&parsed_filename, 0x00, sizeof(parsed_filename));
			memset(filepath, 0x00, 2048);
			memset(src_filepath, 0x00, 2048);
			offset = 0;
			continue;
		}
		parsed_filename[offset++] = config->files[i];
	}
	CombinePaths(filepath, directory, parsed_filename);
	CombinePaths(src_filepath, curpath, parsed_filename);
	int retcode = ProcessFile(config, directory, src_filepath);
	if (retcode != 0) return -1;
	memset((char*)&parsed_filename, 0x00, sizeof(parsed_filename));
	free(filepath);
	free(src_filepath);
	char removecommand[32] = {0};
	snprintf(removecommand, sizeof(removecommand), "rm -rf %s", directory);
	system(removecommand);
	if (config->afterbuildrun == 1){
		char buffer[256] = {0};
		buffer[0] = '.';
		buffer[1] = '/';
		strcpy(((char*)&buffer) + 2, config->output);
		system(buffer);
	}
}

int main(int argc, char *argv[]){
	if (argc == 1) RunBuild();
}
