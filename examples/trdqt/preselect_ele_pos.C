//Preselection routine for electron/positron analysis
#ifndef SELECT_C
#define SELECT_C

#include "preselect_ele_pos.h"

int particle_type=3;//electron

float LONGCUT[4][10]={18.5,15.9,14.1,13.9,14.6,13.3,14.3,17.2,0.,0.,
22.,12.,12.2,15.9,12.2,17.6,11.4,20.9,0.,0.,
6.8,10.4,10.5,12.,10.6,10.7,11.1,10.1,10.9,14.6,
16.6,13.4,11.6,11.2,12.6,10.3,12.6,17.8,0.,0.}; // cm (~100% efficiency cut on tof-track longitudinal coord.)
float TRANCUT[4][10]={11.5,6.6,6.4,6.8,6.7,6.6,6.7,11.3,0.,0.,
13.6,7.1,6.5,6.7,6.6,6.7,6.6,13.8,0.,0.,
10.8,6.4,6.7,6.4,6.9,6.6,6.9,6.3,6.9,10.8,
14.8,6.6,6.4,6.4,6.3,6.5,6.5,13.7,0.,0.}; // cm (~100% efficiency cut on toftrack transverse coord.)

//****DAQ
HeaderR *header;
Level1R *trig;
DaqEventR *daq;

//****Particle
ParticleR *part;       //ParticleR
int part_index;        //ParticleR  (used) index in the event
int npart;

//****Tracker
TrTrackR *trk_track;  //TrTrack
int trk_index;        //TrTrack (used) index in the event
int id_maxspan;
int id_inner;
double trk_chi2;
const int algo = 1;
const int refit = 3;

//****Beta,Tof
BetaHR *beta;          //Beta
int beta_index;       //Positive Beta (used) index in the event
TofClusterR *tof_cl;  // TOF cluster
TofClusterHR *tof_clH; // TOF cluster (IHEP version)
TofTrack *tof_track;

//****TRD
TrdTrackR *trd_track;  //TrdTrack
int trd_index;         //TrdTrack (used) index in the event
TrdSegmentR *trd_seg;
TrdClusterR *trd_clu;
TrdRawHitR *trd_hit;

//****ECAL
EcalShowerR *ecal_shr; //EcalShower
int ecal_index;        //EcalShower (used) index in the event

void SelectParticle(int kParticle){

  particle_type=kParticle;

  return;
}

void InitObjects(){

  header     = NULL;
  trig       = NULL;
  daq        = NULL;

  part       = NULL;
  part_index = -1;
  npart = 0;

  trk_track  = NULL;
  trk_index  = -1;
  id_maxspan = -1;
  id_inner   = -1;
  trk_chi2   = -1;

  beta       = NULL;
  beta_index = -1;
  tof_cl     = NULL;
  tof_clH    = NULL;
  tof_track  = NULL;

  trd_track  = NULL;
  trd_index  = -1;
  trd_seg    = NULL;
  trd_clu    = NULL;
  trd_hit    = NULL;  
 
  ecal_shr   = NULL;
  ecal_index = -1;
}

bool BadRunCheck(AMSEventR *pev){

  if( !pev) return true;

  int tag = pev->IsBadRun("");
  
  if(tag==0) return false;        //good run
  else if(tag==1) return true;    //bad run
  else if(tag==2) {
    printf("Run:%d Ev:%d -> Cannot acces to root_setup. Skipping this event\n", (int)pev->Run(), (int)pev->Event());
    return true;
  }
  return true;    //default
}

bool IsScienceRun(AMSEventR *pev){

  if( !pev) return false;

  // Science runtag
  header = &(pev->fHeader);
  if (!header) return false;
  if (header->RunType<0xF000) return false; //faster than: if ((header->RunType >> 12) != 0xf) return false;
  return true;
}

bool HasGoodLiveTime(Level1R *trig){

  if (!trig) return false;

  // Live Time > 0.65
  if (trig->LiveTime <= 0.65) return false;
  return true;
}

bool GoodHW(DaqEventR *daq){
  /*
  Authors A.Oliva -  M.Duranti
  see http://ams.cern.ch/AMS/DAQsoft/daqsoft_note.pdf

  Reply Status
       bit     15 - DATA bit, set by master when assembling group reply;
       bits 14-11 - reply code (see Section 6.4), set by master;
       bit     10 - build conditions error (Sub-Detector specific);
       bit      9 - build errors (sequencer errors (CDP) or event assembly (CDDC));
       bit      8 - cumulative node status (OR of bits 11-8 in the Node Status Word);
       bit      7 - COMPRESSED event building mode;
       bit      6 - RAW event building mode;
       bit      5 - internal structure bit (0 means group assembly, 1 – no sub-structure);
       bits   4-0 - slave ID, set by master;

  Node Status Word
       bit     11 - Buffer Memory Test errors detected at Node Init;
       bit     10 - Program Memory test errors detected at Node Init;
       bit      9 - Data Memory test errors detected at Node Init;
       bit      8 - Program self-test errors;
  */

  if(!daq) return false;

  //  unsigned int bitstocheck = 0x7F;// Reply Status Word from 8 to 15. This leaves also the "cumulative node address" as bit to be checked. This contains DSP errors from the checked nodes (JINJ, JINF, JLV1 and SDR) even if we decided not to check the DSP errors
  unsigned int bitstocheck = 0x7E;// Reply Status Word from 9 to 15.

  bool errors=false;
  for (int ii=0; ii<4; ii++ ) errors |= (bool)((daq->JINJStatus[ii]>>8) & bitstocheck);
  for (int ii=0; ii<24; ii++) {
    if (ii!=10 && ii!=11) {
      errors |= (bool)(daq->JError[ii] & bitstocheck);
    }
  }

  return !errors;
}

bool GoodHW_deeper(DaqEventR *daq){
/*
  Authors A.Oliva -  M.Duranti
  see DaqEventR class, root.h-root.C

  bool L3ProcError(){return (L3Error & (1ULL<<63))?true:false;}
  bool L3RunError(){return (L3Error & (1ULL<<62))?true:false;}
  bool L3EventError(){return (L3Error & (1ULL<<61))?true:false;}
  unsigned int L3NodeError(int i); ///< Return number of nodes with errors for (ETRG<LV1,SDR,EDR,RDR,UDR,TDR 0...6
  unsigned int DaqEventR::L3NodeError(int i){
  int j=i*8;
  return (unsigned int)((L3Error>>j)&255);
  }

  unsigned long long  L3Error;  ///<Lvl3 event error

  bit 63  proc error --> if during the "unpacking" of the event there were some error/inconsistency
  bit 62  desync error --> desync at any level (even between JINJ and JMDC)
  bit 61  event error --> consistency of the reply of the node (deeper that just looking the Reply Status Word)
  byte 6  number of TDR  with fatal errors
  byte 5  number of UDR  with fatal errors
  byte 4  number of RDR  with fatal errors
  byte 3  number of EDR  with fatal errors
  byte 2  number of SDR  with fatal errors
  byte 1  number of LV1  with fatal errors
  byte 0  number of ETRG  with fatal errors
*/

  if(!daq) return false;

  // Whatever bad REPLY at JINJ, JINF or LVL1 (ROOM, Sync, 0-replies, ...) 
  bool assembly_error = daq->L3EventError();  

  // Desync error at whatever level 
  bool sync_error = daq->L3RunError();

  // Decoding procedure error
  bool proc_error = daq->L3ProcError();

  // Whatever board with bad REPLY
  bool node_error = false;
  for (int inode=0; inode< 7; inode++) {
    if (inode!=4) { //we don't want to check RICH for electron/positrons!!!
      if (daq->L3NodeError(inode)>0) { //whit this code there is still the possibility that we're checking the DSP error (and of course in the wrong way) for these nodes...
	node_error = true;
	//	cdp_errors[inode] += daq->L3NodeError(inode);
      }
    }
  }

  bool errors=false;
  errors|=assembly_error;
  errors|=sync_error;
  errors|=proc_error;
  errors|=node_error;

  return !errors;
}

bool GoodAccPattern(Level1R* trig){

  if (!trig) return false;  

  int nacc=0;
  for( int i=0; i<8; i++){
    if(((trig->AntiPatt>>i)&1)==1) nacc++;
  }

  if(nacc>5) return false;
  return true;
}

bool GoodBeta(BetaHR *this_beta){

  if( !this_beta) return false;

  float beta = this_beta->GetBeta();
  if( (1./beta<0.8) || (1./beta>1.2) ) return false;
  return true;
}

bool GoodBetaProtons(BetaHR *this_beta){

  if( !this_beta) return false;

  float beta = this_beta->GetBeta();
  if( (1./beta<0.8)) return false;
  return true;
}

bool GoodTrkTrack(TrTrackR *this_track){

  if( !this_track) return false;

  if( this_track->IsFake() ){
    cout<<"Found a Fake Track. This should not happen"<<endl;
    return false;
  }

  id_maxspan = part->pTrTrack()->iTrTrackPar(algo,0,refit);
  if( id_maxspan<0 ){
      cout<<"MaxSpan refit not available. This should not happen"<<endl;
      return false;
  }
  id_inner = part->pTrTrack()->iTrTrackPar(algo,3,refit);

  trk_chi2 = part->pTrTrack()->GetNormChisqY( id_maxspan );  
  bool goodtrack = true;
  if( trk_chi2<0 || trk_chi2>15 ) goodtrack = false;
  if( !goodtrack ) return false;
  return true;

}

bool InEcalAcceptance(TrTrackR* this_track){
  //check if TrTrack is inside ECAL acceptance

  if( !this_track) return false;

  int fit_id = id_maxspan;
  AMSPoint entry_point, exit_point;
  AMSDir   entry_dir,   exit_dir;
  Float_t z_entry = -142.792;
  Float_t z_exit  = -158.457;

  this_track->Interpolate(z_entry, entry_point, entry_dir, fit_id  );
  this_track->Interpolate(z_exit,  exit_point,  exit_dir,  fit_id  );

  bool Entry_in_32_4 = (TMath::Abs( entry_point.x()) <32.4)  && (TMath::Abs( entry_point.y() )<32.4);  
  bool Exit_in_32_4  = (TMath::Abs( exit_point.x() ) <32.4)  && (TMath::Abs( exit_point.y() ) <32.4);
  bool Entry_in_31_5 = (TMath::Abs( entry_point.x()) <31.5)  && (TMath::Abs( entry_point.y() )<31.5);  
  bool Exit_in_31_5  = (TMath::Abs( exit_point.x() ) <31.5)  && (TMath::Abs( exit_point.y() ) <31.5);

  //Request: Shower axis in ECAL volume (Entry&Exit<32.4), at least Entry||Exit within 1 cell (0,5 Moliere radius) from the border
  bool inacc = (Exit_in_32_4 && Entry_in_32_4) && ( Exit_in_31_5 || Entry_in_31_5 );
  if( inacc ) return true;
  return false;
}

bool GoodTrdTrack(TrdTrackR *this_trd) {//not used!

  return true;

}

bool InTrdAcceptance(TrTrackR* this_trk){
  //check if particle passing inside acceptance of the 20 layers TRD                                                                                                                            
  //only top layer and bottom layer checked
  //this is a gross check. A more fine tuned check is going to be provided by Aachen group

  int nTrdBottom = 9;
  float AccepBottomX[] = {+40, +78, +78, +40, -40, -78, -78, -40, +40};
  float AccepBottomY[] = {+76, +35, -35, -76, -76, -35, +35, +76, +76};
  float TrdBottomZ = 86.725;

  int nTrdCenter = 9;
  float AccepCenterX[] = {-80.0, -47.0, 47.0, 80.0,  80.0,  47.0, -47.0, -80.0, -80.0};
  float AccepCenterY[] = { 43.5,  75.5, 75.5, 43.5, -43.5, -75.5, -75.5, -43.5,  43.5};
  float TrdCenterZ = 0.5 * (141.825 +  86.725);//                                                                                                                                                                 

  int nTrdTop = 37;
  float AccepTopX[] = {-99.0,-89.0,-89.0,-78.7,-78.7,-67.8,-67.8,-57.7,-57.7, 57.7, 57.7, 67.8, 67.8, 78.7, 78.7, 89.0, 89.0, 99.0,
		       99.0, 89.0, 89.0, 78.7, 78.7, 67.8, 67.8, 57.7, 57.7,-57.7,-57.7,-67.8,-67.8,-78.7,-78.7,-89.0,-89.0,-99.0,-99.0};
  float AccepTopY[] = { 54.5, 54.5, 62.5, 62.5, 74.0, 74.0, 84.0, 84.0, 95.3, 95.3, 84.0, 84.0, 74.0, 74.0, 62.5, 62.5, 54.5, 54.5,
			-51.7,-51.7,-62.2,-62.2,-72.0,-72.0,-82.5,-82.5,-92.5,-92.5,-82.5,-82.5,-72.0,-72.0,-62.2,-62.2,-51.7,-51.7, 54.5};
  float TrdTopZ = 141.825; //                                                                                                                                                                                     

  AMSPoint pTop, pCenter, pBottom;
  AMSDir   dTop, dCenter, dBottom;

  this_trk->Interpolate( TrdBottomZ, pBottom, dBottom, id_maxspan);
  this_trk->Interpolate( TrdCenterZ, pCenter, dCenter, id_maxspan);
  this_trk->Interpolate( TrdTopZ,    pTop,    dTop,    id_maxspan);

  bool passTrdBottom = TMath::IsInside( (float)pBottom.x(), (float)pBottom.y(), nTrdBottom, AccepBottomX, AccepBottomY);
  bool passTrdCenter = TMath::IsInside( (float)pCenter.x(), (float)pCenter.y(), nTrdCenter, AccepCenterX, AccepCenterY);
  bool passTrdTop    = TMath::IsInside( (float)pTop.x(),    (float)pTop.y(),    nTrdTop,    AccepTopX,    AccepTopY);

  return (passTrdTop && passTrdBottom);
}

bool EcalTrkMatch(TrTrackR* this_track, EcalShowerR* this_ecal){

  if(!this_ecal || !this_track) return false;

  AMSPoint point;
  AMSDir dir;
  this_track->Interpolate(this_ecal->CofG[2], point, dir, id_maxspan);
  Double_t DeltaX = TMath::Abs( point[0] - this_ecal->CofG[0] );
  Double_t DeltaY = TMath::Abs( point[1] - this_ecal->CofG[1] );
  
  bool ecal_trk_match = (DeltaX<3.6) && (DeltaY<7.2);
 
  if(ecal_trk_match) return true;
  return false;
}

bool TrdTrkMatch(TrTrackR* this_track, TrdTrackR* this_trd){

  return true;
}

bool TofTrkMatch(AMSEventR* pev, TrTrackR* this_track, int this_fit_id){

  if (!pev || !this_track) return false;
  if( this_fit_id<0 ) return false;
  
  double tlen;
  AMSPoint tofpnt;
  AMSDir tofdir;
  int longit[4]={0,1,1,0};
  int tranit[4]={1,0,0,1};
  double dlong,dtran;
  bool goodstatus=false;
  bool goodmatch=false;
  bool goodlayer[4]={false,false,false,false};
  // match with cluster
  int ilay=0;
  bool good_c = false;
  if(pev->NTofCluster()>0){
    for (int icl=0; icl<pev->NTofCluster(); icl++) {
      tof_cl = pev->pTofCluster(icl);
      if (tof_cl>0) { // cluster exists
	good_c=true;
	int layer=tof_cl->Layer-1;
	int bar=tof_cl->Bar-1;
	// check cluster status
	for (int i = 7; i < 13; ++i) if (((tof_cl->Status >> i) & 1) == 1) good_c = false;
	if (((tof_cl->Status >> 2) & 1) == 1) good_c = false;
	if (((tof_cl->Status >> 4) & 1) == 1) good_c = false;
	if (good_c) goodstatus = true;
	// check track match with TOF clusters
	tlen=this_track->Interpolate(tof_cl->Coo[2],tofpnt,tofdir,this_fit_id);
	dlong=tof_cl->Coo[longit[layer]]-tofpnt[longit[layer]];
	dtran=tof_cl->Coo[tranit[layer]]-tofpnt[tranit[layer]];
	if(fabs(dlong)<LONGCUT[layer][bar] &&
	   fabs(dtran)<TRANCUT[layer][bar]) goodmatch=true;
	if (goodstatus && goodmatch) goodlayer[layer]=true;
      }
    }
    for (int i=0; i<4; i++) if(goodlayer[i]) ilay++;
  }
  return (ilay==3 || ilay==4);
}

bool TofTrkMatchH(AMSEventR* pev, TrTrackR *this_track, int this_fit_id){

  if (!pev || !this_track) return false;
  if( this_fit_id<0 ) return false;

  double tlen;
  AMSPoint tofpnt;
  AMSDir tofdir;
  int longit[4]={0,1,1,0};
  int tranit[4]={1,0,0,1};
  double dlong,dtran;
  bool goodstatus=false;
  bool goodmatch=false;
  bool goodlayer[4]={false,false,false,false};
  // match with cluster
  int ilay=0;
  bool good_c = false;
  if(pev->nTofClusterH()>0){
    for (int icl=0; icl<pev->nTofClusterH(); icl++) {
      tof_clH = pev->pTofClusterH(icl);
      if (tof_clH>0) { // cluster exists
	good_c=true;
	int layer=tof_clH->Layer;
	int bar=tof_clH->Bar;
	// check cluster status
	for (int i = 6; i < 10; ++i) if (((tof_clH->Status >> i) & 1) == 1) good_c = false;
	if (((tof_clH->Status >> 4) & 1) == 1) good_c = false;
	if (good_c) goodstatus = true;
	// check track match with TOF clusters
	tlen=this_track->Interpolate(tof_clH->Coo[2],tofpnt,tofdir,this_fit_id);
	dlong=tof_clH->Coo[longit[layer]]-tofpnt[longit[layer]];
	dtran=tof_clH->Coo[tranit[layer]]-tofpnt[tranit[layer]];
	if(fabs(dlong)<LONGCUT[layer][bar] &&
	   fabs(dtran)<TRANCUT[layer][bar]) goodmatch=true;
	if (goodstatus && goodmatch) goodlayer[layer]=true;
      }
    }
    for (int i=0; i<4; i++) if(goodlayer[i]) ilay++;
  }
  return (ilay==3 || ilay==4);  
}

bool IsInSolarArrays(AMSEventR* pev, int ipart){

  if (!pev) return true;

  AMSPoint pointSolarArray;
  int is_in_shadow = pev->isInShadow(pointSolarArray, ipart);
  return ( !(is_in_shadow==0 || is_in_shadow==-2) );         //accept events with   0 -> Not In Shadow
                                                             //                    -2 -> beta<0 (from BetaR) this can go in conflict when BetaR and BetaHR (used) have different sign 
}

bool EcalEnergyGreaterThan2(EcalShowerR* shower){

  if (!shower) return true;

  float energy=shower->EnergyE;

  if (energy<=2.0) return false;
  return true;
}

bool Clean_Event(AMSEventR* pev){
  
  InitObjects();

  //***************
  //Preliminary Global Checks
  //***************
    
  if(!pev) return false;
  
  trig = pev->pLevel1(0);
  if (!trig) return false;
  
  daq = pev->pDaqEvent(0);
  if(!daq) return false;

  header = &(pev->fHeader); 
  if(!header) return false;
  
  bool gbatcherr = pev->Status(30);
  if( gbatcherr )                         return false;  //skip events with a problem during reconstruction

  if( BadRunCheck(pev) )                  return false;  //remove ALL the bad runs

  if( !IsScienceRun(pev) )                return false;  //here we defined the pointer to HeaderR

  if( !GoodHW(daq) )                      return false;  //here we defined the pointer to DaqEventR
  
  if( !HasGoodLiveTime(trig) )             return false;  //here we defined the pointer to Level1R

  
  //***************
  //Preselection based on ParticleR
  //***************
  
  int nGoodPart=0;
  int goodpart_index=-1;

  if(!pev->nBetaH()) TofRecH::ReBuild();   //reconstruct BetaHR object. This will also ReInit ParticleR and ChargeR AMSEventR

  for(int ipart=0; ipart<pev->nParticle(); ipart++) {
    //  for(int ipart=0; ipart<pev->NParticle(); ipart++) {//never use ev->NSomething. This read the branche even if not needed
      
    part = pev->pParticle(ipart);
    if( !part ) continue;
    
    //Particle must have Beta, Ecal, Tracker and Trd

    beta = part->pBetaH();                              //here we defined the pointer to BetaHR
    if( !beta )      continue;
    beta_index = part->iBetaH();
    
    trk_track = part->pTrTrack();                      //here we defined the pointer to TrTrackR
    if( !trk_track ) continue;
    trk_index = part->iTrTrack();  
    
    ecal_shr = part->pEcalShower();                    //here we defined the pointer to EcalShowerR
    if( !ecal_shr )  continue;
    ecal_index = part->iEcalShower();
    
    trd_track = part->pTrdTrack();                     //here we defined the pointer to TrdTrackR
    //if( !trd_track ) continue;                       //Trd analys independent from TrdTrackR clustering, but based only on the extrapolation of TrTrack into TRD volume
    trd_index = part->iTrdTrack();
    
    //Check quality/acceptances of subdetectors
    if (particle_type==14) {//protons
      if(!GoodBetaProtons(beta)) continue;
    }
    else if (!GoodBeta(beta))             continue;
    
    if( IsInSolarArrays( pev, ipart ) )   continue;
    
    if( !GoodTrkTrack( trk_track ) )      continue;
    if( !InEcalAcceptance( trk_track ) )  continue;
    
    if( !InTrdAcceptance( trk_track ) )   continue;
    
    //Matching between objects
    if( !EcalTrkMatch( trk_track, ecal_shr ) ) continue;  
    if( !TofTrkMatchH( pev, trk_track, id_inner  ) ) continue;
    
    //OK, we like this particle.
    nGoodPart++;
    goodpart_index = ipart;
  }
  
  //**************************
  //how many particles do we have?
  //**************************
  
  if( nGoodPart==0 ) return false;  //No good particle found. Skip this event
  if( nGoodPart>1 )  return false;  //Tricky situation. We found two particles having Ecal,Trd,Track and Beta that fullfill quality checks. This is mostly 2 primaries. Skip this

  part_index = goodpart_index;
  part = pev->pParticle( goodpart_index );
  beta = part->pBetaH(); 
  trk_track = part->pTrTrack();
  id_inner = trk_track->iTrTrackPar(algo,3,refit);
  id_maxspan = trk_track->iTrTrackPar(algo,0,refit);
  ecal_shr = part->pEcalShower(); 
  //tof_track = new TofTrack( beta, trk_track, id_maxspan);    //here we defined the pointer to TofTrack
  trd_track = part->pTrdTrack();  
  
  
  return true;   //Only one particle found. In this case, take any shower has been matched with the particle
}

int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan, int& Id_Inner, TofTrack*& Tof_Track){
  //return the AMSEventR index to the choosen particle (-1 if the event is not "clean")
  //the various quantities used in Clean_Event can be retrieved passing the parameters to be filled

  if (!Clean_Event(pev)) return -1;

  Id_MaxSpan = id_maxspan;
  Id_Inner = id_inner;

  Tof_Track = tof_track;

 return part_index;
}

int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan, int& Id_Inner){
  
  TofTrack* Tof_Track = NULL;

  return Clean_Event_ChoosenParticle(pev, Id_MaxSpan, Id_Inner, Tof_Track);
}

int Clean_Event_ChoosenParticle(AMSEventR* pev, int& Id_MaxSpan){
  
  TofTrack* Tof_Track = NULL;
  int Id_Inner = -99;

  return Clean_Event_ChoosenParticle(pev, Id_MaxSpan, Id_Inner, Tof_Track);
}

int Clean_Event_ChoosenParticle(AMSEventR* pev){
  
  TofTrack* Tof_Track = NULL;
  int Id_Inner = -99;
  int Id_MaxSpan = -99;

  return Clean_Event_ChoosenParticle(pev, Id_MaxSpan, Id_Inner, Tof_Track);
}

#endif
