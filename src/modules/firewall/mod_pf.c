/* Copyright (c) 1982, 1986, 1990, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * ----------------------------------------------------------------------------
 * module: mod_pf.c  - pf firewall module for openbsd
 * author: chris keeley 
 *
 */

#include "../../include/module.h" /* module specific */
#include "../../include/firewall.h" /* module specific */
#include "../../include/configParser.h"
#include "../../include/getipv4.h"
#include "../../include/logger.h"
#include "../../include/utils.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/pfvar.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAX_MESSAGE_LEN 64
#define PWATCH_ERR -1

/* modules specific structures */
static struct module_t *mod_pf = NULL;
static struct firewall_hooks_t *pfCmds = NULL;

/* struct for manipulating rules */
struct pfioc_rule pr;

/* keep the compiler happy */
int inet_pton(int, const char *, void *);
int close(int);

/* firewall utility rules */
static int test_rule(char *, int *, int, int, u_char);
static int pfAddRule(char *, int, struct configFileVars *);
static int get_dgram_socket();

static int pfAddRule(char *incoming_ip, int portnum, struct configFileVars *opts) 
{
	struct pfioc_pooladdr paddr;
	struct sockaddr ifr;
	char curr_ipv4[16];
	char *ifname = opts->interface;
	unsigned short test_result = 0;
	int pf_dev_fd, ret = 1, sockfd = 0;
	/* 
	 * this can be moved out and made a static global variable 
	 * a function/macro needs to be provided to update this variable 
	 */
	static int num_rules_added = 0; /* the rule number to add the next rule after */

	memset(&paddr, 0, sizeof(struct pfioc_pooladdr));
	memset(&ifr, 0, sizeof(struct sockaddr));

	if((sockfd = get_dgram_socket()) < 0) {
		(void)logmsg("get_dgram_socket() no socket for getipv4()");
		(void)report_status_and_bail(BAIL_OUT, errno);
	}
		
	if(! (get_interface_ipv4(sockfd, ifname, &ifr, curr_ipv4))) {
		(void)logmsg("Unable to obtain ipv4 for interface %s\n", ifname);
		(void)close(sockfd);
		(void)report_status_and_bail(BAIL_OUT, errno);
	} 
	else {
		(void)close(sockfd); 
	}

	/* -- add host relevant parameters 
	 * XXX ideally 'curr_ipv4' should be added during init
	 * a minor refactor should fix this 
	 */
	inet_pton(AF_INET, incoming_ip, &pr.rule.src.addr.v.a.addr.v4);
	inet_pton(AF_INET, curr_ipv4, &pr.rule.dst.addr.v.a.addr.v4); 
	pr.rule.dst.port[0] = htons(portnum);
		
	/* 
	 * move this to a seperate function 
	 */
	pf_dev_fd = open("/dev/pf", O_RDWR); 
	
	if(pf_dev_fd < 0) {
		(void)logmsg("failed to open /dev/pf, *not* adding rule\n");
		(void)logmsg("connection attempt from %s on port %d _NOT_ BLOCKED\n", incoming_ip, portnum);
		(void)report_status_and_bail(DO_NOT_BAIL, errno);
		(void)close(pf_dev_fd);
		ret = 0;
	}

	/* check the rule doesn't already exist in rules added so far */
	if(ret) {
		test_result = test_rule(incoming_ip, &pf_dev_fd, portnum, num_rules_added, opts->optionFlags);
	}

	if(test_result && ret) {
		if((ioctl(pf_dev_fd, DIOCBEGINADDRS, &paddr)) < 0) {
			(void)logmsg("err: ioctl DIOCBEGINADDRS");
		}

		pr.action = PF_CHANGE_GET_TICKET;
		pr.pool_ticket = paddr.ticket;
	
		if((ioctl(pf_dev_fd, DIOCCHANGERULE, &pr)) < 0) {
			(void)logmsg("err: ioctl DIOCCHANGERULE");
		}

		pr.action = PF_CHANGE_ADD_AFTER;
		pr.nr = num_rules_added;
		
		if((ioctl(pf_dev_fd, DIOCCHANGERULE, &pr)) < 0) {
			(void)logmsg("err: ioctl DIOCCHANGERULE");
		}

		num_rules_added++; /* static, see above */

		(void)logmsg("added blocking filter rule for remote host %s on port %d", incoming_ip, portnum); 
		
	} 
	else if(test_result == 0) {
		(void)logmsg("rule exists in active ruleset, ignoring");
	}
	else {
		/* this needs to be fixed 
		 * if for some reason we can't parse the active rulset then there is 
		 * a serious problem!
		 * we need to take action to reflect this issue
		 */
		(void)logmsg("connection %s on port %d", incoming_ip, portnum);
		(void)logmsg("ioctl: error searching current ruleset - *no* rule added");
	}
	(void)close(pf_dev_fd);
return ret;
}

/*
 * check we are not adding duplicate entries to the ruleset 
 * return 1 on unique 0 for duplicate -1 for error 
 * since we added after rule:0 we only parse the subesquent rules added 
 * by pwatch unless we are parsing the *entire* active ruleset 
 */
static int test_rule(char *incoming_ip, int *pfdev, int portnum, int num_rules_added, u_char flags)
{
	signed char ret = 1;
	unsigned register int i = 0;
	unsigned int port_from_rset = 0;
	struct pfioc_rule rset_chk;
	char *ip_from_rset = NULL;

	memset(&rset_chk, 0, sizeof(rset_chk));

	if((ioctl(*pfdev, DIOCGETRULES, &rset_chk)) < 0) {
		(void)logmsg("err: ioctl DIOCGETRULES in test_rule()");
		ret = PWATCH_ERR;
	}

	if(flags & PARSE_ALL) { /* parse entire ruleset */
		num_rules_added = rset_chk.nr; /* total number of rules in the active ruleset */
	}			

	if(ret) { /* true if we have ticket */ 
		for(i = 0; i < num_rules_added; i++) {
			rset_chk.nr = i;
			if((ioctl(*pfdev, DIOCGETRULE, &rset_chk)) < 0) {
				(void)logmsg("err: ioctl DIOCGETRULE in test_rule()");
				ret = PWATCH_ERR;
			}

			ip_from_rset = inet_ntoa(rset_chk.rule.src.addr.v.a.addr.v4);
			port_from_rset = htons(rset_chk.rule.dst.port[0]);

			if((strncmp(incoming_ip, ip_from_rset, strlen(incoming_ip)) == 0) \
				&& (portnum == port_from_rset)) {
					ret = 0;	
					break; /* ok, we got a match - exit loop and return */
			}
		}	
	}
return ret;
}

/*  socket for ioctl SIOCGIFADDR to get current interface ipv4 address */
static int get_dgram_socket() 
{
	int sockfd;
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
		(void)logmsg("failed to open dgram socket");
	}
return sockfd;
}

/**********************************
 * initialize the module 
 **********************************/
struct module_t *module_init() 
{
	mod_pf = (struct module_t *)pwatch_malloc(sizeof(struct module_t));
	pfCmds = (struct firewall_hooks_t *)pwatch_malloc(sizeof(struct firewall_hooks_t));

	memset(&pr, 0, sizeof(pr));
	memset(&pr.rule, 0, sizeof(pr.rule));
	
	/* init pf rule structure */
	pr.rule.action = PF_DROP;	
	pr.rule.quick = 1;
	pr.rule.log = 1;
	pr.rule.af = AF_INET;
	pr.rule.proto = IPPROTO_TCP;
	pr.rule.src.addr.type = PF_ADDR_ADDRMASK;
	memset(&pr.rule.src.addr.v.a.mask.v4, 255, 4); /* match only this host:port */
	pr.rule.src.addr.type = PF_ADDR_ADDRMASK;
	memset(&pr.rule.dst.addr.v.a.mask.v4, 255, 4);
	pr.rule.dst.port_op = PF_OP_EQ;
	pr.rule.keep_state = 0;
	pr.rule.flags = TH_SYN;
	pr.rule.flagset = (TH_SYN | TH_ACK | TH_FIN | TH_RST);

	mod_pf->moduleType = MOD_FIREWALL;
	mod_pf->commandStructure = pfCmds;

	/* module hook */
	pfCmds->addRule = pfAddRule;
return mod_pf;
}


/******************
 * cleanup routine
 ******************/
void module_exit() 
{
	memset(&pr, 0, sizeof(struct pfioc_rule));
	free(mod_pf);
	mod_pf = NULL;
	free(pfCmds);
	pfCmds = NULL;
}
