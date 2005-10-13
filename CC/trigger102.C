//  $Id: trigger102.C,v 1.28 2005/10/13 09:01:33 choumilo Exp $
// Simple version 9.06.1997 by E.Choumilov
// D. Casadei added trigger hbook histograms, Feb 19, 1998
//
#include "commons.h" 
#include "trigger102.h"
#include "event.h"
#include "mccluster.h"
#include "tofdbc02.h"
#include "antidbc02.h"
#include "ecaldbc.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antirec02.h"
#include "ecalrec.h"
#include "ntuple.h"
using namespace std;
using namespace ecalconst;
//
 Trigger2LVL1::Scalers Trigger2LVL1::_scaler;
 Trigger2LVL1::Lvl1TrigConfig Trigger2LVL1::l1trigconf;
//
void Trigger2LVL1::build(){
//
  uinteger trigflag(0);//glob trig.patt
// TOF : 
  int i,il,ib,ibmx,ns1,ns2;
  integer ntof=0;
  uinteger tofpatt[TOF2GC::SCLRS]={0,0,0,0};
  uinteger tofpatt1[TOF2GC::SCLRS]={0,0,0,0};//all sides,z>=1
  uinteger tofpatt2[TOF2GC::SCLRS]={0,0,0,0};//all sides,z>=2
  uinteger LVL1Mode(0);//1st 9bits -> (fired & requested)-branche pattern
  integer tflg(-1);
  integer tofflag(-1);//0->4planes,(1-4)->3plns,(5-8)->2plns, <0->noAnyFT, +10->alsoFTZ>=0,+20->onlyFTZ>=0
  integer tofflag1(0),tofflag2(0);
  integer nanti=0;
  uinteger antipatt=0;
  uinteger ectrigfl=0;
  geant ectrsum=0;
  geant gmaglat,gmagphi;
  int tofmatr(1);//tof.matr(centr.area).trigger(if=1)
  uinteger cbt,lsbit(1);
  
//
  if(!AMSJob::gethead()->isReconstruction()){// <---- MC
//
//-->TOF:
    tofflag1=TOF2RawEvent::gettrfl();//<0 ->noFT(z>=1), >=0 ->OK
    tofflag2=TOF2RawEvent::gettrfl1();//<0 ->noFTZ(z>=2), >=0 ->OK
    
    if(tofflag1>=0){//tempor.solution(with only 1 tofflag-variable in lvl1-class)
      tofflag=tofflag1;
      if(tofflag2>=0)tofflag=10+tofflag1;
    }
    else{
      if(tofflag2>=0)tofflag=20+tofflag2;//abnormal(slow) Z>=2
    }
    
    TOF2RawEvent::getpatt(tofpatt1);//z>=1
    TOF2RawEvent::getpatt1(tofpatt2);//z>=2
    if(tofflag>=0){
      if(tofflag/10>1)TOF2RawEvent::getpatt1(tofpatt);//z>=2slow(z>=1 = 0 !)
      else TOF2RawEvent::getpatt(tofpatt);//z>=1
      tflg=tofflag%10;//0->4planes, (1-4)->3plns, (5-8)->2plns, <0->noFT
      if(tflg==0)ntof=4;
      if(tflg>0 && tflg<=4)ntof=3;
      if(tflg>4 && tflg<=9)ntof=2;
//set/reset TOF-matr(center) trig-flag(using actual Z pattern):
      ns1=0;
      ns2=0;
      for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
        ibmx=TOF2DBc::getbppl(il);
        cbt=lsbit<<(ibmx-1);
        if((tofpatt[il] & 1)>0 || (tofpatt[il] & cbt)>0)ns1+=1;//s1 fired   
        if((tofpatt[il] & 1<<16)>0 || (tofpatt[il] & cbt<<16)>0)ns2+=1;//s2 fired
      }
      if(ns1>0 || ns2>0)tofmatr=0;//reset(i.e. trig is not center one)
    }
//   
//-->ANTI:
    
    antipatt=Anti2RawEvent::getpatt();//bits (1-8)->sectors in coinc.with FT
    nanti=Anti2RawEvent::getncoinc();
//
//-->ECAL:
    ectrigfl=AMSEcalRawEvent::gettrfl();
    ectrsum=AMSEcalRawEvent::gettrsum();
// Event: latitude
    AMSEvent::gethead()->getmag(gmaglat,gmagphi);
  }
/*
//----
  else if(TGL1FFKEY.RebuildLVL1){ // <---------- Real
    (AMSEvent::gethead()->getC("TriggerLVL1",0))->eraseC();
    tofflag=1;
    TOF2RawCluster *pcl=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
    while(pcl){
     int plane=pcl->getntof()-1;
     int bar=pcl->getplane()-1;
     ttrpatt[plane]=ttrpatt[plane] | ( 1 << bar);  
     pcl=pcl->next();
    }
    for(i=0;i<TOF2GC::SCLRS;i++)if(ttrpatt[i]>0)ntof+=1;//counts coinc. planes
// ANTI :
    integer antip[2]={0,0};
    for(int k=0;k<2;k++){
      AMSAntiRawCluster *pcl=(AMSAntiRawCluster *)AMSEvent::gethead()->getheadC("AMSAntiRawCluster",k);
      while(pcl){
      int sector=pcl->getsector()-1;
       int updown=pcl->getupdown();
       antip[updown]=antip[updown] | ( 1 << sector);  
       pcl=pcl->next();
      }
    }
    antipatt=antip[0] & antip[1];
 
    integer cbt;
    for(i=0;i<MAXANTI;i++){
      cbt=1<<i;
      if((antipatt&cbt)>0)nanti+=1;//counts paddles
    }
// ECAL :
    ectrigfl=AMSEcalRawEvent::gettrfl();
  }
*/
//------
//
  geant sumsc=_scaler.getsum(AMSEvent::gethead()->gettime());
  geant lifetime=_scaler.getlifetime(AMSEvent::gethead()->gettime());
  // mark default as error here
  integer tm=int(floor(TOF2Varp::tofvpar.getmeantoftemp(0)));   
     if(lifetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData() )
                                                     AMSEvent::gethead()->seterror();
//
// check combined (tof+anti+ecal)trigger flag:
//
  bool BranchOK[8]={0,0,0,0,0,0,0,0};
  bool tofok(0),tofbzok(0),antiok(0),ecok(0),ec1ok(0),ec2ok(0),comtrok(0);
//
  if(tofflag>=0)tofok=1;
  if(tofflag/10>0)tofbzok=1;
//  if(fabs(gmaglat)>TGL1FFKEY.TheMagCut && nanti==0)antiok=1;
//  if(fabs(gmaglat)<TGL1FFKEY.TheMagCut && nanti <= TGL1FFKEY.nanti)antiok=1;
  if(nanti <= TGL1FFKEY.nanti)antiok=1;
  if(ectrigfl>0)ecok=1;//"at least MIP" activity in ECAL
//
  BranchOK[0]=tofok;                        //(1) unbiased#1 (TOF(anyZ) only (equiv TOFFT))
  BranchOK[1]=ecok;                         //(2) unbiased#2 (EC(ectrigfl>0) only)
  BranchOK[2]=tofok && ecok;                //(3) unbiased#3 trigger (TOF && EC(ectrigfl>0))
  BranchOK[3]=tofok || ecok;                //(4) unbiased#4 trigger (TOF || EC(ectrigfl>0))
  BranchOK[4]=tofok && !tofbzok && antiok;  //(5) Z=1 particle trigger(no Z>=2 trig !!!)
  BranchOK[5]=tofbzok;                      //(6) Z>=2 particle trigger(BZ) 
  BranchOK[6]=(tofok && (ectrigfl/10>=2)
                     && (ectrigfl%10==2)
		                       );   //(7) e+- trigger(TOF & ECEt>LowThr & ECWd=em)
  BranchOK[7]=(ectrigfl/10>=3 
               && ectrigfl%10==2);          //(8) photon trigger (ECEt>HiThr & ECWd=em)
//
  int nbreq(0);
  integer trtype(0);
  if(TGL1FFKEY.trtype>0)trtype=TGL1FFKEY.trtype;
  else trtype=Trigger2LVL1::l1trigconf.subtrigmask();
  HF1(ECHIST+25,0.,1.);
  for(i=0;i<8;i++){
    if(BranchOK[i]){
      HF1(ECHIST+25,geant(i+1),1.);
    }
    if(trtype & 1<<i)nbreq+=1;//count requested branches
  }
//
  if((nbreq==0 && trtype !=256) || trtype>256){
    cout<<"Trigger2LVL1::build Error: unknown trigger type requested ! "<<trtype<<endl;
    exit(10);
  }
//
//                         <---- check OR of requested branches(trigger type):
  int nbchk(0);
  if(trtype ==256){
    comtrok=1;
    LVL1Mode=256;
  }
  else{
    for(i=0;i<8;i++){
      if((trtype & 1<<i)>0 && BranchOK[i]){
        nbchk+=1;
	LVL1Mode|=(1<<i);//create (fired&requested)-branch pattern
      }
    }
    if(nbchk>0)comtrok=1;
  }
// 
//cout<<"comtr="<<comtrok<<"tof/anti/ec="<<tofok<<" "<<antiok<<" "<<ectrigfl<<endl;
//
  if(comtrok && (sumsc<TGL1FFKEY.MaxScalersRate || lifetime>TGL1FFKEY.MinLifeTime)){
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(LVL1Mode,tofflag,tofpatt1,tofpatt2,
	                                                antipatt,ectrigfl,ectrsum));
  }
  else if(AMSJob::gethead()->isRealData() && sumsc>=TGL1FFKEY.MaxScalersRate && lifetime<=TGL1FFKEY.MinLifeTime)AMSEvent::gethead()->seterror();
}
//--------------------
void Trigger2LVL1::init(){
}
//--------------------
void Trigger2LVL1::Lvl1TrigConfig::init(){//read needed lvl1trig-masks
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  int mrfp;
  char in[2]="0";
  char inum[11];
  int mcvn,rlvn,dig,endflab(0);
//
  strcpy(inum,"0123456789");
//reset:
 strigmask=1;
 lconf=0;
 zlconf=1;
 for(int il=0;il<TOF2GC::SCLRS;il++){
   for(int ib=0;ib<TOF2GC::SCMXBR;ib++){
     intrigmask[il][ib]=0;//reset mask
     orandmask[il][ib]=0;//reset mask
   }
 }
//
 strcpy(name,"lvl1conf");//generic Lvl1ConfData file name
 if(AMSJob::gethead()->isMCData())           // for MC-data
 {
   cout <<" Trigger2LVL1::Init: LVL1TrigConf-data for MC are requested"<<endl;
   mcvn=TGL1FFKEY.Lvl1ConfMCVers%1000;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real-data
 {
   cout <<" Trigger2LVL1::Init: LVL1TrigConf-data for RealData are requested"<<endl;
   rlvn=TGL1FFKEY.Lvl1ConfRDVers%1000;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
   strcat(name,".dat");
   strcpy(fname,AMSDATADIR.amsdatadir);
   if(TGL1FFKEY.Lvl1ConfRead==1)strcpy(fname,"");
   strcat(fname,name);
   cout<<"Open file : "<<fname<<'\n';
   ifstream tgfile(fname,ios::in); // open file for reading
   if(!tgfile){
     cerr <<"Trigger2LVL1::Init: Missing Lvl1ConfData file "<<fname<<endl;
     exit(1);
   }
//
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> intrigmask[il][ib];
      if(tgfile.eof()){
        cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TofInTrigMask !!!"<<endl;
	exit(1);
      }
    }
  }  
  for(int il=0;il<TOF2DBc::getnplns();il++){
    for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tgfile >> orandmask[il][ib];
      if(tgfile.eof()){
        cerr<<"Trigger2LVL1::Init:Unexpected EOF reading TofSidesOrAndMask !!!"<<endl;
	exit(1);
      }
    }
  }
//  
  tgfile >> strigmask;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Lvl1SubTrigMask !!!"<<endl;
    exit(1);
  }
//  
  tgfile >> lconf;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Lvl1LayerConf !!!"<<endl;
    exit(1);
  }
//
  tgfile >> zlconf;
  if(tgfile.eof()){
    cerr<<"Trigger2LVL1::Init:Unexpected EOF reading Lvl1Z>1LayerConf !!!"<<endl;
    exit(1);
  }
//
  tgfile>>endflab;
//
  tgfile.close();
//
  if(endflab==12345){
    cout<<"Trigger2LVL1::Init: Lvl1TrigConf-data are successfully read !"<<endl;
  }
  else{cout<<"Trigger2LVL1::Init: Error reading Lvl1TrigConf-data !"<<endl;
    exit(1);
  }
//
}
//------
integer Trigger2LVL1::checktofpattor(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt[tof])&(1 << paddle))||((_tofpatt[tof])&(1 << 16+paddle));
}


integer Trigger2LVL1::checktofpattand(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return ((_tofpatt[tof])&(1 << paddle))&&((_tofpatt[tof])&(1 << 16+paddle));
}


void Trigger2LVL1::_writeEl(){

  LVL1Ntuple02* lvl1N = AMSJob::gethead()->getntuple()->Get_lvl102();

  if (lvl1N->Nlvl1>=MAXLVL1) return;

// Fill the ntuple
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  lvl1N->Mode[lvl1N->Nlvl1]=_LifeTime;
  lvl1N->TOFlag[lvl1N->Nlvl1]=_tofflag;
  int i;
  for(i=0;i<4;i++)lvl1N->TOFPatt[lvl1N->Nlvl1][i]=_tofpatt[i];
  for(i=0;i<4;i++)lvl1N->TOFPatt1[lvl1N->Nlvl1][i]=_tofpatt1[i];
  lvl1N->AntiPatt[lvl1N->Nlvl1]=_antipatt;
  lvl1N->ECALflag[lvl1N->Nlvl1]=_ecalflag;
  lvl1N->ECALtrsum[lvl1N->Nlvl1]=_ectrsum;
  lvl1N->Nlvl1++;
}



void Trigger2LVL1::builddaq(integer i, integer n, int16u *p){
  Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->
  getheadC("TriggerLVL1",i);
  *p=getdaqid();
  //  if(ptr){
    //   *(p+1)=int16u(ptr->_tofpatt[0]);
    //   *(p+2)=int16u(ptr->_tofpatt[1]);
    //   *(p+3)=int16u(ptr->_tofpatt[2]);
    //   *(p+4)=int16u(ptr->_tofpatt[3]);
    //   *(p+5)=int16u(ptr->_antipatt);
    //   *(p+6)= ptr->_tofflag | ptr->_TriggerMode <<8;
  //  }
  for( i=1;i<n;i++)*(p+i)=0;
  if(ptr){
  // first anti
  // -x  -z ; -x +z ; +x -z ; +x +z
  // 5-12             1-4   13-16

   int16u anti8=  (ptr->_antipatt >>16) & 255;
   *(p+1)=*(p+1) | (anti8 <<8); 
/*
  {  
    integer cbt,lsbit(1);
    integer antipatt=ptr->_antipatt;
    integer nanti=0;
    for(i=4;i<12;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    if(nanti>3)nanti=3;
    *(p+1)=*(p+1) | (nanti<<8) | (nanti<<10);
  }
  {  
    integer cbt,lsbit(1);
    integer antipatt=ptr->_antipatt;
    integer nanti=0;
    for(i=0;i<4;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    for(i=12;i<16;i++){
     cbt=lsbit<<i;
     if((antipatt&cbt)>0)nanti++;
    }
    if(nanti>3)nanti=3;
    *(p+1)=*(p+1) | (nanti<<12) | (nanti<<14);
   }
*/

  // Word 2 Z > 1
  if((ptr->_tofflag)%10==3)*(p+2)=*(p+2) | (1<<4);
  if(ptr->_tofflag>10)*(p+2)=*(p+2) | (1<<5);

  // Words 3-10  
       *(p+3)=int16u(ptr->_tofpatt[3]);
       *(p+4)=int16u(ptr->_tofpatt[2]);
       *(p+5)=int16u(ptr->_tofpatt[1]);
       *(p+6)=int16u(ptr->_tofpatt[0]);

       *(p+7)=int16u((ptr->_tofpatt[3])>>16);
       *(p+8)=int16u((ptr->_tofpatt[2])>>16);
       *(p+9)=int16u((ptr->_tofpatt[1])>>16);
       *(p+10)=int16u((ptr->_tofpatt[0])>>16);
       for(i=3;i<11;i++){
         //swap bits
         int16u tmp=0;
         int16u tag=*(p+i);
         for(int k=0;k<16;k++)tmp=tmp | (( (tag >> k) & 1) << (13-k));
         *(p+i)=tmp;
       }
       // Words 11-18

       *(p+11)=int16u(ptr->_tofpatt1[3]);
       *(p+12)=int16u(ptr->_tofpatt1[2]);
       *(p+13)=int16u(ptr->_tofpatt1[1]);
       *(p+14)=int16u(ptr->_tofpatt1[0]);

       *(p+15)=int16u((ptr->_tofpatt1[3])>>16);
       *(p+16)=int16u((ptr->_tofpatt1[2])>>16);
       *(p+17)=int16u((ptr->_tofpatt1[1])>>16);
       *(p+18)=int16u((ptr->_tofpatt1[0])>>16);
       for(i=11;i<18;i++){
         //swap bits
         int16u tmp=0;
         int16u tag=*(p+i);
         for(int k=0;k<16;k++)tmp=tmp | (( (tag >> k) & 1) << (13-k));
         *(p+i)=tmp;
       }

  }   
}

void Trigger2LVL1::buildraw(integer n, int16u *p){
  {
   AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL1",0);
   if(ptr)ptr->eraseC();
   else cerr <<"Trigger2LVL1::buildraw-S-NoContainer"<<endl;

  }
  uinteger z,mode,antip,tofp[4]={0,0,0,0},tofp1[4]={0,0,0,0};  
  uinteger tempor[4]={0,0,0,0};
  uinteger tempand[4]={0,0,0,0};
  number ectrsum(0);
  //  tofp[0]=*(p+1);
  //  tofp[1]=*(p+2);
  //  tofp[2]=*(p+3);
  //  tofp[3]=*(p+4);
  //  antip=*(p+5);
  //  z=(*(p+6))&255;
  //  mode=((*(p+6))>>8)&255;
  mode=*(p+2);
  tofp[0]=*(p+6) | *(p+10);
  //  int16u bit2_67= (*(p+6) | *(p+7) ) & (1<<2);
  //  int16u bit11_67= (*(p+6) | *(p+7) ) & (1<<11);
  //  tofp[0]= tofp[0] & (~(1<<2)) & (~(1<<11));
  //  tofp[0]=tofp[0] |   bit2_67 | bit11_67;

  tofp[1]=*(p+5) | *(p+9);
  tofp[2]=*(p+4) | *(p+8);
  tofp[3]=*(p+3) | *(p+7);
  //  int16u bit2_310= (*(p+3) | *(p+10) ) & (1<<2);
  //  int16u bit11_310= (*(p+3) | *(p+10) ) & (1<<11);
  //  tofp[3]= tofp[3] & (~(1<<2)) & (~(1<<11));
  //  tofp[3]=tofp[3] |   bit2_310 | bit11_310;
 int k;
 for(k=0;k<4;k++){
    for(int i=0;i<16;i++){
      tofp[k]=tofp[k] | (((tofp[k]>>i) & 1) <<(29-i));
    }  
    tempor[k]=tofp[k]>>16;     
    
  }
  tofp[0]=*(p+6) & *(p+10);
  //  bit2_67= (*(p+6) & *(p+7) ) & (1<<2);
  //  bit11_67= (*(p+6) & *(p+7) ) & (1<<11);
  //  tofp[0]= tofp[0] & (~(1<<2)) & (~(1<<11));
  //  tofp[0]=tofp[0] |   bit2_67 | bit11_67;
  
  tofp[1]=*(p+5) & *(p+9);
  tofp[2]=*(p+4) & *(p+8);
  tofp[3]=*(p+3) & *(p+7);
  //  bit2_310= (*(p+3) & *(p+10) ) & (1<<2);
  //  bit11_310= (*(p+3) & *(p+10) ) & (1<<11);
  //  tofp[3]= tofp[3] & (~(1<<2)) & (~(1<<11));
  //  tofp[3]=tofp[3] |   bit2_310 | bit11_310;
  for( k=0;k<4;k++){
    for(int i=0;i<16;i++){
      tofp[k]=tofp[k] | (((tofp[k]>>i) & 1) <<(29-i));
    }  
    tempand[k]=tofp[k]>>16;     
  }
  for(k=0;k<4;k++){
    tofp[k]=tempor[k] | (tempand[k] <<16);
  }
  if(n>11){
    tofp1[0]=*(p+14) | *(p+18);
    tofp1[1]=*(p+13) | *(p+17);
    tofp1[2]=*(p+12) | *(p+16);
    tofp1[3]=*(p+11) | *(p+15);
    for( k=0;k<4;k++){
      for(int i=0;i<16;i++){
        tofp1[k]=tofp1[k] | (((tofp1[k]>>i) & 1) <<(29-i));
      }  
      tempor[k]=tofp1[k]>>16;     
    }
    
    tofp1[0]=*(p+14) & *(p+18);
    tofp1[1]=*(p+13) & *(p+17);
    tofp1[2]=*(p+12) & *(p+16);
    tofp1[3]=*(p+11) & *(p+15);
    for( k=0;k<4;k++){
      for(int i=0;i<16;i++){
        tofp1[k]=tofp1[k] | (((tofp1[k]>>i) & 1) <<(29-i));
      }  
      tempand[k]=tofp1[k]>>16;     
    }
    for(k=0;k<4;k++){
      tofp1[k]=tempor[k] | (tempand[k] <<16);
    }
  }
  z= tofp[0] || tofp[1] || tofp[2] || tofp[3]?1:0; 
  //z=1;
  if(*(p+2) & (1<<4))z=3;
  if(*(p+2) & (1<<5))z+=10;
  //anti
  uinteger xneg= min(((*(p+1) >> 8) & 3),  ((*(p+1) >> 10) & 3)); 
  uinteger xpos= min(((*(p+1) >> 12) & 3) , ((*(p+1) >> 14) & 3)); 
  uinteger a2=(*(p+1)>>8) ;
  antip = xpos +xneg ;
  antip = antip | (a2<<16);
  //ecal
  uinteger ecflg=1;// tempor (replace later by real decoding lines)
  //
  geant sumsc=_scaler.getsum(AMSEvent::gethead()->gettime());
  geant lifetime=_scaler.getlifetime(AMSEvent::gethead()->gettime());
  integer tm=int(floor(TOF2Varp::tofvpar.getmeantoftemp(0)));   
  // mark default as error here
     if(lifetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData())AMSEvent::gethead()->seterror();
  if(z>0 && (sumsc<TGL1FFKEY.MaxScalersRate || lifetime>TGL1FFKEY.MinLifeTime))AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0), new
  Trigger2LVL1(integer(lifetime*1000)+tm*10000,z,tofp,tofp1,antip,ecflg,ectrsum));
  else if(AMSJob::gethead()->isRealData() && sumsc>=TGL1FFKEY.MaxScalersRate && lifetime<=TGL1FFKEY.MinLifeTime)AMSEvent::gethead()->seterror();
  // ---> TOF Online histograms
  if(AMSJob::gethead()->isMonitoring() & 
     (AMSJob::MTOF | AMSJob::MAll)){
    HF1(5013,0.5,1.); // for the normalization
    for(int kilo=0;kilo<14;kilo++){
      // Sides
      if(*(p+6) & (1<<kilo) )
	HF1(5000,geant(kilo+1),1.);
      if(*(p+10) & (1<<kilo) )
	HF1(5004,geant(kilo+1),1.);
      if(*(p+5) & (1<<kilo) )
	HF1(5001,geant(kilo+1),1.);
      if(*(p+9) & (1<<kilo) )
	HF1(5005,geant(kilo+1),1.);
      if(*(p+4) & (1<<kilo) )
	HF1(5002,geant(kilo+1),1.);
      if(*(p+8) & (1<<kilo) )
	HF1(5006,geant(kilo+1),1.);
      if(*(p+3) & (1<<kilo) )
	HF1(5003,geant(kilo+1),1.);
      if(*(p+7) & (1<<kilo) )
	HF1(5007,geant(kilo+1),1.);
      // AND of sides
      if((*(p+6) & (1<<kilo)) &&  (*(p+10) & (1<<kilo)))
	HF1(5008,geant(kilo+1),1.);
      if((*(p+5) & (1<<kilo)) &&  (*(p+9) & (1<<kilo)))
	HF1(5009,geant(kilo+1),1.);
      if((*(p+4) & (1<<kilo)) &&  (*(p+8) & (1<<kilo)))
	HF1(5010,geant(kilo+1),1.);
      if((*(p+3) & (1<<kilo)) &&  (*(p+7) & (1<<kilo)))
	HF1(5011,geant(kilo+1),1.);
    }
  }
 
}

/* 
geant TriggerLVL1::Scalers::getsum() const{
int i;
geant sum=0;
for(i=0;i<10;i++)sum+=l1p[i];
for(i=0;i<10;i++)sum+=l1n[i];
for(i=0;i<14;i++)sum+=l2p[i];
for(i=0;i<14;i++)sum+=l2n[i];
for(i=0;i<14;i++)sum+=l3p[i];
for(i=0;i<14;i++)sum+=l3n[i];
for(i=0;i<10;i++)sum+=l4p[i];
for(i=0;i<10;i++)sum+=l4n[i];
return sum;
}

*/

uinteger Trigger2LVL1::Scalers::_GetIndex(time_t time ){
static integer k=0;
// first check k; then either k+1 or run binary search

  if(time>=_Tls[0][k] && time < (k<_Nentries-1?_Tls[0][k+1]:time+1)){
   return k;
  }
  else if(k++<_Nentries-1 ){
   if(time>=_Tls[0][k] && time < (k<_Nentries-1?_Tls[0][k+1]:time+1)){
   return k;
   }
  }
  k=abs(AMSbins(_Tls[0], uinteger(time), _Nentries))-1;   
  if(k<0){
   cerr<<" Trigger2LVL1::Scalers::_GetIndex-S-IndexLessZero "<<k<<" "<<time<<endl;
   k=0;
   AMSEvent::gethead()->seterror();   
  }   
return k;
}




