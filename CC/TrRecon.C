/// $Id: TrRecon.C,v 1.1 2008/12/18 11:19:32 pzuccon Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrRecon.C
///\brief Source file of TrRecon class
///
///\date  2008/02/16 AO  First version
///\date  2008/02/21 AO  BuildTrClusters(), BuildTrRecHits()
///\date  2008/02/24 AO  BuildPatterns(), some bug fixed
///\date  2008/02/28 AO  Bug fixing
///\date  2008/03/11 AO  Some change in clustering methods 
///\date  2008/06/19 AO  Updating TrCluster building 
///
/// $Date: 2008/12/18 11:19:32 $
///
/// $Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecon.h"
#include "TrMap.h"
#include "tkdcards.h"
#include "TrTrack.h"
#include "TrPatt.h"
#include "TkSens.h"
#include "TrMCCluster.h"
#include "MagField.h"

VCon* TrRecon::TRCon=0;


TrRecon* TrRecon::Head=0;
TrCalDB* TrRecon::_trcaldb=0;

TrRecon* TrRecon::Create(int recreate){
  if (Head && !recreate){
    printf("TrRecon::Create Warning a Trecon instance already exist!\n");
    return Head;
  }
  if( (Head && recreate)||(!Head))
    Head= new TrRecon();
  for (int i = 0; i < BUFSIZE; i++) {
    _adcbuf2[i] =  0;
    _sigbuf2[i] =  -1;
    _stabuf2[i] =  -1;
  } 
  TrPatt::Init();
  return Head;
}

// clustering - parameters
float       TrRecon::ThrSeed[2]  = {4.00,4.00};
float       TrRecon::ThrNeig[2]  = {1.00,1.00};
int         TrRecon::SeedDist[2] = {   3,   3};

// clustering - analysis structure
float       TrRecon::_adcbuf[TrRecon::BUFSIZE];
float       TrRecon::_sigbuf[TrRecon::BUFSIZE];
int         TrRecon::_stabuf[TrRecon::BUFSIZE];

float       TrRecon::_adcbuf2[TrRecon::BUFSIZE];
float       TrRecon::_sigbuf2[TrRecon::BUFSIZE];
int         TrRecon::_stabuf2[TrRecon::BUFSIZE];
vector<int> TrRecon::_seedaddresses;

// hit signal correlation (only muons/protons)
float  TrRecon::GGpars[6]  = {1428.,0.0000,0.1444,1645.,0.0109,0.0972};
float  TrRecon::GGintegral = 91765.;
float  TrRecon::ThrProb    = 0.00001;


void TrRecon::SetParFromDataCards(){
  ThrSeed[0]  = TRCLFFKEY.ThrSeed[0] ;
  ThrSeed[1]  =	TRCLFFKEY.ThrSeed[1]  ;
  ThrNeig[0]  = TRCLFFKEY.ThrNeig[0]  ;
  ThrNeig[1]  = TRCLFFKEY.ThrNeig[1]  ;
  
  SeedDist[0] = TRCLFFKEY.SeedDist[0] ;
  SeedDist[1] = TRCLFFKEY.SeedDist[1] ;
  
  GGpars[0]  = TRCLFFKEY.GGpars[0]    ;
  GGpars[1]  = TRCLFFKEY.GGpars[1]    ;
  GGpars[2]  = TRCLFFKEY.GGpars[2]    ;
  GGpars[3]  = TRCLFFKEY.GGpars[3]    ;
  GGpars[4]  = TRCLFFKEY.GGpars[4]    ;
  GGpars[5]  = TRCLFFKEY.GGpars[5]    ;
  GGintegral = TRCLFFKEY.GGintegral   ;
  ThrProb    = TRCLFFKEY.ThrProb      ;
  

}
TrRecon::~TrRecon() {
  Clear();
}

void TrRecon::Clear(Option_t *option) {
  /*
  for(int ii=0;ii<2;ii++){
    ThrSeed[ii]=0;
    ThrNeig[ii]=0;
    SeedDist[ii]=0;
  }
  //
  memset(GGpars,0,6*sizeof(GGpars[0]));
  GGintegral=0;
  ThrProb=0;
  */
  _ClusterTkIdMap.clear();
  _RecHitLayerMap.clear();
  // _Patterns.clear();
}

// void TrRecon::PrintEvent() {
//   // Summary
//   printf("Summary:\n");
//   printf("nRawClusters: %4d\n",GetEvent()->GetnTrRawClusters());
//   printf("nTrClusters:  %4d\n",GetEvent()->GetnTrClusters());
//   printf("nTrRecHits:   %4d\n",GetEvent()->GetnTrRecHits());
//   printf("nTrTracks:    %4d\n",GetEvent()->GetnTrTracks());
//   printf("\n");
//   // Summary by ladder
//   BuildClusterTkIdMap();
//   printf("List of the %3d active ladders: \n",GetnActiveLadders());
//   for (int tt=0; tt<GetnActiveLadders(); tt++) {
//     int tkid = GetTkIdActiveLadder(tt);
//     printf("TkId: %4d  nXCluster: %3d  nYCluster: %3d\n",
//            tkid,GetnTrClusters(tkid,0),GetnTrClusters(tkid,1));
//   }
//   printf("\n");
//   // Summary by layer
//   BuildRecHitLayerMap();
//   printf("Numbers of hits per layer: \n");
//   for (int ll=1; ll<9; ll++) {
//     printf("Layer: %1d  nHits: %3d\n",ll,GetnTrRecHits(ll));
//   }
//   printf("\n");
//   // Hit list
//   printf("Hit list (without multiplicity on x coordinate): \n");
//   for (int hh=0; hh<GetEvent()->GetnTrRecHits(); hh++) {
//     AMSTrRecHit* hit   = GetEvent()->GetTrRecHit(hh);
//     AMSPoint  point = hit->GetGlobalCoordinate();
//     printf("Hit: %3d  (x,y,z)=(%10.4f,%10.4f,%10.4f)  mult: %1d  corr: %8.4f  prob: %7.5f  stat: %2d\n",
//            hh,point.x(),point.y(),point.z(),hit->GetMultiplicity(),hit->GetCorrelation(),hit->GetProb(),hit->GetStatus());
//   }
//   printf("\n");
//  // Track list
//   printf("Track list: \n");
//   for (int tt=0; tt<GetEvent()->GetnTrTracks(); tt++) {
//     TrTrack* track  = GetEvent()->GetTrTrack(tt);
//     printf("nTrack: %2d  (x0,y0,z0)=(%8.4f,%8.4f,%8.4f)  (Theta,Phi)=(%9.6f,%9.6f)  nPoints: %1d  Chi2: %10.4f\n",
//            tt,track->GetP0x(),track->GetP0y(),track->GetP0z(),
//            track->GetTheta(),track->GetPhi(),track->GetNhits(),track->GetChisq());
//   }
//   printf("\n");
// }



//////////////////////
// --- CLUSTERS --- //
//////////////////////

void TrRecon::ClearBuffer() {
 


  memcpy(_adcbuf, _adcbuf2,BUFSIZE*sizeof(float));
  memcpy(_sigbuf, _sigbuf2,BUFSIZE*sizeof(float));
  memcpy(_stabuf, _stabuf2,BUFSIZE*sizeof(int));

  _seedaddresses.clear();
}

int TrRecon::ExpandClusterInBuffer(AMSTrRawCluster* cluster) {
  int entries = 0;
  int nelements = cluster->GetNelem();
  int firststripaddress = cluster->GetAddress();
  cal = GetTrCalDB()->FindCal_TkId(cluster->GetTkId());
  for (int jj=0; jj<nelements; jj++) {
    int address = firststripaddress + jj;
    if ( (address<0)||(address>=1024) ) continue;
    _adcbuf[address] = cluster->GetSignal(jj);
    _sigbuf[address] = cal->GetSigma(address);
    _stabuf[address] = cal->GetStatus(address);
    entries++;
  }
  return entries;
}


int TrRecon::GetAddressInSubBuffer(int address, int first, int last, int ciclicity) {
  // by default returns the address
  if ( (address<last)&&(address>=first) ) return address;
  // if not cyclic returns the first strip in case of underflow and the last-1 in case of overflow  
  if (ciclicity==0) return TMath::Max(first,TMath::Min(address,last-1));
  // if cyclic adds the interval (last-first)
  return (address>=last) ? (address-(last-first)) : (address+(last-first));
}


int TrRecon::BuildTrClusters(int rebuild) { 
  //Get the pointer to the TrRawCluster container
  //  AMSContainer* cont=GetCont(AMSID("AMSTrRawCluster"));
  VCon* cont=TRCon->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrRawCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    printf("TrRecon::BuildTrClusters  AMSTrRawCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }


  VCon* cont2=TRCon->GetCont("AMSTrCluster");
  if(!cont2){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container if requested
  if(rebuild) cont2->eraseC();


  //Build The Map of TrRawClusters
  TrMap<AMSTrRawCluster> RawMap;


  for (int ii=0;ii< cont->getnelem();ii++){
    AMSTrRawCluster* clu=(AMSTrRawCluster*)cont->getelem(ii);
    if (clu)  RawMap.Add(clu);
  }



  //  RawMap.info();
  // loop on ladders
  TrMap<AMSTrRawCluster>::TrMapIT lad=RawMap.LadMap.begin();
  while (lad!=RawMap.LadMap.end()){
    ClearBuffer();
    //expand clusters in the buffer
    for (int iclus=0; iclus<lad->second.size(); iclus++) 
      ExpandClusterInBuffer( lad->second.at(iclus) );
    
    int TkId=lad->first;
    int layer = abs(TkId/100);
    int ncls=BuildTrClustersInSubBuffer(TkId,0,640,0);      // S
    //printf("TkId: %+03d Found %d cluster on S\n",TkId,ncls);
    //printf("TrCluster container has: %d elements\n",cont2->getnelem());
    if ( (layer==1)||(layer==8) ) { 
      BuildTrClustersInSubBuffer(TkId,640,1024,1); // K7
    }
    else { 
      BuildTrClustersInSubBuffer(TkId,640,832,0);  // K5 - sensor1
      BuildTrClustersInSubBuffer(TkId,832,1024,0); // K5 - sensor2 
    }
    lad++;
  }

  if(cont) delete cont;
  if(cont2) delete cont2;
  return 0;
}


int TrRecon::BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity) {
  int   ntrclusters = 0;
  int   side = (first>639)?0:1;
  if (_trcaldb==0) {
    printf("TrRecon::BuildTrClustersInSubBuffer Error, no _trcaldb specified.\n");
    return -9999; 
  }
  cal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!cal) {printf ("TrRecon::BuildTrClustersInSubBuffer, WARNING calibration not found!!\n"); return -9999;}
  // create a list of seeds 
  int nseeds = BuildSeedListInSubBuffer(first, last, cyclicity);
//   printf("Found %d seeds\n",nseeds);
//   for (int jj=0;jj<nseeds;jj++) printf("seed %d  %d\n",jj,_seedaddresses.at(jj));

  VCon* cont=TRCon->GetCont("AMSTrCluster");

  // loop on seeds
  for (int ss=0; ss<nseeds; ss++) {
    int seedaddress  = _seedaddresses.at(ss);
    int boundary     = GetBoundariesInSubBuffer(ss,first,last,cyclicity);
    int leftaddress  = boundary/10000;             
    int rightaddress = boundary-leftaddress*10000; 
    int length       = (leftaddress<=rightaddress) ? rightaddress-leftaddress+1 : last-leftaddress+rightaddress-first+1;
    int seedind      = (seedaddress>=leftaddress)  ? seedaddress-leftaddress    : last-leftaddress+seedaddress-first;  
    float sigg[1024];
    for (int jj=0; jj<length; jj++) { 
      int address = GetAddressInSubBuffer(leftaddress+jj,first,last,cyclicity);
      sigg[jj]=_adcbuf[address];
    }

    AMSTrCluster* cluster = new AMSTrCluster(tkid,side,leftaddress,length,seedind,sigg,0);
    cluster->BuildCoordinates();
    if(cont) cont->addnext(cluster);
    ntrclusters++;   
  }
  if(cont) delete cont;
  return ntrclusters;
}

int TrRecon::BuildSeedListInSubBuffer(int first, int last, int cyclicity) {
  _seedaddresses.clear();
  int side    = (first>639) ? 0 : 1;
  int address = 0;
  for (int ii=first; ii<last; ii++) {
    // 1. the strip has to be well defined
    // 2. a noisy strip can't be a seed strip
    // 3. the seed must exceed the ThrSeed S/N threshold
    if (  (_sigbuf[ii]<=0.)||(_stabuf[ii]!=0)||( (_adcbuf[ii]/_sigbuf[ii])<ThrSeed[side] )  ) continue;
    // 4. if the next strip is good, above threeshold and with signal larger than this one continue
    if( ii<(last-1)&&(_adcbuf[ii+1]>_adcbuf[ii]))
      if ((_sigbuf[ii+1]>0.)&&(_stabuf[ii+1]==0)&&( (_adcbuf[ii+1]/_sigbuf[ii+1])>=ThrSeed[side]))
	continue;

    // 5. the seed has to be the local maximum among 2*SeedDist+1 good (with status==0) strips
    bool localmax = true;
    for (int jj=1; jj<=SeedDist[side]; jj++) {
      // right
      address = GetAddressInSubBuffer(ii+jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
      // left
      address = GetAddressInSubBuffer(ii-jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
    }
    // is not a local maximum
    if (!localmax) continue;
    // A SEED IS CATCHED
    int seedaddress = ii;
    _seedaddresses.push_back(seedaddress);
  }
  return _seedaddresses.size();
}

int TrRecon::GetBoundariesInSubBuffer(int index, int first, int last, int cyclicity) {

  // the seeds vector
  int side         = (first>639)?0:1;
  int nseeds       = _seedaddresses.size();
  int seedaddress  = _seedaddresses.at(index);

  /// default boundary
  int leftaddress  = seedaddress;
  int rightaddress = seedaddress;

  /// the strip perimeter for the searching algorithm (from left to right)
  int left  = (index<=0) ? first : _seedaddresses.at(index-1);
  if ( (cyclicity!=0)&&(index<=0) ) left = _seedaddresses.at(nseeds-1);
  int right = (index>=(nseeds-1)) ? last-1 : _seedaddresses.at(index+1);
  if ((cyclicity!=0)&&(index>=(nseeds-1))) right = _seedaddresses.at(0);

  // search for the left boundary
  int address = GetAddressInSubBuffer(leftaddress-1,first,last,cyclicity); 
  while (address!=left) { 
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=ThrNeig[side]) break;
    // the neighboring strip signal has to be smaller than the seed one
    if (_adcbuf[address]>_adcbuf[seedaddress]) break;   
    // the strip is good
    leftaddress--;
    // new strip under analysis
    address = GetAddressInSubBuffer(leftaddress-1,first,last,cyclicity);
  } 
  leftaddress = GetAddressInSubBuffer(leftaddress,first,last,cyclicity);

  // search for the right boundary
  address = GetAddressInSubBuffer(rightaddress+1,first,last,cyclicity); 
  while (address!=right) { 
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=ThrNeig[side]) break;
    // the neighboring strip signal has to be smaller than the seed one
    if (_adcbuf[address]>_adcbuf[seedaddress]) break;   
    // the strip is good
    rightaddress++;
    // new strip under analysis
    address = GetAddressInSubBuffer(rightaddress+1,first,last,cyclicity);
  } 
  rightaddress = GetAddressInSubBuffer(rightaddress,first,last,cyclicity);

  return 10000*leftaddress + rightaddress; 
}


//////////////////
// --- HITS --- //
//////////////////

float TrRecon::GetCorrelation(AMSTrCluster* cln, AMSTrCluster* clp) { 
  if (!cln) return -1.;
  if (!clp) return  1.;
  float n = cln->GetTotSignal("AGLN");
  float p = clp->GetTotSignal("AGLN");
  return (p - n)/(p + n); 
}

float  TrRecon::GetProbToBeCorrelated(float n, float p) {
  float correlation = GetCorrelation(n,p);
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
	   GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}

float  TrRecon::GetProbToBeCorrelated(AMSTrCluster* cln, AMSTrCluster* clp) {
  float correlation = GetCorrelation(cln,clp);
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
	   GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}

void TrRecon::BuildClusterTkIdMap() {
  _ClusterTkIdMap.clear();
  // loop on TrClusters
  VCon* cont=TRCon->GetCont("AMSTrCluster");
  for (int ii=0;ii<cont->getnelem();ii++){
    AMSTrCluster* cluster = (AMSTrCluster*)cont->getelem(ii);
    int tkid = cluster->GetTkId();
    int side = cluster->GetSide();
    // add the cluster in the vector of the map (if the tkid doesn't exists create the entry)
    if(side==0) 
      _ClusterTkIdMap[tkid].first.push_back(cluster);
    else 
      _ClusterTkIdMap[tkid].second.push_back(cluster);
  }
  if(cont) delete cont;
}


int  TrRecon::GetnTrClusters(int tkid, int side) { 
  return (side==0) ? _ClusterTkIdMap[tkid].first.size() : _ClusterTkIdMap[tkid].second.size(); 
}

AMSTrCluster* TrRecon::GetTrCluster(int tkid, int side, int iclus) { 
  return (side==0) ? _ClusterTkIdMap[tkid].first.at(iclus) : _ClusterTkIdMap[tkid].second.at(iclus); 
}

//================================================================================================
//================================================================================================

int TrRecon::BuildTrRecHits(int rebuild) {

  
  //Get the pointer to the TrCluster container
  VCon* cont=TRCon->GetCont("AMSTrCluster");
  if(!cont){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    printf("TrRecon::BuildTrRecHit  AMSTrCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }

  VCon* cont2=TRCon->GetCont("AMSTrRecHit");
  if(!cont2){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrRecHit Container Reconstruction is Impossible !!!\n");
    return -1;
  }
 
  BuildClusterTkIdMap();

  int ntrrechits = 0;
  int nactiveladders = _ClusterTkIdMap.size();
  for ( ITClusterMap lad=_ClusterTkIdMap.begin();lad!=_ClusterTkIdMap.end();lad++){
    int tkid = lad->first;
    int nx   = lad->second.first.size();
    int ny   = lad->second.second.size();

    /*
    // "ghost" hits
    for (int ix=0; ix<nx; ix++) {
    if (ThrProb>0.) continue;  
    AMSTrRecHit* hit = new AMSTrRecHit(tkid,GetTrCluster(tkid,0,ix),0,-1.,0.,1);
    GetEvent()->AddTrRecHit(hit);     
    ntrrechits++;
    }
    for (int iy=0; iy<ny; iy++) {
    if (ThrProb>0.) continue;  
       AMSTrRecHit* hit = new AMSTrRecHit(tkid,0,GetTrCluster(tkid,1,iy),1.,0.,1);
       GetEvent()->AddTrRecHit(hit);     
       ntrrechits++;
       }
    */

    // all combinations
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
	AMSTrCluster* clX = lad->second.first.at(ix);
	AMSTrCluster* clY = lad->second.second.at(iy);
	float     corr = GetCorrelation(clX,clY);
	float     prob = GetProbToBeCorrelated(clX,clY);
	// the hit must have a minimum required probability (???)
	if (prob<ThrProb) continue;  
	AMSTrRecHit* hit = new AMSTrRecHit(tkid,clX,clY,corr,prob,0);
	cont2->addnext(hit);
	//	hit->Print();
	ntrrechits++;
      }
    }    
  }
  if(cont) delete cont;
  if(cont2) delete cont2;

  return ntrrechits;
} 


////////////////////
// --- TRACKS --- //
////////////////////

double TrRecon::_csqmin;
int    TrRecon::NHitPatterns     = 0;
int   *TrRecon::HitPatternMask   = 0;
int   *TrRecon::HitPatternAttrib = 0;

void TrRecon::BuildRecHitLayerMap() {
  _RecHitLayerMap.clear();

  VCon* cont = TRCon->GetCont("AMSTrRecHit");
  if(!cont){
    printf("TrRecon::BuildRecHitLayerMap:  "
	   "Cant Find AMSTrRecHit Container\n");
    return;
  }

  // loop on TrRecHit
  for (int ii=0;ii<cont->getnelem();ii++){
    AMSTrRecHit* hit = (AMSTrRecHit*)cont->getelem(ii);
    int lay = hit->GetLayer();
    _RecHitLayerMap[lay].push_back(hit);
  }

  delete cont;
}

//================================================================================================

void TrRecon::BuildHitPatterns(int n, int i, int mask)
{
  static int PatternID = 0;

  // Build all the patterns (starting point)
  if (n < 0) {
    NHitPatterns = 256 -1 -8 -8*7/2; // = 219 (Nhit >= 3)
    if (!HitPatternMask)   HitPatternMask   = new int[NHitPatterns];
    if (!HitPatternAttrib) HitPatternAttrib = new int[NHitPatterns];
    PatternID = 0;
    for (int j = 0; j < 6; j++) BuildHitPatterns(j);
    return;
  }

  // Build pattern mask bits recursively with n hits masked
  int maskbit[8] = { 1, 3, 5, 2, 4, 6, 0, 7 };
  if (n > 0) {
    for (int j = i; j < MAXLAY; j++)
      BuildHitPatterns(n-1, j+1, mask | (1<<maskbit[j]));
    return;
  }

  // Check pattern attribute ( = N1*10000 +N2*1000 +N3*100 +N4*10 +N5)
  int atrb = 12221, nmask = 0, iatrb = 10000;
  for (int i = 0; i < MAXLAY; i++) {
    if (i%2 == 1) iatrb /= 10;
    if (mask & (1<<i)) { atrb -= iatrb; nmask++; }
  }
  HitPatternMask  [PatternID] = mask;
  HitPatternAttrib[PatternID] = atrb;

  // Not allowed (1): Both external layers are masked
  if (atrb/10000 == 0 && atrb%10 == 0) HitPatternAttrib[PatternID] = -atrb;

  // Not allowed (2): Both layers on any of internal planes are masked
  if ((atrb/1000)%10 == 0 || (atrb/100)%10 == 0 || (atrb/10)%10 == 0)
    HitPatternAttrib[PatternID] = -atrb;

  // Not allowed (3): Any external layers are masked and Nmask >= 3
  if (nmask >= 3 && (atrb/10000 == 0 || atrb%10 == 0))
    HitPatternAttrib[PatternID] = -atrb;

  PatternID++;
}


//================================================================================================

int TrRecon::BuildTrTracks(int rebuild)
{
  VCon* cont = TRCon->GetCont("AMSTrTrack");
  if(!cont){
    printf("TrRecon::BuildTrTrack  "
	   "Cant Find AMSTrTrack Container\n");
    return -1;
  }

  // Build hit patterns if not yet built
  if (!HitPatternMask) BuildHitPatterns();

  // Build a map for the hits layer
  BuildRecHitLayerMap();

  _ptest = new AMSTrTrack();

  int ntrack = 0;
  while (ntrack < TRFITFFKEY.MaxNtrack) {
    _csqmin = TRFITFFKEY.MaxChisq[0];
    _ptrack = 0;

    // Scan track with each hit pattern
    for (int i = 0; i < NHitPatterns && !_ptrack; i++) {
      // Skip if the pattern is not allowed
      if (!TRFITFFKEY.patternp02[i] && HitPatternAttrib[i] < 0) continue;

      // Make a pattern vector: layer[]
      int layer[MAXLAY], nlyr = 0;
      for (int j = 0; j < MAXLAY; j++)
	if (!(HitPatternMask[i] & (1<<j))) layer[nlyr++] = j;

      if (nlyr < TRFITFFKEY.MinNhit) break;

      // Scan hits
      int iscan[MAXLAY], imult[MAXLAY];
      _ptest->_Nhits = nlyr;
      ScanHit(layer, iscan, imult);
    }

    // Add a new track if found
    if (_ptrack) {
      _ptrack->Fit(AMSTrTrack::kSimple);
      cont->addnext(_ptrack);
      
      // Mark hits as USED
      int nhits = _ptrack->GetNhits();
      // Build the index table
      _ptrack->BuildHitsIndex();
      for (int i = 0; i < nhits; i++) {
	AMSTrRecHit *phit = _ptrack->GetHit(i);
	if (phit->_status&USED) phit->_status |= AMBIG;
	else phit->_status |= USED;

	// Set multiplicity index
	if (phit->_imult < 0) phit->_imult = _ptrack->_iMult[i];
      }
      ntrack++;
      //      _ptrack->myprint();
      _ptrack = 0;
    }

    // Break if no track found
    else break;
  }

  delete _ptest;

  for (int ii=0;ii<cont->getnelem();ii++){
     AMSTrTrack* tr = (AMSTrTrack*)cont->getelem(ii);
     tr->Fit(AMSTrTrack::kSimple);
     tr->Fit(AMSTrTrack::kChoutko);
     printf("Printing Stat for track %d\n",ii);
     tr->myprint();
  }

  delete cont;
  return ntrack;
}

//================================================================================================

void TrRecon::ScanHit(int *layer, int *iscan, int *imult, int i)
{
  // Clear hits vector
  if (i == 0) for (int j = 0; j < MAXLAY; j++) _ptest->_Hits[j] = 0;

  int nhits = _ptest->_Nhits;

  // Evaluate a track
  if (i == nhits) {

    // Ambiguity check
    if (layer[0] != 0 && layer[nhits-1] != MAXLAY-1) return;

    // Fit and check chisquare
    if (_ptest->SimpleFit() < _csqmin && _ptest->Fit(AMSTrTrack::kSimple) < TRFITFFKEY.MaxChisq[1]) {

      // Replace the track candidate
      if (!_ptrack) _ptrack = new AMSTrTrack;
      *_ptrack = *_ptest;
      int pat=0;
      for (int kk=0;kk<_ptrack->GetNhits();kk++)
	pat|=1<<_ptrack->GetHit(kk)->GetLayer();
      _ptrack->_Pattern   = pat;
      _ptrack->_PatternX  = pat;
      _ptrack->_PatternY  = pat;
      _ptrack->_PatternXY = pat;
      // _ptrack->BuildHitsIndex();
      _csqmin = _ptest->GetChisq(AMSTrTrack::kSimple);
    }

    return;
  }

  // Loop for each hit in the layer
  
  int nscan = _RecHitLayerMap[layer[i]+1].size();
  for (iscan[i] = 0; iscan[i] < nscan; iscan[i]++) {

    // Get and check a hit
    AMSTrRecHit *hit = _RecHitLayerMap[layer[i]+1].at(iscan[i]);
    if (hit->GetLayer() != layer[i]+1) continue;
    if ((hit->_status&USED) && hit->GetLayer() > 2) continue;
    _ptest->_Hits[i] = hit;
    _ptest->_iMult[i] = 0;

    // Optimization only for non-B run: Check with only Y-side
    if (MAGSFFKEY.magstat == 0 && i >= 2) {
      double err[3] = { 0, 0.03, 0.03 };

      _ptest->_Nhits = i+1;
      double csq = _ptest->SimpleFit(err);
      _ptest->_Nhits = nhits;
      if (csq > TRFITFFKEY.MaxChisq[0]) continue;
    }

    int nmult = hit->GetMultiplicity(), im0 = 0;

    // Fix multiplicity index if already determined
    if (hit->_imult >= 0) { 
      im0 = hit->_imult; 
      nmult = im0+1; 
    }

    // Tighten multiplicity range if the previous layer is on the same plane
    else if (i > 0 && 
	     (layer[i-1] == 1 || layer[i-1] == 3 || layer[i-1] == 5) &&
	     layer[i-1]+1 == layer[i]) {
      im0   = std::max(imult[i-1]-1, 0);
      nmult = std::min(imult[i-1]+2, hit->GetMultiplicity());
    }

    // Loop for each multiplicity
    for (imult[i] = im0; imult[i] < nmult; imult[i]++) {

      _ptest->_iMult[i] = imult[i];

      // Optimization only for non-B run: Check with only X-side
      if (MAGSFFKEY.magstat == 0 && i >= 2) {
	double err[3] = { 0.03, 0, 0.03 };

	_ptest->_Nhits = i+1;
	double csq = _ptest->SimpleFit(err);
	_ptest->_Nhits = nhits;
	if (csq > TRFITFFKEY.MaxChisq[0]) continue;
      }

      else {

	// Make a quick check parameter
	if (i == 2) {
	  AMSPoint x0 = _ptest->GetHit(0)->GetCoord(imult[0]);
	  AMSPoint x2 = _ptest->GetHit(2)->GetCoord(imult[2]);
	  _Par[0][0] = (x2[0]-x0[0])/(x2[2]-x0[2]);
	  _Par[1][0] = (x2[1]-x0[1])/(x2[2]-x0[2]);
	  _Par[0][1] =  x0[0]-_Par[0][0]*x0[2];
	  _Par[1][1] =  x0[1]-_Par[1][0]*x0[2];
	  _Par[0][2] = sqrt(1+_Par[0][0]*_Par[0][0]);
	  _Par[1][2] = sqrt(1+_Par[1][0]*_Par[1][0]);
	}
	
	// Quick check
	if (i >= 2) {
	  int l = (i == 2) ? 1 : i;
	  AMSPoint x1 = _ptest->GetHit(l)->GetCoord(imult[l]);
	  
	  if ( !(fabs(_Par[0][1]+_Par[0][0]*x1[2]-x1[0]) 
		 < TRFITFFKEY.SearchRegStrLine*_Par[0][2] &&
		 fabs(_Par[1][1]+_Par[1][0]*x1[2]-x1[1]) 
		 < TRFITFFKEY.SearchRegCircle *_Par[1][2]) ) continue;
	  
	  _ptest->_Nhits = i+1;
	  double csq = _ptest->SimpleFit();
	  _ptest->_Nhits = nhits;
	  if (csq > TRFITFFKEY.MaxChisq[0]) continue;
	}
	
      }
      
      // Go to the next layer
      ScanHit(layer, iscan, imult, i+1);
    }
  }
}

//=========================================================================
// #####################################
// #             MC HITS               #
// #####################################

extern "C" double rnormx();


void TrRecon::sitkhits(int idsoft, float vect[],
			   float edep, float step, int itra)
{
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
		TkDBc::Head->_ssize_active[1]/2,
		TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
  int sensor = abs(idsoft)/10000;
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa);
  TkSens tksb(tkid, ppb);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];

  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }
  
  // Create a new object
  VCon* aa=TRCon->GetCont("AMSTrMCCluster");
  if(aa)
    aa->addnext(  new AMSTrMCCluster(idsoft, pa, pb, pgl,pmom,edep , itra));
  if(aa) delete aa;
}



void TrRecon::sitkdigi()
{
  //Get the pointer to the TrMCCluster container
  VCon* cont=TRCon->GetCont("AMSTrMCCluster");
  if(!cont){
    printf("TrSim::sitkdigi()  Cant Find AMSMCCluster Container Digitization is Impossible !!!\n");
    return ;
  }
  if(cont->getnelem()==0){
    printf("TrSim::sitkdigi()  AMSTrMCCluster Container is Empty  Digitzation is Impossible !!!\n");
    return ;
  }else
    printf("TrSim::sitkdigi()  AMSTrMCCluster Container has %d elements \n",cont->getnelem());
  //Create the map of TrMCClusters
  TrMap<AMSTrMCCluster> MCClMap;
  
  for (int ii=0;ii<cont->getnelem();ii++){
    AMSTrMCCluster* clu=(AMSTrMCCluster*)cont->getelem(ii);
    if (clu)MCClMap.Add(clu);
    //clu->_printEl(cerr); 
  }

  //LOOP ON ALL THE LADDERS
  for (int lay=1;lay<=trconst::nlays;lay++){
    for (int slot=1;slot<=trconst::maxlad;slot++){
      for(int side=-1;side<2;side=side+2){
	int tkid=(lay*100+slot)*side;
	if(!TkDBc::Head->FindTkId(tkid)) continue  ;
	
	TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
	if (!tcal) {
	  std::cerr << "ERROR(1) tcal not found: " << tkid << std::endl;
	  continue;
	}
	//	printf("Ladder %+03d Has been selected\n",tkid);
	geant* buf=new geant[1024];
	//Expand the noise
	for(int ii=0;ii<1024;ii++){
	  if(tcal->Status(ii)&TrLadCal::dead)
	    buf[ii]=-1000;
	  else
	    buf[ii]= tcal->Sigma(ii)*rnormx();
	  // printf("%4d: %6.2f ",ii, buf[ii]);
	}
	//expand the TrMCClusters
	//	printf("\n");

	for( int icl=0;icl<MCClMap.GetNelem(tkid);icl++){
	  AMSTrMCCluster* cl=MCClMap.GetElem(tkid,icl);

	  int addK=cl->GetAdd(0);
	  // printf("ClusterK: ");
	  for (int jj=0;jj<cl->GetSize(0);jj++){
	    buf[addK+jj] += cl->GetSignal(jj,0)*10.;
	    //  printf(" %4d: %6.2f",addK+jj,cl->GetSignal(jj,0));
	  }
	  //	   printf("\n");
	  int addS=cl->GetAdd(1);
	  //printf("ClusterS: ");
	  for (int jj=0;jj<cl->GetSize(1);jj++){
	    buf[addS+jj] += cl->GetSignal(jj,1)*10.;
	    // printf(" %4d: %6.2f",addS+jj,cl->GetSignal(jj,1));
	  }
	  //printf("\n");
	  //	  printf("Ladder %+03d Has been selected\n",tkid);
	}
	
	// Do clusterization the DSP way

	DSP_Clusterize(tkid,buf);

	delete [] buf;
      }

    }
  }
  if(cont) delete cont;
  return;
}


void TrRecon::DSP_Clusterize(int tkid,float *buf){

  //Get the pointer to the TrMCCluster container
  VCon* cont=TRCon->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::DSP_Clusterize  Cant Find AMSTrRawCluster Container Digitization is Impossible !!!\n");
    return ;
  }


  TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
  if (!tcal) {
    std::cerr << "TrRecon::DSP_Clusterize ERROR(1) tcal not found: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  
  if(!buf){
    std::cerr << "TrRecon::DSP_Clusterize Buffer not available: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  int used[1024];
  memset(used,0,1024*sizeof(used[0]));

  for(int ii=0;ii<639;ii++){
//     printf("%4d: used: %d st: %4d S: %6.2f N: %6.2f  SN: %6.2f \n",
// 	   ii,used[ii],tcal->Status(ii),buf[ii],tcal->Sigma(ii),buf[ii]/tcal->Sigma(ii)
// 	   );
    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/tcal->Sigma(ii)<=TRMCFFKEY.th1sim[0]) continue;
    if(buf[ii+1]/tcal->Sigma(ii+1)>buf[ii]/tcal->Sigma(ii) ) continue;
    //SEED found
    //    printf("SEED %d \n",tkid);
    int seed=ii;
    used[ii]=1;
    int left=0;
    for (int jj=seed-1;jj>=0;jj--)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0])&& used[jj]==0){
	used[jj]=1;
	left=jj;
      }
    
    int right=639;
    for (int jj=seed+1;jj<640;jj++)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0]) && used[jj]==0){
	used[jj]=1;
	right=jj;
      }
    if(left>0) left--;
    if(right<639) right++;
    //printf("LEFT:  %3d  RIGHT:  %3d \n",left,right);
    //    printf("Adding Raw Clus: %d\n",tkid);
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left+1, &(buf[left])));
  }


  for(int ii=640;ii<1024;ii++){

    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/tcal->Sigma(ii)<=TRMCFFKEY.th1sim[1]) continue;
    if(buf[ii+1]/tcal->Sigma(ii+1)>buf[ii]/tcal->Sigma(ii) ) continue;
    //SEED found
    int seed=ii;
    used[ii]=1;
    int left=640;
    for (int jj=seed-1;jj>=640;jj--)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	left=jj;
      }    
    int right=1023;
    for (int jj=seed+1;jj<1024;jj++)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	right=jj;
      }
    
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left+1, &(buf[left])));
  }
  if(cont) delete cont;

}


/*
Tracker hit pattern list

PatID Nhits   Pattern  Sw Attrib
    0     8  11111111   1  12221

    1     7  10111111   1  11221
    2     7  11101111   1  12121
    3     7  11111011   1  12211
    4     7  11011111   1  11221
    5     7  11110111   1  12121
    6     7  11111101   1  12211
    7     7  01111111   1  02221
    8     7  11111110   1  12220

    9     6  10101111   1  11121
   10     6  10111011   1  11211
   11     6  10011111   0  10221
   12     6  10110111   1  11121
   13     6  10111101   1  11211
   14     6  00111111   1  01221
   15     6  10111110   1  11220
   16     6  11101011   1  12111
   17     6  11001111   1  11121
   18     6  11100111   0  12021
   19     6  11101101   1  12111
   20     6  01101111   1  02121
   21     6  11101110   1  12120
   22     6  11011011   1  11211
   23     6  11110011   1  12111
   24     6  11111001   0  12201
   25     6  01111011   1  02211
   26     6  11111010   1  12210
   27     6  11010111   1  11121
   28     6  11011101   1  11211
   29     6  01011111   1  01221
   30     6  11011110   1  11220
   31     6  11110101   1  12111
   32     6  01110111   1  02121
   33     6  11110110   1  12120
   34     6  01111101   1  02211
   35     6  11111100   1  12210
   36     6  01111110   0  02220

   37     5  10101011   1  11111
   38     5  10001111   0  10121
   39     5  10100111   0  11021
   40     5  10101101   1  11111
   41     5  00101111   0  01121
   42     5  10101110   0  11120
   43     5  10011011   0  10211
   44     5  10110011   1  11111
   45     5  10111001   0  11201
   46     5  00111011   0  01211
   47     5  10111010   0  11210
   48     5  10010111   0  10121
   49     5  10011101   0  10211
   50     5  00011111   0  00221
   51     5  10011110   0  10220
   52     5  10110101   1  11111
   53     5  00110111   0  01121
   54     5  10110110   0  11120
   55     5  00111101   0  01211
   56     5  10111100   0  11210
   57     5  00111110   0  01220
   58     5  11001011   1  11111
   59     5  11100011   0  12011
   60     5  11101001   0  12101
   61     5  01101011   0  02111
   62     5  11101010   0  12110
   63     5  11000111   0  11021
   64     5  11001101   1  11111
   65     5  01001111   0  01121
   66     5  11001110   0  11120
   67     5  11100101   0  12011
   68     5  01100111   0  02021
   69     5  11100110   0  12020
   70     5  01101101   0  02111
   71     5  11101100   0  12110
   72     5  01101110   0  02120
   73     5  11010011   1  11111
   74     5  11011001   0  11201
   75     5  01011011   0  01211
   76     5  11011010   0  11210
   77     5  11110001   0  12101
   78     5  01110011   0  02111
   79     5  11110010   0  12110
   80     5  01111001   0  02201
   81     5  11111000   0  12200
   82     5  01111010   0  02210
   83     5  11010101   1  11111
   84     5  01010111   0  01121
   85     5  11010110   0  11120
   86     5  01011101   0  01211
   87     5  11011100   0  11210
   88     5  01011110   0  01220
   89     5  01110101   0  02111
   90     5  11110100   0  12110
   91     5  01110110   0  02120
   92     5  01111100   0  02210

   93     4  10001011   0  10111
   94     4  10100011   0  11011
   95     4  10101001   0  11101
   96     4  00101011   0  01111
   97     4  10101010   0  11110
   98     4  10000111   0  10021
   99     4  10001101   0  10111
  100     4  00001111   0  00121
  101     4  10001110   0  10120
  102     4  10100101   0  11011
  103     4  00100111   0  01021
  104     4  10100110   0  11020
  105     4  00101101   0  01111
  106     4  10101100   0  11110
  107     4  00101110   0  01120
  108     4  10010011   0  10111
  109     4  10011001   0  10201
  110     4  00011011   0  00211
  111     4  10011010   0  10210
  112     4  10110001   0  11101
  113     4  00110011   0  01111
  114     4  10110010   0  11110
  115     4  00111001   0  01201
  116     4  10111000   0  11200
  117     4  00111010   0  01210
  118     4  10010101   0  10111
  119     4  00010111   0  00121
  120     4  10010110   0  10120
  121     4  00011101   0  00211
  122     4  10011100   0  10210
  123     4  00011110   0  00220
  124     4  00110101   0  01111
  125     4  10110100   0  11110
  126     4  00110110   0  01120
  127     4  00111100   0  01210
  128     4  11000011   0  11011
  129     4  11001001   0  11101
  130     4  01001011   0  01111
  131     4  11001010   0  11110
  132     4  11100001   0  12001
  133     4  01100011   0  02011
  134     4  11100010   0  12010
  135     4  01101001   0  02101
  136     4  11101000   0  12100
  137     4  01101010   0  02110
  138     4  11000101   0  11011
  139     4  01000111   0  01021
  140     4  11000110   0  11020
  141     4  01001101   0  01111
  142     4  11001100   0  11110
  143     4  01001110   0  01120
  144     4  01100101   0  02011
  145     4  11100100   0  12010
  146     4  01100110   0  02020
  147     4  01101100   0  02110
  148     4  11010001   0  11101
  149     4  01010011   0  01111
  150     4  11010010   0  11110
  151     4  01011001   0  01201
  152     4  11011000   0  11200
  153     4  01011010   0  01210
  154     4  01110001   0  02101
  155     4  11110000   0  12100
  156     4  01110010   0  02110
  157     4  01111000   0  02200
  158     4  01010101   0  01111
  159     4  11010100   0  11110
  160     4  01010110   0  01120
  161     4  01011100   0  01210
  162     4  01110100   0  02110

  163     3  10000011   0  10011
  164     3  10001001   0  10101
  165     3  00001011   0  00111
  166     3  10001010   0  10110
  167     3  10100001   0  11001
  168     3  00100011   0  01011
  169     3  10100010   0  11010
  170     3  00101001   0  01101
  171     3  10101000   0  11100
  172     3  00101010   0  01110
  173     3  10000101   0  10011
  174     3  00000111   0  00021
  175     3  10000110   0  10020
  176     3  00001101   0  00111
  177     3  10001100   0  10110
  178     3  00001110   0  00120
  179     3  00100101   0  01011
  180     3  10100100   0  11010
  181     3  00100110   0  01020
  182     3  00101100   0  01110
  183     3  10010001   0  10101
  184     3  00010011   0  00111
  185     3  10010010   0  10110
  186     3  00011001   0  00201
  187     3  10011000   0  10200
  188     3  00011010   0  00210
  189     3  00110001   0  01101
  190     3  10110000   0  11100
  191     3  00110010   0  01110
  192     3  00111000   0  01200
  193     3  00010101   0  00111
  194     3  10010100   0  10110
  195     3  00010110   0  00120
  196     3  00011100   0  00210
  197     3  00110100   0  01110
  198     3  11000001   0  11001
  199     3  01000011   0  01011
  200     3  11000010   0  11010
  201     3  01001001   0  01101
  202     3  11001000   0  11100
  203     3  01001010   0  01110
  204     3  01100001   0  02001
  205     3  11100000   0  12000
  206     3  01100010   0  02010
  207     3  01101000   0  02100
  208     3  01000101   0  01011
  209     3  11000100   0  11010
  210     3  01000110   0  01020
  211     3  01001100   0  01110
  212     3  01100100   0  02010
  213     3  01010001   0  01101
  214     3  11010000   0  11100
  215     3  01010010   0  01110
  216     3  01011000   0  01200
  217     3  01110000   0  02100
  218     3  01010100   0  01110
*/
