// Simple version 9.06.1997 by E.Choumilov
// D. Casadei added trigger hbook histograms, Feb 19, 1998
//
#include <commons.h> 
#include <trigger1.h>
#include <event.h>
#include <mccluster.h>
#include <tofdbc.h>
#include <antidbc.h>
#include <tofrec.h>
#include <tofsim.h>
#include <antirec.h>
#include <ntuple.h>
//
void TriggerLVL1::build(){
// Trigger mode 1 : ntof >= LVL1FFKEY.ntof + nanti <= LVL1FFKEY.nanti
// TOF : 
  int i;
  integer ntof=0;
  uinteger tofpatt[SCLRS]={0,0,0,0};
  integer tofflag(0);
  integer nanti=0;
  integer antipatt=0;
  if(!AMSJob::gethead()->isReconstruction()){
  if(TOFMCFFKEY.fast==0){ // for slow algorithm
    tofflag=AMSTOFRawEvent::gettrfl();
    AMSTOFRawEvent::getpatt(tofpatt);
  }
  else{                   // for fast algorithm
    tofflag=AMSTOFRawCluster::gettrfl();
    AMSTOFRawCluster::getpatt(tofpatt);
  }
  for(i=0;i<SCLRS;i++)if(tofpatt[i]>0)ntof+=1;//counts coinc. planes
// ANTI :
  integer cbt,lsbit(1);
  antipatt=AMSAntiRawEvent::getpatt();
  for(i=0;i<MAXANTI;i++){
    cbt=lsbit<<i;
    if((antipatt&cbt)>0)nanti+=1;//counts paddles
  }
  }
  else if(LVL1FFKEY.RebuildLVL1){
    (AMSEvent::gethead()->getC("TriggerLVL1",0))->eraseC();
    tofflag=1;
    AMSTOFRawCluster *pcl=(AMSTOFRawCluster*)AMSEvent::gethead()->getheadC("AMSTOFRawCluster",0);
    while(pcl){
     int plane=pcl->getntof()-1;
     int bar=pcl->getplane()-1;
     tofpatt[plane]=tofpatt[plane] | ( 1 << bar);  
     pcl=pcl->next();
    }
  for(i=0;i<SCLRS;i++)if(tofpatt[i]>0)ntof+=1;//counts coinc. planes
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
    
  }
  for(i=0;i<4;i++){
    tofpatt[i]=tofpatt[i] | (tofpatt[i]<<16);
  }
  if(tofflag && ntof >=LVL1FFKEY.ntof && nanti <= LVL1FFKEY.nanti)
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
                       new TriggerLVL1(1,tofflag,tofpatt,antipatt));


}



integer TriggerLVL1::checktofpattor(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <SCLRS);
#endif
 return (_tofpatt[tof]) & (1 << paddle);
}


integer TriggerLVL1::checktofpattand(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <SCLRS);
#endif
 return (_tofpatt[tof]) & (1 << (paddle+16));
}


void TriggerLVL1::_writeEl(){

static integer init=0;
static LVL1Ntuple lvl1N;
int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"LVL1",lvl1N.getaddress(),
  "LVL1Event:I, LVL1Mode:I, LVL1TOFlag:I, LVL1TOFPatt(4):I,LVL1TOFPatt1(4):I,LVL1AntiPatt:I");
}

lvl1N.Event()=AMSEvent::gethead()->getid();
lvl1N.Mode=_TriggerMode;
lvl1N.TOFlag=_tofflag;
for(i=0;i<4;i++)lvl1N.TOFPatt[i]=_tofpatt[i];
for(i=0;i<4;i++)lvl1N.TOFPatt1[i]=_tofpatt1[i];
lvl1N.AntiPatt=_antipatt;
HFNTB(IOPA.ntuple,"LVL1");


}



void TriggerLVL1::builddaq(integer i, integer n, int16u *p){
  TriggerLVL1 *ptr=(TriggerLVL1*)AMSEvent::gethead()->
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

  // Word 2 Z > 1
  if(ptr->_tofflag>1)*(p+2)=*(p+2) | (1<<4);

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

void TriggerLVL1::buildraw(integer n, int16u *p){
  {
   AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL1",0);
   if(ptr)ptr->eraseC();
   else cerr <<"TriggerLVL1::buildraw-S-NoContainer"<<endl;

  }
  uinteger z,mode,antip,tofp[4]={0,0,0,0},tofp1[4]={0,0,0,0};  
  uinteger tempor[4]={0,0,0,0};
  uinteger tempand[4]={0,0,0,0};
  //  tofp[0]=*(p+1);
  //  tofp[1]=*(p+2);
  //  tofp[2]=*(p+3);
  //  tofp[3]=*(p+4);
  //  antip=*(p+5);
  //  z=(*(p+6))&255;
  //  mode=((*(p+6))>>8)&255;
  mode=*(p+2);
  //bit 2 & 11 swapped
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

  // ---> TOF Online histograms
  if(AMSJob::gethead()->isMonitoring() & 
     (AMSJob::MTOF | AMSJob::MAll))
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
    };
  // <--- 
  for(int k=0;k<4;k++){
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


  z=1;
  if(*(p+2) & (1<<4))z=3;
  //anti
  integer xneg= (((*(p+1) >> 8) & 3) & ((*(p+1) >> 10) & 3)); 
  integer xpos= (((*(p+1) >> 12) & 3) & ((*(p+1) >> 14) & 3)); 
  antip = xpos | (xneg <<4);
  if(z>0)AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0), new
  TriggerLVL1(mode,z,tofp,tofp1,antip));

}

 
