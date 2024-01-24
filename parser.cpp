#include "parser.hpp"
#include <cstring>
#include <vector>
#include <cstdio>

std::vector<token> ParseFileContents(smake_file& file){
	std::vector<token> tokens;
	char* baseptr = file.contents;
	char buf[1024];
	int bufptr = 0;
	token tok;
	do{
		if (*(file.contents - 1) == ')'){
			if (!strlen(buf)){
				file.contents++;
				continue;
			}
			file.contents++;
			if (!GenerateToken(buf, tok))
				return {};
			tokens.push_back(tok);
			buf[0] = '\0';
			bufptr = 0;
			file.contents++;
			continue;
		}
		buf[bufptr++] = *file.contents;
		printf("[1] Consumed char %c, current offset: %d\n", buf[bufptr - 1], bufptr - 1);
		printf("[1] Char is (: %s\n", *(file.contents - 1) == ')' ? "true" : "false");
	} while (*(++file.contents));
	if (*(--file.contents) == ')'){
		GenerateToken(buf, tok);
		tokens.push_back(tok);
		buf[0] = '\0';
	}
	file.contents = baseptr;
	printf("%zu\n", tokens.size());
	return tokens;
}

err GenerateToken(char (&buf)[1024], token& tok){
	char paramname[33];
	char value[1025];
	int ptr = 0;
	int valptr = 0;
	memset(paramname, 0, sizeof(paramname));
	memset(value, 0, sizeof(value));

	do{
		if (ptr == 32) return ERR_TOO_BIG_PARAMNAME;
		paramname[ptr] = buf[ptr];
		printf("[2] Consumed char %c, current offset: %d\n", buf[ptr], ptr);
	} while (buf[++ptr] != '(');
	ptr++;

	if(!strcmp(paramname, "add_exec")) tok.parameter = ADD_EXEC;
	if(!strcmp(paramname, "set")) tok.parameter = SET;

	while (buf[ptr] != ')'){
		if (valptr == 1024)
			return ERR_TOO_BIG_VALUE;
		printf("[3] Consumed char %c, current offset: %d\n", buf[ptr], ptr);
		value[valptr++] = buf[ptr++];
	}
	printf(value);
	tok.value = std::string(value);
	return NO_ERR;
}
