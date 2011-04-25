//  $Id: event_tk.C,v 1.45 2011/04/25 20:19:15 choutko Exp $
#include "TrRecon.h"
#include "TrSim.h"
#include "TkSens.h"
#include "TrTrackSelection.h"

void AMSEvent::_sitkinitevent(){
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrMCCluster",0),0));
}
 
void AMSEvent::_retkinitevent(){
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRawCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrCluster",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrRecHit",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSTrTrack",0),0));
  AMSEvent::gethead()->add(new AMSContainer(AMSID("AMSContainer:AMSVtx",0),0));
}
 

void AMSEvent::_catkinitevent(){
}

void AMSEvent::_catkevent(){
}

void AMSEvent::_sitkinitrun(){
}

void AMSEvent::_retkinitrun(){
}

void AMSEvent::_retkevent(integer refit){
    
  AMSgObj::BookTimer.start("RETKEVENT");
try{
  TrRecon rec;
  trstat = rec.Build(TRCLFFKEY.recflag, 0, 1);
}
catch (AMSTrTrackError e){
cerr<<"AMSEvent::_retkevent-E-"<<e.getmessage()<<endl;
seterror();
}
  AMSgObj::BookTimer.stop("RETKEVENT");
}

void AMSEvent:: _sitkevent(){
  TrSim::sitkdigi();
}
