// last modif. 10.12.96 by E.Choumilov - AMSTOFRawCluster::build added, 
//                                       AMSTOFCluster::build rewritten
//              16.06.97   E.Choumilov - AMSTOFRawEvent::validate added
//                                       AMSTOFRawCluster::sitofdigi modified for trigg.
//              26.06.97   E.Choumilov - DAQ decoding/encoding added
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <trrec.h>
#include <tofrec.h>
#include <antirec.h>
#include <particle.h>
#include <daqblock.h>
#include <tofcalib.h>
#include <ntuple.h>
#include <time.h>
//
void AMSTOFCluster::init(){
}
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// calibration data
extern TOFVarp tofvpar;// TOF general parameters
integer AMSTOFRawCluster::trpatt[SCLRS]={0,0,0,0};//just init. of static var.
integer AMSTOFRawCluster::trflag=0;
uinteger AMSTOFRawEvent::StartRun(0);
time_t AMSTOFRawEvent::StartTime(0);
//
//
//-----------------------------------------------------------------------
void AMSTOFRawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u nftdc[2],nstdc[2],nadca[2],nadcd[2];
  int16u  ftdc1[SCTHMX2*2],stdc1[SCTHMX3*4],adca1[SCTHMX4*2],adcd1[SCTHMX4*2];
  int16u  ftdc2[SCTHMX2*2],stdc2[SCTHMX3*4],adca2[SCTHMX4*2],adcd2[SCTHMX4*2];
  integer ilay,last,ibar,isid,isds;
  integer i,j,im,nhit,chnum,brnum,am[2],tmi[2],itmf[2],sta,st;
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idd,idN,stat[2],idr;
  number tsr[3],ths[2],fstd,t1,t2,t3,t4,dt;
  geant mtma[2],mtmd[2];
  int bad(1);
  static int first(0);
  geant charge,edep;
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptrN;
//
  pbitn=SCPHBP;
  pbanti=pbitn-1;
  status=1;//bad
  idr=TOFRECFFKEY.reprtf[4];// chan-id for histogramming of stretcher w1/w3
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0,1);//last 1 to sort
  isds=0;
//
#ifdef __AMSDEBUG__
  if(TOFRECFFKEY.reprtf[1]>=1)
  cout<<endl<<"======> TOF::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//
  if(first==0){//store run/time for the first event
    first=1;
    StartRun=AMSEvent::gethead()->getrun();
    StartTime=AMSEvent::gethead()->gettime();
  }
//---- Scint.data length monitoring:
  if(TOFRECFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    im=DAQSBlock::gettbll();//total blocks length for current format
//    im=0;
//    for(i=0;i<8;i++)im+=DAQSBlock::calcblocklength(i);
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
    assert(ilay>=0 && ilay<SCLRS);
    assert(ibar>=0 && ibar<SCMXBR);
    assert(isid>=0 && isid<2);
    if(TOFRECFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    chnum=ilay*SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
    
    edep=ptr->getedep();//tempor
    charge=ptr->getcharg();
    
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      nftdc[isid]=ptr->getftdc(ftdc1);
      nstdc[isid]=ptr->getstdc(stdc1);
      nadca[isid]=ptr->getadca(adca1);
      nadcd[isid]=ptr->getadcd(adcd1);
      TOFJobStat::addch(chnum,11);
//---> check hist-TDC :
      nhit=0;
      im=nftdc[isid];
      for(i=0;i<im;i++)ftdc2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(ftdc1[i]&pbitn);//check p-bit of down-edge (come first, LIFO mode !!!)
        pbup=(ftdc1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOFDBc::pbonup()==1){
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
        TOFJobStat::addch(chnum,12);
      }
      if(TOFRECFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im),1.);
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
        if(TOFDBc::pbonup()==1){
          if(pbup==0||pbup1==0||pbdn!=0||pbdn1!=0)continue;//wrong sequence, take next "4" 
        }
        else{
          if(pbup!=0||pbup1!=0||pbdn==0||pbdn1==0)continue;//wrong  sequence, take next "4" 
        }
// -> check timing of 4 edges:
        t1=(stdc1[i+3]&pbanti)*TOFDBc::tdcbin(1);//1-st up-edge (in real time)
        t2=(stdc1[i+2]&pbanti)*TOFDBc::tdcbin(1);//1-st down-edge
        t3=(stdc1[i+1]&pbanti)*TOFDBc::tdcbin(1);//2-nd up-edge
        t4=(stdc1[i]&pbanti)*TOFDBc::tdcbin(1);//2-nd down-edge
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
        TOFJobStat::addch(chnum,13);
      }
      if(nhit>0)bad=0;//at least one slow_TDC hit -> good tof-event
      if(TOFRECFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+20),1.);
//
//---> check A-TDC :
      nhit=0;
      im=nadca[isid];
      for(i=0;i<im;i++)adca2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(adca1[i]&pbitn);//check p-bit of down-edge (come first)
        pbup=(adca1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOFDBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        adca2[nhit]=adca1[i];
        nhit+=1;
        adca2[nhit]=adca1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good edges
      }
      if(nhit<im){//something was wrong
        ptr->putadca(int16u(nhit),adca2);// refill object with corrected data
        TOFJobStat::addch(chnum,14);
      }
      if(TOFRECFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+40),1.);
//
//---> check D-TDC :
      nhit=0;
      im=nadcd[isid];
      for(i=0;i<im;i++)adcd2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(adcd1[i]&pbitn);//check p-bit of down-edge (come first)
        pbup=(adcd1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOFDBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        adcd2[nhit]=adcd1[i];
        nhit+=1;
        adcd2[nhit]=adcd1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good edges
      }
      if(nhit<im){//something was wrong
        ptr->putadcd(int16u(nhit),adcd2);// refill object with corrected data
        TOFJobStat::addch(chnum,15);
      }
      if(TOFRECFFKEY.reprtf[3]>0)HF1(1300+chnum,geant(im+60),1.);
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
    ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);

    while(ptr){ //  <---- loop over TOF RawEvent hits -----
      int num,den,nedges[4];
      number Atovt,Dtovt;
      int his[SCTHMX2*2],tdc[SCTHMX3*4],ano[SCTHMX4*2],dyn[SCTHMX4*2];     
      idd=ptr->getid();
      id=idd/10;// short id=LBB, where L=1,4 BB=1,14
      ilay=id/100;
      ibar=id%100;
      isid=idd%10;
 
      nedges[0]=int (ptr->getftdc(ftdc1)); // History
      VZERO(stdc1,SCTHMX3*4/2);
      nedges[1]=int (ptr->getstdc(stdc1)); // Time Exp.
      nedges[2]=int (ptr->getadca(adca1)); // Anode
      nedges[3]=int (ptr->getadcd(adcd1)); // Dynode

//
//      Eventually choose the right 2 edges for Anode and Dynode,
//      before calculating Atovt and Dtovt
      Atovt=number((adca1[1]-adca1[0])&pbanti);
      Dtovt=number((adcd1[1]-adcd1[0])&pbanti);

      tsr[0]=(stdc1[3]&pbanti)*TOFDBc::tdcbin(1);
      tsr[1]=(stdc1[2]&pbanti)*TOFDBc::tdcbin(1);
      tsr[2]=(stdc1[0]&pbanti)*TOFDBc::tdcbin(1);
      num=tsr[1]-tsr[2]; // Time Exp. =
      den=tsr[0]-tsr[1]; //    (num - offset)/den
      AMSTOFRawEvent::tofonlinefill1(ilay,ibar,isid, // coord. 1-4,1-14,1-2
		     nedges,         // array of numbers of edges 
		     num,den,        // ~ time stretcher ratio
		     Atovt,Dtovt);   // Anode & Dynode time over thr.
      ptr=ptr->next();// take next RawEvent hit
    }
  }
//
// =============> STRR/AvsD(Contin's) calibration, if requested :
//
  if(TOFRECFFKEY.relogic[0]==1 && status == 0){
    TOFJobStat::addre(16);
//
    ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);
    isds=0;
//                           
    while(ptr){ //  <---- loop over TOF RawEvent hits -----
      idd=ptr->getid();
      id=idd/10;// short id=LBB, where L=1,4 BB=1,14
      ilay=id/100-1;
      ibar=id%100-1;
      isid=idd%10-1;
      chnum=ilay*SCMXBR*2+ibar*2+isid;//channels numbering
      stat[isid]=ptr->getstat();
      if(stat[isid] == 0){  
        isds+=1;
//
// --> fill for STRR-calibration :
//
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadca(adca1);
        nadcd[isid]=ptr->getadcd(adcd1);
        if(nstdc[isid]==4){ // require only one "4-edge" sTDC-hit
          tsr[0]=number((stdc1[3]&pbanti)*TOFDBc::tdcbin(1));
          tsr[1]=number((stdc1[2]&pbanti)*TOFDBc::tdcbin(1));
          tsr[2]=number((stdc1[0]&pbanti)*TOFDBc::tdcbin(1));
          TOFSTRRcalib::fill(chnum,tsr);
          if(nftdc[isid]==2 && nadca[isid]==2){ // require only one "2-edge" f/a-TDC-hit
            ths[0]=(ftdc1[1]&pbanti)*TOFDBc::tdcbin(0);
            ths[1]=(ftdc1[0]&pbanti)*TOFDBc::tdcbin(0);
            fstd=ths[0]-tsr[0];
            TOFSTRRcalib::fill2(chnum,fstd);
          }
        }
//
// --> fill for AVSD-calibration :
//
        mtmd[0]=0.;
        mtmd[1]=0.;
        if(nadca[isid]==2 && (nadcd[isid]==0 || nadcd[isid]==2)){ // accept "2-edge" aTDC-hit
          mtma[0]=(adca1[1]&pbanti)*TOFDBc::tdcbin(2);// 2-nd hit is front_edge 
          mtma[1]=(adca1[0]&pbanti)*TOFDBc::tdcbin(2);// 1-st hit is back_edge (in real time)
          if(nadcd[isid]==2){
            mtmd[0]=(adcd1[1]&pbanti)*TOFDBc::tdcbin(3);
            mtmd[1]=(adcd1[0]&pbanti)*TOFDBc::tdcbin(3);
          }
          TOFAVSDcalib::filltovt(chnum,mtma,mtmd);
        }
      }// --- end of status check --->
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawEvent hits loop ------->
//
    status=1;// set to "bad" to avoid further reconstruction
  }
}
//-----------------------------------------------------------------------
void AMSTOFRawCluster::sitofdigi(int &status){
  AMSTOFMCCluster * ptr=(AMSTOFMCCluster*)
  AMSEvent::gethead()->
   getheadC("AMSTOFMCCluster",0,1); // last 1  to test sorted container
  static number xplane[4][SCMXBR];
  static number xtime[4][SCMXBR];
  static number xtimed[4][SCMXBR];
  static number xz[4][SCMXBR];
  integer plrot;
  number ama[2],amd[2],stm[2],edepd,etd;
  VZERO(xtime,SCMXBR*sizeof(number));
  VZERO(xtimed,SCMXBR*sizeof(number));
  VZERO(xplane,SCMXBR*sizeof(number));
  VZERO(xz,SCMXBR*sizeof(number));
  geant x,y;
  status=1;// bad
  while(ptr){
   integer ntof,plane,status;
   ntof=(ptr->idsoft)/100-1;//ilay
   plane=(ptr->idsoft)%100-1;//ibar
#ifdef __AMSDEBUG__
   assert(plane>=0 && plane< SCMXBR);
   assert (ntof>=0 && ntof<4);
#endif
   xplane[ntof][plane]+=ptr->edep*1000;
   xtime[ntof][plane]+=ptr->tof*(ptr->edep)*1000;//tof*edep
   plrot=TOFDBc::plrotm(ntof);     // =0/1-unrotated/rotated TOF-plane
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
  integer ntrl(0),trpatt[SCLRS]={0,0,0,0};
  integer ntrl3(0),trpatt3[SCLRS]={0,0,0,0};
  integer trflag(0);
  integer bitp,lsbit(1);
  int i,statdb[2];
  geant halfl;
  number enshar,ylon,edp[2];
  number ts1,ts2;
  for(integer kk=0;kk<SCLRS;kk++){
  for(i=0;i<SCMXBR;i++){
    if(xplane[kk][i]>TOFMCFFKEY.Thr){
     xtime[kk][i]=xtime[kk][i]/xplane[kk][i];
     xtimed[kk][i]=xtimed[kk][i]/xplane[kk][i];
     ylon=xtimed[kk][i];// long.coord. in abs. system
// ---> primitive simul.of level-1 trig.pattern (only for Choutko games):
     halfl=0.5*TOFDBc::brlen(kk,i);
     enshar=exp(-(ylon+halfl)/latt)/(exp(-(ylon+halfl)/latt)+exp(-(halfl-ylon)/latt));
     edp[0]=enshar*xplane[kk][i]*exp(-(ylon+halfl)/latt);
     edp[1]=xplane[kk][i]-edp[0];
     scbrcal[kk][i].getbstat(statdb); // "alive" status from DB
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
     if(RNDM(dummy)< TOFMCFFKEY.TimeProbability2){
       ts1=TOFMCFFKEY.TimeSigma2*rnormx();
       ts2=TOFMCFFKEY.TimeSigma2*rnormx();
       etd=TOFMCFFKEY.TimeSigma2*c*0.707;//err. on long.coord.
     }
     else {
       ts1=TOFMCFFKEY.TimeSigma*rnormx();
       ts2=TOFMCFFKEY.TimeSigma*rnormx();
       etd=TOFMCFFKEY.TimeSigma*c*0.707;
     }
     xtime[kk][i]+=(ts1+ts2)/2.;// add  time-error
     xtimed[kk][i]+=(ts1-ts2)/2.*c; // add  coordinate-error
//
     plrot=TOFDBc::plrotm(kk);
     if(plrot==1)xtimed[kk][i]=-xtimed[kk][i];//to agree with slow algorithm
     xtime[kk][i]=-(1.e9)*xtime[kk][i];//sec->-ns to agree with slow algorithm
     ama[0]=0.;
     ama[1]=0.;
     amd[0]=0.;
     amd[1]=0.;
     stm[0]=0.;
     stm[1]=0.;
     edepd=0.;
     AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0),
     new AMSTOFRawCluster(0,kk+1,i+1,xz[kk][i],ama,amd,xplane[kk][i],
         edepd,stm,xtime[kk][i],xtimed[kk][i],etd));
    }

  }
  }
  for(i=0;i<SCLRS;i++)if(trpatt[i]>0)ntrl+=1;// counts triggered (z>=1) layers
  for(i=0;i<SCLRS;i++)if(trpatt3[i]>0)ntrl3+=1;// counts triggered (z>2) layers
  if(TOFMCFFKEY.trlogic[1]==1){// h/w requirement ALL4
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
  AMSTOFRawCluster::settrfl(trflag);// set trig.flag 
  AMSTOFRawCluster::setpatt(trpatt);//tempor: add trigger(z>=1) pattern 
}
//----------------------------------------------------------------------------
void AMSTOFRawCluster::build(int &ostatus){
  int16u nftdc[2]={0,0};
  int16u nstdc[2]={0,0};
  int16u nadca[2]={0,0};
  int16u nadcd[2]={0,0};
  int16u  ftdc1[SCTHMX2*2],stdc1[SCTHMX3*4],adca1[SCTHMX4*2],adcd1[SCTHMX4*2];
  int16u  ftdc2[SCTHMX2*2],stdc2[SCTHMX3*4],adca2[SCTHMX4*2],adcd2[SCTHMX4*2];
  integer ilay,last,ibar,isid,isds,isd,isdsl[SCLRS],hwid,tchan,crat,sfet,slnu,tdcc;
  integer i,j,k,chnum,brnum,am[2],tmi[2],itmf[2],sta,st,smty[2],ftdcfl,reject;
  integer trpatt[SCLRS];
  uinteger Runum(0);
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  int16u id,idd,idN,stat[2];
  number zc,ama[2],amd[2],qtota,qtotd,tmf[2],time,coo,ecoo;//input to RawCluster Constr
  number tm[2],tf,tff,dt,fstd,tmr[2];
  number amf[2],timeD,tamp;
  number treads[2]={0.,0.};
  number treada[2]={0.,0.};
  number charg[2]={0.,0.};
  number t1,t2,t3,t4;
  geant blen,co,eco,point,brlm,pcorr,td2p,etd2p,clong[SCLRS];
  geant strr[2],srof[2],strat[2][2];
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptrN;
  integer nbrl[SCLRS],brnl[SCLRS];
  int bad,tsfl(0),rml4s2;
// some variables for histogramming:
  geant gdt,tch,pch1[SCLRS],pch2[SCLRS];
  geant edepa[SCLRS],edepd[SCLRS],tcorr[SCLRS],elosn;
  geant tuncorr[SCLRS],tdiff[SCLRS],td13,td24,td14;
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                                    ->getheadC("AMSTOFRawEvent",0);
  Runum=AMSEvent::gethead()->getrun();// current run number
  pbitn=SCPHBP;//phase bit position
  pbanti=pbitn-1;// mask to avoid it.
  ostatus=1;// event output status(init. as bad)
  isds=0;
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
  rml4s2=0;
  if(TOFRECFFKEY.relogic[2]!=0)rml4s2=1;
  if(AMSJob::gethead()->isRealData()!=0 && Runum>896888674)rml4s2=1;
//
//    cout<<"TOF_Crate Temperatures :"<<endl;
//    for(k=0;k<8;k++){ 
//      for(i=0;i<DAQSTSC;i++){//loop over temp. SFETs in crate (1)
//        for(j=0;j<DAQSTCS;j++){//loop over temp. channels in SFET (4)
//          tchan=DAQSTCS*DAQSTSC*k+i*DAQSTCS+j;
//          cout<<" "<<DAQSBlock::gettemp(tchan);
//        }
//        cout<<endl;
//      }
//      cout<<endl;
//    }
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
    idd=ptr->getid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,14
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
//    edep=ptr->getedep();
    chnum=ilay*SCMXBR*2+ibar*2+isid;//channels numbering
    brnum=ilay*SCMXBR+ibar;//bar numbering
    stat[isid]=ptr->getstat();
//    if(stat[isid] == 0){  
    scbrcal[ilay][ibar].getbstat(statdb); // "alive" status from DB
    if(statdb[isid] >= 0){  // channel alive(no severe problems), read out it
      hwid=AMSTOFRawEvent::sw2hwid(ilay,ibar,isid);
      crat=hwid/100-1;
      slnu=1;//sequential number of slot with temp. (only 1 exists) 
//       fill working arrays for given side:
      isds+=1;
      if(isid==0){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadca(adca1);
        nadcd[isid]=ptr->getadcd(adcd1);
        tdcc=12;//temper. reading for anode
        tchan=DAQSTSC*DAQSTCS*crat+DAQSTCS*(slnu-1)+(tdcc%4);
        treada[isid]=DAQSBlock::gettemp(tchan);// get "temperature" for anode chain
        tdcc=13;//temper. reading for stretcher
        tchan=DAQSTSC*DAQSTCS*crat+DAQSTCS*(slnu-1)+(tdcc%4);
        treads[isid]=DAQSBlock::gettemp(tchan);// get "temperature" for stretcher chain
      } 
      if(isid==1){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc2);
        nstdc[isid]=ptr->getstdc(stdc2);
        nadca[isid]=ptr->getadca(adca2);
        nadcd[isid]=ptr->getadcd(adcd2);
        tdcc=12;//temper. reading for anode
        tchan=DAQSTSC*DAQSTCS*crat+DAQSTCS*(slnu-1)+(tdcc%4);
        treada[isid]=DAQSBlock::gettemp(tchan);// get "temperature" for anode chain
        tdcc=13;//temper. reading for stretcher
        tchan=DAQSTSC*DAQSTCS*crat+DAQSTCS*(slnu-1)+(tdcc%4);
        treads[isid]=DAQSBlock::gettemp(tchan);// get "temperature" for stretcher chain
      }
      TOFJobStat::addch(chnum,0);        // channel statistics :
      if(nftdc[isid]>0)TOFJobStat::addch(chnum,1);
      if(nstdc[isid]>0)TOFJobStat::addch(chnum,2);
      if(nadca[isid]>0)TOFJobStat::addch(chnum,3);
      if(nadcd[isid]>0)TOFJobStat::addch(chnum,4);
      if(nftdc[isid]==2)TOFJobStat::addch(chnum,5);
      if(nstdc[isid]==4)TOFJobStat::addch(chnum,6);
      if(nadca[isid]==2)TOFJobStat::addch(chnum,7);
      if(nadcd[isid]==2)TOFJobStat::addch(chnum,8);
      if(nftdc[isid]>=2 && nstdc[isid]>=4 && nadca[isid]>=2)TOFJobStat::addch(chnum,9);
      if(TOFRECFFKEY.reprtf[2]>1){
        HF1(1120+crat,geant(treads[isid]),1.);
        HF1(1130+crat,geant(treada[isid]),1.);
      }
    } 
//
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (LBB)
//------------------------------------------------------
    if(idN != id){ // both sides ready, next hit is OTHER_counter/last hit,
//       so process CURRENT counter data : 
//---
      if(isds==2 || isds==1){ // sides presence check
        TOFJobStat::addbr(brnum,0);//h/w status ok(at least one alive(in DB) side is present )
//
// =============> start pattern recognition for given sc.bar :
//
// --------> select bar with >= 1x4-multiplicity in stdc, >= 1x2 in ftdc/adc 
//(first pulse from readout (last in real time) will be used if many stdc/adc are present): 
//
        smty[0]=0;
        smty[1]=0;
        ftdcfl=TOFRECFFKEY.relogic[1];// how to use f-TDC 
        if(nstdc[0]>=4 && nadca[0]>=2 && (nftdc[0] >=2 || ftdcfl==2))smty[0]=1;
        if(nstdc[1]>=4 && nadca[1]>=2 && (nftdc[1] >=2 || ftdcfl==2))smty[1]=1;
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
//----> force suppression of L4S2 for Real data Run>896888674 or by flag
        if(rml4s2){
          if((ilay==3) && (ibar>1) && (ibar<12))smty[1]=0;
        }
//---->
        if(smty[0]==1 || smty[1]==1){ //1-side bar is accepted,if have complete measurements) 
          TOFJobStat::addbr(brnum,1);
          isds=smty[0]+smty[1];// number of  good sides
          sta=0;
// -> add status-bits for known problems:
          if(((statdb[0]%100)/10 > 0) || ((statdb[1]%100)/10 > 0))
                               sta|=SCBADB3;// set bit of known bad t-measurement on any side
//
          if(isds==1){
            sta|=SCBADB2;// set bit for counter with only one-side measurements
            if(smty[1]==0)sta|=SCBADB4;// set bit for side number(s1->reset_bit,s2->set_bit)
          }
          scbrcal[ilay][ibar].gtstrat(strat);
          strr[0]=strat[0][0];// strr.for s-1
          strr[1]=strat[1][0];// .........s-2
          srof[0]=strat[0][1];// offs.for s-1
          srof[1]=strat[1][1];// .........s-2
//
//--------------> identify "corresponding"(to sTDC) hit in fast TDC :
//
// --> calculate s-TDC "start"-edge (each-side) times tm[2] (wrt lev1):
          if(smty[0]>0)tm[0]=number((stdc1[3]&pbanti)*TOFDBc::tdcbin(1));//last(in real time) s-tdc hit
          else tm[0]=0; 
          if(smty[1]>0)tm[1]=number((stdc2[3]&pbanti)*TOFDBc::tdcbin(1));
          else tm[1]=0;
// 
// --> histogr. for single history/slow/a/d-hit bars :
//
          if(TOFRECFFKEY.reprtf[2]>0){
            if(smty[0]>0 && nftdc[0]==2 && nstdc[0]==4){
              tff=number((ftdc1[1]&pbanti)*TOFDBc::tdcbin(0));//2-nd hit is leading(up)-edge
              dt=tff-tm[0];
              HF1(1100,geant(dt),1.);//hist. the same hit f/s-TDC difference
              dt=(int(ftdc1[1]&pbanti)-int(ftdc1[0]&pbanti))
                                         *TOFDBc::tdcbin(0);//f-TDC inp.pulse width
              HF1(1103,geant(dt),1.);
              if(nadca[0]==2){// check anode-stdc correlations
                tf=number((adca1[1]&pbanti)*TOFDBc::tdcbin(2));//2-nd hit is leading(up)-edge
                dt=tf-tm[0];
                HF1(1105,geant(dt),1.);//hist. the same hit a/s-TDC difference
              }
              if(nadcd[0]==2){// check dynode-stdc correlations
                tf=number((adcd1[1]&pbanti)*TOFDBc::tdcbin(3));//2-nd hit is leading(up)-edge
                dt=tf-tm[0];
                HF1(1106,geant(dt),1.);//hist. the same hit d/s-TDC difference
              }
            }
            if(smty[1]>0 && nftdc[1]==2 && nstdc[1]==4){
              tff=number((ftdc2[1]&pbanti)*TOFDBc::tdcbin(0));
              dt=tff-tm[1];
              HF1(1100,geant(dt),1.);
              dt=(int(ftdc2[1]&pbanti)-int(ftdc2[0]&pbanti))
                                         *TOFDBc::tdcbin(0);//f-TDC inp.pulse width
              HF1(1103,geant(dt),1.);
              if(nadca[1]==2){
                tf=number((adca2[1]&pbanti)*TOFDBc::tdcbin(2));//2-nd hit is leading(up)-edge
                dt=tf-tm[1];
                HF1(1105,geant(dt),1.);//hist. the same hit a/s-TDC difference
              }
              if(nadcd[1]==2){// check dynode-stdc correlations
                tf=number((adcd2[1]&pbanti)*TOFDBc::tdcbin(3));//2-nd hit is leading(up)-edge
                dt=tf-tm[1];
                HF1(1106,geant(dt),1.);//hist. the same hit d/s-TDC difference
              }
            }
          }
// --> loop over f-TDC hits (up-edges) to find f-tdc hit, coresponding s-tdc hit (MATCHING):
          tmf[0]=-1.;
          tmf[1]=-1.;
          itmf[0]=0;
          itmf[1]=0;
          fstd=number(scbrcal[ilay][ibar].gtfstrd()); // diff. in f/s same-hit delay
          if(smty[0]>0){
            for(i=0;i<nftdc[0];i+=2){ // side-1
              tf=number((ftdc1[i+1]&pbanti)*TOFDBc::tdcbin(0));//take up-edge(2-nd) of f-TDC
              dt=tf-tm[0]-fstd;
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1115,geant(dt),1.);
              if(fabs(dt) < tofvpar.fstdw()){
                tmf[0]=tf;
                itmf[0]=i+1;
              }
            }
          }                          
          if(smty[1]>0){
            for(i=0;i<nftdc[1];i+=2){ // side-2
              tf=number((ftdc2[i+1]&pbanti)*TOFDBc::tdcbin(0));
              dt=tf-tm[1]-fstd;
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1115,geant(dt),1.);
              if(fabs(dt) < tofvpar.fstdw()){
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
              tf=(ftdc1[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[0]-tf),1.);
              if((tmf[0]-tf) < tofvpar.hiscuta())reject=1;
            }
            j=itmf[0]+2; // Side-1 "befor"-check
            if(j < nftdc[0]){
              tf=(ftdc1[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[0]),1.);
              if((tf-tmf[0]) < tofvpar.hiscutb())reject=1;
            }
          }
//
          if(smty[1]>0){
            j=itmf[1]-2; // Side-2 "after"-check 
            if(j >= 0){
              tf=(ftdc2[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[1]-tf),1.);
              if((tmf[1]-tf) < tofvpar.hiscuta())reject=1;
            }
            j=itmf[1]+2; // Side-2 "befor"-check
            if(j < nftdc[1]){
              tf=(ftdc2[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[1]),1.);
              if((tf-tmf[1]) < tofvpar.hiscutb())reject=1;
            }
          }
        }
//
//===========>>> set time-history status of sc.bar :
//
            if(reject==1)sta|=SCBADB1;// set bit "time-history problem" on any alive side
            if(reject==0)TOFJobStat::addbr(brnum,2);// statistics on "good time-history"
            if(reject==0 && isds==2)TOFJobStat::addbr(brnum,3);// statistics on 2-sided(2x3meas)
//                                                               "good time-history"
//
//===========>>> identify "corresponding"(to sTDC) hit in anode/dynode-TDC :
//
        int ashn[2]={0,0};//anode up-edge number matching sTDC up-edge(=0->last_in_real_time)
        int dshn[2]={0,0};//dynode up-edge number matching sTDC up-edge
        int badah,baddh;
        badah=0;
        baddh=0;
        if(smty[0]>0){ // check anode hits (side-1)
          badah+=1;
          for(i=0;i<nadca[0];i+=2){ 
            tf=number((adca1[i+1]&pbanti)*TOFDBc::tdcbin(2));//2-nd is leading(up)-edge
            dt=tf-tm[0]-tofvpar.satdcd();
            if(fabs(dt) < tofvpar.satdcg()){//s/a-tdc matching found
              ashn[0]=i;
              badah-=1;
              break;
            }
          }
          if(TOFRECFFKEY.reprtf[2]>0){// just for tests
            if(badah==0)HF1(1108,geant(ashn[0]/2+1),1.);
            else HF1(1108,0.,1.);
          }
        } 
        if(smty[1]>0){ // check anode hits (side-2)
          badah+=1;
          for(i=0;i<nadca[1];i+=2){ 
            tf=number((adca2[i+1]&pbanti)*TOFDBc::tdcbin(2));//2-nd is leading(up)-edge
            dt=tf-tm[1]-tofvpar.satdcd();
            if(fabs(dt) < tofvpar.satdcg()){//s/a-tdc matching found
              ashn[1]=i;
              badah-=1;
              break;
            }
          }
        } 
        if(smty[0]>0 && nadcd[0]>=2){ // check dynode hits (side-1)
          baddh+=1;
          for(i=0;i<nadcd[0];i+=2){ 
            tf=number((adcd1[i+1]&pbanti)*TOFDBc::tdcbin(3));//2-nd is leading(up)-edge
            dt=tf-tm[0]-tofvpar.sdtdcd();
            if(fabs(dt) < tofvpar.sdtdcg()){//s/a-tdc matching found
              dshn[0]=i;
              baddh-=1;
              break;
            }
          }
          if(TOFRECFFKEY.reprtf[2]>0){// just for tests
            if(baddh==0)HF1(1109,geant(dshn[0]/2+1),1.);
            else HF1(1109,0.,1.);
          }
        } 
        if(smty[1]>0 && nadcd[1]>=2){ // check dynode hits (side-2)
          baddh+=1;
          for(i=0;i<nadcd[1];i+=2){ 
            tf=number((adcd2[i+1]&pbanti)*TOFDBc::tdcbin(3));//2-nd is leading(up)-edge
            dt=tf-tm[1]-tofvpar.sdtdcd();
            if(fabs(dt) < tofvpar.sdtdcg()){//s/a-tdc matching found
              dshn[1]=i;
              baddh-=1;
              break;
            }
          }
        }
//
        if(badah>0)sta|=SCBADB6;// set bit of "no a/s-TDC matching on any alive side" 
        if(baddh>0)sta|=SCBADB7;// set bit of "no d/s-TDC matching on any alive side" 
        if(badah==0)TOFJobStat::addbr(brnum,4);// statistics on a-matching
        if(baddh==0)TOFJobStat::addbr(brnum,5);// statistics on d-matching
//
//===========>>> calculate times/Edeps of sc.bar :
//
//
// --> prepare fine resolution side-times(using stretcher info):
//
            tm[0]=0;
            ama[0]=0;
            amf[0]=0;
            tmr[0]=0.;
            if(smty[0]==1){// good(3-measurement) side, but matching/history may not be good
              t4=(stdc1[0]&pbanti)*TOFDBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc1[2]&pbanti)*TOFDBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc1[3]&pbanti)*TOFDBc::tdcbin(1);// 1-st edge of str-info
              tm[0]=((t2-t4)-srof[0])/strr[0];// s-1 time (ns,A-noncorr)
              tmf[0]=tm[0];
              i=ashn[0];
              am[0]=integer(adca1[i+1]&pbanti)-integer(adca1[i]&pbanti); // TovT raw values
              ama[0]=am[0]*TOFDBc::tdcbin(2);//TDC_counts->ns
              amf[0]=ama[0]; 
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1104,geant(ama[0]),1.);
              tmr[0]=t1-t2;
            }
            tm[1]=0;
            ama[1]=0;
            amf[1]=0;
            tmr[1]=0.;
            if(smty[1]==1){// good(3-measurement) side, but matching/hist may not be good
              t4=(stdc2[0]&pbanti)*TOFDBc::tdcbin(1);// 4-th edge of str-info
              t2=(stdc2[2]&pbanti)*TOFDBc::tdcbin(1);// 2-nd edge of str-info
              t1=(stdc2[3]&pbanti)*TOFDBc::tdcbin(1);// 1-st edge of str-info
              tm[1]=((t2-t4)-srof[1])/strr[1];// s-2 time (ns,A-noncorr)
              tmf[1]=tm[1];
              i=ashn[1];
              am[1]=integer(adca2[i+1]&pbanti)-integer(adca2[i]&pbanti);
              ama[1]=am[1]*TOFDBc::tdcbin(2);//TDC_counts->ns
              amf[1]=ama[1]; 
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1104,geant(ama[1]),1.);
              tmr[1]=t1-t2;
            }
            if(smty[0]==0){ // make "=" each side times and ampl. for 1-side case:
              tmf[0]=tmf[1];
              amf[0]=amf[1];
            }
            if(smty[1]==0){
              tmf[1]=tmf[0];
              amf[1]=amf[0];
            }
            if(amf[0]>800.)amf[0]=800.;//tempor
            if(amf[1]>800.)amf[1]=800.;
//
            zc=TOFDBc::getzsc(ilay,ibar);
            blen=TOFDBc::brlen(ilay,ibar);
            if(TOFRECFFKEY.relogic[0] != 1){// ====> for all except STRR_calibration runs :
//
//--> Calc. longit. coord/err and position corr. to signal :
              scbrcal[ilay][ibar].tmd2p(tmf,amf,co,eco);// get A-corrected Local(!).coord/err
              brlm=0.5*TOFDBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means: "coord. is more than counter half length"
                pcorr=scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=scbrcal[ilay][ibar].poscor(co);
              }
              if(isds==1)pcorr=1.;// no position correction for one-sided counters
//--> Find Eloss from anode :
              qtota=scbrcal[ilay][ibar].ama2mip(amf)
                                             /pcorr; //an-tot Edep(mev) with corrections
//--> Find counter-time corrected for anode TovT :
              time=scbrcal[ilay][ibar].tm2t(tmf,amf); // time with corrections
//--> Find Eloss from dinode :
              qtotd=0.;
              amd[0]=0.;
              amd[1]=0.;
              if(nadcd[0]>=2){
                i=dshn[0];
                am[0]=integer(adcd1[i+1]&pbanti)-integer(adcd1[i]&pbanti); // TovT raw values
                amd[0]=am[0]*TOFDBc::tdcbin(3);//TDC_counts->ns
              }
              if(nadcd[1]>=2){
                i=dshn[1];
                am[1]=integer(adcd2[i+1]&pbanti)-integer(adcd2[i]&pbanti);
                amd[1]=am[1]*TOFDBc::tdcbin(3);
              }
//  require 2-sides measurements for dinode (otherwise it is useless):
              if(nadcd[0]>=2 && nadcd[1]>=2){
                if(amd[0]>600.)amd[0]=600.;//tempor
                if(amd[1]>600.)amd[1]=600.;
                qtotd=scbrcal[ilay][ibar].amd2mip(amd)
                                            /pcorr;//dinode-tot Edep(mev) with corrections
              }
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
              st=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0)
              ,new AMSTOFRawCluster(sta,ilay+1,ibar+1,zc,ama,amd,
                                  qtota,qtotd,tm,time,coo,ecoo));//store values
            } // ---> end of run-type check
//-----------
        } // ---> end of "side measurement-multiplicity" check
//
      } // ---> end of sides presence check
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
  int nbrch[SCLRS],conf(-1),isum(0);
  int il,ib,ix,iy;
  geant x[2],y[2],zx[2],zy[2],zcb[SCLRS],tgx,tgy,cosi;
  number trlen13,trlen24;
//
  for(i=0;i<SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]>0)nbrch[i]=1;
  }
  for(i=0;i<SCLRS;i++)isum+=nbrch[i];
  HF1(1110,geant(isum),1.);// tot.number of layers
  if(TOFRECFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){

    for(i=0;i<SCLRS;i++)if(nbrch[i]>0)HF1(1111,geant(i+1),1.);// layer appear. freq.
  }
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TOFRECFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
    HF1(1112,geant(conf),1.);
  }
//
  if((nbrch[0]+nbrch[1])>=1 && (nbrch[2]+nbrch[3])>=1)ostatus=0; // good event
// 
// --->same multtipl. checks for single bar layers:
//
  for(i=0;i<SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TOFRECFFKEY.reprtf[2]>0 || 
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
  geant cry[SCLRS];
  AMSTOFRawCluster *ptrc;
  ptrc=(AMSTOFRawCluster*)AMSEvent::gethead()->
                                     getheadC("AMSTOFRawCluster",0);
//
// --> find track-counter crossing points(longit) using counter transv.coord :
//
  ix=0;
  iy=0;
  for(il=0;il<SCLRS;il++){
    ib=brnl[il];
    zcb[il]=TOFDBc::getzsc(il,ib);
    if(TOFDBc::plrotm(il)==0){// unrotated (X-meas) planes
      x[ix]=TOFDBc::gettsc(il,ib);
      zx[ix]=zcb[il];
      ix+=1;
    }
    else{                    // rotated (Y-meas) planes
      y[iy]=TOFDBc::gettsc(il,ib);
      zy[iy]=zcb[il];
      iy+=1;
    }
  }
  tgx=(x[0]-x[1])/(zx[0]-zx[1]);// track slopes in 2 projections
  tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
  for(il=0;il<SCLRS;il++){
    ib=brnl[il];
    if(TOFDBc::plrotm(il)==0){// unrotated (X-meas) planes
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
      if((il==3) && (ib>1) && (ib<12) && (status&SCBADB2)>0){ // tempor recov. only for L=4
//    if((status&SCBADB2)>0){ // 1-sided counter found in any layer/counter
        ptrc->recovers(cry[il]);// recovering
        status=ptrc->getstatus();//new status
        if((status & SCBADB5)>0){
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
  for(i=0;i<SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]==1 && isdsl[i]==2)nbrch[i]=1;
  }
  isum=0;
  conf=-1;
  for(i=0;i<SCLRS;i++)isum+=nbrch[i];
  if(isum>=2)conf=0;
  if(isum>=3){
    for(i=0;i<SCLRS;i++)if(nbrch[i]==0)conf=i+1;
  }
  if(isum==4)conf=5;
  if(TOFRECFFKEY.reprtf[2]>0 || 
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
  td13=(tcorr[0]-tcorr[2])*130./trlen13;// tormalized to 130cm distance
  td24=(tcorr[1]-tcorr[3])*130./trlen24;// tormalized to 130cm distance
  td14=tuncorr[0]-tuncorr[3];
//
  if(TOFRECFFKEY.reprtf[2]>0 || 
     (AMSJob::gethead()->isMonitoring() & (AMSJob::MTOF | AMSJob::MAll))){
      HF1(1532,td13,1.);//ToF for L0->L2
      HF1(1534,td24,1.);//ToF for L1->L3
      HF1(1544,(td13-td24),1.);
      if(TOFRECFFKEY.reprtf[2]>1){ 
        for(il=0;il<SCLRS;il++){// fill de/dx hist. for each bar
          ib=brnl[il];
          brnum=il*SCMXBR+ib;//sequential bar numbering
          elosn=edepa[il]/cosi;//normalized  to norm.inc.
          HF1(1140+brnum,elosn,1.);
        }
        HF1(1529,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
        HF1(1531,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
        HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      }
//      HF1(1533,tdiff[0],1.);//layer=0
//      HF1(1543,clong[0],1.);//Y-coord(loc.r.s.)
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
void AMSTOFRawCluster::recovers(number x){ // function to recover missing side
  geant gn[2],csl,sl[2],dt0,upr[5],vel,xerr,sqr,hclen,co,eco,pcorr;
  number q[2],tm[2],tot[2],tcor;
  int il,ib;
  int isg(1),isb(0);
//
  if(_status&SCBADB4){
    isg=0;
    isb=1;
  }
  il=_ntof-1;
  ib=_plane-1;
  scbrcal[il][ib].getgaina(gn);
  scbrcal[il][ib].getupar(upr);
  scbrcal[il][ib].getd2p(vel,xerr);
  scbrcal[il][ib].getslops(sl);
  csl=scbrcal[il][ib].getslope();
  dt0=scbrcal[il][ib].gettdif();
  hclen=0.5*TOFDBc::brlen(il,ib);
// ---> recover TovT for bad side :
  tot[isg]=_tovta[isg];
  if(tot[isg]>800.)tot[isg]=800.;// tempor.protection
  scbrcal[il][ib].q2t2q(1,isg,0,tot[isg],q[isg]);//TovTgood->Q
  q[isg]/=gn[isg];// Q->Qref
  sqr=upr[0]*(exp(-(hclen+x)/upr[3])+upr[2]*exp(-(hclen+x)/upr[4]))/
      upr[1]/(exp(-(hclen-x)/upr[3])+upr[2]*exp(-(hclen-x)/upr[4]));//Qr1/Qr2
  if(isb==1)sqr=1./sqr;
  q[isb]=q[isg]*sqr;
  q[isb]*=gn[isb];//Qref->Q
  scbrcal[il][ib].q2t2q(0,isb,0,tot[isb],q[isb]);//Q->TovTbad
  _tovta[isb]=tot[isb]; // recovered TovT for bad side
//
  if(fabs(x) > (hclen+5.)){// out of range by more than 5cm
    pcorr=scbrcal[il][ib].poscor(0.);// no correction (==1)
  }
  else{
    pcorr=scbrcal[il][ib].poscor(geant(x));
  }
  if(tot[isb]>800.)tot[isb]=800.;// tempor.protection
  _edep=scbrcal[il][ib].ama2mip(tot)/pcorr; //recowered Eloss
// ---> recover time :
  tcor=2.*csl*(sl[0]/sqrt(q[0])-sl[1]/sqrt(q[1]));//slewing corrections.
  tm[isg]=_sdtm[isg];
  tm[isb]=tm[isg]-(1-2*isg)*(2.*dt0-2.*x/vel-tcor);
  _sdtm[isb]=tm[isb]; // recowered side time(raw)
  _time=scbrcal[il][ib].tm2t(tm,tot); // recovered time(with corr)
  scbrcal[il][ib].tmd2p(tm,tot,co,eco);// get A-corrected Local(!).coord/err
  _timeD=co;// recovered loc. coordinate (chould be = x)
  _etimeD=3.5;// tempor err.on longit.coord(cm) for 1-sided,recovered counters
  _status|=SCBADB5;//add bit for recovered counters
}
//-----------------------------------------------------------------------
void AMSTOFRawCluster::_writeEl(){
  TOFRawClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tofraw();

  if (TN->Ntofraw>=MAXTOFRAW) return;

// Fill the ntuple
  if(AMSTOFRawCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Ntofraw]=_status;
    TN->Layer[TN->Ntofraw]=_ntof;
    TN->Bar[TN->Ntofraw]=_plane;
    for(int i=0;i<2;i++){
      TN->tovta[TN->Ntofraw][i]=_tovta[i];
      TN->tovtd[TN->Ntofraw][i]=_tovtd[i];
      TN->sdtm[TN->Ntofraw][i]=_sdtm[i];
    }
    TN->Ntofraw++;
  }

}
//------

void AMSTOFRawCluster::_copyEl(){
}

//------
void AMSTOFRawCluster::_printEl(ostream & stream){
  stream <<"AMSTOFRawCluster "<<_ntof<<" "<<_edep<<" "<<_plane<<endl;
}

AMSTOFCluster * AMSTOFCluster::_Head[4]={0,0,0,0};


void AMSTOFCluster::_writeEl(){
  TOFClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_tof();

  if (TN->Ntof>=MAXTOF) return;

// Fill the ntuple
  if(AMSTOFCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Ntof]=_status;
    TN->Layer[TN->Ntof]=_ntof;
    TN->Bar[TN->Ntof]=_plane;
    TN->Edep[TN->Ntof]=_edep;
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
  stream <<"AMSTOFCluster "<<_status<<" "<<_ntof<<" "<<_plane<<" "<<_edep<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<_time<<" " <<_etime<<endl;
}


 
void AMSTOFCluster::print(){
for(int i=0;i<4;i++){
AMSContainer *p =AMSEvent::gethead()->getC("AMSTOFCluster",i);
 if(p)p->printC(cout);
}
}

//===========================================================================

void AMSTOFCluster::build(int &stat){
  AMSTOFRawCluster *ptr; 
  AMSTOFRawCluster *ptrr; 
  static AMSTOFRawCluster * xptr[SCMXBR+2];
  static number eplane[SCMXBR+2];
  static AMSTOFRawCluster * membp[3];
  geant dummy,edep,edepl,edepa,edepd,asatl,time,etime,speedl,err;
  integer ntof,barn,status,plrot;
  geant barl,barw,bars,cofg,cofgl,yloc,eyloc,ylocm,c0,ct,cl,ed;
  geant errx,erry,erx1(2.);//tempor errors on cl.transv.coord.
  AMSPoint coo,ecoo;
  int i,j,il,ib,ilay,ibar,ill,ibb;
  int nclust,cllay[SCLRS],nmemb,imb;
//-----
  stat=1; // bad
  for(i=0;i<SCLRS;i++)cllay[i]=0;
//
  for(il=1;il<SCLRS+1;il++){// <-------- TOF layers loop -----
    ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                                     getheadC("AMSTOFRawCluster",0);
    VZERO(eplane,(SCMXBR+2)*sizeof(number)/4);
    VZERO(xptr,(SCMXBR+2)*sizeof(AMSTOFRawCluster*)/4);
    while (ptr){// scan to put all bars of layer "il" in buffer
      if(ptr->getntof()==il){
        ib=ptr->getplane();
#ifdef __AMSDEBUG__
        assert(ib>0 && ib <= SCMXBR);
#endif
        edepa=ptr->getedep();
        edepd=ptr->getedepd();
//    select between anode and dinode measurements:
        edep=edepa;
        asatl=scbrcal[il-1][ib-1].getasatl();
    asatl=300.;// tempor !!! to avoid dynode (~150mips)
        if(edepd/2. > asatl)edep=edepd;
//
        eplane[ib]=edep;
        xptr[ib]=ptr;
      }
      ptr=ptr->next();
    }// ---> end of scan
//------
  nclust=0;
  for (i=1;i<=SCMXBR;i++){// <---- loop over buffer content (clust. search)
    if(   eplane[i]> TOFRECFFKEY.Thr1
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
      barl=TOFDBc::brlen(ilay,ibar);// peak bar length
      barw=TOFDBc::plnstr(5);//bar width
      bars=TOFDBc::plnstr(5)-TOFDBc::plnstr(4)
                            +2.*TOFDBc::plnstr(13);//sc.bar transv. step
      TOFMCFFKEY.padl=bars;//redef. datacard's sc.bar transv.step
      scbrcal[ilay][ibar].getd2p(speedl,err);//get light speed
      yloc=ptr->gettimeD();// get yloc/err for "peak" bar
      eyloc=ptr->getetimeD();
      ylocm=0.5*barl;// limit on max. long. coord.
      if(fabs(yloc) > ylocm){//out of bar size
        eyloc=barl/sqrt(12.);
        if(yloc>0.)yloc=ylocm;//at bar edge
        else yloc=-ylocm;
      }
      status=ptr->getstatus();//may be !=0(bad history/t_meas or single-sided)
      edep=0.;
      edepl=0.;
      cofg=0.;
      cofgl=0.;
      nmemb=0;
      imb=0;
      membp[imb]=ptr;//store raw cl. member pointers(peak)
      for(j=i-1;j<i+2;j++){// calc. clust. energy/COG-transv/COG-long
        ptrr=xptr[j];   
        if(ptrr){
          nmemb+=1;
          if(j!=i){
            imb+=1;
            membp[imb]=ptrr;//store raw cl. member pointers(neigbours)
          }
          ill=ptrr->getntof()-1;
          ibb=j-1;
          ylocm=0.5*TOFDBc::brlen(ill,ibb);// bar length
          edep+=eplane[j];
          cofg+=eplane[j]*(j-i);// relative to "peak" bar
          yloc=ptrr->gettimeD();
          if(fabs(yloc) > ylocm){
            if(yloc>0.)yloc=ylocm;//at bar edge
            else yloc=-ylocm;
          }
          edepl+=eplane[j];
          cofgl+=eplane[j]*yloc;
        }
      }//---> end of the member loop
      time=ptr->gettime();// (ns)
      etime=TOFMCFFKEY.TimeSigma/sqrt(2.);//(sec !!) tempor(121ps,later put in TOFBrcal needed data!)
      if((status&SCBADB2)>0){
        if(status & SCBADB5){ // recovered 1-sided counter
          etime=2.05*TOFMCFFKEY.TimeSigma/sqrt(2.);//tempor(248ps,still no Tracker info)
        }
        else{
          etime=(1.e-9)*barl/speedl/sqrt(12);//(sec !!!)for single-sided counters
        }
      }
//------
      if(edep>TOFRECFFKEY.ThrS){// <--- calc.clus.parameters if Ecl>Ethresh
        if(TOFRECFFKEY.reprtf[2]>0){
        if(il==1){
          HF1(1535,edep,1.);//Cluster energy distr.,L=1
          HF1(1537,edep,1.);//Cluster energy distr.,L=1
        }
        if(il==2){
          HF1(1539,edep,1.);//Cluster energy distr.,L=2
          HF1(1541,edep,1.);//Cluster energy distr.,L=2
        }
        if(il==3){
          HF1(1536,edep,1.);//Cluster energy distr.,L=3
          HF1(1538,edep,1.);//Cluster energy distr.,L=3
        }
        if(il==4){
          HF1(1540,edep,1.);//Cluster energy distr.,L=4
          HF1(1542,edep,1.);//Cluster energy distr.,L=4
        }
        }
        coo[2]=ptr->getz();          //cluster Z-coord.
        ecoo[2]=TOFDBc::plnstr(6)/sqrt(12.);//bar thickness/...   
        plrot=TOFDBc::plrotm(ilay);     // =0/1-unrotated/rotated TOF-plane
        c0=TOFDBc::gettsc(ilay,ibar);   //transv.pos. of "peak" bar
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
        if((status & SCBADB3)!=0)status|=AMSDBc::BAD; 
//          bad=(peak bar has severe problem with t-measurement)
//        if((status & SCBADB2)!=0 && (status & SCBADB5)==0)status|=AMSDBc::BAD;
//          bad=(peak bar is 1-sided and not recovered -> bad for t-measur)
        if(nmemb>0)edep/=nmemb;// take average for multi-bar clusters
//
        AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ilay), new
          AMSTOFCluster(status,ntof,barn,edep,coo,ecoo,time,etime,nmemb,membp));
//
        nclust+=1;
        eplane[i-1]=0.;// remove used bars
        eplane[i]=0.;
        eplane[i+1]=0.; 
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
void AMSTOFCluster::recovers(AMSTrTrack * ptr){ // recreate TOFCluster
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
    status=_mptr[nm]->getstatus();
    mil=_mptr[nm]->getntof()-1;
    mib=_mptr[nm]->getplane()-1;
    if(nm==0){// first member is "peak", use it to find crossing point
      coo[0]=0.;
      coo[1]=0.;
      coo[2]=TOFDBc::getzsc(mil,mib);
      ptr->interpolate(coo,dir,outp,theta,phi,sleng);
      if(TOFDBc::plrotm(mil)==0){
        clo=outp[1];// unrot.(X-meas) -> Y-cross = long.c(loc.r.s.=abs.r.s.)
        ctr=outp[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        clo=outp[0];// rot.(Y-meas) -> take X-cross for long.c.
        clo=-clo;// go to bar local r.s.
        ctr=outp[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
    }
      _mptr[nm]->recovers(clo);//missing side recovering
      newt=_mptr[nm]->gettime();//new time of raw cluster (ns)
      _time=-newt*1.e-9;//new time of cluster(-sec for Vitali)
      _etime=1.323*TOFMCFFKEY.TimeSigma/sqrt(2.);//tempor (sec)(160ps, trecker recovered)
//  }
}
//--------------------------------------------

integer AMSTOFRawCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFRawCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}

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
void TOFUser::Event(){  // some processing when all subd.info is redy (+accros)
  integer i,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad,status,sector;
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number ama[2],amd[2],coo[SCLRS],coot[SCLRS],cstr[SCLRS],dx,dy;
  number am1[SCLRS],am2[SCLRS],am1d[SCLRS],am2d[SCLRS],am[2],eanti(0);
  geant ainp[2],dinp[2],cinp;
  number ltim[SCLRS],tdif[SCLRS],trle[SCLRS],trlr[SCLRS];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq,betof,lflgt;
  number sigt[4]={0.121,0.121,0.121,0.121};// time meas.accuracy 
  number cvel(29.979);// light velocity
  number eacut=0.6;// cut on E-anti (mev)
  number dscut=6.5;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  AMSTOFRawCluster *ptr;
  AMSAntiCluster *ptra;
  uinteger Runum(0);
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  Runum=AMSEvent::gethead()->getrun();// current run number
  TOFJobStat::addre(11);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
  while(ptr){ // <--- loop over AMSTOFRawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    if((status&SCBADB1)==0 && (status&SCBADB3)==0){ //"good_history/good_strr" hits
      if((status&SCBADB2)==0 || (status&SCBADB5)!=0){// 2-sided or recovered
        ptr->gettovta(ama);// get raw anode-ampl(TovT in ns)
        ptr->gettovtd(amd);// get raw dinode-ampl(TovT in ns)
        scbrcal[ilay][ibar].ama2q(ama,am);// convert to charge
        am1[ilay]=am[0];
        am2[ilay]=am[1];
        nbrl[ilay]+=1;
        brnl[ilay]=ibar;
        coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
        ltim[ilay]=ptr->gettime();// get ampl-corrected time
      }
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOFJobStat::addre(12);
//
// -----> check Anti-counter :
  eanti=0;
  while (ptra){ // <--- loop over AMSTOFRawCluster hits
    status=ptra->getstatus();
    if(status==0){ //select only good hits
      sector=(ptra->getsector())-1;
      eanti=eanti+(ptra->getedep());
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(TOFRECFFKEY.reprtf[2]>0)HF1(1503,geant(eanti),1.);
  if(eanti>eacut)return;// remove events with big signal in Anti
//
// -----> remove albedo and very slow part. :
//
  lflgt=2.*TOFDBc::supstr(1)+TOFDBc::supstr(7);
  betof=2.*lflgt/(ltim[0]+ltim[1]-ltim[2]-ltim[3])/cvel;//uderestim.raw TOFbeta
  if(betof<0.5)return;
//
  TOFJobStat::addre(13);
//
//
//------> get parameters from tracker:
//
    static number pmas(0.938),mumas(0.1057),imass;
    number pmom,mom,bet,chi2,betm,pcut[2],massq;
    number the,phi,trl,rid,err,ctran;
    AMSPoint C0,Cout;
    AMSDir dir(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    int ntrk,ipatt;
    ntrk=0;
    cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get TOF-matched track
    if(cptr)
           ntrk+=cptr->getnelem();
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1506,geant(ntrk),1.);
    if(ntrk!=1)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();//get pointer of the track, used in given particle
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
      status=ptrack->getstatus();
    } 
    else rid=0;
    pmom=fabs(rid);
//
    if(TOFCAFFKEY.caltyp==0){ // space calibration
      bet=pmom/sqrt(pmom*pmom+pmas*pmas);
      imass=pmas;
      pcut[0]=TOFCAFFKEY.plhc[0];
      pcut[1]=TOFCAFFKEY.plhc[1];
    }
    else{                     // earth calibration
      bet=pmom/sqrt(pmom*pmom+mumas*mumas);
      imass=mumas;
      pcut[0]=TOFCAFFKEY.plhec[0];
      pcut[1]=TOFCAFFKEY.plhec[1];
    }
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1501,bet,1.);
//
    bad=0;
    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
//    if((status&16384)!=0)bad=1;// remove FalseTOFX tracks
    if(bad==1)return;
    TOFJobStat::addre(14);
//
    bad=0;
//
// ----->  find track crossing points/angles with counters:
//
    C0[0]=0.;
    C0[1]=0.;
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      cstr[il]=cos(the);
      trlr[il]=fabs(trl);
      if(TOFDBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take Y-cross for long.c
        ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take X-cross for long.c.
        coot[il]=-coot[il];// go to bar local r.s.
        ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
      dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc
      if(TOFRECFFKEY.reprtf[2]>0)HF1(1200+il,geant(dy),1.);
      dx=ctran-TOFDBc::gettsc(il,ib);//Transv.coo_tracker-Transv.coo_scint
      if(TOFRECFFKEY.reprtf[2]>0)HF1(1210+il,geant(dx),1.);
      if(fabs(dx)>dscut || fabs(dy)>dscut)bad=1;//too big dist. of tof-tracker
    }
//
    if(bad)return;//too big difference of TOF-Tracker coord.
    TOFJobStat::addre(15);
//
//--------> find beta from TOF :
//
    if(Runum>896888674){
      sigt[3]=0.16;// for good+FalseX
      if((status&16384)!=0)sigt[3]=0.248;//for FalseTOFX
    }
    else sigt[3]=0.121;
//
    trle[0]=0.;
    trle[1]=trlr[0]-trlr[1];//1->2
    trle[2]=trlr[0]+trlr[2];//1->3
    trle[3]=trlr[0]+trlr[3];//1->4
    tdif[0]=0;
    tdif[1]=ltim[0]-ltim[1];
    tdif[2]=ltim[0]-ltim[2];
    tdif[3]=ltim[0]-ltim[3];
    fpnt=0;
    sul=0;
    sut=0;
    sutl=0;
    sul2=0;
    sud=0.;
    for(il=0;il<SCLRS;il++){
      sit2=pow(sigt[il],2);
      fpnt+=1;
      sud+=1./sit2;
      sut+=tdif[il]/sit2;
      sul+=trle[il]/sit2;
      sul2+=(trle[il]*trle[il])/sit2;
      sutl+=(tdif[il]*trle[il])/sit2;
    }
    bci = (sud*sutl-sut*sul)/(sud*sul2-sul*sul);
    tzer=(sut*sul2-sutl*sul)/(sud*sul2-sul*sul);
    chsq=0;
    for(il=0;il<SCLRS;il++)chsq+=pow((tzer+bci*trle[il]-tdif[il])/sigt[il],2);
    chsq/=number(fpnt-2);
    betof=1./bci/cvel;
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1502,betof,1.);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1205,chsq,1.);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1206,tzer,1.);
    if(chsq>8. || betof<0.6)return;//cut on chi2/beta
//
    geant td13,td24;
    td13=tdif[2]*130./trle[2];// tormalized to 130cm distance
    td24=(ltim[1]-ltim[3])*130./(trle[3]-trle[1]);// tormalized to 130cm distance
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1504,(td13-td24),1.);
    return;
//
}
//----------------------------
void TOFUser::InitJob(){
  if(TOFRECFFKEY.reprtf[2]>0){
    HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
    HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
    HBOOK1(1502,"Particle beta(tof)",80,0.7,1.2,0.);
    HBOOK1(1504,"T13-T24(ns,high momentum)",80,-4.,4.,0.);
    HBOOK1(1503,"Anticounter energy(4Lx1bar events)(mev)",80,0.,40.,0.);
    HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
    HBOOK1(1200,"Res_long.coo(track-sc),L=1",50,-10.,10.,0.);
    HBOOK1(1201,"Res_long.coo(track-sc),L=2",50,-10.,10.,0.);
    HBOOK1(1202,"Res_long.coo(track-sc),L=3",50,-10.,10.,0.);
    HBOOK1(1203,"Res_long.coo(track-sc),L=4",50,-10.,10.,0.);
    HBOOK1(1210,"Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);
    HBOOK1(1211,"Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);
    HBOOK1(1212,"Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);
    HBOOK1(1213,"Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);
    HBOOK1(1205,"Chisq (tof-beta-fit)",50,0.,10.,0.);
    HBOOK1(1206,"Tzer (tof-beta-fit)",50,-2.5,2.5,0.);
  }
  return;
}
void TOFUser::EndJob(){
  char chopt[6]="qb";
  char chfun[6]="g";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//
  if(TOFRECFFKEY.reprtf[2]==0)return;
  par[0]=200.;
  par[1]=1.;
  par[2]=0.03;
  step[0]=50.;
  step[1]=0.005;
  step[2]=0.005;
  pmin[0]=10.;
  pmin[1]=0.8;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.2;
  pmax[2]=0.1;
  HFITHN(1502,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" TOFbeta-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1502);
//
  par[0]=200.;
  par[1]=0.;
  par[2]=0.3;
  step[0]=50.;
  step[1]=0.1;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=-2.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=2.;
  pmax[2]=0.5;
  HFITHN(1504,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" TOFTresol-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1504);
//
  HPRINT(1500);
  HPRINT(1501);
  HPRINT(1506);
  HPRINT(1200);
  HPRINT(1201);
  HPRINT(1202);
  HPRINT(1203);
  HPRINT(1210);
  HPRINT(1211);
  HPRINT(1212);
  HPRINT(1213);
  HPRINT(1205);
  HPRINT(1206);
  HPRINT(1503);
  return;
}
//=======================================================================
//  DAQ-interface functions :
//
// function returns number of TOF_data-words for given block/format
//
integer AMSTOFRawEvent::calcdaqlength(int16u blid){
  AMSTOFRawEvent *ptr;
  integer len(0),ncnt,nh,nzc;
  int16u idd,id,ilay,ibar,isid;
  int16u crate,rcrat,sfet,tofc,tdcc,hwid,fmt;
  integer nhits[SCSFET]={0,0,0,0};
  integer nztdcc[SCSFET]={0,0,0,0};
  integer ntoth,nonemp;
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);
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
    hwid=AMSTOFRawEvent::sw2hwid(ilay,ibar,isid);// CST
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
    for(sfet=0;sfet<SCSFET;sfet++){
      nh=nhits[sfet];
      nzc=nztdcc[sfet];
      if(DAQSBlock::isSFETT2(rcrat,sfet)>0 && nonemp>0){//SFET contains also temperatures
        nh+=4;// 2 Temperature measurements (TDC-ch) with 2 hits(edges) each.(2*2=4)
        nzc+=2;                      
      }
      ncnt=nzc/4;//hit-counter words (4 counters each)
      if(nzc%4 > 0)ncnt+=1;
      len=len+ncnt+nh;// nmb.of_hits + nmb.of_counter_words
    }
    len=len+SCSFET;// add SCSFET words for hitmasks.
  }
//
  else{ // =====> Raw format :
    for(sfet=0;sfet<SCSFET;sfet++){
      nh=nhits[sfet];
      if(DAQSBlock::isSFETT2(rcrat,sfet)>0){//this SFET contains also temper.
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
void AMSTOFRawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_TOF_data (first word after block-id)
// on output: len=TOF_data_length .
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nztof,first(0);
  int16u ilay,ibar,isid,idd,id;
  int16u phbit,maxv,ntdc,tdc[16];
  geant charge,edep;
  integer trflag,trpatt[SCLRS];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chip,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,sfet,tofc,tdcc,tdccm;
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptlist[SCSFET][SCTOFC];
//
  phbit=SCPHBP;//uniq phase-bit position used in Reduced_format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=int16u(SCPHBPA);//uniq phase-bit position used in Raw_format in address word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
#ifdef __AMSDEBUG__
  if(TOFRECFFKEY.reprtf[1]==2)cout<<"TOF::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
//
// ---> prepare ptlist[sfet][tofc] - list of TOFRawEvent pointers :
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);
  for(sfet=0;sfet<SCSFET;sfet++)
              for(tofc=0;tofc<SCTOFC;tofc++)
                           ptlist[sfet][tofc]=0;// clear pointer array
  nztof=0;//#of nonzero tof-tdcc per crate
  while(ptr){
    idd=ptr->getid();// LBBS
    id=idd/10;
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
    hwid=AMSTOFRawEvent::sw2hwid(ilay,ibar,isid);
    tofc=hwid%10-1;
    sfet=(hwid/10)%10-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      ptlist[sfet][tofc]=ptr;// store pointer
      nztof+=1;
#ifdef __AMSDEBUG__
      if(TOFRECFFKEY.reprtf[1]==2)ptr->_printEl(cout);
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
  for(sfet=0;sfet<SCSFET;sfet++){ // <--- SFETs loop (4)
    htmsk=0;
    ichm=ic;// bias for nonzero_tdc_channels mask
    ic+=1;
    nzch=0;// #of nonzero tdcc per sfet
    shft=0;
    for(tdcc=0;tdcc<SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      mskb=1<<tdcc;
      tofc=tdcc/4;//TOFch=4TDCch (4 measurements for one side)
      mtyp=DAQSBlock::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ntdc=0;//edges
      if((tofc+1)==DAQSBlock::gettempch(rcrat,sfet) //these tdcc's(from given tofc) are temperatures
                              && nztof>0){// and there are non empty TOF-ch in this crate
        mtyp=5;// means temperature measurements
        ntdc=2;// 2 edges
        tdc[0]=273;// down-edge (273 degree)
        if(!TOFDBc::pbonup())tdc[0]=tdc[0]|int16u(SCPHBP);// add phase bit
        tdc[1]=293;// up-edge (293 degree)
        if(TOFDBc::pbonup())tdc[1]=tdc[1]|int16u(SCPHBP); // add phase bit
        if((tdcc-4*tofc)>1)ntdc=0;//only 2 first tdcc of tofc are occupied by temp.info
      }
      ptr=ptlist[sfet][tofc];// =0 for temper. (sw2hwid-function should provide that)
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);
        if(mtyp==2)ntdc=ptr->getstdc(tdc);
        if(mtyp==3)ntdc=ptr->getadca(tdc);
        if(mtyp==4)ntdc=ptr->getadcd(tdc);
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
  for(sfet=0;sfet<SCSFET;sfet++){ // <--- SFETs loop (4)
    for(tdcc=0;tdcc<SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      chip=tdcc/8;//0-1
      chipc=tdcc%8;//chann.inside chip (0-7)
      chc=((chipc&1)<<2);//convert chipc to reverse bit pattern
      chc|=(chipc&2);
      chc|=((chipc&4)>>2);
      tofc=tdcc/4;//0-3
      mtyp=DAQSBlock::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ntdc=0;
      slad=DAQSBlock::sladdr(sfet);// slot-address (h/w address of sfet in crate)
      adrw=slad;// put slot-addr in address word (lsb)
      adrw|=(chc<<12);// add channel number chipc(inside of TDC-chip)
      if((tofc+1)==DAQSBlock::gettempch(rcrat,sfet)){  //these TDC-channels contain temperatures
        mtyp=5;// means temperature measurements
        ntdc=2;// 2 edges
        tdc[0]=273;// down-edge (273 degree)
        if(!TOFDBc::pbonup())tdc[0]=(tdc[0]|int16u(SCPHBP));// add phase bit
        tdc[1]=293;// up-edge (293 degree)
        if(TOFDBc::pbonup())tdc[1]=(tdc[1]|int16u(SCPHBP));// add phase bit
        if((tdcc-4*tofc)>1)ntdc=0;//only 2 first tdcc of tofc are occupied by temp.info
      }
      ptr=ptlist[sfet][tofc];
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);
        if(mtyp==2)ntdc=ptr->getstdc(tdc);
        if(mtyp==3)ntdc=ptr->getadca(tdc);
        if(mtyp==4)ntdc=ptr->getadcd(tdc);
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
  if(TOFRECFFKEY.reprtf[1]==2){
    cout<<"TOFDAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
    cout<<dec<<endl<<endl;
  }
#endif
}
//----------------------------------------------------------------------
// function to decode DAQ and build RawEvent: 
//  
void AMSTOFRawEvent::buildraw(int16u blid, integer &len, int16u *p){
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
  int16u ftdc[SCTHMX2*2],stdc[SCTHMX3*4],adca[SCTHMX4*2],adcd[SCTHMX4*2],ttdc[16];
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced-format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPA;//take uniq phase-bit position used in Raw-format for address-word
  lentot=*(p-1);// total length of the block(incl. block_id)
  bias=len;
//
// decoding of block-id :
//
  crate=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=1-(blid&63);// data_type ("0"->RawTDC; "1"->ReducedTDC)
#ifdef __AMSDEBUG__
  if(TOFRECFFKEY.reprtf[1]>=1){
    cout<<"TOF::decoding: crate/format="<<crate<<" "<<dtyp<<"  Ltot/bias="<<lentot<<" "<<bias<<endl;
  }
#endif
//
  if(dtyp==1){ // =====> reduced TDC data decoding : 
//
    ic=bias;//jamp to first data word (bias=1 because TOF is first det. in block)
    for(sfet=0;sfet<SCSFET;sfet++){ // <-- SFET's loop (4)
      hmsk=*(p+ic++);
      nzch=0;
      if(hmsk>0){
        for(tdcc=0;tdcc<SCTDCC;tdcc++){ // <-- TDC-chan. loop (16)
          tofc=tdcc/4;
          swid=AMSTOFRawEvent::hw2swid(crate, sfet, tofc);// LBBS
          mtyp=DAQSBlock::mtyptof(tofc,(tdcc-4*tofc));// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
          if((tofc+1)==DAQSBlock::gettempch(crate,sfet)){ //these TDC-channels contain temperatures
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
            if(n4ch>0){// create AMSTOFRawEvent object for !=0 "4xTDC"
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
                AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0),
                new AMSTOFRawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                    nadca,adca,nadcd,adcd));
              }
              else{
                TOFJobStat::addaq4(crate,dtyp);
#ifdef __AMSDEBUG__
                if(TOFRECFFKEY.reprtf[1]>=1){
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
            slnu=DAQSBlock::isSFETT2(crate,sfet);//seq.numbering of "temperature"-slots in 
//                                               crate (1,2,...; for now =1-> one temp-slot)
            if(slnu>0){
              chan=DAQSTSC*DAQSTCS*crate+DAQSTCS*(slnu-1)+(tdcc%4);
              DAQSBlock::puttemp(chan,val);// store temperature
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
    integer hwmx=SCSFET*SCTDCC;// max. h/w channels
    int16u hits[SCSFET*SCTDCC][16];// array to store hit-values(upto 16) for each h/w channel
    integer nhits[SCSFET*SCTDCC];// number of hits in given h/w channel
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
      sfet=DAQSBlock::slnumb(slad);// sequential slot number (0-5, or =DAQSSLT=6 if slad invalid)) 
      if(sfet==DAQSSLT)continue; //---> invalid slad: try to take next pair
      if(DAQSBlock::isSFET(slad)){// <--- SFET data: store in buffer
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
        hwch=SCTDCC*sfet+tdcc;//sequential tdc-ch numbering through all SFETs
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
    for(sfet=0;sfet<SCSFET;sfet++){ // <--- SFETs loop (4)
      for(tdcc=0;tdcc<SCTDCC;tdcc++){// <--- TDC-channels loop (16)
        tofc=tdcc/4;
        hwch=SCTDCC*sfet+tdcc;//sequential tdc-ch numbering through all SFETs
        swid=AMSTOFRawEvent::hw2swid(crate, sfet, tofc);// LBBS
        mtyp=DAQSBlock::mtyptof(tofc,(tdcc-4*tofc));//(1->ftdc,2->stdc,3->adca,4->adcd)
        if((tofc+1)==DAQSBlock::gettempch(crate,sfet)){//these TDC-channels contain temperatures
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
          if(n4ch>0){// create AMSTOFRawEvent object for !=0 "4xTDC"
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
              AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0),
              new AMSTOFRawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                    nadca,adca,nadcd,adcd));// create object
            }
            else{
              TOFJobStat::addaq4(crate,dtyp);
#ifdef __AMSDEBUG__
              if(TOFRECFFKEY.reprtf[1]>=1){
                cout<<"TOF:Raw_Decoding_warning: crate/sfet/tofc are in  contradiction with swid-map"<<endl;
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
          slnu=DAQSBlock::isSFETT2(crate,sfet);//seq.numbering of "temperature"-slots in
//                                               crate (1,2,...; for now =1-> one temp-slot)
          if(slnu>0){
            chan=DAQSTSC*DAQSTCS*crate+DAQSTCS*(slnu-1)+(tdcc%4);
            DAQSBlock::puttemp(chan,val);// store temperature
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
    cout<<"TofDaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
#ifdef __AMSDEBUG__
  if(TOFRECFFKEY.reprtf[1]>=1){
    cout<<"TOFRawEventBuild::decoding: FOUND "<<len<<" good words, hex dump follows:"<<endl;
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
int16u AMSTOFRawEvent::hw2swid(int16u a1, int16u a2, int16u a3){
  int16u swid,hwch;
//
  static int16u sidlst[SCCRAT*SCSFET*SCTOFC]={// 14 LBBS's + 2 empty  per CRATE :
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
  assert(a1>=0 && a1<SCCRAT);//crate(node)(0-7)
  assert(a2>=0 && a2<SCSFET);//sfet(0-3)
  assert(a3>=0 && a3<SCTOFC);//tofch(0-3)
#endif
  hwch=int16u(SCTOFC*SCSFET*a1+SCTOFC*a2+a3);// hardware-channel
  swid=sidlst[hwch]; // software-id LBBS
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;
  return swid;
}
//------
//  function to get hardware-id (CST) for given software-chan (layer, bar, side)
//                                                              0-3   0-13  0-1
//
int16u AMSTOFRawEvent::sw2hwid(int16u a1, int16u a2, int16u a3){
  static int16u first(0);
  integer i;
  int16u ilay,ibar,isid,swch,swid,hwid,crate,sfet,tofc;
  static int16u hidlst[SCLRS*SCMXBR*2]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<SCLRS);// layer(0-3)
  assert(a2>=0 && a2<SCMXBR);// bar(0-13)
  assert(a3>=0 && a3<2); // side(0-1)
#endif
//
  if(first==0){ // create hardw.id list:
    first=1;
    for(i=0;i<SCLRS*SCMXBR*2;i++)hidlst[i]=0;
    for(crate=0;crate<SCCRAT;crate++){
      for(sfet=0;sfet<SCSFET;sfet++){
        for(tofc=0;tofc<SCTOFC;tofc++){
          hwid=100*(crate+1)+10*(sfet+1)+tofc+1;// CrateSfetTofch (CST)
          swid=AMSTOFRawEvent::hw2swid(crate,sfet,tofc);// LBBS
          if(swid>0 && swid<=4142){// legal swid 
            ilay=swid/1000-1;
            ibar=(swid/10)%100-1;
            isid=swid%10-1;
            swch=2*SCMXBR*ilay+2*ibar+isid;
            hidlst[swch]=hwid;
          }
        }// end of tofch-loop
      }// end of sfet-loop
    }// end of crate-loop
  }// end of first
// 
  swch=2*SCMXBR*a1+2*a2+a3;
  hwid=hidlst[swch];// hardware-id CST
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;
  return hwid;
}



AMSID AMSTOFCluster::crgid(int kk){
return AMSID("TOFS",_ntof*100+_plane);
}
