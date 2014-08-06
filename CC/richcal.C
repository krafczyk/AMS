#include "richcal.h"
#include "richradid.h"
#include "richrec.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <map>

//#define __AMSDEBUG__

map<string,unsigned int>   AMSRichCal::cuts_entries; // Entries for a given cut
vector<string>             AMSRichCal::cuts_order;
vector<bool>               AMSRichCal::cuts_isMask; 
map<string,bool>           AMSRichCal::cuts_passed; // Cuts passed for a single event
map<string,bool>::iterator AMSRichCal::cuts_it;      // Helper
int                        AMSRichCal::last_run=-1;
int                        AMSRichCal::last_time=-1;

bool AMSRichCal::select(string name,bool condition,bool isMask){
  if(cuts_passed.find(name)==cuts_passed.end()){
    cuts_order.push_back(name);
    cuts_passed[name]=false;
    cuts_entries[name]=0;
    cuts_isMask.push_back(isMask);      
  }
  if(condition){cuts_passed[name]=true;cuts_entries[name]++;}
  return condition;
}


void AMSRichCal::reportCuts(){
  unsigned int all_entries=cuts_entries[cuts_order[0]];
  unsigned int prev_entries=all_entries;
  printf("\nCuts Efficiency report ------------------------------------------\n");
  for(int i=0;i<cuts_order.size();i++){
    string &name=cuts_order[i];
    unsigned int entries=cuts_entries[name];
    if(cuts_isMask[i]){
      printf("Mask ---     %35s             Entries %8u   Efficiency %8.5g\n",
	     name.c_str(),
	     entries,
	     double(entries)/double(prev_entries)
	     );
    }else{
      printf("Cut %35s    Entries %8u   Efficiency %8.5g   Total eff %8.5g\n",
	     name.c_str(),
	     entries,
	     double(entries)/double(prev_entries),
	     double(entries)/double(all_entries)
	     );
      prev_entries=entries;
    }
  }
  printf("\n");
}


void AMSRichCal::resetCuts(){
  cuts_order.clear();
  cuts_entries.clear();
  cuts_passed.clear();
}

void AMSRichCal::initCuts(){
  for(int i=0;i<cuts_order.size();i++) cuts_passed[cuts_order[i]]=false;
}

// Macros to facilitate using these guys
#define SELECT(_name,_condition) {if(!select(_name,(_condition))) return 0; }
#define EVERY(_counter)  {\
static int _ANONYMOUS_=-1;\
_ANONYMOUS_++;\
if(_ANONYMOUS_==(_counter)-1)_ANONYMOUS_=0;\
if(_ANONYMOUS_==0){
#define CUTS_NEW_EVENT {SELECT("ALL",true) initCuts();}
#define CUTS_CLEAR {resetCuts();}
#define CUTS_REPORT {reportCuts();}
#define MASK(_name,_condition) {select(#_name,(_condition),true);}
#define CHECK(_name) (cuts_passed.find(#_name)!=cuts_passed.end() && cuts_passed[#_name])
#define END };}

//////////////////////////////////////
float        AMSRichCal::_charge_lower_threshold=0.5;   // Region in charge to monitor
float        AMSRichCal::_charge_upper_threshold=1.5;  
float        AMSRichCal::_momentum_threshold=50;        // Threshold in momentum to select beta=1 using tracker   
float        AMSRichCal::_momentum_gain_threshold=6;        // Threshold in momentum to select beta=1 using tracker   
bool         AMSRichCal::_init=false;
bool         AMSRichCal::_calibration=false;

// Information for refractive index calibration
TH1F         AMSRichCal::betaHisto[RICmaxtiles];
double       AMSRichCal::betaCalibration[RICmaxtiles];
int          AMSRichCal::betaPeakEvents=100;
double       AMSRichCal::betaWindowWidth=6;
int          AMSRichCal::betaBins=100;
double       AMSRichCal::betaLearningFactor=0.25;

vector<double> AMSRichCal::signal[RICmaxpmts*RICnwindows];
double         AMSRichCal::signalCalibration[RICmaxpmts*RICnwindows];
int          AMSRichCal::gainEvents=100;
double       AMSRichCal::gainLearningFactor=0.1;


void AMSRichCal::init(int bins,float minx,float maxx){
#pragma omp critical(richcal)
  if(!_init){
    cout<<"AMSRichCal::init -- Initializing dynamic RICH calibration/monitoring"<<endl;
    for(int i=0;i<RICmaxtiles;i++) betaCalibration[i]=1.0;
    for(int i=0;i<RICmaxpmts*RICnwindows;i++) signal[i].reserve(gainEvents);
    for(int i=0;i<RICmaxpmts*RICnwindows;i++) signalCalibration[i]=1/0.91; //0.91 is the median for the perfectly calibrated case
    _init=true;
    _calibration=true;
  }
}




/**********************************************************************
Select Z=1 beta=1 events
**********************************************************************/
AMSRichRing *AMSRichCal::event_selection(){
  CUTS_NEW_EVENT;
#ifdef __AMSDEBUG__
  EVERY(100) CUTS_REPORT END
#endif

  AMSContainer * pcnt;
  int i,npart;

  Trigger2LVL1 *ptr2;
  ptr2=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  SELECT("Has LVL1",ptr2 && ptr2->GlobFasTrigOK());

  npart=0;
  AMSParticle *particle;
  for(i=0;;i++){
    pcnt=AMSEvent::gethead()->getC("AMSParticle",i);
    if(pcnt){npart+=pcnt->getnelem();if(AMSEvent::gethead()->getheadC("AMSParticle",i)) particle=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",i);}
    else break;
  }

  // Check that we have a single particle with an associated track  
  SELECT("Has a single particle",particle && npart==1);
  //  if(npart!=1 || !particle)return 0;
  SELECT("Has trtrack",particle->getptrack());
  //  if(!particle->getptrack()) return 0;

  //  SELECT("Good momentum",fabs(particle->getmomentum()/particle->getcharge())>_momentum_threshold); 
  //  if(fabs(particle->getmomentum()/particle->getcharge())<_momentum_threshold) return 0;  // Select beta=1


  AMSRichRing *ring=particle->getprich();
  SELECT("Has ring",ring);
  //  if(!ring) return 0;
  uinteger naf_ring=ring->checkstatus(richconst::naf_ring);
  SELECT("Is AGL",!naf_ring); 
  //  if(naf_ring) return 0;

  // Check that the event is clean enough
  SELECT("Rich clean matrix",AMSRichRawEvent::Npart()<=1);
  //  if(AMSRichRawEvent::Npart()>1) return 0;
  uinteger dirty_ring=ring->checkstatus(richconst::dirty_ring);
  SELECT("Clean ring",!dirty_ring);
  //  if(dirty_ring) return 0;

  number npexp=ring->getnpexp();
  number prob=ring->getprob();
  number npcol=ring->getcollnpe();
  integer nused=ring->getused();

  SELECT("Npexp>=2",npexp>=2);
  //  if(npexp<2)  return 0;
  SELECT("Prob>1e-2",prob>1e-2);
  //  if(prob<1e-2)return 0;
  SELECT("Npcol>0",npcol>0);
  //  if(npcol<=0) return 0;
  SELECT("used>3",nused>3);
  //  if(nused<4)  return 0;

  double charge=sqrt(nused/npexp);
  SELECT("Charge in range",charge>=_charge_lower_threshold && charge<=_charge_upper_threshold);
  //  if(charge<_charge_lower_threshold || charge>_charge_upper_threshold) return 0;

  double momentum=fabs(particle->getmomentum()/particle->getcharge());
  MASK(tileMomentum,momentum>_momentum_threshold);
  MASK(gainMomentum,momentum>10);

  return ring;   // Selected
}

double AMSRichCal::computeMedian(vector<double> &v){
  int size=v.size();
  if(size==0) return -1;
  sort(v.begin(),v.end());
  
  double value=0;
  if(size&1){
    // Odd size
    value=v[(size-1)/2];
  }else{
    // Even size
    value=v[size/2]+v[size/2-1];
    value/=2;
  }
  return value;
}

double AMSRichCal::computePeakPos(TH1F &h){
  int peak_bin=h.GetMaximumBin();
  double weight=0,sum=0;
  for(int i=-betaWindowWidth;i<=betaWindowWidth;i++){
    weight+=h.GetBinContent(peak_bin+i);
    sum+=h.GetBinContent(peak_bin+i)*h.GetXaxis()->GetBinCenter(peak_bin+i);
  }
  if(weight>=betaPeakEvents) return sum/weight;
  return -1;
}

void AMSRichCal::process_event(){
  if(!_init) init();
  // If this is a good rich event, add the hits to the monitors
  AMSRichRing *ring=event_selection();
  if(!ring) return;

  if(AMSEvent::gethead()->getrun()>last_run) last_run=AMSEvent::gethead()->getrun();
  if(AMSEvent::gethead()->gettime()>last_time) last_time=AMSEvent::gethead()->gettime();

  /********************* INDEX CALIBRATION ******************/
  if(CHECK(tileMomentum)){
    // Find the radiator tile and store the information
    int id=ring->_tile_id;
    if(RichRadiatorTileManager::get_tile_kind(id)!=agl_kind) return;  // Everything is done only with AGL
    if(betaHisto[id].GetNbinsX()<betaBins) betaHisto[id].SetBins(betaBins,0.95,1.05);
    betaHisto[id].Fill(ring->_beta);
    
    double value=computePeakPos(betaHisto[id]);
    if(value>0){
      if(betaCalibration[id]<0) betaCalibration[id]=1.0/value;
      else betaCalibration[id]=betaCalibration[id]*(1-betaLearningFactor)+betaLearningFactor/value;
      betaHisto[id].Reset();
    }
  }

  /********************* END OF INDEX CALIBRATION ***************/
  
  /********************* GAIN CALIBRATION         ***************/
  if(CHECK(gainMomentum)){
    int which=0;
    for(AMSRichRing* r=(AMSRichRing *)AMSEvent::gethead()->
	  getheadC("AMSRichRing",0);r;r=r->next(),which++) if(r==ring) break;
    
    for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
      if(!(hit->getbit(which))) continue;
      double npe=hit->getnpe();
      if(npe==0) continue;
      int channel=hit->getchannel();
      
      // Push the value
      signal[channel].push_back(npe);
      
      // Check if we should update the calibration
      if(signal[channel].size()>=gainEvents){
	// Find the median (which is more robust to outliers than mean)
	double value=computeMedian(signal[channel]);
	if(value>0){
	  if(signalCalibration[channel]<0) signalCalibration[channel]=1.0/value;
	  else signalCalibration[channel]=signalCalibration[channel]*(1.0-gainLearningFactor)+gainLearningFactor/value;
	  signal[channel].clear();
	}
      }
    }
  }
}



void AMSRichCal::finish(){
#pragma omp single
  {

    // For the refractivr index, we assume that, in first approximation, 
    // the obtenied values are OK.
    for(int i=0;i<RichRadiatorTileManager::get_number_of_tiles();i++){
      if(RichRadiatorTileManager::get_tile_kind(i)!=agl_kind) continue;  // Skip NaF 
#ifdef __AMSDEBUG__
      cout<<"UPDATING TILE "<<i<<" PREV INDEX "<<RichRadiatorTileManager::_optical_parameters[i*4]<<" MEAN BETA "<<1/betaCalibration[i]<<" CORRECTED INDEX "<<RichRadiatorTileManager::_optical_parameters[i*4]/betaCalibration[i]<<" entries "<<betaHisto[i].GetEntries()<<endl;
#endif
      
      RichRadiatorTileManager::_optical_parameters[i*4]/=betaCalibration[i];
    }
    
    //
    // For the gain, we have to obtain a correction of the mean given the median.
    // For this we assume that the response for all the channels is approximatly equal
    //
    
    // Compute the median
    double sum=0;
    double median=0;
    const double dx=0.01;
    float rl,rs; GETRLRS(1.0,0.6,rl,rs); 
    for(float x=0;x<2;x+=dx){
      double prob=PDENS(x,rl,rs)*dx;
      if(sum<0.5 && sum+prob>=0.5){
	median=x;
	break;
      }
      sum+=prob;
    }
    
#ifdef __AMSDEBUG__
    cout<<"**************** MEDIAN AT "<<median<<endl;
#endif
    
    // Start correcting everything
    for(int i=0;i<RICmaxpmts*RICnwindows;i++){
      double mean=signalCalibration[i]*median;
      
      // Get the corresponding entry in the gains table
      int pmt,channel;
      RichPMTsManager::UnpackGeom(i,pmt,channel);
#ifdef __AMSDEBUG__
      cout<<"CORRECTING CHANNEL "<<i<<" GAIN "<<RichPMTsManager::_Gain(pmt,channel,1)<<" MEAN NPE FOR SINGLE P:E: "<<1/signalCalibration[i]/median<<" NEW GAIN "<< RichPMTsManager::_Gain(pmt,channel,1)/signalCalibration[i]/median<<" entries "<<signal[i].size()<<endl;  
#endif
      
      RichPMTsManager::_Gain(pmt,channel,1)/=mean; // Update high gain   
      RichPMTsManager::_Gain(pmt,channel,0)/=mean; // Update high gain   
    }
    
    // DUMP TABLES
#ifndef __AMSDEBUG__
    AMSTimeID *ptdv;
    time_t begin,end,insert;
    const int ntdv=2;
    const char* TDV2Update[ntdv]={"RichPMTChannelGain",
				  "RichRadTilesParameters"};
    
    for (int i=0;i<ntdv;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      
      if(CALIB.InsertTimeProc)insert=last_run;
      
#define max(x,y) ((x)>(y)?(x):(y))
      ptdv->SetTime(insert,max(last_run-1,last_time),max(last_run-1,last_time)+864000);
#undef min
      cout <<" RICH info has been updated for "<<*ptdv<<endl;;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert)<<endl;
      cout <<" Time Begin "<<ctime(&begin)<<endl;
      cout <<" Time End "<<ctime(&end)<<endl;
    }
  
    
    
#ifdef __AMSDEBUG__
    // Dump the calibration tables
    cout<<"AMSRichCal::Finish -- Dumping stupid tables "<<endl;
    
    cout<<"TILES CALIBRATION "<<endl;
    for(int i=0;i<110;i++){
      cout<<"TILE "<<i<<" Correction Factor "<<betaCalibration[i]<<" CURRENT "<<betaHisto[i].GetEntries()<<" CURRENTPOS "<<computePeakPos(betaHisto[i])<<endl;
    }
    
    cout<<"CHANNEL CALIBRATION "<<endl;
    for(int i=0;i<RICmaxpmts*RICnwindows;i++){
      cout<<"CHANNEL "<<i<<" Correction Factor "<<signalCalibration[i]<<" CURRENT "<<signal[i].size()<<" CURRENTMEDIAN "<<computeMedian(signal[i])<<endl;
    }
#endif
#endif
  }
}

