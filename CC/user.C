//  $Id: user.C,v 1.20 2009/01/27 16:10:33 choumilo Exp $
#include "typedefs.h"
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
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
bool AMSUser::_NewRunStart=true;
//
void AMSUser::InitJob(){//called after all other initjob's
  _JobFirstRunN=0;
  _PreviousRunN=0;
  _NewRunStart=true;
//
  if(!AMSJob::gethead()->isCalibration()){
    TOF2User::InitJob();
  }
}

void AMSUser::InitRun(){//called after all other initrun's
}

void AMSUser::InitEvent(){
}

void AMSUser::Event(){
  bool glft(false),tofft(false);
//------
  if(!AMSJob::gethead()->isCalibration()){
      Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      if(ptr)glft=ptr->GlobFasTrigOK();
      if(ptr)tofft=ptr->TofFasTrigOK();
      if(!glft)return;// "no globFT in LVL1-trigger"
//      if(!tofft)return;// "no tofFT in LVL1-trigger"
      TOF2User::Event();
  }
}


void AMSUser::EndJob(){
  if(!AMSJob::gethead()->isCalibration()){
    TOF2User::EndJob();
  }
}



