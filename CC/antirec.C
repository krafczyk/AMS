//
// May 27, 1997 Primitive anti Rec
//
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <antirec.h>
#include <ntuple.h>
void AMSAntiRawCluster::siantidigi(){
  const integer MAXANTI=16;
  static number counter[2][MAXANTI];
  VZERO(counter,2*MAXANTI*sizeof(counter[0][0])/sizeof(geant));
  AMSAntiMCCluster * ptr=(AMSAntiMCCluster*)
  AMSEvent::gethead()->
   getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
   geant up=0;
   geant down=0;
   number c0=exp(-ANTIMCFFKEY.PMulZPos/ANTIMCFFKEY.LZero);
  while(ptr){
   integer sector=ptr->getid();
   number edep=ptr->getedep()*ANTIMCFFKEY.GeV2PhEl;
   number z=ptr->getcoo(2);
   up+=edep*c0*exp(z/ANTIMCFFKEY.LZero);
   down+=edep*exp(-z/ANTIMCFFKEY.LZero)*c0;
   if(ptr->testlast()){
    integer nup,ndown,ierr;
    POISSN(up,nup,ierr); 
    POISSN(down,ndown,ierr); 
    up=0;
    down=0;
    #ifdef __AMSDEBUG__
      assert(sector >0 && sector <= MAXANTI);
    #endif
    counter[0][sector-1]=nup;
    counter[1][sector-1]=ndown;
   } 
 
   ptr=ptr->next();  
  }
   siantinoise(counter[0],counter[1],MAXANTI);

  int i,j;
  for (i=0;i<2;i++){
    for (j=0;j<MAXANTI;j++){ 
     if(counter[i][j] > 0)
     AMSEvent::gethead()->addnext(AMSID("AMSAntiRawCluster",i),
     new AMSAntiRawCluster(0,j+1,i,counter[i][j]));
     
    }
  }
}

void AMSAntiRawCluster::siantinoise(number counter1[], number counter2[], integer nm){
 int i,j;
   for(j=0;j<nm;j++){
    integer ns=0;
    integer ierr=0;
    POISSN(ANTIMCFFKEY.SigmaPed,ns,ierr);
    counter1[j]=counter1[j]+ns-ANTIMCFFKEY.SigmaPed;
    POISSN(ANTIMCFFKEY.SigmaPed,ns,ierr);
    counter2[j]=counter2[j]+ns-ANTIMCFFKEY.SigmaPed;
   } 
}


void AMSAntiRawCluster::_writeEl(){
}

void AMSAntiRawCluster::_copyEl(){
}


void AMSAntiRawCluster::_printEl(ostream & stream){
  stream <<"AMSAntiRawCluster "<<_sector<<" "<<_updown<<" "<<_signal<<endl;
}



void AMSAntiCluster::_writeEl(){
  // fill the ntuple
  if(AMSAntiCluster::Out( IOPA.WriteAll ||  checkstatus(AMSDBc::USED ))){
static AntiClusterNtuple TN;
static integer init=0;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"AntiClus",TN.getaddress(),
  "AntiCluster:I*4,AntiStatus:I*4,AntiSector:I*4,AntiEdep:R*4,AntiCoo(3):R*4,AntiErCoo(3):R*4");

}
TN.Event()=AMSEvent::gethead()->getid();
  TN.Status=_status;
  TN.Sector=_sector;
  TN.Edep=_edep;
  int i;
  for(i=0;i<3;i++)TN.Coo[i]=_coo[i];
  for(i=0;i<3;i++)TN.ErrorCoo[i]=_ecoo[i];
  HFNTB(IOPA.ntuple,"AntiClus");
  }
}



void AMSAntiCluster::_copyEl(){
}


void AMSAntiCluster::_printEl(ostream & stream){
  stream <<"AMSAntiCluster "<<_status<<" "<<_sector<<" "<<_edep<<" "<<_coo<<  " "<<_ecoo<<endl;
}


 
void AMSAntiCluster::print(){
AMSContainer *p =AMSEvent::gethead()->getC("AMSAntiCluster",0);
 if(p)p->printC(cout);
}

//===========================================================================



void AMSAntiCluster::build(){
    const integer MAXANTI=16;
    static number counter[2][MAXANTI];
    VZERO(counter,2*MAXANTI*sizeof(counter[0][0])/sizeof(geant));
    int i,j;
    for(i=0;i<2;i++){ 
     AMSAntiRawCluster *ptr=(AMSAntiRawCluster*)AMSEvent::gethead()->
     getheadC("AMSAntiRawCluster",i);
     while(ptr){
      integer sector=ptr->getsector()-1;
      #ifdef __AMSDEBUG__
       assert(sector >=0 && sector < MAXANTI);
      #endif
      counter[i][sector]=counter[i][sector]+ptr->getsignal();
      ptr=ptr->next();
     }
    }
    for (j=0;j<MAXANTI;j++){
     number sup=counter[0][j];
     number sdown=counter[1][j];
     if(sup+sdown > ANTIRECFFKEY.ThrS){
      AMSgvolume *pg=AMSJob::gethead()->getgeomvolume(AMSID("ANTS",j+1));
      #ifdef __AMSDEBUG__
       assert (pg != NULL);
      #endif     
      //
      // Geometry dependent
      // Should be changed if not "TUBS"
      //
      number par[5];
      for(int i=0;i<5;i++)par[i]=pg->getpar(i);
      number r=(par[0]+par[1])/2;
      number er=(par[1]-par[0])/2;
      number phi=(par[3]+par[4])/2;
      number ephi=(par[4]-par[3])/2;
      number delta=(sup-sdown)/(sup+sdown);
      number z;
      if(delta > -1. && delta < 1.)z=ANTIMCFFKEY.LZero/2*log((1.+delta)/(1.-delta));
      else if (delta >=1)z=par[2];
      else z=-par[2];
      number edep=(sup+sdown)*ANTIRECFFKEY.PhEl2MeV/
      (exp(z/ANTIMCFFKEY.LZero)+exp(-z/ANTIMCFFKEY.LZero));
      number ddelta=1/sqrt(sup+sdown);
      number z1,z2;
      number d1=delta+ddelta;
      if(d1 >=1)z1=par[2];      
      else if(d1 <=-1)z1=-par[2];      
      else z1=ANTIMCFFKEY.LZero/2*log((1.+d1)/(1.-d1));
      number d2=delta-ddelta;
      if(d2 <=-1)z2=-par[2];      
      else if(d2 >=1)z2=par[2];      
      else z2=ANTIMCFFKEY.LZero/2*log((1.+d2)/(1.-d2));
      number ez=fabs(z1-z2)/2;
      AMSPoint coo(r,phi,z);
      AMSPoint ecoo(er,ephi,ez);
      integer status=0;
      if (sup ==0 || sdown == 0)status=AMSDBc::BAD;
      AMSEvent::gethead()->addnext(AMSID("AMSAntiCluster",0),
      new     AMSAntiCluster(status,j+1,edep,coo,ecoo));

      
     }
    }
}





integer AMSAntiCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSAntiCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
