#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include <cfortran.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
//
 PROTOCCALLSFFUN3(INT,IFNTREAD,ifntread,STRING,INT,INT)
#define IFNTREAD(A2,A3,A4)  CCALLSFFUN3(IFNTREAD,ifntread,STRING,INT,INT,A2,A3,A4)
//
//-----------------------------------------------------------------|
//output:a) Bit 8 is set -> bad ntuple(+bits (1-7)=1->open problem;|
//            =2->read probl; =3->NevIn>NevRead; =4->NevIn<NevRead;| 
//            =5->miss.arguments)                                  |
//       b) Bit 8 is not set --> bits 1-7 give % of events with    |
//          EventStatus=bad;                                     |
//-----------------------------------------------------------------|
//
 int main(int argc, char * argv[])
 {
//
   int iflg;
   char fname[256];
//
   bool verbose=false;
   int iver=0;
   iflg=0;
   if(argc>2){
     strcpy(fname,argv[1]);
     int nevents=atoi(argv[2]);
     if(argc>3){
      verbose=true;
      iver=1;
     }
//     cout<<"Requested file: "<<fname<<" imply "<<nevents<<" events"<<endl;
     iflg=IFNTREAD(fname,nevents,iver);
     if(iflg>=0){
//       float proc;
//       proc=float(iflg%128);
//       cout<<" proc="<<proc<<endl;
     }
     else{
       iflg=abs(iflg)+128; // abs(-1/-2/-3/-4) + bit 8 for "-"
     }
//     cout<<"Flag="<<iflg<<endl;
   }
   else
   {
//     cout<<"FastNtupleRead-Error: missing arguments ???"<<endl;
     iflg=5+128; // abs(-5) + bit 8 for "-"
   }
     if(verbose)cout<<"Flag="<<iflg<<endl;
     return(iflg);
 }
