//  $Id: tofuser02.C,v 1.3 2001/01/22 17:32:23 choutko Exp $
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
#include <tofrec02.h>
#include <antirec02.h>
#include <particle.h>
//#include <daqblock.h>
#include <tofcalib02.h>
#include <ntuple.h>
#include <time.h>
//
//
//
//=======================================================================
void TOF2User::Event(){  // some processing when all subd.info is redy (+accros)
  integer i,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector,nanti(0);
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number coo[TOF2GC::SCLRS],coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
  number ama[2],amd[2];
  number adch1[TOF2GC::SCLRS],adch2[TOF2GC::SCLRS],adcl1[TOF2GC::SCLRS],adcl2[TOF2GC::SCLRS];
  geant elosa[TOF2GC::SCLRS],elosd[TOF2GC::SCLRS];
  number am1[TOF2GC::SCLRS],am2[TOF2GC::SCLRS],am1d[TOF2GC::SCLRS],am2d[TOF2GC::SCLRS],am[2],eanti(0),eacl;
  geant ainp[2],dinp[2],cinp;
  number ltim[TOF2GC::SCLRS],tdif[TOF2GC::SCLRS],trle[TOF2GC::SCLRS],trlr[TOF2GC::SCLRS];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq,betof,lflgt;
  number sigt[4]={0.13,0.15,0.15,0.13};// time meas.accuracy 
  number cvel(29.979);// light velocity
  number eacut=0.3;// cut on E-anti (mev)
  number dscut=6.;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  TOF2RawCluster *ptr;
  AMSAntiCluster *ptra;
  uinteger Runum(0);
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//
//----
  Runum=AMSEvent::gethead()->getrun();// current run number
  TOF2JobStat::addre(21);
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
//
  while(ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    if((status&TOFGC::SCBADB1)==0 && (status&TOFGC::SCBADB3)==0){ //"good_history/good_strr" hits
      if((status&TOFGC::SCBADB2)==0 || (status&TOFGC::SCBADB5)!=0){// 2-sided or recovered
        ptr->getadch(ama);// get raw high(anode)-ampl(ADC-ch)
        ptr->getadcl(amd);// get raw low(dinode)-ampl(ADC-ch)
        adch1[ilay]=ama[0];
        adch2[ilay]=ama[1];
        adcl1[ilay]=amd[0];
        adcl2[ilay]=amd[1];
        TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// high(anode)-ADC convert to charge
        am1[ilay]=am[0];
        am2[ilay]=am[1];
        TOF2Brcal::scbrcal[ilay][ibar].adc2q(1,amd,am);// low(dynode)-ADC convert to charge
        am1d[ilay]=am[0];
        am2d[ilay]=am[1];
        nbrl[ilay]+=1;
        brnl[ilay]=ibar;
        elosa[ilay]=ptr->getedeph();
        elosd[ilay]=ptr->getedepl();
        coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
        ltim[ilay]=ptr->gettime();// get ampl-corrected time
      }
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> check  bars/layer=1 :
  bad=0;
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrl[i] != 1)bad=1;
//
// -----> check Anti-counter :
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over AMSANTIRawCluster hits
    status=ptra->getstatus();
    if(status==0){ //select only good hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      if(TFREFFKEY.reprtf[2]>0)HF1(1503,geant(eacl),1.);
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(TFREFFKEY.reprtf[2]>0)HF1(1505,geant(nanti),1.);
//
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
  TOF2JobStat::addre(22);
  if(bad==1)return; // remove events with bars/layer != 1
  TOF2JobStat::addre(23);
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    HF1(5040+i,geant(brnl[i]+1),1.);
  }
//
//------> get parameters from tracker:
//
    static number pmas(0.938),mumas(0.1057),imass;
    number pmom,mom,bet,chi2,betm,pcut[2],massq;
    number the,phi,trl,rid,err,ctran;
    integer chargeTOF,chargeTracker;
    AMSPoint C0,Cout;
    AMSDir dir(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSCharge  *pcharge;
    int ntrk,ipatt;
    ntrk=0;
    cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get TOF-matched track
    if(cptr)
           ntrk+=cptr->getnelem();
    if(TFREFFKEY.reprtf[2]>0)HF1(1506,geant(ntrk),1.);
    if(ntrk!=1)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();//get pointer of the track, used in given particle
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
      status=ptrack->getstatus();
      pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
      chargeTracker=pcharge->getchargeTracker();
      chargeTOF=pcharge->getchargeTOF();
    } 
    else {
      rid=0;
      chargeTracker=0;
      chargeTOF=0;
    }
    pmom=fabs(rid);
//
    if(TFCAFFKEY.caltyp==0){ // space calibration
      bet=pmom/sqrt(pmom*pmom+pmas*pmas);
      imass=pmas;
      pcut[0]=TFCAFFKEY.plhc[0];
      pcut[1]=TFCAFFKEY.plhc[1];
    }
    else{                     // earth calibration
      bet=pmom/sqrt(pmom*pmom+mumas*mumas);
      imass=mumas;
      pcut[0]=TFCAFFKEY.plhec[0];
      pcut[1]=TFCAFFKEY.plhec[1];
    }
    if(TFREFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1501,bet,1.);
//
    bad=0;
    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
//    if((status&16384)!=0)bad=1;// remove FalseTOFX tracks
    if(bad==1)return;
    TOF2JobStat::addre(24);
//
    bad=0;
//
// ----->  find track crossing points/angles with counters:
//
    C0[0]=0.;
    C0[1]=0.;
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      cstr[il]=cos(the);
      trlr[il]=fabs(trl);
      if(TOF2DBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take Y-cross for long.c
        ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take X-cross for long.c.
        coot[il]=-coot[il];// go to bar local r.s.
        ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
      dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc
      if(TFREFFKEY.reprtf[2]>0)HF1(1200+il,geant(dy),1.);
      dx=ctran-TOF2DBc::gettsc(il,ib);//Transv.coo_tracker-Transv.coo_scint
      if(TFREFFKEY.reprtf[2]>0)HF1(1210+il,geant(dx),1.);
      if(fabs(dx)>dscut || fabs(dy)>dscut)bad=1;//too big dist. of tof-tracker
//
      elosa[il]/=geant(fabs(cos(the)));// angle correction for dE/dX
      elosd[il]/=geant(fabs(cos(the)));
    }
    cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);//average cos from track
//
    if(bad)return;//too big difference of TOF-Tracker coord.
    TOF2JobStat::addre(25);
//
//--------> find beta TOF-times and Tracker track lengthes :
//
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
    for(il=0;il<TOF2GC::SCLRS;il++){
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
    for(il=0;il<TOF2GC::SCLRS;il++)chsq+=pow((tzer+bci*trle[il]-tdif[il])/sigt[il],2);
    chsq/=number(fpnt-2);
    betof=1./bci/cvel;
    if(TFREFFKEY.reprtf[2]>0)HF1(1502,betof,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1205,chsq,1.);
    if(TFREFFKEY.reprtf[2]>0)HF1(1206,tzer,1.);
    if(chsq>6. || betof<0.3)return;//cut on chi2/beta
//
    geant td13,td24;
    geant zpl1,zpl2,trlnor;
    zpl1=TOF2DBc::supstr(1)+TOF2DBc::supstr(7)+
        (TOF2DBc::plnstr(6)+2.*TOF2DBc::plnstr(7))/2.+TOF2DBc::plnstr(3)/2.;//z-l1-middl
    zpl2=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(6)+2.*TOF2DBc::plnstr(7))/2.-TOF2DBc::plnstr(3)/2.;//z-l2-middl
    trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
    td13=tdif[2]*trlnor/trle[2];// normalized to fix(~125cm) distance
    td24=(ltim[1]-ltim[3])*trlnor/(trle[3]-trle[1]);// normalized to fix(~125cm) distance
    if(TFREFFKEY.reprtf[2]>0)HF1(1504,(td13-td24),1.);
//
    HF1(1507,geant(chargeTOF),1.);
    HF1(1508,geant(chargeTracker),1.);
    HF2(1509,geant(chargeTracker),geant(chargeTOF),1.);
//
    geant *pntr[TOF2GC::SCLRS];
    geant avera[4],averd[2];
//    if(betof>0.9){ // dE/dX only relativistic particles
//      for(il=0;il<TOF2GC::SCLRS;il++){
//        HF1(5001+il,sqrt(elosa[il]/1.8),1.);
//        HF1(5011+il,sqrt(elosd[il]/1.8),1.);
//      }
//                  ---> look at truncated averages :
//      for(il=0;il<TOF2GC::SCLRS;il++)pntr[il]=&elosa[il];//pointers to layer edep's 
//      AMSsortNAG(pntr,TOF2GC::SCLRS);//sort in increasing order
//      avera[0]=(*pntr[0]);// lowest
//      avera[1]=avera[0]+(*pntr[1]);// sum of 2 lowest
//      avera[2]=avera[1]+(*pntr[2]);//        3 lowest
//      avera[3]=avera[2]+(*pntr[3]);// average
//      avera[1]/=2.;
//      avera[2]/=3.;
//      avera[3]/=4.;
//      if(elosd[2]<=0.)elosd[2]=9999.;
//      if(elosd[3]<=0.)elosd[3]=9999.;
//      averd[0]=0.;
//      averd[1]=0.;
//      if(elosd[2]<9999. || elosd[3]<9999.){
//        averd[0]=elosd[2];// lowest of 2 dynodes(l=3,4)
//        if(elosd[2]>elosd[3])averd[0]=elosd[3];
//      }
//      if(elosd[2]<9999. && elosd[3]<9999.)averd[1]=(elosd[2]+elosd[3])/2.;
//      for(il=0;il<TOF2GC::SCLRS;il++)HF1(5005+il,avera[il],1.);
//      if(averd[0]>0.)HF1(5015,averd[0],1.);
//      if(averd[1]>0.)HF1(5016,averd[1],1.);
//      if(avera[0]>0.)HF1(5020,log(avera[0])/2.303,1.);
//      if(avera[1]>0.)HF1(5021,log(avera[1])/2.303,1.);
//      if(avera[2]>0.)HF1(5022,log(avera[2])/2.303,1.);
//      if(averd[0]>0.)HF1(5023,log(averd[0])/2.303,1.);
//      HF1(5030,sqrt(avera[0]/1.8),1.);// eff.Z
//      HF1(5031,sqrt(avera[1]/1.8),1.);
//      HF1(5032,sqrt(avera[2]/1.8),1.);
//      HF1(5033,sqrt(avera[3]/1.8),1.);
//      if(averd[0]>0.)HF1(5034,sqrt(averd[0]/1.8),1.);
//      if(averd[1]>0.)HF1(5035,sqrt(averd[1]/1.8),1.);
//    }
//
    return;
//
}
//----------------------------
void TOF2User::InitJob(){
  int i;
  if(TFREFFKEY.reprtf[2]>0){
    HBOOK1(1500,"Part.rigidity from tracker(gv)",80,0.,32.,0.);
    HBOOK1(1501,"Particle beta(tracker)",80,0.5,1.,0.);
    HBOOK1(1502,"Particle beta(tof)",80,0.7,1.2,0.);
    HBOOK1(1504,"T13-T24(ns,high momentum)",80,-4.,4.,0.);
    HBOOK1(1503,"AntiRawCluster energy(mev)",80,0.,20.,0.);
    HBOOK1(1505,"Number of AntiRawClusters",20,0.,20.,0.);
    HBOOK1(1506,"Tracks multipl. in calib.events",10,0.,10.,0.);
    HBOOK1(1507,"TOF-charge",10,0.,10.,0.);
    HBOOK1(1508,"Ttacker-charge",10,0.,10.,0.);
    HBOOK2(1509,"TOF-ch vs Tracker-ch",10,0.,10.,10,0.,10.,0.);
    HBOOK1(1510,"Anti-hit part.index",50,0.,50.,0.);
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
//      HBOOK1(5001,"Anode SQRT(dE/dX/Mip), L=1",100,0.,10.,0.);
//      HBOOK1(5002,"Anode SQRT(dE/dX/Mip), L=2",100,0.,10.,0.);
//      HBOOK1(5003,"Anode SQRT(dE/dX/Mip), L=3",100,0.,10.,0.);
//      HBOOK1(5004,"Anode SQRT(dE/dX/Mip), L=4",100,0.,10.,0.);
//      HBOOK1(5011,"Dynode SQRT(dE/dX/Mip), L=1",100,0.,10.,0.);
//      HBOOK1(5012,"Dynode SQRT(dE/dX/Mip), L=2",100,0.,10.,0.);
//      HBOOK1(5013,"Dynode SQRT(dE/dX/Mip), L=3",100,0.,10.,0.);
//      HBOOK1(5014,"Dynode SQRT(dE/dX/Mip), L=4",100,0.,10.,0.);
//    HBOOK1(5005,"Anode dE/dX(mev),1/4",80,0.,32.,0.);
//    HBOOK1(5006,"Anode dE/dX(mev),2/4",80,0.,32.,0.);
//    HBOOK1(5007,"Anode dE/dX(mev),3/4",80,0.,32.,0.);
//    HBOOK1(5008,"Anode dE/dX(mev),4/4",80,0.,32.,0.);
//    HBOOK1(5015,"Dynode dE/dX(mev),1/2",80,0.,32.,0.);
//    HBOOK1(5016,"Dynode dE/dX(mev),2/2",80,0.,32.,0.);
//    HBOOK1(5020,"Anode Log10(dE/dX(mev)),1/4",80,-0.2,2.2,0.);
//    HBOOK1(5021,"Anode Log10(dE/dX(mev)),2/4",80,-0.2,2.2,0.);
//    HBOOK1(5022,"Anode Log10(dE/dX(mev)),3/4",80,-0.2,2.2,0.);
//    HBOOK1(5023,"Dynode Log10(dE/dX(mev)),1/2",80,-0.2,2.2,0.);
//    HBOOK1(5030,"Anode Z , 1/4",100,0.,10.,0.);
//    HBOOK1(5031,"Anode Z , 2/4",100,0.,10.,0.);
//    HBOOK1(5032,"Anode Z , 3/4",100,0.,10.,0.);
//    HBOOK1(5033,"Anode Z , 4/4",100,0.,10.,0.);
//    HBOOK1(5034,"Dynode Z , 1/2",100,0.,10.,0.);
//    HBOOK1(5035,"Dynode Z , 2/2",100,0.,10.,0.);
//
    HBOOK1(5040,"L=1,Fired bar number",14,1.,15.,0.);
    HBOOK1(5041,"L=2,Fired bar number",14,1.,15.,0.);
    HBOOK1(5042,"L=3,Fired bar number",14,1.,15.,0.);
    HBOOK1(5043,"L=4,Fired bar number",14,1.,15.,0.);
  }
  return;
}
//===================================================================
void TOF2User::EndJob(){
  int i;
  char chopt[6]="qb";
  char chfun[6]="g";
  char chopt1[5]="LOGY";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//
  if(TFREFFKEY.reprtf[2]==0)return;
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
  HPRINT(1507);
  HPRINT(1508);
  HPRINT(1509);
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
  HPRINT(1505);
  HPRINT(1510);
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5040+i);
//  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5001+i);
//  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5011+i);
//  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5005+i);
//  HPRINT(5015);
//  HPRINT(5016);
//  HIDOPT(5020,chopt1);
//  HPRINT(5020);
//  HIDOPT(5021,chopt1);
//  HPRINT(5021);
//  HIDOPT(5022,chopt1);
//  HPRINT(5022);
//  HIDOPT(5023,chopt1);
//  HPRINT(5023);
//  HIDOPT(5030,chopt1);
//  HPRINT(5030);
//  HIDOPT(5031,chopt1);
//  HPRINT(5031);
//  HIDOPT(5032,chopt1);
//  HPRINT(5032);
//  HIDOPT(5033,chopt1);
//  HPRINT(5033);
//  HIDOPT(5034,chopt1);
//  HPRINT(5034);
//  HIDOPT(5035,chopt1);
//  HPRINT(5035);
  return;
}
