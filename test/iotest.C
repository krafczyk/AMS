//  $Id: iotest.C,v 1.3 2001/01/22 17:32:55 choutko Exp $
#include <iostream>
#include <fstream.h>
#include <stdlib.h>
#include <vector>
void main(){
double arr[10];
char fnam[128]="datafile";
  std::ifstream iftxt(fnam,ios::in);
   if(!iftxt){
     cerr <<"Error open"<<endl;
     exit(1);
   }
   for(int i=0;i<10;i++){
    iftxt >> arr[i];
    if(iftxt.eof() && i<9){
      std::cerr<< "Unexpected eof"<<endl;
      exit(1);
    }
      }
 for ( i=0;i<10;i++)cout<<i<<" "<<arr[i]<<endl;
iftxt.close();
 
   }
