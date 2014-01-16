//  $Id: event_tk.C,v 1.57 2014/01/16 18:01:52 shaino Exp $
#include "TrRecon.h"
#include "TrSim.h"
#include "TkSens.h"
#include "TrExtAlignDB.h"
#include "TrInnerDzDB.h"

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
    if (TrExtAlignDB::GetHead()->GetSize(8) > 0 &&
	TrExtAlignDB::GetHead()->GetSize(9) > 0 && AMSJob::gethead()->isRealData() )
    {
        TrExtAlignDB::GetHead()->UpdateTkDBc(gettime());
        TrExtAlignDB::GetHead()->UpdateTkDBcDyn(getrun(),gettime(),3);
    }
    if(AMSJob::gethead()->isRealData())
      TrInnerDzDB::GetHead()->UpdateTkDBc(gettime());
    // Set TkPlaneExt to PG
    TrExtAlignDB::SetAlKind(0);
    
    trstat = rec.Build(TRCLFFKEY.recflag%10000, 0, TRCLFFKEY.statflag);
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
