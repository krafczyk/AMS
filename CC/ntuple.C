//  $Id: ntuple.C,v 1.94 2002/03/27 18:43:05 choutko Exp $
#include <commons.h>
#include <node.h>
#include <ntuple.h>
#include <job.h>
#include <ecaldbc.h>
#ifdef __WRITEROOT__
#include <TBranch.h>
TTree* AMSNtuple::_tree=0;
TFile* AMSNtuple::_rfile=0;
//TROOT AMSNtuple::_troot("S","S");
#endif
AMSNtuple::~AMSNtuple(){
#ifdef __WRITEROOT__
 if(_rfile){
   _rfile->Write();
   _rfile->Close();
   delete _rfile;
 }    
#endif
}

AMSNtuple::AMSNtuple(char* name) : AMSNode(AMSID(name,0)) {
  _lun=0;
//  initR(name);
}

AMSNtuple::AMSNtuple(integer lun, char* name) : AMSNode(AMSID(name,0)) {
  _lun = lun;
  init();
}

void AMSNtuple::init(){
  _Nentries=0;
  char *a=new char[strlen(getname())+1];
  strcpy(a,getname());
  HBNT(_lun, a," ");
  delete []a;
// -----> Station :
//
  HBNAME(_lun,"EventH",&_event02.Eventno,
 
"eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,TrTracks[0,1000]:I,TrRecHits[0,32766]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFMCClusters[0,10000]:I,AntiMCClusters[0,10000]:I,TRDMCClusters[0,2000]:I,AntiClusters[0,100]:I,EcalClusters[0,500]:I,EcalHits[0,5000]:I,"
"RICMCClusters[0,10000]:I,RICHits[0,10000]:I,TRDRawHits[0,2000]:I,TRDClusters[0,1000]:I,TRDSegments[0,500]:I,TRDTracks[0,255]:I,EventStatus(2):I"); 

  HBNAME(_lun,"Beta",&_beta02.Nbeta,
      "nbeta[0,50],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge02.Ncharge,
    "ncharge[0,30],chargestatus(ncharge):I,chargebetap(ncharge)[-1,30000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"TRDMCCl",&_trdclmc.Ntrdclmc,
  "ntrdmccl[0,200],trdlayermc(ntrdmccl)[0,20]:I,trdladdermc(ntrdmccl)[0,40]:I,trdtubemc(ntrdmccl)[0,100]:I,trdtrackmc(ntrdmccl)[0,1000]:I,trdedepmc(ntrdmccl),trdekinmc(ntrdmccl),trdxglmc(3,ntrdmccl),trdstepmc(ntrdmccl)");

  HBNAME(_lun,"TRDRHT",&_trdht.Ntrdht,
  "ntrdht[0,200],trdhtlayer(ntrdht)[0,19]:I,trdhtladder(ntrdht)[0,17]:I,trdhttube(ntrdht)[0,15]:I,trdampht(ntrdht)");

  HBNAME(_lun,"TRDSEG",&_trdseg.Ntrdseg,
  "ntrdseg[0,100],trdsegstatus(ntrdseg):I,trdsegor(ntrdseg)[0,10]:I,trdsegfpar(2,ntrdseg),trdsegchi2(ntrdseg),trdsegpat(ntrdseg)[0,100]:I,trdsegnh(ntrdseg)[0,12]:I,trdsegpcl(12,ntrdseg)[0,1000]:I");

  HBNAME(_lun,"TRDTRK",&_trdtrk.Ntrdtrk,
  "ntrdt[0,40],trdtstatus(ntrdt):I,trdtcoo(3,ntrdt),trdtercoo(3,ntrdt),trdtphi(ntrdt),trdttheta(ntrdt),trdtchi2(ntrdt),trdtns(ntrdt)[0,5]:I,trdtpat(ntrdt)[0,100]:I,trdtps(5,ntrdt):I");

  HBNAME(_lun,"TRDClu",&_trdcl.Ntrdcl,
  "ntrdcl[0,100],trdclstatus(ntrdcl):I,trdclcoo(3,ntrdcl),trdlayer(ntrdcl)[0,20]:I,trddir(3,ntrdcl),trdmul(ntrdcl)[0,31]:I,trdhmul(ntrdcl)[0,31]:I,trdedep(ntrdcl),ptrdrht(ntrdcl)[0,1000]:I");

  HBNAME(_lun,"Particle",&_part02.Npart,
 
"npart[0,20],pbetap(npart)[-1,254]:I,pchargep(npart)[-1,254]:I,ptrackp(npart)[-1,254]:I,ptrdp(npart)[-1,254]:I,prichp(npart)[-1,254]:I,pecalp(npart)[-1,254]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pbeta(npart),perrbeta(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),cutoff(npart),cootof(3,4,npart),cooanti(3,2,npart),cooecal(3,3,npart),cootr(3,8,npart),cootrd(3,npart)");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
  "ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,bar(ntof)[0,15]:I,nmemb(ntof)[0,15]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//

  HBNAME(_lun,"EcalClus",&_ecclust.Neccl,
 "neccl[0,60],EcclStatus(neccl):I,EcclProj(neccl)[0,1]:I,EcclPlane(neccl)[0,20]:I,EcclLeft(neccl)[0,72]:I,EcclCenter(neccl)[0,72]:I,EcclRight(neccl)[0,72]:I,EcclEdep(neccl),ecclsleak(neccl),eccldleak(neccl),EcclCoo(3,neccl),ecclpleft(neccl):I,ecclnhits(neccl)[0,72]:I");

  HBNAME(_lun,"Ecal2dCl",&_ec2dclust.Nec2dcl,
 "nec2d[0,10],ec2dstatus(nec2d):I,Ec2dProj(nec2d)[0,1]:I,Ec2dNmemb(nec2d):I,Ec2dEdep(nec2d),Ec2dCoo(nec2d),ec2dtan(nec2d),ec2dchi2(nec2d),ec2dpcl(18,nec2d)[0,1000]:I");

  HBNAME(_lun,"EcalShow",&_ecshow.Necsh,
 "necsh[0,5],ecshstatus(necsh):I,Ecshdir(3,necsh),ecshemdir(3,necsh),Ecshentry(3,necsh),Ecshexit(3,necsh),Ecshcofg(3,necsh),EcshErdir(necsh),ecshchi2dir(necsh),ecshfrontedep(necsh),EcshEn(necsh),ecsherat(3,necsh),Ecshener(necsh),ecshdifosum(necsh),EcshsLeak(necsh),EcshrLeak(necsh),EcshdLeak(necsh),EcshoLeak(necsh),ecsho2de(necsh),ecshchi2P(necsh),ecshparP(4,necsh),ecshchi2T(necsh),ecshspevT(3,necsh),ecshpcl(2,necsh):I");
//
  HBNAME(_lun,"EcalHits",&_ecalhit.Necht,
  "necht[0,500],EchtStatus(necht):I,EchtIdsoft(necht):I,EchtProj(necht)[0,1]:I,EchtPlane(necht)[0,20]:I,EchtCell(necht)[0,80]:I,EchtEdep(necht),EchtCoo(3,necht)");
//
  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");
  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");
  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");
  HBNAME(_lun,"TrRecHit",&_trrh02.Ntrrh,
  "ntrrh[0,666],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[0,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh)");
  HBNAME(_lun,"TrTrack",&_trtr02.Ntrtr,
  "ntrtr[0,100],trstatus(ntrtr):I,pattern(ntrtr)[0,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvancedFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRidgidity(ntrtr),Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),gerrridgidity(ntrtr),hchi2(2,ntrtr),HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),ridgidityms(ntrtr),pirigidity(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg02.Nmcg,
  "nmcg[0,100],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"MCTrack",&_mct.Nmct,
  "nmct[0,150],radl(nmct),absl(nmct),pos(3,nmct),vname(nmct):I");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
  "nanti[0,16],AntiStatus(nanti):I,AntiSector(nanti)[0,100]:I,AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,200],AntiMCIdsoft(nantimc):I,AntiMCXcoo(3,nantimc),AntiMCtof(nantimc),AntiMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl302.Nlvl3,
  "nlvl3[0,2],LVL3TOF(nlvl3)[-1,255],LVL3TRD(nlvl3)[0,15],LVL3Tr(nlvl3)[0,15],LVL3Main(nlvl3),LVL3Dir(nlvl3)[-1,1],LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pat(2,nlvl3)[-1,250],LVL3Res(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R,LVL3TRDHits(nlvl3)[0,63],LVL3HMult(nlvl3)[0,31],LVL3TRDPar(2,nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl102.Nlvl1,
 "nlvl1[0,2],mode(nlvl1),LVL1Flag(nlvl1)[-10,20],LVL1TOFPatt(4,nlvl1),LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1),LVL1ECALflag(nlvl1)");
  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");

  HBNAME(_lun,"AntiRawC",&_antiraw.Nantiraw,
  "naraw[0,32],arawstatus(naraw):I,arawsector(naraw)[0,100]:I,arawupdown(naraw)[0,7]:I,arawsignal(naraw)");

  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,
  "ntofraw[0,20],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,7]:I,tofrbar(ntofraw)[0,31]:I,tofrtovta(2,ntofraw),tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");
  HBNAME(_lun,"RICMCCl",&_richmc.NMC,
  	 "nsignals[0,300],sid(nsignals):I,"
  	 "origin(3,nsignals),direction(3,nsignals),RICstatus(nsignals):I,"
         "nphgen:I,phit(nsignals):I");

  HBNAME(_lun,"RICEvent",&_richevent.Nhits,
    	 "Rhits[0,100]:I,Rchtchannel(Rhits):I,Rchtadc(Rhits):I,"
  	 "Rchtx(Rhits),Rchty(Rhits)");

  HBNAME(_lun,"Ring",&_ring.NRings,
  	 "nrings[0,100]:I,rcritrackn(nrings):I,"
  	 "rcrihused(nrings):I,rcrimhused(nrings):I,"
	 "rcribeta(nrings),rcriebeta(nrings),"
	 "rcrichi2(nrings)");

}
void AMSNtuple::reset(int full){
#ifdef __WRITEROOT__
int sto=sizeof(TObject);
#else
int sto=0;
#endif
  if( full){
    VZERO(&_beta02.Nbeta,(sizeof(_beta02)-sto)/sizeof(integer));
    VZERO(&_charge02.Ncharge,(sizeof(_charge02)-sto)/sizeof(integer));
    VZERO(&_part02.Npart,(sizeof(_part02)-sto)/sizeof(integer));
    VZERO(&_tof.Ntof,(sizeof(_tof)-sto)/sizeof(integer));
    VZERO(&_ecclust.Neccl,(sizeof(_ecclust)-sto)/sizeof(integer));
    VZERO(&_ec2dclust.Nec2dcl,(sizeof(_ec2dclust)-sto)/sizeof(integer));
    VZERO(&_ecshow.Necsh,(sizeof(_ecshow)-sto)/sizeof(integer));
    VZERO(&_ecalhit.Necht,(sizeof(_ecalhit)-sto)/sizeof(integer));
    VZERO(&_tofmc.Ntofmc,(sizeof(_tofmc)-sto)/sizeof(integer));
    VZERO(&_trcl.Ntrcl,(sizeof(_trcl)-sto)/sizeof(integer));
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc)-sto)/sizeof(integer));
    VZERO(&_trdclmc.Ntrdclmc,(sizeof(_trdclmc)-sto)/sizeof(integer));
    VZERO(&_trdcl.Ntrdcl,(sizeof(_trdcl)-sto)/sizeof(integer));
    VZERO(&_trdht.Ntrdht,(sizeof(_trdht)-sto)/sizeof(integer));
    VZERO(&_trdtrk.Ntrdtrk,(sizeof(_trdtrk)-sto)/sizeof(integer));
    VZERO(&_trdseg.Ntrdseg,(sizeof(_trdseg)-sto)/sizeof(integer));
    VZERO(&_trrh02.Ntrrh,(sizeof(_trrh02)-sto)/sizeof(integer));
    VZERO(&_trtr02.Ntrtr,(sizeof(_trtr02)-sto)/sizeof(integer));
    VZERO(&_mct.Nmct,(sizeof(_mct)-sto)/sizeof(integer));
    VZERO(&_mcg02.Nmcg,(sizeof(_mcg02)-sto)/sizeof(integer));
    VZERO(&_anti.Nanti,(sizeof(_anti)-sto)/sizeof(integer));
    VZERO(&_antimc.Nantimc,(sizeof(_antimc)-sto)/sizeof(integer));
    VZERO(&_lvl302.Nlvl3,(sizeof(_lvl302)-sto)/sizeof(integer));
    VZERO(&_lvl102.Nlvl1,(sizeof(_lvl102)-sto)/sizeof(integer));
    VZERO(&_trraw.Ntrraw,(sizeof(_trraw)-sto)/sizeof(integer));
    VZERO(&_antiraw.Nantiraw,(sizeof(_antiraw)-sto)/sizeof(integer));
    VZERO(&_tofraw.Ntofraw,(sizeof(_tofraw)-sto)/sizeof(integer));
    VZERO(&_richmc.NMC,(sizeof(_richmc)-sto)/sizeof(integer));
    VZERO(&_richevent.Nhits,(sizeof(_richevent)-sto)/sizeof(integer));
    VZERO(&_ring.NRings,(sizeof(_ring)-sto)/sizeof(integer));
  }
  else{
   _beta02.Nbeta= 0;
   _charge02.Ncharge = 0;
   _part02.Npart = 0;
   _tof.Ntof = 0;
   _ecclust.Neccl = 0;
   _ec2dclust.Nec2dcl = 0;
   _ecshow.Necsh = 0;
   _ecalhit.Necht = 0;
   _tofmc.Ntofmc = 0;
   _trcl.Ntrcl = 0;
   _trdclmc.Ntrdclmc=0;
   _trdcl.Ntrdcl=0;
   _trdht.Ntrdht=0;
   _trdtrk.Ntrdtrk=0;
   _trdseg.Ntrdseg=0;
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc)-sto)/sizeof(integer));
   _trrh02.Ntrrh = 0;
   _trtr02.Ntrtr = 0;
   _mcg02.Nmcg = 0;
   _mct.Nmct = 0;   
   _anti.Nanti = 0;
   _antimc.Nantimc = 0;
   _lvl302.Nlvl3 = 0;
   _lvl102.Nlvl1 = 0;
   _trraw.Ntrraw = 0;
   _antiraw.Nantiraw = 0;
   _tofraw.Ntofraw = 0;
   _richmc.NMC=0;
   _richevent.Nhits=0;
   _ring.NRings=0;
  }
}

void AMSNtuple::write(integer addentry){
  if(_lun){
     HFNT(_lun);
     _Nentries++;
  }
}

void AMSNtuple::endR(){
#ifdef __WRITEROOT__
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
     delete _rfile;
   }
   _rfile=0;
#endif
}
void AMSNtuple::initR(char* fname){
#ifdef __WRITEROOT__
  static TROOT _troot("S","S");
   cout << "Initializing tree...\n"<<endl;
  _Nentries=0;
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
     delete _rfile;
   }
   _rfile= new TFile(fname,"RECREATE");
   if(!_rfile)throw amsglobalerror("UnableToOpenRootFile",3);
   _rfile->SetCompressionLevel(IOPA.WriteRoot);
   _tree= new TTree("AMSRoot","AMS Ntuple Root");
    static void *pev2=(void*)&_event02;
   TBranch *b2=_tree->Branch("event02.", "EventNtuple02",  &pev2, 64000,1); 
   static void *pev4=(void*)&_beta02;
   TBranch *b4=_tree->Branch("beta02.", "BetaNtuple02",  &pev4, 64000,1);
   static void *pev6=(void*)&_charge02;
   TBranch *b6=_tree->Branch("charge02.", "ChargeNtuple02",  &pev6, 64000,1);
   static void *pev77=(void*)&_part02;
   TBranch *b77=_tree->Branch("part02.", "ParticleNtuple02",  &pev77, 64000,1);
   static void *pev8=(void*)&_tof;
   TBranch *b8=_tree->Branch("tofcl.", "TOFClusterNtuple",  &pev8, 64000,1);
   static void *pev9=(void*)&_tofmc;
   TBranch *b9=_tree->Branch("tofmccl.", "TOFMCClusterNtuple",  &pev9, 64000,1);
   static void *pev0=(void*)&_trcl;

   TBranch *b0=_tree->Branch("trcl.", "TrClusterNtuple",  &pev0, 64000,1);
   static void *peva=(void*)&_trclmc;
   TBranch *ba=_tree->Branch("trclmc.", "TrMCClusterNtuple",  &peva, 64000,1);
   static void *pevd=(void*)&_trrh02;
   TBranch *bd=_tree->Branch("trrh02.", "TrRecHitNtuple02",  &pevd, 64000,1);
   static void *pevb=(void*)&_trdclmc;
   TBranch *bb=_tree->Branch("trdclmc.", "TRDMCClusterNtuple",  &pevb, 64000,1);
   static void *pevb1=(void*)&_trdcl;
   TBranch *bb1=_tree->Branch("trdcl.", "TRDClusterNtuple",  &pevb1, 64000,1);
   static void *pevb2=(void*)&_trdht;
   TBranch *bb2=_tree->Branch("trdht.", "TRDRawHitNtuple",  &pevb2, 64000,1);
   static void *pevb3=(void*)&_trdtrk;
   TBranch *bb3=_tree->Branch("trdtrk.", "TRDTrackNtuple",  &pevb3, 64000,1);
   static void *pevb4=(void*)&_trdseg;
   TBranch *bb4=_tree->Branch("trdseg.", "TRDSegmentNtuple",  &pevb4, 64000,1);
   static void *pevf=(void*)&_trtr02;
   TBranch *bf=_tree->Branch("trtr02.", "TrTrackNtuple02",  &pevf, 64000,1);
   static void *pevgg1=(void*)&_mct;
   TBranch *bgg1=_tree->Branch("mct.", "MCTrackNtuple",  &pevgg1, 64000,1);
   static void *pevgg=(void*)&_mcg02;
   TBranch *bgg=_tree->Branch("mcg02.", "MCEventGNtuple02",  &pevgg, 64000,1);
   static void *pevs=(void*)&_ecclust;
   TBranch *bs=_tree->Branch("ecalcl.", "EcalClusterNtuple",  &pevs, 64000,1);

   static void *pevs2=(void*)&_ec2dclust;
   TBranch *bs2=_tree->Branch("ecal2dcl.", "Ecal2DClusterNtuple",  &pevs2, 64000,1);

   static void *pevsh=(void*)&_ecshow;
   TBranch *bsh=_tree->Branch("ecalsh.", "EcalShowerNtuple",  &pevsh, 64000,1);
   static void *pevt=(void*)&_ecalhit;
   TBranch *bt=_tree->Branch("ecalht.", "EcalHitNtuple",  &pevt, 64000,1);
   static void *pevu=(void*)&_richmc;
   TBranch *bu=_tree->Branch("ricmccl.","RICMCNtuple",&pevu,64000,1);
   static void *pevv=(void*)&_richevent;
   TBranch *bv=_tree->Branch("ricevent.","RICEventNtuple",&pevv,64000,1);
   static void *pevw=(void*)&_ring;
   TBranch *bw=_tree->Branch("ring.","RICRing",&pevw,64000,1);  
   static void *pevn=(void*)&_lvl102;
   TBranch *bn=_tree->Branch("lvl102.", "LVL1Ntuple02",  &pevn, 64000,1);
   static void *pevj=(void*)&_anti;
   TBranch *bj=_tree->Branch("anti.", "AntiClusterNtuple",  &pevj, 64000,1);
   static void *pevk=(void*)&_antimc;
   TBranch *bk=_tree->Branch("antimc.", "ANTIMCClusterNtuple",  &pevk, 64000,1);
   static void *pevl=(void*)&_lvl302;
   TBranch *bl=_tree->Branch("lvl302.", "LVL3Ntuple02",  &pevl, 64000,1);
   
   static void *pevx1=(void*)&_trraw;
   TBranch *bx1=_tree->Branch("trraw.", "TrRawClusterNtuple",  &pevx1, 64000,1);
   
   static void *pevx2=(void*)&_antiraw;
   TBranch *bx2=_tree->Branch("antiraw.", "AntiRawClusterNtuple",  &pevx2, 64000,1);
   
   static void *pevx3=(void*)&_tofraw;
   TBranch *bx3=_tree->Branch("tofraw.", "TofRawClusterNtuple",  &pevx3, 64000,1);
   cout <<"AMSNtuple::initR-I-OpenRootFile "<<fname<<" "<<_rfile<<" "<<endl;
#else
cerr <<" RootFileOutput is Not supported in this version "<<endl;
exit(1);
#endif

}
void AMSNtuple::writeR(){
#ifdef __WRITEROOT__
    if(_tree){
    if(!_lun )_Nentries++;
     _tree->Fill();
    }
#endif
}


uinteger AMSNtuple::getrun(){
    return _event02.Run;
}
