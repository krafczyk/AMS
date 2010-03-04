

#include <curl/curl.h>

typedef struct fcurl_data URL_FILE;

extern "C" URL_FILE *url_fopen(const char *url,const char *operation);
extern "C" int url_fclose(URL_FILE *file);
extern "C" int url_feof(URL_FILE *file);
extern "C" size_t url_fread(void *ptr, size_t size, size_t nmemb, URL_FILE *file);
extern "C" char * url_fgets(char *ptr, int size, URL_FILE *file);
extern "C" void url_rewind(URL_FILE *file);
extern "C" int url_stat(const char *url, struct stat *buf);
