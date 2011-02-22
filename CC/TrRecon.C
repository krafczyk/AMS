/// $Id: TrRecon.C,v 1.93 2011/02/22 09:22:29 shaino Exp $ 

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
/// $Date: 2011/02/22 09:22:29 $
///
/// $Revision: 1.93 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecon.h"
#include "TrMap.h"
#include "tkdcards.h"
#include "MagField.h"

#include "TkDBc.h"
#include "TrFit.h"
#include "TkSens.h"
#include "TkCoo.h"
#include "TrMCCluster.h"
#include "Vertex.h"
#include "VCon.h"
#include "TrSim.h"
#include "HistoMan.h"
#include <sys/resource.h>

#ifndef __ROOTSHAREDLIBRARY__
#include "cern.h"
#include "trrec.h"
#include "event.h"
#include "trdrec.h"
#endif


ClassImp(TrReconPar);

TrReconPar::TrReconPar()
{
  // clustering - parameters
  for( int ii=0;ii<9;ii++){
    ThrSeed[0][ii]  = 3.50;
    ThrSeed[1][ii]  = 3.50;
  }
  ThrNeig[0]  = 1.00;
  ThrNeig[1]  = 1.00;
  SeedDist[0] =  3;
  SeedDist[1] =  3;

  // hit signal correlation (only muons/protons)
  // PZ warning the real values for GGpars are in TrRecHit
  GGpars[0]  = 1428.;
  GGpars[1]  = 0.0000;
  GGpars[2]  = 0.1444;
  GGpars[3]  = 1645.;
  GGpars[4]  = 0.0109;
  GGpars[5]  = 0.0972;
  GGintegral = 91765.;
  ThrProb    = 0;//0.00001;

  // Parameters for the full reconstruction
  MaxNrawCls    = 2000;
  lowdt         = 200;
  MaxNtrCls     = 1000;
  MaxNtrCls_ldt = 150;
  MaxNtrHit     = 100;
  TrTimeLim     = 1000;

  // Performance tuning parameters for track reconstruction
  MaxNtrack = 2;
  MinNhitX  = 4;
  MinNhitY  = 5;
  MinNhitXY = 4;
  PatAllowOption = 7;
  MaxChisqAllowed   = 300;
  LadderScanRange   = 7.3; //= TkDBc::Head->_ladder_Ypitch
  ClusterScanRange  = 0.5;
  MaxChisqForLScan  = 2.2;
  ErrXForScan       = 300e-4;
  ErrYForScan       = 300e-4;

  TrackThrSeed[0]   = -1;
  TrackThrSeed[1]   = -1;
  NsigmaMerge       = 100;

  NbuildTrack = NcutRaw = NcutLdt = NcutCls = NcutHit = NcutCpu = 0;
}

void TrReconPar::SetParFromDataCards()
{
  for (int ii=0;ii<9;ii++){
    ThrSeed[0][ii]  = TRCLFFKEY.ThrSeed[0][ii]  ;
    ThrSeed[1][ii]  = TRCLFFKEY.ThrSeed[1][ii]  ;
  }
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

  MaxNrawCls = TRCLFFKEY.MaxNrawCls;
  MaxNtrCls  = TRCLFFKEY.MaxNtrCls;
  lowdt      = TRCLFFKEY.lowdt;
  MaxNtrCls_ldt= TRCLFFKEY.MaxNtrCls_ldt;
  MaxNtrHit  = TRCLFFKEY.MaxNtrHit;
  TrTimeLim  = TRCLFFKEY.TrTimeLim;

  MaxNtrack         = TRCLFFKEY.MaxNtrack;
  MinNhitX          = TRCLFFKEY.MinNhitX;
  MinNhitY          = TRCLFFKEY.MinNhitY;
  MinNhitXY         = TRCLFFKEY.MinNhitXY;
  PatAllowOption    = TRCLFFKEY.PatAllowOption;
  MaxChisqAllowed   = TRCLFFKEY.MaxChisqAllowed;
  LadderScanRange   = TRCLFFKEY.LadderScanRange;
  ClusterScanRange  = TRCLFFKEY.ClusterScanRange;
  MaxChisqForLScan  = TRCLFFKEY.MaxChisqForLScan;
  ErrXForScan       = TRCLFFKEY.ErrXForScan;
  ErrYForScan       = TRCLFFKEY.ErrYForScan;
  TrackThrSeed[0]   = TRCLFFKEY.TrackThrSeed[0];
  TrackThrSeed[1]   = TRCLFFKEY.TrackThrSeed[1];

  TrTrackR::AdvancedFitBits = TRCLFFKEY.AdvancedFitFlag;
}

extern MAGSFFKEY_DEF MAGSFFKEY;

static double AMSgettime(){

// For gcc3
#ifndef RUSAGE_THREAD
#define RUSAGE_THREAD RUSAGE_SELF
#endif

  struct rusage r_usage;
  int aa=getrusage(RUSAGE_THREAD, &r_usage);
  if (aa<0) return -1; //error
  double u=r_usage.ru_utime.tv_sec+ r_usage.ru_utime.tv_usec/1000000.;
  double s=r_usage.ru_stime.tv_sec+ r_usage.ru_stime.tv_usec/1000000.;
  return u+s;
}


//TrRecon* TrRecon::Head=0;
TrCalDB* TrRecon::_trcaldb=0;

// TrRecon* TrRecon::Create(int recreate){
//   if (Head && !recreate){
//     printf("TrRecon::Create Warning a Trecon instance already exist!\n");
//     return Head;
//   }
//   if( (Head && recreate)||(!Head))
//     Head= new TrRecon();

void TrRecon::InitBuffer(){
  for (int i = 0; i < BUFSIZE; i++) {
    _adcbuf2[i] =  0;
    _sigbuf2[i] =  -1;
    _stabuf2[i] =  -1;
  } 
}

//   return Head;
// }

// // clustering - analysis structure
// float       TrRecon::_adcbuf[TrRecon::BUFSIZE];
// float       TrRecon::_sigbuf[TrRecon::BUFSIZE];
// int         TrRecon::_stabuf[TrRecon::BUFSIZE];

float       TrRecon::_adcbuf2[TrRecon::BUFSIZE];
float       TrRecon::_sigbuf2[TrRecon::BUFSIZE];
int         TrRecon::_stabuf2[TrRecon::BUFSIZE];
// vector<int> TrRecon::_seedaddresses;

TrReconPar TrRecon::RecPar;

void TrRecon::SetParFromDataCards()
{
  TrClusterR::DefaultCorrOpt     = TRCLFFKEY.ClusterSigCorrOpt;     // 16
  TrClusterR::DefaultUsedStrips  = TRCLFFKEY.ClusterCofGUsedStrips; // 17
  TrClusterR::TwoStripThresholdX = TRCLFFKEY.TwoStripThresholdX;    // 18
  TrClusterR::TwoStripThresholdY = TRCLFFKEY.TwoStripThresholdY;    // 19

  RecPar.SetParFromDataCards();

  cout<<"TrRecon::SetParFromDataCards-I-TrackThrSeed= "
      <<RecPar.TrackThrSeed[0]<<" "
      <<RecPar.TrackThrSeed[1]<<endl;

  TrDEBUG  = TRCLFFKEY.TrDEBUG;  // 39
  PZDEBUG  = TRCLFFKEY.PZDEBUG;  // 40
  TasRecon = TRCLFFKEY.TasRecon; // 41
}

TrRecon::~TrRecon() {
  Clear();
}

void TrRecon::Clear(Option_t *option) {
  /*
  for(int ii=0;ii<2;ii++){
    for (int jj=0;jj<9;jj++)RecPar.ThrSeed[ii][jj]=0;
    RecPar.ThrNeig[ii]=0;
    RecPar.SeedDist[ii]=0;
  }
  //
  memset(RecPar.GGpars,0,6*sizeof(RecPar.GGpars[0]));
  RecPar.GGintegral=0;
  RecPar.ThrProb=0;
  */

  _ClusterTkIdMap.clear();
  _RecHitLayerMap.clear();
  // _Patterns.clear();
  _TasPar = 0;
  _CpuTimeUp = 0;
  _CpuTime   = 0;
}

// option is temporary, 0:No_reco 1:Up_to_TrCluster 2:Full
int TrRecon::Build(int option)
{
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont) {
    cerr << "TrRecon::Build  Cant Find AMSTrRawCluster" << endl;
    return -1;
  }

  // Check number of TrRawClusters
  int nraw = cont->getnelem();
  delete cont;

  if (option == 0) return nraw;
  if (nraw <= 0 || RecPar.MaxNrawCls <= nraw) return 0;

  // temporary
  int rebuild = 0;

  // Build TrClusters
  int ncls = BuildTrClusters(rebuild);
  if (option == 1) return ncls;

  // Check number of TrClusters
  if (ncls <= 0 || RecPar.MaxNtrCls <= ncls) return 0;

  // Build TrRecHits
  int nhit = BuildTrRecHits(rebuild);
  if (option == 2) return nhit;

  // Check number of TrRecHits
  int ret = 0;
  if (0 < nhit && nhit < RecPar.MaxNtrHit) {
    // Build TrTracks and TrRecHits associated with the track
    ret = BuildTrTracks(rebuild);
  }

  return ret;
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
//     TrRecHitR* hit   = GetEvent()->GetTrRecHit(hh);
//     AMSPoint  point = hit->GetGlobalCoordinate();
//     printf("Hit: %3d  (x,y,z)=(%10.4f,%10.4f,%10.4f)  mult: %1d  corr: %8.4f  prob: %7.5f  stat: %2d\n",
//            hh,point.x(),point.y(),point.z(),hit->GetMultiplicity(),hit->GetCorrelation(),hit->GetProb(),hit->getstatus());
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


void TrRecon::Init(){

  InitBuffer();
}
    
//////////////////////
// --- CLUSTERS --- //
//////////////////////

void TrRecon::ClearBuffer() {
 


  memcpy(_adcbuf, _adcbuf2,BUFSIZE*sizeof(float));
  memcpy(_sigbuf, _sigbuf2,BUFSIZE*sizeof(float));
  memcpy(_stabuf, _stabuf2,BUFSIZE*sizeof(int));

  _seedaddresses.clear();
}

int TrRecon::ExpandClusterInBuffer(TrRawClusterR* cluster) {
  int entries = 0;
  int nelements = cluster->GetNelem();
  int firststripaddress = cluster->GetAddress();
  TrLadCal* cal = GetTrCalDB()->FindCal_TkId(cluster->GetTkId());
  if (!cal) {
    static int nerr=0;
    if (nerr++<100) printf ("TrRecon::ExpandClusterInBuffer, WARNING calibration not found!! TkId= %d\n", cluster->GetTkId());
    return -9999;
  }
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
  if (TasRecon) return BuildTrTasClusters(rebuild);

  if (TRMCFFKEY.SimulationType==TrSim::kNoRawSim) {
    VCon* cont3=GetVCon()->GetCont("AMSTrCluster");
    int ncls3 = cont3->getnelem();
    delete cont3;
    return ncls3;
  }

  //Get the pointer to the TrRawCluster container
  //  AMSContainer* cont=GetCont(AMSID("AMSTrRawCluster"));
  VCon* cont=GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrRawCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
//    printf("TrRecon::BuildTrClusters  AMSTrRawCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }
  //  else printf( "TrRecon::BuildTrClusters  AMSTrRawCluster Container has %d elements\n",cont->getnelem());


  VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
  if(!cont2){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container if requested
  if(rebuild) cont2->eraseC();


  //Build The Map of TrRawClusters
  TrMap<TrRawClusterR> RawMap;


  for (int ii=0;ii< cont->getnelem();ii++){
    TrRawClusterR* clu=(TrRawClusterR*)cont->getelem(ii);
    //clu->Print();
    if (clu)  RawMap.Add(clu);
  }



  // RawMap.info();
  // loop on ladders
  TrMap<TrRawClusterR>::TrMapIT lad=RawMap.LadMap.begin();
  int ncls = 0;
  while (lad!=RawMap.LadMap.end()){
    ClearBuffer();
    //expand clusters in the buffer
    for (int iclus=0; iclus<(int)lad->second.size(); iclus++) 
      ExpandClusterInBuffer( lad->second.at(iclus) );
    
    int TkId=lad->first;
    int layer = abs(TkId/100);
    ncls += BuildTrClustersInSubBuffer(TkId,0,640,0);      // S
    //printf("TkId: %+03d Found %d cluster on S\n",TkId,ncls);
    //printf("TrCluster container has: %d elements\n",cont2->getnelem());
    TkLadder* ll = TkDBc::Head->FindTkId(TkId);
    if(!ll){
      printf("TrRecon::BuildTrClusters: ERROR cant find ladder %d into the database\n",TkId);
    return -1;
    } 
    if ( ll->IsK7()) { 
      ncls += BuildTrClustersInSubBuffer(TkId,640,1024,1); // K7
    }
    else { 
      ncls += BuildTrClustersInSubBuffer(TkId,640,832,0);  // K5 - sensor1
      ncls += BuildTrClustersInSubBuffer(TkId,832,1024,0); // K5 - sensor2 
    }
    lad++;
  }

  if(cont) delete cont;
  if(cont2) delete cont2;
  return ncls;
}


int TrRecon::BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity) {
  int   ntrclusters = 0;
  int   side = (first>639)?0:1;
  if (_trcaldb==0) {
    printf("TrRecon::BuildTrClustersInSubBuffer Error, no _trcaldb specified.\n");
    return -9999; 
  }
  TrLadCal* cal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!cal) {printf ("TrRecon::BuildTrClustersInSubBuffer, WARNING calibration not found!!\n"); return -9999;}
  // create a list of seeds 
  int nseeds = BuildSeedListInSubBuffer(tkid,first, last, cyclicity);
//   printf("Found %d seeds\n",nseeds);
//   for (int jj=0;jj<nseeds;jj++) printf("seed %d  %d\n",jj,_seedaddresses.at(jj));

  VCon* cont=GetVCon()->GetCont("AMSTrCluster");

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

#ifndef __ROOTSHAREDLIBRARY__ 
    if(cont) cont->addnext(new AMSTrCluster(tkid,side,leftaddress,length,seedind,sigg,0));
#else
    if(cont) cont->addnext(new TrClusterR(tkid,side,leftaddress,length,seedind,sigg,0));
#endif
    ntrclusters++;   
  }
  if(cont) delete cont;
  return ntrclusters;
}

int TrRecon::BuildSeedListInSubBuffer(int tkid, int first, int last, int cyclicity) {
  int lay=abs(tkid)/100;
  _seedaddresses.clear();
  int side    = (first>639) ? 0 : 1;  // 0:n(X) 1:p(Y)
  int address = 0;
  for (int ii=first; ii<last; ii++) {
    // 1. the strip has to be well defined
    // 2. a noisy strip can't be a seed strip
    // 3. the seed must exceed the ThrSeed S/N threshold
    if (  (_sigbuf[ii]<1.e-6)||(_stabuf[ii]!=0)||( (_adcbuf[ii]/_sigbuf[ii])<RecPar.ThrSeed[side][lay-1] )  ) continue;
    // 4. if the next strip is good, above threeshold and with signal larger than this one continue
    if( ii<(last-1)&&(_adcbuf[ii+1]>_adcbuf[ii]))
      if ((_sigbuf[ii+1]>0.)&&(_stabuf[ii+1]==0)&&( (_adcbuf[ii+1]/_sigbuf[ii+1])>=RecPar.ThrSeed[side][lay-1]))
	continue;

    // 5. the seed has to be the local maximum among 2*SeedDist+1 good (with status==0) strips
    bool localmax = true;
    for (int jj=1; jj<=RecPar.SeedDist[side]; jj++) {
      // right
      address = GetAddressInSubBuffer(ii+jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
      // left
      address = GetAddressInSubBuffer(ii-jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
    }
    // is not a local maximum
    if (!localmax) continue;
    // A SEED IS FOUND
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
    // the neighboring strip must be defined
    if (_sigbuf[address]<1.e-6) break;
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=RecPar.ThrNeig[side]) break;
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
    // the neighboring strip must be defined
    if (_sigbuf[address]<1.e-6) break;
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=RecPar.ThrNeig[side]) break;
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


void TrRecon::BuildClusterTkIdMap() {
  _ClusterTkIdMap.clear();
  // loop on TrClusters
  VCon* cont=GetVCon()->GetCont("AMSTrCluster");
  for (int ii=0;ii<cont->getnelem();ii++){
    TrClusterR* cluster = (TrClusterR*)cont->getelem(ii);
    //    cluster->Print();
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

TrClusterR* TrRecon::GetTrCluster(int tkid, int side, int iclus) { 
  if (iclus < 0 || GetnTrClusters(tkid, side) <= iclus) return 0;
  return (side==0) ? _ClusterTkIdMap[tkid].first.at(iclus) : _ClusterTkIdMap[tkid].second.at(iclus); 
}

int TrRecon::BuildTrRecHits(int rebuild) 
{
  if (TasRecon) return BuildTrTasHits(rebuild);

  //Get the pointer to the TrCluster container
  VCon* cont=GetVCon()->GetCont("AMSTrCluster");
  if (!cont) {
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrCluster Container "
           "Reconstruction is Impossible !!!\n");
    return -1;
  }
  if (cont->getnelem() == 0) {
    delete cont;
    return 0;
  }

  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrRecHit Container "
           "Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container
  if (rebuild)
    cont2->eraseC();

  BuildClusterTkIdMap();

  int ntrrechits = 0;

  for (ITClusterMap lad = _ClusterTkIdMap.begin();
       lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int nx   = lad->second.first.size();
    int ny   = lad->second.second.size();

    // all combinations
    for (int iy=0; iy<ny; iy++) {
      for (int ix=0; ix<nx; ix++) {
        TrClusterR* clX = lad->second.first.at(ix);
        TrClusterR* clY = lad->second.second.at(iy);
        
#ifndef __ROOTSHAREDLIBRARY__
        AMSTrRecHit* hit= new AMSTrRecHit(tkid,clX,clY,0);
#else
        TrRecHitR* hit=new TrRecHitR(tkid,clX,clY,0);
#endif

	if (hit->GetProb()<RecPar.ThrProb){
	  delete hit;
	  continue;  
	}
	cont2->addnext(hit);
        ntrrechits++;
      }

      // "ghost" hits with only y-cluster
      TrClusterR *clY = GetTrCluster(tkid, 1, iy);
#ifndef __ROOTSHAREDLIBRARY__
      cont2->addnext( new AMSTrRecHit(tkid, 0, clY,  0));
#else
      cont2->addnext( new TrRecHitR(tkid, 0, clY,  0));
#endif
      ntrrechits++;
    }    
  }
  if(cont) delete cont;
  if(cont2) delete cont2;

  return ntrrechits;
} 

TrRecHitR* TrRecon::FindHit(TrClusterR* xcls,TrClusterR* ycls){
  VCon* cont1 = GetVCon()->GetCont("AMSTrRecHit");
  TrRecHitR* goodhit=0;
  TrRecHitR* hit=0;
  for (int ii = 0; ii < cont1->getnelem(); ii++) {
    hit=(TrRecHitR*)cont1->getelem(ii);
    if(hit->GetXCluster()==xcls && hit->GetYCluster()==ycls){
      goodhit=hit;
      break;
    }
  }
  delete cont1;
  return goodhit;

}

////////////////////////////////////////////////////////////////
// ------------------------- TRACKS ------------------------- //
////////////////////////////////////////////////////////////////

#define TR_DEBUG_CODE_00 \
if (TrDEBUG >= 6) {\
  int nhit = cont->getnelem();\
  int tkid = 0, intv = 0;\
  for (int i = 0; i < nhit; i++) {\
    TrRecHitR *hit = (TrRecHitR *)cont->getelem(i);\
    if (tkid != 0 && hit->GetTkId() != tkid) intv = 0;\
    if ((intv++)%3 == 0) cout << endl;\
    cout << Form("%4d[%02d](%2d,%2d,%5.3f) ", hit->GetTkId(), i,\
                 hit->GetXClusterIndex(), hit->GetYClusterIndex(),\
                 hit->GetProb());\
    tkid = hit->GetTkId();\
  }\
  cout << endl;\
}

#define TR_DEBUG_CODE_01 \
if (TrDEBUG >= 5) {\
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();\
       it != _HitsTkIdMap.end(); it++) {\
    int tkid = it->first;\
    int nclx = GetnTrRecHits(tkid, 0);\
    int ncly = GetnTrRecHits(tkid, 1);\
    cout << Form("TkId %4d Nclx= %2d Ncly= %2d", tkid, nclx, ncly) << endl;\
    for (int i = 0; i < nclx; i++) {\
      cout << " ";\
      for (int j = 0; j < ncly; j++) {\
        TrRecHitR *hit = GetTrRecHit(tkid, i, j);\
        if (!hit) { if (j < ncly-1) cout << "-------------- "; }\
        else cout << Form("%2d[%2d][%2d](%2.0f) ",\
                          cont->getindex(hit), hit->GetXClusterIndex(),\
                          hit->GetYClusterIndex(), hit->GetProb()*100);\
      }\
      cout << endl;\
    }\
  }\
}

#define TR_DEBUG_CODE_02 \
if (TrDEBUG >= 1) {\
  for (int ly = 1; ly <= patt->GetSCANLAY(); ly++) {\
    int nlad = _LadderHitMap[ly-1].size();\
    cout << Form("L:%d (%2d,%2d) ", ly, nlad, _NladderXY[ly-1]);\
    for (int i = 0; i < nlad; i++) {\
      int tkid  = _LadderHitMap[ly-1].at(i);\
      int nhity =  GetnTrRecHits(tkid, 1);\
      int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;\
      cout << Form("%4d(%d,%d) ", tkid, nhitx, nhity);\
    }\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_03 \
if (TrDEBUG >= 1) {\
  for (int i = 0; i < patt->GetSCANLAY(); i++) {\
    int nlad = _LadderHitMap[i].size();\
    cout << Form("L:%d (%2d,%2d) ", i+1, nlad, _NladderXY[i]);\
    for (int j = 0; j < nlad; j++) {\
      int tkid  = _LadderHitMap[i].at(j);\
      int nhitx = GetnTrClusters(tkid, 0);\
      int nhity = GetnTrClusters(tkid, 1);\
      cout << Form("%4d(%d,%d) ", tkid, nhitx, nhity);\
    }\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_04 \
if (TrDEBUG >= 11 || (TrDEBUG >= 10 && ddiff < it.psrange)) {\
  cout << "PreScan ";\
  for (int i = 0; i <= nlay; i++) {\
    cout << Form("%4d", it.tkid[it.ilay[i]]);\
    if (it.side == 0) cout << Form("-%d", it.imult[it.ilay[i]]);\
    cout << " ";\
  }\
  for (int i = nlay+1; i < patt->GetSCANLAY(); i++) {\
    cout << "     ";\
    if (it.side == 0) cout << "  ";\
  }\
  cout << Form(": %5.1f %3.1f", pc-intp, it.psrange) << endl;\
}

#define TR_DEBUG_CODE_05 \
if (TrDEBUG >= 6) {\
  int nhit  = GetnTrRecHits(tkid);\
  int nhitx = GetnTrRecHits(tkid, 0);\
  int nhity = GetnTrRecHits(tkid, 1);\
  if ((nhit-nhity)%nhity != 0 || (nhit-nhity)/nhity != nhitx-1) {\
    cout << Form("Warning in TrRecon::SetLayerOrder "\
                 "Consistency check failed: %d %d %d:%2d %d %d %d %d", \
                 i, nlay, layer, nhit, nhity, nhitx,\
                 (nhit-nhity)%nhity, (nhit-nhity)/nhity) << endl;\
  }\
}

#define TR_DEBUG_CODE_11 \
if (TrDEBUG >= 2) {\
  cout << "ScanLadders " << pattern << " "\
       << patt->GetHitPatternStr(pattern, '_', 'O') << endl;\
}

#define TR_DEBUG_CODE_21 \
if (TrDEBUG >= 2) {\
  cout << "Lcand: ";\
  for (int i = 0; i < it.nlayer; i++){\
    int nhity =  GetnTrRecHits(it.tkid[i], 1);\
    int nhitx = (GetnTrRecHits(it.tkid[i])-nhity)/nhity;\
    cout << Form("%4d(%d%d) ", it.tkid[i], nhitx, nhity);\
  }\
  cout << endl;\
}

#define TR_DEBUG_CODE_22 \
if (TrDEBUG >= 1) {\
  cout << "Lcand: ";\
  for (int i = 0; i < it.nlayer; i++){\
    int nhity =  GetnTrRecHits(it.tkid[i], 1);\
    int nhitx = (GetnTrRecHits(it.tkid[i])-nhity)/nhity;\
    cout << Form("%4d(%d%d) ", it.tkid[i], nhitx, nhity);\
  }\
  cout << endl;\
  cout << "Ccand: ";\
  for (int i = 0; i < it.nlayer; i++) {\
    if (_itchit.iscan[i][0] >= 0)\
      cout<< Form(" (%d,%d,",_itchit.iscan[i][0], _itchit.imult[i]);\
    else cout << " (-,-,";\
    if (_itchit.iscan[i][1] >= 0) cout<< _itchit.iscan[i][1];\
    else cout << "-";\
    cout << ") ";\
  }\
  cout << endl << "Chisq= "\
       << _itchit.chisq[0] << " " << _itchit.chisq[1] << endl;\
}

#define TR_DEBUG_CODE_30 \
if (TrDEBUG >= 4) {\
  cout << Form("ScanHits %d %3d %s", it.side, it.pattern,\
               patt->GetHitPatternStr(it.pattern,'_','O')) << endl;\
}

#define TR_DEBUG_CODE_31 \
if (TrDEBUG >= 3) {\
  cout << "EvalHit side= " << it.side << " nlay= " << it.nlayer \
       << " csq= " << csq << " " << _itcand.chisq[it.side] << endl;\
  if (TrDEBUG >= 4) {\
    for (int i = 0; i < it.nlayer && it.ilay[i] >= 0;i++)\
      cout << Form(" %4d-%d,%d", it.tkid[it.ilay[i]],\
                   it.iscan[it.ilay[i]][it.side], it.imult[it.ilay[i]]);\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_32 \
if (TrDEBUG >= 4) {\
  cout << "Replaced: " << endl;\
  for (int i = 0; i < _itcand.nlayer; i++)\
    cout << Form(" %4d-%d%d%d", _itcand.tkid[i],\
                 _itcand.iscan[i][0], _itcand.iscan[i][1], _itcand.imult[i]);\
  cout << endl;\
}

#define TR_DEBUG_CODE_40 \
if (TrDEBUG >= 6) {\
  float locx = TkCoo::GetLocalCoo(tkid, tks.GetStripX()+640, it.imult[il]);\
  AMSPoint loc(locx, 3.5, 0);\
  AMSPoint glo = TkCoo::GetGlobalA(tkid, loc);\
  if (std::abs(px-glo[0]) > 0.5)\
    cout << Form("Diff %5.2f TkId %4d Strp %3d "\
                 "sx %5.2f im %d sen %d dx %6.2f %5.2f", px-glo[0],\
                 tkid, tks.GetStripX(), tks.GetSensCoo().x(), \
                 it.imult[il], tks.GetSensor(), px-lx, ln/2) << endl;\
}

#define TR_DEBUG_CODE_41 \
if (TrDEBUG >= 5) {\
  VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");\
  int ihit = cont2->getindex(hit);\
  cout << "Removed hit " << _itcand.tkid[i] << " " << ihit << endl;\
  int tkid = _itcand.tkid[i];\
  int nclx = GetnTrRecHits(tkid, 0);\
  int ncly = GetnTrRecHits(tkid, 1);\
  cout << Form("TkId %4d Nclx= %2d Ncly= %2d", tkid, nclx, ncly) << endl;\
  for (int ii = 0; ii < nclx; ii++) {\
    cout << " ";\
    for (int j = 0; j < ncly; j++) {\
      TrRecHitR *hit2 = GetTrRecHit(tkid, ii, j);\
      if (!hit2) { if (j < ncly-1) cout << "-------------- "; }\
      else cout << Form("%2d[%2d][%2d](%2.0f) ",\
                        cont2->getindex(hit2), hit2->GetXClusterIndex(),\
                        hit2->GetYClusterIndex(), hit2->GetProb()*100);\
    }\
    cout << endl;\
  }\
  delete cont2;\
}

#define TR_DEBUG_CODE_42 \
if (TrDEBUG >= 1) {\
  cout << "New Track: Pattern: "\
       << patt->GetHitPatternStr(track->GetPattern(),'_','O');\
  cout << " (" << track->GetPattern() << ") "\
       << "Nhits= " << track->GetNhitsX() << " "\
                    << track->GetNhitsY() << " "\
                    << track->GetNhitsXY()<< endl;\
  for (int i = 0; i < track->GetNhits(); i++) {\
    TrRecHitR *hit = track->GetHit(i);\
    if (!hit) continue;\
    TrClusterR *xcls = hit->GetXCluster();\
    TrClusterR *ycls = hit->GetYCluster();\
    if (!xcls && !ycls) continue;\
    int tkid = hit->GetTkId();\
    char cx = (xcls) ? 'X' : ' ';\
    char cy = (ycls) ? 'Y' : ' ';\
    int  im = (xcls) ? hit->GetResolvedMultiplicity() : 0;\
    cout << Form("%4d(%c%c%d) ", tkid, cx, cy, im);\
  }\
  cout << endl;\
  for (int i = 0; i < track->GetNhits(); i++) {\
    TrRecHitR *hit = track->GetHit(i);\
    if (!hit) continue;\
    TrClusterR *xcls = hit->GetXCluster();\
    TrClusterR *ycls = hit->GetYCluster();\
    if (!xcls && !ycls) continue;\
    int tkid = hit->GetTkId();\
    if (xcls) cout << Form("%4.1f ", xcls->GetSeedSN());\
    else      cout << "---- ";\
    if (ycls) cout << Form("%4.1f ", ycls->GetSeedSN());\
    else      cout << "---- ";\
  }\
  cout << endl;\
  for (int i = 0; i < track->GetNhits(); i++) {\
    TrRecHitR *hit = track->GetHit(i);\
    if (!hit) continue;\
    int ily = hit->GetLayer()-1;\
    cout << Form("%4.1f ", track->GetResidual(ily).x()*10);\
    cout << Form("%4.1f ", track->GetResidual(ily).y()*10);\
  }\
  cout << endl;\
  int id;\
  if (MagFieldOn()) {				\
    id = TrTrackR::kSimple;\
    track->FitT(id);\
    cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
                 id, track->GetChisq(id), \
                 track->GetChisqX(id), track->GetChisqY(id), \
                 track->GetRigidity(id), \
                 track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
                 track->GetDir(id)[0]/track->GetDir(id)[2], \
                 track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
    id = TrTrackR::kAlcaraz;\
    track->FitT(id);\
    cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
                 id, track->GetChisq(id), \
                 track->GetChisqX(id), track->GetChisqY(id), \
                 track->GetRigidity(id), \
                 track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
                 track->GetDir(id)[0]/track->GetDir(id)[2], \
                 track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
    id = TrTrackR::kChoutko;\
    track->FitT(id);\
  }\
  else\
    id = TrTrackR::kLinear;\
  cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
               id, track->GetChisq(id),\
               track->GetChisqX(id), track->GetChisqY(id),\
               track->GetRigidity(id), \
               track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
               track->GetDir(id)[0]/track->GetDir(id)[2], \
               track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
  if (TrDEBUG >= 3) {\
    for (int i = 0; i < track->GetNhits(); i++) {\
      TrRecHitR *hit = track->GetHit(i);\
      int il = std::abs(hit->GetTkId())/100-1;\
      cout << Form("%4d %03X %6.2f %6.2f %6.2f res %8.1f %8.1f %8.1f %8.1f", \
                   hit->GetTkId(), hit->getstatus(), hit->GetCoord()[0],\
                   hit->GetCoord()[1], hit->GetCoord()[2],\
                   1e4*track->GetResidual(il, id)[0],\
                   1e4*track->GetResidual(il, TrTrackR::kChoutko)[0],\
                   1e4*track->GetResidual(il, id)[1],\
                   1e4*track->GetResidual(il, TrTrackR::kChoutko)[1])\
           << endl;\
    }\
  }\
}

int TrRecon::TrDEBUG = 0;
int TrRecon::PZDEBUG = 0;



//========================================================
// Maps for fast hit scanning
//========================================================

void TrRecon::RemoveHits(int tkid, int icls, int side)
{
  Hits2DArray *ar = GetHits2DArray(tkid);
  if (!ar) return;
  if ((side != 0 && side != 1) || ar->Ncls[side] <= 0) return;

  int ncls = GetnTrRecHits(tkid, 1-side);
  for (int i = 0; i < ncls; i++) {
    Hits2DArray::iterator it2 = ar->begin();
    it2 += (side == 0) ? ar->index(icls, i)-i
                       : ar->index(i, icls)-i;
    ar->erase(it2);
  }

  if (--ar->Ncls[side] == 0) ar->Ncls[1-side] = 0;
}

void TrRecon::RemoveHits(TrRecHitR *hit)
{
  Hits2DArray *ar = GetHits2DArray(hit->GetTkId());
  if (!ar) return;

  for (int i = 0; i < ar->Ncls[0]; i++)
    for (int j = 0; j < ar->Ncls[1]; j++)
      if (ar->at(i, j) == hit) {
	RemoveHits(hit->GetTkId(), i, 0);
	RemoveHits(hit->GetTkId(), j, 1);
      }
}


void TrRecon::_StartTimer()
{
  _StartTime=AMSgettime();
}

float TrRecon::_CheckTimer() const
{
  float tnow=AMSgettime();
  return tnow-_StartTime;
}

bool TrRecon::SigTERM = false;

void TrRecon::BuildHitsTkIdMap() 
{
  _HitsTkIdMap.clear();

  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if(!cont) {
    cerr << "TrRecon::BuildRecHitLayerMap:  "
            "Cant Find AMSTrRecHit Container" << endl;
    return;
  }
  TR_DEBUG_CODE_00;

  map<int, vector<int> > iclxmap;
  map<int, vector<int> > iclymap;

  // Fill TrRecHits at _HitsTkIdMap[tkid]
  int nhit = cont->getnelem();
  for (int i = 0; i < nhit; i++) {
    TrRecHitR  *hit = (TrRecHitR*)cont->getelem(i);
    TrClusterR *clx = hit->GetXCluster();
    TrClusterR *cly = hit->GetYCluster();
    if (!cly) continue;
    int layr = hit->GetLayer();

    if(TkDBc::Head->GetSetup()==3){
      // AMS-B; Layer 8(on Ecal) and 9(on TRD) excluded from trackfinding
           if (layr >= 8) continue;
      // AMS-B; Layer 8(on Ecal) and 9(on TRD) excluded from trackfinding
    }

    if (hit->Used()) hit->ClearUsed();
    if (RecPar.TrackThrSeed[1] > 0) {
      if (RecPar.TrackThrSeed[0] > 0) {
	if ( clx && clx->GetSeedSN() < RecPar.TrackThrSeed[0] && 
	            cly->GetSeedSN() < RecPar.TrackThrSeed[1]) continue;
      }
      if   (!clx && cly->GetSeedSN() < RecPar.TrackThrSeed[1]) continue;
    }
    
    int tkid = hit->GetTkId();
    _HitsTkIdMap[tkid].push_back(hit);

    int iclx = hit->GetXClusterIndex();
    int icly = hit->GetYClusterIndex();
    vector<int> &vclx = iclxmap[tkid];
    vector<int> &vcly = iclymap[tkid];
    if (iclx >= 0 && std::find(vclx.begin(), vclx.end(), iclx) 
	                                  == vclx.end()) vclx.push_back(iclx);
    if (icly >= 0 && std::find(vcly.begin(), vcly.end(), icly) 
	                                  == vcly.end()) vcly.push_back(icly);
  }

  // Loop on each element of _HitsTkIdMap
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();
       it != _HitsTkIdMap.end(); it++) {
    // Copy original and sort array in descending order of prob.
    Hits2DArray ar0 = it->second;
    std::sort(ar0.begin(), ar0.end(), CompProb());

    // Clear and re-allocate original array
    int tkid = it->first;
    Hits2DArray &arr = it->second;
    arr.clear();
    arr.Ncls[0] = iclxmap[tkid].size()+1;
    arr.Ncls[1] = iclymap[tkid].size();
    arr.insert(arr.begin(), arr.Ncls[0]*arr.Ncls[1], (TrRecHitR*)0);

    // Define cluster index vectors
    vector<int> vidcl[2];

    // Loop on the hits
    for (Hits2DArray::iterator ith = ar0.begin(); ith != ar0.end(); ith++) {
      TrRecHitR *hit = *ith;

      // Get cluster ID (as index in the container)
      int idcl[2], icls[2];
      idcl[0] = hit->GetXClusterIndex();
      idcl[1] = hit->GetYClusterIndex();

      // Loop on X and Y, find cluster ID, push back it if not found
      for (int ss = 0; ss <= 1; ss++) {
        vector<int>::iterator 
          it2 = std::find(vidcl[ss].begin(), vidcl[ss].end(), idcl[ss]);
        icls[ss] = it2-vidcl[ss].begin();
        if (it2 == vidcl[ss].end()) vidcl[ss].push_back(idcl[ss]);
      }

      // Add the hit at [icls[0]][icls[1]]
      int idx = arr.index(icls[0], icls[1]);
      if (idx < 0) std::cerr << "Warning in TrRecon::BuildHitsTkIdMap "
                             << "consistency check failed" << std::endl;
      else arr[idx] = hit;
    }
  }

  TR_DEBUG_CODE_01;
  delete cont;
}

void TrRecon::BuildLadderHitMap()
{
  // Clear LadderHitMap
  for (int i = 0; i < patt->GetSCANLAY(); i++) {
    _NladderXY[i] = 0;
    _LadderHitMap[i].clear();
  }

  // Build LadderHitMap
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();
       it != _HitsTkIdMap.end(); it++) {
    int tkid  = it->first;
    int layer = std::abs(tkid)/100;
    int nhity = GetnTrRecHits(tkid, 1);
    if (nhity <= 0) continue;

    int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;
    if (nhitx > 0) _NladderXY[layer-1]++;
    _LadderHitMap[layer-1].push_back(tkid);
  }
  TR_DEBUG_CODE_02;
}

void TrRecon::BuildLadderClusterMap()
{
  // Clear LadderHitMap
  for (int i = 0; i < SCANLAY_MAX; i++) {
    _NladderXY[i] = 0;
    _LadderHitMap[i].clear();
  }

  // Build LadderHitMap
  for (ITClusterMap lad = _ClusterTkIdMap.begin();
       lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int il   = std::abs(tkid)/100-1;
    if (GetnTrClusters(tkid, 1) > 0 &&
        GetnTrClusters(tkid, 0) > 0) _NladderXY[il]++;
    _LadderHitMap[il].push_back(tkid);
  }
  for (int i = 0; i < patt->GetSCANLAY(); i++)
    std::sort(_LadderHitMap[i].begin(), _LadderHitMap[i].end());

  TR_DEBUG_CODE_03;
}

//========================================================
// Track reconstruction methods
//========================================================

int TrRecon::BuildTrTracks(int rebuild)
{
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("Track1");
#endif
  if (TasRecon) return BuildTrTasTracks(rebuild);

  VCon *cont = GetVCon()->GetCont("AMSTrTrack");
  if (!cont){
    printf("TrRecon::BuildTrTracks  Cant Find AMSTrTrack Container "
           "Reconstruction is Impossible !!!\n");
    return -1;
  }
  if (rebuild) cont->eraseC();
  delete cont;

  if (!patt) {
    int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
    patt = new tkpatt(nn);
    patt->Init(nn);
  }

  // Build _HitTkIdMap
  BuildHitsTkIdMap();

  Ntrials1 = Ntrials2 = 0;

  _CpuTimeUp = false;

  _StartTimer();

  if (RecPar.TrackThrSeed[0] > RecPar.ThrSeed[0][0] || 
      RecPar.TrackThrSeed[1] > RecPar.ThrSeed[1][0]) {
    _MinNhitX  = RecPar.MinNhitX-1;
    _MinNhitY  = RecPar.MinNhitY-1;
    _MinNhitXY = RecPar.MinNhitXY-1;
  }
  else {
    _MinNhitX  = RecPar.MinNhitX;
    _MinNhitY  = RecPar.MinNhitY;
    _MinNhitXY = RecPar.MinNhitXY;
  }
  VCon* cont2 = GetVCon()->GetCont("AMSTrRecHit");
  int ntrrechit=cont2->getnelem();
  delete cont2;
#ifndef __ROOTSHAREDLIBRARY__

AMSgObj::BookTimer.stop("Track1");
  AMSgObj::BookTimer.start("Track2");
#endif
  
  // Main loop
  int ntrack = 0, found = 0;
  TrHitIter it;
  int nhitc =0;
  do {
    // Build LadderHitMap
    BuildLadderHitMap();
    found = 0;
    // Scan Ladders for each pattern until a track found
    for (int pat = 0; !CpuTimeUp() && !found && pat < patt->GetNHitPatterns(); pat++) {
      if (patt->GetHitPatternAttrib(pat) > 0 ){

	//	found=ScanLadders(pat);
	{
	  // Clear the-best-candidate parameters
	  _itcand.nlayer = 0; _itcand.nhitc = _MinNhitXY; 
	  _itcand.chisq[0] = _itcand.chisq[1] = RecPar.MaxChisqAllowed;
	  
	  // Define and fill iterator
	  it.mode = 1; it.pattern = pat; 
	  it.side = 1; it.psrange = RecPar.LadderScanRange;
	  
	  // Loop on layers to check for an empty layer and fill iterator
	  nhitc = it.nlayer = 0;
	  for (int i = 0; i < patt->GetSCANLAY(); i++) {
	    if (!(patt->TestHitPatternMask(pat, i+1))) {
	      if (_LadderHitMap[i].empty()) {found= 0;goto llexit;}
	      if (_NladderXY[i]) nhitc++;
	      it.tkid[it.nlayer++] = (i+1)*100;
	    }
	  }
	  if (it.nlayer < _MinNhitY || nhitc < _MinNhitXY) found=0;
	  else{
#ifndef __ROOTSHAREDLIBRARY__
	    AMSgObj::BookTimer.start("Track4");
#endif
	    SetLayerOrder(it);
	    
	    // Scan ladder combination recursively
	    ScanRecursive(0, it);
#ifndef __ROOTSHAREDLIBRARY__
	    AMSgObj::BookTimer.stop("Track4");
#endif
	    // Return 1 if track has been found
	    found=(!CpuTimeUp() && _itcand.nlayer > 0);
	  }
	llexit:;
	}
	if(found){
	  found = BuildATrTrack(_itcand);
	  
	  if (found ) ntrack += found;
	}
      }
    }
    

  } while (!CpuTimeUp() && found && ntrack < RecPar.MaxNtrack);

  _CpuTime = _CheckTimer();

//PurgeGhostHits should be called at event_tk
//PurgeGhostHits();
#ifndef __ROOTSHAREDLIBRARY__
   AMSgObj::BookTimer.stop("Track2");
#endif
  return ntrack;
}

void TrRecon::PurgeGhostHits()
{
  VCon* contT = GetVCon()->GetCont("AMSTrTrack");
  if(!contT) return;

  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if(!cont) return;

/*
  int nhit = cont->getnelem();
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);
    if (!hit || !hit->OnlyY() || hit->Used()) continue;
    TrClusterR *cls = hit->GetYCluster();
    if (cls->GetSeedSN() < RecPar.TrackThrSeed[1]) {
      if (i == 0) cont->removeEl(0);
      else  	  cont->removeEl(cont->getelem(i-1));
      nhit--;
      i--;
    }
  }
*/

  int ntrack=contT->getnelem();
  for(int itr=0;itr<ntrack;itr++){
    TrTrackR *track = (TrTrackR*)contT->getelem(itr);
    for(int ithit=0;ithit<track->GetNhits();ithit++){
      TrRecHitR *thit=track->GetHit(ithit);
      TrClusterR* yclus= thit->GetYCluster();
      int nhit = cont->getnelem();
      for (int i = 0; i < nhit; i++) {
	TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);
	if (!hit || hit->Used()) continue;
	if(hit==thit) continue;
	TrClusterR* yclus2= hit->GetYCluster();
	if(yclus2==yclus){
	  // delete hit;
	  if (i == 0)
	    cont->removeEl(0);
	  else
	    cont->removeEl(cont->getelem(i-1));
	  nhit--;
	  i--;
	}
      }
    }
  }
  delete cont;

  // create hits indexes
  for (int i = 0; i < ntrack; i++) {
    TrTrackR *track = (TrTrackR*)contT->getelem(i);
    track->BuildHitsIndex();
  }
  delete contT;

}

void TrRecon::PurgeUnusedHits()
{
  VCon* cont1 = GetVCon()->GetCont("AMSTrTrack");
  if (!cont1) return;

  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return;

  VCon *cont3 = GetVCon()->GetCont("AMSTrCluster");
  if (!cont3) return;

  // Clear used status
  int nhit = cont2->getnelem();
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont2->getelem(i);
    if (hit) hit->ClearUsed();
  }

  enum { NT = 10, NL = trconst::maxlay };
  int ihit[NT*NL], icls[NT*NL*2];
  for (int i = 0; i < NT*NL; i++) ihit[i] = icls[i*2] = icls[i*2+1] = -1;

  // Set used status
  int ntrack = cont1->getnelem();
  for (int i = 0; i < ntrack; i++) {
    TrTrackR *track = (TrTrackR*)cont1->getelem(i);
    for (int j = 0; j < track->GetNhits(); j++) {
      TrRecHitR *hit = track->GetHit(j);
      hit->SetUsed();
      ihit[ i*NL+j]      = track->iTrRecHit(j);
      icls[(i*NL+j)*2  ] = hit->iTrCluster('x');
      icls[(i*NL+j)*2+1] = hit->iTrCluster('y');
    }
  }

  // Purge hits
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont2->getelem(i);
    if (hit && !hit->Used()) {
      if (i == 0) cont2->removeEl(0);
      else  	  cont2->removeEl(cont2->getelem(i-1));
      for (int j = 0; j < NT*NL; j++) if (ihit[j] > i) ihit[j]--;
      nhit--;
      i--;
    }
  }

  // Purge clusters
  int ncls = cont3->getnelem();
  for (int i = 0; i < ncls; i++) {
    TrClusterR *cls = (TrClusterR*)cont3->getelem(i);
    if (cls && !cls->Used()) {
      if (i == 0) cont3->removeEl(0);
      else  	  cont3->removeEl(cont3->getelem(i-1));
      for (int j = 0; j < NT*NL*2; j++) if (icls[j] > i) icls[j]--;
      ncls--;
      i--;
    }
  }

  // create hits indexes
  for (int i = 0; i < ntrack; i++) {
    TrTrackR *track = (TrTrackR*)cont1->getelem(i);
    track->SetHitsIndex(&ihit[i*NL]);
    //track->BuildHitsIndex();

    for (int j = 0; j < track->GetNhits(); j++) {
      TrRecHitR *hit = track->GetHit(j);
      hit->SetiTrCluster(icls[(i*NL+j)*2], icls[(i*NL+j)*2+1]);
    }
  }

  delete cont1;
  delete cont2;
  delete cont3;
}


bool TrRecon::PreScan(int nlay, TrHitIter &it) const
{
  if (nlay == 0) return true;

  // Obtain linear interpolation parameters
  if (!MagFieldOn() || it.side == 0) {
    if (nlay == 1) { 
      AMSPoint pos0 = it.coo[it.ilay[0]];
      AMSPoint pos1 = it.coo[it.ilay[1]];
      it.param[2] = it.param[3] = 0;
      it.param[1] = (pos1[it.side]-pos0[it.side])/(pos1[2]-pos0[2]);
      it.param[0] =  pos0[it.side]-it.param[1]*pos0[2];
      return true;
    }
  }

  // Obtain circular (Pol2) interpolation parameters
  else {
    if (nlay == 1) return true;
    if (nlay == 2) {
      AMSPoint pos[3] 
        = { it.coo[it.ilay[0]], it.coo[it.ilay[1]], it.coo[it.ilay[2]] };
      double mtx[3][3], vec[3];
      for (int i = 0; i < 3; i++) {
        mtx[i][0] = 1;
        mtx[i][1] = pos[i][2];
        mtx[i][2] = pos[i][2]*pos[i][2];
        vec[i]    = pos[i][it.side];
      }
      TrFit::Inv33(mtx);
      for (int i = 0; i < 3; i++) {
        it.param[i] = 0;
        for (int j = 0; j < 3; j++) it.param[i] += mtx[i][j]*vec[j];
      }
      it.param[3] = 0;
      return true;
    }
  }

  // Check interpolation
  double pz   = it.coo[it.ilay[nlay]][2];
  double pc   = it.coo[it.ilay[nlay]][it.side];
  double intp = it.param[0]+it.param[1]*pz+it.param[2]*pz*pz;
  if(!isnormal(intp)) intp=0;
  //PZ Bug fix if put on the return line is converted to an integer
  //and often fail the comparison.
  double ddiff=fabs(pc-intp);
  TR_DEBUG_CODE_04;
  return ( ddiff< it.psrange);
}

int TrRecon::SetLayerOrder(TrHitIter &it) const
{
  // Fill layer index to be scanned according to hit pattern
  int nlay = 0;
  for (int i = 0; i < it.nlayer; i++) {
    int tkid  = it.tkid[i];
    int layer = std::abs(tkid)/100;
    if (!(patt->TestHitPatternMask(it.pattern, layer))) {
      it.ilay[nlay++] = i;

      // Check number of hits if tkid is specified
      if (tkid%100 != 0) {
        TR_DEBUG_CODE_05;
        int nhity = GetnTrRecHits(tkid, 1);
        if (nhity <= 0) return 0;

        int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;
        if (it.side == 0 && nhitx <= 0) return 0;
      }
    }
  }
  // Disable the other layers
  for (int i = nlay; i < it.nlayer; i++) it.ilay[i] = -1;

  // Swap 2nd and last layer
  int swp = it.ilay[1]; 
            it.ilay[1] = it.ilay[nlay-1]; 
                         it.ilay[nlay-1] = swp;

  // swap 3rd and middle layer for the bending side
    if (it.side == 1 && MagFieldOn()) {
    swp = it.ilay[2]; 
          it.ilay[2] = it.ilay[nlay/2]; 
                       it.ilay[nlay/2] = swp;
  }

  return 1;
}

int TrRecon::ScanRecursive(int idx, TrHitIter &it)
{
  if (RecPar.TrTimeLim > 0 && _CheckTimer() > RecPar.TrTimeLim) {
    _CpuTimeUp = true;
#ifndef __ROOTSHAREDLIBRARY__
    RecPar.NcutCpu++;
    char mex[255];
    sprintf(mex,"TrRecon::BuildTrTracks Cpulimit(%f) Exceeded", RecPar.TrTimeLim );
    throw AMSTrTrackError(mex);
#endif
  }

  if (CpuTimeUp()) return 0;
//#ifndef __ROOTSHAREDLIBRARY__
//	AMSgObj::BookTimer.start("Track3");
//#endif

  // Evaluate current candidates if idx has reached to the end
  if (idx == it.nlayer) return (it.mode == 1) ? LadderScanEval(it)
                                              : HitScanEval   (it);
//#ifndef __ROOTSHAREDLIBRARY__
//	AMSgObj::BookTimer.stop("Track3");
	//	hman.Fill("Time",AMSgObj::BookTimer.Get("Track3"),ntrrechit);
//#endif

  // Skip if current layer is disabled
  int il = it.ilay[idx];
  if (il < 0) return ScanRecursive(idx+1, it);

  // Loop on hit candidates in the current layer
  int nscan = (it.mode == 1) ? LadderCoordMgr(idx, it, 1)
                             : HitCoordMgr   (idx, it, 1);
  for (it.Iscan(il) = 0; it.Iscan(il) < nscan; it.Iscan(il)++) {

    // Loop on multiplicities
    int mlast = (it.mode == 1) ? LadderCoordMgr(idx, it, 2)
                               : HitCoordMgr   (idx, it, 2);
    for (; it.imult[il] <= mlast; it.imult[il]++) {
      // Set coordinates
      if (it.mode == 1 && LadderCoordMgr(idx, it, 3) < 0) continue;
      if (it.mode == 2 && HitCoordMgr   (idx, it, 3) < 0) continue;

      // Pre selection
      if (!PreScan(idx, it)) continue;

      // Go to the next layer
      ScanRecursive(idx+1, it);
   }
    if (it.imult[il] > 0) it.imult[il]--;
  }
  return 0;
}

// PZ Comment out to speed up the code
// int TrRecon::ScanLadders(int pattern)
// {
//   // Clear the-best-candidate parameters
//   _itcand.nlayer = 0; _itcand.nhitc = _MinNhitXY; 
//   _itcand.chisq[0] = _itcand.chisq[1] = RecPar.MaxChisqAllowed;

//   // Define and fill iterator
//   TrHitIter it;
//   it.mode = 1; it.pattern = pattern; 
//   it.side = 1; it.psrange = RecPar.LadderScanRange;

//   // Loop on layers to check for an empty layer and fill iterator
//   int nhitc = it.nlayer = 0;
//   for (int i = 0; i < patt->GetSCANLAY(); i++) {
//     if (!(patt->TestHitPatternMask(pattern, i+1))) {
//       if (_LadderHitMap[i].empty()) return 0;
//       if (_NladderXY[i]) nhitc++;
//       it.tkid[it.nlayer++] = (i+1)*100;
//     }
//   }
//   if (it.nlayer < _MinNhitY || nhitc < _MinNhitXY) return 0;
//   TR_DEBUG_CODE_11;

//   // Set the order of scanning layers
//   SetLayerOrder(it);
// #ifndef __ROOTSHAREDLIBRARY__
// 	  AMSgObj::BookTimer.start("Track4");
// #endif

//   // Scan ladder combination recursively
//   ScanRecursive(0, it);
// #ifndef __ROOTSHAREDLIBRARY__
// 	  AMSgObj::BookTimer.stop("Track4");
// #endif
//   // Return 1 if track has been found
//   return (!CpuTimeUp() && _itcand.nlayer > 0);
// }

int TrRecon::LadderCoordMgr(int idx, TrHitIter &it, int mode) const
{
  // Check layer index
  int il = it.ilay[idx];
  if (il < 0) return 0;

  // Layer number
  int lyr = std::abs(it.tkid[il])/100;

  // Mode 1: Return number of ladder candidates to be scanned
  if (mode == 1) return _LadderHitMap[lyr-1].size();

  // Mode 2: Set multiplicity range
  if (mode == 2) {
    it.imult[il] = 0;
    return 0;
  }

  // Mode 3: Fill coordinates to be evaluated
  it.tkid[il] = _LadderHitMap[lyr-1].at(it.Iscan(il));
  int layer = std::abs(it.tkid[il])/100;
  it.coo[il] = AMSPoint(TkCoo::GetLadderCenterX(it.tkid[il]),
                        TkCoo::GetLadderCenterY(it.tkid[il]),
                        TkDBc::Head->GetZlayer(layer));
  return 1;
}

int TrRecon::LadderScanEval(TrHitIter &it)
{
  // Check CPU time limit
  if (CpuTimeUp()) return 0;

  // Check number of hits with both-side clusters
  int nhitc = 0;
  for (int i = 0; i < it.nlayer; i++) {
    int nhity = GetnTrRecHits(it.tkid[i], 1);
    if (nhity <= 0) continue;
    int nhitx = (GetnTrRecHits(it.tkid[i])-nhity)/nhity;
    if (nhitx > 0) nhitc++;
  }
  if (nhitc < _itcand.nhitc) return 0; 

  // Obtain a track with linear/circle fitting
  TrFit fit;
  for (int i = 0; i < it.nlayer; i++)
    fit.Add(it.coo[i], 0, RecPar.LadderScanRange*0.75, 1);

  if (MagFieldOn())
    fit.CircleFit(2);
  else
    fit.LinearFit(2);
  
  // Check chisquare
  if (fit.GetChisqY() < 0 ||
      fit.GetChisqY() > RecPar.MaxChisqForLScan) return 0;

  // Check if all the ladder positions are within the range
  for (int i = 0; i < fit.GetNhit(); i++)
    if (std::abs(fit.GetYr(i)) > RecPar.LadderScanRange*0.75) return 0;
  TR_DEBUG_CODE_21;

  // Scan hits among the current ladder combination
  _itchit = it;
  _itchit.nhitc = _itcand.nhitc;
  if (!ScanHits(it)) return 0;
  TR_DEBUG_CODE_22;

  // Replace the best candidate if better one found
  if (_itchit.nhitc > _itcand.nhitc ||
      _itchit.chisq[0]+_itchit.chisq[1] < _itcand.chisq[0]+_itcand.chisq[1])
    _itcand = _itchit;
	


  return 1;
}

int TrRecon::ScanHits(const TrHitIter &itlad)
{
  // Reset the-best-candidate parameters
  _itchit.chisq[0] = _itchit.chisq[1] = RecPar.MaxChisqAllowed;
  for (int i = 0; i < SCANLAY_MAX; i++)
    _itchit.iscan[i][0] = _itchit.iscan[i][1] = _itchit.imult[i] = 0;

  // Define and fill iterator
  TrHitIter it = itlad;

  it.mode = 2;
  it.chisq[0] = it.chisq[1] = RecPar.MaxChisqAllowed;
  it.psrange = RecPar.ClusterScanRange;
  for (int i = 0; i < it.nlayer; i++) 
    it.iscan[i][0] = it.iscan[i][1] = it.imult[i] = 0;

  // Loop on X and Y
  for (it.side = 0; it.side <= 1; it.side++) {
    int maxpat = (it.side == 0) ? patt->GetHitPatternFirst(1+patt->GetSCANLAY()-_MinNhitX) 
                                : itlad.pattern+1;
    _itchit.side = it.side;

    int lmask = patt->GetHitPatternMask(itlad.pattern);

    // Loop on each pattern
    int found = 0;
    for (it.pattern = itlad.pattern; it.pattern < maxpat; it.pattern++) {
      if ((patt->GetHitPatternMask(it.pattern)&lmask) != lmask) continue;

      // Set the order of scanning layers
      if (!SetLayerOrder(it)) continue;

      // Hit scan
      TR_DEBUG_CODE_30;
      ScanRecursive(0, it);

      // Check chisquare
      if (found = (_itchit.chisq[it.side] < RecPar.MaxChisqAllowed)) break;
    }
    if (!found) return 0;

    if (it.side == 0) _itchit.nhitc = patt->GetSCANLAY()-patt->GetHitPatternNmask(it.pattern);
    it = _itchit;
  }
  return 1;
}

int TrRecon::HitCoordMgr(int idx, TrHitIter &it, int mode) const
{
  // Check layer index
  int il = it.ilay[idx];
  if (il < 0) return 0;

  // Get TkId
  int tkid = it.tkid[il];

  // Mode 1: Return number of candidates to be scanned
  if (mode == 1) {
    int nhity = GetnTrRecHits(tkid, 1);
    if (it.side == 1 || nhity <= 0) return nhity;
    return (GetnTrRecHits(tkid)-nhity)/nhity;
  }

  // Mode 2: Set multiplicity range
  if (mode == 2) {
    if (it.side == 1) return it.imult[il];

    TrRecHitR *hit = GetTrRecHit(tkid, it.iscan[il][0], it.iscan[il][1]);
    if (!hit) return it.imult[il];

    if (it.side == 0 && hit->OnlyY()) return -1;

    // Return multiplicity range
    it.imult[il] = 0;
    return hit->GetMultiplicity()-1;
  }

  // Mode 3: Set coordinates to be evaluated
  TrRecHitR *hit = GetTrRecHit(tkid, it.iscan[il][0], it.iscan[il][1]);
  if (!hit) return -1;

  if (hit->OnlyY())
    it.coo[il][1] = hit->GetCoord(it.imult[il])[1];
  else
    it.coo[il] = hit->GetCoord(it.imult[il]);
  return 1;
}

int TrRecon::HitScanEval(const TrHitIter &it)
{
  // Evaluate the track candidate
  TrFit fit;
  for (int i = 0; i < it.nlayer && it.ilay[i] >= 0; i++) {
    int j = (it.side == 0) ? it.ilay[i] : i;
    float bf[3]={0,0,0};
    float pp[3];
    pp[0]=it.coo[j].x();pp[1]=it.coo[j].y();pp[2]=it.coo[j].z();
    GUFLD(pp, bf);
    fit.Add(it.coo[j], RecPar.ErrXForScan, RecPar.ErrYForScan, 1,bf[0],bf[1],bf[2]);
  }

  // Select fitting method
  if (it.side == 0) {
    if (MagFieldOn())
      fit.PolynomialFit(1, 2);
    else
      fit.LinearFit(1);
  }
  else {
    if (MagFieldOn())
      fit.SimpleFit();
    else
      fit.LinearFit(2);
  }

  // Check chisquare
  double csq = (it.side == 0) ? fit.GetChisqX() : fit.GetChisqY();
  TR_DEBUG_CODE_31;
  if (csq < 0 || csq > _itchit.chisq[it.side]) return 0;

  TrHitIter ittmp = it;

  // Fill dummy multiplicity and coordinates for YONLY hits
  if (ittmp.side == 0) {
    int ndim = 1;
    if (MagFieldOn()) {
      if      (fit.GetNhit() >= 4) ndim = 3;
      else if (fit.GetNhit() >= 3) ndim = 2;
      if (ndim > 2) fit.PolynomialFit(1, ndim);
    }
    for (int i = 0; i < 4; i++)
      ittmp.param[i] = (i < ndim+1) ? fit.GetParam(i) : 0;

    for (int i = 0; i < ittmp.nlayer; i++) {
      int layer = std::abs(ittmp.tkid[i])/100;
      if (patt->TestHitPatternMask(ittmp.pattern, layer)) {
        if (EstimateXCoord(i, ittmp) < 0) return 0;
        double zl = TkDBc::Head->GetZlayer(layer);
        ittmp.coo[i][2] = zl;
        ittmp.coo[i][0] = ittmp.param[0]+ittmp.param[1]*zl
                         +ittmp.param[2]*zl*zl+ittmp.param[3]*zl*zl*zl;
        ittmp.iscan[i][0] = GetnTrRecHits(ittmp.tkid[i], 0)-1;
      }
    }
  }

  // Keep XZ fitting parameters
  if (ittmp.side == 1)
    for (int i = 0; i < 4; i++) ittmp.param[i] = _itchit.param[i];

  // Replace candidate iterator
  _itchit = ittmp;
  _itchit.chisq[ittmp.side] = csq;
  TR_DEBUG_CODE_32;

  return 1;
}

TkSens TrRecon::EstimateXCoord(AMSPoint gcoo, int tkid) const
{
  TkSens tks(tkid, gcoo,0);
  if (tks.LadFound() && tks.GetStripX() < 0) {
    float sx = tks.GetSensCoo().x();
    float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->rot.GetEl(0, 0);
    if (sx < 0) gcoo[0] -= sg*(sx-1e-3);
    if (sx > TkDBc::Head->_ssize_active[0])
      gcoo[0] -= sg*(sx-TkDBc::Head->_ssize_active[0]+1e-3);
    tks.SetGlobalCoo(gcoo);
  }
  return tks;
}

int TrRecon::EstimateXCoord(int il, TrHitIter &it) const
{
  int tkid  = it.tkid[il];
  int layer = std::abs(tkid)/100;
  float ln  = TkCoo::GetLadderLength (tkid);
  float lx  = TkCoo::GetLadderCenterX(tkid);
  float ly  = TkCoo::GetLadderCenterY(tkid);
  float lz  = TkDBc::Head->GetZlayer(layer);

  // Interpolated position
  float px = it.param[0]+it.param[1]*lz
            +it.param[2]*lz*lz+it.param[3]*lz*lz*lz;

  // Check if track is inside the ladder
  if (std::abs(px-lx) > ln/2) return -1;

  // Estimate sensor number and strip position
  AMSPoint gcoo(px, ly, lz);
  TkSens tks = EstimateXCoord(gcoo, tkid);
  if (!tks.LadFound() || tks.GetLadTkID() != tkid) return -1;
  if (tks.GetMultIndex() < 0 ||
      tks.GetMultIndex() >= 
      TkCoo::GetMaxMult(tkid,tks.GetStripX()+640)) return -1;

  it.imult[il] = tks.GetMultIndex();
  TR_DEBUG_CODE_40;

  return tks.GetStripX();
}

int TrRecon::MergeLowSNHits(TrTrackR *track, int mfit)
{
  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return -1;

  double rthdx = TRFITFFKEY.ErrX*RecPar.NsigmaMerge;
  double rthdy = TRFITFFKEY.ErrY*RecPar.NsigmaMerge;

  AMSPoint pltrk[SCANLAY_MAX];
  double   rymin[SCANLAY_MAX];
  int      ncmin[SCANLAY_MAX];
  for (int i = 0; i < SCANLAY_MAX; i++) {
    pltrk[i] = track->GetPlayer(i, mfit);
    rymin[i] = rthdy*1.5;
    ncmin[i] = 0;
  }

  // Merge low seed SN clusters
  int nhit = cont->getnelem(), nadd = 0;
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);

    bool thxy = !(patt->TestHitPatternMask(track->GetPatternXY(), 
					   hit->GetLayer()));
    if (!hit || hit->Used() || thxy || 
	(hit->OnlyY() && track->TestHitBits(hit->GetLayer(), mfit))) continue;

    TrClusterR *clx = hit->GetXCluster();
    TrClusterR *cly = hit->GetYCluster();
    if (!cly) continue;
    if  (clx && clx->GetSeedSN() >= RecPar.TrackThrSeed[0] && 
	 cly && cly->GetSeedSN() >= RecPar.TrackThrSeed[1]) continue;
    if (!clx && cly->GetSeedSN() >= RecPar.TrackThrSeed[1]) continue;

    int ily = hit->GetLayer()-1;
    AMSPoint ptrk = pltrk[ily];
    double resy = std::fabs(hit->GetCoord().y()-ptrk.y());
    int nc = ((clx) ? 1 : 0)+((cly) ? 1 : 0);

    if (resy > rymin[ily] || nc < ncmin[ily]) continue;
    rymin[ily] = resy;
    ncmin[ily] = nc;

    int imult = -1;
    if (clx) {
      int    jmin = -1;
      double dmin = rthdx+rthdy;
      for (int j = 0; j < hit->GetMultiplicity(); j++)
	if (hit->GetCoord(j).dist(ptrk) < dmin) {
	  jmin = j;
	  dmin = hit->GetCoord(j).dist(ptrk);
	}
      if (jmin < 0) continue;
      if (std::fabs(hit->GetCoord(jmin).x()-ptrk.x()) > rthdx ||
	  std::fabs(hit->GetCoord(jmin).y()-ptrk.y()) > rthdy) continue;
      imult = jmin;
    }
    else {
      TkSens tks = EstimateXCoord(ptrk);
      if (!tks.LadFound() || tks.GetLadTkID() != hit->GetTkId()) continue;
      imult = tks.GetMultIndex();
      if (imult < 0) imult = 0;
      if (imult >= hit->GetMultiplicity()) imult = hit->GetMultiplicity()-1;

      if (std::fabs(hit->GetCoord(imult).y()-ptrk.y()) > rthdy) continue;
      hit->SetDummyX(tks.GetStripX());
    }
    hit->SetResolvedMultiplicity(imult);
    track->AddHit(hit, imult);
    nadd++;
  }
  delete cont;

  return nadd;
}

int TrRecon::MergeExtHits(TrTrackR *track, int mfit)
{
  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return -1;
  int lyext[2] = {8, 9};

  class ctest {
  public:
    int    ihmin;
    int    icmin;
    int    ncmin;
    int    mlmin;
    AMSPoint diff;
    double rxmin;
    double rymin;
    ctest(){
      ihmin = -1;
      icmin =  0;
      ncmin =  0;
      mlmin =  0;
      rxmin = 10;
      rymin = 10;
    }
  };
  ctest DXY[2];
  ctest DY[2];
  AMSPoint ptrk[2];
  ptrk[0] = track->InterpolateLayer(lyext[0]-1, mfit);
  ptrk[1] = track->InterpolateLayer(lyext[1]-1, mfit);

  int nhit = cont->getnelem();

  //1. Search the XY and Y-only hits closest to the track extrapolation on Y
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);
    if (!hit || hit->OnlyX() || hit->Used()) continue;
    int idx=0;
    if(hit->OnlyY())idx=1;

    int il = -1;
    if (hit->GetLayer() == lyext[0]) il = 0;
    if (hit->GetLayer() == lyext[1]) il = 1;
    if (il < 0) continue;
    int mult;
    AMSPoint DD = hit->HitPointDist(ptrk[il],mult);
    float dxy=sqrt(DD[0]*DD[0]+DD[1]*DD[1]);
    float dy=fabs(DD[1]);
    
    if(hit->OnlyY()){
      if (dy < DY[il].rymin) {
	DY[il].ihmin = i;
	DY[il].icmin = hit->GetYClusterIndex();
	DY[il].rymin = dy;
	DY[il].ncmin = 1;
	DY[il].diff  = DD;
      }
    }else{
      if (dxy < DXY[il].rymin) {
	DXY[il].ihmin = i;
	DXY[il].icmin = hit->GetYClusterIndex();
	DXY[il].rymin = dxy;
	DXY[il].ncmin = 1;
	DXY[il].mlmin = mult;
	DXY[il].diff  = DD;
      }
    }
  }
  
  //2. XY or Y
  float diffY_max=TRFITFFKEY.MergeExtLimY;
  float diffX_max=TRFITFFKEY.MergeExtLimX;
  int nadd=0;

  for (int il=0;il<2;il++){
    if (DY[il].ihmin < 0) continue;

    if(DXY[il].icmin != DY[il].icmin){
       static int mprint=100; 
      if(mprint++<100)printf("TrRecon::MergeExtHits -W- XY and Y cluster do not share the Y cluster!!!!!\n");
     }
    if(il==0){
      if (DXY[il].ihmin>=0)
	hman.Fill("1N_XY",DXY[il].diff.x(),DXY[il].diff.y());
      hman.Fill("1N_Y",DY[il].diff.x(),DY[il].diff.y());
    }
    else{
      if (DXY[il].ihmin>=0)
	hman.Fill("P6_XY",DXY[il].diff.x(),DXY[il].diff.y());
      hman.Fill("P6_Y",DY[il].diff.x(),DY[il].diff.y());
    }
    if (DXY[il].ihmin>=0 && fabs(DXY[il].diff.y()) > diffY_max) continue;
    if (DXY[il].ihmin< 0 && fabs(DY [il].diff.y()) > diffY_max) continue;
    TrRecHitR *hit = 0;

    if (DXY[il].ihmin>=0 &&
	fabs(DXY[il].diff.x()) < diffX_max)
      hit=(TrRecHitR*)cont->getelem(DXY[il].ihmin);
    else
      hit=(TrRecHitR*)cont->getelem(DY[il].ihmin);

    if (hit) {
      if (hit->OnlyY()) {
	TkSens tks = EstimateXCoord(ptrk[il]);
	if (!tks.LadFound() || tks.GetLadTkID() != hit->GetTkId()) continue;
	DY[il].mlmin = tks.GetMultIndex();
	int nmlt = hit->GetMultiplicity();
	if (DY[il].mlmin >= nmlt) DY[il].mlmin = nmlt-1;
	if (DY[il].mlmin <     0) DY[il].mlmin = 0;
	hit->SetDummyX(tks.GetStripX());
	hit->SetResolvedMultiplicity(DY[il].mlmin);
      }
      else
	hit->SetResolvedMultiplicity(DXY[il].mlmin);
      track->AddHit(hit);
      hit->SetUsed();
      nadd++;
    }
  }
  delete cont;

  if (nadd > 0) track->FillExRes();

  if(DY[0].ihmin>=0) {
    if (track->DoAdvancedFit(TrTrackR::kFitLayer8))
      track->Settrdefaultfit(TrTrackR::kFitLayer8 | mfit);
  }
  if(DY[1].ihmin>=0) {
    if (track->DoAdvancedFit(TrTrackR::kFitLayer9))
      track->Settrdefaultfit(TrTrackR::kFitLayer9 | mfit);
  }
  if(DY[0].ihmin>=0 && DY[1].ihmin>=0) {
    if (track->DoAdvancedFit(TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9))
      track->Settrdefaultfit(TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9 | mfit);
  }

  else if ( track->ParExists(mfit | TrTrackR::kFitLayer8) &&
	    track->ParExists(mfit | TrTrackR::kFitLayer9) &&
	   !track->ParExists(mfit | TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9)) {
    if (track->DoAdvancedFit(TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9))
      track->Settrdefaultfit(TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9 | mfit);
  }

  return nadd;
}

int TrRecon::BuildATrTrack(TrHitIter &itcand)
{
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("Trackbuild"); 
#endif
  if (itcand.nlayer == 0) return 0;

  //PZDEBUG
 if (PZDEBUG){
  cout<<" Bulding a track with points\n";
  for (int jj=0;jj<itcand.nlayer;jj++){
    cout<<"tkid "<<itcand.tkid[jj]<<" mult "<<
      itcand.imult[jj]<<" point "<<itcand.coo[jj]<<endl;
  }
 }

  // Create a new track
#ifndef __ROOTSHAREDLIBRARY__
  AMSTrTrack *track = new AMSTrTrack(itcand.pattern);
#else
  TrTrackR *track = new TrTrackR(itcand.pattern);
#endif
  // Loop for each layer
  int maskc = patt->GetHitPatternMask(itcand.pattern);
  int masky = patt->GetHitPatternMask(itcand.pattern);

  for (int i = 0; i < itcand.nlayer; i++) {
    // Get clusters from candidate map
    TrRecHitR *hit = GetTrRecHit(itcand.tkid[i], itcand.iscan[i][0], 
                                                 itcand.iscan[i][1]);
    if (!hit) continue;

    if (hit->OnlyY()) {
      maskc |= (1 << (patt->GetSCANLAY()-hit->GetLayer()));
      int dmx = EstimateXCoord(i, itcand);
      if (dmx < 0) {
	masky |= (1 << (patt->GetSCANLAY()-hit->GetLayer()));
	continue;
      }

      hit->SetDummyX(dmx);
      //hit->BuildCoordinates();
      track->setstatus(AMSDBc::FalseX); // AMSDBc::FalseX = 8192; (0x2000)
    }
    hit->SetResolvedMultiplicity(itcand.imult[i]);
    track->AddHit(hit, itcand.imult[i]);
  }

 if (PZDEBUG){
  cout<< " PZDEBUG track info\n";
  for(int jj=0;jj<track->GetNhits();jj++){
    cout<<" Track hit num "<<jj<<endl;
    track->GetHit(jj)->Print();
  }
 }
  // Fill patterns
  track->SetPatterns(patt->GetHitPatternIndex(maskc),
		     patt->GetHitPatternIndex(masky),
                     patt->GetHitPatternIndex(maskc),
		     patt->GetHitPatternIndex(masky));

  // 1st. step Fit
  int mfit1 = (MagFieldOn()) ? TrTrackR::kChoutko : TrTrackR::kLinear;
  float ret = track->FitT(mfit1);
  if (ret < 0 || 
      track->GetChisqX(mfit1) <= 0 || track->GetChisqY(mfit1) <= 0 ||
      track->GetNdofX (mfit1) <= 0 || track->GetNdofY (mfit1) <= 0) {
    delete track;
    return 0;
  }
  track->Settrdefaultfit(mfit1);

  // Merge low seed SN hits
  if (RecPar.TrackThrSeed[0] > RecPar.ThrSeed[0][0] || 
      RecPar.TrackThrSeed[1] > RecPar.ThrSeed[1][0]) {
    MergeLowSNHits(track, mfit1);
    if (track->GetNhitsX () < RecPar.MinNhitX ||
	track->GetNhitsY () < RecPar.MinNhitY ||
	track->GetNhitsXY() < RecPar.MinNhitXY) {
      delete track;
      return 0;
    }
  }

  // Get interpolated posisions/dirs
  double zhit[trconst::maxlay];
  AMSDir dtrk[trconst::maxlay];
  for (int i = 0; i < track->GetNhits(); i++)
    zhit[i] = track->GetHit(i)->GetCoord().z();
  track->Interpolate(track->GetNhits(), zhit, 0, dtrk, 0, mfit1);

  // Set cluster angles and re-build coordinates
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR  *hit  = track->GetHit(i);

    // Mark the hit as USED
    hit->SetUsed();

    // Remove hit pointer from _HitsTkIdMap
    RemoveHits(hit);
    TR_DEBUG_CODE_41;

    TrClusterR *xcls = hit->GetXCluster();
    TrClusterR *ycls = hit->GetYCluster();
    if (xcls) { xcls->SetDxDz(dtrk[i].x()/dtrk[i].z());
                xcls->SetDyDz(dtrk[i].y()/dtrk[i].z()); }
    if (ycls) { ycls->SetDxDz(dtrk[i].x()/dtrk[i].z());
                ycls->SetDyDz(dtrk[i].y()/dtrk[i].z()); }
  //  hit->BuildCoordinates();
  }

  // 2nd. step Fit
  if (!MagFieldOn()) {
    track->FitT(mfit1);
    track->Settrdefaultfit(mfit1);
  }
  else {
    track->EstimateDummyX(mfit1);

    int mfit2 = TrTrackR::DefaultFitID;
    track->FitT(mfit2);
    if (track->ParExists(mfit2)) {
      track->Settrdefaultfit(mfit2);
      if (TryDropX(track, mfit2))
	track->EstimateDummyX(mfit2);
    }
  }  

  if (track->GetRigidity() == 0 || track->GetChisq() <= 0) {
#ifndef __ROOTSHAREDLIBRARY__
     AMSgObj::BookTimer.stop("Trackbuild");
#endif
    delete track;
    return 0;
  }

  VCon* cont = GetVCon()->GetCont("AMSTrTrack");
  if (cont) {
    cont->addnext(track);
    TR_DEBUG_CODE_42;
  }
#ifndef __ROOTSHAREDLIBRARY__
     AMSgObj::BookTimer.stop("Trackbuild");
#endif
  delete cont;
  return 1;
}

int TrRecon::TryDropX(TrTrackR *track, int mfit)
{
  double rthd1 = TRFITFFKEY.RthdDropX[0];
  double rthd2 = TRFITFFKEY.RthdDropX[1];
  double rcsq  = track->GetChisqX(mfit)/track->GetChisqY(mfit);

  if (rcsq < rthd1) return 0;

  // Try to drop one hit in case ChisqX is very large
  int    imax[2] = { -1, -1 };
  double rmax[2] = {  0,  0 };
  int nhx = 0;
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR *hit = track->GetHit(i);
    if (!hit || hit->OnlyY()) continue;
    int ily = hit->GetLayer()-1;
    if (track->TestHitBits(ily+1, mfit)) {
      double rx = fabs(track->GetResidual(ily, mfit).x());
      if (imax[0] < 0 || rx > rmax[0]) { 
	imax[1] = imax[0]; imax[0] = i;
	rmax[1] = rmax[0]; rmax[0] = rx; 
      }
      else if (imax[1] < 0 || rx > rmax[1]) { 
	imax[1] = i;
	rmax[1] = rx; 
      }
      nhx++;
    }
  }

  if (nhx <= 4) imax[1] = -1;
  if (nhx == 4) rthd2 = 2;

  for (int i = 0; i < 3; i++) {
    TrRecHitR *hit1 = 0;
    TrRecHitR *hit2 = 0;
    if (i  < 2 && imax[i] >= 0)   hit1 = 
                                  hit2 = track->GetHit(imax[i]);
    if (i == 2 && imax[0] >= 0
	       && imax[1] >= 0) { hit1 = track->GetHit(imax[0]);
                                  hit2 = track->GetHit(imax[1]); }
    if (hit1 && hit2) {
      hit1->setstatus(TrRecHitR::YONLY);
      hit2->setstatus(TrRecHitR::YONLY);

      track->FitT(mfit);
      if (track->GetChisqX(mfit)/
	  track->GetChisqY(mfit)*rthd2 < rcsq) return i+1;

      hit1->clearstatus(TrRecHitR::YONLY);
      hit2->clearstatus(TrRecHitR::YONLY);
    }
  }

  return 0;
}



void TrRecon::InitFFKEYs(int magstat)
{
  //init the datacards
  TKGEOMFFKEY.init();
  TRMCFFKEY.init();
  TRCALIB.init();
  TRALIG.init();
  TRCLFFKEY.init();
  TRFITFFKEY.init();

  MAGSFFKEY.magstat = magstat;

  SetParFromDataCards();
  Init();
}

int TrRecon::ReadMagField(const char *fname, 
			  float fscale, int magstat)
{
  //PZMAG  MAGSFFKEY.BZCorr  = 1;
  MAGSFFKEY.magstat = magstat;

 // PZ BUG FIX   This is the second place where scaling factor is set
  // no control here of the scalaing factor !!!
  //MAGSFFKEY.fscale  = fscale;
//  MagField::GetPtr()->SetScale(fscale);
  if (!MagFieldOn()) TrTrackR::DefaultFitID = TrTrackR::kLinear;

  //PZMAG  
#ifdef __ROOTSHAREDLIBRARY__
  return MagField::GetPtr()->Read(fname);
#else
  return 0;
#endif
}

void TrRecon::GetMagField(float *pos, float *bf)
{

  GUFLD(pos,bf);
  //PZMAG MagField::GetPtr()->GuFld(pos, bf);
}

void TrRecon::GetTkFld(float *pos, float **hxy)
{
  float hh[3][3];
  //PZMAG  MagField::GetPtr()->TkFld(pos, (float(*)[3])hxy);
  TKFLD(pos,hh);
  for (int ii=0;ii<3;ii++)
    for (int jj=0;jj<3;jj++)
      hxy[ii][jj]=hh[jj][ii];
}

//void TrRecon::MagFieldCorr(AMSPoint pp, AMSPoint bc)
//{
//#ifdef __ROOTSHAREDLIBRARY__
//  MagField::GetPtr()->AddBcor(pp, bc);
//#endif
//}

int TrRecon::GetTrackerSize()
{
  int TrackerSize = 2; 
  int CrateSize[8] = {2,2,2,2,2,2,2,2}; 
  int TdrSize[8][24];
  for (int icrate=0; icrate<8; icrate++) 
    for (int itdr=0; itdr<24; itdr++) 
      TdrSize[icrate][itdr] = 2;

  enum { NLAD = 192+16 };

  int LadderSize[2][NLAD]; // defined as the sum of clusters sizes!
  for (int iside=0; iside<2; iside++) 
    for (int iladder=0; iladder<NLAD; iladder++) 
      LadderSize[iside][iladder] = 0;

  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if (!cont) return 0;

  int nraw = cont->getnelem();
  for (int iraw=0; iraw<nraw; iraw++) {
    TrRawClusterR* cluster = (TrRawClusterR*)cont->getelem(iraw);
    int iside = cluster->GetSide();
    int tkid  = cluster->GetTkId();

    TkLadder *ladder = TkDBc::Head->FindTkId(tkid);
    int       entry  = TkDBc::Head->TkId2Entry(tkid);
    if (!ladder || entry < 0 || NLAD <= entry) continue;

    int icrate = ladder->GetCrate();
    int itdr   = ladder->GetTdr();
    int clsize = cluster->GetNelem()+2;
    LadderSize[iside][entry] += clsize;
    TdrSize[icrate][itdr] += clsize;
  }

  delete cont;
 
  // Loop on tdr 
  for (int icrate=0; icrate<8; icrate++)
    for (int itdr=0; itdr<24; itdr++) 
      if (TdrSize[icrate][itdr]>2) CrateSize[icrate] += TdrSize[icrate][itdr];
  
  // Loop on crates
  for (int icrate=0; icrate<8; icrate++)
    TrackerSize += CrateSize[icrate];

  return TrackerSize;
}

////////////////////////////////////////////////////////////////
// -------------------- TAS reconstruction ------------------ //
////////////////////////////////////////////////////////////////

#include "TrTasCluster.h"
#include "TrTasDB.h"
#include "TrTasPar.h"

int TrRecon::TasRecon = 0;

int TrRecon::BuildTrTasClusters(int rebuild)
{
  // Check TrTasDB
  if (!TrTasDB::Head) {
    cerr << "Error in TrRecon::BuildTrTasClusters: TrTasDB not exists"
	 << endl;
    return -1;
  }

  if (TasRecon == 1 && !TrTasPar::Head) {
    if (TasRecon && TRCLFFKEY.TasCurr > 0 && TRCLFFKEY.TasLDDR > 0) {
      int lddr = TrTasDB::Dec2Hex(TRCLFFKEY.TasLDDR);
      if (!TrTasDB::Head->FindPar(TRCLFFKEY.TasCurr, lddr)) {
	cerr << "Error in TrRecon::BuildTrTasClusters: TrTasPar not found " 
	     << Form("Ival= %d LDDR= 0x%04x %04d",
		     TRCLFFKEY.TasCurr, lddr, TRCLFFKEY.TasLDDR)<< endl;
	return -1;
      }

      cout << "TrRecon::BuildTrTasClusters: TrTasPar initialized" << endl;
      TrTasPar::Head->Print();
    }
  }
  if (TasRecon == 1 && !_TasPar) _TasPar = TrTasPar::Head;

  // Scan mode note: run only with single thread
  if (TasRecon == 2 && rebuild == 0) {
    int lddr[8] = { 0x0001, 0x0100, 0x0002, 0x0200,
		    0x0004, 0x0400, 0x0008, 0x0800 };
    int isel = -1;
    int nmax =  0;

    for (int i = 0; i < 8; i++) {
      if ((_TasPar = TrTasDB::Head->FindPar(TRCLFFKEY.TasCurr, lddr[i]))) {
	int ncls = BuildTrTasClusters(2);
	cout << Form("TrRecon::BuildTrTasClusters "
		     "try[%d] LDDR=%04x ncls=%d", i, lddr[i], ncls) << endl;
	if (ncls > nmax) {
	  isel = i;
	  nmax = ncls;
	}
      }
    }
    if (isel < 0) return nmax;

    rebuild = 1;
    _TasPar = TrTasDB::Head->FindPar(TRCLFFKEY.TasCurr, lddr[isel]);

    cout << Form("TrRecon::BuildTrTasClusters "
		 "selected [%d] LDDR=%04x datacard=%04x", 
		 isel, lddr[isel], TrTasDB::Dec2Hex(TRCLFFKEY.TasLDDR))
	 << endl;
  }

  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont) {
    cerr << "TrRecon::BuildTrTasClusters: Cant Find AMSTrRawCluster" << endl;
    return -1;
  }

  if (cont->getnelem() == 0) {
    delete cont;
    return 0;
  }

  // Build a map of TrRawClusters
  TrMap<TrRawClusterR> RawMap;
  for (int i = 0; i < cont->getnelem(); i++) {
    TrRawClusterR *raw = (TrRawClusterR *)cont->getelem(i);
    if (raw) RawMap.Add(raw);
  }
  delete cont;

  cont = GetVCon()->GetCont("AMSTrCluster");
  if (!cont) {
    cerr << "TrRecon::BuildTrTasClusters: Cant Find AMSTrCluster" << endl;
    return -1;
  }
  if (rebuild) cont->eraseC();

  enum { NBUF = TrTasPar::NADR, BUFSIZE = 21 };
  float adcbuf[NBUF][BUFSIZE];
  int   adrmin[NBUF];
  int   adrmax[NBUF];

  int ncls = 0;

  // loop on ladders
  for (TrMap<TrRawClusterR>::TrMapIT lad  = RawMap.LadMap.begin(); 
                                     lad != RawMap.LadMap.end(); lad++) {
    int tkid = lad->first;

    // Check if the ladder is on the Laser column
    int ilad = -1;
    for (int i = 0; i < TrTasPar::NLAD; i++)
      if (tkid == _TasPar->GetTkId(i)) { ilad = i; break; }
    if (ilad < 0) continue;

    // Clear ADC buffer
    for (int i = 0; i < NBUF; i++) {
      adrmin[i] = adrmax[i] = -1;
      for (int j = 0; j < BUFSIZE; j++) adcbuf[i][j] = 0;
    }

    // Fill ADC buffer
    for (int i = 0; i < NBUF; i++) {
      int amin = _TasPar->GetAmin(ilad, i);
      int amax = _TasPar->GetAmax(ilad, i);
      if (amin < 0 || amax < 0) continue;

      for (int j = 0; j < (int)lad->second.size(); j++) {
	TrRawClusterR *raw = lad->second.at(j);

	for (int k = 0; k < raw->GetNelem(); k++) {
	  int adr = raw->GetAddress(k);
	  if (amin <= adr && adr <= amax) {
	    if (adrmin[i] < 0 || adrmin[i] > adr) adrmin[i] = adr;
	    if (adrmax[i] < 0 || adrmax[i] < adr) adrmax[i] = adr;
	    adcbuf[i][adr-amin] = raw->GetSignal(k);
	  }
	}
      }
    }

    // Generate TrTasClusters
    for (int i = 0; i < NBUF; i++) {
      int amin = _TasPar->GetAmin(ilad, i);
      if (amin < 0 || adrmin[i] < amin || adrmax[i] < adrmin[i]) continue;

      int addr = adrmin[i];
      int side = (addr < 640) ? 1 : 0;
      int nelm = adrmax[i]-adrmin[i]+1;
      float *abuf = &adcbuf[i][addr-amin];

#ifndef __ROOTSHAREDLIBRARY__ 
      cont->addnext(new AMSTrTasCluster(tkid, side, addr, nelm, abuf));
#else
      cont->addnext(new TrTasClusterR  (tkid, side, addr, nelm, abuf));
#endif
      ncls++;
    }
  }
  delete cont;

  return ncls;
}

int TrRecon::BuildTrTasHits(int rebuild)
{
  if (!_TasPar) return -1;

  // Get the pointer to the TrCluster container
  VCon *cont = GetVCon()->GetCont("AMSTrCluster");
  if (!cont) {
    cerr << "TrRecon::BuildTrTasHit:  Cant Find AMSTrCluster Container "
             "Reconstruction is Impossible !!!" << endl;
    return -1;
  }
  if (cont->getnelem() == 0) {
    delete cont;
    return 0;
  }

  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) {
    cerr << "TrRecon::BuildTrTasHit:  Cant Find AMSTrRecHit Container "
            "Reconstruction is Impossible !!!" << endl;
    delete cont;
    return -1;
  }
  if (rebuild) cont2->eraseC();

  int nhit = 0;
  for (int i = 0; i < cont->getnelem(); i++) {
    TrClusterR *clx = (TrClusterR *)cont->getelem(i);
    if (!clx || clx->GetSide() != 0) continue;

    int tkid = clx->GetTkId();
    int ilad = _TasPar->GetIlad(tkid);
    if (ilad < 0) continue;

    int ipk = _TasPar->GetIpk(ilad, clx->GetAddress());
    if (ipk < 0) continue;

    TrClusterR *cly = 0;
    for (int j = 0; j < cont->getnelem(); j++) {
      TrClusterR *cls = (TrClusterR *)cont->getelem(j);
      if (!cls || cls->GetTkId() != tkid || cls->GetSide() != 1) continue;

      if (_TasPar->GetIpk(ilad, cls->GetAddress())%2 == ipk%2) {
	cly = cls;
	break;
      }
    }
    if (!cly) continue;

    TrRecHitR *hit;
#ifndef __ROOTSHAREDLIBRARY__
    cont2->addnext((hit = new AMSTrRecHit(tkid, clx, cly, 0, TrRecHitR::TASHIT)));
#else
    cont2->addnext((hit = new TrRecHitR  (tkid, clx, cly, 0, TrRecHitR::TASHIT)));
#endif
    nhit++;
  }
  delete cont;
  delete cont2;

  return nhit;
}

int TrRecon::BuildTrTasTracks(int rebuild)
{
  if (!_TasPar) return -1;

  // Get the pointer to the TrCluster container
  VCon *cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) {
    cerr << "TrRecon::BuildTrTasTracks:  Cant Find AMSTrRecHit Container "
             "Reconstruction is Impossible !!!" << endl;
    return -1;
  }

  if (cont->getnelem() == 0) {
    delete cont;
    return 0;
  }

  VCon* cont2 = GetVCon()->GetCont("AMSTrTrack");
  if (!cont2) {
    cerr << "TrRecon::BuildTrTasTracks:  Cant Find AMSTrTrack Container "
             "Reconstruction is Impossible !!!" << endl;
    delete cont;
    return -1;
  }
  if (rebuild) cont2->eraseC();

  double range = 0.1;  // cm

  int ntrk = 0;
  for (int i = 0; i < TrTasPar::NLAS; i++) {
    TrRecHitR *hits[SCANLAY_MAX];
    int nhit = 0;
    int hpat = 0xff;
    for (int j = 0; j < SCANLAY_MAX; j++) hits[j] = 0;

    for (int j = 0; j < cont->getnelem(); j++) {
      TrRecHitR *hit = (TrRecHitR *)cont->getelem(j);
      if (!hit || !hit->TasHit() || hit->Used()) continue;

      AMSPoint hcoo = hit->GetCoord();
      AMSPoint lcoo = _TasPar->GetLasCoo(i, hit->GetLayer()-1);
      if (hcoo.dist(lcoo) < range) {
	hits[nhit++] = hit;
	hpat &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
      }
    }
    if (nhit < RecPar.MinNhitXY) continue;

    int pattern = -1;
    for (int j = 0; j < patt->GetNHitPatterns(); j++)
      if (hpat == patt->GetHitPatternMask(j)) { pattern = j; break; }

#ifndef __ROOTSHAREDLIBRARY__
    AMSTrTrack *track = new AMSTrTrack(pattern);
#else
    TrTrackR   *track = new TrTrackR  (pattern);
#endif

    for (int j = 0; j < nhit; j++) {
      hits[j]->SetUsed();
      track->AddHit(hits[j]);
    }

    int fit_method = TrTrackR::kLinear;
    track->FitT(fit_method);
    cont2->addnext(track);

    ntrk++;
  }

  delete cont;
  delete cont2;

  return ntrk;
}

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

AMSPoint TrRecon::BasicTkTRDMatch(TrTrackR* ptrack,
				  AMSPoint& trd_pnt0, AMSDir& trd_dir,
				  int fit_id)
{
  /// Check the Match between Tracker and TRD tracks
  /// Returns an AMSPoint with
  /// [0] X distance
  /// [1] Y distance
  /// [2] Cos(angle)
  double zpl=83.5; //Z low of TRD in cm;

  if(!ptrack) return AMSPoint(-1000,-1000,-1000);
  // TRD point and direction at Z=  zpl
  double X_TRD= (zpl-trd_pnt0[2])*trd_dir[0]/trd_dir[2]+trd_pnt0[0];
  double Y_TRD= (zpl-trd_pnt0[2])*trd_dir[1]/trd_dir[2]+trd_pnt0[1];
  AMSPoint trd_pnt(X_TRD,Y_TRD,zpl);

  // Tracker point and direction at Z= zpl
  AMSPoint tk_pnt;
  AMSDir   tk_dir;
  ptrack->Interpolate(zpl, tk_pnt, tk_dir, fit_id); 

  // angle between the tracks
  number c=tk_dir.prod(trd_dir);
  //distance 
  AMSPoint bb=(tk_pnt-trd_pnt);
  number d=bb.norm();
  number X=bb[0];
  number Y=bb[1];
  //   hman.Fill("TkTrd",c,d);
  //   hman.Fill("TkTrdXY",X,Y);
  //   hman.Fill("ntrdhit",ptrd->_Base._NHits,Y);
  //   if(ptrd->_Base._NHits>10)
  //     hman.Fill("Yresvsrig",ptrack->GetRigidity(),Y);
  bb[2]=fabs(c);
  return bb;
  
  //PZ DEBUG  printf(" TRDTK MATCH  cos %f dist %f\n",c,d);
}

bool TrRecon::TkTRDMatch(TrTrackR* ptrack, AMSPoint& trdcoo, AMSDir& trddir)
{
  int mfit = TrTrackR::DefaultFitID;
  if (!ptrack->ParExists(mfit)) mfit = ptrack->Gettrdefaultfit();

  number SearchReg(1);
  number MaxCos(0.95);
  AMSPoint dst = BasicTkTRDMatch(ptrack, trdcoo, trddir, mfit);

  // No match in Y-coo or angle between TrTrack-TRD tracks
  if (fabs(dst[1]) > SearchReg || dst[2] < MaxCos) return false;

  // Good match between TrTrack-TRD tracks; no need to move
  if (fabs(dst[0]) < SearchReg) return true;
//printf("The distance is X: %f Y: %f Angle: %f try to move ..\n",dst[0],dst[1],dst[2]);
  
  return MoveTrTrack(ptrack, trdcoo, trddir,  3.);

}



bool TrRecon::MoveTrTrack(TrTrackR* ptr,AMSPoint& pp, AMSDir& dir, float err){
  // search if the XY hits are laying with the X road
  int good_mult[9][2];
  int ngood=0;
  int nxy=0;
  for(int jj=0;jj<ptr->getnhits();jj++){
    TrRecHitR* phit=ptr->GetHit(jj);
    if(phit->OnlyY()) continue;
    nxy++;
    float X=pp[0]+dir[0]/dir[2]*(phit->GetCoord().z()-pp[2]);
    int mm=-1;
    float max=9999.;
    for(int ii=0;ii<phit->GetMultiplicity();ii++){
      int layer=phit->GetLayer();
      float diff=X- phit->GetCoord(ii).x();
      if (fabs(diff)<max) max=fabs(diff);

      if(fabs(diff) < (err*(1.+(layer-1)*0.017) )) 
	{ mm=ii; break;}
    }
    float base=(err>3)?2:0;
    hman.Fill("trdmatch",max,(mm>-1)?(base+1.):(base+2.));
    if(mm>-1) {
      good_mult[ngood][0]=jj;
      good_mult[ngood++][1]=mm;
    }
  }

  // move the XY hits 
  // and do a linear Fit on X
  if(ngood==nxy && ngood>=3){
    float alpha=0,beta=0,gamma=0,delta=0;
    for (int ii=0;ii<ngood;ii++){
      TrRecHitR* phit=ptr->GetHit(good_mult[ii][0]);
      phit->SetResolvedMultiplicity(good_mult[ii][1]);
      float X=phit->GetCoord().x();
      float Z=phit->GetCoord().z();
      alpha+=Z*Z;
      beta+=Z;
      gamma+=X;
      delta+=X*Z;
    }
    float q=(alpha*gamma-beta*delta)/(ngood*alpha-beta*beta);
    float m=(ngood*delta-beta*gamma)/(ngood*alpha-beta*beta);

    // Fix the X coo of the Y only hits
    for(int jj=0;jj<ptr->getnhits();jj++){
      TrRecHitR* phit=ptr->GetHit(jj);
      if(phit && phit->OnlyY()){
	AMSPoint gcoo=phit->GetCoord();
	gcoo[0]=gcoo[2]*m+q;
	int tkid=phit->GetTkId();
	TkSens tks(tkid, gcoo, 0);
	if(tks.LadFound()){
	  if(tks.GetStripX()!=-1) phit->SetDummyX(tks.GetStripX());
	  else phit->SetDummyX(383);
	  //phit->BuildCoordinates();	  
	  phit->SetResolvedMultiplicity(tks.GetMultIndex());
	}
      }
    }
    //ptr->ReFit();
    //printf("TrRecon-MoveTrTrack-I- Track Moved err: %f !!!\n",err);
    return true;
    
  }
  return false;
}


bool TkTOFMatch(TrTrackR* tr);

void TrRecon::MatchTRDandExtend(){
  VCon* cont = GetVCon()->GetCont("AMSTrTrack");
  for(int ii=0;ii< cont->getnelem();ii++){
    TrTrackR* tr=(TrTrackR*)cont->getelem(ii);
#ifndef __ROOTSHAREDLIBRARY__
    bool TRDdone=false;
    if((TRCLFFKEY.ExtMatch%10)>0){
      for (AMSTRDTrack*  trd=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1)
	     ;trd;trd=trd->next())
	{
	  AMSPoint pp= trd->getCooStr();
	  AMSDir dd=trd->getCooDirStr(); 
	  TRDdone=TkTRDMatch(tr, pp,dd);; 
	  if(TRDdone) 
	    break;
	}
    }
    if(!TRDdone &&(TRCLFFKEY.ExtMatch/10)>0)   TkTOFMatch(tr);
#endif

    if(tr->DoAdvancedFit()) {
      if (TrDEBUG >= 1) printf(" Track Advanced Fits Done!\n");
      if (tr->ParExists(TrTrackR::DefaultFitID))
	tr->Settrdefaultfit(TrTrackR::DefaultFitID);
    } else {
      if (TrDEBUG >= 1) 
	printf(" Problems with Track Advanced Fits: %d %d\n",
	       tr->GetNhits(), tr->GetNhitsXY());
    }

    if(TkDBc::Head->GetSetup()==3) MergeExtHits(tr, tr->Gettrdefaultfit());
  }
  delete cont;
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////
//                  VERTEX                     ////////////
///////////////////////////////////////////////////////////


int TrRecon::BuildVertex(integer refit){
   
  VCon* vtx_ctr=GetVCon()->GetCont("AMSVtx");
  if(!vtx_ctr){
    printf("TrRecon::BuildVertex  Cant Find AMSVtx Container Reconstruction is Impossible !!!\n");
    return -1;
  }   
  if (refit&& vtx_ctr) {
    printf("TrRecon::BuildVertex Cleaning up AMSVtx container, as reuested!\n");
    vtx_ctr->eraseC();
  }
 int maxtr=100; 
  // Go ahead...
  int nfound = 0;
  TrTrackR* ptrack[maxtr];

  // Add up tracks
  int maxtracks=maxtr;
//if (TRFITFFKEY.OnlyGammaVtx) maxtracks=2; // SH TRFITFFKEY is cleaned up
  
  // First pass (only tracks with beta)
  VCon* pctr=GetVCon()->GetCont("AMSTrTrack");
  for (int ii=0;ii<pctr->getnelem();ii++) {
    TrTrackR *ptr = (TrTrackR *)pctr->getelem(ii);
    if (1){//(ptr->checkstatus(AMSDBc::GOLDEN)) {
      ptrack[nfound] = ptr;
      nfound++;
    }
  }
//cout<<"nfound= "<<nfound<<endl;

  int debug=0;
  // Create a vertex
  if (nfound>1) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSVtx  *p= new AMSVtx(nfound, ptrack);
#else
    VertexR *p= new VertexR(nfound, ptrack);
#endif
    if(p->IsFilled()){
      vtx_ctr->addnext(p); 
 //      if (debug) {
// 	p->print();
// 	VCon* betac=GetVCon()->GetCont("AMSBeta");
// 	if(!betac) goto exit;
// 	for (int i=0; i<p->getntracks(); i++) {
// 	  TrTrackR* ptr = p->gettrack(i);
// 	  if (!ptr) continue;
// 	  cout << "AMSVtx: itrack " << i;
// 	  //Beta check
// 	  bool track_has_beta = false;
	  
// 	  for(int patb=0; patb<betac->getnelem(); patb++){
// 	    AMSBeta *pbeta = betac->getelem(i);
// 	    if (pbeta->getptrack()==ptr) {
// 	      track_has_beta = true;
// 	      goto exit_betaprint;
// 	    }
// 	  }
	  
// 	exit_betaprint:
	  
// 	 
// 	  cout << ", beta " << track_has_beta;
// 	  //
// 	  cout << ", PI Chi2 " << ptr->getpichi2();
// 	  cout << ", PI Rigidity " << ptr->GetRigidity();
// 	  cout << ", WEAK bit " << ptr->checkstatus(AMSDBc::WEAK);
// 	  cout << ", FalseX bit " << ptr->checkstatus(AMSDBc::FalseX);
// 	  cout << ", FalseTOFX bit " << ptr->checkstatus(AMSDBc::FalseTOFX);
// 	  cout << endl;
// 	  ptr->_printEl(cout);
// 	  for (int i=0;i<ptr->GetNhits();i++){
// 	    cout << "        " << ptr->GetHit(i)->GetCoord()[0];
// 	    cout << ", " << ptr->GetHit(i)->GetCoord()[1];
// 	    cout << ", " << ptr->GetHit(i)->GetCoord()[2];
// 	    cout << endl;
// 	  }
	  
// 	}
//       }
    }
  }
 exit:  
  if(vtx_ctr) delete vtx_ctr;
  if(pctr) delete pctr;
  return nfound;
  
}

///// For debug --------------------------------------

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


