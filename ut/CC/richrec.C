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
  geant mean_noisy=RICnwindows*RICHDB::total*RICHDB::prob_noisy;
  int dummy1;
  integer n_noisy;
  integer hitn=1;


  POISSN(mean_noisy,n_noisy,dummy1);

  for(int i=0;i<n_noisy;i++){
    // Get a random channel number and make it noisy

    integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
    AMSRichMCHit::noisyhit(channel);
  }


  // Construct event: channel signals

   int nhits=0;
   int nnoisy=0;
   geant signal=0;

   for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);hits;hits=hits->next()){ // loop on signals
   
      uinteger channel=hits->getchannel();


      if(channel>=RICnwindows*RICHDB::total){
         cerr<< "AMSRichRawEvent::mc_build-ChannelNoError "<<channel<<endl;
          break;
      }
      hits->puthit(hitn); // To construct the pointer list

//      if(hits->getid()!=Noise && hits->getid()!=Cerenkov_photon)
//          continue;    

      if(hits->getid()==Cerenkov_photon) nhits++; //else nnoisy++;
      if(hits->getid()==Noise) nnoisy++;

      if(hits->testlast()){ // last signal in the hit so construct it
        if(nnoisy>0) signal=AMSRichMCHit::noise(); 
          else if(nhits>0) signal=AMSRichMCHit::adc_empty();
        signal+=AMSRichMCHit::adc_hit(nhits);
        nnoisy=0;
        nhits=0;
        if(integer(signal)>=integer(RICHDB::c_ped)+integer(RICHDB::ped)){
         AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
          new AMSRichRawEvent(channel,integer(signal)));
          hitn++;
          signal=0;
        } else {
       for(AMSRichMCHit* clean=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);clean;clean=clean->next()){
         if(clean->gethit()==hitn) clean->puthit(0);
         if(clean->gethit()==-1) break;
       }
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



