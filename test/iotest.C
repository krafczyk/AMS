#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
void main(){
double arr[10];
char fnam[128]="datafile";
  ifstream iftxt(fnam,ios::in);
   if(!iftxt){
     cerr <<"Error open"<<endl;
     exit(1);
   }
   for(int i=0;i<10;i++){
    iftxt >> arr[i];
    if(iftxt.eof() && i<9){
      cerr<< "Unexpected eof"<<endl;
      exit(1);
    }
      }
 for ( i=0;i<10;i++)cout<<i<<" "<<arr[i]<<endl;
iftxt.close();
 
   }
