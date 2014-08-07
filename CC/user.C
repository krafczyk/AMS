//  $Id$
#include "typedefs.h"
#include <stdlib.h>
#include "cern.h"
#include <stdio.h>
#include "user.h"
#include "trrec.h"
#include "tofrec02.h"
#include "ecalrec.h"
#include "event.h"
#include "trigger102.h"
#include "daqevt.h"
#include "particle.h"
//
uinteger AMSUser::_JobFirstRunN=0;
uinteger AMSUser::_PreviousRunN=0;
time_t AMSUser::_RunFirstEventT=0;
time_t AMSUser::_JobFirstEventT=0;
//
void AMSUser::InitJob(){//called after all other initjob's
  _JobFirstRunN=0;
  _PreviousRunN=0;
  _RunFirstEventT=0;
  _JobFirstEventT=0;
//
  if(TFREFFKEY.relogic[0]==0 && !(AMSJob::gethead()->isCalibration())){//if reco(noncalib) job
    TOF2User::InitJob();
  }
}

void AMSUser::InitRun(){//called after all other initrun's
}

void AMSUser::InitEvent(){
}

void AMSUser::Event(){
  bool glft(false);
//------
  if(TFREFFKEY.relogic[0]==0 && !(AMSJob::gethead()->isCalibration())){
      Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      if(ptr)glft=ptr->GlobFasTrigOK();
      if(!glft)return;// "no globFT in LVL1-trigger"
//      if(!tofft)return;// "no tofFT in LVL1-trigger"
      TOF2User::Event();
  }
}


void AMSUser::EndJob(){
  if(TFREFFKEY.relogic[0]==0 && !(AMSJob::gethead()->isCalibration())){
    TOF2User::EndJob();
  }
}



