#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>

typedef enum paramname{
	ADD_EXEC
} paramname;

typedef enum err{
	ERR_TOO_BIG_PARAMNAME,
	ERR_TOO_BIG_VALUE,
	NO_ERR
} err;

typedef struct smake_file{
	char* filename;
	char* contents;
} smake_file;

typedef struct token{
	paramname parameter;
	std::string value;
} token;


err GenerateToken(char (&buf)[1024], token& tok);
std::vector<token> ParseFileContents(smake_file& file);

#endif
