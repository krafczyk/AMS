//  $Id: ntuple.C,v 1.123 2002/11/26 11:53:50 choutko Exp $
//
//  May 2002, A.Klimentov add Root related part
//                        NB : Delete() should be used before Expand()
//                             for any class inheriting from TObject
//                             and containing character string;
//
#include <commons.h>
#include <node.h>
#include <ntuple.h>
#include <job.h>
#include <ecaldbc.h>
#ifdef __WRITEROOT__
#include <TBranch.h>
TTree* AMSNtuple::_tree=0;
TFile* AMSNtuple::_rfile=0;
const int branchSplit=1;
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
      "nbeta[0,30],betastatus(nbeta):I,betapattern(nbeta)[0,100]:I,beta(nbeta),betac(nbeta),betaerror(nbeta),betaerrorc(nbeta),betachi2(nbeta),betachi2s(nbeta),betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,betaptr(nbeta)[-1,1000]:I");

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
 
"npart[0,10],pbetap(npart)[-1,254]:I,pchargep(npart)[-1,254]:I,ptrackp(npart)[-1,254]:I,ptrdp(npart)[-1,254]:I,pvertp(npart)[-1,2]:I,prichp(npart)[-1,254]:I,pecalp(npart)[-1,254]:I,pid(npart)[0,1000]:I,pidvice(npart)[0,1000]:I,probpid(2,npart),fitmom(npart),pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),pbeta(npart),perrbeta(npart),pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),phigl(npart),pcoo(3,npart),cutoff(npart),cootof(3,4,npart),cooanti(3,2,npart),cooecal(3,3,npart),cootr(3,8,npart),cootrd(3,npart),coorich(3,2,npart),pathrich(2,npart):R,pathrichb(2,npart):R,lengthrich(npart):R,trdlikelihood(npart):R,local(8,npart):R");
//
  HBNAME(_lun,"TOFClust",&_tof.Ntof,
 
"ntof[0,48],TOFStatus(ntof):I,TOFpln(ntof)[0,4]:I,TOFbar(ntof)[0,12]:I,TOFmem(ntof)[0,3]:I,TOFp2m(3,ntof)[0,48]:I,TOFEdep(ntof),TOFEdepd(ntof),TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)");
//

  HBNAME(_lun,"EcalClus",&_ecclust.Neccl,
 "neccl[0,60],EcclStatus(neccl):I,EcclProj(neccl)[0,1]:I,EcclPlane(neccl)[0,20]:I,EcclLeft(neccl)[0,72]:I,EcclCenter(neccl)[0,72]:I,EcclRight(neccl)[0,72]:I,EcclEdep(neccl),ecclsleak(neccl),eccldleak(neccl),EcclCoo(3,neccl),ecclpleft(neccl):I,ecclnhits(neccl)[0,72]:I");

  HBNAME(_lun,"Ecal2dCl",&_ec2dclust.Nec2dcl,
 "nec2d[0,10],ec2dstatus(nec2d):I,Ec2dProj(nec2d)[0,1]:I,Ec2dNmemb(nec2d):I,Ec2dEdep(nec2d),Ec2dCoo(nec2d),ec2dtan(nec2d),ec2dchi2(nec2d),ec2dpcl(18,nec2d)[0,1000]:I");

  HBNAME(_lun,"EcalShow",&_ecshow.Necsh,
 "necsh[0,5],ecshstatus(necsh):I,Ecshdir(3,necsh),ecshemdir(3,necsh),Ecshentry(3,necsh),Ecshexit(3,necsh),Ecshcofg(3,necsh),EcshErdir(necsh),ecshchi2dir(necsh),ecshfrontedep(necsh),EcshEn(necsh),ecsherat(3,necsh),Ecshener(necsh),ecshdifosum(necsh),EcshsLeak(necsh),EcshrLeak(necsh),EcshdLeak(necsh),EcshaLeak(necsh),EcshoLeak(necsh),ecsho2de(necsh),ecshchi2P(necsh),ecshparP(4,necsh),ecshchi2T(necsh),ecshspev(3,necsh),ecshncl(necsh)[0,7]:I,ecshpcl(2,necsh):I");
//
  HBNAME(_lun,"EcalHits",&_ecalhit.Necht,
  "necht[0,450],EchtStatus(necht):I,EchtIdsoft(necht):I,EchtProj(necht)[0,1]:I,EchtPlane(necht)[0,20]:I,EchtCell(necht)[0,80]:I,EchtEdep(necht),echtattc(necht),EchtCoo(3,necht),EchtADC(3,necht),echtped(3,necht),echtgain(necht)");
//
  HBNAME(_lun,"TOFMCClu",&_tofmc.Ntofmc,
  "ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),TOFMCedep(ntofmc)");
  HBNAME(_lun,"TrCluste",&_trcl.Ntrcl, 
  "Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,Sumt(ntrcl),Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),Amplitude(5,ntrcl):R");
  HBNAME(_lun,"TrMCClus",&_trclmc.Ntrclmc,
  "ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),xgl(3,ntrclmc),summc(ntrclmc)");
  HBNAME(_lun,"TrRecHit",&_trrh02.Ntrrh,
  "ntrrh[0,666],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,statusr(ntrrh):I,Layer(ntrrh)[1,10]:I,hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh),cofgx(ntrrh),cofgy(ntrrh)");


   HBNAME(_lun,"Vertex",&_tpai02.Ngam,
   "ngam[0,2],trgmom(ngam):R,trgerrmom(ngam),trgTheta(ngam):R,trgPhi(ngam):R,trgmass(ngam),trgVert(3,ngam):R,trgdist(ngam),trgcharge(ngam)[-2,2]:I,trgStatus(ngam):I,trgpLeft(ngam)[-1,254]:I,trgpRight(ngam)[-1,254]:I,Jthetal(ngam):R,Jphil(ngam):R,Jthetar(ngam):R,Jphir(ngam):R,Jp0l(3,ngam):R,Jp0r(3,ngam):R");

  HBNAME(_lun,"TrTrack",&_trtr02.Ntrtr,
  "ntrtr[0,100],trstatus(ntrtr):I,pattern(ntrtr)[-1,100]:I,address(ntrtr):I,nhits(ntrtr)[0,8],phits(8,ntrtr)[-1,30000]:I,LocDBAver(ntrtr):R,GeaneFitDone(ntrtr)[0,1000]:I,AdvFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),Chi2Circle(ntrtr),CircleRig(ntrtr),Chi2FastFit(ntrtr),Rigidity(ntrtr),ErrRig(ntrtr),Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),grig(ntrtr),gerrrig(ntrtr),hchi2(2,ntrtr),HRigidity(2,ntrtr),HErrRigidity(2,ntrtr),htheta(2,ntrtr),hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),pirigerr(ntrtr),rigms(ntrtr),pirig(ntrtr)");

  HBNAME(_lun,"MCEventG",&_mcg02.Nmcg,
  "nmcg[0,100],nskip(nmcg):I,Particle(nmcg)[-200,500]:I,coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R");

  HBNAME(_lun,"MCTrack",&_mct.Nmct,
  "nmct[0,150],radl(nmct),absl(nmct),pos(3,nmct),vname(nmct):I");

  HBNAME(_lun,"AntiClus",&_anti.Nanti,
  "nanti[0,16],AntiStatus(nanti):I,AntiSector(nanti)[0,100]:I,AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)");

  HBNAME(_lun,"AntiMCCl",&_antimc.Nantimc,
  "nantimc[0,100],AMCIdsoft(nantimc):I,AMCXcoo(3,nantimc),AMCtof(nantimc),AMCedep(nantimc)");

  HBNAME(_lun,"LVL3",&_lvl302.Nlvl3,
"nlvl3[0,2],LVL3TOF(nlvl3)[-1,255],LVL3TRD(nlvl3)[0,15],LVL3Tr(nlvl3)[0,15],LVL3Main(nlvl3),LVL3Dir(nlvl3)[-1,1],LVL3NTrHits(nlvl3)[0,1000],LVL3NPat(nlvl3)[0,10],LVL3Pat(2,nlvl3)[-1,250],LVL3Res(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R,LVL3TRDHits(nlvl3)[0,63],LVL3HMult(nlvl3)[0,31],LVL3TRDPar(2,nlvl3):R,LVL3emag(nlvl3)[-1,1],LVL3ECmat(nlvl3)[-1,1],LVL3ECTRKcr(4,nlvl3):R");

  HBNAME(_lun,"LVL1",&_lvl102.Nlvl1,
  "nlvl1[0,2],mode(nlvl1),LVL1Flag(nlvl1)[-10,20],LVL1TOFPatt(4,nlvl1),LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1),LVL1ECALflag(nlvl1),LVL1ECtrsum(nlvl1):R");

  HBNAME(_lun,"TrRawCl",&_trraw.Ntrraw,
  "ntrraw[0,300],rawaddress(ntrraw):I,rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R");

  HBNAME(_lun,"AntiRawC",&_antiraw.Nantiraw,
  "naraw[0,32],arawstatus(naraw):I,arawsector(naraw)[0,100]:I,arawupdown(naraw)[0,7]:I,arawsignal(naraw)");

  HBNAME(_lun,"TOFRawCl",&_tofraw.Ntofraw,
  "ntofraw[0,48],tofrstatus(ntofraw):I,tofrplane(ntofraw)[0,4]:I,tofrbar(ntofraw)[0,12]:I,tofrtovta(2,ntofraw),tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw),tofreda(ntofraw),tofredd(ntofraw),tofrtm(ntofraw),tofrcoo(ntofraw)");
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
	 //"rcribcheck(nrings):R,"
	 "rcriprob(nrings):R,rcrinpexp(nrings):R,"

	 //	 "rcricnpe(nrings):R,rcrinpexpg(nrings):R,rcrinpexpr(nrings):R,rcrinpexpb(nrings):R,rad:I,bas:I");

           "rcricnpe(nrings):R");


}
void AMSNtuple::reset(int full){
#ifdef __WRITEROOT__
    clearClones();
    expandClones();
#endif
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
    VZERO(&_trclmc.Ntrclmc,(sizeof(_trclmc))/sizeof(integer));
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
   _tpai02.Ngam=0;   
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

void AMSNtuple::clearClones() 
{
  // clear clones arrays
  // (used Delete() in expandClones() if class containing char)
  //
#ifdef __WRITEROOTCLONES__
  if ((void*)&_evroot02) {
    if (_evroot02.fAntiCluster)    _evroot02.fAntiCluster -> Clear();
    if (_evroot02.fAntiMCCluster)  _evroot02.fAntiMCCluster -> Clear();
    if (_evroot02.fAntiRawCluster) _evroot02.fAntiRawCluster -> Clear();
    if (_evroot02.fBeta)           _evroot02.fBeta -> Clear();
    if (_evroot02.fCharge)         _evroot02.fCharge -> Clear();
    if (_evroot02.fECALcluster)    _evroot02.fECALcluster -> Clear();
    if (_evroot02.fECAL2Dcluster)  _evroot02.fECAL2Dcluster -> Clear();
    if (_evroot02.fECALhit)        _evroot02.fECALhit -> Clear();
    if (_evroot02.fECALshower)     _evroot02.fECALshower -> Clear();
    if (_evroot02.fLVL1)           _evroot02.fLVL1 -> Clear();
    if (_evroot02.fLVL3)           _evroot02.fLVL3 -> Clear();
    if (_evroot02.fMCtrtrack)      _evroot02.fMCtrtrack -> Clear();
    if (_evroot02.fMCeventg)       _evroot02.fMCeventg -> Clear();
    if (_evroot02.fParticle)       _evroot02.fParticle -> Clear();
    if (_evroot02.fRICEvent)       _evroot02.fRICEvent -> Clear();
    if (_evroot02.fRICMC)          _evroot02.fRICMC -> Clear();
    if (_evroot02.fRICRing)        _evroot02.fRICRing-> Clear();
    if (_evroot02.fTOFcluster)     _evroot02.fTOFcluster -> Clear();
    if (_evroot02.fTOFMCcluster)   _evroot02.fTOFMCcluster -> Clear();
    if (_evroot02.fTOFRawCluster)  _evroot02.fTOFRawCluster -> Clear();
    if (_evroot02.fTRDMCCluster)   _evroot02.fTRDMCCluster -> Clear();
    if (_evroot02.fTRDrawhit)      _evroot02.fTRDrawhit -> Clear();
    if (_evroot02.fTRDcluster)     _evroot02.fTRDcluster -> Clear();
    if (_evroot02.fTRDsegment)     _evroot02.fTRDsegment -> Clear();
    if (_evroot02.fTRDtrack)       _evroot02.fTRDtrack -> Clear();
    if (_evroot02.fTrCluster)      _evroot02.fTrCluster -> Clear();
    if (_evroot02.fTrMCCluster)    _evroot02.fTrMCCluster -> Clear();
    if (_evroot02.fTrRawCluster)   _evroot02.fTrRawCluster -> Clear();
    if (_evroot02.fTRrechit)       _evroot02.fTRrechit -> Clear();
    //--    if (_evroot02.fTrGamma)        _evroot02.fTrGamma -> Clear();
    if (_evroot02.fTRtrack)        _evroot02.fTRtrack -> Clear();
   }
#endif
}

void AMSNtuple::deleteClones() 
{
  // clear clones arrays
  // (used Delete() in expandClones() if class containing char)
  //
#ifdef __WRITEROOTCLONES__
  if ((void*)&_evroot02) {
    if (_evroot02.fAntiCluster)    _evroot02.fAntiCluster -> Delete();
    if (_evroot02.fAntiMCCluster)  _evroot02.fAntiMCCluster -> Delete();
    if (_evroot02.fAntiRawCluster) _evroot02.fAntiRawCluster -> Delete();
    if (_evroot02.fBeta)           _evroot02.fBeta -> Delete();
    if (_evroot02.fCharge)         _evroot02.fCharge -> Delete();
    if (_evroot02.fECALcluster)    _evroot02.fECALcluster -> Delete();
    if (_evroot02.fECAL2Dcluster)  _evroot02.fECAL2Dcluster -> Delete();
    if (_evroot02.fECALhit)        _evroot02.fECALhit -> Delete();
    if (_evroot02.fECALshower)     _evroot02.fECALshower -> Delete();
    if (_evroot02.fLVL1)           _evroot02.fLVL1 -> Delete();
    if (_evroot02.fLVL3)           _evroot02.fLVL3 -> Delete();
    if (_evroot02.fMCtrtrack)      _evroot02.fMCtrtrack -> Delete();
    if (_evroot02.fMCeventg)       _evroot02.fMCeventg  -> Delete();
    if (_evroot02.fParticle)       _evroot02.fParticle  -> Delete();
    if (_evroot02.fRICEvent)       _evroot02.fRICEvent  -> Delete();
    if (_evroot02.fRICMC)          _evroot02.fRICMC     -> Delete();
    if (_evroot02.fRICRing)        _evroot02.fRICRing      -> Delete();
    if (_evroot02.fTOFcluster)     _evroot02.fTOFcluster   -> Delete();
    if (_evroot02.fTOFMCcluster)   _evroot02.fTOFMCcluster -> Delete();
    if (_evroot02.fTOFRawCluster)  _evroot02.fTOFRawCluster -> Delete();
    if (_evroot02.fTRDMCCluster)   _evroot02.fTRDMCCluster  -> Delete();
    if (_evroot02.fTRDrawhit)      _evroot02.fTRDrawhit     -> Delete();
    if (_evroot02.fTRDcluster)     _evroot02.fTRDcluster    -> Delete();
    if (_evroot02.fTRDsegment)     _evroot02.fTRDsegment    -> Delete();
    if (_evroot02.fTRDtrack)       _evroot02.fTRDtrack      -> Delete();
    if (_evroot02.fTrCluster)      _evroot02.fTrCluster     -> Delete();
    if (_evroot02.fTrMCCluster)    _evroot02.fTrMCCluster   -> Delete();
    if (_evroot02.fTrRawCluster)   _evroot02.fTrRawCluster  -> Delete();
    if (_evroot02.fTRrechit)       _evroot02.fTRrechit      -> Delete();
    //--    if (_evroot02.fTrGamma)        _evroot02.fTrGamma       -> Delete();
    if (_evroot02.fTRtrack)        _evroot02.fTRtrack       -> Delete();
   }
#endif
}

void AMSNtuple::expandClones() 
  //
  // set (shrink or extend) clones arrays size to nominal values
  //
{
#ifdef __WRITEROOTCLONES__

  if ((void*)&_evroot02) {
#ifdef __AMSDEBUG__
    cout<<"expandClones -I- expand clone arrays to nominal values"<<endl;
#endif
    if (_evroot02.fAntiCluster)    _evroot02.fAntiCluster      -> Expand(MAXANTICL);
    if (_evroot02.fAntiMCCluster)  _evroot02.fAntiMCCluster    -> Expand(MAXANTIMC);
    if (_evroot02.fAntiRawCluster) _evroot02.fAntiRawCluster   -> Expand(MAXANTIRAW);
    if (_evroot02.fBeta)           _evroot02.fBeta             -> Expand(MAXBETA02);
    if (_evroot02.fCharge)         _evroot02.fCharge           -> Expand(MAXCHARGE02);

    if (_evroot02.fECALshower)     _evroot02.fECALshower       -> Expand(MAXECSHOW);
    if (_evroot02.fECALcluster)    _evroot02.fECALcluster      -> Expand(MAXECCLUST);
    if (_evroot02.fECAL2Dcluster)  _evroot02.fECAL2Dcluster    -> Expand(MAXEC2DCLUST);
    if (_evroot02.fECALhit)        _evroot02.fECALhit          -> Expand(MAXECHITS);

    if (_evroot02.fLVL1)           _evroot02.fLVL1             -> Expand(MAXLVL1);
    if (_evroot02.fLVL3)           _evroot02.fLVL3             -> Expand(MAXLVL3);
    if (_evroot02.fMCtrtrack)      _evroot02.fMCtrtrack        -> Expand(MAXMCVOL);
    if (_evroot02.fMCeventg)       {
#ifdef __AMSDEBUG__
         cout<<"_evroot02.fMCeventg->GetLast()+1 "<<_evroot02.fMCeventg->GetLast()+1<<endl;
         cout<<"_evroot02.fMCeventg->GetSize() "<<_evroot02.fMCeventg->GetSize()<<endl;
#endif
        _evroot02.fMCeventg         -> Delete();
        _evroot02.fMCeventg         -> Expand(MAXMCG02);
    }

    if (_evroot02.fParticle)       _evroot02.fParticle         -> Expand(MAXPART02);
    if (_evroot02.fRICEvent)       _evroot02.fRICEvent         -> Expand(MAXRICHITS);
    if (_evroot02.fRICMC)          _evroot02.fRICMC            -> Expand(MAXRICMC);
    if (_evroot02.fRICRing)        _evroot02.fRICRing          -> Expand(MAXRICHRIN);
    if (_evroot02.fTOFcluster)     _evroot02.fTOFcluster       -> Expand(MAXTOF);
    if (_evroot02.fTOFMCcluster)   _evroot02.fTOFMCcluster     -> Expand(MAXTOFMC);
    if (_evroot02.fTrCluster)      _evroot02.fTrCluster        -> Expand(MAXTRCL);
    if (_evroot02.fTRDMCCluster)   _evroot02.fTRDMCCluster     -> Expand(MAXTRDCLMC);
    if (_evroot02.fTRDrawhit)      _evroot02.fTRDrawhit        -> Expand(MAXTRDRHT);
    if (_evroot02.fTRDcluster)     _evroot02.fTRDcluster       -> Expand(MAXTRDCL);
    if (_evroot02.fTRDsegment)     _evroot02.fTRDsegment       -> Expand(MAXTRDSEG);
    if (_evroot02.fTRDtrack)       _evroot02.fTRDtrack         -> Expand(MAXTRDTRK);
    if (_evroot02.fTrMCCluster)    _evroot02.fTrMCCluster      -> Expand(MAXTRCLMC);
    if (_evroot02.fTRrechit)       _evroot02.fTRrechit         -> Expand(MAXTRRH02);
    //--    if (_evroot02.fTrGamma)        _evroot02.fTrGamma          -> Expand(MAXPAIR02);
    if (_evroot02.fTRtrack)        _evroot02.fTRtrack          -> Expand(MAXTRTR02);
    if (_evroot02.fTrRawCluster)   _evroot02.fTrRawCluster     -> Expand(MAXTRRAW);
    if (_evroot02.fTOFRawCluster)  _evroot02.fTOFRawCluster    -> Expand(MAXTOFRAW);

   }
#endif
}

void AMSNtuple::createClones()
{
  //
  // create clones arrays
  //
#ifdef __WRITEROOTCLONES__

  if ((void*)&_evroot02) {

    clearClones();
    cout<<"AMSNtuple::createClones -I- create clone arrays..."<<endl;
    if (!_evroot02.fAntiCluster)    _evroot02.fAntiCluster   = new TClonesArray("AntiClusterRoot",MAXANTICL);
    if (!_evroot02.fAntiMCCluster)  _evroot02.fAntiMCCluster = new TClonesArray("ANTIMCClusterRoot",MAXANTIMC);
    if (!_evroot02.fAntiRawCluster) _evroot02.fAntiRawCluster = new TClonesArray("AntiRawClusterRoot",MAXANTIRAW);
    if (!_evroot02.fBeta)           _evroot02.fBeta          = new TClonesArray("BetaRoot02",MAXBETA02);
    if (!_evroot02.fCharge)         _evroot02.fCharge        = new TClonesArray("ChargeRoot02",MAXCHARGE02);
    if (!_evroot02.fECALcluster)    _evroot02.fECALcluster   = new TClonesArray("EcalClusterRoot",MAXECCLUST);
    if (!_evroot02.fECAL2Dcluster)  _evroot02.fECAL2Dcluster = new TClonesArray("Ecal2DClusterRoot",MAXEC2DCLUST);
    if (!_evroot02.fECALhit)        _evroot02.fECALhit       = new TClonesArray("EcalHitRoot",MAXECHITS);
    if (!_evroot02.fECALshower)     _evroot02.fECALshower    = new TClonesArray("EcalShowerRoot",MAXECSHOW);
    if (!_evroot02.fLVL1)           _evroot02.fLVL1          = new TClonesArray("LVL1Root02",MAXLVL1);
    if (!_evroot02.fLVL3)           _evroot02.fLVL3          = new TClonesArray("LVL3Root02",MAXLVL3);
    if (!_evroot02.fMCeventg)       _evroot02.fMCeventg      = new TClonesArray("MCEventGRoot02",MAXMCG02);
    if (!_evroot02.fMCtrtrack)      _evroot02.fMCtrtrack     = new TClonesArray("MCTrackRoot",MAXMCVOL);
    if (!_evroot02.fParticle)       _evroot02.fParticle      = new TClonesArray("ParticleRoot02",MAXPART02);
    if (!_evroot02.fRICMC)          _evroot02.fRICMC          = new TClonesArray("RICMCRoot",MAXRICMC);
    if (!_evroot02.fRICEvent)       _evroot02.fRICEvent       = new TClonesArray("RICEventRoot",MAXRICHITS);
    if (!_evroot02.fRICRing)        _evroot02.fRICRing        = new TClonesArray("RICRingRoot",MAXRICHRIN);
    if (!_evroot02.fTOFcluster)     _evroot02.fTOFcluster    = new TClonesArray("TOFClusterRoot",MAXTOF);
    if (!_evroot02.fTOFMCcluster)   _evroot02.fTOFMCcluster  = new TClonesArray("TOFMCClusterRoot",MAXTOFMC);
    if (!_evroot02.fTOFRawCluster)  _evroot02.fTOFRawCluster  = new TClonesArray("TOFRawClusterRoot",MAXTOFRAW);
    if (!_evroot02.fTRDMCCluster)   _evroot02.fTRDMCCluster  = new TClonesArray("TRDMCClusterRoot",MAXTRDCLMC);
    if (!_evroot02.fTRDrawhit)      _evroot02.fTRDrawhit     = new TClonesArray("TRDRawHitRoot",MAXTRDRHT);
    if (!_evroot02.fTRDcluster)     _evroot02.fTRDcluster    = new TClonesArray("TRDClusterRoot",MAXTRDCL);
    if (!_evroot02.fTRDsegment)     _evroot02.fTRDsegment    = new TClonesArray("TRDSegmentRoot",MAXTRDSEG);
    if (!_evroot02.fTRDtrack)       _evroot02.fTRDtrack      = new TClonesArray("TRDTrackRoot",MAXTRDTRK);
    if (!_evroot02.fTrCluster)      _evroot02.fTrCluster     = new TClonesArray("TrClusterRoot",MAXTRCL);
    if (!_evroot02.fTrMCCluster)    _evroot02.fTrMCCluster   = new TClonesArray("TrMCClusterRoot",MAXTRCLMC);
    if (!_evroot02.fTRrechit)       _evroot02.fTRrechit      = new TClonesArray("TrRecHitRoot02",MAXTRRH02);
    //--    if (!_evroot02.fTrGamma)        _evroot02.fTrGamma       = new TClonesArray("TrGammaRoot02",MAXPAIR02);
    if (!_evroot02.fTRtrack)        _evroot02.fTRtrack       = new TClonesArray("TrTrackRoot02",MAXTRTR02);
    if (!_evroot02.fTrRawCluster)   _evroot02.fTrRawCluster  = new TClonesArray("TrRawClusterRoot",MAXTRRAW);
  }
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
   cout<<"Set Compress Level ..."<<IOPA.WriteRoot<<endl;
   cout<<"Set Split Level ..."<<branchSplit<<endl;

   _rfile->SetCompressionLevel(IOPA.WriteRoot);

#ifdef __WRITEROOTCLONES__
    createClones();
#endif
    cout<<"AMSNtuple::initR -I- create branches"<<endl;
   _tree= new TTree("AMSRoot","AMS Ntuple Root");
    static void *pev1=(void*)&_evroot02;
   TBranch *b1=_tree->Branch("evroot02.","EventRoot02",&pev1,64000,branchSplit); 
#endif
#ifndef __WRITEROOT__
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
    //    deleteClones();
    //    createClones();
#endif
}


uinteger AMSNtuple::getrun(){
    return _event02.Run;
}
