#ifndef HDR_CONFIG_PARSER
#define HDR_CONFIG_PARSER

#define CONFIG_FILE "pwatch.conf"
#define MAX_IFF_LENGTH 4
#define MAX_PORTS 250
#define SOCK_LIMIT 250 

/* config flags - some of these are _not_ yet implemented */
#define PARSE_ALL	0x01 /* parse entire ruleset */
#define REUSE_SKTS 	0x02 /* reuse only pwatch sockets */
#define USE_ANC		0x04 /* use a ruleset anchor */
#define LISTEN_TCP	0x08 /* monitor tcp */
#define LISTEN_UDP	0x10 /* monitor udp */
#define SCAN_PKT	0x20 /* scan packets */

#include <stdio.h>

/* this holds all of our variable options */
struct configFileVars {
	char *portstring;
	char *interface;
	char *tcpPorts;
	char *udpPorts;
	int *failedPorts;
	int socket_union[SOCK_LIMIT]; 
	unsigned int tcpPortsCounter;
	unsigned int udpPortsCounter;
	unsigned int failures;
	unsigned int socketsBound;
	unsigned int optionFlags;

} *cfgVars = NULL;


void parseConfigFile();
struct configFileVars *getConfigStruct();

#endif /* !_HDR_CONFIG_PARSER */
