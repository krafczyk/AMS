// last modif. 10.12.96 by E.Choumilov - TOF2RawCluster::build added, 
//                                       AMSTOFCluster::build rewritten
//              16.06.97   E.Choumilov - TOF2RawEvent::validate added
//                                       TOF2RawCluster::sitofdigi modified for trigg.
//              26.06.97   E.Choumilov - DAQ decoding/encoding added
//
#include <tofdbc02.h>
#include <tofdbc.h>
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
extern TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];// peds/sigmas/.. data
integer TOF2RawCluster::trpatt[TOF2GC::SCLRS]={0,0,0,0};//just init. of static var.
integer TOF2RawCluster::trflag=0;
uinteger TOF2RawEvent::StartRun(0);
time_t TOF2RawEvent::StartTime(0);
AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};
integer AMSTOFCluster::_planes=0;
integer AMSTOFCluster::_padspl[TOFGC::MAXPLN]={0,0,0,0};
//
//
//-----------------------------------------------------------------------
void TOF2RawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u nftdc[2],nstdc[2],nadca[2],nadcd[2];
  int16u  ftdc1[TOF2GC::SCTHMX2*2],stdc1[TOF2GC::SCTHMX3*4],adca1[TOF2GC::SCTHMX4],adcd1[TOF2GC::SCTHMX4];
  int16u  ftdc2[TOF2GC::SCTHMX2*2],stdc2[TOF2GC::SCTHMX3*4],adca2[TOF2GC::SCTHMX4],adcd2[TOF2GC::SCTHMX4];
  integer ilay,last,ibar,isid,isds;
  integer i,j,im,nhit,chnum,brnum,am[2],tmi[2],itmf[2],sta,st;
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idd,idN,stat[2],idr;
  number tsr[3],ths[2],fstd,t1,t2,t3,t4,dt,adch,adcl;
  geant mtma[2],mtmd[2];
  geant peds[4];
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
    id=idd/10;// short id=LBB, where L=1,4 BB=1,14
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
#ifdef __AMSDEBUG__
    assert(ilay>=0 && ilay<TOF2GC::SCLRS);
    assert(ibar>=0 && ibar<TOF2GC::SCMXBR);
    assert(isid>=0 && isid<2);
    if(TFREFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
    edep=ptr->getedep();//tempor
    charge=ptr->getcharg();
    scbrped[ilay][ibar].getped(peds);// get peds(s1-h/l,s2-h/l sequence)
//
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      nftdc[isid]=ptr->getftdc(ftdc1);
      nstdc[isid]=ptr->getstdc(stdc1);
      nadca[isid]=ptr->getadch(adca1);
      nadcd[isid]=ptr->getadcl(adcd1);
      TOF2JobStat::addch(chnum,11);
//---> check hist-TDC :
      nhit=0;
      im=nftdc[isid];
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
        TOF2JobStat::addch(chnum,12);
      }
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im),1.);
//
//---> check stretcher-TDC :
      nhit=0;
      im=nstdc[isid];
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
        TOF2JobStat::addch(chnum,13);
      }
      if(nhit>0)bad=0;//at least one slow_TDC hit -> good tof-event
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+20),1.);
//
//---> check High-ADC(anode) :
      nhit=0;
      im=nadca[isid];
      for(i=0;i<im;i++)adca2[i]=0;
      nhit=im;// tempor
      if(nhit<im){//something was wrong
        ptr->putadch(int16u(nhit),adca2);// refill object with corrected data
        TOF2JobStat::addch(chnum,14);
      }
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+40),1.);
//
//---> check Low-ADC(dinode) :
      nhit=0;
      im=nadcd[isid];
      for(i=0;i<im;i++)adcd2[i]=0;
      nhit=im;// tempor
      if(nhit<im){//something was wrong
        ptr->putadcl(int16u(nhit),adcd2);// refill object with corrected data
        TOF2JobStat::addch(chnum,15);
      }
      if(TFREFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+60),1.);
//
//-----      
    } // ---> end of channel status check 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//  ---- end of RawEvent hits loop ------->
  if(bad==0)status=0;// good TOF-event
//
//---------------------ONLINE----------------------
//
  if(AMSJob::gethead()->isMonitoring() & 
     (AMSJob::MTOF | AMSJob::MAll)){ // TOF Online histograms
    ptr=(TOF2RawEvent*)AMSEvent::gethead()
                        ->getheadC("TOF2RawEvent",0);

    while(ptr){ //  <---- loop over TOF RawEvent hits -----
      int num,den,nedges[4];
      number Atovt,Dtovt;
      int his[TOF2GC::SCTHMX2*2],tdc[TOF2GC::SCTHMX3*4],ano[TOF2GC::SCTHMX4*2],dyn[TOF2GC::SCTHMX4*2];     
      idd=ptr->getid();
      id=idd/10;// short id=LBB, where L=1,4 BB=1,14
      ilay=id/100;
      ibar=id%100;
      isid=idd%10;
 
      nedges[0]=int (ptr->getftdc(ftdc1)); // History
      VZERO(stdc1,TOF2GC::SCTHMX3*4/2);
      nedges[1]=int (ptr->getstdc(stdc1)); // Time Exp.
      nedges[2]=int (ptr->getadch(adca1)); // Anode
      nedges[3]=int (ptr->getadcl(adcd1)); // Dynode

//
//      Eventually choose the right 2 edges for Anode and Dynode,
//      before calculating Atovt and Dtovt
      Atovt=number(adca1[0]);// just first hit
      Dtovt=number(adcd1[0]);

      tsr[0]=(stdc1[3]&pbanti)*TOF2DBc::tdcbin(1);
      tsr[1]=(stdc1[2]&pbanti)*TOF2DBc::tdcbin(1);
      tsr[2]=(stdc1[0]&pbanti)*TOF2DBc::tdcbin(1);
      num=tsr[1]-tsr[2]; // Time Exp. =
      den=tsr[0]-tsr[1]; //    (num - offset)/den
      TOF2RawEvent::tofonlinefill1(ilay,ibar,isid, // coord. 1-4,1-14,1-2
		     nedges,         // array of numbers of edges 
		     num,den,        // ~ time stretcher ratio
		     Atovt,Dtovt);   // Anode & Dynode time over thr.
      ptr=ptr->next();// take next RawEvent hit
    }
  }
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
      id=idd/10;// short id=LBB, where L=1,4 BB=1,14
      ilay=id/100-1;
      ibar=id%100-1;
      isid=idd%10-1;
      chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channels numbering
      stat[isid]=ptr->getstat();
      if(stat[isid] == 0){  
        isds+=1;
//
// --> fill for STRR-calibration :
//
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadch(adca1);
        nadcd[isid]=ptr->getadcl(adcd1);
        if(nstdc[isid]==4){ // require only one "4-edge" sTDC-hit
          tsr[0]=number((stdc1[3]&pbanti)*TOF2DBc::tdcbin(1));
          tsr[1]=number((stdc1[2]&pbanti)*TOF2DBc::tdcbin(1));
          tsr[2]=number((stdc1[0]&pbanti)*TOF2DBc::tdcbin(1));
          TOF2STRRcalib::fill(chnum,tsr);
          if(nftdc[isid]==2 && nadca[isid]==1){ // require only one "2-edge" f-TDC-hit
            ths[0]=(ftdc1[1]&pbanti)*TOF2DBc::tdcbin(0);
            ths[1]=(ftdc1[0]&pbanti)*TOF2DBc::tdcbin(0);
            fstd=ths[0]-tsr[0];
            TOF2STRRcalib::fill2(chnum,fstd);
          }
        }
//
// --> fill for AVSD-calibration :
//
//        if(nadca[isid]==1 && nadcd[isid]==1){ // accept single-hit on both sides
//          adch=number(adca1[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(High_ch, 1st hit)
//          adcl=number(adcd1[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(Low_ch,  1st hit)
//          if((adch+peds[2*isid])>=number(TOF2GC::SCADCMX-1))adch=0.;//mark overflow by "0"
//          if((adcl+peds[2*isid+1])>=number(TOF2GC::SCADCMX-1))adcl=0.;//mark overflow by "0"
//          if(adch>0. && adcl>0.)TOF2AVSDcalib::filltovt(chnum,adch,adcl);
//        }
      }// --- end of status check --->
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawEvent hits loop ------->
//
    status=1;// set to "bad" to avoid further reconstruction
  }
}
//-----------------------------------------------------------------------
void TOF2RawCluster::sitofdigi(int &status){
  AMSTOFMCCluster * ptr=(AMSTOFMCCluster*)
  AMSEvent::gethead()->
   getheadC("AMSTOFMCCluster",0,1); // last 1  to test sorted container
  static number xplane[4][TOF2GC::SCMXBR];
  static number xtime[4][TOF2GC::SCMXBR];
  static number xtimed[4][TOF2GC::SCMXBR];
  static number xz[4][TOF2GC::SCMXBR];
  integer plrot;
  number stm[2],edepd,etd;
  number ama[2],amd[2];
  VZERO(xtime,TOF2GC::SCMXBR*sizeof(number));
  VZERO(xtimed,TOF2GC::SCMXBR*sizeof(number));
  VZERO(xplane,TOF2GC::SCMXBR*sizeof(number));
  VZERO(xz,TOF2GC::SCMXBR*sizeof(number));
  geant x,y;
  status=1;// bad
  while(ptr){
   integer ntof,plane,status;
   ntof=(ptr->idsoft)/100-1;//ilay
   plane=(ptr->idsoft)%100-1;//ibar
#ifdef __AMSDEBUG__
   assert(plane>=0 && plane< TOF2GC::SCMXBR);
   assert (ntof>=0 && ntof<4);
#endif
   xplane[ntof][plane]+=ptr->edep*1000;
   xtime[ntof][plane]+=ptr->tof*(ptr->edep)*1000;//tof*edep
   plrot=TOF2DBc::plrotm(ntof);     // =0/1-unrotated/rotated TOF-plane
   integer ixy=0;
   if(plrot==0)ixy=1;
   xtimed[ntof][plane]+=ptr->xcoo[ixy]*(ptr->edep)*1000;//long_coo*edep
   xz[ntof][plane]=ptr->xcoo[2];
   status=0;
   integer last;
   last=ptr->testlast();
   ptr=ptr->next();  
   
  }
//
//
  geant dummy(-1);
  const number c=1.7e10; // eff. speed of light in scint.(cm/s)
  number latt=195.; // average light att. length (cm) in scint. bar
  number trthr=0.4;// trig.threshold(z>=1) in Mev (=0.2 MIP) (taken from the floor)
  number trthr3=6.;// trig.threshold(z>2) in Mev (=3 MIPs) (taken from the floor)
  integer ntrl(0),trpatt[TOF2GC::SCLRS]={0,0,0,0};
  integer ntrl3(0),trpatt3[TOF2GC::SCLRS]={0,0,0,0};
  integer trflag(0);
  integer bitp,lsbit(1);
  int i,statdb[2];
  geant halfl;
  number enshar,ylon,edp[2];
  number ts1,ts2;
  for(integer kk=0;kk<TOF2GC::SCLRS;kk++){
  for(i=0;i<TOF2GC::SCMXBR;i++){
    if(xplane[kk][i]>TFMCFFKEY.Thr){
     xtime[kk][i]=xtime[kk][i]/xplane[kk][i];
     xtimed[kk][i]=xtimed[kk][i]/xplane[kk][i];
     ylon=xtimed[kk][i];// long.coord. in abs. system
// ---> primitive simul.of level-1 trig.pattern (only for Choutko games):
     halfl=0.5*TOF2DBc::brlen(kk,i);
     enshar=exp(-(ylon+halfl)/latt)/(exp(-(ylon+halfl)/latt)+exp(-(halfl-ylon)/latt));
     edp[0]=enshar*xplane[kk][i]*exp(-(ylon+halfl)/latt);
     edp[1]=xplane[kk][i]-edp[0];
     TOF2Brcal::scbrcal[kk][i].getbstat(statdb); // "alive" status from DB
//      "z>=1" trigger :
     if((edp[0]>trthr || statdb[0]>0) && (edp[1]>trthr || statdb[1]>0)){// 2-ends AND !!
       bitp=lsbit<<i;
       trpatt[kk]|=bitp;
     }
//      "z>2" trigger :
     if((edp[0]>trthr3 || statdb[0]>0) && (edp[1]>trthr3 || statdb[1]>0)){// 2-ends AND !!
       bitp=lsbit<<i;
       trpatt3[kk]|=bitp;
     }
// --->
     if(RNDM(dummy)< TFMCFFKEY.TimeProbability2){
       ts1=TFMCFFKEY.TimeSigma2*rnormx();
       ts2=TFMCFFKEY.TimeSigma2*rnormx();
       etd=TFMCFFKEY.TimeSigma2*c*0.707;//err. on long.coord.
     }
     else {
       ts1=TFMCFFKEY.TimeSigma*rnormx();
       ts2=TFMCFFKEY.TimeSigma*rnormx();
       etd=TFMCFFKEY.TimeSigma*c*0.707;
     }
     xtime[kk][i]+=(ts1+ts2)/2.;// add  time-error
     xtimed[kk][i]+=(ts1-ts2)/2.*c; // add  coordinate-error
//
     plrot=TOF2DBc::plrotm(kk);
     if(plrot==1)xtimed[kk][i]=-xtimed[kk][i];//to agree with slow algorithm
     xtime[kk][i]=-(1.e9)*xtime[kk][i];//sec->-ns to agree with slow algorithm
     ama[0]=0.;
     ama[1]=0.;
     amd[0]=0.;
     amd[1]=0.;
     stm[0]=0.;
     stm[1]=0.;
     edepd=0.;
     AMSEvent::gethead()->addnext(AMSID("TOF2RawCluster",0),
     new TOF2RawCluster(0,kk+1,i+1,xz[kk][i],ama,amd,xplane[kk][i],
         edepd,stm,xtime[kk][i],xtimed[kk][i],etd));
    }

  }
  }
  for(i=0;i<TOF2GC::SCLRS;i++)if(trpatt[i]>0)ntrl+=1;// counts triggered (z>=1) layers
  for(i=0;i<TOF2GC::SCLRS;i++)if(trpatt3[i]>0)ntrl3+=1;// counts triggered (z>2) layers
  if(TFMCFFKEY.trlogic[1]==1){// h/w requirement ALL4
    if(ntrl==4){
      status=0;
      trflag=1;
    }
    if(ntrl3==4){
      status=0;
      trflag=3;
    }
  }
  else{                       // h/w requirement ANY3
    if(ntrl>=3){
      status=0;
      trflag=1;
    }
    if(ntrl3>=3){
      status=0;
      trflag=3;
    }
  } 
  TOF2RawCluster::settrfl(trflag);// set trig.flag 
  TOF2RawCluster::setpatt(trpatt);//tempor: add trigger(z>=1) pattern 
}
//----------------------------------------------------------------------------
void TOF2RawCluster::build(int &ostatus){
  int16u nftdc[2]={0,0};
  int16u nstdc[2]={0,0};
  int16u nadca[2]={0,0};
  int16u nadcd[2]={0,0};
  int16u  ftdc1[TOF2GC::SCTHMX2*2],stdc1[TOF2GC::SCTHMX3*4],adca1[TOF2GC::SCTHMX4],adcd1[TOF2GC::SCTHMX4];
  int16u  ftdc2[TOF2GC::SCTHMX2*2],stdc2[TOF2GC::SCTHMX3*4],adca2[TOF2GC::SCTHMX4],adcd2[TOF2GC::SCTHMX4];
  integer ilay,last,ibar,isid,isds,isd,isdsl[TOF2GC::SCLRS],hwid,tchan,crat,sfet,slnu,tdcc;
  integer i,j,k,chnum,brnum,am[2],tmi[2],itmf[2],sta,st,smty[2],ftdcfl,reject;
  integer trpatt[TOF2GC::SCLRS];
  uinteger Runum(0);
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  int16u id,idd,idN,stat[2],amf[2];
  number ama[2],amd[2],amc[2];
  geant peds[4];
  number zc,qtota,qtotd,tmf[2],time,coo,ecoo;//input to RawCluster Constr
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
  int bad,tsfl(0),rml4s2,highchok,lowchok,hlf;
// some variables for histogramming:
  geant gdt,tch,pch1[TOF2GC::SCLRS],pch2[TOF2GC::SCLRS],rrr[2];
  geant edepa[TOF2GC::SCLRS],edepd[TOF2GC::SCLRS],tcorr[TOF2GC::SCLRS],elosn;
  geant tuncorr[TOF2GC::SCLRS],tdiff[TOF2GC::SCLRS],td13,td24,td14;
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
  rml4s2=0;
  if(TFREFFKEY.relogic[2]!=0)rml4s2=1;
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,14
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
//    edep=ptr->getedep();
    chnum=ilay*TOF2GC::SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*TOF2GC::SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
//    if(stat[isid] == 0){  
    TOF2Brcal::scbrcal[ilay][ibar].getbstat(statdb); // "alive" status from DB
    if(statdb[isid] >= 0){  // channel alive(no severe problems), read out it
      hwid=TOF2RawEvent::sw2hwid(ilay,ibar,isid);
      crat=hwid/100-1;
      slnu=1;//sequential number of slot with temp. (only 1 exists) 
//       fill working arrays for given side:
      isds+=1;
      if(isid==0){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadch(adca1);
        nadcd[isid]=ptr->getadcl(adcd1);
        tdcc=12;//temper. reading for anode
        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
        treada[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for anode chain
        tdcc=13;//temper. reading for stretcher
        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
        treads[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for stretcher chain
      } 
      if(isid==1){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc2);
        nstdc[isid]=ptr->getstdc(stdc2);
        nadca[isid]=ptr->getadch(adca2);
        nadcd[isid]=ptr->getadcl(adcd2);
        tdcc=12;//temper. reading for anode
        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
        treada[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for anode chain
        tdcc=13;//temper. reading for stretcher
        tchan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crat+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
        treads[isid]=DAQS2Block::gettemp(tchan);// get "temperature" for stretcher chain
      }
      TOF2JobStat::addch(chnum,0);        // channel statistics :
      if(nftdc[isid]>0)TOF2JobStat::addch(chnum,1);
      if(nstdc[isid]>0)TOF2JobStat::addch(chnum,2);
      if(nadca[isid]>0)TOF2JobStat::addch(chnum,3);
      if(nadcd[isid]>0)TOF2JobStat::addch(chnum,4);
      if(nftdc[isid]==2)TOF2JobStat::addch(chnum,5);
      if(nstdc[isid]==4)TOF2JobStat::addch(chnum,6);
      if(nadca[isid]==1)TOF2JobStat::addch(chnum,7);
      if(nadcd[isid]==1)TOF2JobStat::addch(chnum,8);
      if(nftdc[isid]>=2 && nstdc[isid]>=4 && nadca[isid]>=1)TOF2JobStat::addch(chnum,9);
      if(TFREFFKEY.reprtf[2]>1){
        HF1(1120+crat,geant(treads[isid]),1.);
        HF1(1130+crat,geant(treada[isid]),1.);
      }
    } 
//
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (LBB)
//------------------------------------------------------
//aaa--->
    if(idN != id){ // both sides ready, next hit is OTHER_counter/last hit,
//       so process CURRENT counter data : 
//---
      if(isds==2 || isds==1){ // sides presence check
        TOF2JobStat::addbr(brnum,0);//h/w status ok(at least one alive(in DB) side is present )
//
// =============> start pattern recognition for given sc.bar :
//
// --------> select bar with >= 1x4-multiplicity in stdc, >= 1x2 in ftdc/adc 
//(first pulse from readout (last in real time) will be used if many stdc/adc are present): 
//
        smty[0]=0;
        smty[1]=0;
        ftdcfl=TFREFFKEY.relogic[1];// how to use f-TDC 
        if(nstdc[0]>=4 && nadca[0]>=1 && (nftdc[0] >=2 || ftdcfl==2))smty[0]=1;
        if(nstdc[1]>=4 && nadca[1]>=1 && (nftdc[1] >=2 || ftdcfl==2))smty[1]=1;
//-------ONLINE--------
	if(AMSJob::gethead()->isMonitoring() & 
	   (AMSJob::MTOF | AMSJob::MAll)){
	  static int nIDhis[1200];
	  static int first=0;
	  if(first==0){
	    for(int k=0;k<1200;k++)nIDhis[k]=0;}
	  first++;
	  if(smty[0]==1 && smty[1]==1){
	    int hisID=5023+ilay;
	    HFF1(hisID,nIDhis[hisID-5000],(geant)(14-ibar),1.);
	  }
	}
//----------------------
//----> force suppression of some side-signals  by flag
        if(rml4s2){
//          if((ilay==3) && (ibar>1) && (ibar<12))smty[1]=0;//tempor (what to suppress)
        }
//bbb--->
        if(smty[0]==1 || smty[1]==1){ //1-side bar is accepted,if have complete measurements) 
          TOF2JobStat::addbr(brnum,1);
          isds=smty[0]+smty[1];// number of  good sides
          sta=0;
// -> add status-bits for known problems:
          if(((statdb[0]%100)/10 > 0) || ((statdb[1]%100)/10 > 0))
                               sta|=TOFGC::SCBADB3;// set bit of known bad t-measurement on any side
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
//---> Time, high(anode)-channel check:
//
            chnum=ilay*TOF2GC::SCMXBR*2+ibar*2;//s1-channel number 
            tm[0]=0;
            ama[0]=0.;
            tmr[0]=0.;
            scbrped[ilay][ibar].getped(peds);// get peds(s1-h/l,s2-h/l sequence)
            if(smty[0]==1){// good(3-measurement) side, but matching/history may not be good
              t4=(stdc1[0]&pbanti)*TOF2DBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc1[2]&pbanti)*TOF2DBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc1[3]&pbanti)*TOF2DBc::tdcbin(1);// 1-st edge of str-info
              tm[0]=((t2-t4)-srof[0])/strr[0];// s-1 time (ns,A-noncorr)
              tmf[0]=tm[0];
              ama[0]=number(adca1[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(High_ch_s1,1st hit)
              if(TFREFFKEY.reprtf[2]>0)HF1(1104,geant(ama[0]),1.);
	      if((ama[0]+peds[0])>=number(TOF2GC::SCADCMX-1))ama[0]=0.;//mark overflow by "0"
              if(ama[0]==0.)TOF2JobStat::addch(chnum,10);//counts h-cnan. overflows
              tmr[0]=t1-t2;
            }
            tm[1]=0;
            ama[1]=0.;
            tmr[1]=0.;
            if(smty[1]==1){// good(3-measurement) side, but matching/hist may not be good
              t4=(stdc2[0]&pbanti)*TOF2DBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc2[2]&pbanti)*TOF2DBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc2[3]&pbanti)*TOF2DBc::tdcbin(1);// 1-st edge of str-info
              tm[1]=((t2-t4)-srof[1])/strr[1];// s-2 time (ns,A-noncorr)
              tmf[1]=tm[1];
              ama[1]=number(adca2[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(High_ch_s2,1st hit)
              if(TFREFFKEY.reprtf[2]>0)HF1(1104,geant(ama[1]),1.);
	      if((ama[1]+peds[2])>=number(TOF2GC::SCADCMX-1))ama[1]=0.;//mark overflow by "0"
              if(ama[1]==0.)TOF2JobStat::addch(chnum+1,10);//counts h-cnan. overflows
              tmr[1]=t1-t2;
            }
            if(smty[0]==0){ // make "=" each side times and ampl. for 1-side case:
              tmf[0]=tmf[1];
              ama[0]=ama[1];
            }
            if(smty[1]==0){
              tmf[1]=tmf[0];
              ama[1]=ama[0];
            }
	    highchok=1;
	    if(ama[0]==0.|| ama[1]==0.){//both sides ->0. if any overflow
	      ama[0]=0.;
	      ama[1]=0.;
	      highchok=0;
	    }
//
//---> Low(dinode)-channel check
//
            amd[0]=0.;
            amd[1]=0.;
            if(smty[0]>0 && nadcd[0]>=1){
              amd[0]=number(adcd1[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(Low_ch_s1,1st hit)
              if(TFREFFKEY.reprtf[2]>0)HF1(1105,geant(amd[0]),1.);
	      if((amd[0]+peds[1])>=number(TOF2GC::SCADCMX-1))amd[0]=0.;//mark overflow by "0"
            }
            if(smty[1]>0 && nadcd[1]>=1){
              amd[1]=number(adcd2[0]*TOF2DBc::tdcbin(2));//DAQ-units-->ADC-ch(float)(Low_ch_s2,1st hit)
              if(TFREFFKEY.reprtf[2]>0)HF1(1105,geant(amd[1]),1.);
	      if((amd[1]+peds[3])>=number(TOF2GC::SCADCMX-1))amd[1]=0.;//mark overflow by "0"
            }
            if(amd[0]==0 && amd[1]>0){ // make "=" each side d-ampl. for 1-side case:
              amd[0]=amd[1];
            }
            if(amd[1]==0 && amd[0]>0){
              amd[1]=amd[0];
            }
	    lowchok=1;
	    if(amd[0]==0.|| amd[1]==0.){//both sides ->0. if any overflow
	      amd[0]=0.;
	      amd[1]=0.;
	      lowchok=0;
	    }
//
            zc=TOF2DBc::getzsc(ilay,ibar);
            blen=TOF2DBc::brlen(ilay,ibar);
//ccc------->
            if(TFREFFKEY.relogic[0] != 1
	             && (highchok+lowchok)>0){//<-- bypass if no signal or srs-calibration
	      if(highchok){
	        amc[0]=ama[0];
	        amc[1]=ama[1];
		hlf=0;
	      }
	      else{
	        amc[0]=amd[0];
	        amc[1]=amd[1];
		hlf=1;
	      }
//
//--> Calc. longit. coord/err and position correction to charge-signal :
//
              TOF2Brcal::scbrcal[ilay][ibar].tmd2p(tmf,amc,hlf,co,eco);// get A-corrected Local(!).coord/err
	      dt=-0.5*(tmf[0]-tmf[1]);
//   cout<<"l/b="<<ilay<<" "<<ibar<<" Td="<<dt<<" co="<<co<<endl; 
              brlm=0.5*TOF2DBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means: "coord. is more than counter half length"
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=TOF2Brcal::scbrcal[ilay][ibar].poscor(co);
              }
              if(isds==1)pcorr=1.;// no position correction for one-sided counte
//
//--> Find Eloss from high(anode)-channel :
//
              qtota=0.;
              if(highchok)qtota=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(0,ama)
                                             /pcorr; // high(anode) Edep(mev) with corrections
//
//--> Find counter-time corrected for high(low) ADC-signal :
//
              time=TOF2Brcal::scbrcal[ilay][ibar].tm2t(tmf,amc,hlf); // time with corrections
//
//--> Find Eloss from low(dinode)-channel :
//
              qtotd=0.;
              if(lowchok)qtotd=TOF2Brcal::scbrcal[ilay][ibar].adc2mip(1,amd)
                                            /pcorr;// low(dinode) Edep(mev) with corrections
//-->
              nbrl[ilay]+=1;
              isdsl[ilay]=isds;
              edepa[ilay]=qtota;
              edepd[ilay]=qtotd;
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
              ,new TOF2RawCluster(sta,ilay+1,ibar+1,zc,ama,amd,
                                  qtota,qtotd,tm,time,coo,ecoo)))st=1;;//store values
            } // ccc------> end of bypass check
//-----------
        } // bbb---> end of "side measurement-multiplicity" check
//
      } // aaa---> end of sides presence check
      isds=0;// clear side-counters/temperatures befor next bar processing
      nftdc[0]=0;
      nftdc[1]=0;
      nstdc[0]=0;
      nstdc[1]=0;
      nadca[0]=0;
      nadca[1]=0;
      nadcd[0]=0;
      nadcd[1]=0;
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
  if((nbrch[0]+nbrch[1])>=1 && (nbrch[2]+nbrch[3])>=1)ostatus=0; // good event
// 
// --->same multtipl. checks for single bar layers:
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
  if(rml4s2){// do recovering
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
  zpl1=TOF2DBc::supstr(1)+TOF2DBc::supstr(7)+
      (TOF2DBc::plnstr(6)+2.*TOF2DBc::plnstr(7))/2.+TOF2DBc::plnstr(3)/2.;//z-l1-middl
  zpl2=TOF2DBc::supstr(1)-
      (TOF2DBc::plnstr(6)+2.*TOF2DBc::plnstr(7))/2.-TOF2DBc::plnstr(3)/2.;//z-l2-middl
  trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
//
  if(TFREFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
      HF1(1532,td13,1.);//ToF for L0->L2
      HF1(1534,td24,1.);//ToF for L1->L3
      gdt=(td13/trlen13-td24/trlen24)*trlnor;// normalized to fix(~125cm) dist. for AMS02
//                                                           (130cm for AMS01)
      HF1(1544,gdt,1.);
      if(TFREFFKEY.reprtf[2]>1){ 
        for(il=0;il<TOF2GC::SCLRS;il++){// fill de/dx hist. for each bar
          ib=brnl[il];
          brnum=il*TOF2GC::SCMXBR+ib;//sequential bar numbering
          elosn=edepa[il]/cosi;//normalized  to norm.inc.
          HF1(1140+brnum,elosn,1.);
        }
        HF1(1529,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1531,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
        HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      }
      HF1(1533,tdiff[0],1.);//layer=0
      HF1(1543,clong[0],1.);//Y-coord(loc.r.s.)
      HF1(1545,clong[1],1.);//Y-coord(loc.r.s.)
      HF1(1546,clong[2],1.);//Y-coord(loc.r.s.)
      HF1(1547,clong[3],1.);//Y-coord(loc.r.s.)
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
  TOF2Brcal::scbrcal[il][ib].getgaind(gnd);
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
// ---> recover High(anode)-signal for bad side :
  if(_edeph>0.){
    tot[isg]=_adch[isg];
    TOF2Brcal::scbrcal[il][ib].q2t2q(1,isg,0,tot[isg],q[isg]);//ADC->Q
    q[isg]/=gn[isg];// Q->Qref
    sqr=upr[0]*(exp(-(hclen+x)/upr[3])+upr[2]*exp(-(hclen+x)/upr[4]))/
      upr[1]/(exp(-(hclen-x)/upr[3])+upr[2]*exp(-(hclen-x)/upr[4]));//Qr1/Qr2
    if(isb==1)sqr=1./sqr;
    q[isb]=q[isg]*sqr;
    q[isb]*=gn[isb];//Qref->Q
    TOF2Brcal::scbrcal[il][ib].q2t2q(0,isb,0,tot[isb],q[isb]);//Q->ADC
    _adch[isb]=tot[isb]; // recovered ADC for bad side
    _edeph=TOF2Brcal::scbrcal[il][ib].adc2mip(0,tot)/pcorr; //recowered high(anode)-Eloss
    tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
    hlf=0;
    adc[0]=tot[0];
    adc[1]=tot[1];
  }
//
// ---> recover Low(dinode)-signal for bad side :
  if(_edepl>0){
    tot[isg]=_adcl[isg];
    TOF2Brcal::scbrcal[il][ib].q2t2q(1,isg,1,tot[isg],q[isg]);//ADC->Q
    q[isg]/=gnd[isg];// Q->Qref
    q[isb]=q[isg]*sqr;
    q[isb]*=gnd[isb];//Qref->Q
    TOF2Brcal::scbrcal[il][ib].q2t2q(0,isb,1,tot[isb],q[isb]);//Q->ADC
    _adcl[isb]=tot[isb]; // recovered ADC for bad side
    _edepl=TOF2Brcal::scbrcal[il][ib].adc2mip(1,tot)/pcorr; //recowered low(dinode)-Eloss
    if(_edeph==0.){//slew.cor. from low(din)-chan., if high-chan. is missing
      tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections for time
      hlf=1;
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
  TOFRawClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tofraw();

  if (TN->Ntofraw>=MAXTOFRAW) return;

// Fill the ntuple
  if(TOF2RawCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Ntofraw]=_status;
    TN->Layer[TN->Ntofraw]=_ntof;
    TN->Bar[TN->Ntofraw]=_plane;
    for(int i=0;i<2;i++){
      TN->tovta[TN->Ntofraw][i]=_adch[i];
      TN->tovtd[TN->Ntofraw][i]=_adcl[i];
      TN->sdtm[TN->Ntofraw][i]=_sdtm[i];
    }
    TN->edepa[TN->Ntofraw]=_edeph;
    TN->edepd[TN->Ntofraw]=_edepl;
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
  stream <<"TOF2RawCluster "<<_ntof<<" "<<_edeph<<" "<<_plane<<endl;
}



void AMSTOFCluster::_writeEl(){
  TOFClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tof();

  if (TN->Ntof>=MAXTOF) return;

// Fill the ntuple
  if(AMSTOFCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
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
    TN->Ntof++;
  }
}



void AMSTOFCluster::_copyEl(){
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
  int i;
  if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    _planes=TOF2GC::SCLRS;
    for(i=0;i<_planes;i++)_padspl[i]=TOF2GC::SCBRS[i];
  }
  else{
    _planes=TOF1GC::SCLRS;
    for(i=0;i<_planes;i++)_padspl[i]=TOF1GC::SCBRS[i];
  }

}
//---------------------------------------------------------------------------
void AMSTOFCluster::build2(int &stat){
  TOF2RawCluster *ptr; 
  TOF2RawCluster *ptrr; 
  static TOF2RawCluster * xptr[TOF2GC::SCMXBR+2];
  static number eplane[TOF2GC::SCMXBR+2],edplane[TOF2GC::SCMXBR+2];
  static AMSlink * membp[3];
  geant dummy,edep,edepl,edepa,edepd,asatl,time,etime,speedl,err;
  integer ntof,barn,status,mstatus,plrot;
  geant barl,barw,bars,cofg,cofgl,yloc,eyloc,ylocm,c0,ct,cl,ed;
  geant errx,erry,erx1(2.);//tempor errors on cl.transv.coord.
  AMSPoint coo,ecoo;
  int i,j,il,ib,ilay,ibar,ill,ibb;
  int nclust,cllay[TOF2GC::SCLRS],nmemb,imb,nmembd;
  geant edmemb[3],ed2nd,edass,ycoo[3],yc,yc2nd,dycoo,epeak,epeakd,cas,cyc;
//-----
  stat=1; // bad
  for(i=0;i<TOF2GC::SCLRS;i++)cllay[i]=0;
//
  for(il=1;il<TOF2GC::SCLRS+1;il++){// <-------- TOF layers loop -----
    ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                                     getheadC("TOF2RawCluster",0);
    VZERO(eplane,(TOF2GC::SCMXBR+2)*sizeof(number)/4);
    VZERO(xptr,(TOF2GC::SCMXBR+2)*sizeof(TOF2RawCluster*)/4);
    while (ptr){// scan to put all bars of layer "il" in buffer
      if(ptr->getntof()==il){
        ib=ptr->getplane();
#ifdef __AMSDEBUG__
        assert(ib>0 && ib <= TOF2GC::SCMXBR);
#endif
        edepa=ptr->getedeph();
        edepd=ptr->getedepl();
//    select between high(anode) and low(dinode) measurements:
	if(edepa>0.)edep=edepa;// no saturation of high-chan --> use it
	else edep=edepd; // saturation in high-chan --> use low channnel
//
        eplane[ib]=edep;
        edplane[ib]=edepd;
        xptr[ib]=ptr;
      }
      ptr=ptr->next();
    }// ---> end of scan
//------
  nclust=0;
  for (i=1;i<=TOF2GC::SCMXBR;i++){// <---- loop over buffer content (clust. search)
    if(   eplane[i]> TFREFFKEY.Thr1
       && eplane[i]> eplane[i-1] 
       && eplane[i]> eplane[i+1] ){ // <--- peak check (over 3-bars group)
      ptr=xptr[i];// peak bar pointer
#ifdef __AMSDEBUG__
      assert(ptr!=NULL);
#endif
      barn=i;// peak bar number
      ntof=ptr->getntof();// TOF-layer number
      ilay=ntof-1;
      ibar=barn-1;
      barl=TOF2DBc::brlen(ilay,ibar);// peak bar length
      barw=TOF2DBc::plnstr(5);//bar width
      bars=TOF2DBc::plnstr(5)-TOF2DBc::plnstr(4)
                            +2.*TOF2DBc::plnstr(13);//sc.bar transv. step
      TOF2Brcal::scbrcal[ilay][ibar].getd2p(speedl,err);//get light speed
      yloc=ptr->gettimeD();// get yloc/err for "peak" bar
      eyloc=ptr->getetimeD();
      ylocm=0.5*barl;// limit on max. long. coord.
      status=ptr->getstatus();//may be !=0(bad history/t_meas or single-sided)
      if(fabs(yloc) > ylocm){//out of bar size
        status|=AMSDBc::BAD; // bad coord. means also bad for t-measurement !!!
        eyloc=barl/sqrt(12.);
        if(yloc>0.)yloc=ylocm;//at bar edge
        else yloc=-ylocm;
        status|=AMSDBc::BAD; 
      }
      edep=0.;
      edepd=0.;
      edepl=0.;
      cofg=0.;
      cofgl=0.;
      nmemb=0;
      nmembd=0;
      imb=0;
      membp[imb]=ptr;//store raw cl. member pointers(peak-bar)
      edmemb[0]=eplane[i];
      edmemb[1]=0.;
      edmemb[2]=0.;
      ycoo[0]=yloc;
      ycoo[1]=0.;
      ycoo[2]=0.;
      for(j=i-1;j<i+2;j++){// calc. clust. energy/COG-transv/COG-long
        ptrr=xptr[j];   
        if(ptrr){
          nmemb+=1;
          ill=ptrr->getntof()-1;
          ibb=j-1;
          ylocm=0.5*TOF2DBc::brlen(ill,ibb);// bar length
          edep+=eplane[j];
          if(edplane[j]>0.){
            edepd+=edplane[j];
            nmembd+=1;
          }
          cofg+=eplane[j]*(j-i);// relative to "peak" bar
          yloc=ptrr->gettimeD();
          if(fabs(yloc) > ylocm){
            if(yloc>0.)yloc=ylocm;//at bar edge
            else yloc=-ylocm;
          }
          if(j!=i){//store some member parameters
            imb+=1;
            membp[imb]=ptrr;//raw cl. member pointers(neigbours)
	    edmemb[imb]=eplane[j];
	    ycoo[imb]=yloc;
          }
          edepl+=eplane[j];
          cofgl+=eplane[j]*yloc;
        }
      }//---> end of the member loop
      time=ptr->gettime();// tempor time from peak(!) bar(ns) 
      etime=TFMCFFKEY.TimeSigma/sqrt(2.);//(sec !!) tempor(121ps,later put in TOFBrcal needed data!)
      if((status&TOFGC::SCBADB2)>0){
        if(status & TOFGC::SCBADB5){ // recovered 1-sided counter
          etime=2.05*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor(248ps,still no Tracker info)
        }
        else{
          etime=(1.e-9)*barl/speedl/sqrt(12);//(sec !!!)for single-sided counters
        }
      }
//------
      if(edep>TFREFFKEY.ThrS){// <--- calc.clus.parameters if Ecl>Ethresh
        coo[2]=ptr->getz();          //cluster Z-coord.
        ecoo[2]=TOF2DBc::plnstr(6)/sqrt(12.);//bar thickness/...   
        plrot=TOF2DBc::plrotm(ilay);     // =0/1-unrotated/rotated TOF-plane
        c0=TOF2DBc::gettsc(ilay,ibar);   //transv.pos. of "peak" bar
        ct=cofg/edep*bars+c0;           //cluster abs. transv. coord.
        if(edepl>0.)cl=cofgl/edepl;    //cluster longit.coord.(Y-loc)
        else cl=0.; // just center of sc.bar
        errx=erx1/sqrt(geant(nmemb));//rough X(transv) err. for multimemb.cluster 
        erry=eyloc/sqrt(geant(nmemb));//rough Y(longit) err. for multimemb.cluster 
//    Calculate abs. 2-D coordinates of cluster COG according to plane rotation mask:
        if(plrot==0){ // non-rotated plane
          coo[0]=ct;//clust. X-coord.
          ecoo[0]=errx;
          coo[1]=cl;//clust. abs.Y-coord.(neglect counter's long.shift)
          ecoo[1]=erry;
        }
        else{ // rotated plane
          coo[0]=-cl;//abs.X-coord(yloc=-xabs and neglect counter's long.shift)
          ecoo[0]=erry;
          coo[1]=ct;
          ecoo[1]=errx;
        }
        time=-time*1.e-9;// ( ns -> sec ,"-" for V.Shoutko fit)
//
//              <--- select between average- or peak-mode of cluster energy calc.:
//
	epeak=edmemb[0];//peak bar energy(here it is already combined "high+low" channel)
	yc=ycoo[0];// peak bar long.coord.
        epeakd=ptr->getedepl();//low-chan. peak bar energy
	edass=-1.;
	dycoo=999.;
	if(nmemb>1){// real cluster
          ed2nd=edmemb[1];
	  yc2nd=ycoo[1];
	  if(edmemb[2]>0. && edmemb[1]<edmemb[2]){
	    ed2nd=edmemb[2];
	    yc2nd=ycoo[2];
	  }
	  edass=(epeak-ed2nd)/(epeak+ed2nd);
	  dycoo=yc-yc2nd;
	  if(TFREFFKEY.reprtf[2]>0){
	    HF1(1548,edass,1.);
	    if(edass<TOF2Varp::tofvpar.eclass())HF1(1549,dycoo,1.);
	    HF2(1550,dycoo,edass,1.);
	  }
//
          if(edass<TOF2Varp::tofvpar.eclass() 
	             && fabs(dycoo)<TOF2Varp::tofvpar.eclmat()){//pure bar-overlaping case(single part)
            edep/=nmemb;// calc. average for "high"-ch in multi-bar clusters
            if(nmembd>0)edepd/=nmembd;//  calc. average for "low"-ch in  multi-bar clusters
	  }
	  else{
	    edep=epeak;
	    edepd=edplane[i];
	  }
//
	}
//
        if(TFREFFKEY.reprtf[2]>0){// some histograms:
          if(il==1){
            HF1(1535,edep,1.);//Cluster energy distr.,L=1
            HF1(1537,edep,1.);
          }
          if(il==2){
            HF1(1539,edep,1.);//Cluster energy distr.,L=2
            HF1(1541,edep,1.);
          }
          if(il==3){
            HF1(1536,edep,1.);//Cluster energy distr.,L=3
            HF1(1538,edep,1.);
          }
          if(il==4){
            HF1(1540,edep,1.);//Cluster energy distr.,L=4
            HF1(1542,edep,1.);
          }
        }
//
        for(j=0;j<nmemb;j++)membp[j]->setstatus(AMSDBc::USED);// set "used" bit for members
        if((status & TOFGC::SCBADB3)!=0)status|=AMSDBc::BAD; 
//          bad=(peak bar has severe problem with t-measurement)
        if((status & TOFGC::SCBADB2)!=0 && (status & TOFGC::SCBADB5)==0)status|=AMSDBc::BAD;
//          bad=(peak bar is 1-sided and not recovered -> bad for t-measur)
//
        AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ilay), new
          AMSTOFCluster(status,ntof,barn,edep,edepd,coo,ecoo,time,etime,nmemb,membp));
//
        nclust+=1;
        eplane[i-1]=0.;// remove used bars
        eplane[i]=0.;
        eplane[i+1]=0.;
	xptr[i-1]=0; 
	xptr[i]=0; 
	xptr[i+1]=0; 
        i=1;// to start clust. search from the beginning
      }// ---> end of Ecl>Ethresh check
//------
    }// ---> end of peak check    
  }// ---> end of buffer (i) loop (clust.search)
  if(nclust>0)cllay[il-1]=1;
//------
  }// ---> end of TOF layers (il) loop
  if((cllay[0]+cllay[1])>0 && (cllay[2]+cllay[3])>0)stat=0;
// ( at least one cluster per top/bot TOF-subsystem)
}
//
//--------------------------------------------
//
void AMSTOFCluster::recovers2(AMSTrTrack * ptr){ // recreate TOFCluster
//  time through recovering(using tracker) of 1-sided TOFRawCluster-members
// (currently only "peak" bar is used)
  integer status,nm,mib,mil;
  AMSDir dir(0,0,1.);
  AMSPoint coo;
  AMSPoint outp;
  number theta,phi,sleng,ctr,clo,newt;
//
//  for(nm=0;nm<_nmemb;nm++){// loop over raw-cluster members
    nm=0;
    status=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getstatus();
    mil=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getntof()-1;
    mib=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->getplane()-1;
    if(nm==0){// first member is "peak", use it to find crossing point
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
        clo=-clo;// go to bar local r.s.
        ctr=outp[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
    }
      dynamic_cast<TOF2RawCluster*>(_mptr[nm])->recovers(clo);//missing side recovering
      newt=dynamic_cast<TOF2RawCluster*>(_mptr[nm])->gettime();//new time of raw cluster (ns)
      _time=-newt*1.e-9;//new time of cluster(-sec for Vitali)
      _etime=1.323*TFMCFFKEY.TimeSigma/sqrt(2.);//tempor (sec)(160ps, trecker recovered)
//  }
}
//-----------------------------------------------------------------------------

integer TOF2RawCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
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
  TOF2RawEvent *ptr;
  integer len(0),ncnt,nh,nzc;
  int16u idd,id,ilay,ibar,isid;
  int16u crate,rcrat,sfet,tofc,tdcc,hwid,fmt;
  integer nhits[TOF2GC::SCSFET]={0,0,0,0};
  integer nztdcc[TOF2GC::SCSFET]={0,0,0,0};
  integer ntoth,nonemp;
  ptr=(TOF2RawEvent*)AMSEvent::gethead()
                        ->getheadC("TOF2RawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
//
  ntoth=0;
  nonemp=0;
  while(ptr){
    idd=ptr->getid();// LBBS
    id=idd/10;
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    hwid=TOF2RawEvent::sw2hwid(ilay,ibar,isid);// CST
    tofc=hwid%10-1;
    sfet=(hwid/10)%10-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      nhits[sfet]+=ptr->gettoth();// counts hits (edges)
      nztdcc[sfet]+=ptr->getnztdc();// counts nonzero tdc-channels
      nonemp+=1;
    } 
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
    for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){
      nh=nhits[sfet];
      nzc=nztdcc[sfet];
      if(DAQS2Block::isSFETT2(rcrat,sfet)>0 && nonemp>0){//SFET contains also temperatures
        nh+=4;// 2 Temperature measurements (TDC-ch) with 2 hits(edges) each.(2*2=4)
        nzc+=2;                      
      }
      ncnt=nzc/4;//hit-counter words (4 counters each)
      if(nzc%4 > 0)ncnt+=1;
      len=len+ncnt+nh;// nmb.of_hits + nmb.of_counter_words
    }
    len=len+TOF2GC::SCSFET;// add TOF2GC::SCSFET words for hitmasks.
  }
//
  else{ // =====> Raw format :
    for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){
      nh=nhits[sfet];
      if(DAQS2Block::isSFETT2(rcrat,sfet)>0){//this SFET contains also temper.
        nh+=4;
      }
      ntoth+=nh;
    }
    len=2*ntoth;// (addr_word+tdc_word)*ntoth
  }
  return len;
//
}
//--------------------------------------------------------------------
void TOF2RawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_TOF_data (first word after block-id)
// on output: len=TOF_data_length .
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nztof,first(0);
  int16u ilay,ibar,isid,idd,id;
  int16u phbit,maxv,ntdc,tdc[16];
  geant charge,edep;
  integer trflag,trpatt[TOF2GC::SCLRS];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chip,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,sfet,tofc,tdcc,tdccm;
  TOF2RawEvent *ptr;
  TOF2RawEvent *ptlist[TOF2GC::SCSFET][TOF2GC::SCTOFC];
//
  phbit=TOF2GC::SCPHBP;//uniq phase-bit position used in Reduced_format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=int16u(TOF2GC::SCPHBPA);//uniq phase-bit position used in Raw_format in address word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]==2)cout<<"TOF::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
//
// ---> prepare ptlist[sfet][tofc] - list of TOFRawEvent pointers :
//
  ptr=(TOF2RawEvent*)AMSEvent::gethead()
                        ->getheadC("TOF2RawEvent",0);
  for(sfet=0;sfet<TOF2GC::SCSFET;sfet++)
              for(tofc=0;tofc<TOF2GC::SCTOFC;tofc++)
                           ptlist[sfet][tofc]=0;// clear pointer array
  nztof=0;//#of nonzero tof-tdcc per crate
  while(ptr){
    idd=ptr->getid();// LBBS
    id=idd/10;
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    hwid=TOF2RawEvent::sw2hwid(ilay,ibar,isid);
    tofc=hwid%10-1;
    sfet=(hwid/10)%10-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      ptlist[sfet][tofc]=ptr;// store pointer
      nztof+=1;
#ifdef __AMSDEBUG__
      if(TFREFFKEY.reprtf[1]==2)ptr->_printEl(cout);
#endif
    }
//
    ptr=ptr->next();
  }
//
  ic=0;
  if(fmt==1){ // =====> Reduced format :
//
//
// ---> encoding of tdc-data :
//
  for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){ // <--- SFETs loop (4)
    htmsk=0;
    ichm=ic;// bias for nonzero_tdc_channels mask
    ic+=1;
    nzch=0;// #of nonzero tdcc per sfet
    shft=0;
    for(tdcc=0;tdcc<TOF2GC::SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      mskb=1<<tdcc;
      tofc=tdcc/4;//TOFch=4TDCch (4 measurements for one side)
      mtyp=DAQS2Block::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ntdc=0;//edges
      if((tofc+1)==DAQS2Block::gettempch(rcrat,sfet) //these tdcc's(from given tofc) are temperatures
                              && nztof>0){// and there are non empty TOF-ch in this crate
        mtyp=5;// means temperature measurements
        ntdc=2;// 2 edges
        tdc[0]=273;// down-edge (273 degree)
        if(!TOF2DBc::pbonup())tdc[0]=tdc[0]|int16u(TOF2GC::SCPHBP);// add phase bit
        tdc[1]=293;// up-edge (293 degree)
        if(TOF2DBc::pbonup())tdc[1]=tdc[1]|int16u(TOF2GC::SCPHBP); // add phase bit
        if((tdcc-4*tofc)>1)ntdc=0;//only 2 first tdcc of tofc are occupied by temp.info
      }
      ptr=ptlist[sfet][tofc];// =0 for temper. (sw2hwid-function should provide that)
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);
        if(mtyp==2)ntdc=ptr->getstdc(tdc);
        if(mtyp==3)ntdc=ptr->getadch(tdc);
        if(mtyp==4)ntdc=ptr->getadcl(tdc);
      }
      if(ntdc>0){
        htmsk|=mskb;
        shft=nzch%4;// hit-counter position in the hit-counters word (0-3)
        if(shft==0){
          hitc=0;
          ichc=ic;// bias for hit-counter
          ic+=1;
        }
        for(i=0;i<ntdc;i++)*(p+ic++)=tdc[i];//fill TDC-hit words
        hitc|=(ntdc-1)<<(shft*4);
        if(shft==3){// it was last hit-counter in current hit-counters word
          *(p+ichc)=hitc;// add hit-counters word
          ichc=ic;// bias for next hit-counters word
        }
        nzch+=1;
      }
    } // end of TDC-channels loop
//
    *(p+ichm)=htmsk;// add sfet-channels mask word
    if((shft<3) && (nzch>0))*(p+ichc)=hitc;// add uncompleted hit-counters word
  } // end of SFET loop
//
  } // end of "Reduced" format
//---------------------------------
  else{ // =====> Raw format :
//
// ---> encoding of tdc-data :
//
  for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){ // <--- SFETs loop (4)
    for(tdcc=0;tdcc<TOF2GC::SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      chip=tdcc/8;//0-1
      chipc=tdcc%8;//chann.inside chip (0-7)
      chc=((chipc&1)<<2);//convert chipc to reverse bit pattern
      chc|=(chipc&2);
      chc|=((chipc&4)>>2);
      tofc=tdcc/4;//0-3
      mtyp=DAQS2Block::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ntdc=0;
      slad=DAQS2Block::sladdr(sfet);// slot-address (h/w address of sfet in crate)
      adrw=slad;// put slot-addr in address word (lsb)
      adrw|=(chc<<12);// add channel number chipc(inside of TDC-chip)
      if((tofc+1)==DAQS2Block::gettempch(rcrat,sfet)){  //these TDC-channels contain temperatures
        mtyp=5;// means temperature measurements
        ntdc=2;// 2 edges
        tdc[0]=273;// down-edge (273 degree)
        if(!TOF2DBc::pbonup())tdc[0]=(tdc[0]|int16u(TOF2GC::SCPHBP));// add phase bit
        tdc[1]=293;// up-edge (293 degree)
        if(TOF2DBc::pbonup())tdc[1]=(tdc[1]|int16u(TOF2GC::SCPHBP));// add phase bit
        if((tdcc-4*tofc)>1)ntdc=0;//only 2 first tdcc of tofc are occupied by temp.info
      }
      ptr=ptlist[sfet][tofc];
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);
        if(mtyp==2)ntdc=ptr->getstdc(tdc);
        if(mtyp==3)ntdc=ptr->getadch(tdc);
        if(mtyp==4)ntdc=ptr->getadcl(tdc);
      }
      if(ntdc>0){
        for(i=0;i<ntdc;i++){
          adr=adrw;
          if((tdc[i]&phbit)>0)adr|=phbtp;// add phase bit to address-word
          tdcw=tdc[i]&maxv;// put hit-value to tdc-word
          tdcw|=((1-chip)<<15);// add chip number [(1-chip) is due to Contin's error]
          *(p+ic++)=tdcw; // write hit-value
          *(p+ic++)=adr; // write hit-address
        }
      }
    } // end of SFET-channels loop
//
  } // end of SFET loop
//
  } // end of "Raw" format
  len=ic;
//
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]==2){
    cout<<"TOFDAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
    cout<<dec<<endl<<endl;
  }
#endif
}
//----------------------------------------------------------------------
// function to decode DAQ and build RawEvent: 
//  
void TOF2RawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
// on input:  *p=pointer_to_beginning_of_block (to block-id word)
//            len=bias_to_first_TOF_data_word(to next_to_block_id word)(=1)
// on output: len=pure TOF_data_length (block_id not included)
//
  integer i,j,ic,ilay,ibar,isid,lentot,bias;
  geant edep(0.),charge(0.);
  integer val,warnfl;
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hmsk,slad,slnu,chip,chipc,chc;
  int16u swid,mtyp,hcnt,shft,nhit,nzch,n4ch,sbit;
  int16u tdca,phbt,tofc;
  int16u phbit,maxv,phbtp; 
  int16u ids,stat,nftdc,nstdc,nadca,nadcd,ntemp,chan;
  int16u ftdc[TOF2GC::SCTHMX2*2],stdc[TOF2GC::SCTHMX3*4],adca[TOF2GC::SCTHMX4*2],adcd[TOF2GC::SCTHMX4*2],ttdc[16];
//
  phbit=TOF2GC::SCPHBP;//take uniq phase-bit position used in Reduced-format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=TOF2GC::SCPHBPA;//take uniq phase-bit position used in Raw-format for address-word
  lentot=*(p-1);// total length of the block(incl. block_id)
  bias=len;
//
// decoding of block-id :
//
  crate=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=1-(blid&63);// data_type ("0"->RawTDC; "1"->ReducedTDC)
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]>=1){
    cout<<"TOF::decoding: crate/format="<<crate<<" "<<dtyp<<"  Ltot/bias="<<lentot<<" "<<bias<<endl;
  }
#endif
//
  if(dtyp==1){ // =====> reduced TDC data decoding : 
//
    ic=bias;//jamp to first data word (bias=1 because TOF is first det. in block)
    for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){ // <-- SFET's loop (4)
      hmsk=*(p+ic++);
      nzch=0;
      if(hmsk>0){
        for(tdcc=0;tdcc<TOF2GC::SCTDCC;tdcc++){ // <-- TDC-chan. loop (16)
          tofc=tdcc/4;
          swid=TOF2RawEvent::hw2swid(crate, sfet, tofc);// LBBS
          mtyp=DAQS2Block::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
          if((tofc+1)==DAQS2Block::gettempch(crate,sfet)){ //these TDC-channels contain temperatures
            mtyp=5;// means temperature measurements
            ntemp=0;
          }
          if(mtyp<5 && tdcc%4==0){//new TOFch(4xTDCch) -> clear "edge"-counters 
            nftdc=0;
            nstdc=0;
            nadca=0;
            nadcd=0;
            n4ch=0;
          }
//
          if( (hmsk & (1<<tdcc)) > 0){
            shft=nzch%4;
            if(shft==0){// take new counters word
              hcnt=*(p+ic++);
            }
            nhit=((hcnt>>(4*shft))&15)+1;// numb.of hits for current mtyp
            for(i=0;i<nhit;i++){//    <-- Hit loop
              if(mtyp==1)ftdc[i]=*(p+ic++);
              if(mtyp==2)stdc[i]=*(p+ic++);
              if(mtyp==3)adca[i]=*(p+ic++);
              if(mtyp==4)adcd[i]=*(p+ic++);
              if(mtyp==5)ttdc[i]=*(p+ic++);
            } //                      --> end of hit loop
            if(mtyp==1)nftdc=nhit;
            if(mtyp==2)nstdc=nhit;
            if(mtyp==3)nadca=nhit;
            if(mtyp==4)nadcd=nhit;
            if(mtyp==5)ntemp=nhit;
            nzch+=1;//count !=0 TDC-ch inside of one SFET
            n4ch+=1;//count !=0 TDC-ch inside of one "4xTDC"="TOF-ch"
          }
//
          if(mtyp<5 && (tdcc+1)%4 == 0){ // complete TOF-chan.(4xTDCch) was scaned
            if(n4ch>0){// create TOF2RawEvent object for !=0 "4xTDC"
              n4ch=0;
              if(swid>0){
                ids=swid;// LBBS
                stat=0; // tempor ?
//--->
                if(AMSJob::gethead()->isRealData()){// if Real_data --> invert ph-bits for sTDC/dTDC
                  for(i=0;i<nstdc;i++){// for sTDC
                    if(stdc[i]&phbit)stdc[i]&=maxv;
                    else stdc[i]|=phbit;
                  }
                  for(i=0;i<nadcd;i++){// for dTDC
                    if(adcd[i]&phbit)adcd[i]&=maxv;
                    else adcd[i]|=phbit;
                  }
                }
//--->
                AMSEvent::gethead()->addnext(AMSID("TOF2RawEvent",0),
                new TOF2RawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                    nadca,adca,nadcd,adcd));
              }
              else{
                TOF2JobStat::addaq4(crate,dtyp);
#ifdef __AMSDEBUG__
                if(TFREFFKEY.reprtf[1]>=1){
                  cout<<"TOF:Reduced_Decoding_warning: crate/sfet/tofc are in  contradiction with swid-map"<<endl;
                  cout<<"                          crate="<<crate<<" sfet="<<sfet<<" tofch="<<tofc<<endl;
                }
#endif
              }
            }
          }// ---> end of complete TOF-channel processing
//
          if(mtyp==5 && ntemp>0){ // processing of one Temperature channel
            val=integer(ttdc[1]&maxv);// take first pair (last input pair) temp-hits
            val=val-integer(ttdc[0]&maxv);
            slnu=DAQS2Block::isSFETT2(crate,sfet);//seq.numbering of "temperature"-slots in 
//                                               crate (1,2,...; for now =1-> one temp-slot)
            if(slnu>0){
              chan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crate+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
              DAQS2Block::puttemp(chan,val);// store temperature
            } 
          }// ---> end of Temperature channel processing
//
        }// ---> end of TDC-chan. loop
      }//---> end of !=0 mask check
//
    }// ---> end of SFET's loop
//
    len=ic-len;//return pure TOF_data_length (bl_id not incl.)
  }// end of reduced data decoding
//
//---------------
//
  else if(dtyp==0){ // =====> raw TDC data decoding :
//
    integer lent(0);
    integer hwmx=TOF2GC::SCSFET*TOF2GC::SCTDCC;// max. h/w channels
    int16u hits[TOF2GC::SCSFET*TOF2GC::SCTDCC][16];// array to store hit-values(upto 16) for each h/w channel
    integer nhits[TOF2GC::SCSFET*TOF2GC::SCTDCC];// number of hits in given h/w channel
    int16u tdcw,adrw,hwch,hitv;
//
    for(i=0;i<hwmx;i++){// clear buffer
      nhits[i]=0;
      for(j=0;j<16;j++)hits[i][j]=0;
    }
//
    ic=bias;//jamp to first data word (bias=1 because TOF is first det. in block) 
    while(ic<lentot){ // <---  words loop
      tdcw=*(p+ic++);// chip# + tdc_value
      chip=tdcw>>15;// TDC-chip number(0-1)
      if(ic>=lentot){
        cout<<"TOF:RawFmt:read_error: attempt to read Extra-word ic="<<ic<<" blocklength="<<lentot<<endl;
        break;   
      }
      adrw=*(p+ic++);// phbit + chipc + slotaddr.
      slad=adrw&15;// get sfet h/w address ((0,1,2,3)-TOF, (5)-C, (4)-A)
      sfet=DAQS2Block::slnumb(slad);// sequential slot number (0-5, or =TOF2GC::DAQSSLT=6 if slad invalid)) 
      if(sfet==TOF2GC::DAQSSLT)continue; //---> invalid slad: try to take next pair
      if(DAQS2Block::isSFET(slad)){// <--- SFET data: store in buffer
        lent+=2;
        chipc=(adrw>>12)&7;// channel inside TDC-chip (0-7),reverse bit pattern!
        chc=((chipc&1)<<2);//convert chipc to normal bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        if((adrw & phbtp)>0)
                            phbt=1; // check/set phase-bit flag
        else
                            phbt=0;
        tdcc=8*(1-chip)+chc; // channel inside SFET(0-15) ((1-chip) is due to Contin's error) 
        hitv=(tdcw & maxv)|(phbt*phbit);// tdc-value with phase bit set as for RawEvent
        hwch=TOF2GC::SCTDCC*sfet+tdcc;//sequential tdc-ch numbering through all SFETs
        if(nhits[hwch]<16){
          warnfl=0;
          hits[hwch][nhits[hwch]]=hitv;
          nhits[hwch]+=1;
        }
        else{
          if(warnfl==0){
          cout<<"TOF:RawFmt:read_warning: more 16 hits in channel: crate="<<crate
          <<" sfet="<<sfet<<" chip="<<chip<<" chipch="<<chc<<endl;
          cout<<"event="<<(AMSEvent::gethead()->getid())<<endl;
          warnfl=1;
          }
        }
      }// ---> end of SFET data check
//
      else{
//        break;// hope no SFET data left 
        continue;// because of parallel readout
      }
//
    }// ---> end of words loop
//-----------------------------
// Now extract TOF+Temperature data from buffer:
//
    for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){ // <--- SFETs loop (4)
      for(tdcc=0;tdcc<TOF2GC::SCTDCC;tdcc++){// <--- TDC-channels loop (16)
        tofc=tdcc/4;
        hwch=TOF2GC::SCTDCC*sfet+tdcc;//sequential tdc-ch numbering through all SFETs
        swid=TOF2RawEvent::hw2swid(crate, sfet, tofc);// LBBS
        mtyp=DAQS2Block::mtyptof(tofc,(tdcc-4*tofc));//(1->ftdc,2->stdc,3->adca,4->adcd)
        if((tofc+1)==DAQS2Block::gettempch(crate,sfet)){//these TDC-channels contain temperatures
          mtyp=5;// means temperature measurements
          ntemp=0;
        }
        if(mtyp<5 && tdcc%4==0){//new TOFch(4xTDCch) -> clear "edge"-counters 
          nftdc=0;
          nstdc=0;
          nadca=0;
          nadcd=0;
          n4ch=0;
        }
//
        nhit=nhits[hwch];// numb.of hits (<=16) for current mtyp
        if(nhit>0){// write h/w-channel hits to measurement-type buffer
//   cout<<"fr_buf:hwch="<<hwch<<" sfet/tofc="<<sfet<<" "<<tofc<<" tdcc="<<tdcc<<endl;
//   cout<<"     mtyp="<<mtyp<<endl;
          for(i=0;i<nhit;i++){//    <-- Hit loop
//   cout<<"     hitn/hitv="<<i<<" "<<hits[hwch][i]<<endl;
            if(mtyp==1)ftdc[i]=hits[hwch][i];
            if(mtyp==2)stdc[i]=hits[hwch][i];
            if(mtyp==3)adca[i]=hits[hwch][i];
            if(mtyp==4)adcd[i]=hits[hwch][i];
            if(mtyp==5)ttdc[i]=hits[hwch][i];
          } //                      --> end of hit loop
          if(mtyp==1)nftdc=nhit;
          if(mtyp==2)nstdc=nhit;
          if(mtyp==3)nadca=nhit;
          if(mtyp==4)nadcd=nhit;
          if(mtyp==5)ntemp=nhit;
          n4ch+=1;//count !=0 TDC-ch inside of one "4xTDC"="TOF-ch"
        }
//
        if(mtyp<5 && (tdcc+1)%4 == 0){ // last TDC-chan of current TOF-chan.(4xTDCch)
          if(n4ch>0){// create TOF2RawEvent object for !=0 "4xTDC"
            n4ch=0;
            if(swid>0){
              ids=swid;// LBBS
              stat=0; // tempor ?
//--->
              if(AMSJob::gethead()->isRealData()){// if Real_data --> invert ph-bits for sTDC/dTDC
                for(i=0;i<nstdc;i++){// for sTDC
                  if(stdc[i]&phbit)stdc[i]&=maxv;
                  else stdc[i]|=phbit;
                }
                for(i=0;i<nadcd;i++){// for dTDC
                  if(adcd[i]&phbit)adcd[i]&=maxv;
                  else adcd[i]|=phbit;
                }
              }
//--->
              AMSEvent::gethead()->addnext(AMSID("TOF2RawEvent",0),
              new TOF2RawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                    nadca,adca,nadcd,adcd));// create object
            }
            else{
              TOF2JobStat::addaq4(crate,dtyp);
#ifdef __AMSDEBUG__
              if(TFREFFKEY.reprtf[1]>=1){
                cout<<"TOF2:Raw_Decoding_warning: crate/sfet/tofc are in  contradiction with swid-map"<<endl;
                cout<<"                          crate="<<crate<<" sfet="<<sfet<<" tofch="<<tofc<<endl;
              }
#endif
            }
          }
        }// ---> end of complete TOF-channel processing
//------------
        if(mtyp==5 && ntemp>0){ // processing of one Temperature-channel
          val=integer(ttdc[1]&maxv);// take first pair (last input pair) temp-hits
          val=val-integer(ttdc[0]&maxv);
          slnu=DAQS2Block::isSFETT2(crate,sfet);//seq.numbering of "temperature"-slots in
//                                               crate (1,2,...; for now =1-> one temp-slot)
          if(slnu>0){
            chan=TOF2GC::DAQSTSC*TOF2GC::DAQSTCS*crate+TOF2GC::DAQSTCS*(slnu-1)+(tdcc%4);
            DAQS2Block::puttemp(chan,val);// store temperature
          } 
        }// ---> end of Temperature channel processing
//------------
      }//---> end of tdc-ch loop in SFET
    }//---> end of SFETs loop
//
  len=lent;
  }// ---> end of Raw format decoding
//------------
  else{
    cout<<"Tof2DaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
#ifdef __AMSDEBUG__
  if(TFREFFKEY.reprtf[1]>=1){
    cout<<"TOF2RawEventBuild::decoding: FOUND "<<len<<" good words, hex dump follows:"<<endl;
    int16u tstw,tstb;
    if(dtyp==1){// only for red.data
      for(i=0;i<len;i++){
        tstw=*(p+i+bias);
        cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
        for(j=0;j<16;j++){
          tstb=(1<<(15-j));
          if((tstw&tstb)!=0)cout<<"x"<<"|";
          else cout<<" "<<"|";
        }
        cout<<endl;
      }
      cout<<dec<<endl<<endl;
    }
  }
#endif
}
//-------------------------------------------------------------------------------
//  function to get sofrware-id (LBBS) for given hardware-chan (crate,sfet,tofch):
//                                                               0-7   0-3  0-3
int16u TOF2RawEvent::hw2swid(int16u a1, int16u a2, int16u a3){
  int16u swid,hwch;
//
  static int16u sidlst[TOF2GC::SCCRAT*TOF2GC::SCSFET*TOF2GC::SCTOFC]={// 14 LBBS's + 2 empty  per CRATE :
// mycrate-1(node-0,cr.01), (4 SFETs)x(4 TOFCs) :
  2112,2122,2132,2142, 1081,1091,1101,1111, 2082,2092,2102,   0, 1121,1141,4121,4141,
//
// mycrate-2(node-1,cr.31), (4 SFETs)x(4 TOFCs) :
  2042,2052,2062,2072, 1082,1092,1102,1112, 2012,2022,2032,   0, 4122,4132,1122,1132,
//
// mycrate-3(node-2,cr.41), (4 SFETs)x(4 TOFCs) :
  2041,2051,2061,2071, 1042,1052,1062,1072, 2011,2021,2031,   0, 1032,1012,4032,4012,
//
// mycrate-4(node-3,cr.71), (4 SFETs)x(4 TOFCs) :
  2111,2121,2131,2141, 1041,1051,1061,1071, 2081,2091,2101,   0, 4031,4021,1031,1021,
//
// mycrate-5(node-4,cr.03), (3 SFETs)x(4 TOFCs) :
  3112,3122,3132,3142, 3082,3092,3102,   0, 4081,4091,4101,4111,    0,   0,   0,   0,
//
// mycrate-6(node-5,cr.33), (3 SFETs)x(4 TOFCs) :
  3042,3052,3062,3072, 3012,3022,3032,   0, 4082,4092,4102,4112,    0,   0,   0,   0,
//
// mycrate-7(node-6,cr.43), (3 SFETs)x(4 TOFCs) :
  3041,3051,3061,3071, 3011,3021,3031,   0, 4042,4052,4062,4072,    0,   0,   0,   0,
//
// mycrate-8(node-7,cr.73), (3 SFETs)x(4 TOFCs) :
  3111,3121,3131,3141, 3081,3091,3101,   0, 4041,4051,4061,4071,    0,   0,   0,   0
  };
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<TOF2GC::SCCRAT);//crate(node)(0-7)
  assert(a2>=0 && a2<TOF2GC::SCSFET);//sfet(0-3)
  assert(a3>=0 && a3<TOF2GC::SCTOFC);//tofch(0-3)
#endif
  hwch=int16u(TOF2GC::SCTOFC*TOF2GC::SCSFET*a1+TOF2GC::SCTOFC*a2+a3);// hardware-channel
  swid=sidlst[hwch]; // software-id LBBS
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;
  return swid;
}
//------
//  function to get hardware-id (CST) for given software-chan (layer, bar, side)
//                                                              0-3   0-13  0-1
//
int16u TOF2RawEvent::sw2hwid(int16u a1, int16u a2, int16u a3){
  static int16u first(0);
  integer i;
  int16u ilay,ibar,isid,swch,swid,hwid,crate,sfet,tofc;
  static int16u hidlst[TOF2GC::SCLRS*TOF2GC::SCMXBR*2]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<TOF2GC::SCLRS);// layer(0-3)
  assert(a2>=0 && a2<TOF2GC::SCMXBR);// bar(0-13)
  assert(a3>=0 && a3<2); // side(0-1)
#endif
//
  if(first==0){ // create hardw.id list:
    first=1;
    for(i=0;i<TOF2GC::SCLRS*TOF2GC::SCMXBR*2;i++)hidlst[i]=0;
    for(crate=0;crate<TOF2GC::SCCRAT;crate++){
      for(sfet=0;sfet<TOF2GC::SCSFET;sfet++){
        for(tofc=0;tofc<TOF2GC::SCTOFC;tofc++){
          hwid=100*(crate+1)+10*(sfet+1)+tofc+1;// CrateSfetTofch (CST)
          swid=TOF2RawEvent::hw2swid(crate,sfet,tofc);// LBBS
          if(swid>0 && swid<=4142){// legal swid 
            ilay=swid/1000-1;
            ibar=(swid/10)%100-1;
            isid=swid%10-1;
            swch=2*TOF2GC::SCMXBR*ilay+2*ibar+isid;
            hidlst[swch]=hwid;
          }
        }// end of tofch-loop
      }// end of sfet-loop
    }// end of crate-loop
  }// end of first
// 
  swch=2*TOF2GC::SCMXBR*a1+2*a2+a3;
  hwid=hidlst[swch];// hardware-id CST
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;
  return hwid;
}



AMSID AMSTOFCluster::crgid(int kk){
return AMSID("TOFS",_ntof*100+_plane);
}
