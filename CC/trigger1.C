// Simple version 9.06.1997 by E.Choumilov
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
  if(!AMSJob::gethead()->isReconstruction()){
// Trigger mode 1 : ntof >= LVL1FFKEY.ntof + nanti <= LVL1FFKEY.nanti
// TOF : 
  int i;
  integer ntof=0;
  integer tofpatt[SCLRS];
  integer tofflag(0);
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
  integer antipatt(0);
  integer cbt,nanti(0),lsbit(1);
  antipatt=AMSAntiRawEvent::getpatt();
  for(i=0;i<MAXANTI;i++){
    cbt=lsbit<<i;
    if((antipatt&cbt)>0)nanti+=1;//counts paddles
  }
//
//cout<<"LVL1:Tr_flag= "<<tofflag<<" tofpatt: "<<tofpatt[0]<<" "<<tofpatt[1]<<
//                               " "<<tofpatt[2]<<" "<<tofpatt[3]<<endl;
  if(tofflag && ntof >=LVL1FFKEY.ntof && nanti <= LVL1FFKEY.nanti)
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
                       new TriggerLVL1(1,tofflag,tofpatt,antipatt));

  }

}



integer TriggerLVL1::checktofpatt(integer tof, integer paddle){
#ifdef __AMSDEBUG__
 assert(tof >=0 && tof <SCLRS);
#endif
 return (_tofpatt[tof]) & (1 << paddle);
}


void TriggerLVL1::_writeEl(){

static integer init=0;
static LVL1Ntuple lvl1N;
int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"LVL1",lvl1N.getaddress(),
  "LVL1Event:I, LVL1Mode:I, LVL1TOFlag:I, LVL1TOFPatt(4):I,LVL1AntiPatt:I");
}

lvl1N.Event()=AMSEvent::gethead()->getid();
lvl1N.Mode=_TriggerMode;
lvl1N.TOFlag=_tofflag;
for(i=0;i<4;i++)lvl1N.TOFPatt[i]=_tofpatt[i];
lvl1N.AntiPatt=_antipatt;
HFNTB(IOPA.ntuple,"LVL1");


}



void TriggerLVL1::builddaq(integer i, integer n, int16u *p){
  TriggerLVL1 *ptr=(TriggerLVL1*)AMSEvent::gethead()->
  getheadC("TriggerLVL1",i);
  *p=getdaqid();
  if(ptr){
   *(p+1)=int16u(ptr->_tofpatt[0]);
   *(p+2)=int16u(ptr->_tofpatt[1]);
   *(p+3)=int16u(ptr->_tofpatt[2]);
   *(p+4)=int16u(ptr->_tofpatt[3]);
   *(p+5)=int16u(ptr->_antipatt);
   *(p+6)= ptr->_tofflag | ptr->_TriggerMode <<8;
  }
  else for(int i=1;i<n;i++)*(p+i)=0;
}

void TriggerLVL1::buildraw(integer n, int16u *p){
  {
   AMSContainer *ptr=AMSEvent::gethead()->getC("TriggerLVL1",0);
   if(ptr)ptr->eraseC();
   else cerr <<"TriggerLVL1::buildraw-S-NoContainer"<<endl;

  }
  integer z,mode,antip,tofp[4];  
  tofp[0]=*(p+1);
  tofp[1]=*(p+2);
  tofp[2]=*(p+3);
  tofp[3]=*(p+4);
  antip=*(p+5);
  z=(*(p+6))&255;
  mode=((*(p+6))>>8)&255;
  if(z>0)AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0), new
  TriggerLVL1(mode,z,tofp,antip));

}

 
