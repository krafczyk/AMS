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
#include <tofcalib.h>
#include <ntuple.h>
//
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// calibration data
extern TOFVarp tofvpar;// TOF general parameters
integer AMSTOFRawCluster::trpatt[SCLRS]={0,0,0,0};//just init. of static var.
integer AMSTOFRawCluster::trflag=0;
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
  int16u id,idd,idN,stat[2];
  number tsr[3];
  int bad;
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptrN;
//
  pbitn=SCPHBP;
  pbanti=pbitn-1;
  status=0;//ok
//  return;
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0,1);//last 1 to sort
  isds=0;
  
  geant charge,edep;
// cout<<"-------> In Validation :"<<" event ="<<GCFLAG.IEVENT<<endl;//tempor
//                             <---- loop over TOF RawEvent hits -----
  while(ptr){
// if(GCFLAG.IEVENT==56)ptr->_printEl(cout);//tempor
    idd=ptr->getid();
    id=idd/10;// short id=LBB, where L=1,4 BB=1,14
    ilay=id/100-1;
    ibar=id%100-1;
    isid=idd%10-1;
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
      }
      if(nhit<im){//something was wrong
        ptr->putftdc(int16u(nhit),ftdc2);// refill object with corrected data
        TOFJobStat::addch(chnum,12);
      }
//---> check stretcher-TDC :
      nhit=0;
      im=nstdc[isid];
      for(i=0;i<im;i++)stdc2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
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
        stdc2[nhit]=stdc1[i];
        nhit+=1;
        stdc2[nhit]=stdc1[i+1];
        nhit+=1;
        stdc2[nhit]=stdc1[i+2];
        nhit+=1;
        stdc2[nhit]=stdc1[i+3];
        nhit+=1;
      }
      if(nhit<im){//something was wrong
        ptr->putstdc(int16u(nhit),stdc2);// refill object with corrected data
        TOFJobStat::addch(chnum,13);
      }
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
      }
      if(nhit<im){//something was wrong
        ptr->putadca(int16u(nhit),adca2);// refill object with corrected data
        TOFJobStat::addch(chnum,14);
      }
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
      }
      if(nhit<im){//something was wrong
        cerr<<" !!! ------------------> found problem in d-TDC !!!"<<endl;
        ptr->putadcd(int16u(nhit),adcd2);// refill object with corrected data
        TOFJobStat::addch(chnum,15);
      }
//-----      
    } 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//  ---- end of RawEvent hits loop ------->
//
//------------------------------------------------------
//
// =============> Stretcher-ratio calibration, if requested :
//
  if(TOFRECFFKEY.relogic[0]==3){
    TOFJobStat::addre(15);
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
        nstdc[isid]=ptr->getstdc(stdc1);
        if(nstdc[isid]==4){ // require only one "4-edge" sTDC-hit
          tsr[0]=(stdc1[3]&pbanti)*TOFDBc::tdcbin(1);
          tsr[1]=(stdc1[2]&pbanti)*TOFDBc::tdcbin(1);
          tsr[2]=(stdc1[0]&pbanti)*TOFDBc::tdcbin(1);
          TOFSTRRcalib::fill(chnum,tsr);
        }
      }// --- end of status check --->
//
      ptr=ptr->next();// take next RawEvent hit
    }//  ---- end of RawEvent hits loop ------->
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
//------
void AMSTOFRawCluster::build(int &status){
  int16u nftdc[2],nstdc[2],nadca[2],nadcd[2];
  int16u  ftdc1[SCTHMX2*2],stdc1[SCTHMX3*4],adca1[SCTHMX4*2],adcd1[SCTHMX4*2];
  int16u  ftdc2[SCTHMX2*2],stdc2[SCTHMX3*4],adca2[SCTHMX4*2],adcd2[SCTHMX4*2];
  integer ilay,last,ibar,isid,isds;
  integer i,j,chnum,brnum,am[2],tmi[2],itmf[2],sta,st;
  integer trpatt[SCLRS];
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  int16u id,idd,idN,stat[2];
  number zc,ama[2],amd[2],qtota,qtotd,tmf[2],time,coo,ecoo;//input to RawCluster Constr
  number tm[2],tf,dt,fstd;
  number amf[2],timeD,tamp;
  number charg[2];
  number t1,t2,t3,t4;
  geant co,eco,point,brlm,pcorr,td2p,etd2p,clong[SCLRS],strr[2];
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptrN;
  integer nbrl[SCLRS],brnl[SCLRS];
  int bad,tsfl(0);
// some variables for histogramming:
  geant gdt,tch,pch1[SCLRS],pch2[SCLRS];
  geant edepa[SCLRS],edepd[SCLRS],tcorr[SCLRS],tdiff[SCLRS],td13,td24;
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                                    ->getheadC("AMSTOFRawEvent",0);
  pbitn=SCPHBP;//phase bit position
  pbanti=pbitn-1;// mask to avoid it.
  status=1;// bad
  isds=0;
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//cout<<"In TOFRawCluster: event = "<<GCFLAG.IEVENT<<endl;// tempor
//if(GCFLAG.IEVENT==56)tsfl=1;//tempor
//
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
    if(statdb[isid] == 0){  // channel alive
//       fill working arrays for given side:
      isds+=1;
      if(isid==0){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc1);
        nstdc[isid]=ptr->getstdc(stdc1);
        nadca[isid]=ptr->getadca(adca1);
        nadcd[isid]=ptr->getadcd(adcd1);
      } 
      if(isid==1){
        charg[isid]=ptr->getcharg();
        nftdc[isid]=ptr->getftdc(ftdc2);
        nstdc[isid]=ptr->getstdc(stdc2);
        nadca[isid]=ptr->getadca(adca2);
        nadcd[isid]=ptr->getadcd(adcd2);
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
    } 
//
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (LBB)
//------------------------------------------------------
    if(idN != id){ // both sides ready, next hit is OTHER_counter/last hit,
//       so process CURRENT counter data : 
//
      if(isds==2){//bar(2sides) h/w status ok -> start pattern recognition in it
        TOFJobStat::addbr(brnum,0);
// --------> select bar with 1x4-multiplicity in stdc, 1x2 in ftdc/adc :
        if(nstdc[0]==4 && nstdc[1]==4 // tempor.solution: require one "4-edge" sTDC-hit
          && nadca[0]==2 && nadca[1]==2
          && nftdc[0] >=2 && nftdc[1] >=2){
          TOFJobStat::addbr(brnum,1);
//
//   --------> identify "corresponding" hit in fast TDC :
          scbrcal[ilay][ibar].gtstrat(strr);
//   calculate f-TDC "start"-edge (each-side) times tm[2] (wrt lev1):
          tm[0]=(stdc1[3]&pbanti)*TOFDBc::tdcbin(1);
          tm[1]=(stdc2[3]&pbanti)*TOFDBc::tdcbin(1);
//
          if(TOFRECFFKEY.reprtf[2]>0 && nftdc[0] ==2 && nftdc[1]==2){// histogr.
            tf=(ftdc1[1]&pbanti)*TOFDBc::tdcbin(0);//2-nd hit is leading(up)-edge
            dt=tf-tm[0];
            HF1(1100,geant(dt),1.);//hist. the same hit f/s-TDC difference
            tf=(ftdc2[1]&pbanti)*TOFDBc::tdcbin(0);
            dt=tf-tm[1];
            HF1(1100,geant(dt),1.);
            dt=(int(ftdc1[1]&pbanti)-int(ftdc1[0]&pbanti))
                                         *TOFDBc::tdcbin(0);//f-TDC inp.pulse width
            HF1(1103,geant(dt),1.);
          }
          tmf[0]=-1.;
          tmf[1]=-1.;
          fstd=scbrcal[ilay][ibar].gtfstrd(); // diff. in f/s same-hit delay
          for(i=0;i<nftdc[0];i+=2){ // side-1
            tf=(ftdc1[i+1]&pbanti)*TOFDBc::tdcbin(0);//take up-edge(2-nd) of f-TDC
            if(fabs(tf-tm[0]-fstd) < tofvpar.fstdw()){
              tmf[0]=tf;
              itmf[0]=i+1;
            }
          }                          
          for(i=0;i<nftdc[1];i+=2){ // side-2
            tf=(ftdc2[i+1]&pbanti)*TOFDBc::tdcbin(0);
            if(fabs(tf-tm[1]-fstd) < tofvpar.fstdw()){
              tmf[1]=tf;
              itmf[1]=i+1;
            }
          }
//  --------> check the presence of f-TDC history hits and                          
//                         do "befor"/"after" cuts for both sides :
          int reject=0;
          if(tmf[0]<0. || tmf[1]<0.)reject=1;// no ftdc/stdc match
//
          if(reject==0){
            j=itmf[0]-2; // Side-1 "after"(real time)-check (LIFO-readout !)
            if(j >= 0){
              tf=(ftdc1[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[0]-tf),1.);
              if((tmf[0]-tf) < tofvpar.hiscuta())reject=1;
            }
            j=itmf[1]-2; // Side-2 "after"-check 
            if(j >= 0){
              tf=(ftdc2[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1102,geant(tmf[1]-tf),1.);
              if((tmf[1]-tf) < tofvpar.hiscuta())reject=1;
            }
            j=itmf[0]+2; // Side-1 "befor"-check
            if(j < nftdc[0]){
              tf=(ftdc1[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[0]),1.);
              if((tf-tmf[0]) < tofvpar.hiscutb())reject=1;
            }
            j=itmf[1]+2; // Side-2 "befor"-check
            if(j < nftdc[1]){
              tf=(ftdc2[j]&pbanti)*TOFDBc::tdcbin(0);
              if(TOFRECFFKEY.reprtf[2]>0)HF1(1101,geant(tf-tmf[1]),1.);
              if((tf-tmf[1]) < tofvpar.hiscutb())reject=1;
            }
          }
//
//----------->>> set status of sc.bar, proc. and store it :
          if(reject==1){
            sta=1; // problem with time-history
          }
          else{
            sta=0;// ok
            TOFJobStat::addbr(brnum,2);
// -----> prepare fine resolution side-times(using stretcher info):
//
            t4=(stdc1[0]&pbanti)*TOFDBc::tdcbin(1);// 4-th edge of str-info
            t2=(stdc1[2]&pbanti)*TOFDBc::tdcbin(1);// 2-nd edge of str-info
            t1=(stdc1[3]&pbanti)*TOFDBc::tdcbin(1);// 1-st edge of str-info
            tm[0]=0.5*((t1-t4)/(strr[0]+1.)+(t2-t4)/strr[0]);// s-1 time (ns,A-noncorr)
            t4=(stdc2[0]&pbanti)*TOFDBc::tdcbin(1);// 4-th edge of str-info
            t2=(stdc2[2]&pbanti)*TOFDBc::tdcbin(1);// 2-nd edge of str-info
            t1=(stdc2[3]&pbanti)*TOFDBc::tdcbin(1);// 1-st edge of str-info
            tm[1]=0.5*((t1-t4)/(strr[1]+1.)+(t2-t4)/strr[1]);// s-2 time (ns,A-noncorr)
//
            am[0]=integer(adca1[1]&pbanti)-integer(adca1[0]&pbanti); // TovT raw values
            am[1]=integer(adca2[1]&pbanti)-integer(adca2[0]&pbanti);
            ama[0]=am[0]*TOFDBc::tdcbin(2);//TDC_counts->ns 
            ama[1]=am[1]*TOFDBc::tdcbin(2);
            HF1(1104,geant(ama[0]),1.);
            zc=TOFDBc::getzsc(ilay,ibar);
            if(TOFRECFFKEY.relogic[0]<=1){// ====> Normal,TZSL/AMPL_calib reconstruction :
//
//--> Calc. longit. coord/err and position corr. to signal :
              scbrcal[ilay][ibar].tmd2p(tm,ama,co,eco);// get A-corrected loc.coord/err
              brlm=0.5*TOFDBc::brlen(ilay,ibar)+3.*eco;//limit on max. coord
              if(fabs(co) > brlm){   //means "coord. is more than counter half length"
                pcorr=scbrcal[ilay][ibar].poscor(0.);// take position corr. as for "0"
              }
              else{
                pcorr=scbrcal[ilay][ibar].poscor(co);
              }
//--> Find Eloss from anode :
              qtota=scbrcal[ilay][ibar].ama2mip(ama)
                                             /pcorr; //an-tot Edep(mev) with corrections
//--> Find counter-time corrected for anode TovT :
              time=scbrcal[ilay][ibar].tm2t(tm,ama); // time with corrections
//--> Find Eloss from dinode :
              qtotd=0.;
              amd[0]=0.;
              amd[1]=0.;
              if(nadcd[0]==2 && nadcd[1]==2){ 
                am[0]=integer(adcd1[1]&pbanti)-integer(adcd1[0]&pbanti); // TovT raw values
                am[1]=integer(adcd2[1]&pbanti)-integer(adcd2[0]&pbanti);
                amd[0]=am[0]*TOFDBc::tdcbin(3);//TDC_counts->ns
                amd[1]=am[1]*TOFDBc::tdcbin(3);
                qtotd=scbrcal[ilay][ibar].amd2mip(amd)
                                              /pcorr;//di-tot Edep(mev) with corrections
              } 
              nbrl[ilay]+=1;
              edepa[ilay]=qtota;// store some number for histogramming :
              edepd[ilay]=qtotd;
              tcorr[ilay]=time;
              tdiff[ilay]=0.5*(tm[0]-tm[1]);// layer A-non.cor. time-diff.(ns) 
              pch1[ilay]=charg[0];
              pch2[ilay]=charg[1];
              brnl[ilay]=ibar;
              clong[ilay]=co;
              st=0;
              coo=co;
              ecoo=eco;
              st=AMSEvent::gethead()->addnext(AMSID("AMSTOFRawCluster",0)
              ,new AMSTOFRawCluster(sta,ilay+1,ibar+1,zc,ama,amd,
                                  qtota,qtotd,tm,time,coo,ecoo));//store values
            } // ---> end of normal/TZSL/AMPL-reco check
          }   // ---> end of history check
//----------->>>
        } // ---> end of "measurement-multiplicity" check
//
      }
      else{ // only one side is present in this counter
//     ???????   tempor doing nothing for that case
      } // ---> end of "both sides h/w status OK" check
      isds=0;// clear side-counter befor next bar processing 
//
    } // ---> end of "next COUNTER" or "last hit" check
//------------------------------------------------------
    ptr=ptr->next();// take next RawEvent hit
//
  } //                ---- end of RawEvent hits loop ------->
//
//
// now check min. multiplicity :
  int nbrch[SCLRS];
  for(i=0;i<SCLRS;i++){
    nbrch[i]=0;
    if(nbrl[i]>0)nbrch[i]=1;
  }
  if((nbrch[0]+nbrch[1])<1 || (nbrch[2]+nbrch[3])<1)return; // remove low layer-mult.
  status=0;
  if(TOFRECFFKEY.reprtf[2]>0){// some hist. for bars/layer=1 events)
    bad=0;
    for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
    if(bad==0){
      HF1(1529,edepa[0],1.); //layer=0 Anode-reconstructed Edep
      HF1(1526,edepa[0],1.); //layer=0 Anode-reconstructed Edep
      HF1(1530,edepa[2],1.); //layer=2 Anode-reconstructed Edep
      HF1(1527,edepa[2],1.); //layer=2 Anode-reconstructed Edep
      HF1(1531,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      HF1(1528,edepd[0],1.); //layer=0 Dinode-reconstructed Edep
      td13=tcorr[0]-tcorr[2];
      td24=tcorr[1]-tcorr[3];
      HF1(1532,td13,1.);//ToF for L0->L2
      HF1(1534,td24,1.);//ToF for L1->L3
      HF1(1544,(td13-td24),1.);
      HF1(1533,tdiff[0],1.);//layer=0
      HF1(1543,clong[0],1.);//Y-coord(loc.r.s.)
      if(AMSJob::gethead()->isSimulation()){
        geant tch;
        charg[0]=pch1[0];
        charg[1]=pch2[0];
        tch=charg[0];
        if(tch<=0.)tch=1.;
        HF1(1070,log(tch),1.);
        tch=charg[0]+charg[1];
        HF1(1071,tch,1.);
        HF1(1072,tch,1.);
      }
    }
    
  }
}
//-----------------------------------------------------------------------
void AMSTOFRawCluster::_writeEl(){
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
  // fill the ntuple
  if(AMSTOFCluster::Out( IOPA.WriteAll ||  checkstatus(AMSDBc::USED ))){
static TOFClusterNtuple TN;
static integer init=0;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"TOFClust",TN.getaddress(),
  "TOFCluster:I*4,TOFStatus:I*4,Ntof:I*4,Plane:I*4, TOFEdep:R*4, TOFTime:R*4,TOFETime:R*4,TOFCoo(3):R*4,TOFErCoo(3):R*4");

}
TN.Event()=AMSEvent::gethead()->getid();
  TN.Status=_status;
  TN.Ntof=_ntof;
  TN.Plane=_plane;
  TN.Edep=_edep;
  TN.Time=_time;
  TN.ErrTime=_etime;
  int i;
  for(i=0;i<3;i++)TN.Coo[i]=_Coo[i];
  for(i=0;i<3;i++)TN.ErrorCoo[i]=_ErrorCoo[i];
  HFNTB(IOPA.ntuple,"TOFClust");
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
  geant dummy,edep,edepl,edepa,edepd,asatl,time,etime;
  integer ntof,barn,status,plrot;
  geant barl,barw,bars,cofg,cofgl,yloc,eyloc,ylocm,c0,ct,cl;
  geant ed;
  AMSPoint coo,ecoo;
  int i,j,il,ib,ilay,ibar;
  int nclust,cllay[SCLRS];
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
       && eplane[i]>= eplane[i-1] 
       && eplane[i]>= eplane[i+1] ){ // <--- peak check (over 3-bars group)
      ptr=xptr[i];// peak pointer
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
                            +2.*TOFDBc::plnstr(7);//sc.bar transv. step
      TOFMCFFKEY.padl=bars;//redef. datacard's sc.bar transv.step
      yloc=ptr->gettimeD();// get yloc/err for "peak" bar
      eyloc=ptr->getetimeD();
      ylocm=0.5*barl+3.*eyloc;// limit on max. long. coord.
      if(fabs(yloc) > ylocm){//out of bar size
        eyloc=barl/sqrt(12.);
        yloc=0.;//at bar center
      }
      status=ptr->getstatus();// tempor: How to use it here ?
      edep=0.;
      edepl=0.;
      cofg=0.;
      cofgl=0.;
      for(j=i-1;j<i+2;j++){// calc. clust. energy/COG-transv/COG-long
       ptrr=xptr[j];   
       if(ptrr){
        edep+=eplane[j];
        cofg+=eplane[j]*(j-i);// relative to "peak" bar
        yloc=ptrr->gettimeD();
        if(fabs(yloc) < ylocm){
         edepl+=eplane[j];
         cofgl+=eplane[j]*yloc;
        }
       }
      }
      time=ptr->gettime();// (ns)
      etime=TOFMCFFKEY.TimeSigma/sqrt(2.);  //(sec !!) tempor(later put in TOFBrcal needed data!)
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
        ecoo[2]=2.*TOFDBc::plnstr(6)/sqrt(12.);//2(for secur) *bar thickness/...   
        plrot=TOFDBc::plrotm(ilay);     // =0/1-unrotated/rotated TOF-plane
        c0=TOFDBc::gettsc(ilay,ibar);   //transv.pos. of "peak" bar
        ct=cofg/edep*bars+c0;           //cluster abs. transv. coord.
        if(edepl>0.)cl=cofgl/edepl;    //cluster longit.coord.(Y-loc)
        else cl=0.; // just center of sc.bar 
//    Calculate abs. 2-D coordinates of cluster COG according to plane rotation mask:
        if(plrot==0){ // non-rotated plane
          coo[0]=ct;//clust. X-coord.
          ecoo[0]=bars/2.7;// 2.7 for security
          coo[1]=cl;//clust. abs.Y-coord.(neglect counter's long.shift)
          ecoo[1]=1.5*eyloc;// 1.5 for security
        }
        else{ // rotated plane
          coo[0]=-cl;//abs.X-coord(yloc=-xabs and neglect counter's long.shift)
          ecoo[0]=1.5*eyloc;
          coo[1]=ct;
          ecoo[1]=bars/2.7;
        }
        time=-time*1.e-9;// ( ns -> sec ,"-" for V.Shoutko fit)
        AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ilay),
        new     AMSTOFCluster(status,ntof,barn,edep,coo,ecoo,time,etime));
        nclust+=1;
        eplane[i-1]=0.;// remove used bars
        eplane[i]=0.;
        eplane[i+1]=0.; 
        i=1;// to start clust. search from the beginning
      }// ---> end of accepted_cluster calculation
//------
    }// ---> end of peak check    
  }// ---> end of buffer (i) loop (clust.search)
  if(nclust>0)cllay[il-1]=1;
//------
  }// ---> end of TOF layers (il) loop
  if((cllay[0]+cllay[1])>0 && (cllay[2]+cllay[3])>0)stat=0;
// ( at least one cluster per top/bot TOF-subsystem)
}
//===============================================================================




// Old Choutko's version, it is not used now:

void AMSTOFCluster::build(){
  for(integer kk=1;kk<5;kk++){
    AMSTOFRawCluster *ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
getheadC("AMSTOFRawCluster",0);
integer const maxpl=22;
static number xplane[maxpl];
static AMSTOFRawCluster * xptr[maxpl];
VZERO(xplane,maxpl*sizeof(number)/4);
VZERO(xptr,maxpl*sizeof(AMSTOFRawCluster*)/4);
while (ptr){
  if(ptr->getntof()==kk){
   integer plane=ptr->getplane();
#ifdef __AMSDEBUG__
   assert(plane>0 && plane < maxpl-1);
#endif
   xplane[plane]=ptr->getedep();
   xptr[plane]=ptr;
  }
 ptr=ptr->next();
}

for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > TOFRECFFKEY.Thr1 && xplane[i]>= xplane[i-1] 
    && xplane[i]>= xplane[i+1] ){
 ptr=xptr[i];
#ifdef __AMSDEBUG__
 assert(ptr!=NULL);
#endif
 integer ntof,plane,status;
 number time,edep,cofg,timed;
 AMSPoint coo,ecoo;
 plane=i;
 ntof=ptr->getntof();
 timed=ptr->gettimeD();
 status=ptr->getstatus();
 edep=0;
 cofg=0;
 for(int j=i-1;j<i+2;j++){
   edep+=xplane[j];
   cofg+=xplane[j]*(j-i);   
 }
 time=ptr->gettime();
 if(edep>TOFRECFFKEY.ThrS){
   number etime, etimed;
#if 1 
      // change to zero when Eugeni  writes a proper code
     etime=TOFMCFFKEY.TimeSigma/sqrt(2.);
     const number c=1.7e10;    //speed of light
     etimed=TOFMCFFKEY.TimeSigma/sqrt(2.)*c;
#else
     // Eugeni code here
#endif
 
   coo[2]=ptr->getz()+0.5;
   coo[0]=0;
   coo[1]=0;
   ecoo[2]=10000;   // big
   
   integer plrot=TOFDBc::plrotm(ntof-1);     // =0/1-unrotated/rotated TOF-plane
   integer ix=0;
   integer iy=0;
   if(plrot==0)ix=1;
   else iy=1;

   coo[ix]=timed;
   ecoo[ix]=etimed;
   
   geant padl=TOFDBc::plnstr(5);
   coo[iy]=cofg/edep*padl+TOFDBc::gettsc(ntof-1,i-1);
   ecoo[iy]=padl/2.7;
   AMSEvent::gethead()->addnext(AMSID("AMSTOFCluster",ntof-1),
   new     AMSTOFCluster(status,ntof,plane,edep,coo,ecoo,time,etime));
   xplane[i-1]=0;
   xplane[i]=0;
   xplane[i+1]=0; 
   i=0;
 }
 }    
 
}
  }
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
//  DAQ-interface functions :
//
// function returns number of data-words (incl. block-id) for given block/format
//
integer AMSTOFRawEvent::calcdaqlength(int16u blid){
  AMSTOFRawEvent *ptr;
  integer len(0),ncnt,nh,nzc;
  int16u idd,id,ilay,ibar,isid;
  int16u crate,rcrat,sfet,tofc,tdcc,hwid,fmt;
  integer nhits[SCSFET]={0,0,0,0};
  integer nztdcc[SCSFET]={0,0,0,0};
  integer ntoth(0);
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=blid&63;// 0-raw, 1-reduced
//  cout<<"---> In RawEv::calcdaqlen: blid="<<hex<<blid<<dec<<endl;//tempor
//  cout<<"ask crate/fmt="<<rcrat<<" "<<fmt<<endl;// tempor
//
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
      nhits[sfet]+=ptr->gettoth();// counts hits
      nztdcc[sfet]+=ptr->getnztdc();// counts nonzero tdc-channels
    } 
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
    for(sfet=0;sfet<SCSFET;sfet++){
      nh=nhits[sfet];
      nzc=nztdcc[sfet];
      ncnt=nzc/4;
      if(nzc%4 > 0)ncnt+=1;
      len=len+ncnt+nh;// nmb.of_hits + nmb.of_counter_words
    }
    return (len+1+SCSFET);// bl_id + SCSFET words of hitmasks.
  }
//
  else{ // =====> Raw format :
    for(sfet=0;sfet<SCSFET;sfet++)ntoth+=nhits[sfet];
    len=1+2*ntoth;//bl_id+(addr+tdc)*ntoth
    return len;
  }
//
}
//------------------
void AMSTOFRawEvent::builddaq(int16u blid, integer &len, int16u *p){
  integer i,j,stat,ic,icc,ichm,ichc,nzch,first(0);
  int16u ilay,ibar,isid,idd,id;
  int16u phbit,maxv,ntdc,tdc[16];
  geant charge,edep;
  integer trflag,trpatt[SCLRS];
  static int16u hidlist[SCLRS*SCMXBR*2];// list of hardw. id's
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u haddr,hbyte,hbyted;
  int16u phbtp;  
  int16u crate,rcrat,sfet,tofc,tdcc;
  AMSTOFRawEvent *ptr;
  AMSTOFRawEvent *ptlist[SCSFET][SCTOFC];
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPH;//take uniq phase-bit position used for phase-bit flag in header byte
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=blid&63;// 0-raw, 1-reduced
  hbyted=0;// default value of header byte (without phase-bit)
//
//
// ---> prepare ptlist[sfet][tofc] - list of TOFRawEvent pointers :
//
  ptr=(AMSTOFRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSTOFRawEvent",0);
  for(sfet=0;sfet<SCSFET;sfet++)
              for(tofc=0;tofc<SCTOFC;tofc++)
                           ptlist[sfet][tofc]=0;// clear pointer array
//  cout<<"-----> Encoding crate "<<dec<<rcrat<<endl;//tempor
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
//      ptr->_printEl(cout);//tempor
    }
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
//
// ---> encode block-id :
  ic=0;
  *(p+ic++)=blid;
//
//
// ---> encoding of tdc-data :
//
  for(sfet=0;sfet<SCSFET;sfet++){ // <--- SFETs loop (4)
    htmsk=0;
    ichm=ic;// bias for nonzero_tdc_channels mask
    ic+=1;
    nzch=0;// #of nonzero tdcc per sfet
    for(tdcc=0;tdcc<SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      mskb=1<<tdcc;
      tofc=tdcc/4;
      mtyp=tdcc-4*tofc+1;// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ptr=ptlist[sfet][tofc];
      ntdc=0;
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);//tempor (not final correspondance)
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
//cerr<<"TofDaqBuild:Reduced fmt, crate "<<rcrat<<" filled with "<<ic<<" words."<<endl;//tempor
//   for(i=0;i<ic;i++)cout<<hex<<(*(p+i))<<endl;//tempor
//
  } // end of "Reduced" format
//---------------------------------
  else{ // =====> Raw format :
//
// ---> encode block-id :
  ic=0;
  *(p+ic++)=blid;
//
//
// ---> encoding of tdc-data :
//
  for(sfet=0;sfet<SCSFET;sfet++){ // <--- SFETs loop (4)
    for(tdcc=0;tdcc<SCTDCC;tdcc++){// <--- TDC-channels loop (16)
      tofc=tdcc/4;
      mtyp=tdcc-4*tofc+1;// measur.type (1->ftdc,2->stdc,3->adca,4->adcd)
      ptr=ptlist[sfet][tofc];
      ntdc=0;
      haddr=(sfet<<4)|tdcc;// hit-address
      if(ptr>0){
        if(mtyp==1)ntdc=ptr->getftdc(tdc);//tempor (not final correspondance)
        if(mtyp==2)ntdc=ptr->getstdc(tdc);
        if(mtyp==3)ntdc=ptr->getadca(tdc);
        if(mtyp==4)ntdc=ptr->getadcd(tdc);
      }
      if(ntdc>0){
        for(i=0;i<ntdc;i++){
          if((tdc[i]&phbit)>0)hbyte=hbyted|phbtp;// add phase bit to header byte
          else                hbyte=hbyted;
          *(p+ic++)=(haddr<<8)|hbyte; // write header
          *(p+ic++)=tdc[i]&maxv; // write TDC-hit word (phase-bit removed)
        }
      }
    } // end of TDC-channels loop
//
  } // end of SFET loop
//cerr<<"TofDaqBuild:Raw fmt, crate "<<rcrat<<"  filled with "<<ic<<" words."<<endl;//tempor
//   for(i=0;i<ic;i++)cout<<hex<<(*(p+i))<<endl;//tempor
  } // end of "Raw" format
  len=ic-1;
}
//-------------
// function to build Raw: *p points to block-id, len - bias to first data word of subd.
// (for TOF ->1, for ANTI or CTC -> length of previous subdet. data in given block.
// On output len = data length of current subdet.block(without block_id).
//  
void AMSTOFRawEvent::buildraw(integer &len, int16u *p){
  integer i,ic,ilay,ibar,isid,lentot;
  geant edep(0.),charge(0.);
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hmsk;
  int16u swid,mtyp,hcnt,shft,nhit,nzch,n4ch,sbit;
  int16u tdca,hbyte,phbt,tofc;
  int16u phbit,maxv,phbtp; 
  int16u ids,stat,nftdc,nstdc,nadca,nadcd,blid;
  int16u ftdc[SCTHMX2*2],stdc[SCTHMX3*4],adca[SCTHMX4*2],adcd[SCTHMX4*2];
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPH;//take uniq phase-bit position used for phase-bit flag in header byte
  lentot=*(p-1);// total length of the block(incl. block_id)
//
// decoding of block-id :
//
  ic=0;
  blid=*(p+ic++);// block ID
  crate=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=blid&63;// data_type ("0"->RawTDC; "1"->ReducedTDC)
  
// cout<<"In TOFRawEvent::buildraw: blid="<<hex<<blid<<dec<<" crt/fmt="<<crate<<" "<<dtyp<<endl;//tempor
// cout<<"bias="<<len<<endl;//tempor
//
  if(dtyp==1){ // =====> reduced TDC data decoding : 
//
    for(sfet=0;sfet<SCSFET;sfet++){ // <-- SFET's loop (4)
      hmsk=*(p+ic++);
      nzch=0;
      if(hmsk>0){
        for(tdcc=0;tdcc<SCTDCC;tdcc++){ // <-- TDC-chan. loop (16)
          tofc=tdcc/4;
          swid=AMSTOFRawEvent::hw2swid(crate, sfet, tofc);// LBBS
          mtyp=tdcc-4*tofc+1;// measurement_type (1-ftdc,2-stdc,3-adca,4-adcd)
          if(tdcc%4==0){//new TOFch(4xTDCch) -> clear "edge"-counters 
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
            } //                      --> end of hit loop
            if(mtyp==1)nftdc=nhit;
            if(mtyp==2)nstdc=nhit;
            if(mtyp==3)nadca=nhit;
            if(mtyp==4)nadcd=nhit;
            nzch+=1;//count !=0 TDC-ch inside of one SFET
            n4ch+=1;//count !=0 TDC-ch inside of one "4xTDC"="TOF-ch"
          }
//
          if((tdcc+1)%4 == 0){ // complete TOF-chan.(4xTDCch) was scaned
            if(n4ch>0){// create AMSTOFRawEvent object for !=0 "4xTDC"
              n4ch=0;
              if(swid>0){
              
//            cout<<"---> Reduced fmt: creating object for swid="<<swid<<endl;//tempor
            
                ids=swid;// LBBS
                stat=0; // tempor ?
                AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0),
                new AMSTOFRawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                    nadca,adca,nadcd,adcd));
              }
              else{
         cerr<<"TOF:DAQ:read_error: found data for non-TOF TDC-channels !"<<endl;
              }
            }
          }
//
        }// ---> end of TDC-chan. loop
      }//---> end of !=0 mask check
//
    }// ---> end of SFET's loop
//
  }// end of reduced data decoding
//---------
  else if(dtyp==0){ // =====> raw TDC data decoding :
    int16u sfeto(999),tofco(999);
    n4ch=0;
    nftdc=0;//clear hit counters
    nstdc=0;
    nadca=0;
    nadcd=0;
//
//    cout<<"lentot="<<lentot<<endl;//tempor
    while(ic<lentot){ // <--- block words loop
      tdca=(*(p+ic))>>8;// get TDC-address byte
      hbyte=(*(p+ic++))&255;// get header byte
      if((hbyte & phbtp)>0)
                          phbt=1; // check/set phase-bit flag
      else
                          phbt=0;
      tdcc=tdca&15; // (0-15)
      sfet=tdca>>4; // (0-3) for TOF
      tofc=tdcc/4;// TOF-ch inside of SFET (0-3)
//   cout<<" take cr/sf/tdcc/tofc="<<crate<<" "<<sfet<<" "<<tdcc<<" "<<tofc<<endl;//tempor
//   cout<<"ic="<<ic<<endl;//tempor 
      mtyp=tdcc-4*tofc+1;// measurement_type (1-ftdc,2-stdc,3-adca,4-adcd)
//
      if((sfet != sfeto) || (tofc != tofco)){// new SFET or new TOF-channel.
//   cout<<"new sfet/tofc/subd    sf0/tofco/n4ch="<<sfeto<<" "<<tofco<<n4ch<<endl;//tempor
        if(n4ch>0){// create AMSTOFRawEvent object for previous !=0 "4xTDC"==TOFch
          swid=AMSTOFRawEvent::hw2swid(crate, sfeto, tofco);// LBBS
//      cout<<"---> Raw fmt: creating object for swid="<<swid<<endl;//tempor
          ids=swid;// LBBS
          stat=0; // tempor ?
          AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0),
          new AMSTOFRawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                  nadca,adca,nadcd,adcd));
          nftdc=0;//clear hit counters
          nstdc=0;
          nadca=0;
          nadcd=0;
          n4ch=0;
        }
        if(sfet>=SCSFET){ // end of TOF data
          ic-=1;
          break;
        } 
        sfeto=sfet;
        tofco=tofc;
      }// end of new sfet/tofc check.
//
      if(mtyp==1){
        ftdc[nftdc]=((*(p+ic++))&maxv)|(phbt*phbit);
        nftdc+=1;
      }
      if(mtyp==2){
        stdc[nstdc]=((*(p+ic++))&maxv)|(phbt*phbit);
        nstdc+=1;
      }
      if(mtyp==3){
        adca[nadca]=((*(p+ic++))&maxv)|(phbt*phbit);
        nadca+=1;
      }
      if(mtyp==4){
        adcd[nadcd]=((*(p+ic++))&maxv)|(phbt*phbit);
        nadcd+=1;
      }
      n4ch+=1;
//
//
    } // ---> end of block words loop
//--------
    if(n4ch>0){ // save last tofc(when it was last in the block) :
        swid=AMSTOFRawEvent::hw2swid(crate, sfeto, tofco);// LBBS
//      cout<<"---> Raw fmt: creating object for swid="<<swid<<endl;//tempor
        ids=swid;// LBBS
        stat=0; // tempor ?
        AMSEvent::gethead()->addnext(AMSID("AMSTOFRawEvent",0),
        new AMSTOFRawEvent(ids,stat,charge,edep,nftdc,ftdc,nstdc,stdc,
                                                nadca,adca,nadcd,adcd));
    }
  } // ---> end of raw-tdc data decoding
//-------------------------
  else{
    cout<<"TofDaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
  len=ic-1;// don't count block-id word
// cerr<<"TOFDaqRead: finaly read words ="<<len<<endl;//tempor
}
//---------------------
//  function to get sofrware-id (LBBS) for given hardware-chan (crate,sfet,tofch):
//
int16u AMSTOFRawEvent::hw2swid(int16u a1, int16u a2, int16u a3){
  int16u swid,hwch;
//
  static int16u sidlst[SCCRAT*SCSFET*SCTOFC]={// 14 LBBS's + 2 empty  per CRATE :
// crate-1, (4 SFETs)x(4 TOFCs), (layer-1, side-1) :
  1011,1021,1031,1041, 1051,1061,1071,1081, 1091,1101,1111,1121, 1131,1141,0,0,
//
// crate-2, (4 SFETs)x(4 TOFCs), (layer-1, side-2):
  1012,1022,1032,1042, 1052,1062,1072,1082, 1092,1102,1112,1122, 1132,1142,0,0,
//
// crate-3, (4 SFETs)x(4 TOFCs), (layer-2, side-1) :
  2011,2021,2031,2041, 2051,2061,2071,2081, 2091,2101,2111,2121, 2131,2141,0,0,
//
// crate-4, (4 SFETs)x(4 TOFCs), (layer-2, side-2):
  2012,2022,2032,2042, 2052,2062,2072,2082, 2092,2102,2112,2122, 2132,2142,0,0,
//
// crate-5, (4 SFETs)x(4 TOFCs), (layer-3, side-1) :
  3011,3021,3031,3041, 3051,3061,3071,3081, 3091,3101,3111,3121, 3131,3141,0,0,
//
// crate-6, (4 SFETs)x(4 TOFCs), (layer-3, side-2):
  3012,3022,3032,3042, 3052,3062,3072,3082, 3092,3102,3112,3122, 3132,3142,0,0,
//
// crate-7, (4 SFETs)x(4 TOFCs), (layer-4, side-1) :
  4011,4021,4031,4041, 4051,4061,4071,4081, 4091,4101,4111,4121, 4131,4141,0,0,
//
// crate-8, (4 SFETs)x(4 TOFCs), (layer-4, side-2):
  4012,4022,4032,4042, 4052,4062,4072,4082, 4092,4102,4112,4122, 4132,4142,0,0,
  };
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<SCCRAT);//crate(0-7)
  assert(a2>=0 && a2<SCSFET);//sfet(0-3)
  assert(a3>=0 && a3<SCTOFC);//tofch(0-3)
#endif
  hwch=int16u(SCTOFC*SCSFET*a1+SCTOFC*a2+a3);// hardware-channel
  swid=sidlst[hwch]; // software-id LBBS
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;//tempor
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
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;//tempor
  return hwid;
}
