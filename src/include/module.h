#ifndef HDR_MODULE
#define HDR_MODULE

/* generic module initialization decl */
struct module_t *module_init(void);

/* module types */
#define MOD_PROTOCOL 0x01 /* tcp, udp etc */
#define MOD_FIREWALL 0x02 /* platform specific firewall rules */

/* generic module structure */
struct module_t {
	int moduleType; /* module type */
	void *commandStructure; /* hooks into module functionality */
	void *moduleSpecificData; /* do we really need this? */
};

#endif /* !_HDR_MODULE */
