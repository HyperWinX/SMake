#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>

typedef enum paramname{
	ADD_EXEC,
	PARENTHESE_OPEN,
	PARENTHESE_CLOSE,
	SET
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

typedef struct target{
	std::string name;
	std::vector<std::string> sources;
	std::string compiler;
	std::string additional_args;
} target;

err GenerateToken(char (&buf)[1024], token& tok);
std::vector<token> ParseFileContents(smake_file& file);

#endif