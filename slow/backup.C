/* Directory backup from Dir1 Dir2 */
#include <iostream.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>
#include <time.h>
int AMSbi(char *a[], char * obj, int i);
int _sort(dirent ** d1, dirent ** d2);
int backup(char * dir1 , char * dir2);
int main (int argc, char *argv[]) {
int delay = 120; /* dlay in minutes */
char *dir1, *dir2;

  if (argc > 1) {
    delay = atoi(argv[1]);
  }
  else {
    cout <<"backup-W-DelayTimeIsNotSet. Will use default delay "<<delay<<" minutes"<<endl;
  }
  delay *= 60;
  
  while (1) {/* main loop */

    

    dir1=getenv("DirFrom");
    if (dir1==NULL) {
      puts("setenv DirFrom and try again.Stop");
      exit(1);
    }
    dir2=getenv("DirTo");
    if (dir2==NULL) {
      puts("setenv DirTo and try again.Stop");
      exit(1);
    }
    int succ=backup(dir1,dir2);

    if(!succ){
       time_t Time;
       time(&Time);
       cerr<<"backup-E-backupErrorAt  "<<ctime(&Time)<<endl; 
    }
    sleep(delay);

  }


}
int _sort(dirent ** d1, dirent **d2){
  return strcmp( (*d1)->d_name, (*d2)->d_name);
}

int backup(char * dir1, char * dir2){
    struct stat statbuf1,statbuf2;
    int i;
   char buf[1024];
   char buf1[256];
   char buf2[256];
   dirent ** namelist1;
   dirent ** namelist2;
   char ** ar1;
   char ** ar2;
   int l1 = scandir(dir1, &namelist1,NULL,&_sort);
    if (l1<0) {
      cout <<"Backup-F-CanNotScanDir "<<dir1<<endl;
      exit(1);
    }
    ar1= new char*[l1];
    for(i=0;i< l1;i++)ar1[i]=namelist1[i]->d_name;
    int l2 = scandir(dir2, &namelist2,NULL,&_sort);
    if (l2<0) {
      strcpy(buf,"mkdir ");
      strcat(buf,dir2);
      system(buf);
      l2 = scandir(dir2, &namelist2,NULL,&_sort);
      if (l2<0){
        cout <<"Backup-F-CanNotCreateDir "<<dir2<<endl; 
        exit(1);
      }
    }
    ar2= new char*[l2];
    for(i=0;i< l2;i++)ar2[i]=namelist2[i]->d_name;
    // first scand dir2 and remove unnecessary files
    for(i=0;i<l2;i++){
        if(AMSbi(ar1,namelist2[i]->d_name,l1)!=-1){
           // delete it
           strcpy(buf,"rm -rf ");
           strcat(buf,dir2);
           strcat(buf,"/");
           strcat(buf,namelist2[i]->d_name);
           system(buf);
        }
    }        
    // scan now dir1
     for (i=0; i<l1; i++) {
        if(AMSbi(ar2,namelist1[i]->d_name,l2)==-1){
            //entry found lets look more closely
           strcpy(buf1,dir1);
           strcat(buf1,"/");
           strcat(buf1,namelist1[i]->d_name);
           strcpy(buf2,dir2);
           strcat(buf2,"/");
           strcat(buf2,namelist1[i]->d_name);
            int ok1=stat(buf1,&statbuf1);
            int ok2=stat(buf2,&statbuf2);
            if(ok1 || ok2){
              if(ok1)cerr<<"backup-E-StatRejected for "<<buf1<<endl;
              if(ok2)cerr<<"backup-E-StatRejected for "<<buf2<<endl;
            }
            else{
               if(statbuf1.st_mtime>statbuf2.st_mtime){
                  //do smth
                  // check first dir status
                  int d1=(statbuf1.st_mode & S_IFMT)==S_IFDIR?1:0;
                  int d2=(statbuf2.st_mode & S_IFMT)==S_IFDIR?1:0;
                   if(d1){
                     if(d2)backup(buf1,buf2);
                     else{
                       strcpy(buf,"rm -rf ");
                       strcat(buf,buf2);
                       system(buf);
                       strcpy(buf,"cp -R");
                       strcat(buf,buf1);
                       strcat(buf, " ");
                       strcat(buf,buf2);  
                       system(buf);
                     }
                   }
                   else{
                    if(d2){
                       strcpy(buf,"rm -rf ");
                       strcat(buf,buf2);
                       system(buf);
                    }
                    strcpy(buf,"cp -R");
                    strcat(buf,buf1);
                    strcat(buf, " ");
                    strcat(buf,buf2);  
                    system(buf);
                    
                   }
               } 
            }
        }
        else {
           // just copy
           strcpy(buf,"cp -R ");
           strcat(buf,dir1);
           strcat(buf,"/");
           strcat(buf,namelist1[i]->d_name);
           strcat(buf," ");
           strcat(buf,dir2);
           strcat(buf,"/");
           strcat(buf,namelist1[i]->d_name);
           system(buf);
        }   

      }
    delete[] ar1;
    delete[] ar2;
    for(i=0;i<l1;i++){
      free(namelist1[i]);
    }
    for(i=0;i<l2;i++){
      free(namelist2[i]);
    }
    free(namelist1);
    free(namelist2);
    return 1;
}



 int AMSbi(char* a[], char * obj, int i){
//
// binary search
// input T *a[]   // adresses pf array to search
// T& obj object
// i array size
// output :
//  -1              obj found
//  any other       min {*a[]} with obj<*a[output]
//
int ia=0;
int il=1;
int ir=1;
int ib=i-1;
int j=0;
while(ia<ib-1){
int k=(ia+ib)/2;
int i=strcmp(obj,a[k]);
if(!i)return -1;
else if(i>0){
ia=k;
il=0;
               }
else  {
ib=k;
ir=0;
}
}
if(ir){
j=strcmp(obj,a[ib]);
if(!j)return -1;
else if(j>0)return ib+1;
}
else if(il){
j=strcmp(obj,a[ia]);
if(!j)return -1;
else if(j<0)return ia;
}
return ib;
}