//  $Id: antirec02.C,v 1.21 2005/10/13 09:01:30 choumilo Exp $
//
// May 27, 1997 "zero" version by V.Choutko
// June 9, 1997 E.Choumilov: 'siantidigi' replaced by
//                           (RawEvent + validate + RawCluster) + trpattern;
//                           complete DAQ-section added; Cluster modified
//
//    18.03.03 E.Choumilov Simu/Reco-logic completely changed according to 
//                                         AMS02 design. 
//
#include "typedefs.h"
#include "point.h"
#include "event.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "trigger102.h"
#include "tofdbc02.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "tofid.h"
#include "antidbc02.h"
#include "daqs2block.h"
#include "antirec02.h"
#include "ecalrec.h"
#include "ntuple.h"
#include "mceventg.h"
using namespace std;
//
//
 uinteger Anti2RawEvent::trpatt=0;
 integer Anti2RawEvent::nscoinc=0;
//----------------------------------------------------
void Anti2RawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u nadca[2],adca1[ANTI2C::ANAHMX];
  int16u ntdct[2],tdct1[ANTI2C::ANTHMX*2],tdct2[ANTI2C::ANTHMX*2];
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idN,stat[2];
  integer sector,isid,isds;
  integer i,j,im,nhit,chnum,am[2],sta,st;
  int bad;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  pbitn=TOF2GC::SCPHBP;// as for TOF
  pbanti=pbitn-1;
  status=1;//bad
  bad=1;// means NO both Charge and History measurements
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0,1);//last 1 to sort
  isds=0;
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]>=1)
  cout<<endl<<"=======> Anti::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//                             
  while(ptr){//<---- loop over ANTI RawEvent hits
#ifdef __AMSDEBUG__
    if(ATREFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    id=ptr->getid();// BBarSide
    sector=id/10-1;//bar 0-7
    isid=id%10-1;//0-1 (top/bot)
    chnum=sector*2+isid;//channels numbering
    stat[isid]=ptr->getstat();
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      ANTI2JobStat::addch(chnum,5);
//
//-----> check Charge-ADC :
//
      nhit=0;
      if(ptr->getadca() > 0)nhit=1;
      if(nhit==0)ANTI2JobStat::addch(chnum,6);
      if(nhit>0)bad=0;//found charge measurement
//
//-----> check Hist-TDC :
//
      ntdct[isid]=ptr->gettdct(tdct1);
      nhit=0;
      im=ntdct[isid];
      for(i=0;i<im;i++)tdct2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(tdct1[i]&pbitn);//check p-bit of down-edge (come first, LIFO mode !!!)
        pbup=(tdct1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOF2DBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        tdct2[nhit]=tdct1[i];
        nhit+=1;
        tdct2[nhit]=tdct1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good edges
      }
      if(nhit<im){//something was wrong (lost edge, overflow ?)
        ptr->puttdct(int16u(nhit),tdct2);// refill object with corrected data
        ANTI2JobStat::addch(chnum,8);
      }
      if(nhit>0)bad=0;//found history measurement
//-----      
    }//--->endof stat check 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//----> endof RawEvent hits loop
//
//
  if(bad==0)status=0;//good anti-event(were ADC+TDC measurements)
}
//----------------------------------------------------
void Anti2RawEvent::mc_build(int &stat){
  geant up(0),down(0),slope[2],athr[2],tau,q2pe,temp;
  geant eup(0),edown(0),tup(0),tdown(0);
  geant thresh[2];
  int i,ii,nup,ndown,nupt,ndownt,sector,sectorN,ierr,it,sta[2];
  int trflag(0),trflag1(0);
  uinteger j,ibmn[2],ibmx[2],nsides(0);
  uinteger ectrfl(0),trpatt(0),hcount[4],cbit,lsbit(1);
  int16u atrpat[2]={0,0};
  int16u phbit,maxv,id,chsta,adca;
  int16u nhtdch,htdc[2*ANTI2C::ANTHMX],itt;
  number edep,ede,edept(0),time,z,tlev1,ftrig,t1,t2,dt;
  geant fadcb[2][ANTI2C::ANFADC+1];
  geant hdthr,tdthr,gain[2],fladcb,htdcb,htdpr,hdmn;
  geant ped,sig,am,amp,tm,tmp,tmd1u,tmd1d,td1b1u,td1b1d,td1b2u,td1b2d,tmark;
  int upd1,upd11,upd12,stackof;
  integer nhtdc,iamp,ncoinc,ncoinct(0);
  number esignal[2][ANTI2C::MAXANTI];
  number htup[ANTI2C::ANTHMX],htdw[ANTI2C::ANTHMX];
  number ptup[ANTI2C::ANTHMX];
  number edepts(0);
  integer nptr;
  integer ssta,nphys,nlogs,nlogsN,nmchts(0);
  geant pwid,pdbr,pgate,tg1,tg2,mv2p,clfdel,attlen;
  AMSAntiMCCluster * ptr;
  AMSAntiMCCluster * ptrN;
  geant * parr;
  AMSBitstr trbs[ANTI2C::MAXANTI][2];
  AMSBitstr trbi;
  AMSBitstr trbl[ANTI2C::MAXANTI];
  geant trigb=TOF2DBc::trigtb();
  integer i1,i2;
//
  static integer first=0;
  static integer nshap;//real length of (light+PMT)-pulse(in bins=ANTI2DBc::fadcbw())
  static geant pshape[ANTI2C::ANFADC];//store PM single ph.el. pulse shape
//(+ result of time-dispersion of original light flash due to light collection) 
  if(first++==0){
    ANTI2DBc::inipulsh(nshap,pshape); // prep.PM-pulse shape arr.(in ANTI2DBc::fadcbw() bins)
  }
//
  VZERO(esignal,2*ANTI2C::MAXANTI*sizeof(esignal[0][0])/sizeof(geant));
  stat=1;//bad
  fladcb=ANTI2DBc::fadcbw();//flash-adc bin-width
  htdcb=ANTI2DBc::htdcbw();//hist-tdc bin width
  htdpr=ANTI2DBc::htdcdr();//h-tdc double pulse resol.
  hdmn=ANTI2DBc::hdpdmn();//h-discr. min. pulse-duration
  pwid=ANTI2DBc::ppfwid();//logic pulse width used for "paddles-in-FTcoinc" pattern  
  pdbr=ANTI2DBc::pbdblr();// .............. dbl-resol(dead time)
  pgate=ANTI2DBc::pbgate();// FT-gate, applied to "pattern" pulses
  trflag=TOF2RawEvent::gettrfl();//FT
  trflag1=TOF2RawEvent::gettrfl1();//FTZ
  Anti2RawEvent::setpatt(trpatt);// reset trigger-pattern in Anti2RawEvent::
  Anti2RawEvent::setncoinc(ncoinct);// reset # of coinc.sectors  to Anti2RawEvent::
//
  integer trtype(0);
  if(TGL1FFKEY.trtype>0)trtype=TGL1FFKEY.trtype;
  else trtype=Trigger2LVL1::l1trigconf.subtrigmask();
//
  if(trtype<256){//<==== NotExternalTrigger"
//
    if(trflag>=0 || trflag1>=0){// use FT from TOF
      ftrig=TOF2RawEvent::gettrtime();//abs. Time when FT came to S-crate(INCL. fixed delay !)
      tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
    }
    else{// have to use FT from ECAL
      ectrfl=AMSEcalRawEvent::gettrfl(); 
      if(ectrfl<=0)return;//no EC trigger also -> no chance to digitize ANTI
      ftrig=AMSEcalRawEvent::gettrtm();
      tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
    }
//
  }//===>endof "NotExtTrig" check
//
  else{//<=== ExtTrigger
    geant ttrig1=0.;//tempor (true ExtTrigSignal-time)
    ftrig=ttrig1+TOF2Varp::tofvpar.ftdelf();// FTrigger abs time (fixed delay added)
    tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
  }
//-----------
  geant padl=0.5*ANTI2DBc::scleng();
  phbit=TOF2GC::SCPHBP;// phase bit position as for TOF !!!
  maxv=phbit-1;// max possible TDC value (16383)
  ptr=(AMSAntiMCCluster*)AMSEvent::gethead()->
               getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
//
  edept=0.;
  ncoinct=0;
  for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
    fadcb[0][i]=0.;
    fadcb[1][i]=0.;
  }
  nupt=0;
  ndownt=0;
  ibmn[0]=ANTI2C::ANFADC;
  ibmn[1]=ANTI2C::ANFADC;
  ibmx[0]=0;
  ibmx[1]=0;
  for(i=0;i<ANTI2C::MAXANTI;i++){
    trbs[i][0].bitclr(1,0);
    trbs[i][1].bitclr(1,0);
    trbl[i].bitclr(1,0);
  }
//-------------
  AMSContainer *cptr;
  cptr=AMSEvent::gethead()->getC("AMSAntiMCCluster",0);//MC container-pointer
  ii=0;
  if(cptr)ii+=cptr->getnelem();
  if(ATMCFFKEY.mcprtf)HF1(2637,geant(ii),1.);
//----
  while(ptr){ // <--- geant-hits loop:
    sector=ptr->getid();//physical sector number(1-16)
    nphys=1-sector%2;   //internal index phys.sect as member of logical(0-1)
    nlogs=(sector-1)/2; //logical(readout) sector number(0-7)
    ede=ptr->getedep();
    edep=ede*1000;
    edept+=edep;
    edepts+=edep;
    z=ptr->getcoo(2);
    time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
//
    clfdel=ANTI2SPcal::antispcal[nlogs].gettzer(nphys);
    attlen=ANTI2VPcal::antivpcal[nlogs].getatlen(nphys);
//simulate side-2(up) responce:    
    if(nphys==0){
      mv2p=ANTI2VPcal::antivpcal[nlogs].getmev2pe1(1);
      ssta=ANTI2VPcal::antivpcal[nlogs].getstat1(1);
    }
    else{
      mv2p=ANTI2VPcal::antivpcal[nlogs].getmev2pe2(1);
      ssta=ANTI2VPcal::antivpcal[nlogs].getstat2(1);
    }
//    
    eup=0.5*edep*exp(z/attlen)*mv2p;// aver. up-side(+z) signal(pe,S2)
//                                  mv2p is implied to be measured at the center 
    POISSN(eup,nup,ierr);//real number of up-side p.e's
    if(nup>0 && ssta==0){ 
      nupt+=nup;
      tup=(time+(padl-z)/ATMCFFKEY.LSpeed)+clfdel;
      j=uinteger(floor(tup/ANTI2DBc::fadcbw()));//time-bin number
      if(j<ibmn[1])ibmn[1]=j;//min.bin
      for(i=0;i<nshap;i++){//"dispersion"
        ii=i+j;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;//ovfl-bin
        fadcb[1][ii]+=nup*pshape[i];
      }
      if(ii>ibmx[1])ibmx[1]=ii;//max.bin
    }
//simulate side-1(down) responce:
    if(nphys==0){
      mv2p=ANTI2VPcal::antivpcal[nlogs].getmev2pe1(0);
      ssta=ANTI2VPcal::antivpcal[nlogs].getstat1(0);
    }
    else{
      mv2p=ANTI2VPcal::antivpcal[nlogs].getmev2pe2(0);
      ssta=ANTI2VPcal::antivpcal[nlogs].getstat2(0);
    }
//    
    edown=0.5*edep*exp(-z/attlen)*mv2p;//aver. down-side(-z) signal(pe,S1)
//                                  mv2p is implied to be measured at the center 
    POISSN(edown,ndown,ierr);//real number of down-side p.e's
    if(ndown>0 && ssta==0){ 
      ndownt+=ndown;
      tdown=(time+(padl+z)/ATMCFFKEY.LSpeed)+clfdel;
      j=uinteger(floor(tdown/ANTI2DBc::fadcbw()));
      if(j<ibmn[0])ibmn[0]=j;//min.bin
      for(i=0;i<nshap;i++){//"dispersion"
        ii=i+j;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;
        fadcb[0][ii]+=ndown*pshape[i];
      }
      if(ii>ibmx[0])ibmx[0]=ii;//max.bin
    }
//--------------
    ptrN=ptr->next();     
    sectorN=-1;
    if(ptrN)sectorN=ptrN->getid(); //next hit PhysSector-number
    nlogsN=(sectorN-1)/2; //Next logical(readout) sector number(0-7, -1 if nlogs was last)
    nmchts+=1;
//
    if(nlogsN != nlogs){//<--- Next(last) ReadoutSector: create signals for Current ReadoutSector
#ifdef __AMSDEBUG__
      assert(nlogs >=0 && nlogs < ANTI2C::MAXANTI);
#endif
    hdthr=ANTI2SPcal::antispcal[nlogs].gethdthr();//hist-discr thresh(pe's)
    esignal[1][nlogs]=nupt; // save tot.number of p.e.'s in buffer
    esignal[0][nlogs]=ndownt;
    if(ATMCFFKEY.mcprtf && (nlogs==0 || nlogs==7))HF1(2639,edepts,1.);
//
//--->upply threshold to flash-adc signal to get times
//
    for(j=0;j<2;j++){//<---------------- side loop(down->up)
      ncoinc=0;
      nptr=0;
      id=(nlogs+1)*10+j+1;//BBS (BB=ReadoutPaddle, S=side(1->down,2->up))
      parr=&fadcb[j][0];
      if(ATMCFFKEY.mcprtf==3)ANTI2DBc::displ_a(id,1,parr);//print pulse
//
//--->loop over flash-adc buffer,upply threshold to signal to get times
//
      nhtdc=0;
      upd1=0; //up/down flag for discr-1
      tmd1u=-9999.;//up-time of discr-1
      tmd1d=-9999.;//down-time of discr-1
//
      upd11=0;//up/down flag for hist-TDC(branche-1 of discr-1)
      td1b1u=-9999.;//discr1_branch1(hist-TDC) up-time
      td1b1d=-9999.;// ......................down-time
//
      upd12=0;//up/down flag for "pattern"(branche-2 of discr-1)
      td1b2u=-9999.;//discr1_branch2(pattern) up-time
      td1b2d=-9999.;// ......................down-time
      stackof=0;//stack overfl.flag
      amp=0.;
      tmp=fladcb*ibmn[j];
      tm=tmp;
      if(ibmx[j]==ANTI2C::ANFADC){
        ANTI2JobStat::addmc(1);
#ifdef __AMSDEBUG__
        cout<<"AntiRawEvent::mc_build-W: Flash-TDC buffer ovfl,id/PElast="
	                                <<id<<" "<<fadcb[j][ibmx[j]]<<endl;
#endif
	ibmx[j]=ANTI2C::ANFADC-1;//don't use overflow-bin
      }
//====>
      for(i=ibmn[j];i<=ibmx[j];i++){//<====== flash-adc buffer loop
        tm+=fladcb;
        am=fadcb[j][i];//instant ampl. from flash-adc buffer
//----------
// discr-1(anode,fast comparator for h-TDC ) up/down setting with LOW thr.:
        if(am>=hdthr){
          if(upd1 ==0){
            tmd1u=tmp+fladcb*(hdthr-amp)/(am-amp);// up time of discr.1(bin-width compencated)
            tmark=tmd1u;// no  t-dispersion by discr.itself
            upd1=1;
          }
        }
        else{
          if(upd1!=0 && (tm-tmd1u)>hdmn){ // down time (+min.duration check)
            upd1=0;
            tmd1d=tm;
          }
        }
//----------
        amp=am;// store ampl to use as "previous" one in next i-loop
        tmp=tm;// .......
//----------
//--->try set all branches of discr  when it is up:
// 
        if(upd1>0){//<--- disc-1 up ?
//"history-TDC branche" :        
          if(upd11==0){
            if((tm-td1b1d)>htdpr){ //dead-time(=h_TDC dbl.resol ?)-check
              upd11=1;  // set flag for h-TDC branch
              td1b1u=tmark;//store up-time
            }
          }
//"trig-pattern branch" :
            if(upd12==0){
              if((tm-td1b2u)>(pwid+pdbr)){//buzy time(pwid+dead) check for "trig-pattern"
                upd12=1;  // set flag 
		td1b2u=tm;//store up-time to use as previous in next "up" set
                if(nptr<ANTI2C::ANTHMX){//store upto ANTHMX(=8) up-edges
                  ptup[nptr]=tm;
                  nptr+=1;
                }
                else{
                  cerr<<"AntiRawEvent::mc_build-W: PatternPulse ovfl(hits number)"<<nptr<<endl;
                  upd12=2;//blockade on overflow for safety
                }
              } 
            }
	}//<-- end of discr-1 up-check     
//
//----------
//--->try reset all branches:
//
//"history-TDC branche":
        if(upd11==1){ 
          if(upd1==0 || i==ibmx[j]){//reset "h-TDC" if discr-1 down, or out-of-time 
            upd11=0;
            td1b1d=tm;
	    for(ii=nhtdc-1;ii>=0;ii--){//move up/down-times in the LIFO-stack
	      if(ii+1<ANTI2C::ANTHMX){
	        htup[ii+1]=htup[ii];
	        htdw[ii+1]=htdw[ii];
	      }
	    }
	    htup[0]=td1b1u;//write latest up-time
	    htdw[0]=td1b1d;//write latest down-time
            if(nhtdc<ANTI2C::ANTHMX)nhtdc+=1;
            else{
              if(stackof==0){//1st stack ovfl for given id
                ANTI2JobStat::addmc(2);
#ifdef __AMSDEBUG__
	        cout<<"AntiRawEvent::mc_build-W: h-TDC stack ovfl,id="<<id<<endl;
#endif
	      }
	      stackof=1;
            }
          }
        }
//"trig-pattern branch":
        if(upd12==1){// "trig-pattern pulse" self-reset(independ.of discr-1 state)
            if((tm-td1b2u)>pwid){//min duration (fixed pulse width) check
              upd12=0;                    // for self-clear
            }
        }
//-------------
      }//======> endof flash-adc buffer loop
//-------------
      bool anchok=(ANTIPeds::anscped[nlogs].PedStOK(j) &&
                  ANTI2VPcal::antivpcal[nlogs].CalStOK(j));//ReadoutChan-OK
//
//---> Create charge raw-hits:
//
      geant adc2pe=ANTI2SPcal::antispcal[nlogs].getadc2pe();
      geant daqthr=ANTI2SPcal::antispcal[nlogs].getdqthr();//DAQ-readout thresh(adc-ch)    
      ped=ANTIPeds::anscped[nlogs].apeda(j);// in adc-chan. units(float)
      sig=ANTIPeds::anscped[nlogs].asiga(j);
      if(ATMCFFKEY.mcprtf)HF1(2632+j,geant(esignal[j][nlogs]),1.);
      amp=esignal[j][nlogs]/adc2pe;//pe -> ADC-ch
      if(amp>TOF2GC::SCPUXMX)amp=TOF2GC::SCPUXMX;//PUX-chip saturation
      amp=amp+ped+sig*rnormx();// adc+ped(no "integerization")
      iamp=integer(floor(amp));//go to real ADC-channels("integerization")
      if(iamp>TOF2GC::SCADCMX){//check ADC-overflow
        ANTI2JobStat::addmc(3);
#ifdef __AMSDEBUG__
        cout<<"ANTI2RawEvent_mcbuld-W: ADC-range overflow,id="<<id<<endl;
#endif
        iamp=TOF2GC::SCADCMX;
      }
      amp=number(iamp)-ped;// subtract pedestal (loose "integerization" !)
      if(ATMCFFKEY.mcprtf)HF1(2634+j,amp,1.);
      adca=0;
      if(amp>daqthr*sig){// DAQ readout threshold check
	iamp=integer(floor(amp*ANTI2DBc::daqscf()+0.5));// floatADC -> internal DAQ bins
        itt=int16u(iamp);
        adca=itt;
      }
//
//---> Create history raw-hits:
//
      if(ATMCFFKEY.mcprtf)HF1(2636,geant(nhtdc),1.);
      nhtdch=0;
      for(i=0;i<nhtdc;i++){//        <--- htdc-hits loop ---
        ii=i;// htup/dw ALREADY(!) have LIFO readout order(1st elem = last stored(first read))
        t1=htup[ii]; 
        t2=htdw[ii];
        dt=tlev1-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(floor(dt/ANTI2DBc::htdcbw())); // conv. to hist-TDC binning
        if(it>maxv){//out of range
          ANTI2JobStat::addmc(4);
#ifdef __AMSDEBUG__
          cout<<"ANTI2RawEvent_mcbuild-W : Hist-TDC overflow(down edge) !!!"<<'\n';
#endif
          it=maxv;
        }
        itt=int16u(it);
        if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        htdc[nhtdch]=itt;
        nhtdch+=1;
        dt=tlev1-t1;// follow LIFO mode of readout : leading(up) edge - second
        it=integer(floor(dt/ANTI2DBc::htdcbw())); // conv. to hist-TDC binning
        if(it>maxv){//out of range
          it=maxv;
        }
        itt=int16u(it);
        if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        htdc[nhtdch]=itt;
        nhtdch+=1;
      }//--- end of htdc-hits loop --->
//
//----> create RawEvent-object and store trig-patt:
//
      if(anchok && adca>0 && nhtdch>0){
        chsta=0;// good
	id=(nlogs+1)*10+j+1;//BBarSide(BBS)
//cout<<"MCFillRaw:id="<<id<<" nadc/ntdc="<<nadca<<" "<<nhtdch<<" nhtdc="<<nhtdc<<endl;
//for(int ih=0;ih<nadca;ih++)cout<<adca[ih]<<" ";
//cout<<endl;
//for(int ih=0;ih<nhtdch;ih++)cout<<htdc[ih]<<" ";
//cout<<endl;
        temp=0;//no temp.meausrement in MC
        AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                       new Anti2RawEvent(id,chsta,temp,adca,nhtdch,htdc));
//
        tg1=ftrig;//GateUpTime=FTime+delay(decision+ pass to S-crate)
        tg2=tg1+pgate;//gate_end_time
	ncoinc=0;
	for(i=0;i<nptr;i++){//all 1-side "trig-pattern" pulses -> 1-side bitstream pattern
          trbi.bitclr(1,0);
          t1=ptup[i];//"trig-pattern" pulse up-time
	  t2=t1+pwid;//.............. pulse down-time
	  dt=tg1-t1;
//	  if(nptr==1 && ATMCFFKEY.mcprtf>0)HF1(2631,geant(dt),1.);
	  if(ATMCFFKEY.mcprtf>0)HF1(2631,geant(dt),1.);
          i1=integer(t1/trigb);
          i2=integer(t2/trigb);
          if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
          if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
          trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	  trbs[nlogs][j]=trbs[nlogs][j] | trbi;//make side-OR of all trig-pulses
          if(!(tg2<=t1 || tg1>=t2))ncoinc+=1;//count side-overlaps with gate
	}
	if(ncoinc)atrpat[j]|=1<<nlogs;//set single-side coincidence bit(not used now)
      }
//------------
    }//--->endof side loop
//
    if(TGL1FFKEY.antisc == 0)
                                trbl[nlogs]=trbs[nlogs][0] & trbs[nlogs][1];// 2-sides AND
    else
                                trbl[nlogs]=trbs[nlogs][0] | trbs[nlogs][1];// 2-sides OR
    trbl[nlogs].clatch();//25MHz-clock latch of 2-sides OR(AND) signal trbl
//
    edepts=0;
    nupt=0;
    ndownt=0;
    ibmn[0]=ANTI2C::ANFADC;
    ibmn[1]=ANTI2C::ANFADC;
    ibmx[0]=0;
    ibmx[1]=0;
    for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
      fadcb[0][i]=0.;
      fadcb[1][i]=0.;
    }
    nmchts=0;
    }//------------------> endof next(last) ReadoutSector check 
    ptr=ptr->next();
//  
  } // ---> end of geant-hits loop
//
  if(ATMCFFKEY.mcprtf)HF1(2630,edept,1.);
//--------------------------------------
//------> create Anti-trigger pattern, count sectors:
//(bits 1-8->sectors in coinc.with FT)
//
  tg1=ftrig;//GateUpTime=FTime+delay(decision+ pass to S-crate)
  tg2=tg1+pgate;//gate_end_time
  trpatt=0;//reset patt.
//-->count FT-coincided sectors,create sector's pattern :
  for(i=0;i<ANTI2C::MAXANTI;i++){//apply 240ns gate
    trbl[i].testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=i2*trigb;
      if(!(tg2<=t1 || tg1>=t2)){
        ncoinct+=1;//overlap -> incr. counter
	trpatt|=1<<i;//mark sector
      }
    }
  }
  if(ncoinct>0)ANTI2JobStat::addmc(5);
//
//-->make side patterns(does not exists now according Lin)                       
//  for(i=0;i<4;i++)hcount[i]=0;
//  if(ncoinct>0){
//    for(i=0;i<ANTI2C::MAXANTI;i++){
//      cbit=lsbit<<i;
//      if((i<2)||(i>5)){ // <-- x>0
//        if(atrpat[1]&cbit)hcount[3]+=1; // z>0
//        if(atrpat[0]&cbit)hcount[2]+=1; // z<0
//      }
//      else{              // <-- x<0
//        if(atrpat[1]&cbit)hcount[1]+=1; // z>0
//        if(atrpat[0]&cbit)hcount[0]+=1; // z<0
//      }
//    }
//
//    trpatt=(atrpat[0] | (atrpat[1]<<8));//1-8=>s1(bot), 9-16=>s2(top)
//    for(i=0;i<4;i++){//add 4 trig.counters
//      if(hcount[i]>3)hcount[i]=3;
//      trpatt|=hcount[i]<<(16+i*2);
//    }
//  }
//
  Anti2RawEvent::setncoinc(ncoinct);// add # of coinc.sectors  to Anti2RawEvent::
  Anti2RawEvent::setpatt(trpatt);// add trigger-pattern to Anti2RawEvent::
  if(ncoinct>0 || edept>0)stat=0;//found anti activity
//
}
//
//===========================================================================
//
void AMSAntiCluster::_writeEl(){
//
  if(AMSAntiCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
   int i;
#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// fill the ntuple
    AntiClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_anti();
    if (TN->Nanti>=MAXANTICL) return;
    TN->Status[TN->Nanti]=_status;
    TN->Sector[TN->Nanti]=_sector;
    TN->Ntimes[TN->Nanti]=_ntimes;
    TN->Npairs[TN->Nanti]=_npairs;
    for(i=0;i<_ntimes;i++)TN->Times[TN->Nanti][i]=_times[i];;
    for(i=0;i<_ntimes;i++)TN->Timese[TN->Nanti][i]=_etimes[i];;
    TN->Edep[TN->Nanti]=_edep;
    for(i=0;i<3;i++)TN->Coo[TN->Nanti][i]=_coo[i];
    for(i=0;i<3;i++)TN->ErrorCoo[TN->Nanti][i]=_ecoo[i];
    TN->Nanti++;
  }
}
//---
void AMSAntiCluster::_copyEl(){
}
//---
void AMSAntiCluster::_printEl(ostream & stream){
  stream <<"AMSAntiCluster: sta/sect= "<<_status<<" "<<_sector
         <<" ntm/npr="<<_ntimes<<" "<<_npairs
         <<" time1,2"<<_times[0]<<" "<<_times[1]
         <<" etime1,2"<<_etimes[0]<<" "<<_etimes[1]
         <<" Edep"<<_edep<<" Coo/err"<<_coo<<  " "<<_ecoo<<endl;
}
//--
void AMSAntiCluster::print(){
AMSContainer *p =AMSEvent::gethead()->getC("AMSAntiCluster",0);
 if(p)p->printC(cout);
}
//
//=============================
//
void AMSAntiCluster::build2(int &statt){
//(combine 2-sides of single paddle)
  bool anchok;
  int16u adca,ntdct,tdct[ANTI2C::ANTHMX*2];
  int16u id,idN,sta;
  int16u pbitn;
  int16u pbanti;
  number edep[2],uptm[2][ANTI2C::ANTHMX],edep2,ass,zc,erz,erfi,err;
  integer nuptm[2];
  integer ftc,ftcin[2];
  integer i,j,jmax,sector,isid,nsds,isdn[2],stat,chnum,n1,n2,i1min,i2min;
  number zcoo[2*ANTI2C::ANTHMX],times[2*ANTI2C::ANTHMX],etimes[2*ANTI2C::ANTHMX];
  number edept,t1,t2,t1mn,t2mn,dt,z1,z2;
  integer ntimes,npairs,nclust(0),nclustc(0),nclustp(0),status(0);
  geant padlen,padrad,padfi,paddfi,zcer1,zcer2,ped,sig,dtmin,attlen,tzer;
  int16u mtyp(0),otyp(0),crat(0);
  geant fttim,ftsum,ftnum;
  geant ftdel,ftwin;
  int nphsok;
  integer ftcoinc(0);
  uinteger lspatt(0);
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
//
  statt=1;//bad
  ftcoinc=Anti2RawEvent::getncoinc();//log.sectors in coinc.with FT
  lspatt=Anti2RawEvent::getpatt();//Coinc.sect.pattern
  if(ATREFFKEY.reprtf[0]>0){
    HF1(2510,geant(ftcoinc),1.);
    if(lspatt<=0)HF1(2511,0.,1.);
    for(i=0;i<ANTI2C::MAXANTI;i++)if(lspatt & 1<<i)HF1(2511,geant(i+1),1.);
  }
  pbitn=TOF2GC::SCPHBP;//phase bit position as for TOF
  pbanti=pbitn-1;// mask to avoid it.
  padlen=ANTI2DBc::scleng();
  padrad=ANTI2DBc::scradi()+0.5*ANTI2DBc::scinth();
  paddfi=360./ANTI2C::MAXANTI;
  zcer1=ATREFFKEY.zcerr1;//Z-coo err. for true pair case
  ftdel=ATREFFKEY.ftdel;
  ftwin=ATREFFKEY.ftwin;
  nsds=0;
  ntdct=0;
  edept=0.;
  edep[0]=0;
  edep[1]=0;
  ftcin[0]=0;
  ftcin[1]=0;
  while(ptr){ // <------ RawEvent hits loop
    id=ptr->getid();//BBS
    sector=id/10-1;//Readout(logical) sector number (0-7)
    isid=id%10-1;
    mtyp=1;
    otyp=0;
    AMSSCIds antid(sector,isid,otyp,mtyp);//otyp=0(anode),mtyp=1(hist. time(=fast_TDC))
    crat=antid.getcrate();
#ifdef __AMSDEBUG__
    assert(crat<TOF2GC::SCCRAT);
#endif
    fttim=TOF2RawCluster::getfttime(crat);//CS-FT=6mks from TOF-stretcher(may be diff. in each crate)
//(it may be missing in case of absent of TOF-hits in given crate !!!)
    if(fttim<=0){//find aver. over non empty crates)
      ftsum=0;
      ftnum=0;
      for(i=0;i<TOF2GC::SCCRAT;i++){
        fttim=TOF2RawCluster::getfttime(i);
	if(fttim>0){
	  ftsum+=fttim;
	  ftnum+=1;
	}
      }
      if(ftnum>0)fttim=ftsum/ftnum;
    }
    chnum=sector*2+isid;//channels numbering
    anchok=(ANTIPeds::anscped[sector].PedStOK(isid) &&
                  ANTI2VPcal::antivpcal[sector].CalStOK(isid));//Chan-DBStat OK
    if(anchok){// <--- channel alive
//channel statistics :
      ANTI2JobStat::addch(chnum,0); 
      adca=ptr->getadca();
      if(adca>0)ANTI2JobStat::addch(chnum,1);
//
      ntdct=ptr->gettdct(tdct);
      if(ntdct>0)ANTI2JobStat::addch(chnum,3);
      if(ntdct==2)ANTI2JobStat::addch(chnum,4);//1hit=2edges
      isdn[nsds]=isid+1;
//DAQ-ch-->edep(mev):
      ped=ANTIPeds::anscped[sector].apeda(isid);//adc-chan
      sig=ANTIPeds::anscped[sector].asiga(isid);//adc-ch sigmas
      edep[nsds]=number(adca)/ANTI2DBc::daqscf()       //DAQ-ch-->ADC-ch
                *ANTI2SPcal::antispcal[sector].getadc2pe() //ADC-ch-->p.e.
                /ANTI2VPcal::antivpcal[sector].getmev2pec(isid); //p.e.-->Edep(Mev)
//cout<<"    decoded edep="<<edep[nsds]<<endl; 
//TDC-ch-->time(ns):
      nuptm[nsds]=0;
      nphsok=ANTI2VPcal::antivpcal[sector].NPhysSecOK();
      if(nphsok==2)tzer=ANTI2SPcal::antispcal[sector].gettzerc();
      else tzer=ANTI2SPcal::antispcal[sector].gettzer(nphsok);
      ftc=0;
      for(i=0;i<ntdct;i++){//<-- hist-hits loop
        if(i%2==1){//use up-edges (i=1,3,...)
	  t1=(pbanti&tdct[i])*ANTI2DBc::htdcbw()
	                                + tzer;//TDC-ch-->ns + compens.tzero
	  if(fttim>0){//check coinc.with FT
	    dt=t1-fttim;
            if(ATREFFKEY.reprtf[0]>0)HF1(2509,geant(dt),1.);
            if(fabs(dt-ftdel)<ftwin)ftc+=1;//count coinc.
	  }
          uptm[nsds][nuptm[nsds]]=t1;   
//cout<<"    decoded Up-time="<<t1<<endl;
          nuptm[nsds]+=1;
        }
      }
      if(ftc>0)ftcin[nsds]+=ftc;//store nFTcoinc/side 
//
      nsds+=1;
    }//--->endof alive-check
//--------
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
      attlen=ANTI2VPcal::antivpcal[sector].getatlenc();
      padfi=paddfi*(0.5+sector);//current paddle phi
      erfi=paddfi/sqrt(12.);
      err=ANTI2DBc::scinth()/sqrt(12.);
      status=0;//sector ok
      ntimes=0;
      npairs=0;
      edep2=0;
      if(edep[0]>0 || edep[1]>0)ANTI2JobStat::addbr(sector,0);
//                                                   <-------- case1=2sides:
      if(nsds==2){
//                        <-- search for true SideTime-pairs(giving best zcoo):
	n1=nuptm[0];
	n2=nuptm[1];
	while(n1>0 && n2>0){//<-- try next pair ?
	  dtmin=9999.;
	  for(int i1=0;i1<nuptm[0];i1++){//<-- combinations loop
	    t1=uptm[0][i1];
	    if(t1<0)continue;
	    for(int i2=0;i2<nuptm[1];i2++){
	      t2=uptm[1][i2];
	      if(t2<0)continue;
              dt=fabs(t1-t2);
	      if(dt<dtmin){
	        dtmin=dt;
	        i1min=i1;
		i2min=i2;
	      }
	    }
	  }//-->endof combin. loop
	  t1=uptm[0][i1min];//best pair
	  t2=uptm[1][i2min];
//
	  zc=-0.5*(t1-t2)*ATMCFFKEY.LSpeed;//abs.Z(neglect by possible ANTI Z-shift)
	  if(fabs(zc)<(0.5*padlen+3*zcer1)){//match found(Zc is within paddle size)
	    if(zc>0.5*padlen)zc=0.5*padlen;
	    if(zc<-0.5*padlen)zc=-0.5*padlen;
	    zcoo[npairs]=zc;
	    times[npairs]=0.5*(t1+t2);
	    etimes[npairs]=zcer1/ATMCFFKEY.LSpeed;
	    npairs+=1;
	    n1-=1;
	    n2-=1;
	    uptm[0][i1min]=-1;//mark used hit(all times are positive)
	    uptm[1][i2min]=-1;
	  }
	  else break;//next pair will be bad(because current one was the best) 
//
	}//-->endof "try next pair" 
//
        if(npairs==1){//imply, that single existing Edep correlates with this single pair
	  zc=zcoo[0];
	  edep2=(edep[0]+edep[1])
               *2/(exp(zc/attlen)+exp(-zc/attlen));
	  erz=zcer1;
	}
	else{//npair=0,>1; not possible to associate single Edep and time
//        (to have something, let us calc.zc using Edep(is) assimetry)
          ass=(edep[1]-edep[0])/(edep[1]+edep[0]);
          zc=attlen/2*log((1.+ass)/(1.-ass));
          edep2=(edep[0]+edep[1])
          *2/(exp(zc/attlen)+exp(-zc/attlen));
          number ddelta=1/sqrt(edep[1]+edep[0]);
          number z1,z2;
          number d1=ass+ddelta;
          if(d1 >=1)z1=0.5*padlen;      
          else if(d1 <=-1)z1=-0.5*padlen;      
          else z1=attlen/2*log((1.+d1)/(1.-d1));
          number d2=ass-ddelta;
          if(d2 <=-1)z2=-0.5*padlen;      
          else if(d2 >=1)z2=0.5*padlen;      
          else z2=attlen/2*log((1.+d2)/(1.-d2));
          erz=fabs(z1-z2)/2;
	}
	ntimes+=npairs;
//
      }//--->endof 2s-case
//                                                  <--------- case1=1side:
      if(nsds==1){
        isid=isdn[nsds];//=1,2
        edep2=edep[nsds]*2;//rough estimation to compensate missing side
	zc=0.;
	erz=padlen/sqrt(12.);
        status|=TOFGC::SCBADB2;// set bit for one-side paddles
        if(isid==2)status|=TOFGC::SCBADB4;// set missing-side bit(s=2 if set)
      }//--->endof 1s-case
      if(ATREFFKEY.reprtf[0] && nsds==2)HF1(2508,geant(edep2),1.);
//---
      if(nsds>0 && edep2>ATREFFKEY.Edthr){//non-empty paddle -> create object
        for(i=0;i<nsds;i++){// add all unpaired times
	  for(j=0;j<nuptm[i];j++){
	    t1=uptm[i][j];
	    if(t1>0){
	      times[ntimes]=t1;
	      etimes[ntimes]=padlen/sqrt(12.)/ATMCFFKEY.LSpeed;
	      ntimes+=1;
	    }
	  }
	}
//
        AMSPoint coo(padrad,padfi,zc);
        AMSPoint ecoo(err,erfi,erz);
        ANTI2JobStat::addbr(sector,1);
//
//cout<<"   FillClustSect="<<sector+1<<" nsds/ntimes/npair="<<nsds<<" "<<ntimes<<" "<<npairs<<endl;
//cout<<"     edep2="<<edep2<<" coo="<<coo<<" err="<<ecoo<<endl;
//for(i=0;i<ntimes;i++)cout<<" times="<<times[i]<<" err="<<etimes[i]<<endl;
        if(ftcin[0]>0 || ftcin[1]>0)nclustc+=1;//count clust.with FT-coinc on any side
	else status|=TOFGC::SCBADB1;//set "missing FTcoinc on both sides"
	
        AMSEvent::gethead()->addnext(AMSID("AMSAntiCluster",0),
         new AMSAntiCluster(status,sector+1,ntimes,npairs,times,etimes,edep2,coo,ecoo));
//
	nclust+=1;
	if(npairs>0)nclustp+=1;
        edept+=edep2;
        if(ATREFFKEY.reprtf[0]){
          HF1(2502,geant(ntimes),1.);
          HF1(2503,geant(npairs),1.);
          if(npairs==1)HF1(2504,geant(zc),1.);
	  if(nsds==2 && npairs!=1)HF1(2505,geant(zc),1.);
          HF1(2506,geant(sector+1),1.);
        }
      }//--->endof non-empty paddle check
      
//
      nsds=0;
      ntdct=0;
      edep[0]=0;
      edep[1]=0;
      ftcin[0]=0;
      ftcin[1]=0;
    }//--->endof next sector check
//---
    ptr=ptr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//
  if(ATREFFKEY.reprtf[0]){
    HF1(2500,geant(edept),1.);
    HF1(2501,geant(nclust),1.);
    HF1(2512,geant(nclustc),1.);
    HF1(2507,geant(nclustp),1.);
  }
  if(edept>0)statt=0;//ok
//
}

//===============================

integer AMSAntiCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSAntiCluster",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}

//===================================================================================
//  DAQ-interface functions :
//
// function returns number of Anti_data-words ( incl. FT-hits)
// for given block/format (one(max) SFEA card/crate is implied !!!)
//
integer Anti2RawEvent::calcdaqlength(int16u blid){
  Anti2RawEvent *ptr;
  int16u id,ibar,isid;
  int16u crate,rcrat,antic,hwid,fmt;
  integer len(0),nhits(0),nzchn(0),cntw;
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
//
  if((2&(DAQS2Block::getdmsk(rcrat))) > 0){ // AntiSubdet. is in this crate 
  while(ptr){
    id=ptr->getid();// BBS
    ibar=id/10-1;
    isid=id%10-1;
    hwid=Anti2RawEvent::sw2hwid(ibar,isid);// CAA, always >0 here
    antic=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
//      nhits+=ptr->getnadca();// counts hits (edges) of TDCA
      nzchn+=ptr->getnzchn();// counts nonzero TDC-channels (only TDCA)
    } 
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
    len+=1; // hit_bitmask
    if(nhits>0){// nonempty SFEA
      nzchn+=2;// add 2 TDCT(=FT) channels
      nhits+=(2*1);// add 2x1 TDCT-hits(for MC each FT-channel contains 1 hit(edge))
      cntw=nzchn/4;
      if(nzchn%4 > 0)cntw+=1;// hit-counter words
      len+=(cntw+nhits);
    } 
  }
//
  else{ // =====> Raw format :
    if(nhits>0){
      nhits+=(2*1);// add 2x1 TDCT-hits(for MC each FT-channel contains 1 hit(edge))
      len=2*nhits;// each hit(edge) require 2 words (header+TDCvalue)
    }
  }// end of format check
//
  }// end of SFEA-presence test
//
  return len;
//
}
//-------------------------------------------------------------------------
void Anti2RawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_ANTI_data
// on output: len=ANTI_data_length; 
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nanti;
  int16u ibar,isid,id;
  int16u phbit,maxv,ntdc,tdc[ANTI2C::ANAHMX*2],ntdct,tdct[ANTI2C::ANAHMX*2];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,tdcc,chip,tdccm;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptlist[ANTI2C::ANCHSF];
//
  phbit=TOF2GC::SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=TOF2GC::SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
  len=0;
  if(2&DAQS2Block::getdmsk(rcrat) == 0)return;//no AntiSubdet. in this crate (len=0)
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]==2)cout<<"Anti::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
// ---> prepare ptlist[tdc_channel] - list of AntiRawEvent pointers :
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0);
  for(tdcc=0;tdcc<ANTI2C::ANCHSF;tdcc++)ptlist[tdcc]=0;// clear pointer array
  nanti=0;
  ic=0;
  len=ic;
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]==2){
//    cout<<"Anti2DAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
//    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
//    cout<<dec<<endl<<endl;
  }
#endif
}
//------------------------------------------------------------------------------
// function to build Raw: 
//  
void Anti2RawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
// on input:  *p=pointer_to_beginning_of_block (to block-id word)
//            len=bias_to_first_Anti_data_word
// on output: len=Anti_data_length.
//
  integer i,j,jj,ic,ibar,isid,lentot,bias;
  integer val,warnfl;
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hwch,hmsk,slad,chip,chipc,chc;
  int16u swid[ANTI2C::ANCHCH],mtyp,hcnt,shft,nhit,nzch,nzcch,sbit;
  int16u phbit,maxv,phbt,phbtp; 
  int16u ids,stat,chan;
  int16u ntdca[ANTI2C::ANCHCH],tdca[ANTI2C::ANCHCH][ANTI2C::ANAHMX*2],ntdct,tdct[ANTI2C::ANAHMX*2],dummy;
//
  phbit=TOF2GC::SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=TOF2GC::SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
  lentot=*(p-1);// total length of the block(incl. block_id)
  bias=len;
//
// decoding of block-id :
  ic=0;
      len=ic-len;//return pure Anti_data_length
//
//---------------
//
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]>=1){
//    cout<<"Anti2RawEventBuild::decoding: FOUND "<<len<<" good words, hex/bit dump follows:"<<endl;
//    int16u tstw,tstb;
//    if(dtyp==1){ // only for red.data
//      for(i=0;i<len;i++){
//        tstw=*(p+i+bias);
//        cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
//        for(j=0;j<16;j++){
//          tstb=(1<<(15-j));
//          if((tstw&tstb)!=0)cout<<"x"<<"|";
//          else cout<<" "<<"|";
//        }
//        cout<<endl;
//      }
//      cout<<dec<<endl<<endl;
//    }
  }
#endif
}
//------------------------------------------------------------------------------
//  function to get sofrware-id (BBS) for given hardware-channel (crate,antich):
//  (imply only one (max) SFEA card per crate)                    0-7   0-15
//
int16u Anti2RawEvent::hw2swid(int16u a1, int16u a2){
  int16u swid,hwch;
//
  swid=0;
  return swid;
}
//-----------------------------------------------------------------------
//  function to get hardware-id (CAA) for given software-chan (bar, side)
//                                                             0-15  0-1
//
int16u Anti2RawEvent::sw2hwid(int16u a1, int16u a2){
  static int16u first(0);
  integer i;
  int16u ibar,isid,swch,swid,hwid,crate,antic;
  static int16u hidlst[ANTI2C::MAXANTI*2]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<ANTI2C::MAXANTI);// bar(0-15)
  assert(a2>=0 && a2<2); // side(0-1)
#endif
//
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;
  hwid=0;
  return hwid;
}
