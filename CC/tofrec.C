#include <tofrec.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <ntuple.h>

void AMSTOFRawCluster::sitofdigi(){
  AMSgObj::BookTimer.start("SITOFDIGI");
  AMSTOFMCCluster * ptr=(AMSTOFMCCluster*)
  AMSEvent::gethead()->
   getheadC("AMSTOFMCCluster",0,1); // last 1  to test sorted container
  integer const maxpl=20;
  static number xplane[4][maxpl];
  static number xtime[4][maxpl];
  static number xtimed[4][maxpl];
  static number xz[4][maxpl];
  VZERO(xtime,maxpl*sizeof(number));
  VZERO(xtimed,maxpl*sizeof(number));
  VZERO(xplane,maxpl*sizeof(number));
  VZERO(xz,maxpl*sizeof(number));

  while(ptr){
   integer ntof,plane,status;
   ntof=(ptr->idsoft)/100-1;
   plane=(ptr->idsoft)%100-1;
#ifdef __AMSDEBUG__
   assert(plane>=0 && plane< maxpl);
   assert (ntof>=0 && ntof<4);
#endif
   xplane[ntof][plane]+=ptr->edep*1000;
   xtime[ntof][plane]+=ptr->tof*(ptr->edep)*1000;
   const number c=2.99792e10;
   integer ixy;
   if(ntof==0)ixy=1;
   else if (ntof==1)ixy=0;
   else if (ntof==2)ixy=0;
   else             ixy=1;
   xtimed[ntof][plane]+=2*ptr->xcoo[ixy]*(ptr->edep)*1000/c;
   xz[ntof][plane]=ptr->xcoo[2];
   status=0;
   integer last;
   last=ptr->testlast();
   ptr=ptr->next();  
   
  }
  for(integer kk=0;kk<4;kk++){
  for(integer i=0;i<maxpl;i++){
    if(xplane[kk][i]>TOFMCFFKEY.Thr){
     xtime[kk][i]=xtime[kk][i]/xplane[kk][i];
     xtimed[kk][i]=xtimed[kk][i]/xplane[kk][i];
#if 1  // change to zero when Eugeni  writes a proper code
     xtime[kk][i]+=TOFMCFFKEY.TimeSigma*rnormx();
     xtimed[kk][i]+=TOFMCFFKEY.TimeSigma*rnormx()*sqrt(2.);

#else
      // Eugeni code here
#endif
    AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0),
    new AMSTOFRawCluster(0,kk+1,i+1,xz[kk][i],xplane[kk][i],
    xtime[kk][i],xtimed[kk][i]));
    }

  }
  }
  AMSgObj::BookTimer.stop("SITOFDIGI");
}


void AMSTOFRawCluster::_writeEl(){
}


void AMSTOFRawCluster::_copyEl(){
}


void AMSTOFRawCluster::_printEl(ostream & stream){
  stream <<"AMSTOFRawCluster "<<_ntof<<" "<<_edep<<" "<<_plane<<endl;
}

AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};
void AMSTOFCluster::_writeEl(){
// fill the ntuple
if(AMSTOFCluster::Out( IOPA.WriteAll ||  checkstatus(AMSDBc::USED ))){
static TOFClusterNtuple TN;
static integer init=0;
if(init++==0){
//book the ntuple block
  HBNAME(IOPA.ntuple,"TOFClust",TN.getaddress(),
  "TOFCluster:I*4,TOFStatus:I*4,Ntof:I*4,Plane:I*4, TOFEdep:R*4, TOFTime:R*4,TOFETime:R*4,TOFCoo(3):R*4,TOFErCoo(3):R*4");

}
  TN.Event()=AMSEvent::gethead()->getid();
  TN.Status=_status;
  TN.Ntof=_ntof;
  TN.Plane=_plane;
  TN.Edep=_edep;
  TN.Time=_time;
  TN.ErrTime=_etime;
  int i;
  for(i=0;i<3;i++)TN.Coo[i]=_Coo[i];
  for(i=0;i<3;i++)TN.ErrorCoo[i]=_ErrorCoo[i];
  HFNTB(IOPA.ntuple,"TOFClust");
}
}

void AMSTOFCluster::_copyEl(){
}


void AMSTOFCluster::_printEl(ostream & stream){
  stream <<"AMSTOFCluster "<<_status<<" "<<_ntof<<" "<<_plane<<" "<<_edep<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
}


 
void AMSTOFCluster::print(){
for(int i=0;i<4;i++){
AMSContainer *p =AMSEvent::gethead()->getC("AMSTOFCluster",i);
 if(p)p->printC(cout);
}
}



void AMSTOFCluster::build(){
for(integer kk=1;kk<5;kk++){
AMSTOFRawCluster *ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
getheadC("AMSTOFRawCluster",0);
integer const maxpl=22;
static number xplane[maxpl];
static AMSTOFRawCluster * xptr[maxpl];
VZERO(xplane,maxpl*sizeof(number)/4);
VZERO(xptr,maxpl*sizeof(AMSTOFRawCluster*)/4);
while (ptr){
  if(ptr->getntof()==kk){
   integer plane=ptr->getplane();
#ifdef __AMSDEBUG__
   assert(plane>0 && plane < maxpl-1);
#endif
   xplane[plane]=ptr->getedep();
   xptr[plane]=ptr;
  }
 ptr=ptr->next();
}

for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > TOFRECFFKEY.Thr1 && xplane[i]>= xplane[i-1] 
 && xplane[i]>= xplane[i+1] ){
 ptr=xptr[i];
#ifdef __AMSDEBUG__
 assert(ptr!=NULL);
#endif
 integer ntof,plane,status;
 number time,edep,cofg,timed;
 AMSPoint coo,ecoo;
 plane=i;
 ntof=ptr->getntof();
 timed=ptr->gettimeD();
 status=ptr->getstatus();
 edep=0;
 cofg=0;
 for(int j=i-1;j<i+2;j++){
   edep+=xplane[j];
   cofg+=xplane[j]*(j-i);   
 }
 time=ptr->gettime();
 if(edep>TOFRECFFKEY.ThrS){
   number etime, etimed;
#if 1  // change to zero when Eugeni  writes a proper code
     etime=TOFMCFFKEY.TimeSigma;
     etimed=TOFMCFFKEY.TimeSigma*sqrt(2.);
#else
      // Eugeni code here
#endif
 
   coo[2]=ptr->getz()+0.5;
   coo[0]=0;
   coo[1]=0;
   ecoo[2]=10000;   // big
   const number c=2.99792e10;
   integer ix;
   if(ntof==1)ix=1;
   else if (ntof==2)ix=0;
   else if (ntof==3)ix=0;
   else             ix=1;
   integer iy;
   if(ntof==1)iy=0;
   else if (ntof==2)iy=1;
   else if (ntof==3)iy=1;
   else             iy=0;

   coo[ix]=timed/2*c;
   ecoo[ix]=etimed/2*c;
   float padl=TOFDBc::plnstr(5);
   coo[iy]=cofg/edep*padl+TOFDBc::gettsc(ntof-1,i-1);
   ecoo[iy]=padl/2.7;
   AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ntof-1),
   new     AMSTOFCluster(status,ntof,plane,edep,coo,ecoo,time,etime));
   xplane[i-1]=0;
   xplane[i]=0;
   xplane[i+1]=0; 
   i=0;
 }
 }    
 
}
}
}


integer AMSTOFCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
