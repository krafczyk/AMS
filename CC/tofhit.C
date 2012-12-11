//  $Id: tofhit.C,v 1.3 2012/12/11 17:50:42 qyan Exp $

// ------------------------------------------------------------
//      AMSlink BetaH  Object
// ------------------------------------------------------------
//      History
//        Created:       2012-Jul-19  Q.Yan  qyan@cern.ch
// ----------------------------------------------------------

#include "tofhit.h"
#include "tofsim02.h"
#include "job.h"
#include "event.h"
#include "ntuple.h"
#include "ecalrec.h"

//========================================================
void AMSTOFClusterH::_writeEl(){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

//========================================================
void AMSTOFClusterH::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 TofClusterHR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TofClusterH(_vpos);
   for(int i=0;i<2;i++){
      if(_tfraws[i])ptr.fTofRawSide.push_back(_tfraws[i]->GetClonePointer());
    }
#endif
}
//=======================================================
integer AMSTOFClusterH::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFClusterH",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

//======================================================
void AMSBetaH::_writeEl(){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

//======================================================
void AMSBetaH::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
    BetaHR & ptr=AMSJob::gethead()->getntuple()->Get_evroot02()->BetaH(_vpos);
    if(_ptrack){
         ptr.fTrTrack=_ptrack->GetClonePointer();
     }
    else {ptr.fTrTrack=-1;}
//-----
    if(_ptrdtrack){
         ptr.fTrdTrack=_ptrdtrack->GetClonePointer();
     }
    else {ptr.fTrdTrack=-1;}
//----ecal show
    if(_pecalshower){
        ptr.fEcalShower= _pecalshower->GetClonePointer();
    }
    else {ptr.fEcalShower=-1;}
//-----
    ptr.fTofClusterH.clear();
    for  (int i=0; i<4; i++) {
      if(_phith[i]){
         ptr.fTofClusterH.push_back(_phith[i]->GetClonePointer());
         ptr.fLayer[i]=_phith[i]->GetClonePointer();
        }
      else ptr.fLayer[i]=-1;
    }
    ptr.fTofChargeH=-1;
#endif
}

//======================================================
integer AMSBetaH::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSBetaH",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//======================================================


