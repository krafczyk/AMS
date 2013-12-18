#include <iostream.h>
#include <fstream.h>
int main(int argc, char* argv[]){


  switch (argc) {
  case 1:
    cerr <<"readst-F-Please give a tdv name as first parameter "<<endl;
    return 1;
  default:
   break;
  }

      ifstream fbin;

     fbin.open(argv[1],ios::in);
unsigned int i,b,e;
unsigned int run=0;
unsigned long long offset=0;
unsigned int nelem;
unsigned int fevent;
unsigned int len;
     if(fbin){
fbin.read((char*)(&len),sizeof(int));
fbin.read((char*)(&i),sizeof(int));
fbin.read((char*)(&b),sizeof(int));
fbin.read((char*)(&e),sizeof(int));
fbin.read((char*)(&run),sizeof(int));
fbin.read((char*)(&offset),sizeof(unsigned long long));
fbin.read((char*)(&nelem),sizeof(int));
fbin.read((char*)(&nelem),sizeof(int));
fbin.read((char*)(&fevent),sizeof(int));
cout<<"readst-I-"<<run<<"-"<<offset<<"-"<<nelem<<"-"<<fevent<<" "<<endl;
return 0;
}
else {
cout<<"readst-F-FILECANNOTBEOPEN"<<endl;
return 1;
}

}
