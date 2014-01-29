//  $Id: ntuple.C,v 1.274 2014/01/29 17:48:08 choutko Exp $
//
//  Jan 2003, A.Klimentov implement MemMonitor from S.Gerassimov
//
//  

#include "cern.h"
#include "commonsi.h"
#include "node.h"
#include "ntuple.h"
#include "job.h"
#include "ecaldbc.h"
#include "event.h"
#include "DynAlignment.h"
extern "C" int ISSLoad(const char *name, const char *line1, const char *line2);
#ifndef _PGTRACK_
#include "tralig.h"
#else
#include "TrCalDB.h"
#include "TrExtAlignDB.h"
#include "TrInnerDzDB.h"
#include "TrParDB.h"
#include "TrPdfDB.h" 
#include "TrGainDB.h"
#include "TrOccDB.h"
#include "TrRecon.h"
#include "TrTasDB.h"
#include "TrTasCluster.h"
#include "TrHistoMan.h"
#endif
#include <signal.h>
#include <iostream>
#include <iomanip>
#ifndef __DARWIN__
#include <malloc.h>
#endif
#include <netdb.h>
#include <time.h>
#include <strstream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef __CORBA__
#include "producer.h"
#endif

#ifdef __WRITEROOT__
#include "TBranch.h"
#include "TH1.h"

#include "TPaletteAxis.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TBox.h"
#include "TPad.h"
#include "TF1.h"
#include "TH2F.h"
#include "TFitter.h"
#include "HistoMan.h"
#ifdef __LZMA__
#include "Compression.h"
#endif
#include"trigger102.h"
AMSEventR AMSNtuple::_evroot02;
AMSSetupR AMSNtuple::_setup02;
EventNtuple02 AMSNtuple::_event02;
TRDInfoNtuple02 AMSNtuple::_trdinfo02;
TTree* AMSNtuple::_tree=0;
TTree* AMSNtuple::_treesetup=0;
TFile* AMSNtuple::_rfile=0;
TObjString AMSNtuple::_dc("");
TObjString AMSNtuple::_ta("");
TObjString AMSNtuple::_ag("");
const int branchSplit=1;

TCanvas* _can=0;
TH2F* tfr=0;
TBox *nubox=0;
TPaletteAxis* palette=0;

TF1* landau=new TF1("landau","landau(0)");
static float mpv_arr[20][18][16];
static float empv_arr[20][18][16];

#endif

const int MEMUPD = 100; // update Memory Monitor Histograms each 
                        // MEMUPD events 

#ifdef _OPENMP
omp_lock_t AMSNtuple::fLock;
#endif
AMSNtuple::~AMSNtuple(){
#ifdef __WRITEROOT__
  //for(int k=0;k<sizeof(_evroot02)/sizeof(_evroot02[0]);k++)if(_evroot02[k])delete _evroot02[k];
 if(_rfile){
   _rfile->Write();
   _rfile->Close();
   delete _rfile;
 }    
#endif
}

AMSNtuple::AMSNtuple(char* name) : AMSNode(AMSID(name,0)) {
  _lun=0;
  evmap.clear();
  //for(int k=0;k<sizeof(_evroot02)/sizeof(_evroot02[0]);k++)_evroot02[k]=new AMSEventR();
//  initR(name);
}

AMSNtuple::AMSNtuple(integer lun, char* name) : AMSNode(AMSID(name,0)) {
  _lun = lun;
  evmap.clear();
  //for(int k=0;k<sizeof(_evroot02)/sizeof(_evroot02[0]);k++)_evroot02[k]=new AMSEventR();
  init();
}

void AMSNtuple::init(){
  _Nentries=0;
  _Lasttime=0;
  _Lastev=0;
  char *a=new char[strlen(getname())+1];
  strcpy(a,getname());
  HBNT(_lun, a," ");
  delete []a;
// -----> Station :
//
/*
  HBNAME(_lun,"EventH",&_event02.Eventno,
 
        
"eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,ISSEqAsc:R,ISSEqDec:R,ISSGalLong:R,ISSGalLat:R,AMSEqAsc:R,AMSeqDec:R,AMSGalLat:R,AMSGalLong:R,BAv:R,TempTracker:R,TrTracks[0,1000]:I,TrRecHits[0,32766]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFRawSides[0,1000]:I,TOFMCClusters[0,10000]:I,AntiMCClusters[0,10000]:I,TRDMCClusters[0,2000]:I,AntiClusters[0,100]:I,AntiRawSides[0,100]:I,EcalClusters[0,500]:I,EcalHits[0,5000]:I," //ISN
"RICMCClusters[0,10000]:I,RICHits[0,10000]:I,TRDRawHits[0,2000]:I,TRDClusters[0,1000]:I,TRDSegments[0,500]:I,TRDTracks[0,255]:I,EventStatus(2):I,TrStat:I"); 

  HBNAME(_lun,"Beta",&_beta02.Nbeta,
      "nbeta[0,20],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge02.Ncharge,
    "ncharge[0,20],chargestatus(ncharge):I,chargebetap(ncharge)[-1,30000]:I,chargeringp(ncharge)[-1,30000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,chargerich(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,probrich(4,ncharge),chinrich(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"TRDMCCl",&_trdclmc.Ntrdclmc,
  "ntrdmccl[0,200],trdlayermc(ntrdmccl)[0,20]:I,trdladdermc(ntrdmccl)[0,40]:I,trdtubemc(ntrdmccl)[0,100]:I,trdpartmc(ntrdmccl)[-1000,1000]:I,trdedepmc(ntrdmccl),trdekinmc(ntrdmccl),trdxglmc(3,ntrdmccl),trdstepmc(ntrdmccl)");

  HBNAME(_lun,"TRDRHT",&_trdht.Ntrdht,
  "ntrdht[0,200],trdhtlayer(ntrdht)[0,19]:I,trdhtladder(ntrdht)[0,17]:I,trdhttube(ntrdht)[0,15]:I,trdampht(ntrdht)");

  HBNAME(_lun,"TRDSEG",&_trdseg.Ntrdseg,
  "ntrdseg[0,100],trdsegstatus(ntrdseg):I,trdsegor(ntrdseg)[0,10]:I,trdsegfpar(2,ntrdseg),trdsegchi2(ntrdseg),trdsegpat(ntrdseg)[0,100]:I,trdsegnh(ntrdseg)[0,12]:I,trdsegpcl(12,ntrdseg)[0,1000]:I");

  HBNAME(_lun,"TRDTRK",&_trdtrk.Ntrdtrk,
  "ntrdt[0,40],trdtstatus(ntrdt):I,trdtcoo(3,ntrdt),trdtercoo(3,ntrdt),trdtphi(ntrdt),trdttheta(ntrdt),trdtchi2(ntrdt),trdtns(ntrdt)[0,5]:I,trdtpat(ntrdt)[0,100]:I,trdtps(5,ntrdt):I");

  HBNAME(_lun,"TRDClu",&_trdcl.Ntrdcl,
  "ntrdcl[0,100],trdclstatus(ntrdcl):I,trdclcoo(3,ntrdcl),trdlayer(ntrdcl)[0,20]:I,trddir(3,ntrdcl),trdmul(ntrdcl)[0,31]:I,trdhmul(ntrdcl)[0,31]:I,trdedep(ntrdcl),ptrdrht(ntrdcl)[0,1000]:I");

  HBNAME(_lun,"Particle",&_part02.Npart,
 
"npart[0,10],pstatus(npart):I,pbetap(npart)[-1,254]:I,pchargep(npart)[-1,254]:I,ptrackp(npart)[-1,254]:I,ptrdp(npart)[-1,254]:I,pvertp(npart)[-1,7]:I,prichp(npart)[-1,254]:I,pecalp(npart)[-1,254]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probgetpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pbeta(npart),perrbeta(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),cutoff(npart),cootof(3,4,npart),cooanti(3,2,npart),cooecal(3,3,npart),cootr(3,8,npart),cootrd(3,npart),coorich(3,2,npart),pathrich(2,npart):R,pathrichb(2,npart):R,lengthrich(npart):R,trdlikelihood(npart):R,local(8,npart):R");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
 
"ntof[0,48],TOFStatus(ntof):I,TOFpln(ntof)[0,4]:I,TOFbar(ntof)[0,12]:I,TOFmem(ntof)[0,3]:I,TOFp2m(3,ntof)[0,48]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//

  HBNAME(_lun,"EcalClus",&_ecclust.Neccl,
 "neccl[0,60],EcclStatus(neccl):I,EcclProj(neccl)[0,1]:I,EcclPlane(neccl)[0,20]:I,EcclLeft(neccl)[0,72]:I,EcclCenter(neccl)[0,72]:I,EcclRight(neccl)[0,72]:I,EcclEdep(neccl),ecclsleak(neccl),eccldleak(neccl),EcclCoo(3,neccl),ecclpleft(neccl):I,ecclnhits(neccl)[0,72]:I");

  HBNAME(_lun,"Ecal2dCl",&_ec2dclust.Nec2dcl,
 "nec2d[0,10],ec2dstatus(nec2d):I,Ec2dProj(nec2d)[0,1]:I,Ec2dNmemb(nec2d):I,Ec2dEdep(nec2d),Ec2dCoo(nec2d),ec2dtan(nec2d),ec2dchi2(nec2d),ec2dpcl(18,nec2d)[0,1000]:I");

  HBNAME(_lun,"EcalShow",&_ecshow.Necsh,
 "necsh[0,5],ecshstatus(necsh):I,Ecshdir(3,necsh),ecshemdir(3,necsh),Ecshentry(3,necsh),Ecshexit(3,necsh),Ecshcofg(3,necsh),EcshErdir(necsh),ecshchi2dir(necsh),ecshfrontedep(necsh),EcshEn(necsh),ecsherat(3,necsh),Ecshener(necsh),ecshdifosum(necsh),EcshsLeak(necsh),EcshrLeak(necsh),EcshdLeak(necsh),EcshaLeak(necsh),EcshnLeak(necsh),EcshoLeak(necsh),ecsho2de(necsh),ecshchi2P(necsh),ecshparP(4,necsh),ecshchi2T(necsh),ecshspev(3,necsh),ecshncl(necsh)[0,7]:I,ecshpcl(2,necsh):I");
//
  HBNAME(_lun,"EcalHits",&_ecalhit.Necht,
 
"necht[0,450],EchtStatus(necht):I,EchtIdsoft(necht):I,EchtProj(necht)[0,1]:I,EchtPlane(necht)[0,20]:I,EchtCell(necht)[0,80]:I,EchtEdep(necht),EchtECor(necht),echtattc(necht),EchtCoo(3,necht),EchtADC(3,necht),echtped(3,necht),echtgain(necht)");
//

  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,100],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");

#ifndef _PGTRACK_
  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");
  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");
  HBNAME(_lun,"TrRecHit",&_trrh02.Ntrrh,
  "ntrrh[0,666],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[1,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh),Bfield(3,ntrrh)");
  HBNAME(_lun,"TrTrack",&_trtr02.Ntrtr,
  "ntrtr[0,100],trstatus(ntrtr):I,pattern(ntrtr)[-1,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2WithoutMS(ntrtr),RigidityWithoutMS(ntrtr),Chi2FastFit(ntrtr),Rigidity(ntrtr),ErrRig(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),grig(ntrtr),gerrrig(ntrtr),hchi2(2,ntrtr),HRigidity(2,ntrtr),HErrRigidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),rigms(ntrtr),pirig(ntrtr)");


  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");

#endif

  HBNAME(_lun,"Vertex",&_vtx02.Nvtx,
  "nvtx[0,3],vtxMomentum(nvtx):R,vtxErrMomentum(nvtx):R,vtxTheta(nvtx):R,vtxPhi(nvtx):R,vtxMass(nvtx):R,vtxCharge(nvtx)[-7,7]:I,vtxStatus(nvtx):I,vtxChi2(nvtx):R,vtxNdof(nvtx):I,vtxNtracks(nvtx)[0,7]:I,vtxPtrTrack(3,nvtx):I,vtxVertex(3,nvtx):R");


  HBNAME(_lun,"MCEventG",&_mcg02.Nmcg,
  "nmcg[0,100],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"MCTrack",&_mct.Nmct,
  "nmct[0,500],radl(nmct),absl(nmct),pos(3,nmct),vname(nmct):I");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
 "nanti[0,8],AntiStatus(nanti):I,AntiSector(nanti)[0,8]:I,AnNtimes(nanti)[0,16]:I,AnNpairs(nanti)[0,8]:I,AnTimes(16,nanti),AnTimesE(16,nanti),AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,100],AMCIdsoft(nantimc):I,AMCXcoo(3,nantimc),AMCtof(nantimc),AMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl302.Nlvl3,
"nlvl3[0,2],LVL3TOF(nlvl3)[-1,255],LVL3TRD(nlvl3)[0,15],LVL3Tr(nlvl3)[0,15],LVL3Main(nlvl3),LVL3Dir(nlvl3)[-1,1],LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pat(2,nlvl3)[-1,250],LVL3Res(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R,LVL3TRDHits(nlvl3)[0,63],LVL3HMult(nlvl3)[0,31],LVL3TRDPar(2,nlvl3):R,LVL3emag(nlvl3)[-1,1],LVL3ECmat(nlvl3)[-1,1],LVL3ECTRKcr(4,nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl102.Nlvl1,

"nlvl1[0,2],LVL1PhysBPatt(nlvl1),LVL1JMembPatt(nlvl1),LVL1Flag1(nlvl1)[-10,20],LVL1Flag2(nlvl1)[-10,20],LVL1TOFPatt1(4,nlvl1),LVL1TOFPatt2(4,nlvl1),LVL1AntiPatt(nlvl1),LVL1ECALflag(nlvl1),LVL1ECALpatt(3,6,nlvl1),LVL1ECtrsum(nlvl1):R,LVL1LiveTime(nlvl1):R,LVL1TrigRates(6,nlvl1):R,LVL1TrigTime(4,nlvl1)");


  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,"ntofraw[0,48],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,4]:I,tofrbar(ntofraw)[0,12]:I,tofradca(2,ntofraw),tofradcd(2,ntofraw),tofradcdr(3,2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");
 
 
 
 
HBNAME(_lun,"TofRawSd",&_tofraws.Ntofraws,"ntofraws[0,70],tofrsswid(ntofraws):I,tofrshwidt(ntofraws):I,tofrshwidq(4,ntofraws):I,tofrsnftdc(ntofraws)[0,8]:I,tofrsftdc(8,ntofraws),tofrsnstdc(ntofraws)[0,16]:I,tofrsstdc(16,ntofraws),tofrsnsumh(ntofraws)[0,16]:I,tofrssumh(16,ntofraws),tofrsnsumsh(ntofraws)[0,16]:I,tofrssumsh(16,ntofraws),tofrsadca(ntofraws),tofrsnadcd(ntofraws)[0,3]:I,tofrsadcd(3,ntofraws),tofrstempT(ntofraws),tofrstempC(ntofraws),tofrstempP(ntofraws)");
 
HBNAME(_lun,"AntiRawSd",&_antirs.Nantirs,"naccraws[0,16],accrsswid(naccraws):I,accrssta(naccraws):I,accrstempT(naccraws),accrsadca(naccraws),accrsnftdc(naccraws)[0,8]:I,accrsftdc(8,naccraws),accrsntdct(naccraws)[0,16]:I,accrstdct(16,ntofraws)");
 

  HBNAME(_lun,"RICMCCl",&_richmc.NMC,
  	 "nsignals[0,250],sid(nsignals):I,"
  	 "rimcorg(3,nsignals),rimcdir(3,nsignals),rimcstatus(nsignals):I,"
         "rimcnphg:I,rimcphit(nsignals):I");

  HBNAME(_lun,"RICEvent",&_richevent.Nhits,
    	 "Rhits[0,100]:I,Rchtch(Rhits):I,Rchtadc(Rhits):I,"
  	 "Rnpe(Rhits):R,Rchtx(Rhits),Rchty(Rhits),Rhstatus(Rhits):I");

  HBNAME(_lun,"Ring",&_ring.NRings,
  	 "nrings[0,10]:I,rctrkn(nrings):I,"
  	 "rcrihu(nrings):I,rcrimhu(nrings):I,"
	 "rcribeta(nrings),rcriebeta(nrings),"
	 "rcrichi2(nrings),rcristatus(nrings):I,"
	 "rcriprob(nrings):R,rcrinpexp(nrings):R,rcricnpe(nrings):R,"
         "rcriliphu(nrings):I,rcrilipthc(nrings):R,rcrilipbeta(nrings):R,"
         "rcrilipebeta(nrings):R,"
         "rcriliplikep(nrings):R,rcrilipchi2(nrings):R,rcriliprprob(nrings):R");
*/  
}
void AMSNtuple::reset(int full){
#ifdef __WRITEROOT__
   Get_evroot02()->clear();
   Get_evroot02()->SetCont();
#endif
/*
if(_lun){
   _beta02.Nbeta= 0;
   _charge02.Ncharge = 0;
   _part02.Npart = 0;
   _tof.Ntof = 0;
   _ecclust.Neccl = 0;
   _ec2dclust.Nec2dcl = 0;
   _ecshow.Necsh = 0;
   _ecalhit.Necht = 0;
   _tofmc.Ntofmc = 0;
#ifndef _PGTRACK_
   _trcl.Ntrcl = 0;
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc))/sizeof(integer));
   _trrh02.Ntrrh = 0;
   _trtr02.Ntrtr = 0;
   _trraw.Ntrraw = 0;
#endif
   _trdclmc.Ntrdclmc=0;
   _trdcl.Ntrdcl=0;
   _trdht.Ntrdht=0;
   _trdtrk.Ntrdtrk=0;
   _trdseg.Ntrdseg=0;
   _mcg02.Nmcg = 0;
   _mct.Nmct = 0;   
   _anti.Nanti = 0;
   _antirs.Nantirs = 0;
   _antimc.Nantimc = 0;
   _lvl302.Nlvl3 = 0;
   _lvl102.Nlvl1 = 0;
   _tofraw.Ntofraw = 0;
   _tofraws.Ntofraws = 0;
   _richmc.NMC=0;
   _richevent.Nhits=0;
   _ring.NRings=0;
   _vtx02.Nvtx=0;
}
*/
}

void AMSNtuple::write(integer addentry){
  if(_lun){
     HFNT(_lun);
     _Nentries++;
  }
}

void AMSNtuple::endR(bool cachewrite){
#ifdef _OPENMP
  omp_destroy_lock(&fLock);
#endif
  if(_rfile && evmap.size()  ){
    cout<<"AMSNtuple::endR-I-WritingCache "<<evmap.size()<<" entries "<<cachewrite<<endl;
    for(evmapi i=evmap.begin();i!=evmap.end();i++){
      if(_tree){
	AMSEventR::Head()=i->second;
        if(Get_setup02() && Get_setup02()->UpdateHeader(AMSEventR::Head())!=2){
  	 if(!_lun )_Nentries++;
        _Lastev=i->second->Event();
        _Lasttime=i->second->UTime();
	if(cachewrite)_tree->Fill();
      }
      }
      delete i->second;
    }
    evmap.clear();
    cout<<"AMSNtupe::endR-I-MapErased "<<evmap.size()<<endl;
  }
#ifdef __WRITEROOT__
  // write tracker alignment structure
  
  if(_rfile){

    writeRSetup();
_rfile->cd();
if(AMSEventR::h1(AMSmceventg::_hid)){
   AMSEventR::h1(AMSmceventg::_hid)->Write();
}
#ifndef _PGTRACK_
    _ta.SetString(AMSTrAligFit::GetAligString());
    //cout <<AMSTrAligFit::GetAligString()<<endl;
    _ta.Write("TrackerAlignment");
#else
    TrCalDB::Head->Write();
    TkDBc  ::Head->Write();
    TrParDB::Head->Write();
    // TrPdfDB::GetHead()->Write(); // don't save it, crete it every time
    if (!TrGainDB::IsNull()) TrGainDB::GetHead()->Write();
    if ((!TrOccDB::IsNull())&&(TrOccDB::GetHead()->GetRun()>0)) TrOccDB::GetHead()->Write();
    TrExtAlignDB::GetHead()->Write();
    TrInnerDzDB::GetHead()->Write();
    if (TrTasDB::Head) TrTasDB::Head->Write();
    if (TrTasClusterR::HistDir) TrTasClusterR::HistDir->Write();
    if (IOPA.histoman%10==1 || IOPA.histoman%10==3) hman.Save(_rfile);
    if (IOPA.histoman%10==2 || IOPA.histoman%10==3) hman.Save();
    // if (AMSJob::gethead()->isMonitoring() && ptrman!=0) ptrman->Write(); // already done by _rfile->Write();
    if (AMSJob::gethead()->isMonitoring() && ptrman!=0) ptrman->GetHeader()->AddRunNumber(getrun());

    _rfile->cd();
    TrRecon::RecPar.Write();
/*
    TDirectory* dd=_rfile->mkdir("datacards");
    dd->cd();
    TKGEOMFFKEY.Write();
    TRMCFFKEY.Write();
    TRCALIB.Write();
    TRALIG.Write();
    TRCLFFKEY.Write();
    TRFITFFKEY.Write();
    TRCHAFFKEY.Write();
    TRDMCFFKEY.Write();
    _rfile->cd();
*/
#endif 
    if(TRDFITFFKEY.FitMethod>0&&TRDFITFFKEY.SaveHistos>0){     
      TRDPlotInit();
      
      _can=TRDPlot(1);
      _can->Write("c_occ");
      
      TRDFitMOP();
      _can=TRDPlot(2);
      _can->Write("c_mop");
      
      _can=TRDPlot(3);
      _can->Write("c_emop");
      
      
      if(TRDFITFFKEY.SaveHistos==1){
	for(int i=0;i!=20;i++) for(int j=0;j!=18;j++) for(int k=0;k!=16;k++){
	  int hid=41000+i*290+j*16+k;
	  // get hit amplitude histogram
	  TH1D* h=AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid);
	  if(h)AMSJob::gethead()->getntuple()->Get_evroot02()->hdelete(hid);
	}
      }
      
      if(tfr)delete tfr;
    }
   
    _rfile->Write();

#ifdef _PGTRACK_
     // deleting the tracker online monitor
     if (AMSJob::gethead()->isMonitoring() && ptrman!=0) {
       delete ptrman;
       ptrman = 0;
     }
#endif

    _rfile->Close();
    delete _rfile;
  }
  _rfile=0;

#endif
}



void AMSNtuple::initR(const char* fname,uinteger run,bool update){
#ifdef __WRITEROOT__
  TTree::SetMaxTreeSize(0xFFFFFFFFFFLL);
  static TROOT _troot("S","S");
#ifdef _OPENMP
  omp_init_lock(&fLock);
#endif
  cout << "Initializing tree...\n"<<endl;
  _Nentries=0;
  _Lasttime=0;
  _Lastev=0;
  if(_rfile){
    _rfile->Write();
    _rfile->Close();
    delete _rfile;
    _rfile=0;
  }
  struct stat64 f_stat;
//    char cmd[1024]="rm -f ";
//    strcat(cmd,fname);
//    system(cmd);
    _rfile=new TFile(fname,update?"UPDATE":"RECREATE");
    //if(!update)return;
#ifdef __CORBA__
  _dc.SetString(AMSProducer::GetDataCards());
  //   cout <<_dc.GetString()<<endl;
#endif
  if(!_rfile || _rfile->IsZombie()){
    if(_rfile){
      delete _rfile;
      _rfile=0;
    }
    throw amsglobalerror("UnableToOpenRootFile",3);
  }
  _dc.Write("DataCards");
    TDirectory* dd=_rfile->mkdir("datacards");
    dd->cd();
#ifdef _PGTRACK_
    TKGEOMFFKEY.Write();
    TRMCFFKEY.Write();
    TRCALIB.Write();
    TRALIG.Write();
    TRCLFFKEY.Write();
    TRFITFFKEY.Write();
    TRCHAFFKEY.Write();
    TRDMCFFKEY.Write();
    _rfile->cd();
#endif


  const int size=5000000;
  char * name=new char[size];
  if(name){
    ostrstream ost(name,size);
    AMSJob::gethead()->getgeom()->printN(ost);
    _ag.SetString(name);
    //     cout <<_ag.GetString();
    delete [] name;
  }
  _ag.Write("AMS02Geometry");
  int clevel=IOPA.WriteRoot>0?IOPA.WriteRoot-1:-IOPA.WriteRoot;
  bool lzma=IOPA.WriteRoot<0;
  cout<<"Set Compress Level ..."<<clevel<<endl;
  cout<<"Set Split Level ..."<<branchSplit<<endl;
  _rfile->SetCompressionLevel(clevel);
#ifdef __LZMA__
  if(lzma)_rfile->SetCompressionAlgorithm(ROOT::kLZMA);
  else _rfile->SetCompressionAlgorithm(ROOT::kZLIB);
  cout<<"Set Compress Alg LZMA ..."<<lzma<<endl;
#else
  cerr<<"AMSNtuple::InitR-W-UnableToSet Compress Alg LZMA ..."<<lzma<<endl;
#endif

  cout<<"AMSNtuple::initR -I- create branches"<<endl;
  _tree= new TTree("AMSRoot","AMS Ntuple Root");
  _treesetup= new TTree("AMSRootSetup","AMS Setup Root");
  Get_setup02()->CreateBranch(_treesetup,branchSplit);
  Get_evroot02()->CreateBranch(_tree,branchSplit);
  //    static void *pev1=(void*)_evroot02;
  //   TBranch *b1=_tree->Branch(AMSEventR::BranchName(),"AMSEventR",&pev1,64000,branchSplit);
  //    AString  bhead=AMSEventR::_Name;
  //    bhead+="Header";
  //     static void *pev2=(void*)_evroot02.fHeader;
  //   TBranch *b2=_tree->Branch((const char*)bhead,"AMSEventHeaderR",&pev2,64000,1); 
   Get_setup02()->UpdateVersion(run,AMSCommonsI::getosno(),AMSCommonsI::getbuildno(),AMSCommonsI::getbuildtime());
#endif
#ifndef __WRITEROOT__
  cerr <<" RootFileOutput is Not supported in this version "<<endl;
  exit(1);
#endif

#ifdef _PGTRACK_
   if (AMSJob::gethead()->isMonitoring() && ptrman==0) { 
     ptrman = new TrOnlineMonitor(AMSNtuple::_rfile,2,200);
     ptrman->Book();
     ptrman->GetHeader()->AddFileName(fname);
   }
#endif

#ifdef __MEMMONITOR__
  int NEVENTS = GCFLAG.NEVENT;
  MemMonitor(MEMUPD, NEVENTS);
#endif
}


uinteger AMSNtuple::writeR(){
#ifdef __WRITEROOT__

  if (ATCAFFKEY.antiPG>1){
    Get_evroot02()->RebuildAntiClusters();
}
if(Trigger2LVL1::SetupIsChanged){
  #pragma omp critical (g4)
Get_setup02()->fLVL1Setup.insert(make_pair(AMSEvent::gethead()->gettime(),Trigger2LVL1::l1trigconf));

}
if(Trigger2LVL1::ScalerIsChanged){
  #pragma omp critical (g4)
{
Get_setup02()->fScalers.insert(make_pair(AMSEvent::gethead()->getutime(),Trigger2LVL1::scalmon));
//for(AMSSetupR::Scalers_i k=Get_setup02()->fScalers.begin();k!=Get_setup02()->fScalers.end();k++){
//cout <<Get_setup02()->fScalers.size()<<" blia ... "<<((k->first)>>32)<<" "<<k->second._LiveTime[0]<<endl;
//}
}
}
    AMSEventR * evn=0;
    int nthr=1;
    static int mode_yeld=false; 
#ifdef _OPENMP
    nthr=omp_get_num_threads();
#endif
    int size_break=IOPA.MaxEventsMap*nthr;
    uint64 runv=AMSEvent::gethead()->getrunev();
  #pragma omp critical (wr1)
  {
    Get_evroot02()->SetCont();
    if(evmap.find(runv)!=evmap.end()){
      cerr<<"AMSEventR::writeR-E-dupliucatedEventInMapFound "<<AMSEvent::gethead()->getrun()<<" "<<AMSEvent::gethead()->getid()<<endl;
    }
    else{
     evn=new AMSEventR(_evroot02);    
    evmap.insert(make_pair(runv,evn));
    
    
#ifdef _PGTRACK_
    // once the AMSEventR is created I fill the monitoring infos
    if (AMSJob::gethead()->isMonitoring() && ptrman!=0) ptrman->Fill(evn); 
#endif
    }


    if(evmap.size()>2*size_break)mode_yeld=true;
    else if(evmap.size()<size_break/2)mode_yeld=false;
}
  



#ifdef _OPENMP
     int maxk=evmap.size()*100;
     if(maxk<50000)maxk=50000;
     for(int k=0;k<maxk;k++){
 if(!omp_test_lock(&fLock)){
   if(!mode_yeld)return _Lastev;
   else usleep(2);
 }
 else goto ok;
     }
 return _Lastev;
 ok:
 //cout << " lock test passed"<<AMSEvent::get_thread_num()<<" "<<_Lastev<<endl;
#endif
  vector<AMSEventR*> del;
#pragma omp critical (wr1)
  {
   int nthr=1;
#ifdef _OPENMP
    nthr=omp_get_num_threads();
#endif
      static int _Size=0;
      if(evmap.size()>_Size ){
      long long ssize=0;
      for(evmapi i=evmap.begin();i!=evmap.end();i++){
	ssize+=i->second->Size();
      }  
      float maxsize=1000.*AMSEvent::get_num_threads()/4.;
      if(maxsize<1000)maxsize=1000;
      if(ssize/1024/1024>maxsize){
      if(!mode_yeld)cerr <<"AMSNtuple::writeR-W-OutputMapSizeTooBigClosingFile "<<AMSEvent::gethead()->get_thread_num()<<" "<<_Size<<" "<<ssize/1024/1024<<" Mb "<<endl;
      mode_yeld=true;
      }
      if(evmap.size()-_Size>10)cout <<"AMSNtuple::writeR-I-Output Map Size Reached "<<evmap.size()<<" "<<ssize/1024/1024<<" Mb "<<endl;
      _Size=evmap.size();
    }

    for(evmapi i=evmap.begin();i!=evmap.end();){
      bool go=true;
      //     cout <<"  go "<<i->second->Event()<<endl;
      for(int k=0;k<nthr;k++){
	if(AMSEvent::runev(k) && AMSEvent::runev(k)<(i->first)){
	  go=false;
	  break;
	}
      }
      if(!go)break;
      else {
	for(int k=0;k<nthr;k++){
	  if(AMSEvent::runev(k)==(i->first)){
	    AMSEvent::runev(k)=0;
//	    break;
	  }
	}
	del.push_back(i->second); 
	evmapi idel=i++;
	evmap.erase(idel);
        if(del.size()>size_break)break;
      }
    }
   }
  if(del.size()){
    //cout << " lock write "<<AMSEvent::get_thread_num()<<" "<<_Lastev<<" "<<del.size()<<endl;
    //#pragma omp critical (wr2)
    for(int k=0;k<del.size();k++){
      if(_tree){
	AMSEventR::Head()=del[k];
        if(Get_setup02()->UpdateHeader(AMSEventR::Head())!=2){
	if(!_lun )_Nentries++;
        _Lastev=del[k]->Event();
        _Lasttime=del[k]->UTime();
	_tree->Fill();
        }
      }
    }
    //cout << " lock writed "<<AMSEvent::get_thread_num()<<" "<<_Lastev<<" "<<del.size()<<endl;
    //   #pragma omp critical (wr1)
    {
      if(AMSCommonsI::AB_catch<0){
	AMSCommonsI::AB_catch=0;
	sigsetjmp(AMSCommonsI::AB_buf,0);
	cout <<"  AMSNtuple:writeR-I-sigsetjmp set "<<AMSEvent::get_thread_num()<<endl;
      }
      if(AMSCommonsI::AB_catch!=1){
        //cout << " lock deleting "<<AMSEvent::get_thread_num()<<" "<<_Lastev<<" "<<del.size()<<endl;
	for(int k=0;k<del.size();k++)delete del[k];
      }
      else{
	cout<<"  AMSNtuple::writeR-I-AbortCatched "<<endl;
      }
    }
  }
  else mode_yeld=false;
  
#ifdef _OPENMP
    omp_unset_lock(&fLock);
    //    cout << " lock unset "<<AMSEvent::get_thread_num()<<endl;
#endif
#endif
  return _Lastev;
}


uinteger AMSNtuple::getrun(){
  return _event02.Run;
}

// Author S.Gerassimov TUM/COMPASS (sergei.gerassimov@cern.ch)
// adapted Jan 21, 2003. ak.
//
// Fill memory consumption histograms
// every n-th call up to N
// Example:
// if to call every event MemMonitor(100, 100000);
// at the end you will get histogram with memory consumption evolution
// 'till event # 100000, sampled every 100 events

void AMSNtuple::MemMonitor(const int n, int N = 0)
{
#if !defined(sun) & !defined(__DARWIN__)

  static int nevt    =0;
  static int nperiod =0;
  static int nmaximum=0;

  struct mallinfo m;

#ifdef __WRITEROOT__
  static TH1D* mm [5];
#endif

  if(nevt == 0) {      // booking
    nperiod = n;
    nmaximum= N;
    int nbins = N/n;
    cout<<"PaUtils::MemMonitor ==> Book histograms "<<endl;
    // Jan 21. 03 explanation for pedestrians (a.k.)
    // arena    - This is the total size of memory allocated with `sbrk'
    // hblkhd   - This is the total size of memory allocated with `mmap'
    // uordblks - This is the total size of memory occupied by chunks 
    // fordblks - This is the total size of memory occupied by free chanks
    // keepcost - This is the size of the top-most releasable chunk that
    //            normally borders the end of the heap (i.e. the high end of
    //            the virtual address space's data segment).

    // 
#ifdef __WRITEROOT__
    mm[0]=new TH1D( "MemMon_00","'malloc'      memory VS event number", nbins,  0, N);
    mm[1]=new TH1D( "MemMon_01","'mmap'        memory VS event number", nbins,  0, N);
    mm[2]=new TH1D( "MemMon_02","'used chunks' memory VS event number", nbins,  0, N);
    mm[3]=new TH1D( "MemMon_03","'free chunks' memory VS event number", nbins,  0, N);
    mm[4]=new TH1D( "MemMon_04","'keepcost'    memory VS event number", nbins,  0, N);
#else
    char cdir[256];
    HCDIR(cdir,"R");
    HMDIR("//PAWC/MEMMON","S");

    HBOOK1(9795,"'malloc'      memory VS event number",nbins,0,N,0);
    HBOOK1(9796,"'mmap'      memory VS event number",nbins,0,N,0);
    HBOOK1(9797,"'used chunks'      memory VS event number",nbins,0,N,0);
    HBOOK1(9798,"'free chunks'      memory VS event number",nbins,0,N,0);
    HBOOK1(9799,"'keepcost'      memory VS event number",nbins,0,N,0);

    char hp[9]="//PAWC";
    HCDIR(hp," ");
    HCDIR (cdir, " ");

#endif
  }
  if(++nevt >= nmaximum) return; // out of histogramm limit

  if(nevt%nperiod == 0) {     // filling
    m=mallinfo();
#ifdef __WRITEROOT__
    mm[0]->Fill(nevt, double(m.arena   /1048576.));
    mm[1]->Fill(nevt, double(m.hblkhd  /1048576.));
    mm[2]->Fill(nevt, double(m.uordblks/1048576.));
    mm[3]->Fill(nevt, double(m.fordblks/1048576.));
    mm[4]->Fill(nevt, double(m.keepcost/1048576.));
#else
    char cdir[256];
    char *BLANK=" ";
    char memdir[256]="//PAWC/MEMMON";
    HCDIR(cdir,"R");
    HCDIR(memdir,BLANK);

    HF1(9795,float(nevt), float(m.arena   /1048576.));   
    HF1(9796,float(nevt), float(m.hblkhd  /1048576.));
    HF1(9797,float(nevt), float(m.uordblks/1048576.));
    HF1(9798,float(nevt), float(m.fordblks/1048576.));
    HF1(9797,float(nevt), float(m.keepcost/1048576.));

    char hp[9]="//PAWC";
    HCDIR(hp," ");
    HCDIR (cdir, " ");
#endif
  }
#endif
  return;
}

void AMSNtuple::TRDFitMOP(){
  // get pointer to TRDInfoNtuple02
  TRDInfoNtuple02 *info=AMSJob::gethead()->getntuple()->Get_trdinfo02();
  if(!info)return;

  TH1D* h=NULL;
  TF1* myfit=NULL;

  // loop over tubes (i=layer j=ladder k=tube)
  for(int i=0;i!=20;i++) for(int j=0;j!=18;j++) for(int k=0;k!=16;k++){
    // reset arrays
    mpv_arr[i][j][k]=0.;
    empv_arr[i][j][k]=0.;

    // skip tubes with less than 100 hits
    int hits=info->hit_arr[i][j][k];
    if(hits<100)continue;
    int hid=41000+i*290+j*16+k;

    // generate and fill hit amplitude histogram
    h=AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid);

    // do fit and get pointer to fit function
    TFitter* fitter=new TFitter();
    fitter->SetObjectFit(h);

    landau->SetParameters(sqrt(hits),h->GetMean(),h->GetRMS());
    h->Fit("landau","LLVEIM");
    myfit=h->GetFunction("landau");

    // fill array with mpv and error on mpv for resp. tube
    mpv_arr[i][j][k]=(float)myfit->GetParameter(1);
    empv_arr[i][j][k]=(float)myfit->GetParError(1);

    h->Reset();


  }
  if(h)delete h;
}

void AMSNtuple::TRDPlotInit(){
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
}
// generate TRD plots (1-occupancy 2-MPV 3-ErrorOnMPV
TCanvas* AMSNtuple::TRDPlot(int mode){
  if(mode==0)return NULL;

#ifdef __WRITEROOTNODRWAINGINGBATCHVC__
  TRDInfoNtuple02 *info=AMSJob::gethead()->getntuple()->Get_trdinfo02();
  if(!info)return NULL;

  if(!tfr){
    tfr=new TH2F("tfr",";ladder;layer",20,-10,10,20,0,20);
    tfr->GetXaxis()->SetLabelSize(0.05);
    tfr->GetYaxis()->SetLabelSize(0.05);
    int lad=-9;
    for(int i=2;i!=21;i++){
      char tit[4];
      if(lad==0)lad++;
      sprintf(tit,"%i",lad);
      tfr->GetXaxis()->SetBinLabel(i,tit);
      lad++;
    }
    tfr->GetXaxis()->SetBinLabel(1,"");
    tfr->GetXaxis()->SetBinLabel(20,"");

    lad=0;
    for(int i=1;i!=21;i++){
      char tit[4];
      sprintf(tit,"%i",lad);
      tfr->GetYaxis()->SetBinLabel(i,tit);
      lad++;
    }

    tfr->SetMinimum(0.);
    tfr->SetContour(50);
  }

  _rfile->cd();

  TCanvas* cnew=new TCanvas();
  cnew->SetFillColor(0);
  cnew->cd();

  float max=0.;
  for(int i=0;i!=20;i++) for(int j=0;j!=18;j++) for(int k=0;k!=16;k++){
    if(mode==1  &&  info->hit_arr[i][j][k]>max)max=info->hit_arr[i][j][k];
    if(mode==2 &&  mpv_arr[i][j][k]>max)max=mpv_arr[i][j][k];
    if(mode==3 &&  empv_arr[i][j][k]>max)max=empv_arr[i][j][k];
  }

  tfr->SetMaximum(max);

  TPad* pad=(TPad*)gPad;
  pad->SetRightMargin(0.16);

  int i=0;
  tfr->Draw("col*z");

  if(!palette)palette= new TPaletteAxis(11.1,-1.,12.1,20,tfr);
  if(mode==2){
    if(max>200.)max=200.;
    palette->GetAxis()->SetTitle("mop [ADC]");
  }

  if(mode==1) palette->GetAxis()->SetTitle("occupancy [hits]");

  if(mode==3) palette->GetAxis()->SetTitle("MOP rel error");
  tfr->GetListOfFunctions()->Add(palette,"br");

  gPad->Update();
  for(int lay=0;lay!=20;lay++){
    for(int j=0;j!=18;j++){
      for(int tub=0;tub!=16;tub++){
        if(mode==1&&info->hit_arr[lay][j][tub]<=0.)continue;
        if(mode==2&&mpv_arr[lay][j][tub]<=0.)continue;
        if(mode==3&&empv_arr[lay][j][tub]<=0.)continue;

        float y1=lay-0.3;

        int lad=j-9;
        if(lay<4){
          if(j>13)continue;
          lad++;}
        if(lay<12){
          if(j>15)continue;
          lad++;}
        if(lad>-1)lad++;

        float x1=lad+0.4;
        if(lad>0)x1-=1;
        y1+=0.5;


        x1+=((tub%8)*0.6/7-0.3);
        y1+=(tub>7)*0.3;

        float x2=x1+0.6 / 7;
        float y2=y1+0.3;


        int color=0;
        if(mode==1)color=(int)((float)info->hit_arr[lay][j][tub]/(float)max*49.);
        if(mode==2)color=(int)((float)mpv_arr[lay][j][tub]/(float)max*49.);
        if(mode==3)color=(int)((float)empv_arr[lay][j][tub]/(float)max*49.);


        nubox=new TBox(x1,y1,x2,y2);
        nubox->SetFillColor(gStyle->GetColorPalette(color));
        nubox->Draw("lsamelogz");

      }
    }
  }

  return (TCanvas*)cnew;

#endif
return NULL;
}

uinteger AMSNtuple::writeRSetup(){
if( _treesetup ){
if(AMSJob::gethead() && AMSJob::gethead()->isSimulation() && Get_setup02())Get_setup02()->ResetMC();
   try{
#ifdef __G4AMS__
        UPool.ReleaseLastResort();
        OPool.ReleaseLastResort();
#endif
	_treesetup->Fill();
}
    catch(std::bad_alloc a){
      cerr<<" AMSNtuple::writeRSetup-E-BadALLOC" <<endl;
    }

	   Get_setup02()->Reset();
	}
	return _treesetup!=NULL;
	}


	bool AMSNtuple::LoadISS(time_t xtime){
	static unsigned int time=0;
	unsigned int tl=0;
	#pragma omp threadprivate(time)
	if(Get_setup02() && Get_setup02()->fISSData.size()>0){
	for(AMSSetupR::ISSData_i i=Get_setup02()->fISSData.begin();i!=Get_setup02()->fISSData.end();i++){
	 if(fabs(i->first-xtime)<fabs(tl-xtime))tl=i->first;
	}
	if(tl!=time && tl){
	time=tl;
	AMSSetupR::ISSData_i i=Get_setup02()->fISSData.find(time);
	if(i!=Get_setup02()->fISSData.end() && ISSLoad((const char *)i->second.Name,(const char *)i->second.TL1,(const char*)i->second.TL2)){
	cout << "LOAD!!!! "<<i->second.Name<<endl;
	return true;
	}
	else{
	static int print=0;
	time=0;
	if(print++<100)cerr<<"AMSNtuple::LoadISS-E-UnableToLoad "<<i->second.Name<<" "<<i->first<<endl;
	return false ;
	}
	}
	return time!=0;

	}
	else{
	static int print=0;
	if(print++<100)cerr<<"AMSEvent::LoadISs-E-noSetupFoundorISSDataEmpty "<<Get_setup02()<<endl;
	return false;
	}
	}



	int AMSNtuple::ISSAtt(float &roll, float&pitch, float &yaw,double xtime){
	if(!Get_setup02())return 2;
	else {
	 return Get_setup02()->getISSAtt(roll,pitch,yaw,xtime);
	}
	}   
	 




	int AMSNtuple::ISSSA(float &alpha, float&b1a, float &b3a, float &b1b, float &b3b, double xtime){
	const double rad=3.14159267/180.;
	if(!Get_setup02())return 2;
	else{
	AMSSetupR::ISSSA a;
	int ret=Get_setup02()->getISSSA(a,xtime);
	alpha=a.alpha*rad;
	b1a=a.b1a*rad;
	b3a=a.b3a*rad;
	b1b=a.b1b*rad;
	b3b=a.b3b*rad;
	return ret;
	}
	} 

	int AMSNtuple::ISSCTRS(float &r, float&theta, float &phi, float &v, float &vtheta, float &vphi,double xtime){
	if(!Get_setup02())return 2;
	else{
	AMSSetupR::ISSCTRSR a;
	int ret=Get_setup02()->getISSCTRS(a,xtime);
	r=a.r;
	theta=a.theta;
	phi=a.phi;
	v=a.v;
	vphi=a.vphi;
	vtheta=a.vtheta;
	return ret;
	}
	} 

	int AMSNtuple::GPSWGS84(float &r, float&theta, float &phi, float &v, float &vtheta, float &vphi,double xtime){
	if(!Get_setup02())return 2;
	else{
	AMSSetupR::GPSWGS84R a;
	int ret=Get_setup02()->getGPSWGS84(a,xtime);
	r=a.r;
	theta=a.theta;
	phi=a.phi;
	v=a.v;
	vphi=a.vphi;
	vtheta=a.vtheta;
	return ret;
	}
	} 


	int AMSNtuple::ISSGTOD(float &r, float&theta, float &phi, float &v, float &vtheta, float &vphi,double xtime){
	if(!Get_setup02())return 2;
	else{
	AMSSetupR::ISSGTOD a;
	int ret=Get_setup02()->getISSGTOD(a,xtime);
	r=a.r;
	theta=a.theta;
	phi=a.phi;
	v=a.v;
	vphi=a.vphi;
	vtheta=a.vtheta;
	return ret;
	}
	} 




	void AMSNtuple::readRSetup(AMSEvent *ev){
	if(!_rfile)return;
	int tmout=0;
	string name=_rfile->GetName();
	//_rfile->Write();
	//_rfile->Close();
	if(Get_setup02() && AMSJob::gethead()->isRealData()){
	#ifdef __CORBA__
	AMSProducer::gethead()->SendTimeout(2500);
	#endif
	if(!Get_setup02()->FillHeader(ev?ev->getrun():0)){
cerr<<"AMSNtuple::readRSetup-E-UnableToFillRootSetupHeader "<<endl;
}
else{
if (IOPA.BuildRichConfig>0 && !Get_setup02()->BuildRichConfig(ev?ev->getrun():0))
  cerr<<"AMSNtuple::readRSetup-E-UnableToBuildRichConfig "<<endl;
string slc;
if(!ev ||  !Get_setup02()->FillSlowcontrolDB(slc)){
cerr<<"AMSNtuple::readRSetup-E-UnableToFillSlowControlDB "<<slc<<endl;
    AMSSetupR::SlowControlR::ReadFromExternalFile=0;
}
else{
//for(;;){
//Get_setup02()->LoadSlowcontrolDB(slc.c_str());
//}
if(!Get_setup02()->LoadSlowcontrolDB(slc.c_str())){
cerr<<"AMSNtuple::readRSetup-E-UnableToLoadSlowControlDB "<<slc<<endl;
    AMSSetupR::SlowControlR::ReadFromExternalFile=0;
    if(_rfile)_rfile->cd();
}
else{
    if(_rfile)_rfile->cd();
string slf;
Get_setup02()->getSlowControlFilePath(slf);
Get_setup02()->updateSlowControlFilePath(slf);
if(slf!=slc && IOPA.ReadAMI){
cout<<"  transferring slow control "<<slf<<" "<<slc<<endl;
#ifdef __CORBA__
if(!AMSProducer::gethead()->SendFile(slf.c_str(),slc.c_str())){
cerr<<"AMSNtuple::readRSetup-E-UnableToTransferFile "<<slf<<" "<<slc<<endl;
}
#endif
}
}
}
}
#ifdef __CORBA__
AMSProducer::gethead()->SendTimeout(600);
#endif

}
else cerr<<"AMSNtuple::readRSetup-E-UnableToGetRootSetup "<<endl;


}
void AMSNtuple::Bell(){
#ifdef __CORBA__
AMSProducer::gethead()->sendCurrentRunInfo();
#endif
}
 int AMSNtuple::Lock(unsigned int tmout){
 pid_t pid=getpid();
char name[256]="";
 int len=255;
 if(gethostname(name,len)){
   struct hostent *hent;
   hent=gethostbyname(name);
   if(hent)strcpy(name,hent->h_name);
   else cerr<<"AMSNtuple::Lock::_gethostname-E-UnableToGetHostbyName "<<name<<endl;
  }

  char tmp[255];
 sprintf(tmp,"lock.%s.%d",name,pid);

 char *lockdir=getenv("AMSLockdir");
 if(!(lockdir && strlen(lockdir))){
  setenv("AMSLockdir","/afs/cern.ch/ams/local/locks/",1);
 }
 lockdir=getenv("AMSLockdir");
 string dir=lockdir;
//  look for other locks
        vector<AMSNtuple::trio> tv;
#ifdef __DARWIN__
      dirent ** namelist=0;
      int nptr=scandir(dir.c_str(),&namelist,_select,NULL);
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
      dirent64 ** namelist=0;
      int nptr=scandir64(dir.c_str(),&namelist,_select,NULL);
#endif
       time_t tnow;
       time(&tnow);
	for(int i=0;i<nptr;i++) {
            trio t;
            t.tmout=0;
            t.filename=dir;
            t.filename+=namelist[i]->d_name;
            struct stat64 statbuf;
            stat64(t.filename.c_str(),&statbuf);
            t.tmod=statbuf.st_mtime;
           ifstream fbin;
            fbin.open(t.filename.c_str());
            if(fbin){
              fbin>>t.tmout;
            }
            fbin.close();
//            cout << "lock " <<t.filename<<" "<<t.tmod<<" "<<t.tmout<<" "<<tnow<<" "<<tmp<<endl; 
            free(namelist[i]);
            if(!strstr(t.filename.c_str(),tmp))tv.push_back(t);
	}
        cout <<"AMSNtuple::Lock-I-AttemptingtoFreeNamelist "<<endl; 
	free(namelist);
        int ok=0;
        for(int i=0;i<tv.size();i++){
          if(-tnow+tv[i].tmout+tv[i].tmod>ok){
            ok=-tnow+tv[i].tmout+tv[i].tmod;
          }
          
        }
       



//  set your own lock

  dir+=tmp;
if(!ok){
ofstream fbin;
 fbin.open(dir.c_str());
 if(fbin){
   fbin<<tmout<<endl;
 }
 fbin.close();
}
 cout <<"AMSNtuple::Lock-I-Returning "<<ok<<endl;
 return ok;

}
void AMSNtuple::UnLock(){
 char *lockdir=getenv("AMSLockdir");
 if(!(lockdir && strlen(lockdir))){
  setenv("AMSLockdir","/afs/cern.ch/ams/local/locks/",1);
 }
 lockdir=getenv("AMSLockdir");
 string dir=lockdir;
 char tmp[512];
char name[256]="";
 int len=255;
 if(gethostname(name,len)){
   struct hostent *hent;
   hent=gethostbyname(name);
   if(hent)strcpy(name,hent->h_name);
   else cerr<<"AMSNtuple::UnLock::_gethostname-E-UnableToGetHostbyName "<<name<<endl;
 }
 else{
   cout <<"AMSNtuple::UnLock-I-HostName "<<name<<endl;
 }
 pid_t pid=getpid();
 sprintf(tmp,"lock.%s.%d",name,pid);
  dir+=tmp;
 unlink(dir.c_str());
}

#ifdef __DARWIN__
integer AMSNtuple::_select(  dirent *entry)
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
integer AMSNtuple::_select(  const dirent64 *entry)
#endif
{
 return strstr(entry->d_name,"lock.")!=NULL;
}
