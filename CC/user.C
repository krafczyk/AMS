#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <cern.h>
#include <stdio.h>
#include <user.h>
#include <trrec.h>
#include <event.h>
void AMSUser::InitJob(){

}

void AMSUser::InitRun(){

}

void AMSUser::InitEvent(){
  // Example of how to reset builder for containter
#ifdef __AMSDEBUG__
  AMSEvent::gethead()->setbuilderC("AMSTrCluster",&AMSTrCluster::build);
#endif
}

void AMSUser::Event(){
}


void AMSUser::EndJob(){
}



