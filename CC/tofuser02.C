//  $Id$
#include "tofdbc02.h"
#include "point.h"
#include "event.h"
#include "trigger102.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "tofsim02.h"
#include "trrec.h"
#include "ecalrec.h"
#include "tofrec02.h"
#include "antirec02.h"
#include "particle.h"
#include "tofid.h"
//#include <daqblock.h>
#include "tofcalib02.h"
//#include "ntuple.h"
#include <time.h>
//
//
//
//=======================================================================
void TOF2User::Event(){  // some processing when all subd.info is redy (+accros)
  integer i,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector;
  integer nanti(0),nantit(0);
  integer il,ib,ix,iy,chan,nbrlc[TOF2GC::SCLRS],brnlc[TOF2GC::SCLRS];
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number coo[TOF2GC::SCLRS],coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
  number ama[2],amd[2];
  number adca1[TOF2GC::SCLRS],adca2[TOF2GC::SCLRS];
  number adcd1[TOF2GC::SCLRS],adcd2[TOF2GC::SCLRS];
  geant elosa[TOF2GC::SCLRS],elosd[TOF2GC::SCLRS],elosc[TOF2GC::SCLRS];
  number am1[TOF2GC::SCLRS],am2[TOF2GC::SCLRS];
  number am[2],eanti(0),eacl;
  number am1d[TOF2GC::SCLRS],am2d[TOF2GC::SCLRS];
  geant ainp[2],dinp[2],cinp;
  number ltim[TOF2GC::SCLRS],tdif[TOF2GC::SCLRS],trle[TOF2GC::SCLRS],trlr[TOF2GC::SCLRS];
  number ldif[TOF2GC::SCLRS];
  number tm[2];
  number tmss[TOF2GC::SCLRS];
  number cltim[TOF2GC::SCLRS];
  integer clmem[TOF2GC::SCLRS];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq,betof,lflgt;
  number sigt[4]={0.15,0.15,0.15,0.15};// time meas.accuracy 
  number cvel(29.979);// light velocity
  number eacut=0.3;// cut on E-anti (mev)
  int16u otyp,mtyp,crat,slot,tsens;
  integer swid,hwid,shwid;
  number temper,stimes[4],strr,offs,tinp,tout;
  number tinpp,toutp,first(0);
  integer brnum;
  TOF2RawSide *ptrt;
  TOF2RawCluster *ptr;
  AMSTOFCluster *ptrc;
  AMSAntiCluster *ptra;
  Anti2RawEvent *ptrr;
  Anti2RawEvent *ptrrN;
  Trigger2LVL1 * plvl1;
  AMSPoint clco[TOF2GC::SCLRS];
  uinteger Runum(0),Evnum(0);
  ptrt=(TOF2RawSide*)AMSEvent::gethead()->
                           getheadC("TOF2RawSide",0);
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
  ptrr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
  AMSmceventg *mcptr=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
  number mcmom,mcmas,mcbeta;
//
//----
  Runum=AMSEvent::gethead()->getrun();// current run number
  Evnum=AMSEvent::gethead()->getid();
  TOF2JobStat::addre(21);
//---> some trigger study:
  plvl1=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
  integer JMembPatt=plvl1->getJMembPatt();
  integer PhysBPatt=plvl1->getPhysBPatt();
/*
//  cout<<"---> JMembPatt="<<hex<<JMembPatt<<dec<<endl;
//  for(i=15;i>=0;i--)cout<<(plvl1->JMembPattBitSet(i))<<"|";
//  cout<<endl;
  if((plvl1->JMembPattBitSet(1)) || (plvl1->JMembPattBitSet(11))){
    cout<<"------>JMBPok:run/ev="<<Runum<<" "<<Evnum<<endl;
    cout<<"      Instant Lev1PhysBranchesPattern(masked ?) :"<<endl;
    cout<<"|  FTC| Z>=1| Z>=2|Z>=2s|Elect|Gamma|  FTE|Extrn|"<<endl;  
    for(i=0;i<8;i++){
      if((PhysBPatt&(1<<i))>0)cout<<"   X  ";
      else {cout<<"   0  ";}
    }
    cout<<endl<<endl;
  }
  if(((PhysBPatt&1)>0) || ((PhysBPatt&(1<<6))>0) || PhysBPatt==0){
    cout<<"------>PhMBPok:run/ev="<<Runum<<" "<<Evnum<<endl;
    cout<<"      Instant Lev1MembersPattern :"<<endl; 
    cout<<"| FTC|FTP0|FTP1|FTT0|FTT1| FTZ| FTE|ACC0|ACC1|  BZ|ECFA|ECFO|ECAA|ECAO|EXG0|EXG1|"<<endl;
    for(i=0;i<16;i++){
      if((JMembPatt&1<<i)>0)cout<<"  X  ";
      else cout<<"  0  ";
    }
    cout<<endl<<endl;
  }
*/
//
  integer rclstok[TOF2GC::SCLRS],clstok[TOF2GC::SCLRS];
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrl[i]=0;
    nbrlc[i]=0;
    brnl[i]=-1;
    brnlc[i]=-1;
    tmss[i]=0;
    clmem[i]=0;
    cltim[i]=0;
    clco[i]=0;
    coo[i]=0;
    rclstok[i]=0;//bad
    clstok[i]=0;//bad
  }
//
//
//==================================> look at TOF2RawSide-hits(test for temper.study):
//
  while(ptrt){ // <--- loop over TOF2RawSide hits
    swid=ptrt->getsid();//LBBS
    hwid=ptrt->gethidt();//CSIIII(Cr(1-4)|SeqSlot(1-5)|Ich(1-5)|FTIch(6)|SumHTIch(7)|SumSHTIch(8))
    shwid=hwid/10000;
    crat=shwid/10;
    slot=shwid%10;//SeqSlot(1,2,3,4,5=>SFET1,SFET2,SFET3,SFET4,SFEA)
//    ptrt->getstdc(stimes);
    temper=ptrt->gettempT();//SFET-based temperature
//    tinp=stimes[0]-stimes[1];
//    tout=stimes[1]-stimes[3];
//    cout<<"swid/hwid="<<swid<<" "<<hwid<<"  tin/out="<<tinp<<" "<<tout<<"  temp="<<temper<<endl;
    if(TFREFFKEY.reprtf[1]>0){
      if(swid==1041){
//        cout<<"swidOK, dti/dto="<<fabs(tinp-tinpp)<<" "<<fabs(tinp-tinpp)<<endl;
//	if(fabs(tinp-tinpp)>5 && fabs(tinp-tinpp)<200){//curr-prev. meas. ok
//	  strr=(tout-toutp)/(tinp-tinpp);
//	  offs=tout-strr*tinp;
//#pragma omp critical (hf2)
//{
//          HF2(1120,geant(tinp),geant(tout),1.);
//          HF2(1121,geant(temper),geant(strr),1.);
//          HF2(1122,geant(temper),geant(offs),1.);
//}
//	}
//	tinpp=tinp;
//	toutp=tout;
      }
    }
    ptrt=ptrt->next();
  }// --- end of hits loop --->
//
//===================================> take Tof RawCluster-hits :
//
  while(ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
//    if((ibar==0 || (ibar+1)==TOF2DBc::getbppl(ilay)))goto next;
    if((status&TOFGC::SCBADB1)==0 && (status&TOFGC::SCBADB3)==0 //"good_history/good_in_DB
                                  && (status&TOFGC::SCBADB2)==0  // both sides measurement
                                                               )rclstok[ilay]=1;
    
    elosa[ilay]=ptr->getedepa();
    ptr->getadca(ama);// Anode-ampl(ADC-ch)
    if(elosa[ilay]>TFREFFKEY.Thr1){//apply the same thresh. as for cluster
//    if(ama[0]>10 && ama[1]>10){//apply thresh. on side ampl
      ptr->getadcd(amd);// Dynode(equiliz.sum)-ampl(ADC-ch)
      adca1[ilay]=ama[0];
      adca2[ilay]=ama[1];
      adcd1[ilay]=amd[0];
      adcd2[ilay]=amd[1];
      TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// Anode-ADC convert to charge
      am1[ilay]=am[0];
      am2[ilay]=am[1];
      am[0]=0;
      am[1]=0;
      if(amd[0]>0 && amd[1]>0)TOF2Brcal::scbrcal[ilay][ibar].adc2q(1,amd,am);// dynode(sum)-ADC convert to charge
      am1d[ilay]=am[0];
      am2d[ilay]=am[1];
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      elosd[ilay]=ptr->getedepd();
      coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
      ltim[ilay]=ptr->gettime();// get ampl-corrected time
      ptr->getsdtm(tm);// get raw side-times(A-noncorrected)
      tmss[ilay]=0.5*(tm[0]+tm[1]);// slew-non-corrected side time sum
    }
//next:
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> check TofRawClusters/layer low ?
  bool TofRClMultOK=true;
//
//  cout<<"------->RawClusters:"<<endl;
//  for(i=0;i<TOF2GC::SCLRS;i++){
//    cout<<"   L/B="<<i+1<<" "<<brnl[i]+1<<" Ncls="<<nbrl[i]<<"  stat="<<rclstok[i]<<endl;
//  }
//
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrl[i] != 1)TofRClMultOK=false;
//  for(i=0;i<TOF2GC::SCLRS;i++)if(rclstok[i]==0)TofRClMultOK=false;//require status ok also
  if(TofRClMultOK)TOF2JobStat::addre(30);
//  if(TofRClMultOK)cout<<"    RclMult OK"<<endl;
//  if(!TofRClMultOK)return;//tempor
//
//=====================================> check TofCluster-hits :
//
  integer rclmem[4][3]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  integer nrcls;
  AMSlink * mptr[3];
  for(ilay=0;ilay<TOF2GC::SCLRS;ilay++){// <--- layers loop (TofClus containers) ---
    ptrc=(AMSTOFCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFCluster",ilay,0);
    while(ptrc){ // <--- loop over AMSTofCluster hits in L=ilay
      ibar=(ptrc->getplane())-1;
      status=ptrc->getstatus();
      if((status&TOFGC::SCBADB1)==0 && (status&AMSDBc::BAD)==0     //good_hist/good4t 
                                                                  )clstok[ilay]=1;
      elosc[ilay]=ptrc->getedep();//continious
      clco[ilay]=ptrc->getcoo();
      cltim[ilay]=-(ptrc->gettime())*1.e+9;//(back from V.C. format) 
      clmem[ilay]=ptrc->getnmemb();
      nrcls=ptrc->getmemb(mptr);
      for(i=0;i<clmem[ilay];i++){
	rclmem[ilay][i]=((TOF2RawCluster*)mptr[i])->getplane();
      }
      nbrlc[ilay]+=1;
      brnlc[ilay]=ibar;
      ptrc=ptrc->next();
    }//--->endof bar-loop in given layer
  }//--->endof layer-loop
//
//------> check  TofClust/layer=low :
  bool TofClMultOK=true;
  bool TofClOneMem(true);
  geant zcoo;
//
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrlc[i] != 1)TofClMultOK=false;
  for(i=0;i<TOF2GC::SCLRS;i++)if(clstok[i]==0)TofClMultOK=false;//require good status also
  for(i=0;i<TOF2GC::SCLRS;i++)if(clmem[i] != 1)TofClOneMem=false;
  if(TofClMultOK)TOF2JobStat::addre(31);
//  if(!TofClMultOK)return;// remove events with bad mult. of Tof-Clusters/layer 
//=========================================> TofPattern special selection for beta><0 probl:
//  if(!(((brnl[1]+1)==6 || (brnl[1]+1)==7) && ((brnl[2]+1)==3 || (brnl[2]+1)==4)))return;//tempor**2
//  if(!(((brnl[0]+1)==4 || (brnl[0]+1)==5) && ((brnl[3]+1)==4 || (brnl[3]+1)==5)))return;//tempor**2
  TOF2JobStat::addre(50);
//======================================> Ecal-test:
  AMSEcalShower * ptsh;
  number ecshen,ecshener,efront,chi2dir,difosum,ecshsleak,ecshrleak,ecshdleak,ecsholeak;
  AMSEcalHit * ptr1;
  integer maxpl,nhtot(0);
  geant padc[3];
  integer cid,isl,pmt,sbc,cell,proj;
  maxpl=2*ECALDBc::slstruc(3);// SubCell(Pix) planes(18)
  int  ecshnum(0);
  ptsh=(AMSEcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0);
  while(ptsh){ // <------- ecal shower
    ecshnum++;
    ptsh=ptsh->next();
  }
  if(ecshnum==1){// only use event with one shower
    ptsh=(AMSEcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0);
    ecshen=ptsh->getEnergyC();
#pragma omp critical (hf1)
{
    HF1(5050,ecshen,1.);
    HF1(5051,ecshen,1.);
    HF1(5052,ecshen,1.);
}
//----------------    
    nhtot=0;
    for(int ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell(pix)-Planes loop(0-17)
      ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);

      while(ptr1){ // <--- EcalHits(fired subcells=pixels) loop in pix-plane:
        cid=ptr1->getid();//LTTP(sLayer/pmTube/Pixel)
        sbc=cid%10-1;//SubCell(0-3)
        proj=ptr1->getproj();//0/1->X/Y (have to be corresponding to ipl)
        cell=ptr1->getcell();// 0,...71
        isl=ipl/2;//0-8
        pmt=cell/2;//0-35
        ptr1->getadc(padc);//get raw ampl (Ah,Al,Ad already ovfl-corrected)
        if(padc[0]>0){
          nhtot+=1;
#pragma omp critical (hf1)
{
	  if(ipl==0)HF1(5054,geant(cell+1),1.);
	  if(ipl==2)HF1(5055,geant(cell+1),1.);
}
        }
        ptr1=ptr1->next();  
      } // ---> end of EcalHits loop in pixPlane
    }//---> end of PixPlanes-loop
#pragma omp critical (hf1)
{
    HF1(5053,geant(nhtot),1.);
}
  }//--->endof Nshow=1 check
//
// ========================================> check Anti-counter :
// -->RawEvent:
// 
  integer ntdct,tdct[ANTI2C::ANTHMX],nftdc,ftdc[TOF2GC::SCTHMX1];
  geant adca;
  int16u id,idN,sta;
  number ampe[2],uptm[2];
  number ama1[ANTI2C::MAXANTI],ama2[ANTI2C::MAXANTI];
  integer frsecn[ANTI2C::MAXANTI],frsect;
  integer j,jmax,isid,nsds,stat,chnum,n1,n2,i1min,i2min;
  geant ftdel[2],padlen,padrad,padth,padfi,paddfi,ped[2],sig[2];
  int nphsok;
  padlen=ANTI2DBc::scleng();//z-size
  padrad=ANTI2DBc::scradi();//int radious
  padth=ANTI2DBc::scinth();//thickness
  paddfi=360./ANTI2C::MAXANTI;//per logical sector
//
  frsect=0;
  nsds=0;
  ntdct=0;
  uptm[0]=0;
  uptm[1]=0;
  ampe[0]=0;
  ampe[1]=0;
  for(i=0;i<ANTI2C::MAXANTI;i++)frsecn[i]=0;
  for(i=0;i<ANTI2C::MAXANTI;i++)ama1[i]=0;
  for(i=0;i<ANTI2C::MAXANTI;i++)ama2[i]=0;
//
  uinteger accpat=Anti2RawEvent::getpatt();
//
  while(ptrr){ // <------ RawEvent hits loop
    id=ptrr->getid();//BBS
    sector=id/10-1;//Readout(logical) sector number (0-7)
    isid=id%10-1;
    chnum=sector*2+isid;//channels numbering
    adca=ptrr->getadca();
    ntdct=ptrr->gettdct(tdct);
    nftdc=ptrr->getftdc(ftdc);
    if(ntdct>=0 && nftdc==1){//select only >=1 Hist/FT-hit events
//DAQ-ch-->p.e's:
      ped[isid]=ANTIPeds::anscped[sector].apeda(isid);//adc-chan
      sig[isid]=ANTIPeds::anscped[sector].asiga(isid);//adc-ch sigmas
//      ampe[isid]=number(adca)/ANTI2DBc::daqscf()       //DAQ-ch-->ADC-ch
//              *ANTI2SPcal::antispcal[sector].getadc2pe(); //ADC-ch-->p.e.
      ampe[isid]=number(adca);       
//cout<<"    decoded signal="<<ampe[isid]<<endl; 
//TDC-ch-->time(ns):
      nphsok=ANTI2VPcal::antivpcal[sector].NPhysSecOK();
      if(nphsok==2)tzer=ANTI2SPcal::antispcal[sector].gettzerc();
      else tzer=ANTI2SPcal::antispcal[sector].gettzer(nphsok);
      uptm[isid]=(ftdc[0]-tdct[0])*ANTI2DBc::htdcbw() + tzer;//TDC-ch-->ns + compens.tzero
//cout<<"    decoded Up-time="<<uptm[isid]<<endl;
//
      nsds+=1;
    }//--->endof 1-hit check
//--------
    ptrrN=ptrr->next();
    idN=0;
    if(ptrrN)idN=(ptrrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
//
      if(nsds>0 
                && ((ampe[0]>20) || (ampe[1]>20))
//                && ((ampe[0]>5*sig[0] && uptm[0]>0) || (ampe[1]>5*sig[1] && uptm[1]>0))
//                && ((ampe[0]>5*sig[0]) && (ampe[1]>5*sig[1]))
		                                                               ){//good sector ?
	 frsecn[sector]=1;//store fired sect.number
	 ama1[sector]=ampe[0];//store signals(pe's)
	 ama2[sector]=ampe[1];
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
    ptrr=ptrr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//------- for ESTEC:
#pragma omp critical (estec)
{
  geant acctrthr=100.;
  for(i=0;i<8;i++){
    if((accpat&(1<<i))>0 && frsecn[i]==1){
      HF1(1289,geant(i+1),1.);
      if(ama1[i]>acctrthr && ama2[i]>acctrthr)HF1(1289,geant(10+i+1),1.);
      if(ama1[i]>acctrthr || ama2[i]>acctrthr)HF1(1289,geant(20+i+1),1.);
    }
  }
}
//-------
// ---> Cluster:
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over ANTICluster hits
    status=ptra->getstatus();
    if((status & TOFGC::SCBADB1)==0){ //select only important(=FTCoincident) hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
        HF1(1503,geant(eacl),1.);
        if(sector<8 && sector>=0){
	  HF1(15001+sector,geant(eacl),1.);
	}
}
      }
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
    HF1(1505,geant(nanti),1.);
    if(TofClMultOK)HF1(1514,geant(nanti),1.);
}
  }
  nantit=Anti2RawEvent::getncoinc();//same from trigger
  if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
    HF1(1517,geant(nantit),1.);
}
  }
//====================================
//  if(nanti>4)return;// remove events with >1 sector(e>ecut) in Anti
  TOF2JobStat::addre(22);
//====================================
//
//  if(TFREFFKEY.reprtf[1]>0){
//#pragma omp critical (hf1)
//{
//    for(i=0;i<TOF2GC::SCLRS;i++){
//      if(brnl[i]>=0)HF1(5040+i,geant(brnl[i]+1),1.);
//    }
//}
//  }
//
//===================================> get parameters from tracker:
//
    number pmas(0.938),mumas(0.1057),imass;
    number pmom,mom,bet,chi2,betm,pcut[2],massq,beta,chi2t,chi2s;
    number momentum;
    number the,phi,trl,rid,err,ctran,charge,partq;
    integer chargeTOF(0),chargeTracker(0),betpatt(-1),trpatt,trhits(0);
    uintl traddr(0,0);
    integer ilad[2][8]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    AMSPoint C0,Cout;
    AMSDir dir(0,0,1.);
    AMSPoint cooCyl(0,0,0);
    AMSPoint crcCyl;
    AMSDir dirCyl(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSTrTrack *ptrack_r;
    AMSTRDTrack *ptrd;
    AMSCharge  *pcharge;
    AMSBeta * pbeta;
    int npart,ipatt,envindx(0);
    bool trktr,trdtr,ecaltr,nottr,badint;
    number toftrlen[TOF2GC::SCLRS]={0,0,0,0};
//
    npart=0;
    for(i=0;i<2;i++){//i=0->parts.with true(Trk)-track, i=1->...false(nonTrk)-track
      cptr=AMSEvent::gethead()->getC("AMSParticle",i);// get pointer to part-envelop "i"
      if(cptr){
        if(cptr->getnelem()!=0){
          npart+=cptr->getnelem();
	  envindx=i;
          break;//use 1st non-empty envelop
        }
      }
    }

    if(npart<1)return;// require events with 1 particle(in envel) at least 
    TOF2JobStat::addre(24);
//
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",envindx);
    bool TrkTrPart=false;
    bool AnyTrPart=false;
    bool GoodTrPart=false;
    bool GoodTrkTrack=false;

//			      
    while(ppart){//<--- loop over envelop content
      partq=ppart->getcharge();
      ptrack=ppart->getptrack();//get pointer of the Track used in given particle
      ptrd=ppart->getptrd();//get pointer of the TRD-track, used in given particle
      if(ptrd)TOF2JobStat::addre(42);
      if(ptrack){
        AnyTrPart=true;
        TOF2JobStat::addre(25);
        trdtr=(ptrack->checkstatus(AMSDBc::TRDTRACK)!=0);
        ecaltr=(ptrack->checkstatus(AMSDBc::ECALTRACK)!=0);
        nottr=(ptrack->checkstatus(AMSDBc::NOTRACK)!=0);
        badint=(ptrack->checkstatus(AMSDBc::BADINTERPOL)!=0);
        if(trdtr)TOF2JobStat::addre(26);
        if(ecaltr)TOF2JobStat::addre(27);
        if(nottr)TOF2JobStat::addre(28);
        if(badint)TOF2JobStat::addre(29);
	if(nottr || ecaltr || badint)goto Nextp;
	GoodTrPart=true;
	TOF2JobStat::addre(40);//good track part(trd|trk)
	if(!trdtr){//tracker-track part
	  TrkTrPart=true;
	  TOF2JobStat::addre(41);
          for(i=0;i<TOF2GC::SCLRS;i++)toftrlen[i]=ppart->gettoftlen(i);//exists only for trk-track
          trpatt=ptrack->getpattern();//TRK-track pattern
#ifdef _PGTRACK_
	  if(!(ptrack->IsFake()))
#else
	  if(trpatt>=0)
#endif
	  {//trk-track ok
            GoodTrkTrack=true;
#ifdef _PGTRACK_
	    //PZ FIXME UNUSED 	    traddr=ptrack->getaddress();//TRK-track ladders combination id
	    trhits=ptrack->GetNhits();
#else

	    traddr=ptrack->getaddress();//TRK-track ladders combination id
	    trhits=ptrack->getnhits();
#endif
            ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
            status=ptrack->getstatus();
            pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
            pbeta=ppart->getpbeta();
	    momentum=ppart->getmomentum();
            betpatt=pbeta->getpattern();
            beta=pbeta->getbeta();
            chi2t=pbeta->getchi2();
            chi2s=pbeta->getchi2S();
            chargeTracker=pcharge->getchargeTracker();
            chargeTOF=pcharge->getchargeTOF();
	    charge=partq;
	    TOF2JobStat::addre(43);
//	     if(trhits==8){
//	       AMSTrTrack::decodeaddress(ilad,traddr);
//	       cout<<"TofUser:TRK-address/pattern="<<traddr<<" "<<trpatt<<endl;
//	       cout<<"        Address(decoded):"<<endl;
//	       for(i=0;i<8;i++)cout<<ilad[0][i]<<" ";
//	       cout<<endl;
//	       for(i=0;i<8;i++)cout<<ilad[1][i]<<" ";
//	       cout<<endl;
//	     }
          }//endof "trk-track ok"
	}//endof "tracker, not trd-part"
      }//--->endof "track-part exists"
//      if(GoodTrPart)break;//accept 1st particle with any good track
      if(GoodTrkTrack)break;//accept 1st particle with good Trk-track
Nextp:
      ppart=ppart->next();
    }//--->endof envel.content loop
     
//==================================================
    if(!GoodTrPart)return;//require part. with good(good interp, TRD|TRK) Track
    TOF2JobStat::addre(44);
//
    if(GoodTrkTrack)TOF2JobStat::addre(45);
    if(!GoodTrkTrack)return;//require part. with good TRK-Track
    TOF2JobStat::addre(49);//Finally accepted 
//----
    if(AMSJob::gethead()->isSimulation()){
      mcmom=mcptr->getmom();
      mcmas=mcptr->getmass();
      mcbeta=mcmom/sqrt(mcmom*mcmom+mcmas*mcmas);
      if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
        HF1(1518,mcbeta,1.);
}
      }
    }
// more track-cuts:
    pmom=fabs(rid);
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
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
      if(beta>0)HF1(1500,geant(rid),1.);
      if(beta<0)HF1(1516,geant(rid),1.);
      HF1(11516,geant(rid),1.);
}
    }
//
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
    if(betpatt<5){//>=3of4
      if(beta<0)HF1(1501,beta,1.);
      else HF1(11501,beta,1.);
      HF1(1520,beta,1.);
      if(beta>0)HF1(1280,bet,1.);//using implied mass amd measured mom.
      else HF1(1281,bet,1.);
      HF1(1511,chi2t,1.);
      HF1(1512,chi2s,1.);
      HF1(1513,chi2,1.);
    }
}
    }
    bad=0;
//    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
    if((status&16384)!=0)bad=1;// remove FalseTOFX tracks
//    if(bad==1)return;
//
    TOF2JobStat::addre(46);//TrackParamsOK
//=========================================================================
//-------------------------------------------> beta><0 problem study:
//  if(beta<0)return;//tempor**2
//--------------------------------
//  if(!(chargeTracker==1 && momentum>10.))return;//tempor to check resol
//
  if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
    for(i=0;i<TOF2GC::SCLRS;i++){
      if(brnl[i]>=0)HF1(5040+i,geant(brnl[i]+1),1.);
    }
}
  }
/*
    number FtCrAver[4]={0,0,0,0};
    integer FtCrSlts[4]={0,0,0,0};
    integer fthits;
    number fttime;
    for(ilay=0;ilay<4;ilay++){
      ibar=brnl[ilay];//0->
      if(ibar<0)continue;//empty layer
      mtyp=0;
      otyp=0;
      AMSSCIds tofida1(ilay,ibar,0,otyp,mtyp);//otyp=0(Anode),mtyp=0(LT)
      crat=tofida1.getcrate();//current crate#(0,1,2,3)
      slot=tofida1.getslot();//sequential slot#(0,1,...10)(4 last are fictitious to identify SFECs)
      tsens=tofida1.gettempsn();//... sensor#(1,2,...,5)(not all slots have temp-sensor!)
      slot=tsens-1;
      fthits=TOF2RawSide::FThits[crat][slot];
      if(fthits==1){
	fttime=number(TOF2RawSide::FTtime[crat][slot][0]*TOF2DBc::tdcbin(1));
	FtCrSlts[crat]+=1;
	FtCrAver[crat]+=fttime;
      }
      mtyp=0;
      otyp=0;
      AMSSCIds tofida2(ilay,ibar,1,otyp,mtyp);//otyp=0(Anode),mtyp=0(LT)
      crat=tofida2.getcrate();//current crate#(0,1,2,3)
      slot=tofida2.getslot();//sequential slot#(0,1,...10)(4 last are fictitious to identify SFECs)
      tsens=tofida2.gettempsn();//... sensor#(1,2,...,5)(not all slots have temp-sensor!)
      slot=tsens-1;
      fthits=TOF2RawSide::FThits[crat][slot];
      if(fthits==1){
	fttime=number(TOF2RawSide::FTtime[crat][slot][0]*TOF2DBc::tdcbin(1));
	FtCrSlts[crat]+=1;
	FtCrAver[crat]+=fttime;
      }
    }
//
    for(int cr=0;cr<4;cr++){
      if(FtCrSlts[cr]>0){
        FtCrAver[cr]/=FtCrSlts[cr];
      }
    }
    if(beta>0.8 && beta<1.2){
      if(FtCrAver[0]>0 && FtCrAver[2]>0)HF1(1290,geant(FtCrAver[0]-FtCrAver[2]),1.);
      if(FtCrAver[0]>0 && FtCrAver[3]>0)HF1(1291,geant(FtCrAver[0]-FtCrAver[3]),1.);
      if(FtCrAver[1]>0 && FtCrAver[2]>0)HF1(1292,geant(FtCrAver[1]-FtCrAver[2]),1.);
      if(FtCrAver[1]>0 && FtCrAver[3]>0)HF1(1293,geant(FtCrAver[1]-FtCrAver[3]),1.);
    }
    if(beta<-0.75 && beta>-1.15){
      if(FtCrAver[0]>0 && FtCrAver[2]>0)HF1(1294,geant(FtCrAver[0]-FtCrAver[2]),1.);
      if(FtCrAver[0]>0 && FtCrAver[3]>0)HF1(1295,geant(FtCrAver[0]-FtCrAver[3]),1.);
      if(FtCrAver[1]>0 && FtCrAver[2]>0)HF1(1296,geant(FtCrAver[1]-FtCrAver[2]),1.);
      if(FtCrAver[1]>0 && FtCrAver[3]>0)HF1(1297,geant(FtCrAver[1]-FtCrAver[3]),1.);
    }
*/
//
//======================================> find/study track crossings with ACC:
//
/*
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
      HF1(1515,geant(nanti),1.);
}
    }
//
    number dirt,cpsn,cpcs,cphi,dphi1,dphi2,phil,phih,zcut,zscr,thes,phis;
    integer nseccr(0),isphys(-1);
    bool zcross,phicross;
    integer nsfir,nscrs;
    zcut=padlen/2-1;//-1 to compens. track accuracy
      dirt=1;//1=>forw, -1=>backw
      ptrack->interpolateCyl(cooCyl,dirCyl,padrad,dirt,crcCyl,the,phi,trl);
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
      if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
        HF1(1241,geant(crcCyl[2]),1.);
        HF1(1242,geant(cphi),1.);
        HF1(1243,geant(phi),1.);
}
      }
//
      for(int is=0;is<8;is++){//<-- loop over all ACC-sectors
        nscrs=0;
        nsfir=0;
        zcross=false;
        phicross=false;
        padfi=paddfi*(0.5+is);// logical-paddle phi
        phil=padfi-0.5*paddfi;//boundaries of sector
        phih=padfi+0.5*paddfi;
        if(fabs(crcCyl[2])<zcut)zcross=true;//match in Z 
	if(zcross)nscrs=floor(cphi/45.)+1;
	if(frsecn[is]>0)nsfir=is+1;
	if(nscrs>0 && nsfir>0){
          if(TFREFFKEY.reprtf[2]>2){
#pragma omp critical (hf2)
{
	    HF2(1235,geant(nscrs),geant(nsfir),1.);
}
	  }
	}
        dphi1=padfi-cphi;//PHIsect-PHIcrp
	if(dphi1>180)dphi1=360-dphi1;
	if(dphi1<-180)dphi1=-(360+dphi1);
        if(cphi<(phih-1) && cphi>(phil+1))phicross=true;//match in phi (-1degr for accur.compens)
	if(cphi<padfi)isphys=2*is;//physical sector number
	else isphys=2*is+1;
	zscr=crcCyl[2];
	thes=the;
	phis=phi;
        if(zcross && phicross && frsecn[is]>0)nseccr+=1;
	dphi2=cphi-phi;//PHIcrp-PHIimp
	if(dphi2>180)dphi2=360-dphi2;
	if(dphi2<-180)dphi2=-(360+dphi2);
//
        if(phicross && frsecn[is]>0){
          if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
	    HF1(1244,geant(crcCyl[2]),1.);
}
	  }
	}
	if(zcross && frsecn[is]>0){
          if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
	    HF1(1245,geant(cphi),1.);
	    HF1(1270+is,geant(dphi1),1.);//PHIsect-PHIcrp
}
	  }
	}
        if(zcross && phicross && frsecn[is]>0){
          if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
            HF1(1246,geant(phi),1.);//inpact phi(degr,+-180)
            HF1(1248,geant(dphi2),1.);//PHIcrp-PHIimpp
            HF1(1240,geant(is+1+20),1.);//fired+crossed patt
	    if(ama1[is]>0)HF1(1250+is,geant(ama1[is]),1.);
	    if(ama2[is]>0)HF1(1250+8+is,geant(ama2[is]),1.);
}
	  }
	}
	
        if(zcross && phicross){
          if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
	    HF1(1240,geant(is+1),1.);//TRK-crossed patt
}
	  }
	}
        if(frsecn[is]>0){
          if(TFREFFKEY.reprtf[1]>2){
#pragma omp critical (hf1)
{
	    HF1(1240,geant(is+1+10),1.);//fired patt
}
	  }
	}
//      
      }//--->endof ACC-sectors loop
//
      if(nseccr>0)TOF2JobStat::addre(48);
*/
//==================================================
//    if(!TofRClMultOK)return; // remove events with bad mult. of Tof-RawClusters/layer 
        if(!TofClMultOK)return;// remove events with bad mult. of Tof-Clusters/layer 
    TOF2JobStat::addre(23);
//
// =================================>  find track crossing points/angles with Tof-counters:
//
    bool TofTrackMatch(true);
    bool TofWithExtPad(false);
    geant barw,dtcut,dlcut(8);
    bool slopepos,slopeneg;
    bad=0;
    C0[0]=0.;
    C0[1]=0.;
    AMSPoint crd;
    number cllc[TOF2GC::SCLRS];//cluster long.coo
    number cltc[TOF2GC::SCLRS];//cluster tran.coo
    number xentr(0),yentr(0),xexit(0),yexit(0);
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      crd=clco[il];//from Cluster
      ib=brnlc[il];
      if(ib<0)continue;
      brnum=il*TOF2GC::SCMXBR+ib;//bar numbering ...
//      if(clmem[il]!=1)continue;//single bar clusters
//
      if(ib>0 && (ib+1)<TOF2DBc::getbppl(il))barw=TOF2DBc::plnstr(5);//stand.(not outer) bar width
      else barw=TOF2DBc::outcp(il,1);//outer counter width
      dtcut=barw/2+0.25;
//      if(ib==0 || ib==(TOF2DBc::getbppl(il)-1))TofWithExtPad=true;//found outer counters in track
      TOF2JobStat::addbr(brnum,5);//count entries
      zc[il]=TOF2DBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      if(il==0){
        xentr=Cout[0];
	yentr=Cout[1];
      }
      if(il==3){
        xexit=Cout[0];
	yexit=Cout[1];
      }
      cstr[il]=cos(the);
      trlr[il]=trl;
      if(TOF2DBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take trk Y-cross as long.c
        ctran=Cout[0];// TRK transv. coord.(abs. r.s.)(X-cross)
	cltc[il]=crd[0];//Cl.tran.coo(XCl,abs)
	cllc[il]=crd[1];//Cl.long.coo(YCl,abs)
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take trk X-cross as long.c.
        ctran=Cout[1];// TRK transv. coord.(abs. r.s.)(Y-cross) 
	cllc[il]=crd[0];//cl.long.coo(XCl,abs) 
	cltc[il]=crd[1];//Cl.tran.coo(YCl,abs)
      }
      dy=coot[il]-cllc[il];//Long.coo_track-Long.coo_TofClust
      if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
        if(clmem[il]==1)HF1(1200+il,geant(dy),1.);
        if(clmem[il]==2)HF1(1204+il,geant(dy),1.);
}
        if(nbrl[il]==1 && fabs(dx)<dtcut){
#pragma omp critical (hf2)
{
	  if(TFREFFKEY.reprtf[1]>1)HF2(1231+il,geant(cllc[il]),geant(dy),1.);
}
	}
      }
      dx=ctran-cltc[il];//Transv.coo_tracker-Transv.coo_TofClust
      tgx=sin(the)*cos(phi)/cos(the);
      tgy=sin(the)*sin(phi)/cos(the);
      if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
        if(clmem[il]==1)HF1(1210+il,geant(dx),1.);
        if(clmem[il]==2)HF1(1214+il,geant(dx),1.);
}
      }
      if(fabs(dx)>dtcut || fabs(dy)>dlcut)TofTrackMatch=false;//too big dist. of tof-track
//
      if(fabs(dx)<=dtcut){
        TOF2JobStat::addbr(brnum,6);//count transv.matchOK
	if(fabs(dy)<=dlcut)TOF2JobStat::addbr(brnum,7);//count (transv+long).matchOK
      }
//
      elosc[il]*=geant(fabs(cos(the)));// angle correction for dE/dX(to norm.inc)
      adca1[il]*=geant(fabs(cos(the)));// .....................adca
      adca2[il]*=geant(fabs(cos(the)));
    }//---> endof tof-layer loop
//
    cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);//average cos from track
//----------------------------
    if(!TofTrackMatch)return;//too big difference of TOF-Track coord. in any layer
//    if(TofWithExtPad)return;
    TOF2JobStat::addre(47);
//--
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
      if(beta>=0)HF1(1287,beta,1.);
      else{
        HF1(11287,beta,1.);
      }
}
    }
//==================================> Private Beta-fit using TofClusters and TRK-track length :
//
  if(TofClOneMem){//only for all 4 TofClusters with 1memb/layer
//cout<<" Trlen="<<trlr[0]<<" "<<trlr[1]<<" "<<trlr[2]<<" "<<trlr[3]<<endl;
    trle[0]=0.;
//
    trle[1]=trlr[1]-trlr[0];//1->2, >0
    trle[2]=trlr[2]-trlr[0];//1->3
    trle[3]=trlr[3]-trlr[0];//1->4
    tdif[0]=0;
    tdif[1]=cltim[0]-cltim[1];//>0 for beta>0
    tdif[2]=cltim[0]-cltim[2];
    tdif[3]=cltim[0]-cltim[3];
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
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
      if(betof>0.85 && betof<1.15){//for beta-resol fit
        if(momentum>10)HF1(1502,fabs(betof),1.);
        HF1(1208,chsq,1.);
        HF1(1209,tzer,1.);
      }
      if(betof>0)HF1(1285,betof,1.);
      else HF1(1286,betof,1.);
}
    }
//
//    if(chsq>20)return;//<==== cut on chi2/beta
//
//
    geant td13,td24,td14,td23;
    geant zpl1,zpl2,trlnor;
    zpl1=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l1-middl
    zpl2=TOF2DBc::supstr(1)-
        (TOF2DBc::plnstr(2)+TOF2DBc::plnstr(6)/2+TOF2DBc::plnstr(3)/2);//z-l2-middl
     
    trlnor=zpl1+zpl2;//z-dist. L1-L3(L2-L4)
    td13=tdif[2]*trlnor/trle[2];//(1->3) normalized to fix(~127cm) distance
    td24=(cltim[1]-cltim[3])*trlnor/(trle[3]-trle[1]);//(2->4) normalized to fix(~127cm) distance
    td14=tdif[3]*trlnor/trle[3];//(1->4) normalized to fix(~127cm) distance
    td23=(cltim[1]-cltim[2])*trlnor/(trle[2]-trle[1]);//(2->3) normalized to fix(~127cm) distance
//cout<<"---> T1-3/2-4:"<<td13<<" "<<td24<<endl;
    if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
      if(betof>0){
        HF1(1504,(td13-td24),1.);
        HF1(15041,td13,1.);
        HF1(15042,td24,1.);
        HF1(15043,td14,1.);
        HF1(15044,td23,1.);
//	if(td13<3 && td13>1.8){
//	  cout<<"--->BadTd13:"<<td13<<" bars:"<<(brnl[0]+1)<<" "<<(brnl[1]+1)<<" "<<(brnl[2]+1)<<" "<<(brnl[3]+1)<<endl;
//	}
      }
}
    }
  }//endof "Clust with 1memb/layer" check
//
//======================================> Beta<0 problem test:
//
/*
    slopepos=(xentr>10 && xexit<-10);
    slopeneg=(xentr<-10 && xexit>10);
//    tdif[0]=(ltim[0]-ltim[1])/(trlr[0]-trlr[1]);
//    tdif[1]=(ltim[2]-ltim[3])/(trlr[2]-trlr[3]);
//cout<<"<--- Trlen:"<<trlr[0]<<" "<<trlr[1]<<" "<<trlr[2]<<" "<<trlr[3]<<" beta="<<beta<<endl;
    ldif[0]=fabs(trlr[0]-trlr[2]);
    ldif[1]=fabs(trlr[0]-trlr[3]);
    ldif[2]=fabs(trlr[1]-trlr[2]);
    ldif[3]=fabs(trlr[1]-trlr[3]);
    tdif[0]=fabs(ltim[0]-ltim[2]);//take fabs to use |beta| in single histogr.
    tdif[1]=fabs(ltim[0]-ltim[3]);
    tdif[2]=fabs(ltim[1]-ltim[2]);
    tdif[3]=fabs(ltim[1]-ltim[3]);
    HF1(1282,geant(tdif[0]),1.);
    HF1(1283,geant(tdif[1]),1.);
    HF1(1284,geant(tdif[2]),1.);
    HF1(1288,geant(tdif[3]),1.);
    HF1(11282,geant(ldif[0]),1.);
    HF1(11283,geant(ldif[1]),1.);
    HF1(11284,geant(ldif[2]),1.);
    HF1(11288,geant(ldif[3]),1.);
    if(beta>0.6){
//      HF1(1282,geant((ltim[0]-ltim[1])/(trlr[0]-trlr[1])),1.);
//      HF1(1284,geant((ltim[2]-ltim[3])/(trlr[2]-trlr[3])),1.);
//      HF1(1282,geant(trlr[0]-trlr[1]),1.);
//      HF1(1284,geant(trlr[2]-trlr[3]),1.);
    }
    if(beta<-0.6){
//      HF1(1283,geant((ltim[0]-ltim[1])/(trlr[0]-trlr[1])),1.);
//      HF1(1288,geant((ltim[2]-ltim[3])/(trlr[2]-trlr[3])),1.);
//      HF1(1283,geant(trlr[0]-trlr[1]),1.);
//      HF1(1288,geant(trlr[2]-trlr[3]),1.);
    }
*/    
//===================================
//---> for equiliz.procedure:
    if(TFCAFFKEY.spares[0]==1){//PMEquilization Mode
      plvl1=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
      bool intrig=plvl1->checktofpattor(il,ib);
    }

//===================================
//
// some test for t-resolution:
/*
    trle[0]=0.;
    trle[1]=fabs(trlr[0]-trlr[1]);//1->2
    tdif[0]=0;
    tdif[1]=(tmss[1]-tmss[0])*fabs((cstr[0]+cstr[1])/2);//Time L1->L2, cos-corrected
    if(brnl[0]==3 && brnl[1]==2)HF1(1520,geant(tdif[1]),1.);
    if(brnl[0]==3 && brnl[1]==3)HF1(1521,geant(tdif[1]),1.);
    if(brnl[0]==3 && brnl[1]==4)HF1(1522,geant(tdif[1]),1.);
    if(brnl[0]==3 && brnl[1]==5)HF1(1523,geant(tdif[1]),1.);
*/
//--------------------------------------------
    if(TFREFFKEY.reprtf[1]>0){
      bad=0;
      for(il=0;il<TOF2GC::SCLRS;il++){
        if(nbrl[il]>1)bad=1;
	if((il==0) && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if((il==3) && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if(il==1 && (brnl[il]==0 || brnl[il]==7))bad+=1;
	if(il==2 && (brnl[il]==0 || brnl[il]==9))bad+=1;
      }
      if(bad<2){
#pragma omp critical (hf1)
{
        HF1(1507,geant(chargeTOF),1.);//accept max one trapez.counter
        HF1(1508,geant(chargeTracker),1.);
        HF1(1506,geant(charge),1.);
}
#pragma omp critical (hf2)
{
        HF2(1509,geant(chargeTracker),geant(chargeTOF),1.);
}
      }
    }
//
//
//return;
    geant *pntr[TOF2GC::SCLRS];
    geant avera[4];
    if(betof>0.9){ // dE/dX only relativistic particles
//                  ---> look at truncated averages :
      for(il=0;il<TOF2GC::SCLRS;il++)pntr[il]=&elosc[il];//pointers to layer edep's 
      AMSsortNAG(pntr,TOF2GC::SCLRS);//sort in increasing order
      avera[0]=(*pntr[0]);// lowest
      avera[1]=avera[0]+(*pntr[1]);// sum of 2 lowest
      avera[2]=avera[1]+(*pntr[2]);//        3 lowest
      avera[3]=avera[2]+(*pntr[3]);// average
      avera[1]/=2.;
      avera[2]/=3.;
      avera[3]/=4.;
      if(TFREFFKEY.reprtf[1]>0){
#pragma omp critical (hf1)
{
        for(il=0;il<TOF2GC::SCLRS;il++)HF1(5001+il,avera[il],1.);
        if(TFREFFKEY.reprtf[2]>1){
          HF1(5010,sqrt(avera[0]/1.8),1.);// eff.Z
          HF1(5011,sqrt(avera[1]/1.8),1.);
          HF1(5012,sqrt(avera[2]/1.8),1.);
          HF1(5013,sqrt(avera[3]/1.8),1.);
	}
}
      }
    }
//
//-----> look at angle-corrected anode amplitudes(adc-chan) for all bar-types:
//
    integer ibtyp,nbrs;
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      if(ib<0)continue;
      nbrs=nbrl[il];
      if(nbrl[il]==1 && fabs(coot[il])<10){//select counter center crossing
        ibtyp=TOF2DBc::brtype(il,ib)-1;
        if(TFREFFKEY.reprtf[1]>1){
#pragma omp critical (hf1)
{
	  HF1(1220+ibtyp,geant(adca1[il]),1.);
}
	}
      }
    } 
//
//-----> simulate TOF-group Npe measurement procedure for some counter (used in Bologna DB)
    number varr,amp1,amp2,sigm,noise;
    if(TFREFFKEY.reprtf[2]>0 && brnl[0]>=2 && brnl[0]<=5){//use counters 103-106(type=2) for the moment
//    if(TFREFFKEY.reprtf[2]>0 && brnl[0]==3){//use counter 104(type=2) for the moment
      if(fabs(coot[0])<2.5){//use narrow area at the counter center
//        sigm=3.5;//(adc-ch, 10% of of prot mprob.ampl=35)
	sigm=0;
        noise=sigm*rnormx(); 
        amp1=adca1[0]+noise;
        noise=sigm*rnormx(); 
	amp2=adca2[0]+noise;
        varr=(amp1-amp2)/(amp1+amp2);
#pragma omp critical (hf1)
{
        HF1(1519,geant(varr),1.);
}
      }
    }
//
//
//----------------
//  if((brnl[0]+1)==5 && (brnl[1]+1)==4 && (brnl[2]+1)==5 && (brnl[3]+1)==5){
    plvl1=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
#pragma omp critical (hf1)
{
    HF1(1524,0.,1.);//just count entries
}
    for(i=0;i<16;i++){
      if(plvl1->JMembPattBitSet(i)){
#pragma omp critical (hf1)
{
        HF1(1524,geant(i+1),1.);//bit was set
}
      }
    }
//  }
//----------------
    return;
//
}
//----------------------------
void TOF2User::InitJob(){
  int i;
  if(TFREFFKEY.reprtf[1]>0){
    HBOOK1(1518,"TofUser:MCBeta",100,0.8,1.,0.);
    HBOOK1(1500,"TofUser:Particle Rigidity(gv),Beta>0",90,-15.,15.,0.);
    HBOOK1(1501,"TofUser:Particle(Tof) Beta(<0)",80,-1.4,-0.6,0.);
    HBOOK1(11501,"TofUser:Particle(Tof) Beta(>0)",80,0.6,1.4,0.);
    HBOOK1(1520,"TofUser:Particle(Tof) Beta",100,0.85,1.15,0.);
    HBOOK1(1280,"TofUser:Particle Beta>0(calc. as p/m)",100,0.2,1.2,0.);
    HBOOK1(1281,"TofUser:Particle Beta<0(calc. as p/m)",100,0.2,1.2,0.);
//
    HBOOK1(1285,"TofUser:MyBeta(>0,TRKmatchOK)",100,0.,1.25,0.);
    HBOOK1(1286,"TofUser:MyBeta(<0,TRKmatchOK)",100,-1.25,0.,0.);
    HBOOK1(1287,"TofUser:Particle(Tof) Beta(>=0,TRKmatchOK)",80,0.6,1.4,0.);
    HBOOK1(11287,"TofUser:Particle(Tof) Beta(<0,TRKmatchOK)",80,-1.4,-0.6,0.);
    
//    HBOOK1(1282,"TofUser:t1-t2,beta>0,TRKmatchOK",100,-2.5,2.5,0.);
//    HBOOK1(1283,"TofUser:t1-t2,beta<0,TRKmatchOK",100,-2.5,2.5,0.);
//    HBOOK1(1284,"TofUser:t3-t4,beta>0,TRKmatchOK",100,-2.5,2.5,0.);
//    HBOOK1(1288,"TofUser:t3-t4,beta<0,TRKmatchOK",100,-2.5,2.5,0.);
    HBOOK1(1282,"TofUser:t1-t3,PattBetlt0,TRKmatchOK",100,3.,7.,0.);
    HBOOK1(1283,"TofUser:t1-t4,PattBetlt0,TRKmatchOK",100,3.,7.,0.);
    HBOOK1(1284,"TofUser:t2-t3,PattBetlt0,TRKmatchOK",100,3.,7.,0.);
    HBOOK1(1288,"TofUser:t2-t4,PattBetlt0,TRKmatchOK",100,3.,7.,0.);
    HBOOK1(11282,"TofUser:l1-l3,PattBetlt0,TRKmatchOK",100,100.,200.,0.);
    HBOOK1(11283,"TofUser:l1-l4,PattBetlt0,TRKmatchOK",100,100.,200.,0.);
    HBOOK1(11284,"TofUser:l2-l3,PattBetlt0,TRKmatchOK",100,100.,200.,0.);
    HBOOK1(11288,"TofUser:l2-l4,PattBetlt0,TRKmatchOK",100,100.,200.,0.);
//
    HBOOK1(1289,"TofUser:ACC-patt(orig,HiAmpAnd/HiAmpOr",30,1.,30.,0.);
//
/*
    HBOOK1(1290,"TofUser:Beta > 0,FTCr0-2",100,-20.,20.,0.);
    HBOOK1(1291,"TofUser:Beta > 0,FTCr0-3",100,-20.,20.,0.);
    HBOOK1(1292,"TofUser:Beta > 0,FTCr1-2",100,-20.,20.,0.);
    HBOOK1(1293,"TofUser:Beta > 0,FTCr1-3",100,-20.,20.,0.);
    HBOOK1(1294,"TofUser:Beta < 0,FTCr0-2",100,-20.,20.,0.);
    HBOOK1(1295,"TofUser:Beta < 0,FTCr0-3",100,-20.,20.,0.);
    HBOOK1(1296,"TofUser:Beta < 0,FTCr1-2",100,-20.,20.,0.);
    HBOOK1(1297,"TofUser:Beta < 0,FTCr1-3",100,-20.,20.,0.);
*/
//
    HBOOK1(1511,"TofUser:Particle BetaChi2",80,0.,16.,0.);
    HBOOK1(1512,"TofUser:Particle BetaChi2S",80,0.,16.,0.);
    HBOOK1(1513,"TofUser:Particle TrackChi2",80,0.,240.,0.);
    HBOOK1(1514,"TofUser:Number of AccSectors(FTCoinc,E>Thr,TOFmultOK)",20,0.,20.,0.);
    HBOOK1(1515,"TofUser:Number of AccSectors(FTCoinc,E>Thr,TOF/ANTImultOK,TrkOK)",20,0.,20.,0.);
    HBOOK1(1502,"TofUser:MyBeta(private beta-fit on TofClust, TRKmatchOK)",100,0.7,1.2,0.);
    HBOOK1(1504,"TofUser:TofClust T13-T24(ns, TRK-track matched,Beta>0)",80,-4.,4.,0.);
    HBOOK1(15041,"TofUser:TofClust T13(ns, TRK-track matched,Beta>0)",80,1.5,9.5,0.);
    HBOOK1(15042,"TofUser:TofClust T24(ns, TRK-track matched,Beta>0)",80,1.5,9.5,0.);
    HBOOK1(15043,"TofUser:TofClust T14(ns, TRK-track matched,Beta>0)",80,1.5,9.5,0.);
    HBOOK1(15044,"TofUser:TofClust T23(ns, TRK-track matched,Beta>0)",80,1.5,9.5,0.);
    HBOOK1(1503,"TofUser:AccSector energy(mev,FTCoinc)",80,0.,20.,0.);
    HBOOK1(15001,"TofUser:AccSector-1 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15002,"TofUser:AccSector-2 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15003,"TofUser:AccSector-3 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15004,"TofUser:AccSector-4 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15005,"TofUser:AccSector-5 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15006,"TofUser:AccSector-6 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15007,"TofUser:AccSector-7 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(15008,"TofUser:AccSector-8 energy(mev,FTCoinc)",100,0.,10.,0.);
    HBOOK1(1505,"TofUser:Number of AccSectors(FTCoinc,E>Thr)",20,0.,20.,0.);
    HBOOK1(1506,"TofUser:Part.charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK1(1507,"TofUser:TOF-charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK1(1508,"TofUser:Tracker-charge(trapez.c <=1)",20,0.,20.,0.);
    HBOOK2(1509,"TofUser:TOF-ch vs Tracker-ch(trapez.c <=1)",10,0.,10.,10,0.,10.,0.);
    HBOOK1(1510,"TofUser:Anti-hit part.index",50,0.,50.,0.);
    HBOOK1(1516,"TofUser:Part.rigidity(gv),Beta<0",100,-10.,10.,0.);
    HBOOK1(11516,"TofUser:Part.rigidity(gv)",100,-500.,500.,0.);
    HBOOK1(1517,"TofUser:Number of AccSectors(FTCoincAccordingToTrigPatt)",20,0.,20.,0.);
    HBOOK1(1519,"TofUser:(As1-As2)/(As1+As2)(Ampl.in adc-ch, cid=104)",80,-0.6,0.6,0.);
    
//    HBOOK1(1521,"L1/L2 raw time diff(cos-corrected),B4/B4",60,0.,6.,0.);
//    HBOOK1(1522,"L1/L2 raw time diff(cos-corrected),B4/B5",60,0.,6.,0.);
//    HBOOK1(1523,"L1/L2 raw time diff(cos-corrected),B4/B6",60,0.,6.,0.);
  HBOOK1(1524,"TrigPatt:ftc,cp0,cp1,ct0,ct1,ftz,fte,ac0,ac1,bz,ecfa,ecfo,ecaa,ecao,ex1,ex2",20,0.,20.,0.);
    
    HBOOK1(1200,"TofUser:LongCooDiff(Track-TofCl),L=1,Nmem=1",60,-15.,15.,0.);
    HBOOK1(1201,"TofUser:LongCooDiff(Track-TofCl),L=2,Nmem=1",60,-15.,15.,0.);
    HBOOK1(1202,"TofUser:LongCooDiff(Track-TofCl),L=3,Nmem=1",60,-15.,15.,0.);
    HBOOK1(1203,"TofUser:LongCooDiff(Track-TofCl),L=4,Nmem=1",60,-15.,15.,0.);
    HBOOK1(1204,"TofUser:LongCooDiff(Track-TofCl),L=1,Nmem=2",60,-15.,15.,0.);
    HBOOK1(1205,"TofUser:LongCooDiff(Track-TofCl),L=2,Nmem=2",60,-15.,15.,0.);
    HBOOK1(1206,"TofUser:LongCooDiff(Track-TofCl),L=3,Nmem=2",60,-15.,15.,0.);
    HBOOK1(1207,"TofUser:LongCooDiff(Track-TofCl),L=4,Nmem=2",60,-15.,15.,0.);
    
    HBOOK1(1208,"TofUser:MyBetaChisq(private beta-fit using TofClust)",80,0.,40.,0.);
    HBOOK1(1209,"TofUser:MyTzero(private beta-fit with TofCl)",50,-2.5,2.5,0.);
    
    HBOOK1(1210,"TofUser:TranCooDiff(Track-TofCl),L=1,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1211,"TofUser:TranCooDiff(Track-TofCl),L=2,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1212,"TofUser:TranCooDiff(Track-TofCl),L=3,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1213,"TofUser:TranCooDiff(Track-TofCl),L=4,Nmem=1",50,-20.,20.,0.);
    HBOOK1(1214,"TofUser:TranCooDiff(Track-TofCl),L=1,Nmem=2",50,-2.5,2.5,0.);
    HBOOK1(1215,"TofUser:TranCooDiff(Track-TofCl),L=2,Nmem=2",50,-2.5,2.5,0.);
    HBOOK1(1216,"TofUser:TranCooDiff(Track-TofCl),L=3,Nmem=2",50,-2.5,2.5,0.);
    HBOOK1(1217,"TofUser:TranCooDiff(Track-TofCl),L=4,Nmem=2",50,-2.5,2.5,0.);
    
  if(TFREFFKEY.reprtf[1]>1){
    HBOOK1(1220,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-1)",80,0.,160.,0.);
    HBOOK1(1221,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-2)",80,0.,160.,0.);
    HBOOK1(1222,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-3)",80,0.,160.,0.);
    HBOOK1(1223,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-4)",80,0.,160.,0.);
    HBOOK1(1224,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-5)",80,0.,160.,0.);
    HBOOK1(1225,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-6)",80,0.,160.,0.);
    HBOOK1(1226,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-7)",80,0.,160.,0.);
    HBOOK1(1227,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-8)",80,0.,160.,0.);
    HBOOK1(1228,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-9)",80,0.,160.,0.);
    HBOOK1(1229,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-10)",80,0.,160.,0.);
    HBOOK1(1230,"TofUser:S1AnodeRawAmpl(adc-ch, angl.corr,bt-11)",80,0.,160.,0.);
  }
    
  if(TFREFFKEY.reprtf[1]>1){
    HBOOK2(1231,"TofUser:TofClusLcoo-TrkCrosCoo,L1,(TransvMatchOK)",70,-70.,70.,60,-30.,30.,0.);
    HBOOK2(1232,"TofUser:TofClusLcoo-TrkCrosCoo,L2,(TransvMatchOK)",70,-70.,70.,60,-30.,30.,0.);
    HBOOK2(1233,"TofUser:TofClusLcoo-TrkCrosCoo,L3,(TransvMatchOK)",70,-70.,70.,60,-30.,30.,0.);
    HBOOK2(1234,"TofUser:TofClusLcoo-TrkCrosCoo,L4,(TransvMatchOK)",70,-70.,70.,60,-30.,30.,0.);
  }

  if(TFREFFKEY.reprtf[1]>2){    
    HBOOK2(1235,"TofUser:ACC NSfired vs NScrossed",10,0,10.,10,0.,10.,0.);
    HBOOK1(1240,"TofUser:AccRaw: Crossed/Fired(+10)/(fired+crossed)(+20) sectors pattern",30,1.,31.,0.);
    HBOOK1(1241,"TofUser:AccRaw: Cyl-track Zcross(noCuts)",75,-75.,75.,0.);
    HBOOK1(1242,"TofUser:AccRaw: TRK-track CrossPointPhi(noCuts)",91,0.,364.,0.);
    HBOOK1(1243,"TofUser:AccRaw: TRK-track ImpactPhi(noCuts)",91,0.,364.,0.);
    HBOOK1(1244,"TofUser:AccRaw: Sector-track Zcross(fired+PhiMatch)",75,-75.,75.,0.);
    HBOOK1(1245,"TofUser:AccRaw: TRK-track CrossPointPhi(fired+ZMatch)",91,0.,364.,0.);
    HBOOK1(1246,"TofUser:AccRaw: TRK-track ImpactPhi(fired+matched)",91,0.,364.,0.);
//    HBOOK1(1247,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch)",91,-182.,182.,0.);
    HBOOK1(1248,"TofUser:AccRaw: PHIcrosPoint-PHIimp(fired+matched)",91,-182.,182.,0.);
    
    HBOOK1(1270,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-1",91,-182.,182.,0.);
    HBOOK1(1271,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-2",91,-182.,182.,0.);
    HBOOK1(1272,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-3",91,-182.,182.,0.);
    HBOOK1(1273,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-4",91,-182.,182.,0.);
    HBOOK1(1274,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-5",91,-182.,182.,0.);
    HBOOK1(1275,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-6",91,-182.,182.,0.);
    HBOOK1(1276,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-7",91,-182.,182.,0.);
    HBOOK1(1277,"TofUser:AccRaw: PHIsect-PHIcrosPoint(fired+ZMatch),Seq-8",91,-182.,182.,0.);

    HBOOK1(1250,"TofUser: AccRaw when crossed,Sect-1,side-1",80,0.,3200.,0.);
    HBOOK1(1251,"TofUser: AccRaw when crossed,Sect-2,side-1",80,0.,3200.,0.);
    HBOOK1(1252,"TofUser: AccRaw when crossed,Sect-3,side-1",80,0.,3200.,0.);
    HBOOK1(1253,"TofUser: AccRaw when crossed,Sect-4,side-1",80,0.,3200.,0.);
    HBOOK1(1254,"TofUser: AccRaw when crossed,Sect-5,side-1",80,0.,3200.,0.);
    HBOOK1(1255,"TofUser: AccRaw when crossed,Sect-6,side-1",80,0.,3200.,0.);
    HBOOK1(1256,"TofUser: AccRaw when crossed,Sect-7,side-1",80,0.,3200.,0.);
    HBOOK1(1257,"TofUser: AccRaw when crossed,Sect-8,side-1",80,0.,3200.,0.);
    HBOOK1(1258,"TofUser: AccRaw when crossed,Sect-1,side-2",80,0.,3200.,0.);
    HBOOK1(1259,"TofUser: AccRaw when crossed,Sect-2,side-2",80,0.,3200.,0.);
    HBOOK1(1260,"TofUser: AccRaw when crossed,Sect-3,side-2",80,0.,3200.,0.);
    HBOOK1(1261,"TofUser: AccRaw when crossed,Sect-4,side-2",80,0.,3200.,0.);
    HBOOK1(1262,"TofUser: AccRaw when crossed,Sect-5,side-2",80,0.,3200.,0.);
    HBOOK1(1263,"TofUser: AccRaw when crossed,Sect-6,side-2",80,0.,3200.,0.);
    HBOOK1(1264,"TofUser: AccRaw when crossed,Sect-7,side-2",80,0.,3200.,0.);
    HBOOK1(1265,"TofUser: AccRaw when crossed,Sect-8,side-2",80,0.,3200.,0.);
  }
    
    HBOOK1(5001,"TofUser:TofCl EdepNormInc(mev),AverTrunc1of4",80,0.,32.,0.);
    HBOOK1(5002,"TofUser:TofCl EdepNormInc(mev),AverTrunc2of4",80,0.,32.,0.);
    HBOOK1(5003,"TofUser:TofCl EdepNormInc(mev),AverTrunc3of4",80,0.,32.,0.);
    HBOOK1(5004,"TofUser:TofCl EdepNormInc(mev),AverTrunc4of4",80,0.,32.,0.);
    
    if(TFREFFKEY.reprtf[1]>1){    
      HBOOK1(5010,"TofUser:TofCl Sqrt(AverTrunc1of4/MIP)",80,0.,32.,0.);
      HBOOK1(5011,"TofUser:TofCl Sqrt(AverTrunc2of4/MIP)",80,0.,32.,0.);
      HBOOK1(5012,"TofUser:TofCl Sqrt(AverTrunc3of4/MIP)",80,0.,32.,0.);
      HBOOK1(5013,"TofUser:TofCl Sqrt(AverTrunc4of4/MIP)",80,0.,32.,0.);
    }
    
    HBOOK1(5040,"TofUser:L=1,Fired RawCl bar number",14,1.,15.,0.);
    HBOOK1(5041,"TofUser:L=2,Fired RawCl bar number",14,1.,15.,0.);
    HBOOK1(5042,"TofUser:L=3,Fired RawCl bar number",14,1.,15.,0.);
    HBOOK1(5043,"TofUser:L=4,Fired RawCl bar number",14,1.,15.,0.);
    
    HBOOK1(5050,"EcUser:ShowerEnergy(gev)",100,0.,1.,0.);
    HBOOK1(5051,"EcUser:ShowerEnergy(gev)",100,0.,100.,0.);
    HBOOK1(5052,"EcUser:ShowerEnergy(gev)",100,0.,400.,0.);
    HBOOK1(5053,"Ecal TotHits(fired pixels)",100,1.,301.,0.);
    HBOOK1(5054,"Ecal fired pixels,PixL1",72,1.,73.,0.);
    HBOOK1(5055,"Ecal fired pixels,PixL3",72,1.,73.,0.);
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
  if(TFREFFKEY.reprtf[1]==0)return;
  par[0]=200.;
  par[1]=1.;
  par[2]=0.03;
  step[0]=50.;
  step[1]=0.005;
  step[2]=0.005;
  pmin[0]=10.;
  pmin[1]=0.85;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.15;
  pmax[2]=0.1;
  HFITHN(1502,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" TOFbeta-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1502);

  HPRINT(1285);
  HPRINT(1286);
  HPRINT(1208);
  HPRINT(1209);
//--
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
  HPRINT(15041);
  HPRINT(15043);
  HPRINT(15044);
  HPRINT(15042);
//-----
  HPRINT(1518);
  HPRINT(1500);
  HPRINT(1516);
  HPRINT(11516);
  HPRINT(1501);
  HPRINT(11501);
//----
  par[0]=200.;
  par[1]=1.;
  par[2]=0.03;
  step[0]=50.;
  step[1]=0.005;
  step[2]=0.005;
  pmin[0]=10.;
  pmin[1]=0.85;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.15;
  pmax[2]=0.1;
  HFITHN(1520,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" PartBeta-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1520);
//----
  HPRINT(1511);
  HPRINT(1512);
  HPRINT(1513);
  HPRINT(1280);
  HPRINT(1281);
  HPRINT(1287);
  HPRINT(11287);
//----
  HPRINT(1289);//ACC-patt
//
  
  HPRINT(1282);
  HPRINT(1283);
  HPRINT(1284);
  HPRINT(1288);
  HPRINT(11282);
  HPRINT(11283);
  HPRINT(11284);
  HPRINT(11288);
//  for(i=0;i<8;i++)HPRINT(1290+i);
  
  HPRINT(1503);
  for(i=0;i<8;i++)HPRINT(15001+i);
  HPRINT(1505);
  HPRINT(1514);
  HPRINT(1517);
  HPRINT(1515);
//----
/*  
  par[0]=200.;//evs in max
  par[1]=2.5;//most prob
  par[2]=0.3;//rms
  step[0]=50.;
  step[1]=0.2;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=6.;
  pmax[2]=1.;
  HFITHN(1520,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" L1/L2-time fit: Mp/rms="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1520);
  
  par[0]=200.;//evs in max
  par[1]=2.5;//most prob
  par[2]=0.3;//rms
  step[0]=50.;
  step[1]=0.2;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=6.;
  pmax[2]=1.;
  HFITHN(1521,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" L1/L2-time fit: Mp/rms="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1521);
  
  par[0]=200.;//evs in max
  par[1]=2.5;//most prob
  par[2]=0.3;//rms
  step[0]=50.;
  step[1]=0.2;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=6.;
  pmax[2]=1.;
  HFITHN(1522,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" L1/L2-time fit: Mp/rms="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1522);
  
  par[0]=200.;//evs in max
  par[1]=2.5;//most prob
  par[2]=0.3;//rms
  step[0]=50.;
  step[1]=0.2;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.;
  pmin[2]=0.05;
  pmax[0]=10000.;
  pmax[1]=6.;
  pmax[2]=1.;
  HFITHN(1523,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<" L1/L2-time fit: Mp/rms="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(1523);
*/
  HPRINT(1524);
  HPRINT(1519);
  HPRINT(1507);
  HPRINT(1508);
  HPRINT(1506);
  HPRINT(1509);
  HPRINT(1200);
  HPRINT(1201);
  HPRINT(1202);
  HPRINT(1203);
  HPRINT(1204);
  HPRINT(1205);
  HPRINT(1206);
  HPRINT(1207);
  HPRINT(1210);
  HPRINT(1211);
  HPRINT(1212);
  HPRINT(1213);
  HPRINT(1214);
  HPRINT(1215);
  HPRINT(1216);
  HPRINT(1217);
  HPRINT(1510);
  
  if(TFREFFKEY.reprtf[1]>1){//raw ampl  
    HPRINT(1220);
    HPRINT(1221);
    HPRINT(1222);
    HPRINT(1223);
    HPRINT(1224);
    HPRINT(1225);
    HPRINT(1226);
    HPRINT(1227);
    HPRINT(1228);
    HPRINT(1229);
    HPRINT(1230);
  }

  if(TFREFFKEY.reprtf[1]>1){//dy vs cr-coo  
    HPRINT(1231);
    HPRINT(1232);
    HPRINT(1233);
    HPRINT(1234);
  }
  if(TFREFFKEY.reprtf[1]>2){//ACC 
    HPRINT(1235);  
    HPRINT(1240);
    HPRINT(1241);
    HPRINT(1242);
    HPRINT(1243);
    HPRINT(1244);
    HPRINT(1245);
    HPRINT(1246);
//   HPRINT(1247);
    HPRINT(1248);
    for(i=0;i<2*ANTI2C::MAXANTI;i++)HPRINT(1250+i);
    for(i=0;i<ANTI2C::MAXANTI;i++)HPRINT(1270+i);
  }
  
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5040+i);
  for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5001+i);
  if(TFREFFKEY.reprtf[2]>1){//sqrt(truncaver)  
    for(i=0;i<TOF2GC::SCLRS;i++)HPRINT(5010+i);
  }
//  HIDOPT(5020,chopt1);
//  HPRINT(5020);
//
  HPRINT(5050);  
  HPRINT(5051);
  HPRINT(5052);
  HPRINT(5053);
  HPRINT(5054);
  HPRINT(5055);
//  
  return;
}
