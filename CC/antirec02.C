//  $Id: antirec02.C,v 1.43 2011/05/10 21:11:42 choumilo Exp $
//
// May 27, 1997 "zero" version by V.Choutko
// June 9, 1997 E.Choumilov: 'siantidigi' replaced by
//                           (RawEvent + validate + RawCluster) + trpattern;
//                           complete DAQ-section added; Cluster modified
//
//    18.03.03 E.Choumilov Simu/Reco-logic completely changed according to 
//                                         AMS02 design. 
//    15.03.06   E.Choumilov - complete rebuild caused by new readout(new TDC)
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
#include "anticalib02.h"
using namespace std;
//
//
 uinteger Anti2RawEvent::trpatt=0;
 integer Anti2RawEvent::nscoinc=0;
//----------------------------------------------------
void Anti2RawEvent::validate(int &status){ //Check/correct RawEvent-structure
  integer nadca;
  geant adca;
  integer ntdct,tdct[ANTI2C::ANTHMX],nftdc,ftdc[ANTI2C::ANTHMX];
  int16u id,idN,stat;
  integer sector,isid,isds;
  integer tmfound,complm;
  integer i,j,im,nhit,chnum,am[2],sta,st,chn;
  int16u mtyp(0),otyp(0),crat(0),slot(0),tsens(0);
  geant ped,sig,temp;
  number adcf,rdthr,dt;
  int bad;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  status=1;//bad
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0,1);//last "1" to sort
  isds=0;
//
  if(ATREFFKEY.reprtf[1]>0)
  cout<<endl<<"=======> Anti::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
//-----
// ====> check for PedCalib data if PedCalJob(class/ds) :
//
  if((ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3)
                           && AMSJob::gethead()->isCalibration()
			                                          ){//PedCalJob
    int pedobj(0);
    ANTI2JobStat::addre(30);
    ANTPedCalib::hiamreset();
    while(ptr){//<--RawEvent-objects loop
      id=ptr->getid();// BBarSide
      sector=id/10-1;//bar 0-7
      isid=id%10-1;//0-1 (bot/top)
      stat=ptr->getstat();
      adca=ptr->getadca();
      if(stat==1){//not PedSubtractedData, fill PedCal arrays
        pedobj+=1;
	if(adca>0){
#pragma omp critical (accval_pedc)
{
	  ANTPedCalib::fill(sector,isid,adca);
}
	}
      }
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawEvent hits loop ------->
    if(pedobj>0)ANTI2JobStat::addre(31);
    return;//PedCalJob always exit here with status=1(bad) to bypass next reco-stages !!!
  }
//---------------------------------------                             
//
// =============> check/combine adc/tdc/FT/temper data :
//
  bad=1;// means NO both Charge and History measurements
  while(ptr){//<---- loop over ANTI RawEvent hits
    id=ptr->getid();// BBarSide
    sector=id/10-1;//bar 0-7
    isid=id%10-1;//0-1 (bot/top)
    mtyp=0;
    otyp=0;
    AMSSCIds antid(sector,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
    crat=antid.getcrate();
    slot=antid.getslot();//sequential slot#(0,1,...10)(4 last are fictitious for d-adcs)
    tsens=antid.gettempsn();//... sensor#(1,2,...,5)(not all slots have temp-sensor!; =link#)
    temp=TOF2JobStat::gettemp(crat,tsens-1);//fast(SFEA) temper. from static array(may be undef for MC or some RD)
    if(AMSJob::gethead()->isRealData())ptr->settemp(temp);
//   (some value may be still set to undefined in RawSide-obj) 
    else{
     temp=TOF2Varp::tofvpar.Tdtemp();//get true(DataCard/TDV) def.value for MC
     ptr->settemp(temp);//set true(DataCard/TDV) def.value for MC
     TOF2JobStat::puttemp(crat,tsens-1,temp);//just for compatibility
    }
    chnum=sector*2+isid;//channels numbering
    stat=ptr->getstat();//upto now it is just ped-subtr flag(should be =0(if PedSubtracted))
    if(stat>0 && ATREFFKEY.relogic==0){
      cout<<"AntiRawEvent::validate:-E- Found not PedSubtracted Data while NormalRun-Job  !!"<<endl;
      return;
    } 
//
//--->  fill working arrays for given side:
//
      chn=isid*8+sector;
      isds+=1;
      ANTI2JobStat::addch(chnum,5);
      adca=ptr->getadca();
      nadca=0;
      if(adca>0)nadca=1;
      ntdct=ptr->gettdct(tdct);
      nftdc=TOF2RawSide::FThits[crat][tsens-1];
      if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
        HF1(2515,geant(nadca),1.);
	if(ntdct<10)nhit=ntdct;
	else nhit=9;
        HF1(2515,geant(nhit+10),1.);
        HF1(2515,geant(nftdc+20),1.);
}
      }
      tmfound=0;
      complm=0;
//
//---> check FTtime info in related crat/slot :
//
      if(nftdc>0){
        for(i=0;i<nftdc;i++)ftdc[i]=TOF2RawSide::FTtime[crat][tsens-1][i];
	ptr->putftdc(nftdc,ftdc);//attach FTtime info to given AntiRawEvent-object
        if(nftdc>1)ANTI2JobStat::addch(chnum,7);//count multy FT cases
      }
      else{
        ANTI2JobStat::addch(chnum,6);
	stat=1;//mark missing FTtime(unrecoverable case)
      }
//
//---> check LTtime info :
//
      if(ntdct>0){
        if(nftdc>0 && adca>10 && ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
          for(i=0;i<ntdct;i++){
	    dt=(ftdc[0]-tdct[i])*ANTI2DBc::htdcbw();//ns
            HF1(2516,geant(dt),1.);//look at LTtime/FTtime correl
	  }
}
	}
      }
      else{
        ANTI2JobStat::addch(chnum,8);
	stat+=10;;//mark missing HistoryTime
      }
//
      if(nftdc>0 && ntdct>0)tmfound=1;//found object with LTtime & FTtime
// 
      if(tmfound==1 && ATREFFKEY.reprtf[0]>2){
#pragma omp critical (hf1)
{
        HF1(2587,geant(chn+1),1.);
}
      }
//
//
//---> check Charge-ADC :
//
      if(adca>0){//---->ped-subtr. and DAQ-readout thr. was already done during decoding or mcbuild stage
        if(tmfound==1)complm=1;//found object with complete t+amp measurement
	if(ATREFFKEY.reprtf[0]>2){
#pragma omp critical (hf1)
{
	  HF1(2570+chn,geant(adca),1.);
}
	}
      }
      else{
        ANTI2JobStat::addch(chnum,9);
	stat+=100;//mark missing Ampl.info
      }
//
      if(complm==1 && ATREFFKEY.reprtf[0]>2){
#pragma omp critical (hf1)
{
        HF1(2586,geant(chn+1),1.);
}
      }
//---
//
      ptr->updstat(stat);//set RawEvent-obj status to filter at reco-stage(befor it was =0(ok))
      if(complm==1)bad=0;//found at least one channel with t+amp measurement per event - accept event
//      if(nadca>0)bad=0;//tempor: found at least one channel with amp measurement per event - accept event
    if(ATREFFKEY.reprtf[1]>0)ptr->_printEl(cout);
//---------------
    ptr=ptr->next();// to take next RawEvent hit
  }//----> endof RawEvent hits loop
//
//
  if(bad==0)status=0;//good anti-event(were at least 1 RawEvent-obj with Time and Ampl measurements)
}
//----------------------------------------------------
geant Anti2RawEvent::accsatur(geant aout){
  geant norm(17);//tempor. scaling coeff to have about the same most prob of aout and ain
//  integer bmx=12;
//  geant ainref[12]={0,74,148,370,740,1480,2960,3256,4096,5400,21620,90700};//kunin
//  geant aouref[12]={0,74,120,220,340, 500, 740, 770, 840,1000, 2000, 4096};
  integer bmx=22;
  geant ainref[22]={0, 0.2, 0.67, 1.3, 2.6, 5, 10, 15, 20, 30, 40, 50, 70, 100,
                             150, 200, 300, 400, 500, 740, 1000, 4600};
  geant aouref[22]={0, 11,35,68,120,190,300,400,480,630,790,930,1200,1400,
                             1750,1900,2200,2400,2550,2850,3000,4000};
//
  if(aout<=0)return(0);
  for(int ib=0;ib<bmx-1;ib++){
    if(aout>aouref[ib] && aout<=aouref[ib+1]){
      return(norm*(ainref[ib]+(ainref[ib+1]-ainref[ib])/(aouref[ib+1]-aouref[ib])*(aout-aouref[ib])));
    }
  }
  return(norm*ainref[bmx-1]);
//
}
//----------------------------------------------------
void Anti2RawEvent::mc_build(int &stat){
  geant up(0),down(0),slope[2],athr[2],tau,q2pe,temp;
  geant eup(0),edown(0),tup(0),tdown(0);
  geant thresh[2];
  int i,ii,nup,ndown,sector,sectorN,ierr,it,sta[2];
  number npest[2][2];
  uinteger j,ibmn[2],ibmx[2],nsides(0);
  uinteger ectrfl(0),trpatt(0),hcount[4],cbit,lsbit(1);
  int16u atrpat[2]={0,0};
  int16u phbit,maxv,id,chsta;
  geant adca,daqthr,pe2adc,peaka,speaka;
  integer nftdc,ftdc[ANTI2C::ANTHMX], nhtdc,htdc[ANTI2C::ANTHMX],itt;
  number edep,ede,edept(0),time,z,tlev1,ftrig,lev1tm,lev1tms,htims,t1,t2,dt;
  geant fadcb[2][ANTI2C::ANFADC+1];
  geant hdthr,tdthr,gain[2],fladcb,htdcb,htdpr,hdmn;
  geant ped,sig,am,amp,tm,tmp,tmd1u,tmd1d,td1b1u,td1b1d,td1b2u,td1b2d,tmark;
  int upd1,upd11,upd12,stackof;
  integer iamp,ncoinc,ncoinct(0);
  number esignal[2][ANTI2C::MAXANTI];
  number htup[ANTI2C::ANTHMX];
  number ptup[ANTI2C::ANTHMX],ptdn[ANTI2C::ANTHMX];
  number edepts(0);
  integer nhtup,nptr;
  integer ssta,nphys,nlogs,nlogsN,nmchts(0);
  geant domn,dimn,didt,tgpw,tgdt,tdcd,dacc,pgate,tg1,tg2,mev2p,clfdel,attlen,attf;
  AMSAntiMCCluster * ptr;
  AMSAntiMCCluster * ptrN;
  geant * parr;
  AMSBitstr trbs[ANTI2C::MAXANTI][2];
  AMSBitstr trbi;
  AMSBitstr trbl[ANTI2C::MAXANTI];
  geant trigb=TOF2DBc::trigtb();
  integer i1,i2,intrig,sorand;
  geant daqp7,daqp10;
  geant nebav;
  int nebin;
  int16u crat(0),sslot(0);
  bool anchok;
//
  static integer first=0;
  static integer nshap;//real length of (PMT)-pulse array(in bins=ANTI2DBc::fadcbw())
  static integer ndisp;//real length of pe-arrival time dispersion array(in bins=ANTI2DBc::fadcbw())
  static geant pshape[ANTI2C::ANFADC+1];//store PM single ph.el. pulse shape
  static geant pedisp[ANTI2C::ANFADC+1];//store ph.el. arrival time dispersion(due to light collection)
  static geant mv2pc;
  static integer nshapo,ndispo;
  static integer err1(0);
  geant peavr;
  int petru;
  geant pevstm[2][ANTI2C::ANFADC+1];
//
  if(first++==0){
    mv2pc=ANTI2DBc::inipulsh(nshap,pshape);//prep.PM-pulse shape arr.(in fadcbw() bins) and 1mv->pC conv.fact
    ANTI2DBc::inipedisp(ndisp,pedisp); // prep. ph.el. arrival time dispersion arr(in ANTI2DBc::fadcbw() bins)
    cout<<"<------ Anti2RawEvent::mc_build: PE pulsesh/tspread init done, npsh/nspr="<<nshap<<" "<<ndisp<<endl;
    nshapo=nshap;
    ndispo=ndisp;
  }
  if(nshap!=nshapo || ndisp!=ndispo){
    if(err1++<50)cerr<<"<--- Anti2RawEvent::mc_build: Error - shape/spread dims damaged, nshap/ndisp="<<nshap<<" "<<ndisp<<"   "<<nshapo<<" "<<ndispo<<endl; 
  }
//
  VZERO(esignal,2*ANTI2C::MAXANTI*sizeof(esignal[0][0])/sizeof(geant));
  stat=1;//bad
  fladcb=ANTI2DBc::fadcbw();//flash-adc bin-width
  htdcb=ANTI2DBc::htdcbw();//hist/ft-tdc bin width
  domn=ANTI2DBc::dopwmn();// min.outPW of discr.(outPW=inpTovT-dimn when outPW>domn)
  dimn=ANTI2DBc::dipwmn();// minimal inp.pulse width(TovT) to fire discr.(its "rise time")
  didt=ANTI2DBc::didtim();// inp dead time of generic discr(outpDT=didt+dimn)(daqp4 analog)
  tgpw=ANTI2DBc::tgpwid();//"TrigPatt"-branch output pulse width(fixed, ACTEL-outp going to SPT/Lin)
  tgdt=ANTI2DBc::tgdtim();// dead time of "TrigPatt"-branch on ACTEL-output(going to SPT-inp)(analog of daqp8)
//                                      (Guido: ACTEL-inp is faster than Discr, so no ACTEL-inp DT check)
  tdcd=ANTI2DBc::tdcdtm();//dead time of TDC-inputs, the same for LT-/FT-inputs[ns](analog of daqp7)
  dacc=ANTI2DBc::daccur();//generic discr. intrinsic accuracy[ns](analog od daqp10)
  pgate=ANTI2DBc::pbgate();// FT-gate, applied to "pattern" pulses
  Anti2RawEvent::setpatt(trpatt);// reset trigger-pattern in Anti2RawEvent::
  Anti2RawEvent::setncoinc(ncoinct);// reset # of coinc.sectors  to Anti2RawEvent::
//
//-----
//
  integer ftpatt(0);
  ftrig=TOF2RawSide::gettrtime();//get FTrigger time in S-crate(incl.delay from JLV1 and decision time) 
  lev1tm=ftrig+TOF2DBc::lev1del();// "Lev1"-signal abs.time at S-crate
  ftpatt=TOF2RawSide::getftpatt();//get globFT members pattern (after masking)
  if(ftpatt==0)return;//<=== no globFT and ExtTrig
//
  ANTI2JobStat::addmc(7);//count FT existance
//---
  AMSContainer *cptr;
  cptr=AMSEvent::gethead()->getC("AMSAntiMCCluster",0);//MC container-pointer
  ii=0;
  if(cptr)ii+=cptr->getnelem();
  if(ATMCFFKEY.mcprtf)HF1(2637,geant(ii),1.);
  if(ii==0)return;//empty ACC
//---
  ANTI2JobStat::addmc(9);//count ACC non-empty
//---
  geant padl=0.5*ANTI2DBc::scleng();
  ptr=(AMSAntiMCCluster*)AMSEvent::gethead()->
               getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
//
  edept=0.;
  ncoinct=0;
  for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
    fadcb[0][i]=0.;
    fadcb[1][i]=0.;
    pevstm[0][i]=0;
    pevstm[1][i]=0;
  }
  npest[0][0]=0;
  npest[1][0]=0;
  npest[0][1]=0;
  npest[1][1]=0;
  ibmn[0]=ANTI2C::ANFADC;
  ibmn[1]=ANTI2C::ANFADC;
  ibmx[0]=0;
  ibmx[1]=0;
  for(i=0;i<ANTI2C::MAXANTI;i++){
    trbs[i][0].bitclr(1,0);
    trbs[i][1].bitclr(1,0);
    trbl[i].bitclr(1,0);
    esignal[0][i]=0;
    esignal[1][i]=0;
  }
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
//cout<<"  PHsect/Logsect="<<sector<<" "<<nlogs<<" Edep="<<edep<<" time="<<time<<endl;
//
//--->simulate side-2(up) responce caused by given geant-hit:    
//    
    attlen=ANTI2VPcal::antivpcal[nlogs].getatlen(nphys,1);
    mev2p=ANTI2DBc::mev2pe()*ANTI2SPcal::antispcal[nlogs].getmev2pe(nphys,1);//aver*indiv.corrections
    if(nphys==0)ssta=ANTI2VPcal::antivpcal[nlogs].getstat1(1);
    else ssta=ANTI2VPcal::antivpcal[nlogs].getstat2(1);
    attf=ANTI2VPcal::antivpcal[nlogs].SignalAtt(nphys,1,z);
    eup=0.5*edep*attf*mev2p;// aver. up-side(+z) signal(pe,S2)
//                                  ref. mev2p is implied to be measured at the center 
    if(eup>0 && ssta==0){ 
      tup=(time+(padl-z)/ATMCFFKEY.LSpeed)+clfdel;//hit arrival time at PM
      j=uinteger(floor(tup/ANTI2DBc::fadcbw()));//start abs time-bin 
      if(j<ibmn[1])ibmn[1]=j;//min.bin
      for(i=0;i<ndisp;i++){//apply PEs time dispersion
        ii=j+i;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;//time ovfl-bin
	if(i>=ANTI2C::ANFADC)break;
        pevstm[1][ii]+=eup*pedisp[i];//accumulate aver.PEs per time-bin 
        if(ii>ibmx[1])ibmx[1]=ii;//max.bin
      }
      npest[1][nphys]+=eup;//store S2 PEs for phys-sector
//cout<<" add to S2:pes="<<eup<<" tside="<<tup<<" ibmn/mx="<<ibmn[1]<<" "<<ibmx[1]<<endl;
    }
//
//--->simulate side-1(down) responce caused by given geant-hit:
//
    attlen=ANTI2VPcal::antivpcal[nlogs].getatlen(nphys,0);
    mev2p=ANTI2DBc::mev2pe()*ANTI2SPcal::antispcal[nlogs].getmev2pe(nphys,0);//aver*indiv.corrections
    if(nphys==0)ssta=ANTI2VPcal::antivpcal[nlogs].getstat1(0);
    else ssta=ANTI2VPcal::antivpcal[nlogs].getstat2(0);
    attf=ANTI2VPcal::antivpcal[nlogs].SignalAtt(nphys,0,z);
    edown=0.5*edep*attf*mev2p;//aver. down-side(-z) signal(pe,S1)
//                                  mev2p is implied to be measured at the center 
    if(edown>0 && ssta==0){ 
      tdown=(time+(padl+z)/ATMCFFKEY.LSpeed)+clfdel;//arrival time at PM
      j=uinteger(floor(tdown/ANTI2DBc::fadcbw()));//start abs time-bin 
      if(j<ibmn[0])ibmn[0]=j;//min.bin
      for(i=0;i<ndisp;i++){//apply pe's time dispersion
        ii=j+i;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;//time ovfl-bin
	if(i>=ANTI2C::ANFADC)break;
        pevstm[0][ii]+=edown*pedisp[i];//accumulate aver.pe's per time-bin per side;
        if(ii>ibmx[0])ibmx[0]=ii;//max.bin
      }
      npest[0][nphys]+=edown;//store S1 PEs for phys-sector
//cout<<" add to S1:pes="<<edown<<" tside="<<tdown<<" ibmn/mx="<<ibmn[0]<<" "<<ibmx[0]<<endl;
    }
//cout<<"LogSec="<<nlogs<<" nup/ndown="<<eup<<" "<<edown<<" edep="<<edep<<" mev2p="<<mev2p<<" atl="<<attlen<<endl;
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
    hdthr=ANTI2SPcal::antispcal[nlogs].gethdthr();//history-discr thresh(adc-ch units)
    if(ATMCFFKEY.mcprtf>0 && (nlogs>=0 && nlogs<=7))HF1(2639,edepts,1.);
//
//--->for nlogs-paddle create 2sides flash-adc arrays and upply threshold to get time-measurements
//
    for(j=0;j<2;j++){//<-------------------------- side loop(down->up) for current paddle nlogs
//cout<<"  --->Doing real signals for side="<<j+1<<endl;
//
      pe2adc=(npest[j][0]*ANTI2VPcal::antivpcal[nlogs].getpe2adc1(j)+
              npest[j][1]*ANTI2VPcal::antivpcal[nlogs].getpe2adc2(j))
                                 /(npest[j][0]+npest[j][1]);//physsect-weighted pe2adc for Q-channel
//--->create flash-adc array:
      int ibm=ibmx[j];
      geant pe2mv=pe2adc/ANTI2DBc::t2qgainr()     //this is scaling to T-channel electronics gain
                        /ANTI2DBc::pc2adc()       //this is to get pe2pc[pc/pe]
                        /mv2pc;                   //finally this is pe2mv[mv/pe] in T-channel including real PM-gain
//                                                       extracted from Q-channel adcs
//cout<<"    pe2mv="<<pe2mv<<" pe2adc="<<pe2adc<<" pc2adc="<<ANTI2DBc::pc2adc()<<" mv2pc="<<mv2pc<<endl;  
      for(i=ibmn[j];i<=ibm;i++){//<--- "average pes/bin" T-buffer(flash-adc) loop
        peavr=pevstm[j][i];      
	POISSN(peavr,petru,ierr);//get petru - real number of  PEs in time-bin
	speaka=0;
	for(int pe=0;pe<petru;pe++){//calc. real time-bin ampl(pe-units) caused by real PEs
	  peaka=1.+ANTI2DBc::perrms()*rnormx();//apply SE-ampl variations 
	  if(peaka<0)peaka=0; 
	  speaka+=peaka;
	}
//        esignal[j][nlogs]+=speaka*pe2adc; //tot.adc in Q-channel(no pulse-shape applied - "sample-and-hold")  
	if(speaka>0){//apply pulse shape for time-measurement
          for(int ish=0;ish<nshap;ish++){//pulse-shape bins loop
            ii=ish+i;
            if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;//time ovfl-bin
            fadcb[j][ii]+=(speaka*pshape[ish]*pe2mv);//pe->mV to get flash-adc ampl[mV] in T-channel
          }
          if(ii>ibmx[j])ibmx[j]=ii;//new max.bin(expanding of time-range is going only to higher values)
	}
      }//--->endof "average pes/bin" T-buffer loop
//cout<<"    new ibmn/mx="<<ibmn[j]<<" "<<ibmx[j]<<endl;
//---
      ncoinc=0;
      nptr=0;
      nhtup=0;
      id=(nlogs+1)*10+j+1;//BBS (BB=ReadoutPaddle, S=side(1->down,2->up))
      parr=&fadcb[j][0];
      if(ATMCFFKEY.mcprtf>2)ANTI2DBc::displ_a("AntiC PM-pulse:id(PPS)=",id,2,parr);//print pulse
//
//--->loop over flash-adc buffer,upply threshold to signal to get times
//
//--->
//   Based on available from Guido information, i created the following logic of
//      SFEA front part(simple and fast but may be wrong a little):
//    1) in LowThr(LT) logical channel there is a generic discriminator(fast core)
//      which require: min input pulse duration(~5ns) to go UP, have small dead time(~10ns),
//      have extending (proportional to input TovT) outp. pulse duration with some small 
//      minimum pulse width(~7ns)
//    2) the core is followed by slower output branches(Hist,FT) where its own dead time(buzy) state
//      is controlled, core up-times are stored in the FIFO buffer. Branch up-setting is driven
//      by discriminator, but is branch's own dead time controlled !. Branch may be self-resetted to
//      prowide nesessary outp.pulse width(independently on discr. state !). 
//<---
      int upd1=0; //up flag for LowThr discr-1(followed by History(time)- and TrigPatt-branches) 
      geant tmd1u=-9999.;//up-time of discr-1
      geant tmd1d=-9999.;//down-time of discr-1
      int pupd1=0;//pre_up-flag (to manage minimal inp.pulse duration(rise time) requirement)
      geant tpupd1=0;//time of the 1st pre_up (.................................)
      geant td1ref=-9999;
//
      int upd11=0;//up flag for history=precise_time(branche-1 of discr-1)
      geant tmd11u=-9999.;//branche-1 up-time
      geant tmd11d=-9999.;//branche-1 down-time
//
      int upd12=0;//up flag for "pattern"(branche-2 of discr-1)
      geant tmd12u=-9999.;//branche-2 up-time
      geant tmd12d=-9999.;//branche-2 down-time
      geant maxtu;
      amp=0.;
      tmp=fladcb*ibmn[j];//low edge of ibmn[j]-bin
      tm=tmp;
      if(ibmx[j]==ANTI2C::ANFADC){
        ANTI2JobStat::addmc(1);
#ifdef __AMSDEBUG__
        cout<<"AntiRawEvent::mc_build-W: Flash-TDC buffer ovfl,id/mV_last="
	                                <<id<<" "<<fadcb[j][ibmx[j]]<<endl;
#endif
	ibmx[j]=ANTI2C::ANFADC-1;//don't use overflow-bin
      }
//====>
      geant maxamp=0;
      geant qtotal=0;
      for(i=ibmn[j];i<=ibmx[j];i++){//<====== flash-adc buffer loop
        tm+=fladcb;//high edge of the current bin
        am=fadcb[j][i];//instant ampl.(adc-ch) from flash-adc buffer
//        am=am+5.*rnormx();//tempor high freq. noise(5mv rms)
        if(am>maxamp)maxamp=am;
	if(am>0)qtotal+=am;
//----------
// generic discr-1 up/down setting(used by TimeHistory and TrigPatt branches):
        if(am>=hdthr){// <=== Am>LTthr
	  if(pupd1==0 && (tm-tmd1d)>didt){// try set D1 pre-up state(inp. dead time check)
	    pupd1=1;
	    tpupd1=tm;
	    tmark=tmp+fladcb*(hdthr-amp)/(am-amp);//precise up time (bin-width compencated)
	  }
	  if(pupd1==1 && upd1==0){// try set d1 "up" if it is "down" and pre-up state OK 
            if((tm-tpupd1)>dimn){//min inpTovT check
              upd1=1;
              tmd1u=tm;// up time of discr.1(delayed wrt tpupd1 according to min inpTovT (rise time))
              td1ref=tmark+dimn;//store D1 "precise time"(when pre-up state was 1st time UP + FIXED rise-time)
            }
	  }
        }
        else{//Am<LTthr
	  pupd1=0;//reset pre-up state
          if(upd1!=0 && (tm-tmd1u)>domn){ //try reset D1 (min. outPW check)
            upd1=0;
            tmd1d=tm;//down time of discr.1
          }
        }
//----------
        amp=am;// store ampl to use as "previous" one in next i-loop(for interpolation)
        tmp=tm;// ......time .........................................................
//----------
//=============>try set all branches of discr-1  when it is up:
// 
        if(upd1>0 && tm==tmd1u){//<-- D1-up moment
//--->D1,branch-1(TimeHistory) :        
          if(upd11==0){//try to set branch-1 up
            if((tm-tmd11d)>tdcd){ // branch-1(TDC-input itself) dead time(buzy) check
              upd11=1;  // set up-state 
              tmd11u=tm;//store up-time 
              if(nhtup<ANTI2C::ANTHMX){//store upto ANTHMX up-edges
                htup[nhtup]=td1ref+dacc*rnormx();//store precise up-time + intrinsic fluct.
                nhtup+=1;
              }
              else{
                ANTI2JobStat::addmc(2);
	        cout<<"AntiRawEvent::mc_build-W: TimeHistory buffer ovfl,id="<<id<<endl;
                upd11=2;//to block buffer input
              }
            } 
          }
// ---> D1,branch-2(TrigPatt) :
          if(upd12==0){//try to set branch-2 up
            if((tm-tmd12d)>tgdt){//ACTELoutp=SPTinp dead time(buzy) check
              upd12=1;  // set up-state
              tmd12u=tm;//store up-time to use at self-reset(outpPW) check
            } 
          }
        }//<-- end of discr-1 up-check
//
//============>try reset all branches:
//
// "TimeHistory"-branche(#1) of Discr1
        if(upd11==1){ // branch is up - try reset it
          if(tm==tmd1d || i==ibmx[j]){//reset(go-to-ready) on D1 "down"-edge or on "time-out"(immediately)
	    upd11=0;
	    tmd11d=tm;//store down-time to use in next i-loop set-up (DT) check
	  }
        }
//--------
// "TrigPatt"-branch(#2) of Discr1(imply fixed outp.pulse width made by ACTEL)
        if(upd12==1){// branch is up - try reset it(depend.of discr-1 state)
	  maxtu=max(tmd12u,tmd1u);//latest from br12-up and d1-up
          if((tm-maxtu)>tgpw || i==ibmx[j]){//self-reset in "tgpw" after latest event: d1-up|br12-up|timeout
            upd12=0;
	    tmd12d=tm;//store down-time to use in next i-loop set-up stage
            if(nptr<ANTI2C::ANTHMX){//store upto ANTHMX up-edges
              ptup[nptr]=tmd12u;//don't need accurate up-time for trigger
              if(i<ibmx[j])ptdn[nptr]=tmd12d;
              else ptdn[nptr]=maxtu+tgpw;//"internal time-out" case, use fixed pwid starting from
//                                      max(tmd12u,tmd1u)(if there is exra D1up during bran12 "up"state, tmd1u > tmd12u)
	      if(ATMCFFKEY.mcprtf)HF1(2640,geant(ptdn[nptr]-ptup[nptr]),1.);//tempor
              nptr+=1;
            }
            else{
              ANTI2JobStat::addmc(8);
              cout<<"AntiRawEvent::mc_build-W: TrigPatt buffer ovfl"<<nptr<<endl;
              upd12=2;//to block buffer input
            }
          }
        }
//--------
      }//======> endof flash-adc buffer loop
//-------------
      qtotal*=(ANTI2DBc::fadcbw()/50.);//tot charge from T-channel flash-adc [pC]
      if(ATMCFFKEY.mcprtf>1 && nptr>1)ANTI2DBc::displ_a("AntiC_MultiFT, PM-pulse:id(PPS)=",id,20,parr);
      if(ATMCFFKEY.mcprtf>0){
        HF1(2641,maxamp,1.);
        HF1(2642,qtotal,1.);
      }
      esignal[j][nlogs]=qtotal*ANTI2DBc::pc2adc()*ANTI2DBc::t2qgainr(); //tot.adc in Q-channel  
// get current crate/sslot:
      AMSSCIds antid(nlogs,j,0,0);//otyp=0(anode),mtyp=0(LT-time)
      crat=antid.getcrate();
      sslot=antid.gettempsn();//formally is temper.sensor#(1,2,...,5), but give also SFET,SFEA seq.slot number
//
//---> Create charge raw-hits:
//
      daqthr=ANTI2SPcal::antispcal[nlogs].getdqthr();//DAQ-readout thresh(pedsig-units, "onboard")    
      ped=ANTIPedsMS::anscped[nlogs].apeda(j);// in adc-chan. units(float, MC-Seeds)
      sig=ANTIPedsMS::anscped[nlogs].asiga(j);
      if(ATMCFFKEY.mcprtf>0)
                     HF1(2632+j,geant(esignal[j][nlogs]/ANTI2VPcal::antivpcal[nlogs].getpe2adcc(j)),1.);
      amp=esignal[j][nlogs];//ADC-ch
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
//---
      adca=0;
      chsta=0;// good(def)
      if(ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3){//PedCalib-Job (no ped subtr/suppr)
        chsta=1;// signal for validate-stage: no "ped" subtraction/suppression
        if(iamp>0)adca=geant(iamp)+0.5;//"+0.5" to be at ADC-bin middle
      }
      else{//normal run
        ped=ANTIPeds::anscped[nlogs].apeda(j);// in adc-chan. units(float, ReallyMeasured)
        sig=ANTIPeds::anscped[nlogs].asiga(j);
        if(iamp>(ped+daqthr*sig))adca=geant(iamp)+0.5-ped;//OnBoard_PedSuppression + OfflinePedSubtraction
	else adca=0;
      }
//      if(ATMCFFKEY.mcprtf>0)HF1(2634+j,adca,1.);
//
//---> Create history(LT) raw-hits:
//
      nhtdc=0;
      for(i=0;i<nhtup;i++){//        <--- htdc-hits loop ---
        t1=htup[i];// TimeHist abs.time, FIFO readout order(1st elem = 1st stored(first read)) 
        htims=t1+TOF2Tovt::TofATdcT0[crat][sslot-1];//add sincronization(CoarseCounter state)
        lev1tms=lev1tm+TOF2Tovt::TofATdcT0[crat][sslot-1];//add sincronization(TrigTimeTagCounter state)
	it=TofTdcCorMS::getbins(htims,lev1tms);//TDC's bin number (linear mode)
	if(it>=0){//hit is in TDC's search window
          htdc[nhtdc]=it;
          nhtdc+=1;
	}
	else{
          ANTI2JobStat::addmc(4);
#ifdef __AMSDEBUG__
          cout<<"ANTI2RawEvent_mcbuild-W : TimeHist-hit out of search window !!!"<<'\n';
#endif
        }
      }//--- end of htdc-hits loop --->
      if(ATMCFFKEY.mcprtf>0)HF1(2636,geant(nhtdc),1.);
//
//----> create RawEvent-object and store trig-patt:
//
      anchok=(ANTIPeds::anscped[nlogs].PedStOK(j) &&
                  ANTI2VPcal::antivpcal[nlogs].CalStOK(j));//ReadoutChan-OK (real= not seed)
//
      if(anchok && (adca>0 || nhtdc>0)){
	id=(nlogs+1)*10+j+1;//BBarSide(BBS)
//cout<<"MCFillRaw:id="<<id<<" adca/nhtdc="<<adca<<" "<<nhtdc<<endl;
//for(int ih=0;ih<nhtdc;ih++)cout<<htdc[ih]<<" ";
//cout<<endl;
        temp=999;//means undefined value(real/default one will be set at valid-stage from common with TOF static store)
	nftdc=0;//dummy(real will be set at validation stage using TOF(+Anti) static store for slot's FT-times)
	ftdc[0]=0;
//cout<<"   writeRawEvent: id="<<id<<" adc="<<adca<<endl;
        AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                       new Anti2RawEvent(id,chsta,temp,adca,nftdc,ftdc,nhtdc,htdc));//write object
//--->check GlobFT/SideTrigPattSignal correlation(physically done in JLV1-crate)
        intrig=Trigger2LVL1::l1trigconf.antinmask(int(nlogs));//AntiInTrig from DB
        if(intrig==1 || (intrig>1 && (intrig-2)!=j)){//<--sector/side is in trigger(not masked)
          tg1=ftrig-ATMCFFKEY.FTdel;//GateUpTime=glFTime(at JLV1 !!!) 
          tg2=tg1+pgate;//gate_end_time
	  ncoinc=0;
	  for(i=0;i<nptr;i++){// 1-side "trig-pattern" pulses -> 1-side bitstream pattern
            trbi.bitclr(1,0);
            t1=ptup[i];//"trig-pattern" pulse up-time
	    t2=ptdn[i];//.............. pulse down-time
	    dt=tg1-t1;
//	    if(nptr==1 && ATMCFFKEY.mcprtf>0)HF1(2631,geant(dt),1.);
	    if(ATMCFFKEY.mcprtf>0)HF1(2631,geant(dt),1.);
            i1=integer(t1/trigb);
            i2=integer(t2/trigb);
            if(i1>=TOFGC::SCBITM)i1=TOFGC::SCBITM-1;
            if(i2>=TOFGC::SCBITM)i2=TOFGC::SCBITM-1;
            trbi.bitset(i1,i2);//set bits according to hit-time and pulse width
	    trbs[nlogs][j]=trbs[nlogs][j] | trbi;//make side-OR of all trig-pulses
	  }
	}//--->endof "InTrig" check
//	if(ncoinc)atrpat[j]|=1<<nlogs;//set single-side coincidence bit(not used now)
      }
//----
    }//------------------------------------>endof sides loop for paddle nlogs
//
    sorand=Trigger2LVL1::l1trigconf.antoamask(int(nlogs));//AntiSidesOrAnd from DB/File(already redef by DC)
    if(sorand == 0)
                   trbl[nlogs]=trbs[nlogs][0] & trbs[nlogs][1];// 2-sides AND
    else
                   trbl[nlogs]=trbs[nlogs][0] | trbs[nlogs][1];// 2-sides OR
    trbl[nlogs].clatchJLV();//25MHz-clock latch of 2-sides OR(AND) signal "trbl"
//
    edepts=0;
    npest[0][0]=0;
    npest[1][0]=0;
    npest[0][1]=0;
    npest[1][1]=0;
    ibmn[0]=ANTI2C::ANFADC;
    ibmn[1]=ANTI2C::ANFADC;
    ibmx[0]=0;
    ibmx[1]=0;
    for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
      fadcb[0][i]=0.;
      fadcb[1][i]=0.;
      pevstm[0][i]=0;
      pevstm[1][i]=0;
    }
    nmchts=0;
    }//------------------> endof next(last) ReadOutSector check 
    ptr=ptr->next();
//  
  } // ---> end of geant-hits loop
//
  if(ATMCFFKEY.mcprtf>0)HF1(2630,edept,1.);
//--------------------------------------
//------> create Anti-trigger pattern, count sectors:
//(bits 1-8->sectors in coinc.with FT)
//This pattern is produced in JLV1 !!!, So i need JLV1 FT-time, i.e.need to subtr.delay from S-cr time 
  tg1=ftrig-ATMCFFKEY.FTdel;//GateUpTime=ScrateFTime-delay (come back to JLV1 FT-time)
  tg2=tg1+pgate;//gate_end_time
  trpatt=0;//reset patt.
//-->count FT-coincided sectors,create sector's pattern :
  for(i=0;i<ANTI2C::MAXANTI;i++){//apply 240ns gate
    trbl[i].testbit(i1,i2);
    if(i2>=i1){
      t1=i1*trigb;
      t2=i2*trigb;
      if(ATMCFFKEY.mcprtf>0)HF1(2643,geant(tg1-t1),1.);
      if(tg1>=t1 && tg1<=t2){//tempor: gate fr.edge is within "pattern" side-pulse
        ncoinct+=1;//overlap -> incr. counter
	trpatt|=1<<i;//mark sector
      }
    }
  }
  if(ncoinct>0)ANTI2JobStat::addmc(5);
//
//---> filling of empty channels with peds (may be essential because true signal is too close to ped),
//     also needed for PedCalib-job:         
//
  for(int side=0;side<2;side++){
    for(int lsec=0;lsec<ANTI2C::MAXANTI;lsec++){
      anchok=(ANTIPeds::anscped[lsec].PedStOK(side)
                                         && ANTI2VPcal::antivpcal[lsec].CalStOK(side));//ReadoutChan-OK in DB
      if(esignal[side][lsec]==0){//<--empty(no MC-hits) channel(was ignored in Anti2RawEvent-obj creation above)
        ped=ANTIPedsMS::anscped[lsec].apeda(side);// in adc-chan. units(float, MC-Seeds)
        sig=ANTIPedsMS::anscped[lsec].asiga(side);
        daqthr=ANTI2SPcal::antispcal[lsec].getdqthr();//DAQ-readout thresh(pedsig-units)    
        amp=ped+sig*rnormx();//fill with ped
        iamp=integer(floor(amp));//go to real ADC-channels("integerization")
        adca=0;
        chsta=0;// good(def)
        if(ATREFFKEY.relogic==2 || ATREFFKEY.relogic==3){//PedCalib-job
          if(iamp>0)adca=geant(iamp)+0.5;//No ped-suppression for Ped-run, "+0.5" to be at ADC-bin middle
	  chsta=1;//signal for validation-stage: no "ped" subtraction/suppression
        }
        else{//normal run
          ped=ANTIPeds::anscped[lsec].apeda(side);// in adc-chan. units(float, really measured)
          sig=ANTIPeds::anscped[lsec].asiga(side);
          if(iamp>(ped+daqthr*sig))adca=geant(iamp)+0.5-ped;//OnBoard_PedSuppression + OfflinePedSubtraction
	  else adca=0;
        }
        if(anchok && adca>0){
	  id=(lsec+1)*10+side+1;//BBarSide(BBS)
	  nhtdc=0;//no time-history measurement for "peds" channels
	  htdc[0]=0;
          temp=999;//undefined (real/default value will be set at valid-stage)
	  nftdc=0;//really filled later during validation stage
	  ftdc[0]=0;
	  if(ATMCFFKEY.mcprtf<0){
	    cout<<"=====> Create Anti2RawEvent: swid="<<id<<" adca="<<adca<<endl;
	    cout<<"       nLThits="<<nhtdc<<"  LThits:";
	    for(i=0;i<nhtdc;i++)cout<<htdc[i]<<" ";
	    cout<<endl;
	  }
          AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                       new Anti2RawEvent(id,chsta,temp,adca,nftdc,ftdc,nhtdc,htdc));//write object
	}
      }//--->endof "empty channel" check
    }//--->endof sectors loop
  }//--->endof sides loop
//----
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
/*
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
*/
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
  integer ntdct,tdct[ANTI2C::ANTHMX],nftdc,ftdc[ANTI2C::ANTHMX];
  geant adca;
  int16u id,idN,sta;
  int16u pbitn;
  int16u pbanti;
  number adcf,edep[2],uptm[2][ANTI2C::ANTHMX],edep2,ass,zc,erz,erfi,err;
  integer nuptm[2];
  integer ftc,ftcin[2];
  integer i,j,jmax,sector,isid,nsds,isdn[2],stat,chnum,n1,n2,i1min,i2min;
  number zcoo[2*ANTI2C::ANTHMX],times[2*ANTI2C::ANTHMX],etimes[2*ANTI2C::ANTHMX];
  number edept,t1,t2,t1mn,t2mn,dt,z1,z2;
  integer ntimes,npairs,nclust(0),nclustc(0),nclustp(0),status(0);
  geant padlen,padrad,padfi,paddfi,zcer1,zcer2,ped,sig,dtmin,tzer;
  geant attlen[2],pe2adc[2],pes[2];
  int16u mtyp(0),otyp(0),crat(0),slot(0);
  geant fttim,ftsum,ftnum;
  geant ftdel,ftwin;
  geant mev2pe,mev2pea;
  geant rdthr;
  geant tempT[2]={0,0};
  geant stdifmx(15.);//tempor max(+/-) side-times diff for pairing(ns), later use calib for better accur
  int nphsok;
  integer ftcoinc(0);
  uinteger lspatt(0);
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
  mev2pea=ANTI2DBc::mev2pe();//aver mev2pe per paddle/side
//
//cout<<"--->In AntiCluster::build..."<<endl;
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
//
  statt=1;//bad
  ftcoinc=Anti2RawEvent::getncoinc();//total log.sectors in coinc.with FT
  lspatt=Anti2RawEvent::getpatt();//Coinc.sect.pattern
  if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
    HF1(2510,geant(ftcoinc),1.);
    if(lspatt<=0)HF1(2511,0.,1.);
    for(i=0;i<ANTI2C::MAXANTI;i++)if(lspatt & 1<<i)HF1(2511,geant(i+1),1.);
}
  }
  padlen=ANTI2DBc::scleng();
  padrad=ANTI2DBc::scradi()+0.5*ANTI2DBc::scinth();
  paddfi=360./ANTI2C::MAXANTI;
  zcer1=ATREFFKEY.zcerr1;//Z-coo err. for true pair case
  ftdel=ATREFFKEY.ftdel;//aver. FT-delay wrt Anti time-hit(at S-crate)
  ftwin=ATREFFKEY.ftwin;
  nsds=0;
  ntdct=0;
  nftdc=0;
  edept=0.;
  edep[0]=0;
  edep[1]=0;
  ftcin[0]=0;
  ftcin[1]=0;
  while(ptr){ // <------ RawEvent hits loop
    id=ptr->getid();//BBS
    sector=id/10-1;//Readout(logical) sector number (0-7)
    isid=id%10-1;
    mtyp=0;
    otyp=0;
    AMSSCIds antid(sector,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
    crat=antid.getcrate();
#ifdef __AMSDEBUG__
    assert(crat<TOF2GC::SCCRAT);
#endif
    slot=antid.getslot();//sequential slot#(0,1,...10)(but only one is SFEA-slot)
    chnum=sector*2+isid;//channels numbering
    sta=ptr->getstat();//ijk after validation
    tempT[isid]=ptr->gettemp();//SFEA-slot temper(was set at Validation-stage from static job-store)
    if(tempT[isid]==999.)tempT[isid]=TOF2Varp::tofvpar.Tdtemp();//set def.value if some bad sens. (RD)
    fttim=0;
    anchok=(sta%10==0                       //<--- validation status(FTtime is absolutely required)
                && ANTIPeds::anscped[sector].PedStOK(isid)
                && ANTI2VPcal::antivpcal[sector].CalStOK(isid)
		                                             );//DBstat OK
    if(anchok){// <--- channel alive according validation and DB
//channel statistics :
      ANTI2JobStat::addch(chnum,0); 
      adca=ptr->getadca();
      if(adca>0)ANTI2JobStat::addch(chnum,1);
      sig=ANTIPeds::anscped[sector].asiga(isid);//adc-ch sigmas
//---
      rdthr=ANTI2SPcal::antispcal[sector].getdqthr();//readout(former DAQ) thresh(ped-sigmas)
      rdthr*=1;//tempor: can apply slightly higher threshold than in DAQ
      adcf=number(adca);
      if(adcf>rdthr*sig){//----> Above additional thr.
        ANTI2JobStat::addch(chnum,2);
        if(ATREFFKEY.nlcorr==1
	 && AMSJob::gethead()->isRealData())adcf=number(Anti2RawEvent::accsatur(adca));//nonlin-corr
        ntdct=ptr->gettdct(tdct);
        if(ntdct>0)ANTI2JobStat::addch(chnum,3);
        if(ntdct==1)ANTI2JobStat::addch(chnum,4);//1hit
	nftdc=ptr->getftdc(ftdc);//should be >=1 due above stat-selection
        if(nftdc>0)fttim=ftdc[0]*ANTI2DBc::htdcbw();// tempor use 1st FTtime-hit (ch->ns)
        isdn[nsds]=isid+1;
        nphsok=ANTI2VPcal::antivpcal[sector].NPhysSecOK();//alive PhysSectNumb(0/1, 2 if both OK)
	if(nphsok==2)mev2pe=mev2pea*ANTI2SPcal::antispcal[sector].getmev2pec(isid);//aver of 2 PhysSectors
	else mev2pe=mev2pea*ANTI2SPcal::antispcal[sector].getmev2pe(nphsok,isid);//individual
//DAQ-ch-->edep(mev):
        edep[nsds]=adcf
                  /ANTI2VPcal::antivpcal[sector].getpe2adcc(isid) //ADC-ch-->p.e.(aver of 2 PhysSectors)
                  /mev2pe; //p.e.-->Edep(Mev, aver of 2 PhysSectors), Light-atten.still present !!!
//cout<<" LSect="<<sector+1<<" rdthr/sigp="<<rdthr<<" "<<sig<<" adc/m2pe/pe2adc="<<adcf<<" "<<mev2pe<<
//           " "<<ANTI2VPcal::antivpcal[sector].getpe2adcc(isid)<<" is/edep="<<isdn[nsds]<<" "<<edep[nsds]<<endl;
//TDC-ch-->time(ns):
        nuptm[nsds]=0;
        if(nphsok==2)tzer=ANTI2SPcal::antispcal[sector].gettzerc();//aver. of 2 PhysSectors tzer
        else tzer=ANTI2SPcal::antispcal[sector].gettzer(nphsok);//individual
        ftc=0;
        for(i=0;i<ntdct;i++){//<-- history-hits loop
	  t1=tdct[i]*ANTI2DBc::htdcbw()
	                                  - tzer;//TDC-ch-->ns + "compens.tzero=fib.delay"(side-independent)
	  if(fttim>0){//check coinc.with FT
	    dt=fttim-t1;//Rel.time wrt FT("+" means "befor" FTtime)
            if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
	      HF1(2509,geant(dt),1.);
}
	    }
            if(fabs(dt-ftdel)<ftwin){//found coincidence(ftdel is exp.measured aver. FT-delay wrt Anti-hit)
	      ftc+=1;//count coinc.
              uptm[nsds][nuptm[nsds]]=dt;//keep all FTcoincided hit times (wrt FT)   
              nuptm[nsds]+=1;
	    }
	  }
        }
        if(ftc>0)ftcin[nsds]+=ftc;//store nFTcoinc/side to set cluster OK(FT-coinc) status later
//
        nsds+=1;
//---
      }//---> endof "AbovePed" check 
//
    }//--->endof alive-check
//--------
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
      padfi=paddfi*(0.5+sector);//current paddle phi
      erfi=paddfi/sqrt(12.);
      err=ANTI2DBc::scinth()/sqrt(12.);
      attlen[0]=ANTI2VPcal::antivpcal[sector].getatlenc(0);
      attlen[1]=ANTI2VPcal::antivpcal[sector].getatlenc(1);
      pe2adc[0]=ANTI2VPcal::antivpcal[sector].getpe2adcc(0);//proportional to (electronics+pm) gain
      pe2adc[1]=ANTI2VPcal::antivpcal[sector].getpe2adcc(1);
      status=0;//sector ok
      ntimes=0;
      npairs=0;
      edep2=0;
      if(edep[0]>0 || edep[1]>0)ANTI2JobStat::addbr(sector,0);
//                                                   <-------- case1=2sides:
      if(nsds==2){//both Edep>0
//                        <-- search for true SideTime-pairs(giving best zcoo):
	n1=nuptm[0];//FT-coincided
	n2=nuptm[1];
	if(n1==1 && n2==1){
	  t1=uptm[0][0];
	  t2=uptm[1][0];
	  if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
	    HF1(2520+sector,geant(t1-t2),1.);
}
	  }
	}
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
          if(fabs(t1-t2)<stdifmx){
	    zc=-0.5*(t1-t2)*ATMCFFKEY.LSpeed;//abs.Z(neglect by possible ANTI Z-shift)
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
	  edep2=edep[0]/ANTI2VPcal::antivpcal[sector].SignalAttc(0,zc)+
	        edep[1]/ANTI2VPcal::antivpcal[sector].SignalAttc(1,zc);//corr.for attenuation
	  erz=zcer1;
	}
	else{//npair=0,>1; not possible to associate single Edep and time
//        (to have something, let us calc.zc using Edep(is) assimetry and neglecting edge eff)
	  zc=log(edep[0]/edep[1])/(1/attlen[0]-1/attlen[1]);
	  if(zc>0.5*padlen)zc=0.5*padlen;
	  if(zc<-0.5*padlen)zc=-0.5*padlen;
	  edep2=edep[0]/ANTI2VPcal::antivpcal[sector].SignalAttc(0,zc)+
	        edep[1]/ANTI2VPcal::antivpcal[sector].SignalAttc(1,zc);//corr.for attenuation
	  erz=2*zcer1;
	}
	ntimes+=npairs;
//
      }//--->endof 2s-case
//                                                  <--------- case1=1side:
      if(nsds==1){
        isid=isdn[nsds-1];//=1,2
        edep2=edep[nsds-1]*2;//rough estimation to compensate missing side
	zc=0.;
	erz=padlen/sqrt(12.);
        status|=TOFGC::SCBADB2;// set bit for one-side paddles
        if(isid==2)status|=TOFGC::SCBADB4;// set missing-side bit(s=2 if set)
      }//--->endof 1s-case
      if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
        HF1(2508,geant(edep2),1.);
}
      }
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
        if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
          HF1(2502,geant(ntimes),1.);
          HF1(2503,geant(npairs),1.);
          if(npairs==1)HF1(2504,geant(zc),1.);
	  if(nsds==2 && npairs!=1)HF1(2505,geant(zc),1.);
          HF1(2506,geant(sector+1),1.);
}
        }
      }//--->endof non-empty paddle check
      
//
      nsds=0;
      ntdct=0;
      nftdc=0;
      edep[0]=0;
      edep[1]=0;
      ftcin[0]=0;
      ftcin[1]=0;
    }//--->endof next sector check
//---
    ptr=ptr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//
  if(ATREFFKEY.reprtf[0]>1){
#pragma omp critical (hf1)
{
    HF1(2500,geant(edept),1.);
    HF1(2501,geant(nclust),1.);
    HF1(2512,geant(nclustc),1.);
    HF1(2507,geant(nclustp),1.);
}
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
//
void Anti2RawEvent::_writeEl(){
//
  if(Anti2RawEvent::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
   int i;
#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
// fill the ntuple
    AntiRawSideNtuple* TN = AMSJob::gethead()->getntuple()->Get_antirs();
    if (TN->Nantirs>=MAXANTIRS) return;
    TN->swid[TN->Nantirs] = _idsoft;
    TN->stat[TN->Nantirs] = _status;
    TN->temp[TN->Nantirs] = _temp;
    TN->adca[TN->Nantirs] = _adca;
    TN->nftdc[TN->Nantirs] = _nftdc;
    for(int i=0;i<_nftdc;i++)TN->ftdc[TN->Nantirs][i] = _ftdc[i];
    TN->ntdct[TN->Nantirs] = _ntdct;
    for(int i=0;i<_ntdct;i++)TN->tdct[TN->Nantirs][i] = _tdct[i];
    TN->Nantirs++;
*/
  }
}
//------
integer Anti2RawEvent::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("Anti2RawEvent",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}

//===================================================================================

AMSID AMSAntiCluster::crgid(int i){
const char ants[5]="ANTS";
   return AMSID(ants,_sector);
}
//
