/// $Id$ 

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
///\date  2014/11/19 SH  TTCS off simulation
///
/// $Date$
///
/// $Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecon.h"
#include "TrMap.h"
#include "tkdcards.h"
#include "MagField.h"
#include "bcorr.h"

#include "TkDBc.h"
#include "TrFit.h"
#include "TkSens.h"
#include "TkCoo.h"
#include "TrMCCluster.h"
#include "Vertex.h"
#include "VCon.h"
#include "TrSim.h"
#include "HistoMan.h"
#include "TrTrackSelection.h"
#include "TrCharge.h"

#include <sys/resource.h>

#ifndef __ROOTSHAREDLIBRARY__
#include "beta.h"
#include "trrec.h"
#include "event.h"
#include "trdrec.h"
#else
#include "root.h"
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
  MaxNtrCls_ldt = MaxNtrCls;
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

  /*
  cout<<"TrRecon::SetParFromDataCards-I-TrackThrSeed= "
      <<RecPar.TrackThrSeed[0]<<" "
      <<RecPar.TrackThrSeed[1]<<endl;
  */

  TrDEBUG  = TRCLFFKEY.TrDEBUG;  // 39
  PZDEBUG  = TRCLFFKEY.PZDEBUG;  // 40
  TasRecon = TRCLFFKEY.TasRecon; // 41
}

TrRecon::~TrRecon() {
  Clear();
}

void TrRecon::Clear(Option_t *option)
{
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



void TrRecon::Init(){

  InitBuffer();
}
    
///////////////////////////////////////////////////////////
// ---      MAIN RECONSTRUCTION FUNCTION            --- //
//////////////////////////////////////////////////////////

double memchk(void);

int TrRecon::Build(int iflag, int rebuild, int hist)
{
  int flag=abs(iflag);
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if (!cont) return 0;
  if (iflag<0){ // Redo the digitization of TrRawcluster
    cont->eraseC();
    TrSim::sitkdigi();
  }

#ifdef __ROOTSHAREDLIBRARY__
  if (rebuild) {
    VCon *contt = GetVCon()->GetCont("AMSTrTrack");
    if (!contt) return -1;
    contt->eraseC();
    delete contt;
  }
#endif

  if (flag/100000 > 0) {
    int   config   = (flag/100000)%10;
    float noise[4] = { 0.9, 0.8, 0.8, 0.7 };

    // Noise fluctuation according to magnet temperature
    if (TRMCFFKEY.TrSim2010_AddNoise[0] < 0 &&
	TRMCFFKEY.TrSim2010_AddNoise[1] < 0) {
      float temp = 0;
      if (MagnetVarp::btempcor(temp, 0, 1) == 0 && 
	  MagnetVarp::mgtt.getmeanmagnettemp(temp, 1) == 0) {
	// Coefficient and offset tuned from actual TTCS-off data (2014/Oct)
	float dn = (temp-7)/40;
	for (int i = 0; i < 4; i++) noise[i] += dn;
      }
    }

    // Layers 2-8
    if (TRMCFFKEY.TrSim2010_AddNoise[0] >= 0) {
      noise[0] = TRMCFFKEY.TrSim2010_AddNoise[0];           // K
      noise[1] = TRMCFFKEY.TrSim2010_AddNoise[0]*0.8/0.9;   // S
    }
    // Layer 9
    if (TRMCFFKEY.TrSim2010_AddNoise[1] >= 0) {
      noise[2] = TRMCFFKEY.TrSim2010_AddNoise[1];           // K
      noise[3] = TRMCFFKEY.TrSim2010_AddNoise[1]*0.7/0.8;   // S
    }

    SimulateTTCSoff(config, noise);
    flag -= config*100000;
  }

  int nraw = cont->getnelem();
  delete cont;

  int trstat = 0;
  int ncls   = 0;
  int nhit   = 0;
  int ntrk   = 0;
  if (nraw >= RecPar.MaxNrawCls) trstat |= 1;

  // No reconstruction
  if (flag == 0) return trstat;

  // Check small-deltaT event
  bool lowdt = (GetTrigTime(4) <= RecPar.lowdt);

  //////////////////// TrCluster reconstruction ////////////////////
  if (flag%10 >= 1 && nraw < RecPar.MaxNrawCls) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrCluster");
#endif

    ncls = BuildTrClusters(rebuild);

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrCluster");
#endif
  }
  if (ncls > 0) {
    if ( lowdt && ncls >= RecPar.MaxNtrCls_ldt) trstat |= 2;
    if (!lowdt && ncls >= RecPar.MaxNtrCls)     trstat |= 4;
  }

  //////////////////// TrCluster reordering ////////////////////
  if ( (TRCLFFKEY.ReorderTrClusters!=0)&&
       ( (lowdt&&ncls>=RecPar.MaxNtrCls_ldt)||
         (!lowdt&&ncls>=RecPar.MaxNtrCls) ) ) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrClusterReorder");
#endif

    ReorderTrClusters();

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrClusterReorder");
#endif
  }

  //////////////////// Unbiased charge calculation ////////////////////
  FillChargeSeeds();

  //////////////////// TrRecHit reconstruction ////////////////////
  if (flag%100 >= 10 &&
      ncls >  0  &&
      (( lowdt && ncls < RecPar.MaxNtrCls_ldt) || // at small dt
       (!lowdt && ncls < RecPar.MaxNtrCls)))      // at large dt
    {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrRecHit");
#endif

    nhit = BuildTrRecHits(rebuild);

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrRecHit");
#endif
  }
  if (nhit >= RecPar.MaxNtrHit) trstat |= 8;

  //////////////////// TrRecHit reordering ////////////////////
  if ( (TRCLFFKEY.ReorderTrRecHits!=0)&&(nhit<TRCLFFKEY.MaxNtrHit) ) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrRecHitReorder");
#endif

    ReorderTrRecHits(TRCLFFKEY.ReorderTrRecHits);

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrRecHitReorder");
#endif
  }

  //////////////////// TrTrack reconstruction ////////////////////  
  int simple = (flag%10000  >=  1000) ? 1 : 0;
  int vertex = (flag%100000 >= 10000) ? 1 : 0;

  if (vertex == 1 && flag%1000 == 0) {
    _StartTimer();
    VCon *cont = GetVCon()->GetCont("AMSTrRecHit");
    int nhit = (cont) ? cont->getnelem() : 0;
    delete cont;
    if (nhit < 10 || nhit > 200) return 0;

    if (PreselTrTracksVertex() == 1) {
      trstat |= 0x20;
      if (BuildTrTracksVertex(rebuild) == 1)
	if (BuildVertex(2) == 1) trstat |= 0x2000;
      _CpuTime += _CheckTimer();
    }
    return trstat;
  }

/*
  int evid = GetEventID();
  if (evid%1000 == 0)
    cout << Form("TrRecon::Build-Memory check: %8d %5.0f",
		 evid, memchk()/1024) << endl;
*/
  if ( (flag%1000>=100)&&(nhit>0) ) {
    RecPar.NbuildTrack++;

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrTrack");
#endif
    _CpuTime = 0;

    if (simple) {
      if (TRCLFFKEY.BuildTracksSimpleChargeSeedTag==1) { 
        if (GetNHitsWithTag(TrRecHitR::ZSEED)<RecPar.MaxNtrHit) 
          ntrk += BuildTrTracksSimple(rebuild,TrRecHitR::ZSEED); 
        if (nhit<RecPar.MaxNtrHit)             
          ntrk += BuildTrTracksSimple(0);
        /* AO Warning: it could happen that GetHits(tag)<RecPar.MaxNtrHit and nhit>RecPar.MaxNtrHit.
           Fixed saving all hits for v5. */
      }
      else {
        if (nhit<RecPar.MaxNtrHit) ntrk += BuildTrTracksSimple(rebuild);
      }
    } 
    else if (!vertex) { // old algo.
      ntrk += BuildTrTracks(rebuild);
      if (!CpuTimeUp()) {
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.start("TrTrack3Extension");
#endif
        VCon* cont = GetVCon()->GetCont("AMSTrTrack"); 
        // extend the tracks to the external planes
        if (cont){
          int ntks = cont->getnelem();
          for (int itr=0;itr<ntks;itr++){
            TrTrackR* tr=(TrTrackR*) cont->getelem(itr);
            MergeExtHits(tr, tr->Gettrdefaultfit());        
          }
          delete cont;
        }
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.stop("TrTrack3Extension");
#endif
      }
    }
    if (vertex) {
      _StartTimer();
      if (nhit > 200) {
	if (!simple) trstat |= 0x08;
      }
      else {
	int retp = PreselTrTracksVertex();
	if (!simple) {
	  if (retp == 2) trstat |= 0x040;  // Multi-TOF/TRD
	  if (retp == 3) trstat |= 0x080;  // Multi-TrTrack
	  if (retp == 4) trstat |= 0x100;  // HighC-TrTrack
	}
	if (retp == 5) trstat |= 0x2000;
	if (retp == 1) {
	  if (!simple) trstat |= 0x20;
	  if (BuildTrTracksVertex(rebuild) == 1)
	    if (BuildVertex(2) == 1) trstat |= 0x2000;
	}
      }

      _CpuTime += _CheckTimer();
    }

    if (TrDEBUG >= 2)
      cout << "Event " << GetEventID() << " ntrk= " << ntrk << endl;

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack");
#endif

#ifdef __ROOTSHAREDLIBRARY__
  if (rebuild) {
    AMSEventR *ev = AMSEventR::Head();
    for (unsigned int i = 0; i < ev->NParticle(); i++)
      // Update TrTrack index
      ev->pParticle(i)->UpdateTrTrack(5, 5);
  }
#endif

  }
  if (CpuTimeUp()) {
    trstat |= 0x10;
    if (SigTERM)
      cout << "TrRecon::Build: SIGTERM detected: TrTime= "
	   << GetCpuTime() << " at Event: " <<  GetEventID() << endl;
    else {
#ifndef __ROOTSHAREDLIBRARY__
      _CpuTimeTotal += _CpuTime;
      RecPar.NcutCpu++;

      // Throw cpulimit here
      char mex[255];
      sprintf(mex,"TrRecon::BuildTrTracks Cpulimit(%.1f) Exceeded: %.1f", 
	      RecPar.TrTimeLim, GetCpuTime());
      throw AMSTrTrackError(mex);
#endif
    }
  }

  //////////////////// Post-rec. process ////////////////////

#ifndef __ROOTSHAREDLIBRARY__
 if (0 && !vertex)
  // Purge "ghost" hits and assign hit index to tracks
  PurgeGhostHits();
#endif

  // Fill histograms
  if (hist > 0) {
    trstat = CountTracks(trstat);
    trstat = FillHistos (trstat);
  }

  return trstat;
}


//////////////////////////
// --- RAW CLUSTERS --- //
//////////////////////////

#include "TRandom3.h"

class TrRawClusterRS : public TrRawClusterR {
public:
  static TRandom3 *RND;
#pragma omp threadprivate(RND)

  static void Init(int run, int evt) {
    if (RND) delete RND;
    TRandom3 rr(evt);
    int seed = run^rr.Integer(0x7fffffff);
    RND = new TRandom3(seed);
  }
  void AddNoise(float noise) {
    for (vector<float>::iterator it  = _signal.begin(); 
	                         it != _signal.end(); it++)
      *it += RND->Gaus()*noise;
  }
};

TRandom3 *TrRawClusterRS::RND = 0;

int TrRecon::SimulateTTCSoff(int config, float add_noise[4])
{
  /*
   * \param[in]  config    Configuration ID
   * 1: RUN 1412285225 - 1412688825 L2,4,6,8:Yonly    L9:XY-on
   * 2: RUN 1413209505 - 1413874948 L2,4,6,8:NCL Off  L9:XY-on
   * 3: RUN 1413897220 -            L2,4,6,8:NCL OFF  L9:Yonly
   *  (Layer 1 is always XY-on, L3,5,7 are always off)
   *
   * \param[in] add_noise[4]  Additional noise to L2-8(0:x,1:y) and L9(2:x,3:y)
   *  typically put : { 0.9, 0.8, 0.8, 0.7 };
   */

  VCon *cont = GetVCon()->GetCont("AMSTrRawCluster");
  if (!cont) {
    printf("TrRecon::SimulateTTCSoff Can't Find AMSTrRawCluster Container\n");
    return -1;
  }

  static bool first = true;
  if (first) {
#pragma omp critical (simulatettcsoff)
    {if (first) {
	cout << "TrRecon::SimulateTTCSoff-I-"
	     << "config= " << config << " add_noise= "
	     << add_noise[0] << " " << add_noise[1] << " "
	     << add_noise[2] << " " << add_noise[3] << endl;
	first = false;
      }
    }
  }

  int run = 0, evt = 0;
#ifndef __ROOTSHAREDLIBRARY__
  run = AMSEvent::gethead()->getrun();
  evt = AMSEvent::gethead()->getEvent();
#else
  AMSEventR *ev = AMSEventR::Head();
  run = (ev) ? ev->Run  () : 0;
  evt = (ev) ? ev->Event() : 0;
#endif
  TrRawClusterRS::Init(run, evt);

  int nrm = 0;

  TrRawClusterR *prev = 0;
  for (int i = 0; i < cont->getnelem(); i++) {
    TrRawClusterR *raw = (TrRawClusterR *)cont->getelem(i);
    int lay  = raw->GetLayerJ();
    int side = raw->GetSide();
    int ison = 1;

    // Layer 1 is always nominal condition (do nothing)
    if (lay == 1) ;

    // Layer 9 is Yonly if config=3
    else if (lay == 9) {
      if (config == 3 && side == 0) ison = 0;
    }

    // Layers 3,5,7 are always off
    else if (lay == 3 || lay == 5 || lay == 7) ison = 0;

    // Layers 2,4,6,8
    else if (lay == 2 || lay == 4 || lay == 6 || lay == 8) {
      // config:1 Y-only
      if (config == 1) {
	if (side == 0) ison = 0;
      }
      // config:2,3 NCL off
      else if (config == 2 || config == 3) {
	int tkid = TMath::Abs(raw->GetTkId());
	 if (lay == 2 && (tkid <= 103 || 113 <= tkid)) ison = 0;
	 if (lay == 4 && (tkid <= 303 || 313 <= tkid)) ison = 0;
	 if (lay == 6 && (tkid <= 503 || 513 <= tkid)) ison = 0;
	 if (lay == 8 && (tkid <= 703 || 713 <= tkid)) ison = 0;

	if (side == 0) {
	 if (lay == 2 && (tkid <= 105 || 111 <= tkid)) ison = 0;
	 if (lay == 4 && (tkid <= 305 || 311 <= tkid)) ison = 0;
	 if (lay == 6 && (tkid <= 505 || 511 <= tkid)) ison = 0;
	 if (lay == 8 && (tkid <= 705 || 711 <= tkid)) ison = 0;
	}
      }
    }

    // Add noise if ON (layers 2-9)
    if (lay != 1) {
      float noise = 0;
      if (lay == 9) noise = (side == 0) ? add_noise[2] : add_noise[3];
      else          noise = (side == 0) ? add_noise[0] : add_noise[1];
      if (noise > 0) ((TrRawClusterRS *)raw)->AddNoise(noise);
    }

    // Remove TrRawCluster if the corresponding ladder/side is OFF
    if (!ison) {
      static int npri = 0;
      if (npri < 10) {
#pragma omp critical (simulatettcsoffpri)
	{if (npri < 10) {
	    cout << "TrRecon::SimulateTTCSoff-I-Removing TkID= "
		 << raw->GetTkId() << " layJ= " << raw->GetLayerJ()
		 << " side= " <<raw->GetSide() << endl;
	    npri++;
	  }
	}
      }
      cont->removeEl(prev);
      nrm++;
      i--;
    }
    else prev = raw;
  }
  delete cont;

  TrLadCal::AddNoiseK[1] = TrLadCal::AddNoiseK[3] = 
  TrLadCal::AddNoiseK[5] = TrLadCal::AddNoiseK[7] = add_noise[0];
  TrLadCal::AddNoiseS[1] = TrLadCal::AddNoiseS[3] = 
  TrLadCal::AddNoiseS[5] = TrLadCal::AddNoiseS[7] = add_noise[1];
  TrLadCal::AddNoiseK[8]                          = add_noise[2];
  TrLadCal::AddNoiseS[8]                          = add_noise[3];

  return nrm;
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


int TrRecon::GetAddressInSubBuffer(int address, int first, int last, int cyclicity) {
  // by default returns the address
  if ( (address<last)&&(address>=first) ) return address;
  // if not cyclic and out-of-bounds return out-of-bounds value
  if (cyclicity==0) return -1;
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

  // Get the pointer to the TrRawCluster container
  // AMSContainer* cont=GetCont(AMSID("AMSTrRawCluster"));
  VCon* cont=GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrRawCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    // printf("TrRecon::BuildTrClusters  AMSTrRawCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }
  // else printf( "TrRecon::BuildTrClusters  AMSTrRawCluster Container has %d elements\n",cont->getnelem());

  VCon* cont2=GetVCon()->GetCont("AMSTrCluster");
  if(!cont2){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  // Empty the destination container if requested
  if(rebuild) cont2->eraseC();


  // Build The Map of TrRawClusters
  TrMap<TrRawClusterR> RawMap;

  for (int ii=0;ii< cont->getnelem();ii++){
    TrRawClusterR* clu=(TrRawClusterR*)cont->getelem(ii);
    //clu->Print();
    if (clu) RawMap.Add(clu);
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
    
    // for (int istrip=0; istrip<1024; istrip++) {
    //  if (_adcbuf[istrip]!=0) {
    //    printf("%+4d %4d %7.2f %7.2f %7.2f %6d\n",
    //      lad->first,istrip,_adcbuf[istrip],_sigbuf[istrip],_adcbuf[istrip]/_sigbuf[istrip],_stabuf[istrip]
    //    ); 
    //  }
    //}
 
    int TkId = lad->first;
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


void TrRecon::ReorderTrClusters() {
  VCon* cont = GetVCon()->GetCont("AMSTrCluster");
  int nclu = cont->getnelem();
  for (int iclu=0; iclu<nclu-1; iclu++) {
    for (int jclu=iclu+1; jclu<nclu; jclu++) {
      TrClusterR* clu1 = (TrClusterR*) cont->getelem(iclu);
      float       s1   = clu1->GetTotSignal();
      TrClusterR* clu2 = (TrClusterR*) cont->getelem(jclu);
      float       s2   = clu2->GetTotSignal();
      if (s2>s1) cont->exchangeEl(clu1,clu2);
    }
  }
  if (cont) delete cont;
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
  int nseeds = BuildSeedListInSubBuffer(tkid,first,last,cyclicity);

  // printf("Found %d seeds\n",nseeds);
  // for (int jj=0;jj<nseeds;jj++) printf("seed %d  %d\n",jj,_seedaddresses.at(jj));

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
    // printf("tkid: %+4d  iseed: %2d  seedadd: %4d  lenght: %3d  seedindex: %4d  left: %4d  right: %4d\n",
    //  tkid,ss,seedaddress,length,seedind,leftaddress,rightaddress
    // );
    for (int jj=0; jj<length; jj++) { 
      int address = GetAddressInSubBuffer(leftaddress+jj,first,last,cyclicity);
      sigg[jj]=_adcbuf[address];
    }

#ifndef __ROOTSHAREDLIBRARY__ 
    if(cont) cont->addnext(new AMSTrCluster(tkid,side,leftaddress,length,seedind,sigg,0));
#else
    if(cont) cont->addnext(new TrClusterR(tkid,side,leftaddress,length,seedind,sigg,0));
    TrClusterR* cluster = (TrClusterR*) cont->getelem((int)cont->getnelem()-1);
    if (!cluster->Check(1)) cluster->Print(10);
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
  /// move first --> (first-1) and (last-1) --> last to avoid border effects
  int left  = (index<=0) ? first-1 : _seedaddresses.at(index-1);
  if ( (cyclicity!=0)&&(index<=0) ) left = _seedaddresses.at(nseeds-1);
  int right = (index>=(nseeds-1)) ? last : _seedaddresses.at(index+1);
  if ((cyclicity!=0)&&(index>=(nseeds-1))) right = _seedaddresses.at(0);

  // printf("boundary search, left: %4d  right: %4d  cycl: %1d  iseed: %2d   seedadd: %4d\n",left,right,cyclicity,index,seedaddress);

  // search for the left boundary
  int address = GetAddressInSubBuffer(leftaddress-1,first,last,cyclicity); 
  while ( (address!=left)&&(address!=-1) ) { 
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
  while ( (address!=right)&&(address!=-1) ) { 
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
  // get the pointer to the TrCluster container
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
  // get the pointer to the TrRecHit container
  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrRecHit Container "
           "Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container
  if (rebuild) cont2->eraseC();
  // build cluster map
  BuildClusterTkIdMap();
  // number of hits created
  int ntrrechits = 0;

  ///////////////////////
  // first hits from all combinations
  ///////////////////////

  for (ITClusterMap lad = _ClusterTkIdMap.begin(); lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int nx   = lad->second.first.size();
    int ny   = lad->second.second.size();
    int ilay = std::abs(tkid)/100-1;
    // skip if requested by datacard
    if (RecPar.ThrSeed[1][ilay] < 0) continue;
    // loop on x and y
    for (int iy=0; iy<ny; iy++) {
      for (int ix=0; ix<nx; ix++) {
        TrClusterR* clX = lad->second.first.at(ix);
        TrClusterR* clY = lad->second.second.at(iy);
#ifndef __ROOTSHAREDLIBRARY__
        AMSTrRecHit* hit = new AMSTrRecHit(tkid,clX,clY,0);
#else
        TrRecHitR* hit  = new TrRecHitR(tkid,clX,clY,0);
#endif
        // x/y association (the corrections are the same of TrRecHit::GetCorrelationProb)
        float sigx = clX->GetTotSignal(TrClusterR::kAsym); 
        float sigy = clY->GetTotSignal(TrClusterR::kAsym);
        float prob = hit->GetCorrelationProb();
        hman.Fill("AmpyAmpx",sqrt(sigx),sqrt(sigy));
        hman.Fill("ProbAmpx",sqrt(sigx),log10(hit->GetCorrelationProb()));
        float sigx_corr = clX->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain);
        float sigy_corr = clY->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain);        
        hman.Fill("AmpyAmpxC",sqrt(sigx_corr),sqrt(sigy_corr));
        // bad association
	if (prob<TRCLFFKEY.CorrelationProbThr) { 
          delete hit;
	  continue;  
	}
        // compatibility between hit clusters and charge seeds in status
        if ( (TRCLFFKEY.ChargeSeedTagActive!=0)&&(CompatibilityWithChargeSeed(hit)) ) hit->setstatus(TrRecHitR::ZSEED);
        // eliminate hit based on charge seed (high-z clean rec.)        
        if ( (TRCLFFKEY.ChargeSeedSelActive!=0)&&(!CompatibilityWithChargeSeed(hit)) ) { 
          delete hit; 
          continue; 
        }
        // add
	cont2->addnext(hit);
        ntrrechits++;
      }
    }
  }

  ///////////////////////
  // then hits with only y-cluster
  ///////////////////////

  for (ITClusterMap lad = _ClusterTkIdMap.begin(); lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int ny   = lad->second.second.size();
    int ilay = std::abs(tkid)/100-1;
    // skip if requested by datacard
    if (RecPar.ThrSeed[1][ilay] < 0) continue;
    // loop on y only
    for (int iy=0; iy<ny; iy++) {
      TrClusterR* clY = GetTrCluster(tkid,1,iy);
      // histos
      hman.Fill("SeedSNyN",clY->GetNelem(),clY->GetSeedSN());
      hman.Fill("SignalyN",clY->GetNelem(),clY->GetTotSignal());
      // y cluster reasonable signal-to-noise (we want to be sure of what we get!)
      hman.Fill("ClSNyN",clY->GetNelem(),clY->GetClusterSN());
      if (clY->GetClusterSN()<TRCLFFKEY.YClusterSNThr) continue; 
      if (clY->GetNelem()<=TRCLFFKEY.YClusterNStripThr) continue;
#ifndef __ROOTSHAREDLIBRARY__
      AMSTrRecHit* hit = new AMSTrRecHit(tkid,0,clY,0);
#else
      TrRecHitR* hit   = new TrRecHitR(tkid,0,clY,0);
#endif
      // compatibility between hit clusters and charge seeds in status
      if ( (TRCLFFKEY.ChargeSeedTagActive!=0)&&(CompatibilityWithChargeSeed(clY)) ) hit->setstatus(TrRecHitR::ZSEED);
      // eliminate hit based on charge seed (high-z clean rec.)
      if ( (TRCLFFKEY.ChargeSeedSelActive!=0)&&(!CompatibilityWithChargeSeed(clY)) ) { 
        delete hit; 
        continue; 
      }
      // add
      cont2->addnext(hit);
      ntrrechits++;
    }    
  }

  /* // DUMP
  printf("NHits: %2d    ChargeSeedX = %7.2f   ChargeSeedY = %7.2f\n",cont2->getnelem(),GetChargeSeed(0),GetChargeSeed(1));
  for (int ihit=0; ihit<cont2->getnelem(); ihit++) {
    TrRecHitR* hit = (TrRecHitR*) cont2->getelem(ihit);
    if (hit==0) continue;
    hit->Print(0);
  }
  */

  // delete containers 
  if(cont) delete cont;
  if(cont2) delete cont2;
  return ntrrechits;
} 

int TrRecon::GetNHitsWithTag(int tag) {
  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) { 
    printf("TrRecon::GetNHitsWithTag-W no valid hit container. Return 0.\n");
    return 0;
  }
  int ntag = 0;
  int nhit = cont->getnelem();
  for (int ihit=0; ihit<nhit; ihit++) {
    TrRecHitR* hit = (TrRecHitR*) cont->getelem(ihit);
    if (hit->checkstatus(tag)) ntag++;
  }
  return ntag;
}

void TrRecon::ReorderTrRecHits(int type) {
  if (type<=0) return;
  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) { 
    printf("TrRecon::ReorderTrRecHits-W no valid hit container. Skip reordering.\n");
    return;
  }
  int nhit = cont->getnelem();
  for (int ihit=0; ihit<nhit-1; ihit++) {
    for (int jhit=ihit+1; jhit<nhit; jhit++) {
      TrRecHitR* hit1 = (TrRecHitR*) cont->getelem(ihit);
      TrRecHitR* hit2 = (TrRecHitR*) cont->getelem(jhit);
      if ( (hit1==0)||(hit2==0) ) continue;
      // reorder separately XY hits and only-Y 
      if ( (hit1->OnlyY()!=hit2->OnlyY())||(hit1->OnlyX()!=hit2->OnlyX()) ) continue;
      float value1 = 0;
      float value2 = 0;
      switch (type) {
        case 1: // reorder by probability 
          value1 = hit1->GetCorrelationProb();
          value2 = hit2->GetCorrelationProb();
          break;
        case 2: // reorder by y-signal and probability
          value1 = hit1->GetSignalCombination(1,TrClusterR::DefaultCorrOpt);
          value2 = hit2->GetSignalCombination(1,TrClusterR::DefaultCorrOpt);
          if (fabs(value1-value2)<1e-06) {
            value1 = hit1->GetCorrelationProb();
            value2 = hit2->GetCorrelationProb();
          }
          break;
        case 3: // reorder by y-signal and x-signal
          value1 = hit1->GetSignalCombination(1,TrClusterR::DefaultCorrOpt);
          value2 = hit2->GetSignalCombination(1,TrClusterR::DefaultCorrOpt);
          if (fabs(value1-value2)<1e-06) {
            value1 = hit1->GetSignalCombination(0,TrClusterR::DefaultCorrOpt);
            value2 = hit2->GetSignalCombination(0,TrClusterR::DefaultCorrOpt);   
          }
          break;
      }
      if (value2>value1) cont->exchangeEl(hit1,hit2);
    }
  }
  if (cont) delete cont;
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

#define TR_DEBUG_CODE_401 \
if (TrDEBUG >= 1) {\
  cout << "C401 1st fit "<<mfit1<<" "<<track->GetChisqX(mfit1)<<" "\
                                     <<track->GetChisqY(mfit1)<<endl;\
  for (int i = 0; i < track->GetNhits(); i++) {\
    TrRecHitR *hit = track->GetHit(i);\
    if (hit)\
      cout << Form("%4d %6.2f %6.2f %6.2f X(%3d) Y(%3d)", hit->GetTkId(),\
	           hit->GetCoord().x(), hit->GetCoord().y(),\
                   hit->GetCoord().z(), hit->iTrCluster('x'),\
		                        hit->iTrCluster('y')) << endl;\
  }\
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
    int ily = hit->GetLayer();\
    cout << Form("%4.1f ", track->GetResidualO(ily).x()*10);\
    cout << Form("%4.1f ", track->GetResidualO(ily).y()*10);\
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
      int il = std::abs(hit->GetTkId())/100;\
      cout << Form("%4d %03X %6.2f %6.2f %6.2f res %8.1f %8.1f %8.1f %8.1f", \
                   hit->GetTkId(), hit->getstatus(), hit->GetCoord()[0],\
                   hit->GetCoord()[1], hit->GetCoord()[2],\
                   1e4*track->GetResidualO(il, id)[0],\
                   1e4*track->GetResidualO(il, TrTrackR::kChoutko)[0],\
                   1e4*track->GetResidualO(il, id)[1],\
                   1e4*track->GetResidualO(il, TrTrackR::kChoutko)[1])\
           << endl;\
    }\
  }\
}

#define TR_DEBUG_CODE_100 \
if (TrDEBUG >= 4) {\
  double clr = sig/cls->GetSeedSignal()/cls->GetNelem();\
  double cog = std::fabs(cls->GetCofG());\
  int   sadd = cls->GetSeedAddress();\
  cout << Form("C100 %4d:%02d %2d "\
               "nelm:%d sadd:%4d sSN:%5.2f sig:%5.1f clr:%5.3f cog:%5.3f",\
	       cls->GetTkId(),j,sign,cls->GetNelem(),sadd,\
	       cls->GetSeedSN(),sig,clr,cog)<<endl;\
}

#define TR_DEBUG_CODE_101 \
if (TrDEBUG >= 4) {\
  cout << "C101 ";\
  for (int j = 0; j < NP; j++)\
    if (ic[j][i[j]] > 0) {\
      TrClusterR *cls = (TrClusterR *)cont_clu->getelem(ic[j][i[j]]/10);\
      if (cls) cout << Form("%4d:%02d ", cls->GetTkId(), ic[j][i[j]]/10);\
    }\
  cout << Form(": %6.3f %6.3f %.2f", csq, cthd, rgt) << endl;\
}

#define TR_DEBUG_CODE_102 \
if (TrDEBUG >= 3) {\
  cout << "C102 ins.at " << jc << " ";\
  for (int j = 0; j < NC && tmin[j].csq > 0; j++) cout << tmin[j].csq << " ";\
  cout << endl;\
}

#define TR_DEBUG_CODE_103 \
if (TrDEBUG >= 3) {\
  cout << Form("Y%02d: ", jc);\
  for (int k = 0; k < NL; k++)\
    if (tmin[jc].ic[k] > 0) {\
      TrClusterR *cls = (TrClusterR *)cont_clu->getelem(tmin[jc].ic[k]/10);\
      if (cls) cout << Form("%4d:%02d ", cls->GetTkId(), tmin[jc].ic[k]/10);\
    }\
  cout << ": " << csq << " " << cthd << " " << trfit.GetRigidity() << " "\
       << tmin[jc].csq << endl;\
  cout << "     ";\
  for (int k = 0; k < NL; k++)\
    if (tmin[jc].ic[k] > 0)\
      cout << Form("  %5.1f ", tmin[jc].qc[k]);\
  cout << endl;\
}

#define TR_DEBUG_CODE_104 \
if (TrDEBUG >= 2) {\
  cout << "C104 Sorted" << endl;\
  for (int jj = 0; jj < NC*2; jj++) {\
    int jc = jci[jj];\
    if (tmin[jc].csq < 0) continue;\
    cout << Form("Y%02d: ", jc);\
    for (int k = 0; k < NL; k++)\
      if (tmin[jc].ic[k] > 0) {\
	TrClusterR *cls = (TrClusterR *)cont_clu->getelem(tmin[jc].ic[k]/10);\
	if (cls) cout << Form("%4d:%02d ", cls->GetTkId(), tmin[jc].ic[k]/10);\
      }\
    cout << ": " << tmin[jc].csqf << " " << tmin[jc].rgtf << endl;\
  }\
}

#define TR_DEBUG_CODE_105 \
if (TrDEBUG >= 4) {\
  TrClusterR *cls = (TrClusterR *)cont_clu->getelem(ai/10);\
  cout << Form("C105 %4d:%02d %7.3f %7.3f %7.3f", (cls) ? cls->GetTkId() : 0,\
	       ai/10, r, rmin, rsig) << endl;\
}

#define TR_DEBUG_CODE_106 \
if (TrDEBUG >= 2) {\
  cout << Form("X%02d: ", jc);\
  for (int j = 0; j < NL; j++) {\
    if (imin[j] == 0) continue;\
    TrRecHitR *hit = (TrRecHitR *)cont_hit->getelem(abs(imin[j])/1000);\
    if (hit) cout << Form("%4d:%03d%02d ", hit->GetTkId(),\
			  imin[j]/1000, (abs(imin[j])/10)%100);\
    else     cout << Form("----:%03d%02d ",\
			  imin[j]/1000, (abs(imin[j])/10)%100);\
  }\
  cout << ": " << cmin << endl;\
}

#define TR_DEBUG_CODE_107 \
if (TrDEBUG >= 1) {\
  VCon *cont = GetVCon()->GetCont("AMSTrTrack");\
  TrTrackR *track = (TrTrackR *)cont->getelem(cont->getnelem()-1);\
  cout << "TR" << jc << " " << track->GetNhits()<<" ";\
  for (int j = 0; j < track->GetNhits(); j++) {\
    TrRecHitR *hit = track->GetHit(j);\
    cout << Form("%4d:%03d%c", hit->GetTkId(), track->iTrRecHit(j),\
		 ((hit->OnlyY()) ? 'Y' : ' '));\
  }\
  cout << Form("%5.2f %5.2f %.2f %4.1f %d %d", track->GetNormChisqX(),\
	                                       track->GetNormChisqY(),\
	       track->GetRigidity(),\
	       (TrCharge::GetQ(track, 0)+TrCharge::GetQ(track, 1))/2,\
               ntcls, ntmax) << endl;\
  cout << "      ";\
  int opt = TrClusterR::kAngle|TrClusterR::kAsym|TrClusterR::kGain|\
            TrClusterR::kLoss|TrClusterR::kMIP;\
  for (int j = 0; j < track->GetNhits(); j++) {\
    TrRecHitR *hit = track->GetHit(j);\
    double qx = hit->GetSignalCombination(0,opt,1);\
    double qy = hit->GetSignalCombination(1,opt,1);\
    cout << Form((qx < 10) ? "%4.1f" : "%4.0f", qx)\
	 << Form((qy < 10) ? "%4.1f" : "%4.0f", qy)\
	 << " ";\
  }\
  cout << Form("%5.2f %5.2f", TrCharge::GetQ(track, 0),\
	                      TrCharge::GetQ(track, 1)) << endl;\
}

#define TR_DEBUG_CODE_108 \
if (TrDEBUG >= 5) {\
  cout << "C108 ";\
  for (int j = 0; j < NP; j++) {\
    TrRecHitR *hit = (TrRecHitR *)cont_hit->getelem(abs(ih[j%2][i[j]])/1000);\
    if (hit) cout << Form("%4d:%03d%02d ", hit->GetTkId(),\
			  ih[j%2][i[j]]/1000, (abs(ih[j%2][i[j]])/10)%100);\
    else     cout << Form("----:%03d%02d ",\
			  ih[j%2][i[j]]/1000, (abs(ih[j%2][i[j]])/10)%100);\
  }\
  cout << Form(": %6.3f %6.3f %6.3f %d", trfit.GetChisqX(), csq, cmin, nmin)\
       << endl;\
}

#define TR_DEBUG_CODE_109 \
if (TrDEBUG >= 5) {\
  cout << "C109 ";\
  for (int j = 0; j < NP; j++) {\
    TrRecHitR *hit = (TrRecHitR *)cont_hit->getelem(abs(ih[j%2][i[j]])/1000);\
    if (hit) cout << Form("%4d:%03d%d ", hit->GetTkId(),\
			  ih[j%2][i[j]]/1000, abs(ih[j%2][i[j]])%10);\
    else     cout << Form("----:%03d%d ",\
			  ih[j%2][i[j]]/1000, abs(ih[j%2][i[j]])%10);\
  }\
  cout << Form(": %6.3f %6.3f %f", dps3, pslx, argt) << endl;\
}

#define TR_DEBUG_CODE_110 \
if (TrDEBUG >= 1) {\
  cout << "C110 nhc= " << nhc << " " << nlx << " | ";\
  for (int k = 0; k < NL; k++) cout << " " << nhx[k]; cout << endl;\
}

#define TR_DEBUG_CODE_111 \
if (TrDEBUG >= 2) {\
  cout << "C111 " << Form("%d %6d %4.1f | %6d %4.1f %5.1f %4.1f | ",\
			  il, imin[il], ((imin[il] != 0) ? dmin[il] : 0),\
			  ih[is][j], d, dtc.x(), dtc.y()) << ch[is][j];\
  if (imin[il] == ih[is][j]) cout << " SEL";\
  cout << endl;\
}

#define TR_DEBUG_CODE_112 \
if (TrDEBUG >= 2) {\
  cout << "C112 nk,nmin= " << nk << " " << nmin << " "\
       << tmin[jc].GetN() << " " << nlx << " " << nhc << " "\
       << "p= " << p1 << " " << p2 << endl;\
}

#define TR_DEBUG_CODE_113 \
if (TrDEBUG >= 2) cout << "ProcessTrack" << endl;

#define TR_DEBUG_CODE_114 \
if (TrDEBUG >= 2) cout << "ProcessTrack OK" << endl;

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
  for (int i = 0; i < _SCANLAY; i++) {
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

int TrRecon::BuildTrTracksSimple(int rebuild, int select_tag) {

  //////////////////// Check VCon ////////////////////

  VCon* cont_trk = GetVCon()->GetCont("AMSTrTrack");
  if (!cont_trk) {
    printf("TrRecon::BuildTrTracksSimple-W Can't find AMSTrTrack container. Reconstruction is impossible.\n");
    return -1;
  }
  if (rebuild) cont_trk->eraseC();
  int ntrack = cont_trk->getnelem();
  // in the case BuildTrTracksSimple is called many times quit when the number of tracks is too high 
  // (maybe RecPar.MaxNtrack has to be increased)
  if (ntrack >= RecPar.MaxNtrack) {
    if (cont_trk) delete cont_trk;
    return ntrack;
  }

  VCon* cont_hit = GetVCon()->GetCont("AMSTrRecHit");
  if(!cont_hit) {
    printf("TrRecon::BuildTrTracksSimple-W Can't find AMSTrRecHit container. Reconstruction is impossible.\n");
    return -1;
  }
  int nhit = cont_hit->getnelem();

  VCon* cont_clu = GetVCon()->GetCont("AMSTrCluster");
  if(!cont_clu) {
    printf("TrRecon::BuildTrTracksSimple-W Can't find AMSTrCluster container. Reconstruction is impossible.\n");
    return -1;
  }
  if (!patt) {
    int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
    patt = new tkpatt(nn);
    patt->Init(nn);
  }
  
  _CpuTimeUp = false;
  _StartTimer();
  
  int tracks_simple_stat[10] = {0,0,0,0,0,0,0,0,0,0};
  
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack0Find");
#endif
  
  //////////////////// Parameters definition ////////////////////
  
  // Parameters to be tuned
  double pselm = 4.00;    // X-Pre-selection range in cm (dummyX)
  double pselx = 1.00;    // X-Pre-selection range in cm
  double psely = 4.00;    // Y-Pre-selection range in cm
  double mselx = 0.25;    // X-merge range in cm
  double msely = 0.25;    // Y-merge range in cm
  double errx  = 300e-4;  // Fitting sigma in cm
  double erry  = 300e-4;  // Fitting sigma in cm
  double cmaxx = 300;     // Maximum chisquare allowed
  double cmaxy = 300;     // Maximum chisquare allowed

  pselm = RecPar.ClusterScanRange*8;  // 4.00
  pselx = RecPar.ClusterScanRange*2;  // 1.00
  psely = RecPar.ClusterScanRange*8;  // 4.00
  mselx = RecPar.ClusterScanRange/2;  // 0.25
  msely = RecPar.ClusterScanRange/2;  // 0.25
  errx  = RecPar.ErrXForScan;         // 300e-4
  erry  = RecPar.ErrYForScan;         // 300e-4
  cmaxx = RecPar.MaxChisqAllowed;     // 300
  cmaxy = RecPar.MaxChisqAllowed;     // 300
  
  static bool first = true;
  if (first) {
#pragma omp critical (trrecsimple)
  {if (first) {
    cout << "TrRecon::BuildTrTracksSimple-I-"
	 << "psel= " << pselm << " " << pselx << " " << psely << " "
	 << "msel= " << mselx << " " << msely << " "
	 << "cmax= " << cmaxx << " " << cmaxy << endl;
    first = false;
   }
  }}
  
  //////////////////// Buffers definition ////////////////////
  
  enum { NL = 7, NP = 4, NH = 500, NC = 7 };
  
  AMSPoint cc[NP][NH];
  int      ic[NP][NH], nc[NP];
  double   qc[NP][NH];
  for (int j = 0; j < NP; j++) nc[j] = 0;
  
  double zlay[NL];
  for (int j = 0; j < NL; j++) zlay[j] = TkDBc::Head->GetZlayer(j+1);
  
  //////////////////// Fill buffers with Y-clusters ////////////////////

  // take the only-Y from hit list 
  for (int ihit=0; ihit<nhit; ihit++) {
    TrRecHitR* hit = (TrRecHitR*) cont_hit->getelem(ihit); 
    
    if (!hit) continue;
    if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
    if ( (hit->Used()) || (!hit->OnlyY()) || (hit->GetLayer()>=8) ) continue; 
    TrClusterR* cls = (TrClusterR*) hit->GetYCluster();
    if (!cls) continue;
    if (cls->Used()) continue;
    int j = hit->GetYClusterIndex();

    cls->SetDxDz(0);
    cls->SetDyDz(0);
    cls->SetQtrk(1);

    double sig = cls->GetTotSignal();
    int    sign = 1;
    
    if (cls->GetLayer() != 1) {
      if (RecPar.TrackThrSeed[1] > 3.5 &&
	  RecPar.TrackThrSeed[1] > cls->GetSeedSN()) sign = -1;
     
      // tag bad-signals? Not active 
      // else if (sig < 10) sign = -1;
      
      /* // Try to select a good hit, do delta rays, TO-BE-STUDIED further
	 else if (cls->GetNelem() >= 4 && sig > 70) {
	 double clr = sig/cls->GetSeedSignal()/cls->GetNelem();
	 double cog = std::fabs(cls->GetCofG());
	 double xx  = cog*cog*cog*cog;
	 if ((cls->GetNelem() == 4 && clr > 0.6+3*xx) ||
	 (cls->GetNelem() >= 5 && clr > 0.5+4*xx)) sign = -1;
	 }
      */
    }
    TR_DEBUG_CODE_100;
    
    int ly = cls->GetLayer();
    int ip = (ly == 1) ? 0 : ly/2;
    if (ip >= NP || nc[ip] >= NH) continue;
    ic[ip][nc[ip]] = sign*(j*10+ly);
    qc[ip][nc[ip]] = sig;
    // printf("%2d %2d %2d %+4d %7.2f %7.2f %7.2f\n",
    //   ip,nc[ip],sign,cls->GetTkId(),cls->GetTotSignal(),cls->GetTotSignal(0),cls->GetSeedSN());
    cc[ip][nc[ip]++].setp(0, cls->GetGCoord(0), zlay[ly-1]);
  }
  
  struct TrSeed {
    double csq, csqf;
    double rgt, rgtf;
    int      ic[NL];
    AMSPoint cc[NL];
    double   qc[NL];
    double ps1, ps2, qsm, qtm;
    double rmrg[6];

    TrSeed() : csq(-1), csqf(-1) { for (int j = 0; j < NL; j++) { ic[j] = 0; qc[j] = 0; } }    
    void Set(double q, double r, double p1, double p2,
	     int *i, const int IC[NP][NH], const AMSPoint CC[NP][NH],
	     const double   QC[NP][NH]) {
      csq = q; rgt = r; ps1 = p1; ps2 = p2; qsm = 0;
      for (int j = 0; j < NP; j++) { ic[j] = IC[j][i[j]];
	cc[j] = CC[j][i[j]];
	qsm += (qc[j] = QC[j][i[j]])/NP; }
    }
    void Set(double q,
	     int *i, const int IC[NP][NH], const AMSPoint CC[NP][NH],
	     const double   QC[NP][NH]) {
      csq = q; qsm = 0;
      for (int j = 1; j < NP; j++) { ic[j] = IC[j][i[j]];
	cc[j] = CC[j][i[j]];
	qsm += (qc[j] = QC[j][i[j]])/(NP-1); }
    }
    int GetN(void) const {
      int n = 0; for (int j = 0; j < NL; j++) if (ic[j] != 0) n++;
      return n;
    }
    int GetNshr(TrSeed &ts) const {
      int n = 0;
      for (int j = 0; j < NL; j++)
	for (int k = 0; ic[j] > 0 && k < NL; k++) if (ic[j] == ts.ic[k]) n++;
      return n;
    }
    int Intp(AMSPoint *pint, double *zlay) {
      int p[3][3] = { { 0, 2, 3 }, { 0, 1, 3 }, { 0, 1, 2 } };
      // No Layer 1
      if (ic[0] == 0) {
	for (int j = 0; j < 3; j++) {
	  double z = (ic[j+1]%10 == 2) ? zlay[(j+1)*2] : zlay[(j+1)*2-1];
	  pint[j].setp(0, Intpol1(cc[1].z(), cc[3].z(), 
				  cc[1].y(), cc[3].y(), z), z);
	}
      }
      // SimpleFitTop
      else {
	TrFit trfit;
	double err = 300e-4;
	for (int j = 0; j < NP; j++) trfit.Add(cc[j], err, err, err);
	if (trfit.SimpleFitTop() < 0) return -1;
	for (int j = 0; j < 3; j++) {
	  double z = (ic[j+1]%10 == 2) ? zlay[(j+1)*2] : zlay[(j+1)*2-1];
	  double y = Intpol2(cc[p[j][0]].z(), cc[p[j][1]].z(), cc[p[j][2]].z(),
			     cc[p[j][0]].y(), cc[p[j][1]].y(), cc[p[j][2]].y(),
			     z);
	  pint[j].setp(0, y, z);
	}
	trfit.InterpolateSimple(3, pint);
      }
      return 1;
    }
    void Print() {
      printf("signals ");
      for (int il=0; il<NL; il++)
        printf(" %5.0f",qc[il]);
      printf(" chisq %5.1f rig %5.1f ps1 %5.1f ps2 %5.1f qsm %5.1f qtm %5.1f\n",csq,rgt,ps1,ps2,qsm,qtm);
    }
  } tmin[NC*2];
  
  //////////////////// Search on Y-clusters ////////////////////
 
#define CY(I) cc[I][i[I]].y()
#define CZ(I) cc[I][i[I]].z()
  
  int hskip = 0;
  int cskip = 0;

  double dpsmax = 0.2;
  if (TRCLFFKEY.AllowYonlyTracks) dpsmax = 2;
  
  int i[NP];
  for (i[1] = 0; i[1] < nc[1]; i[1]++) {  // Plane 2 (Layer 2, 3)
    if (CpuTimeUp()) break;
    if (ic[1][i[1]] < 0) continue;
    
    for (i[3] = 0; i[3] < nc[3]; i[3]++) {  // Plane 4 (Layer 6, 7)
      if (CpuTimeUp()) break;
      if (ic[3][i[3]] < 0) continue;
      
      // double qs0 = (qc[1][i[1]]+qc[3][i[3]])/2;
      // double qs1 =  qc[1][i[1]]/qc[3][i[3]];
      //double qth = ((qc[1][i[1]] > 20 &&
	  //     qc[3][i[3]] > 20) || qs0 > 45) ? 3+20/qs0 : 10;
      // if (qs1 < 1/qth || qth < qs1) continue; // SH-to-be-fixed
      float xxx = sqrt(qc[1][i[1]]);
      float yyy = sqrt(qc[3][i[3]]);
      hman.Fill("ySig0",xxx,yyy);
      if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue;                 

      for (i[0] = 0; i[0] <= nc[0]; i[0]++) { // Plane 1 (Layer 1)
	if (CpuTimeUp()) break;
	double dps1 = 0;
	if (i[0] < nc[0]) {
	  dps1 = std::fabs(CY(1)-Intpol1(CZ(0), CZ(3), CY(0), CY(3), CZ(1)));
	  if (dps1 > psely*2.5) continue;
	  
	  // if (qc[0][i[0]]/qs0 < 1/(2.5+50/qs0) || 
	  //     qc[0][i[0]]/qs0 >    2.5+50/qs0) continue;  // SH-to-be-fixed
          float xxx = sqrt((qc[1][i[1]]+qc[3][i[3]])/2);
          float yyy = sqrt(qc[0][i[0]]);
          hman.Fill("ySig1",xxx,yyy);
          if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue; 
	}
	
	for (i[2] = 0; i[2] < nc[2]; i[2]++) {  // Plane 3 (Layer 4, 5)
	  if (CpuTimeUp()) break;
	  if (ic[2][i[2]] < 0) continue;
	  
	  if (++cskip == 100) {
	    // Check cpu time limit
	    if ((RecPar.TrTimeLim > 0 && 
		 RecPar.TrTimeLim < _CheckTimer())|| SigTERM) {
	      _CpuTimeUp = true;
	      break;
	    }
	    cskip = 0; 
	  }
	  
	  // if (qc[2][i[2]]/qs0 < 1/(2.5+50/qs0) || 
	  // qc[2][i[2]]/qs0 >    2.5+50/qs0) continue; // SH-to-be-fixed
          float xxx = sqrt((qc[1][i[1]]+qc[3][i[3]])/2);
          float yyy = sqrt(qc[2][i[2]]);
          hman.Fill("ySig2",xxx,yyy);
          if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue;     

	  if (i[0] == nc[0]) {
	    double dps = std::fabs(CY(2)-Intpol1(CZ(1), CZ(3), 
						 CY(1), CY(3), CZ(2)));
	    if (dps > dpsmax) continue;
	    
	    int jr;
	    for (jr = 0; jr < NC && tmin[jr].csq > 0; jr++) {
	      if (tmin[jr].ic[1] == ic[1][i[1]] && 
		  tmin[jr].ic[3] == ic[3][i[3]]) { jr = -1; break; }
	    }
	    if (jr < 0) continue;
	    
	    for (jr = NC; jr < NC*2 && tmin[jr].csq > 0; jr++) {
	      int nshr = 0;
	      for (int j = 1; j < NP; j++) if (tmin[jr].ic[j] == ic[j][i[j]]) nshr++;
	      if (nshr >= 2) { if (dps > tmin[jr].csq) jr = -1; break; }
	    }
	    if (jr < 0) continue;
	    
	    int jc;
	    for (jc = NC; jc < jr && tmin[jc].csq > 0 && dps > tmin[jc].csq; jc++);
	    if (jc >= NC*2) continue;
	    
	    if (jr >= NC*2) jr = NC*2-1;
	    for (int j = jr-1; j >= jc; j--) tmin[j+1] = tmin[j];
	    tmin[jc].Set(dps, i, ic, cc, qc);
	    
	    TR_DEBUG_CODE_102;
	    continue;
	  }
	  
	  double dps2 = std::fabs(CY(2)-Intpol2(CZ(0), CZ(1), CZ(3), 
						CY(0), CY(1), CY(3), CZ(2)));
	  if (++hskip == 100) { 
	    hman.Fill("TfPsY0", dps1, dps2); 
	    hskip = 0; 
	  }
	  
	  if (dps2 > dps1*0.5+0.03) continue;
	  if (dps2 >  50/dps1/dps1) continue;
	  
	  if (hskip == 0) hman.Fill("TfPsY2", dps1, dps2);
	  
	  double sg[2] = { CY(1)-Intpol1(CZ(0), CZ(3), CY(0), CY(3), CZ(1)),
			   CY(2)-Intpol1(CZ(0), CZ(3), CY(0), CY(3), CZ(2)) };
	  double dsgm = (sg[0]+sg[1])/2;
	  double dsgr = ((sg[0]-dsgm)*(sg[0]-dsgm)+(sg[1]-dsgm)*(sg[1]-dsgm));
	  double csq  = dsgr/erry/erry;
	  double rgt  = 0.3/std::fabs(dsgm);
	  double cthd = (rgt != 0) ? 0.5+5/rgt/rgt : 0;
	  
	  TR_DEBUG_CODE_101;
	  if (hskip == 0) hman.Fill("TfCsq0", rgt, csq);
	  if (csq <= 0 || csq > cthd || csq < cthd*1e-6 || rgt < 0.05)
	    continue;
	  
	  int jr;
	  for (jr = 0; jr < NC && tmin[jr].csq > 0; jr++) {
	    int nshr = 0;
	    for (int j = 0; j < NP; j++)
	      if (tmin[jr].ic[j] == ic[j][i[j]]) nshr++;
	    if (nshr >= 2) { if (csq > tmin[jr].csq) jr = -1; break; }
	  }
	  if (jr < 0) continue;
	  
	  int jc;
	  for (jc = 0; jc < jr && tmin[jc].csq > 0 && csq > tmin[jc].csq; jc++);
	  if (jc >= NC) continue;
	  
	  if (jr >= NC) jr = NC-1;
	  for (int j = jr-1; j >= jc; j--) tmin[j+1] = tmin[j];
	  tmin[jc].Set(csq, rgt, dps1, dps2, i, ic, cc, qc);
	  
	  TR_DEBUG_CODE_102;
	}
      }
    }
  }
  
  if (CpuTimeUp()) {
    _CpuTime += _CheckTimer();
    if (cont_clu) delete cont_clu;
    if (cont_hit) delete cont_hit;
    if (cont_trk) delete cont_trk;
    return 0;
  }
  
  for (int j = 0; j < NC*2; j++) if (tmin[j].csq < 0) tracks_simple_stat[0]++;
  
  //////////////////// Merge Y-clusters ////////////////////

  for (int jc = 0; jc < NC*2; jc++) {
    if (tmin[jc].csq < 0) continue;
    
    AMSPoint pint[NP-1];
    if (tmin[jc].Intp(pint, zlay) < 0) continue;
    
    double rsig = std::sqrt(tmin[jc].csq);
    double rmax = msely*(0.2+0.3*rsig);
    if (tmin[jc].ic[0] == 0) rmax = tmin[jc].csq*5;
    if (rmax > msely*5) rmax = msely*5;
    
    for (int j = 1; j < NP; j++) {
      double rmin = rmax;
      tmin[jc].rmrg[j-1] = tmin[jc].qc[j+NP-1] = 0;
      for (int k = 0; k < nc[j]; k++) {
	int ai = std::abs(ic[j][k]);
	if (ai%10 == tmin[jc].ic[j]%10) continue;
	
	//double q0 = tmin[jc].qsm;
	// double qs = qc[j][k]/q0;
	//if (qs < 1/(2+20/q0) || 2+20/q0 < qs) continue; // SH-to-be-fixed
        float xxx = sqrt(tmin[jc].qsm);
        float yyy = sqrt(qc[j][k]);
        hman.Fill("ySig3",xxx,yyy);
        if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue;
	
	double r = std::fabs(cc[j][k].y()-pint[j-1].y());
	TR_DEBUG_CODE_105;
	if (r < rmin) {
	  tmin[jc].ic[j+NP-1] = ai;
	  tmin[jc].cc[j+NP-1] = cc[j][k];
	  tmin[jc].qc[j+NP-1] = qc[j][k];
	  tmin[jc].rmrg[j-1]  = rmin = r;
	}
      }
    }

    TrFit trfit;
    for (int j = 0; j < NL; j++) {
      int k;
      for (k = 0; k < NL && std::abs(tmin[jc].ic[k])%10 != j+1; k++);
      if (k == NL) continue;
      trfit.Add(tmin[jc].cc[k], errx, erry, erry);
    }
    if (trfit.SimpleFit() < 0) continue;
    
    double csq  = trfit.GetChisqY();
    double rgt  = std::fabs(trfit.GetRigidity());
    double cthd = (rgt != 0) ? 3+30/rgt/rgt : 0;
    if (cthd > 1e3) cthd = 1e3;
    if (tmin[jc].ic[0] == 0) cthd = 1;
    TR_DEBUG_CODE_103;
    if (csq > cthd || rgt < 0.05) continue;
    
    tmin[jc].csqf = csq;
    tmin[jc].rgtf = rgt;
    
    double qm = 0, qh = 0;
    int n = 0;
    for (int j = 0; j < NL; j++) {
      if (tmin[jc].ic[j] != 0) {
	qm += tmin[jc].qc[j];
	n++;
	if (qh < tmin[jc].qc[j]) qh = tmin[jc].qc[j];
      }
    }
    tmin[jc].qtm = (qm-qh)/(n-1);
  }
  
  // Sort TrSeeds
  double tc[NC*2];
  int nt = 0;
  for (int j = 0; j < NC*2; j++) {
    if (tmin[j].csqf > 0) {
      tc[j] = (tmin[j].csqf > 0.01) ? tmin[j].csqf : 0.01;
      int n = tmin[j].GetN();
      if (n == 4) tc[j] *= 10;
      if (n == 5) tc[j] *=  2;
      if (tmin[j].ic[0] == 0) tc[j] += 1e5;
      nt++; 
    }
    else tc[j] = 1e5;
  }
  
  int jci[NC*2];
  TMath::Sort(NC*2, tc, jci, kFALSE);
  TR_DEBUG_CODE_104;
  
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop ("TrTrack0Find");
#endif
  
  if (nt == 0) {
    _CpuTime += _CheckTimer();
    return 0;
  }
  
  for (int j = 0; j < NC*2; j++) if (tmin[j].csq < 0) tracks_simple_stat[1]++;
  
  //////////////////// Now with TrRecHits ////////////////////

  //////////////////// Loop on pattern candidates ////////////////////
  
  for (int j = 0; j < 4*9; j++) BTSdebug[j] = 0; 

  for (int jj = 0; jj < NC*4; jj++) {

    int trdh = 0;
    if (jj >= NC*2) {
      if (ntrack > 0 || TRCLFFKEY.NhitXForTRDTrackHelp <= 0) break;
      trdh = 1;
    }

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrTrack1Eval");
#endif
    
    int jc = jci[jj%(NC*2)];
    if (tmin[jc].csq < 0 || tmin[jc].csqf < 0) continue;
    tracks_simple_stat[2]++;
    
    // Check cpu time limit
    if ((RecPar.TrTimeLim > 0 && _CheckTimer() > RecPar.TrTimeLim)|| SigTERM) {
      _CpuTimeUp = true;
      break;
    }
    tracks_simple_stat[3]++;

    //////////////////// Fill buffers with TrRecHits ////////////////////
    
    AMSPoint ch[2][NH];
    int      ih[2][NH], nh[2];
    for (int j = 0; j < 2; j++) nh[j] = 0;

    int nhx[NL];
    int nhc = 0;
    for (int j = 0; j < NL; j++) nhx[j] = 0;

    for (int j = 0; j < nhit; j++) {
      TrRecHitR *hit = (TrRecHitR*) cont_hit->getelem(j);

      if (!hit) continue;
      if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
      if ( (hit->Used())||(hit->GetLayer()>=8) ) continue;

      int ly = hit->GetLayer();
      int ip = (ly == 1) ? 0 : ly/2;
      int is = (ly <= 3) ? 0 : 1;
      
      if ( hit->iTrCluster('y') != tmin[jc].ic[ip]/10 && 
	  (hit->iTrCluster('y') != tmin[jc].ic[ip+NP-1]/10 || ip == 0))
	continue;
      
      TrClusterR *cly = hit->GetYCluster();
      if (!cly) continue;
      
      TrClusterR *clx = hit->GetXCluster();
      if (clx) {
	double  q0 = tmin[jc].qtm;
	//double sq0 = std::pow(q0, 0.8);
	double  qs = clx->GetTotSignal()/cly->GetTotSignal();
	// SH-to-be-fixed (compatibility, done in BuildTrRecHit)
	//if (qs < 0.2*(1+0.01*sq0) || 5*(1+0.01*sq0) < qs) continue;
	hman.Fill("TfCsn3", q0, qs); 

	int il = clx->GetLayer()-1;
	if (0 <= il && il < NL)	{
	  nhx[il]++;
	  nhc++;
	}
      }
      
      bool low = false;
      //if (RecPar.TrackThrSeed[0] > 3.5 && clx && 
      //    RecPar.TrackThrSeed[0] > clx->GetSeedSN()) low = true;
      
      int sign = 1;
      if (hit->OnlyY()) { sign = -1; hit->SetDummyX(192); }
      
      for (int k = 0; k < hit->GetMultiplicity(); k++) {
	if (nh[is] >= NH) continue;
	int kk = (!low) ? k : k+50;
	ih[is][nh[is]]   = ((j*100+kk)*10+ly)*sign;
	ch[is][nh[is]++] = hit->GetCoord(k);
      }
      
      if (nh[is] >= NH) {
	static int nerr = 0;
	if (nerr++ < 20) {
	  cout << "TrRecon::BuildTrTracksSimple-W-Hit buffer size reached "
	    "Maximum of " << NH << " at Event " << GetEventID()
	       << " jc,ip,is= " << jc << " " << ip << " " << is << endl;
	  break;
	}
      }
    }

    int nlx = 0;
    for (int k = 0; k < NL; k++) if (nhx[k] > 0) nlx++;

    if (!TRCLFFKEY.AllowYonlyTracks && nlx < 2) continue;
    TR_DEBUG_CODE_110;

    if (trdh &&
	!((nhx[0] == 0 && nhc <= TRCLFFKEY.NhitXForTRDTrackHelp) ||
	  (nhx[0] == 1 && nhc <= TRCLFFKEY.NhitXForTRDTrackHelp-1))) continue;
    
    //////////////////// Search on TrRecHits ////////////////////
    
#define TX(I) ch[I%2][i[I]].x()
#define HZ(I) ch[I%2][i[I]].z()
    
    double cmin = cmaxx;
    int    nmin = 3;
    int      imin[NL];
    AMSPoint hmin[NL];
    double   dmin[NL];
    double   smin[4] = {0};
    double   pmin[2] = {0};
    
    for (int k = 0; k < NL; k++) imin[k] = 0;

    //// Use help of TRD if Tracker cannot solve multiplicity by itself
    if (trdh) {
      AMSPoint ptrd;
      AMSDir   dtrd;
      double   xtrd_match = 6;
      double   ytrd_match = 3;
      if (TRCLFFKEY.AllowYonlyTracks) { xtrd_match = 10; ytrd_match = 5; }

#ifndef __ROOTSHAREDLIBRARY__
      AMSTRDTrack *trd = (AMSTRDTrack*)AMSEvent::gethead()
	                               ->getheadC("AMSTRDTrack", 0, 1);
      for (; trd; trd = trd->next()) {
	ptrd = trd->getCooStr();
	dtrd = trd->getCooDirStr(); 
#ifdef __DUMMY_FOR_PARENTHESIS_CLOSE__
      }
#endif
#else
      AMSEventR *evt = AMSEventR::Head();
      for (int itrd = 0; evt && itrd < evt->nTrdTrack(); itrd++) {
	TrdTrackR *trd = evt->pTrdTrack(itrd);
	if (trd) {
	  ptrd = AMSPoint(trd->Coo[0], trd->Coo[1], trd->Coo[2]);
	  dtrd = AMSDir  (trd->Theta,  trd->Phi);
	}
#endif
	if (dtrd.z() == 0) continue;

	for (int k = 0; k < NL; k++) imin[k] = 0;

	for (int is = 0; is < 2; is++) {
	  for (int j = 0; j < nh[is]; j++) {
	    int il = TMath::Abs(ih[is][j])%10-1;
	    if (il < 0 || NL <= il) continue;

	    AMSPoint dtc = ch[is][j]-(ptrd+dtrd/dtrd.z()
			 *(ch[is][j].z()-ptrd.z()));
	    if (ih[is][j] < 0) hman.Fill("TfDtm1", dtc.x(), dtc.y());
	    else               hman.Fill("TfDtm2", dtc.x(), dtc.y());

	    if (TMath::Abs(dtc.x()) > xtrd_match ||
		TMath::Abs(dtc.y()) > ytrd_match) continue;

	    double d = dtc.norm();
            if (imin[il] == 0 || (imin[il] < 0 && ih[is][j] > 0)
		              || (imin[il]*ih[is][j] > 0 && d < dmin[il])) {
	      imin[il] = ih[is][j];
	      hmin[il] = ch[is][j];
	      dmin[il] = d;
	    }
	    TR_DEBUG_CODE_111;
	  }
	}

	AMSPoint p1, p2;
	int nk = 0;
	nmin = 0;
	for (int k = 0; k < NL; k++) {
	  if (imin[k] == 0) continue;
	  nk++;
	  if (imin[k] > 0) nmin++;
	  if (p1.z() == 0 || p1.z() < hmin[k].z()) p1 = hmin[k];
	  if (p2.z() == 0 || p2.z() > hmin[k].z()) p2 = hmin[k];
	}
	TR_DEBUG_CODE_112;

        if (nk == tmin[jc].GetN() && nmin == nlx) {
	  cmin = 0.1;
	  smin[0] = smin[1] = smin[2] = smin[3] = 0;
	  pmin[1] = (p2.x()-p1.x())/(p2.z()-p1.z());
	  pmin[0] = p1.x()-pmin[1]*p1.z();
	  break;
	}
	nmin = 0;
      }
    }

    for (i[0] = 0; i[0] < nh[0]; i[0]++) { // Plane 1,2 (Layer 1,2,3)
      if ( ih[0][i[0]]         <=  0) continue; // OnlyY is skipped
      if ((ih[0][i[0]]/10%100) >= 50) continue; // Low S/N is skipped
      
      for (i[1] = 0; i[1] < nh[1]; i[1]++) { // Plane 3,4 (Layer 4,5,6,7)
	if ( ih[1][i[1]]         <=  0) continue; // OnlyY is skipped
	if ((ih[1][i[1]]/10%100) >= 50) continue; // Low S/N is skipped
	
	for (i[2] = 0; i[2] < nh[0]; i[2]++) { // Plane 1,2 (Layer 1,2,3)
	  // Same layer is skipped
	  if (std::abs(ih[0][i[0]])%10 == std::abs(ih[0][i[2]])%10) continue;
	  
	  // Layer 1 needed
	  if (tmin[jc].ic[0] > 0 &&
	      std::abs(ih[0][i[0]])%10 != 1 &&
	      std::abs(ih[0][i[2]])%10 != 1) continue;
	  
	  // Pre-selection
	  double pslx = pselm;
	  double argt = tmin[jc].rgtf;
	  if (ih[0][i[2]] > 0) {
	    pslx = 0.03+0.2/argt;
	    if (pslx > pselx) pslx = pselx;
	  }
	  double dps1 = std::fabs(TX(2)-Intpol1(HZ(0), HZ(1),
						TX(0), TX(1), HZ(2)));
	  double dps2 = 0;
	  hman.Fill("XvsR1",argt,dps1);
	  if (dps1 > pslx) continue;
	  if (ih[0][i[2]] < 0) { dps2 = dps1; dps1 = 0; }
	  
	  for (i[3] = 0; i[3] < nh[1]; i[3]++) { // Plane 3,4 (Layer 4,5,6,7)
	    // Same plane is skipped
	    if ((std::abs(ih[1][i[1]])%10)/2 == (std::abs(ih[1][i[3]])%10)/2)
	      continue;
	    
	    // 2 OnlyY hits are not allowed
	    //if (ih[0][i[2]] <= 0 && ih[1][i[3]] <= 0) continue;
	    
	    // Pre-selection
	    double pslx = pselm;
	    if (ih[1][i[3]] > 0) {
	      pslx = 0.03+0.2/argt;
	      if (pslx > pselx) pslx = pselx;
	    }
	    double dps3 = std::fabs(TX(3)-Intpol1(HZ(0), HZ(1),
						  TX(0), TX(1), HZ(3)));
	    double dps4 = 0;
	    TR_DEBUG_CODE_109;
	    if (dps3 > pslx) continue;
	    if (ih[1][i[3]] < 0) { dps4 = dps3; dps3 = 0; }
	    
	    TrFit trfit;
	    for (int j = 0; j < NP; j++) {
	      double erx = (ih[j%2][i[j]] > 0) ? errx : pselm;
	      trfit.Add(ch[j%2][i[j]], erx, erry, erry);
	    }
	    if (trfit.LinearFit(1) < 0) continue;
	    
	    double csq = trfit.GetChisqX();
	    // Layer 1 is OnlyY
	    if (std::abs(ih[0][i[2]])%10 == 1 && ih[0][i[2]] < 0)
	      csq += cmaxx*0.5;
	    
	    // The other is OnlyY
	    else if (ih[0][i[2]] < 0 || ih[1][i[3]] < 0) csq += cmaxx*0.9;
	    TR_DEBUG_CODE_108;
	    
	    if (csq > cmaxx) continue;
	    
	    int      isel[NL];
	    AMSPoint hsel[NL];
	    for (int j = 0; j < NL; j++) isel[j] = 0;
	    for (int j = 0; j < NP; j++) {
	      int ly = std::abs(ih[j%2][i[j]])%10;
	      int jp = (ly == 1) ? 0 : ly/2;
	      isel[jp] = ih[j%2][i[j]];
	      hsel[jp] = ch[j%2][i[j]];
	    }
	    
	    //////////////////// Merge TrRecHits ////////////////////
	    
	    AMSPoint pfit[NL];
	    for(int j = 0; j < NL; j++) 
	      pfit[j].setp(trfit.GetParam(1)*zlay[j]+trfit.GetParam(0), 0, 0);
	    
	    double rsig = std::sqrt(tmin[jc].csqf);
	    double rmax = (mselx+rsig*0.7)*0.1;
	    if (rmax > mselx) rmax = mselx;
	    
	    for (int j = 1; j < NP; j++) {
	      double rmin = rmax;
	      tmin[jc].rmrg[j-1+NP-1] = 0;
	      for (int k = 0; k < nh[j/2]; k++) {
		if (std::abs(ih[j/2][k])%10 == std::abs(isel[j])%10 ||
		    (std::abs(ih[j/2][k])%10)/2 != j) continue;
		double r = std::fabs(ch[j/2][k].x()
				     -pfit[std::abs(ih[j/2][k])%10-1].x());
		if (ih[j/2][k] < 0) r = rmax*(0.95+0.02*r/pselm);
		if (r < rmin) {
		  isel[j+NP-1] = ih[j/2][k];
		  hsel[j+NP-1] = ch[j/2][k];
		  rmin = r;
		  if (ih[j/2][k] > 0) tmin[jc].rmrg[j-1+NP-1] = r;
		}
	      }
	    }
	    int nx = 0;
	    for (int j = 0; j < NL; j++) if (isel[j] > 0) nx++;
	    if (csq > cmin || nx < nmin) continue;
	    
	    cmin = csq;
	    nmin = nx;
	    smin[0] = dps1; smin[1] = dps2;
	    smin[2] = dps3; smin[3] = dps4;
	    pmin[0] = trfit.GetParam(0);
	    pmin[1] = trfit.GetParam(1);
	    for (int j = 0; j < NL; j++) {
	      imin[j] = isel[j]; hmin[j] = hsel[j]; }
	  }
	}
      }
    }
    
    if (cmin >= cmaxx) continue;
    tracks_simple_stat[4]++;
    
    TR_DEBUG_CODE_106;
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack1Eval");
#endif
    
    //////////////////// Fit again ////////////////////
    
    int      ifit[NL], iadd[NL];
    AMSPoint hfit[NL], hadd[NL];
    for (int j = 0; j < NL; j++) ifit[j] = iadd[j] = 0;
    
    TrFit trfit;
    for (int j = 0; j < NL; j++) {
      int k;
      for (k = 0; k < NL && std::abs(imin[k])%10 != j+1; k++);
      if (k == NL) continue;
      
      if (imin[k] < 0) hmin[k][0] = pmin[0]+pmin[1]*hmin[k].z();
      ifit[j] = imin[k]; hfit[j] = hmin[k];
      trfit.Add(hfit[j], errx, erry, erry);
    }
    if (trfit.SimpleFit() < 0) continue;
    tracks_simple_stat[5]++;
    
    double csqx = trfit.GetChisqX();
    double csqy = trfit.GetChisqY();
    double rgt3 = trfit.GetRigidity();
    if (rgt3 == 0 || csqx <= 0 || csqy <= 0) continue;
    tracks_simple_stat[6]++;
    
    //////////////////// Merge again ////////////////////
    
    double mmxx = mselx*(0.1+0.2*std::sqrt(csqx));
    double mmxy = msely*(0.2+0.2*std::sqrt(csqy));
    
    double dxmrg[NL], dymrg[NL];
    int nmrg = 0;
    
    for (int j = 0; j < NL; j++) {
      if (ifit[j] != 0) continue;
      
      AMSPoint pnt;
      trfit.Interpolate(1, &zlay[j], &pnt);
      
      AMSPoint hc;
      double xmin = mmxx, ymin = mmxy;
      int    kmin =   -1, icym =   -1;

      for (int k = 0; k < nhit; k++) {
	TrRecHitR *hit = (TrRecHitR *)cont_hit->getelem(k);

        if (!hit) continue;
        if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
        if ( (hit->Used())||(!hit->OnlyY())||(hit->GetLayer()!=j+1) ) continue;

	TrClusterR *cly = hit->GetYCluster();
	if (!cly) continue;
	
	double q0  = tmin[jc].qtm;
	double qsy = cly->GetTotSignal()/q0;
	//if (qsy < 1/(2+20/q0) || 2+20/q0 < qsy) continue; // SH-to-be-fixed
        float xxx = sqrt(tmin[jc].qtm);
        float yyy = sqrt(cly->GetTotSignal());
        hman.Fill("ySig4",xxx,yyy);
        if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue;
	
	double dy  = std::fabs(hit->GetCoord().y()-pnt.y());
	if (dy < ymin) { 
	  hman.Fill("TfCsn5", q0, qsy);
	  double xm = 50;
	  int    mm =  0;
	  for (int m = 0; m < hit->GetMultiplicity(); m++) {
	    double dx = std::fabs(hit->GetCoord(m).x()-pnt.x());
	    if (dx < xm) { xm = dx; mm = m; }
	  }
	  ymin = dy;
	  kmin = k*100+mm;
	  icym = hit->iTrCluster('y');
	  hc   = hit->GetCoord(mm);
	}
      }
      if (kmin < 0) continue;
      
      bool xmrg = false;
      for (int k = 0; k < nhit; k++) {
	TrRecHitR *hit = (TrRecHitR *)cont_hit->getelem(k);

        if (!hit) continue;
        if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
        if ( (hit->Used())||(hit->OnlyY())||(hit->GetLayer()!=j+1) ) continue;
	if (hit->GetYClusterIndex()!=icym) continue;

	TrClusterR *clx = hit->GetXCluster();
	TrClusterR *cly = hit->GetYCluster();
	if (!clx || !cly) continue;
	
	double  q0 = tmin[jc].qtm;
	// double sq0 = std::pow(q0, 0.8);
	double  qs = clx->GetTotSignal()/cly->GetTotSignal();
	// SH-to-be-fixed (compatibility, done in BuildTrRecHit)
	//if (qs < 0.2*(1+0.01*sq0) || 5*(1+0.01*sq0) < qs) continue;
	hman.Fill("TfCsn4", q0, qs);
	
	for (int m = 0; m < hit->GetMultiplicity(); m++) {
	  double dx = std::fabs(hit->GetCoord(m).x()-pnt.x());
	  if (dx < xmin) { 
	    xmin = dx;
	    kmin = k*100+m;
	    hc   = hit->GetCoord(m);
	    xmrg = true;
	  }
	}
      }
      
      dxmrg[nmrg  ] = (xmrg) ? xmin : 0;
      dymrg[nmrg++] = ymin;
      
      iadd[j] = ((xmrg) ? 1 : -1)*(kmin*10+j+1);
      hadd[j] = hc;
    }
    
    TrFit trfit2;
    if (nmrg > 0) {
      for (int j = 0; j < NL; j++) {
	if (iadd[j] == 0) continue;
	
	trfit2.Clear();
	for (int k = 0; k < NL; k++) {
	  if (k != j && ifit[k] == 0) continue;
	  int      *ii = (k == j) ? iadd : ifit;
	  AMSPoint *pp = (k == j) ? hadd : hfit;
	  double erx = (ii[k] > 0) ? errx : pselm;
	  trfit2.Add(pp[k], erx, erry, erry);
	}
	if (trfit2.SimpleFit() < 0) continue;
	if (trfit2.GetChisqY()/csqy > 10) continue;
	if (trfit2.GetChisqX()/csqx > 10 && iadd[j] > 0) iadd[j] *= -1;
	
	ifit[j] = iadd[j]; iadd[j] = 0;
	hfit[j] = hadd[j];
      }
    }
    
    //////////////////// Create a new TrTrack ////////////////////
    
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("TrTrack2Build"); 
    AMSTrTrack *track = new AMSTrTrack(0);
#else
    TrTrackR *track = new TrTrackR(0);
#endif
    
    int masky = 0x7f, maskc = 0x7f;
    for (int j = 0; j < NL; j++) {
      if (ifit[j] == 0) continue;
      
      int ihit = std::abs(ifit[j])/10;
      int imlt = ihit%100;
      if (imlt >= 50) imlt -= 50;
      
      TrRecHitR* hit = (TrRecHitR*) cont_hit->getelem(ihit/100);
      if (!hit) {
	static int nerr = 0;
	if (nerr++ < 20)
	  cout << "TrRecon::BuildTrTracksSimple-W-" 
	       << "Null hit ptr: " << ihit/100 << " " << ifit[j] 
	       << " at Event " << GetEventID()
	       << " jc,j= " << jc << " " << j << endl;
	continue;
      }

      if (!hit->OnlyY() && ifit[j] < 0) {
	for (int k = 0; k < nhit; k++) {
	  TrRecHitR *hh = (TrRecHitR*) cont_hit->getelem(k);
          if (!hh) continue;
          if ( (select_tag!=0)&&(!hh->checkstatus(select_tag)) ) continue;
	  if ((hh->OnlyY())&&(hh->GetYClusterIndex()==hit->GetYClusterIndex())) {
	    hit = hh;
	    break;
	  }
	}
      }

      hit->SetResolvedMultiplicity(imlt);
      track->AddHit(hit, imlt);
      
      unsigned int bit = 1 << (patt->GetSCANLAY()-hit->GetLayer());
      bit = ~bit;
      if (!hit->OnlyX()) masky &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
      if (!hit->OnlyY()) maskc &= ~(1 << (patt->GetSCANLAY()-hit->GetLayer()));
    }
    track->SetPatterns(patt->GetHitPatternIndex(maskc),
		       patt->GetHitPatternIndex(masky),
		       patt->GetHitPatternIndex(maskc),
		       patt->GetHitPatternIndex(masky));
    TR_DEBUG_CODE_113;

    int vertex = 0;//(TRCLFFKEY.recflag%100000 >= 10000) ? 1 : 0;
    if (vertex) {
      int fid = TrTrackR::kAlcaraz;
      if (track->FitT(fid) > 0) {
	track->Settrdefaultfit(fid);
	cont_trk->addnext(track);
	ntrack = cont_trk->getnelem();
	if (ntrack >= 3) break;
      }
      else {
	delete track;
	track = 0;
      }
    }
    else

    if (ProcessTrack(track, 0, select_tag) > 0) {

      TR_DEBUG_CODE_114;
#ifdef __ROOTSHAREDLIBRARY__
      track = (TrTrackR*) cont_trk->getelem(cont_trk->getnelem()-1);
#endif
      ntrack = cont_trk->getnelem();
      
      double q0 = tmin[jc].qtm;
      
      int ntcls = 0, ntmax = 0, nedg = 0;
      for (int j = 0; j < NL; j++) {
	TrRecHitR *hit = track->GetHitLO(j+1);
	int tkid;
	if (hit) {
	  tkid = hit->GetTkId();
	  TrClusterR *cls = hit->GetYCluster();
	  if (cls) {
	    int sadd = cls->GetSeedAddress();
	    if (sadd <= 1 || 638 <= sadd) nedg++;
	  }
	}
	else {
	  AMSPoint pnt = track->InterpolateLayerO(j+1);
	  TkSens   tks(pnt, 0);
	  if (!tks.LadFound()) continue;
	  tkid = tks.GetLadTkID();
	}
	int ntl = 0;
	for (int k = 0; k < cont_clu->getnelem(); k++) {
	  TrClusterR* cls = (TrClusterR*) cont_clu->getelem(k);
	  if (!cls || cls->GetTkId()!=tkid) continue;
	  
	  double ssn = cls->GetTotSignal();
	  if (0.2 < ssn/q0 && ssn/q0 < 10) {
	    hman.Fill("TfCsn6", q0, ssn/q0);
	    ntcls++;
	    ntl++;
	  }
	}
	if (ntl > ntmax) ntmax = ntl;
      }
      if ((ntcls >= 16 && ntmax >= 6) || 
	  ntcls >= 20 ||
	  nedg  >  0) track->setstatus(16);
      
      TR_DEBUG_CODE_107;
      
      double csq1 = tmin[jc].csq;
      double csq2 = tmin[jc].csqf;
      double rgt1 = tmin[jc].rgt;
      double rgt2 = tmin[jc].rgtf;
      double rgtf = std::fabs(track->GetRigidity());
      double csqf = track->GetNormChisqY();
      
      hman.Fill("TrNclsL", rgtf, ntcls);
      hman.Fill("TrNmaxL", rgtf, ntmax);
      hman.Fill("TrNmxcl", ntcls, ntmax);
      
      if (jj < 4) {
	float *par = &BTSdebug[jj*9];
	*(par++) = tmin[jc].ps1;       // 0
	*(par++) = tmin[jc].ps2;       // 1
	*(par++) = csq1;               // 2
	*(par++) = csq2;               // 3
	*(par++) = rgt1;               // 4
	*(par++) = rgt2;               // 5
	*(par++) = tmin[jc].rmrg[0];   // 6
	*(par++) = tmin[jc].rmrg[1];   // 7
	*(par++) = tmin[jc].rmrg[2];   // 8
      }
      
//      if (!hman.IsEnabled()) {
//	hman.Enable();
//	hman.BookHistos(0);
//      }
      if (tmin[jc].ic[0] == 0) {
	hman.Fill("TfCsq3", rgt2, csq1);
	hman.Fill("TfCsq4", rgt2, csq2);
      }
      else {
	double *qc = tmin[jc].qc;
	double qs0 = (qc[1]+qc[3])/2;
	hman.Fill("TfCsn0", qs0, qc[1]/qc[3]);
	hman.Fill("TfCsn1", qs0, qc[0]/qs0);
	hman.Fill("TfCsn1", qs0, qc[2]/qs0);
	hman.Fill("TfCsn2", qs0, qc[4]/qs0);
	hman.Fill("TfCsn2", qs0, qc[5]/qs0);
	hman.Fill("TfCsn2", qs0, qc[6]/qs0);
	
	hman.Fill("TfPsY1", tmin[jc].ps1, tmin[jc].ps2);
	hman.Fill("TfCsq1", rgt1, csq1);
	hman.Fill("TfCsq2", rgt2, csq2);
	hman.Fill("TfCsq5", std::fabs(rgt3), csqy);
	hman.Fill("TfMrg1", csq1, tmin[jc].rmrg[0]);
	hman.Fill("TfMrg1", csq1, tmin[jc].rmrg[1]);
	hman.Fill("TfMrg1", csq1, tmin[jc].rmrg[2]);
	hman.Fill("TfPsX1", rgt2, smin[0]);
	hman.Fill("TfPsX2", rgt2, smin[1]);
	hman.Fill("TfPsX1", rgt2, smin[2]);
	hman.Fill("TfPsX2", rgt2, smin[3]);
	hman.Fill("TfMrg2", csq2, tmin[jc].rmrg[3]);
	hman.Fill("TfMrg2", csq2, tmin[jc].rmrg[4]);
	hman.Fill("TfMrg2", csq2, tmin[jc].rmrg[5]);
	hman.Fill("TfRgt1", rgtf, rgt1);
	hman.Fill("TfRgt2", rgtf, rgt2);
	hman.Fill("TfCsqf", csq2, csqf);
	
	if (nmrg > 0) {
	  hman.Fill("TfCsq6", csqx, trfit2.GetChisqX());
	  hman.Fill("TfCsq7", csqy, trfit2.GetChisqY());
	}
	
	for (int j = 0; j < nmrg; j++) {
	  hman.Fill("TfMrg3", csqx, dxmrg[j]);
	  hman.Fill("TfMrg4", csqy, dymrg[j]);
	}
      }

      // more histos for charge reconstruction check
      float qx = TrCharge::GetQ(track,TrCharge::kX,1);
      float qy = TrCharge::GetQ(track,TrCharge::kY,1);
      hman.Fill("QxQy_final",qx,qy);

      for (int ihit=0; ihit<track->GetNhits(); ihit++) {
	TrRecHitR* hit = track->GetHit(ihit);
	if (hit==0) continue; 
	if (hit->OnlyY()) continue;
	TrClusterR* clX = hit->GetXCluster();
	TrClusterR* clY = hit->GetYCluster();
	if ( (clX==0)||(clY==0) ) continue;
	float sigx = clX->GetTotSignal(TrClusterR::kAsym);
	float sigy = clY->GetTotSignal(TrClusterR::kAsym);
	hman.Fill("AmpxCSx_final",sqrt(GetChargeSeed(0)),sqrt(sigx));
	hman.Fill("AmpyCSy_final",sqrt(GetChargeSeed(1)),sqrt(sigy));
	hman.Fill("AmpyAmpx_final",sqrt(sigx),sqrt(sigy));
	hman.Fill("ProbAmpx_final",sqrt(sigx),log10(hit->GetCorrelationProb()));
        float sigx_corr = clX->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain);
        float sigy_corr = clY->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain);
        hman.Fill("AmpyAmpxC_final",sqrt(sigx_corr),sqrt(sigy_corr));
        sigx_corr = clX->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle);
        sigy_corr = clY->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle);
        hman.Fill("AmpxCSxC_final",sqrt(_htmx_corr),sqrt(sigx_corr));
        hman.Fill("AmpyCSyC_final",sqrt(_htmy_corr),sqrt(sigy_corr));

      /*
      if ( (sqrt(sigx)<(0.8*sqrt(GetChargeSeed(0))-4))||
           (sqrt(sigy)<(0.8*sqrt(GetChargeSeed(1))-4)) ) {
        printf("ERROR ---   ChargeSeedX = %7.2f   ChargeSeedY = %7.2f\n",GetChargeSeed(0),GetChargeSeed(1));  
        hit->Print(0);
      }
      */
      }

#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.start("TrTrack3Extension");
#endif
      // extend the tracks to the external planes
      MergeExtHits(track, track->Gettrdefaultfit(), select_tag);

#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.stop("TrTrack3Extension");
#endif
    }  // ENDOF: if (ProcessTrack(track, 0, select_tag) > 0) {
    
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop ("TrTrack2Build");
#endif
    
    if (ntrack >= RecPar.MaxNtrack) break;
    tracks_simple_stat[6]++;
    
  } // ENDOF: for (int jc = 0; jc < NC; jc++)
  
  tracks_simple_stat[7] = ntrack;
  
  // summary
  for (int j=0; j<10; j++) hman.Fill("TrSimple",j,tracks_simple_stat[j]);
  
  _CpuTime += _CheckTimer();
  
  if (cont_clu) delete cont_clu;
  if (cont_hit) delete cont_hit;
  if (cont_trk) delete cont_trk;
  return ntrack;
}

int TrRecon::BuildTrTracks(int rebuild)
{
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack0Find");
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
  delete cont2;
#ifndef __ROOTSHAREDLIBRARY__

AMSgObj::BookTimer.stop("TrTrack0Find");
  AMSgObj::BookTimer.start("TrTrack1Eval");
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
	    SetLayerOrder(it);
	    // Scan ladder combination recursively
	    ScanRecursive(0, it);
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

#ifndef __ROOTSHAREDLIBRARY__
   AMSgObj::BookTimer.stop("TrTrack1Eval");
#endif
  return ntrack;
}


#define TR_DEBUG_CODE_201 \
if (TrDEBUG >= 3) {\
  cout << "C201 Insert: " << il << " " << hitp[i].y() << endl;\
  for (int ii = 0; ii < NL; ii++) {\
    for (int jj = 0; jj < NH; jj++) {\
      int id = idx[ii*NH+jj];\
      if (id >= 0) cout << Form(" %2d(%5.1f)", ii*NH+jj, hitp[id].y());\
    }\
    cout<< endl;\
  }\
}

#define TR_DEBUG_CODE_202 \
if (TrDEBUG >= 2) {\
  cout << "C202 nht= " << nht[1] << " " << nht[2] << " " << nht[3] << endl;\
}

#define TR_DEBUG_CODE_2021 \
  if (TrDEBUG >= 4)\
    cout << Form("C2021 %2d:%2d:%2d %2d:%2d:%2d | %6.3f %7.4f %7.4f",\
		 i11, i21, i31, i12, i22, i32, csq, par[2], par[5])\
	 << endl;

#define TR_DEBUG_CODE_203 \
if (TrDEBUG >= 2) {\
  cout << "C203";\
  for (int i = 0; i < NT; i++) cout << Form(" %2d",\
				(it1[i]>=0)?hiti[idx[it1[i]]]:-1); \
  cout << Form(" (%d %d)", nc1, nm1) << endl;\
  cout << "    ";\
  for (int i = 0; i < NT; i++) cout << Form(" %2d",\
				(it2[i]>=0)?hiti[idx[it2[i]]]:-1); \
  cout << Form(" (%d %d)", nc2, nm2);\
  cout << Form(" %6.2f %6.2f %6.1f %6.1f %6.1f",\
	       csq0, csq, par[2]*2500, par[5]*2500, par[6]);\
  if (cmin[ic] == csq) cout << " MIN " << ic << " " << nsc;\
  cout << endl;\
}

#define TR_DEBUG_CODE_205 \
if (TrDEBUG >= 1) {\
 for (int ic = 0; ic < NC; ic++) {\
  if (cmin[ic] < csth) {\
   cout << "C205 @" << GetEventID() << endl;\
   cout << "C205";\
   for (int i = 0; i < NT; i++) cout << Form(" %2d",\
		  (tmin[ic][i] >= 0) ? hiti[idx[tmin[ic][i]]] : -1); \
   cout << Form(" (%d %d)", nmin[ic][0], nmin[ic][2]) << endl;\
   cout << "    ";\
   for (int i = 0; i < NT; i++) cout << Form(" %2d",\
		  (tmin[ic][i+NT] >= 0) ? hiti[idx[tmin[ic][i+NT]]] : -1); \
   cout << Form(" (%d %d)", nmin[ic][1], nmin[ic][3]);\
   cout << Form(" %6.2f %6.1f %6.1f %6.1f",\
	        cmin[ic], pmin[ic][2]*2500, pmin[ic][5]*2500, pmin[ic][6]);\
   cout << endl;\
  }\
 }\
}

#define TR_DEBUG_CODE_2051 \
if (TrDEBUG >= 1) {\
 for (int ic = 0; ic < NC; ic++) {\
  if (cmin[ic] < csth) {\
   cout << "C2051 @" << GetEventID() << endl;\
   cout << "C2051";\
   for (int i = 0; i < NT; i++) if(tmin[ic][i] >= 0) \
     cout << Form(" %2d %6.2f %6.2f %6.2f", hiti[idx[tmin[ic][i]]],\
 	        hitp[idx[tmin[ic][i]]].x(), hitp[idx[tmin[ic][i]]].y(),\
	        hitp[idx[tmin[ic][i]]].z()) << endl;\
   cout << endl;\
   for (int i = 0; i < NT; i++) if(tmin[ic][i+NT] >= 0) \
     cout << Form(" %2d %6.2f %6.2f %6.2f", hiti[idx[tmin[ic][i+NT]]],\
	     hitp[idx[tmin[ic][i+NT]]].x(), hitp[idx[tmin[ic][i+NT]]].y(),\
	     hitp[idx[tmin[ic][i+NT]]].z()) << endl;\
  }\
 }\
}

#define TR_DEBUG_CODE_206 \
if (TrDEBUG >= 1) {\
  for (int i = 0; i < NFL; i++) {\
    cout << Form("C206 TOF%d", i+1);\
    for (int j = 0; j < ntof[i]; j++)\
      cout << Form(" %5.1f(%5.1f)%5.1f",\
		   tofx[j+i*NTF], tofe[j+i*NTF], tofz[j+i*NTF]);\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_207 \
if (TrDEBUG >= 1) {\
  cout << "nhml=";\
  for (int i = 0; i < NL; i++) cout << " " << nhml[i];\
  cout << endl;\
  cout << "is= " << is0 << " " << is1 << " " << is2 << " " << is3 << endl;\
}

#define TR_DEBUG_CODE_208 \
if (TrDEBUG >= 1) {\
  cout << Form("C208 TOF= %2d %2d %2d %2d ", i0, i1, i2, i3)\
       << Form("(%6.2f %6.2f) (%6.2f %6.2f)", x[0], z[0], x[1], z[1])\
       << Form(" sig= %5.3f %5.3f", tsig, xsig) << endl;\
}

#define TR_DEBUG_CODE_2081 \
if (TrDEBUG >= 3) {\
  cout << Form("C2081 %2d(%6.2f) %2d(%6.2f) %2d(%6.2f) %2d(%6.2f) "\
	       "%5.1f %5.2f", j0, x[2], j1, x[3], j2, x[4], j3, x[5],\
	       csq, cmin[ic]) << endl;\
}

#define TR_DEBUG_CODE_209 \
if (TrDEBUG >= 1) {\
  cout << "C209 " << i;\
  for (int j = 0; j < NT; j++) cout << Form(" %2d", tmin[i][j]);\
  cout << Form("(%d) %6.2f", nm, cmin[i]) << endl;\
}

#define TR_DEBUG_CODE_210 \
if (TrDEBUG >= 2) {\
  int nht[7] = { 0, 0, 0, 0, 0, 0, 0 };\
  int nhc[7] = { 0, 0, 0, 0, 0, 0, 0 };\
  for (int j = 0; j < nhit; j++) {\
    TrRecHitR *hh = (TrRecHitR*)cont_hit->getelem(j);\
    int il = hh->GetLayer()-1;\
    if (il < 0 || 7 <= il) continue;\
    nht[il]++;\
    if (!hh->OnlyY()) nhc[il]++;\
  }\
  cout << "Nhits= " << nhit << " | ";\
  for (int j = 0; j < 7; j++)\
    cout << Form(" %2d(%2d)", nht[j], nhc[j]);\
  cout << endl;\
  for (int j = 0; j < 7; j++) {\
    for (int k = 0; k < nhit; k++) {\
      TrRecHitR *hh = (TrRecHitR*)cont_hit->getelem(k);\
      if (hh->GetLayer() != j+1) continue;\
      cout << Form(" %4d(%d:%2d)",\
		   hh->GetTkId(), hh->OnlyY(), hh->iTrCluster('y'));\
    }\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_211 \
if (TrDEBUG >= 2) {\
  if (track->GetNhits() == 0)\
    cout << "Track: " << it << endl;\
  AMSPoint pnt = hsel->GetCoord();\
  cout << Form("  Hit: %4d %6.2f %6.2f %6.2f %d(%2d) %5.2f",\
	       hsel->GetTkId(), pnt.x(), pnt.y(), pnt.z(),\
	       hsel->OnlyY(), hsel->iTrCluster('y'), dmin) << endl;\
}


/*
// Performance comparison

      |  TRCL | Nt | Nmtr |  Ph |   ph |   Cpu  |     RETK     |     REAMS    |
------+-------+----+------+-----+------+--------+-------+------+-------+------|
ref19 |       |    |      |     |      |        |       |      |       |      |
phv4  | 11111 | 5k | 7396 |  21 | 2138 | 0.0301 | 0.035 | 1765 | 0.111 | 5556 |
phv6  | 10111 | 5k |      |  11 |  985 | 0.0041 | 0.009 |  427 | 0.079 | 3944 |
phv8  |  1111 | 5k | 3335 |     |      | 0.0229 | 0.028 | 1403 | 0.111 | 5580 |
------+-------+----+------+-----+------+--------+-------+------+-------+------|
ref21 |       |    |      |     |      |        |       |      |       |      |
phv1  | 11111 | MC | 2935 | 383 | 1946 | 0.0307 | 0.032 |  277 | 0.112 |  960 |
phv2* | 11111 | MC |      | 336 |      | 0.0042 | 0.057 |  487 | 0.127 | 1086 |
phv3  | 10111 | MC |      | 349 | 1605 | 0.0148 | 0.016 |  138 | 0.071 |  607 |
phv4  | 11111 | 5k | 7552 |  22 | 2158 | 0.0305 | 0.036 | 1778 | 0.111 | 5561 |
------+-------+----+------+-----+------+--------+-------+------+-------+------|
ref20 |       |    |      |     |      |        |       |      |       |      |
phv8* |  1111 | 5k | 3331 |     |      | 0.0107 | 0.027 | 1357 | 0.100 | 5006 |
------+-------+----+------+-----+------+--------+-------+------+-------+------|
ref2  |       |    |      |     |      |        |       |      |       |      |
phv4  | 11111 | 5k | 5579 |  12 | 1111 | 0.0302 | 0.034 | 1688 | 0.097 | 4846 |
phv5* | 11111 | 5k |      |  10 |      | 0.0099 | 0.034 | 1696 | 0.097 | 4839 |
phv6  | 10111 | 5k |      |  11 |  987 | 0.0084 | 0.013 |  647 | 0.046 | 2319 |
phv7* |  1111 | 5k | 3338 |     |      | 0.0104 | 0.028 | 1419 | 0.093 | 4625 |


*TrRecon: v1.168
*/

int TrRecon::PreselTrTracksVertex()
{
  VCon *cont_trk = GetVCon()->GetCont("AMSTrTrack");
  if (!cont_trk) return -1;

  int ntrk = cont_trk->getnelem(); delete cont_trk;
  if (ntrk == 1) return 1;

  int simple = (TRCLFFKEY.recflag%10000 >= 1000) ? 1 : 0;
  if (simple && ntrk == 0) return 1;

  if (ntrk >= 2) {
    enum { NL = 7, NP = 7+2 };
    double zvtx = 50;
    double hx1[NL], hy1[NL], hx2[NL], hy2[NL];

    TrTrackR *tmin[2] = { 0, 0 };
    double    cmin = 20;

    VCon *cont = GetVCon()->GetCont("AMSTrTrack");

    for (int i = 0; i < ntrk-1; i++) {
      TrTrackR *tr1 = (TrTrackR *)cont->getelem(i);

      int n1 = 0;
      for (int k = 0; k < tr1->GetNhits(); k++) {
	TrRecHitR *hit = tr1->GetHit(k);
	if (hit && hit->GetLayer() < 8) {
	  hx1[n1] = hit->GetCoord().z();
	  hy1[n1] = hit->GetCoord().y();
	  n1++;
	}
      }

      for (int j = i+1; j < ntrk; j++) {
	TrTrackR *tr2 = (TrTrackR *)cont->getelem(j);
	if (tr1 == tr2) continue;

	int n2 = 0;
	for (int k = 0; k < tr2->GetNhits(); k++) {
	  TrRecHitR *hit = tr2->GetHit(k);
	  if (hit && hit->GetLayer() < 8) {
	    hx2[n2] = hit->GetCoord().z();
	    hy2[n2] = hit->GetCoord().y();
	    n2++;
	  }
	}
	int ndf = n1+n2-4;
	if (ndf <= 0) continue;

	double par[NP] = { 0, 0, 0, 0, 0, 0, zvtx, 0, 0 };
	double csq = FitVtx(n1, hx1, hy1, n2, hx2, hy2, par, 1)/ndf;
	double pp  = fabs((par[2]+par[5])/(par[2]-par[5]));

	if (pp < 2 && csq < cmin) {
	  cmin = csq;
	  tmin[0] = tr1;
	  tmin[1] = tr2;
	}
      }
    }
    delete cont;

    if (tmin[0] && tmin[1]) {
      tmin[0]->setstatus(AMSDBc::TOFFORGAMMA);
      tmin[1]->setstatus(AMSDBc::TOFFORGAMMA);

      if (BuildVertex(2) == 1) return 5;
      return 0;
    }

    if (ntrk <= 2) return 1;
  }


  VCon *cont_cls = GetVCon()->GetCont("AMSTrCluster");
  if (!cont_cls) return -1;

  int ncls = cont_cls->getnelem();

  int ncly[7] = { 0, 0, 0, 0, 0, 0, 0 };
  for (int i = 0; i < ncls; i++) {
    TrClusterR *cls = (TrClusterR *)cont_cls->getelem(i); 
    if (!cls || cls->GetSide() != 1) continue;

    int il = cls->GetLayer()-1;
    if (0 <= il && il < 7) ncly[il]++;
  }
  delete cont_cls;

  for (int i = 1; i < 7; i++)
    if (ncly[i] < 1 || 10 < ncly[i]) return 0;

#ifndef __ROOTSHAREDLIBRARY__
#ifndef _STANDALONE_
  int ntrc = 0;
  for (unsigned int i = 0; i < TRDDBc::nlay(); i++) {
    AMSTRDCluster *trc = AMSTRDCluster::gethead(i);
    for (; trc; trc = trc->next()) ntrc++;
  }
  if (ntrc >= 150) return 2;

  AMSTRDTrack *trd = (AMSTRDTrack*)AMSEvent::gethead()
	                                   ->getheadC("AMSTRDTrack", 0, 1);
  int ntrd = 0;
  for (; trd; trd = trd->next()) ntrd++;
  if (ntrd > 2) return 2;

#endif //#ifndef _STANDALONE_
  AMSgObj::BookTimer.start("TrTrack0Vpsel");
#endif

  cont_cls = GetVCon()->GetCont("AMSTrCluster");

  enum { NC = 20, NP = 2, NL = 4 };

  double cly[NP*NC], clz[NP*NC];
  int    ily[NP*NC];
  int    ncl[NP] = { 0, 0 };

  for (int i = 0; i < ncls; i++) {
    TrClusterR *cls = (TrClusterR *)cont_cls->getelem(i); 
    if (!cls || cls->GetSide() != 1) continue;
    if (cls->GetLayer() < 2 || 5 < cls->GetLayer()) continue;

    int il = (cls->GetLayer()-2)/2;
    if (il < 0 || NP <= il || ncl[il] >= NC) continue;

    double cy = cls->GetGCoord(0);
    double cz = TkCoo::GetLadderCenterZ(cls->GetTkId());
    int j = 0;
    for (; j < ncl[il] && cly[il*NC+j] < cy; j++);
    for (int k = ncl[il]; k > j; k--) { ily[il*NC+k] = ily[il*NC+k-1];
                                        cly[il*NC+k] = cly[il*NC+k-1];
                                        clz[il*NC+k] = clz[il*NC+k-1]; }
    ily[il*NC+j] = cls->GetLayer();
    cly[il*NC+j] = cy;
    clz[il*NC+j] = cz;

    if (++ncl[il] >= NC) break;
  }
  delete cont_cls;

  if (ncl[0] <   2 || ncl[1] <   2 || ncl[0]+ncl[1] < 6 ||
      ncl[0] >= NC || ncl[1] >= NC) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack0Vpsel");
#endif
    return 0;
  }

  double csig =  1;
  double pthd =  0.1;
  double qthd =  1.5;
  double dthd =  1.5;
  double cthd =  2;
  double rthd = 20;
  double zref = 27.5;
  int    npat =  0;

  static bool first = true;
  if (first) {
#pragma omp critical (trrecpreselvertex)
  {if (first) {
    cout << "TrRecon::PreselTrTracksVertex-I-"
	 << "csig= " << csig << " pthd= " << pthd << " "
	 << "dthd= " << dthd << " cthd= " << cthd << " "
	 << "rthd= " << rthd << " zref= " << zref << " "
	 << "npat= " << npat << endl;
    first = false;
   }
  }}

  for (int i0 =    0; i0 <    ncl[0]-1; i0++) { if (ily[i0] <= 0) continue;
  for (int i1 =   NC; i1 < NC+ncl[1]-1; i1++) { if (ily[i1] <= 0) continue;
  for (int j0 = i0+1; j0 <    ncl[0];   j0++) { if (ily[j0] <= 0) continue;
  for (int j1 = i1+1; j1 < NC+ncl[1];   j1++) { if (ily[j1] <= 0) continue;

    double pi1 = (cly[i0]-cly[i1])/(clz[i0]-clz[i1]);
    double pj1 = (cly[j0]-cly[j1])/(clz[j0]-clz[j1]);
    if (pi1-pj1 < -pthd) continue;

    double pi0 = cly[i0]-pi1*clz[i0];
    double pj0 = cly[j0]-pj1*clz[j0];

    double cti = std::cos(std::atan(pi1));
    double ctj = std::cos(std::atan(pj1));

    double zc = (pi0-pj0)/(pj1-pi1);
    if (pi1-pj1 > 0 && (zc-clz[i0] < 0 || zc-clz[j0] < 0)) continue;

    int ni = 0, nj = 0;

    int     im0 = -1,    jm0 = -1;
    double dmi0 = dthd, dmj0 = dthd;

    for (int i = 0; i < j0; i++)
      if (i != i0 && ily[i] != ily[i0] && (zc < 0 || clz[i] < zc)) {
	double d = fabs(pi0+pi1*clz[i]-cly[i])*cti;
	if (d < dmi0) { dmi0 = d; im0 = i; }
      }
    for (int j = i0+1; j < ncl[0]; j++)
      if (j != j0 && ily[j] != ily[j0] && (zc < 0 || clz[j] < zc)) {
	double d = fabs(pj0+pj1*clz[j]-cly[j])*ctj;
	if (d < dmj0) { dmj0 = d; jm0 = j; }
      }
    if (dmi0 < dthd) ni++;
    if (dmj0 < dthd) nj++;

    int     im1 = -1,    jm1 = -1;
    double dmi1 = dthd, dmj1 = dthd;

    for (int i = NC; i < j1; i++)
      if (i != i1 && ily[i] != ily[i1]) {
	double d = fabs(pi0+pi1*clz[i]-cly[i])*cti;
	if (d < dmi1) { dmi1 = d; im1 = i; }
      }
    for (int j = i1+1; j < NC+ncl[1]; j++)
      if (j != j1 && ily[j] != ily[j1]) {
	double d = fabs(pj0+pj1*clz[j]-cly[j])*ctj;
	if (d < dmj1) { dmj1 = d; jm1 = j; }
      }
    if (dmi1 < dthd) ni++;
    if (dmj1 < dthd) nj++;

    if (ni == 0 || nj == 0) continue;

    double px[NL*2] = { clz[i0], clz[i1], (im0 >= 0) ? clz[im0] : clz[im1],
			                  (im1 >= 0) ? clz[im1] : 0,
			clz[j0], clz[j1], (jm0 >= 0) ? clz[jm0] : clz[jm1],
			                  (jm1 >= 0) ? clz[jm1] : 0 };
    double py[NL*2] = { cly[i0], cly[i1], (im0 >= 0) ? cly[im0] : cly[im1],
			                  (im1 >= 0) ? cly[im1] : 0,
			cly[j0], cly[j1], (jm0 >= 0) ? cly[jm0] : cly[jm1],
			                  (jm1 >= 0) ? cly[jm1] : 0 };

    double par[4];
    double csqi = TrRecon::FitLin(ni+2, &px[0],  &py[0],  &par[0], csig);
    double csqj = TrRecon::FitLin(nj+2, &px[NL], &py[NL], &par[2], csig);
    double dref = (pj0+pj1*zref)-(pi0+pi1*zref);
    if (csqi > cthd || csqj > cthd || dref > rthd) continue;
    if (par[1]-par[3] < -pthd*2 || qthd < par[1]-par[3]) continue;

    ily[i0] = ily[i1] = 0;
    ily[j0] = ily[j1] = 0;

    if (im0 >= 0) ily[im0] = 0; if (im1 >= 0) ily[im1] = 0;
    if (jm0 >= 0) ily[jm0] = 0; if (jm1 >= 0) ily[jm1] = 0;

    npat++;
  }}}}

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack0Vpsel");
#endif

  return (0 < npat && npat <= 2) ? 1 : 0;
}

int TrRecon::BuildTrTracksVertex(int rebuild)
{
  //////////////////// Check VCon ////////////////////

  VCon *cont_trk = GetVCon()->GetCont("AMSTrTrack");
  if (!cont_trk) return -1;

  if (rebuild) cont_trk->eraseC();
  delete cont_trk;

  VCon *cont_hit = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont_hit) return -1;

  int nhit = cont_hit->getnelem();

  if (!patt) {
    int nn = (TkDBc::Head->GetSetup()==3) ? 7 : 8;
    patt = new tkpatt(nn);
    patt->Init(nn);
  }

  //////////////////// Fill hitY buffer ////////////////////

  enum { NH = 20, NL = 4, NT = 8, NP = 7+2, NC = 3 };

  AMSPoint hitp[NH*NL];
  int      hitl[NH*NL], hiti[NH*NL], nh = 0;
  for (int i = 0; i < NH*NL; i++) { hitl[i] = hiti[i] = -1; }

  for (int i = 0; i < nhit && nh < NH*NL; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont_hit->getelem(i); 
    if (!hit || hit->GetLayer() >= 8) continue; 
#ifndef __ROOTSHAREDLIBRARY__
    if (!hit->OnlyY()) continue;
#else
    if (!hit->Used() && (hit->Used() || !hit->OnlyY())) continue;
#endif
    int iml = hit->GetResolvedMultiplicity();
    if (iml < 0) iml = hit->GetMultiplicity()-1;
    hitp[nh] = hit->GetCoord(iml);
    hitl[nh] = hit->GetLayer();
    hiti[nh] = i;
    nh++;
  }

  //////////////////// Fill index buffer ////////////////////

  int idx[NL*NH], nht[NL] = { 0, 0, 0, 0 };
  for (int i = 0; i < NL*NH; i++) idx[i] = -1;

  for (int i = 0; i < NH*NL && hitl[i] > 0; i++) {
    int il = hitl[i]/2;
    if (il < 0 || 4 <= il) continue;

    if (nht[il] < NH) {
      int ol = il*NH;
      int j = 0;
      for (; j < nht[il] && idx[ol+j] >= 0 && 
	     hitp[i].y() > hitp[idx[ol+j]].y(); j++);
      for (int k = nht[il]; k > j; k--) idx[ol+k] = idx[ol+k-1];
      idx[ol+j] = i;
      nht[il]++;
      TR_DEBUG_CODE_201;
    }
  }

  TR_DEBUG_CODE_202;
  if (nht[1] < 2 || nht[2] < 2 || nht[3] < 1) return 0;

  //////////////////// Parameters to be tuned ////////////////////

  double csig = 1.5;
  double csth =   2;
  double cxth =  10;
  double p2th = 30./2500;
  double ppth =   1;
  double zvtx =  50;

  static bool first = true;
  if (first) {
#pragma omp critical (trrecvertex)
  {if (first) {
    cout << "TrRecon::BuildTrTracksVertex-I-"
	 << "csig= " << csig << " csth= " << csth << " "
	 << "cxth= " << cxth << " p2th= " << p2th << " "
	 << "zvtx= " << zvtx << endl;
    first = false;
   }
  }}

  double cmin[NC];
  double pmin[NC][NP];
  int    tmin[NC][NT*2];
  int    nmin[NC][4];
  for (int i = 0; i < NC; i++)   { cmin[i] = csth;
    for (int j = 0; j <    4; j++) nmin[i][j] =  2;
    for (int j = 0; j < NT*2; j++) tmin[i][j] = -1; }

  //////////////////// Loops on YZ hit combinations ////////////////////

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack1Vfind");
#endif

  for (int i11 =    NH; i11 <    NH+nht[1]-1; i11++)  // L3,4
  for (int i12 = i11+1; i12 <    NH+nht[1];   i12++)
  for (int i21 =  2*NH; i21 <  2*NH+nht[2]-1; i21++)  // L5,6
  for (int i22 = i21+1; i22 <  2*NH+nht[2];   i22++)
  for (int i31 =  3*NH; i31 <= 3*NH+nht[3];   i31++)  // L7,8
  for (int i32 =  3*NH; i32 <= 3*NH+nht[3];   i32++) {
    // Check cpu time limit
    if (i21 == 2*NH && i22 == i21+1 && i31 == 3*NH && i32 == 3*NH)
      if ((RecPar.TrTimeLim > 0 &&
	   RecPar.TrTimeLim < _CheckTimer()) || SigTERM) {
	_CpuTimeUp = true;

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack1Vfind");
#endif
	return 0;
      }
    if (i31 == i32) continue;

    int nsc = 0, ic = 0;
    for (int i = 0; i < NC && nsc < 6; i++) {
      if (cmin[i] < csth) {
	int ns = 0;
	if (tmin[i][0]    == i11 || tmin[i][4]    == i11 ||
	    tmin[i][0+NT] == i11 || tmin[i][4+NT] == i11) ns++;
	if (tmin[i][0]    == i12 || tmin[i][4]    == i12 ||
	    tmin[i][0+NT] == i12 || tmin[i][4+NT] == i12) ns++;
	if (tmin[i][1]    == i21 || tmin[i][5]    == i21 ||
	    tmin[i][1+NT] == i21 || tmin[i][5+NT] == i21) ns++;
	if (tmin[i][1]    == i22 || tmin[i][5]    == i22 ||
	    tmin[i][1+NT] == i22 || tmin[i][5+NT] == i22) ns++;
	if (tmin[i][2]    == i31 || tmin[i][6]    == i31 ||
	    tmin[i][2+NT] == i31 || tmin[i][6+NT] == i31) ns++;
	if (tmin[i][2]    == i32 || tmin[i][6]    == i32 ||
	    tmin[i][2+NT] == i32 || tmin[i][6+NT] == i32) ns++;
	if (ns > nsc) { ic = i; nsc = ns; }
      }
    }
    if (nsc >= 5) continue;
    if (nsc <= 2) {
      for (int i = 0; i < NC; i++)
	if (cmin[i] == csth) { ic = i; break; }
    }

    int    it1[NT] = { i11, i21, -1, -1, -1, -1, -1, -1 };
    int    it2[NT] = { i12, i22, -1, -1, -1, -1, -1, -1 };
    int    hl1[NT] = { hitl[idx[i11]], hitl[idx[i21]], 0, 0, 0, 0, 0, 0 };
    int    hl2[NT] = { hitl[idx[i12]], hitl[idx[i22]], 0, 0, 0, 0, 0, 0 };
    double hx1[NT] = { hitp[idx[i11]].z(),
		       hitp[idx[i21]].z(), 0, 0, 0, 0, 0, 0 };
    double hy1[NT] = { hitp[idx[i11]].y(),
		       hitp[idx[i21]].y(), 0, 0, 0, 0, 0, 0 };
    double hx2[NT] = { hitp[idx[i12]].z(),
		       hitp[idx[i22]].z(), 0, 0, 0, 0, 0, 0 };
    double hy2[NT] = { hitp[idx[i12]].y(),
		       hitp[idx[i22]].y(), 0, 0, 0, 0, 0, 0 };

    int nc1 = 2;
    int nc2 = 2;
    if (i31 < 3*NH+nht[3]) { it1[2] = i31; 
                             hl1[2] = hitl[idx[i31]]; 
			     hx1[2] = hitp[idx[i31]].z();
			     hy1[2] = hitp[idx[i31]].y(); nc1++; }

    if (i32 < 3*NH+nht[3]) { it2[2] = i32;
                             hl2[2] = hitl[idx[i32]];
                             hx2[2] = hitp[idx[i32]].z();
			     hy2[2] = hitp[idx[i32]].y(); nc2++; }

    if (nc1 == 3 && (Intdiff(hx1, hy1, hx2[0], hy2[0]) < csig/5 ||
		     Intdiff(hx1, hy1, hx2[1], hy2[1]) < csig/5)) continue;
    if (nc2 == 3 && (Intdiff(hx2, hy2, hx1[0], hy1[0]) < csig/5 ||
		     Intdiff(hx2, hy2, hx1[1], hy1[1]) < csig/5)) continue;

    if (nc1 == 3 && fabs(IntPar2(hx1, hy1)) > p2th) continue;
    if (nc2 == 3 && fabs(IntPar2(hx2, hy2)) > p2th) continue;

    int ndf = nc1+nc2-4;
    if (ndf <= 0) continue;

    double par[NP] = { 0, 0, 0, 0, 0, 0, zvtx, 0, 0 };
    double csq = FitVtx(nc1, hx1, hy1, nc2, hx2, hy2, par, csig)/ndf;
    TR_DEBUG_CODE_2021;
    if (fabs( par[2]) > 1e-2 &&
	fabs( par[5]) > 1e-2 && par[2]*par[5] >    0) continue;
    if (fabs( par[2]) > p2th || fabs(par[5])  > p2th) continue;
    if (fabs((par[2]+par[5])/(par[2]-par[5])) > ppth) continue;

    if (Intdiff(&par[0], hx1[0], hy1[0], 0) >  csig &&
	Intdiff(&par[3], hx2[0], hy2[0], 0) < -csig) {
      par[6] += 20;
      csq = FitVtx(nc1, hx1, hy1, nc2, hx2, hy2, par, csig)/ndf;
      TR_DEBUG_CODE_2021;
      if (fabs( par[2]) > 1e-2 &&
	  fabs( par[5]) > 1e-2 && par[2]*par[5] > 0) continue;
    }

    int nm1 = 0, nm2 = 0;
    for (int i = 0; i < NL; i++) {
      double dmin = csig;
      int    jmin = -1;

      for (int j = i*NH; j < i*NH+nht[i]; j++) {
	if (i > 0 && j == it2[i-1]) continue;
	if (hitl[idx[j]] == hl1[i-1]) continue;
	double d1 = (nc1 == 3)
	          ? Intdiff(hx1, hy1, hitp[idx[j]].z(), hitp[idx[j]].y()) : 0;
	double d2 = Intdiff(&par[0],  hitp[idx[j]].z(), hitp[idx[j]].y());
	double d  = (d1 > 0 && d1 < d2) ? d1 : d2;
	if (d < dmin) { jmin = j; dmin = d; }
      }
      if (jmin >= 0) { hl1[3+i] = hitl[idx[jmin]]; it1[3+i] = jmin;
		       hx1[3+i] = hitp[idx[jmin]].z();
		       hy1[3+i] = hitp[idx[jmin]].y(); nm1++; }

      dmin = csig;
      jmin = -1;
      for (int j = i*NH; j < i*NH+nht[i]; j++) {
	if (i > 0 && j == it1[i-1]) continue;
	if (hitl[idx[j]] == hl2[i-1]) continue;
	double d1 = (nc2 == 3)
	          ? Intdiff(hx2, hy2, hitp[idx[j]].z(), hitp[idx[j]].y()) : 0;
	double d2 = Intdiff(&par[3],  hitp[idx[j]].z(), hitp[idx[j]].y());
	double d  = (d1 > 0 && d1 < d2) ? d1 : d2;
	if (d < dmin) { jmin = j; dmin = d; }
      }
      if (jmin >= 0) { hl2[3+i] = hitl[idx[jmin]]; it2[3+i] = jmin;
		       hx2[3+i] = hitp[idx[jmin]].z();
		       hy2[3+i] = hitp[idx[jmin]].y(); nm2++; }
    }

    double csq0 = csq;
    if (nm1 > 0 || nm2 > 0) {
      ndf = nc1+nc2+nm1+nm2-4;
      csq = FitVtx(NT, hx1, hy1, NT, hx2, hy2, par, csig)/ndf;
      TR_DEBUG_CODE_2021;
      if (fabs(par[2]) > 1e-3 &&
	  fabs(par[5]) > 1e-3 && par[2]*par[5] > 0) continue;

      int xchk = 0;
      for (int i = 0; i < 2; i++) {
	double xs = 0, xmin = 99, xmax = -99;
	int    nx = 0;
	for (int j = 0; j < NT; j++) {
	  int jt = (i == 0) ? it1[j] : it2[j];
	  if (jt >= 0) {
	    AMSPoint p = hitp[idx[jt]];
	    if (p.x() < xmin) xmin = p.x();
	    if (p.x() > xmax) xmax = p.x();
	    xs += p.x();
	    nx++;
	  }
	}
	if (nx < 2 || (xs-xmin)/(nx-1)*xmin < -20
	           || (xs-xmax)/(nx-1)*xmax < -20) {
	  xchk = 1;
	  break;
	}
      }
      if (xchk) continue;
    }

    int cut = 0;
    for (int l = 7; l >= 4 ; l--) {
      double y1 = -99, y2 = 99;
      for (int j = 0; j < NT; j++) if (hl1[j] == l) y1 = hy1[j];
      for (int j = 0; j < NT; j++) if (hl2[j] == l) y2 = hy2[j];
      if (y2 < y1-0.5) { cut = cut*10+l; }
    }

    if ((csq < cmin[ic] && ((nc1 >= nmin[ic][0] && nc2 >= nmin[ic][1]  &&
			     nm1 >= nmin[ic][2] && nm2 >= nmin[ic][3]) ||
			     nc1+nc2+nm1+nm2 >= 12)) ||
	(cut == 0 &&
	 csq < csth && csq < cmin[ic]+csth/2 &&((nm1 >  nmin[ic][2]  &&
						 nm2 >= nmin[ic][3]) ||
						(nm1 >= nmin[ic][2]  &&
						 nm2 >  nmin[ic][3])))
      ) {

      cmin[ic] = csq;
      for (int i = 0; i < NT; i++) tmin[ic][i]    = it1[i];
      for (int i = 0; i < NT; i++) tmin[ic][i+NT] = it2[i];
      for (int i = 0; i < NP; i++) pmin[ic][i]    = par[i];
      nmin[ic][0] = nc1; nmin[ic][1] = nc2;
      nmin[ic][2] = nm1; nmin[ic][3] = nm2;

      TR_DEBUG_CODE_203;
    }
  }

#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("TrTrack1Vfind");
#endif

  if (cmin[0] == csth) return 0;

  TR_DEBUG_CODE_205;
  TR_DEBUG_CODE_2051;

  //////////////////// Fill TOF buffer ////////////////////

  enum { NTF = 5, NFL = 4 };

  double tofx[NTF*NFL], tofz[NTF*NFL], tofe[NTF*NFL];
  int    ntof[NFL] = { 0, 0, 0, 0 };

#ifndef __ROOTSHAREDLIBRARY__
#ifndef _STANDALONE_
  for (int i = 0; i < NFL; i++) {
    for (AMSTOFCluster *tof
	   = AMSTOFCluster::gethead(i); tof; tof = tof->next())
    if (ntof[i] < NTF) {
      tofx[ntof[i]+NTF*i] = tof->getcoo ().x();
      tofz[ntof[i]+NTF*i] = tof->getcoo ().z();
      tofe[ntof[i]+NTF*i] = tof->getecoo().x();
      ntof[i]++;
    }
  }
#endif // _STANDALONE_
#else
  AMSEventR *evt = AMSEventR::Head();
  for (int i = 0; evt && i < evt->nTofCluster(); i++) {
    TofClusterR *tof = evt->pTofCluster(i);
    int il = (tof) ? tof->Layer-1 : -1;
    if (0 <= il && il < NFL && ntof[il] < NTF) {
      tofx[ntof[il]+NTF*il] = tof->Coo[0];
      tofz[ntof[il]+NTF*il] = tof->Coo[2];
      tofe[ntof[il]+NTF*il] = tof->ErrorCoo[0];
      ntof[il]++;
    }
  }
#endif

  TR_DEBUG_CODE_206;
  if ((ntof[0] == 0 && ntof[1] == 0) ||
      (ntof[2] == 0 && ntof[3] == 0)) return 0;

  //////////////////// Fill XZ buffer ////////////////////

  enum { NX = 200 };

  double hitx[NX*NL], hitz[NX*NL];
  int    nhml[NL];

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack1Vhmlt");
#endif

  for (int ic = 0; ic < NC; ic++) {

  for (int i = 0; i < NL;    i++) nhml[i] =  0;

   if (cmin[ic] == csth) continue;
   for (int i = 0; i < NT*2; i++) {
    if (tmin[ic][i] < 0) continue;

    TrRecHitR *hit = (TrRecHitR*)cont_hit->getelem(hiti[idx[tmin[ic][i]]]);
    if (!hit) continue;

    int tkid = hit->GetTkId();
    int il   = hit->GetLayer()/2;
    for (int j = 0; j < nhit && nhml[il] < NX; j++) {
      TrRecHitR *hh = (TrRecHitR*)cont_hit->getelem(j);
      if (hh->OnlyY() || hh->GetTkId() != tkid) continue;

      int k1 = 0, k2 = hh->GetMultiplicity()-1;
      if (hh->GetResolvedMultiplicity() >= 0)
	k1 = k2 = hh->GetResolvedMultiplicity();

      for (int k = k1; k <= k2 && nhml[il] < NX; k++) {
	AMSPoint coo = hh->GetCoord(k);
	hitx[nhml[il]+il*NX] = coo.x();
	hitz[nhml[il]+il*NX] = coo.z();
	nhml[il]++;
      }
    }
   }

  int is0 = -1, is1 = -1, is2 = -1, is3 = -1;
  for (int i =    0; i < NL && is0 < 0; i++) if (nhml[i] > 0) is0 = i;
  for (int i = NL-1; i >= 0 && is1 < 0; i--) if (nhml[i] > 0) is1 = i;
  if (is0 < 0 || is1 < 0 || is0 == is1) {
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrTrack1Vhmlt");
#endif
    return 0;
  }

  for (int i = 0; i < NL && is2 < 0; i++)
    if (nhml[i] > 0 && i != is0 && i != is1) is2 = i;

  for (int i = 0; i < NL && is3 < 0; i++)
    if (nhml[i] > 0 && i != is0 && i != is1
	                        && i != is2) is3 = i;
  TR_DEBUG_CODE_207;

  //////////////////// Loops on XZ hit combinations ////////////////////

  cmin[ic] = cxth;

  // Loop on UTOF combinations
  for (int i0 =     0; i0 <=     ntof[0]; i0++)
  for (int i1 =   NTF; i1 <= NTF+ntof[1]; i1++) {
    if (i0 == ntof[0] && i1 == NTF+ntof[1]) continue;

    if (i0 <     ntof[0] && tofe[i0] > 10) continue;
    if (i1 < NTF+ntof[1] && tofe[i1] > 10) continue;

    double teru = (i0 ==     ntof[0]) ? tofe[i1] :
                 ((i1 == NTF+ntof[1]) ? tofe[i0]
		  : TMath::Sqrt(tofe[i0]*tofe[i0]+tofe[i1]*tofe[i1]));
    if (i0 < ntof[0] && i1 < NTF+ntof[1] && 
	fabs(tofx[i0]-tofx[i1]) > teru*2) continue;

  // Loop on LTOF combinations
  for (int i2 = 2*NTF; i2 <= 2*NTF+ntof[2]; i2++)
  for (int i3 = 3*NTF; i3 <= 3*NTF+ntof[3]; i3++) {
    if (i2 == 2*NTF+ntof[2] && i3 == 3*NTF+ntof[3]) continue;

    if (i2 < 2*NTF+ntof[2] && tofe[i2] > 10) continue;
    if (i3 < 3*NTF+ntof[3] && tofe[i3] > 10) continue;

    double terl = (i2 == 2*NTF+ntof[2]) ? tofe[i3] :
                 ((i3 == 3*NTF+ntof[3]) ? tofe[i2]
		  : TMath::Sqrt(tofe[i2]*tofe[i2]+tofe[i3]*tofe[i3]));
    if (i2 < 2*NTF+ntof[2] && i3 < 3*NTF+ntof[3] &&
	fabs(tofx[i2]-tofx[i3]) > terl*2) continue;

    double x[2+NL], z[2+NL];
    x[0] = (i0 ==       ntof[0]) ? tofx[i1] :
          ((i1 ==   NTF+ntof[1]) ? tofx[i0] : (tofx[i0]+tofx[i1])/2);
    z[0] = (i0 ==       ntof[0]) ? tofz[i1] :
          ((i1 ==   NTF+ntof[1]) ? tofz[i0] : (tofz[i0]+tofz[i1])/2);
    x[1] = (i2 == 2*NTF+ntof[2]) ? tofx[i3] :
          ((i3 == 3*NTF+ntof[3]) ? tofx[i2] : (tofx[i2]+tofx[i3])/2);
    z[1] = (i2 == 2*NTF+ntof[2]) ? tofz[i3] :
          ((i3 == 3*NTF+ntof[3]) ? tofz[i2] : (tofz[i2]+tofz[i3])/2);

    double tsig = TMath::Sqrt((teru*teru+terl*terl)*2);
    double xsig = TMath::Sqrt(tsig*tsig/4+csig*csig);
    TR_DEBUG_CODE_208;

    // Check cpu time limit
    if ((RecPar.TrTimeLim > 0 &&
	 RecPar.TrTimeLim < _CheckTimer()) || SigTERM) {
      _CpuTimeUp = true;

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrTrack1Vhmlt");
#endif
	return 0;
    }

    // Loop on hitx combinations
    for (int j0 = is0*NX; j0 < is0*NX+nhml[is0]; j0++) {
      x[2] = hitx[j0]; z[2] = hitz[j0];
      if (fabs(x[2]-Intpol1(z[0], z[1],
				  x[0], x[1], z[2])) > tsig) continue;

    for (int j1 = is1*NX; j1 < is1*NX+nhml[is1]; j1++) {
      x[3] = hitx[j1]; z[3] = hitz[j1];
      if (fabs(x[3]-Intpol1(z[0], z[1],
				  x[0], x[1], z[3])) > tsig) continue;

    int j20 = (is2 >= 0) ? is2*NX           : 0;
    int n2  = (is2 >= 0) ? is2*NX+nhml[is2] : 1;
    for (int j2 = j20; j2 < n2; j2++) {
      int n = 4;
      if (is2 >= 0) {
	x[4] = hitx[j2]; z[4] = hitz[j2]; n = 5;
	if (fabs(x[4]-Intpol1(z[2], z[3],
				    x[2], x[3], z[4])) > xsig) continue;
      }

      int j30 = (is3 >= 0) ? is3*NX           : 0;
      int n3  = (is3 >= 0) ? is3*NX+nhml[is3] : 1;
      for (int j3 = j30; j3 < n3; j3++) {
	if (n >= 5 && is3 >= 0) {
	  x[5] = hitx[j3]; z[5] = hitz[j3]; n = 6;
	  if (fabs(x[5]-Intpol1(z[2], z[3],
				      x[2], x[3], z[5])) > xsig) continue;
	}
	double par[2];
	double csq = FitLin(n, z, x, par, xsig);
	TR_DEBUG_CODE_2081;

	if (csq < cmin[ic]) {
	  cmin[ic] = csq;
	  pmin[ic][7] = par[0];
	  pmin[ic][8] = par[1];
	}
   }}}} // j0-j3

   }} // i1, i3
  } // ic
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrTrack1Vhmlt");
#endif

  int ic = -1, nmax = 0;
  for (int i = 0; i < NC; i++) {
    if (cmin[i] >= cxth) continue;
    int nm = nmin[i][0]+nmin[i][1]+nmin[i][2]+nmin[i][3];
    TR_DEBUG_CODE_209;
    if (nm > nmax) {
      ic   = i;
      nmax = nm;
    }
  }
  if (ic < 0) return 0;

  //////////////////// Create new TrTracks ////////////////////

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack2Build"); 
#endif

  for (int it = 0; it < 2; it++) {
#ifndef __ROOTSHAREDLIBRARY__
   AMSTrTrack *track = new AMSTrTrack(0);
#else
   TrTrackR   *track = new TrTrackR(0);
#endif
   track->setstatus(AMSDBc::TOFFORGAMMA);
   track->setstatus(AMSDBc::RECOVERED);

   int msel[7] = { -1, -1, -1, -1, -1, -1, -1 };

   int masky = 0x7f;
   int maskc = 0x7f;

   TR_DEBUG_CODE_210;
   for (int i = 0; i < NT; i++) {
    int  lay = i+1;
    int jofs = it*NT;

    TrRecHitR *hit = 0;
    for (int j = 0; j < NT; j++) {
      if (tmin[ic][j+jofs] < 0) continue;

      int ih = hiti[idx[tmin[ic][j+jofs]]];
      TrRecHitR *hh = (TrRecHitR*)cont_hit->getelem(ih);
      if (hh && hh->GetLayer() == lay) { hit = hh; break; }
    }
    if (!hit) continue;

    int tkid = hit->GetTkId();
    int il   = hit->GetLayer()-1;
    if (il < 0 || 7 <= il) continue;

    double     dmin = 0;
    TrRecHitR *hsel = 0;
    TrClusterR *cly = hit->GetYCluster();
    if (!cly) continue;

    for (int j = 0; j < nhit; j++) {
      TrRecHitR *hh = (TrRecHitR*)cont_hit->getelem(j);
      if (hh->GetTkId() != tkid || hh->GetYCluster() != cly) continue;
      if (hsel && hh->OnlyY()) continue;

      int nml = hh->GetMultiplicity();
      for (int k = 0; k < nml; k++) {
	AMSPoint coo = hh->GetCoord(k);
	double d = fabs(pmin[ic][7]+pmin[ic][8]*coo.z()-coo.x());
	if (hh->OnlyY()) d += 100;
	if (dmin == 0 || d < dmin) {
	  dmin = d; msel[il] = k; hsel = hh;
	}
      }
    }
    if (!hsel) hsel = hit;

  //if (msel >= 0) hsel->SetResolvedMultiplicity(msel);
    TR_DEBUG_CODE_211;

    track->AddHit(hsel);

    if (!hsel->OnlyX()) masky &= ~(1 << (patt->GetSCANLAY()-hsel->GetLayer()));
    if (!hsel->OnlyY()) maskc &= ~(1 << (patt->GetSCANLAY()-hsel->GetLayer()));
   }

   TrTrackR *tshr = 0;
   int       nshr = 0;
   VCon *cont = GetVCon()->GetCont("AMSTrTrack");
   for (int i = 0; i < cont->getnelem(); i++) {
     TrTrackR *trk = (TrTrackR *)cont->getelem(i);
     int ns = 0;
     for (int j = 0; j < trk->GetNhits(); j++)
       for (int k = 0; k < track->GetNhits(); k++) {
	 if (trk  ->GetHit(j)->iTrCluster('y') == 
	     track->GetHit(k)->iTrCluster('y')) ns++;
       }
     if (ns > nshr) { tshr = trk; nshr = ns; }
   }
   delete cont;

 //if (tshr && tshr != tref && nshr > 2) {
   if (tshr && nshr > 2) {
     tshr->setstatus(AMSDBc::TOFFORGAMMA);
     delete track;
     continue;
   }

   for (int j = 0; j < 7; j++) {
     TrRecHitR *hit = track->GetHitLO(j+1);
     if (hit && hit->GetResolvedMultiplicity() < 0 && msel[j] >= 0)
       hit->SetResolvedMultiplicity(msel[j]);
   }

   track->SetPatterns(patt->GetHitPatternIndex(maskc),
		      patt->GetHitPatternIndex(masky),
		      patt->GetHitPatternIndex(maskc),
		      patt->GetHitPatternIndex(masky));

   double msave = TrTrackR::DefaultMass;
   TrTrackR::DefaultMass = 511e-3;

   if (track->GetNhits() == 3) {
     int mfit[3] = { TrTrackR::kSimple, TrTrackR::kAlcaraz,
		     TrTrackR::kChoutko };
     int mfit1 = -1;
     for (int i = 0; i < 3; i++) {
       float ret = track->FitT(mfit[i]);
       if (ret > 0) track->Settrdefaultfit(mfit1 = mfit[i]);
     }
     if (mfit1 > 0) {
       track->RecalcHitCoordinates(mfit1);
       track->EstimateDummyX(mfit1);
       track->UpdateBitPattern();
       if (track->DoAdvancedFit()) {
	 if (TrDEBUG >= 1) printf(" Track Advanced Fits Done!\n");
       }
       if (track->Gettrdefaultfit() != TrTrackR::DefaultFitID &&
	   track->FitDone(TrTrackR::DefaultFitID))
	 track->Settrdefaultfit(TrTrackR::DefaultFitID);

       VCon *cont = GetVCon()->GetCont("AMSTrTrack");
       if (cont) cont->addnext(track);
       delete cont;
     }
   }
   else ProcessTrack(track, 0);

   TrTrackR::DefaultMass = msave;
  }

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop ("TrTrack2Build");
#endif

  delete cont_hit;

  return 1;
}

double TrRecon::FitLin(int n, double *x, double *y, double *par, double sig)
{
  double mtx[2][2] = { { 0, 0 }, { 0, 0 } }, minv[2][2];
  double vec[2] = { 0, 0 };
  for (int i = 0; i < n; i++) {
    mtx[0][0] += 1;    mtx[0][1] += x[i];
    mtx[1][0] += x[i]; mtx[1][1] += x[i]*x[i];
    vec[0]    += y[i]; vec[1]    += y[i]*x[i];
  }

  double det = mtx[0][0]*mtx[1][1]-mtx[0][1]*mtx[1][0];
  if (det == 0) return -1;
  minv[0][0] = mtx[1][1]/det; minv[0][1] = -mtx[0][1]/det;
  minv[1][1] = mtx[0][0]/det; minv[1][0] = -mtx[1][0]/det;

  par[0] = minv[0][0]*vec[0]+minv[0][1]*vec[1];
  par[1] = minv[1][0]*vec[0]+minv[1][1]*vec[1];

  double chisq = 0;
  for (int i = 0; i < n; i++) {
    double res = y[i]-(par[0]+par[1]*x[i]);
    chisq += res*res/sig/sig;
  }
  return chisq;
}

double TrRecon::FitVtx(int n1, double *x1, double *y1,
		       int n2, double *x2, double *y2, double *par, double sig)
{
  if (n1 < 2 || n2 < 2) return -1;

  enum { NP = 4 };

  double a = par[6];

  if (0) { //(n1 == 2 && n2 == 2) {
    double aa = (y1[0]-y1[1])/(x1[0]-x1[1]);
    double bb =  x1[0]+x1[1];
    double m[4] = { x1[0]*x1[0] +a*a -2*a*x2[0] -bb*(x1[0]-x2[0]),
		                -a*a +2*a*x2[0]     -x2[0]*x2[0],
		    x1[1]*x1[1] +a*a -2*a*x2[1] -bb*(x1[1]-x2[1]),
		                -a*a +2*a*x2[1]     -x2[1]*x2[1] };
    double v[2] = { y1[0]-y2[0]                 -aa*(x1[0]-x2[0]),
		    y1[1]-y2[1]                 -aa*(x1[1]-x2[1]) };
    double det = m[0]*m[3]-m[1]*m[2];
    if (det == 0) return -1;

    double mv[4] = { m[3]/det, -m[1]/det, -m[2]/det, m[0]/det };

    par[2] = mv[0]*v[0]+mv[1]*v[1];
    par[5] = mv[2]*v[0]+mv[3]*v[1];
    par[0] = y1[0]-par[1]*x1[0]-par[2]*x1[0]*x1[0];
    par[1] = aa-bb*par[2];
    par[3] = par[0]-a*a*(par[2]-par[5]);
    par[4] = par[1]+2*a*(par[2]-par[5]);

    return 0;
  }

  double xi0 = 0, xi1 = 0, xi2 = 0, xi3 = 0, xi4 = 0;
  double yi0 = 0, yi1 = 0, yi2 = 0;
  double xj0 = 0, xj1 = 0, xj2 = 0, xj3 = 0;
  double yj0 = 0, yj1 = 0;
  double aj0 = 0, aj1 = 0, aj2 = 0, ajy = 0;
  double bj0 = 0, bj1 = 0, bj2 = 0, bjy = 0;

  for (int i = 0; i < n1; i++) {
    if (x1[i] == 0 && y1[i] == 0) continue;

    double x = 1, y = y1[i];
    xi0 += x; yi0 += y; x *= x1[i]; y *= x1[i];
    xi1 += x; yi1 += y; x *= x1[i]; y *= x1[i];
    xi2 += x; yi2 += y; x *= x1[i];
    xi3 += x;           x *= x1[i];
    xi4 += x;
  }
  for (int i = 0; i < n2; i++) {
    if (x2[i] == 0 && y2[i] == 0) continue;

    double x = 1, y = y2[i];
    xj0 += x; yj0 += y; x *= x2[i]; y *= x2[i];
    xj1 += x; yj1 += y; x *= x2[i];
    xj2 += x;           x *= x2[i];
    xj3 += x;

    double aa = -a*a+2*a*x2[i];
    double bb =  a*a-2*a*x2[i]+x2[i]*x2[i];
    aj0 += aa;             bj0 += bb;
    aj1 += aa*x2[i];       bj1 += bb*x2[i];
    aj2 += aa*x2[i]*x2[i]; bj2 += bb*x2[i]*x2[i];
    ajy += aa*y2[i];       bjy += bb*y2[i];
  }

  double mtx[NP*NP] =
    { xi0 +xj0,  xi1 +xj1,  xi2 -a*a*xj0 +2*a*xj1,  a*a*xj0 -2*a*xj1 +xj2,
      xi1 +xj1,  xi2 +xj2,  xi3 -a*a*xj1 +2*a*xj2,  a*a*xj1 -2*a*xj2 +xj3,
      xi2 +aj0,  xi3 +aj1,  xi4 -a*a*aj0 +2*a*aj1,  a*a*aj0 -2*a*aj1 +aj2,
	   bj0,       bj1,      -a*a*bj0 +2*a*bj1,  a*a*bj0 -2*a*bj1 +bj2 };
  double vec[NP] = { yi0+yj0,  yi1+yj1,  yi2+ajy,  bjy };

  int ret = TrFit::Inv44((double(*)[4])mtx);
  if (ret < 0) return -1;

  for (int i = 0; i < NP; i++) {
    par[i] = 0;
    for (int j = 0; j < NP; j++) par[i] += mtx[i*NP+j]*vec[j];
  }
  par[5] = par[3];
  par[3] = par[0]-a*a*(par[2]-par[5]);
  par[4] = par[1]+2*a*(par[2]-par[5]);

  double chisq = 0;
  for (int i = 0; i < n1; i++) {
    if (x1[i] == 0 && y1[i] == 0) continue;
    double res = y1[i]-(par[0] +par[1]*x1[i] +par[2]*x1[i]*x1[i]);
    chisq += res*res/sig/sig;
  }
  for (int i = 0; i < n2; i++) {
    if (x2[i] == 0 && y2[i] == 0) continue;
    double res = y2[i]-(par[3] +par[4]*x2[i] +par[5]*x2[i]*x2[i]);
    chisq += res*res/sig/sig;
  }

  return chisq;
}

int TrRecon::RecoverExtHits()
{
#ifdef __ROOTSHAREDLIBRARY__
  AMSEventR *ev = AMSEventR::Head();
  if (!ev) return -1;
  if (!ev->pParticle(0) || !ev->pParticle(0)->pTrTrack()) return 0;

  TrTrackR *trk = ev->pParticle(0)->pTrTrack();
  if (trk->HasExtLayers() == 0 || 
     (trk->GetBitPatternXY()&0x180) == 3) return 0;

  int DEBUG = TrDEBUG;

  vector<TrRawClusterR> &vr = ev->TrRawCluster();
  vr.clear();

  int ntyp = TRMCFFKEY.NoiseType; TRMCFFKEY.NoiseType = 0;
  TrSim::sitkdigi();
  TRMCFFKEY.NoiseType = ntyp;

  int nr = 0;
  for (vector<TrRawClusterR>::iterator i = vr.begin(); i != vr.end();) {
    if (i->GetLayer() < 8 || i->GetSide() != 0) i = vr.erase(i);
    else { ++i; nr++; }
  }
  if (DEBUG) cout << "TrRecon::RecoverExtHits-I-nraw= " << nr << endl;
  if (nr == 0) return 0;

  vector<TrClusterR> &vc = ev->TrCluster();
  vector<TrRecHitR>  &vh = ev->TrRecHit();
  int nc0 = ev->nTrCluster();
  int nh0 = ev->nTrRecHit();

  FillChargeSeeds();
  if (DEBUG) cout << "TrRecon::RecoverExtHits-I-htm= "
		  << sqrt(_htmx) << " " << sqrt(_htmy) << endl;

  BuildTrClusters(0);

  if (DEBUG) cout << "TrRecon::RecoverExtHits-I-ncls= "
		  << ev->NTrCluster() << " " << nc0 << endl;
  if (ev->NTrCluster() <= nc0) return 0;

  vector<TrClusterR>::iterator i0 = vc.begin();
  advance(i0, nc0);

  for (vector<TrClusterR>::iterator i = i0; i != vc.end();) {
    if (i->GetTotSignal() < 100 ||  // Small tuning
	(TRCLFFKEY.ChargeSeedTagActive && 
	 !CompatibilityWithChargeSeed(&*i))) { i = vc.erase(i); continue; }
    int tk = i->GetTkId();
    int a1 = i->GetAddress(0), a2 = i->GetAddress(i->GetNelem()-1);
    for (vector<TrRecHitR>::iterator j = vh.begin(); j != vh.end(); ++j)
      if (!j->OnlyY() && j->GetTkId() == tk) {
	TrClusterR *xc = j->GetXCluster();
	if (xc && a1 <= xc->GetAddress(xc->GetNelem()-1) &&
	                xc->GetAddress(0) <= a2) {
	  i = vc.erase(i); tk = 0; break;
	}
      }
    if (tk == 0) continue;

    for (vector<TrClusterR>::iterator j = vc.begin(); j != i0; ++j) {
      if (j->GetTkId() == tk && j->GetSide() == 1) {
	TrRecHitR hit(tk, &(*i), &(*j), 0);
	float prob = hit.GetCorrelationProb();
	if (DEBUG >= 2)
	  cout << Form("hit %4d %5.3f %d %5.1f %5.1f", tk, prob,
		       CompatibilityWithChargeSeed(&hit),
		       i->GetTotSignal(), j->GetTotSignal()) << endl;
        if (prob > TRCLFFKEY.CorrelationProbThr && 
	    (TRCLFFKEY.ChargeSeedTagActive == 0 ||
	     CompatibilityWithChargeSeed(&hit))) {
	      hit.setstatus(TrRecHitR::ZSEED);
	      vh.push_back(hit);
	}
      }
    }
    ++i;
  }

  if (DEBUG) cout << "TrRecon::RecoverExtHits-I-nhit= " 
		  << vh.size() << " " << nh0 << endl;

  if (vh.size() == nh0) return 0;

  int nm = 0;
  for (int i = 0; i < ev->nTrTrack(); i++) {
    TrTrackR *trk = ev->pTrTrack(i);
    int bx0 = (trk->GetBitPatternXY()&0x180)>>7;

    int ifit = TrTrackR::DefaultFitID;
    if (trk->ParExists(ifit)) {
      TrRecHitR *h1 = trk->GetHitLJ(1); if (h1) h1->ClearUsed();
      TrRecHitR *h9 = trk->GetHitLJ(9); if (h9) h9->ClearUsed();
      MergeExtHits(trk, ifit, TrRecHitR::ZSEED);
    }

    int bx1 = (trk->GetBitPatternXY()&0x180)>>7;
    if (bx1 > bx0) nm++;

    if (DEBUG) cout << "TrRecon::RecoverExtHits-I-bx= " 
		    << bx0 << " " << bx1 << endl;

    if (bx1 == 3) {
      TrRecHitR *h1 = trk->GetHitLJ(1);
      TrRecHitR *h9 = trk->GetHitLJ(9);
      if (DEBUG) cout << "TrRecon::RecoverExtHits-I-qq= " 
		      << ev->Event() << " "
		      << bx0 << " " << bx1 << " "
		      << h1->GetXCluster()->GetTotSignal() << " "
		      << h9->GetXCluster()->GetTotSignal() << " "
		      << h1->GetCorrelationProb() << " "
		      << h9->GetCorrelationProb() << endl;
    }
  }

  return nm;

#endif
  return 0;
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
      if (!thit) continue;
      if (!thit->Used()) thit->SetUsed();
      TrClusterR* yclus= thit->GetYCluster();
      if (!yclus) continue;
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

  // Check hit indexes
  for (int i = 0; i < ntrack; i++) {
    TrTrackR *track = (TrTrackR*)contT->getelem(i);

    for(int j=0;j<track->GetNhits();j++)
      if (track->iTrRecHit(j) < 0) {
	static int nerr = 0;
	if (nerr++ < 100)
	  cerr << "TrRecon::PurgeGhostHits-W- wrong hit index: "
	       << "at Event " << GetEventID()
	       << " track " << i << " hit " << j << " " << endl;
      }
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




int   TrRecon::fTrackCounter[TrRecon::NTrackCounter] = { -1 };
float TrRecon::_CpuTimeTotal = 0;

int TrRecon::CountTracks(int trstat)
{
  VCon* cont = GetVCon()->GetCont("AMSTrTrack");
  if (!cont) return trstat;

#pragma omp critical (trcount)
 {
  // Counter initialization
  if (fTrackCounter[0] < 0) {
    for (int i = 0; i < NTrackCounter; i++) fTrackCounter[i] = 0;
  }
  int ntrk = cont->getnelem();

  fTrackCounter[0]++;
  _CpuTimeTotal += _CpuTime;

 int simple = (TRCLFFKEY.recflag%10000  >=  1000) ? 1 : 0;
 int vertex = (TRCLFFKEY.recflag%100000 >= 10000) ? 1 : 0;
 if (vertex && !simple) {
   if (trstat & 0x0020) fTrackCounter[1]++;  // PreselTrTracksVertex
   if (trstat & 0x0040) fTrackCounter[2]++;  // Multi-TOF/TRD
   if (trstat & 0x0080) fTrackCounter[3]++;  // Multi-TrTrack
   if (trstat & 0x0100) fTrackCounter[4]++;  // HighC-TrTrack
   if (trstat & 0x2000) fTrackCounter[5]++;  // BuildTrTracksVertex
   if (trstat & 0x01c0) fTrackCounter[6]++;
 }
 else {
  // Fitting algorithm for iTrTrackPar
  int malgo = 1;  // 1: Choutko, 2: Alcaraz, +10: no-MSC, +20: same-weight

  for (int itrk = 0; itrk < ntrk; itrk++) {
    TrTrackR *trk = (TrTrackR*)cont->getelem(itrk);

    int mfi = trk->iTrTrackPar(malgo, 3, 0); // Inner fitting
    if (mfi < 0 || trk->GetRigidity(0) == 0) continue;

    double csqx = trk->GetNormChisqX(mfi);
    double csqy = trk->GetNormChisqY(mfi);
    double argt = fabs(trk->GetRigidity(mfi));
    if (csqx < 0 || csqy < 0 || argt == 0) continue;

    // Look for a track with at least one hit at all the INNER PLANES
    int span = (TrTrackSelection::GetSpanFlags(trk) & 0xff);
    if (!(trk->GetBitPattern() & 0x006) ||
	!(trk->GetBitPattern() & 0x018) ||
	!(trk->GetBitPattern() & 0x060)) continue;

    int spflag[4] = { TrTrackSelection::kAllPlane,
		      TrTrackSelection::kHalfL1N,
		      TrTrackSelection::kHalfL9,
		      TrTrackSelection::kMaxSpan };

                            fTrackCounter[1]++; trstat |= 0x020;
    if (span & spflag[0]) { fTrackCounter[2]++; trstat |= 0x040; }
    if (span & spflag[1]) { fTrackCounter[3]++; trstat |= 0x080; }
    if (span & spflag[2]) { fTrackCounter[4]++; trstat |= 0x100; }
    if (span & spflag[3]) { fTrackCounter[5]++; trstat |= 0x200; }

    hman.Fill("TrCsqXi", argt, csqx);
    hman.Fill("TrCsqYi", argt, csqy);

    if ((span & spflag[0]) && csqx < 20 && csqy < 20) {
      fTrackCounter[6]++;
      trstat |= 0x400;

      int mff = trk->iTrTrackPar(malgo, 7, 0); // Full span
      if ((span & spflag[3]) && mff > 0) {
	csqx = trk->GetNormChisqX(mff);
	csqy = trk->GetNormChisqY(mff);
	argt = fabs(trk->GetRigidity(mff));
	hman.Fill("TrCsqXf", argt, csqx);
	hman.Fill("TrCsqYf", argt, csqy);

      //double ersq = TrTrackSelection::GetHalfRessq(mff, trk);
      //if (ersq < 20) {
	if (csqx < 20 && csqy < 20) {
	  fTrackCounter[7]++;
	  trstat |= 0x800;
	}
      }
    }

    break;
  }
 }  // if (vertex); else

  // Multitracks
  if (ntrk >= 2) fTrackCounter[8]++;

  delete cont;

  if (trstat > 0) {
    if (trstat & 0x01) RecPar.NcutRaw++;
    if (trstat & 0x02) RecPar.NcutLdt++;
    if (trstat & 0x04) RecPar.NcutCls++;
    if (trstat & 0x08) RecPar.NcutHit++;
    if (trstat & 0x10) RecPar.NcutCpu++;
  }

  int nfill = fTrackCounter[0];
  int intv  = (nfill < 10000) ? 1000 : 10000;
  if (nfill%intv == 0) {
    static bool first = true;
    if (first) {
      if (vertex && !simple)
	cout << "TrRecon-I-Report:   Nfill  NprSel  NevVtx "
	     << "Rsel(Pcut) Rvtx(sel) Rcut Rcpu TrTime" << endl;
      else
	cout << "TrRecon-I-Report:   Nfill  NevTrk  NevT89 "
	     << "Rtrk(sel) RT89(sel) Rcut Rcpu TrTime" << endl;
      first = false;
    }

    float trtime = _CpuTimeTotal/fTrackCounter[0];

    int ntevt = fTrackCounter[1];
    int ntful = fTrackCounter[5];
    cout << Form("TrRecon-I-Report: %7d %7d %7d "
		 ".%03.0f(.%2.0f) .%03.0f(.%2.0f) .%03.0f .%03.0f %6.4f",
		 nfill, ntevt, ntful, 
		 1000.*ntevt/nfill, 100.*fTrackCounter[6]/ntevt,
		 1000.*ntful/nfill, 
		 (ntful > 0) ? 100.*fTrackCounter[7]/ntful : 0,
		 1000.*(RecPar.NcutCls+RecPar.NcutHit)/nfill,
		 1000.* RecPar.NcutCpu /nfill, trtime) << endl;
  }
 }//#pragma omp critical (trcount)

 return trstat;
}

int TrRecon::FillHistos(int trstat, int refit)
{
  if (!hman.IsEnabled()) {
#ifdef __ROOTSHAREDLIBRARY__
    // Book histos if not yet
    AMSEventR *evt = AMSEventR::Head();
    int mc = (evt && evt->pMCEventg(0)) ? 3 : 0;
    hman.Enable();
    hman.BookHistos(mc);
#else
    return trstat;
#endif
  }

  VCon *cont0 = GetVCon()->GetCont("AMSTrRawCluster");
  VCon *cont1 = GetVCon()->GetCont("AMSTrCluster");
  VCon *cont2 = GetVCon()->GetCont("AMSTrRecHit");
  VCon* cont3 = GetVCon()->GetCont("AMSTrTrack");
  if (!cont0 || !cont1 || !cont2 || !cont3) return trstat;

  int nraw = cont0->getnelem();
  int ncls = cont1->getnelem();
  int nhit = cont2->getnelem();
  int ntrk = cont3->getnelem();
  int evid = GetEventID();

  ////////// Fill number of raw,cls,hit //////////
  bool lowdt = (GetTrigTime(4) <= RecPar.lowdt);
  hman.Fill((lowdt ? "TrNrawLt" : "TrNrawHt"), evid, nraw);
  hman.Fill((lowdt ? "TrNclsLt" : "TrNclsHt"), evid, ncls);
  hman.Fill((lowdt ? "TrNhitLt" : "TrNhitHt"), evid, nhit);


  ////////// Check simulation momentum and rigidity //////////
  bool ismc = false;
  double rmc = 0;
#ifndef __ROOTSHAREDLIBRARY__
  if (AMSJob::gethead()->isMCData()) {
    AMSmceventg *mcg 
      = (AMSmceventg *)AMSEvent::gethead()->getC("AMSmceventg")->gethead();
    if (mcg) {
      ismc = true;
       rmc = (mcg->getcharge() != 0) ? mcg->getmom()/mcg->getcharge() : 0;
    }
  }
#else
  AMSEventR *evt = AMSEventR::Head();
  MCEventgR *mcg = (evt) ? evt->pMCEventg(0) : 0;
  if (mcg) {
    ismc = true;
     rmc = (mcg->Charge != 0) ? mcg->Momentum/mcg->Charge : 0;
  }
#endif

  ////////// CPU time and Tracker data size //////////
  float dlt = GetTrigTime(4); // delta-T
  float tcp = GetCpuTime()+5e-4;
  hman.Fill("TrSizeDt", dlt, GetTrackerSize());
  hman.Fill("TrTimH",  nhit, tcp);
  hman.Fill("TrTimT",  ntrk, tcp);
  hman.Fill("TrRecon", trstat);

  ////////// Raw cluster occupancy //////////
  for (int i = 0; i < nraw; i++) {
    TrRawClusterR *cls = (TrRawClusterR *)cont0->getelem(i);
    // strip SN>4 occupancy
    int index = TkDBc::GetHead()->TkId2Entry(cls->GetTkId());
    for (int istrip=0; istrip<cls->GetNelem(); istrip++) {
      int add = cls->GetAddress()+istrip;
      float sn = cls->GetSN(istrip);
      if (sn>4) hman.Fill("TrOccRaw",index,add);
    }
  }

  ////////// Cluster signal and occupancy //////////
  for (int i = 0; i < ncls; i++) {
    TrClusterR *cls = (TrClusterR *)cont1->getelem(i);
    // signal
    hman.Fill(((cls->GetSide() == 1) ? "TrClsSigP" : "TrClsSigN"),
	      0, cls->GetTotSignal());
    // seed occupancy
    int seedadd = cls->GetSeedAddress(); 
    int index = TkDBc::GetHead()->TkId2Entry(cls->GetTkId());
    hman.Fill("TrOccSeed",index,seedadd);
    // strip SN>4 occupancy
    for (int istrip=0; istrip<cls->GetNelem(); istrip++) {
      int add = cls->GetAddress(istrip);
      float sn = cls->GetSN(istrip);
      if (sn>4) hman.Fill("TrOccStri",index,add);
    }
  }

  delete cont0;
  delete cont1;
  delete cont2;

  int simple = (TRCLFFKEY.recflag%10000  >=  1000) ? 1 : 0;
  int vertex = (TRCLFFKEY.recflag%100000 >= 10000) ? 1 : 0;

  VertexR *vtx = 0;
  if (vertex) {
    VCon *cont4 = GetVCon()->GetCont("AMSVtx");
    for (int i = 0; cont4 && i < cont4->getnelem(); i++) {
      VertexR *v = (VertexR *)cont4->getelem(i);
      if (v->IsPhotonVertex()) vtx = v;
    }
    delete cont4;
  }
  if (vtx) {
    TrTrackR *trk1 = vtx->pTrTrack(0); //(TrTrackR *)cont3->getelem(0);
    TrTrackR *trk2 = vtx->pTrTrack(1); //(TrTrackR *)cont3->getelem(1);

    int fid = TrTrackR::kVertex;
    if (!trk1 || !trk2 || !trk1->ParExists(fid) || !trk2->ParExists(fid)) {
      delete cont3;
      return trstat;
    }
    double rgt [2] = { trk1->GetRigidity  (fid), trk2->GetRigidity  (fid) };
    double csqx[2] = { trk1->GetNormChisqX(fid), trk2->GetNormChisqX(fid) };
    double csqy[2] = { trk1->GetNormChisqY(fid), trk2->GetNormChisqY(fid) };
    double momt    = vtx->Momentum;
    double vchk    = trk1->GetP0().dist(trk2->GetP0())*std::pow(momt, 0.7);
    double rchk    = (rgt[0]+rgt[1])/(rgt[0]-rgt[1]);

    AMSPoint pntv(vtx->Vertex[0], vtx->Vertex[1], VertexR::ZrefV);
    AMSDir   dirv(vtx->Theta, vtx->Phi);
    int psel = 0;

#ifndef __ROOTSHAREDLIBRARY__
#ifndef _STANDALONE_
    int ntrc = 0, ntrl = 0, ntrd = 0, ntrp = 0;

    double   qtof[2] = { 0, 0 };
    AMSPoint dtof[2];

    for (int i = 0; i < 2; i++) {
      AMSPoint dmin;
      double   qmin = 0;
      double   emip = 1.7;
      for (AMSTOFCluster *tof
	     = AMSTOFCluster::gethead(i); tof; tof = tof->next()) {
	AMSPoint tc = tof->getcoo();
	AMSPoint td = pntv+dirv*(tc.z()-pntv.z())/dirv.z()-tc;
	double   qt = tof->getedep()/emip*dirv.z();
	if (qt > 0 && (qmin == 0 || td.norm() < dmin.norm())) {
	  dmin = td;
	  qmin = std::sqrt(qt);
	}
      }
      if (dmin.norm() > 0 && qmin > 0) {
	qtof[i] = qmin;
	dtof[i] = dmin;
      }
    }

    if (dtof[0].norm() > 0) hman.Fill("TvDtfU", dtof[0].x(), dtof[0].y());
    if (dtof[1].norm() > 0) hman.Fill("TvDtfU", dtof[1].x(), dtof[1].y());
    if (qtof[0] > 0 && dtof[0].norm() < 7 &&
	qtof[1] > 0 && dtof[1].norm() < 7) {
      hman.Fill("TvQtfU", qtof[0], qtof[1]);
      if (1.2 < qtof[0] && qtof[0] < 2.5 &&
	  1.2 < qtof[1] && qtof[1] < 2.5) psel |= 1;
    }

    for (unsigned int i = 0; i < TRDDBc::nlay(); i++) {
      AMSPoint dmin;
      AMSTRDCluster *trc = AMSTRDCluster::gethead(i);
      for (; trc; trc = trc->next()) {
	AMSPoint tc = trc->getCoo();
	AMSPoint td = pntv+dirv*(tc.z()-pntv.z())/dirv.z()-tc;
	ntrc++;
	if (dmin.norm() == 0 || td.norm() < dmin.norm()) dmin = td;
      }
      double dd = dmin.norm();
      if (dd > 0) {
	hman.Fill("TvDtrc", dmin.x(), dmin.y());
	hman.Fill("TvLtrd", i+0.5, dd);
	if (dd < 10) ntrl++;
      }
    }
    AMSTRDTrack *trd = (AMSTRDTrack*)AMSEvent::gethead()
	                                     ->getheadC("AMSTRDTrack", 0, 1);
    for (; trd; trd = trd->next()) {
      double zref = 65;
      AMSDir   dirt = AMSDir(trd->gettheta(), trd->getphi());
      AMSPoint pntt = trd->getcoo();
      AMSPoint tp   = pntt+dirt*(zref-pntt.z())/dirt.z();
      AMSPoint vp   = pntv+dirv*(zref-pntv.z())/dirv.z();
      AMSPoint dp   = tp-vp;
      hman.Fill("TvDtrd", dp.x(), dp.y());
      if (dp.norm() < 10) ntrp++;
      ntrd++;
    }

    hman.Fill("TvNtrl", ntrl, ntrc);
    hman.Fill("TvNtrd", ntrd, ntrc);
    hman.Fill("TvNtrp", ntrp, ntrc);
    if (ntrp == 0 && ntrc < 20) psel |= 2;
#endif
#endif
    for (int i = 0; i < 2; i++) {
      if (i == 1 && psel != 3) continue;

      TString shn = (i == 0) ? "Tv" : "Tp";
      if (rgt[0] > 0) hman.Fill(shn+"CsqXp",  rgt[0], csqx[0]);
      if (rgt[1] > 0) hman.Fill(shn+"CsqXp",  rgt[1], csqx[1]);
      if (rgt[0] < 0) hman.Fill(shn+"CsqXn", -rgt[0], csqx[0]);
      if (rgt[1] < 0) hman.Fill(shn+"CsqXn", -rgt[1], csqx[1]);

      if (rgt[0] > 0) hman.Fill(shn+"CsqYp",  rgt[0], csqy[0]);
      if (rgt[1] > 0) hman.Fill(shn+"CsqYp",  rgt[1], csqy[1]);
      if (rgt[0] < 0) hman.Fill(shn+"CsqYn", -rgt[0], csqy[0]);
      if (rgt[1] < 0) hman.Fill(shn+"CsqYn", -rgt[1], csqy[1]);

      hman.Fill(shn+"CsqXY", csqx[0], csqy[0]);
      hman.Fill(shn+"CsqXY", csqx[1], csqy[1]);

      hman.Fill(shn+"RdfP", momt, rchk);
      hman.Fill(shn+"DV7P", momt, vchk);
    }

    if (csqx[0] < 100 && csqx[1] < 100 && 
	csqy[0] <  20 && csqy[1] <  20 && vchk < 20 && fabs(rchk) < 1) {
      if (!simple) {
	trstat |= 0x800;
	fTrackCounter[7]++;
      }
    //cout << "TrRecon::FillHistos-I-photon" << psel
    //     << " @" << GetEventID() << " CpuTime= " << _CpuTime << endl;

      if (psel == 3) {
	trstat |= 0x4000;
	fTrackCounter[9]++;
	vtx->setstatus(AMSDBc::GOOD);
      }
    }

    delete cont3;
    return trstat;
  }

  // Select event with only one track
  if (ntrk >= 2) {
    delete cont3;
    return trstat;
  }

  // Fitting algorithm for iTrTrackPar
  int malgo = 1;  // 1: Choutko, 2: Alcaraz, +10: no-MSC, +20: same-weight

  // Get track
  TrTrackR *trk = (TrTrackR *)cont3->getelem(0);
  delete cont3;

  // Request on a track with at least one hit at all the INNER PLANES
  int span = (TrTrackSelection::GetSpanFlags(trk) & 0xff);
  if (!(span & TrTrackSelection::kAllPlane)) return trstat;

  // Check if the inner track fitting exists
  int mfi = trk->iTrTrackPar(malgo, 3, refit); // Inner fitting
  if (mfi < 0) return trstat;

  ////////// Hit ladders and cluster signals on track //////////
  int nhxi = 0, nhyi = 0;
  for (int j = 0; j < TkDBc::Head->nlay(); j++) {
    TrRecHitR *hit = trk->GetHitLO(j+1);
    if (hit) {
      int slot  = hit->GetTkId()%100;
      int layer = hit->GetLayer();
      TrClusterR *clx = hit->GetXCluster();
      TrClusterR *cly = hit->GetYCluster();

      hman.Fill("TrLadTrk", slot, layer);
      if (cly) { hman.Fill("TrLadYh",  slot, layer);
	if (clx) hman.Fill("TrLadXYh", slot, layer);
      }
      if (clx) {
	double sig = clx->GetTotSignal();
	hman.Fill("TrClsSigN", 1, sig);
	hman.Fill("TrClsStrN", clx->GetNelem(), std::fabs(clx->GetCofG()),
		  sig/clx->GetSeedSignal()/clx->GetNelem());
      }
      if (cly) {
	double sig = cly->GetTotSignal();
	hman.Fill("TrClsSigP", 1, sig);
	hman.Fill("TrClsStrP", cly->GetNelem(), std::fabs(cly->GetCofG()),
		  sig/cly->GetSeedSignal()/cly->GetNelem());
      }

      if (layer <= 7) {
	if (clx) nhxi++;
	if (cly) nhyi++;
      }
    }
    else {
      AMSPoint play = trk->InterpolateLayerO(j+1);
      TkSens tks(play, 0);
      if (tks.LadFound())
	hman.Fill("TrLadTrk", tks.GetLadTkID()%100,
		  abs(tks.GetLadTkID())/100);
    }
  }
  hman.Fill("TrNhit", 0, nhxi);
  hman.Fill("TrNhit", 1, nhyi);
  hman.Fill("TrNhit", 2, trk->GetNhitsX());
  hman.Fill("TrNhit", 3, trk->GetNhitsY());
  hman.Fill("TrNhit", 4, trk->GetNhitsXY());

  ////////// Inner fitting chisquares //////////
  double csqx = trk->GetNormChisqX(mfi);
  double csqy = trk->GetNormChisqY(mfi);
  double eriv = trk->GetErrRinv   (mfi);
  double argt = fabs(trk->GetRigidity(mfi));
  hman.Fill("TrCsqX", argt, csqx);
  hman.Fill("TrCsqY", argt, csqy);

  eriv /= std::sqrt(0.004*0.004+0.03*0.03/argt/argt
		               +0.02*0.02/argt/argt/argt/argt);

  // Select a track with reasonable inner chisquare
  if (csqx > 20 || csqy > 20) return trstat;
  if (trk->checkstatus(16)) return trstat;

  hman.Fill("TrEriC", argt, eriv);
  if (eriv > 20) return trstat;

  ////////// Hit residuals and trunc-mean signals //////////
  for (int j = 0; j < 7; j++) {  // Inner layers
    TrRecHitR *hit = trk->GetHitLO(j+1);
    AMSPoint   res = trk->GetResidualJ(j+1, mfi);  // Inner fitting residual
    if (hit && !hit->OnlyY()) hman.Fill("TrResX", argt, res.x()*1e4);
    if (hit && !hit->OnlyX()) hman.Fill("TrResY", argt, res.y()*1e4);
  }

  // Fill truncated mean of charge
  double chgp = TrCharge::GetQ(trk, 1);
  double chgn = TrCharge::GetQ(trk, 0);
  double chg  = (chgp > 0 && chgn > 0) ? (chgp+chgn)/2
                                       : ((chgp > 0) ? chgp : chgn);
  hman.Fill("TrChgP", argt, chgp);
  hman.Fill("TrChgN", argt, chgn);

  if (chg > 1.5*std::sqrt(1.5*1.5/argt/argt+1)) {
    hman.Fill("TrCsqXh", argt, csqx);
    hman.Fill("TrCsqYh", argt, csqy);
  }

  // Check fitting with external layers
  int mf8 = trk->iTrTrackPar(malgo, 5, 0); // With L1N
  int mf9 = trk->iTrTrackPar(malgo, 6, 0); // With L9
  int mff = trk->iTrTrackPar(malgo, 7, 0); // Full span

  ////////// Track position at external planes //////////
  AMSPoint pl1, pl4, pl7, pl8, pl9;
  AMSDir   dr1, dr4, dr7, dr8, dr9;
  trk->InterpolateLayerO(1, pl1, dr1, mfi);
  trk->InterpolateLayerO(4, pl4, dr4, mfi);
  trk->InterpolateLayerO(7, pl7, dr7, mfi);
  trk->InterpolateLayerO(8, pl8, dr8, mfi);
  trk->InterpolateLayerO(9, pl9, dr9, mfi);
  hman.Fill("TrPtkL1", pl1.x(), pl1.y());
  hman.Fill("TrPtkL4", pl4.x(), pl4.y());
  hman.Fill("TrPtkL7", pl7.x(), pl7.y());
  if (pl8.z() >  150) hman.Fill("TrPtkL8", pl8.x(), pl8.y());
  if (pl9.z() < -130) hman.Fill("TrPtkL9", pl9.x(), pl9.y());

  if (mf8 > 0 && argt > 10) hman.Fill("TrPftL8", pl8.x(), pl8.y());
  if (mf9 > 0 && argt > 10) hman.Fill("TrPftL9", pl9.x(), pl9.y());

  AMSPoint   res8 = trk->GetResidualO(8, mfi);
  AMSPoint   res9 = trk->GetResidualO(9, mfi);
  TrRecHitR *hit8 = trk->GetHitLO(8);
  TrRecHitR *hit9 = trk->GetHitLO(9);

  // Check TOF beta
  double beta = 0;
#ifndef _STANDALONE_
  double ttm[4], sln[4];
  for (int j = 0; j < 4; j++) {
    double dmin = 25, dxmin = 0, dymin = 0;
    ttm[j] = sln[j] = 0;
    int itc = (j == 0 || j == 3) ? 1 : 0;
#ifndef __ROOTSHAREDLIBRARY__
    for (AMSTOFCluster *tofcls
	   = AMSTOFCluster::gethead(j); tofcls; tofcls = tofcls->next()) {
      AMSPoint tcoo = tofcls->getcoo();
      AMSPoint ecoo = tofcls->getecoo();
      double   time = tofcls->gettime();
#else
    AMSEventR *evt = AMSEventR::Head();
    for (int k = 0; evt && k < evt->nTofCluster(); k++) {
      TofClusterR *tofcls = evt->pTofCluster(k);
      if (tofcls->Layer != j+1) continue;
      AMSPoint tcoo(tofcls->Coo);
      AMSPoint ecoo(tofcls->ErrorCoo);
      double   time = tofcls->Time;
#endif
      if (time > 2.5e-10) continue;
      if (ecoo.x() == 0 || ecoo.y() == 0) continue;

      double zl = tcoo.z();
      AMSPoint pnt;
      AMSDir   dir;
      double slen = trk->Interpolate(zl, pnt, dir);
      if (pnt.z() > 0) slen *= -1;

      AMSPoint dp = tcoo-pnt;
      double dx = dp[  itc]/ecoo[  itc];
      double dy = dp[1-itc]/ecoo[1-itc];
      double dd = dx*dx+dy*dy;
      if (std::fabs(dx) < 3.5 && std::fabs(dy) < 3.5 && dd < dmin) {
	dmin   = dd;   dxmin  = dx; dymin = dy;
	ttm[j] = time; sln[j] = slen;
      }
    }
    if (ttm[j] < 0) hman.Fill("TrTofDD", dxmin, dymin);

  } // for (int j = 0; j < 4; j++)

  if (ttm[0] < 0 && ttm[1] < 0 && ttm[2] < 0 && ttm[3] < 0) {
    double dtof = (ttm[2]+ttm[3])/2-(ttm[0]+ttm[1])/2;
    double dlen = (sln[2]+sln[3])/2-(sln[0]+sln[1])/2;
    beta = 0.01*dlen/dtof/TrProp::Clight;
  }
#endif // _STANDALONE_

  if (beta != 0 && mfi > 0 && chg > 0) {
    double schg = (beta > 0) ? chg : -chg;
    double rgt   = trk->GetRigidity(mfi);
    hman.Fill("TrRiIBi", 1/rgt, 1/beta);

    if (schg != 0)
      hman.Fill("TrRiICs", 1/rgt, schg);

    if (mff > 0) {
      double ersq = TrTrackSelection::GetHalfRessq(mff, trk);
      if (ersq < 20) {
	double rgt = trk->GetRigidity(mff);
	hman.Fill("TrRiFBi", 1/rgt, 1/beta);
	if (schg != 0) hman.Fill("TrRiFCs", 1/rgt, schg);
      }
    }
  }

  ////////// Alignment check //////////
  if (0.9 < beta && beta < 1.1 &&
      ((trk->GetBitPattern() & 0x080) ||
       (trk->GetBitPattern() & 0x100))) {  // Require L1-OR-L9

    for (int j = 0; j < 9; j++) {
      int       layj = j+1;
      TrRecHitR *hit = trk->GetHitLJ(layj);
      if (!hit) continue;
      AMSPoint res = trk->GetResidualJ(layj, mfi);  // Inner fitting residual
      AMSPoint coo;
      AMSDir   dir;
      trk->InterpolateLayerJ(layj, coo, dir, mfi);
      if (dir.z() == 0) continue;

      // Exclude Layer9 hits out of Ecal window
      if (layj == 9 && std::fabs(coo.x()) > 33) continue;

      TString shn = "TrRes";
      shn += (char)('0'+j+1);
      if (!hit->OnlyY()) {
	hman.Fill(shn+"1", coo.x(), dir.x()/dir.z(), res.x()*1e4);
	hman.Fill(shn+"3", coo.y(), dir.x()/dir.z(), res.x()*1e4);
      }
      if (!hit->OnlyX()) {
	hman.Fill(shn+"2", coo.x(), dir.y()/dir.z(), res.y()*1e4);
	hman.Fill(shn+"4", coo.y(), dir.y()/dir.z(), res.y()*1e4);
      }
    }

    trstat |= 0x1000;
  }

  // Fit id for the maximum span
  int mf0 = trk->iTrTrackPar(malgo, 0, refit);
  if (mf0 > 0) {

    ////////// Residual vs layer //////////
    for (int ilay = 0; ilay < 9; ilay++) {
      TrRecHitR *hit = trk->GetHitLO(ilay+1);
      if (!hit) continue;

      AMSPoint res = trk->GetResidualO(ilay+1, mf0);
      if (hit->GetXCluster()) hman.Fill("TrResLayx", ilay, res.x()*1.e+04);
      if (hit->GetYCluster()) hman.Fill("TrResLayy", ilay, res.y()*1.e+04);
    }
  }

  if (!ismc) return trstat;

  ////////// For simulation //////////
  double armc = fabs(rmc);
  TrSim::fillreso(trk);
  if (hit8 && mf8 > 0) hman.Fill("TrDtyL81", armc, res8.y());
  if (hit9 && mf9 > 0) hman.Fill("TrDtyL91", armc, res9.y());

  if (mff > 0) {
    if (mf9 > 0) hman.Fill("TrDtyL82", armc, trk->GetResidualO(8, mf9).y());
    if (mf8 > 0) hman.Fill("TrDtyL92", armc, trk->GetResidualO(9, mf8).y());
  }

  int mf[4] = { mfi, mf8, mf9, mff };
  for (int j = 0; j < 4; j++) {
    if (mf[j] < 0) continue;

    double rgt = trk->GetRigidity(mf[j]);
    if (rgt != 0 && rmc != 0) {
      TString shn = "TrRres";
      shn += (char)('0'+j+1);
      hman.Fill(shn+"1", armc, 1e3*(1/rgt-1/rmc));
      hman.Fill(shn+"2", armc, rmc*(1/rgt-1/rmc));
    }
  }

  return trstat;
}

void TrRecon::PrintStats()
{
  cout << endl;
  cout
    << "    ======================= TrRecon statistics ======================"
    << endl;

  int *nc = fTrackCounter;
  if (nc[0] == 0) nc[0] = 1;  // Avoid NAN
  if (nc[1] == 0) nc[1] = 1;  // Avoid NAN

  cout << endl;
  cout << Form(" Events in total             :  %8d", nc[0]) << endl;
  cout << Form(" Cut by MaxNrawCls    (%4d) :  %8d (%5.3f)",
	       RecPar.MaxNrawCls, RecPar.NcutRaw, 
	                       1.*RecPar.NcutRaw/nc[0]) << endl;
  cout << Form(" Cut by MaxNtrCls_ldt (%4d) :  %8d (%5.3f)",
	       RecPar.MaxNtrCls_ldt, RecPar.NcutLdt, 
	                          1.*RecPar.NcutLdt/nc[0]) << endl;
  cout << Form(" Cut by MaxNtrCls     (%4d) :  %8d (%5.3f)",
	       RecPar.MaxNtrCls, RecPar.NcutCls,
	                      1.*RecPar.NcutCls/nc[0]) << endl;
  cout << Form(" Cut by MaxNtrHit     (%4d) :  %8d (%5.3f)",
	       RecPar.MaxNtrHit, RecPar.NcutHit,
	                      1.*RecPar.NcutHit/nc[0]) << endl;
  cout << Form(" Cut by CpuLimit      (%4.0f) :  %8d (%5.3f)",
	       RecPar.TrTimeLim, RecPar.NcutCpu,
	                      1.*RecPar.NcutCpu/nc[0]) << endl;
  
  if (hman.Get("TrTimH")) {
    float trtime = _CpuTimeTotal/fTrackCounter[0];
    cout << Form(" TrRecon::Cpu time    (s/ev) :    %6.4f", trtime)
	 << endl;
  }
  cout << endl;

 int simple = (TRCLFFKEY.recflag%10000  >=  1000) ? 1 : 0;
 int vertex = (TRCLFFKEY.recflag%100000 >= 10000) ? 1 : 0;
 if(simple) {
  cout << Form(" Events with >0 track        :  %8d (%5.3f)",
	       nc[1], 1.*nc[1]/nc[0]) << endl;
  cout << Form("  with all-plI  track        :  %8d (%5.3f)",
	       nc[2], 1.*nc[2]/nc[1]) << endl;
  cout << Form("  with half-L1N track        :  %8d (%5.3f)",
	       nc[3], 1.*nc[3]/nc[1]) << endl;
  cout << Form("  with half-L9  track        :  %8d (%5.3f)",
	       nc[4], 1.*nc[4]/nc[1]) << endl;
  cout << Form("  with fullspan track        :  %8d (%5.3f)",
	       nc[5], 1.*nc[5]/nc[1]) << endl;
  cout << Form(" Events with >1 track        :  %8d (%5.3f)",
	       nc[8], 1.*nc[8]/nc[0]) << endl;
 }
 else if (vertex) {
  cout << Form(" Events with TOF/TRD cut     :  %8d (%5.3f)",
	       nc[2], 1.*nc[2]/nc[0]) << endl;
  cout << Form("        with mult-Tr cut     :  %8d (%5.3f)",
	       nc[3], 1.*nc[3]/nc[0]) << endl;
  cout << Form("        with high-Q  cut     :  %8d (%5.3f)",
	       nc[4], 1.*nc[4]/nc[0]) << endl;
  cout << Form(" Events with pre-selection   :  %8d (%5.3f)",
	       nc[1], 1.*nc[1]/nc[0]) << endl;
  cout << Form("        with vertex          :  %8d (%5.3f)",
	       nc[5], 1.*nc[5]/nc[1]) << endl;
  cout << Form("        with quality sel     :  %8d (%5.3f)",
	       nc[7], 1.*nc[7]/nc[1]) << endl;
  cout << Form(" Events with vertex(q)       :  %8d (%5.3f)",
	       nc[7], 1.*nc[7]/nc[0]) << endl;
 }
 if (vertex) {
  cout << Form(" Photon candidates           :  %8d (%5.3f)",
	       nc[9], 1.*nc[9]/nc[0]) << endl;
 }


  cout 
    << "    ================================================================="
    << endl;
  cout << endl;
}


#ifndef __ROOTSHAREDLIBRARY__

#include "trigger102.h"
float TrRecon::GetTrigTime(int i)
{
  Trigger2LVL1 *trig
    = (Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1", 0);
  return (trig && 0 <= i && i < 5) ? trig->gettrtime(i) : 0;
}

int TrRecon::GetTofFlag(int i)
{
  Trigger2LVL1 *trig
    = (Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1", 0);
  return (trig) ? ((i == 0) ? trig->gettoflag1() 
		            : trig->gettoflag2()) : 0;
}

int TrRecon::GetRunID(void)
{
  return AMSEvent::gethead()->getrun();
}

int TrRecon::GetEventID(void)
{
  return AMSEvent::gethead()->getEvent();
}

bool TrRecon::IsMC(void)
{
  return (AMSJob::gethead()->isMCData() && 
	  AMSEvent::gethead()->getC("AMSmceventg")->gethead());
}

#else

float TrRecon::GetTrigTime(int i)
{
  AMSEventR *evt = AMSEventR::Head();
  if (!evt) return 0;
  Level1R  *trig = evt->pLevel1(0);
  return (trig && 0 <= i && i < 5) ? trig->TrigTime[i] : 0;
}

int TrRecon::GetTofFlag(int i)
{
  AMSEventR *evt = AMSEventR::Head();
  if (!evt) return 0;
  Level1R  *trig = evt->pLevel1(0);
  return (trig) ? ((i == 0) ? trig->TofFlag1
		            : trig->TofFlag2) : 0;
}

int TrRecon::GetRunID(void)
{
  AMSEventR *evt = AMSEventR::Head();
  return (evt) ? evt->Run() : 0;
}

int TrRecon::GetEventID(void)
{
  AMSEventR *evt = AMSEventR::Head();
  return (evt) ? evt->Event() : 0;
}

bool TrRecon::IsMC(void)
{
  AMSEventR *evt = AMSEventR::Head();
  return (evt && evt->pMCEventg(0));
}

#endif


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
  if(!std::isnormal(intp)) intp=0;
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
  if ((RecPar.TrTimeLim > 0 && _CheckTimer() > RecPar.TrTimeLim) || SigTERM) {
    _CpuTimeUp = true;
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
  for (int i = 0; i < _SCANLAY; i++)
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
      if ((found = (_itchit.chisq[it.side] < RecPar.MaxChisqAllowed))) break;
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
    float sg = TkDBc::Head->FindTkId(tks.GetLadTkID())->GetRotMat().GetEl(0, 0);
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

int TrRecon::MergeLowSNHits(TrTrackR *track, int mfit, int select_tag)
{
  // TMP: AO for old reconstruction selection tag doesn't work

  VCon* cont = GetVCon()->GetCont("AMSTrRecHit");
  if (!cont) return -1;

  double rthdx = TRFITFFKEY.ErrX*RecPar.NsigmaMerge;
  double rthdy = TRFITFFKEY.ErrY*RecPar.NsigmaMerge;

  AMSPoint pltrk[_SCANLAY];
  double   rymin[_SCANLAY];
  int      ncmin[_SCANLAY];
  for (int i = 0; i < _SCANLAY; i++) {
    pltrk[i] = track->InterpolateLayerO(i+1, mfit);
    rymin[i] = rthdy*1.5;
    ncmin[i] = 0;
  }

  // Merge low seed SN clusters
  int nhit = cont->getnelem(), nadd = 0;
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);
    if (!hit || hit->Used() ||
	(TkDBc::Head->GetSetup()==3 && hit->GetLayer() >= 8)) continue;

    int ily = hit->GetLayer()-1;
    if (track->TestHitBits(hit->GetLayer(), mfit)) {
      if (hit->OnlyY()) continue;
      TrRecHitR *hh = track->GetHitLO(ily+1);
      if (hh && (!hh->OnlyY() ||
		  hh->iTrCluster('y') != hit->iTrCluster('y'))) continue;
    }

    TrClusterR *clx = hit->GetXCluster();
    TrClusterR *cly = hit->GetYCluster();
    if (!cly) continue;
    if (cly->GetSeedSN() >= RecPar.TrackThrSeed[1] && hit->OnlyY()) continue;

/*  if  (clx && clx->GetSeedSN() >= RecPar.TrackThrSeed[0] && 
	 cly && cly->GetSeedSN() >= RecPar.TrackThrSeed[1]) continue;
    if (!clx && cly->GetSeedSN() >= RecPar.TrackThrSeed[1]) continue;
*/
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
      for (int j = 0; j < hit->GetMultiplicity(); j++) {
	double ddp = hit->GetCoord(j).dist(ptrk);
	if (ddp < dmin) {
	  jmin = j;
	  dmin = ddp;
	}
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

int TrRecon::MergeExtHits(TrTrackR *track, int mfit, int select_tag)
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
  ptrk[0] = track->InterpolateLayerO(lyext[0], mfit);
  ptrk[1] = track->InterpolateLayerO(lyext[1], mfit);
  float rig= std::fabs(track->GetRigidity(mfit));
  if (rig == 0) return -2;

  // average charge (previous)
  float mean = 0.;
  for (int ihit=0; ihit<track->GetNhits(); ihit++) {
    TrRecHitR *hit = (TrRecHitR*) track->GetHit(ihit);
    if (!hit) continue;
    if (!hit->GetYCluster()) continue; 
    mean += hit->GetYCluster()->GetTotSignal(); 
  }
  mean /= track->GetNhits(); 

  // 1. Search the XY and Y-only hits closest to the track extrapolation on Y 
  //    (discarding bad hits for charge compatibility)
  int nhit = cont->getnelem();
  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = (TrRecHitR*)cont->getelem(i);

    if (!hit) continue;
    if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
    if (hit->OnlyX() || hit->Used()) continue;

    // charge compatibility test
    float xxx = sqrt(mean);
    float yyy = sqrt(hit->GetYCluster()->GetTotSignal());
    hman.Fill("ySig5",xxx,yyy);
    if ( (TRCLFFKEY.TrackFindChargeCutActive)&&( (yyy>xxx+15)||(yyy<xxx-15) ) ) continue;

    int il = -1;
    if (hit->GetLayer() == lyext[0]) il = 0;
    if (hit->GetLayer() == lyext[1]) il = 1;
    if (il < 0) continue;

    // Ignore layer9 hits which are out of Ecal window (|x| < PAR (it was33 cm now default 45))
    if (hit->GetLayer() == 9 && std::fabs(ptrk[1].x()) > TRFITFFKEY.EcalMaxX) continue;

    int mult;
    AMSPoint DD = hit->HitPointDist(ptrk[il],mult);
    float dxy=sqrt(DD[0]*DD[0]+DD[1]*DD[1]);
    float dy=fabs(DD[1]);
    
    if (hit->OnlyY()) {
      if (dy < DY[il].rymin) {
	DY[il].ihmin = i;
	DY[il].icmin = hit->GetYClusterIndex();
	DY[il].rymin = dy;
	DY[il].ncmin = 1;
	DY[il].diff  = DD;
      }
    }
    else{
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
  
  // 2. XY or Y
  float limx = TRFITFFKEY.MergeExtLimX;
  float limy = TRFITFFKEY.MergeExtLimY;

  float sp0  = 0.02;
  float sp1  = 1;
  float rcor = std::sqrt(sp0*sp0+sp1*sp1/rig/rig);
  float sigx = limx*rcor;
  float sigy = limy*rcor;

  float diffX_max = sigx*4;
  float diffY_max = sigy*4;
  if (diffX_max > limx*10) diffX_max = limx*10;
  if (diffY_max > limy*10) diffY_max = limy*10;

  int nadd=0;
  int iadd[2]={0,0};
  for (int il=0;il<2;il++){
    if (DY[il].ihmin < 0) continue;

    if(DXY[il].icmin != DY[il].icmin){
       static int mprint=100; 
      if(mprint++<100)printf("TrRecon::MergeExtHits -W- XY and Y cluster do not share the Y cluster!!!!!\n");
     }
    if(il==0){
      if (DXY[il].ihmin>=0) {
	hman.Fill("1N_XY",DXY[il].diff.x(),DXY[il].diff.y());
	hman.Fill("TrEdx1",rig, DXY[il].diff.x()/sigx);
      }
      hman.Fill("1N_Y",DY[il].diff.x(),DY[il].diff.y());
      hman.Fill("TrEdy1",rig, DY[il].diff.y()/sigy);
    }
    else{
      if (DXY[il].ihmin>=0) {
	hman.Fill("P6_XY",DXY[il].diff.x(),DXY[il].diff.y());
	hman.Fill("TrEdx9",rig, DXY[il].diff.x()/sigx);
      }
      hman.Fill("P6_Y",DY[il].diff.x(),DY[il].diff.y());
      hman.Fill("TrEdy9",rig, DY[il].diff.y()/sigy);
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
      iadd[il]=1;
      nadd++;
    }
  }
  delete cont;

  if (nadd > 0) {
    track->FillExRes();
    int fitid=track->Gettrdefaultfit();
    if(iadd[0]==1 && iadd[1]==1) 
      fitid|=TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9;
    else     if(iadd[0]==1) 
      fitid|=TrTrackR::kFitLayer8;
    else     if(iadd[1]==1) 
      fitid|=TrTrackR::kFitLayer9;
    if (track->FitT(fitid) > 0)
      track->RecalcHitCoordinates(fitid);

  }
  else return nadd;

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack6FitE");
#endif
  int good_base1=TrTrackR::DefaultFitID;
  int good_base2=TrTrackR::DefaultFitID;
  int base=mfit&0xf;
  if(base==TrTrackR::kChoutko)
    good_base2=(mfit & 0xfffffff0) + TrTrackR::kAlcaraz;
  else if(base==TrTrackR::kAlcaraz)
    good_base2=(mfit & 0xfffffff0) + TrTrackR::kChoutko;

  if(iadd[0]==1) {
    track->DoAdvancedFit(TrTrackR::kFitLayer8);
    if(       track->FitDone(good_base1|TrTrackR::kFitLayer8))
      track->Settrdefaultfit(good_base1|TrTrackR::kFitLayer8);
    else if  (track->FitDone(good_base2|TrTrackR::kFitLayer8))
      track->Settrdefaultfit(good_base2|TrTrackR::kFitLayer8);
  }
  if(iadd[1]==1) {
    track->DoAdvancedFit(TrTrackR::kFitLayer9);
    if(       track->FitDone(good_base1|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base1|TrTrackR::kFitLayer9);
    else if  (track->FitDone(good_base2|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base2|TrTrackR::kFitLayer9);
  }
  
  if(iadd[0]==1 && iadd[1]==1) {
    track->DoAdvancedFit(TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);

    if(       track->FitDone(good_base1|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base1|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);
    else if  (track->FitDone(good_base2|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base2|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);
  }
  else if ( track->ParExists(mfit|TrTrackR::kFitLayer8) &&
            track->ParExists(mfit|TrTrackR::kFitLayer9) &&
           !track->ParExists(mfit|TrTrackR::kFitLayer8|
			          TrTrackR::kFitLayer9)) {
    track->DoAdvancedFit(TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);
    
    if(       track->FitDone(good_base1|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base1|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);
    else if  (track->FitDone(good_base2|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9))
      track->Settrdefaultfit(good_base2|TrTrackR::kFitLayer8|TrTrackR::kFitLayer9);
  }

#ifndef __ROOTSHAREDLIBRARY__
     AMSgObj::BookTimer.stop("TrTrack6FitE");
#endif

  return nadd;
}

int TrRecon::BuildATrTrack(TrHitIter &itcand)
{
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
  AMSgObj::BookTimer.start("TrTrack2Build"); 
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

  // Fill patterns
  track->SetPatterns(patt->GetHitPatternIndex(maskc),
		     patt->GetHitPatternIndex(masky),
                     patt->GetHitPatternIndex(maskc),
                     patt->GetHitPatternIndex(masky));

  int bb=ProcessTrack(track);
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrTrack2Build"); 
#endif
  return bb;
}

int TrRecon::ProcessTrack(TrTrackR *track, int merge_low, int select_tag)
{
 if (PZDEBUG){
  cout<< " PZDEBUG track info\n";
  for(int jj=0;jj<track->GetNhits();jj++){
    cout<<" Track hit num "<<jj<<endl;
    track->GetHit(jj)->Print();
  }
 }

  // 1st. step Fit
  int mfit1 = (MagFieldOn()) ? TrTrackR::kChoutko : TrTrackR::kLinear;

  float ret = track->FitT(mfit1);
  int ndofy = (ret > 0) ? track->GetNdofY(mfit1) : 0;
  if (TRCLFFKEY.AllowYonlyTracks && ndofy == 0) ndofy = 1; 
  if (ret < 0 || 
      track->GetChisqX(mfit1) < 0 || track->GetChisqY(mfit1) <= 0 ||
      track->GetNdofX (mfit1) < 0 || ndofy <= 0) {

    if (TrDEBUG >= 1)
      cout << "1st.fit failed; " << ret << " "
	   << track->GetChisqX(mfit1) << " " << track->GetChisqY(mfit1) << " "
	   << track->GetNdofX (mfit1) << " " << track->GetNdofY (mfit1)
	   << endl;
    delete track;
    return 0;
  }
  TR_DEBUG_CODE_401;
  if (track->GetChisqX(mfit1) > 1e6) {
    if (TrDEBUG >= 1) cout << "Too large ChisqX, force to fail" << endl;
    delete track;
    return 0;
  }

  track->Settrdefaultfit(mfit1);

  // Merge low seed SN hits
  if (merge_low && 
      (RecPar.TrackThrSeed[0] > RecPar.ThrSeed[0][0] || 
       RecPar.TrackThrSeed[1] > RecPar.ThrSeed[1][0])) {
    MergeLowSNHits(track, mfit1);
    if (track->GetNhitsX () < RecPar.MinNhitX ||
        track->GetNhitsY () < RecPar.MinNhitY ||
        track->GetNhitsXY() < RecPar.MinNhitXY) {
      delete track;
      return 0;
    }
  }

  // recalc hit coordinates using the 1st fit information
  track->RecalcHitCoordinates(mfit1);

  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR  *hit = track->GetHit(i);
    // Mark the hit as USED
    hit->SetUsed();
    // Remove hit pointer from _HitsTkIdMap 
    if (!_HitsTkIdMap.empty()) {
      RemoveHits(hit);
      TR_DEBUG_CODE_41;
    }
  }

  // 2nd. step Fit
  if (!MagFieldOn()) {
    track->FitT(mfit1);
    track->Settrdefaultfit(mfit1);
  }
  else {
    track->EstimateDummyX(mfit1);
    int mfit2 = TrTrackR::DefaultFitID;
    int mfit3 = TrTrackR::DefaultFitID;

    if ((mfit2 & 0xf) == TrTrackR::kChoutko)
      mfit3 = TrTrackR::kAlcaraz | TrTrackR::kMultScat;
    if ((mfit2 & 0xf) == TrTrackR::kAlcaraz)
      mfit3 = TrTrackR::kChoutko | TrTrackR::kMultScat;

    track->FitT(mfit2);
    if (track->FitDone(mfit2)) track->Settrdefaultfit(mfit2);
    else {
      track->FitT(mfit3);
      if (track->FitDone(mfit3)) track->Settrdefaultfit(mfit3);
    }

    if (TryDropX(track, track->Gettrdefaultfit()))
      track->EstimateDummyX(track->Gettrdefaultfit());
  }

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("TrTrack4Match"); 
#endif

  // Check it the X matching with the TRD and/or TOF direction
  MatchTOF_TRD(track,select_tag);

  // Update bit patterns
  track->UpdateBitPattern();

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("TrTrack4Match"); 
  AMSgObj::BookTimer.start("TrTrack5Fit");
#endif

    if(track->DoAdvancedFit()) {
      if (TrDEBUG >= 1) printf(" Track Advanced Fits Done!\n");
    } else {
      if (TrDEBUG >= 1) {
        printf(" Problems with Track Advanced Fits: %d %d\n",
               track->GetNhits(), track->GetNhitsXY());
      }
    }
    if (track->Gettrdefaultfit() != TrTrackR::DefaultFitID &&
	track->FitDone(TrTrackR::DefaultFitID))
      track->Settrdefaultfit(TrTrackR::DefaultFitID);

#ifndef __ROOTSHAREDLIBRARY__
     AMSgObj::BookTimer.stop("TrTrack5Fit");
#endif

  if (track->GetRigidity() == 0 || track->GetChisq() <= 0) {
    if (TrDEBUG >= 1)
      cout << "Default fitting is wrong: "
	   << track->GetRigidity() << " " << track->GetChisq() << endl;
    delete track;
    return 0;
  }
  // Add the track to the collection
  VCon* cont = GetVCon()->GetCont("AMSTrTrack");
  if (cont) {
    cont->addnext(track);
    //TR_DEBUG_CODE_42;
  }

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
      double rx = fabs(track->GetResidualO(ily+1, mfit).x());
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
  TRCHAFFKEY.init();

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
    TrRecHitR *hits[_SCANLAY];
    int nhit = 0;
    int hpat = 0xff;
    for (int j = 0; j < _SCANLAY; j++) hits[j] = 0;

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
  // number d=bb.norm();
  // number X=bb[0];
  // number Y=bb[1];
  //   hman.Fill("TkTrd",c,d);
  //   hman.Fill("TkTrdXY",X,Y);
  //   hman.Fill("ntrdhit",ptrd->_Base._NHits,Y);
  //   if(ptrd->_Base._NHits>10)
  //     hman.Fill("Yresvsrig",ptrack->GetRigidity(),Y);
  bb[2]=fabs(c);
  return bb;
  
  //PZ DEBUG  printf(" TRDTK MATCH  cos %f dist %f\n",c,d);
}

int TrRecon::TkTRDMatch(TrTrackR* ptrack, AMSPoint& trdcoo, AMSDir& trddir, int select_tag)
{
  int mfit = TrTrackR::DefaultFitID;
  if (!ptrack->ParExists(mfit)) mfit = ptrack->Gettrdefaultfit();

  number SearchReg(1.1);
  number MaxCos(0.95);
  AMSPoint dst0 = BasicTkTRDMatch(ptrack, trdcoo, trddir, mfit);

  if (dst0[2] > MaxCos)
    hman.Fill("TkTrdD0", dst0[0], dst0[1]);

  // No match in Y-coo or angle between TrTrack-TRD tracks
  if (fabs(dst0[1]) > SearchReg || dst0[2] < MaxCos) return -1;

  // Good match between TrTrack-TRD tracks; no need to move
  if (fabs(dst0[0]) < SearchReg) return 1;
  //printf("The distance is X: %f Y: %f Angle: %f try to move ..\n",dst[0],dst[1],dst[2]);

  bool moved=MoveTrTrack(ptrack, trdcoo, trddir,  5., select_tag);
  if(moved){
    ptrack->FitT(mfit);
    AMSPoint dst1 = BasicTkTRDMatch(ptrack, trdcoo, trddir, mfit);
    hman.Fill("TkTrdDD", dst0[0], dst1[0]);
    return 10;
  }else{
    AMSPoint dst1 = BasicTkTRDMatch(ptrack, trdcoo, trddir, mfit);
    hman.Fill("TkTrdDD", dst0[0], dst1[0]);
    return 0;
  }
}



bool TrRecon::MoveTrTrack(TrTrackR* ptr,AMSPoint& pp, AMSDir& dir, float err, int select_tag){
  // search if the XY hits are laying with the X road
  int good_mult[9][2];
  int ngood=0;
  int nxy=0;
  for(int jj=0;jj<ptr->getnhits();jj++){
    TrRecHitR* phit=ptr->GetHit(jj);
    if (phit->OnlyY()) continue;
    if (phit->GetLayer()==1||phit->GetLayer()>7) continue;
    nxy++;
    float X=pp[0]+dir[0]/dir[2]*(phit->GetCoord().z()-pp[2]);
    int mm;
    float max;
    max=9999.;
    mm=-1;
    for(int ii=0;ii<phit->GetMultiplicity();ii++){
      float diff=X- phit->GetCoord(ii).x();
      if(fabs(diff) <max) { 
	max=fabs(diff);
	mm=ii;
      }
    }
    float base=(err>3)?2:0;
    hman.Fill("trdmatch",max,(mm>-1)?(base+1.):(base+2.));
    if(mm>-1 && max < (err*(1.+(phit->GetLayer()-1)*0.017))) {
      good_mult[ngood][0]=jj;
      good_mult[ngood++][1]=mm;
    }
  }
  if(ngood!=nxy || ngood<2) return false;

//   // prepare a simple fit fit with the moved point
//   TrFit fit1, fit2;
//   double ferr = 300e-4;
//   for (int ii=0;ii<ngood;ii++){
//     TrRecHitR* phit=ptr->GetHit(good_mult[ii][0]);
//     AMSPoint coo1 = phit->GetCoord();
//     AMSPoint coo2 = phit->GetCoord(good_mult[ii][1]);
//     fit1.Add(coo1, ferr, ferr, ferr);
//     fit2.Add(coo2, ferr, ferr, ferr);
//   }
//   if (fit1.SimpleFit() < 0 || fit2.SimpleFit() < 0) return false;

  // move the XY hits 
  for (int ii=0;ii<ngood;ii++){
    TrRecHitR* phit=ptr->GetHit(good_mult[ii][0]);
    phit->SetResolvedMultiplicity(good_mult[ii][1]);
  }
  
  double sx=0;
  double sx2=0;
  double sy=0;
  double sxy=0;
  int    N=0;
  for (int ii=0;ii<ngood;ii++){
    TrRecHitR* phit=ptr->GetHit(good_mult[ii][0]);
    if(phit->OnlyY()) continue;
    AMSPoint coo1 = phit->GetCoord();
    sx+=coo1[2];
    sx2+=coo1[2]*coo1[2];
    sy+=coo1[0];
    sxy+=coo1[2]*coo1[0];
    N++;
  } 
  double Delta=N*sx2-sx*sx;
  
  double q=1/Delta*(sx2*sy - sx *sxy);
  double m=1/Delta*(N*sxy - sx *sy);

  //  Lets try to find the best hit for the new track on layer 1

  TrRecHitR* phit2=ptr->GetHitLJ(2);


  // Check that hit on lay 1 exists and it is not Y only
  if(phit2 && !phit2->OnlyY()){
    TrClusterR*   cl1y=phit2->GetYCluster();
    float X=q+m*phit2->GetCoord().z();
    VCon* cont=GetVCon()->GetCont("AMSTrRecHit");
    int idx=-1;
    int mm=-1;
    float max=9999.;
    
    for (int kk=0;kk< cont->getnelem();kk++){
      // loop on all the hits containing y, searching for a better x 
      TrRecHitR *hit = (TrRecHitR*) cont->getelem(kk);
      if (!hit) continue;
      if (hit->Used()) continue;
      if ( (select_tag!=0)&&(!hit->checkstatus(select_tag)) ) continue;
      if (hit->GetYCluster()!=cl1y) continue;
      for (int mult=0; mult<hit->GetMultiplicity(); mult++){
	float diff = X - hit->GetCoord(mult).x();
	if(fabs(diff)<max){
	  max=fabs(diff);
	  idx=kk;
	  mm=mult;
	}
      }
    }
    
    if(idx!=-1&&mm!=-1){
      TrRecHitR *hit_new = (TrRecHitR*) cont->getelem(idx);
      if (hit_new!=phit2){
	//add the new one
	ptr->AddHit(hit_new,mm);
	hit_new->SetUsed();
      }  
      hit_new->SetResolvedMultiplicity(mm);
    }
    if(cont) delete cont;
  }


  sx=0;
  sx2=0;
  sy=0;
  sxy=0;
  N=0;
  for (int ii=0;ii<ptr->getnhits();ii++){
    TrRecHitR* phit=ptr->GetHit(ii);
    if(phit->OnlyY()) continue;
    AMSPoint coo1 = phit->GetCoord();
    sx+=coo1[2];
    sx2+=coo1[2]*coo1[2];
    sy+=coo1[0];
    sxy+=coo1[2]*coo1[0];
    N++;
  } 
  Delta=N*sx2-sx*sx; 
  q=1/Delta*(sx2*sy - sx *sxy);
  m=1/Delta*(N*sxy - sx *sy);


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
	else phit->SetDummyX(tks.GetCloseChanX());
	//phit->BuildCoordinates();	  
	phit->SetResolvedMultiplicity(tks.GetMultIndex());
      }
    }
  }

  return true;
}


bool TkTOFMatch(TrTrackR* tr, int select_tag);
bool TkTOFMatch2(TrTrackR* tr, int select_tag);

int TrRecon::MatchTOF_TRD(TrTrackR* tr, int select_tag){
  int  TRDdone = -10;
  bool TOFdone = false;
  //return 0;
  if((TRCLFFKEY.ExtMatch/10)>0) {
  tr->FitT(tr->Gettrdefaultfit());
    TOFdone = TkTOFMatch2(tr, select_tag);
  }

  if (!(TOFdone) &&(TRCLFFKEY.ExtMatch%10)>0) {
#ifdef __ROOTSHAREDLIBRARY__
    AMSEventR *evt = AMSEventR::Head();
    for (int i = 0; evt && i < evt->nTrdTrack(); i++) {
      TrdTrackR *trd = evt->pTrdTrack(i);
      AMSPoint pp(trd->Coo[0], trd->Coo[1], trd->Coo[2]);
      AMSDir   dd(trd->Theta,  trd->Phi);

#else
    for (AMSTRDTrack*  trd=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,1); trd; trd=trd->next()) {
      AMSPoint pp= trd->getCooStr();
      AMSDir dd=trd->getCooDirStr(); 
#endif
      TRDdone=TkTRDMatch(tr,pp,dd,select_tag); 
      if (TRDdone>0) {
        tr->FitT(tr->Gettrdefaultfit());
        if (log10(tr->GetNormChisqX(tr->Gettrdefaultfit()))>TRCLFFKEY.logChisqXmax) TRDdone = -9;
      }
      if (TRDdone>0) break;
    }
  }
  
  if(TRDdone/10>0||TOFdone) {
    if (tr->FitT(tr->Gettrdefaultfit() > 0))
      tr->RecalcHitCoordinates(tr->Gettrdefaultfit());
  }

  return 0;
}
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------

///////////////////////////////////////////////////////////
//                  VERTEX                     ////////////
///////////////////////////////////////////////////////////


int TrRecon::BuildVertex(integer refit){
   
  VCon *pcvt = GetVCon()->GetCont("AMSVtx");
  if (!pcvt) {
    cerr << "TrRecon::BuildVertex-E-Cant Find AMSVtx Container" << endl;
    return -1;
  }   

  VCon *pctr = GetVCon()->GetCont("AMSTrTrack");
  if (!pctr) {
    cerr << "TrRecon::BuildVertex-E-Cant Find AMSTrTrack Container" << endl;
    return -1;
  }

  if (refit == 1) pcvt->eraseC();

  enum { Nmax = 10 };
  TrTrackR *ptrack[Nmax];

  int ntr = 0;
  for (int ii = 0; ii < pctr->getnelem() && ntr < Nmax; ii++) {
    TrTrackR *ptr = (TrTrackR *)pctr->getelem(ii);
    if (refit != 2 || (ptr->checkstatus(AMSDBc::TOFFORGAMMA) && ntr < 2))
//  if (!ptr->checkstatus(AMSDBc::USED)) ptrack[ntr++] = ptr;
    ptrack[ntr++] = ptr;
  }

  int ret = 0;

  if (ntr == 2 && refit == 2) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSVtx  *p = new AMSVtx (ptrack[0], ptrack[1]);
#else
    VertexR *p = new VertexR(ptrack[0], ptrack[1]);
#endif
    if (p->IsFilled()) {
      pcvt->addnext(p); 
      ret = 1;
    }
  }
  else if (ntr >= 2) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSVtx  *p = new AMSVtx (ntr, ptrack);
#else
    VertexR *p = new VertexR(ntr, ptrack);
#endif
    if (p->IsFilled()) {
      pcvt->addnext(p); 
      ret = 1;
    }
  }

  delete pcvt;
  delete pctr;

  return ret;
}



//========================================================
// Charge methods
//========================================================

void TrRecon::ClearChargeSeeds() {
  _htmy = 0;
  _htmx = 0;
  _htmx_corr = 0;
  _htmy_corr = 0; 
}

void TrRecon::FillChargeSeeds() {
  _htmx = TrCharge::GetMeanHighestFourClusters(TrCharge::kInner|TrCharge::kTruncMean,TrCharge::kX,TrClusterR::kAsym|TrClusterR::kAngle).Mean;
  _htmy = TrCharge::GetMeanHighestFourClusters(TrCharge::kInner|TrCharge::kTruncMean,TrCharge::kY,TrClusterR::kAsym|TrClusterR::kAngle).Mean;
  _htmx_corr = TrCharge::GetMeanHighestFourClusters(TrCharge::kInner|TrCharge::kTruncMean,TrCharge::kX,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle).Mean;
  _htmy_corr = TrCharge::GetMeanHighestFourClusters(TrCharge::kInner|TrCharge::kTruncMean,TrCharge::kY,TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle).Mean;  
  hman.Fill("CSxCSy",sqrt(_htmy),sqrt(_htmx));
  hman.Fill("CSxCSyC",sqrt(_htmy_corr),sqrt(_htmx_corr));
}

bool TrRecon::CompatibilityWithChargeSeed(TrClusterR* cluster) {
  int   iside  = cluster->GetSide();
  float signal = cluster->GetTotSignal(TrClusterR::kAsym|TrClusterR::kAngle); 
  float chseed = GetChargeSeed(iside);
  if (iside==0) hman.Fill("AmpxCSx",sqrt(chseed),sqrt(signal)); 
  else          hman.Fill("AmpyCSy",sqrt(chseed),sqrt(signal)); 
  float signal_corr = cluster->GetTotSignal(TrClusterR::kAsym|TrClusterR::kGain|TrClusterR::kAngle); 
  float chseed_corr = (iside==0) ? _htmx_corr : _htmy_corr; 
  if (iside==0) hman.Fill("AmpxCSxC",sqrt(chseed_corr),sqrt(signal_corr));
  else          hman.Fill("AmpyCSyC",sqrt(chseed_corr),sqrt(signal_corr));
  // test 
  // if (sqrt(signal)>(0.8*sqrt(chseed)-4)) return true;
  if (sqrt(signal_corr)>(0.7*sqrt(chseed_corr)-4)) return true; 
  return false;
}

bool TrRecon::CompatibilityWithChargeSeed(TrRecHitR* hit) {
  // invalid pointers 
  if      ( (hit->GetYCluster()==0)&&(hit->GetXCluster()==0) ) 
    return false;
  // if only y, y signal should be compatible with y charge seed
  else if ( (hit->GetYCluster()!=0)&&(hit->GetXCluster()==0) ) 
    return CompatibilityWithChargeSeed(hit->GetYCluster());
  // if only x, x signal should be compatible with x charge seed
  else if ( (hit->GetYCluster()==0)&&(hit->GetXCluster()!=0) ) 
    return CompatibilityWithChargeSeed(hit->GetXCluster());
  // if both, both x/y signals should be compatible with x/y charge seeds
  else if ( (hit->GetYCluster()!=0)&&(hit->GetXCluster()!=0) ) 
    return CompatibilityWithChargeSeed(hit->GetXCluster()) && CompatibilityWithChargeSeed(hit->GetYCluster());
  return false;
}


#include "TSystem.h"

double memchk(void)
{
  TString sfn(Form("/proc/%d/status", gSystem->GetPid()));
  std::ifstream fin(sfn);
  if (!fin) return 0;

  TString str;
  while (!fin.eof()) {
    str.ReadToken(fin);
    if (str == "VmSize:") {
      float vmsize;
      fin >> vmsize;
      return vmsize;
    }
  }
  return 0;
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




