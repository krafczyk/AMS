#include <ctcsim.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <ctcdbc.h>
#include <ntuple.h>
AMSID AMSCTCRawHit::crgid(integer imat){
      //      imat == 0   // agl
      //      imat == 1   // pmt
      //      imat == 2   // ptf
      integer y=(_row+1)/2;
      integer x=(_column+1)/2;
      integer iy=(_row+1)%2;
      integer ix=(_column+1)%2;      
      
     integer id=1000000*_layer+1000*y+100*x+2*iy+ix+1;
      if(imat==2){
       if(_layer==1)return AMSID("PTFU",id+10);     
       else if(x<6) return AMSID("PTFL",id+10);     
       else return AMSID("PTFE",id+10);     
      }
      else if(imat==0){
       if(_layer==1)return AMSID("AGLU",id+20);     
       else if(x<6) return AMSID("AGLL",id+20);     
       else return AMSID("AGLE",id+20);     
      }
      else if(imat==1){
       if(_layer==1)return AMSID("PMTU",id+30);     
       else if(x<6) return AMSID("PMTL",id+30);     
       else return AMSID("PMTE",id+30);     
      }
      else{
        cerr <<"AMSCTCRawHit::crgid-F-Invalid media par "<<imat<<endl;
        exit(1);
      }
}


void AMSCTCRawHit::sictcdigi(){
  AMSgObj::BookTimer.start("SICTCDIGI");
  AMSCTCMCCluster * ptr;
  int icnt;
  for(icnt=0;icnt<CTCDBc::getnlay();icnt++){
  integer nrow=CTCDBc::getny()*2;
  integer ncol=CTCDBc::getnx(icnt+1)*2;
  geant * Tmp= (geant*)UPool.insert(sizeof(geant)*nrow*ncol);
  geant * TmpT= (geant*)UPool.insert(sizeof(geant)*nrow*ncol);
   ptr=(AMSCTCMCCluster*)AMSEvent::gethead()->
   getheadC("AMSCTCMCCluster",icnt,1); // last 1  to test sorted container
   VZERO(Tmp,sizeof(geant)*nrow*ncol/4);
   VZERO(TmpT,sizeof(geant)*nrow*ncol/4);
   geant value=0;     
   geant time=0;
   while(ptr){
    integer det=ptr->getdetno()-1;
#ifdef __AMSDEBUG__
    assert (det >=0 && det<2);
#endif
    number z=1.-1./pow(CTCMCFFKEY.Refraction[det],2.)/pow(ptr->getbeta(),2.);
    number Response;
    if(z>0)Response=z;
    else Response=0;
    value+=CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    time+=ptr->gettime() * CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    if( ptr->testlast()){
      integer ind=ptr->getcolno()-1+(ptr->getrowno()-1)*ncol;
#ifdef __AMSDEBUG__
   assert(ind >=0 && ind<nrow*ncol);
#endif
      Tmp[ind]+=value;
      TmpT[ind]+=value>0?time/value:time;
      value=0;
      time=0;
    }            

   ptr=ptr->next();  
   }
    for(int i=0;i<nrow*ncol;i++){
     integer ierr,ival;
     if(Tmp[i]>0){
      POISSN(Tmp[i],ival,ierr);
      AMSEvent::gethead()->addnext(AMSID("AMSCTCRawHit",icnt),
      new AMSCTCRawHit(0,i/ncol+1,icnt+1,ival,TmpT[i],i%ncol+1));
     }      
    }
   UPool.udelete(Tmp); 
   UPool.udelete(TmpT); 
  }

  

  AMSgObj::BookTimer.stop("SICTCDIGI");
}


void AMSCTCRawHit::_writeEl(){

static integer init=0;
static CTCHitNtuple CTCCLN;
int i;

  if(init++==0){
    //book the ntuple block
  HBNAME(IOPA.ntuple,"CTCHit",CTCCLN.getaddress(),
  "CTCHit:I*4,CTCHitStatus:I*4, CTCHitLayer:I*4, CTCHitColumn:I*4,CTCHitRow:I*4,  CTCHitSignal:R*4");

  }
  CTCCLN.Event()=AMSEvent::gethead()->getid();
  CTCCLN.Status=_status;
  CTCCLN.Layer=_layer;
  CTCCLN.Column=_column;
  CTCCLN.Row=_row;
  CTCCLN.Signal=_signal;
  HFNTB(IOPA.ntuple,"CTCHit");





}


void AMSCTCRawHit::_copyEl(){
}


void AMSCTCRawHit::_printEl(ostream & stream){
  stream <<"AMSCTCRawHit  "<<_row<<" "<<" "<<_layer<<" "<<_signal<<endl;
}

