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
  uint RunTag;   //runTag
  time_t Time;   //UNIX sec, of the manipulator PC
  float alpha;
  float beta;
  float H;
   float V;
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
   uint Good;
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
             cerr << "Error opening Histo file beam"<<endl;
             exit(1);
           }
           else cout <<"Histo file "<<"beam"<<" opened."<<endl;

    char ref[]="/offline/vdev/slow/gsi/testbeam.log";
    char prod[]="/offline/vdev/slow/gsi/testbeam.info";
    char junk[256];
    char junk1[256];
    char junk2[256];
    ifstream fref;
    fref.open(ref);
    int nlineRef=0;
    int runno,events,eventsp;
    int expected=0;
    int expectedp=0;
    int produced=0;
    while(1){
     fref >> junk;
     fref.ignore(INT_MAX,'\n');
     if(fref.eof())break;
     nlineRef++;
    }     
    fref.close();
    fref.open(ref);
    BeamParEntry * BeamRef = new BeamParEntry[nlineRef];  
    nlineRef=0;
    while(1){
     fref >>junk;
     if(fref.eof())break;
     sscanf(junk,"%x",&BeamRef[nlineRef].RunTag);
     fref >>junk;
     sscanf(junk,"%d",&BeamRef[nlineRef].Time);
     fref >>junk;
     sscanf(junk,"%f",&BeamRef[nlineRef].alpha);
     fref >>junk;
     sscanf(junk,"%f",&BeamRef[nlineRef].beta);
     fref >>junk;
     sscanf(junk,"%f",&BeamRef[nlineRef].H);
     fref >>junk;
     sscanf(junk,"%f",&BeamRef[nlineRef].V);
     fref.ignore(INT_MAX,'\n');
     if(fref.eof())break;
     beam_coo_calc(BeamRef[nlineRef++]);
    }      
    fref.close();

    ifstream fprod;
    fprod.open(prod);
    int nlineProd=0;
    while(1){
     fprod >> junk;
     fprod.ignore(INT_MAX,'\n');
     if(fprod.eof())break;
     nlineProd++;
    }      
    fprod.close();
     nlineProd/=2;
    fprod.open(prod);
    BeamParEntry *BeamInfo = new BeamParEntry[nlineProd];
    nlineProd=0;
    while(1){
     fprod >> junk;
     sscanf(junk,"%x",&BeamInfo[nlineProd].Time);
     if(fprod.eof())break;
     fprod >> junk;
     sscanf(junk,"%x",&BeamInfo[nlineProd].RunTag);
     fprod.ignore(INT_MAX,'\n');
     fprod >> junk;
     if(fprod.eof())break;
     if(strstr(junk,"Helium"))BeamInfo[nlineProd].Pid=47;
     else BeamInfo[nlineProd].Pid=67;
     fprod >> junk;
     int iek;
     sscanf(junk,"%d",&iek); 
     float ek=iek/1000.;
     if(BeamInfo[nlineProd].Pid==47){
       double mass=3.724;
       double etot=ek*4+mass;
       BeamInfo[nlineProd].Mom=sqrt(etot*etot-mass*mass);
     }
     else {
       double mass=11.17793;
       double etot=ek*12+mass;
       BeamInfo[nlineProd].Mom=sqrt(etot*etot-mass*mass);
     }
      fprod >> junk1;
      fprod >> junk;
      fprod >> junk;
      sscanf(junk,"%d",&BeamInfo[nlineProd].Position);
      fprod.ignore(INT_MAX,'\n');
      if(strstr(junk1,"MeV"))nlineProd++;       
   }      
    fprod.close();
     cout <<" Phase 1 Terminated "<<nlineRef<<" "<<nlineProd<<endl;
      int i;
      int good=0;
      for(i=0;i<nlineRef;i++){
          int found=0;
          BeamRef[i].Good=0;
       for(int j=0;j<nlineProd;j++){
          if(BeamRef[i].RunTag!=4096 && 
             BeamRef[i].RunTag==BeamInfo[j].RunTag){
              // check times;
              if(BeamRef[i].Time>BeamInfo[j].Time){
                if(BeamRef[i].Time-BeamInfo[j].Time<1000 && i!=0 &&
                 BeamInfo[j].Time>BeamRef[i-1].Time ){
                //apply correction
                BeamRef[i].Time=BeamInfo[j].Time;
                cout <<"Time Error corrected "<<endl;
               }
               else {
               cerr<<" Time Error "<<hex<<BeamRef[i].RunTag<<" "<<dec<<BeamRef[i].Time-BeamInfo[j].Time<<" "<<BeamRef[i].Time<<hex<<" "<<BeamInfo[j].Time<<endl;
               }
              }
            good++;
            found=1;
            BeamRef[i].Good=1;
            BeamRef[i].Mom=BeamInfo[j].Mom;
            BeamRef[i].Position=BeamInfo[j].Position;
            BeamRef[i].Pid=BeamInfo[j].Pid;
            break;
          }
       }
          if(!found)cerr<<"error "<<BeamRef[i].Time<<" "<<hex<<BeamRef[i].RunTag<<endl;          
      }
       cout <<"good found "<<good<<endl;
       BeamParEntry blockNt;
              HBNT(1,"Beam Data"," ");
              HBNAME(1,"GSIBeam",(int*)(&blockNt),"RunTag:I,Time:I,alpha,beta,H,V,X,Y,Z,Theta,Phi,Pid:I,mom:R,intens:R,Position:I,size:R,good:I");
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
                HROUT (1, ICYCL, " ");
                HREND ("output");
                CLOSEF(1);

//   write BeamPar
         BeamPar block;
        int curr=-1;
        time_t begin,end,insert;
            time(&insert);
        uint crc;
     for(i=0;i<nlineRef;i++){
       if(BeamRef[i].Good){
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
            sprintf(fname,"BeamPar.1.%d",begin);
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

// from given alpha(deg), beta(deg),H(cm),V(cm),z(cm)
//calculates theta(rad),phi(rad),x & y(cm) of the beam crossing
// the plane of z=const
void beam_coo_calc(BeamParEntry & record){
  float x0,y0,z0,sinp,cosp,R,u,v,w,cb,sb;
  float Pi=3.141592654;
  float xyz[3], xyz1[3], rec_m[3][3];
  int i,j;

  record.Z=72.;
  float alpha=record.alpha;
  float beta=record.beta;
  float H=record.H;
  float V=record.V;
  record.Theta=acosf(-cosdf(alpha)*cosdf(beta));

  sinp=sindf(beta)*cosdf(alpha)/sinf(record.Theta);
  cosp=sindf(alpha)/sinf(record.Theta);
  record.Phi=atan2f(sinp,cosp);
  //crossing of the plane, perp. to the beam, through the magnet center

  x0=-H; y0=-V; z0=0;

  xyz[1]=y0*cosdf(beta)+z0*sindf(beta);
  xyz[0]=x0;
  xyz[2]=y0*sindf(beta)-z0*cosdf(beta);

  u=0;v=cosdf(beta);w=sindf(beta);

  cb=cosdf(alpha); sb=sindf(alpha);

  rec_m[0][0]=cb+u*u*(1-cb);
                 rec_m[0][1]=-w*sb+u*v*(1-cb); rec_m[0][2]=v*sb+u*w*(1-cb);
  rec_m[1][0]=w*sb+u*v*(1-cb);
                 rec_m[1][1]=cb+v*v*(1-cb); rec_m[1][2]=-u*sb+v*w*(1-cb);
  rec_m[2][0]=-v*sb+u*w*(1-cb);
                 rec_m[2][1]=u*sb+v*w*(1-cb); rec_m[2][2]=cb+w*w*(1-cb);

  for (i=0; i<3; i++) {
    xyz1[i]=0.;
    for (j=0; j<3; j++) {
      xyz1[i] += xyz[j]*rec_m[j][i];
    }
  }

  R=cosf(record.Theta);
  if (R*R<1e-6)
    record.Z=0.;
  //crossing a plane, perp. to the AMS z-axis, for a given z
  R=(record.Z-xyz1[2]);
  record.X=xyz[0]+R*tanf(record.Theta)*cosf(record.Phi);
  record.Y=xyz[1]+R*tanf(record.Theta)*sinf(record.Phi);

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
