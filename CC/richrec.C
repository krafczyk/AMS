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
  geant origin[RICmaxpmts][RICnwindows][3];
  geant momentum[RICmaxpmts][RICnwindows][3];
  AMSRichMCHit *hits;
#ifdef __AMSDEBUG__
  integer total=0;
  integer real=0;
#endif

  // First clean up array
  
  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++)
      n_hits[i][j]=0;
  
  // Fill it with the values stored on AMSRichMCHit

  for(hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0);
      hits;hits=hits->next())
    {
      integer pmt,window;
      geant x,y;
      switch(hits->getkind()){
      case 50: // Dealing with Cerenkov Photons
	
	pmt=hits->getid();
	x=hits->getcoo(0)-RICHDB::pmt_p[pmt][0]+RICcatolength/2;
	y=hits->getcoo(1)-RICHDB::pmt_p[pmt][1]+RICcatolength/2;
	x/=RICcatolength/4;
	y/=RICcatolength/4;
	window=4*integer(y)+integer(x);

	n_hits[pmt][window]++;
	origin[pmt][window][0]=hits->getorigin(0);
	origin[pmt][window][1]=hits->getorigin(1);
	origin[pmt][window][2]=hits->getorigin(2);
	momentum[pmt][window][0]=hits->getmomentum(0);
	momentum[pmt][window][1]=hits->getmomentum(1);
	momentum[pmt][window][2]=hits->getmomentum(2);
    
	break;
      }
    
    }


  for(int i=0;i<RICHDB::total;i++)
    for(int j=0;j<RICnwindows;j++){
      integer adc;

      adc=integer(RICHDB::pmt_response(n_hits[i][j]));
#ifdef __AMSDEBUG__
      if(n_hits[i][j]>0) total++;
#endif

      if(adc>RICHDB::c_ped){// So we get a signal
      	AMSPoint orig(origin[i][j][0],origin[i][j][1],origin[i][j][2]);
	AMSPoint mom(momentum[i][j][0],momentum[i][j][1],momentum[i][j][2]);

	AMSEvent::gethead()->
	  addnext(AMSID("AMSRichRawEvent",0), 
		  new AMSRichRawEvent(16*i+j,n_hits[i][j]>0?0:1,adc,
		  RICHDB::pmt_p[i][0]+(j%4)*RICcatolength/4-RICcatolength/2,
		  RICHDB::pmt_p[i][1]+(j/4)*RICcatolength/4-RICcatolength/2,
				      orig,mom));
      
  
#ifdef __AMSDEBUG__
	real++;
#endif
    }
#ifdef __AMSDEBUG__
      if(n_hits[i][j])
	HF1(RIChistos+1,adc,1.);
      else
	HF1(RIChistos+2,adc,1.);
#endif
    }

#ifdef __AMSDEBUG__
  if(total>2) HF1(RIChistos+0,total,1.);  
  if(real>0){
    cout<<"RICH:mc_build Number of hits detected:" << real << endl;
    cout<<"RICH:mc_build Number of hits in the realm:" << total <<endl;
  }
#endif
}


