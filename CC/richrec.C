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
//#include <valarray>
void AMSRichRawEvent::mc_build(){
  // Add noise...

  geant mean_noisy=integer(RICnwindows*RICHDB::total*RICHDB::prob_noisy);
  int dummy1;
  integer n_noisy;

  POISSN(mean_noisy,n_noisy,dummy1);

  for(int i=0;i<n_noisy;i++){
    // Get a random channel number and make it noisy

    integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
    AMSRichMCHit::noisyhit(channel);
  }


  // Construct event: channel signals

  
  char with_noise=0; // flag to add the noise
  geant signal=0;

  // Fill it with the values stored on AMSRichMCHit
  for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);
      hits;hits=hits->next())
    {
      uinteger channel=hits->getchannel();
      
      if(channel>=RICnwindows*RICHDB::total){
          cerr<< "AMSRichRawEvent::mc_build-ChannelNoError "<<channel<<endl;
          break;
      }


      switch(hits->getid()){
      case Noise:
	with_noise=1;
	signal+=hits->getcounts();
	break;
      case Cerenkov_photon:
	signal+=hits->getcounts();    
	break;
      }
      if(hits->testlast()){
        integer adc=integer(signal+(with_noise?0:AMSRichMCHit::adc_empty()));
	signal=0;
	with_noise=0;

        if(adc>RICHDB::sigma_ped*RICHDB::c_ped+RICHDB::ped){// So we get a signal
 
    	 AMSEvent::gethead()->
	  addnext(AMSID("AMSRichRawEvent",0), 
          new AMSRichRawEvent(channel,adc));
      
  
	}
      }
    }

}


void AMSRichRawEvent::_writeEl(){
  
  RICEventNtuple* cluster=AMSJob::gethead()->getntuple()->Get_richevent();
  
  if(cluster->Nhits>=MAXRICHITS) return;

  cluster->channel[cluster->Nhits]=_channel;
  cluster->adc[cluster->Nhits]=_counts;
  cluster->x[cluster->Nhits]=RICHDB::x(_channel);
  cluster->y[cluster->Nhits]=RICHDB::y(_channel);
  cluster->Nhits++;

}



