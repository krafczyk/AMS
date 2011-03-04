#include "TrSim.h"
#include "tkdcards.h"
#include "random.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "amsgobj.h"
#include "trrec.h"
#else
#include "TrRawCluster.h"
#include "TrMCCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#endif

extern "C" double rnormx();

AMSPoint TrSim::sitkrefp[trconst::maxlay];
AMSPoint TrSim::sitkangl[trconst::maxlay];

TrMap<TrMCClusterR> TrSim::MCClusterTkIdMap;
//TrSimSensor TrSim::_sensors[3] = {TrSimSensor(0), TrSimSensor(1), TrSimSensor(2)};
// Initialization afterward
TrSimSensor TrSim::_sensors[3];


// FIX ME: particle table. If is rootshared read it. If not write it (please put the writing date). Commit one. 
//   // PARTICLE LIST
//   for(int ipart=0;ipart<1000;ipart++){
//     char chp[22]="";
//     integer itrtyp=0;
//     geant mass=0;
//     geant charge=0;
//     geant tlife=0;
//     geant ub[1];
//     integer nwb=0;
//     GFPART(ipart,chp,itrtyp,mass,charge,tlife,ub,nwb);
//     printf("%d %g %g\n",ipart,mass,charge);
//   }
// ATTENTION: this is the particle chart extracted from GBATCH (18/05/2010)  
float  TrSim::_g3mass[213] = {0,0,0.000510999,0.000510999,0,0.105658,0.105658,0.134976,0.13957,0.13957,0.497672,0.493677,0.493677,0.939566,0.938272,0.938272,0.497672,0.54745,1.11568,1.18937,1.19255,1.19744,1.3149,1.32132,1.67245,0.939566,1.11568,1.18937,1.19255,1.19744,1.3149,1.32132,1.67245,0,0,0,0,0,0,0,0,0,0,0,0,1.87561,2.80925,3.72742,0,2.80923,0,0,0,0,0,0,0,0,0,0,0,5.60305,6.53536,6.53622,8.39479,9.32699,10.2551,11.1779,13.0438,14.8992,17.6969,18.6228,21.4148,22.3419,25.1331,26.0603,28.8519,29.7818,32.5733,33.5036,36.2945,37.2249,41.8762,44.6632,47.454,48.3823,51.1745,52.1031,54.8959,53.9664,58.6186,59.5496,68.8571,74.4418,78.1631,81.8836,83.7457,91.1983,98.65,106.11,111.688,122.868,128.458,130.321,141.512,152.699,162.022,171.349,180.675,183.473,188.135,193.729,221.743,16.146,9.33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1.87561,2.80925,3.72742,0,2.80923,0,0,0,0,0,0,0,0,0,0,0,5.60305,6.53536,6.53622,8.39479,9.32699,10.2551,11.1779,13.0438,14.8992,17.6969,18.6228,21.4148,22.3419,25.1331,26.0603,28.8519,29.7818,32.5733,33.5036,36.2945,37.2249,41.8762,44.6632,47.454,48.3823,51.1745,52.1031,54.8959,53.9664,58.6186,59.5496,68.8571,74.4418,78.1631,81.8836,83.7457,91.1983,98.65,106.11,111.688,122.868,128.458,130.321,141.512,152.699,162.022,171.349,180.675,183.473,188.135,193.729,221.743};
// ATTENTION: this is the particle chart extracted from GBATCH (18/05/2010)  
float   TrSim::_g3charge[213] = {0,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,1,-1,0,0,0,1,0,-1,0,-1,-1,0,0,-1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,2,0,2,0,0,0,0,0,0,0,0,0,0,0,3,3,4,4,5,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,32,34,36,38,40,42,46,48,50,54,56,58,62,66,70,74,78,79,80,82,92,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,-1,-2,0,-2,0,0,0,0,0,0,0,0,0,0,0,-3,-3,-4,-4,-5,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16,-17,-18,-19,-20,-21,-22,-23,-24,-25,-26,-27,-28,-29,-30,-32,-34,-36,-38,-40,-42,-46,-48,-50,-54,-56,-58,-62,-66,-70,-74,-78,-79,-80,-82,-92};



void TrSim::sitkhits(int idsoft, float vect[], float edep, float step, int itra) {

  // PrintSimPars();

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkSimuAll"); // simulation counter
#endif

  // XXPLSS | XX=sensor| P 0 neg 1 pos | L=layer | SS=slot |
  // int sensor = abs(idsoft)/10000;
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  //  printf("TrSim::sitkhits: tkid %d LAYER %d !!!\n",tkid,tkid/100);
  TkLadder* lad=TkDBc::Head->FindTkId(tkid);
  // skip unnown ladders! This should never happen!!!
  if(!lad) { printf("TrSim::sitkhits-E tkid %d unknown This sholud never happen!!!!\n",tkid);return;}

  // Skip not connected ladders 
  if(lad->GetHwId()<0) {
    //  printf("TrSim::sitkhits: tkid %d NO HWID !!!\n",tkid);
    return;
  }

  // If fast simulation make the TrRawCluster directly with gencluster() 
  if (TRMCFFKEY.SimulationType==kNoRawSim) {
    gencluster(idsoft, vect, edep, step, itra);
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkSimuAll");
#endif
    return;
  }
  if (edep <= 0) edep = 1e-4;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,TkDBc::Head->_ssize_active[1]/2,TkDBc::Head->_silicon_z/2);
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];

  // Set reference points and angles
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }

  // Create a new object
  VCon* aa = GetVCon()->GetCont("AMSTrMCCluster");
  if (aa!=0)
#ifndef __ROOTSHAREDLIBRARY__
    aa->addnext(new AMSTrMCCluster(idsoft,  pgl,pmom,edep , itra));
#else
    aa->addnext(new TrMCClusterR(idsoft, pgl,pmom,edep , itra));
#endif
  if (aa!=0) delete aa;
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkSimuAll");
#endif
}



void TrSim::gencluster(int idsoft, float vect[], float edep, float step, int itra) {
  if (edep <= 0) edep = 1e-4;
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
                TkDBc::Head->_ssize_active[1]/2,
                TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa,1);
  TkSens tksb(tkid, ppb,1);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];
  int ily = abs(tkid)/100-1;
  sitkrefp[ily] = pgl;
  TkSens tksc(tkid, pgl,1);
  AMSPoint pc = tksc.GetSensCoo();
  if (pc.x() < 0 || pc.x() > TkDBc::Head->_ssize_active[0]) return;
  VCon* cont=GetVCon()->GetCont("AMSTrCluster");
  double lx = pc.x();
  double ly = pc.y();
  double lz = pc.z();
  int stx = tksc.GetStripX();
  int sty = tksc.GetStripY();
  double ipx = tksc.GetImpactPointX();
  double ipy = tksc.GetImpactPointY();
  int seedx = (ipx > 0) ? 0 : 1;
  int seedy = (ipy > 0) ? 0 : 1;
  if (ipx < 0) stx--;
  if (ipy < 0) sty--;
  double sig0  = 30;
  double sigx  = 5;
  double sigy  = 4;
  int sig_mode = 1;
  double thx = TMath::Abs(TMath::ATan(dirg.x()/dirg.z())*TMath::RadToDeg());
  double thy = TMath::Abs(TMath::ATan(dirg.y()/dirg.z())*TMath::RadToDeg());
  if (sig_mode == 1) {
    sigx = (40-1.6*thx+0.05*thx*thx)/5;
    sigy = (10+0.5*thy)/2.5;
  }
  sitkangl[ily] = AMSPoint(thx, thy, 0);

  double a1x = (ipx > 0) ? sig0*(1-ipx) : -sig0*ipx;
  double a2x = (ipx < 0) ? sig0*(1+ipx) :  sig0*ipx;
  double a1y = (ipy > 0) ? sig0*(1-ipy) : -sig0*ipy;
  double a2y = (ipy < 0) ? sig0*(1+ipy) :  sig0*ipy;

  a1x += rnormx()*sigx; a2x += rnormx()*sigx;
  a1y += rnormx()*sigy; a2y += rnormx()*sigy;
  if (a1x < 0) a1x = 0;
  if (a2x < 0) a2x = 0;
  if (a1y < 0) a1y = 0;
  if (a2y < 0) a2y = 0;
  double asx = a1x+a2x;
  double asy = a1y+a2y;

  if (seedx == 0 && a1x < a2x) seedx = 1;
  if (seedx == 1 && a1x > a2x) seedx = 0;
  if (seedy == 0 && a1y < a2y) seedy = 1;
  if (seedy == 1 && a1y > a2y) seedy = 0;

  int lyr = TMath::Abs(tkid)/100;

  float adc[2] = { 0, 0 };

  float dummy = 0;
  float rndx  = (float)RNDM((int)dummy);
  float rndy  = (float)RNDM((int)dummy);
  int  layer = abs(tkid)/100;
  float effx = (layer == 8) ? 0.87 : 0.90;
  float effy = 1;

  // 20-23/Dec/2009 CR test (Offline S/N thd.=3.5)
  if (tkid == -202) effy = 0.25;
  if (tkid ==  213) effx = 0.70;
  if (tkid ==  214) effx = 0.50;
  if (tkid == -405) effx = 0.50;
  if (tkid ==  405) effx = 0;
  if (tkid == -509) effx = 0.75;
  if (tkid == -512) effx = 0.50;
  if (tkid ==  504) effx = 0.70;
  if (tkid ==  510) effx = 0.35;
  

  if (rndx < effx && asx+asy > 0 && 0 < stx && stx+seedx < 383) {
    adc[0] = a1x;
    adc[1] = a2x;
#ifndef __ROOTSHAREDLIBRARY__ 
    if(cont!=0) cont->addnext(new AMSTrCluster
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#else
    if(cont!=0) cont->addnext(new TrClusterR
			   (tkid, 0, stx+640, 2, seedx, adc, 0));
#endif
  }
  if (rndy < effy && asx+asy > 0 && 0 < sty && sty+seedy < 639) {
    adc[0] = a1y;
    adc[1] = a2y;
#ifndef __ROOTSHAREDLIBRARY__ 
    if (cont) cont->addnext(new AMSTrCluster(tkid, 0, sty, 2, seedy, adc, 0));
#else
    if (cont) cont->addnext(new   TrClusterR(tkid, 0, sty, 2, seedy, adc, 0));
#endif
  }
  if (cont!=0) delete cont;
  return; 
}


void TrSim::fillreso(TrTrackR *track) {
  // if (fabs(track->GetRigidity()) < 100) return;
  for (int i = 0; i < track->GetNhits(); i++) {
    TrRecHitR *hit = track->GetHit(i);
    int        ily = hit->GetLayer()-1;
    AMSPoint   dpc = hit->GetCoord()-sitkrefp[ily];
    hman.Fill("TrSimRx", sitkangl[ily].x(), dpc.x()*1e4);
    hman.Fill("TrSimRy", sitkangl[ily].y(), dpc.y()*1e4);
  }
}



void TrSim::sitkdigi() {

  if (VERBOSE) printf("TrSim::sitkdigi-V called\n");

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkDigiAll"); 
#endif

  // If fast simulation skip, digitization is already done (gencluster)!
  if (TRMCFFKEY.SimulationType==kNoRawSim) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiAll");
#endif
    return;
  }

  // Beginning of other digitizations (GBATCH, TrSim2010)
#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.start("SiTkDigiShow");
#endif

  // Initialize _sensor if not yet
  if (_sensors[0].GetSensorType() < 0 ||
      _sensors[1].GetSensorType() < 0 ||
      _sensors[2].GetSensorType() < 0) {
    cout << "TrSim::sitkdigi-I-Initialize TrSimSensors" << endl;
    InitSensors();
  }

  // Create the TrMCCluster map and make the simulated cluster (_shower(), GenSimCluster())
  CreateMCClusterTkIdMap();

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkDigiShow");
#endif

  // Skip digitization if the number of cluster is too low  
  if (MCClusterTkIdMap.Size()<TRMCFFKEY.MinMCClusters) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiAll");
#endif
    return;
  }

  // Number of "fully" simulated ladders
  int nsimladders = 0;

  // Loop on all the ladders
  for (int icrate=0; icrate<8; icrate++) {
    for (int itdr=0; itdr<24; itdr++) {
      int hwid = icrate*100 + itdr;
      int tkid = (TkDBc::Head->FindHwId(hwid))->GetTkId();

      // Skip ladders if no simulation is forseen (is not full noise & there are no clusters)
      if ( (TRMCFFKEY.NoiseType!=1)&&(MCClusterTkIdMap.GetNelem(tkid)<=0) ) continue; // speed-up!

      // Initialize the ladder buffer  
      double  ladbuf[1024]; // = {0};
      memset(ladbuf,0,1024*sizeof(ladbuf[0]));

      // Take the right calibration
      TrLadCal* ladcal = TrCalDB::Head->FindCal_HwId(hwid);
      if (ladcal==0) {
        if (WARNING) printf("TrSim::AddNoiseOnBuffer-W no ladder calibration found, no noise for ladder tkid=%+04d\n",tkid);
        continue;
      }

      // Number of "ideal" cluster
      int nclu = 0;

#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.start("SiTkDigiLadd");
#endif
      // Loop on MC Clusters  
      for(int icl=0; icl<MCClusterTkIdMap.GetNelem(tkid); icl++) {
        TrMCClusterR* cluster = (TrMCClusterR*) MCClusterTkIdMap.GetElem(tkid,icl);
        if (cluster==0) continue;  
        // Add ideal clusters
        if      (TRMCFFKEY.SimulationType==kRawSim)    nclu += AddOldSimulationSignalOnBuffer(cluster,ladbuf);
        else if (TRMCFFKEY.SimulationType==kTrSim2010) nclu += AddTrSimClustersOnBuffer(cluster,ladbuf);
      }
#ifndef __ROOTSHAREDLIBRARY__
      AMSgObj::BookTimer.stop("SiTkDigiLadd");
#endif

      // Add noise simulation  
      if ( (TRMCFFKEY.NoiseType==1) ||             // noise on all ladders
  	   (TRMCFFKEY.NoiseType==2)&&(nclu>0) ) {  // noise on ladders with some cluster 
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.start("SiTkDigiNoise");
#endif
        AddNoiseOnBuffer(ladbuf,ladcal);
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.stop("SiTkDigiNoise");
#endif
      } 

      // DSP Simulation
      if ( (TRMCFFKEY.NoiseType==1) ||  // DSP on all ladders
           (nclu>0) ) {                 // DSP only on ladders with some cluster/noise
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.start("SiTkDigiDsp");
#endif
        // DSP on n-side
        BuildTrRawClustersWithDSP(0,tkid,ladcal,ladbuf);         
        // DSP on p-side
        BuildTrRawClustersWithDSP(1,tkid,ladcal,ladbuf); 
        // Number of ladders "fully" simulated
        nsimladders++;
#ifndef __ROOTSHAREDLIBRARY__
        AMSgObj::BookTimer.stop("SiTkDigiDsp");
#endif
      }
    }
  }

  // Build fake noise clusters
  if ( (TRMCFFKEY.NoiseType>1)&&(TRMCFFKEY.FakeClusterType>0) ) {
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.start("SiTkDigiFake");
#endif
    sitknoise(nsimladders);
#ifndef __ROOTSHAREDLIBRARY__
    AMSgObj::BookTimer.stop("SiTkDigiFake");
#endif
  }

#ifndef __ROOTSHAREDLIBRARY__
  AMSgObj::BookTimer.stop("SiTkDigiAll"); 
#endif
  
  return;
}


void TrSim::CreateMCClusterTkIdMap() {
  // Generate the list of MC Cluster on each ladder
  MCClusterTkIdMap.Clear();
  // Get the pointer to the TrMCCluster container
  VCon* container = GetVCon()->GetCont("AMSTrMCCluster");
  if (container==0) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-W no TrMCCluster container, skip.\n");
    return;
  }
  if (container->getnelem()==0) {
    if (WARNING) printf("TrSim::CreateMCClusterTkIdMap-W TrMCCluster container is empty, skip.\n");
    if (container!=0) delete container;
    return;
  }
  for (int ii=0; ii<container->getnelem(); ii++) {
    TrMCClusterR* cluster = (TrMCClusterR*) container->getelem(ii);
    if (cluster) {
      // Construct ideal clusters!!!    
      if      (TRMCFFKEY.SimulationType==kRawSim)    cluster->_shower();
      else if (TRMCFFKEY.SimulationType==kTrSim2010) cluster->GenSimClusters();
      MCClusterTkIdMap.Add(cluster);
    }
  }
  if (container!=0) delete container;
  return;
}


void TrSim::AddNoiseOnBuffer(double* ladbuf, TrLadCal* ladcal) {
  // Noise baseline 
  for (int ii=0; ii<1024; ii++) {
    if (ladcal->GetStatus(ii)&TrLadCal::dead) ladbuf[ii] = DEADSTRIPADC; // dead strip    
    else                                      ladbuf[ii] += ladcal->Sigma(ii)*rnormx(); // normal/hot stri
  }
  /* 
     TMP: NO NOISE ONLY NEAR THE SIGNAL
     for now just create noise only on ladders with signal
     if (TRMCFFKEY.TrSim2010_NoiseType==2) {
     if (ladbuf[ii] > 0 ||
     (ii+1 < 1024 && ladbuf[ii+1] > 0) ||
     (ii-1 >=   0 && ladbuf[ii-1] > 0))
     ladbuf[ii] += ladcal->Sigma(ii)*rnormx();
     }
  */
}


int TrSim::AddTrSimClustersOnBuffer(TrMCClusterR* cluster, double* ladbuf) {
  int nclusters = 0;
  bool isK7 = ((TkLadder*)TkDBc::Head->FindTkId(cluster->GetTkId()))->IsK7();
  // Loop on the two sides
  for (int iside=0; iside<2; iside++) {
    TrSimCluster* simcluster = cluster->GetSimCluster(iside);
    if (simcluster==0) continue;
    // Putting cluster on the ladder buffer 
    for (int ist=0; ist<simcluster->GetWidth(); ist++) {
      // Take address on buffer
      int address = simcluster->GetAddress(ist) + 640*(1-iside); 
      if ( (isK7)&&(iside==0) ) address = simcluster->GetAddressCycl(ist) + 640; // considering cyclicity
      if ( (0<=address)&&(address<1024) ) ladbuf[address] += simcluster->GetSignal(ist);
      else printf("TrSim::AddSimulatedClustersOnBuffer-E address out of bounds (%d)\n",address);
    }
    nclusters++;
  }
  return nclusters;
}


int TrSim::AddOldSimulationSignalOnBuffer(TrMCClusterR* cluster, double* ladbuf) {
  printf("TrSim::AddOldSimulationSignalOnBuffer-E OLD Simulation declared OBSOLETE!! I'm not doing anything\n");
  return -1;
//   int addK = cluster->GetAdd(0);
//   for (int jj=0;jj<cluster->GetSize(0);jj++) ladbuf[addK+jj] += cluster->GetSignal(jj,0);
//   int addS = cluster->GetAdd(1);
//   for (int jj=0;jj<cluster->GetSize(1);jj++) ladbuf[addS+jj] += cluster->GetSignal(jj,1);
//   return 2;
}


int TrSim::BuildTrRawClustersWithDSP(const int iside, const int tkid, TrLadCal* ladcal,double * ladbuf) {
  // Does the calibration exist?
  if (!ladcal) {
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-W no ladder calibration found, no noise for ladder tkid=%+04d\n",tkid);
    return 0;
  }

  // Does the AMSTrRawCluster container exist?
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if (!cont) {
    if (WARNING) printf("TrSim::BuildTrRawClustersOnSide-W cannot find AMSTrRawCluster, no cluster produced\n");
    return 0;
  } 

  // Init
  int nclusters = 0;
  int addmin[2] = {  0, 640};
  int addmax[2] = {640,1024};
  int used[1024]; memset(used,0,1024*sizeof(used[0]));

  // Loop on address (0 - 640, or 640 - 1024)
  for (int ii=addmin[iside]; ii<addmax[iside]; ii++) {

    if (used[ii]!=0)           continue; // a strip used in a cluster 
    if (ladcal->Status(ii)!=0) continue; // a strip with status different from 0
    if ( (ladbuf[ii]/ladcal->Sigma(ii))<=TRMCFFKEY.DSPSeedThr[iside] ) continue; // a strip under seed threshold

    // "Seed" Found (not max)
    int seed = ii;
    used[ii] = 1;
    int maxstrip = ii;

    // Left Loop
    int left = seed;
    for (int jj=seed-1; jj>=addmin[iside]; jj--) {
      if ( (((ladbuf[jj]/ladcal->Sigma(jj))>TRMCFFKEY.DSPNeigThr[iside])||(ladcal->Status(jj)!=0)) && (used[jj]==0) ) {
        used[jj] = 1;
        left = jj;
      } 
      else 
        break;
    }
 
    // Right Loop
    int right = seed;
    for (int jj=seed+1; jj<addmax[iside]; jj++) {
      if ( (((ladbuf[jj]/ladcal->Sigma(jj))>TRMCFFKEY.DSPNeigThr[iside]) || (ladcal->Status(jj)!=0)) && used[jj]==0) {
        used[jj] = 1;
        right = jj;
      } 
      else 
        break;
    }

    // at least 3 strips!
    if(left >addmin[iside])     left--;
    if(right<(addmax[iside]-1)) right++;

    // Preparing the cluster
    short int signal[128];
    int nstrips = right - left + 1;
    for(int ist=0; ist<nstrips; ist++) { 
      signal[ist%128] = int(8*ladbuf[left + ist]); 
      if ( (((ist+1)%128)==0) || ((ist+1)==nstrips) ) { 
        int cluslenraw = (((ist+1)%128)==0) ? 127 : (nstrips%128)-1; // 0->1, 1->2, ..., 127->128
        int seedvalue  = int(4*ladbuf[maxstrip]);
        // cluslenraw |= (seedvalue<<7);
        int n128 = int((ist+1)/128);
        int clusaddraw = left + 128*n128;
        // clusaddraw |= (0<<10); // CN bits status (not simulated)
        // clusaddraw |= (0<<14); // Power Bits status (not simulated)
#ifndef __ROOTSHAREDLIBRARY__
        if (cont) cont->addnext(new AMSTrRawCluster(tkid,clusaddraw,cluslenraw,signal));
#else
        if (cont) cont->addnext(new TrRawClusterR(  tkid,clusaddraw,cluslenraw,signal));
#endif
	nclusters++;
        if (VERBOSE) {
          printf("TrSim::TrRawCluster\n");
          TrRawClusterR* cluster = (TrRawClusterR*) cont->getelem((int)cont->getnelem()-1);
          cluster->Print(10);
          printf("TrSim::TrRawCluster-More Local Seed Coordinate = %f\n",TkCoo::GetLocalCoo(tkid,cluster->GetSeedAddress(),0));
        }
      }
    }
  }
  if (cont!=0) delete cont;
  return nclusters;
}

void TrSim::sitknoise(int nsimladders) {
  // does the AMSTrRawCluster container exist?
  VCon* cont = GetVCon()->GetCont("AMSTrRawCluster");
  if(!cont){
    if (WARNING) printf("TrSim::sitknoise-W cannot find AMSTrRawCluster, no fake cluster produced\n");
    return;
  } 
  // preliminary calculation
  float dummyfloat=0;
  int   dummyint=0;
  int   nstrips[2] = {384, 640};
  int   nchannel[2];
  float integral[2];
  int   ncluster[2];
  for (int iside=0; iside<2; iside++) {
    nchannel[iside] = (192-nsimladders)*nstrips[iside]; // simulate only the not-already-simulated ladders
    integral[iside] = TMath::Erfc(TRMCFFKEY.DSPSeedThr[iside]/sqrt(2))/2.; // prob-to-be over threshold
    // ncluster[iside] = nchannel[iside]*integral[iside]*(1 + rnormx()*(1-integral[iside])); // gaus as binomial approximation
    float mean = nchannel[iside]*integral[iside];
    if (TRMCFFKEY.FakeClusterType==2) mean*=2; // --> tuning on real data (KSC and TbAug2010, considering low thresholds)
    POISSN(mean,ncluster[iside],dummyint);     // poissn as a binomial approximation (n>=20 and p<=0.05)
  }
  // cluster
  short int signal[128];
  vector<float> clusterSN;
  if (VERBOSE) printf("sitknoise::pclusters=%3d nclusters=%3d\n",ncluster[0],ncluster[1]);
  // loop
  for (int iside=0; iside<2; iside++) {
    for (int iclu=0; iclu<ncluster[iside]; iclu++) {
      // creation of a SN cluster 
      clusterSN.clear();
      // extract a not-already-simulated ladder
      bool ladderfound = false;
      int tkid = 0;
      while (!ladderfound) {
	int ientry = int(192.*RNDM(dummyint));
        int hwid = int(ientry/24)*100 + (ientry%24);
	TkLadder* ladder = TkDBc::Head->FindHwId(hwid);
	tkid = ladder->GetTkId();
	if (MCClusterTkIdMap.GetNelem(tkid)<=0) ladderfound = true;
      }
      // take the right calibration
      TrLadCal* ladcal = TrCalDB::Head->FindCal_TkId(tkid);
      if (ladcal==0) {
	if (WARNING) printf("TrSim::sitknoise-W no ladder calibration found, no noise for ladder tkid=%+04d skipping creation of fake clusters\n",tkid);
	continue;
      }
      // extract seed position 
      int seedadd = int(nstrips[iside]*RNDM((int)dummyfloat));
      int address = seedadd;
      // extract seed value SN
      float aminim = 1-integral[iside];
      float amaxim = 1.;    
      float extrac = aminim + (amaxim-aminim)*RNDM(dummyint);
      float seedSN = sqrt(2)*TMath::ErfInverse(2*extrac-1);
      // printf("min=%10.8g max=%10.8g extr=%7.3f seed=%7.3f\n",aminim,amaxim,extrac,seedSN);
      clusterSN.push_back(seedSN);
      // left loop
      for (int add=seedadd-1; add>=0; add--) {
        float SN = rnormx();
        while (SN>=TRMCFFKEY.DSPSeedThr[iside]) SN = rnormx();
        // cout << "+- " << SN << " " << ladcal->Status(add) << endl;
        if ( (SN>TRMCFFKEY.DSPNeigThr[iside])||(ladcal->Status(add)!=0) ) { 
	  clusterSN.insert(clusterSN.begin(),SN);
	  address = add;
        }
	else { 
          clusterSN.insert(clusterSN.begin(),SN); // additional one
          address = add;
	  break;
        }
      }	
      // right loop
      for (int add=seedadd+1; add<nstrips[iside]; add++) {
        float SN = rnormx();
        while (SN>=TRMCFFKEY.DSPSeedThr[iside]) SN = rnormx();
        // cout << "++ " << SN << " " << ladcal->Status(add) << endl;
        if ( (SN>TRMCFFKEY.DSPNeigThr[iside])||(ladcal->Status(add)!=0) ) {
          clusterSN.push_back(SN);
        }
        else {
          clusterSN.push_back(SN); // additional one
          break;
        }
      } 
      // normalizing to noise (no gain correction for pure noise clusters)
      address += (1-iside)*640;
      int nstrips = int(clusterSN.size());
      for (int ii=0; ii<nstrips; ii++) {
        if (ladcal->GetStatus(address+ii)&TrLadCal::dead) signal[ii] = int(8*DEADSTRIPADC);                               // dead strip    
	else                                              signal[ii] = int(8*clusterSN.at(ii)*ladcal->Sigma(address+ii)); // normal strips
      }
      // FIX ME: Check if is a good cluster? (no repetition of used strips, no under threshold strips, ...)
      int cluslenraw = (nstrips%128)-1;
      int clusaddraw = address;
#ifndef __ROOTSHAREDLIBRARY__
      if (cont) cont->addnext(new AMSTrRawCluster(tkid,clusaddraw,cluslenraw,signal));
#else
      if (cont) cont->addnext(new TrRawClusterR(  tkid,clusaddraw,cluslenraw,signal));
#endif	
      if (VERBOSE) {
        printf("TrSim::TrRawCluster\n");
        TrRawClusterR* cluster = (TrRawClusterR*) cont->getelem((int)cont->getnelem()-1);
        cluster->Print(10);
        printf("TrSim::TrRawCluster-More Local Seed Coordinate = %f\n",TkCoo::GetLocalCoo(tkid,cluster->GetSeedAddress(),0));
      }
    }
  }
  if(cont) delete cont; 
  return;
}



TrSimSensor* TrSim::GetTrSimSensor(int side, int tkid) {
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll) {
    printf("TrSim::GetTrSimSensor-E cannot find ladder %d into the database, returning 0.\n",tkid);
    return 0;
  }
  if (side==1)    return &_sensors[0]; // S 
  if (ll->IsK7()) return &_sensors[2]; // K7
  else            return &_sensors[1]; // K5
  return 0; 
} 


void TrSim::PrintBuffer(double *_ladbuf) {
  for (int ii=0; ii<64; ii++) {
    for (int jj=0; jj<16; jj++) {
      printf("%5.2f ",_ladbuf[ii*16+jj]);
    }
    printf("\n");
  }
}


void TrSim::PrintSimPars() {
  cout << "SimulationType:             " << TRMCFFKEY.SimulationType << endl;
  cout << "MinMCClusters:              " << TRMCFFKEY.MinMCClusters << endl;
  cout << "NoiseType:                  " << TRMCFFKEY.NoiseType << endl;
  cout << "FakeClusterType:            " << TRMCFFKEY.FakeClusterType << endl;
  cout << "DSPSeedThr(n,p):            " << TRMCFFKEY.DSPSeedThr[0] << " " << TRMCFFKEY.DSPSeedThr[1] << endl;
  cout << "DSPNeigThr(n,p):            " << TRMCFFKEY.DSPNeigThr[0] << " " << TRMCFFKEY.DSPNeigThr[1] << endl;
  cout << "TrSim2010_ADCConvType(n,p): " << TRMCFFKEY.TrSim2010_ADCConvType[0] << " " << TRMCFFKEY.TrSim2010_ADCConvType[1] << endl;
  cout << "TrSim2010_EDepType(n,p):    " << TRMCFFKEY.TrSim2010_EDepType[0] << " " << TRMCFFKEY.TrSim2010_EDepType[1] << endl;
  cout << "TrSim2010_ADCSat(n,p):      " << TRMCFFKEY.TrSim2010_ADCSat[0] << " " << TRMCFFKEY.TrSim2010_ADCSat[1] << endl;
  cout << "TrSim2010_Cint(n,p):        " << TRMCFFKEY.TrSim2010_Cint[0] << " " << TRMCFFKEY.TrSim2010_Cint[1] << endl;
  cout << "TrSim2010_Cbk(n,p):         " << TRMCFFKEY.TrSim2010_Cbk[0] << " " << TRMCFFKEY.TrSim2010_Cbk[1] << endl;
  cout << "TrSim2010_Cdec(n,p):        " << TRMCFFKEY.TrSim2010_Cdec[0] << " " << TRMCFFKEY.TrSim2010_Cdec[1] << endl;
  cout << "TrSim2010_DiffType(n,p):    " << TRMCFFKEY.TrSim2010_DiffType[0] << " " << TRMCFFKEY.TrSim2010_DiffType[1] << endl;
  cout << "TrSim2010_DiffRadius(n,p):  " << TRMCFFKEY.TrSim2010_DiffRadius[0] << " " << TRMCFFKEY.TrSim2010_DiffRadius[1] << endl;
  cout << "TrSim2010_FracNoise(n,p):   " << TRMCFFKEY.TrSim2010_FracNoise[0] << " " << TRMCFFKEY.TrSim2010_FracNoise[1] << endl;
  cout << "TrSim2010_AddNoise(n,p):    " << TRMCFFKEY.TrSim2010_AddNoise[0] << " " << TRMCFFKEY.TrSim2010_AddNoise[1] << endl;
}
