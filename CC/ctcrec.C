#include <ctcrec.h>
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

void AMSCTCCluster::_writeEl(){

static integer init=0;
static CTCClusterNtuple CTCCLN;
int i;
if(AMSCTCCluster::Out( IOPA.WriteAll ||  checkstatus(AMSDBc::USED))){

if(init++==0){
//book the ntuple block
  HBNAME(IOPA.ntuple,"CTCClust",CTCCLN.getaddress(),
  "CTCCluster:I*4,CTCStatus:I*4, CTCLayer:I*4, CTCCoo(3):R*4,CTCErCoo(3):R*4,CTCRawSignal:R*4,CTCSignal:R*4,CTCESignal:R*4");

}
  CTCCLN.Event()=AMSEvent::gethead()->getid();
  CTCCLN.Status=_status; 
  CTCCLN.Layer=_Layer;
  for(i=0;i<3;i++)CTCCLN.Coo[i]=_Coo[i];
  for(i=0;i<3;i++)CTCCLN.ErrCoo[i]=_ErrorCoo[i];
  CTCCLN.RawSignal=_Signal;
  CTCCLN.Signal=_CorrectedSignal;
  CTCCLN.ErrorSignal=_ErrorSignal;
  HFNTB(IOPA.ntuple,"CTCClust");

}
}
void AMSCTCCluster::_copyEl(){
}


void AMSCTCCluster::_printEl(ostream & stream){
  stream <<"AMSCTCCluster "<<_status<<" "<<_Layer<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<
 _Signal<<" " <<_ErrorSignal<<endl;
}


 
void AMSCTCCluster::print(){
int i;
for(i=0;;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCTCCluster",i);
 if(p)p->printC(cout);
 else break;
}
}



void AMSCTCCluster::build(){
if(CTCDBc::getgeom()<2){
  cerr << " Non Annecy setups are not supported any more "<<endl;
  exit(1);
}
else {
 for(integer kk=0;kk<CTCDBc::getnlay();kk++){
  AMSCTCRawHit *ptr=(AMSCTCRawHit*)AMSEvent::gethead()->
  getheadC("AMSCTCRawHit",kk,1);
  integer const maxpl=200;
  static number xplane[maxpl];
  static integer xstatus[maxpl];
  VZERO(xplane,maxpl*sizeof(number)/4);
  while (ptr){
    if(ptr->getlayno()==kk+1){
    integer plane=2*CTCDBc::getnx(kk+1)*(ptr->getrowno()-1)+ptr->getcolno()-1;
#ifdef __AMSDEBUG__
    assert(plane>=0 && plane < maxpl);
#endif
    xplane[plane]+=ptr->getsignal();
    xstatus[plane]+=ptr->getstatus();
    }
   ptr=ptr->next();
  }
  addnew(xstatus,xplane,maxpl,kk+1);
 }
}
}

void AMSCTCCluster::addnew(integer xstatus[],number xplane[],integer maxpl,integer layer){
number smax=0;
integer imax=-1;
for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > smax){
  smax=xplane[i];
  imax=i;
 }
}
if(imax >=0 && smax >0 && smax>=CTCRECFFKEY.Thr1){
  number edep=0;
  AMSPoint cofg(0,0,0);
  number xsize=1000;
  number ysize=1000;
  number zsize=1000;
  integer status=xstatus[imax];
  integer row=imax/CTCDBc::getnx(layer)/2+1;
  integer col=imax%(CTCDBc::getnx(layer)*2)+1;
  int i,j;
  for(i=-1;i<2;i++){ 
   for(j=-1;j<2;j++){
    integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
    if(col-1+j >=0 && col-1+j <CTCDBc::getnx(layer)*2 && row-1+i >=0 
    && row-1+i < CTCDBc::getny()*2){ 
    if(k>=0 && k<maxpl && xplane[k]>0){
     AMSCTCRawHit d(row+i,layer,col+j);
     AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
     if(p){
      cofg=cofg+p->loc2gl(AMSPoint(0,0,0))*xplane[k];
      edep+=xplane[k];
     }
     else cerr << "AMSCTCCluster::build-S-GeomVolumeNotFound "<<row-1+i<<" "
     <<col-1+j<<endl;
    }
   }  
   }
  }
  if(edep>0)cofg=cofg/edep;
  // GetTypical Error Size - 
  AMSCTCRawHit d(1,1,1);
  AMSgvolume *p0= AMSJob::gethead()->getgeomvolume(d.crgid(0));
  if(p0 ){
          xsize=p0->getpar(0);
          ysize=p0->getpar(1);
          zsize=p0->getpar(2)*2;
  }
  AMSPoint ecoo(xsize,ysize,zsize);
  if(edep>=TOFRECFFKEY.ThrS){
   AMSEvent::gethead()->addnext(AMSID("AMSCTCCluster",layer-1),
   new     AMSCTCCluster(status,layer,cofg,ecoo,edep,sqrt(edep)));
   for(i=-1;i<2;i++){ 
    for(j=-1;j<2;j++){
      integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
      if(k>=0 && k<maxpl && xplane[k]>0)xplane[k]=0;
    }
   }
   addnew(xstatus,xplane,maxpl,layer);
  }
}
}


integer AMSCTCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSCTCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}




