//  $Id: user.C,v 1.10 2001/09/11 12:57:04 choumilo Exp $
#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <cern.h>
#include <stdio.h>
#include <user.h>
#include <trrec.h>
#include <tofrec02.h>
#include <tofrec.h>
#include <ecalrec.h>
#include <event.h>
#include <trigger102.h>
#include <daqevt.h>
void AMSUser::InitJob(){
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02"))TOF2User::InitJob();
    else TOFUser::InitJob();
  }
}

void AMSUser::InitRun(){
}

void AMSUser::InitEvent(){
}

void AMSUser::Event(){
  integer toftrigfl(0);
  uinteger ectrigfl(0);
//
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      if(ptr){
        toftrigfl=ptr->gettoflg();
        ectrigfl=ptr->getecflg();
      }
      if(toftrigfl<=0 && ectrigfl<=0){
        return;// "no TOF/EC in LVL1-trigger"
      }
      TOF2User::Event();
    }
    else TOFUser::Event();
  }
}


void AMSUser::EndJob(){
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02"))TOF2User::EndJob();
    else TOFUser::EndJob();
  }
}



