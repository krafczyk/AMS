#include <commons.h>
#include <node.h>
#include <ntuple.h>

AMSNtuple::AMSNtuple(integer lun, char* name) : AMSNode(AMSID(name,0)) {

  _lun = lun;

  HBNT(_lun, name," ");

  HBNAME(_lun,"EventH",&_event.Eventno,
  "eventno:I,run:I,runtype:I,time(2):I,RawWords:I,GrMedPhi:R,RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,Particles:I,Tracks:I,Betas:I,Charges:I,TrRecHits:I,TrClusters:I,TrRawClusters:I,TrMCClusters:I,TOFClusters:I,TOFMCClusters:I,CTCClusters:I,CTCMCClusters:I,AntiMCClusters:I,AntiClusters:I");

  HBNAME(_lun,"Beta",&_beta.Nbeta,
      "nbeta[0,100],betastatus(nbeta):I,betapattern(nbeta):I,beta(nbeta),betaerror(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta):I,betaptof(4,nbeta):I,betaptr(nbeta):I");

  HBNAME(_lun,"Charge",&_charge.Ncharge,
    "ncharge[0,10],chargestatus(ncharge):I,chargebetap(ncharge):I,chargetof(ncharge):I,chargetracker(ncharge):I,probtof(7,ncharge),probtracker(7,ncharge)");

  HBNAME(_lun,"Particle",&_part.Npart,
  "npart[0,10],pctcp(2,npart):I,pbetap(npart):I,pchargep(npart):I,ptrackp(npart):I,pid(npart):I,pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pcharge(npart),ptheta(npart),pphi(npart),pcoo(3,npart),signalctc(2,npart),betactc(2,npart),errorbetactc(2,npart),cooctc(3,2,npart),cootof(3,4,npart),cooanti(3,2,npart),cootr(3,6,npart)");

  HBNAME(_lun,"TOFClust",&_tof.Ntof,
  "ntof[0,20],TOFStatus(ntof):I,plane(ntof):I,bar(ntof):I,TOFEdep(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");

  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");

  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl):I,NelemR(ntrcl):I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");

  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");

  HBNAME(_lun,"TrRecHit",&_trrh.Ntrrh,
  "ntrrh[0,500],px(ntrrh):I,py(ntrrh):I,statusr(ntrrh):I,Layer(ntrrh):I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh)");

  HBNAME(_lun,"TrTrack",&_trtr.Ntrtr,
  "ntrtr[0,20],trstatus(ntrtr):I,pattern(ntrtr):I,nhits(ntrtr),phits(6,ntrtr):I,FastFitDone(ntrtr):I,GeaneFitDone(ntrtr):I,AdvancedFitDone(ntrtr):I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRidgidity(ntrtr),Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr),gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),gchi2ms(ntrtr),ridgidityms(ntrtr),gridgidityms(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg.Nmcg,
  "nmcg[0,20],nskip(nmcg):I,Particle(nmcg):I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"CTCClust",&_ctccl.Nctccl,
  "nctccl[0,20],CTCStatus(nctccl):I,CTCLayer(nctccl):I,ctccoo(3,nctccl),ctcercoo(3,nctccl),ctcrawsignal(nctccl),ctcsignal(nctccl),ctcesignal(nctccl)");

  HBNAME(_lun,"CTCMCClu",&_ctcclmc.Nctcclmc,
  "nctcclmc[0,200],CTCMCIdsoft(nctcclmc):I,CTCMCXcoo(3,nctcclmc),CTCMCXdir(3,nctcclmc),CTCstep(nctcclmc),ctccharge(nctcclmc),ctcbeta(nctcclmc),ctcedep(nctcclmc)");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
  "nanti[0,16],AntiStatus(nanti):I,AntiSector(nanti):I,AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,200],AntiMCIdsoft(nantimc):I,AntiMCXcoo(3,nantimc),AntiMCtof(nantimc),AntiMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl3.Nlvl3,
  "nlvl3[0,2],LVL3TOFTr(nlvl3),LVL3AntiTr(nlvl3),LVL3TrackerTr(nlvl3),LVL3NTrHits(nlvl3),LVL3NPat(nlvl3),LVL3Pattern(2,nlvl3),LVL3Residual(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl1.Nlvl1,
  "nlvl1[0,2],LVL1Mode(nlvl1),LVL1Flag(nlvl1),LVL1TOFPatt(4,nlvl1),LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1)");

  HBNAME(_lun,"CTCHit",&_ctcht.Nctcht,
  "nctcht[0,50],CTChitStatus(nctcht):I,CTChitLayer(nctcht):I,ctchitcolumn(nctcht):I,ctchitrow(nctcht):I,ctchitsignal(nctcht)");

  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,500],rawaddress(ntrraw):I,rawlength(ntrraw):I,s2n(ntrraw):R");

  HBNAME(_lun,"AntiRawC",&_antiraw.Nantiraw,
  "nantiraw[0,32],antirawstatus(nantiraw):I,antirawsector(nantiraw):I,antirawupdown(nantiraw):I,antirawsignal(nantiraw)");

  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,
  "ntofraw[0,20],tofrstatus(ntofraw):I,tofrplane(ntofraw):I,tofrbar(ntofraw):I,tofrtovta(2,ntofraw),tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw)");

}

void AMSNtuple::reset(){
//  _event.Eventno = 0;
  _beta.Nbeta = 0;
  _charge.Ncharge = 0;
  _part.Npart = 0;
  _tof.Ntof = 0;
  _tofmc.Ntofmc = 0;
  _trcl.Ntrcl = 0;
  _trclmc.Ntrclmc = 0;
  _trrh.Ntrrh = 0;
  _trtr.Ntrtr = 0;
  _mcg.Nmcg = 0;
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

void AMSNtuple::write(){
  HFNT(_lun);
}
