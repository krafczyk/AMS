//  $Id: trigger102.C,v 1.14 2002/04/17 12:42:14 choumilo Exp $
// Simple version 9.06.1997 by E.Choumilov
// D. Casadei added trigger hbook histograms, Feb 19, 1998
//
#include <commons.h> 
#include <trigger102.h>
#include <event.h>
#include <mccluster.h>
#include <tofdbc02.h>
#include <antidbc02.h>
#include <tofrec02.h>
#include <tofsim02.h>
#include <antirec02.h>
#include <ecalrec.h>
#include <ntuple.h>
using namespace std;
//
Trigger2LVL1::Scalers Trigger2LVL1::_scaler;
void Trigger2LVL1::build(){
// Trigger mode 1 : ntof >= TGL1FFKEY.ntof + nanti <= TGL1FFKEY.nanti
// TOF : 
  int i;
  integer ntof=0;
  integer tofpatt[TOF2GC::SCLRS]={0,0,0,0};
  uinteger ttrpatt[TOF2GC::SCLRS]={0,0,0,0};
  integer tofflag(0);
  integer nanti=0;
  integer antipatt=0;
  uinteger ectrigfl=0;
  geant ectrsum=0;
  if(!AMSJob::gethead()->isReconstruction()){// <---- MC
    tofflag=TOF2RawEvent::gettrfl();
    TOF2RawEvent::getpatt(tofpatt);
    for(i=0;i<TOF2GC::SCLRS;i++)if(tofpatt[i]>0)ntof+=1;//counts coinc. planes
    for(i=0;i<TOF2GC::SCLRS;i++){ // create unsigned pattern
      if(tofpatt[i]>0)ttrpatt[i]=tofpatt[i];
      else ttrpatt[i]=0;
    }
// ANTI :
    integer cbt,lsbit(1);
    antipatt=Anti2RawEvent::getpatt();
    nanti=antipatt&255;
// ECAL :
    ectrigfl=AMSEcalRawEvent::gettrfl();
    ectrsum=AMSEcalRawEvent::gettrsum();
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
  for(i=0;i<TOF2GC::SCLRS;i++){
    ttrpatt[i]=ttrpatt[i] | (ttrpatt[i]<<16);// create AND+OR pattern 
  }
  geant sumsc=_scaler.getsum(AMSEvent::gethead()->gettime());
  geant lifetime=_scaler.getlifetime(AMSEvent::gethead()->gettime());
  // mark default as error here
  integer tm=floor(TOF2Varp::tofvpar.getmeantoftemp(0));   
     if(lifetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData() )
                                                     AMSEvent::gethead()->seterror();
//
// check combined (tof+anti+ecal)trigger flag:
//
  bool tofok(0),antiok(0),ecok(0),ec1ok(0),ec2ok(0),comtrok(0);
  bool unbtr1(0),unbtr2(0),unbtr3(0),zge1ptr(0),zge2ptr(0),elpotr(0),photr(0),unitr(0);
//
  if(tofflag>0 && ntof >=TGL1FFKEY.ntof)tofok=1;
  if(nanti <= TGL1FFKEY.nanti)antiok=1;
  if(ectrigfl>0)ecok=1;//"at least MIP" activity in ECAL
//
  unbtr1=tofok;                              //unbiased#1 trigger (TOF only)
  unbtr2=ecok;                               //unbiased#2 trigger (EC(ectrigfl>0) only)
  unbtr3=tofok && ecok;                      //unbiased#3 trigger (TOF+EC(ectrigfl>0))
  zge1ptr=tofok && antiok;                   // Z>=1 particle trigger
  zge2ptr=tofok && (tofflag>2);              // Z>=2 particle trigger
  elpotr=tofok && (ectrigfl%10>=2);          //e+- trigger(softEM_in_EC +TOF)
  photr=(ectrigfl==13);                      //photon trigger (High_(EM+En)_in_EC)
  unitr=zge1ptr || zge2ptr || elpotr || photr;//univers.trigger
//
  if(TGL1FFKEY.trtype==0)comtrok=unitr;
  else if(TGL1FFKEY.trtype==1)comtrok=unbtr1; 
  else if(TGL1FFKEY.trtype==2)comtrok=unbtr2; 
  else if(TGL1FFKEY.trtype==3)comtrok=unbtr3; 
  else if(TGL1FFKEY.trtype==4)comtrok=zge1ptr; 
  else if(TGL1FFKEY.trtype==5)comtrok=zge2ptr; 
  else if(TGL1FFKEY.trtype==6)comtrok=elpotr; 
  else if(TGL1FFKEY.trtype==7)comtrok=photr; 
  else if(TGL1FFKEY.trtype==8)comtrok=(tofok || ecok); 
  else{
    cout<<"Trigger2LVL1::build Error: unknown trigger type "<<TGL1FFKEY.trtype<<endl;
    exit(10);
  }
// 
//cout<<"comtr="<<comtrok<<"tof/anti/ec="<<tofok<<" "<<antiok<<" "<<ectrigfl<<endl;
//
  if(comtrok && (sumsc<TGL1FFKEY.MaxScalersRate || lifetime>TGL1FFKEY.MinLifeTime)){
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(lifetime*1000+tm*10000,tofflag,ttrpatt,antipatt,ectrigfl,ectrsum));
  }
  else if(AMSJob::gethead()->isRealData() && sumsc>=TGL1FFKEY.MaxScalersRate && lifetime<=TGL1FFKEY.MinLifeTime)AMSEvent::gethead()->seterror();
  


}



integer Trigger2LVL1::checktofpattor(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return (_tofpatt[tof]) & (1 << paddle);
}


integer Trigger2LVL1::checktofpattand(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <TOF2GC::SCLRS);
#endif
 return (_tofpatt[tof]) & (1 << (paddle+16));
}


void Trigger2LVL1::_writeEl(){

  LVL1Ntuple02* lvl1N = AMSJob::gethead()->getntuple()->Get_lvl102();

  if (lvl1N->Nlvl1>=MAXLVL1) return;

// Fill the ntuple
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
  integer tm=floor(TOF2Varp::tofvpar.getmeantoftemp(0));   
  // mark default as error here
     if(lifetime>1. && !MISCFFKEY.BeamTest && AMSJob::gethead()->isRealData())AMSEvent::gethead()->seterror();
  if(z>0 && (sumsc<TGL1FFKEY.MaxScalersRate || lifetime>TGL1FFKEY.MinLifeTime))AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0), new
  Trigger2LVL1(lifetime*1000+tm*10000,z,tofp,tofp1,antip,ecflg,ectrsum));
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




