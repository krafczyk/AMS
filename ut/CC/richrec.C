#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <commons.h>
#include <ntuple.h>
#include <richdbc.h>
#include <richrec.h>
#include <mccluster.h>
#include <iostream>
//#include <vector>
#include <valarray>
void AMSRichRawEvent::mc_build(){
  valarray<integer> n_hits(0,RICnwindows);
  valarray<AMSPoint> origin(RICnwindows);
  valarray<AMSPoint> momentum(RICnwindows);
/*
  vector<integer> n_hits(RICnwindows,0);
  vector<AMSPoint> origin(RICnwindows);
  vector<AMSPoint> momentum(RICnwindows);
*/
  
#ifdef __AMSDEBUG__
  integer total=0;
  integer real=0;
#endif

  // First clean up array
{
     // do not need any more as done during init
}  
  
  // Fill it with the values stored on AMSRichMCHit

  for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",1);
      hits;hits=hits->next())
    {
      uinteger pmt=hits->getid();
      if(pmt>=RICmaxpmts){
          cerr<< "AMSRichRawEvent::mc_build-PMTNoError "<<pmt<<endl;
          break;
      }
      switch(hits->getkind()){
      case Cerenkov_Photon:
	geant x=hits->getcoo(0)-RICHDB::pmt_p[pmt][0]+RICcatolength/2;
	geant y=hits->getcoo(1)-RICHDB::pmt_p[pmt][1]+RICcatolength/2;
	x/=RICcatolength/4;
	y/=RICcatolength/4;
	uinteger window=4*integer(y)+integer(x);
        if(window>=RICnwindows ){
         cerr<< "AMSRichRawEvent::mc_build-WindowNoError "<<window<<endl;
         break;
        }
	n_hits[window]++;
	origin[window]=hits->getorigin();
	momentum[window]=hits->getmomentum();
    
	break;
      }
      if(hits->testlast()){
       for(int j=0;j<RICnwindows;j++){
        integer adc=integer(RICHDB::pmt_response(n_hits[j]));
#ifdef __AMSDEBUG__
        if(n_hits[j]>0) total++;
#endif
        if(adc>RICHDB::c_ped){// So we get a signal
     	 AMSEvent::gethead()->
	  addnext(AMSID("AMSRichRawEvent",0), 
          new AMSRichRawEvent(16*pmt+j,n_hits[j]>0?0:1,adc,
                  RICHDB::pmt_p[pmt][0]+(j%4)*RICcatolength/4-RICcatolength/2,
		  RICHDB::pmt_p[pmt][1]+(j/4)*RICcatolength/4-RICcatolength/2,
                  n_hits[j]>0?origin[j]:AMSPoint(),n_hits[j]>0?momentum[j]:AMSPoint()));
      
  
#ifdef __AMSDEBUG__
	real++;
#endif
    }

#ifdef __AMSDEBUG__
      if(n_hits[j])
	HF1(RIChistos+1,adc,1.);
      else
	HF1(RIChistos+2,adc,1.);
#endif
}
       // clean up the array
       //       for(int iw=0;iw<RICnwindows;iw++)n_hits[iw]=0;
        n_hits=0;           // for valarray only 
      }
    }



#ifdef __AMSDEBUG__
  if(total>2) HF1(RIChistos+0,total,1.);  
  if(real>0){
    cout<<"RICH:mc_build Number of hits detected:" << real << endl;
    cout<<"RICH:mc_build Number of hits in the realm:" << total <<endl;
  }
#endif
}


