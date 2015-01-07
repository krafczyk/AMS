#include "InterMVA.h"
#include "ConfigParser.h"
#include "MVAmanager.h"
#include "OutTreeManager.h"
#include "OutHistoManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <bitset>
#include <sys/resource.h>
#include <omp.h>
#include <dirent.h>


#include <root.h>
#include <amschain.h>
#include "TrdSCalib.h"
#include "TrdHCalib.h"
#include "TrTrackSelection.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "DynAlignment.h"
#include "TrdKCluster.h"
#include "TrRecon.h"
#include "point.h"


#include "TrdQt.hh"
#include "TrkLH.h"

#include "TTree.h"
#include "TFrame.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"
//some constants
#include "constants.h"


InterMVA::InterMVA(string cfile) {
fmva=0;
ftree=0;
fconf=new ConfigParser(cfile);
Debug=fconf->Value("general","Debug");
Init();
}

InterMVA::InterMVA() {
fmva=0;
ftree=0;
string cfile="/afs/cern.ch/user/z/zhukov/work/public/AMSvdev/testmva/testmvaana.cfg";
fconf=new ConfigParser(cfile);
Debug=fconf->Value("general","Debug");
Init();
}


InterMVA::~InterMVA() {
  delete fmva;
  delete ftree;
}

// book simple out  tree filled from AMSchain, all doubles only
int InterMVA::Init() {
  if(Debug) cout<<" InterMVA:: init "<<endl;
  if(fmva||ftree) {cout<<"***already initialized InterMVA  "<<endl;return 1;}

fmva=new MVAmanager(fconf); //usemva
fmva->setmethods();
fmva->initreadvariables(); //also fill variables

ftree=new OutTreeManager();
if(!ftree) return 0;
ftree->AddPar("time");
ftree->AddPar("exptime");
ftree->AddPar("event");
ftree->AddPar("run");
ftree->AddPar("lat");
ftree->AddPar("long");
ftree->AddPar("trgltime");
ftree->AddPar("trgpat");
ftree->AddPar("trgjmem");
ftree->AddPar("nacccl");
ftree->AddPar("accpatt");
ftree->AddPar("dacctrk");
ftree->AddPar("accEdep");
//tof
ftree->AddPar("ntofcl");
ftree->AddPar("betah");
ftree->AddPar("tofQ");
ftree->AddPar("tofQe");
ftree->AddPar("tofQ1");
ftree->AddPar("tofQ2");
ftree->AddPar("tofQ3");
ftree->AddPar("tofQ4");
ftree->AddPar("tofQu");
ftree->AddPar("tofQl");
ftree->AddPar("tofZ");
ftree->AddPar("tofPZ");
ftree->AddPar("tofQL");
ftree->AddPar("XUtof");
ftree->AddPar("YUtof");
ftree->AddPar("ThetaUtof");
ftree->AddPar("PhiUtof");
ftree->AddPar("tofpatt");
ftree->AddPar("ntofclpatt"); 
ftree->AddPar("deltaToftmax1");
ftree->AddPar("deltaToftmax2");
//ecal
ftree->AddPar("necalsh");
ftree->AddPar("ecalE");
ftree->AddPar("ecalBDT");
ftree->AddPar("deltaEcal_x");
ftree->AddPar("deltaEcal_y");
ftree->AddPar("deltaEcal_a");
//trk
ftree->AddPar("trkQ");
ftree->AddPar("trkQe");
ftree->AddPar("trkextQ");
ftree->AddPar("trkextQe");
ftree->AddPar("trkQ1");
ftree->AddPar("trkQ2");
ftree->AddPar("trkQ9");
ftree->AddPar("trkQ34");
ftree->AddPar("trkQ56");
ftree->AddPar("trkQ78");
ftree->AddPar("trkmnoise1");
ftree->AddPar("trkmnoise2");
ftree->AddPar("trkmnoise3");
ftree->AddPar("trkmnoise4");

ftree->AddPar("trkrigdiff");
ftree->AddPar("TrkHitres1");
ftree->AddPar("TrkHitres9");
ftree->AddPar("TrkHitres2");
ftree->AddPar("TrkHitresmax");

ftree->AddPar("ntrk");
ftree->AddPar("ntrkh");
ftree->AddPar("Rig");
ftree->AddPar("iRigerr");
ftree->AddPar("trkrigdiff");
ftree->AddPar("trkghe");
ftree->AddPar("trkchX");
ftree->AddPar("trkchY");
ftree->AddPar("trkpat");
ftree->AddPar("Rig0");
ftree->AddPar("Rig1");
ftree->AddPar("Rig2");
ftree->AddPar("Rig3");
ftree->AddPar("Rig4");
ftree->AddPar("Rig5");
ftree->AddPar("trklh");
 //
ftree->AddPar("ntrdrh");
ftree->AddPar("ntrdtrk");
ftree->AddPar("ntrdhtrk");
ftree->AddPar("ntrdcl");
ftree->AddPar("trdLep");
ftree->AddPar("trdLhep");
ftree->AddPar("trdQ");
ftree->AddPar("trdQe");
ftree->AddPar("deltaTrd_ax");
ftree->AddPar("deltaTrd_ay");

 if(!ftree->InitTree()) {
cout<<"*** failed to init InterMVA tree "<<endl;
 return 0; 
      }
 return 1;
}

// populate OutTree branches from AMSEvent 
int InterMVA::filloutevent(AMSEventR* pev) {
if(Debug) cout<<"==filloutevent "<<endl;
if(!ftree) return -1; 
if(!pev) return -2;

int ipart=0; //use first particle if many
//Check objects
 if(!pev->nParticle()) return 1;
 ParticleR*  ppart=0;
 if(pev->NParticle()!=0)
 ppart  = pev->pParticle(ipart);
 if(!ppart) return 1;
//BetaH
if(!pev->nBetaH()) TofRecH::ReBuild();
BetaHR* ptofbetah = ppart->pBetaH();
if(!ptofbetah) return 2;
//Tracks
TrTrackR* ptrk = ppart->pTrTrack();
if(!ptrk) return 3;
// Charge
ChargeR* pcharge=pev->pCharge(0);
 if(!pcharge) return 4;
//ECAL
EcalShowerR* pecal = ppart->pEcalShower();
//TRD
TrdTrackR * ptrdtrk=ppart->pTrdTrack();
TrdHTrackR * ptrdhtrk=ppart->pTrdHTrack();
 //RICH	
RichRingBR *prichb=ppart->pRichRingB ();
Level1R* pLevel1 = pev->pLevel1(0);
if(!pLevel1) return 5;
BetaR* ptofbeta = ppart->pBeta();
if(!ptofbeta) return 6;

if(Debug>1) cout<<" ---select event "<<endl; 
 
ftree->FillTreePar("event",pev->Event());
ftree->FillTreePar("time",pev->UTime());
ftree->FillTreePar("run",pev->Run());
ftree->FillTreePar("lat", pev->fHeader.ThetaS);
ftree->FillTreePar("long",pev->fHeader.PhiS);
//objects used in analysis
 bool btrg=true; 
 bool bacc=true; 
 bool btof=true;
 bool btrd=true;
 bool btrk=true;
 bool becal=true;

 //btrg
 if(btrg) {
 ftree->FillTreePar("trgpat",pLevel1->PhysBPatt);
 ftree->FillTreePar("trgjmem", pLevel1->JMembPatt);
 ftree->FillTreePar("trgltime",pLevel1->LiveTime);  
 }
//bacc
 if(bacc) {
int naccsectors[8]={0.};
double accedep=0;
for (unsigned int iacc=0; iacc<pev->nAntiCluster (); iacc++){
 AntiClusterR* panti=pev->pAntiCluster(iacc);
 if(!panti) continue;
 accedep+= panti->Edep;
 int isec=panti->Sector;
 if(isec>=0&&isec<8) 
   naccsectors[isec]++;
 }//iacc
 if(accedep==0) accedep=-99;
 int accpatt=0;
 // analyse pattern
  if(naccsectors[0]) accpatt  |=1;
  if(naccsectors[1]) accpatt  |=2;
  if(naccsectors[2]) accpatt  |=4;
  if(naccsectors[3]) accpatt  |=8;
  if(naccsectors[4]) accpatt  |=16;
  if(naccsectors[5]) accpatt  |=32;
  if(naccsectors[6]) accpatt  |=64;
  if(naccsectors[7]) accpatt  |=128;

 ftree->FillTreePar("accpatt",accpatt); 
 ftree->FillTreePar("nacccl",pev->nAntiCluster ()); 
 ftree->FillTreePar("accEdep",accedep); // total acc energy deposit, MeV

 // calculate max distance from track at Z  
  AMSPoint amsptrk;
  AMSDir   amsdtrk;
double coo[3]={0.};
int id_inner=ptrk->iTrTrackPar(1,3,3);
 if(id_inner>0) {
   double distmax=0;
for (unsigned int iacc=0; iacc<pev->nAntiCluster (); iacc++){
 AntiClusterR* panti=pev->pAntiCluster(iacc);
 coo[0]=panti->AntiCoo[0]*cos(panti->AntiCoo[1]);
 coo[1]=panti->AntiCoo[0]*sin(panti->AntiCoo[1]);
 coo[2]=panti->AntiCoo[2];
 AMSPoint amsp1(coo[0], coo[1], coo[2]);
 ptrk->Interpolate(panti->AntiCoo[2],amsptrk,amsdtrk,id_inner);
 double dist=amsp1.dist(amsptrk);
 if(dist>distmax) distmax=dist;
 }//iacc
 ftree->FillTreePar("dacctrk",distmax);
 }//id_inner
} //bacc

 ////btof
 if(btof) {
ftree->FillTreePar("ntofcl",pev->nTofClusterH ());
ftree->FillTreePar("betah",ptofbetah->GetBetaC());
 // charges
 double tofq[4]={-99.};
 for (unsigned int ilay=0; ilay<4; ilay++){
      if(  ptofbetah->TestExistHL(ilay) ) {
	tofq[ilay]=ptofbetah->GetQL(ilay);
      }
    }
 //
 int nl;
 float qrms;
 // tofQ by layers
 double tofaq=ptofbetah->GetQ(nl,qrms);
ftree->FillTreePar("tofQ",tofaq); 
ftree->FillTreePar("tofQe",qrms); 
ftree->FillTreePar("tofQ1",tofq[0]);
ftree->FillTreePar("tofQ2",tofq[1]);
ftree->FillTreePar("tofQ3",tofq[2]);
ftree->FillTreePar("tofQ4",tofq[3]);
// tof up, low
  TofTrack* ptof_track = new TofTrack(ptofbeta,ptrk);
  double qtofu = ptof_track->GetChargePlane(TofTrack::kUpper);
  double qtofl = ptof_track->GetChargePlane(TofTrack::kLower);
ftree->FillTreePar("tofQu",qtofu); 
ftree->FillTreePar("tofQl",qtofl); 
  int ntofql;float tofpz;
ftree->FillTreePar("tofZ",ptofbetah->GetZ(ntofql,tofpz)); 
ftree->FillTreePar("tofPZ",tofpz); 
 ftree->FillTreePar("tofQL",ptofbetah->GetLikeQ(ntofql)); 
// toftrk match and tof pattern
// trk at utof position
  AMSPoint tofpnt;
  AMSDir tofdir;
int id_inner=ptrk->iTrTrackPar(1,3,3);
 if(id_inner>0) {
ptrk->Interpolate(ZTOFUpper,tofpnt,tofdir,id_inner);
 ftree->FillTreePar("XUtof", tofpnt.x());
 ftree->FillTreePar("YUtof", tofpnt.y());
 ftree->FillTreePar("ThetaUtof", tofdir.gettheta());
 ftree->FillTreePar("PhiUtof", tofdir.getphi());
// ala A.Contin
  double deltaTof_lon[4]={-1.};
  double deltaTof_tran[4]={-1.};
  int ntofcl[4]={0};
  double tlen;
  int longit[4]={0,1,1,0};
  int tranit[4]={1,0,0,1};
  double dlong,dtran;
  bool goodstatus=false;
  bool goodmatch=false;
  bool goodL=false;
  bool goodlayer[4]={false,false,false,false};
  // match with cluster
  int ilay=0;
  bool good_c = false;
  if(pev->NTofCluster()>0){
    for (int icl=0; icl<pev->NTofCluster(); icl++) {
     TofClusterR*  tof_cl = pev->pTofCluster(icl);
      if (tof_cl>0) { // cluster exists
	good_c=true;
	int layer=tof_cl->Layer-1;
	int bar=tof_cl->Bar-1;
          ntofcl[layer]++;
	// check cluster status
	for (int i = 7; i < 13; ++i) if (((tof_cl->Status >> i) & 1) == 1) good_c = false;
	if (((tof_cl->Status >> 2) & 1) == 1) good_c = false;
	if (((tof_cl->Status >> 4) & 1) == 1) good_c = false;
	if (good_c) goodstatus = true;
	// check track match with TOF clusters
	tlen=ptrk->Interpolate(tof_cl->Coo[2],tofpnt,tofdir,id_inner);
	dlong=tof_cl->Coo[longit[layer]]-tofpnt[longit[layer]];
	dtran=tof_cl->Coo[tranit[layer]]-tofpnt[tranit[layer]];
	if(fabs(dlong)<LONGCUT[layer][bar] &&
	   fabs(dtran)<TRANCUT[layer][bar]) goodmatch=true;
        else goodmatch=false;

	// goodpath
	if(ptofbetah->IsGoodQPathL(layer)) goodL=true;
        else goodL=false;

	if (goodstatus &  goodmatch & goodL ) {
goodlayer[layer]=true;
deltaTof_lon[layer]=fabs(dlong);
deltaTof_tran[layer]=fabs(dtran);
	}
      }     
    }//icluster

  }//Ntofcluster

 double deltaToftmax1=0.;
 double deltaToftmin1=10000.;
 for(unsigned int i=0;i<2;i++) {
   if(deltaTof_tran[i]>0) {
 if(deltaTof_tran[i]>deltaToftmax1) deltaToftmax1=deltaTof_tran[i];
if(deltaTof_tran[i]<deltaToftmin1) deltaToftmin1=deltaTof_tran[i];
   }
 }

double deltaToftmax2=0.;
 double deltaToftmin2=10000.;
 for(unsigned int i=2;i<4;i++) {
   if(deltaTof_tran[i]>0) {
 if(deltaTof_tran[i]>deltaToftmax2) deltaToftmax2=deltaTof_tran[i];
 if(deltaTof_tran[i]<deltaToftmin2) deltaToftmin2=deltaTof_tran[i];
   }
 }
// tof cluster status
  int tofpatt=0;
  if(goodlayer[0]) tofpatt |=1;
  if(goodlayer[1]) tofpatt |=2;
  if(goodlayer[2]) tofpatt |=4;
  if(goodlayer[3]) tofpatt |=8;
  //tofpatt 
 ftree->FillTreePar("tofpatt",tofpatt);
 ftree->FillTreePar("deltaToftmax1",deltaToftmax1); //upper tof
 ftree->FillTreePar("deltaToftmax2",deltaToftmax2);   // lower tof
//number of tof clusters in layers
//2222 -2 clusters in each layer
 double ntofclpatt=1000*ntofcl[3]+100*ntofcl[2]+10*ntofcl[1]+ntofcl[0];
 if(goodlayer[0]) ftree->FillTreePar("ntofclpatt",ntofclpatt); 
  } // trck fit
 }//btof

 // use btrd
 if(btrd) {
ftree->FillTreePar("ntrdrh",pev->NTrdRawHit());
ftree->FillTreePar("ntrdtrk",pev->NTrdTrack());
ftree->FillTreePar("ntrdhtrk",pev->NTrdHTrack()); 
ftree->FillTreePar("ntrdcl",pev->NTrdCluster());
//// TRDK charge
 int iTrackParDefault = ptrk->iTrTrackPar(0,0,0); // Get default-fit code
 if(iTrackParDefault) {
TrdKCluster TRDkcluster = TrdKCluster(pev,ptrk,iTrackParDefault);
 if(    TRDkcluster.IsReadAlignmentOK     // 0: no Alignment   1: Static Alignment on Layers  2: Dynamic Alignment for entire TRD 
        && TRDkcluster.IsReadCalibOK     ) { // 0: no Gain Calibration   1: Gain Calibration Succeeded
 TRDkcluster.CalculateTRDCharge();
double trdkQ=TRDkcluster.GetTRDCharge();
double trdkQe=TRDkcluster.GetTRDChargeError();
ftree->FillTreePar("trdQ",trdkQ);
ftree->FillTreePar("trdQe",trdkQe);
 } // trdkcalib
 }// default


//trdqt lhoods
 if(!trdqt) trdqt=new TrdQt();
 if(trdqt->ProcessEvent(pev, 0, 1, 3, 3)) {
 if(trdqt->UsefulForTrdParticleId()) {
// get the e/p log-likelihood ratio
float trdLep = trdqt->LogLikelihoodRatioElectronProton();
float trdLhep = trdqt->LogLikelihoodRatioHeliumProton () ;
ftree->FillTreePar("trdLep",trdLep);
ftree->FillTreePar("trdLhep",trdLhep);
float atrdtrkx=	trdqt->GetTrdTrkAngle (0); 
float atrdtrky=	trdqt->GetTrdTrkAngle (1); 
ftree->FillTreePar("deltaTrd_ax",atrdtrkx);
ftree->FillTreePar("deltaTrd_ay",atrdtrky);
    }// goodtrd
 }//trdqt
 }

 if(btrk) {
 ftree->FillTreePar("ntrk",pev->NTrTrack ());
 ftree->FillTreePar("ntrkh",pev->NTrRecHit ());
 int JLpatt=0;
 int nReconstructedHits = ptrk->NTrRecHit();
  for (int reconstructedHitIndex=0; reconstructedHitIndex < nReconstructedHits; ++reconstructedHitIndex) {
    TrRecHitR* pTrRecHit = ptrk->pTrRecHit(reconstructedHitIndex);
    if(!pTrRecHit) continue;
  int lay= pTrRecHit->GetLayerJ(); 
   switch (lay){
    case  2: JLpatt |= 1; break;
    case  3: JLpatt |= 2; break;
    case  4: JLpatt |= 2; break;
    case  5: JLpatt |= 4; break;
    case  6: JLpatt |= 4; break;
    case  7: JLpatt |= 8; break;
    case  8: JLpatt |= 8; break;
    case  1:  if(!(pTrRecHit->OnlyX()||pTrRecHit->OnlyY())) JLpatt |= 16; break; // xy in Jlay-1
    case  9: if(!(pTrRecHit->OnlyX()||pTrRecHit->OnlyY())) JLpatt |= 32; break; // xy in Jlay-9
   }
  }//recohit
  // trk patter ala ACsoft
  ftree->FillTreePar("trkpat",JLpatt);

  // tracker charges
double betac = ptofbetah->GetBetaC();
 float trkchargein  = ptrk->GetInnerQ(betac);
 float trkerrorin   = ptrk->GetInnerQ_RMS(betac) / sqrt(max(1.0,(float)ptrk->GetInnerQ_NPoints(betac) - 1.0));
 float trkcharge  = ptrk->GetQ(betac);
 float trkerror   = ptrk->GetQ_RMS(betac) / sqrt(max(1.0,(float)ptrk->GetQ_NPoints(betac) - 1.0));
 // trkQ for inner and total
 ftree->FillTreePar("trkQ",trkchargein);
 ftree->FillTreePar("trkQe",trkerrorin);
 ftree->FillTreePar("trkextQ",trkcharge);
 ftree->FillTreePar("trkextQe",trkerror);

 // get charges lay2,34,56,78
 double qtrkc[9]={-99};
 for (int reconstructedHitIndex=0; reconstructedHitIndex < nReconstructedHits; ++reconstructedHitIndex) {
    TrRecHitR* pTrRecHit = ptrk->pTrRecHit(reconstructedHitIndex);
    if(!pTrRecHit) continue;
   int lay= pTrRecHit->GetLayerJ(); 
  qtrkc[lay-1]= ptrk->GetLayerJQ(lay, betac); 
 }//recohits
 //  layers
 ftree->FillTreePar("trkQ1",qtrkc[0]);//lay1
 ftree->FillTreePar("trkQ2",qtrkc[1]);   //lay2
 ftree->FillTreePar("trkQ9",qtrkc[8]);   //lay9

double qq=0.; 
int nn=0;
 int i1;
 int i2;
 { i1=2; i2=3;
   qq=0; nn=0;
 if(qtrkc[i1]>0) {qq+=qtrkc[i1];nn++; }
 if(qtrkc[i2]>0) {qq+=qtrkc[i2];nn++; }
 if(qq==0) qq=-99;
 if(nn>0)  ftree->FillTreePar("trkQ34",qq/nn);   //lay34
 }
{ i1=4; i2=5;
   qq=0; nn=0;
 if(qtrkc[i1]>0) {qq+=qtrkc[i1];nn++; }
 if(qtrkc[i2]>0) {qq+=qtrkc[i2];nn++; }
 if(qq==0) qq=-99;
 if(nn>0)  ftree->FillTreePar("trkQ56",qq/nn);   //lay34
 }

{ i1=6; i2=7;
   qq=0; nn=0;
 if(qtrkc[i1]>0) {qq+=qtrkc[i1];nn++; }
 if(qtrkc[i2]>0) {qq+=qtrkc[i2];nn++; }
 if(qq==0) qq=-99;
 if(nn>0)  ftree->FillTreePar("trkQ56",qq/nn);   //lay34
 }
{
   qq=0; nn=0;
 if(qtrkc[2]>0) {qq+=qtrkc[2];nn++; }
 if(qtrkc[3]>0) {qq+=qtrkc[3];nn++; }
 if(qq==0) qq=-99;
 if(nn>0)  ftree->FillTreePar("trkQ78",qq/nn);   //lay34
 }

 
// tracker noisy hits ala TkLHH
// search nearest noise hit for each layer
  int trk_layer_pattern[9] = {0,0,0,0,0,0,0,0,0};
  Double_t noise_y[9] = {1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9,1.e9};
  int laypa = 1;
  int btrkpat=ptrk->GetBitPatternJ();
  for (int layer=1; layer<=9; layer++) {
    int bitexam = int(pow(2.,layer-1));
    bitexam = (bitexam &  btrkpat);
    if (bitexam>0)    trk_layer_pattern[layer-1]  = 1;  // flags hitted layers
    AMSPoint pnoise = TrTrackSelection::GetMinDist(ptrk, laypa, 1);
    noise_y[layer-1]=fabs(pnoise.y());
    laypa=laypa*10;
  }//layer

  int nlayer = 0;
  double  minoise1 = 7.;
  double  minoise2 = 7.;
  double  minoise3 = 7.;
  double  minoise4 = 7.;
  int lminoise1 = -1;
  for (int layer=1; layer<=9; layer++) {
    if(trk_layer_pattern[layer-1] ==1) nlayer=nlayer+1.;  // found number hitted layer
    if (noise_y[layer-1]<minoise1 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1)
      {minoise1 = noise_y[layer-1]; lminoise1=layer;}}

  int lminoise2 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<minoise2 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1)
      {minoise2 = noise_y[layer-1]; lminoise2=layer;}}

  int lminoise3 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<minoise3 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2)
      {minoise3 = noise_y[layer-1]; lminoise3=layer;}}

  int lminoise4 = -1;
  for (int layer=1; layer<=9; layer++) {
    if (noise_y[layer-1]<minoise4 && noise_y[layer-1] >0.001 && trk_layer_pattern[layer-1] ==1 && layer != lminoise1 && layer != lminoise2 && layer != lminoise3)
      {minoise4 = noise_y[layer-1]; lminoise4=layer;}}


  // 1,2,3,4 noisy closest hits for all layers
ftree->FillTreePar("trkmnoise1",minoise1);
ftree->FillTreePar("trkmnoise2",minoise2);
ftree->FillTreePar("trkmnoise3",minoise3);
ftree->FillTreePar("trkmnoise4",minoise4);


// track residuals
double diffrig= TrTrackSelection::GetHalfRdiff(ptrk,7); // difference for the full span in upper-bootom rigidities 
ftree->FillTreePar("trkrigdiff",diffrig);
 int ghe= TrTrackSelection::GetPatternForGoodHelium(ptrk,betac);	
ftree->FillTreePar("trkghe",ghe);

 double trkresid[9]={-99};
 double maxres=0.;
for(int il=1;il<10;il++) {
 TrRecHitR * rhit=ptrk->GetHitLJ(il);
 if(!rhit) continue;
trkresid[il-1]=(ptrk->GetResidualJ(il)).norm();
 if(il>2&&il<9) if(trkresid[il-1]>maxres) maxres=trkresid[il-1];
 }
// residuals for 1,9,2 layers and highest for inner tracker
ftree->FillTreePar("TrkHitres1",trkresid[0]);
ftree->FillTreePar("TrkHitres9",trkresid[8]);
ftree->FillTreePar("TrkHitres2",trkresid[1]);
ftree->FillTreePar("TrkHitresmax",maxres); // max residual in inner layers 
// track fits
 int idfit=-1;
idfit=ptrk->iTrTrackPar(1,0,23); // all, PG+CIEMAT  refit
 
if(idfit>=0) {
  double rrig=ptrk->GetRigidity(idfit);
 ftree->FillTreePar("Rig",  rrig);
 ftree->FillTreePar("iRigerr", ptrk->GetErrRinv(idfit));
 ftree->FillTreePar("trkchX",ptrk->GetNormChisqX(idfit));
 ftree->FillTreePar("trkchY",ptrk->GetNormChisqY(idfit));

 AMSPoint trpnt;
 AMSDir trdir;
 ptrk->Interpolate(Z1,trpnt,trdir,idfit);

 }
 idfit=ptrk->iTrTrackPar(21,0,1); // all without MS
 if(idfit>=0) ftree->FillTreePar("iRigerr", ptrk->GetErrRinv(idfit));

 idfit=ptrk->iTrTrackPar(1,3,1); // inner
 if(idfit>=0) ftree->FillTreePar("Rig0",  ptrk->GetRigidity(idfit));
 idfit=ptrk->iTrTrackPar(1,5,1); // inner+L1
 if(idfit>=0) ftree->FillTreePar("Rig1",  ptrk->GetRigidity(idfit));
 idfit=ptrk->iTrTrackPar(1,6,1); // inner+L9
 if(idfit>=0) ftree->FillTreePar("Rig2",  ptrk->GetRigidity(idfit));
 idfit=ptrk->iTrTrackPar(1,7,1); // inner+L9+L1
 if(idfit>=0) ftree->FillTreePar("Rig3",  ptrk->GetRigidity(idfit));
idfit=ptrk->iTrTrackPar(1,1,1); // innerup
if(idfit>=0) ftree->FillTreePar("Rig4",  ptrk->GetRigidity(idfit));
idfit=ptrk->iTrTrackPar(1,2,1); // innerdwn
if(idfit>=0) ftree->FillTreePar("Rig5",  ptrk->GetRigidity(idfit));

// TrkLH
double trklh=TrkLH::gethead()->GetLikelihoodRatioElectronWG(0);
ftree->FillTreePar("trklh", trklh);
 }// btrk


 if(becal) {
 ftree->FillTreePar("necalsh",pev->NEcalShower());
  if(pecal) {
    // check if track in ecal acceptance
  AMSPoint entry_point, exit_point;
  AMSDir   entry_dir,   exit_dir;
  Float_t z_entry = -142.792;
  Float_t z_exit  = -158.457;
int fit_id = ppart->pTrTrack()->iTrTrackPar(1,0,1);
 if(fit_id>=0) {
 ptrk->Interpolate(z_entry, entry_point, entry_dir, fit_id  );
 ptrk->Interpolate(z_exit,  exit_point,  exit_dir,  fit_id  );
  bool Entry_in_32_4 = (TMath::Abs( entry_point.x()) <32.4)  && (TMath::Abs( entry_point.y() )<32.4);  
  bool Exit_in_32_4  = (TMath::Abs( exit_point.x() ) <32.4)  && (TMath::Abs( exit_point.y() ) <32.4);
  bool Entry_in_31_5 = (TMath::Abs( entry_point.x()) <31.5)  && (TMath::Abs( entry_point.y() )<31.5);  
  bool Exit_in_31_5  = (TMath::Abs( exit_point.x() ) <31.5)  && (TMath::Abs( exit_point.y() ) <31.5);

  //Request: Shower axis in ECAL volume (Entry&Exit<32.4), at least Entry||Exit within 1 cell (0,5 Moliere radius) from the border
  bool binacc = (Exit_in_32_4 && Entry_in_32_4) && ( Exit_in_31_5 || Entry_in_31_5 );
  if( binacc) {
    ftree->FillTreePar("ecalE",pecal->EnergyE);
  // GetEcalBDT() does NOT check if the data structures are valid. Do null pointer checks here before calling GetEcalBDT() to make sure it won't crash on broken files.
    bool canComputeBDT = true;
    int n2DCLUSTERs = pecal->NEcal2DCluster();
    for (int i2dcluster = 0; i2dcluster < n2DCLUSTERs; ++i2dcluster) {
      Ecal2DClusterR* pecalcluster2d  = pecal->pEcal2DCluster(i2dcluster);
      if (!pecalcluster2d) {
        canComputeBDT = false;
        break;
      }
      int nCLUSTERs = pecalcluster2d->NEcalCluster();
      for (int icluster = 0; icluster < nCLUSTERs; ++icluster) {
        EcalClusterR* pecalcluster = pecalcluster2d->pEcalCluster(icluster);
        if (!pecalcluster) {
          canComputeBDT = false;
          break;
        }
        int nclHITs = pecalcluster->NEcalHit();
        for (int ihit = 0; ihit < nclHITs; ++ihit) {
          EcalHitR* pecalhit = pecalcluster->pEcalHit(ihit);
          if (!pecalhit) {
            canComputeBDT = false;
            break;
          }
        }
        if (!canComputeBDT)
          break;
      }
      if (!canComputeBDT)
        break;
    }
    if(canComputeBDT) {
       ftree->FillTreePar("ecalBDT",pecal->GetEcalBDT());
      // ftree->FillTreePar("ecalBDT",ecal->EcalStandaloneEstimatorV2());
    }

    AMSPoint ecalp9;  // top of ecal
    AMSPoint ecalp0; // bottom of ecal but after is used for cofg 
    // put in correct way top-bottom (sometime is flipped) 
    Double_t dzn = (pecal->Entry[2]);
    Double_t dzx = (pecal->Exit[2]);
    if (dzn*dzn < dzx*dzx)
      {// standard direction
	ecalp9.setp(pecal->Entry[0], pecal->Entry[1],pecal->Entry[2]);
        ecalp0.setp(pecal->Exit[0], pecal->Exit[1],pecal->Exit[2]);
      } else
      {// contrary direction
        ecalp0.setp(pecal->Entry[0], pecal->Entry[1],pecal->Entry[2]);
        ecalp9.setp(pecal->Exit[0], pecal->Exit[1],pecal->Exit[2]);
       }

    ecalp0.setp(pecal->CofG[0], pecal->CofG[1],pecal->CofG[2]); //cofg 
  AMSDir dird;
  AMSPoint Pec9;
  AMSPoint Pec0;
    ptrk->Interpolate(ecalp0.z(), Pec0, dird, fit_id);
    ptrk->Interpolate(ecalp9.z(), Pec9, dird, fit_id);
    double ecp9x = ecalp9.x();
    double ecp9y = ecalp9.y();
    double ecp9z = ecalp9.z();
    double ecp0x = ecalp0.x();
    double ecp0y = ecalp0.y();
    double ecp0z = ecalp0.z();
    double tecdx = dird.x();
    double tecdy = dird.y();
    double tecdz = dird.z();

   double norm1 = (ecp9x-ecp0x)*(ecp9x-ecp0x)+(ecp9y-ecp0y)*(ecp9y-ecp0y)+(ecp9z-ecp0z)*(ecp9z-ecp0z);
    norm1 = sqrt(norm1);
    double norm2 = sqrt(tecdx*tecdx+tecdy*tecdy+tecdz*tecdz);
    double coset = (ecp9x-ecp0x)*tecdx+(ecp9y-ecp0y)*tecdy+(ecp9z-ecp0z)*tecdz;
    coset = fabs(coset)/(norm1*norm2);
    coset = fabs(1.-coset*coset);    // is sinsquare trk-ecal

    // match trk and ECAL shower
    ftree->FillTreePar("deltaEcal_x",ecalp0.x()-Pec0.x()); //difference at COG level
    ftree->FillTreePar("deltaEcal_y",ecalp0.y()-Pec0.y());
    ftree->FillTreePar("deltaEcal_a",coset); // angular difference
       }// in acceptance
    }//interpolate 
   }// pecal
 }//becal


 if(Debug) cout<<"==fill outree "<<endl;
  return 0;
}


// the parameters are extracted from OutTreeBranches
// here all parameters which can be potentially used in MVA, 
// should be defined by names used in config
double InterMVA::GetMVAOut(AMSEventR* pev) {
 if(Debug) cout<<"==getmva from OutTreeManager "<<fmva<<endl;
  if(!ftree) return 1;
   if(!fmva) return 1;
  
   if(filloutevent(pev)) {if(Debug) cout<<"*failed process event"<<endl; return -99; }

       //fill parameters
     vector<AParam*> vvar=fmva->getparams();
   for(vector<AParam*>::iterator ip = vvar.begin(); ip != vvar.end(); ++ip) {
     if(Debug>2) cout<<" fillvar "<<(*ip)->name<<endl;
    if((*ip)->name=="(tofQl-tofQu)") 
          {
(*ip)->val=ftree->GetTreePar("tofQl")-ftree->GetTreePar("tofQu");
 if(Debug>2)  cout<<(*ip)->name<< " found "<<(*ip)->val<<endl;
}
    if((*ip)->name=="tofQe/tofQ") 
          {(*ip)->val=ftree->GetTreePar("tofQe")/ftree->GetTreePar("tofQ");}
    if((*ip)->name=="trkQe/trkQ") 
          {(*ip)->val=ftree->GetTreePar("trkQe")/ftree->GetTreePar("trkQ");}
    if((*ip)->name=="(trkQ2-trkQ)/trkQ") 
         {(*ip)->val=(ftree->GetTreePar("trkQ2")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}

   if((*ip)->name=="(trkQ2-trkQ)") 
         {(*ip)->val=(ftree->GetTreePar("trkQ2")-ftree->GetTreePar("trkQ"));}
  if((*ip)->name=="(trkQ-trkextQ)") 
         {(*ip)->val=(ftree->GetTreePar("trkQ")-ftree->GetTreePar("trkextQ"));}
     
   if((*ip)->name=="accEdep") 
         {(*ip)->val=ftree->GetTreePar("accEdep");}
   if((*ip)->name=="trkmnoise2") 
         {(*ip)->val=ftree->GetTreePar("trkmnoise2");}
   if((*ip)->name=="trkmnoise1") 
        {(*ip)->val=ftree->GetTreePar("trkmnoise1");}
 if((*ip)->name=="trkmnoise3") 
        {(*ip)->val=ftree->GetTreePar("trkmnoise3");}
 if((*ip)->name=="trkmnoise4") 
        {(*ip)->val=ftree->GetTreePar("trkmnoise4");}
   if((*ip)->name=="trkchY+trkchX") 
       {(*ip)->val=ftree->GetTreePar("trkchY")+ftree->GetTreePar("trkchX");}
   if((*ip)->name=="deltaToftmax1") 
      {(*ip)->val=ftree->GetTreePar("deltaToftmax1");}
   if((*ip)->name=="abs(deltaTrd_ay)") 
    {(*ip)->val=fabs(ftree->GetTreePar("deltaTrd_ay"));}
  if((*ip)->name=="abs(deltaTrd_ay+deltaTrd_ax)") 
    {(*ip)->val=fabs(ftree->GetTreePar("deltaTrd_ay")+ftree->GetTreePar("deltaTrd_ax"));}
   if((*ip)->name=="trkQ") 
    {(*ip)->val=ftree->GetTreePar("trkQ");}
   if((*ip)->name=="tofQ")     {(*ip)->val=ftree->GetTreePar("tofQ");}
   if((*ip)->name=="tofQ1/tofQ") 
          {if(ftree->GetTreePar("tofQ")!=0) (*ip)->val=ftree->GetTreePar("tofQ1")/ftree->GetTreePar("tofQ");}
  if((*ip)->name=="tofQ2/tofQ") 
          {if(ftree->GetTreePar("tofQ")!=0) (*ip)->val=ftree->GetTreePar("tofQ2")/ftree->GetTreePar("tofQ");}
 if((*ip)->name=="tofQ3/tofQ") 
          {if(ftree->GetTreePar("tofQ")!=0) (*ip)->val=ftree->GetTreePar("tofQ3")/ftree->GetTreePar("tofQ");}
 if((*ip)->name=="tofQ4/tofQ") 
          {if(ftree->GetTreePar("tofQ")!=0) (*ip)->val=ftree->GetTreePar("tofQ4")/ftree->GetTreePar("tofQ");}
  if((*ip)->name=="ntrdrh") 
    {(*ip)->val=ftree->GetTreePar("ntrdrh");}
  if((*ip)->name=="ntrdtrk") 
    {(*ip)->val=ftree->GetTreePar("ntrdtrk");}
 if((*ip)->name=="nacccl") 
    {(*ip)->val=ftree->GetTreePar("nacccl");}
 if((*ip)->name=="ntofcl") 
    {(*ip)->val=ftree->GetTreePar("ntofcl");}

if((*ip)->name=="deltaEcal_a") 
    {(*ip)->val=ftree->GetTreePar("deltaEcal_a");}

if((*ip)->name=="(trkQ-tofQ)") 
  {(*ip)->val=ftree->GetTreePar("trkQ")-ftree->GetTreePar("tofQ") ;}

// 
if((*ip)->name=="ntrk") 
    {(*ip)->val=ftree->GetTreePar("ntrk");}
if((*ip)->name=="ntrkh") 
    {(*ip)->val=ftree->GetTreePar("ntrkh");}

if((*ip)->name=="trkchY+trkchX") 
    {(*ip)->val=ftree->GetTreePar("trkchY")+ftree->GetTreePar("trkchX");}


if((*ip)->name=="(trkQ-tofQ)/trkQ") 
  { if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("tofQ")>0) (*ip)->val=(ftree->GetTreePar("trkQ")-ftree->GetTreePar("tofQ"))/ftree->GetTreePar("trkQ");}


if((*ip)->name=="(trkQ1-tofQu)/trkQ1") 
  { if(ftree->GetTreePar("trkQ1")>0) (*ip)->val=(ftree->GetTreePar("trkQ1")-ftree->GetTreePar("tofQu"))/ftree->GetTreePar("trkQ1");}

if((*ip)->name=="(tofQl-tofQu)/tofQ") 
  { if(ftree->GetTreePar("tofQ")>0) (*ip)->val=(ftree->GetTreePar("tofQl")-ftree->GetTreePar("tofQu"))/ftree->GetTreePar("tofQ");}

if((*ip)->name=="trkQe/trkQ") 
  {if(ftree->GetTreePar("trkQ")!=0)  (*ip)->val=ftree->GetTreePar("trkQe")/ftree->GetTreePar("trkQ");}

if((*ip)->name=="ecalE/Rig") 
  { if(ftree->GetTreePar("Rig")!=0&&ftree->GetTreePar("ecalE")>1.0) (*ip)->val=ftree->GetTreePar("ecalE")/ftree->GetTreePar("Rig");}


   if((*ip)->name=="(trkQ1-trkQ)/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ1")>0) (*ip)->val=(ftree->GetTreePar("trkQ1")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}
 
 if((*ip)->name=="trkQ1/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ1")>0) (*ip)->val=ftree->GetTreePar("trkQ1")/ftree->GetTreePar("trkQ");}

 if((*ip)->name=="trkQ2/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ2")>0) (*ip)->val=ftree->GetTreePar("trkQ2")/ftree->GetTreePar("trkQ");}
 
 if((*ip)->name=="trkQ1/trkQ2") 
         {if(ftree->GetTreePar("trkQ1")>0&&ftree->GetTreePar("trkQ2")>0) (*ip)->val=ftree->GetTreePar("trkQ1")/ftree->GetTreePar("trkQ2");}
 

if((*ip)->name=="(trkQ2-trkQ)/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ2")>0) (*ip)->val=(ftree->GetTreePar("trkQ2")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}

 if((*ip)->name=="(trkQ34-trkQ)/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ34")>0) (*ip)->val=(ftree->GetTreePar("trkQ34")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}

 if((*ip)->name=="(trkQ56-trkQ)/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ56")>0) (*ip)->val=(ftree->GetTreePar("trkQ56")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}

  
 if((*ip)->name=="(trkQ78-trkQ)/trkQ") 
         {if(ftree->GetTreePar("trkQ")>0&&ftree->GetTreePar("trkQ78")>0) (*ip)->val=(ftree->GetTreePar("trkQ78")-ftree->GetTreePar("trkQ"))/ftree->GetTreePar("trkQ");}

if((*ip)->name=="(trkQ2-trkQ78)/trkQ") 
         {if(ftree->GetTreePar("trkQ78")>0&&ftree->GetTreePar("trkQ2")>0) (*ip)->val=(ftree->GetTreePar("trkQ2")-ftree->GetTreePar("trkQ78"))/ftree->GetTreePar("trkQ78");}

if((*ip)->name=="necalsh") 
    {(*ip)->val=ftree->GetTreePar("necalsh");}


if((*ip)->name=="trdLhep") 
  {if(ftree->GetTreePar("trdLhep")>0) (*ip)->val=ftree->GetTreePar("trdLhep");}


if((*ip)->name=="Rig*iRigerr") 
    {(*ip)->val=ftree->GetTreePar("Rig")*ftree->GetTreePar("iRigerr");}

if((*ip)->name=="(Rig-Rig0)/Rig") 
  {if(ftree->GetTreePar("Rig")!=0) (*ip)->val=(ftree->GetTreePar("Rig")-ftree->GetTreePar("Rig0"))/ftree->GetTreePar("Rig");  }

if((*ip)->name=="(Rig4-Rig5)/Rig") 
  {if(ftree->GetTreePar("Rig")!=0) (*ip)->val=(ftree->GetTreePar("Rig4")-ftree->GetTreePar("Rig5"))/ftree->GetTreePar("Rig");  }
} //ip
   // ....
 double mvaout=fmva->getclassifier("BDTG");
 if(Debug>2) cout<<" mvaout="<<mvaout<<endl;
 return mvaout;
}






