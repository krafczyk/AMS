#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <typedefs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <iomanip.h>
void _convertl(int16u & w, int be);
void _convert(int16u * pdat, int length, int be);
void convert(int, int, char[], char[]);
int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime);
int main(int argc, char* argv[]){
  switch (argc) {
  case 1:
    cerr <<"cdaq-F-Please give an input file name as first parameter "<<endl;
    return 1;
  case 2:
    cerr <<"cdaq-F-Please give an output file name as last parameter "<<endl;
    return 1;
  default:
   break;
  }
  int i,k,ok;
   for(i=1;i<argc-1;i++){
     cout <<"cdaq-I-Open I file "<<argv[i]<<endl;
     convert(i-1,argc-i-2,argv[i],argv[argc-1]);
   }
   return 0;
}


void convert(int ibeg, int iend,char in[], char out[]){
integer BigEndian=0;
static ofstream fbout;
static int16u Record[100000];
int16u pData[24][1536];
  enum open_mode{binary=0x80};
  if(ibeg==0){
    fbout.open(out,ios::out|binary|ios::app);
      if(fbout){
       cout <<"cdaq-I-Open O file "<<out<<endl;
      }
      else {
        cerr <<"cdaq=F-CouldNotOPenFileO "<<out<<endl;
        exit(1);
      }




       integer b64=0;
       if(sizeof(ulong)>sizeof(uinteger))b64=1;
       uinteger test1,test2;
       test1=1;
       test1+=2<<8;
       test1+=4<<16;
       test1+=8<<24;
       unsigned char *pt= (unsigned char*)&test1;
       test2=pt[0];
       test2+=pt[1]<<8;
       test2+=pt[2]<<16;
       test2+=pt[3]<<24;
       integer lend = test1==test2;
       if(lend)cout <<"AMSTrIdSoftI-I-Identified as LittleEndian";
       else {
         cout <<"AMSTrIdSoftI-I-Identified as BigEndian";
         BigEndian=1;
       }
       if(b64)cout <<" 64 bit machine."<<endl;
       else cout <<" 32 bit machine."<<endl;
      






  }
    
    FILE * fbin=fopen(in,"r");
    static int gevt=0;
    int tevt=0;
    int evt;
    char name[80];
    strncpy(name,in+strlen(in)-9,5);
    int run =atoi(name);    
    cout <<"run "<<run<<endl;
    long id, size, flags;
    time_t time;
    int suc=UnixFilestat(in, &id, &size,&flags, &time);
    cout <<ctime(&time);
    int ie;
      if(fbin){
       cout <<"cdaq-I-Open I file "<<in<<endl;
      }
      else {
        cerr <<"cdaq=F-CouldNotOPenFileI "<<in<<endl;
        exit(1);
      }
       do {
       int l16ptr=0;
       int tlength,ntdr,len,tdrn,ch1,ch2,elem;
       ie=fscanf(fbin,"%x",&tlength);
       l16ptr++;
       ie=fscanf(fbin,"%x",&ntdr);
       l16ptr++;
       //         cout << " tlength "<<tlength <<endl;
       //         cout << " ntdr "<<ntdr <<endl;
       if(ie!=EOF && ntdr>0){
       }
       else{
         
         break;
       }
       do{
       ie=fscanf(fbin,"%x",&tdrn);
       ie=fscanf(fbin,"%x",&len);
       ie=fscanf(fbin,"%x",&evt);
       //         cout << " ntdr "<<ntdr <<" "<<tdrn<<" "<<len<<" "<<evt<<endl;
         assert(len-4 <= 1536);
         for(int i=0;i<len-4;i++){
          ie=fscanf(fbin,"%x",&elem);
          pData[tdrn][i]=elem;
         }                
          ie=fscanf(fbin,"%x",&ch1);
          ie=fscanf(fbin,"%x",&ch2);
          l16ptr+=len+1;         
       }while (l16ptr<tlength);
       if(tdrn == 23){
         // read whole event; try to write it
         Record[1]=0x0;
         Record[2]=9;
         Record[3]=0x200;
         Record[4]=run&65535;
         Record[5]=(run>>16)&65535;
         Record[6]=0;
         Record[7]=evt&65535;
         Record[8]=(evt>>16)&65535;
         Record[9]=time&65535;
         Record[10]=(time>>16)&65535;
         Record[11]=0;
         Record[12]=32*(640+1)+32*(384+1)+1;
         Record[0]=Record[12]+1+Record[2]+1+1;
         Record[13]= 2<<6 | 11 <<9;    // Crate 32; 5 for 72
         int frp=14;
         for (int k=0;k<16;k++){
           for(int j=0;j<2;j++){
             // make address
             int16u conn, tdrs;
             if(k%2 ==0 ){
               if(j==0)conn=0;
               else conn=1; 
             }             
             else{
               if(j==0)conn=2;
               else conn=3; 
             }
             tdrs=k/2;
             int16u addr=(conn<<10) | (tdrs <<12);
             Record[frp]=addr;
             frp++;
             int lj;
             if(j==0)lj=3;
             else lj=1;
             for(int l=0;l<320;l++){
              Record[frp]=pData[k][lj*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
             }
             if(j==0)lj=2;
             else lj=0;
             for( l=0;l<320;l++){
              Record[frp]=pData[k][lj*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
             }
           }
         }
         for(k=16;k<24;k++){
           for(int j=0;j<4;j++){
             // make address
             int16u conn, tdrk;
             if(k%2 ==0){
              if(j<2)conn=j+2;
              else conn=j-2;
             }
             else {
              if(j<2)conn=j+2;
              else conn=j-2;
               conn+= 4;
             }
             tdrk=(k-16)/2;
             //swap 3.2 <-> 3.0
             if(run >=21 && tdrk == 2){
              if(conn == 0)conn=3;
              else if (conn==3)conn=0;  
             }
             int16u addr=(10<<6) |( conn<<10) | (tdrk <<13);
               
             Record[frp]=addr;
             frp++;
             for(int l=0;l<384;l++){
              Record[frp]=pData[k][j*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
              
             }
           }
         }
         int tl=Record[0]+1;
              //  Dump every 1000 event
         //                  if(evt%1000 == 0){
         //                    for(int itl=0;itl<tl;itl++){
         //                      cout <<itl <<" "<<Record[itl]<<endl;
         //                    }
         //                  }
         _convert(Record,tl,BigEndian);
         if(tevt!=0)fbout.write((unsigned char*)Record,tl*sizeof(Record[0]));  
         tevt++;
       }
      }while (ie!=EOF);
      
  fclose(fbin);
  gevt+=tevt-1;
  if(iend==0){
      fbout.close();
      cout <<" Total of "<<gevt<<" events  converted"<<endl;
  }
}






int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime)
{
   // Get info about a file: id, size, flags, modification time.
   // Id      is (statbuf.st_dev << 24) + statbuf.st_ino
   // Size    is the file size
   // Flags   is file type: bit 0 set executable, bit 1 set directory,
   //                       bit 2 set regular file
   // Modtime is modification time
   // The function returns 0 in case of success and 1 if the file could
   // not be stat'ed.
   //
   struct stat statbuf;

   if (path != 0 && stat(path, &statbuf) >= 0) {
      if (id)
         *id = (statbuf.st_dev << 24) + statbuf.st_ino;
      if (size)
         *size = statbuf.st_size;
      if (modtime)
         *modtime = statbuf.st_mtime;
      if (flags) {
         *flags = 0;
         if (statbuf.st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
            *flags |= 1;
         if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            *flags |= 2;
         if ((statbuf.st_mode & S_IFMT) != S_IFREG &&
             (statbuf.st_mode & S_IFMT) != S_IFDIR)
            *flags |= 4;
      }
      return 0;
   }
   return 1;
}





void _convertl(int16u & l16, int BigEndian){
  if(BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(&l16);
     tmp=*pc;
     *pc=*(pc+1);
     *(pc+1)=tmp;
  }
}


void _convert(int16u * _pData,int _Length, int BigEndian){





  if(BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(_pData);
   int i;
   for(i=0;i<_Length;i++){
     tmp=*pc;
     *pc=*(pc+1);
     *(pc+1)=tmp;
     pc=pc+sizeof(int16u);
   }
  }
}
