/* Directory backup from Dir1 Dir2 */

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>

int main (int argc, char *argv[]) {
int delay = 120; /* dlay in minutes */
char *dir1, *dir2;
struct dirent ** name_list1;
struct dirent ** name_list2;
int length1, length2,i,j,copied_files, comp;
char chbuf1[120], chbuf2[120];
time_t cur_time;

  if (argc > 1) {
    delay = atoi(argv[1]);
  }
  else {
    printf("delay time is not set. Use default %d minutes instead\n",delay);
  }
  delay *= 60;
  
  while (1) {/* main loop */

    copied_files=0;

    dir1=getenv("DirFrom");
    if (dir1==NULL) {
      puts("setenv DirFrom and try again.Stop");
      exit(0);
    }
    dir2=getenv("DirTo");
    if (dir2==NULL) {
      puts("setenv DirTo and try again.Stop");
      exit(1);
    }

    length1 = scandir(dir1, &name_list1,NULL,NULL);
    if (length1<0) {
      printf("Cannot open directory %s. Stop\n",dir1);
      exit(0);
    }
    length2 = scandir(dir2, &name_list2,NULL,NULL);
    if (length2<0) {
      printf("Cannot open directory %s. Stop\n",dir2);
      exit(0);
    }
    
    sleep(10);
    
    for (i=0; i<length1; i++) {
      comp=1;
      for (j=0; j<length2; j++) {
	if ((strncmp(name_list1[i]->d_name, name_list2[j]->d_name,80))==0) {
	  comp=0;
	  j=length2;
	}
      }
      if (comp) {
	sprintf(chbuf1,"cp %s/%s %s/",dir1,name_list1[i]->d_name,dir2);
	system(chbuf1);
	copied_files++;
      }
    }
    time(&cur_time);
    printf("%s %d files updated\n",ctime(&cur_time),copied_files);

    free(name_list1);
    free(name_list2);
    sleep(delay);
  }
  return(1);
}

