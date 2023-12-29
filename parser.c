#include <stdlib.h>
#include "tokens.h"
#include "list_basic.h"

int parse(char* filename){
	struct list tokens;
	list_init(&tokens, sizeof(struct list), sizeof(struct list));
	    FILE* config = fopen(filename, "r");
    if (!config){
        fprintf(stderr, "Failed to open config!");
	return -1;
    }
    fseek(config, 0L, SEEK_END);
    uint64_t filesize = ftell(config);
    if (filesize == 0) return 1;
    rewind(config);
    char contents[filesize + 1];
    int bytesRead = fread(contents, sizeof(char), filesize, config);
    contents[filesize] = '\0';
    fclose(config);
    if (bytesRead == 0){
        fprintf(stderr, "Cannot read file contents!\n");
        return -1;
    }

}
