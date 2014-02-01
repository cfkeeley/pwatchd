%{
#include <stdio.h>
#include <string.h>
#include "include/configParser.h"


static void assignCfgVar(void *, void *);
int *set = 1;

%}

/* tokens */
%token DBLQUOTE COMMENT BLANK EQUALS_SIGN

%union {
	int *number;
	char *string;
}

%token <string> CFGVAR
%token <string> INTERFACE
%token <string> TCP_PORT_LIST
%token <string> UDP_PORT_LIST

%token <number> REUSE_SOCKETS
%token <number> PKT_SCAN
%token <number> PARSE_ENTIRE_RSET

/* types */
%type <string> getStringVal
%type <number> getIntVal 

%start input
%% 
input: identifier {
     }

identifier: INTERFACE getStringVal {
	  	(void)assignCfgVar(cfgVars->interface, $2);
	} input  

	| TCP_PORT_LIST getStringVal {
	  	(void)assignCfgVar(cfgVars->tcpPorts, $2);
	} input 

	| UDP_PORT_LIST getStringVal {
	  	(void)assignCfgVar(cfgVars->udpPorts, $2);
	} input 

	| REUSE_SOCKETS getIntVal {
		if($2 == set) {
			cfgVars->optionFlags |= REUSE_SKTS;
		}
	} input  
	 
	| PKT_SCAN getIntVal {  
		if($2 == set) {
			cfgVars->optionFlags |= SCAN_PKT;
		}
	} input

	| PARSE_ENTIRE_RSET getIntVal {
		if($2 == set) {
			cfgVars->optionFlags |= PARSE_ALL;
		}
	} input

	|
;

/* need to alter and remove casts */
getStringVal: EQUALS_SIGN DBLQUOTE CFGVAR DBLQUOTE {
     	$$ = (char *)$3;
     }
;

getIntVal: EQUALS_SIGN DBLQUOTE CFGVAR DBLQUOTE {
     	$$ = (int *)$3;
     }
;

%%

static void assignCfgVar(void *structVar, void *val) {
	void **sVar, **sVal;
	sVar = structVar;
	sVal = val;
	
	if(!val) {
		(void)yyerror();
	} else {
		(void)printf("assigning: %s\n", sVal);
		sVar = sVal;
	}
}

struct configFileVars *getConfigStruct() {
	return cfgVars;
}

void parseConfigFile() 
{
	extern FILE *yyin;

	yyin = fopen(CONFIG_FILE, "r");

	if(!yyin) {
		(void)perror("failed to open config file: ");
	} else {
		(void)yyparse();
		(void)fclose(yyin);
	}
}

