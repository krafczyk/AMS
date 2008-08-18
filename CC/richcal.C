#include "richcal.h"

AMSRichCalChannel* AMSRichCal::_channel[RICmaxpmts*RICnwindows];
int    AMSRichCal::_histos=RICmaxpmts*RICnwindows;
unsigned int AMSRichCal::_processed_events=0;
unsigned int AMSRichCal::reason=0;   // Reason to neglect an event
int AMSRichCal::_calibration=0;

// These are public 
unsigned int AMSRichCal::_events_per_orbit=1000;   // LVL1 events per orbit
unsigned int AMSRichCal::_orbits_per_calibration=5; // Number of orbits to calibrate
double       AMSRichCal::_threshold=1;          // sigmas to consider a PMT ini an orbit an outsider
float        AMSRichCal::_lower_threshold=0.25;   // Region in p.e. to monitor
float        AMSRichCal::_upper_threshold=1.75;  

float        AMSRichCal::_charge_lower_threshold=0.5;   // Region in charge to monitor
float        AMSRichCal::_charge_upper_threshold=1.5;  

int          AMSRichCal::_low_stat_threshold=83;       // lower limit in hits to consider a channel for monitoring


// Calibration status constants
int AMSRichCalChannel::calibrated=1;
int AMSRichCalChannel::calibrated_but_failed=2;
int AMSRichCalChannel::low_stat=3;


void AMSRichCal::init(int bins,float minx,float maxx){
  _calibration=1;
  for(int i=0;i<_histos;i++){
    _channel[i]=new AMSRichCalChannel(bins,minx,maxx);
  }
}


void AMSRichCal::finish(){
  // Free memory
  for(int i=0;i<_histos;i++){
    delete _channel[i];
  }
}


AMSRichRing *AMSRichCal::event_selection(){
  AMSContainer * pcnt;
  int i,npart,ncharge,nbeta=0;

  npart=0;
  AMSParticle *particle;
  for(i=0;;i++){
    pcnt=AMSEvent::gethead()->getC("AMSParticle",i);
    if(pcnt){npart+=pcnt->getnelem();if(AMSEvent::gethead()->getheadC("AMSParticle",i)) particle=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",i);}
    else break;
  }
  if(npart!=1 || !particle){reason=1;return 0;}

  if(!particle->getptrack()) {reason=102;return 0;}
  if(!particle->getpcharge()) {reason=103;return 0;}
  if(!particle->getpbeta()) {reason=104;return 0;}

  // Check that we have a ring
  pcnt=AMSEvent::gethead()->getC("AMSRichRing",0);
  if(!pcnt) {reason=2;return 0;}
  int nring=pcnt->getnelem(); 
  if(nring<=0) {reason=3;return 0;}

  AMSRichRing *ring=particle->getprich();
  if(!ring) {reason=4;return 0;}
  uinteger naf_ring=ring->checkstatus(richconst::naf_ring);
  if(naf_ring){reason=5;return 0;}

  // Check that the event is clean enough
  if(AMSRichRawEvent::Npart()>2){reason=6; return 0;}

  uinteger dirty_ring=ring->checkstatus(richconst::dirty_ring);
  if(dirty_ring){
    // Check if it has been rebuilt
    AMSRichRing *next=ring->next();
    if(!next) {reason=7;return 0;}  // not rebuilt
    if(next->gettrack()!=ring->gettrack()) {reason=8;return 0;}  // not rebuilt
    if(next->checkstatus(dirty_ring)) {reason=9;return 0;}  // not rebuilt
    ring=next;
  }

  number npexp=ring->getnpexp();
  number prob=ring->getprob();
  number npcol=ring->getcollnpe();

  if(npexp<2) {reason=10;return 0;}
  if(prob<1e-3){reason=11;return 0;}
  if(npcol<=0){reason=12;return 0;}

  double charge=sqrt(npcol/npexp);
  if(charge<_charge_lower_threshold || charge>_charge_upper_threshold) {reason=12;return 0;}

  return ring;   // Selected
}


void AMSRichCal::process_event(){
  reason=0;
#ifdef __AMSDEBUG__
  static int hits_accounted=0; // this is for DEBUGGING
  static time_t init_time=0;
  if(init_time==0) init_time=AMSEvent::gethead()->gettime();
#endif

  // First, check that the event has LVL1
  Trigger2LVL1 *ptr2;
  ptr2=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(!ptr2 || !ptr2->GlobFasTrigOK()) return;

  // Increase counter of level 1 events
  _processed_events++;

  // If this is a good rich event, add the hits to the monitors
  AMSRichRing *ring=event_selection();
  if(ring){
    // Loop through all the hits and accumulate the statistics
    
    int ring_counter=0;
    for(AMSRichRing *r=(AMSRichRing*)AMSEvent::gethead()->getheadC("AMSRichRing",0);r;r=r->next()){
      if(r==ring) break;
      ring_counter++;
    }
   
    for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
      if(hit->checkstatus(uinteger(1)<<ring_counter)){
	// Hit belongs to ring, thus add it
	int id=hit->getchannel();
	//	_channel[id]->add(hit->getnpe());


	// Use dynamic gain if within the right range
	if(hit->getnpe()<0.25 || hit->getnpe()>1.75) continue;

	int counts=hit->getcounts();
	int pmt,window;
	RichPMTsManager::UnpackGeom(id,pmt,window);

	geant old_gain=RichPMTsManager::_Gain(pmt,window);
	geant sigma_over_q=RichPMTsManager::_GainSigma(pmt,window)/
	  old_gain;
	geant gain1_over_gain5=RichPMTsManager::_Gain(pmt,window,0)/
	  old_gain;
	geant sigma_over_q_0=RichPMTsManager::_GainSigma(pmt,window,0)/
	  RichPMTsManager::_Gain(pmt,window,0);

	geant new_gain=0.9*old_gain+0.1*geant(counts);
	
	RichPMTsManager::_Gain(pmt,window)=new_gain;
	RichPMTsManager::_GainSigma(pmt,window)=sigma_over_q*new_gain;
	RichPMTsManager::_Gain(pmt,window,0)=gain1_over_gain5*new_gain;
	RichPMTsManager::_GainSigma(pmt,window,0)=sigma_over_q_0*gain1_over_gain5*new_gain;

#ifdef __AMSDEBUG__
	hits_accounted++;  // this is for DEBUGGING
#endif
      }
    }
 
  }
  /*
#ifdef __AMSDEBUG__
  cout<<"Monitoring inform: processes events "<<_processed_events<<" event "<<AMSEvent::gethead()->getEvent()<<" orbits "<<_processed_events/_events_per_orbit<<" HITS "<<hits_accounted<<" TIME FROM START "<<(AMSEvent::gethead()->gettime()-init_time)/60<<" minutes  ring was"<<ring<<" HITS "<<(ring?ring->getused():0)<<" REASON "<<reason<<" hits "<<AMSEvent::gethead()->getC("AMSRichRawEvent",0)->getnelem()<<endl;
#endif
  
  // If this is an orbit limit, increase the number of orbits and
  // check if calibration is needed
  if((_processed_events%_events_per_orbit)==0){

    // Start monitoring of a single channel
    for(int i=0;i<_histos;i++){
      AMSRichCalChannel::Histo &histo=*(_channel[i]->monitor());
      int entries=histo.entries();
#ifdef __AMSDEBUG__
      cout<<"********** Entries for channel "<<i<<" : "<<entries<<" events "<<_processed_events<<endl;
#endif

      if(entries<_low_stat_threshold){
	// flag the channel or whatever
	_channel[i]->FlagLowStat();
	continue;
      }
      
      double mean,rms,counts;
      int minx=histo.findbin(_lower_threshold);
      int maxx=histo.findbin(_upper_threshold);
      
      for(int bin=minx;bin<=maxx;bin++){
	mean+=histo.getx(bin)*histo[bin];
	rms+=histo.getx(bin)*histo[bin]*histo.getx(bin)*histo[bin];
	counts+=histo[bin];
      }

      assert(counts>0);

      mean/=counts;
      rms/=counts;
      rms-=mean*mean;
      double error=sqrt(mean/counts);

#ifdef __AMSDEBUG__
      cout<<">>> Monitoring of "<<i<<" gives "<<mean<<" "<<error<<endl;
#endif

      // Compare the obtained value with the expected one
      if((mean-1)>_threshold*error){
	_channel[i]->AddOrbit();

	if(_channel[i]->GetCounter()==_orbits_per_calibration){
	  if(_channel[i]->Calibrate()){
	    // update TDVs and the status words
	    int pmt,window;
	    RichPMTsManager::UnpackGeom(i,pmt,window);
	    int current_status=RichPMTsManager::Status(pmt,window);
	    RichPMTsManager::_Status(pmt,window)=current_status%10+AMSRichCalChannel::calibrated;
	  }else{
	    // Set the status of the channel as calibration failed
	    int pmt,window;
	    RichPMTsManager::UnpackGeom(i,pmt,window);
	    int current_status=RichPMTsManager::Status(pmt,window);
	    RichPMTsManager::_Status(pmt,window)=current_status%10+AMSRichCalChannel::calibrated_but_failed;
	  }

	}

      }else _channel[i]->ForgetOrbit();


      // Fill some statistic to check the method
    }

 
  }
  */
}
