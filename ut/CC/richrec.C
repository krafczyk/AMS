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
  while(hits){
//    if(hits->getkind()!=0) continue; // Don't kwnow how to treat others
    if(!hits->getkind()){
     integer pmt=integer(hits->getid()/10000);
     integer window=integer(hits->getid()%10000);
 
     n_hits[pmt][window]++;
    }
    hits=hits->next();
    
  }

  integer n_times=0;
  integer real=0;

  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++){
      integer adc;
      adc=RICHDB::pmt_response(n_hits[i][j]);
      real+=n_hits[i][j];
      if(adc>RICHDB::c_ped+1){// So we get a signal
      	AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0), 
      				     new AMSRichRawEvent(10000*i+j,0,adc,
      							 RICHDB::x(i,j),
      							 RICHDB::y(i,j)));
      	n_times++;
      }
    }
#ifdef __AMSDEBUG__
  cout<<"RICH:mc_build Number of photons detected:" << n_times << endl;
#endif

}


