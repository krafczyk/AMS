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


void AMSRichRawEvent::mc_build(){
  integer n_hits[RICmaxpmts][RICnwindows];


  /*
  // First clean up array
  
  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++)
      n_hits[i][j]=0;

  // Fill it with the values stored on AMSRichMCHit

  AMSRichMCHit *hits;

  hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0);
  while(hits){
    switch(hits->getkind()){
    case 0:
      integer pmt=integer(hits->getid()/10000);
      integer window=integer(hits->getid()%10000);      
      n_hits[pmt][window]++;
      break;
    }
      

      hits=hits->next();
    
  }

  integer n_times=0;
  integer real=0;
  integer total=0;

  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++){
      integer adc;
      adc=RICHDB::pmt_response(n_hits[i][j]);
      real+=n_hits[i][j]?1:0;
      if(adc>RICHDB::c_ped+1){// So we get a signal
      	AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0), 
      				     new AMSRichRawEvent(10000*i+j,0,adc,
      							 RICHDB::x(i,j),
      							 RICHDB::y(i,j)));
      	n_times++;
      }
#ifdef __AMSDEBUG__
      if(n_hits[i][j])
	HF1(RIChistos+1,adc,1.);
      else
	HF1(RIChistos+2,adc,1.);
#endif
    }

#ifdef __AMSDEBUG__
  HF1(RIChistos+0,n_times,1.);  
  if(real>0){
    cout<<"RICH:mc_build Number of photons detected:" << n_times << endl;
    cout<<"RICH:mc_build Number of photons in the realm:" << real <<endl;
  }
#endif
*/
}


