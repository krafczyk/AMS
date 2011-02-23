#ifndef XMLRPCLIB_H
#define XMLRPCLIB_H

#ifndef TEST
 #include <xmlrpc-c/base.h>
 #include <xmlrpc-c/client.h>
#else
#include <time.h>
struct xmlrpc_env_s {
	int fault_occurred;
	char *fault_string;
	int fault_code;
};
typedef struct xmlrpc_env_s xmlrpc_env;
typedef char xmlrpc_client;
typedef char xmlrpc_value;
#define XMLRPC_CLIENT_NO_FLAGS 0
int xmlrpc_env_init(xmlrpc_env *);
int xmlrpc_client_setup_global_const(xmlrpc_env *);
int xmlrpc_client_create(xmlrpc_env *,int,char *, char *, char *, int, 
	xmlrpc_client **);
int xmlrpc_env_clean(xmlrpc_env *);
int xmlrpc_client_destroy(xmlrpc_client *);
int xmlrpc_client_teardown_global_const();
xmlrpc_value * xmlrpc_array_new(xmlrpc_env *);
int xmlrpc_string_new(xmlrpc_env *,char *);
int xmlrpc_array_append_item(xmlrpc_env *, xmlrpc_value *, int);
int xmlrpc_client_call2f(xmlrpc_env *, xmlrpc_client *, char *, char *, 
	xmlrpc_value **, char *, char *, char *, char *, char *, 
	xmlrpc_value *);
int xmlrpc_DECREF(xmlrpc_value *);
int xmlrpc_read_string(xmlrpc_env *, xmlrpc_value *, const char **);
#endif

#define URLLEN 200

struct xmlrpcstr {
	xmlrpc_env env;
	xmlrpc_client *cli;
	int debug;
	char url[URLLEN];
};
typedef struct xmlrpcstr xmlrpcstr_t; 

int xmlrpc_init(xmlrpcstr_t *s,char *url,int debug);
int xmlrpc_cleanup(xmlrpcstr_t *s);

int AMI_insert_raw_value(xmlrpcstr_t *s,time_t time,char *node_number,
	char *data_type, int *vals, int len);
int AMI_insert_raw_value_with_offset(xmlrpcstr_t *s,time_t time,
	char *node_number, char *data_type, int *vals, int offset, int len);
int AMI_insert_raw_value_with_timestamps(xmlrpcstr_t *s,time_t *time,
	char *node_number, char *data_type, int *vals, int offset, int len);

#endif /* XMLRPCLIB_H */
