#include "parser.hpp"
#include "fileops.hpp"
#include <cstdio>
#include <vector>

int main(int argc, char* argv[]){
	//if (argc == 1) return 1;
	for (int i = 0; i < argc; i++) printf("%s\n", argv[i]);
	smake_file sfile;
	FILE* fd = openfile(argv[1], "r");
	if (!fd) return 1;
	readfile(sfile, fd);
	std::vector<token> tokens = ParseFileContents(sfile);
	printf("Total commands fetched: %d\n", tokens.size());
	for (int i = 0; i < tokens.size(); i++){
		printf("Command name: %s\n", tokens[i].parameter == ADD_EXEC ? "ADD_EXEC" : "undefined");
		printf("Command args: %s\n", tokens[i].value.c_str());
	}
}
