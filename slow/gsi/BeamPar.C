//  $Id: BeamPar.C,v 1.6 2010/02/17 12:06:59 choutko Exp $
#include <typedefs.h>
#include <cern.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <iostream.h>
#include <fstream.h>
const int NWPAW=700000;
struct PAWC_DEF{
float q[NWPAW];
};
#define PAWC COMMON_BLOCK(PAWC,pawc)
#define N_bad 500   // maximum number of bad CAS blocks

COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
uint _CalcCRC(uint *pData, int length);
class BeamParEntry {
 public:
  uint Run;       //run
  uint RunTag;   //runTag
  time_t Time;       //run
  float X;       //mm, the beam cross in AMS coo system
  float Y;       //mm
  float Z;       //mm
  float Theta;   //rad [0-Pi], direction of the beam (Pi- from z-direction)
  float Phi;     //rad [0-2Pi]
  float ux;
  float uy;
  float uz;
  uint  Pid;     //Beam particle ID  == Geant Pid
  float Mom;     //Beam particle momentum (GeV/c) - 0-default
  float Intens;  //Beam intensity (part/sec), 0-default
  uint  Position;    // position number (1-60) tracker (10001-..) ecal1 (20001-...)ecal2
  float Size;    //beam size in cm;
  uint Good;
  char current[255] ; //magnet current
  char label[255]; //label
  char comment[1024];
 };
class BeamPar {
 public:
  uint RunTag;   //runTag
  time_t Time;   //UNIX sec, of the manipulator PC
  float X;       //cm, the beam cross in AMS coo system
  float Y;       //cm
  float Z;       //cm
  float Theta;   //rad [0-Pi], direction of the beam (Pi- from z-direction)
  float Phi;     //rad [0-2Pi]
  uint  Pid;     //Beam particle ID  == Geant Pid
  float Mom;     //Beam particle momentum (GeV/c) - 0-default
  float Intens;  //Beam intensity (part/sec), 0-default
  uint  Position;    // position number
  float Size;    //beam size in cm;
 };
   void beam_coo_calc(BeamParEntry & record);
   static BeamPar record[60];
   
   main(){

        int iostat,rsize=1024;
        HLIMIT(NWPAW);
        HROPEN(3,"output","beam","N",rsize,iostat);
           if(iostat){
             cout << "Error opening Histo file beam"<<endl;
             exit(1);
           }
           else cout <<"Histo file "<<"beam"<<" opened."<<endl;

              HBOOK1(101,"td",400,-38.5,361.5,0.);
    char prod[]="/f2users/choutko/TestBeamLogbook.csv";
    char junk[256];
    char junk1[256];
    char junk2[256];
    ifstream fref;
    fref.open(prod);
    int nlineRef=0;
    while(1){
     fref >> junk;
     fref.ignore(INT_MAX,'\n');
     if(fref.eof())break;
     nlineRef++;
    }      

    fref.close();
    fref.open(prod);
    BeamParEntry * BeamRef = new BeamParEntry[nlineRef];  
    nlineRef=0;
    while(1){
     fref >>junk;
     if(fref.eof())break;
     sscanf(junk,"%d",&BeamRef[nlineRef].Run);
     BeamRef[nlineRef].Time=BeamRef[nlineRef].Run-1;
     fref >>junk;
     fref >>junk;
     sscanf(junk,"'%x'",&BeamRef[nlineRef].RunTag);
     fref >>junk;
     sscanf(junk,"%s",BeamRef[nlineRef].label);
     fref >>BeamRef[nlineRef].Position;
     fref >>junk;
     sscanf(junk,"%s",BeamRef[nlineRef].current);
     fref >>junk;
     fref >> BeamRef[nlineRef].Pid;
     fref >> BeamRef[nlineRef].Mom;
     fref >> BeamRef[nlineRef].X;
     fref >> BeamRef[nlineRef].Y;
     fref >> BeamRef[nlineRef].Z;
     fref >> BeamRef[nlineRef].ux;
     fref >> BeamRef[nlineRef].uy;
     fref >> BeamRef[nlineRef].uz;
     fref >>junk;
     sscanf(junk,"%s",&BeamRef[nlineRef].comment);
     do{
     fref>>junk;
     sscanf(junk,"%s",junk1);
     strcat(BeamRef[nlineRef].comment,junk1);
     }while(!strstr(junk,"'"));     
     beam_coo_calc(BeamRef[nlineRef]);
     nlineRef++;
     fref.ignore(INT_MAX,'\n');
     if(fref.eof())break;
    }      
    fref.close();


     cout <<" Phase 1 Terminated "<<nlineRef<<endl;

      // add critin to us
      int i;
      int good=0;
      for(i=0;i<nlineRef;i++){
          int found=0;
          BeamRef[i].Good=1;
          if(strstr(BeamRef[i].comment,"BAD")){
           BeamRef[i].Good=0;
          }            
          else good++;
          if(strstr(BeamRef[i].label,"ECAL1")){
           BeamRef[i].Position+=10000;
          }
          else if (strstr(BeamRef[i].label,"ECAL2")){
           BeamRef[i].Position+=20000;
          }
          else if (strstr(BeamRef[i].label,"ECALTRIG")){
           BeamRef[i].Position+=30000;
          }
          if(BeamRef[i].Mom==400){
           BeamRef[i].Size=0.2;
           BeamRef[i].Intens=1000;
          } 
          else{
           BeamRef[i].Size=5;
           BeamRef[i].Intens=100;
          } 
       }
      for(i=0;i<nlineRef;i++){
          if(strstr(BeamRef[i].comment,"MOVEMENT")){
            BeamRef[i].X=(BeamRef[i-1].X+BeamRef[i+1].X)/2;
            BeamRef[i].Y=(BeamRef[i-1].Y+BeamRef[i+1].Y)/2;
            BeamRef[i].Z=(BeamRef[i-1].Z+BeamRef[i+1].Z)/2;
            BeamRef[i].ux=(BeamRef[i-1].ux+BeamRef[i+1].ux)/2;
            BeamRef[i].uy=(BeamRef[i-1].uy+BeamRef[i+1].uy)/2;
            BeamRef[i].uz=(BeamRef[i-1].uz+BeamRef[i+1].uz)/2;
            BeamRef[i].Phi=atan2(BeamRef[i].uy,BeamRef[i].ux);
            BeamRef[i].Theta=acos(BeamRef[i].uz);
            BeamRef[i].Size+=sqrt((BeamRef[i-1].X-BeamRef[i+1].X)*(BeamRef[i-1].X-BeamRef[i+1].X)+(BeamRef[i-1].Y-BeamRef[i+1].Y)*(BeamRef[i-1].Y-BeamRef[i+1].Y));
          }
          if(BeamRef[i].Run<= 1265364635){
             BeamRef[i].X+=5;
          }
          else if( BeamRef[i].Run<1265373533){
             BeamRef[i].X+=10;
          }
          if(strstr(BeamRef[i].comment,"UNKNOWNENERGY")){
            BeamRef[i].Mom=10000;
          }
      }
       BeamParEntry blockNt;
              HBNT(1,"Beam Data"," ");
              HBNAME(1,"CERNBeam",(int*)(&blockNt),"RUN:I,RunTag:I,Time:I,X,Y,Z,Theta,Phi,ux,uy,uz,Pid:I,mom:R,intens:R,Position:I,size:R,good:I");

       for(i=0;i<nlineRef;i++){
        blockNt=BeamRef[i];
        HFNT(1);
       }
                int ntuple_entries;
                HNOENT(1, ntuple_entries);
                cout << " Closing CAS ntuple  with "<< ntuple_entries
                << " events" << endl;
                char hpawc[256]="//PAWC";
                HCDIR (hpawc, " ");
                char houtput[9]="//output";
                HCDIR (houtput, " ");
                integer ICYCL=0;
                HROUT (0, ICYCL, " ");
                HREND ("output");
//                CLOSEF(1);

//   write BeamPar
         BeamPar block;
        int curr=-1;
        time_t begin,end,insert;
            time(&insert);
        uint crc;
     for(i=0;i<nlineRef;i++){
       if(BeamRef[i].Good || 1){
        block.RunTag=BeamRef[i].RunTag; 
        block.Time=BeamRef[i].Time; 
        block.Theta=BeamRef[i].Theta; 
        block.Phi=BeamRef[i].Phi; 
        block.X=BeamRef[i].X; 
        block.Y=BeamRef[i].Y; 
        block.Z=BeamRef[i].Z; 
        block.Pid=BeamRef[i].Pid;
        block.Mom=BeamRef[i].Mom;
        block.Position=BeamRef[i].Position;
        block.Intens=BeamRef[i].Intens; 
        block.Size=BeamRef[i].Size;
        curr++;
         curr=curr%60;
         int iw=0;
         if(curr==0){
           //start record
           begin=BeamRef[i].Time;
         }
         else if(curr==59 || i==nlineRef-1){
           //end record;
               iw=1;
            if(i<nlineRef-1 && BeamRef[i+1].Good){
              end=BeamRef[i+1].Time;
            }
            else end=BeamRef[i].Time+1200;
            for(int k=curr;k<60;k++)record[k]=block;
            ofstream fbin;
            char fname[256];
            sprintf(fname,"/f2users/choutko/BeamPar/BeamPar.1.%d",begin);
            fbin.open(fname);
            insert++;
            uint crc=_CalcCRC((uint*)record,sizeof(record));
            fbin.write((char*)record,sizeof(record));
            fbin.write((char*)&crc,sizeof(crc));
            fbin.write((char*)&insert,sizeof(insert));
            fbin.write((char*)&begin,sizeof(begin));
            fbin.write((char*)&end,sizeof(end));
            fbin.close();
            continue;  
         }
         record[curr]=block;
     }
   }
}

void beam_coo_calc(BeamParEntry & record){
  float x0,y0,z0,sinp,cosp,R,u,v,w,cb,sb;
  float Pi=3.141592654;
  float z=650;
  record.X=(record.X+record.ux/record.uz*(z-record.Z))/10.; 
  record.Y=(record.Y+record.uy/record.uz*(z-record.Z))/10.; 
  record.Z=z/10.;
  record.Phi=atan2(record.uy,record.ux);
  record.Theta=acos(record.uz);

}



/*  CRC calculation */
uint _Table[256];
const uint CRC32=0x04c11db7;
uint Table_init=0;

void _InitTable(){
  if(!Table_init){
    int i,j;
    uint crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)
        crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
    }
    Table_init=1;
  }
}

uint _CalcCRC(uint *pData, int nsize){
  int i,j;
  uint crc, *pu= (uint *)pData;
 _InitTable();
  if( nsize < 1) return 0;
  crc=~pu[0];
    for(i=1;i<(nsize/4);i++){
      for(j=0;j<3;j++)
        crc=_Table[crc>>24]^(crc<<8);
     crc=crc^pu[i];
    }
  return ~crc;
}
