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
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02"))TOF2User::Event();
    else TOFUser::Event();
  }
}


void AMSUser::EndJob(){
  if(!AMSJob::gethead()->isCalibration()){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02"))TOF2User::EndJob();
    else TOFUser::EndJob();
  }
}



