//  $Id: ntuple.C,v 1.70 2001/03/05 10:51:18 choutko Exp $
#include <commons.h>
#include <node.h>
#include <ntuple.h>
#include <job.h>
#include <ecaldbc.h>
#ifdef __WRITEROOT__
#include <TBranch.h>
TTree* AMSNtuple::_tree=0;
TFile* AMSNtuple::_rfile=0;
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
  initR(name);
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
  if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
  HBNAME(_lun,"EventH",&_event.Eventno,
 
"eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,Particles[0,1000]:I,Tracks[0,1000]:I,Betas[0,1000]:I,Charges[0,1000]:I,TrRecHits[0,10000]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFMCClusters[0,10000]:I,CTCClusters[0,100]:I,CTCMCClusters[0,100000]:I,AntiMCClusters[0,10000]:I,AntiClusters[0,100]:I,EventStatus:I");
  HBNAME(_lun,"Beta",&_beta.Nbeta,
      "nbeta[0,150],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge.Ncharge,
    "ncharge[0,150],chargestatus(ncharge):I,chargebetap(ncharge)[-1000,1000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"Particle",&_part.Npart,
  "npart[0,100],pbetap(npart)[0,30000]:I,pchargep(npart)[-1,30000]:I,ptrackp(npart)[-1,30000]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),atcnbcel(2,npart):I,atcnbphe(2,npart),atcidcel(2,npart):I,atcdispm(2,npart):I,atcdaero(2,npart):I,atcstatu(2,npart):I,cutoff(npart),cooctc(3,2,npart),cootof(3,4,npart),cooanti(3,2,npart),cootr(3,6,npart)");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
  "ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,bar(ntof)[0,15]:I,nmemb(ntof)[0,15]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//
  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");

  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");

  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");

  HBNAME(_lun,"TrRecHit",&_trrh.Ntrrh,
  "ntrrh[0,500],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[0,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh)");

  HBNAME(_lun,"TrTrack",&_trtr.Ntrtr,
  "ntrtr[0,200],trstatus(ntrtr):I,pattern(ntrtr)[0,100]:I,address(ntrtr):I,nhits(ntrtr)[0,6],phits(6,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvancedFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRidgidity(ntrtr),Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr),gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),ridgidityms(ntrtr),pirigidity(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg.Nmcg,
  "nmcg[0,20],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

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
else{
// -----> Station :
  HBNAME(_lun,"EventH",&_event02.Eventno,
 
"eventno:I,run:I,runtype:I,time(2):I,RawWords:I,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,VelTheta:R,VelPhi:R,ThetaM:R,PhiM:R,Particles[0,1000]:I,Tracks[0,1000]:I,Betas[0,1000]:I,Charges[0,1000]:I,TrRecHits[0,10000]:I,TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,TOFMCClusters[0,10000]:I,TRDMCClusters[0,2000]:I,AntiMCClusters[0,10000]:I,AntiClusters[0,100]:I,EcalClusters[0,50]:I,EcalHits[0,500]:I,"
"RICMCClusters[0,10000]:I,RICHits[0,10000]:I,EventStatus(2):I"); //CJM

  HBNAME(_lun,"Beta",&_beta02.Nbeta,
      "nbeta[0,100],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

  HBNAME(_lun,"Charge",&_charge02.Ncharge,
    "ncharge[0,100],chargestatus(ncharge):I,chargebetap(ncharge)[-1,30000]:I,chargetof(ncharge)[0,100]:I,chargetracker(ncharge)[0,100]:I,probtof(4,ncharge),chintof(4,ncharge)[0,100]:I,probtracker(4,ncharge),chintracker(4,ncharge)[0,100]:I,proballtracker(ncharge),truntof(ncharge),truntofd(ncharge),truntracker(ncharge)");

  HBNAME(_lun,"TRDMCCl",&_trdclmc.Ntrdclmc,
  "ntrdmccl[0,200],trdlayer(ntrdmccl)[0,20]:I,trdladder(ntrdmccl)[0,40]:I,trdtube(ntrdmccl)[0,100]:I,trdtrack(ntrdmccl)[0,1000]:I,trdedep(ntrdmccl),trdekin(ntrdmccl),trdxgl(3,ntrdmccl)");

  HBNAME(_lun,"Particle",&_part02.Npart,
 
"npart[0,50],pbetap(npart)[0,30000]:I,pchargep(npart)[-1,30000]:I,ptrackp(npart)[-1,30000]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),cutoff(npart),cootof(3,4,npart),cooanti(3,2,npart),cooecal(3,18,npart),cootr(3,8,npart)");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
  "ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,bar(ntof)[0,15]:I,nmemb(ntof)[0,15]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//
  HBNAME(_lun,"EcalClus",&_ecclust.Neccl,
 "neccl[0,50],EcclStatus(neccl):I,EcclProj(neccl)[0,1]:I,EcclPlane(neccl)[0,20]:I,EcclNmemb(neccl)[0,72]:I,EcclEdep(neccl),EcclCoo(3,neccl),EcclErrCoo(3,neccl)");
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
  "ntrtr[0,100],trstatus(ntrtr):I,pattern(ntrtr)[0,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvancedFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRidgidity(ntrtr),Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr),gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),ridgidityms(ntrtr),pirigidity(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg02.Nmcg,
  "nmcg[0,100],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");


  HBNAME(_lun,"AntiClus",&_anti.Nanti,
  "nanti[0,16],AntiStatus(nanti):I,AntiSector(nanti)[0,100]:I,AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,200],AntiMCIdsoft(nantimc):I,AntiMCXcoo(3,nantimc),AntiMCtof(nantimc),AntiMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl3.Nlvl3,
  "nlvl3[0,2],LVL3TOFTr(nlvl3)[-1,10],LVL3AntiTr(nlvl3)[0,10],LVL3TrackerTr(nlvl3),LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pattern(2,nlvl3)[-1,250],LVL3Residual(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl102.Nlvl1,
 "nlvl1[0,2],LVL1LifeTime(nlvl1),LVL1Flag(nlvl1)[-10,20],LVL1TOFPatt(4,nlvl1),LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1),LVL1ECALflag(nlvl1)");


  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");

  HBNAME(_lun,"AntiRawC",&_antiraw.Nantiraw,
  "nantiraw[0,32],antirawstatus(nantiraw):I,antirawsector(nantiraw)[0,100]:I,antirawupdown(nantiraw)[0,7]:I,antirawsignal(nantiraw)");

  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,
  "ntofraw[0,20],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,7]:I,tofrbar(ntofraw)[0,31]:I,tofrtovta(2,ntofraw),tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");

  HBNAME(_lun,"RICMCCl",&_richmc.NMC,
  	 "nsignals[0,300],sid(nsignals):I,"
  	 "origin(3,nsignals),direction(3,nsignals),RICstatus(nsignals):I,"
         "nphgen:I,phit(nsignals):I");

  HBNAME(_lun,"RICEvent",&_richevent.Nhits,
    	 "Rhits[0,100]:I,Rchannel(Rhits):I,Radc(Rhits):I,"
  	 "Rx(Rhits),Ry(Rhits)");

  HBNAME(_lun,"Ring",&_ring.NRings,
  	 "nrings[0,100]:I,trackn(nrings):I,"
  	 "hused(nrings):I,rbeta(nrings),quality(nrings)");

}
}
void AMSNtuple::reset(int full){
#ifdef __WRITEROOT__
int sto=sizeof(TObject);
#else
int sto=0;
#endif
  if( full){
    VZERO(&_beta.Nbeta,(sizeof(_beta)-sto)/sizeof(integer));
    VZERO(&_beta02.Nbeta,(sizeof(_beta02)-sto)/sizeof(integer));
    VZERO(&_charge02.Ncharge,(sizeof(_charge02)-sto)/sizeof(integer));
    VZERO(&_part.Npart,(sizeof(_part)-sto)/sizeof(integer));
    VZERO(&_part02.Npart,(sizeof(_part)-sto)/sizeof(integer));
    VZERO(&_tof.Ntof,(sizeof(_tof)-sto)/sizeof(integer));
    VZERO(&_ecclust.Neccl,(sizeof(_ecclust)-sto)/sizeof(integer));
    VZERO(&_ecalhit.Necht,(sizeof(_ecalhit)-sto)/sizeof(integer));
    VZERO(&_tofmc.Ntofmc,(sizeof(_tofmc)-sto)/sizeof(integer));
    VZERO(&_trcl.Ntrcl,(sizeof(_trcl)-sto)/sizeof(integer));
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc)-sto)/sizeof(integer));
    VZERO(&_trdclmc.Ntrdclmc,(sizeof(_trdclmc)-sto)/sizeof(integer));
    VZERO(&_trrh.Ntrrh,(sizeof(_trrh)-sto)/sizeof(integer));
    VZERO(&_trrh02.Ntrrh,(sizeof(_trrh02)-sto)/sizeof(integer));
    VZERO(&_trtr.Ntrtr,(sizeof(_trtr)-sto)/sizeof(integer));
    VZERO(&_trtr02.Ntrtr,(sizeof(_trtr02)-sto)/sizeof(integer));
    VZERO(&_mcg.Nmcg,(sizeof(_mcg)-sto)/sizeof(integer));
    VZERO(&_mcg02.Nmcg,(sizeof(_mcg02)-sto)/sizeof(integer));
    VZERO(&_ctccl.Nctccl,(sizeof(_ctccl)-sto)/sizeof(integer));
    VZERO(&_ctcclmc.Nctcclmc,(sizeof(_ctcclmc)-sto)/sizeof(integer));
    VZERO(&_anti.Nanti,(sizeof(_anti)-sto)/sizeof(integer));
    VZERO(&_antimc.Nantimc,(sizeof(_antimc)-sto)/sizeof(integer));
    VZERO(&_lvl3.Nlvl3,(sizeof(_lvl3)-sto)/sizeof(integer));
    VZERO(&_lvl1.Nlvl1,(sizeof(_lvl1)-sto)/sizeof(integer));
    VZERO(&_lvl102.Nlvl1,(sizeof(_lvl102)-sto)/sizeof(integer));
    VZERO(&_ctcht.Nctcht,(sizeof(_ctcht)-sto)/sizeof(integer));
    VZERO(&_trraw.Ntrraw,(sizeof(_trraw)-sto)/sizeof(integer));
    VZERO(&_antiraw.Nantiraw,(sizeof(_antiraw)-sto)/sizeof(integer));
    VZERO(&_tofraw.Ntofraw,(sizeof(_tofraw)-sto)/sizeof(integer));
    VZERO(&_richmc.NMC,(sizeof(_richmc)-sto)/sizeof(integer));
    VZERO(&_richevent.Nhits,(sizeof(_richevent)-sto)/sizeof(integer));
    VZERO(&_ring.NRings,(sizeof(_ring)-sto)/sizeof(integer));
  }
  else{
   _beta.Nbeta= 0;
   _charge.Ncharge = 0;
   _beta02.Nbeta= 0;
   _charge02.Ncharge = 0;
   _part.Npart = 0;
   _part02.Npart = 0;
   _tof.Ntof = 0;
   _ecclust.Neccl = 0;
   _ecalhit.Necht = 0;
   _tofmc.Ntofmc = 0;
   _trcl.Ntrcl = 0;
   _trdclmc.Ntrdclmc=0;
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc)-sto)/sizeof(integer));
   _trrh.Ntrrh = 0;
   _trrh02.Ntrrh = 0;
   _trtr.Ntrtr = 0;
   _trtr02.Ntrtr = 0;
   //_mcg.Nmcg = 0;
   _ctccl.Nctccl = 0;
   _ctcclmc.Nctcclmc = 0;
   _anti.Nanti = 0;
   _antimc.Nantimc = 0;
   _lvl3.Nlvl3 = 0;
   _lvl1.Nlvl1 = 0;
   _lvl102.Nlvl1 = 0;
   _ctcht.Nctcht = 0;
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
   cout << "Initializing tree...\n"<<endl;
  _Nentries=0;
   static  TROOT dummy("S","S");
   if(_rfile){
     _rfile->Write();
     _rfile->Close();
//     delete _rfile;
   }
   _rfile= new TFile(fname,"RECREATE");
   if(!_rfile)throw amsglobalerror("UnableToOpenRootFile",3);
   _rfile->SetCompressionLevel(IOPA.WriteRoot);
//   _tree->SetAutoSave(100000000);
   cout <<"AMSNtuple::initR-I-OpenRootFile "<<fname<<" "<<_rfile<<" "<<endl;
#else
cerr <<" RootFileOutput is Not supported in this version "<<endl;
exit(1);
#endif

}
void AMSNtuple::writeR(){
#ifdef __WRITEROOT__
char key[64];
    if(!_lun )_Nentries++;
     sprintf(key,"%d",_Nentries);
     Write(key);
#endif
}


uinteger AMSNtuple::getrun(){
  if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
    return _event.Run;
  }
  else{
    return _event02.Run;
  }
}