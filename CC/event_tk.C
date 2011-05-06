//  $Id: event_tk.C,v 1.48 2011/05/06 21:53:28 pzuccon Exp $
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
  TrRecon rec;
  try{
    
    trstat = rec.Build(TRCLFFKEY.recflag, 0, TRCLFFKEY.statflag);
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
