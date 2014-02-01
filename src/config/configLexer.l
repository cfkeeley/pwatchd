%{
#include <stdio.h>
#include <string.h>
#include "include/y.tab.h"


%}

%option yylineno 

/* config file tokens */
interface		[Ii][Nn][Tt][Ee][Rr][Ff][Aa][Cc][Ee]
reuse_sockets		[Rr][Ee][Uu][Ss][Ee]{uscore}[Ss][Oo][Cc][Kk][Ee][Tt][Ss]
tcp_port_list		[Tt][Cc][Pp]{uscore}[Pp][Oo][Rr][Tt]{uscore}[Ll][Ii][Ss][Tt]
udp_port_list		[Uu][Dd][Pp]{uscore}[Pp][Oo][Rr][Tt]{uscore}[Ll][Ii][Ss][Tt]
pkt_scan		[Pp][Kk][Tt]{uscore}[Ss][Cc][Aa][Nn]
parse_entire_rset	[Pp][Aa][Rr][Ss][Ee]{uscore}[Ee][Nn][Tt][Ii][Rr][Ee]{uscore}[Rr][Uu][Ll][Ee][Ss][Ee][Tt]


cfgVar			[a-zA-Z]+|[a-zA-Z0-9]+|[0-9{1,2},{1,1}]+|[0-9]
uscore			_
dblQuote  		\"
equals_sign		=

%%

[\t ]+   /* ignore whitespace */
\n 	 /* ignore newlines */
^#.*	 /* ignore comments */

{parse_entire_rset} {
	yylval.number = (int *)atoi(yytext);
	return PARSE_ENTIRE_RSET;
}

{pkt_scan} {
	yylval.number = (int *)atoi(yytext);
	return PKT_SCAN;
}

{tcp_port_list} {
	yylval.string = strdup(yytext);
	return TCP_PORT_LIST;
}

{udp_port_list} {
	yylval.string = strdup(yytext);
	return UDP_PORT_LIST;
}

{interface} {
	yylval.string = strdup(yytext);
	return INTERFACE;
}

{cfgVar} {
	yylval.string = strdup(yytext);
	return CFGVAR;
}

{reuse_sockets} {
	yylval.number = (int *)atoi(yytext);
	return REUSE_SOCKETS;
}

{equals_sign} {
	return EQUALS_SIGN;
}

{dblQuote} {
	return DBLQUOTE;
}

%%

void yyerror(const char *str) 
{
	(void)fprintf(stderr, "config file: '%s' on line: %d\n", str, yylineno);
}

int yywrap()
{
	return 1;
}
