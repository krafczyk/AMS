#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdio.h>

#define buf_n 200 /* number of file names buffered */

typedef char ch80[80];

int last_file_read;
char * CAS_dir, * Coo_db_dir, last_file_n[80], chb80[80];
char last_name[80]="last.file";
int new_files;
time_t file_t, last_file_time;

ch80    new_file_n[buf_n];
time_t  new_file_t[buf_n];
int new_file_c; /* how many new files */

/* opens a CAS file */
int new_files_coming() {
  FILE *last_f;
  char chbuf80[80], last_file_n[80];
  time_t last_file_time;
  DIR *dirp;
  struct dirent *dp;
  struct stat statf;
  time_t tmin=999999999;
  new_files=0;

  /* reads the last read file name */
  last_f=fopen(last_name,"r");
  if (last_f == NULL) {
    last_file_time = 0;
  }
  else {
    fgets(chb80,80,last_f);
    if (&chb80 == NULL) {
      printf("ERROR:Coo_file: file %s corrupted. Stop\n",last_name);
      exit(0);
    }
    sprintf(last_file_n,"%s",chb80);
    fgets(chb80,80,last_f);
    last_file_time=atol(chb80);
    fclose(last_f);
  }
  /* list directory */
  if ((dirp = opendir(CAS_dir)) == NULL) {
    printf( "Coo_file: open (%s) fails. Stop. \nCheck CASDir set \n", CAS_dir);
    exit(0);
  }
  new_file_c=-1;

  while ((dp = readdir( dirp)) != NULL) {
    if (fnmatch( "[0-9][0-9][0-9][0-9][0-9][0-9]", dp->d_name, 0) == 0) {
      sprintf(chbuf80,"%s/%s",CAS_dir,dp->d_name);
      stat(chbuf80,&statf);

      file_t = statf.st_mtime;
      if ((file_t>last_file_time) && (file_t<tmin)) {

	new_file_c++;
	new_file_t[new_file_c]=file_t;
	sprintf(new_file_n[new_file_c],"%s",dp->d_name);
	new_files=1;
	if (new_file_c >= (buf_n-1))
	  return(new_files);
      }
    }
  }
  return(new_files);
}

FILE * open_file(int i) {
char chbuf[80];
FILE *fp;

  sprintf(chbuf,"%s/%s",CAS_dir,new_file_n[i]);
  fp=fopen(chbuf,"r");
  if (fp == NULL) {
    printf("Warning: CAS file %s cannot be open\n",new_file_n[i]);
    return(NULL);
  }
  return(fp);
}
