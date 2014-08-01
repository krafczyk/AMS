#include "trdrec.h"
#include "trdhrec.h"
#include "event.h"
#include "ntuple.h"
#include "extC.h"
#include "root.h"
#include "tofrec02.h"
#include "fenv.h"
#include "TrdHRecon.h"


void AMSTRDHSegment::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  TrdHSegmentR & ptr =AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHSegment(_vpos); 
  
  ptr.fTrdRawHit.clear();
  for (int i=0; i<(int)gbhits.size(); i++) 
    if (gbhits[i])
      ptr.fTrdRawHit.push_back(gbhits[i]->GetClonePointer());

#endif
}

void AMSTRDHSegment::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
    || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHSegment::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHSegment",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

/*integer AMSTRDHSegment::_addnext(integer d, float m, float em, float r, float er, float z, float w, integer nhits, TrdRawHitR* pthit[]){
#ifdef __UPOOL__
  cout<< "AMSTRDHSegment::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else
  AMSTRDHSegment *ptrack= new AMSTRDHSegment(d,m,em,r,er,z,w,nhits,pthit);
#endif
  ptrack->_addnextR(0);

  delete ptrack;  
  return 0;
}

void AMSTRDHSegment::_addnextR(uinteger iseg){
  // permanently add;
  //  if(iseg>TrdHReconR::getInstance()->nhsegvec&&iseg<10){
    //    AMSEvent::gethead()->addnext(AMSID("AMSTRDHSegment",iseg),new AMSTRDHSegment(TrdHReconR::getInstance()->hsegvec[iseg]));
  //  }
  }*/

void AMSTRDHTrack::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  (void)AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHTrack(_vpos);
#endif
}

void AMSTRDHTrack::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHTrack::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

void AMSTRDHSegment::_printEl(ostream &o){
  o << " TRDHSegment printout "<<endl;
}

void AMSTRDHTrack::_printEl(ostream &o){
  o <<"TRDHTrack printout"<<endl;
}

/*integer AMSTRDHTrack::_addnext(float pos[3],float dir[3], AMSTRDHSegment* pthit[]){

#ifdef __UPOOL__
  cout<< "AMSTRDHTrack::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else
  
  //  AMSTRDHTrack *ptrack=new AMSTRDHTrack(new TrdHTrackR(pos,dir));
  //  if(!pthit[0]&&!pthit[1]) ptrack->SetSegments(new AMSTRDHSegment(pthit[0]),new AMSTRDHSegment(pthit[1]));
  
#endif
  
  //  ptrack->_addnextR(0);
  
  //  delete ptrack;  
  return 0;
}

void AMSTRDHTrack::_addnextR(uinteger itr){
  int i;
  // permanently add;
  //  if(itr<4)
    //    AMSEvent::gethead()->addnext(AMSID("AMSTRDHTrack",itr),new AMSTRDHTrack(TrdHReconR::getInstance()->htrvec[itr]));
}
*/



/*AMSTRDHSegment *AMSTRDHSegment::gethead(uinteger i){
  if(i<TRDDBc::nlayS()){
    return (AMSTRDHSegment*)AMSEvent::gethead()->getheadC("AMSTRDHSegment",i,1);
  }
  else {
#ifdef __AMSDEBUG__
    cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
    return 0;
  }
}

AMSTRDHTrack *AMSTRDHTrack::gethead(uinteger i){
  return (AMSTRDHTrack*)AMSEvent::gethead()->getheadC("AMSTRDHTrack",i,1);
#ifdef __AMSDEBUG__
  cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
  return 0;
  }*/


/*
AMSTRDHSegment *AMSTRDHSegment::gethead(integer i)
{
  if (i < 0 ) return 0;

  VCon* cont = GetVCon()->GetCont("AMSTRDHSegment");
  if (!cont||cont->getnelem()<=i) return 0;

  AMSTRDHSegment *phead = (AMSTRDHSegment*)cont->getelem(i);
  for (int j = 0; j < cont->getnelem(); j++) {
    AMSTRDHSegment *seg = (AMSTRDHSegment*)cont->getelem(i);
    if (hit->GetLayer() == i+1) {
      phead = hit;
      break;
    }
    }

  delete cont;
  return phead;
}

AMSTRDHTrack *AMSTRDHTrack::gethead(integer i)
{
  if (i < 0 ) return 0;

  VCon* cont = GetVCon()->GetCont("AMSTRDHTrack");
  if (!cont||cont->getnelem()<=i) return 0;

  AMSTRDHTrack *phead = (AMSTRDHTrack*)cont->getelem(i);
  for (int j = 0; j < cont->getnelem(); j++) {
      AMSTRDHSegment *seg = (AMSTRDHSegment*)cont->getelem(i);
    if (hit->GetLayer() == i+1) {
      phead = hit;
      break;
    }
  }

  delete cont;
  return phead;
}


AMSTRDHSegment *AMSTRDHSegment::next()
{
  VCon* cont = GetVCon()->GetCont("AMSTRDHSegment");
  if (!cont) return 0;

  bool found = false;
  AMSTRDHSegment *pnext = 0;
  for (int j = 0; j < cont->getnelem(); j++) {
    AMSTRDHSegment *seg = (AMSTRDHSegment*)cont->getelem(j);
    if (found) {
      pnext = seg;
      break;
    }
    if (!found && seg == this) found = true;
  }

  delete cont;
  return pnext;
}

AMSTRDHTrack *AMSTRDHTrack::next()
{
  VCon* cont = GetVCon()->GetCont("AMSTRDHTrack");
  if (!cont) return 0;

  bool found = false;
  AMSTRDHTrack *pnext = 0;
  for (int j = 0; j < cont->getnelem(); j++) {
    AMSTRDHTrack *seg = (AMSTRDHTrack*)cont->getelem(j);
    if (found) {
      pnext = seg;
      break;
    }
    if (!found && seg == this) found = true;
  }

  delete cont;
  return pnext;
}

*/
