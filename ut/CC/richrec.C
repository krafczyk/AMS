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
  AMSRichMCHit *hits;
  integer *id_list=new integer[RICHDB::total*RICnwindows];
  integer bottom=0;


#ifdef __AMSDEBUG__
  integer total=0;
  integer real=0;
#endif

  // Fill it with the values stored on AMSRichMCHit
  
  for(hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0);
      hits;hits=hits->next()){
    AMSRichMCHit *present;
    integer pmt;
    integer window;
    integer id,pos;
    geant x,y;

    pmt=hits->getid();
    x=hits->getcoo(0)-RICHDB::pmt_p[pmt][0]+RICcatolength/2;
    y=hits->getcoo(1)-RICHDB::pmt_p[pmt][1]+RICcatolength/2;
    x/=RICcatolength/4;
    y/=RICcatolength/4;
    window=4*integer(y)+integer(x);

    if(window>RICnwindows || window<0){ //Error
      cerr<< "AMSRichRawEvent::mc_build-WindowNoError "<<window<<endl;
      break;
    }

    id=RICnwindows*pmt+window;

    // Check if it's on the list
    pos=-1;
    for(integer i=0;i<bottom;i++) if(id==id_list[i]) pos=i;

    if(pos!=-1) continue;

    integer n_hits=0;
    integer adc;
    geant origin[3];
    geant momentum[3];
    if(bottom==RICHDB::total*RICnwindows){ //Error
      cerr<< "AMSRichRawEvent::mc_build-IdNoError "<<bottom<<endl;
      break;
    }
    id_list[bottom++]=id;
    



    for(present=hits;present;present=present->next()){
      geant xa,ya;
      xa=present->getcoo(0)-RICHDB::pmt_p[pmt][0]+RICcatolength/2;
      ya=present->getcoo(1)-RICHDB::pmt_p[pmt][1]+RICcatolength/2;
      xa/=RICcatolength/4;
      ya/=RICcatolength/4;

      if(present->getid()*RICnwindows+4*integer(ya)+integer(xa)==id)
      switch(present->getkind()){
      case 50: // Cerenkov photons
	n_hits++;
	origin[0]=hits->getorigin(0);
	origin[1]=hits->getorigin(1);
	origin[2]=hits->getorigin(2);
	momentum[0]=hits->getmomentum(0);
	momentum[1]=hits->getmomentum(1);
	momentum[2]=hits->getmomentum(2);
	
	break; // Don't forget the break
      }
    }


#ifdef __AMSDEBUG__
    if(n_hits>0) total++;
#endif
      
    // Simulate the PMT response
      
    adc=integer(RICHDB::pmt_response(n_hits));
      
    if(adc>RICHDB::c_ped){// So we get a signal
      AMSPoint orig(origin[0],origin[1],origin[2]);
      AMSPoint mom(momentum[0],momentum[1],momentum[2]);
	
      AMSEvent::gethead()->
	addnext(AMSID("AMSRichRawEvent",0), 
		new AMSRichRawEvent(id,n_hits>0?0:1,adc,
				    RICHDB::pmt_p[pmt][0]+(window%4)*RICcatolength/4-RICcatolength/2,
				    RICHDB::pmt_p[pmt][1]+(window/4)*RICcatolength/4-RICcatolength/2,
				    orig,mom));
      
      
#ifdef __AMSDEBUG__
      real++;
#endif
    }
      

#ifdef __AMSDEBUG__
    if(n_hits){
      HF1(RIChistos+1,adc,1.);
      //HF1(RIChistos+2,adc,1.);
    }
#endif
  }


  // Here the PMT array noise should be simulated, but with the
  // actual pedestal is negligible


  
#ifdef __AMSDEBUG__
  if(total>0) HF1(RIChistos+0,total,1.);  
  if(real>0){
    cout<<"RICH:mc_build Number of hits detected:" << real << endl;
    cout<<"RICH:mc_build Number of hits in the realm:" << total <<endl;
  }
#endif

  delete []id_list; //free dynamic memory

}


