#include <iostream.h>
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <amsstl.h>
#include <commons.h>
#include <ntuple.h>
#include <richdbc.h>
#include <richrec.h>
#include <mccluster.h>
//#include <trigger3.h>

void AMSRichRawEvent::mc_build(){
  integer n_hits[RICmaxpmts][RICnwindows];

  // First clean up array
  
  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++)
      n_hits[i][j]=0;

  // Fill it with the values stored on AMSRichMCHit

  AMSRichMCHit *hits;

  hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0);
  int n_times=0;
  while(hits){
    n_times++;
    integer pmt=integer(hits->getid()/10000);
    integer window=integer(hits->getid()%10000);

    geant x=RICHDB::x(pmt,window);
    geant y=RICHDB::y(pmt,window);
    geant xr=hits->getcoo(0);
    geant yr=hits->getcoo(1);

#ifdef __AMSDEBUG__
    cout << "RICH:mc-build pos diff x:" << xr-x << " y:" << yr-y << endl; // debug

#endif
   
    hits=hits->next();
    
  }
}
