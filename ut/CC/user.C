#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <cern.h>
#include <stdio.h>
#include <user.h>
#include <trrec.h>
#include <tofrec.h>
#include <event.h>
#include <daqevt.h>
void AMSUser::InitJob(){
  TOFUser::InitJob();
}

void AMSUser::InitRun(){
}

void AMSUser::InitEvent(){
}

void AMSUser::Event(){
  if(!AMSJob::gethead()->isCalibration())TOFUser::Event();
}


void AMSUser::EndJob(){
  if(!AMSJob::gethead()->isCalibration())TOFUser::EndJob();
}



