// prototype 28.09.1999 by E.Choumilov
//
#include <typedefs.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <ecaldbc.h>
#include <ecalrec.h>
#include <ntuple.h>
//
 integer AMSEcalRawEvent::trpatt=0;
//----------------------------------------------------
void AMSEcalRawEvent::validate(int &status){ //Check/correct RawEvent-structure
}
//----------------------------------------------------
void AMSEcalRawEvent::mc_build(int &stat){
  int i,j,k;
  integer id,nhits;
  number x,y,z,ede,edept(0.),time,timet(0.);
  AMSEcalMCHit * ptr;
//
  stat=1;//bad
  edept=0.;
  nhits=0;
  timet=0.;
  for(i=0;i<ECALDBc::slstruc(3);i++){ // <----- super-layer loop
    ptr=(AMSEcalMCHit*)AMSEvent::gethead()->
               getheadC("AMSEcalMCHit",i,1); // last 1  to get sorted container
    while(ptr){ // <--- geant-hits loop in superlayer:
      nhits+=1;
      id=ptr->getid();
      ede=ptr->getedep()*1000;// MeV
      edept+=ede;
      x=ptr->getcoo(0);
      y=ptr->getcoo(1);
      z=ptr->getcoo(2);
      time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
      timet+=ede*time;
      ptr=ptr->next();  
    } // ---> end of hit-loop in superlayer
  } // ---> end of super-layer loop
//
  if(ECMCFFKEY.mcprtf==1){
    HF1(ECHIST+1,geant(nhits),1.);
    HF1(ECHIST+2,geant(edept),1.);
    HF1(ECHIST+3,geant(timet/edept),1.);
  }
  if(nhits>0)stat=0;
}
//---------------------------------------------------
