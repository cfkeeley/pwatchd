/* module: tcp 
 * author: chris keeley 
 * date: 
 */
#include "../../../include/tcp_init.h"
#include "../../../include/module.h"
#include "../../../include/protocol.h"

struct module_t *tcp;
struct protocl_hooks_t *tcpHooks;



/* 
 * this may be able to be refactored and included in the module 
 * 
 */
/*
void tcp_init(struct configFileVars *opts, int *sockfd_array)  
{
	register int i;
	int tempSockFd;

	if(opts->optionFlags & LISTEN_TCP) {
		(void)fprintf(stderr, "=> binding tcp sockets\n");
		for(i = 0; i < opts->tcpPortsCounter; i++) {
			tempSockFd = get_stream_sockfd(opts->optionFlags);
			if(tempSockFd > 0) {
				sockfd_array[opts->socketsBound] = tempSockFd;
			}

			if(! (bindsockfd(sockfd_array[opts->socketsBound], opts->tcpPorts[i]))) {
				opts->failedPorts[opts->failures] = opts->tcpPorts[i];
				opts->failures++;
			} 
			else {
				opts->socketsBound++;
			}
		}

		(void)free(opts->tcpPorts);
		opts->tcpPorts = NULL;

		if(opts->socketsBound == 0) {
			(void)fprintf(stderr, "=> error: failed to bind any sockets, quitting\n");
			(void)logmsg("err: failed to bind any sockets, quitting");
			exit(1);
		} 
		else {
			(void)fprintf(stderr, "=> tcp sockets bound: %d/%d\n", opts->socketsBound, opts->tcpPortsCounter);
			(void)logmsg("bound %d/%d sockets", opts->socketsBound, opts->tcpPortsCounter);
			if(opts->failures) {
				(void)fprintf(stderr, "=> bind failed on port:");
				for(i = 0; i < opts->failures; i++) {
					(void)fprintf(stderr, " %d", opts->failedPorts[i]);
				}
				(void)fprintf(stderr, "\n");
			}
		}

		(void)free(opts->failedPorts);
		opts->failedPorts = NULL;
	}
}
*/


struct module_t *module_init() 
{

}

void module_exit() 
{

}
