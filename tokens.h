typedef enum TokenType{
	PARAM_TYPE,
	INT_LIT,
	STR_LIT
};

typedef struct Token{
	TokenType tokentype;
	void* value;
};
