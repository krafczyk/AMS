#include <commons.h>
#include <node.h>
#include <ntuple.h>
#ifdef __WRITEROOT__
#include <TBranch.h>
TTree* AMSNtuple::_tree=0;
TFile* AMSNtuple::_rfile=0;
#endif
AMSNtuple::~AMSNtuple(){
#ifdef __WRITEROOT__
#ifdef __AMSDEBUG__
 if(_tree)_tree->Print();
#endif
 if(_rfile){
   _rfile->Write();
   _rfile->Close();
   delete _rfile;
 }    
#endif
}

AMSNtuple::AMSNtuple(char* name) : AMSNode(AMSID(name,0)) {
  _lun=0;
  initR(name);
}

AMSNtuple::AMSNtuple(integer lun, char* name) : AMSNode(AMSID(name,0)) {
  _lun = lun;
  init();
}

void AMSNtuple::init(){
  _Nentries=0;
  HBNT(_lun, getname()," ");
  HBNAME(_lun,"EventH",&_event.Eventno,
  "eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,Particles[0,1000]:I,Tracks[0,1000]:I,Betas[0,1000]:I,Charges[0,1000]:I,TrRecHits[0,10000]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFMCClusters[0,10000]:I,CTCClusters[0,100]:I,CTCMCClusters[0,100000]:I,AntiMCClusters[0,10000]:I,AntiClusters[0,100]:I,EventStatus:I");

  HBNAME(_lun,"Beta",&_beta.Nbeta,
      "nbeta[0,150],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge.Ncharge,
    "ncharge[0,150],chargestatus(ncharge):I,chargebetap(ncharge)[-1,30000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"Particle",&_part.Npart,
  "npart[0,100],pbetap(npart)[0,30000]:I,pchargep(npart)[-1,30000]:I,ptrackp(npart)[-1,30000]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),atcnbcel(2,npart):I,atcnbphe(2,npart),atcidcel(2,npart):I,atcdispm(2,npart):I,atcdaero(2,npart):I,atcstatu(2,npart):I,cutoff(npart),cooctc(3,2,npart),cootof(3,4,npart),cooanti(3,2,npart),cootr(3,8,npart)");

  HBNAME(_lun,"TOFClust",&_tof.Ntof,
//  "ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,bar(ntof)[0,100]:I,TOFEdep(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
  "ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,bar(ntof)[0,15]:I,nmemb(ntof)[0,15]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");

  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");

  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");

  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");

  HBNAME(_lun,"TrRecHit",&_trrh.Ntrrh,
  "ntrrh[0,500],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[0,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh)");

  HBNAME(_lun,"TrTrack",&_trtr.Ntrtr,
  "ntrtr[0,200],trstatus(ntrtr):I,pattern(ntrtr)[0,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvancedFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRidgidity(ntrtr),Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr),gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),ridgidityms(ntrtr),pirigidity(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg.Nmcg,
  "nmcg[0,20],nskip(nmcg):I,Particle(nmcg)[-1,30000]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"CTCClust",&_ctccl.Nctccl,
  "nctccl[0,20],CTCStatus(nctccl):I,CTCLayer(nctccl)[0,10]:I,ctccoo(3,nctccl),ctcercoo(3,nctccl),ctcrawsignal(nctccl),ctcsignal(nctccl),ctcesignal(nctccl)");

  HBNAME(_lun,"CTCMCClu",&_ctcclmc.Nctcclmc,
  "nctcclmc[0,200],CTCMCIdsoft(nctcclmc):I,CTCMCXcoo(3,nctcclmc),CTCMCXdir(3,nctcclmc),CTCstep(nctcclmc),ctccharge(nctcclmc),ctcbeta(nctcclmc),ctcedep(nctcclmc)");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
  "nanti[0,16],AntiStatus(nanti):I,AntiSector(nanti)[0,100]:I,AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,200],AntiMCIdsoft(nantimc):I,AntiMCXcoo(3,nantimc),AntiMCtof(nantimc),AntiMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl3.Nlvl3,
  "nlvl3[0,2],LVL3TOFTr(nlvl3)[-1,10],LVL3AntiTr(nlvl3)[0,10],LVL3TrackerTr(nlvl3),LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pattern(2,nlvl3)[-1,250],LVL3Residual(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl1.Nlvl1,
  "nlvl1[0,2],LVL1LifeTime(nlvl1),LVL1Flag(nlvl1)[-10,20],LVL1TOFPatt(4,nlvl1),LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1)");

  HBNAME(_lun,"CTCHit",&_ctcht.Nctcht,
  "nctcht[0,50],CTChitStatus(nctcht):I,CTChitLayer(nctcht)[0,10]:I,ctchitcolumn(nctcht)[0,100]:I,ctchitrow(nctcht)[0,100]:I,ctchitsignal(nctcht)");

  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");

  HBNAME(_lun,"AntiRawC",&_antiraw.Nantiraw,
  "nantiraw[0,32],antirawstatus(nantiraw):I,antirawsector(nantiraw)[0,100]:I,antirawupdown(nantiraw)[0,7]:I,antirawsignal(nantiraw)");

  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,
  "ntofraw[0,20],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,7]:I,tofrbar(ntofraw)[0,31]:I,tofrtovta(2,ntofraw),tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");

}
void AMSNtuple::reset(int full){
  if(full){
    if(_beta.Nbeta)VZERO(&_beta,sizeof(_beta)/sizeof(integer));
    if(_charge.Ncharge)VZERO(&_charge,sizeof(_charge)/sizeof(integer));
    if(_part.Npart)VZERO(&_part,sizeof(_part)/sizeof(integer));
    if(_tof.Ntof)VZERO(&_tof,sizeof(_tof)/sizeof(integer));
    if(_tofmc.Ntofmc)VZERO(&_tofmc,sizeof(_tofmc)/sizeof(integer));
    if(_trcl.Ntrcl)VZERO(&_trcl,sizeof(_trcl)/sizeof(integer));
    if(_trclmc.Ntrclmc)VZERO(&_trclmc,sizeof(_trclmc)/sizeof(integer));
    if(_trrh.Ntrrh)VZERO(&_trrh,sizeof(_trrh)/sizeof(integer));
    if(_trtr.Ntrtr)VZERO(&_trtr,sizeof(_trtr)/sizeof(integer));
    if(_mcg.Nmcg)VZERO(&_mcg,sizeof(_mcg)/sizeof(integer));
    if(_ctccl.Nctccl)VZERO(&_ctccl,sizeof(_ctccl)/sizeof(integer));
    if(_ctcclmc.Nctcclmc)VZERO(&_ctcclmc,sizeof(_ctcclmc)/sizeof(integer));
    if(_anti.Nanti)VZERO(&_anti,sizeof(_anti)/sizeof(integer));
    if(_antimc.Nantimc)VZERO(&_antimc,sizeof(_antimc)/sizeof(integer));
    if(_lvl3.Nlvl3)VZERO(&_lvl3,sizeof(_lvl3)/sizeof(integer));
    if(_lvl1.Nlvl1 )VZERO(&_lvl1,sizeof(_lvl1)/sizeof(integer));
    if(_ctcht.Nctcht)VZERO(&_ctcht,sizeof(_ctcht)/sizeof(integer));
    if(_trraw.Ntrraw)VZERO(&_trraw,sizeof(_trraw)/sizeof(integer));
    if(_antiraw.Nantiraw)VZERO(&_antiraw,sizeof(_antiraw)/sizeof(integer));
    if(_tofraw.Ntofraw)VZERO(&_tofraw,sizeof(_tofraw)/sizeof(integer));
  }
  else{
   _beta.Nbeta= 0;
   _charge.Ncharge = 0;
   _part.Npart = 0;
   _tof.Ntof = 0;
   _tofmc.Ntofmc = 0;
   _trcl.Ntrcl = 0;
   _trclmc.Ntrclmc = 0;
   _trrh.Ntrrh = 0;
   _trtr.Ntrtr = 0;
   //_mcg.Nmcg = 0;
   _ctccl.Nctccl = 0;
   _ctcclmc.Nctcclmc = 0;
   _anti.Nanti = 0;
   _antimc.Nantimc = 0;
   _lvl3.Nlvl3 = 0;
   _lvl1.Nlvl1 = 0;
   _ctcht.Nctcht = 0;
   _trraw.Ntrraw = 0;
   _antiraw.Nantiraw = 0;
   _tofraw.Ntofraw = 0;
  }
}

void AMSNtuple::write(integer addentry){
  if(_lun){
     HFNT(_lun);
     _Nentries++;
  }
}

void AMSNtuple::initR(char* fname){
#ifdef __WRITEROOT__
   static  TROOT dummy("S","S");
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
     delete _rfile;
   }
   if(_tree)delete _tree;
   _rfile= new TFile(fname,"RECREATE");
   _rfile->SetCompressionLevel(IOPA.WriteRoot);
   _tree= new TTree("AMSRoot","AMS Ntuple Root");
   //_tree->SetAutoSave(100000000);
   void *pev1=(void*)&_event;
   TBranch *b1=_tree->Branch("event", "EventNtuple",  &pev1, 32000,1);
   void *pev2=(void*)&_beta;
   TBranch *b2=_tree->Branch("beta", "BetaNtuple",  &pev2, 32000,1);
   void *pev3=(void*)&_charge;
   TBranch *b3=_tree->Branch("charge", "ChargeNtuple",  &pev3, 32000,1);
   void *pev4=(void*)&_part;
   TBranch *b4=_tree->Branch("part", "ParticleNtuple",  &pev4, 32000,1);
   void *pev5=(void*)&_tof;
   TBranch *b5=_tree->Branch("tofcl", "TOFClusterNtuple",  &pev5, 32000,1);
   void *pev6=(void*)&_tofmc;
   TBranch *b6=_tree->Branch("tofmccl", "TOFMCClusterNtuple",  &pev6, 32000,1);
   void *pev7=(void*)&_trcl;
   TBranch *b7=_tree->Branch("trcl", "TrClusterNtuple",  &pev7, 32000,1);
   void *pev8=(void*)&_trclmc;
   TBranch *b8=_tree->Branch("trclmc", "TrMCClusterNtuple",  &pev8, 32000,1);
   void *pev9=(void*)&_trrh;
   TBranch *b9=_tree->Branch("trrh", "TrRecHitNtuple",  &pev9, 32000,1);
   void *pev0=(void*)&_trtr;
   TBranch *b0=_tree->Branch("trtr", "TrTrackNtuple",  &pev0, 32000,1);
   void *peva=(void*)&_mcg;
   TBranch *ba=_tree->Branch("mcg", "MCEventGNtuple",  &peva, 32000,1);
   void *pevb=(void*)&_ctccl;
   TBranch *bb=_tree->Branch("ctccl", "CTCClusterNtuple",  &pevb, 32000,1);
   void *pevc=(void*)&_ctcclmc;
   TBranch *bc=_tree->Branch("ctcclmc", "CTCMCClusterNtuple",  &pevc, 32000,1);
   void *pevd=(void*)&_anti;
   TBranch *bd=_tree->Branch("anti", "AntiClusterNtuple",  &pevd, 32000,1);
   void *peve=(void*)&_antimc;
   TBranch *be=_tree->Branch("antimc", "ANTIMCClusterNtuple",  &peve, 32000,1);
   void *pevf=(void*)&_lvl3;
   TBranch *bf=_tree->Branch("lvl3", "LVL3Ntuple",  &pevf, 32000,1);
   void *pevg=(void*)&_lvl1;
   TBranch *bg=_tree->Branch("lvl1", "LVL1Ntuple",  &pevg, 32000,1);
   void *pevh=(void*)&_ctcht;
   TBranch *bh=_tree->Branch("ctcht", "CTCHitNtuple",  &pevh, 32000,1);
   void *pevj=(void*)&_trraw;
   TBranch *bj=_tree->Branch("trraw", "TrRawClusterNtuple",  &pevj, 32000,1);
   void *pevk=(void*)&_antiraw;
   TBranch *bk=_tree->Branch("antiraw", "AntiRawClusterNtuple",  &pevk, 32000,1);
   void *pevl=(void*)&_tofraw;
   TBranch *bl=_tree->Branch("tofraw", "TOFRawClusterNtuple",  &pevl, 32000,1);
   cout <<"AMSNtuple::initR-I-OpenRootFile "<<fname<<" "<<_rfile<<" "<<_tree<<endl;
#else
cerr <<" RootFileOutput is Not supported in this version "<<endl;
exit(1);
#endif
}

void AMSNtuple::writeR(){
#ifdef __WRITEROOT__
  if(_tree){
    _tree->Fill();
    if(!_lun )_Nentries++;
  }
#endif
}
