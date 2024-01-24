#include "parser.hpp"
#include "fileops.hpp"
#include <cstdio>

int main(int argc, char* argv[]){
		smake_file sfile;
		readfile(sfile, openfile("test.smake", "r"));
		std::vector<token> tokens = ParseFileContents(sfile);
		printf("Total commands fetched: %d\n", tokens.size());
		for (int i = 0; i < tokens.size(); i++){
				printf("Command name: %s\n", tokens[i].parameter == ADD_EXEC ? "ADD_EXEC" : "undefined");
				printf("Command args: %s\n", tokens[i].value.c_str());
		}
}
