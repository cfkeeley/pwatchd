#define DBLQUOTE 257
#define COMMENT 258
#define BLANK 259
#define EQUALS_SIGN 260
#define CFGVAR 261
#define INTERFACE 262
#define TCP_PORT_LIST 263
#define UDP_PORT_LIST 264
#define REUSE_SOCKETS 265
#define PKT_SCAN 266
#define PARSE_ENTIRE_RSET 267
typedef union {
	int *number;
	char *string;
} YYSTYPE;
extern YYSTYPE yylval;
