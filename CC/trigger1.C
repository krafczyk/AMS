// Simple version 9.06.1997 by E.Choumilov
//
#include <commons.h> 
#include <trigger1.h>
#include <event.h>
#include <mccluster.h>
#include <tofdbc.h>
#include <antidbc.h>
#include <tofrec.h>
#include <antirec.h>
#include <ntuple.h>
//
void TriggerLVL1::build(){
// Trigger mode 1 : ntof >= LVL1FFKEY.ntof + nanti <= LVL1FFKEY.nanti
// TOF : 
  int i;
  integer ntof=0;
  integer tofpatt[SCLRS];
  AMSTOFRawCluster::getpatt(tofpatt);
  for(i=0;i<SCLRS;i++)if(tofpatt[i]>0)ntof+=1;//counts coinc. planes
// ANTI :
  integer antipatt(0);
  integer cbt,nanti(0),lsbit(1);
  antipatt=AMSAntiRawCluster::getpatt();
  for(i=0;i<MAXANTI;i++){
    cbt=lsbit<<i;
    if((antipatt&cbt)>0)nanti+=1;//counts paddles
  }
//
  if(ntof >=LVL1FFKEY.ntof && nanti <= LVL1FFKEY.nanti)
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
                       new TriggerLVL1(1,tofpatt,antipatt));
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
  "LVL1Event:I, LVL1Mode:I, LVL1TOFPatt(4):I,LVL1AntiPatt:I");
}

lvl1N.Event()=AMSEvent::gethead()->getid();
lvl1N.Mode=_TriggerMode;
for(i=0;i<4;i++)lvl1N.TOFPatt[i]=_tofpatt[i];
lvl1N.AntiPatt=_antipatt;
HFNTB(IOPA.ntuple,"LVL1");


}


integer TriggerLVL1::calcdaqlength(){
 return 0;
}

void TriggerLVL1::builddaq(integer n, uinteger *p){
}

void TriggerLVL1::buildraw(integer n, uinteger *p){
}

 
