//  $Id: user.C,v 1.9 2001/05/17 12:10:13 choumilo Exp $
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
  integer trflag(0);
//
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
      Trigger2LVL1 *ptr=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      if(ptr)trflag=ptr->gettoflg();
      if(trflag<=0){
        return;// "no TOF in LVL1-trigger"   
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



