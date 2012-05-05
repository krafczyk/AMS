//        ANTI-calib program, v1.0, 15.10.2003, E.Choumilov
#include "tofdbc02.h"
#include "tofsim02.h"
#include "point.h"
#include "typedefs.h"
#include "event.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "antidbc02.h"
#include "trrec.h"
#include "antirec02.h"
#include "daqs2block.h"
#include "anticalib02.h"
#include "particle.h"
#include "user.h"
#include <time.h>
#include "timeid.h"
#ifdef _PGTRACK_
#include "MagField.h"
#endif
using namespace std;
using namespace ANTI2C;
//
//
//=============================================================================
//
  integer AntiCalib::evs2bin[ANTI2C::CalChans][ANTI2C::LongBins];
  number AntiCalib::adccount[ANTI2C::CalChans][ANTI2C::LongBins][ANTI2C::BinEvsMX];
  number AntiCalib::fitpars[5];//working arrays for profile-fit
  integer AntiCalib::fitbins;
  number AntiCalib::values[ANTI2C::LongBins]; 
  number AntiCalib::errors[ANTI2C::LongBins]; 
  number AntiCalib::coords[ANTI2C::LongBins];
//
//--------------------------
void AntiCalib::init(){ // ----> initialization for AMPL-calibration 
  integer i,j,il,ib,ii,jj,id,nadd,nbnr,chan,is,ic,ie,logs,phys;
  geant blen,dd,bw,bl,bh,hll,hhl;
  char title[127];
  char hname[127];
  geant padlen=ANTI2DBc::scleng();//z-size
//
  for(ic=0;ic<CalChans;ic++){
    for(ib=0;ib<LongBins;ib++){
      evs2bin[ic][ib]=0;
      for(ie=0;ie<BinEvsMX;ie++){
        adccount[ic][ib][ie]=0;
      }
    }
  }
//
//  ---> book hist. for side-signals:
//
  if(ATREFFKEY.reprtf[0]>1){
    hll=10;
    hhl=4000;
    for(logs=0;logs<MAXANTI;logs++){//log.sect.loop
      for(phys=0;phys<2;phys++){//phys.sect.index
        for(is=0;is<2;is++){//side loop
          id=2*(2*logs+phys)+is;
          sprintf(hname,"MidBinAmpl(AllCorr), LogS-%01d, PhysS-%01d, Side-%01d",logs+1,phys+1,is+1);
          HBOOK1(2561+id,hname,100,hll,hhl,0.);
        }
      }
    }
  }
//---
  HBOOK1(2624,"Long.responce profile",LongBins,-padlen/2,padlen/2,0.);//buffer histogr
//
}
//--------------------------------------
//
void AntiCalib::select(){ // ------> event selection for AMPL-calibration
  bool anchok;
  integer ntdct,tdct[ANTI2C::ANTHMX],nftdc,ftdc[TOF2GC::SCTHMX1];
  geant adca;
  int16u id,idN,sta;
  number ampe[2],uptm[2];
  number am1[ANTI2C::MAXANTI],am2[ANTI2C::MAXANTI];
  integer frsecn[ANTI2C::MAXANTI],frsect;
  integer i,j,jmax,sector,isid,nsds,stat,chnum,n1,n2,i1min,i2min;
  integer status(0);
  uinteger Runum(0);
  geant ftdel[2],padlen,padrad,padth,padfi,paddfi,ped,sig,tzer;
  int nphsok;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
//----
  ANTI2JobStat::addre(11);
  Runum=AMSEvent::gethead()->getrun();// current run number
  padlen=ANTI2DBc::scleng();//z-size
  padrad=ANTI2DBc::scradi();//int radious
  padth=ANTI2DBc::scinth();//thickness
  paddfi=360./ANTI2C::MAXANTI;//per logical sector
  frsect=0;
  nsds=0;
  ntdct=0;
  uptm[0]=0;
  uptm[1]=0;
  ampe[0]=0;
  ampe[1]=0;
  while(ptr){ // <------ RawEvent hits loop
    id=ptr->getid();//BBS
    sector=id/10-1;//Readout(logical) sector number (0-7)
    isid=id%10-1;
    chnum=sector*2+isid;//channels numbering
    adca=ptr->getadca();
    ntdct=ptr->gettdct(tdct);
    nftdc=ptr->getftdc(ftdc);
    if(ntdct>0 && ntdct<=5 && nftdc==1){//select only low mult. Hist/FT-hit events
//
      ped=ANTIPeds::anscped[sector].apeda(isid);//adc-chan
      sig=ANTIPeds::anscped[sector].asiga(isid);//adc-ch sigmas
      ampe[nsds]=number(adca);
//cout<<"    decoded signal="<<ampe[nsds]<<endl; 
//TDC-ch-->time(ns):
      nphsok=ANTI2VPcal::antivpcal[sector].NPhysSecOK();
      if(nphsok==2)tzer=ANTI2SPcal::antispcal[sector].gettzerc();
      else tzer=ANTI2SPcal::antispcal[sector].gettzer(nphsok);
      uptm[nsds]=(ftdc[0]-tdct[0])*ANTI2DBc::htdcbw() + tzer;//TDC-ch-->ns + compens.tzero
//cout<<"    decoded Up-time="<<uptm[nsds]<<endl;
//
      nsds+=1;
    }//--->endof low mult. check
//--------
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
//
      if(nsds==2 && ampe[0]>30. && ampe[1]>30.){//good sector ?
         ANTI2JobStat::addbr(sector,2);//count good sect
	 frsecn[frsect]=sector;//store fired log.sect.number
	 am1[frsect]=ampe[0];//store signals(adc)
	 am2[frsect]=ampe[1];
	 frsect+=1;//counts fired good sectors
      }//--->endof good sector check
//
      nsds=0;
      ntdct=0;
      ampe[0]=0;
      ampe[1]=0;
      uptm[0]=0;
      uptm[1]=0;
    }//--->endof next sector check
//---
    ptr=ptr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//
  if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
    HF1(2530,geant(frsect),1.);
}
  }
  if(frsect!=1)return;//remove empty/multiple sector events
  ANTI2JobStat::addre(12);
//
  padfi=paddfi*(0.5+frsecn[0]);//fired logical-paddle phi
//
//
//------> get parameters from tracker:
//
    number pmas;
    number pmom,mom,bet(0.97),chi2,betm,pcut[2];
    number the,phi,trl,rigid(2),err,ctran;
    integer chargeTOF,chargeTracker,charge,trpatt,betpatt;
    number chi2t,chi2s;
    AMSPoint C0(0,0,0);
    AMSPoint cooCyl(0,0,0);
    AMSPoint crcCyl;
    AMSDir dirCyl(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSCharge  *pcharge;
    AMSTRDTrack *ptrd;
    AMSBeta *pbeta;
    int npart(0),ipatt,bad(1),envindx(0);
    bool trktr,trdtr,ecaltr,nottr,badint;
//
    for(i=0;i<2;i++){//i=0->keeps parts.with true(Trk/Trd)-track, i=1->...false(nonTrk/Trd)-track
      cptr=AMSEvent::gethead()->getC("AMSParticle",i);// get pointer to part-envelop "i"
      if(cptr){
        if(cptr->getnelem()!=0){
          npart+=cptr->getnelem();
	  envindx=i;
          break;//use 1st non-empty envelop
        }
      }
    }
    if(npart!=1)return;// require events with 1 particle
    ANTI2JobStat::addre(19);
//
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",envindx);
    bool TrkTrPart=false;
    bool AnyTrPart=false;
    bool GoodTrPart=false;
    bool GoodTrkTrack=false;
//			      
    while(ppart){
      ptrack=ppart->getptrack();//get pointer of the Track used in given particle
      if(ptrack){
        AnyTrPart=true;
        ANTI2JobStat::addre(20);
        ptrd=ppart->getptrd();//get pointer of the TRD-track, used in given particle
        trdtr=(ptrack->checkstatus(AMSDBc::TRDTRACK)!=0);
        ecaltr=(ptrack->checkstatus(AMSDBc::ECALTRACK)!=0);
        nottr=(ptrack->checkstatus(AMSDBc::NOTRACK)!=0);
        badint=(ptrack->checkstatus(AMSDBc::BADINTERPOL)!=0);
	if(nottr || ecaltr || badint)goto Nextp;//looking only for TRK/TRD-track particle
	GoodTrPart=true;
        ANTI2JobStat::addre(21);//good track part(TRK || TRD)
	if(!trdtr){//tracker-track based part
	  TrkTrPart=true;
	  ANTI2JobStat::addre(22);
          trpatt=ptrack->getpattern();//TRK-track pattern
	  if(trpatt>=0){
#ifdef _PGTRACK_
	    chi2=ptrack->GetChisq(AMSTrTrack::kChoutko);
	    rigid=ptrack->GetRigidity(AMSTrTrack::kChoutko);
	    err=ptrack->GetErrRinv(AMSTrTrack::kChoutko);
	    the=ptrack->GetTheta(AMSTrTrack::kChoutko);
	    phi=ptrack->GetPhi(AMSTrTrack::kChoutko);
	    C0=ptrack->GetP0(AMSTrTrack::kChoutko);
#else
            ptrack->getParFastFit(chi2,rigid,err,the,phi,C0);
#endif
            status=ptrack->getstatus();
            pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
            pbeta=ppart->getpbeta();
	    if(pbeta){
              betpatt=pbeta->getpattern();
              bet=pbeta->getbeta();
              chi2t=pbeta->getchi2();
              chi2s=pbeta->getchi2S();
	    }
            pmas=ppart->getmass();
            chargeTracker=pcharge->getchargeTracker();
	    charge=chargeTracker;
//                                    <--- put here good TRK-track selection(chi2,...)
	    bad=0;
	    ANTI2JobStat::addre(23);
            GoodTrkTrack=true;
          }
	}//--->endof "trk-track particle" check
	else{//trd-track based particle
	  ANTI2JobStat::addre(24);
//                                    <--- put here good TRD-track selection(chi2,...)
	  bad=0;
	  charge=1;
	  ANTI2JobStat::addre(25);
	}
      }//--->endof "any track particle" check
      if(GoodTrPart)break;//accept 1st particle with any good(not ecal-based) track
//      if(GoodTrkTrack)break;//accept 1st particle with good Trk-track
Nextp:
      ppart=ppart->next();
    } 
//
    if(bad)return;//require good track part.
    ANTI2JobStat::addre(26);
    if(ATCAFFKEY.trackmode==0 && !TrkTrPart)return;//trd-track part when trk requested
    if(TrkTrPart){
      ANTI2JobStat::addre(28);
    }
    else{
      ANTI2JobStat::addre(27);
    }
//
// ----->  find track crossing points/angles with sectors:
//
    number dir,cpsn,cpcs,cphi,dphi1,dphi2,phil,phih,zcut,zscr,thes,phis;
    integer nseccr(0),isphys(-1);
    phil=padfi-0.5*paddfi;//boundaries of fired sector
    phih=padfi+0.5*paddfi;
    zcut=padlen/2;
    for(i=0;i<2;i++){//<---dir loop
      dir=2*i-1;
      ptrack->interpolateCyl(cooCyl,dirCyl,padrad,dir,crcCyl,the,phi,trl);//phi/the in rads !!!
// cout<<"i/dir="<<i<<" "<<dir<<" cr="<<padrad<<" th/ph/tr="<<the<<" "<<phi<<" "<<trl<<endl;
// cout<<"cooCyl="<<cooCyl[0]<<" "<<cooCyl[1]<<" "<<cooCyl[2]<<endl; 
// cout<<"dirCyl="<<dirCyl[0]<<" "<<dirCyl[1]<<" "<<dirCyl[2]<<endl; 
// cout<<"crcCyl="<<crcCyl[0]<<" "<<crcCyl[1]<<" "<<crcCyl[2]<<endl; 
      cpsn=crcCyl[1]/sqrt(crcCyl[0]*crcCyl[0]+crcCyl[1]*crcCyl[1]);//sin(phi) of cr.point
      cpcs=crcCyl[0]/sqrt(crcCyl[0]*crcCyl[0]+crcCyl[1]*crcCyl[1]);//cos(phi) of cr.point
      cphi=atan2(cpsn,cpcs)*AMSDBc::raddeg;//phi of cr.point(degr,+-180)
      if(cphi<0)cphi=360+cphi;//0-360
      phi=phi*AMSDBc::raddeg;//inpact phi(degr,+-180)
      if(phi<0)phi=360+phi;//0-360
      if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
        HF1(2531,geant(crcCyl[2]),1.);
}
      }
      if(fabs(crcCyl[2])<zcut){//match in Z 
        if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
          if(i==0)HF1(2626,geant(frsecn[0]),1.);
	  else HF1(2627,geant(frsecn[0]),1.);
          HF1(2540,geant(cphi),1.);
          HF1(2541,geant(phi),1.);
}
	}
        dphi1=padfi-cphi;//PHIsect-PHIcrp
	if(dphi1>180)dphi1=360-dphi1;
	if(dphi1<-180)dphi1=-(360+dphi1);
        if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
          HF1(2532,geant(dphi1),1.);//PHIsect-PHIcrp
}
	}
        if(cphi<(phih-0.5) && cphi>(phil+0.5)){//match in phi (-0.5 degr for safety)
	  if(cphi<padfi)isphys=2*frsecn[0];//physical sector number(0-15)
	  else isphys=2*frsecn[0]+1;
	  zscr=crcCyl[2];
	  thes=the;//rads !!!
	  phis=phi;//degrees
          nseccr+=1;
	  dphi2=cphi-phi;//PHIcrp-PHIimpp
	  if(dphi2>180)dphi2=360-dphi2;
	  if(dphi2<-180)dphi2=-(360+dphi2);
          if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
            if(i==0)HF1(2628,geant(frsecn[0]),1.);
	    else HF1(2629,geant(frsecn[0]),1.);
            HF1(2533,geant(dphi2),1.);//PHIcrp-PHIimpp
}
	  }
        }//phi-match ok?
      }//z-match ok?
    }//endof dir loop
//
    if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
      HF1(2530,geant(nseccr+10),1.);
}
    }
    if(nseccr!=1)return;//requir. 1-sect. crossing
    if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
      HF1(2539,geant(isphys+1),1.);//phys.sector number
}
    }
    ANTI2JobStat::addre(14);
//
    if(abs(charge)!=1)return;//requir. Z=1
    ANTI2JobStat::addre(15);
//
    if(fabs(dphi2)>=60)return;//too high impact phi
    ANTI2JobStat::addre(16);
//    
    number betg(4);
    if(MAGSFFKEY.magstat==1 && TrkTrPart){
      pmom=fabs(rigid);
      if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
        HF1(2534,geant(pmom),1.);
        HF1(2535,bet,1.);
        HF1(2536,pmas,1.);
}
      }
      betg=pmom/pmas;//use trk-defined betg when possible
    }
    if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
      HF1(2537,betg,1.);
}
    }
    if(betg>6)return;//non MIP area
    ANTI2JobStat::addre(17);
//
// ------> normalize Edep(adc-ch) to normal  incidence :
//
    geant ama[2],cinp,crlen,betnor,pathcorr;
    dphi2/=AMSDBc::raddeg;//degr->rad
    pathcorr=sqrt(pow(cos(dphi2),2)*pow(sin(thes),2)/
                  (pow(sin(thes),2)+pow(cos(thes),2)*pow(cos(dphi2),2)));
    crlen=padth/pathcorr;//pass-length in scint(ignore local curvature)
    if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
      HF1(2538,crlen,1.);
      if(frsecn[0]==3){
        HF1(2596,geant(am1[0]),1.);
        HF1(2597,geant(am2[0]),1.);
      }
}
    }
    if(crlen>4)return;//too high cr.length: suspicious
//
    ANTI2JobStat::addre(18);
    ama[0]=am1[0];//imply single sector event
    ama[1]=am2[0];
    ama[0]=Anti2RawEvent::accsatur(am1[0]);//<--apply nonlin.corrections(because we use raw amplitudes)
    ama[1]=Anti2RawEvent::accsatur(am2[0]);
    if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
      if(frsecn[0]==3){
        HF1(2594,ama[0],1.);
        HF1(2595,ama[1],1.);
      }
}
    }
    ama[0]*=pathcorr;//normalize to norm.inc(ignore loc.curv.)
    ama[1]*=pathcorr;//...                   
    cinp=zscr;
//
// ------> normalize Edep(adc-ch) to mip :
//
    if(fabs(bet)<0.97)betnor=pow(fabs(bet),5./3)/pow(0.97,5./3);//norm. to MIP
    else betnor=1;
    ama[0]*=betnor;
    ama[1]*=betnor;
//
// ------> fill working arrays and histograms :
//
//   cout<<"Fill:sect/ama/x="<<isphys<<" "<<ama[0]<<" "<<ama[1]<<" "<<cinp<<endl;
#pragma omp critical (acccal_fill)
{
    AntiCalib::fill(isphys,ama,cinp);//for relat.gains,profile,abs.norm.
} 
}
//--------------------------------------
//   ---> program to accumulate data for relat.gains/prof/abs.nor-calibration:
void AntiCalib::fill(integer isec, geant am[2], geant coo){
//am is in adc-ch units, isec is abs.phys.sect number 0-15
  integer id,isd,bin,logs,phys;
  geant ampc,padlen;
//
  padlen=ANTI2DBc::scleng();//z-size
  if(ATREFFKEY.reprtf[0]>1){
    HF1(2545+isec,coo,1.);//phys.sect longit.imp.point distribution
  }
  if(fabs(coo)>=padlen/2)return;
  bin=floor(LongBins*(coo+padlen/2)/padlen);
//
  logs=isec/2;
  phys=isec%2;
  for(isd=0;isd<2;isd++){
    ampc=am[isd];
    id=2*(2*logs+phys)+isd;
    if(evs2bin[id][bin]<BinEvsMX && ampc>20 && ampc<=3500){
      adccount[id][bin][evs2bin[id][bin]]=ampc;
      evs2bin[id][bin]+=1;
    }  
    if(ATREFFKEY.reprtf[0]>1){
      if(bin==9 || bin==10)HF1(2561+id,ampc,1.);
    }
  }
//
}
//--------------------------------------
//            ---> fit-program to get final Anti-calibration consts:
void AntiCalib::fit(){
//
  integer i,j,k,chan,isd,nev,nmin,nmax,lsec;
  integer phys,logs,bin,id,nbins(0);
  int ndef[2]={0,0};
  geant elos(1.6/2);//m.p. eloss/pad, norm.inc., per side
  geant atl[2],att,adc2pe,mev2pe;
  geant profl[ANTI2C::LongBins],profle[ANTI2C::LongBins];
  number *pntr[ANTI2C::BinEvsMX];
  geant paddl=ANTI2DBc::scleng();//sector length
  geant binw=paddl/LongBins;//bin width
  geant coo;
  number aver,avere,aver2;
  number slope,norm,chi2;
  geant lowcut(0.03),higcut(0.95);// portion of ampls to cut on low/high ends of spectrum
//  
  geant pe2ad[ANTI2C::MAXANTI][2][2],pe2add(35);//[logs][physs][side]
  integer stat[ANTI2C::MAXANTI][2][2];//
  geant atlen[ANTI2C::MAXANTI][2][2],atlend(210);//
  geant power[ANTI2C::MAXANTI][2][2],powerd(2.6);//
  geant snor[ANTI2C::MAXANTI][2][2],snord(0.32);//
//
  int ifit[4];
  char pnam[4][6],pnm[6];
  number argl[10];
  int iargl[10],ier;
  number start[4],pstep[4],plow[4],phigh[4];
  number defslop=0.004;//250cm
  strcpy(pnam[0],"slope");
  strcpy(pnam[1],"mnorm");
  strcpy(pnam[2],"power");
  strcpy(pnam[3],"snorm");
  start[0]=0.004;//slope
  start[1]=500.;//mnorm
  start[2]=2.6;//power
  start[3]=0.3;//snorm
  pstep[0]=0.001;
  pstep[1]=25.;
  pstep[2]=0.2;
  pstep[3]=0.05;
  plow[0]=-0.01;
  plow[1]=50;
  plow[2]=0.1;
  plow[3]=0;
  phigh[0]=0.01;
  phigh[1]=2000;
  phigh[2]=6.;
  phigh[3]=1;
  for(i=0;i<4;i++)ifit[i]=1;//release all par's by default
//  ifit[3]=0;//fix snorm
  cout<<"-----> Start Gain/AttLen-calibr, init. parameters..."<<endl;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("ACC Longit.Resp.Uniformity-calibration");
  argl[0]=number(-1);
  MNEXCM(mfun,"SET PRINT",argl,1,ier,0);
  for(i=0;i<4;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"  <--- Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"  <--- Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
  cout<<"       init done, start fit ..."<<endl;
//---
  for(logs=0;logs<MAXANTI;logs++){//log.sect.loop
    for(phys=0;phys<2;phys++){//phys.sect.index
      for(isd=0;isd<2;isd++){//side loop
        mev2pe=ANTI2DBc::mev2pe()*ANTI2SPcal::antispcal[logs].getmev2pe(phys,isd);//mev2pe
        stat[logs][phys][isd]=1;//bad
        atlen[logs][phys][isd]=atlend;
        pe2ad[logs][phys][isd]=pe2add;
        power[logs][phys][isd]=powerd;
        snor[logs][phys][isd]=snord;
        i=0;//redef.slope with correct sign vs isd
	if(isd==0)start[i]=-defslop;
	else start[i]=defslop;
        strcpy(pnm,pnam[i]);
        ier=0;
        argl[0]=number(0);
        MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
        if(ier!=0){
          cout<<"  <--  Param-ReInit problem for param="<<pnam[i]<<'\n';
          exit(10);
        }
        id=2*(2*logs+phys)+isd;
        if(ATREFFKEY.reprtf[0]>1)
	                         HPRINT(2561+id);
	fitbins=0;
	for(bin=0;bin<LongBins;bin++){//<--bin-loop
	  profl[bin]=0;
	  profle[bin]=0;
	  nev=evs2bin[id][bin];
	  coo=-paddl/2+bin*binw+0.5*binw;
	  profl[bin]=0;
	  if(nev>100){
            for(k=0;k<nev;k++)pntr[k]=&adccount[id][bin][k];//pointers to event-signals of chan=id 
            AMSsortNAG(pntr,nev);//sort in increasing order
            nmax=integer(floor(nev*higcut));// to keep % of lowest amplitudes
            nmin=integer(floor(nev*lowcut));// to remove % of lowest amplitudes
	    aver=0;
	    avere=0;
	    aver2=0;
            for(j=nmin;j<nmax;j++){
	      aver+=(*pntr[j]);
	      aver2+=(*pntr[j])*(*pntr[j]);
	    }
            if((nmax-nmin)>0){
	      aver=aver/(nmax-nmin);
	      aver2=aver2/(nmax-nmin);
	      avere=aver2-aver*aver;
	      if(avere>0){
	        avere=sqrt(avere);//rms
		avere/=sqrt(nmax-nmin);//Sig
		values[fitbins]=aver;
		errors[fitbins]=avere;
		coords[fitbins]=coo;
		fitbins+=1;
	      }
	    }
	    profl[bin]=aver;
	    profle[bin]=avere;
	  }
	}//--->endof bin-loop
	cout<<" <--- start fit for Logs/Phys/Side"<<logs+1<<" "<<phys+1<<" "<<isd+1<<endl<<endl;
	HPAK(2624,profl);
        HPAKE(2624,profle);
        HPRINT(2624);
//
	if(fitbins>=10){
          argl[0]=0.;
          ier=0;
          MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
          if(ier!=0){
            cout<<"       MINIMIZE problem !"<<'\n';
            continue;
          }  
          MNEXCM(mfun,"MINOS",argl,0,ier,0);
          if(ier!=0){
            cout<<"       MINOS problem !"<<'\n';
            continue;
          }
          argl[0]=number(3);
          ier=0;
          MNEXCM(mfun,"CALL FCN",argl,1,ier,0);
          if(ier!=0){
            cout<<"      ECREUN-calib: final CALL_FCN problem !"<<'\n';
            continue;
          }
          cout<<"       -->>> fit done <<<--"<<endl<<endl;
	}
	else{
	  cout<<endl;
	  cout<<"  <-- Logs/Phys/Side"<<logs+1<<" "<<phys+1<<" "<<isd+1<<" Too Low nbins="<<fitbins<<endl;
	  continue; 
	}
//
        chi2=fitpars[4];
        if(ATREFFKEY.reprtf[0]>1)
	                         HF1(2593,geant(chi2),1.);
	slope=fitpars[0];
	if(chi2<30 && fabs(slope)>0.001 && fabs(slope)<0.04){
	  atlen[logs][phys][isd]=geant(1/slope);
	  norm=fitpars[1];
	  pe2ad[logs][phys][isd]=geant(norm/mev2pe/elos);
	  power[logs][phys][isd]=geant(fitpars[2]);
	  snor[logs][phys][isd]=geant(fitpars[3]);
          stat[logs][phys][isd]=0;//ok
          if(ATREFFKEY.reprtf[0]>1){
            HF1(2542+isd,fabs(atlen[logs][phys][isd]),1.);
            HF1(2544,pe2ad[logs][phys][isd],1.);
	  }
	}
	else{
	  cout<<"  <-- Logs/Phys/Side"<<logs+1<<" "<<phys+1<<" "<<isd+1<<" Bad chi2="<<chi2<<endl;
	  continue;   
	}
      }//--->endof side-loop
    }//--->endof phys.sect.index-loop
  }//--->endof Log.sect-loop
//------------------>save parameters:
  char fname[1024];
  char frdate[30];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  uinteger StartRun=AMSUser::JobFirstRunN();
  time_t StartTime=time_t(StartRun);
//
  strcpy(frdate,asctime(localtime(&StartTime)));//data date
//
//--> create name for output file
// 
  strcpy(fname,"AccVrpar");
  if(AMSJob::gethead()->isMCData()){
    strcat(fname,vers1);
    sprintf(fext,"%d",ATMCFFKEY.calvern+1);//MC-versn
  }
  else{
    strcat(fname,vers2);
    sprintf(fext,"%d",StartRun);//tempor RD-Run# = UTC-time of 1st "on-board" event
  }
  strcat(fname,".");
  strcat(fname,fext);
//
  ofstream acfile(fname,ios::out|ios::trunc);
  if(!acfile){
    cout<<"====> AccVarParsCalib:error opening file for output  "<<fname<<'\n';
    exit(8);
  }
  cout<<"      AccVarParsCalib: Open file for output, fname:  "<<fname<<'\n';
//
  for(logs=0;logs<MAXANTI;logs++){//<---readout(logical) sect. loop
// ----> write to file :   
    acfile.setf(ios::fixed);
//stat
    for(isd=0;isd<2;isd++){//sides loop
      acfile.width(1);
      acfile.precision(1);
      acfile << stat[logs][0][isd] <<" ";
      acfile.width(5);
      acfile.precision(1);
      acfile << pe2ad[logs][0][isd] <<" ";
//	   
      acfile.width(1);
      acfile.precision(1);
      acfile << stat[logs][1][isd] <<" ";
      acfile.width(5);
      acfile.precision(1);
      acfile << pe2ad[logs][1][isd] <<endl;
    }
//attlen
    for(isd=0;isd<2;isd++){//sides loop
      acfile.width(7);
      acfile.precision(1);
      acfile << atlen[logs][0][isd] <<" ";	   
      acfile << atlen[logs][1][isd] <<endl;
    }	   
//pow
    for(isd=0;isd<2;isd++){//sides loop
      acfile.width(5);
      acfile.precision(2);
      acfile << power[logs][0][isd] <<" ";	   
      acfile << power[logs][1][isd] <<endl;
    }	   
//snor
    for(isd=0;isd<2;isd++){//sides loop
      acfile.width(6);
      acfile.precision(3);
      acfile << snor[logs][0][isd] <<" ";	   
      acfile << snor[logs][1][isd] <<endl;	   
    }
//
    acfile << endl;	   
//
  }// -----> endof ReadoutSector loop
//  
    acfile << 12345 << endl;//endoffile label
    acfile << endl;
    acfile << endl<<"======================================================"<<endl;
    acfile << endl<<"      First run used for calibration is "<<StartRun<<endl;
    acfile << endl<<"      Date of the first event : "<<frdate<<endl;
    acfile.close();
    cout<<"      First run used for calibration is "<<StartRun<<endl;
    cout<<"      Date of the first event : "<<frdate<<endl;
    cout<<"<---- AntiVariableParamsCalib output file closed !"<<endl;
}
//--------
void AntiCalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff,fitval;
  f=0.;
//
  geant padl=ANTI2DBc::scleng()/2;//half z-size
  geant bndr=25;
//
  for(i=0;i<fitbins;i++){
    fitval=x[1]*exp(coords[i]*x[0]);
    if(fabs(coords[i])>bndr)fitval-=(x[1]*x[3]*pow((fabs(coords[i])-bndr)/(padl-bndr),x[2]));
    ff=(values[i]-fitval)/errors[i];
    f+=(ff*ff);
  }
  if(flg==3){
    f=f/number(fitbins-4);
    cout<<" <-- longit.fit done:"<<endl;
    cout<<"     chi2="<<f<<endl;
    for(i=0;i<4;i++){
      cout<<"     par-"<<i<<" ---> "<<x[i]<<endl;
      fitpars[i]=x[i];
    }
    fitpars[4]=f;
  }
}
//
//=============================================================================
//===============================> ANTPedCalib:
//
  number ANTPedCalib::adc[ANTI2C::MAXANTI][2];//store Anode/Dynode adc sum
  number ANTPedCalib::adc2[ANTI2C::MAXANTI][2];//store adc-squares sum
  number ANTPedCalib::adcm[ANTI2C::MAXANTI][2][ATPCSTMX];//max. adc-values stack
  number ANTPedCalib::port2r[ANTI2C::MAXANTI][2];//portion of hi-ampl to remove
  integer ANTPedCalib::nevt[ANTI2C::MAXANTI][2];// events in sum
  geant ANTPedCalib::peds[ANTI2C::MAXANTI][2];
  geant ANTPedCalib::dpeds[ANTI2C::MAXANTI][2];
  geant ANTPedCalib::thrs[ANTI2C::MAXANTI][2];
  geant ANTPedCalib::sigs[ANTI2C::MAXANTI][2];
  uinteger ANTPedCalib::stas[ANTI2C::MAXANTI][2];
  integer ANTPedCalib::nstacksz;//really needed stack size
  integer ANTPedCalib::hiamap[ANTI2C::MAXANTI];//high signal Paddles map (1 event) 
  time_t ANTPedCalib::BeginTime;
  uinteger ANTPedCalib::BeginRun;
  ANTPedCalib::ANTPedCal_ntpl ANTPedCalib::ANTPedCalNT;
//---------------------------------------------------------------------------
void ANTPedCalib::initb(){//called in retof2initjob() if TOF+AC is requested for OnBoard-calib data proc 
// histograms booking / reset vars
  integer i,j;
  char hmod[2]=" ";
//
  if(TFREFFKEY.reprtf[1]>0)cout<<endl;
//
//  ---> book hist.  :
//
  if((CALIB.SubDetInCalib/1000)%10>1){
    HBOOK1(2670,"Peds vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2670,50.);
    HMAXIM(2670,350.);
    HBOOK1(2671,"Ped-rms vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2671,0.);
    HMAXIM(2671,40.);
    HBOOK1(2672,"Ped-stat(1=bad) vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2672,0.);
    HMAXIM(2672,1.);
    HBOOK1(2673,"Ped-diff(old-new) vs paddle for bot/top sides",20,1.,21.,0.);
    HMINIM(2673,-5.);
    HMAXIM(2673,5.);
  }
//
// ---> clear arrays:
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      peds[i][j]=0;
      dpeds[i][j]=0;
      thrs[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
    }
  }
//
  if(TFREFFKEY.reprtf[1]>0)cout<<"<---- ACC OnBoardPedCalib: init done..."<<endl<<endl;;
}
//----
void ANTPedCalib::resetb(){ // run-by-run reset for OnBoardPedTable processing 
//called in buildonbP
  integer i,j;
  char hmod[2]=" ";
  static int first(0);
//
  cout<<endl;
//
  if((CALIB.SubDetInCalib/1000)%10>1){//reset hist
    for(i=0;i<4;i++)HRESET(2670+i,hmod);
  }
//
// ---> clear arrays:
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      peds[i][j]=0;
      dpeds[i][j]=0;
      thrs[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
    }
  }
  cout<<"====> ANTPedCalib::OnBoardPedTable: Reset done..."<<endl<<endl;;
}
//----
void ANTPedCalib::filltb(int sr, int sd, geant ped, geant dped, geant thr, geant sig){
// for usage with OnBoardPedTables
  peds[sr][sd]=ped;
  dpeds[sr][sd]=dped;
  thrs[sr][sd]=thr;
  sigs[sr][sd]=sig;
}
//-----
void ANTPedCalib::outptb(int flg){//called in buildonbP
// flg=1/2/3=>/write2DB/NoAction(hist only)/(write2file+hist)
   int i,sr,sd;
   int totch(0),goodtbch(0),goodch(0);
   geant pedo,sigo,pdiff;
   geant pedmn,pedmx,sigmn,sigmx;
   int stao;
   uinteger runn=BRun();//run# 
   time_t begin=time_t(runn);//begin time = run-time(=runn) 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   geant goodchp(0);
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer spatt=TFCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
   bool dpedin=((spatt&16)==1);//dyn.peds-section present(90 words)
   bool thrsin=((spatt&2)==1);//thresholds ..............(90...)
   geant rthrs,rdped;
//
   if(TFREFFKEY.reprtf[1]>0)cout<<endl<<"=====> ANTPedCalib:OnBoardTable-Report:"<<endl<<endl;
//---- fill ntuple:
//   ANTPedCalNT.Run=BRun();
//   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
//     ANTPedCalNT.Sector=sr+1;
//     for(sd=0;sd<2;sd++){
//       ANTPedCalNT.PedA[sd]=peds[sr][sd];
//       ANTPedCalNT.SigA[sd]=sigs[sr][sd];
//       ANTPedCalNT.StaA[sd]=stas[sr][sd];
//     }
//     HFNT(IOPA.ntuple);
//   }
//----
   pedmn=ATCAFFKEY.pedlim[0];
   pedmx=ATCAFFKEY.pedlim[1];
   sigmn=ATCAFFKEY.siglim[0];
   sigmx=ATCAFFKEY.siglim[1];
//
   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
     for(sd=0;sd<2;sd++){
       totch+=1;
       pedo=ANTIPeds::anscped[sr].apeda(sd);//extract prev.calib ped/sig/sta for comparison
       sigo=ANTIPeds::anscped[sr].asiga(sd);
       stao=ANTIPeds::anscped[sr].astaa(sd);
       pdiff=peds[sr][sd]-pedo;
       if(DAQS2Block::CalFirstSeq() || stao==1)pdiff=0;//to exclude pdiff-check for 1st run or bad prev.channel
//
       if(thrsin)rthrs=thrs[sr][sd];
       else rthrs=5;
       if(dpedin)rdped=dpeds[sr][sd];
       else rdped=50;
//
       if(peds[sr][sd]>0 && rthrs>1 && rdped>0){//channel OK in table ? 
	 goodtbch+=1;
	 if(sigs[sr][sd]>sigmn && sigs[sr][sd]<=sigmx &&
                   peds[sr][sd]>pedmn && peds[sr][sd]<=pedmx && fabs(pdiff)<30){//MyCriteria:chan.OK 
	   stas[sr][sd]=0;//ch ok
	   goodch+=1;
//update ped-object in memory(PedCal-Obj):
	   ANTIPeds::anscped[sr].apeda(sd)=peds[sr][sd];
	   ANTIPeds::anscped[sr].asiga(sd)=sigs[sr][sd];
	   ANTIPeds::anscped[sr].astaa(sd)=stas[sr][sd];
	   if(flg>1){
             HF1(2670,geant(sd*10+sr+1),ANTIPeds::anscped[sr].apeda(sd));
	     HF1(2671,geant(sd*10+sr+1),ANTIPeds::anscped[sr].asiga(sd));
	     HF1(2672,geant(sd*10+sr+1),geant(stas[sr][sd]));
	     HF1(2673,geant(sd*10+sr+1),pdiff);
	   }
	 }
	 else{//MyCriteria: bad chan
	   if(TFREFFKEY.reprtf[1]>0){
	     cout<<"       MyCriteriaBadCh: Sector/Side="<<sr<<" "<<sd<<endl;
	     cout<<"                      ped/sig/sta="<<peds[sr][sd]<<" "<<sigs[sr][sd]<<" "<<stas[sr][sd]<<endl;    
	     cout<<"                      PedDiff="<<pdiff<<endl;
	   }    
	 }
       }//--->endof "channel OK in table ?" check
       else{
	 if(TFREFFKEY.reprtf[1]>0){
	   cout<<"       BadTableChan:Sector/Side="<<sr<<" "<<sd<<endl;    
	   cout<<"       ped/sig/sta="<<peds[sr][sd]<<" "<<sigs[sr][sd]<<" "<<stas[sr][sd]<<endl;
	 }    
       }
     }//--->endof side-loop
   }//--->endof sector-loop
// 
    goodchp=geant(goodch)/totch;
//
   if(TFREFFKEY.reprtf[1]>0)
     cout<<"       GoodChan(Table/My)="<<goodtbch<<" "<<goodch<<" from total="<<totch<<" GoodChsPort="<<goodchp<<endl;  
//   
// ---> prepare update of DB :
   if(goodchp>=0.5 && flg==1){//Update DB "on flight"
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Antipeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();//redefine according to VC.
     ptdv->SetTime(insert,min(AMSEvent::gethead()->getrun()-1,uinteger(AMSEvent::gethead()->gettime())),AMSEvent::gethead()->getrun()-1+86400*30);
     cout <<"      <--- AccOnBoardPeds DB-info has been updated for "<<*ptdv<<endl;
     ptdv->gettime(insert,begin,end);
     cout<<"           Time ins/beg/end: "<<endl;
     cout<<"           "<<ctime(&insert);
     cout<<"           "<<ctime(&begin);
     cout<<"           "<<ctime(&end);
//
//     if(AMSFFKEY.Update==2 ){
//       AMSTimeID * offspring = 
//         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
//       while(offspring){
//         if(offspring->UpdateMe())cout << "       Start update ANT-peds DB "<<*offspring; 
//         if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
//         cerr <<"       Problem To Update ANT-peds in DB"<<*offspring;
//         offspring=(AMSTimeID*)offspring->next();//get one-by-one
//       }
//     }
   }
   else{
     if(goodchp<0.5)cout<<"      <--- GoodCh% is too low("<<goodchp<<") - No DB-writing !"<<endl;
   }
//
// ---> write OnBoardPedTable to ped-file:
//
   if(flg==3 && AMSFFKEY.Update==0){
     integer endflab(12345);
     char fname[1024];
     char name[80];
     char buf[20];
//
     strcpy(name,"antp_tb_rl.");//from OnBoardTable
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new ONBT-Peds file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write peds/sigmas/stat:
//
     for(sr=0;sr<ANTI2C::MAXANTI;sr++){   // <-------- loop over layers
       icfile.width(2);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << stas[sr][sd]<<" ";//stat
       icfile << " ";
       icfile.width(6);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << peds[sr][sd]<<" ";//ped
       icfile << " ";
       icfile.width(5);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << sigs[sr][sd]<<" ";//sig
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   if(flg>1 && AMSFFKEY.Update==0){
     for(i=0;i<4;i++)HPRINT(2670+i);
   }
   cout<<endl;
   cout<<"<========= AccOnbPedCalib: run "<<runn<<" is processed, goodpeds%="<<goodchp<<endl;
   cout<<endl;
//
}
//--------------------------------------------------------------------
void ANTPedCalib::init(){ // ----> initialization for AccPed-calibration(Class/DS) 
  integer i,j,k,il,ib,id,ii,jj,chan;
  char htit1[60],htit2[60];
  char inum[11];
  char in[2]="0";
  geant por2rem;
//
  strcpy(inum,"0123456789");
//
  cout<<endl;
/*
   if(ATREFFKEY.relogic==4){//open Ntuple file (for OnBoardTable only for the moment)
     char hfile[161];
     UHTOC(IOPA.hfile,40,hfile,160);  
     char filename[256];
     strcpy(filename,hfile);
     integer iostat;
     integer rsize=1024;
     char event[80];  
     HROPEN(IOPA.hlun+1,"antpedsig",filename,"NP",rsize,iostat);
     if(iostat){
       cerr << "<==== ANTPedCalib::init: Error opening antpedsig ntuple file "<<filename<<endl;
       exit(1);
     }
     else cout <<"====> ANTPedCalib::init: Ntuple file "<<filename<<" opened..."<<endl;
     HBNT(IOPA.ntuple,"AntPedSigmas"," ");
    
     HBNAME(IOPA.ntuple,"ANTPedSig",(int*)(&ANTPedCalNT),"Run:I,Sector:I,PedA(2):R,SigA(2):R,StaA(2):I");
     return;
   }
*/
  if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//ClassPed(random)
  else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//DownScaled(in trigger)
//  nstacksz=integer(floor(por2rem*ATPCEVMX+0.5));
  nstacksz=ATPCSTMX;
  if(nstacksz>ATPCSTMX){
    cout<<"ANTPedCalib::init-W- Stack size too small, change truncate-value or max.events/ch !!!"<<nstacksz<<endl;
    cout<<"                Its size set back to max-possible:"<<ATPCSTMX<<endl;
    nstacksz=ATPCSTMX;
  }
  if(nstacksz<1)nstacksz=1;
  cout<<"ANTPedCalib::init: real stack-size="<<nstacksz<<endl;
//
//  ---> book hist.  :
//
if(ATREFFKEY.reprtf[0]>1){
  HBOOK1(2670,"Peds vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2670,75.);
  HMAXIM(2670,275.);
  HBOOK1(2671,"Ped-rms vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2671,0.);
  HMAXIM(2671,8.);
  HBOOK1(2672,"Ped-stat(1=bad) vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2672,0.);
  HMAXIM(2672,1.);
  HBOOK1(2673,"Ped-diff(old-new) vs paddle for bot/top sides",20,1.,21.,0.);
  HMINIM(2673,-5.);
  HMAXIM(2673,5.);
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      strcpy(htit1,"AccPedCalib:Raw ADCs(when accepted) for Sector/Side=");
      strcpy(htit2,"AccPedCalib:Raw ADCs(raw) for Sector/Side=");
      in[0]=inum[i+1];
      strcat(htit1,in);
      strcat(htit2,in);
      in[0]=inum[j+1];
      strcat(htit1,in);
      strcat(htit2,in);
      id=2674+i*2+j;
      HBOOK1(id,htit1,80,80.,240.,0.);
      HBOOK1(id+16,htit2,80,80.,240.,0.);
    }
  }
}
  //
// ---> clear arrays:
//
  for(i=0;i<ANTI2C::MAXANTI;i++){
    for(j=0;j<2;j++){
      nevt[i][j]=0;
      adc[i][j]=0;
      adc2[i][j]=0;
      peds[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
      port2r[i][j]=0;
      for(k=0;k<ATPCSTMX;k++)adcm[i][j][k]=0;
    }
  }
  cout<<"ANTPedCalib::init done..."<<endl<<endl;;
}
//-------------------------------------------
void ANTPedCalib::fill(int sr, int sd, geant val){//
   int i,ist,nev,evs2rem,srl,srr,id;
   geant lohil[2]={0,9999};//means no limits on val, if partial ped is bad
   geant ped,sig,sig2,spikethr;
   bool accept(true);
   geant por2rem,p2r;
   geant pedmn,pedmx,sigmn,sigmx;
   geant apor2rm[10]={0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,};
   number ad,ad2,dp,ds;
   geant pedi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   geant sigi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
//
   if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//def ClassPed(random)
   else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//def DownScaled(in trigger)
   sigmn=ATCAFFKEY.siglim[0];
   sigmx=ATCAFFKEY.siglim[1];
//
//cout<<"--->In ANTPedCalib::fill: sect/sid="<<sr<<" "<<sd<<" val="<<val<<endl;
   nev=nevt[sr][sd];
// peds[sr][sd];//SmalSample(SS) ped (set to "0" at init)
   if(peds[sr][sd]==0 && nev==ATPCEVMN){//calc. SubSet-ped/sig when TFPCEVMN events is collected
//
//     cout<<"<----- start SS-peds calculation for Sec/Sid="<<sr+1<<" "<<sd+1<<endl;
//     for(i=0;i<nstacksz;i++){
//       cout<<adcm[sr][sd][i]<<" ";
//       if((i+1)%10==0)cout<<endl;
//     }
//     cout<<endl;
     int llindx(-1);   
     for(int ip2r=0;ip2r<10;ip2r++){//<--- portion-to-remove loop
       p2r=apor2rm[ip2r];
       evs2rem=int(floor(p2r*nev+0.5));
       if(evs2rem>nstacksz)evs2rem=nstacksz;
       ad=adc[sr][sd];
       ad2=adc2[sr][sd];
       for(i=0;i<evs2rem;i++){//remove "evs2rem" highest amplitudes
         ad=ad-adcm[sr][sd][i];
         ad2=ad2-adcm[sr][sd][i]*adcm[sr][sd][i];
       }
       ped=ad/number(nev-evs2rem);//truncated average
       sig2=ad2/number(nev-evs2rem);
       sig2=sig2-ped*ped;// truncated rms**2
       if(sig2>0)sig=sqrt(sig2);
       else sig=0;
       if(ip2r>0){
         dp=pedi[ip2r-1]-ped;
         ds=sigi[ip2r-1]-sig;
       }
       else{
         dp=9999;
         ds=9999;
       }
       pedi[ip2r]=ped;
       sigi[ip2r]=sig;
//       cout<<"  ip2r/p2r="<<ip2r<<" "<<p2r<<" p/s="<<ped<<" "<<sig<<" dp/ds="<<dp<<" "<<ds<<" s2="<<sig2<<endl;
       if((sig < sigmx && sig>sigmn)
                                      && (dp < 1.)
                                                   && (ds < 0.5)
		                                                && ip2r > 0){
         port2r[sr][sd]=p2r;
         llindx=ip2r;
	 break;
       }
     }//--->endof portion to remove loop
//
     if(llindx<0){//fail to find SubSet-ped/sig - suspicious channel
       sig=0;
       ped=0;
       port2r[sr][sd]=-1;
     }
     sigs[sr][sd]=sig;
     peds[sr][sd]=ped;//is used now as flag that SS-PedS ok
     adc[sr][sd]=0;//reset to start new life(with real selection limits)
     adc2[sr][sd]=0;
     nevt[sr][sd]=0;
     for(i=0;i<ATPCSTMX;i++)adcm[sr][sd][i]=0;
//     cout<<"PartialPed:sr/sd="<<sr<<" "<<sd<<endl;
//     cout<<"           ped/sig2="<<ped<<" "<<sig2<<endl;
//     cout<<"           evs2rem="<<evs2rem<<endl;
   }//--->endof SS-peds calculations
//
   ped=peds[sr][sd];//now !=0 or =0 
   sig=sigs[sr][sd];
//
   if(ped>0){//set val-limits if partial ped OK
     lohil[0]=ped-4*sig;
     lohil[1]=ped+6*sig;
     spikethr=max(6*sig,ATPCSPIK);
     if(val>(ped+spikethr)){//spike(>~1mips in higain chan)
       hiamap[sr]=1;//put it into map
       accept=false;//mark as bad for filling
     }
     else{//candidate for "fill" - check neigbours
       if(sr>0)srl=sr-1;
       else srl=0;
       if(sr < (ANTI2C::MAXANTI-1))srr=sr+1;
       else srr=ANTI2C::MAXANTI-1;
       accept=(hiamap[srl]==0 && hiamap[srr]==0);//not accept if there is any neighbour
     }
   }
//
//   accept=true;//tempor to switch-off spike algorithm
//
   if(val>lohil[0] && val<lohil[1] && accept){//check "in_limits/not_spike"
     if(nev<ATPCEVMX){//limit statistics(to keep max-stack size small)
       adc[sr][sd]+=val;
       adc2[sr][sd]+=(val*val);
       nevt[sr][sd]+=1;
       for(ist=0;ist<nstacksz;ist++){//try to position val in stack of nstacksz highest max-values
          if(val>adcm[sr][sd][ist]){
	    for(i=nstacksz-1;i>ist;i--)adcm[sr][sd][i]=adcm[sr][sd][i-1];//move stack -->
	    adcm[sr][sd][ist]=val;//store max.val
	    break;
	  }
       }
     }
   }//-->endof "in limits" check
//
   if(ATREFFKEY.reprtf[0]>1){
     id=2674+sr*2+sd;
     if(ped>0 && val>lohil[0] && val<lohil[1] && accept)HF1(id,val,1.); 
     HF1(id+16,val,1.);
   }
}
//-------------------------------------------
void ANTPedCalib::outp(int flg){// very preliminary
// flg=0/1/2=>HistOnly/write2DB+file/write2file
   int i,sr,sd,statmin(9999);
   geant pdiff,por2rem,p2r;
   geant pedmn,pedmx,sigmn,sigmx;
   uinteger runn=AMSUser::JobFirstRunN();//job 1st run# 
   time_t begin=time_t(runn);//begin time => runn
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   int totchs(0),goodchs(0);
   geant goodchp(0);
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer evs2rem;
   if(ATREFFKEY.relogic==2)por2rem=ATCAFFKEY.pedcpr[0];//def ClassPed(random)
   else if(ATREFFKEY.relogic==3)por2rem=ATCAFFKEY.pedcpr[1];//def DownScaled(in trigger)
   pedmn=ATCAFFKEY.pedlim[0];
   pedmx=ATCAFFKEY.pedlim[1];
   sigmn=ATCAFFKEY.siglim[0];
   sigmx=ATCAFFKEY.siglim[1];
//
   cout<<endl;
   cout<<"=====> ANTPedCalib-Report:"<<endl<<endl;
   for(sr=0;sr<ANTI2C::MAXANTI;sr++){
     for(sd=0;sd<2;sd++){
       totchs+=1;
       if(port2r[sr][sd]<0)p2r=por2rem;//use default for suspicious channel
       else p2r=port2r[sr][sd]/5;//use reduced value because of the ped+-n*sig limits
       if(nevt[sr][sd]>=ATPCEVMN){//statistics ok
	 evs2rem=integer(floor(p2r*nevt[sr][sd]+0.5));
	 if(evs2rem>nstacksz)evs2rem=nstacksz;
         if(evs2rem<1)evs2rem=1;
	 for(i=0;i<evs2rem;i++){//remove highest amplitudes
	   adc[sr][sd]=adc[sr][sd]-adcm[sr][sd][i];
	   adc2[sr][sd]=adc2[sr][sd]-adcm[sr][sd][i]*adcm[sr][sd][i];
	 }
	 adc[sr][sd]/=number(nevt[sr][sd]-evs2rem);//truncated average
	 adc2[sr][sd]/=number(nevt[sr][sd]-evs2rem);
	 adc2[sr][sd]=adc2[sr][sd]-adc[sr][sd]*adc[sr][sd];//truncated rms**2
	 if(adc2[sr][sd]>(sigmn*sigmn) && adc2[sr][sd]<=(sigmx*sigmx)
		               && adc[sr][sd]>pedmn && adc[sr][sd]<=pedmx){//chan.OK
	   peds[sr][sd]=geant(adc[sr][sd]);
	   sigs[sr][sd]=geant(sqrt(adc2[sr][sd]));
	   stas[sr][sd]=0;//ok
	   goodchs+=1;
//update ped-object in memory:
	   pdiff=peds[sr][sd]-ANTIPeds::anscped[sr].apeda(sd);
	   ANTIPeds::anscped[sr].apeda(sd)=peds[sr][sd];
	   ANTIPeds::anscped[sr].asiga(sd)=sigs[sr][sd];
	   ANTIPeds::anscped[sr].astaa(sd)=stas[sr][sd];
           if(ATREFFKEY.reprtf[0]>1){
             HF1(2670,geant(sd*10+sr+1),ANTIPeds::anscped[sr].apeda(sd));
	     HF1(2671,geant(sd*10+sr+1),ANTIPeds::anscped[sr].asiga(sd));
	     HF1(2672,geant(sd*10+sr+1),geant(stas[sr][sd]));
	     HF1(2673,geant(sd*10+sr+1),pdiff);
	   }
	   if(statmin>nevt[sr][sd])statmin=nevt[sr][sd];
	 }
	 else{//bad chan
	   cout<<"      BadCh=Sector/Side="<<sr<<" "<<sd<<endl;
	   cout<<"                      Nevents="<<nevt[sr][sd]<<endl;    
	   cout<<"                      ped/sig**2="<<adc[sr][sd]<<" "<<adc2[sr][sd]<<endl;    
	 }
       }//--->endof "good statistics" check
       else{
	 if(nevt[sr][sd]>0)cout<<"      LowStatCh=Sector/Side="<<sr<<" "<<sd<<" Nevents="<<nevt[sr][sd]<<endl;    
       }
     }//--->endof side-loop
   }//--->endof sector-loop
   goodchp=geant(goodchs)/totchs;
   cout<<"      MinAcceptableStatistics/channel was:"<<statmin<<"  GoodChsPort="<<goodchp<<endl; 
//   
// ---> prepare update of DB :
   if(flg==1 && goodchp>=0.5){
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Antipeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
   }
   else{
     if(flg==1 && goodchp<0.5)cout<<" <-- Too small GoodChsPortion - block automatic writing to DB !"<<endl;
   }
// ---> write MC/RD ped-file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[1024];
     char name[80];
     char buf[20];
//
     if(ATREFFKEY.relogic==2)strcpy(name,"antp_cl");//classic(all_events rundom trig)
     if(ATREFFKEY.relogic==3)strcpy(name,"antp_ds");//down_scaled events
     if(AMSJob::gethead()->isMCData())           // for MC-event
     {
       cout <<"      new MC peds-file will be written..."<<endl;
       strcat(name,"_mc.");
     }
     else                                       // for Real events
     {
       cout <<"      new RD peds-file will be written..."<<endl;
       strcat(name,"_rl.");
     }
     sprintf(buf,"%d",runn);
     strcat(name,buf);
//     if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//     if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
     strcpy(fname,"");
     strcat(fname,name);
     cout<<"Open file : "<<fname<<'\n';
     cout<<" Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new pedestals-file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write Anodes peds/sigmas/stat:
//
     for(sr=0;sr<ANTI2C::MAXANTI;sr++){   // <-------- loop over layers
       icfile.width(2);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << stas[sr][sd]<<" ";//stat
       icfile << " ";
       icfile.width(6);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << peds[sr][sd]<<" ";//ped
       icfile << " ";
       icfile.width(5);
       icfile.precision(1);
       for(sd=0;sd<2;sd++)icfile << sigs[sr][sd]<<" ";//sig
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   if(ATREFFKEY.reprtf[0]>1){
     for(i=0;i<36;i++)HPRINT(2670+i);
   }
   cout<<endl;
   cout<<"====================== ANTPedCalib: job is completed ! ======================"<<endl;
   cout<<endl;
//
}
//-------------------------------------------
//--------------
void ANTPedCalib::ntuple_close(){
//
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//antpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("antpedsig");
  CLOSEF(IOPA.hlun+1);
//
}
//
