//  $Id$
// v0.0 28.09.1999 by E.Choumilov
// v1.1 22.04.2008 by E.Choumilov, Ecal1DCluster bad ch. treatment corrected by V.Choutko.
//
//
#include <stdio.h>
#include "typedefs.h"
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "point.h"
#include "amsgobj.h"
#include "event.h"
#include "amsstl.h"
#include "commons.h"
#include "ntuple.h"
#include "tofsim02.h"
#include "ecaldbc.h"
#include "ecalrec.h"
#include "ecalcalib.h"
#include "mccluster.h"
#include "trigger102.h"
#include "trigger302.h"
#include "timeid.h"
#include <fenv.h>
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TMinuit.h"
#ifdef __MIC__
#define __DARWIN__
#endif

using namespace std;
using namespace ecalconst;
//
uinteger AMSEcalRawEvent::trigfl=0;// just memory reservation/initialization for static
uinteger AMSEcalRawEvent::trigconf=0;// just memory reservation/initialization for static
number AMSEcalRawEvent::trigtm=0.;// just memory reservation/initialization for static
geant AMSEcalRawEvent::trsum=0.;// just memory reservation/initialization for static
geant AMSEcalRawEvent::dynadc[ECSLMX][ECPMSMX];
int16u AMSEcalRawEvent::trpatt[6][3];
uinteger AMSEcalRawEvent::StartRun(0);
time_t AMSEcalRawEvent::StartTime(0);
//----------------------------------------------------
void AMSEcalRawEvent::validate(int &stat){ //Check/correct RawEvent-structure
  int i,k;
  integer sta,id,idd,isl,pmt,subc,pmsl;
  number radc[2]; 
  geant padc[3];
  integer swid;
  geant pedh[4],pedl[4],sigh[4],sigl[4],ph,pl;
  integer ovfl[2];
  AMSEcalRawEvent * ptr;
  integer ecalflg(0);
  Trigger2LVL1 *ptrt;
  bool ecalftok(false);
  number asum4[ECPMSL],pixrg;
  integer npmx=ECALDBc::slstruc(4);//numb.of PM's/Sl(36)
  integer nslmx=ECALDBc::slstruc(3);//numb.of Slayers(9)
  //
  stat=1;//bad
  bool nopedsubt(false);
  integer nhitst(0);
  //
  ptrt=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptrt){
    EcalJobStat::addre(7);
    ecalftok=ptrt->EcalFasTrigOK();//JMembPatt's FTE-bit(Lev1-obj; MC:JMembPat EC-bits created by GlobFTpatt&ecprjconf)
    if(ecalftok)EcalJobStat::addre(8);
    ecalflg=ptrt->getecflag();//ecflag of Lev1-obj(RD:created using JMembPatt, MC:in simu)
    if(ecalflg>0){
      EcalJobStat::addre(9);
      if(ECREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
	{
	  HF1(ecalconst::ECHISTR+30,geant(ecalflg),1.);
	}
      }
    }
  }
  //
  if(ECREFFKEY.reprtf[0]>1){
    if(ptrt){
      if(ecalftok){
	for(int sl=0;sl<6;sl+=2){
	  for(int pm=0;pm<36;pm++){
	    k=floor(geant(sl)/2);
	    if(ptrt->EcalDynON(sl,pm)){
#pragma omp critical (hf1)
	      {
		HF1(ecalconst::ECHISTR+28,geant(pm+1+40*k),1.);
	      }
	    }
	  }
	}
	for(int sl=1;sl<6;sl+=2){
	  for(int pm=0;pm<36;pm++){
	    k=floor(geant(sl)/2);
	    if(ptrt->EcalDynON(sl,pm)){
#pragma omp critical (hf1)
	      {
		HF1(ecalconst::ECHISTR+29,geant(pm+1+40*k),1.);
	      }
	    }
	  }
	}
      }
    }
  }
  //
  if((ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5)){
    ECPedCalib::hiamreset();//hiamap[][] is threadprivate (1-event map)
  }  
  //
  for(int nc=0;nc<AMSECIds::ncrates();nc++){ // <-------------- crate(container) loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
      getheadC("AMSEcalRawEvent",nc,0);
    while(ptr){ // <--- RawEvent-hits loop in crate(container):
      nhitst+=1;
      isl=ptr->getslay();//0->...
      id=ptr->getid();//LTTP
      idd=id/10;//LTT
      subc=id%10-1;//Pixel(0-3)
      pmt=idd%100-1;//pmTube(0-...)
      ptr->getpadc(padc);
      padc[2]=AMSEcalRawEvent::getadcd(isl,pmt);//extract Dynode
      ptr->setadcd(padc[2]);//add Dyn-info(common for 4 pixels) in pixel-object
      sta=ptr->getstatus();
      if(sta){//in given ah/al/d peds were not subtracted, it means presence of raw format for calib
	nopedsubt=true;
	for(i=0;i<2;i++){//hi/low
	  swid=id*10+(i+1);//long swid=LTTPG for anodes (P=1-4,G=1/2->hi/low)
#pragma omp critical (ecval_pedc)
	  ECPedCalib::fill(swid,padc[i]);//call fill-routine of PedCalib(DS) for anodes(pix=1-4)
	}
	if(subc==0){//call fill for Dynode only once per 4 pixels(here when subc=0) because all subc "on")
	  swid=idd*100+5*10+1;//long swid=LTTPG for Dynode (P=5,G=1)
#pragma omp critical (ecval_pedc)
	  ECPedCalib::fill(swid,padc[2]);//call fill-routine of PedCalib(DS) for dynode
	}
      }//--->endof PedCal check
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop in crate
    //
  } // ---> end of crate-loop
  //
  if(nopedsubt && (ECREFFKEY.relogic[1]==4 || ECREFFKEY.relogic[1]==5)){
    EcalJobStat::addre(20);//count pedcal-events
    return;//PedCal exit with stat=1(bad) to bypass next reco-stages !!!
  }
  else if(nopedsubt){
    cerr<<"<=== AMSEcalRawEvent::validate:-E- Found not PedSubtracted Data while not PedCalJob !!"<<endl;
    return;
  }
  //
  if(nhitst==0)return;//return with stat=1(bad - empty detector)
  //
  //----> fill arrays for Hi2Low-ratio calc.(in REUN-calibration):
  //
  if(ECREFFKEY.relogic[1]<=2){// if REUN-calibration
    for(i=0;i<ECPMSL;i++)asum4[i]=0;
    for(int nc=0;nc<AMSECIds::ncrates();nc++){ // <-------------- crate(container) loop
      ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
	getheadC("AMSEcalRawEvent",nc,0);
      while(ptr){ // <--- RawEvent-hits loop in crate:
        isl=ptr->getslay();//0,...
        id=ptr->getid();//LTTP
        idd=id/10;
        subc=id%10-1;//Pixel(0-3)
        pmt=idd%100-1;//pmTube(0-...)
        pmsl=pmt+ECPMSMX*isl;//sequential numbering of PM's over all superlayers
        ptr->getpadc(padc);
	sta=ptr->getstatus();
        pixrg=ECcalib::ecpmcal[isl][pmt].pmscgain(subc)*
	  ECcalib::ecpmcal[isl][pmt].pmrgain();// pix gain corr(really 1/pmscg) from DB
        ECPMPeds::pmpeds[isl][pmt].getpedh(pedh);
        ECPMPeds::pmpeds[isl][pmt].getsigh(sigh);
        ECPMPeds::pmpeds[isl][pmt].getpedl(pedl);
        ECPMPeds::pmpeds[isl][pmt].getsigl(sigl);
        radc[0]=number(padc[0]);//ADC-high-chain
        radc[1]=number(padc[1]);//ADC-low-chain
        ph=pedh[subc];
        pl=pedl[subc];
	if(AMSJob::gethead()->isSimulation()){
          if(ECMCFFKEY.silogic[0]==2){
            ph=0.;
	    pl=0.;
          }
	}
        ovfl[0]=0;
        ovfl[1]=0;
        if(radc[0]>0)if((ECADCMX[0]-(radc[0]+ph))<=1)ovfl[0]=1;// mark as ADC-Overflow(hi)
        if(radc[1]>0)if((ECADCMX[1]-(radc[1]+pl))<=1)ovfl[1]=1;// mark as ADC-Overflow(low)
        if(radc[0]>(33.*3.*ECCAFFKEY.adcmin) && ovfl[0]==0 && radc[1]>ECCAFFKEY.adcmin){
#pragma omp critical (ecval_fill2)
          ECREUNcalib::fill_2(isl,pmt,subc,radc);//<--fill
        }
	if(radc[0]>ECCAFFKEY.adcmin && ovfl[0]==0)asum4[pmsl]+=(radc[0]*pixrg);//sum4-Gcorrected(for a2d ratio) 
        ptr=ptr->next();  
      } // ---> end of RawEvent-hits loop in crate
      //
    } // ---> end of crates loop
    //--> fill a2d arrays:
    for(isl=0;isl<nslmx;isl++){
      for(pmt=0;pmt<npmx;pmt++){
        pmsl=pmt+ECPMSMX*isl;//sequential numbering of PM's over all superlayers
        padc[2]=AMSEcalRawEvent::getadcd(isl,pmt);//Dynode
	if(padc[2]>1.5*ECCAFFKEY.adcmin && asum4[pmsl]>30*ECCAFFKEY.adcmin){
#pragma omp critical (ecval_fill3)
	  ECREUNcalib::fill_3(pmsl,padc[2],asum4[pmsl]);
	}
      }
    }
  } // ---> endif of REUN-calib
  //
  stat=0;//ok
}
//----------------------------------------------------
void AMSEcalRawEvent::mc_build(int &stat){

  int j,k,ic;
  integer fid,cid,cidar[4],nhits,nraw,nrawd,il,pm,sc,proj,rdir,nslhits;
  number x,y,coo,hflen=0,pmdis,edep,edepr,edept,edeprt,emeast,time,timet(0.);
  number attfdir,attfrfl,ww[4],anen,dyen;
  number attfdir0,attfrfl0;
  geant attf,attf0;
  number sum[ECPMSMX][4],pmtmap[ECSLMX][ECPMSMX],pmlprof[ECSLMX];
  int dytrc[ECSLMX];
  int dycog[ECSLMX];//to work with integer as in real EC trigger
  number pmedepr[4];
  integer adch,adcl,adcd;
  geant radc,a2dr,h2lr,mev2adc,ares,phel,pmrgn,scgn;
  geant lfs,lsl,ffr;
  geant pedh[4],pedl[4],sigh[4],sigl[4],pedd,sigd;
  AMSEcalMCHit * ptr;
  integer id,sta,nslmx,npmmx;
  geant adc[2];
  number dyresp,dyrespt;// dynode resp. in mev(~mV) (for trigger)
  number an4resp,an4respt,an4qin;// (for trigger)
  number qin,saturf,a4maxq(0);
  integer adchmx,adclmx,adcdmx;
  float Tsim,Tref,deltaT,Tcorr;
  float mevtot;
  //
  nslmx=ECALDBc::slstruc(3);
  npmmx=ECALDBc::slstruc(4);
  stat=1;//bad
  edept=0.;
  edeprt=0.;
  emeast=0.;
  nhits=0;
  nraw=0;
  nrawd=0;
  an4respt=0;
  dyrespt=0;
  timet=0.;
  a4maxq=0.;
  adchmx=0;
  adclmx=0;
  adcdmx=0;
  for(il=0;il<ECSLMX;il++){
    pmlprof[il]=0.;
    dytrc[il]=0;
    dycog[il]=0;
    for(int ipm=0;ipm<ECPMSMX;ipm++){
      pmtmap[il][ipm]=0.;
    }
  }
  mevtot=0.;
  //
  for(il=0;il<nslmx;il++){ // <-------------- super-layer loop
    ptr=(AMSEcalMCHit*)AMSEvent::gethead()->
      getheadC("AMSEcalMCHit",il,0);   
    for(int ipm=0;ipm<npmmx;ipm++)
      for(k=0;k<4;k++)sum[ipm][k]=0.; 
    nslhits=0;
    while(ptr){ // <------------------- geant-hits loop in superlayer:
      nhits+=1;
      nslhits+=1;
      fid=ptr->getid();//SSLLFFF
      edep=ptr->getedep()*1000;// MeV(dE/dX)
      //      if(il==0 && (fid%100000)/1000>=1 &&
      //                    (fid%100000)/1000<=5 && (fid%1000)==254)edep=0.;//tempor test(in pl/sc=1/38 )
      edept+=edep;
      if(edep<=0){
        cout<<"======>ECEdep<0::il="<<il<<" edep="<<edep<<endl;
      }
      EcalJobStat::addzprmc1(il,edep);//geant SL-profile
      time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
      timet+=edep*time;
      x=ptr->getcoo(0);// global coord.
      y=ptr->getcoo(1);
      x=x-ECALDBc::gendim(5);// go to local (ECAL-radiator) system
      y=y-ECALDBc::gendim(6);
      proj=(1-2*(il%2))*(2*ECALDBc::slstruc(1)-1);//proj=+1/-1=>Y/X
      if(proj>0){ // <-- fiber from Y-proj
        coo=x;// get X-coord(along fiber)
        hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber length in X-dir
      }
      else{                                          //<-- fiber from X-proj
        coo=y;// get Y-coord(along fiber)
        hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber length in Y-dir
      }
      //
      for(k=0;k<4;k++){
        cidar[k]=0;
	ww[k]=0.;
      }
      ECALDBc::fid2cida(fid,cidar,ww);//cidar=SSPPC
      //
      for(j=0;j<4;j++){ // <-- loop over coupled PM's
        cid=cidar[j];
        if(cid>0){
          sc=cid%10-1; // SubCell(pixel)
          pm=(cid/10)%100-1; // PM
	  //
          if(proj>0)rdir=(1-2*(pm%2))*ECALDBc::slstruc(6);//+-1 readout dir(along pos/neg X)
          else rdir=(1-2*(pm%2))*ECALDBc::slstruc(5);//+-1 readout dir(along pos/neg Y)
	  //
          pmdis=coo+hflen;//to count from "-" edge of fiber (0-2*hflen)
          if(rdir<0)pmdis=2.*hflen-pmdis;
	  lfs=ECcalib::ecpmcal[il][pm].alfast();//att_len(fast comp) from DB(MC-Seeds)
	  lsl=ECcalib::ecpmcal[il][pm].alslow();//att_len(slow comp) from DB(MC-Seeds)
	  ffr=ECcalib::ecpmcal[il][pm].fastfr();//fast comp. fraction from DB(MC-Seeds)
          attfdir=(1.-ffr)*exp(-pmdis/lsl)+ffr*exp(-pmdis/lfs);//fiber att.factor(direct light)
	  attfrfl=((1-ffr)*exp(-(2*hflen-pmdis)/lsl)+ffr*exp(-(2*hflen-pmdis)/lfs))
	    *((1-ffr)*exp(-2*hflen/lsl)+ffr*exp(-2*hflen/lfs))*ECMCFFKEY.fendrf;//(refl)
          attf=0.5*(attfdir+attfrfl);
          edepr=edep*attf*ww[j];//geant dE/dX(mev) + Attenuation for direct/refl light
          sum[pm][sc]+=edepr;
          edeprt+=edepr;
	}
      }// -----> end of the coupled PM's loop
      //
      ptr=ptr->next(); 
    } // ---------------> end of geant-hit-loop in superlayer
    //
    //
    for(int ipm=0;ipm<npmmx;ipm++){ // <------- loop over PM's in this(il) S-layer
      a2dr=ECcalib::ecpmcal[il][ipm].an2dyr();// anode/dynode gains ratio from DB(MC-Seeds)
      mev2adc=ECMCFFKEY.mev2adc;// MC Emeas->ADCchannel to have MIP-m.p. in 10th channel
      //                (only mev2mev*mev2adc has real meaning providing Geant_dE/dX->ADCchannel)
      pmrgn=ECcalib::ecpmcal[il][ipm].pmrgain();// PM gain(wrt ref. one) from DB(MC-Seeds)
      ECPMPeds::pmpeds[il][ipm].getpedh(pedh);//No PedSeeds used due to very small real ped sigmas !!!
      ECPMPeds::pmpeds[il][ipm].getsigh(sigh);
      ECPMPeds::pmpeds[il][ipm].getpedl(pedl);
      ECPMPeds::pmpeds[il][ipm].getsigl(sigl);
      pedd=ECPMPeds::pmpeds[il][ipm].pedd();
      sigd=ECPMPeds::pmpeds[il][ipm].sigd();
      //---  
      lfs=ECcalib::ecpmcal[il][ipm].alfast();//att_len(fast comp) from DB(MC-Seeds)
      lsl=ECcalib::ecpmcal[il][ipm].alslow();//att_len(slow comp) from DB(MC-Seeds)
      ffr=ECcalib::ecpmcal[il][ipm].fastfr();//fast comp. fraction from DB(MC-Seeds)
      attfdir0=(1.-ffr)*exp(-hflen/lsl)+ffr*exp(-hflen/lfs);//fiber att.factor(direct light, center)
      attfrfl0=((1-ffr)*exp(-hflen/lsl)+ffr*exp(-hflen/lfs))
  	*((1-ffr)*exp(-2*hflen/lsl)+ffr*exp(-2*hflen/lfs))*ECMCFFKEY.fendrf;//(refl, center)
      attf0=0.5*(attfdir0+attfrfl0);
      //
      //---
      an4qin=0;//PM Anode-resp(4cells,pC)
      for(ic=0;ic<4;ic++)pmedepr[ic]=0; 
      Tref= ECREFFKEY.Tref;
      Tsim= ECMCFFKEY.Tsim;
      deltaT=Tsim-Tref;     
      if ( !ECCAFFKEY.useTslope || AMSEvent::gethead()->getrun()<1305763200) deltaT=0.; // T correction not applied for rund before May 19, 2011  
      for(ic=0;ic<4;ic++){//<--- PM 4-subc loop to calc. common PMsatur(due to divider !)
        if(sum[ipm][ic]>0){
	  scgn=ECcalib::ecpmcal[il][ipm].pmscgain(ic);//SubC gain(really 1/pmrg/scgn)(Seed-DB)
  	  // include Gain dependence on Temperature
 	  Tcorr=1.+ECTslope::ecpmtslo[il][ipm].tslope(ic)/100.*deltaT;
 	  if ( Tcorr > 0 ){
  	    scgn*=Tcorr;
  	  }
  	  else{
  	    cerr << "WRONG Sub cell gain correction with T:" << Tcorr << endl;
  	  }
 	  //
	  ares=0.;
	  phel=sum[ipm][ic]*ECMCFFKEY.mev2pes;//numb.of photoelectrons(dE/dX->Npes)(mev2pes incl trapping+Quant.eff)
	  if(phel>=1){
	    ares=ECMCFFKEY.pmseres/sqrt(phel);//ampl.resol.
            // New definition of MeV2MeV (12 nov 2011)            
	    edepr=sum[ipm][ic]*(1.+ares*rnormx())/attf0*ECMCFFKEY.mev2mev;//dE/dX(Mev)->Evis(Mev)(incl.amp.fluct)
	  }
	  else edepr=0;
	  if(edepr<0)edepr=0;
	  pmedepr[ic]=edepr;//Evis(incl.Npes fluct, still not cell-individual)
	  qin=(edepr/scgn)*mev2adc*ECMCFFKEY.adc2q;//AnodeCell "ic" charge(pC)(Evis->ADC->Q)
	  an4qin+=qin;
	}
      }//--->endof 4-subc loop
      if(an4qin>a4maxq)a4maxq=an4qin;
      saturf=1.;
      if(an4qin>0){
	//        saturf=ECcalib::pmsatf1(0,an4qin);//saturation due to 4xAnodes current(divider)
        if(ECMCFFKEY.mcprtf>0)HF1(ECHIST+20,geant(an4qin),1.);
      }
      //---
      an4resp=0;//PM Anode-resp(4cells,tempor in mev)
      dyresp=0;//PM Dynode-resp(tempor in mev)
      anen=0.;
      dyen=0.;
      for(k=0;k<4;k++){//<--- loop over 4-subcells in PM to fill ADC's
        EcalJobStat::addzprmc2(il,sum[ipm][k]);//geant SL(PM-assigned)-profile
	h2lr=ECcalib::ecpmcal[il][ipm].hi2lowr(k);//PM subcell high/low ratio from DB
	scgn=ECcalib::ecpmcal[il][ipm].pmscgain(k);//SubCell gain(really 1/pmrg/scgn)
 	// Gain dependence on Temperature
 	Tcorr=1.+ECTslope::ecpmtslo[il][ipm].tslope(k)/100.*deltaT;
  	if ( Tcorr > 0 ){
  	  scgn*=Tcorr;
  	}
  	else{
  	  cerr << "WRONG Sub cell gain correction with T:" << Tcorr << endl;
  	} 
	edepr=pmedepr[k];//Evis(incl.Npe-fluct, mev)
	emeast+=edepr;
	anen+=saturf*edepr/scgn;//sum for 4xSubc. signal(for trig.study)
	dyen+=edepr;//summing to get dyn.signal for FT and Dadc(no satur.for Dynode ??)
	mevtot+=edepr;
	// ---------> digitization:
	// High-gain channel:
        radc=saturf*edepr*mev2adc/scgn;//Evis(mev)->indiv.Em(adc)
        if(ECMCFFKEY.silogic[0]==0){
	  radc+=(pedh[k]+sigh[k]*rnormx());//+ped+noise
	}
	else if(ECMCFFKEY.silogic[0]==1){
	  radc+=pedh[k];//+ped
	}
	else if(ECMCFFKEY.silogic[0]==2){
	  radc+=0;
	}
	else{
	}
	adc[0]=0;
	adch=integer(floor(radc));//"digitization"(on board)(>=0)
	if(adch>adchmx)adchmx=adch;
	if(adch>=ECADCMX[0])adch=ECADCMX[0];//"ADC-saturation (12 bit)"(on board)
	if(ECREFFKEY.relogic[1]==4){//<-Just test for RD classic PedCal (no "ped" subtraction/suppression)
	  if(adch>0)adc[0]=geant(adch)+0.5;
	}
	else{//normal run
          if(adch>=(pedh[k]+sigh[k]*ECALVarp::ecalvpar.daqthr(0))){//check DAQ-readout threshold(on board)
	    if(ECMCFFKEY.silogic[0]<2)adc[0]=geant(adch)+0.5-pedh[k];// ped-subtraction(earth offline)
	    else if(adch>0)adc[0]=geant(adch)+0.5;
	  }
	  else{ adc[0]=0;}
	}
	// Low-gain channel:
        radc=saturf*edepr*mev2adc/h2lr/scgn;//Evis(mev)->indiv.Em/h2lr(adc)
	//        radc=h2lo+saturf*edepr*mev2adc/h2lr/scgn;//Evis(mev)->indiv.Em/h2lr(adc) + low vs hi offset
        if(ECMCFFKEY.silogic[0]==0){
	  radc+=(pedl[k]+sigl[k]*rnormx());//+ped+noise
	}
	else if(ECMCFFKEY.silogic[0]==1){
	  radc+=pedl[k];//+ped
	}
	else if(ECMCFFKEY.silogic[0]==2){
	  radc+=0;
	}
	else{
	}
	adc[1]=0;
	adcl=integer(floor(radc));//"digitization")(on board)
	if(adcl>adclmx)adclmx=adcl;
	if(adcl>=ECADCMX[1])adcl=ECADCMX[1];//"ADC-saturation (12 bit)"(on board)
	if(ECREFFKEY.relogic[1]==4){//<-Just test for RD class.PedCal (no "ped" subtraction/suppression)
	  if(adcl>0)adc[1]=geant(adcl)+0.5;
	}
	else{//normal run
          if(adcl>=(pedl[k]+sigl[k]*ECALVarp::ecalvpar.daqthr(0))){//check DAQ-readout threshold(on board)
	    if(ECMCFFKEY.silogic[0]<2)adc[1]=geant(adcl)+0.5-pedl[k];// ped-subtraction(earth offline)
	    else if(adcl>0)adc[1]=geant(adcl)+0.5;//no ped-subtr.
	  }
	  else{ adc[1]=0;}
	}
	// <---------
	id=(k+1)+10*(ipm+1)+1000*(il+1);// LTTP(sLayer|Tube|Pixel)
        AMSECIds ids(id);
	if(ids.HCHisBad())adc[0]=0;
	if(ids.LCHisBad())adc[1]=0;
	if(adc[0]>0 || adc[1]>0){
	  nraw+=1;
	  sta=0;
	  if(ECREFFKEY.relogic[1]==4)sta=1;//<-Just test for RD class.PedCal (no "ped" subtraction/suppression)
	  if(ECREFFKEY.reprtf[2]>0){
	    cout<<"<----- Create MC EcalRawEvent: LTTP="<<id<<" ADCh/l="<<adc[0]<<" "<<adc[1]<<" sta="<<sta<<endl;
	    cout<<"       crate/slot(edr)/ch="<<ids.getcrate()<<" "<<ids.getslot()<<" chan="<<ids.getrdch()<<endl;
	    cout<<"       used pedh/l="<<pedh[k]<<" "<<pedl[k]<<endl;
	  }
          AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",ids.getcrate()),
				       new AMSEcalRawEvent(id,sta,0,adc));//side=0(a) for MC
	}
      }//<---- end of 4-PMSubcell-loop
      //---
      // Dynode channel:
      radc=dyen*pmrgn*mev2adc/a2dr;//Ev(mev)->Em/a2dr(adc)=Emd
      if(ECMCFFKEY.silogic[0]==0){
        radc+=(pedd+sigd*rnormx());//+ped+noise
      }
      else if(ECMCFFKEY.silogic[0]==1){
	radc+=pedd;//+ped
      }
      else if(ECMCFFKEY.silogic[0]==2){
        radc+=0;
      }
      else{
      }
      adcd=integer(floor(radc));//"digitization")(on board)(>=0)
      radc=0;
      if(adcd>adcdmx)adcdmx=adcd;
      if(adcd>=ECADCMX[2])adcd=ECADCMX[2];//"ADC-saturation (12 bit)"(on board)
      if(ECREFFKEY.relogic[1]==4){//<-Just test for RD class.PedCal (no "ped" subtraction/suppression)
	if(adcd>0)radc=geant(adcd)+0.5;
      }
      else{//normal run
        if(adcd>=(pedd+sigd*ECALVarp::ecalvpar.daqthr(4))){//check DAQ-readout threshold(on board)
          if(ECMCFFKEY.silogic[0]<2)radc=geant(adcd)+0.5-pedd;// ped-subtraction(earth offline)
          else if(adcd>0)radc=number(adcd)+0.5;//no ped-subtr.
        }
        else{ radc=0;}
      }
      //
      if(ECPMPeds::pmpeds[il][ipm].DCHisBad() || ECcalib::ecpmcal[il][ipm].DCHisBad())radc=0;
      if(radc>0){
	nrawd+=1;
	id=1000*(il+1)+10*(ipm+1)+5;// LTTP(sLayer|Tube|Pixel=5)
        AMSECIds ids(id);
	if(ECREFFKEY.reprtf[2]>0){
	  cout<<"<----- Add Dyn:LTTP/adc="<<id<<" "<<radc<<" to crate/slot(edr)="<<
	    ids.getcrate()<<" "<<ids.getslot()
	      <<" ped="<<pedd<<endl;
	}
	dynadc[il][ipm]=radc;//store in static array of AMSEcalRawEvent class
      }
      //
      an4resp=anen;//PM 4anode-sum resp(true mev ~ mV !!!)
      //      dyresp=dyen*pmrgn/ECMCFFKEY.physa2d;//dyn.signal for EC_FT(tempor in DynodeMev ~ real mV !!!)
      //                                      (later to add some factor to conv. to mV)
      dyresp=dyen*pmrgn;//Tempor dyn.signal for EC_FT in AnodeMevs ~ real mV !!!
      an4respt+=an4resp;
      dyrespt+=dyresp;
      if(ECMCFFKEY.mcprtf>0){
        if(dyresp>0)HF1(ECHIST+5,geant(dyresp),1.);
        if(dyresp>0 && an4resp<5000.)HF1(ECHIST+6,geant(an4resp/dyresp),1.);
      }
      //            arrays for trigger study:
      pmtmap[il][ipm]=dyresp;//tempor Dynode-resp in Anode scale(mev)
      pmlprof[il]+=dyresp; 
      //
    }//-------> end of PM-loop in superlayer
    //
  } // ------------> end of super-layer loop
  //---
  if(ECREFFKEY.reprtf[2]>1){
    cout<<endl;
    cout<<"-----> DynodesMap: "<<endl<<endl;
    for(il=0;il<nslmx;il++){//prepare trigger Dynode map(variab.thr vs layer)
      for(int ipm=0;ipm<npmmx;ipm++){
        if(dynadc[il][ipm]>0)cout<<1<<" ";
	else cout<<0<<" ";
      }
      cout<<endl;
    }
    cout<<endl;    
  }
  if(ECMCFFKEY.mcprtf>0){
    HF1(ECHIST+1,geant(nhits),1.);
    HF1(ECHIST+2,geant(edept),1.);
    HF1(ECHIST+3,geant(edeprt),1.);
    HF1(ECHIST+4,geant(emeast),1.);
    HF1(ECHIST+9,geant(dyrespt),1.);
    //
    if(a4maxq>0)HF1(ECHIST+21,geant(a4maxq),1.);
    if(adchmx>0)HF1(ECHIST+22,geant(adchmx),1.);
    if(adclmx>0)HF1(ECHIST+23,geant(adclmx),1.);
    if(adcdmx>0)HF1(ECHIST+24,geant(adcdmx),1.);
  }
  //
  //===> Create FT/LVL1 parts of EC-trigger(now Pisa-algorithm only):
  //
  uinteger trflen(0),trflwd(0);
  // trflen(energy(mult).trig.flag)=0/1/2/3->Etot<mip/>=mip/multLow/multOK
  // trflwd(angle(width) trig.flag)=0/1/2->unknown/bad/OK
  integer nprx(0),npry(0);
  integer nlmin;
  number dyslmx[ECSLMX]={0.,0.,0.,0.,0.,0.,0.,0.,0.};
  number dytrsum(0),dysl;
  int word,bit;
  trigfl=0;
  for(il=0;il<nslmx;il++){//prepare trigger Dynode map(variab.thr vs layer)
    for(pm=0;pm<npmmx;pm++){
      dysl=pmtmap[il][pm];
      dytrsum+=dysl;
      if(dysl>dyslmx[il])dyslmx[il]=dysl;
      //      if(ECMCFFKEY.mcprtf>0){
      //        if(dysl>0)HF1(ECHIST+30+il,geant(dysl),1.);
      //      }
      
      if(dysl>ECALVarp::ecalvpar.daqthr(5+il) && !ECcalib::ecpmcal[il][pm].DCHisBad()){//incl."in-trig" check
	dytrc[il]+=1;//count PMs>thr per layer
	dycog[il]+=(pm+1);
	if(il>=1 && il<=6){//set trig.pattern bits for 6 "trigger" sup-layers
	  word=pm/16;//0-2
	  bit=pm%16;//0-15
	  trpatt[il-1][word]|=(1<<bit);
	}
      }
    }
    if(dytrc[il]>0){
      dycog[il]*=64;//as in real trigger electronics(according Stefano Di Falco)
      dycog[il]=integer(geant(dycog[il])/dytrc[il]);//layer COG
    }
  }
  //
  if(ECMCFFKEY.mcprtf>0){// histogr of max in SL
    for(il=0;il<nslmx;il++)if(dyslmx[il]>0)HF1(ECHIST+30+il,geant(dyslmx[il]),1.);
  }
  //
  for(il=0;il<5;il+=2){//count in SL 2:7
    if(dytrc[il+1]>0)nprx+=1;//counts SLs with at least 1 PMs above threshold in each proj
    if(dytrc[il+2]>0)npry+=1;
  }
  EcalJobStat::addsr(0);
  if(nprx>0 || npry>0){
    EcalJobStat::addsr(1);//intermediate state
  }
  //
  //---> check FT conditions:
  //
  nlmin=integer(ECALVarp::ecalvpar.rtcuts(4));//min layers/proj (with at least 1 PM>thr in layer)
  integer orand;
  orand=Trigger2LVL1::l1trigconf.ecorand();//proj-or/and according to DB/File(redef by DC)
  integer prjmsk;
  prjmsk=Trigger2LVL1::l1trigconf.ecprjmask();//active proj.mask(mlki) from DB/File(redef by DC)
  integer ftmsk=prjmsk%100;//(ki->yx)proj.mask for FT-check
  //
  bool ftex=(nprx>=nlmin && ftmsk%10==1);
  bool ftey=(npry>=nlmin && ftmsk/10==1);
  if(ftex)AMSEcalRawEvent::settrpbit(5,38);//set related bits in trig.patt
  if(ftey)AMSEcalRawEvent::settrpbit(5,39);
  //check proj or/and and set proj-flag for FTE:
  if(ftex && ftey)trigconf=20;
  else if(ftex || ftey)trigconf=10;
  else trigconf=0;
  // check FTE:
  if(orand==1){//<- reqOR 
    if((trigconf/10)>=1){
      trflen=2;//FTE&1proj
      if((trigconf/10)==2)trflen=3;//FTE&2proj
      EcalJobStat::addsr(3);//FTE ok
    }
  }
  else if(orand==2){//<- reqAND
    if((trigconf/10)==2){
      trflen=3;//FTE&2proj
      EcalJobStat::addsr(3);
    }
    else if((trigconf/10)==1)trflen=1;//no FTE when OR at AND-requirement 
  }
  else{
    cerr<<"AMSEcalRawEvent::mc_build:Error - Wrong projOR/AND-requirement for FTE!!! "<<orand<<endl;
    exit(1);
  }
  if(ECMCFFKEY.mcprtf>0){
    HF1(ECHIST+39,geant(npry),1.);
    HF1(ECHIST+40,geant(nprx),1.);
    if(dytrsum>0)HF1(ECHIST+43,geant(dytrsum),1.);
  }
  //
  //---> check LVL1 Angle(width) conditions if FT found:
  //
  integer l1msk=prjmsk/100;//(ml->yx)proj.mask for Angle-check(in LVL1)
  number dbx24(-99),dbx46(-99),dbx26(-99),dbxm(-99);
  number dby35(-99),dby57(-99),dby37(-99),dbym(-99);
  int wdxcut,wdycut;//to follow the real electronics logic
  if(trflen>=2){//<-- FT OK
    if(dytrc[1]>0 && dytrc[3]>0)dbx24=abs(dycog[1]-dycog[3]);
    if(dytrc[3]>0 && dytrc[5]>0)dbx46=abs(dycog[3]-dycog[5]);
    if(dytrc[1]>0 && dytrc[5]>0)dbx26=abs(dycog[1]-dycog[5])/2;
    if(dytrc[2]>0 && dytrc[4]>0)dby35=abs(dycog[2]-dycog[4]);
    if(dytrc[4]>0 && dytrc[6]>0)dby57=abs(dycog[4]-dycog[6]);
    if(dytrc[2]>0 && dytrc[6]>0)dby37=abs(dycog[2]-dycog[6])/2;
    //
    if(dbx26>=0)dbxm=dbx26;
    else{
      if(dbx24>=0)dbxm=dbx24;
      else dbxm=dbx46;
    }
    //
    if(dby37>=0)dbym=dby37;
    else{
      if(dby35>=0)dbym=dby35;
      else dbym=dby57;
    }
    //
    if((dytrc[3]+dytrc[5])<ECALVarp::ecalvpar.rtcuts(2))wdxcut=integer(64*ECALVarp::ecalvpar.rtcuts(0));
    else wdxcut=integer(64*ECALVarp::ecalvpar.rtcuts(1));
    if((dytrc[4]+dytrc[6])<ECALVarp::ecalvpar.rtcuts(3))wdycut=integer(64*ECALVarp::ecalvpar.rtcuts(0));
    else wdycut=integer(64*ECALVarp::ecalvpar.rtcuts(1));
    //"64" to convert logic cuts into real electronics scale (according to Stefano Di Falco)   
    if(ECMCFFKEY.mcprtf>0){
      HF1(ECHIST+41,geant(dbym/64),1.);//"64" to view variables in the logic scale
      HF1(ECHIST+42,geant(dbxm/64),1.);
    }
    //
    bool angx=(dbxm<wdxcut && l1msk%10==1);
    bool angy=(dbym<wdycut && l1msk/10==1);
    if(angx)AMSEcalRawEvent::settrpbit(5,36);//det related bits in trig.patt
    if(angy)AMSEcalRawEvent::settrpbit(5,37);
    //check proj or/and and set proj-flag for LVL1:
    if(angx && angy)trigconf+=2;
    else if(angx || angy)trigconf+=1;
    else trigconf+=0;
    // check LVL1:
    if((trigconf%10)==2){
      trflwd=3;//was LVL1-and(=2prj)
      EcalJobStat::addsr(4);
    }
    else if((trigconf%10)==1){
      trflwd=2;//was LVL1-or(=1prj)
      EcalJobStat::addsr(4);
    }
    else trflwd=1;//was FTE, but 0proj
  }//->endof "FT" check
  //
  trigfl=10*trflen+trflwd;//MN, M->EnergyFlag, N->WidthFlag
  //
  //--->print trigpatt:
  if(ECREFFKEY.reprtf[2]>1){
    cout<<"      TrigPattern(slayer=1-6, pmt=1-36):"<<endl<<endl;
    for(il=0;il<ECSLMX-3;il++){
      for(pm=0;pm<ECPMMX;pm++){
        if(AMSEcalRawEvent::gettrpbit(il,pm)>0)cout<<1<<"|";
        else cout<<0<<"|";
      }
      cout<<endl;
    }
    cout<<endl;
    cout<<"FastTrigBits(XF/YF)="<<AMSEcalRawEvent::gettrpbit(5,38)<<" "<<AMSEcalRawEvent::gettrpbit(5,39)<<endl;
    cout<<"Lvl1TrigBits(XA/YA)="<<AMSEcalRawEvent::gettrpbit(5,36)<<" "<<AMSEcalRawEvent::gettrpbit(5,37)<<endl;
  }
  if(ECREFFKEY.reprtf[2]>0)
    cout<<"<----- TrigConf/TrigFlg MN(FTE/LV1)="<<trigconf<<" "<<trigfl<<endl<<endl;;    
  //
  //-------> create ECAL fast trigger(FT)-time:
  //
  if(trflen>=2){
    trigtm=timet/edept+100;//tempor: EC-crate signals eff.time(aver. over mc-hits)(ns) + EC-crates decision time
    trigtm+=ECALDBc::ftedel();//+ fix.delay EC-to-LVL1 + FTE-decision time in JLV1(FTE-time in AMSEcalRawEvent-obj)
    trsum=geant(dyrespt/1000.);//dynode signal (gev tempor)
  }
  //---
  if(ECMCFFKEY.mcprtf>0){
    HF1(ECHIST+10,geant(trigconf),1.);
    HF1(ECHIST+19,geant(trigfl),1.);
  }
  if(trflen>=2)stat=0;
  // DEBUG
  if (ECREFFKEY.reprtf[1]==2) cout << "///////////////////////////// MEVTOT=" << mevtot << " //////////" <<endl;
  return;
}

//---------------------------------------------------
integer AMSEcalRawEvent::lvl3format(int16 *ptr, integer rest){
  //(to fill aux-part of lvl3 data block with EC raw info)
  //
  //  integer _idsoft; //readout cell ID=SSPPC (SS->S-layer,PP->PMcell, C->SubCell in PMcell)
  //  integer _padc[2];// Anode pulse hights (ADC-channels)[HighGain,LowGain]
  //
  if (_padc[0]<=0) return 0;
  *(ptr)=_idsoft;
  *(ptr+1)=int16(_padc[0]);
  *(ptr+2)=int16(_padc[1]);
  return 3; 
}
//---------------------------------------------------
void AMSEcalRawEvent::BeamTestLinCorr(int gain,int id,
				      number radc[2],geant ped[2],number &fadc){//linearity corr. function
  //gain=1/2->Ah/Al
  if(gain==1){// h-ch
    number ph=ped[0];
    number a=1.21;
    number b=1.26e-4;
    if(radc[0]+ph<1666){
      fadc=radc[0];
    }
    else{
      fadc=radc[0]*(a-b*(radc[0]+ph));
    }
  }
  else{ // l-ch
    //      assume tri-angular  h/l correction;

    number ph=ped[0];
    number a=1.05;
    number b=0.5e-4/2/2;
    number x1=3766;
    number x2=5866;
    number x3=x2+x2-x1;
    number al=b;
    number be=b*ph-a;
    if(fadc<(x2-ph)*(a-b*x2)){
      number mfadc=(-be-sqrt(be*be-4*fadc*al))/2/al;
      //         cout <<" case 1 "<<fadc<<" "<<mfadc<<endl;
      fadc=mfadc;
    }
    else if(fadc<x3){
      a=1-b*x3;
      al=-b;
      be=-b*ph-a;
      number mfadc=(-be-sqrt(be*be-4*fadc*al))/2/al;
      //         cout <<" case 2 "<<fadc<<" "<<mfadc<<endl;
      fadc=mfadc;
    }
  }
}
//---------------------------------------------------
void AMSEcalHit::build(int &stat){
  int i;
  integer sta,id,idd,isl,pmc,subc,nraw,nhits;
  integer proj,plane,cell,icont;
  number radc[3],edep,edepc,adct,fadc,qmeas,saturf,emeast,coot,cool,cooz;
  number adchovfl; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,pd,sl,sd;
  geant h2lo;
  integer ovfl[3];
  geant padc[3],bpadc[4][2];
  integer iddn,bsta[4],bsubc[4],bid[4],nsubc,nsubco;
  number bedep[4],edepgd,dedep,etrue,edcort;
  AMSEcalRawEvent * ptr;
  AMSEcalRawEvent * ptrn;
  geant scgn,deltaT,Tcorr;
  geant mevtot;
  int ReadTemp;
  float T_PMT;
  //
  stat=1;//bad
  nhits=0;
  nraw=0;
  adct=0.;
  emeast=0.;
  edcort=0.;
  adchovfl=ECADCMX[0]-1;//default 
  mevtot=0.; 
  if(AMSJob::gethead()->isRealData() && AMSEvent::gethead()->getrun()<1265450278)adchovfl=2000;//due to err in DSP
  //
  for(int nc=0;nc<AMSECIds::ncrates();nc++){ // <-------------- cr. loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
      getheadC("AMSEcalRawEvent",nc,0);
    nsubc=0;//clear buff.arr.
    qmeas=0.;
    for(i=0;i<4;i++)bsubc[i]=0;
    for(i=0;i<4;i++)bedep[i]=0;
    while(ptr){ // <--- RawEvent-hits loop in superlayer:
      isl=ptr->getslay();//0,1,...
      nraw+=1;
      id=ptr->getid();//SSPPC
      AMSECIds ids(id);
      idd=id/10;
      subc=id%10-1;//SubCell(0-3)
      pmc=idd%100-1;//PMCell(0-...)
      ptr->getpadc(padc);
      ECPMPeds::pmpeds[isl][pmc].getpedh(pedh);
      ECPMPeds::pmpeds[isl][pmc].getsigh(sigh);
      ECPMPeds::pmpeds[isl][pmc].getpedl(pedl);
      ECPMPeds::pmpeds[isl][pmc].getsigl(sigl);
      h2lr=ECcalib::ecpmcal[isl][pmc].hi2lowr(subc);
      h2lo=ECcalib::ecpmcal[isl][pmc].hi2lowo(subc);
      radc[0]=number(padc[0]);//ADC-high-chain
      radc[1]=number(padc[1]);//ADC-low-chain
      //      if(radc[2]>0)cout << " found Dyn "<<radc[0]<<" "<<radc[1]<<" "<<radc[2]<<endl;
      ph=pedh[subc];
      pl=pedl[subc];
      sl=sigl[subc];
      if(AMSJob::gethead()->isSimulation()){
        if(ECMCFFKEY.silogic[0]==1){
          sl=0;
        }
        if(ECMCFFKEY.silogic[0]==2){
          ph=0;
	  pl=0;
        }
      }
      ovfl[0]=0;
      ovfl[1]=0;
      if(radc[0]+ph>=adchovfl)ovfl[0]=1;// mark as HiChan ADC-Overflow
      if(radc[1]+pl>=ECADCMX[1]-1)ovfl[1]=1;// mark as ADC-Overflow
      // ---> take decision which chain to use for energy calc.(Hi or Low):
      sta=0;
      fadc=0.;
      if(radc[0]>0 && ovfl[0]==0 && !ids.HCHisBad()){//<-use h-chan
        fadc=radc[0];
      }
      //
      else if(radc[1]>max(3.*sl,3.)  && ovfl[1]==0 && !ids.LCHisBad()){//Hch=Miss/Ovfl/bad -> use Lch
	//        fadc=radc[1]*h2lr;//rescale LowG-chain to HighG
        fadc=(radc[1]-h2lo)*h2lr;//rescale LowG-chain to HighG(h2lo(~0.5) is offset in Low-ch)
	sta|=AMSDBc::LOWGCHUSED;// set "LowGainChannel used" status bit
      }
      //
      else if(ovfl[0]==1 && !ids.HCHisBad() && (radc[1]<max(3.*sl,3.) || ids.LCHisBad())){//Hch=ovfl(good) but Lch=Miss/bad, use Hch
        fadc=radc[0];
	sta|=AMSDBc::AOVERFLOW;// set overflow status bit
      }
      //
      else if(ovfl[1]==1 && !ids.LCHisBad()){//<-use even overflowed good Lgain-chan
	//        fadc=radc[1]*h2lr;//use low ch.,rescale LowG-chain to HighG
        fadc=(radc[1]-h2lo)*h2lr;//rescale LowG-chain to HighG(h2lo(~0.5) is offset in Low-ch)
	sta|=AMSDBc::AOVERFLOW;// set overflow status bit
	sta|=AMSDBc::LOWGCHUSED;// set "LowGainChannel used" status bit
	//
      }
      else {
	sta|=AMSDBc::BAD;// 0 pixel amplitude (anode) due bad Lch and Hch
      }
      //---> mark bad (according to calib-DB) pix-channels for possible use at later stages:
      if(ids.HCHisBad())sta|=AMSDBc::ECHCISBAD;
      if(ids.LCHisBad())sta|=AMSDBc::ECLCISBAD;
      //
      // read temperature from map
      if (AMSJob::gethead()->isRealData()){
 	ReadTemp= ECTslope::ecpmtslo[isl][pmc].GetECALSensorTemper(isl,pmc,&T_PMT);
 	if (ReadTemp==0) {	  
 	  deltaT=T_PMT-ECREFFKEY.Tref;
 	}
 	else{
 	  //cout << "WARNING: No T sensor found for SL " << isl << " PMT " << pmc  << endl;
	  deltaT=0.;
	}
      }
      else{
 	deltaT=ECMCFFKEY.Tsim-ECREFFKEY.Tref;
      }
      if ( !ECCAFFKEY.useTslope || AMSEvent::gethead()->getrun()<1305763200) deltaT=0.; // T correction not applied for runs before May 19, 2011
      //
      scgn=ECcalib::ecpmcal[isl][pmc].pmscgain(subc);//SubC gain(really 1/pmrg/scgn)(Seed-DB)
      // correct for Gain dependence on Temperature
      Tcorr=1.+ECTslope::ecpmtslo[isl][pmc].tslope(subc)/100.*deltaT;
      //Additional temperature correction to shift from Reference temperature Tref
      //to TestBeam temperature Ttb using global slope Tgsl
      //Not to be applied to MC
      //(4/6/13: Tref=10deg, Ttb=23deg, Tgsl=0.25%/deg)
      if (AMSJob::gethead()->isRealData() && AMSEvent::gethead()->getrun()>=1305763200) 
	Tcorr=Tcorr*(1.+ECREFFKEY.Tgsl/100.*(ECREFFKEY.Tref-ECREFFKEY.Ttb));
      if ( Tcorr > 0 ){
  	scgn*=Tcorr;
      }
      else{
  	cout << "WRONG Sub cell gain correction with T:" << Tcorr << endl;
      }
      edep=fadc*scgn;
      //      (because in Calib.object pmsc-gain was defined as 1/pmrg/pmscg)
      if(ECREFFKEY.reprtf[0]>1 && edep>2){
#pragma omp critical (hf1)
	{
	  HF1(ECHISTR+16,geant(edep),1.);//adc
	  HF1(ECHISTR+17,geant(edep),1.);
	}
      }
      adct+=edep;//tot.adc
      edep=edep*ECcalib::ecpmcal[isl][pmc].adc2mev();// ADCch->Emeasured(MeV)
      if(fadc>0.){// store good (h+l)-hit info in buffer:
	mevtot+=edep;
	bsta[nsubc]=sta;
	bsubc[nsubc]=subc+1;
	bid[nsubc]=id;
	for(i=0;i<2;i++){//high/low gain
	  bpadc[nsubc][i]=padc[i];
	}
	bedep[nsubc]=edep;
	qmeas+=(fadc*ECMCFFKEY.adc2q);//sum to get pm tot.charge(pC)
	nsubc+=1;
      }
      //
      ptrn=ptr->next();
      iddn=0;
      if(ptrn)iddn=ptrn->getid()/10;//SSPP of next hit
      //
      if(idd!=iddn && nsubc>0){//next hits belong to next PM -> process buffer of current PM
	// ------>  1st: check dynode chan:
        dedep=0;
        padc[2]=AMSEcalRawEvent::getadcd(isl,pmc);//get dynode from static array
        radc[2]=number(padc[2]);//ADC-dyn-chain
        pd=ECPMPeds::pmpeds[isl][pmc].pedd();
        sd=ECPMPeds::pmpeds[isl][pmc].sigd();
        if(AMSJob::gethead()->isSimulation()){
          if(ECMCFFKEY.silogic[0]==1){
            sd=0;
          }
          if(ECMCFFKEY.silogic[0]==2){
            pd=0;
          }
        }
        ovfl[2]=0;
        if(radc[2]+pd>=ECADCMX[2]-1)ovfl[2]=1;// mark as Dynode-ADC-Overflow
	int dychok=0;
        if(radc[2]>max(3.*sd,3.) && ovfl[2]==0 && !ids.DCHisBad()){//to use for possible pix-ovfl treatment
	  dychok=1;//<-one can use dy-chan
	  fadc=radc[2];
	  dedep=fadc*ECcalib::ecpmcal[isl][pmc].adc2mev()
	    *ECcalib::ecpmcal[isl][pmc].an2dyr()
	    /ECcalib::ecpmcal[isl][pmc].pmrgain();//PM(4xSubc=Dy) gain(wrt ref. one)
	}
	dychok=0;//tempor to prevent any corrections
	//
	// --->  2nd: buffer->AMSEcalHit + l-ch ovfl correction(if needed) by dynode info:
        edepgd=0;//PM(4xSubc) "nonoverflowed" energy 
	nsubco=0;//number of overflowed subcells
        for(i=0;i<nsubc;i++){//count l-ch ovfls
	  if((bsta[i]&AMSDBc::AOVERFLOW)>0)nsubco+=1;
	  else edepgd+=bedep[i]; 
	}
	if(dychok>0 && nsubco>0)etrue=(dedep-edepgd)/geant(nsubco);
	//                               (average dyn.energy per overflowed_cell)
        saturf=ECcalib::pmsatf1(1,qmeas);//saturf=Qin/Qmeas(<0 if saturated, 1 if qmeas small)
	saturf=1;//tempor to prevent any satur.corrections(tot.pm-charge satur due divider extra-current)
        if(saturf<0)EcalJobStat::addsr(10);
	//
        for(i=0;i<nsubc;i++){//buffer(subcells) loop to fill AMSEcalHit
          nhits+=1;
	  edep=bedep[i];
	  sta=bsta[i];
	  if(ids.DCHisBad())sta|=AMSDBc::ECDCISBAD;//mark dynode if bad (according to calib-DB)
	  if((sta&AMSDBc::AOVERFLOW)>0 && dychok>0){//make correction for adc-ovfl if needed/possible
	    edcort+=(etrue-edep);//counts total correction
	    edep=etrue;//welcome any other ideas...
	  }
	  if(saturf<0){//above the internal range of gmeas
	    sta|=AMSDBc::LEAK;
	    saturf=fabs(saturf);
	  }
	  edepc=edep*(saturf-1.);//add this value to corr. for PM-saturation(type-1)
	  edep=edep*saturf; 
	  id=bid[i];
	  subc=bsubc[i]-1;
	  padc[0]=bpadc[i][0];
	  padc[1]=bpadc[i][1];
          ECALDBc::getscinfoa(isl,pmc,subc,proj,plane,cell,coot,cool,cooz);//SubCell info
	  icont=plane;//container number for storing of EcalHits(= plane number)
          emeast+=edep;//tot.Mev
	  if(ECREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
	    {
	      HF1(ECHISTR+9,geant(edep),1.);
	    }
	  }
          AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",icont), new
				       AMSEcalHit(sta,id,padc,proj,plane,cell,edep,edepc,coot,cool,cooz));
	  //       (object is created even if was Anode ovfl, but sta is set properly)
	}//--->endof buffer(subcells) loop
	//
	nsubc=0;//clear buffer arrays
	qmeas=0;
        for(i=0;i<4;i++)bsubc[i]=0;
        for(i=0;i<4;i++)bedep[i]=0;
      }
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop
    //
  }// ---> end of crate loop
  //
  if(ECREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
    {
      HF1(ECHISTR+10,geant(nraw),1.);
      HF1(ECHISTR+11,geant(adct),1.);
      HF1(ECHISTR+12,geant(adct),1.);
      HF1(ECHISTR+13,geant(nhits),1.);
      HF1(ECHISTR+14,geant(emeast),1.);
      HF1(ECHISTR+15,geant(edcort),1.);
    }
  }
  if(nhits>0)stat=0;
  Smooth();
  // DEBUG
  if (ECREFFKEY.reprtf[1]==2) cout << "****************************** MEVTOT=" << mevtot << " ******" << endl; 
}
//-------------------------------------------------
// Function to find and correct hot cells
// A cell identified as Hot is tagged as BAD (hit status BAD)
integer AMSEcalHit::Smooth(){

  const integer  maxpl=2*ECALDBc::slstruc(3);//real planes
  const integer Maxcell=2*ecalconst::ECPMSMX;//numebr of cells
  const number maxfractions1s3 = 0.98; // max s1s3 fraction to not reject the hit
  const number maxfractions1s6 = 0.98; // max s1s6 fraction to not reject the hit

  number enemins1s3 = 1000; // 1000 MeV s1s3 
  number enemins1s6 = 1000; // 1000 MeV s1s6
  bool badfractions1s3 = false;
  bool badfractions1s6 = false;
  number planemap[Maxcell];
  number superlayermap[2][Maxcell];
  
  for(int iplane=0;iplane<maxpl;iplane++){
    //list of hits in the plane iplane
    AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->getheadC("AMSEcalHit",iplane,1);
    if(!ptr)continue;//ptr doesn't exit, move to next plane 

    AMSEcalHit *temp = ptr;
    AMSEcalHit *temp2=NULL;
    //list of hits in the plane of iplane SuperLayer
    if((iplane%2)==0)
      temp2=(AMSEcalHit*)AMSEvent::gethead()->
	getheadC("AMSEcalHit",iplane+1,1);
    else
      temp2=(AMSEcalHit*)AMSEvent::gethead()->
	getheadC("AMSEcalHit",iplane-1,1);
  
    //Init hitmap matrix
    for(int icell=0;icell<Maxcell;icell++){
      planemap[icell]=0;
      superlayermap[0][icell]=0;
      superlayermap[1][icell]=0;
    }

    //Fill planemap matrix
    while(temp){
      number edep = temp->getedep();
      if(!temp->checkstatus(AMSDBc::BAD) && edep>0){
	planemap[temp->getcell()]=edep;
	superlayermap[0][temp->getcell()]=edep;//the layer of ptr is always 0 in superlayermap
	// the layer in the same superlayer of ptr layer is always 1 in superlayermap
	//indipendent by the real disposition (before or after in the real geometry)
      }
      else // if hit channel is BAD, put in hitmap energy = - edep
	planemap[temp->getcell()]=-edep;
      temp=temp->next();
    }//end while on temp  
    //Fill superlayer map
    while(temp2){
      number edep = temp2->getedep();
      if(!temp2->checkstatus(AMSDBc::BAD) && edep>0)
	superlayermap[1][temp2->getcell()]=edep;
      else // if hit channel is BAD, put energy = - edep
	superlayermap[1][temp2->getcell()]=-edep;
      temp2=temp2->next();
    }//end while on temp2
    
    AMSEcalHit *temp3 = ptr;
    //check for hot cells (using only cell on same plane)
    while(temp3){
      badfractions1s3 = false;
      badfractions1s6 = false;
      number edep=temp3->getedep();  
      int cell = temp3->getcell();
      // check on s1s3
      if(!temp3->checkstatus(AMSDBc::BAD) && cell!=0 && cell!=Maxcell-1){
	if(edep>enemins1s3)
	  if(planemap[cell-1]>=0 && planemap[cell+1]>=0){
	    number fractions1s3 = edep / (planemap[cell-1]+ edep + planemap[cell+1]);
	    if(fractions1s3 >= maxfractions1s3)
	      badfractions1s3 = true;
	  }// end if on cell-1 and cell+1
	//end check on s1s3
	//check on s1s6
	if(edep>enemins1s6)
	  if(superlayermap[0][cell-1]>=0 && superlayermap[0][cell+1]>=0 &&
	     superlayermap[1][cell-1]>=0 && superlayermap[1][cell] >=0 && superlayermap[1][cell+1]>=0){
	    number fractions1s6 = edep / (superlayermap[0][cell-1]+ edep + superlayermap[0][cell+1]+
					  superlayermap[1][cell-1]+superlayermap[1][cell]+superlayermap[0][cell+1]
					  );
	    if(fractions1s6 >= maxfractions1s6)
	      badfractions1s6 = true;
	  }// end if on cell-1 and cell+1
	//end check on s1s6
      }//end if temp3 is good
      // if one of two fraction is bad, tag the hit as BAD
      if(badfractions1s6 == true || badfractions1s3 == true)
	temp3->setstatus(AMSDBc::BAD);
      temp3=temp3->next();
    }//end while on temp3
  }// end loop on planes
  return 1;// good exit

}

//---------------------------------------------------
void AMSEcalHit::attcor(number coo){//correct measured _edep for atten.in fibers
  if(!checkstatus(AMSDBc::REFITTED) && !checkstatus(AMSDBc::RECOVERED)){
    // coo is longit.coord(i.e. from other projection) in mother ref.syst. !!!
    geant pmdist,hflen,attf,attf0,attfdir,attfrfl;
    int sl=_plane/2;
    int pm=_cell/2;
    int rdir=1-2*(pm%2);
    _cool=coo;
    if(_proj==1){//Y-proj hit
      coo=coo-ECALDBc::gendim(5);//convert coo(from X-proj) into ECAL coord.syst.
      hflen=ECALDBc::gendim(1)/2;
      rdir=rdir*ECALDBc::slstruc(6);//=1/-1 -> along/opposit X-axes
    }
    else{//X-proj hit
      coo=coo-ECALDBc::gendim(6);//convert coo(from Y-proj) into ECAL coord.syst.
      hflen=ECALDBc::gendim(2)/2;
      rdir=rdir*ECALDBc::slstruc(5);//=1/-1 -> along/opposit Y-axes
    }
    pmdist=coo+hflen;
    if(rdir<0)pmdist=2*hflen-pmdist;
    attfdir=ECcalib::ecpmcal[sl][pm].attfdir(pmdist);//fiber att.factor(direct light)
    attfrfl=ECcalib::ecpmcal[sl][pm].attfrfl(pmdist,hflen);//(refl)
    attf=0.5*(attfdir+attfrfl);//pmdist=[0,2*hflen]
    attf0=0.5*(ECcalib::ecpmcal[sl][pm].attfdir(hflen)
	       +ECcalib::ecpmcal[sl][pm].attfrfl(hflen,hflen));//attf at center
    _attcor=_edep*(attf0/attf-1.);

    _edep+=_attcor;
    setstatus(AMSDBc::REFITTED);
  }
}
//---------------------------------------------------
integer Ecal1DCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("Ecal1DCluster",AMSJob::gethead()->gettriggerC(n))==0){
	WriteAll=1;
	break;
      }
    }
  }
  return (WriteAll || status);
}

integer AMSEcal2DCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("Ecal2DCluster",AMSJob::gethead()->gettriggerC(n))==0){
	WriteAll=1;
	break;
      }
    }
  }
  return (WriteAll || status);
}


//---------------------------------------------------
void AMSEcalHit::_writeEl(){

  //  EcalHitNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecalhit();

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    /*
      if (TN->Necht>=MAXECHITS ) return;
      // Fill the ntuple
      TN->Status[TN->Necht]=_status;
      TN->Idsoft[TN->Necht]=_idsoft;
      TN->Proj[TN->Necht]=_proj;
      TN->Plane[TN->Necht]=_plane;
      TN->Cell[TN->Necht]=_cell;
      TN->Edep[TN->Necht]=_edep;
      TN->EdCorr[TN->Necht]=_edepc;
      TN->AttCor[TN->Necht]=_attcor;
      if(_proj){ //<-- y-proj
      TN->Coo[TN->Necht][0]=_cool;
      TN->Coo[TN->Necht][1]=_coot;
      }
      else{     //<-- x-proj
      TN->Coo[TN->Necht][0]=_coot;
      TN->Coo[TN->Necht][1]=_cool;
      }
      AMSECIds ic(getid());
      TN->Coo[TN->Necht][2]=_cooz;
      TN->ADC[TN->Necht][0]=_adc[0];
      TN->ADC[TN->Necht][1]=_adc[1]*ic.gethi2lowr();
      TN->ADC[TN->Necht][2]=_adc[2]*ic.getan2dyr();
      TN->Ped[TN->Necht][0]=ic.getped(0);
      TN->Ped[TN->Necht][1]=ic.getped(1);
      TN->Ped[TN->Necht][2]=ic.getpedd();
      TN->Gain[TN->Necht]=ic.getgain();
      TN->Necht++;
    */
  }
}
//---------------------------------------------------
void AMSEcalHit::_copyEl(){
}
//---------------------------------------------------
integer AMSEcalHit::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSEcalHit",AMSJob::gethead()->gettriggerC(n))==0){
	WriteAll=1;
	break;
      }
    }
  }
  return (WriteAll || status);
}





void Ecal1DCluster::_writeEl(){
  //
  //
  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    /*
      EcalClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecclust();

      if (TN->Neccl>=MAXECCLUST) return;

      // Fill the ntuple
      TN->Status[TN->Neccl]=_status;
      TN->Proj[TN->Neccl]=_proj;
      TN->Plane[TN->Neccl]=_plane;
      TN->Left[TN->Neccl]=_Left;
      TN->Center[TN->Neccl]=_MaxCell;
      TN->Right[TN->Neccl]=_Right;
      if(checkstatus(AMSDBc::JUNK)){
      TN->Edep[TN->Neccl]=-_EnergyC;
      }
      else TN->Edep[TN->Neccl]=_EnergyC;
      //    TN->Edep[TN->Neccl][1]=_Energy3C;
      //    TN->Edep[TN->Neccl][2]=_Energy5C;
      //    TN->RMS[TN->Neccl]=_RMS;
      TN->SideLeak[TN->Neccl]=_SideLeak;
      TN->DeadLeak[TN->Neccl]=_DeadLeak;
      int i;
      for(i=0;i<3;i++)TN->Coo[TN->Neccl][i]=_Coo[i];
      TN->NHits[TN->Neccl]=_NHits;
      if(_NHits){
      TN->pLeft[TN->Neccl]=_pHit[0]->getpos();
      if (AMSEcalHit::Out(IOPA.WriteAll%10==1)){
      // Writeall
      for(i=0;i<_plane;i++){
      AMSContainer *pc=AMSEvent::gethead()->getC("AMSEcalHit",i);
      #ifdef __AMSDEBUG__
      assert(pc != NULL);
      #endif
      TN->pLeft[TN->Neccl]+=pc->getnelem();
      }
      }
      else{
      // Writeall
      for(int i=0;i<_plane;i++){
      AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->getheadC("AMSEcalHit",i);
      while(ptr && ptr->checkstatus(AMSDBc::USED)){
      TN->pLeft[TN->Neccl]++;
      ptr=ptr->next();
      }
      }
      }
      }
      else TN->pLeft[TN->Neccl]=-1;
      TN->Neccl++;
    */
  }
}
void Ecal1DCluster::_copyEl(){
#ifdef __WRITEROOT__ 
  if(PointerNotSet())return; 
  EcalClusterR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->EcalCluster(_vpos);
  for (int i=0; i<_NHits; i++) {
    if(_pHit[i])ptr.fEcalHit.push_back(_pHit[i]->GetClonePointer());
  }
#endif
}


integer Ecal1DCluster::build(int rerun){
  if(rerun){
    //  remove everything
    for(int i=0;i<2;i++){
      AMSEvent::gethead()->getC("Ecal1DCluster",i)->eraseC();
    }
    const integer  maxpl=2*ECALDBc::slstruc(3);//real planes
    int removed=0;
    for(int i=0;i<maxpl;i++){
      AMSContainer *pc=AMSEvent::gethead()->getC("AMSEcalHit",i);
      for(AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->
	    getheadC("AMSEcalHit",i);ptr;ptr=ptr->next()){

	while(ptr->next() && ptr->next()->checkstatus(AMSDBc::RECOVERED)){
	  pc->removeEl(ptr,1);       
	  removed++;
	}
      }
    }
    //   cout <<"  removed **** "<<removed<<endl;
  }
  const integer Maxcell=2*ecalconst::ECPMSMX;
  integer maxcell=Maxcell;
  integer mincell=0;
  integer statusa[Maxcell+3];
  number adc[Maxcell+3];
  AMSEcalHit * ptrh[Maxcell+3];
  const integer  maxpl=2*ECALDBc::slstruc(3);//real planes
  for(int ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-plane loop
    AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->
      getheadC("AMSEcalHit",ipl,1);
    integer proj=0;
    if(ptr){
      for(int ii=0;ii<Maxcell+3;ii++){
        adc[ii]=0;
	statusa[ii]=0;
	ptrh[ii]=0;
      }
      //      VZERO(adc,sizeof(adc)/sizeof(number));
      //      VZERO(statusa,sizeof(statusa)/sizeof(integer));
      //      VZERO(ptrh,sizeof(ptrh)/sizeof(integer));
      proj=ptr->getproj();
    }
    number emax=-1;
    while(ptr){
      number edep=ptr->getedep();      
      if(!ptr->checkstatus(AMSDBc::BAD) && edep>emax){
        emax=edep;
      }
#ifdef __AMSDEBUG__
      if(ptr->getcell()>=Maxcell){
	cerr<<"Ecal1DCluster::buils-S-wrong cell "<<ptr->getcell()<<endl;
	continue;
      }
#endif
      if(ptr->checkstatus(AMSDBc::BAD))adc[ptr->getcell()]=-edep;
      else {
	ptrh[ptr->getcell()]=ptr;
	adc[ptr->getcell()]=edep;
      }
      ptr=ptr->next();
    }
    if(emax>0){


      //  Check BadChannels

      

      for (int i=mincell;i<maxcell;i++){
	AMSECIds ids(ipl,i);
	if(adc[i]<0 || (adc[i]==0 && ids.LCHisBad()))mincell++;
	else break;
      }
      for (int i=maxcell-1;i>=0;i--){
	AMSECIds ids(ipl,i);
	if(adc[i]<0 || (adc[i]==0 && ids.LCHisBad()))maxcell--;
	else break;
      }

      for (int i=mincell;i<maxcell;i++){
	AMSECIds ids(ipl,i);

	if((adc[i]<0 || (adc[i]==0 && ids.LCHisBad()))){
	  bool bl=i-1>=0;
	  if(bl){
	    AMSECIds ids1(ipl,i-1);
	    bl =bl && ids1.LCHisBad() && adc[i-1]<=0;
	  }
	  bool br= i+1<Maxcell;
	  if(br){
	    AMSECIds ids1(ipl,i+1);
	    br =br && ids1.LCHisBad() && adc[i+1]<=0;
	  }
	  if(!bl && !br){
	    adc[i]=(adc[i-1]+adc[i+1])/2;
	    if(adc[i]){
	      statusa[i]|=AMSDBc::LEAK;
	      int st=statusa[i] | AMSDBc::RECOVERED;
	      AMSECIds ids(ipl,i);
	      AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2),ids.makesswid());
	      //          cout <<" added****a "<<endl;
	      AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",ipl), pnew);
	      ptrh[i]=pnew;
         
	    }
	  }
	  else if(!bl){
	    adc[i]=(adc[i-1])/2;
	    if(adc[i]){
	      statusa[i]|=AMSDBc::LEAK;
	      int st=statusa[i] | AMSDBc::RECOVERED;
	      AMSECIds ids(ipl,i);
	      AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2),ids.makesswid());
	      //          cout <<" added****b "<<endl;
	      AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",ipl), pnew);
	      ptrh[i]=pnew;
         
	    }
	  }
	  else if(!br){
	    adc[i]=(adc[i+1])/2;
	    if(adc[i]){
	      statusa[i]|=AMSDBc::LEAK;
	      AMSECIds ids(ipl,i);
	      int st=statusa[i] | AMSDBc::RECOVERED;
	      AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2),ids.makesswid());
	      //          cout <<" added****c "<<endl;
	      AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",ipl), pnew);
	      ptrh[i]=pnew;
	    }
	  }
	  else{
	    statusa[i]|=AMSDBc::CATLEAK;
	    adc[i]=0;
	  }
	}
      }

      //  Start Cluster Search
      
      number Thr=max(min(emax,number(ECREFFKEY.Thr1DSeed)),emax*ECREFFKEY.Thr1DRSeed);

      integer status=0;
      number ref=-FLT_MAX;
      for (int i=mincell;i<maxcell+1;i++){
	if(adc[i]<ref && adc[i+1]<ref){
          if( adc[i]< adc[i+1] && adc[i+1]> Thr && adc[i+2]>adc[i+1]){
	    status|=AMSDBc::WIDE;
          }
          else if(adc[i+1]<adc[i+2] && adc[i+2]>Thr){
	    status|=AMSDBc::NEAR;
          }
          int center=i-1;
          int left=mincell;
          int right=maxcell-1;
          for(int k=center-1;k>=mincell;k--){
            if(adc[k]<=0){  
	      left=k+1;
	      break;
            }
          }
          for(int k=center+1;k<maxcell;k++){
	    if((adc[k]>adc[k-1] && adc[k]>Thr && adc[k+1]>adc[k]) || (adc[k]==0) ){
	      right=k-1;
	      break;
	    }
          }
	  //        Getting Coordinates

          integer ileft=center-ECREFFKEY.Cl1DCoreSize;

          if( ileft<left)ileft=left;
          integer iright=center+ECREFFKEY.Cl1DCoreSize;
          if(iright>right)iright=right;
          integer ilr=min(iright-center,center-ileft);
          ileft=center-ilr;
          iright=center+ilr; 
	  number w=0;
	  number cx=0;
	  number cz=0;
          for(int k=ileft;k<=iright;k++){
	    number e;
	    if(k==center+1 && (status & AMSDBc::WIDE)){
              ptrh[k]->setstatus(AMSDBc::WIDE);
	      e=adc[k]/2;
	    }
	    else {
	      e=adc[k];
	    }
	    cx+=ECALDBc::CellCoo(ipl,k,0)*e;
	    cz+=ECALDBc::CellCoo(ipl,k,2)*e;
	    w+=e;
          }
          if(w){
            cx/=w;
            cz/=w;
          }
          AMSPoint coo(0,0,cz);
          coo[proj]=cx;
	  //   getting Energies

	  number ec=0;
	  number ec3=0;
	  number ec5=0;
	  number ec9=0;
	  number leak=0;
	  number dead=0;
	  integer ir=right-center;
	  integer il=center-left;
	  number x2=0;
	  number x=0;
	  if(left==mincell && il<ECREFFKEY.Cl1DLeakSize && il<ir){
	    if(il>0)status|= AMSDBc::LEAK;
	    else status|= AMSDBc::CATLEAK;
	    for(int k=center+il+1;k<=center+min(ECREFFKEY.Cl1DLeakSize,ir);k++){
	      ec+=adc[k];
	      leak+=adc[k];
	      if(k-center<2)ec3+=adc[k];
	      if(k-center<3)ec5+=adc[k];
	      if(k-center<5)ec9+=adc[k];
	    } 
	  }
	  else if(right==maxcell-1 && ir<il && ir<ECREFFKEY.Cl1DLeakSize){
	    if(ir>0)status|= AMSDBc::LEAK;
	    else status|= AMSDBc::CATLEAK;
	    for(int k=max(left,center-ECREFFKEY.Cl1DLeakSize);k<=center-ir-1;k++){
	      ec+=adc[k];
	      leak+=adc[k];
	      if(k-center<2)ec3+=adc[k];
	      if(k-center<3)ec5+=adc[k];
	      if(k-center<5)ec9+=adc[k];
	    }
	  }
	  for(int k=left;k<=right;k++){
	    if(ptrh[k])(ptrh[k])->setstatus(AMSDBc::USED);
	    if(statusa[k] & AMSDBc::LEAK && adc[k]>0){
	      status|=AMSDBc::LEAK;
	      dead+=adc[k];
	    }
	    if(statusa[k] & AMSDBc::CATLEAK){
	      status|=AMSDBc::CATLEAK;
	    }
	    number e;
	    if(k==center+1 && (status & AMSDBc::WIDE)){
	      ptrh[k]->setstatus(AMSDBc::WIDE);
	      e=adc[k]/2;
	    }
	    else e=adc[k];
	    if(abs(k-center)<2){
	      ec3+=e;
	    }
	    if(abs(k-center)<3){
	      ec5+=e;
	    }
	    if(abs(k-center)<5){
	      ec9+=e;
	    }
	    ec+=e;
	    x+=ECALDBc::CellCoo(ipl,k,0)*e;
	    x2+=ECALDBc::CellCoo(ipl,k,0)*ECALDBc::CellCoo(ipl,k,0)*e;
	    adc[k]+=-e;
	  }
	  if(ec){
	    x/=ec;
	    x2/=ec;
	    x2=sqrt(fabs(x2-x*x));
	  }
	  //           if(AMSJob::gethead()->isRealData() && AMSEvent::gethead()->getrun()<1213469291){
	  //            coo[0]+=0.15;
	  //            coo[1]+=-0.1;
	  //coo[2]+=1.5;
	  //           }
     
	  AMSEvent::gethead()->addnext(AMSID("Ecal1DCluster",proj),new Ecal1DCluster(status,proj,ipl,left,right,center,ec,ec3,ec5,ec9,leak,dead,coo,w,x2,ptrh));
	  //     cout<<" 1d cluster found proj "<<proj<<" plane "<<ipl <<" center "<<center <<" ec "<<ec<<" coo "<<coo<<endl;
	  ref=-FLT_MAX;
	}
	else if(adc[i]>Thr){
	  ref=adc[i];
	}
      }
      // Count Orphan clusters
      number ec=0;
      number coox=0;
      number coox2=0;
      for (int i=mincell;i<maxcell;i++){
	if(ptrh[i] && !ptrh[i]->checkstatus(AMSDBc::USED)){
	  ec+=adc[i];
	  coox+=adc[i]*ECALDBc::CellCoo(ipl,i,0);
	  coox2+=adc[i]*ECALDBc::CellCoo(ipl,i,0)*ECALDBc::CellCoo(ipl,i,0);
	}
      }
      if(ec){
	status=AMSDBc::JUNK  ;
	AMSPoint coo(0,0,ECALDBc::CellCoo(ipl,0,2));
	coox/=ec;
	coo[proj]=coox;
	coox2/=ec;
	coox2=sqrt(fabs(coox2-coox*coox));
     
	if(AMSJob::gethead()->isRealData() && AMSEvent::gethead()->getrun()<1213469291){
	  //coo[2]+=1.5;
	}
	AMSEvent::gethead()->addnext(AMSID("Ecal1DCluster",proj),new Ecal1DCluster(status,proj,ipl,0,0,0,-ec,0,0,0,0,0,coo,0,coox2,0));
      }
    }
  }//--->endof SubCell-planes loop
  return 1;
}

number Ecal1DCluster::Distance(Ecal1DCluster *p){
  if(_proj==p->getproj()){
    number dz=_Coo[2]-p->getcoo()[2];
    if(dz){
      return fabs(_Coo[_proj]-p->getcoo()[_proj])/fabs(dz);
    }
  }
  return FLT_MAX;
}




void AMSEcal2DCluster::_writeEl(){

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    /*
    // Fill the ntuple
    Ecal2DClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_ec2dclust();

    if (TN->Nec2dcl>=MAXEC2DCLUST) return;

    TN->Status[TN->Nec2dcl]=getstatus();
    TN->Proj[TN->Nec2dcl]=_proj;
    TN->Nmemb[TN->Nec2dcl]=_NClust;
    TN->Edep[TN->Nec2dcl]=_EnergyC;
    //    TN->Edep[TN->Nec2dcl][1]=_Energy3C;
    //    TN->Edep[TN->Nec2dcl][2]=_Energy5C;
    //    TN->SideLeak[TN->Nec2dcl]=_SideLeak;
    //    TN->DeadLeak[TN->Nec2dcl]=_DeadLeak;
    TN->Coo[TN->Nec2dcl]=_Coo;
    TN->Tan[TN->Nec2dcl]=_Tan;
    TN->Chi2[TN->Nec2dcl]=_Chi2;

    for(int i=0;i<maxp;i++)TN->pCl[TN->Nec2dcl][i]=0;
    int realp=min(_NClust,maxp);
    for(int i=0;i<realp;i++) {
    int pat=_pCluster[i]->getproj();
    TN->pCl[TN->Nec2dcl][i]=_pCluster[i]->getpos();
    if (Ecal1DCluster::Out(IOPA.WriteAll%10==1)){
    for(int j=0;j<pat;j++){
    AMSContainer *pc=AMSEvent::gethead()->getC("Ecal1DCluster",j);
    TN->pCl[TN->Nec2dcl][i]+=pc->getnelem();
    }
    }
    else{
    for(int j=0;j<pat;j++){
    Ecal1DCluster *ptr=( Ecal1DCluster*)AMSEvent::gethead()->getheadC(" Ecal1DCluster",j);
    while(ptr && ptr->checkstatus(AMSDBc::USED)){
    TN->pCl[TN->Nec2dcl][i]++;
    ptr=ptr->next();
    }
    }
    }
    //        cout <<"pos "<<i<<" "<< TN->pCl[TN->Nec2dcl][i]<<endl;
    }

    TN->Nec2dcl++;
    */
  }
}

void AMSEcal2DCluster::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  Ecal2DClusterR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Ecal2DCluster(_vpos);
  for (int i=0; i<_NClust; i++) {
    if(_pCluster[i])ptr.fEcalCluster.push_back(_pCluster[i]->GetClonePointer());
  }
#endif
}

integer AMSEcal2DCluster::build(int rerun){
  if(rerun){
    //  remove everything
    AMSEvent::gethead()->getC("Ecal2DCluster",0)->eraseC();
  }



  const integer Maxrow=ecalconst::ECSLMX*2;
     
  for (int proj=0;proj<2;proj++){
    Ecal1DCluster *pshmax=0;
    do{
      pshmax=0;
      Ecal1DCluster *ptr=(Ecal1DCluster*)AMSEvent::gethead()->
	getheadC("Ecal1DCluster",proj,1);
      bool newplane=true;   
      Ecal1DCluster *p1d[Maxrow];
      VZERO(p1d,sizeof(p1d)/sizeof(integer));
      while(ptr){
	if(ptr->Good()){
	  if(!pshmax || pshmax->getEnergy()<ptr->getEnergy())pshmax=ptr;
	  if(newplane){
	    p1d[ptr->getplane()]=ptr;
	    newplane=false;
	  }    
	}
	if(ptr->testlast())newplane=true;
	ptr=ptr->next();
      }
      if(pshmax){
	Ecal1DCluster *p1c[Maxrow];
	VZERO(p1c,sizeof(p1c)/sizeof(integer));
	p1c[pshmax->getplane()]=pshmax;
	Ecal1DCluster *plast=pshmax;
	for(int ipl=pshmax->getplane()+1;ipl<int(ECALDBc::GetLayersNo());ipl++){
	  Ecal1DCluster *pcan=0;
	  for(Ecal1DCluster *p=p1d[ipl];p;p=p->next()){
	    if(p->Good() && p->Distance(plast)<ECREFFKEY.Thr2DMax){
	      pcan=p->EnergyMatch(plast,pcan);
	    }
	    if(p->testlast())break;
	  }
	  p1c[ipl]=pcan;
	  if(pcan)plast=pcan;   
	}

	plast=pshmax;
	for(int ipl=pshmax->getplane()-1;ipl>=0;ipl--){
	  Ecal1DCluster *pcan=0;
	  for(Ecal1DCluster *p=p1d[ipl];p;p=p->next()){
	    if(p->Good() && p->Distance(plast)<ECREFFKEY.Thr2DMax){
	      pcan=p->EnergyMatch(plast,pcan);
	    }
	    if(p->testlast())break;
	  }
	  p1c[ipl]=pcan;
	  if(pcan)plast=pcan;   
	}
	//Now Fit  (No Shower Correction Yet)
	number chi2,t0,tantz;
	integer tot;
	bool reset=false;
	bool suc=StrLineFit(p1c,ECALDBc::GetLayersNo(),proj,reset,NULL,tot,chi2,t0,tantz);
	if(suc && chi2<ECREFFKEY.Chi22DMax){
	  //       cout <<" 2dcluster found proj"<<proj<<" tot "<<tot<<" tantz "<<tantz<<" chi2 "<<chi2<<endl;
	  for(int ipl=0;ipl<int(ECALDBc::GetLayersNo());ipl++){
	    if(p1c[ipl])p1c[ipl]->setstatus(AMSDBc::USED);
	  }
	  AMSEcal2DCluster *pcl=new AMSEcal2DCluster(proj,tot,p1c,t0,tantz,chi2);
	  pcl->_Fit();
	  AMSEvent::gethead()->addnext(AMSID("Ecal2DCluster",0),pcl);
	}
	else pshmax->setstatus(AMSDBc::DELETED);
      }
   
    }while(pshmax);
    // Add Orphaned Clusters in the vicinity of shower
    Ecal1DCluster *ptr=(Ecal1DCluster*)AMSEvent::gethead()->
      getheadC("Ecal1DCluster",proj,1);
    while(ptr){
      if(!ptr->checkstatus(AMSDBc::BAD) && !ptr->checkstatus(AMSDBc::USED) && !ptr->checkstatus(AMSDBc::JUNK)){ 
	AMSEcal2DCluster *p2d=(AMSEcal2DCluster*)AMSEvent::gethead()->
	  getheadC("Ecal2DCluster",0,1);
	number dist=FLT_MAX;
	AMSEcal2DCluster *p2dc=0;
	while(p2d){
	  if(p2d->getproj()==proj){
	    number intercep=p2d->getcoo()+ptr->getcoo()[2]*p2d->gettan();
	    if(fabs(intercep-ptr->getcoo()[proj])<dist){
	      dist=fabs(intercep-ptr->getcoo()[proj]);
	      p2dc=p2d;
	    }
	  }
	  p2d=p2d->next();
	}
	if(p2dc){
	  if(dist<ECREFFKEY.TransShowerSize2D/fabs(cos(atan(p2dc->gettan()))))p2dc->_AddOrphane(ptr);
         
	}
      }
      ptr=ptr->next();
    } 
  }
  return 1;
}


bool AMSEcal2DCluster::StrLineFit(Ecal1DCluster *p1c[],int Maxrow,int proj,bool reset, number *pcorrect, int &tot, number &chi2, number &t0, number &tantz){
  if(reset){
    for(int i=0;i<Maxrow;i++){
      if(p1c[i] && p1c[i]->getproj()==proj)p1c[i]->clearstatus(AMSDBc::DELETED);
    }
  }
  bool again=false;
  bool restore=false;
  number chi2old=0;
  integer ipmaxold=-1;
 AGAIN:
  number z=0;
  number z2=0;
  number t=0;
  number tz=0;
  number e=0;
  tot=0;
  for(int ipl=0;ipl<Maxrow;ipl++){
    if(p1c[ipl]&& p1c[ipl]->getproj()==proj && !p1c[ipl]->checkstatus(AMSDBc::DELETED)){
      number w=p1c[ipl]->getweight();
      AMSPoint coo=p1c[ipl]->getcoo();
      number zc=0;
      if(pcorrect){ 
        zc=pcorrect[ipl];
      }
      z+=(coo[2]+zc)*w;
      z2+=(coo[2]+zc)*(coo[2]+zc)*w;
      t+=coo[p1c[ipl]->getproj()]*w;
      tz+=coo[p1c[ipl]->getproj()]*coo[2]*w;
      e+=w;
      tot++;
    }
  }
  if(tot>ECREFFKEY.Length2DMin && e>0){
    z/=e;
    z2/=e;
    t/=e;
    tz/=e;
    tantz=(tz-t*z)/(z2-z*z);
    t0=t-z*tantz;
    chi2=0;
    number chi2max=0;
    int ipmax=-1;
    for (int ipl=0;ipl<Maxrow;ipl++){
      if(p1c[ipl] && p1c[ipl]->getproj()==proj && !p1c[ipl]->checkstatus(AMSDBc::DELETED)){
	number w=1./p1c[ipl]->PosError();
	AMSPoint coo=p1c[ipl]->getcoo();
	number zc=0;
	if(pcorrect){ 
	  zc=pcorrect[ipl];
	}
	number dx=(coo[p1c[ipl]->getproj()]-tantz*(coo[2]+zc)-t0);
	number delta=(dx*w)*(dx*w);
	if(delta>chi2max){
	  chi2max=delta;
	  ipmax=ipl;
	}    
	chi2+=delta;
      }
    }
    chi2=chi2/(tot-1);
    if(again && chi2>chi2old){
      restore=true;
      again=false;
      p1c[ipmaxold]->clearstatus(AMSDBc::DELETED);
      goto AGAIN;
    }
    number chi2proj=(chi2*(tot-1)-chi2max)/(tot-2);
    if(chi2>0 && !restore && tot>ECREFFKEY.Length2DMin+1 && ((chi2>ECREFFKEY.Chi22DMax/10. && chi2proj/chi2<2*ECREFFKEY.Chi2Change2D) || chi2proj/chi2<ECREFFKEY.Chi2Change2D)){
      again=true;
      chi2old=chi2;
      p1c[ipmax]->setstatus(AMSDBc::DELETED);
      ipmaxold=ipmax;
      goto AGAIN;
    }
    return true;
  }
  return false;
}

void AMSEcal2DCluster::_AddOrphane(Ecal1DCluster *ptr){
  _AddOneCluster(ptr,true);
  _OrpLeak+=ptr->getEnergy();
  // cout <<" orphane found "<<ptr->getEnergy()<<endl;
}

void AMSEcal2DCluster::_AddOneCluster(Ecal1DCluster *ptr, bool addpointer){
  if(addpointer){
    ptr->setstatus(AMSDBc::USED);
    if(_NClust<int(sizeof(_pCluster)/sizeof(_pCluster[0])))_pCluster[_NClust++]=ptr;
    else cerr <<"AMSEcal2DCluster::_AddOneCluster-E-UnabletoAddCluster "<<ptr->getEnergy()<<"  mev energy lost "<<endl;
  }
  _EnergyC+=ptr->getEnergy();
  _SideLeak+=ptr->getsleak();
  _DeadLeak+=ptr->getdleak();
  number intercep=getcoo()+ptr->getcoo()[2]*gettan();
  number cm3=3;
  number cm5=5;
  number cm9=1.;
  for(unsigned int i=0;i<ptr->getNHits();i++){
    number cosa=fabs(cos(atan(gettan())));
    if(fabs(intercep-ptr->getphit(i)->getcoot())*cosa<cm3){
      _Energy3C+=ptr->getphit(i)->getedep();
    }
    if(fabs(intercep-ptr->getphit(i)->getcoot())*cosa<cm5){
      _Energy5C+=ptr->getphit(i)->getedep();
    }
    if(fabs(intercep-ptr->getphit(i)->getcoot())*cosa<cm9){
      _Energy9C+=ptr->getphit(i)->getedep();
    }
    _Energy+=ptr->getphit(i)->getedep();
  }

}




void AMSEcal2DCluster::_Fit(){
  _Energy=0;
  _EnergyC=0;
  _Energy3C=0;
  _Energy5C=0;
  _Energy9C=0;
  _SideLeak=0;
  _DeadLeak=0;
  _OrpLeak=0;
  for (int i=0;i<_NClust;i++){
    _AddOneCluster(_pCluster[i]);
  }

}


integer AMSEcalShower::build(int rerun){
  if(rerun){
    //  remove everything
    AMSEvent::gethead()->getC("EcalShower",0)->eraseC();
  }
  int found=0;



  // Loop over 2dcluster
  for(;;){
    AMSEcal2DCluster *p2d=(AMSEcal2DCluster*)AMSEvent::gethead()->
      getheadC("Ecal2DCluster",0,2);

    number BestMatch=FLT_MAX;
    AMSEcal2DCluster *p2dc[2]={0,0};      
    while(p2d){
      if(p2d->Good()){
	AMSEcal2DCluster *p2do=p2d->next();
	while(p2do){
	  if(p2do->Good() && p2do->getproj()!=p2d->getproj()){
	    number cmatch=(p2d->getEnergy()-p2do->getEnergy())/(p2d->getEnergy()+p2do->getEnergy());
	    if(fabs(cmatch)<BestMatch){
	      p2dc[p2do->getproj()]=p2do;
	      p2dc[p2d->getproj()]=p2d;
	      BestMatch=fabs(cmatch);
	    }       
	    break;
	  }               
	  p2do=p2do->next();
	}      
      }
      p2d=p2d->next();       
    }
    if(p2dc[0] && p2dc[1]){
      p2dc[0]->setstatus(AMSDBc::USED);
      p2dc[1]->setstatus(AMSDBc::USED);
      AMSEvent::gethead()->addnext(AMSID("EcalShower",0),new AMSEcalShower(p2dc[0],p2dc[1]));         
      found=1;       
    }
    else break;
  }






  //  2nd pass needed in case there are orphaned 2d clusters

  AMSEcal2DCluster *p2di=(AMSEcal2DCluster*)AMSEvent::gethead()->
    getheadC("Ecal2DCluster",0);

  for( AMSEcal2DCluster * p2d=p2di;p2d;p2d=p2d->next()){
    if(!p2d->checkstatus(AMSDBc::USED)){
      number difosumMin=1.e20;
      AMSEcalShower* peca=0;
      AMSEcalShower *pesi=(AMSEcalShower*)AMSEvent::gethead()->
	getheadC("EcalShower",0);
      for(AMSEcalShower *pes=pesi;pes;pes=pes->next()){
	number enx=pes->getEnergyXY(0);
	number eny=pes->getEnergyXY(1);
	number adden=p2d->getproj()==0?p2d->getEnergy():-p2d->getEnergy();
	number difosum=fabs(enx-eny+adden)/(enx+eny+fabs(adden));
	if(difosum<difosumMin){
	  peca=pes;
	  difosumMin=difosum;
	}
      }
      if(peca){
	peca->AddOrphan(p2d);
      }
    }
  }
  AMSEcalShower *pesi=(AMSEcalShower*)AMSEvent::gethead()->
    getheadC("EcalShower",0);
  for(AMSEcalShower *pes=pesi;pes;pes=pes->next()){
    if(rerun)pes->_AttCorr();
    pes->EnergyFit();
    if(rerun){         
      pes->ProfileFit();
    }
    pes->DirectionFit();
    if(rerun){         
      pes->EMagFit();
      pes->SphFit();
    }
    else pes->_AttCorr();
    AMSgObj::BookTimer.start("ReEcalLAPPFit");
    // LAPP
    pes->LAPPVariables();    
    pes->ZProfile();    
    AMSgObj::BookTimer.stop("ReEcalLAPPFit");
  }
  return found;
}


AMSEcalShower::AMSEcalShower(AMSEcal2DCluster *px, AMSEcal2DCluster *py):AMSlink(),_AttLeak(0),_NLinLeak(0),_Et(0){
  _Orp2DEnergy=0;
  _pCl[0]=px;
  _pCl[1]=py;
  _N2dCl=2;
  _Direction=0;



  /*
  // At the moment just a skeleton 

  number cofg[3]={0,0,0};
  number edep[3]={0,0,0};
  number cofgy=0;
  number _ECalShowerMax=0;
  number ecalmax=0;
  for(int ipl=0;ipl<2*ECALDBc::slstruc(3);ipl++){ //loop over containers(planes)
  AMSEcalHit* ptr=(AMSEcalHit*)AMSEvent::gethead()->getheadC("AMSEcalHit",ipl,0);
  number ecal=0;
  number ecalx=0;
  number ecaly=0;
  while(ptr){
  ecal+=ptr->getedep();
  if(ecal>ecalmax){
  ecalmax=ecal;
  _ECalShowerMax=ptr->getcooz();
  }
  edep[ptr->getproj()]+=ptr->getedep();
  cofg[ptr->getproj()]+=ptr->getcoot()*ptr->getedep();
  cofg[2]=ptr->getcooz()*ptr->getedep();
  edep[2]+=ptr->getedep();
  ptr=ptr->next();
  }
  }
  integer pr,pl,ce;
  number cl,ct;
  number EcalFirstPlaneZ;
  ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,EcalFirstPlaneZ);
  for(int k=0;k<3;k++){
  if(edep[k]>0)cofg[k]/=edep[k];
  }
  if(edep[2]>0){
  AMSEvent::gethead()->addnext(AMSID("EcalShower",0),new AMSEcalShower(AMSPoint(cofg),edep[2]/1000.,EcalFirstPlaneZ-_ECalShowerMax)); 
  }
  */


}


void AMSEcalShower::_writeEl(){

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    /*
      EcalShowerNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecshow();

      if (TN->Necsh>=MAXECSHOW) return;

      // Fill the ntuple
      TN->Status[TN->Necsh]=_status;
      for(int i=0;i<3;i++)TN->Dir[TN->Necsh][i]=_Dir[i];
      for(int i=0;i<3;i++)TN->EMDir[TN->Necsh][i]=_EMDir[i];
      for(int i=0;i<3;i++)TN->Entry[TN->Necsh][i]=_EntryPoint[i];
      //    for(int i=0;i<3;i++)cout <<"  entry "<<TN->Entry[TN->Necsh][i]<<endl;
      for(int i=0;i<3;i++)TN->Exit[TN->Necsh][i]=_ExitPoint[i];
      for(int i=0;i<3;i++)TN->CofG[TN->Necsh][i]=_CofG[i];
      TN->ErTheta[TN->Necsh]=_Angle3DError;
      TN->Chi2Dir[TN->Necsh]=_AngleTrue3DChi2;
      TN->FirstLayerEdep[TN->Necsh]=_FrontEnergyDep;
      TN->EnergyC[TN->Necsh]=_EnergyC;
      TN->Energy3C[TN->Necsh][0]=_Energy3C;
      TN->Energy3C[TN->Necsh][1]=_Energy5C;
      TN->Energy3C[TN->Necsh][2]=_Energy9C;
      TN->ErEnergyC[TN->Necsh]=_ErrEnergyC;
      TN->DifoSum[TN->Necsh]=_DifoSum;
      TN->SideLeak[TN->Necsh]=_SideLeak;
      TN->RearLeak[TN->Necsh]=_RearLeak;
      TN->DeadLeak[TN->Necsh]=_DeadLeak;
      TN->OrpLeak[TN->Necsh]=_OrpLeak;
      TN->AttLeak[TN->Necsh]=_AttLeak;
      TN->NLinLeak[TN->Necsh]=_NLinLeak;
      TN->Orp2DEnergy[TN->Necsh]=_Orp2DEnergy;
      TN->Chi2Profile[TN->Necsh]=_ProfilePar[4+_Direction*5];
      for(int i=0;i<4;i++){
      if(_ProfilePar[i+_Direction*5] >FLT_MAX){
      _ProfilePar[i+_Direction*5]=FLT_MAX;
      }         
      }
      for(int i=0;i<4;i++)TN->ParProfile[TN->Necsh][i]=_ProfilePar[i+_Direction*5];
      TN->Chi2Trans[TN->Necsh]=_TransFitChi2;
      for(int i=0;i<3;i++)TN->SphericityEV[TN->Necsh][i]=_SphericityEV[i];       
      TN->N2DCl[TN->Necsh]  = _N2dCl;
      for(int i=0;i<2;i++)TN->p2DCl[TN->Necsh][i]=_pCl[i]->getpos();
      TN->Necsh++;
    */
  }
}

void AMSEcalShower::DirectionFit(){

  // correct tan(theta) /shower profile dependence  
  // by linear extrapolation
  // (later by profilefit?)

  const integer Maxrow=ecalconst::ECSLMX*2;
  Ecal1DCluster *p1c[Maxrow+1];
  VZERO(p1c,sizeof(p1c)/sizeof(integer));
  for (int proj=0;proj<2;proj++){
    for (int i=0;i<_pCl[proj]->getNClustKernel();i++){
      Ecal1DCluster *p=_pCl[proj]->getpClust(i);
      p1c[p->getplane()]=p;
    }
  }

  
  integer tot[2];
  number chi2[2],t0[2],tantz[2];
  for(int proj=0;proj<2;proj++){
    AMSEcal2DCluster::StrLineFit(p1c,ECALDBc::GetLayersNo(),proj,true,_Zcorr,tot[proj],chi2[proj],t0[proj],tantz[proj]);
  }

  integer pr,pl,ce;
  number cl,ct;
  if(_Direction==0){
    ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_EntryPoint[2]);
    ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,0,2,pr,pl,ce,ct,cl,_ExitPoint[2]);
  }
  else{
    ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_ExitPoint[2]);
    ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,0,2,pr,pl,ce,ct,cl,_EntryPoint[2]);
  }
  bool zcorr[2]={true,true};      
  for (int proj=0;proj<2;proj++){
    // Renonce if chi2 is bad;
    if(chi2[proj]>1 && chi2[proj]*ECREFFKEY.Chi2Change2D>_pCl[proj]->_Chi2 ){
      t0[proj]=_pCl[proj]->_Coo;
      tantz[proj]=_pCl[proj]->_Tan;
#ifdef __AMSDEBUG__
      cerr<<" AMSEcalShower::DirectionFit-W-correction Failed for proj "<<proj<<" new chi2 "<<chi2[proj]<<" old chi2 "<<_pCl[proj]->_Chi2<<endl;
#endif
      chi2[proj]=_pCl[proj]->_Chi2;
      zcorr[proj]=false;
    }
    _EntryPoint[proj]=t0[proj]+tantz[proj]*_EntryPoint[2];
    _ExitPoint[proj]=t0[proj]+tantz[proj]*_ExitPoint[2];
  }
  _AngleTrue3DChi2=getTrue3DChi2(tantz,t0,zcorr);
  _AngleTrue3DChi2/=_Chi2Corr();
  _Dir=_ExitPoint-_EntryPoint;
  _Angle3DChi2=(chi2[0]*(tot[0]-1)+chi2[1]*(tot[1]-1))/(tot[0]+tot[1]-2);
  _Angle3DChi2/=_Chi2Corr();
  if(max(fabs(_ExitPoint[0]),fabs(_ExitPoint[1]))>ECREFFKEY.CalorTransSize)setstatus(AMSDBc::CATLEAK);
  if(max(fabs(_EntryPoint[0]),fabs(_EntryPoint[1]))>ECREFFKEY.CalorTransSize)setstatus(AMSDBc::CATLEAK);

  //    Get corrected EM dir
  AMSPoint Entry(0,0,_EntryPoint[2]);      
  AMSPoint Exit(0,0,_ExitPoint[2]);      
  for(int i=0;i<2;i++){
    Entry[i]=_pCl[i]->getcoo()+_pCl[i]->gettan()*ECREFFKEY.EMDirCorrection*Entry[2];
    Exit[i]=_pCl[i]->getcoo()+_pCl[i]->gettan()*ECREFFKEY.EMDirCorrection*Exit[2];
  }
  _EMDir=Exit-Entry;
  integer front=_Direction==0?0:ECALDBc::GetLayersNo()-1;
  _FrontEnergyDep=0;
  AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->
    getheadC("AMSEcalHit",front,1);
  while(ptr){
    if(!ptr->checkstatus(AMSDBc::BAD)){
      int proj=ptr->getproj();
      number inter=_EntryPoint[proj]+_Dir[proj]/_Dir[2]*(ptr->getcooz()-_EntryPoint[2]);
      if(fabs(inter-ptr->getcoot())<ECALDBc::CellSize(front,ptr->getcell(),0)){
        _FrontEnergyDep+=ptr->getedep();
      }
    }
    ptr=ptr->next();

  }
     
 
}

number AMSEcal2DCluster::_ZCorr(Ecal1DCluster * p1c[],integer ipl, integer iplmax){
  bool zdone[2]={false,false};
  number zcorr[2]={0,0};
  for(int i=ipl-1;i>=0;i--){
    if(p1c[i]){
      number e1=p1c[i]->getEnergy();
      number z1=p1c[i]->getcoo()[2];
      number e2=p1c[ipl]->getEnergy();
      number z2=p1c[ipl]->getcoo()[2];
      zcorr[0]=(e2-e1)/(e1+e2)/3*(z2-z1);   
      zdone[0]=true;
      break;
    }
  }
  for(int i=ipl+1;i<iplmax;i++){
    if(p1c[i]){
      number e1=p1c[i]->getEnergy();
      number z1=p1c[i]->getcoo()[2];
      number e2=p1c[ipl]->getEnergy();
      number z2=p1c[ipl]->getcoo()[2];
      zcorr[1]=(e2-e1)/(e1+e2)/3*(z2-z1);   
      zdone[1]=true;
      break;
    }
  }
  if(zdone[0] && zdone[1]){
    return (zcorr[0]+zcorr[1])/2;
  }
  else if(zdone[0]){
    return zcorr[0];
  }
  else return zcorr[1];
 



}



void AMSEcalShower::EnergyFit(){

  number energy=0;
  _EnergyC=0;
  _EnergyPIC=0;
  _EnergyA=0;

  _Energy3C=0;
  _Energy5C=0;
  _Energy9C=0;

  _SideLeak=0;
  _DeadLeak=0;
  _RearLeak=0;
  _OrpLeak=0;
  _S13Leak=0;

  _SideLeakPI=0;
  _DeadLeakPI=0;
  _RearLeakPI=0;
  _OrpLeakPI=0;
  _S13LeakXPI=0;
  _S13LeakYPI=0;

  _SideLeakA=0;
  _DeadLeakA=0;
  _RearLeakA=0;
  _OrpLeakA=0;
  _S13LeakXA=0;
  _S13LeakYA=0;
  _S13LeakXA0=0;
  _S13LeakYA0=0;

  _S35LeakXA=0;
  _S35LeakYA=0;
  _VarLeakXA=0;
  _VarLeakYA=0;

  _Nhits=0;
 
  Ecal1DCluster *p1cl;
  const integer Maxrow=ECALDBc::GetLayersNo();
  integer n1cl; 
  int is1[Maxrow];
  float s1[Maxrow];
  float s3[Maxrow];
  //begin LAPP
  int icofgcell[Maxrow];
  float cofgcell[Maxrow];
  const integer Maxcell = ECALDBc::GetCellsNo(); 
  float s1_a[Maxrow][Maxcell]; 
  float s1a[Maxrow];
  float s3a[Maxrow];
  float s5a[Maxrow];
  float edepl[Maxrow];
  float cofgl=0.;
  float cofgcellx=0.;
  float cofgcelly=0.;
  float edept=0.;
  float edepx=0.;
  float edepy=0.;
  int nhitsx=0;
  int nhitsy=0;
  int nhitshx=0;
  int nhitshy=0;
  //float madc[3];
  //end LAPP
  for(int k=0;k<Maxrow;k++){
    is1[k]=-2;
    s1[k]=0;
    s3[k]=0;
    //begin LAPP
    icofgcell[k]=-2;
    cofgcell[k]=0.;
    s1a[k]=0.;
    s3a[k]=0.;
    s5a[k]=0.;
    edepl[k]=0.;
    for( int kk=0;kk< Maxcell;kk++) {
      s1_a[k][kk] = 0; 
    }
    //end LAPP
  }

  for (int proj=0;proj<_N2dCl;proj++){
    energy+=_pCl[proj]->_Energy;   
    //_EnergyC+=_pCl[proj]->_EnergyC;   
    _Energy3C+=_pCl[proj]->_Energy3C;   
    _Energy5C+=_pCl[proj]->_Energy5C;   
    _Energy9C+=_pCl[proj]->_Energy9C;   
    //   Very  primitive side leak estimation (just double it)
    _SideLeak+=2*_pCl[proj]->_SideLeak;
    //_EnergyC+=_pCl[proj]->_SideLeak;   
    //
    _DeadLeak+=_pCl[proj]->_DeadLeak;
    _OrpLeak+=_pCl[proj]->_OrpLeak;
   
    n1cl=_pCl[proj]->getNClust();
    for(int ic=0;ic<n1cl;ic++){
      p1cl=_pCl[proj]->getpClust(ic);
      _Nhits+=p1cl->getNHits();
      for(unsigned int ih=0;ih<p1cl->getNHits();ih++){
	AMSEcalHit * phit=p1cl->getphit(ih);
	if(phit->getedep()>s1[phit->getplane()]){
	  is1[phit->getplane()]=phit->getcell();
	  s1[phit->getplane()]=phit->getedep();
	  s3[phit->getplane()]=phit->getedep();
	}
	//begin LAPP
        if(phit->getedep()>0){           
	  cofgcell[phit->getplane()]+=phit->getcell()*phit->getedep();        
	  edepl[phit->getplane()]+=phit->getedep();
          /*
	  //phit->getadc(madc);          
          //if (madc[0]>4){// count only hits with adc_highgain>4
          */
          if (phit->getedep()>=2){// count only hits with edep >= 2 MeV 
	    // Modified CG 
	    s1_a[phit->getplane()][phit->getcell()] += phit->getedep();  
	    if (((phit->getplane()/2)%2)==0){
	      nhitshy++;
	    }else {
	      nhitshx++;
	    }
          }
        }        
        // end LAPP
      }
    }
  }
  //begin LAPP
  for(int k=0;k<Maxrow;k++){
    if (edepl[k]>0){
      if (((k/2)%2)==0){ 
	edepy+=edepl[k];
	cofgcelly+=cofgcell[k];
      }
      else
	{
	  edepx+=edepl[k];
	  cofgcellx+=cofgcell[k];
	}
      cofgcell[k]/=edepl[k];
      edept+=edepl[k];
    }
    icofgcell[k]=int(cofgcell[k]);
    if ((cofgcell[k]-icofgcell[k])>=0.5)  icofgcell[k]++; 
    // modifief CG - 05/11/2013/ 
    int cog = icofgcell[k];
    int cogm = max(0,cog-1); 
    int cogp = min(71,cog+1); 
    if (s1_a[k][cogm] > s1_a[k][cog] ) cog = cogm; 
    if (s1_a[k][cogp] > s1_a[k][cog] ) cog = cogp; 
    icofgcell[k] = cog; 
    cofgl+=k*edepl[k];
  }
  if (edept>0){ 
    cofgl/=edept;
    if (edepx>0){
      cofgcellx/=edepx;
    }
    else
      {
	cofgcellx=-1;
      }
    if (edepy>0) {
      cofgcelly/=edepy;
    }
    else
      {
	cofgcelly=-1;
      }
  }
  else
    {
      cofgl=-1;
    }
  for (int proj=0;proj<_N2dCl;proj++){
    n1cl=_pCl[proj]->getNClust();
    for(int ic=0;ic<n1cl;ic++){
      p1cl=_pCl[proj]->getpClust(ic);
      for(unsigned int ih=0;ih<p1cl->getNHits();ih++){
	AMSEcalHit * phit=p1cl->getphit(ih);
	//LAPP S1, S3 calculation (M.P. April 26, 2011)
	if((phit->getcell()-icofgcell[phit->getplane()])==0)
	  {
	    s1a[phit->getplane()]=phit->getedep();
	    s3a[phit->getplane()]=s1a[phit->getplane()];
	    s5a[phit->getplane()]=s3a[phit->getplane()];
	  }
      }
    }
  }
  
  //end LAPP
  for(int ipl=0;ipl<Maxrow;ipl++){
    AMSEcalHit *ptr=(AMSEcalHit*)AMSEvent::gethead()->
      getheadC("AMSEcalHit",ipl,1);
    while(ptr){
      if(abs(ptr->getcell()-is1[ipl])==1)s3[ipl]+=ptr->getedep();
      //LAPP S1, S3 calculation (M.P. April 26, 2011)
      if(abs(ptr->getcell()-icofgcell[ipl])==1)
	{
	  s3a[ipl]+=ptr->getedep();
	  s5a[ipl]+=ptr->getedep();
	}
      if(abs(ptr->getcell()-icofgcell[ipl])==2)s5a[ipl]+=ptr->getedep();
      //ptr->getadc(madc);
      //if (madc[0]>4){// count only hits with adc_highgain>4 ADC
      if (ptr->getedep()>=2){// count only hits with edep >=2 MeV
	if (((ptr->getplane()/2)%2)==0){
	  nhitsy++;
	}else {
	  nhitsx++;
	}
      } 
      ////end LAPP
      ptr=ptr->next();
    }
  }
  float ss1=0;
  float ss3=0;
  float ss1p[2]={0};
  float ss3p[2]={0};
  //begin LAPP
  float ss1a[2]={0};
  float ss3a[2]={0};
  float ss5a[2]={0};
  //end LAPP
  int ksl=0;
  int kpr=0;
  for(int k=0;k<Maxrow;k++)
    {
      ss1+=s1[k];
      ss3+=s3[k];

      ksl = (int) k/2;
      //if((ksl%2)==0) kpr= ECALDBc::slstruc(0);
      //else kpr=1-ECALDBc::slstruc(0);
      
      if((ksl%2)==0) kpr=1;
      else kpr=0; 
      
      ss1p[kpr] +=s1[k];
      ss3p[kpr] +=s3[k];
      //begin LAPP
      ss1a[kpr] +=s1a[k];
      ss3a[kpr] +=s3a[k];
      ss5a[kpr] +=s5a[k];
      //end LAPP
    }
  
  _S13Rpi[0]=ss1p[0]/ss3p[0];
  _S13Rpi[1]=ss1p[1]/ss3p[1];
  _S13R=ss1/ss3;
  //LAPP S1/S3 S1/S5 (S1 = Edep at layer CofG cell)  0->X , 1->Y
  _S13Ra[0]=ss1a[0]/ss3a[0];
  _S13Ra[1]=ss1a[1]/ss3a[1];

  _S35Ra[0]=ss3a[0]/ss5a[0];
  _S35Ra[1]=ss3a[1]/ss5a[1];

  // compute S1, S3, S5 (CofG used as seed)
  _S1tot=ss1a[0]+ss1a[1];
  _S3tot=ss3a[0]+ss3a[1];
  _S5tot=ss5a[0]+ss5a[1];
  if ((edepx+edepy)>0){
    _S1tot=_S1tot/(edepx+edepy);
    _S3tot=_S3tot/(edepx+edepy);
    _S5tot=_S5tot/(edepx+edepy);
  }
  // end S1, S3, S5
  _S1totx=0.;
  _S3totx=0.;
  _S5totx=0.;
  if (edepx>0){
    _S1totx=ss1a[0]/edepx;
    _S3totx=ss3a[0]/edepx;
    _S5totx=ss5a[0]/edepx;
  }
  _S1toty=0.;
  _S3toty=0.;
  _S5toty=0.;
  if (edepy>0){
    _S1toty=ss1a[1]/edepy;
    _S3toty=ss3a[1]/edepy;
    _S5toty=ss5a[1]/edepy;
  }

  //end LAPP
  //
  if(energy){
    _Energy3C/=energy;
    _Energy5C/=energy;
    _Energy9C/=energy;
  }

  _CofG=AMSPoint(0,0,0);
  for(unsigned int i=0;i<sizeof(_Zcorr)/sizeof(_Zcorr[0]);i++)_Zcorr[i]=0; 
  number ec=0;
  //float efinal=0;
  _ShowerMax=-1;
  AMSPoint ep(0,0,0);
  VZERO(_Edep,sizeof(_Edep)/sizeof(integer));
  VZERO(_Ez,sizeof(_Ez)/sizeof(integer));
  for (int proj=0;proj<_N2dCl;proj++){
    for (int i=0;i<_pCl[proj]->getNClust();i++){
      Ecal1DCluster *p=_pCl[proj]->getpClust(i);
      if(p->checkstatus(AMSDBc::CATLEAK)){
	setstatus(AMSDBc::CATLEAK);
      }
      int plane=p->getplane();
      _CofG[_pCl[proj]->getproj()]+=p->getEnergy()*p->getcoo()[_pCl[proj]->getproj()];
      _CofG[2]+=p->getEnergy()*p->getcoo()[2];
      _Ez[plane]+=-p->getEnergy()*p->getcoo()[2];
      _Edep[plane]+=p->getEnergy();

      ep[_pCl[proj]->getproj()]+=p->getEnergy();
      ep[2]+=p->getEnergy();
      ec+=p->getEnergy();
    }
  }

  float efrac = _Edep[17]+_Edep[16];

  _ShowerMax=-1;
  if(ec){
    _CofG=_CofG/ep;
    _DifoSum=(ep[0]-ep[1])/(ep[0]+ep[1]);
    number xmax=-1;
    _Dz=0;
    number dn=0;
    for(int i=0;i<Maxrow;i++){
      if(_Edep[i]){
	_Ez[i]/=_Edep[i];
	if(i>0 && _Edep[i-1]>0){
	  _Dz+=_Ez[i]-_Ez[i-1];
	  dn++;
	}
      }
      _Edep[i]/=ec;
      if(_Edep[i]>xmax){
	xmax=_Edep[i];
	_ShowerMax=i;
      }
    }
    if(dn)_Dz/=dn;
    else{
      //  try whatever it is
      for(int i=0;i<Maxrow;i++){
	if(_Edep[i]){
	  for(int j=i+1;j<Maxrow;j++){
	    if(_Edep[j]){
	      _Dz+=(_Ez[j]-_Ez[i])/(j-i);
	      dn++;
	    }
	  }
	}   
      }
      if(dn)_Dz/=dn;
      else{
	cerr<<"AMSEcalShower::EnergyFit-E-CouldNotGet Z info"<<endl;
	_Dz=0.9;
      }
    }
    for(int i=0;i<Maxrow;i++){
      if(!_Edep[i]){
	for(int j=i-1;j>=0;j--){
	  if(_Edep[j]){
	    _Ez[i]=_Ez[j]+(i-j)*_Dz;
	    break;
	  }
	}
	for(int j=i+1;j<Maxrow;j++){
	  if(_Edep[j]){
	    _Ez[i]=_Ez[i]!=0?(_Ez[i]+_Ez[j]+(i-j)*_Dz)/2:_Ez[j]+(i-j)*_Dz;
	    break;
	  }
	}
      }
    }
    ////LAPP impact-point correction (implemented by M.P. April 6, 2011, updated March 19, 2012)  
    //begin lapp  impact-point correction:
    //X side:
    if(_S13Ra[0]>ECREFFKEY.S1S3XA[3] && _S13Ra[0]<=ECREFFKEY.S1S3XA[2]){
      _S13LeakXA0=ECREFFKEY.S1S3XA[0]*_S13Ra[0]+ECREFFKEY.S1S3XA[1]-1.;
    } 
    else if(_S13Ra[0]<=ECREFFKEY.S1S3XA[3]){ 
      _S13LeakXA0=ECREFFKEY.S1S3XA[0]*ECREFFKEY.S1S3XA[3]+ECREFFKEY.S1S3XA[1]-1.;
    }
    //X-side done
    //beginning Y-side:
    if(_S13Ra[1]>ECREFFKEY.S1S3YA[3] && _S13Ra[1]<=ECREFFKEY.S1S3YA[2]){
      _S13LeakYA0=ECREFFKEY.S1S3YA[0]*_S13Ra[1]+ECREFFKEY.S1S3YA[1]-1.;
    }
    else if(_S13Ra[1]<=ECREFFKEY.S1S3YA[3]){
      _S13LeakYA0=ECREFFKEY.S1S3YA[0]*ECREFFKEY.S1S3YA[3]+ECREFFKEY.S1S3YA[1]-1.;

    }      
   

    //begin lapp  impact-point correction for S35R x, y and ShowerLatDisp x, y:
    //X side:
    if(_S13Ra[0]>ECREFFKEY.S3S5XA[0] && _S13Ra[0]<=ECREFFKEY.S3S5XA[1]){
      _S35LeakXA=((ECREFFKEY.S3S5XA[2]+ECREFFKEY.S3S5XA[3]*_S13Ra[0])/(ECREFFKEY.S3S5XA[2]+ECREFFKEY.S3S5XA[3]*ECREFFKEY.S3S5XA[1]))-1.;
      _VarLeakXA=((ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*_S13Ra[0])/(ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[1]))-1.;
    } 
    else if(_S13Ra[0]<=ECREFFKEY.S3S5XA[0] ){ 
      _S35LeakXA=((ECREFFKEY.S3S5XA[2]+ECREFFKEY.S3S5XA[3]*ECREFFKEY.S3S5XA[0])/(ECREFFKEY.S3S5XA[2]+ECREFFKEY.S3S5XA[3]*ECREFFKEY.S3S5XA[1]))-1.;
      _VarLeakXA=((ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[0])/(ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[1]))-1.;
    }
    //X-side done
    //beginning Y-side:
    if(_S13Ra[1]>ECREFFKEY.S3S5YA[0] && _S13Ra[1]<=ECREFFKEY.S3S5YA[1]){
      _S35LeakYA=((ECREFFKEY.S3S5YA[2]+ECREFFKEY.S3S5YA[3]*_S13Ra[1])/(ECREFFKEY.S3S5YA[2]+ECREFFKEY.S3S5YA[3]*ECREFFKEY.S3S5YA[1]))-1.;
      _VarLeakYA=((ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*_S13Ra[1])/(ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[1]))-1.;
    } 
    else if(_S13Ra[1]<=ECREFFKEY.S3S5YA[0] ){ 
      _S35LeakYA=((ECREFFKEY.S3S5YA[2]+ECREFFKEY.S3S5YA[3]*ECREFFKEY.S3S5YA[0])/(ECREFFKEY.S3S5YA[2]+ECREFFKEY.S3S5YA[3]*ECREFFKEY.S3S5YA[1]))-1.;
      _VarLeakYA=((ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[0])/(ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[1]))-1.;
    }   
    /// end lapp  impact-point correction for S35R x, y
    /*  
    //begin lapp  impact-point correction for ShowerLatDisp x, y:
    //X side:
    if(_S13Ra[0]>ECREFFKEY.VarXA[0] && _S13Ra[0]<=ECREFFKEY.VarXA[1]){
    _VarLeakXA=((ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*_S13Ra[0])/(ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[1]))-1.;
    } 
    else if(_S13Ra[0]<=ECREFFKEY.VarXA[0] ){ 
    _VarLeakXA=((ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[0])/(ECREFFKEY.VarXA[2]-ECREFFKEY.VarXA[3]*ECREFFKEY.VarXA[1]))-1.;
    }
    //X-side done
    //beginning Y-side:
    if(_S13Ra[1]>ECREFFKEY.VarYA[0] && _S13Ra[1]<=ECREFFKEY.VarYA[1]){
    _VarLeakYA=((ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*_S13Ra[1])/(ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[1]))-1.;
    } 
    else if(_S13Ra[1]<=ECREFFKEY.VarYA[0] ){ 
    _VarLeakYA=((ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[0])/(ECREFFKEY.VarYA[2]-ECREFFKEY.VarYA[3]*ECREFFKEY.VarYA[1]))-1.;
    }   
    /// end lapp  impact-point correction for ShowerLatDisp x, y
    */
    // Apply Impact point correction to S35Ra:
    _S35Ra[0]=_S35Ra[0]/(1 +_S35LeakXA);
    _S35Ra[1]=_S35Ra[1]/(1 +_S35LeakYA);
    //done Impact point correction to S35Ra

    edepx=ep[0]/1000.;// redefine edepx from ep and express in GeV
    edepy=ep[1]/1000.;// redefine edepy from ep and express in GeV

    float erec_ipcorr= edepx/(1+_S13LeakXA0) + edepy/(1+_S13LeakYA0);  //energy w impact-point correction  (in GeV)

    _Energy0A[0]=edepx/(1+_S13LeakXA0); //  Energy X-side w impact point correction in GeV
    _Energy0A[1]=edepy/(1+_S13LeakYA0);  // Energy Y-side w impact point correction in GeV

    //end LAPP impact point-correction

    // begin LAPP hit multiplicity correction
    float hitrx=nhitsx/((float)nhitshx);
    float hitry=nhitsy/((float)nhitshy);

    float hitleakx=(edepx+ECREFFKEY.LAPPHitLeak[1])/(ECREFFKEY.LAPPHitLeak[2]*(hitrx-1)+edepx+ECREFFKEY.LAPPHitLeak[1])-1.;
    float hitleaky=(edepy+ECREFFKEY.LAPPHitLeak[3])/(ECREFFKEY.LAPPHitLeak[4]*(hitry-1)+edepy+ECREFFKEY.LAPPHitLeak[3])-1.;

    _S13LeakXA=_S13LeakXA0+hitleakx+_S13LeakXA0*hitleakx;
    _S13LeakYA=_S13LeakYA0+hitleaky+_S13LeakYA0*hitleaky;

    float erec_new= edepx/(1+_S13LeakXA) + edepy/(1+_S13LeakYA); // impact-point and  hitmult corrections (GeV)

    erec_new= erec_new/ECREFFKEY.LAPPHitLeak[0];

    // end LAPP hit multiplicity correction

    //Beginning LAPP rear leakage correction (implemented by M.P. on April 6, 2011, updated 20 March 2012)
   
    float frac2=(edepl[17]+ edepl[16])/((1+_S13LeakYA0)*erec_ipcorr*1000.);
    //begin corr_leak_f 
      
    float corr_leak_f=1.;

    if (frac2>ECREFFKEY.LAPPRearLeak[4]){
      corr_leak_f=ECREFFKEY.LAPPRearLeak[0]*exp(-ECREFFKEY.LAPPRearLeak[1]*pow(erec_ipcorr/((float)cofgl),ECREFFKEY.LAPPRearLeak[3]))-ECREFFKEY.LAPPRearLeak[2]*frac2;
    }
   
    // if (frac2>0.25){
    //     corr_leak_f=4.234*exp(-1.388*pow(erec_ipcorr/((float)cofgl),0.01))-1.604*frac2;
    //   }      
     
    if ((erec_new>=ECREFFKEY.LAPPRearLeak[5])||(frac2>=ECREFFKEY.LAPPRearLeak[6])) corr_leak_f=1.;// do not apply correction for E>=2TeV or too big Edep in last2layers (frac2 < 0.5) 
    if ((erec_new/corr_leak_f)<0){
      //           setstatus(AMSDBc::CATLEAK);
      EcalJobStat::addre(12);
#ifdef __AMSDEBUG__
      cerr<<"EcalShower::EnergyFit-W-CATLEAKDetected "<< frac2 <<endl;
#endif
      //corr_leak_f=1./FLT_MAX*100; 
      corr_leak_f=1;// 18Apr2012 do not apply correction in this case nor flag
      
    }
 
    _EnergyA=erec_new/corr_leak_f;
    _RearLeakA=_EnergyA-erec_new;

    _SideLeakA=_SideLeak/(1000.*corr_leak_f);
    _OrpLeakA=_OrpLeak/(1000.*corr_leak_f);
    _DeadLeakA=_DeadLeak/(1000.*corr_leak_f);
    _AttLeakA=_AttLeak/(1000.*corr_leak_f);
    _NLinLeakA=_NLinLeak/(1000.*corr_leak_f); 
    //end corr_leak_f
    //end LAPP rear-leakage correction


    // now add s13leak: Choutko version
    _Energy= ec;


    //Pisa version
    // float _S13LeakPI[2]={0.,0.};
    // float nspar[2][6]={{0.529,0.753,0.922,47.25,-104.,-0.006},
    // 		      {0.517,0.743,0.933,49.19,-7.23,-0.193}};

    // for(int ip=0; ip<2; ip++)
    //   {
    //     if(_S13Rpi[ip]>nspar[ip][0] && _S13Rpi[ip]<nspar[ip][1])
    // 	 {
    // 	   _S13LeakPI[ip]=((1-nspar[ip][2])/(nspar[ip][1]-nspar[ip][0]))*(_S13Rpi[ip]-nspar[ip][1]);
    // 	 }
    //     else if(_S13Rpi[ip]<nspar[ip][0])
    // 	 _S13LeakPI[ip]=nspar[ip][2] -1;
    //   }

    //_S13LeakXPI=_S13LeakPI[0];
    //_S13LeakYPI=_S13LeakPI[1];

    if(_S13Rpi[0]>ECREFFKEY.S1S3X[0] && _S13Rpi[0]<ECREFFKEY.S1S3X[1])
      {
	_S13LeakXPI=((1-ECREFFKEY.S1S3X[2])/(ECREFFKEY.S1S3X[1]-ECREFFKEY.S1S3X[0]))*(_S13Rpi[0]-ECREFFKEY.S1S3X[1]);
      }
    else if(_S13Rpi[0]<ECREFFKEY.S1S3X[0])
      _S13LeakXPI=ECREFFKEY.S1S3X[2] -1;
   
    if(_S13Rpi[1]>ECREFFKEY.S1S3Y[0] && _S13Rpi[1]<ECREFFKEY.S1S3Y[1])
      {
	_S13LeakYPI=((1-ECREFFKEY.S1S3Y[2])/(ECREFFKEY.S1S3Y[1]-ECREFFKEY.S1S3Y[0]))*(_S13Rpi[1]-ECREFFKEY.S1S3Y[1]);
      }
    else if(_S13Rpi[1]<ECREFFKEY.S1S3Y[0])
      _S13LeakYPI=ECREFFKEY.S1S3Y[2] -1; 

    ec = ep[0]/(1+_S13LeakXPI) + ep[1]/(1+_S13LeakYPI);

    // Now Add RearLeak : Choutko version
    if(_Edep[Maxrow-1]>ECREFFKEY.SimpleRearLeak[0]){
      number alpha=1-_Edep[Maxrow-1]*ECREFFKEY.SimpleRearLeak[2];
      if(alpha<=0){
	setstatus(AMSDBc::CATLEAK);
	EcalJobStat::addre(10);
#ifdef __AMSDEBUG__
	cerr<<"EcalShower::EnergyFit-W-CATLEAKDetected "<<_Edep[Maxrow-1]<<endl;
#endif
	alpha=ECREFFKEY.SimpleRearLeak[1]*_Energy/FLT_MAX*100;
      }
      //_Energy= ec;
      _EnergyC= ECREFFKEY.SimpleRearLeak[1]*_Energy/alpha;
      _RearLeak= _EnergyC-ECREFFKEY.SimpleRearLeak[1]*_Energy;
      //    cout <<" case 1 "<<_EnergyC<<" "<<_RearLeak<<endl;
    }
    else{
      /*
	_RearLeak= ECREFFKEY.SimpleRearLeak[1]*ec*(_Edep[Maxrow-1]*ECREFFKEY.SimpleRearLeak[2]*_Edep[Maxrow-1]/ECREFFKEY.SimpleRearLeak[0]);
	_EnergyC= ECREFFKEY.SimpleRearLeak[3]*ec+_RearLeak;
      */
      _EnergyC=ECREFFKEY.SimpleRearLeak[3]*_Energy;
      _RearLeak= _EnergyC-ECREFFKEY.SimpleRearLeak[1]*_Energy;
   
      //    cout <<" case 2 "<<_EnergyC<<" "<<_RearLeak<<endl;
    }

    _SideLeakPI=_SideLeak/1000;
    _OrpLeakPI=_OrpLeak/1000;
    _DeadLeakPI=_DeadLeak/1000;
    _AttLeakPI=_AttLeak/1000;
    _NLinLeakPI=_NLinLeak/1000;
   
    _SideLeak=ECREFFKEY.SimpleRearLeak[3]*_SideLeak;
    _OrpLeak=ECREFFKEY.SimpleRearLeak[3]*_OrpLeak;
    _DeadLeak=ECREFFKEY.SimpleRearLeak[3]*_DeadLeak;
    _AttLeak=ECREFFKEY.SimpleRearLeak[3]*_AttLeak;
    _NLinLeak=ECREFFKEY.SimpleRearLeak[3]*_NLinLeak;
  
    //Pisa version
    efrac /= (1+_S13LeakYPI)*ec;
    
    if(efrac>0.01)
      {   
	
	//float EALPHA0=-3.0;
	//float EBETA=75.2;
	//float EGAMMA=563.3;
	//float EALPHA_PAR[2]={-5.,0.0143};
	
	float EshiftInPercent= ECREFFKEY.ealpha0+ ECREFFKEY.ebeta*efrac+ ECREFFKEY.egamma*(pow(efrac,2));
	float ecorr2ebeam=1.+EshiftInPercent/100.;
	_EnergyPIC=ec*ecorr2ebeam;
	
	float alphae =  ECREFFKEY.ealpha_par[0]+ ECREFFKEY.ealpha_par[1]*_EnergyPIC/1000;
	EshiftInPercent=alphae+ ECREFFKEY.ebeta*efrac+ ECREFFKEY.egamma*(pow(efrac,2));
	
	if(EshiftInPercent<=0)
	  {
	    //	    setstatus(AMSDBc::CATLEAK);
	    EcalJobStat::addre(11);
#ifdef __AMSDEBUG__
	    cerr<<"EcalShower::EnergyFit-W-CATLEAKDetected "<<efrac<<endl;
#endif
	    EshiftInPercent=0;
	  }
	
	ecorr2ebeam=1.+EshiftInPercent/100.;
	_EnergyPIC=ec*ecorr2ebeam/1000;
	_RearLeakPI= _EnergyPIC - ec/1000;
      }
    else
      {
	_EnergyPIC= ec/1000.;
	_RearLeakPI=0.;
      }
    
  }
  
  _S13Leak=0;
  if(_S13R>ECREFFKEY.S1S3[2] && _S13R<ECREFFKEY.S1S3[3]){
    _S13Leak=-ECREFFKEY.S1S3[1]*(_S13R-ECREFFKEY.S1S3[0]);
  }
  _EnergyC/=(1-_S13Leak);

  if(_EnergyC){
    _RearLeak/=_EnergyC;
    _OrpLeak/=_EnergyC;
    _DeadLeak/=_EnergyC;
    _SideLeak/=_EnergyC;
    _AttLeak/=_EnergyC;
    _NLinLeak/=_EnergyC;
  }
  
  if(_EnergyPIC){
    _RearLeakPI /=_EnergyPIC;
    _OrpLeakPI/=_EnergyPIC;
    _DeadLeakPI/=_EnergyPIC;
    _SideLeakPI/=_EnergyPIC;
    _AttLeakPI/=_EnergyPIC;
    _NLinLeakPI/=_EnergyPIC;
  }
  //begin LAPP Energy (M.P. April 7,2011)
  if(_EnergyA){
    _RearLeakA/=_EnergyA;
    _OrpLeakA/=_EnergyA;
    _DeadLeakA/=_EnergyA;
    _SideLeakA/=_EnergyA;
    _AttLeakA/=_EnergyA;
    _NLinLeakA/=_EnergyA;
  }
  //end LAPP
  // Final EnergyCorrection

  _EnergyCorr();

  if(_ShowerMax==Maxrow-1 || _ShowerMax==0){
    setstatus(AMSDBc::CATLEAK);
  }
  else if(_ShowerMax==Maxrow-2 || _ShowerMax==1){
    setstatus(AMSDBc::LEAK);
  }
  _AngleRes();
  _EnergyRes(); 



  // anode efficiency correction layer by layer ( 2014 EnergyE calculation)
  _ReFitDirCR();
  _ReCalEnergy();
}



float z[18],x[18],errorz[18];
float zv2[18],xv2[18],errorzv2[18];
Int_t nbins;
//______________________________________________________________________________
Double_t func(float x,Double_t *par)
{	
  Double_t value= par[0]*par[1]*TMath::Exp(-par[1]*x)*pow(par[1]*x,par[1]*par[2])/TMath::Gamma(par[1]*par[2]+1.);;
  return value;
}
//______________________________________________________________________________
void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  //  const Int_t nbins = 18;
  Int_t i;
  //calculate chisquare
  Double_t chisq = 0.;
  Double_t delta;
  for (i=0;i<nbins; i++) {
    delta = (z[i]-func(x[i],par))/errorz[i]; 
    chisq += delta*delta;	
  }
  f = chisq;
}

//______________________________________________________________________________
double funcv2(float x, double *par){
  //scale factor is set to 0.48 
  double fitval = (x<par[2])?0.:(par[0]*0.48*TMath::Exp(-0.48*(x-par[2]))*pow(0.48*(x-par[2]),0.48*par[1])/TMath::Gamma(0.48*par[1]+1.));
  return fitval;
}

void fcnv2(int &npar, double *gin, double &f, double *par, int iflag)
{
  int i;
  double chisq = 0.;
  double delta;
  for (i=0;i<18; i++) {
    delta = (zv2[i]-funcv2(xv2[i],par))/errorzv2[i];
    chisq += delta*delta;
  }
  f = chisq;
}



namespace ecalrec_ZProf {
  static TMinuit *fMinuit = 0;
#pragma omp threadprivate(fMinuit)
  static TMinuit *fMinuitv2 = 0;
#pragma omp threadprivate(fMinuitv2)
};

//______________________________________________________________________________
void AMSEcalShower::ZProfile()
{
  _Zprofile[0]=-1.;
  _Zprofile[1]=-1.;
  _Zprofile[2]=-1.;
  _Zprofile[3]=-1.;
  _ZprofileChi2=-1.;
  // Version #2 Mai16
  _Zprofilev2[0]=-1.;
  _Zprofilev2[1]=-1.;
  _Zprofilev2[2]=-1.;
  _EnergyF=-1.;
  _ZprofileChi2v2=-1.;

  int necalcl=0;
  int plane;
  int nhits_cl;
  float etot=0.;
  float err;
  float par0,par1,par2;
  float frac[18]={0.};
  float fracv2[18]={0.};
  float ECalEdepFrac[18]={0.};
   
  // Impact point correction for version #2
  float ratio= _S1totx/_S3totx;
  if (ratio<=0.5) ratio=0.5;
  //
  ratio=_S1toty/_S3toty;
  if (ratio<0.5) ratio=0.5;

  // Loop over 2d clusters
  for (int cl2=0 ; cl2 < _N2dCl ; cl2++){
    // Number of 1d cluster in 2D cluster
    necalcl=_pCl[cl2]->getNClust();
    // Loop over 1d clusters in cluster 2d
    for (int cl=0; cl<necalcl; cl++){
      nhits_cl=_pCl[cl2]->getpClust(cl)->getNHits();
      // Loop over hits in cluster 1d
      for (int ie=0; ie<nhits_cl; ie++){ 
	plane= _pCl[cl2]->getpClust(cl)->getphit(ie)->getplane();
	if(!isnan(_pCl[cl2]->getpClust(cl)->getphit(ie)->getedep()))
	  ECalEdepFrac[plane] += _pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();
	etot += _pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();
      }
    }
  }
  nbins=0;
  if(etot<=0)
    return;
  par2= 1.05*(log(_EnergyPIC*1000./(8.))-0.5);
  //     std::cout << "EShower " <<  _EnergyPIC << std::endl;
  for(int a=0;a<18;a++){
    // Impact point correction layer by layer version #2
    fracv2[a] = ECalEdepFrac[a]/etot;	      
    err = fracv2[a] * 0.1;  
    if (err<.009&&etot<10000.) err=0.009;
    if (err<.004&&etot>10000.) err=0.004;
    zv2[a] = frac[a];
    errorzv2[a] = err;
    xv2[a]=(float) a;

    frac[a] = ECalEdepFrac[a]/(_EnergyPIC*1000.);
    err=(ECalEdepFrac[a]/(_EnergyPIC*1000.))*0.10; 
    if (err<.009&&_EnergyPIC<1000.) err=0.009;
    if (err<.004&&_EnergyPIC>1000.) err=0.004;
    if(!isnan(frac[a]) && !isnan(err) && frac[a]>0. && frac[a]<1. && err>0. && err<1.){
      if((par2>8&&a>4)||(par2<8&&a<15)){
	z[nbins] = frac[a];
	errorz[nbins] = err;
	x[nbins]=(float) a + 0.5;
	nbins++;
      }
    }
  }



  Double_t arglist[10];
  // The z values	
  Double_t zprof[3],errprof[3];
  par0=1.;
  par1=0.5; 
  Int_t ierflg = 0;

  if(nbins>3){
	 
    // par2==zmax if the shower is an electromagnetic shower 
    // Minuit
    if (!ecalrec_ZProf::fMinuit) {
#pragma omp critical (tminuit)
      ecalrec_ZProf::fMinuit = new TMinuit(3);
    }		
    TMinuit *minuit = ecalrec_ZProf::fMinuit;
	
    //Set Minuit print Options
    minuit->SetPrintLevel(-1); 
    minuit->SetFCN(fcn);
    arglist[0]   = 1;
    minuit->mnexcm("SET ERR", arglist ,1,ierflg);
    // Set starting values and step sizes for parameters
    Double_t vstart[3] = {par0,par1,par2};
    Double_t step[3]   = {0.0001 , 0.0001 , .0001};
    minuit->mnparm(0, "a1", vstart[0], step[0],0.2*par0,3.*par0,ierflg);
    minuit->mnparm(1, "a2", vstart[1], step[1],0.,1.,ierflg);
    minuit->mnparm(2, "a3", vstart[2], step[2],0.5*par2,3.*par2,ierflg);
    // Now ready for minimization step
    arglist[0] = 1000;
    arglist[1] = 1;
    minuit->mnexcm("MIGRAD", arglist ,2,ierflg);
    // Print results
    minuit->GetParameter(0,zprof[0],errprof[0]);
    minuit->GetParameter(1,zprof[1],errprof[1]);
    minuit->GetParameter(2,zprof[2],errprof[2]);
    Double_t amin,edm,errdef;
    Int_t nvpar,nparx,icstat;
    minuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
    _Zprofile[0] = zprof[0];
    _Zprofile[2] = zprof[2];
    _Zprofile[3] = par2;
    _Zprofile[1] = zprof[1];
    _ZprofileChi2 = amin/(nbins-3);
  }

  // Fit Version #2 
  float par3=1.;
  par0=1.; 
  // Minuit
  if (!ecalrec_ZProf::fMinuitv2) {
#pragma omp critical (tminuit)
    ecalrec_ZProf::fMinuitv2 = new TMinuit(3);
  }         
  TMinuit *minuitv2 = ecalrec_ZProf::fMinuitv2;
  minuitv2->SetPrintLevel(-1);
  minuitv2->SetFCN(fcnv2);
  arglist[0]=2;
  minuitv2->mnexcm("SET ERR",arglist,1,ierflg);
  minuitv2->mnparm(0, "Normalisation", par0 , 0.1,0.,2.,ierflg);
  minuitv2->mnparm(1, "Zmax", par2, 0.1,.5*par2,1.5*par2,ierflg);
  minuitv2->mnparm(2, "Zshift", par3 , 0.5,-2.,18.,ierflg);
  arglist[0]=400;
  minuitv2->mnexcm("MIGRAD",arglist,1,ierflg);
  minuitv2->GetParameter(0,zprof[0],errprof[0]);
  minuitv2->GetParameter(1,zprof[1],errprof[1]);
  minuitv2->GetParameter(2,zprof[2],errprof[2]);
  float delta=0.;
  _ZprofileChi2 = 0.;
  for (int i=0;i<18; i++) {
    delta = (zv2[i]-funcv2(xv2[i],zprof))/errorzv2[i];
    _ZprofileChi2 += delta*delta;
  }
  _Zprofilev2[0] = zprof[0];
  _Zprofilev2[2] = zprof[2];
  _Zprofilev2[3] = par2;
  _Zprofilev2[1] = zprof[1];
  _EnergyF = etot * zprof[0];  	


  // Normalised energy dep frac
  if(etot>0.&&etot<10000000.){
    for(int a=0;a<18;a++)
      _EcalEdepFrac[a] = ECalEdepFrac[a] / etot;
  }
  else{
    for(int a=0;a<18;a++)
      _EcalEdepFrac[a] = -1.;
  }	
  return;
}


void AMSEcalShower::LAPPVariables(){
   
  _ShowerLatDisp=0.;
  _ShowerLatDispx=0.;
  _ShowerLatDispy=0.;

  _ShowerLongDisp=0.;

  _NbLayerX = 0;
  _NbLayerY = 0;
  
  _ShowerDepth=0.;
  
  _ShowerFootprintx = 0.;	
  _ShowerFootprinty = 0.;	
  _ShowerFootprint = 0.;

  float bcell_lat[18];
  float bcell2_lat[18];
  float s_cell_w[18];
  float s_cell2_w[18];
  float edep_cell[18][72];
  float edep_layer[18];
  int nhitcell[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  for (int elay=0; elay<18; elay++){
    bcell_lat[elay]=0.;
    bcell2_lat[elay]=0.;
    s_cell_w[elay]=0.;
    s_cell2_w[elay]=0.; 
    edep_layer[elay]=0.;
    nhitcell[elay]=0;
    for (int icell=0; icell<72; icell++){
      edep_cell[elay][icell]=0.;
    }    
  }  
  int necalcl=0;
  int cell,plane;
  int nhits_cl;

 
  // Loop over 2d clusters

  for (int cl2=0 ; cl2 < _N2dCl ; cl2++){

    // Number of 1d cluster in 2D cluster

    necalcl=_pCl[cl2]->getNClust();

    // Loop over 1d clusters in cluster 2d

    for (int cl=0; cl<necalcl; cl++){

      nhits_cl=_pCl[cl2]->getpClust(cl)->getNHits();

      // Loop over hits in cluster 1d
      for (int ie=0; ie<nhits_cl; ie++){ 

	cell = _pCl[cl2]->getpClust(cl)->getphit(ie)->getcell();
	plane= _pCl[cl2]->getpClust(cl)->getphit(ie)->getplane();

	if(!isnan(_pCl[cl2]->getpClust(cl)->getphit(ie)->getedep())){
            
	  edep_layer[plane]      += _pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();
	  edep_cell[plane][cell] =  _pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();
	  s_cell_w[plane]        += (cell+1)*_pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();
	  s_cell2_w[plane]       += pow((cell+1),2.)*_pCl[cl2]->getpClust(cl)->getphit(ie)->getedep();          
	  nhitcell[plane]++;
        }
        
      }
    }
  }


  float bplane=0.;
  float bplane2=0.;
  float bcell_latmx=0., bcell_latmy=0.;
  float edep_tot=0., edep_totx=0., edep_toty=0.;

  float shower_depth_op=0.;
 
  int nbcellx=0,nbcelly=0;
  for (int jj=0; jj<18; jj++){   
    if (edep_layer[jj]!=0){

      edep_tot+=edep_layer[jj];

      // Compute shower longitudinal dispersion
      bplane2+=pow((jj+1),2.)*edep_layer[jj];  
      bplane+=(jj+1)*edep_layer[jj]; 

      // Compute shower lateral dispersion  
      bcell_lat[jj]=s_cell_w[jj]/edep_layer[jj];
      bcell2_lat[jj]=s_cell2_w[jj]/edep_layer[jj];
      _ShowerLatDisp += bcell2_lat[jj] - (bcell_lat[jj]*bcell_lat[jj]);

      // Compute shower depth term  
      shower_depth_op+=(jj+1)*(1./pow(edep_layer[jj],2));

      if (((jj/2)%2)==0){
	_ShowerLatDispy+=bcell2_lat[jj]-pow((bcell_lat[jj]),2);
	bcell_latmy+=bcell_lat[jj]*edep_layer[jj];
	edep_toty+=edep_layer[jj];
	nbcelly++;
      }
      else{
	_ShowerLatDispx+=bcell2_lat[jj]-pow((bcell_lat[jj]),2);
	bcell_latmx+=bcell_lat[jj]*edep_layer[jj];
	edep_totx+=edep_layer[jj];
	nbcellx++;      
      }
    }
  }

  // Apply Impact point correction to ShowerLatDisp x,y:
  _ShowerLatDispx=_ShowerLatDispx/(1+_VarLeakXA);
  _ShowerLatDispy=_ShowerLatDispy/(1+_VarLeakYA);
  //done Impact point correction to ShowerLatDisp x, y

  _NbLayerX = nbcellx;
  _NbLayerY = nbcelly;

  //Compute longitudinal dispersions
  if(edep_tot>0.){
    _ShowerLongDisp =bplane2/edep_tot-pow((bplane/edep_tot),2);
  }  
  else{
    _ShowerLongDisp = -1.; 
  }

  edep_tot=edep_totx+edep_toty;

  //Compute shower depth
  _ShowerDepth = TMath::Log(shower_depth_op);

  //Compute shower shower Cofg_z
  if(edep_tot>0.){
    bplane=bplane/edep_tot;
  } 
  else { 
    bplane = -1000.;  
  }

  //Compute shower CofG x,y 
  if (edep_totx>0.){
    bcell_latmx=bcell_latmx/edep_totx;   
  }
  if (edep_toty>0.){
    bcell_latmy=bcell_latmy/edep_toty;   
  }
 
  //Compute shower footprint

  float sigmax2=0., sigmaxz=0., sigmay2=0., sigmayz=0., sigmaz2_x=0., sigmaz2_y=0.;
  for (int jj=0; jj<18; jj++)
    {

      if (edep_layer[jj]!=0)
	{
	  if (((jj/2)%2)==0)
	    {
              for (int cc=0; cc<72; cc++)
		{
                  if (edep_cell[jj][cc]>0) 
		    {
		      sigmay2+=pow(((cc+1)-bcell_latmy),2)*edep_cell[jj][cc];
		      sigmayz+=((cc+1)-bcell_latmy)*((jj+1)-bplane)*edep_cell[jj][cc];
		      sigmaz2_y+=pow(((jj+1)-bplane),2)*edep_cell[jj][cc];
		    }		     
		}
	    }
	  else
	    {
              for (int cc=0; cc<72; cc++)
		{
                  if (edep_cell[jj][cc]>0) 
		    {
		      sigmax2+=pow(((cc+1)-bcell_latmx),2)*edep_cell[jj][cc];
		      sigmaxz+=((cc+1)-bcell_latmx)*((jj+1)-bplane)*edep_cell[jj][cc];
		      sigmaz2_x+=pow(((jj+1)-bplane),2)*edep_cell[jj][cc];	
		    }
		}

	    }
	}
    }

	
  if(edep_totx>0.){ 
    sigmax2=sigmax2/edep_totx;
    sigmaxz=sigmaxz/edep_totx;
    sigmaz2_x=sigmaz2_x/edep_totx;
    _ShowerFootprintx=TMath::Sqrt(TMath::Abs(sigmax2*sigmaz2_x-pow(sigmaxz,2)));
  }
  if(edep_toty>0.){ 
    sigmay2=sigmay2/edep_toty;
    sigmayz=sigmayz/edep_toty;
    sigmaz2_y=sigmaz2_y/edep_toty;
    _ShowerFootprinty=TMath::Sqrt(TMath::Abs(sigmay2*sigmaz2_y-pow(sigmayz,2)));
  }
  _ShowerFootprint= _ShowerFootprintx + _ShowerFootprinty;	 

  return;
}


extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

extern "C" void d01amf_(void *alfun, number &bound, integer &inf, number &epsa, number &epsr, number &result, number &abserr, number w[], int &lw, int iw[], int &liw , int &ifail, void *p );

void AMSEcalShower::ProfileFit(){

#ifndef NO_NAG
  void (*palfun)(int &n, double x[], double &f, AMSEcalShower *p)=&AMSEcalShower::gamfun;
  void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
    &AMSEcalShower::monit;
  void (*psalfun)(double &x, double &f, AMSEcalShower *p)=&AMSEcalShower::gamfunr;
#endif

#ifndef __DARWIN__
  int env=0;
  env=fegetexcept();
  if(MISCFFKEY.RaiseFPE<=2){
    fedisableexcept(FE_OVERFLOW);
    fedisableexcept(FE_INVALID);
  }
#endif    
  // NowFit
    
  const integer Maxrow=ECALDBc::GetLayersNo();



  integer ifail=1;
  const integer mp=4;
  x[0]=1;
  x[1]=_Dz*_ShowerMax;
  x[2]=1;
  _Direction=0;
  number f=0,x[mp];
#ifndef NO_NAG
  integer n=3;
  number tol=3.99e-2;
  integer maxcal=25000;
  number w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
  integer iw=n+1;
  e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif     
  if(ifail==0){
    _ProfilePar[0]=x[0];    
    _ProfilePar[1]=x[1];    
    _ProfilePar[2]=x[2]!=0?1./x[2]:FLT_MAX;    
    _ProfilePar[4]=f;
    //    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
    _iflag=3;
    ifail=1;
#ifndef NO_NAG
    integer one=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
#endif
    // Leak Estimation
#ifndef NO_NAG
    const integer lwc=1000;
    const integer liwc=lwc/4;
    number ww[lwc];
	integer iww[liwc];
#endif
	number bound,epsa,epsr,result=0;
    integer inf;
    ifail=1;
    for(int i=Maxrow-1;i>=0;i--){
      if(_Edep[i]){
	bound=_Ez[i]-_Ez[0]+_Dz/2;
	break;
      }
    }
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
#ifndef NO_NAG
    int liw=liwc;
    int lw=lwc;
    number abserr;
    d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
#endif
    if(ifail==0){
      _ProfilePar[3]=result;
    }
    else{
      _ProfilePar[3]=-1;
    }
  }
  else{
    _ProfilePar[0]=0;
    _ProfilePar[1]=0;
    _ProfilePar[2]=0;
    _ProfilePar[4]=FLT_MAX;
  }
  // Try To inverted fit
  for(int i=0;i<(Maxrow+1)/2;i++){
    number tmpz=-_Ez[i];
    number tmpe=_Edep[i];
    _Ez[i]=-_Ez[Maxrow-1-i];
    _Edep[i]=_Edep[Maxrow-1-i];
    _Ez[Maxrow-1-i]=tmpz;
    _Edep[Maxrow-1-i]=tmpe;
  }
  x[0]=1;
  x[1]=_Dz*(Maxrow-_ShowerMax);
  x[2]=1;
  ifail=1;
#ifndef NO_NAG
  e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif
  //    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
  if(ifail==0){
    _ProfilePar[5]=x[0];    
    _ProfilePar[6]=x[1];    
    _ProfilePar[7]=x[2]!=0?1./x[2]:FLT_MAX;    
    _ProfilePar[9]=f;

	number bound,epsa,epsr,result=0;
    integer inf;
    ifail=1;
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
    for(int i=Maxrow-1;i>=0;i--){
      if(_Edep[i]){
	bound=_Ez[i]-_Ez[0]+_Dz/2;
	break;
      }
    }
    ifail=1;
#ifndef NO_NAG     
    const integer lwc=1000;
    const integer liwc=lwc/4;
	number abserr, ww[lwc];
	integer iww[liwc];
    int liw=liwc;
    int lw=lwc;
    d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
#endif
    if(ifail==0){
      _ProfilePar[8]=result;
    }
    else _ProfilePar[8]=-1; 
  }
  else{
    _ProfilePar[5]=0;
    _ProfilePar[6]=0;
    _ProfilePar[7]=0;
    _ProfilePar[9]=FLT_MAX;
  }
  if(_ProfilePar[9]<_ProfilePar[4]){
    _iflag=3;
    ifail=1;
#ifndef NO_NAG
    integer one=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
#endif
    _Direction=1;
  }
#ifndef __DARWIN__
  feclearexcept(FE_OVERFLOW);
  feclearexcept(FE_INVALID);
  if(env){
    feenableexcept(env);        
  }
#endif    
}

void AMSEcalShower::gamfunr(number& x, number &fc, AMSEcalShower *p){
  fc=0;
  if(x>0){
    number et=p->_Et;
    number xc[3];
    for(int i=0;i<3;i++){
      xc[i]=p->_ProfilePar[i+5*p->_Direction];
    }
   
    number aa=-x/xc[2];
   
    if(et && xc[2]!=0){
      fc=pow(x,xc[1]/xc[2])*exp(aa);
      fc*=xc[0]/et;
    }
  }
}

void AMSEcalShower::gamfun(integer &n, number xc[], number &fc, AMSEcalShower *p){
  /*
    PROTOCALLSFFUN4(DOUBLE,DGAUSS,dgauss,ROUTINE,DOUBLE,DOUBLE,DOUBLE)
    #define DGAUSS(A2,A3,A4,A5) CCALLSFFUN4(DGAUSS,dgauss,,ROUTINE,DOUBLE,DOUBLE,DOUBLE,A2,A3,A4,A5)
  */
  const integer Maxrow=ECALDBc::GetLayersNo();
  fc=0;
  for(int i=0;i<n;i++){
    if(xc[i]<0){
      fc=FLT_MAX;
      return;
    }
  }
  if(xc[1]>p->_Dz*Maxrow){
    fc=FLT_MAX;
    return;
  }
  {
    number et=0;
    const integer nint=7;
    for (int i=2;i<Maxrow;i++){
      number edep=0;
      number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
      for(int j=0;j<nint;j++){
	number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
	number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
	/*
	  number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
	  number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
	*/
	number p1=x1>0?exp(log(x1)*xc[1]*xc[2]-xc[2]*x1):0;
	number p2=x2>0?exp(log(x2)*xc[1]*xc[2]-xc[2]*x2):0;

	edep+= (p1+p2)*0.5*dz/nint;
      }
      et+=edep;
    } 
    for (int i=2;i<Maxrow;i++){
      if(p->_Edep[i]){
	number edep=0;
	number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
	for(int j=0;j<nint;j++){
	  number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
	  number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
	  /*
	    number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
	    number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
	  */
	  number p1=x1>0?exp(log(x1)*xc[1]*xc[2]-xc[2]*x1):0;
	  number p2=x2>0?exp(log(x2)*xc[1]*xc[2]-xc[2]*x2):0;
	  edep+= (p1+p2)*0.5*dz/nint;
	}
	fc+=(edep/et*xc[0]-p->_Edep[i])*(edep/et*xc[0]-p->_Edep[i])/p->_Edep[i]*70;
      }
    }
    // cout <<" xc "<<xc[0]<< " "<<xc[1]<<" "<<xc[2]<<" "<<fc<<endl;  
  }
  if(p->_iflag==3){
    p->_Et=0;
    const integer nint=7;
    for (int i=0;i<Maxrow;i++){
      number edep=0;
      number edepz=0;
      number dz=i<Maxrow-1?p->_Ez[i+1]-p->_Ez[i]:p->_Dz;
      for(int j=0;j<nint;j++){
	number x1=p->_Ez[i]-p->_Ez[0]+dz*(j)/nint;
	number x2=p->_Ez[i]-p->_Ez[0]+dz*(j+1)/nint;
	//    number f3=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
	//    number f4=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
	number f1=x1>0?exp(min(number(log(FLT_MAX/2)),-xc[2]*x1+xc[1]*xc[2]*log(x1))):0;
	number f2=x2>0?exp(min(number(log(FLT_MAX/2)),-xc[2]*x2+xc[1]*xc[2]*log(x2))):0;
	//    cout<<" f4/f2 "<<f4/f2<<endl;
	edep+= (f1+f2)*0.5;
	p->_Et+=edep*dz/nint;
	edepz+=(f1*(x2+2*x1)+f2*(x1+2*x2))/6.;
      }
      number zcorr=-(edepz/edep-(p->_Ez[i]-p->_Ez[0]+dz/2));
      if(fabs(zcorr)/fabs(dz)<1){
	p->_Zcorr[i]=-(edepz/edep-(p->_Ez[i]-p->_Ez[0]+dz/2));
      }
      else{
	p->_Zcorr[i]=0;
	cerr<<"AMSEcalShower::Energyfit-W-TooBigZcorrAttempted "<<zcorr<<" "<<dz<<endl;
      }
    }
    p->_iflag=4;
  }
}
void AMSEcalShower::EMagFit(){
#ifndef NO_NAG
  void (*palfun)(int &n, double x[], double &f, AMSEcalShower *p)=&AMSEcalShower::expfun;
  void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=&AMSEcalShower::monit;
#endif

  _TransFitChi2=0;
  for(int i=0;i<3;i++)_TransFitPar[i]=0;


  // just 2 exp fit around center

  number norm=0;
  VZERO(_TmpFit,sizeof(_TmpFit)/sizeof(integer));
  const int MaxSize=sizeof(_TmpFit)/sizeof(_TmpFit[0]);
  number step=ECREFFKEY.TransShowerSize2D/MaxSize;
  for(int proj=0;proj<_N2dCl;proj++){
    for (int i=0;i<_pCl[proj]->getNClust();i++){
      Ecal1DCluster *ptr=_pCl[proj]->getpClust(i);
      number intercep=_pCl[proj]->getcoo()+ptr->getcoo()[2]*_pCl[proj]->gettan();
      for(unsigned int j=0;j<ptr->getNHits();j++){
	number cosa=fabs(cos(atan(_pCl[proj]->gettan())));
	number dist=fabs(intercep-ptr->getphit(j)->getcoot())*cosa;
	integer chan=floor((dist/step));
	if(chan>=0 && chan<MaxSize){
	  _TmpFit[chan]+=ptr->getphit(j)->getedep();
	}
        norm+=ptr->getphit(j)->getedep();
      }
    }
  }
  if(norm){
    for (int i=0;i<MaxSize;i++)_TmpFit[i]/=norm;

    // Fit

    const integer mp=5;
    number f=0,x[mp];
    x[0]=1;
    x[1]=3;
    x[2]=0.1;
    x[3]=0.5;
#ifndef NO_NAG
    integer n=4;
    integer iw=n+1;
    integer maxcal=25000;
    number tol=3.99e-2;
	number w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    integer ifail=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif
    _TransFitPar[0]=x[1]!=0?1/x[1]:FLT_MAX;
    _TransFitPar[1]=x[3]!=0?1/x[3]:FLT_MAX;
    _TransFitPar[2]=x[2];
    _TransFitChi2=f;
  }
}

void AMSEcalShower::expfun(integer &n, number xc[], number &fc, AMSEcalShower *p){
  fc=0;
  for(int i=0;i<n;i++){
    if(xc[i]<0){
      fc=FLT_MAX;
      return;
    }
  }
  const int MaxSize=sizeof(p->_TmpFit)/sizeof(p->_TmpFit[0]);
  number step=ECREFFKEY.TransShowerSize2D/MaxSize;
  fc=0;
  number nn=0;
  for(int i=0;i<MaxSize;i++){
    if(p->_TmpFit[i]>0){
      number x1=step*i;
      number x2=step*(i+1);
      number df=xc[0]*(exp(-xc[1]*x1)-exp(-xc[1]*x2))+xc[2]*(exp(-xc[3]*x1)-exp(-xc[3]*x2));
      fc+=(df-p->_TmpFit[i])*(df-p->_TmpFit[i])/p->_TmpFit[i]*p->_EnergyC*100;
      nn++;
    }
  }
  if(nn)fc/=nn;
  //  cout <<" xc "<<xc[0]<<" "<<xc[1]<<" "<<xc[2]<<" "<<xc[3]<<" "<<fc<<" "<<nn<<endl;
}


integer AMSEcalShower::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("EcalShower",AMSJob::gethead()->gettriggerC(n))==0){
	WriteAll=1;
	break;
      }
    }
  }
  return (WriteAll || status);
}


void AMSEcalShower::_AngleRes(){
  _Angle3DError=0;
  _Angle3DErrorPI=0;
  _Angle3DErrorA=0;//LAPP
  if(_EnergyC>0){
    _Angle3DError= 3.1415926/180.*sqrt(0.8*0.8+8.4*8.4/_EnergyC);
  }

  if(_EnergyPIC>0){
    _Angle3DErrorPI= 3.1415926/180.*sqrt(0.8*0.8+8.4*8.4/_EnergyPIC);
  }
  if(_EnergyA>0){
    _Angle3DErrorA= 3.1415926/180.*sqrt(0.8*0.8+8.4*8.4/_EnergyA);
  }
}


number AMSEcalShower::_Chi2Corr(){
  if(_EnergyC>0){
    return sqrt(0.35*0.35+100/_EnergyC);
  }
  else return 1;
}

void AMSEcalShower::_EnergyCorr(){

  //Try to take into account lower energy + adcoverflow
  //pure phenomelogical one, should be replaced by more smart one...
  if(_EnergyC){
    number minen=_EnergyC>1?_EnergyC:1;
    number lowencorr=1.0009-2.7e-2/pow(minen,0.5);
    _EnergyC/=lowencorr;
    if(_EnergyC>ECREFFKEY.HiEnThr && !checkstatus(AMSDBc::CATLEAK)){
      number maxen=_EnergyC/ECREFFKEY.HiEnThr*ECREFFKEY.HiEnCorFac;
      if(maxen>=1){
	setstatus(AMSDBc::CATLEAK);
	maxen=0.9999999;
      }
      number hiencorr=(1-ECREFFKEY.HiEnCorFac)/(1-maxen);
      _EnergyC*=hiencorr;
    }
  }
}


void AMSEcalShower::_EnergyRes(){

  _ErrEnergyC= fabs(_DifoSum)*_EnergyC/sqrt(2.);
  _ErrEnergyPIC= fabs(_DifoSum)*_EnergyPIC/sqrt(2.);
  _ErrEnergyA= fabs(_DifoSum)*_EnergyA/sqrt(2.);//LAPP
  
}

number AMSEcalShower::getTrue3DChi2(number tantz[2],number t0[2],bool zcorr[2]){
  number fc=0;
  number xfc=0;
  AMSPoint sp(t0[0],t0[1],0.);
  AMSPoint sp2(0,0,10.);
  sp2[0]=sp[0]+tantz[0]*(sp2[2]-sp[2]);
  sp2[1]=sp[1]+tantz[1]*(sp2[2]-sp[2]);
  AMSDir sdir=sp2-sp;
  for(int i=0;i<2;i++){
    AMSDir coodir=i==0?AMSDir(0,1,0):AMSDir(1,0,0);
    for(int k=0;k<_pCl[i]->getNClustKernel();k++){
      Ecal1DCluster *p= _pCl[i]->getpClust(k);
      if(p && !p->checkstatus(AMSDBc::DELETED)){
	AMSPoint dc=sp-p->getcoo();
	if(zcorr[i])dc[2]-=_Zcorr[p->getplane()];
	AMSPoint alpha=sdir.crossp(coodir);
	AMSPoint beta= sdir.crossp(dc);
	number t=alpha.prod(beta)/alpha.prod(alpha);
	if(t>ECALDBc::CellSize(p->getplane(),p->getmaxcell(),1))t=ECALDBc::CellSize(p->getplane(),p->getmaxcell(),1);
	else if(t<-ECALDBc::CellSize(p->getplane(),p->getmaxcell(),1))t=-ECALDBc::CellSize(p->getplane(),p->getmaxcell(),1);
	AMSPoint hit=p->getcoo()+coodir*t;
	if(zcorr[i])hit[2]+=_Zcorr[p->getplane()];
	AMSPoint dc2=sp-hit;
	number d1=sdir.prod(dc2);
	d1=dc2.prod(dc2)-d1*d1;
	fc+=d1/p->PosError()/p->PosError();
	xfc++;
      }
    }
  }
  if(xfc>2)return fc/(xfc-2);
  else return -1;
}

/*
  PROTOCCALLSFSUB11(F02EAF,f02eaf,STRING,INT,DOUBLEVV,INT,DOUBLEV,DOUBLEV,DOUBLEVV,INT,DOUBLEV,INT,INT)
  #define F02EAF(A1,A2,A3,A4,A5,A6,A7,A8,A9,AA,AB)  CCALLSFSUB11(F02EAF,f02eaf,STRING,INT,DOUBLEVV,INT,DOUBLEV,DOUBLEV,DOUBLEVV,INT,DOUBLEV,INT,INT,A1,A2,A3,A4,A5,A6,A7,A8,A9,AA,AB)
  PROTOCCALLSFSUB2(F02EAFW,f02eafw,DOUBLEVV,DOUBLEV)
  #define F02EAFW(A1,A2)  CCALLSFSUB2(F02EAFW,f02eafw,DOUBLEVV,DOUBLEV,A1,A2)
*/
extern "C" void f02eafw_(number matrix[3][3], number ev[3]);
void AMSEcalShower::SphFit(){
  for(int i=0;i<3;i++)_SphericityEV[i]=-1;

  // calc sphericity only for two first 2dclusters
  number sab[3][3];
  for(int i=0;i<3;i++){
    for(int j=0;j<3;j++)sab[i][j]=0;
  }
  number snorm=0;
  for (int j=0;j<2;j++){
    /*
      number th=atan2(_EMDir[_pCl[j]->getproj()],_Dir[2]);
      number cth=cos(th);
      number sth=sin(th);
    */
    int over=_pCl[j]->getproj()==0?1:0;
    for (int i=0;i<_pCl[j]->getNClust();i++){
      Ecal1DCluster *p=_pCl[j]->getpClust(i);
      AMSPoint coo=p->getcoo();
      coo[over]=_EntryPoint[over];
      AMSDir e=coo-_EntryPoint;
      /*
	AMSPoint newdirp;
	newdirp[p->getproj()]=cth*e[p->getproj()]-sth*e[2];
	newdirp[over]=0;
	newdirp[2]=sth*e[p->getproj()]+cth*e[2];
	AMSDir newdir(newdirp);
	AMSPoint v=newdir*p->getEnergy();
      */
      AMSPoint v=e*p->getEnergy();
      for(int m=0;m<3;m++){
	for(int n=0;n<3;n++){
	  sab[m][n]+=v[m]*v[n];
	}
      }
      snorm+=v.prod(v);
    }
  }
  for(int m=0;m<3;m++){
    for(int n=0;n<3;n++)sab[m][n]/=snorm;
  }
  /*    
	int N=3;
	const int lda=3;
	int LDA=lda;
	number WI[lda];
	number WR[lda];
	int LDZ=3;
	number Z[3][3];
	int LWORK=64*lda;
	number WORK[64*lda];
	int IFAIL=-1;
	F02EAF("N",N,sab,LDA,_SphericityEV,WI,Z,LDZ,WORK,LWORK,IFAIL);
  */
  f02eafw_(sab,_SphericityEV); 
  //     cout <<_SphericityEV[0]<<endl;
}


void AMSEcalShower::AddOrphan(AMSEcal2DCluster *ptr){
  ptr->setstatus(AMSDBc::USED);
  _Orp2DEnergy+=ptr->_EnergyC/1000;

  if(_N2dCl<int(sizeof(_pCl)/sizeof(_pCl[0]))){
    _pCl[_N2dCl++]=ptr;
  }
  else{
    cerr<<"AMSEcalShower::AddOrphan-E-UnableToAdd "<<_Orp2DEnergy<<" GeV";
    setstatus(AMSDBc::BAD);
  }
}


number AMSEcalShower::getEnergyXY(int proj) const{
  number enr=0;
  for(int i=0;i<_N2dCl;i++){
    if(_pCl[i]->getproj()==proj)enr+=_pCl[i]->getEnergy();
  }
  return enr;
}


//===============================================================


// int with DAQ


//int16u AMSEcalRawEvent::getdaqid(int i){
//  if (i<getmaxblocks())return ( (2+i) | 14 <<9);
//  else return 0x0;
//}

//integer AMSEcalRawEvent::checkdaqid(int16u id){
// for(int i=0;i<getmaxblocks();i++){
//  if(id==getdaqid(i))return i+1;
// }
// return 0;
//}

void AMSEcalRawEvent::setTDV(){
  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVped());
  if(ptdv)ptdv->Verify()=true;
  ptdv = AMSJob::gethead()->gettimestructure(getTDVcalib());
  if(ptdv)ptdv->Verify()=true;
  ptdv = AMSJob::gethead()->gettimestructure(getTDVvpar());
  if(ptdv)ptdv->Verify()=true;
  ptdv = AMSJob::gethead()->gettimestructure(getTDVcalibTslo());
  if(ptdv)ptdv->Verify()=true; 
}


/*
  void AMSEcalRawEvent::buildrawRaw(integer n, int16u *p){
  integer ic=checkdaqid(*p)-1;
  geant tmp[2][ECSLMX*2][ECPMSMX*2];
  VZERO(tmp,sizeof(tmp)/sizeof(integer));
  int leng=0;
  int count=0; 
  int dynode=0;
  int dead=0;
  static geant sum_dynode=0;
  integer ecalflag;
  if(ic==0)sum_dynode=0;
  for(int16u* ptr=p+1;ptr<p+n;ptr++){//<--- daq-words loop  
  int16u pmt=count%36;
  int16u anode=(*ptr>>15)& 1;
  int16u channel=((*ptr)>>12)&3;
  int16u gain=((*ptr)>>14)&1;
  int16u value=( (*ptr))&4095; 
  if(!anode){
  channel=0;
  gain=3;
  }
  AMSECIds id(ic,pmt,channel);//create id's from crate/haddr/chanh

  if(anode){
  tmp[1-gain][id.getlayer()][id.getcell()]=value-id.getped(1-gain);
  }
  else{
  //  store dynode info in stat.array of AMSEcalRawEvent-class
  if((value-id.getpedd())>3*id.getsigd()){//above the threshold
  dynadc[id.getslay()][id.getpmt()]=floor((value-id.getpedd())
  *ECALDBc::scalef()+0.5);//to daq-scale
  }
  sum_dynode+=(value-id.getpedd())*id.getan2dyr()*id.getadc2mev();   
  dynode++; 
  }
  count++;               
  }//---> endof daq-words loop
  //----
  //Harwired here, should be via datacards
  geant HighThr=3.75;
  geant LowThr=1;
  geant LowAmp=10;
  for(int i=0;i<ECSLMX*2;i++){
  int last=-1;
  for(int j=0;j<ECPMSMX*2;j++){
  AMSECIds id(i,j);
  bool accept=tmp[0][i][j]>id.getsig(0)*HighThr;
  accept=accept || (tmp[0][i][j]> LowAmp && tmp[0][i][j]> id.getsig(0)*LowThr);
  accept=accept || (tmp[1][i][j]> LowAmp && tmp[1][i][j]> id.getsig(1)*HighThr*2);
  if(accept){
  for(int k=max(last+1,j-1);k<j+2;k++) {           
  AMSECIds idk(i,k);
  if(idk.getcrate()==ic){             
  int padc[2];
  for(int l=0;l<2;l++){
  padc[l]=floor(tmp[l][i][k]*ECALDBc::scalef()+0.5);//to daq scale 
  if(padc[l]<0)padc[l]=0;
  }
  AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",ic), new
  AMSEcalRawEvent(idk.makeshortid(),0,padc));
  }
  }
  last=j+1;
  }
  }
  }



  // build lvl1 trigger
  if(ic==getmaxblocks()-1){
  integer tofpatt[4]={0,0,0,0};
  int16u ecpatt[6][3]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  geant rates[6]={0,0,0,0,0,0};
  AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
  new Trigger2LVL1(1,1<<6,0,0,tofpatt,0,0,ecalflag,ecpatt,sum_dynode/1000,1,rates));

  }
  }
*/


/*
  AMSEcalRawEvent::AMSEcalRawEvent(const AMSECIds & id, int16u dynode,int16u gain,int16u adc):AMSlink(),_gain(gain),_id(id){
  for(int i=0;i<2;i++)_padc[i]=0;
  if(dynode)_padc[2]=adc;
  else{
  if(_gain<2)_padc[_gain]=adc;
  }
  _idsoft=id.makeshortid();
  }
*/


void AMSEcalShower::_AttCorr(){
  //  Attenuation Energy Corr & pmtnonl corr
  for(int i=0;i<_N2dCl;i++){
    for(int j=0;j<_pCl[i]->getNClust();j++){
      for (unsigned int k=0;k<_pCl[i]->getpClust(j)->getNHits();k++){
        int over=_pCl[i]->getpClust(j)->getproj()==0?1:0;
        number coo=0;
        if(_Dir[2]){
          coo=_EntryPoint[over]+_Dir[over]/_Dir[2]*(_pCl[i]->getpClust(j)->getphit(k)->getcooz()-_EntryPoint[2]);
	}
	if(coo !=coo){
	  cerr<<"  coo "<<coo<<endl;
	}
	_pCl[i]->getpClust(j)->getphit(k)->attcor(coo);
	_AttLeak+=_pCl[i]->getpClust(j)->getphit(k)->getattcor();
	_NLinLeak+=_pCl[i]->getpClust(j)->getphit(k)->getedepc();
      }
      //      cout <<"  coo "<<_pCl[i]->getpClust(j)->getcoo()[0]<<" "<<_pCl[i]->getpClust(j)->getcoo()[1]<<" "<<_pCl[i]->getpClust(j)->getcoo()[2]<<" "<<endl;
    }
  }


}

void AMSEcalShower::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  EcalShowerR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->EcalShower(_vpos);
  for (int i=0; i<_N2dCl; i++) {
    ptr.fEcal2DCluster.push_back(_pCl[i]->GetClonePointer());
  }
#endif
}  

void AMSEcalShower::_ReFitDirCR(){

  const static double CellWidth=0.9;
  float CooCOG[18];

  const static double UM2CM = 1.*1e-4;

  //Par for CR
  Double_t P_1x_ISS[3] = { 6.44811, 2.0972, 0.323359 }; //ISS
  Double_t P_2x_ISS[3] = { -0.397205, 0.0319445, 1.86641};//ISS
  Double_t P_1y_ISS[3] = {  6.43306, 1.73144, 0.0791552 }; //ISS
  Double_t P_2y_ISS[3] = { -1.09495, 0.107193, 0.232615 }; //ISS


  Double_t P_1x_MC[3] = { 6.99937, 0.0813625, 0.00474232 }; //MC
  Double_t P_2x_MC[3] = { -0.393693, 0.0240556, 11.3286};//MC
  Double_t P_1y_MC[3] = { 6.81918, 2.39385, 0.117458 }; //MC
  Double_t P_2y_MC[3] = {-0.408571, 0.0277098, 1.24271 }; //MC

  Double_t P1[2];   //[0] for X, [1] for y
  Double_t P2[2];   //[0] for x, [1] for y
  if (AMSJob::gethead()->isMCData()){
    P1[0] = P_1x_MC[0] - P_1x_MC[1]*TMath::Exp(-1. * P_1x_MC[2]*_EnergyPIC);
    if(_EnergyPIC>0)P2[0] = P_2x_MC[0] + P_2x_MC[1]*TMath::Log(_EnergyPIC) + TMath::Power(_EnergyPIC,-1.*P_2x_MC[2]);
    else P2[0] = 0.;
    P1[1] = P_1y_MC[0] - P_1y_MC[1]*TMath::Exp(-1. * P_1y_MC[2]*_EnergyPIC);
    if(_EnergyPIC>0)P2[1] = P_2y_MC[0] + P_2y_MC[1]*TMath::Log(_EnergyPIC) + TMath::Power(_EnergyPIC,-1.*P_2y_MC[2]);
    else P2[1] = 0.;
  }
  else{
    P1[0] = P_1x_ISS[0] - P_1x_ISS[1]*TMath::Exp(-1. * P_1x_ISS[2]*_EnergyPIC);
    if(_EnergyPIC>0)P2[0] = P_2x_ISS[0] + P_2x_ISS[1]*TMath::Log(_EnergyPIC) + TMath::Power(_EnergyPIC,-1.*P_2x_ISS[2]);
    else P2[0] = 0.;
    P1[1] = P_1y_ISS[0] - P_1y_ISS[1]*TMath::Exp(-1. * P_1y_ISS[2]*_EnergyPIC);
    if(_EnergyPIC>0)P2[1] = P_2y_ISS[0] + P_2y_ISS[1]*TMath::Log(_EnergyPIC) + TMath::Power(_EnergyPIC,-1.*P_2y_ISS[2]);
    else P2[1] = 0.;
  }

    
  //Par for Linear fit Weight of CR Methode
  Double_t WeightPar_ISS[18][3] ={ //ISS
    2.21605   ,   -2.1036  ,   0.000247644   ,
    2.19426   ,   -2.01735  ,   -0.000177171   ,
    0.598035   ,  -0.505857 ,   -0.0398346   ,
    1.33065   ,   -1.30213  ,   -0.026076   ,
    2.43469 , -2.4352 , -0.0102615,
    5.42861 , -5.44565 , -0.00528856,
    5.6952 , -5.80503 , -0.00972158,
    14.8548 , -14.9397 , -0.00335767,
    143.378 , -143.438 , -0.000222705,
    11.0292 , -11.1681 , -0.00311395,
    88.3388 , -88.5045 , -0.000432378,
    264.127 , -264.315 , -0.000122839,
    8.89312 , -8.98343 , -0.00202462,
    200.187 , -200.268 , -6.62233e-05,
    208.756 , -208.864 , -6.28318e-05,
    11.6781 , -11.7483 , -0.000759786,
    2.17098 , -2.19247 , -0.00181211,
    2.22295 , -2.22983 , -0.000909882
  };

    
  Double_t WeightPar_MC[18][3] ={ //MC
    5.17953  , -5.08484  ,  1.47755e-06  ,
    5.2431  , -4.99729  ,  -1.08931e-06  ,
    0.869854  , -0.599749  ,  -0.0116514  ,
    2.2637  , -2.02706  ,  -0.00776025  ,
    4.5917  , -4.43558  ,  -0.00348769  ,
    7.30869  , -7.11846  ,  -0.00274286  ,
    11.5314  , -11.2963  ,  -0.00349027  ,
    15.0101  , -14.8141  ,  -0.00287746  ,
    20.6217  , -20.5361  ,  -0.00150433  ,
    23.2943  , -23.333  ,  -0.00135089  ,
    33.9453  , -34.1189  ,  -0.00123685  ,
    30.9135  , -31.2807  ,  -0.00127686  ,
    31.9754  , -32.1955  ,  -0.000708125  ,
    37.6698  , -37.9253  ,  -0.00050569  ,
    24.5923  , -24.8841  ,  -0.000783484  ,
    32.3625  , -32.6477  ,  -0.000466563  ,
    32.0213  , -32.2323  ,  -0.000290185  ,
    38.4202  , -38.5792  ,  -0.000163912
  };
   

  Double_t LFWeight[18];
  for(int il=0;il<18;il++){
    if (AMSJob::gethead()->isMCData()){
      LFWeight[il] = WeightPar_MC[il][0] + WeightPar_MC[il][1]*TMath::Exp(WeightPar_MC[il][2]*_EnergyPIC);
    }
    else{
      LFWeight[il] = WeightPar_ISS[il][0] + WeightPar_ISS[il][1]*TMath::Exp(WeightPar_ISS[il][2]*_EnergyPIC);
    }
    
    if(LFWeight[il]<0.05)LFWeight[il]=0.05;
    if(LFWeight[il]>5.)LFWeight[il]=5.;
  }
  

  //Par for Linear fit Weight of COG Method
  Double_t WPCOG0[3] = {5.10998,1.30892,-7.18807};
  Double_t WP_COG0;
  if(_EnergyPIC+WPCOG0[2]>0)WP_COG0 = WPCOG0[0] + WPCOG0[1]*TMath::Log(_EnergyPIC+WPCOG0[2]);
  else WP_COG0 = 0.; 
  Double_t WPCOG1[5] = {1.54966, -6.22604e-08,-0.0103966,0.610089,-5.06742};
  Double_t WP_COG1;
  if(_EnergyPIC+WPCOG1[4]>0)WP_COG1 = WPCOG1[0] - WPCOG1[1]*TMath::Exp(-1.*WPCOG1[2]*_EnergyPIC)+WPCOG1[3]*TMath::Log(_EnergyPIC + WPCOG1[4]);
  else WP_COG1 =0.;
  Double_t WPCOG2[5] = {-30.6324,-7.22955,0.00115106,3.9701,389.901};
  Double_t WP_COG2;
  if(_EnergyPIC+WPCOG2[4]>0) WP_COG2 = 1./(WPCOG2[0]-WPCOG2[1]*TMath::Exp(-1.*WPCOG2[2]*_EnergyPIC)+WPCOG2[3]*TMath::Log(_EnergyPIC+WPCOG2[4]));
  else WP_COG2 = 0.;
  
  

  Double_t CooTrans[3][18];
  Double_t EdepCellRatio[18];
  Double_t Alpha[18];
  Double_t DisLeft[18];
  Double_t RecCoo[18];
  Double_t Weight[18];

  Double_t AngleTrk[3];
  Double_t AngleEcal[3];

  memset(CooTrans,0,sizeof(CooTrans));
  memset(EdepCellRatio,0,sizeof(EdepCellRatio));
  memset(Alpha,0,sizeof(Alpha));
  memset(DisLeft,0,sizeof(DisLeft));
  memset(RecCoo,0,sizeof(RecCoo));
  memset(_CooCR,0,sizeof(_CooCR));
  memset(Weight,0,sizeof(Weight));
  memset(AngleTrk,0,sizeof(AngleEcal));
  memset(AngleEcal,0,sizeof(AngleEcal));
  memset(_CellEdep,0,sizeof(_CellEdep));

  for(int i=0;i<_N2dCl;i++){
    for(int j=0;j<_pCl[i]->getNClust();j++){
      for (unsigned int k=0;k<_pCl[i]->getpClust(j)->getNHits();k++){
	AMSEcalHit * phit=_pCl[i]->getpClust(j)->getphit(k);
	_CellEdep[phit->getplane()][phit->getcell()]+=phit->getedep();
      }
    }
  }
  //Calculate COOCOG & COOCR
  for(int il =0;il<18;il++){
    int proj = 1- il/2%2;   //0 for x, 1 for y;
    Alpha[il] =  -1.*P1[proj] - P2[proj]*(il+0.5);  //Used for Calculate COOCR for layer

    //Find Bin_Max in the Layer;
    float tmpedepmax = -1.;
    int tmpcellmax = -1; 
    for(int ic=0;ic<72;ic++){
      if(_CellEdep[il][ic] >tmpedepmax){
	tmpedepmax = _CellEdep[il][ic];
	tmpcellmax = ic;
      }
    }
    int pmtside = tmpcellmax/2%2;
    
    //Caculate CooCOG
    Double_t tmpcoo=0.;
    Double_t eccl = 0.;
    for(int ic=0;ic<72;ic++){
      if(_CellEdep[il][ic]>0) tmpcoo += _CellEdep[il][ic] * (ic+0.5);
      eccl += _CellEdep[il][ic];
    }
    if(eccl>0) tmpcoo/=eccl;
    else tmpcoo =-9999.;
    CooCOG[il] = (tmpcoo - 36)*CellWidth;
    //Ecal Track Alignment
    if(AMSJob::gethead()->isMCData()){
      CooCOG[il] -= ECCommonOffSetMC[proj]; // MC
    }
    else{
      if(AMSEvent::gethead()->getrun()<1305763200){
	CooCOG[il] -= ECCommonOffSetBT[proj]; // Test Beam
      }
      else  CooCOG[il] -= ECCommonOffSetISS[proj]; // ISS
    }
    //Side Shift
    if(AMSJob::gethead()->isMCData()){
      CooCOG[il] += UM2CM * ECOffSetMC[il/2][pmtside]; // MC
    }
    else{
      if(AMSEvent::gethead()->getrun()<1305763200){
	CooCOG[il] += UM2CM * ECOffSetBT[il/2][pmtside]; // Test Beam
      }
      else  CooCOG[il] += UM2CM*ECOffSetISS[il/2][pmtside]; // ISS
    }

    //Calculate CooCR
    if(tmpcellmax ==0 || tmpcellmax == 71){
      _CooCR[il] = -9999.;
    }
    else{
      if(_CellEdep[il][tmpcellmax+1]>0.&&_CellEdep[il][tmpcellmax-1]>0.){
	_CooCR[il] = tmpcellmax + 0.5 + TMath::Log(_CellEdep[il][tmpcellmax-1]/_CellEdep[il][tmpcellmax+1])/Alpha[il];
	//Convert Cell Unit to cm
	_CooCR[il] -= 36;
	_CooCR[il] *= CellWidth;
	
	//Ecal Track Alignment and Side Shift 
	if(AMSJob::gethead()->isMCData()){
	  _CooCR[il] -= ECCommonOffSetMC[proj]; // MC 
	  _CooCR[il] += UM2CM * ECOffSetMC[il/2][pmtside];
	}
	else{
	  if(AMSEvent::gethead()->getrun()<1305763200){
	    _CooCR[il] -= ECCommonOffSetBT[proj]; // Test Beam
	    _CooCR[il] += UM2CM*ECOffSetBT[il/2][pmtside];
	  }
	  else{
	    _CooCR[il] -= ECCommonOffSetISS[proj]; // ISS 
	    _CooCR[il] += UM2CM*ECOffSetISS[il/2][pmtside];
	  }
	}
      }
      else{
	_CooCR[il] = -9999.;
      }
    }
  }

  //Do Linear Fit in XZ and YZ plane to get kx and ky with COOCOG
  Float_t coocr[18];
  _KCR[0]=0.;
  _KCR[1]=0.;

  //Calculate Weight used for Linear Fit  Weight = 1/Sigma_pos_layer/Sigma_pos_layer(mm^-2), minimum = 1/4.5/4.5=0.05, Maximum 1/0.45/0.45 = 5;
  for(int il=0;il<18;il++){
    //Weight[il] = elayer[il];
    if(CooCOG[il] >-100){
      coocr[il] = CooCOG[il];
        
      if(WP_COG1>0&&WP_COG2>0){
	Weight[il] =  WP_COG0*TMath::GammaDist(il+0.5,WP_COG1,0,WP_COG2);;
	if(Weight[il]<0.05)  Weight[il] = 0.05; //Minimum Weight for C.O.G
	if(Weight[il]>4)  Weight[il] = 4.;  //Maximum weight for C.O.G
      }
      else Weight[il] =0.05;
    }
    else { Weight[il] =0.; coocr[il] = 0.; }
  }

  Double_t sumz2=0.,sumy=0.,sumyz=0.,sumw=0.,sumz=0.;
  for(int isl=0;isl<9;isl+=2){
    for(int il=2*isl;il<2*isl +2;il++){
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
	Double_t tmpz = EClayer_Z[il]-_CofG[2];
	sumz2 += Weight[il] * tmpz*tmpz;
	sumy += Weight[il]*coocr[il];
	sumyz += Weight[il]*coocr[il]*tmpz;
	sumw += Weight[il];
	sumz += Weight[il] * tmpz;
      }
    }
  }
  if(sumw*sumz2-sumz*sumz>0){
    _CooNew[1] = (sumz2*sumy - sumz * sumyz)/(sumw*sumz2-sumz*sumz);
    _KCR[1] = (sumw*sumyz - sumz*sumy)/(sumw*sumz2 - sumz*sumz);
  }
  else { 
    _CooNew[1] = -99999.;
    _KCR[1]=-99999.;
  }
  
  sumz2=0;
  sumw=0;
  sumz=0;
  float sumx=0,sumxz=0;
  for(int isl=1;isl<9;isl+=2){
    for(int il=2*isl;il<2*isl +2;il++){
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
	Double_t tmpz =  EClayer_Z[il]-_CofG[2];
	sumz2 += Weight[il] * tmpz*tmpz;
	sumx += Weight[il]*coocr[il];
	sumxz += Weight[il]*coocr[il]*tmpz;
	sumw += Weight[il];
	sumz += Weight[il] * tmpz;
      }
    }
  }
  if(sumw*sumz2-sumz*sumz>0){
    _CooNew[0] = (sumz2*sumx - sumz * sumxz)/(sumw*sumz2-sumz*sumz);
    _KCR[0] = (sumw*sumxz - sumz*sumx)/(sumw*sumz2 - sumz*sumz);
  }
  else { 
    _CooNew[0]=-1;
    _KCR[0]=-99999.;
  }
  _CooNew[2] = _CofG[2];


  //Add Fiber Rotation to CooCR[il]
  for(int il=0;il<18;il++){
    //Find Bin_Max in the Layer;
    float tmpedepmax = -1.;
    int tmpcellmax = -1; 
    for(int ic=0;ic<72;ic++){
      if(_CellEdep[il][ic] >tmpedepmax){
	tmpedepmax = _CellEdep[il][ic];
	tmpcellmax = ic;
      }
    }
    double coof = il/2%2==0?(EClayer_Z[il]-_CofG[2])*_KCR[0]+_CooNew[0]:(EClayer_Z[il]-_CofG[2])*_KCR[1]+_CooNew[1];
    if(coof>32.4)coof=32.4;
    if(coof<-32.4)coof=-32.4;
    if( tmpcellmax/2%2==il/2%2 ){
      _CooCR[il] += ECfiberRotation[il/2]*(coof + 32.4);
    }
    else{
      _CooCR[il] += ECfiberRotation[il/2]*(coof - 32.4);
    }
  }
  
  //Second loop
  //Do Linear Fit in XZ and YZ plane to get kx and ky using COOCR
  _KCR[0]=0.;
  _KCR[1]=0.;

  //init values
  sumz2=0.;sumy=0.;sumyz=0.;sumw=0.;sumz=0.;
  sumx=0;sumxz=0;
  //Calculate Weight used for Linear Fit;
  for(int il=0;il<18;il++){
    if(_CooCR[il]>-100){
      coocr[il] = _CooCR[il];
      Weight[il] = LFWeight[il];
    }
    else{
      if(CooCOG[il] >-100){
	coocr[il] = CooCOG[il]; 
	//Weight[il] = elayer[il];
	if(WP_COG1>0&&WP_COG2>0){
	  Weight[il] =  WP_COG0*TMath::GammaDist(il+0.5,WP_COG1,0,WP_COG2);;
	  if(Weight[il]<0.05)Weight[il] = 0.05;
	  if(Weight[il]>4)Weight[il] = 4;
	}
	else Weight[il] =0.05;
      }
      else Weight[il] =0.;
    }
  }

  for(int isl=0;isl<9;isl+=2){
    for(int il=2*isl;il<2*isl +2;il++){
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
	Double_t tmpz = EClayer_Z[il]-_CofG[2];
	sumz2 += Weight[il] * tmpz*tmpz;
	sumy += Weight[il]*coocr[il];
	sumyz += Weight[il]*coocr[il]*tmpz;
	sumw += Weight[il];
	sumz += Weight[il] * tmpz;
      }
    }
  }
  if(sumw*sumz2-sumz*sumz>0){
    _CooNew[1] = (sumz2*sumy - sumz * sumyz)/(sumw*sumz2-sumz*sumz);
    _KCR[1] = (sumw*sumyz - sumz*sumy)/(sumw*sumz2 - sumz*sumz);
  }
  else { 
    _CooNew[1] = -99999.;
    _KCR[1]=-99999.;
  }
  
  sumz2=0;
  sumw=0;
  sumz=0;
  sumx=0;
  sumxz=0;
  for(int isl=1;isl<9;isl+=2){
    for(int il=2*isl;il<2*isl +2;il++){
      if(coocr[il]<32.4 && coocr[il] > -32.4&&Weight[il]>0){
	Double_t tmpz =  EClayer_Z[il]-_CofG[2];
	sumz2 += Weight[il] * tmpz*tmpz;
	sumx += Weight[il]*coocr[il];
	sumxz += Weight[il]*coocr[il]*tmpz;
	sumw += Weight[il];
	sumz += Weight[il] * tmpz;
      }
    }
  }
  if(sumw*sumz2-sumz*sumz>0){
    _CooNew[0] = (sumz2*sumx - sumz * sumxz)/(sumw*sumz2-sumz*sumz);
    _KCR[0] = (sumw*sumxz - sumz*sumx)/(sumw*sumz2 - sumz*sumz);
  }
  else { 
    _CooNew[0]=-1;
    _KCR[0]=-99999.;
  }
  _CooNew[2] = _CofG[2];
  //Done _KCR and _CooNew Calculation
}

void AMSEcalShower::_ReCalEnergy(){
  const static double CellWidth=0.9;
  const static double UM2CM = 1.*1e-4;

  //Sigma need to be refined.
  //ISS
  double aa = 0.336094;
  double bb[3] = {0.0848548,0.0870487,-0.0315658};
  //MC
  double a1 = aa;
  double b1 = bb[0] + bb[1] * TMath::Exp(bb[2]*_EnergyPIC);
   
  double Elpmteffsigma[18];

  double kk = TMath::Sqrt(1. + _KCR[0]*_KCR[0]+_KCR[1]*_KCR[1]);
  for(int il=0;il<18;il++){
    Elpmteffsigma[il] = a1+b1*(il+0.5)*kk;
    if(Elpmteffsigma[il]<.5)Elpmteffsigma[il]=0.55;
  }

  //Last Layer Correction
  float EnergyD_Corr = 0.;
  float elayer[18];
  float elayer_corr[18];


  for(int il=0;il<18;il++){
    elayer[il] = 0.;
    for(int ic=0;ic<72;ic++){
      if(_CellEdep[il][ic]>0) elayer[il] += _CellEdep[il][ic];
    }
    int proj = 1- il/2%2;   //0 for x, 1 for y;
    float tmppos;
    float coofit = _CooNew[proj] + _KCR[proj]*(EClayer_Z[il]-_CofG[2]);
    if(_CooCR[il]>-33&&_CooCR[il]<33)tmppos = _CooCR[il] ;
    else tmppos = coofit ;
    //tmppos = coofit;
               
    //Ecal Track Alignment
    if(AMSJob::gethead()->isMCData()){
      tmppos += ECCommonOffSetMC[proj]; // MC 
    }
    else{
      if(AMSEvent::gethead()->getrun()<1305763200){
	tmppos += ECCommonOffSetBT[proj]; // Test Beam 
      } 
      else{
	tmppos += ECCommonOffSetISS[proj]; // ISS 
      }
    }
    int tmpcellmax = (int)(tmppos/CellWidth) +36;
    int pmtside = tmpcellmax /2%2;
    //Side Shift
    if(AMSJob::gethead()->isMCData()){
      tmppos -= UM2CM * ECOffSetMC[il/2][pmtside]; // MC
    }
    else{
      if(AMSEvent::gethead()->getrun()<1305763200){
	tmppos -= UM2CM * ECOffSetBT[il/2][pmtside]; // Test Beam
      } 
      else{
	tmppos -= UM2CM*ECOffSetISS[il/2][pmtside]; // ISS
      }
    }

    //Fiber Rotation
    double coof = il/2%2==0?(EClayer_Z[il]-_CofG[2])*_KCR[0]+_CooNew[0]:(EClayer_Z[il]-_CofG[2])*_KCR[1]+_CooNew[1];
    if(coof>32.4)coof=32.4;
    if(coof<-32.4)coof=-32.4;
    if( tmpcellmax/2%2==il/2%2 ){
      tmppos -= ECfiberRotation[il/2]*(coof + 32.4);
    }
    else{
      tmppos -= ECfiberRotation[il/2]*(coof - 32.4);
    }

    tmppos /= CellWidth ;
    float pmtpos = tmppos - TMath::FloorNint(tmppos);

    elayer_corr[il] = elayer[il] /TMath::Gaus(pmtpos,0.5,Elpmteffsigma[il]);
    EnergyD_Corr += elayer_corr[il];
    _elayer_corr[il]=elayer_corr[il];
  }
   
  /* Z.Li original leakage correction 
  if(EnergyD_Corr!=0&&(elayer_corr[16]+elayer_corr[17])/EnergyD_Corr<0.5){
    _EnergyH = EnergyD_Corr/1000./(LastFrac_ECorrParA + LastFrac_ECorrParB*(elayer_corr[16]+elayer_corr[17])/EnergyD_Corr);
  }
  else{
    _EnergyH=EnergyD_Corr+elayer_corr[16]+elayer_corr[17];
  }
  */
  number kink_highenergy = 850;
  number kink_lowenergy = 5;
  double alpha;
  number correction_factor = 1.;
  double parleak_0[5]={1.60341,-1.52486e-01,1.88086e-02,-5.31496e-01,2.32455e-02};
  double parleak_1[5]={1.04922,-1.95889e-01,2.61411e-01,-1.12636e-08,2.07651};
  double parleak_2[5]={7.33469e-01,5.12555e+02,-2.19282e+05,-8.21973e+04,-5.12837e+01};
  if(_EnergyPIC<kink_lowenergy)
      alpha = parleak_0[0]+parleak_0[1]/_EnergyPIC+parleak_0[2]/TMath::Power(_EnergyPIC,2)+parleak_0[3]*TMath::Power(_EnergyPIC,parleak_0[4]);
  else
    if(_EnergyPIC<kink_highenergy)
      alpha = parleak_1[0]+parleak_1[1]/_EnergyPIC+parleak_1[2]/TMath::Power(_EnergyPIC,2)+parleak_1[3]*TMath::Power(_EnergyPIC,parleak_1[4]);  
    else
      alpha = parleak_2[0]+parleak_2[1]/_EnergyPIC+parleak_2[2]/TMath::Power(_EnergyPIC,2)+parleak_2[3]*TMath::Power(_EnergyPIC,parleak_2[4]);

  double energyfractionlast2layers;
  energyfractionlast2layers=(elayer_corr[16]+elayer_corr[17])/EnergyD_Corr;
  double beta =-0.752;
  double gamma=-5.633;
  correction_factor = alpha + beta * energyfractionlast2layers + gamma * TMath::Power(energyfractionlast2layers,2);
      
  double energy;
  EnergyD_Corr /=1000.; // EnergyD from now in GeV
  if(correction_factor>0)
    energy = EnergyD_Corr/correction_factor;
  else
    energy = _EnergyPIC;
      
  //second iteration
  if(energy<kink_lowenergy)
      alpha = parleak_0[0]+parleak_0[1]/energy+parleak_0[2]/TMath::Power(energy,2)+parleak_0[3]*TMath::Power(energy,parleak_0[4]);
  else
    if(energy<kink_highenergy)
      alpha = parleak_1[0]+parleak_1[1]/energy+parleak_1[2]/TMath::Power(energy,2)+parleak_1[3]*TMath::Power(energy,parleak_1[4]);  
    else
      alpha = parleak_2[0]+parleak_2[1]/energy+parleak_2[2]/TMath::Power(energy,2)+parleak_2[3]*TMath::Power(energy,parleak_2[4]);

  correction_factor = alpha + beta * energyfractionlast2layers + gamma * TMath::Power(energyfractionlast2layers,2);
    
  //corrected value  
  if(correction_factor>0)
    _EnergyH =  EnergyD_Corr/correction_factor;
  else
    _EnergyH =  EnergyD_Corr;
      
  //reconstructed energy never less than deposited energy corrected for anode efficiency
  if(_EnergyH<EnergyD_Corr)
    _EnergyH =  EnergyD_Corr;

  ///////////
  //It is not an electron, the fraction is too high (checked by MC, 3 sigmas)
  //if(energyfractionlast2layers>0.2)
  // _EnergyH =  EnergyD_Corr;
  ///////////
}
