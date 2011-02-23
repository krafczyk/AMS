
#include <stdio.h>
#include <string.h>
#include "xmlrpclib.h"
#define NAME "XML-RPC AMI client"
#define VERSION "0.1"

#ifdef TEST
int xmlrpc_env_init(xmlrpc_env *){return 0;}
int xmlrpc_client_setup_global_const(xmlrpc_env *){return 0;}
int xmlrpc_client_create(xmlrpc_env *,int,char *, char *, char *, int, 
	xmlrpc_client **){return 0;}
int xmlrpc_env_clean(xmlrpc_env *){return 0;}
int xmlrpc_client_destroy(xmlrpc_client *){return 0;}
int xmlrpc_client_teardown_global_const(){return 0;}
xmlrpc_value * xmlrpc_array_new(xmlrpc_env *){return NULL;}
int xmlrpc_string_new(xmlrpc_env *,char *){return 0;}
int xmlrpc_array_append_item(xmlrpc_env *, xmlrpc_value *, int){return 0;}
int xmlrpc_client_call2f(xmlrpc_env *, xmlrpc_client *, char *, char *, 
	xmlrpc_value **, char *, char *, char *, char *, char *, 
	xmlrpc_value *){return 0;}
int xmlrpc_DECREF(xmlrpc_value *){return 0;}
int xmlrpc_read_string(xmlrpc_env *, xmlrpc_value *, const char **){return 0;}
#endif

int fault_occurred (xmlrpc_env *env) {
#ifndef TEST
    if (env->fault_occurred) {
        fprintf(stderr, "XML-RPC Fault: %s (%d)\n",
                env->fault_string, env->fault_code);
        return 1;
    }
#endif
    return 0;
}

int xmlrpc_init(xmlrpcstr_t *s,char *url,int debug){
#ifdef TEST
	return 0;
#else
	xmlrpc_env_init(&(s->env));
	xmlrpc_client_setup_global_const(&(s->env));
	xmlrpc_client_create(&(s->env),
		XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION, NULL, 0, &(s->cli));
	s->debug = debug;
	strncpy(s->url,url,URLLEN-1);
	s->url[(URLLEN-1)] = '\0';
	if(debug) printf("AMI using url: %s\n",s->url);
	return fault_occurred(&(s->env));
#endif
}

int xmlrpc_cleanup(xmlrpcstr_t *s){
#ifndef TEST
	xmlrpc_env_clean(&(s->env));
	xmlrpc_client_destroy(s->cli);
	xmlrpc_client_teardown_global_const();
#endif
	return 0;
}

/* dtime is seconds from epoch */
int __AMI_insert_raw_value(xmlrpcstr_t *s,time_t time,char *node_number,
	char *data_type, int *vals, int offset,int len){

	int i;
#ifdef TEST
	fprintf(stderr,"*** FAKE insert into DB ***\n");
	fprintf(stderr,"time: %d, node_number: %s ",(int)time,node_number);
	fprintf(stderr,"data_type: %s\n",data_type);
	for(i=0; i<len; i++){
		fprintf(stderr,"vals[%d]: %d\n",i,vals[i]);
	}
	return 0;
#else
	char *const method_name = "insert_raw_value_with_offset";
	char str[80];
	const char *res;
	char dtime[15],off[10];
	xmlrpc_value *array,*result;

	array = xmlrpc_array_new(&(s->env));
	for(i=0; i<len; i++){
		sprintf(str,"%d",vals[i]);
		xmlrpc_array_append_item(&(s->env), array, 
			xmlrpc_string_new(&(s->env),str));
	}
	/* convert time to unix seconds since epoch */
	sprintf(dtime,"%ld",time);
	/* convert offset */
	sprintf(off,"%d",offset);
	/* Make the remote procedure call */
	xmlrpc_client_call2f(&(s->env), s->cli, s->url, method_name, &result,
            "(ssssA)", dtime, node_number, data_type, off,array);
	if(fault_occurred(&(s->env))){
		xmlrpc_DECREF(result);
		return -1;
	}
	xmlrpc_read_string(&(s->env), result, &res);
	if(fault_occurred(&(s->env))){
		xmlrpc_DECREF(result);
		return -1;
	}
	if(res[0] == 'O' && res[1] == 'K') return 0;
	else fprintf(stderr,"AMI returns: %s\n",res);
	return 1;
#endif
}

int __AMI_insert_raw_value_with_timestamps(xmlrpcstr_t *s,time_t *time,
	char *node_number, char *data_type, int *vals, int offset,int len){

	int i;
#ifdef TEST
	fprintf(stderr,"*** FAKE insert into DB ***\n");
	fprintf(stderr,"node_number: %s ",node_number);
	fprintf(stderr,"data_type: %s\n",data_type);
	for(i=0; i<len; i++){
		fprintf(stderr,"time: %d vals[%d]: %d",(int)time[i],i,vals[i]);
		if(!time[i]) fprintf(stderr," (will be skipped)");
		fprintf(stderr,"\n");
	}
	return 0;
#else
	char *const method_name = "insert_raw_value_with_offset";
	char str[80];
	const char *res;
	char off[10];
	xmlrpc_value *tarray,*array,*result;

	array = xmlrpc_array_new(&(s->env));
	tarray = xmlrpc_array_new(&(s->env));
	for(i=0; i<len; i++){
		sprintf(str,"%d",vals[i]);
		xmlrpc_array_append_item(&(s->env), array, 
			xmlrpc_string_new(&(s->env),str));
		sprintf(str,"%ld",(long int)time[i]);
		xmlrpc_array_append_item(&(s->env), tarray, 
			xmlrpc_string_new(&(s->env),str));
	}
	/* convert offset */
	sprintf(off,"%d",offset);
	/* Make the remote procedure call */
	xmlrpc_client_call2f(&(s->env), s->cli, s->url, method_name, &result,
            "(AsssA)", tarray, node_number, data_type, off,array);
	if(fault_occurred(&(s->env))){
		xmlrpc_DECREF(result);
		return -1;
	}
	xmlrpc_read_string(&(s->env), result, &res);
	if(fault_occurred(&(s->env))){
		xmlrpc_DECREF(result);
		return -1;
	}
	if(res[0] == 'O' && res[1] == 'K') return 0;
	else fprintf(stderr,"AMI returns: %s\n",res);
	return 1;
#endif
}

int AMI_insert_raw_value_with_offset(xmlrpcstr_t *s,time_t time,
	char *node_number, char *data_type, int *vals, int offset, int len){
	return __AMI_insert_raw_value(s,time,node_number,data_type,vals,offset,
		len);
}

int AMI_insert_raw_value(xmlrpcstr_t *s,time_t time,
	char *node_number, char *data_type, int *vals, int len){
	return __AMI_insert_raw_value(s,time,node_number,data_type,vals,0,len);
}

int AMI_insert_raw_value_with_timestamps(xmlrpcstr_t *s,time_t *time,
	char *node_number, char *data_type, int *vals, int offset, int len){
	return __AMI_insert_raw_value_with_timestamps(s,time,node_number,
		data_type,vals,offset,len);
}

