//  $Id: ecalrec.C,v 1.63 2002/10/11 16:47:01 choutko Exp $
// v0.0 28.09.1999 by E.Choumilov
//
#include <iostream.h>
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <amsstl.h>
#include <commons.h>
#include <ntuple.h>
#include <tofsim02.h>
#include <ecaldbc.h>
#include <ecalrec.h>
#include <ecalcalib.h>
#include <mccluster.h>
#include <trigger102.h>
#include <trigger302.h>
#include <timeid.h>
using namespace std;
using namespace ecalconst;
//
uinteger AMSEcalRawEvent::trigfl=0;// just memory reservation/initialization for static
number AMSEcalRawEvent::trigtm=0.;// just memory reservation/initialization for static
geant AMSEcalRawEvent::trsum=0.;// just memory reservation/initialization for static
//----------------------------------------------------
void AMSEcalRawEvent::validate(int &stat){ //Check/correct RawEvent-structure
  int i,j,k;
  integer sta,status,dbstat,id,idd,isl,pmc,subc;
  number radc[2]; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl;
  integer ovfl[2];
  AMSEcalRawEvent * ptr;
  uinteger ecalflg;
  integer tofflg;
  Trigger2LVL1 *ptrt;
//
  ptrt=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  if(ptrt){
    tofflg=ptrt->gettoflg();
    ecalflg=ptrt->getecflg();
    HF1(ecalconst::ECHISTR+30,geant(ecalflg),1.);
  }
//
//
//----> fill arrays for Hi2Low-ratio calc.(in REUN-calibration):
//
  if(ECREFFKEY.relogic[1]<=2){// if REUN-calibration
    for(int nc=0;nc<AMSECIdSoft::ncrates();nc++){ // <-------------- super-layer loop
      ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
                       getheadC("AMSEcalRawEvent",nc,0);
      while(ptr){ // <--- RawEvent-hits loop in superlayer:
        isl=ptr->getslay();
        id=ptr->getid();//SSPPC
        idd=id/10;
        subc=id%10-1;//SubCell(0-3)
        pmc=idd%100-1;//PMCell(0-...)
        integer padc[3];
        ptr->getpadc(padc);
        ECPMPeds::pmpeds[isl][pmc].getpedh(pedh);
        ECPMPeds::pmpeds[isl][pmc].getsigh(sigh);
        ECPMPeds::pmpeds[isl][pmc].getpedl(pedl);
        ECPMPeds::pmpeds[isl][pmc].getsigl(sigl);
        radc[0]=number(padc[0])/ECALDBc::scalef();//DAQ-format-->ADC-high-chain
        radc[1]=number(padc[1])/ECALDBc::scalef();//DAQ-format-->ADC-low-chain
        ph=pedh[subc];
        sh=sigh[subc];
        pl=pedl[subc];
        sl=sigl[subc];
	if(AMSJob::gethead()->isSimulation()){
          if(ECMCFFKEY.silogic[0]==1){
            sh=0.;
            sl=0.;
          }
          if(ECMCFFKEY.silogic[0]==2){
            ph=0.;
	    pl=0.;
          }
	}
        ovfl[0]=0;
        ovfl[1]=0;
        if(radc[0]>0.)if((ECADCMX[0]-(radc[0]+ph))<=1)ovfl[0]=1;// mark as ADC-Overflow
        if(radc[1]>0.)if((ECADCMX[1]-(radc[1]+pl))<=1)ovfl[1]=1;// mark as ADC-Overflow
        if(radc[0]>0. && ovfl[0]==0 && radc[1]>0)ECREUNcalib::fill_2(isl,pmc,subc,radc);//<--- fill 
        ptr=ptr->next();  
      } // ---> end of RawEvent-hits loop in superlayer
//
    } // ---> end of super-layer loop
  } // ---> endif of REUN-calib
//
  stat=0;
}
//----------------------------------------------------
void AMSEcalRawEvent::mc_build(int &stat){

  int i,j,k;
  integer fid,cid,cidar[4],nhits,nraw,nrawd,il,pm,sc,proj,rdir,nslhits;
  number x,y,z,coo,hflen,pmdis,edep,edepr,edept,edeprt,emeast,time,timet(0.);
  number attf,ww[4],anen,dyen;
  number sum[ECPMSMX][4],pmtmap[ECSLMX][ECPMSMX],pmlprof[ECSLMX];
  integer zhitmap[ECSLMX];
  integer adch,adcm,adcl,adcd;
  geant radc,a2dr,h2lr,mev2adc,mev2adcd,ares,phel;
  geant lfs,lsl,ffr;
  geant pedh[4],pedl[4],sigh[4],sigl[4],pedd,sigd;
  AMSEcalMCHit * ptr;
  integer id,sta,scsta,nslmx,npmmx;
  integer adc[2];
  number dyresp,dyrespt,toftrtm;// dynode resp. in mev(~mV) (for trigger)
  number an4resp,an4respt;// (for trigger)
//
  nslmx=ECALDBc::slstruc(3);
  npmmx=ECALDBc::slstruc(4);
  stat=1;//bad
  trigfl=0;//reset trigger-flag
  trsum=0;//reset tot.energy(trig.sum)
  edept=0.;
  edeprt=0.;
  emeast=0.;
  nhits=0;
  nraw=0;
  nrawd=0;
  an4respt=0;
  dyrespt=0;
  timet=0.;
  for(il=0;il<ECSLMX;il++){
    pmlprof[il]=0.;
    zhitmap[il]=0;
    for(i=0;i<ECPMSMX;i++)pmtmap[il][i]=0.;
  }
//
  for(il=0;il<nslmx;il++){ // <-------------- super-layer loop
    ptr=(AMSEcalMCHit*)AMSEvent::gethead()->
               getheadC("AMSEcalMCHit",il,0);
    for(i=0;i<npmmx;i++)
                                     for(k=0;k<4;k++)sum[i][k]=0.;
    nslhits=0;
    while(ptr){ // <------------------- geant-hits loop in superlayer:
      nhits+=1;
      nslhits+=1;
      fid=ptr->getid();//SSLLFFF
      edep=ptr->getedep()*1000;// MeV(dE/dX)
//      if(il==0 && (fid%100000)/1000>=1 &&
//                    (fid%100000)/1000<=5 && (fid%1000)==254)edep=0.;//tempor test(in pl/sc=1/38 )
      edept+=edep;
      EcalJobStat::zprmc1[il]+=edep;//geant SL-profile
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
	  lfs=ECcalib::ecpmcal[il][pm].alfast();//att_len(fast comp) from DB
	  lsl=ECcalib::ecpmcal[il][pm].alslow();//att_len(slow comp) from DB
	  ffr=ECcalib::ecpmcal[il][pm].fastfr();//fast comp. fraction from DB
          attf=(1.-ffr)*exp(-pmdis/lsl)+ffr*exp(-pmdis/lfs);//fiber attenuation factor
          edepr=edep*attf*ww[j];
          sum[pm][sc]+=edepr;
          edeprt+=edepr;
	}
      }// -----> end of the coupled PM's loop
//
        ptr=ptr->next(); 
    } // ---------------> end of geant-hit-loop in superlayer
//
//    if(nslhits==0)continue;//low noise(no noise hits will be added later in this empty sl)
//
    for(i=0;i<npmmx;i++){ // <------- loop over PM's in this(il) S-layer
      a2dr=ECMCFFKEY.an2dyr;// MC anode/dynode gains ratio
      mev2adc=ECMCFFKEY.mev2adc;// MC Emeas->ADCchannel to have MIP-m.p. in 5th channel
//                (only mev2mev*mev2adc has real meaning providing Geant_dE/dX->ADCchannel)
      mev2adcd=ECMCFFKEY.mev2adcd;// same for dynode
      geant pmrgn=ECcalib::ecpmcal[il][i].pmrgain();// PM gain(wrt ref. one)
      ECPMPeds::pmpeds[il][i].getpedh(pedh);
      ECPMPeds::pmpeds[il][i].getsigh(sigh);
      ECPMPeds::pmpeds[il][i].getpedl(pedl);
      ECPMPeds::pmpeds[il][i].getsigl(sigl);
//      ECPMPeds::pmpeds[il][i].getpedd(pedd);//tempor
//      ECPMPeds::pmpeds[il][i].getsigd(sigd);
      pedd=2;//tempor
      sigd=0.2;//tempor
//
      an4resp=0;//PM Anode-resp(4cells,tempor in mev)
      dyresp=0;//PM Dynode-resp(tempor in mev)
      anen=0.;
      dyen=0.;
      for(k=0;k<4;k++){//<--- loop over 4-subcells in PM
      
        EcalJobStat::zprmc2[il]+=sum[i][k];//geant SL(PM-assigned)-profile
        h2lr=ECcalib::ecpmcal[il][i].hi2lowr(k);//PM subcell high/low ratio from DB
	geant scgn=ECcalib::ecpmcal[il][i].pmscgain(k);//SubCell gain(really 1/pmrg/scgn)
	ares=0.;
	phel=sum[i][k]*ECMCFFKEY.mev2pes;//numb.of photoelectrons
	if(phel>=1){
	  ares=ECMCFFKEY.pmseres/sqrt(phel);//ampl.resol.
          edepr=sum[i][k]*(1.+ares*rnormx())*ECMCFFKEY.mev2mev;//dE/dX(Mev)->Emeas(Mev)(incl.amp.fluct)
	}
	else edepr=0;
	if(edepr<0)edepr=0;
	emeast+=edepr;
	anen+=edepr+sigh[k]*rnormx()/mev2adc;//tempor add noise in 4xSubc. signal(for trig.study)
	dyen+=edepr;
// ---------> digitization+DAQ-scaling:
// High-gain channel:
        if(ECMCFFKEY.silogic[0]==0){
	  radc=edepr*mev2adc/scgn+pedh[k]+sigh[k]*rnormx();//Em(mev)->Em(adc)+ped+noise
	}
	else if(ECMCFFKEY.silogic[0]==1){
	  radc=edepr*mev2adc/scgn+pedh[k];//Em(mev)->Em(adc)+ped
	}
	else if(ECMCFFKEY.silogic[0]==2){
	  radc=edepr*mev2adc/scgn;//Em(mev)->Em(adc)
	}
	else{
	}
	adch=floor(radc);//"digitization"
	if(adch>=ECADCMX[0])adch=ECADCMX[0];//"ADC-saturation (12 bit)"
	if(ECMCFFKEY.silogic[0]<2)radc=number(adch)-pedh[k];// ped-subtraction
	else radc=number(adch);//no ped-subtr.
        if(radc>=sigh[k]*ECALVarp::ecalvpar.daqthr(0)){// use only hits above DAQ-readout threshold
	  adch=floor(radc*ECALDBc::scalef()+1/ECALDBc::scalef());//DAQ scaling
	}
	else{ adch=0;}
// Low-gain channel:
        if(ECMCFFKEY.silogic[0]==0){
	  radc=edepr*mev2adc/h2lr/scgn+pedl[k]+sigl[k]*rnormx();//Em(mev)->Em/h2lr(adc)+ped+noise
	}
	else if(ECMCFFKEY.silogic[0]==1){
	  radc=edepr*mev2adc/h2lr/scgn+pedl[k];//Em(mev)->Em/h2lr(adc)+ped
	}
	else if(ECMCFFKEY.silogic[0]==2){
	  radc=edepr*mev2adc/h2lr/scgn;//Em(mev)->Em/h2lr(adc)
	}
	else{
	}
	adcl=floor(radc);//"digitization")
	if(adcl>=ECADCMX[1])adcl=ECADCMX[1];//"ADC-saturation (12 bit)"
	if(ECMCFFKEY.silogic[0]<2)radc=number(adcl)-pedl[k];// ped-subtraction
	else radc=number(adcl);//no ped-subtr.
        if(radc>=sigl[k]*ECALVarp::ecalvpar.daqthr(4)){// use only hits above DAQ-readout threshold
	  adcl=floor(radc*ECALDBc::scalef()+1/ECALDBc::scalef());//DAQ scaling
	}
	else{ adcl=0;}
// <---------
	id=(k+1)+10*(i+1)+1000*(il+1);// SSPPC
        AMSECIdSoft ids(id);
	if(ids.HCHisBad())adch=0;
	if(ids.LCHisBad())adcl=0;
	if(adch>0 || adcl>0){
	  nraw+=1;
	  sta=0;
	  adc[0]=adch;
	  adc[1]=adcl;
          if(!ids.dead()){
          AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",ids.getcrate()),
                new AMSEcalRawEvent(id,sta,adc));
          }
	}
      }//<---- end of PMSubcell-loop
//
// Dynode channel:
        if(ECMCFFKEY.silogic[0]==0){
	  radc=dyen*pmrgn*mev2adcd/a2dr+pedd+sigd*rnormx();//Em(mev)->Em/a2dr(adc)+ped+noise
	}
	else if(ECMCFFKEY.silogic[0]==1){
	  radc=dyen*pmrgn*mev2adcd/a2dr+pedd;//Em(mev)->Em/a2dr(adc)+ped
	}
	else if(ECMCFFKEY.silogic[0]==2){
	  radc=edepr*pmrgn*mev2adcd/a2dr;//Em(mev)->Em/a2dr(adc)
	}
	else{
	}
	adcd=floor(radc);//"digitization")
	if(adcd>=ECADCMX[2])adcd=ECADCMX[2];//"ADC-saturation (12 bit)"
	if(ECMCFFKEY.silogic[0]<2)radc=number(adcd)-pedd;// ped-subtraction
	else radc=number(adcd);//no ped-subtr.
        if(radc>=ECALVarp::ecalvpar.daqthr(4)){// use only hits above DAQ-readout threshold
	  adcd=floor(radc*ECALDBc::scalef()+1/ECALDBc::scalef());//DAQ scaling
	}
	else{ adcd=0;}
//
	if(adcd>0){
	  nrawd+=1;
	  id=(i+1)+100*(il+1);// SSPP
	  sta=0;
// put here dynode-object creation, if it will be decided to have it in readout
	}
//
      an4resp=anen;//PM 4anode-sum resp(true mev ~ mV !!!)(later to add some factor to conv. to mV)
      dyresp=dyen/a2dr+sigd*rnormx()/mev2adc;//dynode+noise (currently in mev ~ real mV !!!)
      an4respt+=an4resp;
      dyrespt+=dyresp;
      if(ECMCFFKEY.mcprtf==1){
        HF1(ECHIST+5,geant(dyresp),1.);
        if(dyresp>3*sigd/mev2adc)HF1(ECHIST+6,geant(an4resp/dyresp),1.);
      }
//            arrays for trigger study:
      pmtmap[il][i]=an4resp;//tempor 4xAnode-resp(later dynode-resp)
      pmlprof[il]+=an4resp;//tempor 
//
    }//-------> end of PM-loop in superlayer
//
  } // ------------> end of super-layer loop
//
//                          <--- some variables for "electromagneticity" calc.
  geant rrr,efrnt,efrnta,ebase,epeak,epeaka,p2brat,p2frat,trwid1,trwid2;
  geant esep1,esep2,p2bcut,p2fcut,wdcut1,wdcut2,wdthr;
  geant etrsum1[ECPMSMX],etrsum2[ECPMSMX];
//
  esep1=ECALVarp::ecalvpar.daqthr(5);// separation.energy for p2b,p2f cuts
  p2bcut=ECALVarp::ecalvpar.daqthr(6);
  esep2=2.*esep1;//energy high-limit for width-cut action;
  p2fcut=ECALVarp::ecalvpar.daqthr(7);
  wdthr=ECALVarp::ecalvpar.daqthr(8);
  wdcut1=ECALVarp::ecalvpar.daqthr(9);//L=1,7,2(bend.proj)
  wdcut2=ECALVarp::ecalvpar.daqthr(9)-2.;//L=2,8,2
//
  efrnt=pmlprof[0]+pmlprof[1]+pmlprof[2];//energy in 1st 3SL's(~5X0)
  efrnta=(pmlprof[0]+pmlprof[1]+pmlprof[2])/3.;//aver.energy/sl in 1st 3SL's(~5X0)
  ebase=pmlprof[0]+pmlprof[nslmx-1];//esum in 1st+last SL's
  epeak=pmlprof[1]+pmlprof[2]+pmlprof[3];//esum aroud peak(low energies)
  epeaka=(pmlprof[2]+pmlprof[3]+pmlprof[4])/3.;//aver.energy/sl aroud peak(high energies)
//
  if(ebase>0.)p2brat=epeak/ebase;
  else p2brat=39.5;
  if(p2brat>39.5)p2brat=39.5;
//
  if(efrnta>0.)p2frat=epeaka/efrnta;
  else p2frat=9.8;
  if(p2frat>9.8)p2frat=9.8;
//
  trwid1=0.;
  for(pm=0;pm<npmmx;pm++){ // get summed over depth transv.profile(pr=1)
    etrsum1[pm]=0.;
    for(il=0;il<7;il+=2)etrsum1[pm]+=pmtmap[il][pm];
    if(etrsum1[pm]>wdthr)trwid1+=1.;
  }
  trwid2=0.;
  for(pm=0;pm<npmmx;pm++){ // get summed over depth transv.profile(pr=2)
    etrsum2[pm]=0.;
    for(il=1;il<8;il+=2)etrsum2[pm]+=pmtmap[il][pm];
    if(etrsum2[pm]>wdthr)trwid2+=1.;
  }
//
  if(ECMCFFKEY.mcprtf==1){
    HF1(ECHIST+1,geant(nhits),1.);
    HF1(ECHIST+2,geant(edept),1.);
    HF1(ECHIST+3,geant(edeprt),1.);
    HF1(ECHIST+4,geant(emeast),1.);
    HF1(ECHIST+9,geant(an4respt),1.);
    HF1(ECHIST+10,efrnt,1.);
  }
//  ---> create ECAL H/W-trigger(0->non;1->MIP+nonEM;2->softEM;3->hardEM(req.for Phot);
//                               prev+10->HighEn):
//
  if(an4respt>ECALVarp::ecalvpar.daqthr(1)){// mip thr.cut
    EcalJobStat::addsr(0);
    trigfl=1;// at least MIP (some non-zero activity in EC)
//
    if(efrnt>ECALVarp::ecalvpar.daqthr(2)){ //<--- Efront cut
      trigfl=2;// softEM 
      EcalJobStat::addsr(1);
      if(ECMCFFKEY.mcprtf==1 && an4respt<esep1){
        HF1(ECHIST+11,p2brat,1.);
        HF1(ECHIST+12,ebase,1.);
      }
//      if(an4respt<esep1 && p2brat<p2bcut)goto TrExit1;// ---> too low Peak/base(LE)
      EcalJobStat::addsr(2);
//
      if(ECMCFFKEY.mcprtf==1 && an4respt>esep1)HF1(ECHIST+13,p2frat,1.);
//      if(an4respt>esep1 && p2frat<p2fcut)goto TrExit1;// ---> too low Peak/front(HE)
      EcalJobStat::addsr(3);
//
      for(pm=0;pm<npmmx;pm++)if(etrsum1[pm]>0.)HF1(ECHIST+14,etrsum1[pm],1.);
      for(pm=0;pm<npmmx;pm++)if(etrsum2[pm]>0.)HF1(ECHIST+16,etrsum2[pm],1.);
      if(ECMCFFKEY.mcprtf==1 && an4respt<esep2){
        HF1(ECHIST+15,trwid1,1.);
        HF1(ECHIST+17,trwid2,1.);
      }
      if(ECMCFFKEY.mcprtf==1)HF1(ECHIST+18,geant(an4respt),1.);
      if(an4respt<esep2 && (trwid1>=wdcut1 || trwid2>=wdcut2))goto TrExit1;// ---> too high width(LE)
      EcalJobStat::addsr(4);
      trigfl=3;// hardEM 
TrExit1:
        rrr=0;    
    }
    if(an4respt>ECALVarp::ecalvpar.daqthr(3))trigfl+=10;//high energy
  }
//
//-------> create ECAL fast trigger(FT):
//
  if(trigfl>0){
    trigtm=timet/edept;// FT abs.time(ns) (stored in AMSEcalRawEvent object)
    trigtm+=TOF2Varp::tofvpar.ftdelf();// add the same fixed delay as for TOF-FT
    trsum=geant(an4respt/1000.);//tempor 4xAnodes (later dynode signal)
  }
//---
  if(ECMCFFKEY.mcprtf==1){
    HF1(ECHIST+19,geant(trigfl),1.);
  }
  if(trigfl>0)stat=0;
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
    *(ptr+1)=_padc[0];
    *(ptr+2)=_padc[1];
    return 3; 
}
//---------------------------------------------------
void AMSEcalHit::build(int &stat){
  int i,j,k;
  integer sta,status,dbstat,id,idd,isl,pmc,subc,nraw,nhits;
  integer proj,plane,cell,icont;
  number radc[2],edep,adct,fadc,emeast,coot,cool,cooz; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl;
  integer ovfl[2];
  AMSEcalRawEvent * ptr;
//
  stat=1;//bad
  nhits=0;
  nraw=0;
  adct=0.;
  emeast=0.;
//
  for(int nc=0;nc<AMSECIdSoft::ncrates();nc++){ // <-------------- cr. loop
    ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->
               getheadC("AMSEcalRawEvent",nc,0);
    while(ptr){ // <--- RawEvent-hits loop in superlayer:
      isl=ptr->getslay();
      nraw+=1;
      id=ptr->getid();//SSPPC
      AMSECIdSoft ids(id);
      idd=id/10;
      subc=id%10-1;//SubCell(0-3)
      pmc=idd%100-1;//PMCell(0-...)
      integer padc[3];
      ptr->getpadc(padc);
//      cout << "  found "<<padc[0]<<" "<<padc[1]<<endl;
      ECPMPeds::pmpeds[isl][pmc].getpedh(pedh);
      ECPMPeds::pmpeds[isl][pmc].getsigh(sigh);
      ECPMPeds::pmpeds[isl][pmc].getpedl(pedl);
      ECPMPeds::pmpeds[isl][pmc].getsigl(sigl);
      h2lr=ECcalib::ecpmcal[isl][pmc].hi2lowr(subc);
      radc[0]=number(padc[0])/ECALDBc::scalef();//DAQ-format-->ADC-high-chain
      radc[1]=number(padc[1])/ECALDBc::scalef();//DAQ-format-->ADC-low-chain
      ph=pedh[subc];
      sh=sigh[subc];
      pl=pedl[subc];
      sl=sigl[subc];
      if(AMSJob::gethead()->isSimulation()){
        if(ECMCFFKEY.silogic[0]==1){
          sh=0.;
          sl=0.;
        }
        if(ECMCFFKEY.silogic[0]==2){
          ph=0.;
	  pl=0.;
        }
      }
      ovfl[0]=0;
      ovfl[1]=0;
      if(radc[0]+ph>=ECADCMX[0]-1)ovfl[0]=1;// mark as ADC-Overflow
      if(radc[1]+pl>=ECADCMX[1]-1)ovfl[1]=1;// mark as ADC-Overflow
// take decision which chain to use for energy calc.(Hi or Low):
      sta=0;
      fadc=0.;
      if(radc[0]>0 && ovfl[0]==0 && !ids.HCHisBad()){
        fadc=radc[0];//use highCh.
        if(AMSJob::gethead()->isRealData() ){// tempor RealData Lch-corr.
        number ped=ph;
        number a=1.2;
        number b=1.2e-4;
        number x1=1666;
        number x2=3766;
        if(radc[0]+ph<1666){
          fadc=radc[0];
        }
        else{
         fadc=radc[0]*(a-b*(radc[0]+ph));
        }
       }
      }
      else if(radc[1]>(sl>1?5*sl:5) && ovfl[1]==0 && !ids.LCHisBad()){//Hch=Miss/Ovfl -> use Lch
        fadc=radc[1]*h2lr;//rescale LowG-chain to HighG
	sta|=AMSDBc::LOWGCHUSED;// set "LowGainChannel used" status bit
//
        if(AMSJob::gethead()->isRealData() ){// tempor RealData Lch-corr.
//      assume tri-angular  h/l correction;

        number ped=ph;
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
      else if(ovfl[1]==1 && !ids.LCHisBad()){
            fadc=radc[1]*h2lr;//use low ch.,rescale LowG-chain to HighG
	    sta|=AMSDBc::AOVERFLOW;// set overflow status bit
	    sta|=AMSDBc::LOWGCHUSED;// set "LowGainChannel used" status bit
      }
      else {
	    sta|=AMSDBc::BAD;// bad or 0 amplitude channel
      }
      edep=fadc*ECcalib::ecpmcal[isl][pmc].pmscgain(subc);//gain corr(really 1/pmrg/pmscg
//       because in Calib.object pmsc-gain was defined as 1/pmrg/pmscg)
      if(ECREFFKEY.reprtf[0]>0){
        HF1(ECHISTR+16,geant(edep),1.);
        HF1(ECHISTR+17,geant(edep),1.);
      }
      adct+=edep;//tot.adc
      edep=edep*ECcalib::ecpmcal[isl][pmc].adc2mev();// ADCch->Emeasured(MeV)
      emeast+=edep;//tot.Mev
      if(fadc>0.){// store hit
        nhits+=1;
        ECALDBc::getscinfoa(isl,pmc,subc,proj,plane,cell,coot,cool,cooz);// get SubCell info
	icont=plane;//container number for storing of EcalHits(= plane number)
        AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",icont), new
                       AMSEcalHit(sta,id,padc,proj,plane,cell,edep,coot,cool,cooz));
//               (conv. of padc(in daq_scale) to ADC is done inside of constructor)
      }
      ptr=ptr->next();  
    } // ---> end of RawEvent-hits loop in superlayer
//
  } // ---> end of crate loop
//
  if(ECREFFKEY.reprtf[0]>0){
    HF1(ECHISTR+10,geant(nraw),1.);
    HF1(ECHISTR+11,geant(adct),1.);
    HF1(ECHISTR+12,geant(adct),1.);
    HF1(ECHISTR+13,geant(nhits),1.);
    HF1(ECHISTR+14,geant(emeast),1.);
    HF1(ECHISTR+15,geant(emeast),1.);
  }
  if(nhits>0)stat=0;
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

integer Ecal2DCluster::Out(integer status){
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
  EcalHitNtuple* TN = AMSJob::gethead()->getntuple()->Get_ecalhit();

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOTCLONES__
       AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
    if (TN->Necht>=MAXECHITS ) return;
// Fill the ntuple
    TN->Status[TN->Necht]=_status;
    TN->Idsoft[TN->Necht]=_idsoft;
    TN->Proj[TN->Necht]=_proj;
    TN->Plane[TN->Necht]=_plane;
    TN->Cell[TN->Necht]=_cell;
    TN->Edep[TN->Necht]=_edep;
    if(_proj){ //<-- y-proj
      TN->Coo[TN->Necht][0]=_cool;
      TN->Coo[TN->Necht][1]=_coot;
    }
    else{     //<-- x-proj
      TN->Coo[TN->Necht][0]=_coot;
      TN->Coo[TN->Necht][1]=_cool;
    }
    AMSECIdSoft ic(getid());
    TN->Coo[TN->Necht][2]=_cooz;
    TN->ADC[TN->Necht][0]=_adc[0];
    TN->ADC[TN->Necht][1]=_adc[1]*ic.gethi2lowr();
    TN->ADC[TN->Necht][2]=_adc[2]*ic.getan2dyr();
    TN->Ped[TN->Necht][0]=ic.getped(0);
    TN->Ped[TN->Necht][1]=ic.getped(1);
    TN->Ped[TN->Necht][2]=ic.getpedd();
    TN->Gain[TN->Necht]=ic.getgain();
    TN->Necht++;
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
//---------------------------------------------------





//---------------------------------------------------
void Ecal1DCluster::_writeEl(){
 //
 // Root related part moved to 
 // ../CC/root.C:EcalClusterRoot::EcalClusterRoot
 //
  int i;
  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOTCLONES__
     AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
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
          AMSContainer *pc=AMSEvent::gethead()->getC("AMSEcalHit",_plane);
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
  }
}
void Ecal1DCluster::_copyEl()
{
#ifdef __WRITEROOTCLONES__
  EcalClusterRoot *ptr = (EcalClusterRoot*)_ptr;
  if (ptr) {
    // AMSEcalHit *_pHit[2*ecalconst::ECPMSMX];
    for (int i=0; i<_NHits; i++) {
    if (_pHit[i]) ptr->fEcalHit->Add(_pHit[i]->GetClonePointer());
  }
} else {
  cout<<"Ecal1DCluster::_copyEl -I-  Ecal1DCluster::EcalClusterRoot *ptr is NULL "<<endl;
}
#endif
}

integer Ecal1DCluster::build(int rerun){
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
      VZERO(adc,sizeof(adc)/sizeof(integer));
      VZERO(statusa,sizeof(statusa)/sizeof(integer));
      VZERO(ptrh,sizeof(ptrh)/sizeof(integer));
      proj=ptr->getproj();
    }
    number emax=-1;
    integer imax=-1;
    while(ptr){
      number edep=ptr->getedep();      
      if(!ptr->checkstatus(AMSDBc::BAD) && edep>emax){
        emax=edep;
        imax=ptr->getcell();
      };
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
     integer badchannel=ECcalib::BadCell(ipl,i);
     if(adc[i]<0 || (adc[i]==0  && badchannel))mincell++;
     else break;
    }
    for (int i=maxcell-1;i>=0;i--){
     integer badchannel=ECcalib::BadCell(ipl,i);
     if(adc[i]<0 || (adc[i]==0 && badchannel))maxcell--;
     else break;
    }

    for (int i=mincell;i<maxcell;i++){
     integer badchannel=ECcalib::BadCell(ipl,i);
     if((adc[i]<0 || (adc[i]==0 && badchannel))){
        bool bl= i-1>=0 && ECcalib::BadCell(ipl,i-1) ;
        bool br= i+1<Maxcell && ECcalib::BadCell(ipl,i+1) ;
       if(!bl && !br){
        adc[i]=(adc[i-1]+adc[i+1])/2;
        if(adc[i]){
         statusa[i]|=AMSDBc::LEAK;
         int st=statusa[i] | AMSDBc::RECOVERED;
         AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2));
          AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",ipl), pnew);
          ptrh[i]=pnew;
         
        }
       }
       else if(!bl){
        adc[i]=(adc[i-1])/2;
        if(adc[i]){
         statusa[i]|=AMSDBc::LEAK;
         int st=statusa[i] | AMSDBc::RECOVERED;
         AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2));
          AMSEvent::gethead()->addnext(AMSID("AMSEcalHit",ipl), pnew);
          ptrh[i]=pnew;
         
        }
       }
       else if(!br){
        adc[i]=(adc[i+1])/2;
        if(adc[i]){
         statusa[i]|=AMSDBc::LEAK;
         int st=statusa[i] | AMSDBc::RECOVERED;
         AMSEcalHit * pnew=new AMSEcalHit(st,proj,ipl,i,adc[i],ECALDBc::CellCoo(ipl,i,0),ECALDBc::CellCoo(ipl,i,1),ECALDBc::CellCoo(ipl,i,2));
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
     
     AMSEvent::gethead()->addnext(AMSID("Ecal1DCluster",proj),new Ecal1DCluster(status,proj,ipl,0,0,0,-ec,0,0,0,0,0,coo,0,coox2,0));
    }
}
}
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




void Ecal2DCluster::_writeEl(){

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    const int maxp=18;
#ifdef __WRITEROOTCLONES__
       AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
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
             AMSContainer *pc=AMSEvent::gethead()->getC("Ecal1DCluster",pat);
             TN->pCl[TN->Nec2dcl][i]+=pc->getnelem();
           }
          }
          else{
           for(int j=0;j<pat;j++){
          Ecal1DCluster *ptr=( Ecal1DCluster*)AMSEvent::gethead()->getheadC(" Ecal1DCluster",pat);
           while(ptr && ptr->checkstatus(AMSDBc::USED)){
            TN->pCl[TN->Nec2dcl][i]++;
            ptr=ptr->next();
           }
          }
         }
//        cout <<"pos "<<i<<" "<< TN->pCl[TN->Nec2dcl][i]<<endl;
        }

    TN->Nec2dcl++;
  }
}

void Ecal2DCluster::_copyEl(){
#ifdef __WRITEROOTCLONES__
  Ecal2DClusterRoot *ptr = (Ecal2DClusterRoot*)_ptr;
  if (ptr) {
    // Ecal1DCluster * _pCluster[4*ecalconst::ECSLMX];
    for (int i=0; i<_NClust; i++) {
      if (_pCluster[i]) ptr->fEcal1DCluster->Add(_pCluster[i]->GetClonePointer());
    }
  } else {
   cout<<"Ecal2DCluster::_copyEl -I- Ecal2DCluster::EcalClusterRoot *ptr is NULL "<<endl;
  }
#endif
}

integer Ecal2DCluster::build(int rerun){
  const integer Maxrow=ecalconst::ECSLMX*2;
     
   for (int proj=0;proj<ECALDBc::GetLayersNo();proj++){
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
     for(int ipl=pshmax->getplane()+1;ipl<ECALDBc::GetLayersNo();ipl++){
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
      for(int ipl=0;ipl<ECALDBc::GetLayersNo();ipl++){
       if(p1c[ipl])p1c[ipl]->setstatus(AMSDBc::USED);
      }
      Ecal2DCluster *pcl=new Ecal2DCluster(proj,tot,p1c,t0,tantz,chi2);
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
      Ecal2DCluster *p2d=(Ecal2DCluster*)AMSEvent::gethead()->
                       getheadC("Ecal2DCluster",0,1);
         number dist=FLT_MAX;
         Ecal2DCluster *p2dc=0;
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


bool Ecal2DCluster::StrLineFit(Ecal1DCluster *p1c[],int Maxrow,int proj,bool reset, number *pcorrect, int &tot, number &chi2, number &t0, number &tantz){
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

void Ecal2DCluster::_AddOrphane(Ecal1DCluster *ptr){
 _AddOneCluster(ptr,true);
 _OrpLeak+=ptr->getEnergy();
// cout <<" orphane found "<<ptr->getEnergy()<<endl;
}

void Ecal2DCluster::_AddOneCluster(Ecal1DCluster *ptr, bool addpointer){
  if(addpointer){
   ptr->setstatus(AMSDBc::USED);
   if(_NClust<sizeof(_pCluster)/sizeof(_pCluster[0]))_pCluster[_NClust++]=ptr;
   else cerr <<"Ecal2DCluster::_AddOneCluster-E-UnabletoAddCluster "<<ptr->getEnergy()<<"  mev energy lost "<<endl;
  }
  _EnergyC+=ptr->getEnergy();
  _SideLeak+=ptr->getsleak();
  _DeadLeak+=ptr->getdleak();
  number intercep=getcoo()+ptr->getcoo()[2]*gettan();
  number cm3=3;
  number cm5=5;
  number cm9=8;
  for(int i=0;i<ptr->getNHits();i++){
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

void Ecal2DCluster::_Fit(){
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


integer EcalShower::build(int rerun){

// Loop over 2dcluster
   for(;;){
      Ecal2DCluster *p2d=(Ecal2DCluster*)AMSEvent::gethead()->
                       getheadC("Ecal2DCluster",0,2);

      number BestMatch=FLT_MAX;
      Ecal2DCluster *p2dc[2]={0,0};      
      while(p2d){
      if(p2d->Good()){
       Ecal2DCluster *p2do=p2d->next();
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
         AMSEvent::gethead()->addnext(AMSID("EcalShower",0),new EcalShower(p2dc[0],p2dc[1]));         
       
      }
      else break;
     }






//  2nd pass needed in case there are orphaned 2d clusters

      Ecal2DCluster *p2di=(Ecal2DCluster*)AMSEvent::gethead()->
                       getheadC("Ecal2DCluster",0);

      for( Ecal2DCluster * p2d=p2di;p2d;p2d=p2d->next()){
        if(!p2d->checkstatus(AMSDBc::USED)){
          number difosumMin=1.e20;
          EcalShower* peca=0;
          EcalShower *pesi=(EcalShower*)AMSEvent::gethead()->
                       getheadC("EcalShower",0);
          for(EcalShower *pes=pesi;pes;pes=pes->next()){
           number enx=pes->getEnergyXY(0);
           number eny=pes->getEnergyXY(1);
           number adden=p2d->getproj()==0?p2d->getEnergy():-p2d->getEnergy();
           number difosum=fabs(enx-eny+adden)/(enx+eny+fabs(adden));
            if(difosum<difosumMin){
             peca=pes;
             difosum=difosumMin;
            }
          }
          if(peca){
           peca->AddOrphan(p2d);
          }
        }
      }
          EcalShower* peca=0;
          EcalShower *pesi=(EcalShower*)AMSEvent::gethead()->
                       getheadC("EcalShower",0);
          for(EcalShower *pes=pesi;pes;pes=pes->next()){
            pes->EnergyFit();
            pes->DirectionFit();
            pes->EMagFit();
            pes->SphFit();
          } 
return 1;
}


EcalShower::EcalShower(Ecal2DCluster *px, Ecal2DCluster *py):AMSlink(),_Et(0){
_Orp2DEnergy=0;
_pCl[0]=px;
_pCl[1]=py;
_N2dCl=2;




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
    AMSEvent::gethead()->addnext(AMSID("EcalShower",0),new EcalShower(AMSPoint(cofg),edep[2]/1000.,EcalFirstPlaneZ-_ECalShowerMax)); 
  }
*/


}


void EcalShower::_writeEl(){

  if(Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOTCLONES__
     AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
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
    TN->Orp2DEnergy[TN->Necsh]=_Orp2DEnergy;
     TN->Chi2Profile[TN->Necsh]=_ProfilePar[4+_Direction*5];
     for(int i=0;i<4;i++)TN->ParProfile[TN->Necsh][i]=_ProfilePar[i+_Direction*5];
     TN->Chi2Trans[TN->Necsh]=_TransFitChi2;
     for(int i=0;i<3;i++)TN->SphericityEV[TN->Necsh][i]=_SphericityEV[i];       
     for(int i=0;i<2;i++)TN->p2DCl[TN->Necsh][i]=_pCl[i]->getpos();
    TN->Necsh++;
  }

}

void EcalShower::DirectionFit(){

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
      Ecal2DCluster::StrLineFit(p1c,ECALDBc::GetLayersNo(),proj,true,_Zcorr,tot[proj],chi2[proj],t0[proj],tantz[proj]);
     }

  integer pr,pl,ce;
  number cl,ct;
  if(_Direction==0){
   ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_EntryPoint[2]);
   ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,2,0,pr,pl,ce,ct,cl,_ExitPoint[2]);
  }
  else{
   ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,_ExitPoint[2]);
   ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,2,0,pr,pl,ce,ct,cl,_EntryPoint[2]);
  }
      bool zcorr[2]={true,true};      
      for (int proj=0;proj<2;proj++){
        // Renonce if chi2 is bad;
        if(chi2[proj]*ECREFFKEY.Chi2Change2D>_pCl[proj]->_Chi2 ){
          t0[proj]=_pCl[proj]->_Coo;
          tantz[proj]=_pCl[proj]->_Tan;
#ifdef __AMSDEBUG__
          cerr<<" EcalShower::DirectionFit-W-correction Failed for proj "<<proj<<" new chi2 "<<chi2[proj]<<" old chi2 "<<_pCl[proj]->_Chi2<<endl;
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
      if(fabs(inter-ptr->getcoot())<ECALDBc::CellSize(front,ptr->getcell(),0))_FrontEnergyDep+=ptr->getedep();
     }
     ptr=ptr->next();

    }
      
}

number Ecal2DCluster::_ZCorr(Ecal1DCluster * p1c[],integer ipl, integer iplmax){
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

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

extern "C" void d01amf_(void *alfun, number &bound, integer &inf, number &epsa, number &epsr, number &result, number &abserr, number w[], int &lw, int iw[], int &liw , int &ifail, void *p );
void EcalShower::EnergyFit(){
    void (*palfun)(int &n, double x[], double &f, EcalShower *p)=&EcalShower::gamfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &EcalShower::monit;
    void (*psalfun)(double &x, double &f, EcalShower *p)=&EcalShower::gamfunr;

  number energy=0;
 _EnergyC=0;
 _Energy3C=0;
 _Energy5C=0;
 _Energy9C=0;
 _SideLeak=0;
 _DeadLeak=0;
 _RearLeak=0;
 _OrpLeak=0;
  for (int proj=0;proj<_N2dCl;proj++){
    energy+=_pCl[proj]->_Energy;   
   _EnergyC+=_pCl[proj]->_EnergyC;   
   _Energy3C+=_pCl[proj]->_Energy3C;   
   _Energy5C+=_pCl[proj]->_Energy5C;   
   _Energy9C+=_pCl[proj]->_Energy9C;   
//   Very  primitive side leak estimation (just double it)
   _SideLeak+=2*_pCl[proj]->_SideLeak;
   _EnergyC+=_pCl[proj]->_SideLeak;   
//
   _DeadLeak+=_pCl[proj]->_DeadLeak;
   _OrpLeak+=_pCl[proj]->_OrpLeak;
  }
  if(energy){
   _Energy3C/=energy;
   _Energy5C/=energy;
   _Energy9C/=energy;
  }


 _CofG=AMSPoint(0,0,0);
  const integer Maxrow=ECALDBc::GetLayersNo();
  number ec=0;
  _ShowerMax=-1;
  number xmax=-1;
  AMSPoint ep(0,0,0);
  for(int i=0;i<sizeof(_Zcorr)/sizeof(_Zcorr[0]);i++)_Zcorr[i]=0; 
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
    cerr<<"EcalShower::EnergyFit-E-CouldNotGet Z info"<<endl;
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
   // Now Add RearLeak
   if(_Edep[Maxrow-1]>ECREFFKEY.SimpleRearLeak[0]){
    number alpha=1-_Edep[Maxrow-1]*ECREFFKEY.SimpleRearLeak[2];
    if(alpha<=0){
     setstatus(AMSDBc::CATLEAK);
     cerr<<"EcalShower::EnergyFit-W-CATLEAKDetected "<<_Edep[Maxrow-1]<<endl;
     alpha=ECREFFKEY.SimpleRearLeak[1]*ec/FLT_MAX*100;
    }
    _EnergyC= ECREFFKEY.SimpleRearLeak[1]*ec/alpha;
    _RearLeak= _EnergyC-ECREFFKEY.SimpleRearLeak[1]*ec;
//    cout <<" case 1 "<<_EnergyC<<" "<<_RearLeak<<endl;
   }
   else{
    _RearLeak= ECREFFKEY.SimpleRearLeak[1]*ec*(_Edep[Maxrow-1]*ECREFFKEY.SimpleRearLeak[2]*_Edep[Maxrow-1]/ECREFFKEY.SimpleRearLeak[0]);
    _EnergyC= ECREFFKEY.SimpleRearLeak[3]*ec+_RearLeak;
//    cout <<" case 2 "<<_EnergyC<<" "<<_RearLeak<<endl;
   }
    _SideLeak=ECREFFKEY.SimpleRearLeak[3]*_SideLeak;
    _OrpLeak=ECREFFKEY.SimpleRearLeak[3]*_OrpLeak;
    _DeadLeak=ECREFFKEY.SimpleRearLeak[3]*_DeadLeak;
  }
  if(_EnergyC){
   _RearLeak/=_EnergyC;
   _OrpLeak/=_EnergyC;
   _DeadLeak/=_EnergyC;
   _SideLeak/=_EnergyC;
  }

// Final EnergyCorrection
   _EnergyCorr();

  if(_ShowerMax==Maxrow-1 || _ShowerMax==0){
   setstatus(AMSDBc::CATLEAK);
  }
  _AngleRes();
  _EnergyRes(); 
  // NowFit
    



    integer n=3;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=25000;
    const integer mp=4;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    number tol=3.99e-2;
    x[0]=1;
    x[1]=_Dz*_ShowerMax;
    x[2]=1;
     _Direction=0;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
     if(ifail==0){
     _ProfilePar[0]=x[0];    
     _ProfilePar[1]=x[1];    
     _ProfilePar[2]=x[2]!=0?1./x[2]:FLT_MAX;    
     _ProfilePar[4]=f;
//    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
    integer one=1;
    _iflag=3;
    ifail=1;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);

// Leak Estimation
    const integer lwc=1000;
    const integer liwc=lwc/4;
    number ww[lwc],bound,epsa,epsr,result,abserr;
    integer inf,iww[liwc];
    ifail=1;
    for(int i=Maxrow-1;i>=0;i++){
      if(_Edep[i]){
       bound=_Ez[i]-_Ez[0]+_Dz/2;
       break;
      }
    }
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
    int liw=liwc;
    int lw=lwc;
    d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
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
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
//    cout << "ecalshower::profilefit finished "<<ifail<<" "<<f<<endl;
    if(ifail==0){
     _ProfilePar[5]=x[0];    
     _ProfilePar[6]=x[1];    
     _ProfilePar[7]=x[2]!=0?1./x[2]:FLT_MAX;    
     _ProfilePar[9]=f;
    const integer lwc=1000;
    const integer liwc=lwc/4;
    number ww[lwc],bound,epsa,epsr,result,abserr;
    integer inf,iww[liwc];
    ifail=1;
    epsa=1.e-4;
    epsr=1.e-3;
    inf=1;
    int liw=liwc;
    int lw=lwc;
    for(int i=Maxrow-1;i>=0;i++){
      if(_Edep[i]){
       bound=_Ez[i]-_Ez[0]+_Dz/2;
       break;
      }
    }
     ifail=1;
     d01amf_((void*)psalfun, bound, inf, epsa, epsr,result,abserr,ww,lw,iww,liw,ifail,this);
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
     integer one=1;
     e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
     _Direction=1;
    }        
}

void EcalShower::gamfunr(number& x, number &fc, EcalShower *p){
 fc=0;
 if(x>0){
 number et=p->_Et;
 number xc[3];
 for(int i=0;i<3;i++){
  xc[i]=p->_ProfilePar[i+5*p->_Direction];
 }

 
 if(et){
  fc=pow(x,xc[1]/xc[2])*exp(-x/xc[2]);
  fc*=xc[0]/et;
 }
}
}

void EcalShower::gamfun(integer &n, number xc[], number &fc, EcalShower *p){
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
    number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
    number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
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
    number p1=x1>0?pow(x1,xc[1]*xc[2])*exp(-xc[2]*x1):0;
    number p2=x2>0?pow(x2,xc[1]*xc[2])*exp(-xc[2]*x2):0;
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
    number f1=x1>0?exp(min(log(FLT_MAX/2),-xc[2]*x1+xc[1]*xc[2]*log(x1))):0;
    number f2=x2>0?exp(min(log(FLT_MAX/2),-xc[2]*x2+xc[1]*xc[2]*log(x2))):0;
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
    cerr<<"EcalShower::Energyfit-W-TooBigZcorrAttempted "<<zcorr<<" "<<dz<<endl;
   }
 }
 p->_iflag=4;
}
}
void EcalShower::EMagFit(){
    void (*palfun)(int &n, double x[], double &f, EcalShower *p)=&EcalShower::expfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=&EcalShower::monit;

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
     for(int j=0;j<ptr->getNHits();j++){
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

     integer n=4;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=25000;
    const integer mp=5;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    number tol=3.99e-2;
    x[0]=1;
    x[1]=3;
    x[2]=0.1;
    x[3]=0.5;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    _TransFitPar[0]=x[1]!=0?1/x[1]:FLT_MAX;
    _TransFitPar[1]=x[3]!=0?1/x[3]:FLT_MAX;
    _TransFitPar[2]=x[2];
    _TransFitChi2=f;
  }
}

void EcalShower::expfun(integer &n, number xc[], number &fc, EcalShower *p){
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


integer EcalShower::Out(integer status){
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


void EcalShower::_AngleRes(){
_Angle3DError=0;
if(_EnergyC>0){
 _Angle3DError= 3.1415926/180.*sqrt(0.8*0.8+8.4*8.4/_EnergyC);
}
}


number EcalShower::_Chi2Corr(){
if(_EnergyC>0){
 return sqrt(0.35*0.35+100/_EnergyC);
}
else return 1;
}

void EcalShower::_EnergyCorr(){

//Try to take into account lower energy + adcoverflow
//pure phenomelogical one, should be replaced by more smart one...
if(_EnergyC){
 number minen=_EnergyC>2?_EnergyC:2;
 number lowencorr=1.0067-6.6e-2/pow(minen,0.5);
 _EnergyC/=lowencorr;
 if(_EnergyC>2500){
  number maxen=_EnergyC>15000?15000:_EnergyC;
  number hiencorr=(1-0.165)/(1-0.165*maxen/2500.);
  _EnergyC*=hiencorr;
 }
}
}


void EcalShower::_EnergyRes(){

_ErrEnergyC= fabs(_DifoSum)*_EnergyC/sqrt(2.);
}

number EcalShower::getTrue3DChi2(number tantz[2],number t0[2],bool zcorr[2]){
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
void EcalShower::SphFit(){
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


void EcalShower::AddOrphan(Ecal2DCluster *ptr){
 ptr->setstatus(AMSDBc::USED);
 _Orp2DEnergy+=ptr->_EnergyC/1000;

 if(_N2dCl<sizeof(_pCl)/sizeof(_pCl[0])){
  _pCl[_N2dCl++]=ptr;
  }
 else{
  cerr<<"EcalShower::AddOrphan-E-UnableToAdd "<<_Orp2DEnergy<<" GeV";
  setstatus(AMSDBc::BAD);
 }
}


number EcalShower::getEnergyXY(int proj) const{
 number enr=0;
 for(int i=0;i<_N2dCl;i++){
  if(_pCl[i]->getproj()==proj)enr+=_pCl[i]->getEnergy();
 }
 return enr;
}


//===============================================================


// int with DAQ


int16u AMSEcalRawEvent::getdaqid(int i){
  if (i<getmaxblocks())return ( (2+i) | 14 <<9);
  else return 0x0;
}

integer AMSEcalRawEvent::checkdaqid(int16u id){
 for(int i=0;i<getmaxblocks();i++){
  if(id==getdaqid(i))return i+1;
 }
 return 0;
}

void AMSEcalRawEvent::setTDV(){
  AMSTimeID * ptdv = AMSJob::gethead()->gettimestructure(getTDVped());
  if(ptdv)ptdv->Verify()=true;
              ptdv = AMSJob::gethead()->gettimestructure(getTDVcalib());
  if(ptdv)ptdv->Verify()=true;
              ptdv = AMSJob::gethead()->gettimestructure(getTDVvpar());
  if(ptdv)ptdv->Verify()=true;
}

void AMSEcalRawEvent::buildraw(integer n, int16u *p){

  integer ic=checkdaqid(*p)-1;
  int leng=0;
  int count=0; 
  int dynode=0;
  int dead=0;
  static geant sum_dynode=0;
  if(ic==0)sum_dynode=0;
  for(int16u* ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            if(!anode){
               channel=0;
               gain=3;
            }
  
           AMSECIdSoft id(ic,pmt,channel);//create id's
       if(id.dead()){
         dead++;
       }

       if(!id.dead()){
//           if(value>3500)cout <<id.makeshortid()<<" "<<gain<<" "<<channel<<" "<<anode<<" "<<value<<endl;
        if(anode){
            AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEvent",ic), new
          AMSEcalRawEvent(id,1-anode,1-gain,value));
         }
         else{
//  put here dynode related class

           AMSEvent::gethead()->addnext(AMSID("AMSEcalRawEventD",ic), new
          AMSEcalRawEvent(id,1-anode,gain,value));
          sum_dynode+=(value-id.getpedd())*id.getan2dyr()*id.getadc2mev();   
          dynode++; 
         }
       }    
   count++;               
  }
   //cout <<" Total of "<<count <<" "<<dynode<<" "<<sum_dynode<<" "<<dead<<" for crate "<<ic<<endl;
//  add two adc together
      AMSEcalRawEvent *ptro=0;
      AMSContainer * pct=AMSEvent::gethead()->getC("AMSEcalRawEvent",ic);
      for(AMSEcalRawEvent* ptr=(AMSEcalRawEvent*)AMSEvent::gethead()->getheadC("AMSEcalRawEvent",ic,1);ptr;ptr=ptr->next()){
        if(ptr->testlast()){
          if(ptro){
            if(ptr->getgain() <2 && ptro->getgain()<2 && ptr->getgain() !=ptro->getgain()){
              ptr->setgain(2);
              ptr->setadc(ptro->getadc(ptro->getgain()),ptro->getgain());
              ptr->TestThreshold();// subtr/suppress pedestals
              ptro->setstatus(AMSDBc::DELETED);
            }
            else{
              cerr<<"AMSEcalRawEvent::buildraw-S-FormatError "<<ptro->getgain()<<" "<<ptr->getgain()<<" "<<ptr->getid()<<endl;
              ptro->setstatus(AMSDBc::DELETED);
            }       
            ptro=0;
          }//---> endof ptro test         
          else{
            cerr<<"AMSEcalRawEvent::buildraw-E-No2ndGainFound "<<ptr->getid()<<endl;
            ptr->setstatus(AMSDBc::DELETED);
          }
        }//--->endof testlast
        else{
          ptro=ptr;
        }
      }//--->endof hits loop

      // Delete marked clusters
     AMSlink *pcl =AMSEvent::gethead()->getheadC("AMSEcalRawEvent",ic);
      while(pcl && pcl->checkstatus(AMSDBc::DELETED)){
        pct->removeEl(0,0);
        pcl=pct->gethead(); 
      }     
      while(pcl){
        while(pcl->next() && (pcl->next())->checkstatus(AMSDBc::DELETED))
        pct->removeEl(pcl,0);
        pcl=pcl->next();
      }
      // Restore positions
     AMSlink * ptmp=pct->gethead();
     integer ip=1;
     while(ptmp){
      ptmp->setpos(ip++);
      ptmp=ptmp->_next;
     }

// build lvl1 trigger
      if(ic==getmaxblocks()-1){
       uinteger tofpatt[4]={0,0,0,0};
       AMSEvent::gethead()->addnext(AMSID("TriggerLVL1",0),
          new Trigger2LVL1(999,0,tofpatt,0,12,sum_dynode/1000));

      }
}

void AMSEcalRawEvent::TestThreshold(){
 // hardwired here, should be via dcards
 if(_gain!=2)return ;
 geant HighThr=ECALVarp::ecalvpar.daqthr(0);//was 3
 geant LowThr=ECALVarp::ecalvpar.daqthr(4);//was 1
 geant LowAmp=10;
 AMSECIdSoft id(_idsoft);
 float x0=_padc[0]-id.getped(0);
 float x1=_padc[1]-id.getped(1);
 if((x0> id.getsig(0)*HighThr) ||  (x0> LowAmp && x0> id.getsig(0)*HighThr/3)){
   for(int i=0;i<2;i++){
      _padc[i]=floor((_padc[i]-id.getped(i)+1/ECALDBc::scalef())*ECALDBc::scalef());
      if(_padc[i]<0)_padc[i]=0;
   }
   //add dynode if any
      for(AMSEcalRawEvent*   ptrd=(AMSEcalRawEvent*)AMSEvent::gethead()->
                       getheadC("AMSEcalRawEventD",_id.getcrate(),1);ptrd;ptrd=ptrd->next()){
       if(*ptrd == *this){
//       cout <<"  dynodes found****"<<ptrd->getpadc(2)<<endl;;
        //dynode found
         _padc[2]=floor((ptrd->getpadc(2)-id.getpedd()+1/ECALDBc::scalef())*ECALDBc::scalef());
         break;
       }
     }

}
else if((x1> id.getsig(1)*HighThr*2) && x1> LowAmp ){// tempor (HighThr->LowThr ?)
   for(int i=0;i<2;i++){
      _padc[i]=floor((_padc[i]-id.getped(i)+1/ECALDBc::scalef())*ECALDBc::scalef
());
      if(_padc[i]<0)_padc[i]=0;
   //add dynode if any
      for(AMSEcalRawEvent*   ptrd=(AMSEcalRawEvent*)AMSEvent::gethead()->
                       getheadC("AMSEcalRawEventD",_id.getcrate(),1);ptrd;ptrd=ptrd->next()){
        if(*ptrd == *this){
          //dynode found
         _padc[2]=floor((ptrd->getpadc(2)-id.getpedd()+1/ECALDBc::scalef())*ECALDBc::scalef());
         break;
       }
     }
}
#ifdef __AMSDEBUG__
cout << "  HighGainChannelAbsent for "<<id.makeshortid()<<" "<<_padc[1]<<endl; 
#endif 
}


 else setstatus(AMSDBc::DELETED);
}


AMSEcalRawEvent::AMSEcalRawEvent(const AMSECIdSoft & id, int16u dynode,int16u gain,int16u adc):AMSlink(),_gain(gain),_id(id){
 for(int i=0;i<2;i++)_padc[i]=0;
  if(dynode)_padc[2]=adc;
  else{
   if(_gain<2)_padc[_gain]=adc;
  }
  _idsoft=id.makeshortid();
}
