//  $Id: tofrec02.C,v 1.29 2005/01/04 16:48:01 choumilo Exp $
// last modif. 10.12.96 by E.Choumilov - TOF2RawCluster::build added, 
//                                       AMSTOFCluster::build rewritten
//              16.06.97   E.Choumilov - TOF2RawEvent::validate added
//                                       TOF2RawCluster::sitofdigi modified for trigg.
//              26.06.97   E.Choumilov - DAQ decoding/encoding added
//
#include <tofdbc02.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofsim02.h>
#include <trrec.h>
#include <tofid.h>
#include <tofrec02.h>
#include <antirec02.h>
#include <particle.h>
#include <daqs2block.h>
#include <tofcalib02.h>
#include <ntuple.h>
#include <time.h>
//
// mem.reservation for some static arrays:
//
uinteger TOF2RawCluster::trpatt[TOF2GC::SCLRS]={0,0,0,0};//just init. of static var.
integer TOF2RawCluster::trflag=0;
uinteger TOF2RawEvent::StartRun(0);
time_t TOF2RawEvent::StartTime(0);
AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};
integer AMSTOFCluster::_planes=0;
integer AMSTOFCluster::_padspl[TOF2GC::SCLRS]={0,0,0,0};
//
//
//-----------------------------------------------------------------------
void TOF2RawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u nftdc,nstdc,nadcd,nadcdl;
  int16u ftdc1[TOF2GC::SCTHMX2*2],stdc1[TOF2GC::SCTHMX3*4];
  int16u adca1,adcal1;
  int16u adcd1[TOF2GC::PMTSMX],adcdl1[TOF2GC::PMTSMX];
  int16u ftdc2[TOF2GC::SCTHMX2*2],stdc2[TOF2GC::SCTHMX3*4];
  int16u adca2,adcal2;
  int16u adcd2[TOF2GC::PMTSMX],adcdl2[TOF2GC::PMTSMX];
  integer ilay,last,ibar,isid,isds;
  integer i,j,im,nhit,chnum,brnum,am[2],tmi[2],itmf[2],sta,st;
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idd,idN,stat[2],idr;
  number tsr[3],ths[2],fstd,t1,t2,t3,t4,dt,adca,adcal,adcd,adcdl;
  geant mtma[2],mtmd[2];
  geant peda,pedd,peddl;
  int bad(1);
  static int first(0);
  geant charge,edep;
  TOF2RawEvent *ptr;
  TOF2RawEvent *ptrN;
//
  pbitn=TOF2GC::SCPHBP;
  pbanti=pbitn-1;
  status=1;//bad
  idr=TFREFFKEY.reprtf[4];// chan-id for histogramming of stretcher w1/w3
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(TOF2RawEvent*)AMSEvent::gethead()
                        ->getheadC("TOF2RawEvent",0,1);//last 1 to sort
  isds=0;
//
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]>=1)
  cout<<endl<<"======> TOF::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//
  if(first==0){//store run/time for the first event
    first=1;
    StartRun=AMSEvent::gethead()->getrun();
    StartTime=AMSEvent::gethead()->gettime();
  }
//---- Scint.data length monitoring:
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    im=DAQS2Block::gettbll();//total blocks length for current format
//    im=0;
//    for(i=0;i<8;i++)im+=DAQS2Block::calcblocklength(i);
    HF1(1107,geant(im),1.);
  }
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getid();
    id=idd/10;// short id=LBB
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
#ifdef __AMSDEBUG__
    assert(ilay>=0 && ilay<TOF2DBc::getnplns());
    assert(ibar>=0 && ibar<TOF2DBc::getbppl(ilay));
    assert(isid>=0 && isid<2);
    if(TFREFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
    edep=ptr->getedep();//tempor
    charge=ptr->getcharg();
//
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      nftdc=ptr->getftdc(ftdc1);
      nstdc=ptr->getstdc(stdc1);
      adca1=ptr->getadca();
      adcal1=ptr->getadcal();
      nadcd=ptr->getadcd(adcd1);
      nadcdl=ptr->getadcdl(adcdl1);
      TOF2JobStat::addch(chnum,12);
//---> check hist-TDC :
      nhit=0;
      im=nftdc;
      for(i=0;i<im;i++)ftdc2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(ftdc1[i]&pbitn);//check p-bit of down-edge (come first, LIFO mode !!!)
        pbup=(ftdc1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOF2DBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        ftdc2[nhit]=ftdc1[i];
        nhit+=1;
        ftdc2[nhit]=ftdc1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good edges
      }
      if(nhit<im){//something was wrong (lost edge, overflow ?)
        ptr->putftdc(int16u(nhit),ftdc2);// refill object with corrected data
        TOF2JobStat::addch(chnum,13);
      }
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im),1.);
//
//---> check stretcher-TDC :
      nhit=0;
      im=nstdc;
      for(i=0;i<im;i++)stdc2[i]=0;
      for(i=0;i<im-3;i++){// find all correct 4's of up/down bits settings
        pbdn=(stdc1[i]&pbitn);//check p-bit of 2-nd down-edge (come first, LIFO mode !!!)
        pbup=(stdc1[i+1]&pbitn);//check p-bit of 2-nd up-edge (come second)
        pbdn1=(stdc1[i+2]&pbitn);//check p-bit of 1-st down-edge (come third)
        pbup1=(stdc1[i+3]&pbitn);//check p-bit of 1-st up-edge (come fourth)
        if(TOF2DBc::pbonup()==1){
          if(pbup==0||pbup1==0||pbdn!=0||pbdn1!=0)continue;//wrong sequence, take next "4" 
        }
        else{
          if(pbup!=0||pbup1!=0||pbdn==0||pbdn1==0)continue;//wrong  sequence, take next "4" 
        }
// -> check timing of 4 edges:
        t1=(stdc1[i+3]&pbanti)*TOF2DBc::tdcbin(1);//1-st up-edge (in real time)
        t2=(stdc1[i+2]&pbanti)*TOF2DBc::tdcbin(1);//1-st down-edge
        t3=(stdc1[i+1]&pbanti)*TOF2DBc::tdcbin(1);//2-nd up-edge
        t4=(stdc1[i]&pbanti)*TOF2DBc::tdcbin(1);//2-nd down-edge
        dt=t2-t3;
        if(dt<5. || dt>24.)continue;//wrong "hole" width(w2), take next "4"
        dt=t1-t2;
        if(idr==idd)HF1(1138,geant(dt),1.);
        if(dt<10. || dt>200.)continue;//wrong "1st_pulse" width(w1), ...
        dt=t2-t4;
        if(idr==idd)HF1(1139,geant(dt),1.);
        if(dt<2000. || dt>6000.)continue;//wrong "2nd_pulse" width(w3), ...
//
        stdc2[nhit]=stdc1[i];
        nhit+=1;
        stdc2[nhit]=stdc1[i+1];
        nhit+=1;
        stdc2[nhit]=stdc1[i+2];
        nhit+=1;
        stdc2[nhit]=stdc1[i+3];
        nhit+=1;
        i+=3;// to bypass current 4 good edges
      }
      if(nhit<im){//something was wrong
        ptr->putstdc(int16u(nhit),stdc2);// refill object with corrected data
        TOF2JobStat::addch(chnum,14);
      }
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+20),1.);
//
//---> check Anode(h/l)-adc :
      im=0;
      if(adca1>0)im=1;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+40),1.);
      im=0;
      if(adcal1>0)im=1;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+50),1.);
      if(adca1==0 && adcal1==0)TOF2JobStat::addch(chnum,15);//something wrong
//
//---> check Dynode(h)-adc :
      im=nadcd;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+60),1.);
      im=nadcdl;
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+70),1.);
  number pedv,peds;
      pedv=TOFBPeds::scbrped[ilay][ibar].apedal(isid);
      peds=TOFBPeds::scbrped[ilay][ibar].asigal(isid);
      if(adca1==0 && 
         (adcal1*TOF2DBc::tdcbin(2)+pedv+peds)>=number(TOF2GC::SCADCMX)){
//                                      +peds to be sure in high value of A(l)(close to ovfl)
        if(nadcd==0 && nadcdl==0)TOF2JobStat::addch(chnum,16);//miss D(h/l)-something wrong
      }
//
      if(nhit>0 && (adca1>0 || adcal1>0 || nadcd>0 || nadcdl>0))bad=0;//at least one
//                                                        t+amp measurement per event
//
//-----      
    } // ---> end of channel status check 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//  ---- end of RawEvent hits loop ------->
  if(bad==0)status=0;// good TOF-event(at least one t+amp measurement )
//
// =============> STRR-calibration, if requested :
//
  if(TFREFFKEY.relogic[0]==1 && status == 0){
    TOF2JobStat::addre(16);
//
    ptr=(TOF2RawEvent*)AMSEvent::gethead()
                        ->getheadC("TOF2RawEvent",0);
    isds=0;
//                           
    while(ptr){ //  <---- loop over TOF RawEvent hits -----
      idd=ptr->getid();
      id=idd/10;// short id=LBB, where L=1,4 BB=1,10
      ilay=id/100-1;
      ibar=id%100-1;
      isid=idd%10-1;
      chnum=2*TOF2DBc::barseqn(ilay,ibar)+isid;//sequential solid numbering
      stat[isid]=ptr->getstat();
      if(stat[isid] == 0){  
        isds+=1;
//
// --> fill for STRR-calibration :
//
        nftdc=ptr->getftdc(ftdc1);
        nstdc=ptr->getstdc(stdc1);
        adca1=ptr->getadca();
        adcal1=ptr->getadcal();
        if(nstdc==4){ // require only one "4-edge" sTDC-hit
          tsr[0]=number((stdc1[3]&pbanti)*TOF2DBc::tdcbin(1));
          tsr[1]=number((stdc1[2]&pbanti)*TOF2DBc::tdcbin(1));
          tsr[2]=number((stdc1[0]&pbanti)*TOF2DBc::tdcbin(1));
          TOF2STRRcalib::fill(chnum,tsr);
          if(nftdc==2 && (adca1>0 || adcal1>0)){// 1 "2-edge" fTDC and anode>0
            ths[0]=(ftdc1[1]&pbanti)*TOF2DBc::tdcbin(0);
            ths[1]=(ftdc1[0]&pbanti)*TOF2DBc::tdcbin(0);
            fstd=ths[0]-tsr[0];//fTDC-sTDC time-shift
            TOF2STRRcalib::fill2(chnum,fstd);
          }
        }
//
      }// --- end of status check --->
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawEvent hits loop ------->
//
    status=1;// set to "bad" to avoid further(normal) reconstruction(useless if i need STRR calib)
  }//--- endof STRR check --->
}
//-----------------------------------------------------------------------
void TOF2RawCluster::build(int &ostatus){
  int16u nftdc[2]={0,0};
  int16u nstdc[2]={0,0};
  int16u nadca[2]={0,0};
  int16u nadcal[2]={0,0};
  int16u nadcd[2]={0,0};
  int16u nadcdl[2]={0,0};
  int16u ftdc1[TOF2GC::SCTHMX2*2],stdc1[TOF2GC::SCTHMX3*4];
  int16u adca1,adcal1;
  int16u adcd1[TOF2GC::PMTSMX],adcdl1[TOF2GC::PMTSMX];
  int16u ftdc2[TOF2GC::SCTHMX2*2],stdc2[TOF2GC::SCTHMX3*4];
  int16u adca2,adcal2;
  int16u adcd2[TOF2GC::PMTSMX],adcdl2[TOF2GC::PMTSMX];
  integer ilay,last,ibar,isid,isds,isd,isdsl[TOF2GC::SCLRS],hwid,tchan,crat,sfet,slnu,tdcc;
  integer i,j,k,chnum,brnum,am[2],tmi[2],itmf[2],sta,st,smty[2],ftdcfl,reject;
  integer nmpts;
  uinteger trpatt[TOF2GC::SCLRS];
  uinteger Runum(0);
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  int16u id,idd,idN,stat[2],amf[2];
  int16u mtyp(0),otyp(0);
  number ama[2],amal[2],amd[2],amdl[2],amc[2];
  geant peds[2];
  number zc,elosa,elosal,elosd,elosdl,tmf[2],time,coo,ecoo;//input to RawCluster Constr
  number aedep,dedep;
  number tm[2],tf,tff,dt,fstd,tmr[2];
  number timeD,tamp;
  number treads[2]={0.,0.};
  number treada[2]={0.,0.};
  number charg[2]={0.,0.};
  number t1,t2,t3,t4;
  geant blen,co,eco,point,brlm,pcorr,td2p,etd2p,clong[TOF2GC::SCLRS];
  geant strr[2],srof[2],strat[2][2];
  TOF2RawEvent *ptr;
  TOF2RawEvent *ptrN;
  integer nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS];
  int bad,tsfl(0),anchok,anlchok,dychok,dylchok,hlf;
// some variables for histogramming:
  geant gdt,tch,pch1[TOF2GC::SCLRS],pch2[TOF2GC::SCLRS],rrr[2];
  geant edepa[TOF2GC::SCLRS],edepd[TOF2GC::SCLRS],edepdl[TOF2GC::SCLRS];
  geant tcorr[TOF2GC::SCLRS],elosn;
  geant tuncorr[TOF2GC::SCLRS],tdiff[TOF2GC::SCLRS],td13,td24,td14;
  geant gnd,dh2l,a2d;
  number aaa,ddd;
  int dovfl,dlovfl;
  integer npmts;
  number adcdr[2][TOF2GC::PMTSMX],adcdlr[2][TOF2GC::PMTSMX];
//
  ptr=(TOF2RawEvent*)AMSEvent::gethead()
                                    ->getheadC("TOF2RawEvent",0);
  Runum=AMSEvent::gethead()->getrun();// current run number
  pbitn=TOF2GC::SCPHBP;//phase bit position
  pbanti=pbitn-1;// mask to avoid it.
  ostatus=1;// event output status(init. as bad)
  isds=0;
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
//
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,10
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    mtyp=0;
    otyp=0;
    AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(time)
//    edep=ptr->getedep();
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channel numbering for job-stat counters
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering ...
    stat[isid]=ptr->getstat();
//
    if(TOF2Brcal::scbrcal[ilay][ibar].SideOK(isid)
      && TOFBPeds::scbrped[ilay][ibar].PedAHchOK(isid)
      && TOFBPeds::scbrped[ilay][ibar].PedDHchOK(isid)
                                                     ){//<--- check hit DB(cal+ped)-status
//      hwid=TOF2RawEvent::sw2hwid(ilay,ibar,isid);//tempor(no tofid !)
//      crat=hwid/100-1;
        crat=0;
//      slnu=1;//sequential number of slot with temp. (only 1 exists) 
//       fill working arrays for given side:
      isds+=1;
      if(isid==0){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        adca1=ptr->getadca();
	if(adca1>0)nadca[isid]+=1;
        adcal1=ptr->getadcal();
	if(adcal1>0)nadcal[isid]+=1;
        nadcd[isid]=ptr->getadcd(adcd1);
        nadcdl[isid]=ptr->getadcdl(adcdl1);
//        tdcc=12;//temper. reading for anode
//        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
//        treada[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for anode chain
//        tdcc=13;//temper. reading for stretcher
//        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
//        treads[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for stretcher chain
      } 
      if(isid==1){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc2);
        nstdc[isid]=ptr->getstdc(stdc2);
        adca2=ptr->getadca();
	if(adca2>0)nadca[isid]+=1;
        adcal2=ptr->getadcal();
	if(adcal2>0)nadcal[isid]+=1;
        nadcd[isid]=ptr->getadcd(adcd2);
        nadcdl[isid]=ptr->getadcdl(adcdl2);
//        tdcc=12;//temper. reading for anode
//        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
//        treada[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for anode chain
//        tdcc=13;//temper. reading for stretcher
//        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
//        treads[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for stretcher chain
      }
      TOF2JobStat::addch(chnum,0);        // channel statistics :
      if(nftdc[isid]>0)TOF2JobStat::addch(chnum,1);
      if(nstdc[isid]>0)TOF2JobStat::addch(chnum,2);
      if(nadca[isid]>0)TOF2JobStat::addch(chnum,3);
      if(nadcal[isid]>0)TOF2JobStat::addch(chnum,18);
      if(nadcd[isid]>0)TOF2JobStat::addch(chnum,4);
      if(nadcdl[isid]>0)TOF2JobStat::addch(chnum,5);
      if(nftdc[isid]==2)TOF2JobStat::addch(chnum,6);
      if(nstdc[isid]==4)TOF2JobStat::addch(chnum,7);
      if(nftdc[isid]>=2 && nstdc[isid]>=4 && (nadca[isid]>0 || nadcal[isid]>0))TOF2JobStat::addch(chnum,8);
      if(TFREFFKEY.reprtf[2]>1){
        HF1(1120+crat,geant(treads[isid]),1.);
        HF1(1130+crat,geant(treada[isid]),1.);
      }
    }// endof DB-status check ---> 
//
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (LBB)
//------------------------------------------------------
//aaa--->
    if(idN != id){ // both sides ready, next hit is NEW_counter_hit/last_hit,
//       so process CURRENT counter data : 
//---
      if(isds==2 || isds==1){ // sides presence check
        TOF2JobStat::addbr(brnum,0);//bar entries(at least one alive(in DB) side is present )
//
// =============> start pattern recognition for given sc.bar :
//
// --------> select bar with >= 1x4-multiplicity in stdc, >= 1x2 in ftdc and A||D-adc >0 
//(first pulse from readout (last in real time) will be used if many f/stdc are present): 
//
        smty[0]=0;
        smty[1]=0;
        ftdcfl=TFREFFKEY.relogic[1];// use/not flag for f-TDC 
        if(nstdc[0]>=4  
	    && (nadca[0]>0 || nadcal[0]>0 || nadcd[0]>0 || nadcdl[0]>0)
	    && (nftdc[0] >=2 || ftdcfl==2))smty[0]=1;//
        if(nstdc[1]>=4  
	    && (nadca[1]>0 || nadcal[1]>0 || nadcd[1]>0 || nadcdl[1]>0)
	    && (nftdc[1] >=2 || ftdcfl==2))smty[1]=1;//
//----------------------
//bbb--->
        if(smty[0]==1 || smty[1]==1){ //1-side bar is accepted,if have complete measurements) 
          TOF2JobStat::addbr(brnum,1);
          isds=smty[0]+smty[1];// number of  good sides
          sta=0;
// -> add status-bits for known problems:
//          if(!(TOF2Brcal::scbrcal[ilay][ibar].SchOK(0)&&TOF2Brcal::scbrcal[ilay][ibar].SchOK(1)))
//	                                sta|=TOFGC::SCBADB3;//mark bad t-measurement on any side
//
          if(isds==1){
            sta|=TOFGC::SCBADB2;// set bit for counter with only one-side measurements
            if(smty[1]==0)sta|=TOFGC::SCBADB4;// bad_side_number bit(s1->reset_bit,s2->set_bit)
          }
          TOF2Brcal::scbrcal[ilay][ibar].gtstrat(strat);
          strr[0]=strat[0][0];// strr.for s-1
          strr[1]=strat[1][0];// .........s-2
          srof[0]=strat[0][1];// offs.for s-1
          srof[1]=strat[1][1];// .........s-2
//
//--------------> identify "corresponding"(to sTDC) hit in fast TDC :
//
// --> calculate s-TDC "start"-edge (each-side) times tm[2] (wrt lev1):
          if(smty[0]>0)tm[0]=number((stdc1[3]&pbanti)*TOF2DBc::tdcbin(1));//last(in real time) s-tdc hit
          else tm[0]=0; 
          if(smty[1]>0)tm[1]=number((stdc2[3]&pbanti)*TOF2DBc::tdcbin(1));
          else tm[1]=0;
// 
// --> histogr. for events with single history/slow-hit :
//
          if(TFREFFKEY.reprtf[2]>0){
	    if(smty[0]>0)HF1(1108,float(int16u(nftdc[0]/2)),1.);
	    if(smty[1]>0)HF1(1108,float(int16u(nftdc[1]/2)),1.);
//
            if(smty[0]>0 && nftdc[0]==2 && nstdc[0]==4){
              tff=number((ftdc1[1]&pbanti)*TOF2DBc::tdcbin(0));//2-nd hit is leading(up)-edge
              dt=tff-tm[0];
              HF1(1100,geant(dt),1.);//hist. the same hit f/s-TDC difference
              dt=(int(ftdc1[1]&pbanti)-int(ftdc1[0]&pbanti))
                                         *TOF2DBc::tdcbin(0);//f-TDC inp.pulse width
              HF1(1103,geant(dt),1.);
            }
            if(smty[1]>0 && nftdc[1]==2 && nstdc[1]==4){
              tff=number((ftdc2[1]&pbanti)*TOF2DBc::tdcbin(0));
              dt=tff-tm[1];
              HF1(1100,geant(dt),1.);
              dt=(int(ftdc2[1]&pbanti)-int(ftdc2[0]&pbanti))
                                         *TOF2DBc::tdcbin(0);//f-TDC inp.pulse width
              HF1(1103,geant(dt),1.);
            }
          }
// --> loop over f-TDC hits (up-edges) to find f-tdc hit, coresponding s-tdc hit (MATCHING):
          tmf[0]=-1.;
          tmf[1]=-1.;
          itmf[0]=0;
          itmf[1]=0;
          fstd=number(TOF2Brcal::scbrcal[ilay][ibar].gtfstrd()); // diff. in f/s same-hit delay
          if(smty[0]>0){
            for(i=0;i<nftdc[0];i+=2){ // side-1
              tf=number((ftdc1[i+1]&pbanti)*TOF2DBc::tdcbin(0));//take up-edge(2-nd) of f-TDC
              dt=tf-tm[0]-fstd;
              if(TFREFFKEY.reprtf[2]>0)HF1(1115,geant(dt),1.);
              if(fabs(dt) < TOF2Varp::tofvpar.fstdw()){
                tmf[0]=tf;
                itmf[0]=i+1;
              }
            }
          }                          
          if(smty[1]>0){
            for(i=0;i<nftdc[1];i+=2){ // side-2
              tf=number((ftdc2[i+1]&pbanti)*TOF2DBc::tdcbin(0));
              dt=tf-tm[1]-fstd;
              if(TFREFFKEY.reprtf[2]>0)HF1(1115,geant(dt),1.);
              if(fabs(dt) < TOF2Varp::tofvpar.fstdw()){
                tmf[1]=tf;
                itmf[1]=i+1;
              }
            }
          }
//---
          reject=0;
          if(ftdcfl==0){//require f/s-TDC time matching
            if((tmf[0]<0. && smty[0]==1) ||
             (tmf[1]<0. && smty[1]==1))reject=1;//NO MATCHING on any of the alive(3-meas) sides
          }
//
//-----------> do "befor"/"after" f-tdc hit presence test for each of the good(upto now) sides :
//
        if(reject==0 && ftdcfl==0){
          if(smty[0]>0){
            j=itmf[0]-2; // Side-1 "after"(real time)-check (LIFO-readout !)
            if(j >= 0){
              tf=(ftdc1[j]&pbanti)*TOF2DBc::tdcbin(0);
              if(TFREFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[0]-tf),1.);
              if((tmf[0]-tf) < TOF2Varp::tofvpar.hiscuta())reject=1;
            }
            j=itmf[0]+2; // Side-1 "befor"-check
            if(j < nftdc[0]){
              tf=(ftdc1[j]&pbanti)*TOF2DBc::tdcbin(0);
              if(TFREFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[0]),1.);
              if((tf-tmf[0]) < TOF2Varp::tofvpar.hiscutb())reject=1;
            }
          }
//
          if(smty[1]>0){
            j=itmf[1]-2; // Side-2 "after"-check 
            if(j >= 0){
              tf=(ftdc2[j]&pbanti)*TOF2DBc::tdcbin(0);
              if(TFREFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[1]-tf),1.);
              if((tmf[1]-tf) < TOF2Varp::tofvpar.hiscuta())reject=1;
            }
            j=itmf[1]+2; // Side-2 "befor"-check
            if(j < nftdc[1]){
              tf=(ftdc2[j]&pbanti)*TOF2DBc::tdcbin(0);
              if(TFREFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[1]),1.);
              if((tf-tmf[1]) < TOF2Varp::tofvpar.hiscutb())reject=1;
            }
          }
        }
//
//===========>>> set time-history status of sc.bar :
//
            if(reject==1)sta|=TOFGC::SCBADB1;// set bit "time-history/match problem" on any alive side
            if(reject==0)TOF2JobStat::addbr(brnum,2);// statistics on "good time-history"
            if(reject==0 && isds==2)TOF2JobStat::addbr(brnum,3);// statistics on 2-sided(2x3meas)
//                                                               "good time-history"
//
//===========>>> calculate times/Edeps of sc.bar :
//
//---> Anode-Time/Charge reconstruction:
//
            chnum=ilay*TOF2GC::SCMXBR*2+ibar*2;//s1-channel number 
            tm[0]=0;
            ama[0]=0.;
            amal[0]=0.;
            tmr[0]=0.;
            TOFBPeds::scbrped[ilay][ibar].getpedal(peds);// get anode(l)-adc peds(s1/2)
            if(smty[0]==1){// good S1(3-measurement) side, but matching/history may not be good
              t4=(stdc1[0]&pbanti)*TOF2DBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc1[2]&pbanti)*TOF2DBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc1[3]&pbanti)*TOF2DBc::tdcbin(1);// 1-st edge of str-info
              tm[0]=((t2-t4)-srof[0])/strr[0];// s-1 time (ns,A-noncorr)
              tmf[0]=tm[0];
              tmr[0]=t1-t2;
	      if(nadca[0]>0){//Anode(h),s1
                ama[0]=number(adca1*TOF2DBc::tdcbin(2));//DAQ-units-->ADC(float)(anodeH s1)
                if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1104,geant(ama[0]),1.);
//              don't check here ovfl because Ah-ovfl is not read out by ADC-chip
	      }
	      if(nadcal[0]>0){//Anode(l),s1
                amal[0]=number(adcal1*TOF2DBc::tdcbin(2));//DAQ-units-->ADC(float)(anodeL s1)
                if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1109,geant(amal[0]),1.);
	        if((amal[0]+peds[0])>=number(TOF2GC::SCADCMX-1))amal[0]=0.;//mark overflow by "0"
                if(amal[0]==0.)TOF2JobStat::addch(chnum,19);//counts Al-cnan. overflows
		if(nadca[0]==0)TOF2JobStat::addch(chnum,9);//counts Ah-cnan. overflows
	      }
            }
//
            tm[1]=0;
            ama[1]=0.;
            amal[1]=0.;
            tmr[1]=0.;
            if(smty[1]==1){// good S2(3-measurement) side, but matching/hist may not be good
              t4=(stdc2[0]&pbanti)*TOF2DBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc2[2]&pbanti)*TOF2DBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc2[3]&pbanti)*TOF2DBc::tdcbin(1);// 1-st edge of str-info
              tm[1]=((t2-t4)-srof[1])/strr[1];// s-2 time (ns,A-noncorr)
              tmf[1]=tm[1];
              tmr[1]=t1-t2;
	      if(nadca[1]>0){//Anode(h),s2
                ama[1]=number(adca2*TOF2DBc::tdcbin(2));//DAQ-units-->ADC(float)(anodeH s2)
//              don't check here ovfl because Ah-ovfl is not read out by ADC-chip
	      }
	      if(nadcal[1]>0){//Anode(l),s2
                amal[1]=number(adcal2*TOF2DBc::tdcbin(2));//DAQ-units-->ADC(float)(anodeL s2)
	        if((amal[1]+peds[1])>=number(TOF2GC::SCADCMX-1))amal[1]=0.;//mark overflow by "0"
                if(amal[1]==0.)TOF2JobStat::addch(chnum+1,19);//counts Al-cnan. overflows
		if(nadca[1]==0)TOF2JobStat::addch(chnum+1,9);//counts Ah-cnan. overflows
	      }
            }
            if(smty[0]==0){ //set "=" s1&s2 times and ampl. for 1-sided case:
              tmf[0]=tmf[1];
              ama[0]=ama[1];
              amal[0]=amal[1];
            }
            if(smty[1]==0){
              tmf[1]=tmf[0];
              ama[1]=ama[0];
              amal[1]=amal[0];
            }
	    anchok=1;
	    if(ama[0]==0.|| ama[1]==0.){//set s1&s2->0(no Ah info) if any side-overflow
	      ama[0]=0.;
	      ama[1]=0.;
	      anchok=0;
	    }
	    anlchok=1;
	    if(amal[0]==0.|| amal[1]==0.){//set s1&s2->0(no Al info) if any side-overflow
	      amal[0]=0.;
	      amal[1]=0.;
	      anlchok=0;
	    }
//
//---> Dynode-charge reconstruction:
//
            npmts=TOF2Brcal::scbrcal[ilay][ibar].getnpm();
	    for(int is=0;is<2;is++){
	      for(int ip=0;ip<TOF2GC::PMTSMX;ip++){
		adcdr[is][ip]=0;
		adcdlr[is][ip]=0;
	      }
	    }
//
            amd[0]=0.;
            amdl[0]=0.;
	    dovfl=0;
	    dlovfl=0;
            if(smty[0]>0){// <===S1
	      if(nadcd[0]>0 || nadcdl[0]>0){//combine D(h/l) pm-signals into S1 joined Dh-signal
	        for(int ip=0;ip<npmts;ip++){//<--s1 pm-loop
		  if(adcd1[ip]>0){//use only Dh(pm) if >0
		    dh2l=TOF2Brcal::scbrcal[ilay][ibar].getdh2l(0,ip);
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(0,ip);
		    aaa=number(adcd1[ip]*TOF2DBc::tdcbin(2));//Daq->Dh(pm)
		    amd[0]+=(aaa/gnd);//Daq->Dh(equilised)
                    if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1105,geant(aaa),1.);
		  }
		  else if(adcdl1[ip]>0){//try use Dl(pm) since Dh(pm) is missing(ovfl)
                    TOFBPeds::scbrped[ilay][ibar].getpeddl(ip,peds);//Dl peds(s1/2)
		    dh2l=TOF2Brcal::scbrcal[ilay][ibar].getdh2l(0,ip);
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(0,ip);
		    ddd=number(adcdl1[ip]*TOF2DBc::tdcbin(2));//Daq->Dl(pm)
                    if(TFREFFKEY.reprtf[2]>0 && id==104)HF1(1106,geant(ddd),1.);
		    amd[0]+=(ddd*dh2l/gnd);//Dl->Dh->Dh(eq)
	            if((ddd+peds[0])>=number(TOF2GC::SCADCMX-1))dlovfl=1;//Dl s1-overflow
		    dovfl=1;//Dh s1-overflow
		  }
		}//-->endof pm-loop
		if(dovfl>0)TOF2JobStat::addch(chnum,10);//counts s1 Dh-cnan. overflows
		if(dlovfl>0)TOF2JobStat::addch(chnum,11);//counts s1 Dl-cnan. overflows
	      }
	    }
//
            amd[1]=0.;
            amdl[1]=0.;
	    dovfl=0;
	    dlovfl=0;
            if(smty[1]>0){// <===S2
	      if(nadcd[1]>0 || nadcdl[1]>0){//combine D(h/l) pm-signals into S2 joined Dh-signal
	        for(int ip=0;ip<npmts;ip++){//<--s2 pm-loop
		  if(adcd2[ip]>0){//use only Dh(pm) if >0
		    dh2l=TOF2Brcal::scbrcal[ilay][ibar].getdh2l(1,ip);
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(1,ip);
		    aaa=number(adcd2[ip]*TOF2DBc::tdcbin(2));//Daq->Dh(pm)
		    amd[1]+=(aaa/gnd);//Daq->Dh(equilised)
		  }
		  else if(adcdl2[ip]>0){//try use Dl(pm) since Dh(pm) is missing(ovfl)
                    TOFBPeds::scbrped[ilay][ibar].getpeddl(ip,peds);//Dl peds(s1/2)
		    dh2l=TOF2Brcal::scbrcal[ilay][ibar].getdh2l(1,ip);
		    gnd=TOF2Brcal::scbrcal[ilay][ibar].getgnd(1,ip);
		    ddd=number(adcdl2[ip]*TOF2DBc::tdcbin(2));//Daq->Dl(pm)
		    amd[1]+=(ddd*dh2l/gnd);//Dl->Dh->Dh(eq)
	            if((ddd+peds[1])>=number(TOF2GC::SCADCMX-1))dlovfl=1;//Dl s2-overflow
		    dovfl=1;//Dh s2-overflow
		  }
		}//-->endof pm-loop
		if(dovfl>0)TOF2JobStat::addch(chnum+1,10);//counts s2 Dh-cnan. overflows
		if(dlovfl>0)TOF2JobStat::addch(chnum+1,11);//counts s2 Dl-cnan. overflows
	      }
            }
//
            if(smty[0]==0)amd[0]=amd[1];//set "=" s1&s2 D-ampl. for 1-sided case:
            if(smty[1]==0)amd[1]=amd[0];
	    dychok=1;
	    if(amd[0]==0.|| amd[1]==0.){//missing Dynode info
	      amd[0]=0.;
	      amd[1]=0.;
	      dychok=0;
	    }
//----
            zc=TOF2DBc::getzsc(ilay,ibar);
            blen=TOF2DBc::brlen(ilay,ibar);
//ccc------->
            if(TFREFFKEY.relogic[0] != 1
	      && (anchok+anlchok+dychok)>0){//<--require any signal>0(nonovfl) and NOT SRS-calib
	      if(anchok){
	        amc[0]=ama[0];
	        amc[1]=ama[1];
		hlf=0;
	      }
	      else if(anlchok){
	        amc[0]=amal[0];
	        amc[1]=amal[1];
		hlf=1;
	      }
	      else if(dychok){
	        amc[0]=amd[0];
	        amc[1]=amd[1];
		hlf=2;
	      }
//
//--> Calc. longit. coord/err and position correction to charge-signal :
//
              TOF2Brcal::scbrcal[ilay][ibar].tmd2p(tmf,amc,hlf,co,eco);// get A-corrected Local(!).coo/err
	      dt=-0.5*(tmf[0]-tmf[1]);
//   cout<<"l/b="<<ilay<<" "<<ibar<<" Td="<<dt<<" co="<<co<<endl; 
              brlm=0.5*TOF2DBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means: "coord. is more than counter half length"
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(co);
              }
              if(isds==1)pcorr=1.;// no position correction for one-sided counters
//
//--> Find counter-time, corrected for slewing(according to ADC-signal) :
//
              time=TOF2Brcal::scbrcal[ilay][ibar].tm2t(tmf,amc,hlf); // time with corrections
//
//--> Find Eloss from Anode(h)-adc(if present) :
//
              elosa=0.;
              if(anchok)elosa=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(0,ama)
                                             /pcorr; //A(h) Edep(mev)(normalized to center)
//
//--> Find Eloss from Anode(l)-adc(if present) :
//
              elosal=0.;
              if(anlchok)elosal=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(1,amal)
                                             /pcorr; //A(l) Edep(mev)(normalized to center)
//--> Combined Anode edep:
              if(elosa>0)aedep=elosa;
	      else aedep=elosal;
//
//--> Find Eloss from combined Dynode(h+l)-adc :
//
              elosd=0.;
              if(dychok)elosd=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(2,amd)
                                            /pcorr;//D(h+l) Edep(mev)(normalized to center)
              dedep=elosd;
//---> store raw(but converted from daq-format) ADCs in RawCl-obj for calibr.purpose:
	      for(int ip=0;ip<npmts;ip++){
		if(nadcd[0]>0)adcdr[0][ip]=number(adcd1[ip]*TOF2DBc::tdcbin(2));
		if(nadcdl[0]>0)adcdlr[0][ip]=number(adcdl1[ip]*TOF2DBc::tdcbin(2));
		if(nadcd[1]>0)adcdr[1][ip]=number(adcd2[ip]*TOF2DBc::tdcbin(2));
		if(nadcdl[1]>0)adcdlr[1][ip]=number(adcdl2[ip]*TOF2DBc::tdcbin(2));
              }
//
//-->
              nbrl[ilay]+=1;
              isdsl[ilay]=isds;
              edepa[ilay]=aedep;
              edepd[ilay]=dedep;
              tcorr[ilay]=time;
              tuncorr[ilay]=0.5*(tmf[0]+tmf[1]);// layer A-non.cor. time
              tdiff[ilay]=0.5*(tmf[0]-tmf[1]);// layer A-non.cor. time-diff.(ns) 
              pch1[ilay]=geant(charg[0]);
              pch2[ilay]=geant(charg[1]);
              brnl[ilay]=ibar;
              clong[ilay]=co;
              st=0;
              coo=co;// Local coord.!!!
              ecoo=eco;
              if(isds==1)ecoo=blen/sqrt(12.);//for single-sided counters
              if(AMSEvent::gethead()->addnext(AMSID("TOF2RawCluster",0)
              ,new TOF2RawCluster(sta,ilay+1,ibar+1,zc,ama,amal,amd,adcdr,adcdlr,
                              aedep,dedep,tm,time,coo,ecoo)))st=1;;//store value
//	      cout<<"StoreNewRawCl(il/ib)="<<ilay+1<<" "<<ibar+1<<" time="<<time<<endl;
//	      cout<<"      sta/Aedep/coo="<<sta<<" "<<aedep<<" "<<coo<<endl;
            } // ccc------> end of bypass check
//-----------
        } // bbb---> end of "side measurement-multiplicity" check
//
      } // aaa---> end of "new counter" check(current counter both side ready)
      isds=0;// clear side-counters/temperatures befor next bar processing
      nftdc[0]=0;
      nftdc[1]=0;
      nstdc[0]=0;
      nstdc[1]=0;
      nadca[0]=0;
      nadca[1]=0;
      nadcal[0]=0;
      nadcal[1]=0;
      nadcd[0]=0;
      nadcd[1]=0;
      nadcdl[0]=0;
      nadcdl[1]=0;
      treads[0]=0.;
      treads[1]=0.;
      treada[0]=0.;
      treada[1]=0.;
//
    } // ---> end of "next COUNTER" or "last hit" check
//------------------------------------------------------
    ptr=ptr->next();// take next RawEvent hit
//
  } //                ---- end of RawEvent hits loop ------->
//
//
// ---> now check min. multiplicity and make some histograms :
//
  int nbrch[TOF2GC::SCLRS],conf(-1),isum(0);
  int il,ib,ix,iy;
  geant x[2],y[2],zx[2],zy[2],zcb[TOF2GC::SCLRS],tgx,tgy,cosi;
  number trlen13,trlen24;
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]>0)nbrch[i]=1;
  }
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  HF1(1110,geant(isum),1.);// tot.number of layers
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){

    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]>0)HF1(1111,geant(i+1),1.);// layer appear. freq.
  }
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1112,geant(conf),1.);
  }
//
  if((nbrch[0]+nbrch[1]) + (nbrch[2]+nbrch[3])>=1)ostatus=0; // good event
// 
// ---> multtipl. checks for single bar layers:
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1113,geant(conf),1.);
  }
//
  if(ostatus!=0)return;//remove bad events
//
  if(conf != 5)return;//use only 1bar/layer,4-layer events for next processing
//
//--------------> try to recover 1-sided counters
//
  integer status;
  number crc;
  geant cry[TOF2GC::SCLRS];
  TOF2RawCluster *ptrc;
  ptrc=(TOF2RawCluster*)AMSEvent::gethead()->
                                     getheadC("TOF2RawCluster",0);
//
// --> find track-counter crossing points(longit) using counter transv.coord :
//
  ix=0;
  iy=0;
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    zcb[il]=TOF2DBc::getzsc(il,ib);
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      x[ix]=TOF2DBc::gettsc(il,ib);
      zx[ix]=zcb[il];
      ix+=1;
    }
    else{                    // rotated (Y-meas) planes
      y[iy]=TOF2DBc::gettsc(il,ib);
      zy[iy]=zcb[il];
      iy+=1;
    }
  }
  tgx=(x[0]-x[1])/(zx[0]-zx[1]);// track slopes in 2 projections
  tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      crc=y[1]+tgy*(zcb[il]-zy[1]);// long. crossing coord. in abs. ref.syst.
      cry[il]=crc;//x-crossing in local syst. 
    }
    else{                     // rotated (Y-meas) planes
      crc=x[1]+tgx*(zcb[il]-zx[1]);// long. crossing coord. in abs. ref.syst.
      cry[il]=-crc;//y-crossing in local syst. 
    }
  }
//
// ----------> recover missing sides
//
  number stin[2],stout[2],timin,timout,edin,edout,clin,clout;//tempor for test
  integer isdb(1),isdg;// bad/good side
  number ddt;
  if(TFREFFKEY.relogic[3]){// do recovering of missing side
    while(ptrc){ // loop over counters(raw clusters)
      il=ptrc->getntof()-1;
      ib=ptrc->getplane()-1;
      status=ptrc->getstatus();
      if((status&TOFGC::SCBADB2)>0){ // 1-sided counter found -> try to recover it
        ptrc->recovers(cry[il]);// recovering
        status=ptrc->getstatus();//new status
        if((status & TOFGC::SCBADB5)>0){
          isdsl[il]+=1;//add recovered side
          tcorr[il]=ptrc->gettime();
        }
      }
      ptrc=ptrc->next();
    }
  }
//--------------
// 
// ---> multtipl. checks for single 2-sided bar layers:
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1 && isdsl[i]==2)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<TOF2GC::SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<TOF2GC::SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1114,geant(conf),1.);
  }
//
// -> make hist. only for 4layer x 1bar(2-sided) events:
//
  if(conf != 5)return;
//  
  cosi=sqrt(1.+tgx*tgx+tgy*tgy);// this is 1/cos(theta) !!!
  trlen13=(zcb[0]-zcb[2])*cosi;//1->3
  trlen24=(zcb[1]-zcb[3])*cosi;//2->4
  td13=(tcorr[0]-tcorr[2]);
  td24=(tcorr[1]-tcorr[3]);
  td14=tuncorr[0]-tuncorr[3];
//
  geant zpl1,zpl2,trlnor;
  zpl1=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l1-middl
  zpl2=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(2)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l2-middl
  trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
//
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
      HF1(1532,td13*trlnor/trlen13,1.);//ToF for L0->L2, normalized to trlnor
      HF1(1534,td24*trlnor/trlen24,1.);//ToF for L1->L3, ....................
      gdt=(td13/trlen13-td24/trlen24)*trlnor;// normalized to fix(~127cm) dist. for AMS02
//                                                           (130cm for AMS01)
      HF1(1544,gdt,1.);
      if(TFREFFKEY.reprtf[2]>1){ 
        for(il=0;il<TOF2GC::SCLRS;il++){// fill de/dx hist. for each bar
          ib=brnl[il];
          brnum=il*TOF2GC::SCMXBR+ib;//sequential bar numbering
          elosn=edepa[il]/cosi;//normalized  to norm.inc.
          HF1(1140+brnum,elosn,1.);
        }
        HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1527,edepa[2],1.); //layer=3 Anode-reconstructed Edep
        HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
        HF1(1529,edepd[2],1.); //layer=3 Dinode-reconstructed Edep
        if(AMSJob::gethead()->isSimulation()){
          geant tch;
          charg[0]=pch1[0];
          charg[1]=pch2[0];
          tch=charg[0]+charg[1];
          HF1(1071,tch,1.);
          HF1(1072,tch,1.);
        }
      }
  }
}
//-----------------------------------------------------------------------
void TOF2RawCluster::recovers(number x){ // function to recover missing side
  geant gn[2],gnd[2],csl,sl[2],dt0,upr[5],vel,xerr,sqr,hclen,co,eco,pcorr;
  number q[2],tm[2],tcor;
  number tot[2],adc[2];
  int il,ib,hlf;
  int isg(1),isb(0);
//
  if(_status&TOFGC::SCBADB4){
    isg=0;
    isb=1;
  }
  il=_ntof-1;
  ib=_plane-1;
  TOF2Brcal::scbrcal[il][ib].getgaina(gn);
  TOF2Brcal::scbrcal[il][ib].getupar(upr);
  TOF2Brcal::scbrcal[il][ib].getd2p(vel,xerr);
  TOF2Brcal::scbrcal[il][ib].getslops(sl);
  csl=TOF2Brcal::scbrcal[il][ib].getslope();
  dt0=TOF2Brcal::scbrcal[il][ib].gettdif();
  hclen=0.5*TOF2DBc::brlen(il,ib);
  if(fabs(x) > (hclen+5.)){// out of range by more than 5cm
    pcorr=TOF2Brcal::scbrcal[il][ib].poscor(0.);// no correction (==1)
  }
  else{
    pcorr=TOF2Brcal::scbrcal[il][ib].poscor(geant(x));
  }
// ---> recover Anode-signal for bad side :
  if(_edepa>0.){
    if(_adca[isg]>0){
      tot[isg]=_adca[isg];
      hlf=0;
    }
    else{
      tot[isg]=_adcal[isg];
      hlf=1;
    }
    TOF2Brcal::scbrcal[il][ib].q2a2q(1,isg,hlf,tot[isg],q[isg]);//ADC->Q
    q[isg]/=gn[isg];// Q->Qref
    sqr=upr[0]*(exp(-(hclen+x)/upr[3])+upr[2]*exp(-(hclen+x)/upr[4]))/
      upr[1]/(exp(-(hclen-x)/upr[3])+upr[2]*exp(-(hclen-x)/upr[4]));//Qr1/Qr2
    if(isb==1)sqr=1./sqr;
    q[isb]=q[isg]*sqr;
    q[isb]*=gn[isb];//Qref->Q
    TOF2Brcal::scbrcal[il][ib].q2a2q(0,isb,hlf,tot[isb],q[isb]);//Q->ADC
    if(hlf==0)_adca[isb]=tot[isb]; // recovered ADC for bad side
    else _adcal[isb]=tot[isb];
    _edepa=TOF2Brcal::scbrcal[il][ib].adc2mip(hlf,tot)/pcorr; //recowered anode-Eloss
    tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
    adc[0]=tot[0];
    adc[1]=tot[1];
  }
//
// ---> recover Dynode(pm/h/l-combined)-signal for bad side :
  if(_edepd>0 && _adcd[isg]>0){
    tot[isg]=_adcd[isg];
    hlf=2;
    TOF2Brcal::scbrcal[il][ib].q2a2q(1,isg,hlf,tot[isg],q[isg]);//ADC(isg)->Q(pmt-comb,equilized)
    q[isb]=q[isg]*sqr;//s1/s2 ratio correction(vs imp.point)
    TOF2Brcal::scbrcal[il][ib].q2a2q(0,isb,hlf,tot[isb],q[isb]);//Q->ADC(isb)
    _adcd[isb]=tot[isb]; // recovered ADC for bad side
    _edepd=TOF2Brcal::scbrcal[il][ib].adc2mip(hlf,tot)/pcorr; //recowered dinode-Eloss
    if(_edepa==0.){//slew.cor. from dynode-chan., if anode-chan. is missing
      tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
      adc[0]=tot[0];
      adc[1]=tot[1];
    }
  }
//
// ---> recover time :
  tm[isg]=_sdtm[isg];
  tm[isb]=tm[isg]-(1-2*isg)*(2.*dt0-2.*x/vel-tcor);
  _sdtm[isb]=tm[isb]; // recowered side time(raw)
  _time=TOF2Brcal::scbrcal[il][ib].tm2t(tm,tot,hlf); // recovered time(with corr)
  TOF2Brcal::scbrcal[il][ib].tmd2p(tm,adc,hlf,co,eco);// get A-corrected Local(!).coord/err
  _timeD=co;// recovered loc. coordinate (chould be = x)
  _etimeD=3.5;// tempor err.on longit.coord(cm) for 1-sided,recovered counters
//
  _status|=TOFGC::SCBADB5;//add bit for recovered counters
}
//-----------------------------------------------------------------------
void TOF2RawCluster::_writeEl(){

  if(TOF2RawCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// Fill the ntuple
    TOFRawClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tofraw();
    if (TN->Ntofraw>=MAXTOFRAW) return;
    TN->Status[TN->Ntofraw]=_status;
    TN->Layer[TN->Ntofraw]=_ntof;
    TN->Bar[TN->Ntofraw]=_plane;
    for(int i=0;i<2;i++){
      TN->adca[TN->Ntofraw][i]=_adca[i];
      TN->adcal[TN->Ntofraw][i]=_adcal[i];
      TN->adcd[TN->Ntofraw][i]=_adcd[i];
      for(int ip=0;ip<TOF2GC::PMTSMX;ip++)TN->adcdr[TN->Ntofraw][i][ip]=_adcdr[i][ip];
      for(int ip=0;ip<TOF2GC::PMTSMX;ip++)TN->adcdlr[TN->Ntofraw][i][ip]=_adcdlr[i][ip];
      TN->sdtm[TN->Ntofraw][i]=_sdtm[i];
    }
    TN->edepa[TN->Ntofraw]=_edepa;
    TN->edepd[TN->Ntofraw]=_edepd;
    TN->time[TN->Ntofraw]=_time;
    TN->cool[TN->Ntofraw]=_timeD;
    TN->Ntofraw++;
  }
}
//------

void TOF2RawCluster::_copyEl(){
}

//------
void TOF2RawCluster::_printEl(ostream & stream){
  stream <<"TOF2RawCluster "<<_ntof<<" "<<_edepa<<" "<<_plane<<endl;
}



void AMSTOFCluster::_writeEl(){

  // p2memb for Root
  if(AMSTOFCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  TOFClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tof();

  if (TN->Ntof>=MAXTOF) return;

// Fill the ntuple
    TN->Status[TN->Ntof]=_status;
    TN->Layer[TN->Ntof]=_ntof;
    TN->Bar[TN->Ntof]=_plane;
    TN->Nmemb[TN->Ntof]=_nmemb;
    TN->Edep[TN->Ntof]=_edep;
    TN->Edepd[TN->Ntof]=_edepd;
    TN->Time[TN->Ntof]=_time;
    TN->ErrTime[TN->Ntof]=_etime;
    int i;
    for(i=0;i<3;i++)TN->Coo[TN->Ntof][i]=_Coo[i];
    for(i=0;i<3;i++)TN->ErrorCoo[TN->Ntof][i]=_ErrorCoo[i];
    if(TOF2RawCluster::Out(IOPA.WriteAll%10==1)){//WriteAll
     for(int i=0;i<3;i++)TN->P2memb[TN->Ntof][i]=0;
      for(int i=0;i<_nmemb;i++) {
        TN->P2memb[TN->Ntof][i]=_mptr[i]->getpos();
      }
    }
    else{// Used
      integer mpos;
      TOF2RawCluster *ptr;
      for(int i=0;i<_nmemb;i++) {
        mpos=_mptr[i]->getpos();
	ptr=(TOF2RawCluster*)AMSEvent::gethead()->getheadC("TOF2RawCluster",0);
        for(int j=0;j<mpos;j++){
          if(ptr && ptr->checkstatus(AMSDBc::USED))TN->P2memb[TN->Ntof][i]++;
          ptr=ptr->next();
        }
      }
    }
    TN->Ntof++;
  }
}



void AMSTOFCluster::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 TofClusterR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TofCluster(_vpos);
      for(int i=0;i<_nmemb;i++)(ptr.fTofRawCluster).push_back(_mptr[i]->GetClonePointer());
#endif
}


void AMSTOFCluster::_printEl(ostream & stream){
  stream <<"AMSTOFCluster "<<_status<<" "<<_ntof<<" "<<_plane<<" "<<_edep<<" "
//  <<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
  <<_edepd<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
}


 
void AMSTOFCluster::print(){
for(int i=0;i<4;i++){
AMSContainer *p =AMSEvent::gethead()->getC("AMSTOFCluster",i);
 if(p)p->printC(cout);
}
}

//===========================================================================
//
void AMSTOFCluster::init(){
  _planes=TOF2DBc::getnplns();
  for(int i=0;i<_planes;i++)_padspl[i]=TOF2DBc::getbppl(i);
}
//---------------------------------------------------------------------------
void AMSTOFCluster::build2(int &stat){
//(search for true dublets(overlaping counter events))
//
  TOF2RawCluster *ptr; 
  TOF2RawCluster *ptrn; 
  static TOF2RawCluster * xptr[TOF2GC::SCMXBR];
  static number eplane[TOF2GC::SCMXBR],edplane[TOF2GC::SCMXBR];
  static AMSlink * membp[2];
  int nclust,cllay[TOF2GC::SCLRS],nmemb;
  AMSPoint coo,ecoo;
  int i,j,il,ib,bmax;
  integer ntof,barn,status,statusn,plrot,ok;
  geant barl,barw,cl,cle,ct,cte,cln,clne,ctn,ctne,cz,czn,clm,clnm;
  geant edep,edepn,edepa,edepd,edepdl,edepdn,edass;
  geant time,etime,timen,etimen,speedl,err;
//-----
  stat=1; // bad
  for(i=0;i<TOF2GC::SCLRS;i++)cllay[i]=0;
//
  for(il=0;il<TOF2GC::SCLRS;il++){// <-------- TOF layers loop -----
    ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                                     getheadC("TOF2RawCluster",0);
    VZERO(eplane,(TOF2GC::SCMXBR)*sizeof(number)/4);
    VZERO(edplane,(TOF2GC::SCMXBR)*sizeof(number)/4);
    VZERO(xptr,(TOF2GC::SCMXBR)*sizeof(TOF2RawCluster*)/4);
//
    while (ptr){// scan to put all fired bars of layer "il" in buffer
      if(ptr->getntof()==il+1){
        ib=ptr->getplane();
#ifdef __AMSDEBUG__
        assert(ib>0 && ib <= TOF2GC::SCMXBR);
#endif
        edepa=ptr->getedepa();
        edepd=ptr->getedepd();
//    select between anode and dynode measurements:
	if(edepa>0.)edep=edepa;// no overflow of anode --> use it
	else edep=edepd; //  --> use Dynode otherwise
//
        eplane[ib-1]=edep;//store full range Edep(An/Dyn-combined)
        if(edepd>0)edplane[ib-1]=edepd;//store separately dynode(h/l-combined) Edep
        xptr[ib-1]=ptr;
      }
      ptr=ptr->next();
    }// ---> end of scan
//------
  nclust=0;
  bmax=TOF2DBc::getbppl(il);//bars per given plane
  for (ib=0;ib<bmax;ib++){// <---- loop over buffer content (clust. search)
    if(eplane[ib]> TFREFFKEY.Thr1){//<--- thresh. check
      ok=1;
      nmemb=0;
      ptr=xptr[ib];
      ntof=il+1;
      barn=ib+1;
      barl=TOF2DBc::brlen(il,ib);// bar length
      barw=TOF2DBc::plnstr(5);//bar width
      TOF2Brcal::scbrcal[il][ib].getd2p(speedl,err);//get light speed
      plrot=TOF2DBc::plrotm(il);     // =0/1-unrotated/rotated TOF-plane
      cz=ptr->getz();          //bar(raw clust) Z-coord.
      cl=ptr->gettimeD();// get yloc/err for ib bar
      cle=ptr->getetimeD();
      clm=0.5*barl;// limit on max. long. coord.
      ct=TOF2DBc::gettsc(il,ib);   //transv.pos. of  bar
      cte=barw/sqrt(12.);
      status=ptr->getstatus();//may be !=0(bad history/t_meas or single-sided)
      edep=eplane[ib];
      edepd=edplane[ib];
      if(fabs(cl) > clm+2*cle){//bad l-coord.measurement(out of bar size)
        status|=AMSDBc::BAD; // bad coord. means also bad for t-measurement !!!
        cle=barl/sqrt(12.);
        if(cl>0.)cl=clm;//at bar edge
        else cl=-clm;
	ok=0;//means bar with suspicious time measurement 
      }
      membp[nmemb]=ptr;//store ib cluster pointer
      nmemb+=1;
      time=ptr->gettime();// time from ib bar(ns) 
      etime=TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(0.15ns,later put in TOFBrcal needed data!)
      if((status&TOFGC::SCBADB2)>0){//1-sided
        if(status & TOFGC::SCBADB5){ // recovered 1-sided counter
          etime=2.05*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(still no Tracker info)
        }
        else{
          etime=(1.e-9)*barl/speedl/sqrt(12.);//(sec !!!)for single-sided counters
        }
	ok=0;//means bar with time meas.problems 
      }
      if((status&TOFGC::SCBADB3)>0)ok=0;//bar "ib" is bad for t-meas. according to DB
//
      if(ok==1 && ib+1<bmax && eplane[ib+1]> TFREFFKEY.Thr1){// try next(adjacent) bar if prev.OK
        ptrn=xptr[ib+1];
        barl=TOF2DBc::brlen(il,ib+1);// peak bar length
        czn=ptrn->getz();          //next bar(raw clust) Z-coord.
        cln=ptrn->gettimeD();// get yloc/err for "peak" bar
        clne=ptrn->getetimeD();
        clnm=0.5*barl;// limit on max. long. coord.
        ctn=TOF2DBc::gettsc(il,ib+1);   //transv.pos. of  bar
        ctne=barw/sqrt(12.);
        statusn=ptrn->getstatus();//may be !=0(bad history/t_meas or single-sided)
        edepn=eplane[ib+1];
        edepdn=edplane[ib+1];
        if(fabs(cln) > clnm+2*clne){//bad l-coord.measurement(out of bar size)
          clne=barl/sqrt(12.);
          if(cln>0.)cln=clnm;//at bar edge
          else cln=-clnm;
	  ok=0;//means bar with suspicious time measurement 
        }
	if(ok==1 && (statusn&TOFGC::SCBADB2)==0
                 && (statusn&TOFGC::SCBADB3)==0	
	                                       ){//<--- next bar good for gluing ?
          timen=ptrn->gettime();// time from ib+1 bar(ns) 
          etimen=TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(0.15ns,later put in TOFBrcal needed data!)
	  if(TFREFFKEY.reprtf[2]>0){
	    if(fabs(time-timen)<3*etime*sqrt(2.))HF1(1549,cl-cln,1.);
	    HF1(1550,time-timen,1.);
	  }
	  if(fabs(time-timen)<3*etime*sqrt(2.)
	             && fabs(cl-cln)<3*clne*sqrt(2.)){//t+coo match -> create cluster(glue "next")
	    etime=etime/sqrt(2.);//recalc. parameters using glued bar
	    time=0.5*(time+timen);
	    cle=cle/sqrt(2.);
	    cl=0.5*(cl+cln);
	    cte=(TOF2DBc::plnstr(3)+TOF2DBc::plnstr(4)
	                           +TOF2DBc::plnstr(6))/sqrt(12.);//max.estim(overlap+v.gap+thickn)
	    ct=0.5*(ct+ctn);
	    cz=0.5*(cz+czn);
	    if(edep<edepn)barn=ib+2;//use bar-number for cluster from highest edep bar 
	    edass=(edep-edepn)/(edep+edepn);//edep,edepn>thresh>0 
	    if(TFREFFKEY.reprtf[2]>0)HF1(1548,edass,1.);
            if(fabs(edass)<TOF2Varp::tofvpar.eclass()){//tempor(need real data to clarify algor.)
	      edep=0.5*(edep+edepn);
	      if(edepd>0 && edepdn>0)edepd=0.5*(edepd+edepdn);
	      else{
	        if(edepdn>0)edepd=edepdn;
	      }
	    }
	    else{
	      if(edepn>edep){
	        edep=edepn;//use highest when big assimetry in edep's
		edepd=edepdn;
	      }
	    }
            membp[nmemb]=ptrn;//store glued cluster pointer
            nmemb+=1;
            if((statusn&TOFGC::SCBADB1)!=0)status|=TOFGC::SCBADB1;//bad hist OR-ed for members 
	    ib+=1;//to skip already glued "next" bar in further buffer scan 
	  }//---> endof time/coo-match check
	}//endof check for  "next" bar gluing
      }//---> endof next bar check
//
      coo[2]=cz;
      ecoo[2]=TOF2DBc::plnstr(6)/sqrt(12.);//bar thickness/...   
      if(plrot==0){ // non-rotated plane
        coo[0]=ct;//clust. X-coord.
        ecoo[0]=cte;
        coo[1]=cl;//clust. abs.Y-coord.(yabs=yloc and neglect counter's long.shift)
        ecoo[1]=cle;
      }
      else{ // rotated plane
        coo[0]=cl;//clust. abs.X-coord.(xabs=yloc and neglect counter's long.shift)
        ecoo[0]=cle;
        coo[1]=ct;
        ecoo[1]=cte;
      }
//
      if(TFREFFKEY.reprtf[2]>0){// some histograms for clusters(1/2 members):
        if(il==0){
          HF1(1535,edep,1.);//Cluster energy distr.,L=1
          HF1(1537,edep,1.);
          HF1(1541,geant(coo[0]),1.);
          HF1(1542,geant(coo[1]),1.);
        }
        if(il==1){
          HF1(1539,edep,1.);//Cluster energy distr.,L=2
        }
        if(il==2){
          HF1(1536,edep,1.);//Cluster energy distr.,L=3
          HF1(1538,edep,1.);
        }
        if(il==3){
          HF1(1540,edep,1.);//Cluster energy distr.,L=4
        }
      }
//
      time=-time*1.e-9;// ( ns -> sec ,"-" for V.Shoutko fit)
      etime*=1.e-9;// ( ns -> sec for V.Shoutko fit)
      for(j=0;j<nmemb;j++)membp[j]->setstatus(AMSDBc::USED);// set "used" bit for members
      if((status & TOFGC::SCBADB3)!=0)status|=AMSDBc::BAD;//bad for t-meas. according to DB 
//
      if((status & TOFGC::SCBADB2)!=0 && (status & TOFGC::SCBADB5)==0)status|=AMSDBc::BAD;
//          bad=(bar is 1-sided and not recovered -> bad for t-measur)
//
      AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",il), new
        AMSTOFCluster(status,ntof,barn,edep,edepd,coo,ecoo,time,etime,nmemb,membp));
//
      nclust+=1;
//------
    }// ---> end of ib thresh. check    
//------
  }// ---> end of buffer (ib) loop (clust.search)
  if(nclust>0)cllay[il]=1;
//------
  }// ---> end of TOF layers (il) loop
  if((cllay[0]+cllay[1]+cllay[2]+cllay[3])>0)stat=0;//at least one layer with cluster
// 
}
//
//--------------------------------------------
//
void AMSTOFCluster::recovers2(AMSTrTrack * ptr){ // recreate TOFCluster
//  time through recovering(using tracker) of 1-sided TOFRawCluster-members
// (only for 1-member clusters: 2-memb.clusters are 2-sided by definition(creation))
  integer status,nm,mib,mil;
  AMSDir dir(0,0,1.);
  AMSPoint coo;
  AMSPoint outp;
  number theta,phi,sleng,ctr,clo,newt;
//
  if(_nmemb==1){// only for 1-memb clusters
    nm=0;
    status=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getstatus();
    mil=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getntof()-1;
    mib=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getplane()-1;
    if(nm==0){// 
      coo[0]=0.;
      coo[1]=0.;
      coo[2]=TOF2DBc::getzsc(mil,mib);
      ptr->interpolate(coo,dir,outp,theta,phi,sleng);
      if(TOF2DBc::plrotm(mil)==0){
        clo=outp[1];// unrot.(X-meas) -> Y-cross = long.c(loc.r.s.=abs.r.s.)
        ctr=outp[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        clo=outp[0];// rot.(Y-meas) -> take X-cross for long.c.
//        clo=-clo;// go to bar local r.s.
        ctr=outp[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
    }
      dynamic_cast<TOF2RawCluster*>(_mptr[nm])->recovers(clo);//missing side recovering
      newt=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->gettime();//new time of raw cluster (ns)
      _time=-newt*1.e-9;//new time of cluster(-sec for Vitali)
      _etime=1.323*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(sec-> 160ps, trecker recovered)
  }
}
//-----------------------------------------------------------------------------

integer TOF2RawCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("TOF2RawCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}
//---
integer AMSTOFCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
   init=1;
   integer ntrig=AMSJob::gethead()->gettriggerN();
   for(int n=0;n<ntrig;n++){
     if(strcmp("AMSTOFCluster",AMSJob::gethead()->gettriggerC(n))==0){
       WriteAll=1;
       break;
     }
   }
  }
  return (WriteAll || status);
}
//=======================================================================
//  DAQ-interface functions :
//
// function returns number of TOF_data-words for given block/format
//
integer TOF2RawEvent::calcdaqlength(int16u blid){
  return(0);
}
//--------------------------------------------------------------------
void TOF2RawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
}
//----------------------------------------------------------------------
// function to decode DAQ and build RawEvent: 
//  
void TOF2RawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
}
//-------------------------------------------------------------------------------
//  function to get sofrware-id (LBBS) for given hardware-chan (crate,sfet,tofch):
//                                                               0-7   0-3  0-3
int16u TOF2RawEvent::hw2swid(int16u a1, int16u a2, int16u a3){
  return(200);
}



AMSID AMSTOFCluster::crgid(int k){
  int ii,kk;
  char inum[11];
  char in[2]="0";
  char vname[5];
  strcpy(inum,"0123456789");
  strcpy(vname,"TF");
  kk=(_ntof-1)*TOF2GC::SCMXBR+_plane;//counter ID used in volume name
  ii=kk/10;
  in[0]=inum[ii];
  strcat(vname,in);
  ii=kk%10;
  in[0]=inum[ii];
  strcat(vname,in);
  return AMSID(vname,_ntof*100+_plane);
}
