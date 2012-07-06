// v1.0 8.12.2000 by E.Choumilov
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
#include "mccluster.h"
#include "trrec.h"
#include "tofrec02.h"
#include "tofsim02.h"
#include "antirec02.h"
#include "ecaldbc.h"
#include "ecalrec.h"
#include "ecalcalib.h"
#include "daqecblock.h"
#include "user.h"
#include "trigger102.h"
#include "trigger302.h"
#include "particle.h"
#include <time.h>
#include "timeid.h"
#ifdef _PGTRACK_
#include "MagField.h"
#endif

using namespace ecalconst;
//
//
number ECREUNcalib::slslow;
number ECREUNcalib::slfast;
number ECREUNcalib::fastfr;
integer ECREUNcalib::nfits; 
number ECREUNcalib::pxrgain[ECPMSL][4];
number ECREUNcalib::pxrgainc[ECPMSL][4];
integer ECREUNcalib::pxstat[ECPMSL][4]; 
integer ECREUNcalib::dystat[ECPMSL]; 
number ECREUNcalib::pmrgain[ECPMSL];    
number ECREUNcalib::pmrgainc[ECPMSL];    
number ECREUNcalib::pmlres[ECPMSL][ECCLBMX][ECEVBUF];    
//number ECREUNcalib::pmlres[ECPMSL][ECCLBMX];    
//number ECREUNcalib::pmlres2[ECPMSL][ECCLBMX];
//
number ECREUNcalib::pmcresp[ECPMSL];//PM-resps for PM rel.gains(central bins aver.responce)    
number ECREUNcalib::pmcrahb[ECPMSL][ECAHBUF+1];//Ahighest buff .....................    
integer ECREUNcalib::pmccros[ECPMSL];//crossings counter ...........................    
integer ECREUNcalib::pmcrahm[ECPMSL];//Ahighest buff members .........................   
number ECREUNcalib::pmcrespc[ECPMSL];//PM-resps gain-corrected.....................
//    
number ECREUNcalib::sbcres[ECPMSL][4];    
number ECREUNcalib::sbcresc[ECPMSL][4];    
number ECREUNcalib::hi2lowr[ECPMSL][4]; 
number ECREUNcalib::hi2lowo[ECPMSL][4]; 
number ECREUNcalib::an2dynr[ECPMSL]; 
number ECREUNcalib::a2dnevs[ECPMSL]; 
integer ECREUNcalib::tevpml[ECPMSL][ECCLBMX]; 
number ECREUNcalib::tevsbf[ECPMSL][4]; 
number ECREUNcalib::tevsbc[ECPMSL][4]; 
number ECREUNcalib::tevpmc[ECPMSL];
number ECREUNcalib::tevpmf[ECPMSL];
number ECREUNcalib::tevpmm[ECPMSL];
number ECREUNcalib::values[ECCLBMX]; 
number ECREUNcalib::errors[ECCLBMX]; 
number ECREUNcalib::coords[ECCLBMX];
integer ECREUNcalib::nbins; 
number ECREUNcalib::tevhlc[ECPMSL*4][ECCHBMX]; 
number ECREUNcalib::sbchlc[ECPMSL*4][ECCHBMX]; 
number ECREUNcalib::sbchlc2[ECPMSL*4][ECCHBMX];
number ECREUNcalib::sbchlc3[ECPMSL*4][ECCHBMX];
number ECREUNcalib::et2momr;
integer ECREUNcalib::net2mom; 
//-----------------------------
void ECREUNcalib::init(){
  int i,j,k;
//
  cout<<"====> ECREUNcalibration job started....."<<endl;
//
  for(i=0;i<ECPMSL;i++){//pm*sl
    for(j=0;j<ECCLBMX;j++){//Lbin
      for(k=0;k<ECEVBUF;k++)pmlres[i][j][k]=0.;
      tevpml[i][j]=0.;
    }
    tevpmc[i]=0.;
    tevpmf[i]=0.;
    tevpmm[i]=0.;
    pmrgain[i]=1.;//default value
    pmrgainc[i]=1.;//default value(g-corrected)
    pmcresp[i]=0;
    pmcrespc[i]=0;
    pmccros[i]=0;
    pmcrahm[i]=0;
    for(k=0;k<ECAHBUF;k++)pmcrahb[i][k]=0;
    an2dynr[i]=0.;
    a2dnevs[i]=0.;
    dystat[i]=0;//default value(ok)
    for(j=0;j<4;j++){//subcell
      sbcres[i][j]=0.;
      sbcresc[i][j]=0.;
      tevsbc[i][j]=0.;
      tevsbf[i][j]=0.;
      pxrgain[i][j]=1.;//default value
      pxrgainc[i][j]=1;//used for calib-quality check
      pxstat[i][j]=0;//default value(ok)
      hi2lowr[i][j]=35.;//default value
      hi2lowo[i][j]=0.5;//default value
    }
  }
// for hi2low calibr.
  for(i=0;i<ECPMSL*4;i++){//pm*sl*sbc
    for(j=0;j<ECCHBMX;j++){//Hg-chan. bin
      tevhlc[i][j]=0;
      sbchlc[i][j]=0;
      sbchlc2[i][j]=0;
      sbchlc3[i][j]=0;
    }
  }
  et2momr=0.;
  net2mom=0;
}
//-----------------------------
void ECREUNcalib::select(){
  int i,j,k;
  integer sta,status,dbstat,id,idd,isl,pmt,sbc,pmsl,rdir,ipl;
  integer nhtot(0);
  geant padc[3];
  integer npmx,nslmx,maxpl,maxcl,nraw,ovfl[2],proj,plane,cell,lbin;
  number radc[2],binw,trc[3],pmb[3],rrr,sbl,sbr,crl,crr;
  number ct1,ct2,cl1,cl2,dct,z1,z2,hflen,ctcr,ctpm,clcr,clsh,clpm,acorr;
  number sumh[ECPMSMX][4],suml[ECPMSMX][4],pmsum,pmdis;
  number sumhe[ECPMSMX][4]; 
  number wbuff[ECSLMX][ECPMMX][4];
  number webuff[ECSLMX][ECPMMX][4];
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl,apmt,epmt,epix,apix,an2dy;
  geant pmpit,pxsiz;
  AMSEcalRawEvent * ptr;
  AMSEcalHit * ptr1;
//
  AMSContainer *cptr;
  AMSParticle *ppart;
  AMSTrTrack *ptrack;
  AMSCharge  *pcharge;
  AMSBeta  *pbeta;
  AMSPoint C0,Cout1,Cout2,Cpmt;
  AMSDir dir(0,0,1.);
  int ntrk,ipatt;
  number chi2,the,phi,rid,err,trl,beta,ebeta,momentum,betap,pmass;
  integer chargeTOF,chargeTracker;
//
  npmx=ECALDBc::slstruc(4);//numb.of PM's/Sl(36)
  nslmx=ECALDBc::slstruc(3);//numb.of Slayers(9)
  pxsiz=ECALDBc::rdcell(5);// pm(lg)-pixel size(imply =pm_size/2)
  pmpit=ECALDBc::rdcell(7);// pm transv.pitch
  maxpl=2*ECALDBc::slstruc(3);// SubCell(Pix) planes(18)
  maxcl=2*ECALDBc::slstruc(4);// SubCells per plane(72)
//
// ---------------> get track info :
//
  EcalJobStat::addca(0);
// 
  bool trktr,trdtr,ecaltr,nottr,badint;
  integer trpatt;
//
  rid=0;
  the=0;
  chi2=-1;
  ntrk=0;
  sta=1;
  cptr=AMSEvent::gethead()->getC("AMSParticle",0);// envelop 0
  if(cptr)ntrk+=cptr->getnelem();
  if(ntrk!=1)return;// require events with 1 part.
  ppart=(AMSParticle*)AMSEvent::gethead()->
                                           getheadC("AMSParticle",0);
  if(ppart){
    ptrack=ppart->getptrack();
    if(ptrack){//check track-type
      trdtr=(ptrack->checkstatus(AMSDBc::TRDTRACK)!=0);
      ecaltr=(ptrack->checkstatus(AMSDBc::ECALTRACK)!=0);
      nottr=(ptrack->checkstatus(AMSDBc::NOTRACK)!=0);
      badint=(ptrack->checkstatus(AMSDBc::BADINTERPOL)!=0);
      if(!(nottr || ecaltr || badint || trdtr)){//use only TRK-track particle
        trpatt=ptrack->getpattern();//TRK-track pattern
#ifdef _PGTRACK_
	if(!(ptrack->IsFake()))
#else
	if(trpatt>=0)
#endif
	{//trk-track ok
          ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
          pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
          chargeTracker=pcharge->getchargeTracker();
          chargeTOF=pcharge->getchargeTOF();
          pbeta=pcharge->getpbeta();
          beta=0.;
          if(pbeta)beta=pbeta->getbeta();
          ebeta=pbeta->getebeta()*beta*beta;
	}
      }
    }
  } 
//
  if(chi2<0)return;
//
  EcalJobStat::addca(1);
// ---------------> check that track is high-mom,top->bot and may hit ECAL:
//
  if(MAGSFFKEY.magstat<1){
    if(ECCAFFKEY.prtuse==-1)rid=3;//magnet-off, average earth Mu
    if(ECCAFFKEY.prtuse==1)rid=2;//magnet-off, average space He4
    if(ECCAFFKEY.prtuse==0)rid=2;//magnet-off, average space Pr
  }
//
  if(ECCAFFKEY.prtuse==-1)pmass=0.105;//gev
  if(ECCAFFKEY.prtuse==0)pmass=0.938;
  if(ECCAFFKEY.prtuse==1)pmass=3.84;
  momentum=rid*chargeTracker;
  betap=momentum/sqrt(momentum*momentum+pmass*pmass);
//
  if(ECCAFFKEY.hprintf>1){
    HF1(ECHISTC+12,geant(rid),1.);
    HF1(ECHISTC+6,geant(chi2),1.);
    HF1(ECHISTC+35,geant(beta),1.);
    HF1(ECHISTC+36,geant(chargeTracker),1.);
  }
//
  if(ECCAFFKEY.prtuse==1){//select He4
    if(chargeTracker!=2)return;
  }
  else{//select Protons/mu
    if(chargeTracker!=1)return;
  }
  EcalJobStat::addca(2);
//---
  if(ECCAFFKEY.hprintf>1){
    HF1(ECHISTC+47,geant(betap),1.);
  }
//
  if(beta<0 || (rid<0 && ECCAFFKEY.prtuse>=0 ))return;//---> it is not He4(prot)(charge>0) from AMS_top->bot
  if(chi2>600.)return;//---> bad chi2
//  if(fabs(rid)<ECCAFFKEY.trmin)return;//---> too low rigidity(too high mult.scatt)
  if(beta<0.8 || beta>1.2)return;// ----> remove low/wrong beta
  EcalJobStat::addca(3);
//
  number dx,dy,sfg(0.5);
  int icr(0);
  number adcmax;
  if(ECCAFFKEY.prtuse<=0)adcmax=ECCAFFKEY.adcpmx;//prot/mu
  else adcmax=4*ECCAFFKEY.adcpmx;//He
//
//---> Nhits when no crossing requirement:
  nhtot=0;
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell(pix)-Planes loop(0-17)
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    while(ptr1){ // <--- EcalHits(fired subcells=pixels) loop in pix-plane:
      ptr1->getadc(padc);//get raw ampl (Ah,Al,Ad already ovfl-corrected)
      if(padc[0]>ECCAFFKEY.adcmin)nhtot+=1;
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in pixPlane
  }//---> end of PixPlanes-loop
//----
  C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
  C0[0]=0.;
  C0[1]=0.;
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
  if(ECCAFFKEY.hprintf>0){
    HF1(ECHISTC,geant(cos(the)),1.);
  }
  if(fabs(cos(the))<0.94)return;// ----> check the impact angle(should be < 20degr)
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
//
  C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
  if(icr<=1)return;// ----> No crossing 
//
  EcalJobStat::addca(4);
//
//--------------------------------> select Punch-Through events and fill working arrays:
//   (use EcalHits with Edep already gain-corrected(though using Previous(!) calibration)
//
  number edthr,edep,edlpr[2*ECSLMX],edtpr[2*ECPMSMX];
  integer nsclpr[2*ECSLMX];
  integer refsl,refpm,imax,imin;
  int badsc,badscl(0);
  geant ad2mv;
//
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
//---> cut on number of hits to remove e/gammas:
//
  nhtot=0;
  geant ectotprev(0);
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell(pix)-Planes loop(0-17)
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    while(ptr1){ // <--- EcalHits(fired subcells=pixels) loop in pix-plane:
      ptr1->getadc(padc);//get raw ampl (Ah,Al,Ad already ovfl-corrected)
      edep=ptr1->getedep();//(mev) is already gain-corrected using previous calibration data !!!
      if(padc[0]>ECCAFFKEY.adcmin){//~2adc
        nhtot+=1;
        ectotprev+=edep;//mev
      }
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in pixPlane
  }//---> end of PixPlanes-loop
  if(ECCAFFKEY.hprintf>1){
    HF1(ECHISTC+37,ectotprev,1.);
    HF1(ECHISTC+38,geant(nhtot),1.);
  }
//---
  if((nhtot > 36) || (nhtot < 15))return;// tempor cut-value to remove e/gammas, etc...
  EcalJobStat::addca(9);
//---
  integer plmask[2*ECSLMX];
  for(ipl=0;ipl<maxpl;ipl++){//0-17
    plmask[ipl]=1;//flag of good PixLayers
  }
//
  for(isl=0;isl<nslmx;isl++){//sl(1-9)
    for(i=0;i<npmx;i++){//pmts(1-36) in SL
      for(k=0;k<4;k++){
        wbuff[isl][i][k]=0;
        webuff[isl][i][k]=0;
      }
    }
  }
//---> check Pix/Plane pattern:
//
geant pm17(0),pm18(0),pmd17(0),pmd18(0);
  for(ipl=0;ipl<maxpl;ipl++){ // <----PixPlanes loop(0-17)
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    for(i=0;i<maxcl;i++){//1-72
      edtpr[i]=0.;
    }
    edlpr[ipl]=0.;
    nsclpr[ipl]=0;
    while(ptr1){ // <--- EcalHits(fired subcells=pixels) loop in pix-plane:
      ptr1->getadc(padc);//get raw ampl (Ah,Al,Ad already ovfl-corrected)
      id=ptr1->getid();//LTTP(sLayer/pmTube/Pixel)
      sbc=id%10-1;//SubCell(0-3)
      edep=ptr1->getedep();//(mev) is already gain-corrected using previous calibration data !!!
      proj=ptr1->getproj();//0/1->X/Y (have to be corresponding to ipl)
      cell=ptr1->getcell();// 0,...71
      isl=ipl/2;//0-8
      pmt=cell/2;//0-35
      if(isl==5 && pmt==16){
        if(padc[0]>0)pm17+=padc[0];
	if(padc[2]>0)pmd17+=padc[2];
      }
      if(isl==5 && pmt==17){
        if(padc[0]>0)pm18+=padc[0];
	if(padc[2]>0)pmd18+=padc[2];
      }
      if(padc[0]>ECCAFFKEY.adcmin){
        wbuff[isl][pmt][sbc]=padc[0];//pix Ah for later use
	webuff[isl][pmt][sbc]=edep;//pix Edep(GainCorrected)
        if(ECCAFFKEY.hprintf>1){
	  HF1(ECHISTC+24,geant(edep/ad2mv),1.);
	}
        if(edep>=(ad2mv*ECCAFFKEY.adcmin)){// because adcmin-threshold is in ADC-units
          edlpr[ipl]+=edep;//plane edep
	  edtpr[cell]+=edep;//pix edep
        }
      }
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in pixPlane
    badsc=0;
    imin=-1;
    imax=999;
    for(i=0;i<maxcl;i++){// <--- exam. patt. of fired pixels(1-72) in plane ipl
      if(edtpr[i]>adcmax*ad2mv)badsc+=1;//count pix with high signal(interact.,ovfl,...) ???
      if(edtpr[i]>0){
        nsclpr[ipl]+=1;//count fired SubCell's per plane
	imax=i;//to find most right fired sc
	if(imin<0)imin=i;//to find most left fired sc
      }
    }//---> end of SubCell loop in plane
    k=imax-imin;
//
    if(ECCAFFKEY.hprintf>1){
      HF1(ECHISTC+25,geant(nsclpr[ipl]),1.);
    }
    if(nsclpr[ipl]>2 || k>1 || badsc>1){//bad PixPlane(>2 pixels(or 2 but separated) or >1 with high signal)
      badscl+=1;
      plmask[ipl]=0;//mark this PixLayer as bad
      edlpr[ipl]=0;//set to 0 plane edep in bad plane
    }
  }//---> end of PixPlanes-loop
  HF1(ECHISTC+115,pm17,1.);
  HF1(ECHISTC+116,pm18,1.);
  HF1(ECHISTC+117,pmd17,1.);
  HF1(ECHISTC+118,pmd18,1.);
//----
//
// -----> mark prev "good" PixPlanes with highest Edep (calc. Truncated average Edep/plane
//                                                                 to simplify Prot/He4 selection):
//
  number *pntro[2*ECSLMX];
  number *pntrn[2*ECSLMX];
  integer ngd(0);
  number ampo,ampn,edept;
  for(ipl=0;ipl<maxpl;ipl++){
    pntro[ipl]=&edlpr[ipl];
    pntrn[ipl]=&edlpr[ipl];
  }
  AMSsortNAG(pntrn,maxpl);//sort PixPlanes-Energies in increasing order
  for(i=0;i<ECCAFFKEY.ntruncl;i++){// mark Ntrunc(2) PixPlanes having highest Edep
    for(ipl=0;ipl<maxpl;ipl++){
      if(pntro[ipl]==pntrn[maxpl-1-i]){
	plmask[ipl]=0;//mark PixPlanes with highest Edep(delt-e)
	badscl+=1;
      }
    }
  }
//
  if(ECCAFFKEY.hprintf>1){
    HF1(ECHISTC+17,geant(badscl),1.);
  }
//
  if(ECCAFFKEY.prtuse==1){// He4
    if(badscl>ECCAFFKEY.badplmx)return;// <----limit on number of bad pix planes
  }
  else{// Prot/mu
    if(badscl>ECCAFFKEY.badplmx+1)return;// <----limit on number of bad pix planes
  }
  EcalJobStat::addca(5);
//
// ----> check Etrunc using good planes (to be sure that have needed particle(p or He or ...):
  edept=0.;
  for(isl=0;isl<nslmx;isl++){
    if((edlpr[2*isl]*plmask[2*isl]>0) && (edlpr[2*isl+1]*plmask[2*isl+1]>0)){
      edept+=(edlpr[2*isl]+edlpr[2*isl+1]);
      ngd+=1;
      EcalJobStat::nprofac[isl]+=1.;
      EcalJobStat::zprofac[isl]+=(edlpr[2*isl]+edlpr[2*isl+1]);
      if(ECCAFFKEY.hprintf>1){
        HF1(ECHISTC+28,geant(isl+1),1.);
      }
    }
  }
  edept/=ngd;
  if(ECCAFFKEY.hprintf>1){
    HF1(ECHISTC+16,geant(edept),1.);
  }
  if(ECCAFFKEY.prtuse==1){//select He4
    if(edept>ECCAFFKEY.etrunmx || edept<ECCAFFKEY.etrunmn)return;//--->Etrunc->He4 ?
  }
  else{//select Protons/muons
    if(edept>ECCAFFKEY.etrunmx/4 || edept<ECCAFFKEY.etrunmn/4)return;//--->Etrunc->prot/mu ?
  }
  EcalJobStat::addca(6);
// 
//
// ==========================> find all crossed or fired/matched ECAL RawHits  in SL:
//
  number expm;//extention of PM transv.size for preselection of PM-Track crossings 
  number exsc;//extention of SubCell transv.size for search of Sub_Cell-Track crossings
  number zpass;//particle pass from ECAL Zfront to middle of SL
  number mscat;//mult.scatt.par.
  int sccr,kdir,ins,ibmem;
  bool patt1,patt2,patt3,pat4pxrg,pat4pmrg,center;
  number pixam[4],pixed[4];
  integer pixcr[4];
  mscat=pow((13.6/fabs(betap*rid)/1000.),2)/3./1.04;// ~ SigmaMS**2 /dz**3 (use 1X0~1.04cm)
  mscat=mscat*pow(ECCAFFKEY.mscatp,2);// empirical fine tuning (increase of SigmaMS)
//
  for(isl=0;isl<ECALDBc::slstruc(3);isl++){ // <-------------- super-layer loop(0-8)
    if(plmask[2*isl]==0 || plmask[2*isl+1]==0)continue;//bad SL(with high Edep) --> try next
//
    nraw=0;
    for(i=0;i<npmx;i++){//pmts(0-35) in SL
      for(k=0;k<4;k++){
        sumh[i][k]=wbuff[isl][i][k];//36x4 <- 9x36x4
        sumhe[i][k]=webuff[isl][i][k];//36x4 <- 9x36x4
	nraw+=1;//count !=0 pixels in SL
      }
    }
    if(nraw<2)continue;// require at least 2 Pix per SL
//
// ---> extrapolate track to S-Layer "isl" :
//
    ECALDBc::getscinfoa(isl,0,0,proj,plane,cell,ct1,cl1,z1);//1stPM,1stPixel
    ECALDBc::getscinfoa(isl,0,2,proj,plane,cell,ct2,cl2,z2);//1stPM,3rdPixel
    C0[2]=(z1+z2)/2.+ECALDBc::rdcell(5);// Z-top of PM(LG) window(no gap betw.LG-1/4parts?)
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<---cross with SL at Z=PMtop
    if(proj==0)trc[0]=Cout1[0];//x-proj(0)
    else trc[0]=Cout1[1];//y-proj(1)
    if(ECCAFFKEY.hprintf>1 && isl==0){
      HF1(ECHISTC+1,geant(Cout1[0]),1.);
      HF1(ECHISTC+2,geant(Cout1[1]),1.);
    }
    C0[2]=(z1+z2)/2.-ECALDBc::rdcell(5);// Z-bot of PM(LG) window(no gap betw.LG-1/4parts?)
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout2,the,phi,trl);//<---cross with SL at Z=PMbottom
    if(proj==0)trc[2]=Cout2[0];//x-proj(0)
    else trc[2]=Cout2[1];//y-proj(1)
    trc[1]=(trc[0]+trc[2])/2.;//trc[0->2] are transv.cross.coo with PM at Z-top/mid/bot
    kdir=0;// neans "\"
    if(trc[0]>trc[2]){//arrange in increasing order
      rrr=trc[0];
      trc[0]=trc[2];
      trc[2]=rrr;
      kdir=1;// means "/"
    }
//
// ---> check track-SL longit. matching (transv.matching with PM/cell done below ):
//
    Cpmt=(Cout1+Cout2)/2.;//cross.coo at middle of SL(PM)
//    if(isl==0){//<--- check TRK imp.point accuracy(MC only)
//      HF1(ECHISTC+33,geant(Cpmt[0]-AMSEcalMCHit::impoint[0]),1.);
//      HF1(ECHISTC+34,geant(Cpmt[1]-AMSEcalMCHit::impoint[1]),1.);
//    }
    zpass=ECALDBc::gendim(7)-Cpmt[2];//dist.from ECAL Zfront till SL-middle
    if(proj==0){// <--- x-proj (0)
      ctcr=Cpmt[0];
      clcr=Cpmt[1];
      hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir
      clsh=ECALDBc::gendim(6);//EC-pos in y
      acorr=1./sqrt(1.+pow(sin(the)*sin(phi)/cos(the),2));
      exsc=sqrt(pow(ECCAFFKEY.trxac,2)+mscat*pow(zpass,3));//imp.point accuracy vs SL
      exsc*=ECCAFFKEY.nsigtrk;//N-Sig extent. to Pix transv.size
    }
    else{//        <--- y-proj (1)
      ctcr=Cpmt[1];
      clcr=Cpmt[0];
      hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber(EC) length in X-dir
      clsh=ECALDBc::gendim(5);//EC-pos in x
      acorr=1./sqrt(1.+pow(sin(the)*cos(phi)/cos(the),2));
      exsc=sqrt(pow(ECCAFFKEY.tryac,2)+mscat*pow(zpass,3));
      exsc*=ECCAFFKEY.nsigtrk;
    }
    if(fabs(clcr-clsh)>hflen)continue;//track out of SL ===> try next SL
    if(ECCAFFKEY.hprintf>1){
      HF2(ECHISTC+46,geant(isl+1),geant(exsc),1.);
    }
    expm=exsc;//use pm-cell size extention as for pixel  
//
    for(pmt=0;pmt<npmx;pmt++){ // <======== loop over PM's(0-35) to find crossed and fired cells
//
      if(proj==0){// <--- x-proj (0)
        rdir=(1-2*(pmt%2))*ECALDBc::slstruc(5);//readout dir(+-1->along pos/neg Y)
      }
      else{//        <--- y-proj (1)
        rdir=(1-2*(pmt%2))*ECALDBc::slstruc(6);//readout dir(+-1->along pos/neg X)
      }
      clpm=(cl1+cl2)/2.;// middle of fiber in AMS r.s.!
      if(rdir>0){
        clpm=clpm-hflen;//pm-pos if pm is at -x(-y) (i.e. rdir=1)
        pmdis=clcr-clpm;//0-2*hflen
      }
      else{
        clpm=clpm+hflen;//pm-pos if pm is at +x(+y) (i.e. rdir=-1)
        pmdis=clpm-clcr;//0-2*hflen
      }
//define longit.bin:
      pmdis=fabs(clcr-clpm);//cr.point dist. from PM (0-2*hflen)
      binw=2.*hflen/ECCLBMX;
      lbin=integer(floor(pmdis/binw));//longit.crossing bin number
      if(lbin>=ECCLBMX)continue;
      center=(lbin>=(ECCLBMX/2-ECLBMID) && lbin<=(ECCLBMX/2+ECLBMID-1));//ECAL-center(few central bins)
//
      pmsl=pmt+ECPMSMX*isl;//sequential numbering of PM's over all superlayers
      pmb[1]=-(npmx-1)*pmpit/2.+pmt*pmpit;//transv.coo for middle of PM
      pmb[0]=pmb[1]-pxsiz;//transv.coo for left edge of PM
      pmb[2]=pmb[1]+pxsiz;//transv.coo for right edge of PM
      sbl=pmb[0];
      sbr=pmb[2];
//      sbl-=expm;//increase PM-transv.size for safe PM-Track crossing check
//      sbr+=expm;
      sbl+=expm;//decrease PM-transv.size for safe PM-Track crossing check
      sbr-=expm;
//-------> PM-cross. pre-check (to speed up SubCell-crossing search):
      if(sbr<trc[0] || sbl>trc[2]){//hard crossing(full path in PM)
//      if(sbr<trc[1] || sbl>trc[1]){//soft crossing(middle of PM)
      }//                                   <--- No crossing with PM
//
      else{//          <--------- Found Pre-Crossing with PM, now check SubCells
//
        apmt=0.;
	epmt=0;
	sccr=0;
	for(i=0;i<4;i++){
	  pixam[i]=0;
	  pixed[i]=0;
	  pixcr[i]=0;
	}
//
        for(sbc=0;sbc<4;sbc++){// <-------- 4 pixels loop
          if(sbc%2==0){//<-left column of subcells
	    sbl=pmb[0];//subcell left/right coo
	    sbr=pmb[1];
	  }
	  else{//        <-right column of subcells
	    sbl=pmb[1];
	    sbr=pmb[2];
	  }
//
	  if(sbc<=1){//<---top pair of subcells
	    if(kdir==0){
	      crl=trc[0];//crossing left/right coo
	      crr=trc[1];
	    }
	    else{
	      crl=trc[1];
	      crr=trc[2];
	    }
	  }
	  else{//      <---bot pair of subcells
	    if(kdir==0){
	      crl=trc[1];//crossing left/right coo
	      crr=trc[2];
	    }
	    else{
	      crl=trc[0];
	      crr=trc[1];
	    }
	  }
//          sbl-=exsc;//increase SC-transv.size for TRK_accuracy+MulScat compensation !!!
//          sbr+=exsc;
          sbl+=exsc;//decrease PM-transv.size for safe Pix-Track crossing check
          sbr-=exsc;
	  if((crl<=sbr && crl>=sbl) || (crr<=sbr && crr>=sbl)){// <------------ Found crossing with pixel
	    radc[0]=sumh[pmt][sbc];// hi-ch, ADC-ch
	    epix=sumhe[pmt][sbc];// hi-ch Edep(GainCorrected)
	    acorr=1.;
	    radc[0]*=acorr;//normalization to norm.incidence(for fiber long.dir only !!)
	    epix*=acorr;
	    if(ECCAFFKEY.hprintf>1){
	      rrr=0.5*(crl+crr)-0.5*(sbl+sbr);//check Pix-TrkCros mismatch
	      if(proj==0)HF1(ECHISTC+39,geant(rrr),1.);
	      else HF1(ECHISTC+40,geant(rrr),1.);
	    }
	    sccr+=1;
	    pixam[sbc]=radc[0];
	    pixed[sbc]=epix;
//	    pixed[sbc]=radc[0];//tempor
	    pixcr[sbc]=1;//soft crossing
	    if((crl<=sbr && crl>=sbl) && (crr<=sbr && crr>=sbl))pixcr[sbc]=2;//hard crossing
          }//---> endif of found pixel crossing
        }//-------> end of 4 pixels loop
//
        patt1=((pixcr[0]==2 && pixcr[2]==2) || (pixcr[1]==2 && pixcr[3]==2));//pix 0&2|1&3 hard
        patt2=(pixcr[0]==2 || pixcr[2]==2 || pixcr[1]==2 || pixcr[3]==2);//at least 1 pix hard
	patt3=(pixcr[0]>=1 || pixcr[2]>=1 || pixcr[1]>=1 || pixcr[3]>=1);//at least 1 pix soft
	pat4pxrg=patt1;//patt for pix rel.gain calcul.
	pat4pmrg=patt2;//patt for pmt rel.gain calcul
//
	if(sccr>0){//at least 1 crossing
	  sta=0;
	  for(sbc=0;sbc<4;sbc++){// <-------- pix-loop
	    if(pixcr[sbc]==0)continue;//no any(soft|hard) crossing
            ECPMPeds::pmpeds[isl][pmt].getpedh(pedh);
            ECPMPeds::pmpeds[isl][pmt].getsigh(sigh);
            ECPMPeds::pmpeds[isl][pmt].getpedl(pedl);
            ECPMPeds::pmpeds[isl][pmt].getsigl(sigl);
            ph=pedh[sbc];
            sh=sigh[sbc];
            pl=pedl[sbc];
            sl=sigl[sbc];
	    apix=pixam[sbc];
	    epix=pixed[sbc]/ad2mv;//Edep->ADC ( gain-corrected(prev.calib) ADC)
	    apmt+=apix;
	    epmt+=epix;
//--->for Pix rel.gains calibration:
            ovfl[0]=0;
            if(apix>0 && (ECADCMX[0]-(apix+ph))<=4.*sh)ovfl[0]=1;// mark as ADC-Overflow
            if(pat4pxrg){//<-------- select pix-crossing pattern for Pix rel.gains calib:
	      tevsbc[pmsl][sbc]+=1.;//count crossed subcells(36x9)x4
	      if(apix>0 && ovfl[0]==0){//select fired pixel + no ovfl
	        if(ECCAFFKEY.hprintf>1){
	          HF1(ECHISTC+44,apix,1.);//single pix signals(adc)
	          HF1(ECHISTC+45,epix,1.);//single pix signals(adc, GainCorr)
//                  if(center && isl==0 && pmt==17)HF1(ECHISTC+41,geant(apix),1.);
                  if(isl==1 && pmt==19)HF1(ECHISTC+41,geant(apix),1.);
	        }
                sbcres[pmsl][sbc]+=apix;
                sbcresc[pmsl][sbc]+=epix;//used for check of calib
                tevsbf[pmsl][sbc]+=1.;//count fired subcells(36x9)x4
              }//---> endif of fired pixel
	    }//--->endif of crossed pixel + pattern1
//
	  }//--->endof pix-loop
//---------
//--> general PM-eff:
          if(pat4pmrg){
            tevpmc[pmsl]+=1.;//count crossed(by pix-patt) PMs (all long.bins)
	    if(apmt>0)tevpmf[pmsl]+=1.;//count fired PM's
	  }
//--> FIAT-calib: Raw-ampl responce vs long.bin, fill ampl-buff with ALL amps in decreas-order:
          if(pat4pmrg && apmt>0){//use non-corrected ampls !!!
            ins=0;
//	    if(isl==0 && pmt==17 && lbin==3)cout<<"---> going to add Amp="<<apmt<<" to evinbuf="<<tevpml[pmsl][lbin]<<endl;
	    if(tevpml[pmsl][lbin]<ECEVBUF){//keep All ampls in buffer !!!
	      if(tevpml[pmsl][lbin]>0){
	        for(int ib=0;ib<tevpml[pmsl][lbin];ib++){//<---buff-loop
	          if(apmt>pmlres[pmsl][lbin][ib]){
		    for(i=tevpml[pmsl][lbin];i>ib;i--){
		      pmlres[pmsl][lbin][i]=pmlres[pmsl][lbin][i-1];
		    }
		    pmlres[pmsl][lbin][ib]=apmt;
		    tevpml[pmsl][lbin]+=1;
		    ins=1;
		    break;
		  }
	        }//--->endof buf-loop
		if(ins==0){//put new <=ampt after the last in the sequence 
		  pmlres[pmsl][lbin][tevpml[pmsl][lbin]]=apmt;
		  tevpml[pmsl][lbin]+=1;
		  ins=1;
		}
	      }
	      else{//1st event
		pmlres[pmsl][lbin][tevpml[pmsl][lbin]]=apmt;
		tevpml[pmsl][lbin]+=1;
		ins=1;
	      }
	    }
//	    if(isl==0 && pmt==17 && lbin==3)cout<<"  After the try ins="<<ins<<endl;
//
//	    if(isl==0 && pmt==17 && lbin==3 && ins==1){
//	      cout<<endl;
//	      cout<<"  now events="<<tevpml[pmsl][lbin]<<" buf.cont:"<<endl;
//	      for(i=0;i<tevpml[pmsl][lbin];i++){
//	        cout<<pmlres[pmsl][lbin][i]<<" ";
//	        if((i+1)%10==0)cout<<endl;
//	      }
//	      cout<<endl; 
//	    }
//
          }
//--> for PM-relgain calib: look at PM-resp(4pix-sum,no gain-corr,central bins):
//      (keep only max ECAHBUF highest ampl events in buffer out of all(unlimited) events) 
          if(pat4pmrg && center && apmt>0 && pmccros[pmsl]<ECEVEMX){//ECAL-center
	    ins=0;
	    if(pmccros[pmsl]>0){//not 1st event
	      ibmem=ECAHBUF;
	      if(pmcrahm[pmsl]<ECAHBUF)ibmem=pmcrahm[pmsl];
	      for(int ib=0;ib<ibmem;ib++){//<---buff-loop
	        if(apmt>pmcrahb[pmsl][ib]){
		  for(i=ibmem;i>ib;i--){
		    pmcrahb[pmsl][i]=pmcrahb[pmsl][i-1];
		  }
		  pmcrahb[pmsl][ib]=apmt;
		  if(ibmem<ECAHBUF)pmcrahm[pmsl]+=1;
		  ins=1;
		  break;
	        }
              }//--->endof buf-loop
	      if(ins==0){//curr.amp <= last in buf: put it after the last in the sequence 
	        pmcrahb[pmsl][ibmem]=apmt;
	        if(ibmem<ECAHBUF)pmcrahm[pmsl]+=1;
	        ins=1;
	      }
//
	    }
	    else{//1st event
              pmcrahb[pmsl][0]=apmt;
	      pmcrahm[pmsl]+=1;
	      ins=1;
	    }
            pmcresp[pmsl]+=apmt;//sum PM-resp. for +-ECLBMID central bins
            pmcrespc[pmsl]+=epmt;//sum PM-resp. for +-ECLBMID central bins (gain-corr)
            pmccros[pmsl]+=1;//        ( need for PM RelGain Calibration)
            if(ECCAFFKEY.hprintf>1){
	      HF1(ECHISTC+29,apmt,1.);//4pix-sum (adc)
	      HF1(ECHISTC+30,epmt,1.);//4pix-sum (adc,GainCorr)
	    }
	  }
//--> ??? :	  
	  if(patt1){
	    if(ECCAFFKEY.hprintf>1){
	      HF1(ECHISTC+43,padc[2],1.);
	    }
	  }
//
	}//--->endof "at least 1 pix crossed
//
      }//---> endif of PM pre-crossing 
//
    } // ---> end of PM-loop loop in S-layer
//
  } // ---> end of super-layer loop
//
  if(sta)return;
  EcalJobStat::addca(7);
//  cout<<endl<<"========> Used for calib.event= "<<(AMSEvent::gethead()->getid())<<endl;
}
//-----------------------------------------------------------
void ECREUNcalib::makeToyRLGAfile(){
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  char fname[1024];
  char frdate[30];
  char vers1[3]="mc";
  char vers2[3]="sd";
  integer cfvn;
  int dig;
  geant pmrg,pxrg[4],hi2lr,a2dr;
  geant hi2lo;
//
//--> create RLGA-type 2 files(PM/SubCell rel.gains,Hi2Low-ratios,An2Dyn-ratios):
//    ...sd.cof simulate copy of RealData RLGA-calib.file used as MCSeed
//    ...mc.cof simulate RLGA-calibration OUTPUT-file using seeds from ...sd.cof file
//                                i.e. simulating calibration procedure accuracies !!!
// 
  integer endfm(12345);
  strcpy(inum,"0123456789");
  cfvn=ECMCFFKEY.calvern%1000;
  strcpy(fname,"ecalrlga");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  strcat(fname,vers2);//sd = copy of rl
  strcat(fname,".cof");
//---
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"ECREUNcalib::makeToyRLGAfile:Error opening ToyRLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for ToyRLGA-calibration output(MC-Seeds) "<<fname<<endl;
//---
    cout<<"Pixels/Dynode status will be written(MC-Seeds) !"<<endl;
    tcfile.setf(ios::fixed);
    tcfile << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(3);
        tcfile.precision(2);// precision for status
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<0<<" ";//all pixels are good
        }
        tcfile << endl;
      }
      tcfile.width(3);
      tcfile.precision(2);// precision for status
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile <<0<<" ";//all dynodes are good
      }
      tcfile << endl;
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"PM RelGains will be written(MC-Seeds) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile.width(5);
      tcfile.precision(2);// precision for PM Rel.gain
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	pmrg=1.+0.2*rnormx();
	if(pmrg>2)pmrg=2;
	if(pmrg<0.3)pmrg=0.3;
	pmrgain[pmsl]=pmrg;
        tcfile <<pmrgain[pmsl]<<" ";
      }
      tcfile << endl;
    }
    tcfile << endl;
    tcfile << endl;
    cout<<"...done!"<<endl;
//
//--- create pixel rel.gains:
    geant spxrg;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering
        spxrg=0;
        for(sc=0;sc<4;sc++){
	  pxrg[sc]=1.+0.05*rnormx();//5%
	  spxrg+=pxrg[sc];
	}
        for(sc=0;sc<4;sc++)pxrgain[pmsl][sc]=4*pxrg[sc]/spxrg;//normalise
      }
    }
//---    
    cout<<"Pixel RelGains will be written(MC-Seeds) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(5);
        tcfile.precision(2);// precision for Pixel rel.gain
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile <<pxrgain[pmsl][sc]<<" ";
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"Pixel Hi/Low Gain ratios will be written(MC-Seeds) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(5);
        tcfile.precision(1);// precision for Hi/Low ratio
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	  hi2lr=36*(1.+0.05*rnormx());//5%
	  hi2lowr[pmsl][sc]=hi2lr;
          tcfile <<hi2lowr[pmsl][sc]<<" ";
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"Pixel Hi/Low Gain offsets will be written(MC-Seeds) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile.width(6);
        tcfile.precision(2);// precision for Hi/Low ratio
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	  hi2lo=0.5*(1.+0.05*rnormx());//5%
	  hi2lowo[pmsl][sc]=hi2lo;
          tcfile <<hi2lowo[pmsl][sc]<<" ";
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    cout<<"...done!"<<endl;
//
    cout<<"PM Anode/Dynode ratios will be written !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile.width(4);
      tcfile.precision(1);// precision for PM Anode/Dynode ratio
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	a2dr=25*(1.+0.05*rnormx());//5%
	an2dynr[pmsl]=a2dr;
        tcfile <<an2dynr[pmsl]<<" ";
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    cout<<"...done!"<<endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" Toy MCSeeds RLGA-file  is done !"<<endl;
    tcfile.close();
    cout<<"ECREUNcalib: Toy MCSeeds RLGA-file closed !"<<endl;
//------------------------
// now simulate RLGA-calibr.procedure and create ...mc.cof file:
//
  strcpy(fname,"ecalrlga");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  strcat(fname,vers1);//mc
  strcat(fname,".cof");
//
    ofstream tcfile1(fname,ios::out|ios::trunc);
    if(!tcfile1){
      cerr<<"ECREUNcalib::makeToyRLGAfile:error opening RLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"Open file for ToyRLGA-calibration output(MC-calib) "<<fname<<endl;
//---
    cout<<"Pixels/Dynodes status will be written(MC-calib) !"<<endl;
    tcfile1.setf(ios::fixed);
    tcfile1 << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(3);
        tcfile1.precision(2);// precision for status
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<0<<" ";//all are good
        }
        tcfile1 << endl;
      }
      tcfile1.width(3);
      tcfile1.precision(2);// precision for status
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<0<<" ";//all dynodes are good
      }
      tcfile1 << endl;
      tcfile1 << endl;
    }
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//---
    cout<<"PM RelGains will be written(MC-calib) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile1.width(5);
      tcfile1.precision(2);// precision for PM Rel.gain
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<pmrgain[pmsl]*(1.+0.02*rnormx())<<" ";//2% calib.accuracy
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//
//--- create pixel rel.gains:
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering
	spxrg=0;
        for(sc=0;sc<4;sc++){
	  pxrg[sc]=pxrgain[pmsl][sc]*(1.+0.02*rnormx());//2% calib.accuracy
	  spxrg+=pxrg[sc];
	}
	for(sc=0;sc<4;sc++)pxrgain[pmsl][sc]=4*pxrg[sc]/spxrg;//normalise
      }
    }
//---    
    cout<<"Pixel RelGains will be written(MC-calib) !"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(5);
        tcfile1.precision(2);// precision for Pixel rel.gain
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<pxrgain[pmsl][sc]<<" ";
        }
        tcfile1 << endl;
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
    cout<<"...done!"<<endl;
//
    cout<<"Pixel Hi/Low Gain ratios will be written(MC-calib) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(5);
        tcfile1.precision(1);// precision for Hi/Low ratio
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<hi2lowr[pmsl][sc]*(1.+0.01*rnormx())<<" ";//1% calib.accuracy
        }
        tcfile1 << endl;
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
//
    cout<<"Pixel Hi/Low Gain offsets will be written(MC-calib) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        tcfile1.width(6);
        tcfile1.precision(2);// precision for Hi/Low offs
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile1 <<hi2lowo[pmsl][sc]*(1.+0.01*rnormx())<<" ";//1% calib.accuracy
        }
        tcfile1 << endl;
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
//
    cout<<"PM Anode/Dynode ratios will be written(MC-calib) !"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      tcfile1.width(4);
      tcfile1.precision(1);// precision for PM Anode/Dynode ratio
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile1 <<an2dynr[pmsl]*(1.+0.01*rnormx())<<" ";//1% calib.accuracy
      }
      tcfile1 << endl;
    }
    tcfile1 << endl;
//
    tcfile1 <<" "<<endfm<<endl;
    tcfile1 << endl;
//
    cout<<"...done!"<<endl;
    tcfile1 << endl<<"======================================================"<<endl;
    tcfile1 << endl<<" Toy RLGA-MC-calib file is done !"<<endl;
    tcfile1.close();
    cout<<"ECREUNcalib: Toy RLGA-MC-calib file closed !"<<endl;
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ECREUNcalib::mfit(){
//
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  geant arr[ECPMSMX*2];
  geant glscscan[2*ECSLMX];
  int nlscan,nof4;
  number rrr,sum4,sum4c,eff,hflen,binw;
  char htit1[60];
  char htit2[60];
  char ext[10];
  char inum[11];
  char in[2]="0";
  geant llim,hlim;
//
  cout<<endl<<"====> Starting ECREUNcalib::mfit...."<<endl<<endl;
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  pmslr=refpm-1+ECPMSMX*(refsl-1);//sequential numbering for ref. PM
  hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir(=X)
  binw=2.*hflen/ECCLBMX;
//
//---------> print hist. of event multiplicity/eff in cells:
//
  if(ECCAFFKEY.hprintf>2){
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){//1-9
    for(pm=0;pm<npmx;pm++){//1-36
      pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
      arr[2*pm]=tevsbc[pmsl][0];
      arr[2*pm+1]=tevsbc[pmsl][1];
    }
    strcpy(htit1,"Crossings/Pixel (72 top pixels row of SL ");
    in[0]=inum[sl+1];
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;
      arr[2*pm]=tevsbc[pmsl][2];
      arr[2*pm+1]=tevsbc[pmsl][3];
    }
    strcpy(htit1,"Crossings/Pixel (72 bot pixels row of SL ");
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
  }
// now Pixel-efficiencies:
//  
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){//<-----SL-loop
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
      arr[2*pm]=0.;
      arr[2*pm+1]=0.;
      if(tevsbc[pmsl][0]>0)arr[2*pm]=tevsbf[pmsl][0]/tevsbc[pmsl][0];
      if(tevsbc[pmsl][1]>0)arr[2*pm+1]=tevsbf[pmsl][1]/tevsbc[pmsl][1];
    }
    strcpy(htit1,"Efficiency/Pixel (72 top pixels row of SL ");
    in[0]=inum[sl+1];
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    if(ECCAFFKEY.prtuse==1){//He4
      HMINIM(ECHISTC+7,0.1);
      HMAXIM(ECHISTC+7,1.1);
    }
    else{//prot/mu
      HMINIM(ECHISTC+7,0.1);
      HMAXIM(ECHISTC+7,1.1);
    }
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
//
    nlscan=0;
    glscscan[2*sl]=0.;
    for(i=0;i<2*npmx;i++){
      if(arr[i]>0.){
        glscscan[2*sl]+=arr[i];
	nlscan+=1;
      }
    }
    if(nlscan>0)glscscan[2*sl]/=geant(nlscan);
//
    for(pm=0;pm<npmx;pm++){
      pmsl=pm+ECPMSMX*sl;
      arr[2*pm]=0.;
      arr[2*pm+1]=0.;
      if(tevsbc[pmsl][2]>0)arr[2*pm]=tevsbf[pmsl][2]/tevsbc[pmsl][2];
      if(tevsbc[pmsl][3]>0)arr[2*pm+1]=tevsbf[pmsl][3]/tevsbc[pmsl][3];
    }
    strcpy(htit1,"Efficiency/Pixel (72 bot pixels row of SL ");
    strcat(htit1,in);
    HBOOK1(ECHISTC+7,htit1,72,1.,73.,0.);
    if(ECCAFFKEY.prtuse==1){//He4
      HMINIM(ECHISTC+7,0.1);
      HMAXIM(ECHISTC+7,1.1);
    }
    else{//prot/mu
      HMINIM(ECHISTC+7,0.1);
      HMAXIM(ECHISTC+7,1.1);
    }
    HPAK(ECHISTC+7,arr);
    HPRINT(ECHISTC+7);
    HDELET(ECHISTC+7);
//
    nlscan=0;
    glscscan[2*sl+1]=0.;
    for(i=0;i<2*npmx;i++){
      if(arr[i]>0.){
        glscscan[2*sl+1]+=arr[i];
	nlscan+=1;
      }
    }
    if(nlscan>0)glscscan[2*sl+1]/=geant(nlscan);
//
  }//---> end of sl loop
  HPAK(ECHISTC+14,glscscan);
  }//---> endif of printing
//
//------------> look at SubCell/PM's efficiency in projections:
//
  geant pmeflsc[ECSLMX];
  if(ECCAFFKEY.hprintf>2){
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){//<-----SL-loop
      k=0;
      pmeflsc[sl]=0.;
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        for(i=0;i<4;i++){
	  eff=0.;
          if(tevsbc[pmsl][i]>0.)eff=tevsbf[pmsl][i]/tevsbc[pmsl][i];
          if(sl%2==0)HF1(ECHISTC+26,geant(eff),1.);
          if(sl%2==1)HF1(ECHISTC+27,geant(eff),1.);
	}
	eff=0.;
	if(tevpmc[pmsl]>0.)eff=tevpmf[pmsl]/tevpmc[pmsl];
        if(sl%2==0)HF1(ECHISTC+31,geant(eff),1.);
        if(sl%2==1)HF1(ECHISTC+32,geant(eff),1.);
	if(eff>0.){
	  k+=1;
	  pmeflsc[sl]+=eff;
	}
      }//---> pm loop
      if(k>0)pmeflsc[sl]/=geant(k);
    }//---> sl loop
    HPAK(ECHISTC+15,pmeflsc);
  }
//
//
// ------------> check that ref.pm OK (pm & sc  eff. are reasonable):
//
  cout<<endl;
  cout<<"      ECREUNcalib:: RefPM: SL="<<refsl<<" PM="<<refpm<<" Crossings :"<<tevpmc[pmslr]<<endl;
  cout<<"      Crossings in 4-subc: "<<tevsbc[pmslr][0]<<" "<<tevsbc[pmslr][1]                    
                                     <<" "<<tevsbc[pmslr][2]<<" "<<tevsbc[pmslr][3]<<endl;
  bad=0;
  if(tevpmc[pmslr]>5.){
    for(i=0;i<4;i++){
      if(tevsbc[pmslr][i]<15.)bad=1;//too low cross.statistics/subc
      else{
        eff=tevsbf[pmslr][i]/tevsbc[pmslr][i];//fired/crossed
	if(eff<0.3)bad=1;//too low eff
      }
    }
  }
  else bad=1;
  if(bad){
    cout<<"<---- ECREUNcalib: Ref-PM problem, no calibration done! eff="<<eff<<" crs="<<tevpmc[pmslr]<<endl;
//    return;//tempor
  }
  cout<<endl<<endl;
//
// --------------> Calculate SubCell(pixel) relative gains:
//
  for(i=0;i<ECPMSL;i++){// <--- PM*SL loop
    pm=i%ECPMSMX+1;
    sl=integer(floor(number(i)/ECPMSMX))+1;
    sum4=0.;
    nof4=0;
    sum4c=0.;
    for(j=0;j<4;j++){// <--- 4 subcell loop
      eff=0;
      if(tevsbf[i][j]>20){
        eff=tevsbf[i][j]/tevsbc[i][j];
	sbcres[i][j]/=tevsbc[i][j];// aver. pix-signal(using "crossed" cases -> include pix-eff)
//	sbcres[i][j]/=tevsbf[i][j];// aver. pix-signal(using "fired" cases to exclude pix-eff)
	sbcresc[i][j]/=tevsbc[i][j];// aver.,Gcorr pix-signal(using "crossed" cases -> include pix-eff)
      } 
      if(eff<0.1){
        if(eff>0.)pxstat[i][j]=10;//limited quality subc(low eff)
        else pxstat[i][j]=90;//unusable subc(very low eff or statistics)
      }
      if(pxstat[i][j]==90)
          cout<<"   <--REUN: Low statist/eff Pixel, Sl/Pm/Pix="<<sl<<" "<<pm<<" "<<j<<" statF/eff="<<
	               tevsbc[i][j]<<" "<<eff<<endl;
      if(pxstat[i][j] != 90){
        sum4+=sbcres[i][j];
	nof4+=1;
        sum4c+=sbcresc[i][j];
      }
      if(ECCAFFKEY.hprintf>0)HF1(ECHISTC+8,geant(eff),1.);
    }// ---> end of 4sc loop 
//
    for(j=0;j<4;j++){
      if(sum4>0 && pxstat[i][j] != 90)pxrgain[i][j]=nof4*sbcres[i][j]/sum4;//"nof4" to have pxrg~1
      if(ECCAFFKEY.hprintf>0)HF1(ECHISTC+9,geant(pxrgain[i][j]),1.);
    }
    for(j=0;j<4;j++){
      if(sum4c>0 && pxstat[i][j] != 90)pxrgainc[i][j]=nof4*sbcresc[i][j]/sum4c;//"nof4" to have pxrg~1
      if(ECCAFFKEY.hprintf>0)HF1(ECHISTC+51,geant(pxrgainc[i][j]),1.);
    }
  }// ---> end of PM*SL loop
  cout<<endl<<endl;
//
//-------------->FIAT-calib: calc./FIT average resp. in each long.bin:
//
  geant pmprof[ECCLBMX],pmprer[ECCLBMX];
  int first(1); 
  int ier;  
  int ifit[4];
  char pnam[4][6],pnm[6];
  number argl[10];
  int iargl[10];
  number start[4],pstep[4],plow[4],phigh[4];
  strcpy(pnam[0],"anorm");
  strcpy(pnam[1],"lenhi");
  strcpy(pnam[2],"admix");
  strcpy(pnam[3],"lenlo");
  start[0]=50.;//anorm
  start[1]=310.;//lenhi
  start[2]=0.2;//admix
  start[3]=20.;//lenlow
  pstep[0]=5.;
  pstep[1]=10.;
  pstep[2]=0.05;
  pstep[3]=5.;
  plow[0]=1.;
  plow[1]=50.;
  plow[2]=0.;
  plow[3]=2.;
  phigh[0]=1000.;
  phigh[1]=10000.;
  phigh[2]=1.;
  phigh[3]=100.;
  for(i=0;i<4;i++)ifit[i]=1;//release all par's by default
  ifit[2]=0;//fix admix
//  ifit[3]=0;//fix lenlow
  slslow=0.;
  slfast=0.;
  fastfr=0.;
  nfits=0;
  cout<<"-----> Start FIAT-calibr, init. parameters..."<<endl;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("ECAL Fibers Longit.Resp.Uniformity-calibration(FIAT)");
  argl[0]=number(-1);
  MNEXCM(mfun,"SET PRINT",argl,1,ier,0);
  for(i=0;i<4;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"  <--- Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"  <--- Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
  cout<<"       init done, start fit ..."<<endl;
// ---> calc. profiles and fitting for monitored pm's:
//
  number p2reml=0.025;//events portion to remove from left side
  number p2remr=0.05;//events portion to remove from right side
  integer nrest,n2reml,n2remr;
  number lbaver,lberr;
//
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){ // <---- SL-loop
    for(pm=0;pm<npmx;pm++){ // <---- PM-loop
      pmsl=pm+ECPMSMX*sl;
      nbins=0;
//
      for(lb=0;lb<ECCLBMX;lb++){ // <---- Bin-loop (lb)
        lbaver=0;
	lberr=0;
	pmprof[lb]=0;
	pmprer[lb]=0;
        if(tevpml[pmsl][lb]>50){
	  if(pmsl==pmslr && ECCAFFKEY.hprintf!=0){
	    strcpy(htit1,"RefPMT: PMresponce(center) for Lbin=");
	    sprintf(ext,"%d",lb+1);
	    strcat(htit1,ext);
            if(ECCAFFKEY.prtuse<=0){//prot(mu)
	      llim=0;
	      hlim=200;
	    }
            else{//He
	      llim=0;
	      hlim=4*200;
	    }
            if(ECCAFFKEY.hprintf>2){
	      HBOOK1(ECHISTC+50,htit1,50,llim,hlim,0.);
	      for(k=0;k<tevpml[pmsl][lb];k++)HF1(ECHISTC+50,geant(pmlres[pmsl][lb][k]),1.);
	      HPRINT(ECHISTC+50);
	      HDELET(ECHISTC+50);
	    }
	  }
	  n2remr=floor(p2remr*tevpml[pmsl][lb]+0.5);
	  n2reml=floor(p2reml*tevpml[pmsl][lb]+0.5);
	  nrest=tevpml[pmsl][lb]-n2reml;
	  for(k=n2remr;k<nrest;k++){//use only Nbin-n2reml-n2remr "midium" edeps
	    lbaver+=pmlres[pmsl][lb][k];
	    lberr+=(pmlres[pmsl][lb][k]*pmlres[pmsl][lb][k]);
	  }
	  lbaver/=nrest;
	  lberr/=nrest;
	  rrr=lberr-lbaver*lbaver;//rms**2
	  if(rrr>=0)lberr=sqrt(rrr/nrest);//aver err
	  else{
	    cout<<"FIAT:wrong lbin rms, aver/rrr="<<lbaver<<" "<<rrr<<" n2rl/n2rr="<<n2reml<<" "<<n2remr<<endl;
	    continue;//try next bin
	  }
        }
	else continue;//try next bin
// ---> bin ok
//	if(pmsl==pmslr)
//	      cout<<"  RefPm: bin/nevs="<<lb<<" "<<tevpml[pmsl][lb]<<" aver/rms="<<lbaver<<" "<<lberr<<endl;
	pmprof[lb]=lbaver;
	pmprer[lb]=lberr;
	values[nbins]=lbaver;
	errors[nbins]=lberr;
	coords[nbins]=binw/2.+lb*binw;//0-fiblength
	nbins+=1;//counts good bins
      }//--->end of lb loop
//      
//      if((pm==1 || pm==16 || pm==17 || pm==18  pm==19 || pm==(npmx-2))){// <---- fit for monitored PM's(left,center,right)
      if(pm==16 || pm==17 || pm==18 || pm==19){// <---- fit for monitored PM's(left,center,right)
	cout<<endl;
        cout<<"-----> Start uniformity-fit for Sl/Pm="<<sl<<" "<<pm<<" nbins="<<nbins<<endl;
        if(ECCAFFKEY.hprintf!=0){
	  cout<<"       EvsInLbin/aver:"<<endl;
	  for(k=0;k<ECCLBMX;k++)cout<<tevpml[pmsl][k]<<" "<<pmprof[k]<<"  ";
	  cout<<endl<<endl;
	  if(nbins>0){
	    if(ECREFFKEY.relogic[1]==2 && ECCAFFKEY.hprintf>0){
	      HPAK(ECHISTC+10,pmprof);
              HPAKE(ECHISTC+10,pmprer);
              HPRINT(ECHISTC+10);
	    }
	  }
	}
        if(nbins>=(ECCLBMX-3)){//<-- do fit if have enough bins
          i=0;//<--- reinit for param #1 with realistic value from 1st bin
	  start[i]=values[0];
          strcpy(pnm,pnam[i]);
          ier=0;
          argl[0]=number(0);
          MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
          if(ier!=0){
            cout<<"  <--  Param-ReInit problem for param="<<pnam[i]<<'\n';
            exit(10);
          }
          argl[0]=0.;
          ier=0;
          MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
          if(ier!=0){
            cout<<"       MINIMIZE problem !"<<'\n';
            continue;
          }  
          MNEXCM(mfun,"MINOS",argl,0,ier,0);
          if(ier!=0){
            cout<<"       MINOS problem !"<<'\n';
            continue;
          }
          argl[0]=number(3);
          ier=0;
          MNEXCM(mfun,"CALL FCN",argl,1,ier,0);
          if(ier!=0){
            cout<<"      ECREUN-calib: final CALL_FCN problem !"<<'\n';
            continue;
          }
          cout<<"       -->>> fit done <<<--"<<endl<<endl;
        }//--> end of fit
	else{
	  cout<<endl;
	  cout<<"   <-- SL/PM="<<sl<<" "<<pm<<" not enought lbins to fit!, nbins="<<nbins<<endl; 
	}
      }//--> endif of monitored pm
    }//---> end of pm loop
  }//--->end of sl loop
  cout<<endl<<endl;
  cout<<"<----- REUN(FIAT)-calibration finished: number of good fits : "<<nfits<<endl;
  if(nfits>0){
    slslow/=number(nfits);
    slfast/=number(nfits);
    fastfr/=number(nfits);
    cout<<"      Average slow/fast/frac="<<slslow<<" "<<slfast<<" "<<fastfr<<" nfits="<<nfits<<
                                                                       endl<<endl<<endl<<endl;
  }
//
// ----------------> calc. PM relative gains (wrt aver(over EC) PM-responce):
//--->calc. aver.PMresp over EC as normalization:
  geant avr[ECPMSL];
  number avrtot(0),navrtot(0);
  number avrtotc(0);
  p2remr=0.05;//remove only highest(right) ampls
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){//<--SL-loop
    for(pm=0;pm<npmx;pm++){//<--Pm-loop
      pmsl=pm+ECPMSMX*sl;
      avr[pmsl]=0;
      if(pmccros[pmsl]>20){
        n2remr=floor(p2remr*pmccros[pmsl]+0.5);
	avr[pmsl]=pmcresp[pmsl];//total sum of all responces
	for(i=0;i<n2remr;i++){//remove n2remr highest amplitudes from total sum
	  avr[pmsl]-=pmcrahb[pmsl][i];//subtr. highest
	}
        avr[pmsl]/=(pmccros[pmsl]-n2remr);//pm trunc.average adc
	avrtot+=avr[pmsl];
	avrtotc+=(pmcrespc[pmsl]/pmccros[pmsl]);//pm aver adc=edep(gaincorrected by prev.cal)/adc2mev
	navrtot+=1;
      }
    }
  }
  if(navrtot>0){
    avrtot/=navrtot;
    avrtotc/=navrtot;
  }
  else{
    cout<<"<----- No PM-rel.gain calibr. done(empty ECAL),ref.PM crossings:"<<pmccros[pmslr]<<endl;
    return; 
  }
//  
  if(pmccros[pmslr]>20){
    cout<<"<----- Average(over ECAL) PM-responce(adc,no gain-corr)="<<avrtot<<", used PMs="<<navrtot<<endl;
    cout<<"       Ref.PM events/aver.responce="<<pmccros[pmslr]<<" "<<avr[pmslr]<<endl<<endl;
  }
  else cout<<"<----- Average(over ECAL) PM-responce(adc,no gain-corr)="<<avrtot<<", used PMs="<<navrtot<<endl<<endl;
//
  geant glscan[ECSLMX],gtscan[ECPMSMX];
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){//<--SL-loop
    glscan[sl]=0;
    nlscan=0;
    for(pm=0;pm<npmx;pm++){//<--Pm-loop
      pmsl=pm+ECPMSMX*sl;
      if(pmccros[pmsl]>20){
        n2remr=floor(p2remr*pmccros[pmsl]+0.5);
	for(i=0;i<n2remr;i++){//remove nev2rem highest amplitudes from total sum
	  pmcresp[pmsl]-=pmcrahb[pmsl][i];
	}
        pmcresp[pmsl]/=(pmccros[pmsl]-n2remr);//pm trunc.average adc
        pmrgain[pmsl]=pmcresp[pmsl]/avrtot;//relat(vrt aver.pm) pm-responce(rel.gain, trunc)
        pmcrespc[pmsl]/=pmccros[pmsl];
        pmrgainc[pmsl]=pmcrespc[pmsl]/avrtotc;//relat(vrt aver.pm) pm-responce(rel.gain corrected,prev.cal)
//	pmrgainc[pmsl]/=pmrgain[pmsl];//tempor
        nlscan+=1;
	glscan[sl]+=geant(pmrgain[pmsl]);
	if(ECCAFFKEY.hprintf>0){
	  HF1(ECHISTC+11,geant(pmrgain[pmsl]),1.);
	  HF1(ECHISTC+49,geant(pmrgainc[pmsl]),1.);
	}
      }
    }//---> end of Pm-loop
    if(nlscan>0)glscan[sl]/=geant(nlscan);
  }//--->endof SL-loop
  if(ECCAFFKEY.hprintf>0)HPAK(ECHISTC+13,glscan);//PM RelGain SL-profile
//
// ---------> print PM rel.gains:
//
  if(ECCAFFKEY.hprintf>0){
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        gtscan[pm]=pmrgain[pmsl];
      }
      strcpy(htit1,"PmRelGain vs PM-number in SL ");
      in[0]=inum[sl+1];
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx,1.,geant(npmx+1),0.);
      HMINIM(ECHISTC+7,0.);
      HMAXIM(ECHISTC+7,2.);
      HPAK(ECHISTC+7,gtscan);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
    }
//
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        gtscan[pm]=pmrgainc[pmsl];
      }
      strcpy(htit1,"PmRelGainCorrected vs PM-number in SL ");
      in[0]=inum[sl+1];
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx,1.,geant(npmx+1),0.);
      HMINIM(ECHISTC+7,0.);
      HMAXIM(ECHISTC+7,2.);
      HPAK(ECHISTC+7,gtscan);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
    }
//
// ---------> print SC rel.gains:
//
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        arr[2*pm]=0.;
        arr[2*pm+1]=0.;
        if(pxstat[pmsl][0]>=0)arr[2*pm]=pxrgain[pmsl][0];
        if(pxstat[pmsl][1]>=0)arr[2*pm+1]=pxrgain[pmsl][1];
      }
      strcpy(htit1,"RelGains of top cells in SL ");
      in[0]=inum[sl+1];
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx*2,1.,geant(2*npmx+1),0.);
      HMINIM(ECHISTC+7,0.2);
      HMAXIM(ECHISTC+7,1.5);
      HPAK(ECHISTC+7,arr);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;
        arr[2*pm]=0.;
        arr[2*pm+1]=0.;
        if(pxstat[pmsl][2]>=0)arr[2*pm]=pxrgain[pmsl][2];
        if(pxstat[pmsl][3]>=0)arr[2*pm+1]=pxrgain[pmsl][3];
      }
      strcpy(htit1,"RelGains of bot cells in SL ");
      strcat(htit1,in);
      HBOOK1(ECHISTC+7,htit1,npmx*2,1.,geant(2*npmx+1),0.);
      HMINIM(ECHISTC+7,0.2);
      HMAXIM(ECHISTC+7,1.5);
      HPAK(ECHISTC+7,arr);
      HPRINT(ECHISTC+7);
      HDELET(ECHISTC+7);
    }
  }//---> endif of printing
//--------------------------------------------------------
//
// ----------------> calc. hi2low ratios for each subcell:
//
  number t0,slo,t,tq,co,dis,nevf,bins,avs,avo;
  number sumc,sumt,sumct,sumc2,sumt2,sumid,nonzer;
  number chi2[ECPMSL*4],slop[ECPMSL*4],offs[ECPMSL*4];
  integer hchok[ECPMSL*4],ibinw;
  int ngdbins[ECPMSL*4];
  int gsbins,nevft;
  number offsf(0.5);//fixed offset for "fixed offset" fit
//
  ibinw=integer(floor(number(ECCADCR)/ECCHBMX));
  cout<<endl<<"<==== Start Hi2LowR calculation, HichBinw="<<ibinw<<endl;
  avs=0.;
  avo=0.;
  nonzer=0.;
  for(i=0;i<ECPMSL*4;i++){ // <-- Pix-chan. loop
    sumc=0.;
    sumt=0.;
    sumct=0.;
    sumc2=0.;
    sumt2=0.;
    sumid=0.;
    bins=0.;
    t0=0.;
    slo=0.;
    slop[i]=0.;
    offs[i]=0.;
    chi2[i]=0.;
    hchok[i]=0;
    ngdbins[i]=0;
    pmsl=i/4;
    sc=i%4;
    gsbins=0;
    nevft=0;
    for(j=0;j<ECCHBMX;j++){//<-- HchADC bin loop
      nevf=number(tevhlc[i][j]);//events in LowGain channel for HiGain chan bin=j
      if(nevf>=10){ // min. 10 events
        gsbins+=1;
	nevft+=nevf;
//        co=(number(j)+0.5)*ibinw;// mid. of High-chan. bin
        co=sbchlc3[i][j]/nevf;// aver. of High-chan. bin
        t=sbchlc[i][j]/nevf; // get mean
        tq=sbchlc2[i][j]/nevf; // mean square
        dis=tq-t*t;// rms**2
        if(dis>0.){
//          dis/=(nevf-1.);// rms**2 of the mean
          if(ECCAFFKEY.hprintf>0)HF1(ECHISTC+23,geant(sqrt(dis)),1.);
	  if(sqrt(dis)<3.){
            bins+=1.;
            sumc+=(co/dis);
            sumt+=(t/dis);
            sumid+=(1./dis);
            sumct+=(co*t/dis);
            sumc2+=(co*co/dis);
            sumt2+=(t*t/dis);
	    ngdbins[i]+=1;
	  }
//	  if(sqrt(dis)<2.5){
//            bins+=1.;
//            sumc+=(co/dis);//
//            sumt+=(t/dis);//
//            sumid+=(1./dis);//
//            sumct+=(co*t/dis);
//            sumc2+=(co*co/dis);
//            sumt2+=(t*t/dis);
//	    ngdbins[i]+=1;
//	  }//--> end of sig-check
        }//-->end of dis>0 check
      }//--> end of min evs/bin check
    }//--> end of bin loop
//
    if(gsbins<10){
      cout<<"----> Hi2LowR Warning : pmsl/pix="<<pmsl<<" "<<sc<<" have low NgoodStatBins="<<gsbins<<", Ntotevs="<<nevft<<endl;
    }
//
    if(bins>=10){
//-->complete fit:
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      slo=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[i]=sumt2+t0*t0*sumid+slo*slo*sumc2
        -2.*t0*sumt-2.*slo*sumct+2.*t0*slo*sumc;
      chi2[i]/=(bins-2.);
      slop[i]=1./slo;// goto "hi vs low" slope
      offs[i]=t0;
      if(chi2[i]>0 && chi2[i]<3. && slop[i]>25 && slop[i]<45){
//-->fixed(=0) offset fit:
//      slo=sumct/sumc2;
//      chi2[i]=sumt2+slo*slo*sumc2-2.*slo*sumct;
//      chi2[i]/=(bins-1.);
//      slop[i]=1./slo;// goto "hi vs low" slope
//      offs[i]=t0;
//      if(chi2[i]>0 && chi2[i]<5. && slop[i]>24 && slop[i]<37){
//-->fixed(0.5) offset fit:
//      slo=(sumct-offsf*sumc)/sumc2;
//      slop[i]=1./slo;// goto "hi vs low" slope
//      chi2[i]=sumt2+slo*slo*sumc2+offsf*offsf*sumid-2.*slo*sumct
//	      -2.*offsf*sumt+2.*slo*offsf*sumc;
//      chi2[i]/=(bins-1.);
//      offs[i]=offsf;
//      if(chi2[i]>0 && chi2[i]<3 && slop[i]>25 && slop[i]<45){
//--
        nonzer+=1.;
        avs+=slop[i];
        avo+=offs[i];
        hchok[i]=1;
      }
      if(ECCAFFKEY.hprintf>0){
        HF1(ECHISTC+20,geant(slop[i]),1.);
        HF1(ECHISTC+21,geant(offs[i]),1.);
        HF1(ECHISTC+22,geant(chi2[i]),1.);
        HF1(ECHISTC+42,geant(bins),1.);
      }
    }// endof Nbibs check
    else if(bins<10 && gsbins>=10){
      cout<<"----> Hi2LowR Warning : pmsl/pix="<<pmsl<<" "<<sc<<" have low NgoodBins="<<bins<<", Ntotevs="<<nevft<<endl;
    }
  }//--> end of chan. loop
//
  if(nonzer>0){
    avs/=nonzer;
    avo/=nonzer;
  }
  for(i=0;i<ECPMSL*4;i++){ // <-- SubCell-chan. loop
    pmsl=i/4;
    sc=i%4;
    if(hchok[i]==1){//means chi2&slope OK
      hi2lowr[pmsl][sc]=slop[i];
      hi2lowo[pmsl][sc]=offs[i];
    }
    else{
      if(slop[i]>29 && slop[i]<35){//chan. still ok (just chi2 too high, use more severe slope-cut)
        hi2lowr[pmsl][sc]=slop[i];
        hi2lowo[pmsl][sc]=offs[i];
      }
      else if(slop[i]>24 && ngdbins[i]>40){//big numb. of bins - accept...
        hi2lowr[pmsl][sc]=slop[i];
        hi2lowo[pmsl][sc]=offs[i];
//        pxstat[pmsl][sc]+=1;//suspicious LchSubCel
      }
      else{
        hi2lowr[pmsl][sc]=avs;
        hi2lowo[pmsl][sc]=avo;
        pxstat[pmsl][sc]+=9;//bad LchSubCel
      }
      cout<<"----> Hi2LowR Warning : pmsl/pix="<<pmsl<<" "<<sc<<" has slop/offs/chi2="<<slop[i]<<" "<<offs[i]<<" "<<chi2[i]<<endl;
    }
  }
  cout<<"<==== Hi2LowR done, good channels:"<<nonzer<<" from total 1296"<<endl<<endl;
//--------------------------------------------------------
//
// ----------------> calc. Anode(PixGcorrected sum4)/Dynode ratios for each PMT:
//
  number a2d;
  for(sl=0;sl<ECALDBc::slstruc(3);sl++){
    for(pm=0;pm<npmx;pm++){
      a2d=0;
      pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
      if(a2dnevs[pmsl]>0)a2d=an2dynr[pmsl]/a2dnevs[pmsl];
      if(a2dnevs[pmsl]>=5 && a2d>10 && a2d<50){
        if(ECCAFFKEY.hprintf>0)HF1(ECHISTC+48,geant(a2d),1.);
        an2dynr[pmsl]=a2d;
      }
      else{
        cout<<"     A2D:bad for sl/pm="<<sl<<" "<<pm<<" Nevs="<<a2dnevs[pmsl]<<" a2d="<<a2d<<endl;
        dystat[pmsl]=1;
        an2dynr[pmsl]=28.5;//default value
      }
    }
  }
//--------------------------------------------------------
//------------> write outp.files :
//  
  char fname[1024];
  char frdate[30];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  integer cfvn;
  uinteger StartRun,overid,verid;
  time_t StartTime;
  int dig;
//
//--> create RLGA output file(PM/SubCell rel.gains,Hi2Low-ratios,An2Dyn-ratios):
// 
  integer endfm(12345);
//
//--> get run/time of the first event
//
  StartRun=AMSUser::JobFirstRunN();
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  strcpy(fname,"EcalRlga");
  if(AMSJob::gethead()->isMCData()){
    strcat(fname,vers1);
    overid=ECcalib::CFlistC[1];
    verid=overid+1;//new MC-versn = old+1
    sprintf(fext,"%d",verid);
    ECcalib::CFlistC[1]=verid;//update RLGA-calib version in static store
  }
  else{
    strcat(fname,vers2);
    overid=ECcalib::CFlistC[1];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
    ECcalib::CFlistC[1]=verid;//update RLGA-calib version in static store
  }
  strcat(fname,".");
  strcat(fname,fext);
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<" <---- error opening RLGA-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<endl<<endl;
    cout<<"-----> Opening file for RLGA-calibration output, fname:"<<fname<<endl;
    cout<<"       First run used for calibration is "<<StartRun<<endl;
    cout<<"       Date of the first event : "<<frdate<<endl;
    cout<<"       Pixels status will be written !"<<endl;
    tcfile.setf(ios::fixed);
//    tcfile.width(3);
//    tcfile.precision(1);// precision for status
    tcfile << endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){//SL-loop
      for(sc=0;sc<4;sc++){//pix-statuses
        for(pm=0;pm<npmx;pm++){//PM-loop
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile.width(3);
          tcfile.precision(1);
          tcfile << pxstat[pmsl][sc];
        }
        tcfile << endl;
      }
      for(pm=0;pm<npmx;pm++){//PM-loop for Dyn-statuses
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile.width(3);
        tcfile.precision(1);
        tcfile << dystat[pmsl];
      }
      tcfile << endl;
      tcfile << endl;
    }
    tcfile << endl;
//
    cout<<"   <-- PM RelGains will be written "<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
	if(pmrgain[pmsl]<=0)pmrgain[pmsl]=1;//set missing chan. gain to default 1
        tcfile.width(5);
        tcfile.precision(2);
        tcfile << pmrgain[pmsl];
      }
      tcfile << endl;
    }
    tcfile << endl;
    tcfile << endl;
//---
    cout<<"   <-- Pixel RelGains will be written"<<endl;
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile.width(5);
          tcfile.precision(2);// precision for Pixel rel.gain
          tcfile << pxrgain[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//---
    cout<<"   <-- Pixel Hi/Low Gain ratios will be written"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile.width(6);
          tcfile.precision(2);
          tcfile << hi2lowr[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//---
    cout<<"   <-- Pixel Hi/Low Gain offsets will be written"<<'\n';
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(sc=0;sc<4;sc++){
        for(pm=0;pm<npmx;pm++){
          pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
          tcfile.width(6);
          tcfile.precision(2);
          tcfile << hi2lowo[pmsl][sc];
        }
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
//---
    cout<<"   <-- PM Anode/Dynode ratios will be written"<<'\n';
//    tcfile.width(4);
//    tcfile.precision(1);// precision for PM Anode/Dynode ratio
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        tcfile.width(6);
        tcfile.precision(2);
        tcfile << an2dynr[pmsl];
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<"      First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<"      Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"<----- ECREUNcalib: RLGA-calibration is done, output file closed !"<<endl<<endl;
//-------------------------------------------------------
//
//--> create FIAT output file(fiber att.parameters):
//
  if(ECREFFKEY.relogic[1]==2){//only if both RLGA and FIAT parts are requested 
    strcpy(fname,"EcalFiat");
    if(AMSJob::gethead()->isMCData()){
      strcat(fname,vers1);
      overid=ECcalib::CFlistC[2];
      verid=overid+1;//new MC-versn = old+1
      sprintf(fext,"%d",verid);
      ECcalib::CFlistC[2]=verid;//update FIAT-calib version in static store
    }
    else{
      strcat(fname,vers2);
      overid=ECcalib::CFlistC[2];
      if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
      else verid=StartRun;
      sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
      ECcalib::CFlistC[2]=verid;//update FIAT-calib version in static store
    }
    strcat(fname,".");
    strcat(fname,fext);
//
    ofstream fifile(fname,ios::out|ios::trunc);
    if(!fifile){
      cerr<<" <---- ECREUNcalib:error opening FIAT-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"-----> Opening file for FIAT-calibration output, fname:"<<fname<<endl;
    cout<<"       First run used for calibration is "<<StartRun<<endl;
    cout<<"       Date of the first event : "<<frdate<<endl;
    cout<<"       lfast/lslow/frac will be written !"<<endl;
    fifile.setf(ios::fixed);
    fifile.width(4);
    fifile.precision(1);// precision for slfast
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<slfast;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile.width(4);
    fifile.precision(0);// precision for slslow
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<slslow;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile.width(4);
    fifile.precision(2);// precision for fastfr
    for(sl=0;sl<ECALDBc::slstruc(3);sl++){
      for(pm=0;pm<npmx;pm++){
        pmsl=pm+ECPMSMX*sl;//sequential numbering of PM's over all superlayers
        fifile <<" "<<fastfr;
      }
      fifile << endl;
    }
    fifile << endl;
//
    fifile <<" "<<endfm;
    fifile << endl;
//
    fifile << endl<<"======================================================"<<endl;
    fifile << endl<<"      First run used for calibration is "<<StartRun<<endl;
    fifile << endl<<"      Date of the first event : "<<frdate<<endl;
    fifile.close();
    cout<<"<----- ECREUNcalib: FIAT-calibration is done, output file closed !"<<endl;
  }
//
}
//--------
void ECREUNcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
//
//
  for(i=0;i<nbins;i++){
    ff=(values[i]-x[0]*((1.-x[2])*exp(-coords[i]/x[1])
      +x[2]*exp(-coords[i]/x[3])))/errors[i];
    f+=(ff*ff);
  }
  if(flg==3){
    f=f/number(nbins-4);
    cout<<" <-- longit.fit done:"<<endl;
    cout<<"     chi2="<<f<<endl;
    for(i=0;i<4;i++){
      cout<<"     par-"<<i<<" ---> "<<x[i]<<endl;
    }
    if(f<5. && x[1]<1000){//chi2/slope ok
      cout<<"     Chi2 OK, SlopeSlow below limit - Fit OK"<<endl;
      nfits+=1;
      slslow+=x[1];
      fastfr+=x[2];
      slfast+=x[3];
    }
    else cout<<"     Chi2 Bad or SlopeSlow > limit - Bad Fit: chi2/SSl="<<f<<" "<<x[1]<<endl;
  }
}
//---
void ECREUNcalib::fill_1(integer sl, integer pm, integer sc, integer lb, number adc){
//NOT USED NOW !!! used for Aadc_hgain relat.gains calib 
  integer pmsl;
//
  pmsl=pm+ECPMSMX*sl;//PM continious numbering through all SL's
  sbcres[pmsl][sc]+=adc;
  tevsbf[pmsl][sc]+=1.;
//
}
//---
void ECREUNcalib::fill_2(integer sl, integer pm, integer sc, number adc[2]){
// used for A_hg/A_lg calib, called in ecalrec(val)
  integer i,slpmc;
  integer slpmcr,binw;
//
  slpmcr=4*(ECCAFFKEY.refpid%100-1)+4*ECPMSMX*(ECCAFFKEY.refpid/100-1);//ref.SC
  binw=integer(floor(number(ECCADCR)/ECCHBMX));
//
  slpmc=sc+pm*4+sl*4*ECPMSMX;//continious numbering of sl,pm,sc
  i=integer(floor(adc[0]/binw));// Hchannel bin (in Lch vs Hch dependence)
  if(i<ECCHBMX){//min.value of Lgain chan. is checked at call time
    tevhlc[slpmc][i]+=1.;
    sbchlc[slpmc][i]+=adc[1];
    sbchlc2[slpmc][i]+=(adc[1]*adc[1]);
    sbchlc3[slpmc][i]+=adc[0];//to have bin aberage for hi-chan
  }
  if(ECCAFFKEY.hprintf>1 && slpmc==slpmcr)HF2(ECHISTC+19,geant(adc[0]),geant(adc[1]),1.);
}
//---
void ECREUNcalib::fill_3(integer pmsl, number ad, number a4){
// used for A/D calib, called in ecalrec(val)
  number a2d;
  a2d=a4/ad;//instant a2/d
  if(ECCAFFKEY.hprintf>1)HF1(ECHISTC+5,geant(a2d),1.);//single pix signals(adc)
  if(a2d>10 && a2d<50){
    an2dynr[pmsl]+=a2d;
    a2dnevs[pmsl]+=1;
  }
}
//
//=============================================================================
void ECREUNcalib::selecte(){// <--- for ANOR calibration
  int i,j,k;
  integer sta,status,dbstat,padc[2],id,idd,isl,pmt,sbc,pmsl,rdir;
  integer npmx,nslmx,maxpl,maxcl,nraw,ovfl[2],proj,plane,cell,lbin;
  number radc[2],binw,trc[3],pmb[3],rrr,sbl,sbr,crl,crr;
  number ct,cl,dct,z,hflen,ctcr,ctpm,clcr,clsh,clpm,acorr;
  number sumh[ECPMSMX][4],suml[ECPMSMX][4],pmsum,pmdis; 
  geant pedh[4],pedl[4],sigh[4],sigl[4],h2lr,ph,pl,sh,sl,apmt;
  geant pmpit,pxsiz;
  AMSEcalRawEvent * ptr;
  AMSEcalHit * ptr1;
  AMSAntiCluster *ptra;
//
  AMSContainer *cptr;
  AMSParticle *ppart;
  AMSTrTrack *ptrack;
  AMSCharge  *pcharge;
  AMSBeta  *pbeta;
  AMSTrRecHit *phit;
  AMSTrCluster *pxcl;
  AMSTrCluster *pycl;
  AMSTrCluster *x;
  AMSTrCluster *y;
#ifndef _PGTRACK_
  AMSTrIdSoft idx;
  AMSTrIdSoft idy;
#endif
  integer ilay,hitla,nxcl[trconst::maxlay],nycl[trconst::maxlay];
  number axtcl[trconst::maxlay],aytcl[trconst::maxlay];
  number axbcl[trconst::maxlay],aybcl[trconst::maxlay];
  number amp;
//
  integer chargeTOF,chargeTracker,AdvFit,ntrh;
  AMSPoint C0,gC0,Cout1,Cout2,Cpmt,Cout[2];
  AMSDir dir(0,0,1.);
  int ntrk,ipatt;
  number chi2,the,phi,rid,err,trl,hchi2[2],hrid[2],herr[2],hthe[2],hphi[2],ass,beta;
  number gchi2,gthe,gphi,grid,gerr;
  int badtrlx,badtrly,badtrl,badebkg,badclam;
//
  npmx=ECALDBc::slstruc(4);//numb.of PM's/Sl
  nslmx=ECALDBc::slstruc(3);//numb.of Sl
  pxsiz=ECALDBc::rdcell(5);// pm(lg)-pixel size(imply =pm_size/2)
  pmpit=ECALDBc::rdcell(7);// pm transv.pitch
  maxpl=2*ECALDBc::slstruc(3);// SubCell planes
  maxcl=2*ECALDBc::slstruc(4);// SubCells per plane
//
  EcalJobStat::addca(0);
//
//----------------> check Anti :
//
  number eacut=0.6;// thresh. on E-anti-sector (mev)
  integer sector,nanti;
  number eacl,eanti;
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over AMSANTIRawCluster hits
    status=ptra->getstatus();
    if(status==0){ //select only good hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      if(ECCAFFKEY.hprintf!=0){
        HF1(ECHISTC+39,geant(eacl),1.);
      }
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+40,geant(nanti),1.);
  }
//
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
//
  EcalJobStat::addca(1);
//
// ---------------> get track info :
//
  bool trktr,trdtr,ecaltr,nottr,badint;
  integer trpatt;
//
  ntrk=0;
  sta=1;
  AdvFit=0;
  chi2=-1.;
  badtrlx=0;
  badtrly=0;
  badtrl=0;
  badebkg=0;
  badclam=0;
  for(i=0;i<trconst::maxlay;i++){
    nxcl[i]=0;
    nycl[i]=0;
    axtcl[i]=0.;//track cluster amp.
    aytcl[i]=0.;
    axbcl[i]=0.;//backgr.clusters amp.
    aybcl[i]=0.;
  } 
  cptr=AMSEvent::gethead()->getC("AMSParticle",0);//p2 part.envelop with true track particle
  if(cptr)
          ntrk+=cptr->getnelem();
  if(ntrk!=1)return;// require events with 1 track-part
  ppart=(AMSParticle*)AMSEvent::gethead()->
                                           getheadC("AMSParticle",0);
  if(ppart){
    ptrack=ppart->getptrack();
    if(ptrack){//check track-type
      trdtr=(ptrack->checkstatus(AMSDBc::TRDTRACK)!=0);
      ecaltr=(ptrack->checkstatus(AMSDBc::ECALTRACK)!=0);
      nottr=(ptrack->checkstatus(AMSDBc::NOTRACK)!=0);
      badint=(ptrack->checkstatus(AMSDBc::BADINTERPOL)!=0);
      if(!(nottr || ecaltr || badint || trdtr)){//use only TRK-track particle
        trpatt=ptrack->getpattern();//TRK-track pattern
#ifdef _PGTRACK_
	if(!(ptrack->IsFake()))
#else
	  if(trpatt>=0)
#endif
	  {//trk-track ok
          ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
          AdvFit=ptrack->AdvancedFitDone();
#ifdef _PGTRACK_
          if(
	     (ptrack->GetPar(AMSTrTrack::kChoutko|AMSTrTrack::kUpperHalf).FitDone)&&
	     (ptrack->GetPar(AMSTrTrack::kChoutko|AMSTrTrack::kLowerHalf).FitDone)
	     )
	    {
	      hrid[0]=ptrack->GetRigidity(AMSTrTrack::kChoutko|AMSTrTrack::kUpperHalf);
	      hrid[1]=ptrack->GetRigidity(AMSTrTrack::kChoutko|AMSTrTrack::kLowerHalf);
	    }
#else
          if(AdvFit)ptrack->
             getParAdvancedFit(gchi2,grid,gerr,gthe,gphi,gC0,hchi2,hrid,herr,hthe,hphi,Cout);
#endif

          pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
          chargeTracker=pcharge->getchargeTracker();
          pbeta=pcharge->getpbeta();
          beta=0.;
          if(pbeta)beta=pbeta->getbeta();
//
#ifdef _PGTRACK_
          ntrh=ptrack->GetNhits();
          for(i=0;i<ntrh;i++){//<---track hits(2Dclust) loop
            phit=dynamic_cast<AMSTrRecHit*>(ptrack->GetHit(i));
            hitla=phit->GetLayer();
//
            pxcl=dynamic_cast<AMSTrCluster*>(phit->GetXCluster());
            if(pxcl){
              axtcl[hitla-1]+=pxcl->GetTotSignal();
              x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
              while (x){//<--- x-clust loop
                ilay=x->GetLayer();
                if(hitla==ilay){//the same layer
                  if(x->checkstatus(AMSDBc::BAD)==0){
                    if(pxcl==x){
                    }
                    else{//count background
                      axbcl[ilay-1]+=x->GetTotSignal();
                      nxcl[ilay-1]+=1;
                    }
                  }
                }
                x=x->next();
              }//---> end of x-clust loop
            }//-->pxcl  
//
            pycl=dynamic_cast<AMSTrCluster*>(phit->GetYCluster());
            if(pycl){
              aytcl[hitla-1]+=pycl->GetTotSignal();
              y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0);
              while (y){//<--- y-clust loop
                ilay=y->GetLayer();
                if(hitla==ilay){//the same layer
                  if(y->checkstatus(AMSDBc::BAD)==0){
                    if(pycl==y){
                    }
                    else{//count background
                      aybcl[ilay-1]+=y->GetTotSignal();
                      nycl[ilay-1]+=1;
                    }
                  }
                }
                y=y->next();

#else
          ntrh=ptrack->getnhits();
          for(i=0;i<ntrh;i++){//<---track hits(2Dclust) loop
            phit=ptrack->getphit(i);
            hitla=phit->getLayer();
//
            pxcl=phit->getClusterP(0);
            if(pxcl){
              axtcl[hitla-1]+=pxcl->getVal();
              x=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",0);
              while (x){//<--- x-clust loop
                idx=x->getid();
                ilay=idx.getlayer();
	        if(hitla==ilay){//the same layer
	          if(x->checkstatus(AMSDBc::BAD)==0){
	            if(pxcl==x){
	            }
                    else{//count background
	              axbcl[ilay-1]+=x->getVal();
	              nxcl[ilay-1]+=1;
	            }
	          }
	        }
                x=x->next();
              }//---> end of x-clust loop
            }//-->pxcl  
//
            pycl=phit->getClusterP(1);
            if(pycl){
              aytcl[hitla-1]+=pycl->getVal();
              y=(AMSTrCluster*)AMSEvent::gethead()->getheadC("AMSTrCluster",1,0);
              while (y){//<--- y-clust loop
                idy=y->getid();
                ilay=idy.getlayer();
	        if(hitla==ilay){//the same layer
	          if(y->checkstatus(AMSDBc::BAD)==0){
	            if(pycl==y){
	            }
                    else{//count background
	              aybcl[ilay-1]+=y->getVal();
	              nycl[ilay-1]+=1;
	            }
                  }
                }
                y=y->next();
#endif
              }//---> end of y-clust loop
            }//-->pycl  
//
          }//---> end of track hits loop
        }//--->endof "Trk-track ok" check
      }//--->endof "Trk-track ?" check
    }//---> endof if(ptrack)
  }//---> end if(ppart)
//-----------------------
  if(chi2<0.)return;//--->track not found
  EcalJobStat::addca(2);
//
//---> look at track clusters:
//
  if(ECCAFFKEY.hprintf!=0){
#ifdef _PGTRACK_
    for(i=0;i<TkDBc::Head->nlay();i++)
#else
    for(i=0;i<TKDBc::nlay();i++)
#endif
      {
      HF1(ECHISTC+30,geant(nycl[i]),1.);
      HF1(ECHISTC+31,geant(nxcl[i]),1.);
    }
  }
//
#ifdef _PGTRACK_
    for(i=0;i<TkDBc::Head->nlay();i++)
#else
    for(i=0;i<TKDBc::nlay();i++)
#endif
      {
    rrr=0.;
    if(axtcl[i]>0.)rrr=axbcl[i]/axtcl[i];
    if(rrr>0.4)badebkg=1;
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+32,geant(rrr),1.);
      if(axtcl[i]>0.)HF1(ECHISTC+34,geant(axtcl[i]),1.);
    }
//    if(nxcl[i]>4 || rrr>1. || axtcl[i]>100.)badtrlx+=1;
    if(axtcl[i]>200.)badclam=1;
    rrr=0.;
    if(aytcl[i]>0.)rrr=aybcl[i]/aytcl[i];
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+33,geant(rrr),1.);
      if(aytcl[i]>0.)HF1(ECHISTC+35,geant(aytcl[i]),1.);
      HF1(ECHISTC+42,geant(axtcl[i]+aytcl[i]),1.);
    }
    if(rrr>0.6)badebkg=1;
//    if(nycl[i]>4 || rrr>1. || aytcl[i]>100.)badtrly+=1;
    if(aytcl[i]>200.)badclam=1;
    if((axtcl[i]+aytcl[i])>150.)badtrl+=1;
  }
//
// ---------------> check particle(electron) quality :
//
  ass=0.;
  if(AdvFit){
    if(fabs(hrid[0]+hrid[1])>0.)ass=(hrid[0]-hrid[1])/(hrid[0]+hrid[1]);
  }
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+1,geant(chi2),1.);
    HF1(ECHISTC+2,geant(fabs(rid)),1.);// R
    HF1(ECHISTC+3,geant(fabs(rid*err)),1.);//dR/R
    HF1(ECHISTC+4,geant(ass),1.);//half-rig. ass.
    HF1(ECHISTC+44,geant(beta),1.);//beta
    if(badclam)HF1(ECHISTC+36,geant(fabs(rid)),1.);
    if(badebkg)HF1(ECHISTC+43,geant(fabs(rid)),1.);
  }
  if(fabs(rid)<ECCAFFKEY.pmin || fabs(rid)>ECCAFFKEY.pmax)return;//--->out of needed range
//
  if(!(beta>0. && rid<0.))return;//---> it is not electron(charge<0) from AMS_top->bot
//
  if(chi2>20.)return;//---> bad chi2
//
  if(fabs(beta-1.)>0.1)return;//---> bad beta (too low to be electron)
// 
  if(fabs(ass)>0.06)return;//---> bad half-rigidities assimetry
//
//  if(fabs(rid*err)>0.014)return;// do not affect
//  if(badclam>0)return;//do not affect on low-Rig tail(at least for MC)
//  if(badebkg>0)return;
//
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+41,geant(fabs(rid)),1.);
  }
//
  EcalJobStat::addca(3);
//
// ---------------> check that track  hits ECAL:
//
  number dx,dy,sfg;
  int icr(0);
//
  C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
  C0[0]=0.;
  C0[1]=0.;
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC,geant(cos(the)),1.);
    HF1(ECHISTC+37,geant(Cout1[0]),1.);
    HF1(ECHISTC+38,geant(Cout1[1]),1.);
  }
  if(fabs(cos(the))<0.94)return;// ----> check the impact angle(should be < 20degr)
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  sfg=-1.;
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
//
  C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
  ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
  dx=Cout1[0]-ECALDBc::gendim(5);
  dy=Cout1[1]-ECALDBc::gendim(6);
  sfg=-4.;
  if(fabs(dx)<(ECALDBc::gendim(1)/2.+sfg) && fabs(dy)<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
  if(icr<=1)return;// ----> no good crossing with ECAL 
//
  EcalJobStat::addca(4);
//
//-------------------------> select True-electron events :
// (EcalHits  ALREADY gain-corrected using RLGA/(FIAT) part(s) of REUN-calibration)
// (Absolute normalization(mev) is STILL from previous (OLD) calibration !!!)
//
  number edthr,edep,edept,esleak,ebleak,edlpr[2*ECSLMX],edtpr[2*ECSLMX][2*ECPMSMX];
  integer nsclpr[2*ECSLMX];
  integer refsl,refpm,ipl,imax,imin,plholes[2*ECSLMX];
  int nnn,bad1,bad2,bad3,spikes,badscl(0);
  int badhl[6]={0,0,0,0,0,0};
  geant ad2mv;
  number scadcmx;
//
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
  edept=0.;
  esleak=0.;
  scadcmx=0.;
  bad3=0;
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-Planes loop
    ptr1=(AMSEcalHit*)AMSEvent::gethead()->
                               getheadC("AMSEcalHit",ipl,0);
    for(i=0;i<maxcl;i++){
      edtpr[ipl][i]=0.;
    }
    edlpr[ipl]=0.;
    nsclpr[ipl]=0;
    while(ptr1){ // <--- EcalHits(fired subcells) loop in plane:
      edep=ptr1->getedep();//(mev)
      proj=ptr1->getproj();//0/1->X/Y (have to be corresponding to ipl)
      cell=ptr1->getcell();// 0,...
      isl=ipl/2;
      pmt=cell/2;
      if(ECCAFFKEY.hprintf!=0){
        HF1(ECHISTC+5,geant(edep/ad2mv),1.);
      }
      if(edep>=(ad2mv*ECCAFFKEY.scmin)){// because scmin-threshold is in ADC-units
        edlpr[ipl]+=edep;
	edtpr[ipl][cell]+=edep;
	edept+=edep;
	if(pmt==0 || pmt==(npmx-1))esleak+=edep;
      }
      ptr1=ptr1->next();  
    } // ---> end of EcalHits loop in Plane ipl
//
    plholes[ipl]=0;
    spikes=0;
    bad1=0;
    bad2=0;
    imin=-1;
    imax=0;
    for(i=0;i<maxcl;i++){// <--- patt. of fired subcells in plane
      if(edtpr[ipl][i]>ECCAFFKEY.scmax*ad2mv)spikes+=1;//count spikes(sparks,ovfl,...)
      if(edtpr[ipl][i]/ad2mv>scadcmx)scadcmx=edtpr[ipl][i]/ad2mv;
      if(edtpr[ipl][i]>0.){
        nsclpr[ipl]+=1;//count fired SubCell's per plane
	imax=i;//to find most right fired sc
	if(imin<0)imin=i;//to find most left fired sc
      }
    }//---> end of SubCell loop in plane
    k=imax-imin;
    if(nsclpr[ipl]>0)plholes[ipl]=k+1-nsclpr[ipl];
//
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+6,geant(nsclpr[ipl]),1.);
      nnn=nsclpr[ipl];
      if(nnn>19)nnn=19;
      if(ipl<=5)HF1(ECHISTC+7,geant(nnn+ipl*20),1.);
      nnn=plholes[ipl];
      if(nnn>19)nnn=19;
      if(ipl==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==1 && badhl[0]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==2 && badhl[0]==0 && badhl[1]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==3 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                                              && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==4 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                               && badhl[3]==0 && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      if(ipl==5 && badhl[0]==0 && badhl[1]==0 && badhl[2]==0
                               && badhl[3]==0 && badhl[4]==0
			                      && nsclpr[ipl]>0)HF1(ECHISTC+20,geant(nnn+ipl*20),1.);
      HF1(ECHISTC+24,geant(spikes),1.);
    }
    if(nsclpr[ipl]>0){//non empty plane
      if(spikes>ECCAFFKEY.spikmx)bad1=1;//mark plane with "spike(s)" 
      if(nsclpr[ipl]>ECCAFFKEY.lmulmx)bad2=1;//abnorm.multiplicity(any layer)
      if(ipl<=5 && (nsclpr[ipl]>ECCAFFKEY.nhtlmx[ipl] 
                           || plholes[ipl]>ECCAFFKEY.nholmx[ipl]))badhl[ipl]=1;//mark early showering
    }
    if(bad1>0 || bad2>0)badscl+=1;//mark bad Plane(spikes or too high multiplicity)
//
  }//---> end of Planes-loop
//
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+8,geant(badscl),1.);
    HF1(ECHISTC+19,geant(scadcmx),1.);
  }
//
  if(badscl>ECCAFFKEY.nbplmx)return;//too many planes with spikes,...
//
  nnn=badhl[0]+badhl[1]+badhl[4]+badhl[5];//only pl 1-2,5-6 are really working...
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+25,geant(nnn),1.);
  }
//
  if(ECREFFKEY.relogic[2]==0 || ECREFFKEY.relogic[2]==2){//use "holes" cut if requested
    if(nnn>0)return;//early shower("nnn>1" works slightly worse but with 1.5 times higher eff)
  }
  EcalJobStat::addca(5);
//
// ---->electron identification(soft):
//
  geant e4x0,epeaka,etaila,rr,rematch;
  e4x0=edlpr[0]+edlpr[1]+edlpr[2]+edlpr[3];//energy(mev) in 1st 4X0's(1-4pl) of Z-profile
  epeaka=(edlpr[4]+edlpr[5]+edlpr[6])/3.;//aver.energy/plane in peak(5-7pl) of Z-profile
  etaila=(edlpr[maxpl-2]+edlpr[maxpl-1])/2.;//aver.energy/plane in tail(17-18pl) ...
  rr=0.;
  if(epeaka>0.)rr=etaila/epeaka;
  if(rr>0.99)rr=0.99;
//
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+9,geant(edept),1.);
  }
  int ecflg(0);
  if(edept>ECCAFFKEY.edtmin && (0.001*edept)<(1.5*ECCAFFKEY.pmax)){// not MIP, Et not exeed max. mom
    esleak/=edept;//Esleak fraction
    ebleak=edlpr[maxpl-1]/edept;
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+10,geant(esleak),1.);
      HF1(ECHISTC+17,geant(ebleak),1.);
    }
    if(esleak<ECCAFFKEY.esleakmx && ebleak<ECCAFFKEY.ebleakmx){
      if(ECCAFFKEY.hprintf!=0){
        HF1(ECHISTC+11,e4x0,1.);
      }
      if(e4x0>ECCAFFKEY.edfrmn){
        if(ECCAFFKEY.hprintf!=0){
	  HF1(ECHISTC+12,rr,1.);
	}
	if(rr<ECCAFFKEY.edt2pmx){
	  rematch=0.001*edept/fabs(rid)-1.;
	  if(ECCAFFKEY.hprintf!=0){
	    HF1(ECHISTC+13,rematch,1.);
	  }
	  if(fabs(rematch)<ECCAFFKEY.ed2momc)ecflg=1;
	}
      }
    }
  }
  if(ecflg==0)return;//---> Not electron 
  EcalJobStat::addca(6);
//
//---------> check track-ECHits matching in SC-planes:
//
  geant lfs,lsl,ffr,dctmx;
  geant mismcut[2*ECSLMX];
  number edeptnc,attf,cog,edpl;
  number emism[2*ECSLMX],ematch[2*ECSLMX];
  int cmism,nlmism,cogmism;
  int mism[6]={0,0,0,0,0,0};
//
  mismcut[0]=ECCAFFKEY.scdismx[0];
  mismcut[1]=ECCAFFKEY.scdismx[1];
  mismcut[2]=ECCAFFKEY.scdismx[2];
  mismcut[3]=ECCAFFKEY.scdismx[3];
  mismcut[4]=ECCAFFKEY.scdismx[4];
  mismcut[5]=ECCAFFKEY.scdismx[5];
  for(i=6;i<2*ECSLMX;i++)mismcut[i]=ECCAFFKEY.scdisrs;
//
  edept=0.;
  edeptnc=0.;
  cogmism=0;
  nlmism=0;//layers with early showering(mism.cells)
  sta=0;//good
  for(ipl=0;ipl<maxpl;ipl++){ // <-------------- SubCell-Planes loop
    emism[ipl]=0.;
    ematch[ipl]=0.;
//
//  ---> extrapolate track to plane :
    isl=ipl/2;
    if(ipl%2==0)ECALDBc::getscinfoa(isl,0,0,proj,plane,cell,ct,cl,z);//1stPM,1stPixel
    else ECALDBc::getscinfoa(isl,0,2,proj,plane,cell,ct,cl,z);//1stPM,3rdPixel
    C0[2]=z;
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<---cross with SC
    if(proj==0){// <--- x-proj (0)
      ctcr=Cout1[0];
      clcr=Cout1[1];
      hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir
      clsh=ECALDBc::gendim(6);//EC-pos in y
      acorr=1./sqrt(1.+pow(sin(the)*sin(phi)/cos(the),2));
    }
    else{//        <--- y-proj (1)
      ctcr=Cout1[1];
      clcr=Cout1[0];
      hflen=ECALDBc::gendim(1)/2.;// 0.5*fiber(EC) length in X-dir
      clsh=ECALDBc::gendim(5);//EC-pos in x
      acorr=1./sqrt(1.+pow(sin(the)*cos(phi)/cos(the),2));
    }
    if(fabs(clcr-clsh)>hflen)continue;//track out of plane ===> try next plane
    clpm=cl;// middle of fiber in AMS r.s.!
//
    edpl=0.;
    cog=0.;
    dctmx=-1.;
    cmism=0;//count cells out of track(from early shower)
//
    for(cell=0;cell<maxcl;cell++){ // <-------------- SubCell loop in plane
      pmt=cell/2;
      ad2mv=ECcalib::ecpmcal[isl][pmt].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
      edep=edtpr[ipl][cell];//mev
      if(edep>0.){
        if(proj==0){// <--- x-proj (0)
          rdir=(1-2*(pmt%2))*ECALDBc::slstruc(5);//readout dir(+-1->along pos/neg Y)
        }
        else{//        <--- y-proj (1)
          rdir=(1-2*(pmt%2))*ECALDBc::slstruc(6);//readout dir(+-1->along pos/neg X)
        }
        if(rdir>0){
          clpm=clpm-hflen;//pm-pos if pm is at -x(-y) (i.e. rdir=1)
          pmdis=clcr-clpm;//0-2*hflen
        }
        else{
          clpm=clpm+hflen;//pm-pos if pm is at +x(+y) (i.e. rdir=-1)
          pmdis=clpm-clcr;//0-2*hflen
        }
	lfs=ECcalib::ecpmcal[isl][pmt].alfast();//att_len(fast comp) from DB
	lsl=ECcalib::ecpmcal[isl][pmt].alslow();//att_len(slow comp) from DB
	ffr=ECcalib::ecpmcal[isl][pmt].fastfr();//fast comp. fraction from DB
        attf=(1.-ffr)*exp(-pmdis/lsl)+ffr*exp(-pmdis/lfs);//fiber attenuation factor
	if(ipl%2==0){//sbc=0,1
	  sbc=cell%2;
	}
	else{//sbc==2,3
	  sbc=(cell%2)+2;
	}
	ECALDBc::getscinfoa(isl,pmt,sbc,proj,plane,i,ct,cl,z);//get cell transv.coo
	dct=ct-ctcr;
	if(ECCAFFKEY.hprintf!=0){
	  if(ipl==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==1 && mism[0]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==2 && mism[0]==0 && mism[1]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==3 && mism[0]==0 && mism[1]==0 && mism[2]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==4 && mism[0]==0 && mism[1]==0 && mism[2]==0
	                                        && mism[3]==0)HF1(ECHISTC+50+ipl,dct,1.);
	  if(ipl==5 && mism[0]==0 && mism[1]==0 && mism[2]==0
	                          && mism[3]==0 && mism[4]==0)HF1(ECHISTC+50+ipl,dct,1.);
	}
//
	if(fabs(dct)>mismcut[ipl]){//count mismatched(backgr) cells\energy
	  cmism+=1;
	  emism[ipl]+=edep;
	}
	else ematch[ipl]+=edep;//count matched energy
//
	if(fabs(dct)>dctmx)dctmx=fabs(dct);
	if(fabs(dct)<18.){// tempor simulate clustering algorithm(replace later) 
	  edeptnc+=edep;
//	  if(fabs(dct)<1.8)edep/=attf;//"nearby only" fib.att. correction(don't help !)
	  edpl+=edep;
	  cog+=edep*ct;
	  edept+=edep;
	}
      }//---> endif of Edep>0
    }//----------> end of SC-loop
//
    if(ipl<=5 && cmism>0)mism[ipl]=1;//count planes(among 1st 6) with early showering(mism.cells)
//
    if(ECCAFFKEY.hprintf!=0 && dctmx>0.){
      HF1(ECHISTC+14,dctmx,1.);
      if(ipl==0)HF1(ECHISTC+21,dctmx,1.);
    }
    if(edpl>0.){
      cog/=edpl;// plane CenterOfGravity(in transv.dir)
      dct=cog-ctcr;
      if(ECCAFFKEY.hprintf!=0){
        if(ipl>3 && ipl<=5)HF1(ECHISTC+15,geant(dct),1.);
        if(ipl<=1)HF1(ECHISTC+22,geant(dct),1.);
        if(ipl>1 && ipl<=3)HF1(ECHISTC+23,geant(dct),1.);
	HF1(ECHISTC+29,geant(ipl),geant(edpl/1000.));
      }
      if(ipl<=5 && fabs(dct)>ECCAFFKEY.cog1cut)cogmism+=1;
    }
//
  }//--------> end of SC-planes loop
//------------------------------
//
  bad3=0;
  for(i=0;i<6;i++){
    rrr=0.;
    if(ematch[i]>0.)rrr=emism[i]/ematch[i];
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+56+i,geant(rrr),1.);
    }
    if(rrr>ECCAFFKEY.b2scut[i])bad3+=1;
  }
//
//
  if(cogmism>0)sta=1;//---> too high COG-mismatching in plane 1,2 or 3
  if(sta>0)return;
  EcalJobStat::addca(7);
//
  if(ECREFFKEY.relogic[2]==1 || ECREFFKEY.relogic[2]==2){
    if(bad3>0)sta=1;//---> Ebkg/Esignal too high in one of the 1st 6 planes
  }
  if(sta>0)return;
  EcalJobStat::addca(8);
//
//g.chen
/*
  if(ECCAFFKEY.ecshswit==1){ // add shower info here
    AMSEcalShower * ptsh;
    number ecshen,ecshener,efront,chi2dir,difosum,ecshsleak,ecshrleak,ecshdleak,ecsholeak;
    number ecsherdir,profchi2,transchi2;
    AMSDir ecshdir(0.,0.,0.);
    AMSPoint ecshentry,ecshexit,ecshcog;
    int  ecshnum;
    ecshnum=0;
    ecshen=0.;
    ecshener=0.;
    efront=0.;
    ptsh=(AMSEcalShower*)AMSEvent::gethead()->
      getheadC("EcalShower",0);
    while(ptsh){ // <------- ecal shower
      ecshnum++;
      ptsh=ptsh->next();
    }
    if(ecshnum!=1)return;          // only use event with one shower  
    
    ptsh=(AMSEcalShower*)AMSEvent::gethead()->
      getheadC("EcalShower",0);
    //  while(ptsh){ // <------- ecal shower
    ecshen=ptsh->getEnergy();
    //    ecshener=ptsh->getEnergyErr();
    ecshdir=ptsh->getDir();
    ecshentry=ptsh->getEntryPoint();
    ecshexit=ptsh->getExitPoint();
    ecshcog=ptsh->getCofG();
    efront=ptsh->getEnFront();
    chi2dir=ptsh->getDirChi2();
    ecsherdir=ptsh->getErDir();
    difosum=ptsh->getDifoSum();
    ecshsleak=ptsh->getSLeak();
    ecshrleak=ptsh->getRLeak();
    ecshdleak=ptsh->getDLeak();
    ecsholeak=ptsh->getOLeak();
    profchi2=ptsh->getProfChi2();
    transchi2=ptsh->getTransChi2();
    
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+62,ecshen,1.);
      HF1(ECHISTC+63,chi2dir,1.);
      HF1(ECHISTC+64,efront,1.);
      HF1(ECHISTC+65,difosum,1.);
      HF1(ECHISTC+66,ecshsleak,1.);
      HF1(ECHISTC+67,ecshrleak,1.);
      HF1(ECHISTC+68,ecshdleak,1.);
      HF1(ECHISTC+69,ecsholeak,1.);
      HF1(ECHISTC+70,profchi2,1.);
      HF1(ECHISTC+71,transchi2,1.);
      HF1(ECHISTC+79,ecshen/fabs(rid),1.);
      HF1(ECHISTC+102,ecshener,1.);
      HF1(ECHISTC+103,geant(1000.*fabs(rid)),1.);
    }
    if(chi2dir>ECCAFFKEY.chi2dirmx) return;
    if(profchi2>ECCAFFKEY.prchi2mx) return;   //profile fit (long.)
    if(transchi2>ECCAFFKEY.trchi2mx) return; //trans. fit
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+104,ecshen,1.);
      HF1(ECHISTC+105,geant(1000.*fabs(rid)),1.);
    }
    icr=0;
    sfg=-1.;
    if(fabs(ecshentry[0])<(ECALDBc::gendim(1)/2.+sfg) && fabs(ecshentry[1])<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
    sfg=-4.;
    if(fabs(ecshexit[0])<(ECALDBc::gendim(1)/2.+sfg) && fabs(ecshexit[1])<(ECALDBc::gendim(2)/2.+sfg))icr+=1;
    if(icr<=1) return; //shower entry and exit should cross with ECAL
    //    if(ecshener>ECCAFFKEY.eshermax) return;
    if(ecshsleak>ECCAFFKEY.eshsleakmx) return;  // side leakage
    if(ecshrleak>ECCAFFKEY.eshrleakmx) return;  // rear leakage
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+106,ecshen,1.);
      HF1(ECHISTC+107,geant(1000.*fabs(rid)),1.);
    }
    if(ecshdleak>ECCAFFKEY.eshdleakmx) return;  // dead leakage
    if(ecsholeak>ECCAFFKEY.esholeakmx) return;   // orphan hits
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+108,ecshen,1.);
      HF1(ECHISTC+109,geant(1000.*fabs(rid)),1.);
    }
    if(difosum>ECCAFFKEY.difsummx) return; // (E_x-E_y)/(E_x+E_y)
    //
    EcalJobStat::addca(9);
    //
    // ---->track match with shower:
    //
    number dxent, dyent, dxext, dyext, rematch;
    C0[2]=ECALDBc::gendim(7);// Z-top of ECAL
    C0[0]=0.;
    C0[1]=0.;
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Ztop of EC
    dxent=Cout1[0]-ecshentry[0];
    dyent=Cout1[1]-ecshentry[1];
    C0[2]=ECALDBc::gendim(7)-nslmx*(ECALDBc::gendim(9)+2.*ECALDBc::gendim(10));// Z-bot of ECAL
    ptrack->interpolate(C0,dir,Cout1,the,phi,trl);//<--- cross. with Zbot of EC
    dxext=Cout1[0]-ecshexit[0];
    dyext=Cout1[1]-ecshexit[1];
    rematch=ecshen/fabs(rid)-1.;
    
    if(ECCAFFKEY.hprintf!=0){
      HF1(ECHISTC+72,fabs(rid)/ecshen,1.);
      HF1(ECHISTC+73,rematch,1.);
      HF1(ECHISTC+74,dxent,1.);
      HF1(ECHISTC+75,dyent,1.);
      HF1(ECHISTC+76,dxext,1.);
      HF1(ECHISTC+77,dyext,1.);
      HF1(ECHISTC+110,ecshen,1.);
      HF1(ECHISTC+111,geant(1000.*fabs(rid)),1.);
    }
    if(fabs(rematch)>ECCAFFKEY.ed2momc) return;
    if(fabs(dxent)>ECCAFFKEY.trentmax[0]) return;
    if(fabs(dyent)>ECCAFFKEY.trentmax[1]) return;
    if(fabs(dxext)>ECCAFFKEY.trextmax[0]) return;
    if(fabs(dyext)>ECCAFFKEY.trextmax[1]) return;
    EcalJobStat::addca(10);
    HF1(ECHISTC+112,ecshen,1.);
    //
  }//---> end of "g.chen" check
*/
//
  if(ECCAFFKEY.hprintf!=0){
    HF1(ECHISTC+16,geant(0.001*edept/fabs(rid)),1.);
    HF1(ECHISTC+18,geant(1000.*fabs(rid)/edept),1.);
    HF1(ECHISTC+26,geant(edept),1.);
    HF1(ECHISTC+27,geant(edeptnc),1.);
    HF1(ECHISTC+28,geant(1000.*fabs(rid)),1.);
  }
}
//----------------------------
void ECREUNcalib::mfite(){
//
  int i,j,k,bad;
  integer sl,pm,sc,pmsl,lb,npmx,slpmc;
  integer pmslr,refsl,refpm;
  geant ad2mv,anorf;
  number rrr,sum4,eff,hflen,binw;
  char htit1[60];
  char htit2[60];
  char inum[11];
  char in[2]="0";
  char chopt[6]="qb";
  char chfun[6]="g";
  char chopt1[5]="LOGY";
  geant par[3],step[3],pmin[3],pmax[3],sigp[3],chi2;
//
  strcpy(inum,"0123456789");
  npmx=ECALDBc::slstruc(4);//numb.of PM's/sl
  refsl=ECCAFFKEY.refpid/100;// <=====Ref.PM(counting from 1->9 and 1->36)
  refpm=ECCAFFKEY.refpid%100;
  pmslr=refpm-1+ECPMSMX*(refsl-1);//sequential numbering for ref. PM
  hflen=ECALDBc::gendim(2)/2.;// 0.5*fiber(EC) length in Y-dir(=X)
  ad2mv=ECcalib::ecpmcal[refsl-1][refpm-1].adc2mev();// ADCch->Emeasured(MeV) factor(OLD!)
//
// ---> fit histogr. Epart/Eecal with gaussian:
//
  par[0]=200.;//amplitude
  par[1]=1.;//most prob.
  par[2]=0.1;//sigma
  step[0]=50.;
  step[1]=0.1;
  step[2]=0.05;
  pmin[0]=10.;
  pmin[1]=0.2;
  pmin[2]=0.01;
  pmax[0]=10000.;
  pmax[1]=1.8;
  pmax[2]=0.9;
  HFITHN(ECHISTC+18,chfun,chopt,3,par,step,pmin,pmax,sigp,chi2);
  cout<<endl<<endl;
  cout<<"      ECCALIB-ANOR-fit: Mp/resol="<<par[1]<<" "<<par[2]<<" chi2="<<chi2<<endl;
  HPRINT(ECHISTC+18);
  anorf=ad2mv*par[1];// New (1) ad2mv factor
  cout<<"      ECCALIB-ANOR: old/new ADC->Mev factor = "<<ad2mv<<" / "<<anorf<<endl;
//
//------------> write outp.files :
//  
  char fname[1024];
  char frdate[30];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
  integer endfm(12345);
//
//--> create ANOR output file(absolute normalization factor(the same for all PM's):
// 
//
//--> get run/time of the first event
//
  StartRun=AMSUser::JobFirstRunN();
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  strcpy(fname,"EcalAnor");
  if(AMSJob::gethead()->isMCData()){
    strcat(fname,vers1);
    sprintf(fext,"%d",(ECMCFFKEY.calvern+1));//MC-versn
  }
  else{
    strcat(fname,vers2);
    sprintf(fext,"%d",StartRun);//hope it is start-run UTC
  }
  strcat(fname,".");
  strcat(fname,fext);
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"<---- ECREUNcalib:error opening ANOR-file for output"<<fname<<endl;
      exit(8);
    }
    cout<<"      Opening file for ANOR-calibration output, fname:"<<fname<<endl;
    cout<<"      First run used for calibration is "<<StartRun<<endl;
    cout<<"      Date of the first event : "<<frdate<<endl;
    cout<<"      Abs.normaliz.factors will be written !"<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(5);
    tcfile.precision(3);// precision
    tcfile << endl;
    tcfile <<" "<<anorf;//hope it is common for all cells
    tcfile << endl;
    tcfile << endl;
//
    tcfile <<" "<<endfm<<endl;
    tcfile << endl;
//
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<"      First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<"      Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"<---- ECREUNcalib: ANOR-calibration is done, output file closed !"<<endl;
//
}
//------------------------------------------------------------------------------------
//
//=======================> "On-Data" + "OnBoardPedTable" Pedestals/Sigmas Calibration
//
 time_t ECPedCalib::BeginTime;
 uinteger ECPedCalib::BeginRun;
 number ECPedCalib::adc[ECPMSL][5][2];
 number ECPedCalib::adc2[ECPMSL][5][2];//**2
 number ECPedCalib::adcm[ECPMSL][5][2][ECPCSTMX];//stack of ECPCSS max's (Highest is 1st)
 number ECPedCalib::port2r[ECPMSL][5][2];//portion of hi-ampl to remove
 integer ECPedCalib::nevt[ECPMSL][5][2];
 integer ECPedCalib::hiamap[ecalconst::ECSLMX][ecalconst::ECPMSMX];//high signal PMTs map (1 event) 
 geant ECPedCalib::peds[ECPMSL][5][2];
 geant ECPedCalib::dpeds[ECPMSL][5][2];
 geant ECPedCalib::thrs[ECPMSL][5][2];
 geant ECPedCalib::sigs[ECPMSL][5][2];
 uinteger ECPedCalib::stas[ECPMSL][5][2];
// geant ECPedCalib::rpeds[ECPMSL][5][2];
// geant ECPedCalib::rsigs[ECPMSL][5][2];
// uinteger ECPedCalib::rstas[ECPMSL][5][2];
 integer ECPedCalib::nstacksz;//needed stack size (ev2rem*ECPCEVMX)
 ECPedCalib::ECPedCalib_ntpl  ECPedCalib::ECPedCalNT;
//
   
//-----
 void ECPedCalib::initb(){//called in reecalinitjob() in EC is requested for OnBoard-calib data proc
// histograms booking / reset vars
   int16u i,sl,pm,pix,gn,gnm;
   int hnm,ch;
   char buf[6];
   char htit[80];
   static int first(0);
   char hmod[2]=" ";
//
//
 if((CALIB.SubDetInCalib%10)>1){//histograms are requested
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Peds vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+100+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+100+hnm,20.);
       HMAXIM(ECHISTC+100+hnm,800.);
     }
   }
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Sigs vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+136+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+136+hnm,0.);
       HMAXIM(ECHISTC+136+hnm,5.);
     }
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Peds vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+172+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+172+sl,20.);
     HMAXIM(ECHISTC+172+sl,800.);
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Sigs vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+181+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+181+sl,0.);
     HMAXIM(ECHISTC+181+sl,5.);
   }
   HBOOK1(ECHISTC+193,"AllChannels Anode(HiGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+194,"AllChannels Anode(LoGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+195,"AllChannels Dynode PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+196,"AllChannels Anode(HiGain) PedDiff",50,-5.,5.,0.);
   HBOOK1(ECHISTC+197,"AllChannels Anode(LoGain) PedDiff",50,-5.,5.,0.);
   HBOOK1(ECHISTC+198,"AllChannels Dynode PedDiff",50,-5.,5.,0.);
 }
//
   DAQECBlock::PrevRunNum()=0;
   DAQECBlock::NReqEdrs()=0;
   DAQECBlock::GoodPedBlks()=0;
   DAQECBlock::FoundPedBlks()=0;
   DAQECBlock::FirstPedBlk()=true;
   DAQECBlock::CalFirstSeq()=true;
//
   for(int cr=0;cr<ECRT;cr++){
     for(int edr=0;edr<ECEDRS;edr++)DAQECBlock::PedBlkCrat(cr,edr)=-1;
   }
//
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   peds[ch][pix][gn]=0;
	   dpeds[ch][pix][gn]=-1;//missing
	   thrs[ch][pix][gn]=-1;
	   sigs[ch][pix][gn]=0;
	   stas[ch][pix][gn]=1;//bad
	 }
       }
     }
   }
//
 } 
//-----
 void ECPedCalib::resetb(){//run-by-run reset for OnBoardPedTable processing,
//called in buildonbP
   int16u sl,pm,pix,gn,gnm;
   char hmod[2]=" ";
   int16u i,j,ch;
//
 if((CALIB.SubDetInCalib%10)>1){//reset hist
   for(i=0;i<100;i++){
     if(i==90 || i==91 || i==92 || i==99)continue;
     HRESET(ECHISTC+100+i,hmod);
   }
 }
//
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   peds[ch][pix][gn]=0;
	   dpeds[ch][pix][gn]=-1;//missing
	   thrs[ch][pix][gn]=-1;
	   sigs[ch][pix][gn]=0;
	   stas[ch][pix][gn]=1;//bad
	 }
       }
     }
   }
//
   DAQECBlock::GoodPedBlks()=0;//start new calib.sequence
   DAQECBlock::FoundPedBlks()=0;
   DAQECBlock::NReqEdrs()=0;
//
    for(i=0;i<ECRT;i++)for(j=0;j<ECEDRS;j++)DAQECBlock::PedBlkCrat(i,j)=-1;
//
   if(ECREFFKEY.reprtf[2]>0){
     cout<<endl;
     cout<<"   <--- EcOnbPedCalib: Reset done..."<<endl<<endl;
   }
//
 }
//-----
 void ECPedCalib::filltb(integer swid, geant ped, geant dped, geant thr, geant sig){
//
   int16u i,sl,pm,pix,gn,ch,is;
// sl=0->, pm=0->, pix=1-3=>anodes,=4->dynode, gn=0/1->hi/low(for dynodes only hi)
// swid=>LTTPG(SupLayer/PMTube/Pixel/Gain)
//
   sl=swid/10000;
   sl=sl-1;//0-8
   pm=(swid%10000)/100-1;//0-35
   ch=ECPMSMX*sl+pm;//seq.# of sl*pm
   pix=(swid%100)/10;
   pix=pix-1;//0-4 (4->dynode)
   gn=(swid%10)-1;//0/1->high/low
   if(pix==4)gn=0;
   peds[ch][pix][gn]=ped;
   dpeds[ch][pix][gn]=dped;
   thrs[ch][pix][gn]=thr;
   sigs[ch][pix][gn]=sig;
 }
//----------
 void ECPedCalib::outptb(int flg){//called in buildonbP
// flg=1/2/3=>/write2DB/NoAction(hist only)/(write2file+hist)
   int i,j;
   int totchs(0),goodtbch(0),goodchs(0);
   geant goodchp(0);
   geant pedo,sigo;
   geant pedmin,pedmax,sigmin,sigmax;
   int stao;
   geant pdiff;
   int16u sl,pm,pix,gn,gnm,pln,cll,ch;
   uinteger runn=BRun();//run# 
   time_t begin=time_t(runn);//begin time = run-time(=runn) 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
//   strcpy(DataDate,asctime(localtime(&begin)));
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   pedmin=ECCAFFKEY.pedlim[0];
   pedmax=ECCAFFKEY.pedlim[1];
   sigmin=ECCAFFKEY.siglim[0];
   sigmax=ECCAFFKEY.siglim[1];
//
   integer spatt=ECCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
   bool dpedin=((spatt&4)==4);//dyn.peds-section present(243 words)
   bool thrsin=((spatt&2)==2);//thresholds ..............(243...)
   geant rthrs,rdped;
//
   if(ECREFFKEY.reprtf[2]>0)cout<<endl<<"       EcOnbPedCalib::EndOfRun:OnBoardTable-Report:"<<endl<<endl;
//----
   for(sl=0;sl<ECSLMX;sl++){//<---sup/layer loop
     for(pm=0;pm<ECPMSMX;pm++){//<--- pmt loop
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){//<--- pixel loop
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){//<--- gain loop
	   totchs+=1;
	   if(pix<4){//anodes, extract prev.calib ped/sig/sta for comparison
	     pedo=ECPMPeds::pmpeds[sl][pm].ped(pix,gn);
	     sigo=ECPMPeds::pmpeds[sl][pm].sig(pix,gn);
	     stao=ECPMPeds::pmpeds[sl][pm].sta(pix,gn);
	   }
	   else{//dynodes, extract prev.calib ped/sig/sta for comparison
	     pedo=ECPMPeds::pmpeds[sl][pm].pedd();
	     sigo=ECPMPeds::pmpeds[sl][pm].sigd();
	     stao=ECPMPeds::pmpeds[sl][pm].stad();
	   }
	   pdiff=peds[ch][pix][gn]-pedo;
	   if(DAQECBlock::CalFirstSeq() || stao==1)pdiff=0;//to exclude pdiff-check for 1st run or bad prev.channel
//	   cout<<" PrevRunNum="<<DAQECBlock::PrevRunNum()<<" stao="<<stao<<" peddif="<<pdiff<<endl;
//
           if(thrsin)rthrs=thrs[ch][pix][gn];
	   else rthrs=5;
	   if(dpedin)rdped=dpeds[ch][pix][gn];
	   else rdped=50;
//
	   if(peds[ch][pix][gn]>0 && rthrs>1 && rdped>0){// channel OK in table ? 
	     goodtbch+=1;
	     if(
//	        sigs[ch][pix][gn]>sigmin && 
		sigs[ch][pix][gn]<=sigmax
		&& peds[ch][pix][gn]>pedmin && peds[ch][pix][gn]<=pedmax && fabs(pdiff)<50){//MyCriteria:chan.OK
		
	       if(sigs[ch][pix][gn]<sigmin){
	         cout<<"        LowSigmaChannel: Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	         cout<<"                        ped/sig="<<peds[ch][pix][gn]<<" "<<sigs[ch][pix][gn]<<endl;    
	         sigs[ch][pix][gn]=sigmin;//"0"-sigma treatment
	       }
	       stas[ch][pix][gn]=0;//ch ok
	       goodchs+=1;
	       if(pix<4){//anodes
	         ECPMPeds::pmpeds[sl][pm].ped(pix,gn)=peds[ch][pix][gn];//save current values in PedCal-obj
	         ECPMPeds::pmpeds[sl][pm].sig(pix,gn)=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sta(pix,gn)=stas[ch][pix][gn];
		 pln=sl*2+(pix/2);//0-17
		 cll=pm*2+(pix%2);//0-71
		 if(flg>1){
		   HF1(ECHISTC+100+2*pln+gn,geant(cll+1),peds[ch][pix][gn]);
		   HF1(ECHISTC+136+2*pln+gn,geant(cll+1),sigs[ch][pix][gn]);
		   if(gn==0)HF1(ECHISTC+193,sigs[ch][pix][gn],1.);
		   else HF1(ECHISTC+194,sigs[ch][pix][gn],1.);
		   if(gn==0)HF1(ECHISTC+196,pdiff,1.);
		   else HF1(ECHISTC+197,pdiff,1.);
		 }
	       }
	       else{//dynodes
	         ECPMPeds::pmpeds[sl][pm].pedd()=peds[ch][pix][gn];//save current values in PedCal-obj
	         ECPMPeds::pmpeds[sl][pm].sigd()=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].stad()=stas[ch][pix][gn];
		 if(flg>1){
		   HF1(ECHISTC+172+sl,geant(pm+1),peds[ch][pix][gn]);
		   HF1(ECHISTC+181+sl,geant(pm+1),sigs[ch][pix][gn]);
		   HF1(ECHISTC+195,sigs[ch][pix][gn],1.);
		   HF1(ECHISTC+198,pdiff,1.);
		 }
	       }
	     }
	     else{//MyCriteria: bad chan
	       if(ECREFFKEY.reprtf[2]>0){
	         cout<<"       MyCriteriaBadCh: Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	         cout<<"                        ped/sig="<<peds[ch][pix][gn]<<" "<<sigs[ch][pix][gn]<<endl;    
	         cout<<"                        PedDiff="<<pdiff<<endl;
	       }    
	     }
	   }//--->endof "channel OK in table ?" check
	   else{
	     if(ECREFFKEY.reprtf[2]>0){
	       cout<<"       BadTableChan: Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	       cout<<"                        ped/sig="<<peds[ch][pix][gn]<<" "<<sigs[ch][pix][gn]<<endl;
	     }    
	   } 
	 }//--->endof gain-loop
       }//--->endof pixel-loop
     }//--->endof pmt-loop
   }//--->endof Slayer-loop
   goodchp=geant(goodchs)/totchs;
   
//
   if(ECREFFKEY.reprtf[2]>0)
    cout<<"     GoodChan(table/My):"<<goodtbch<<" "<<goodchs<<" from total="<<totchs<<" GoodChsPort="<<goodchp<<endl;  
//   
// ---> prepare update of DB if requested :
   if(goodchp>=0.5 && flg==1){// "on flight" DB-update
// 
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();//redefine according to VC.
     ptdv->SetTime(insert,min(AMSEvent::gethead()->getrun()-1,uinteger(AMSEvent::gethead()->gettime())),AMSEvent::gethead()->getrun()-1+86400*30);
     cout <<"      <--- EcalOnBoardPeds DB-info has been updated for "<<*ptdv<<endl;
     ptdv->gettime(insert,begin,end);
     cout<<"           Time ins/beg/end: "<<endl;
     cout<<"           "<<ctime(&insert);
     cout<<"           "<<ctime(&begin);
     cout<<"           "<<ctime(&end);
//
//
//     if(AMSFFKEY.Update==2 ){
//       AMSTimeID * offspring = 
//         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
//       while(offspring){
//         if(offspring->UpdateMe())cout << "         Start update Ecal-peds DB "<<*offspring; 
//         if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
//         cerr <<"         Problem To Update Ecal-peds in DB"<<*offspring;
//         offspring=(AMSTimeID*)offspring->next();//get one-by-one
//       }
//     }
   }
   else{
     if(goodchp<0.5)cout<<"      <--- GoodCh% is too low("<<goodchp<<") - No DB-writing !"<<endl;
   }
// ---> write OnBoardPedTable to ped-file:
   if(flg==3 && AMSFFKEY.Update==0){
     integer endflab(12345);
     char fname[1024];
     char name[80];
     char buf[20];
//
     strcpy(name,"eclp_tb_rl.");//from OnBoardTable
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- ECPedCalib: Problems to write new ONBT-Peds file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write HighGain peds/sigmas/stat:
//
     icfile.width(6);
     icfile.precision(1);// precision
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][0];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write LowGain peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][1];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write Dynode peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][4][0];
         }
         icfile << endl;
     } 
     icfile << endl;
//
// ---> write EndFileLabel :
//
     icfile << endflab;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the processed data : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   if(flg>1 && AMSFFKEY.Update==0){
       for(i=0;i<36;i++)HPRINT(ECHISTC+100+i);
       for(i=0;i<8;i++)HPRINT(ECHISTC+172+i);
       for(i=0;i<6;i++)HPRINT(ECHISTC+193+i);
   }
//
   DAQECBlock::CalFirstSeq()=false;
//
   cout<<endl;
   cout<<"<========= EcOnbPedCalib: run "<<runn<<" is processed, goodpeds%="<<goodchp<<endl;
   cout<<endl;
//
 }
//---------------------------------------------------
 void ECPedCalib::init(){//DS, called in caecinitjob() 
   int16u i,sl,pm,pix,gn,gnm;
   int hnm,ch;
   char buf[6];
   char htit[80];
//
   cout<<endl;
   if(ECREFFKEY.relogic[1]==6){//open Ntuple file (for OnBoardTable only for the moment)
/*
     char hfile[161];
     UHTOC(IOPA.hfile,40,hfile,160);  
     char filename[256];
     strcpy(filename,hfile);
     integer iostat;
     integer rsize=1024;
     char event[80];  
     HROPEN(IOPA.hlun+1,"ecpedsig",filename,"NP",rsize,iostat);
     if(iostat){
       cerr << "<==== ECPedCalib::init: Error opening ecpedsig ntuple file "<<filename<<endl;
       exit(1);
     }
     else cout <<"====> ECPedCalib::init: Ntuple file "<<filename<<" opened..."<<endl;
     HBNT(IOPA.ntuple,"EcalPedSigmas"," ");
     HBNAME(IOPA.ntuple,"ECPedSig",(int*)(&ECPedCalNT),"Run:I,SLayer:I,PMTNo:I,PedH(5):R,SigH(5):R,"
                                                                               "PedL(5):R,SigL(5):R,"
									       "StaH(5):I,StaL(5):I");
*/
     return;
   }
//
//   nstacksz=floor(ECCAFFKEY.pedcpr*ECPCEVMX+0.5);
   nstacksz=ECPCSTMX;
   cout<<"====> ECPedCalib::init: real stack-size="<<nstacksz<<endl;
//
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   adc[ch][pix][gn]=0;
	   adc2[ch][pix][gn]=0;
	   for(i=0;i<ECPCSTMX;i++)adcm[ch][pix][gn][i]=0;
           port2r[ch][pix][gn]=0;//portion of hi-ampl to remove
	   nevt[ch][pix][gn]=0;
	   peds[ch][pix][gn]=0;
	   sigs[ch][pix][gn]=0;
	   stas[ch][pix][gn]=1;//bad
	 }
       }
     }
   }
//
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Peds vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+100+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+100+hnm,20.);
       HMAXIM(ECHISTC+100+hnm,800.);
     }
   }
   for(sl=0;sl<2*ECSLMX;sl++){
     sprintf(buf,"%2d",sl);
     for(gn=0;gn<2;gn++){
       strcpy(htit,"Anode Sigs vs Cell for Plane/Gain=");
       strcat(htit,buf);
       if(gn==0)strcat(htit,"H");
       else strcat(htit,"L");
       hnm=2*sl+gn;
       HBOOK1(ECHISTC+136+hnm,htit,72,1.,73.,0.);
       HMINIM(ECHISTC+136+hnm,0.);
       HMAXIM(ECHISTC+136+hnm,5.);
     }
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Peds vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+172+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+172+sl,20.);
     HMAXIM(ECHISTC+172+sl,800.);
   }
   for(sl=0;sl<ECSLMX;sl++){
     strcpy(htit,"Dynode Sigs vs Pmt for SuperLayer=");
     sprintf(buf,"%2d",sl);
     strcat(htit,buf);
     HBOOK1(ECHISTC+181+sl,htit,36,1.,37.,0.);
     HMINIM(ECHISTC+181+sl,0.);
     HMAXIM(ECHISTC+181+sl,5.);
   }
   HBOOK1(ECHISTC+190,"Sl/Pm/Pix=5/18/2 AnodeH(raw)",100,50.,550.,0.);
   HBOOK1(ECHISTC+191,"Sl/Pm/Pix=5/18/2 AnodeL(raw)",100,50.,550.,0.);
   HBOOK1(ECHISTC+192,"Sl/Pm=5/18 Dynode(raw)",100,50.,550.,0.);
   HBOOK1(ECHISTC+193,"AllChannels Anode(HiGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+194,"AllChannels Anode(LoGain) PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+195,"AllChannels Dynode PedRms",50,0.,2.5,0.);
   HBOOK1(ECHISTC+196,"AllChannels Anode(HiGain) PedDiff",50,-5.,5.,0.);
   HBOOK1(ECHISTC+197,"AllChannels Anode(LoGain) PedDiff",50,-5.,5.,0.);
   HBOOK1(ECHISTC+198,"AllChannels Dynode PedDiff",50,-5.,5.,0.);
   HBOOK1(ECHISTC+199,"Sl/Pm/Pix=5/18/2 AnodeH(inlim)",100,50.,550.,0.);
   HBOOK1(ECHISTC+200,"Sl/Pm/Pix=5/18/2 AnodeL(inlim)",100,50.,550.,0.);
   HBOOK1(ECHISTC+201,"Sl/Pm=5/18 Dynode(inlim)",100,50.,550.,0.);
   cout<<"====> ECPedCalib::init done..."<<endl<<endl;;
 }
//-----
 void ECPedCalib::fill(integer swid, geant val){//DS, called at validate-stage
//
   int16u i,sl,pm,pix,gn,ch,nev,is;
   geant lohil[2]={0,9999};//means no limits on val, if partial ped is bad
// sl=0->, pm=0->, pix=1-3=>anodes,=4->dynode, gn=0/1->hi/low(for dynodes only hi)
// swid=>LTTPG(SupLayer/PMTube/Pixel/Gain)
   geant ped,sig,sig2;
   geant hi2lr,a2dr,gainf,spikethr;
   bool accept(true);
   int pml,pmr;
//
   integer evs2rem;
   geant por2rem,p2r;
   geant apor2rm[10]={0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,};
   number ad,ad2,dp,ds;
   geant pedi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   geant sigi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   geant pedmin,pedmax,sigmin,sigmax,smn2;
//
   sigmin=ECCAFFKEY.siglim[0];
   sigmax=ECCAFFKEY.siglim[1];
   smn2=sigmin*sigmin;
//
   sl=swid/10000;
   sl=sl-1;//0-8
   pm=(swid%10000)/100-1;//0-35
   ch=ECPMSMX*sl+pm;//seq.# of sl*pm
   pix=(swid%100)/10;
   pix=pix-1;//0-4 (4->dynode)
   gn=(swid%10)-1;//0/1->high/low
   if(pix==4)gn=0;
//
   nev=nevt[ch][pix][gn];
// peds[ch][pix][gn];//SmalSample(SS) ped (set to "0" at init)
   if(peds[ch][pix][gn]==0 && nev==ECPCEVMN){//calc. SubSet-ped/sig when ECPCEVMN events is collected
//
//     cout<<"<----- start SS-peds calculation for L/B/S/PM="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
//     for(i=0;i<nstacksz;i++){
//       cout<<adcm[ch][pix][gn][i]<<" ";
//       if((i+1)%10==0)cout<<endl;
//     }
//     cout<<endl;   
     int llindx(-1);   
     for(int ip2r=0;ip2r<10;ip2r++){//<--- portion to remove loop
       p2r=apor2rm[ip2r];
       evs2rem=int(floor(p2r*nev+0.5));
       if(evs2rem>nstacksz)evs2rem=nstacksz;
       ad=adc[ch][pix][gn];
       ad2=adc2[ch][pix][gn];
       for(i=0;i<evs2rem;i++){//remove "evs2rem" highest amplitudes
         ad=ad-adcm[ch][pix][gn][i];
         ad2=ad2-adcm[ch][pix][gn][i]*adcm[ch][pix][gn][i];
       }
       ped=ad/number(nev-evs2rem);//truncated average
       sig2=ad2/number(nev-evs2rem);
       sig2=sig2-ped*ped;// truncated rms**2
       if(sig2<=smn2 && sig2>-smn2)sig2=smn2;//tempor bypass accur.problem ???
       if(sig2>0)sig=sqrt(sig2);
       else sig=0;
       if(ip2r>0){
         dp=pedi[ip2r-1]-ped;
         ds=sigi[ip2r-1]-sig;
       }
       else{
         dp=9999;
         ds=9999;
       }
       pedi[ip2r]=ped;
       sigi[ip2r]=sig;
//       cout<<"<-- ip2r/por2r="<<ip2r<<"/"<<p2r<<" ped/dp="<<ped<<" "<<dp<<" sig/ds="<<sig<<" "<<ds<<endl;
       if((sig < sigmax && sig>=sigmin)
                                      && (dp < 1.0)
                                                   && (ds < 0.5)
		                                                && ip2r > 0){
         port2r[ch][pix][gn]=p2r;
         llindx=ip2r;
	 break;
       }
//       cout<<"   pedi/sigi="<<pedi[ip2r]<<" "<<sigi[ip2r]<<endl;
     }//--->endof portion to remove loop
//
     if(llindx<0){//fail to find SubSet-ped/sig - suspicious channel
       sig=0;
       ped=0;
       port2r[ch][pix][gn]=-1;
     }
//
     sigs[ch][pix][gn]=sig;
     peds[ch][pix][gn]=ped;//is used now as flag that SubSet-PedS ok
     adc[ch][pix][gn]=0;//reset to start new life(with real selection limits)
     adc2[ch][pix][gn]=0;
     nevt[ch][pix][gn]=0;
     for(i=0;i<ECPCSTMX;i++)adcm[ch][pix][gn][i]=0;
   }//--->endof SS(1st 100evs) check
//
   ped=peds[ch][pix][gn];//now !=0 or =0 
   sig=sigs[ch][pix][gn];
//
   if(ped>0){//set val-limits if partial ped OK
     lohil[0]=ped-3.5*sig;
     lohil[1]=ped+5*sig;
     if(gn==0){//hi(an or dyn)
       if(pix<4)gainf=1.;//hi an
       else gainf=ECcalib::ecpmcal[sl][pm].an2dyr();//dyn
     }
     else{//low an
       gainf=ECcalib::ecpmcal[sl][pm].hi2lowr(pix);//low an
     }
     spikethr=max(5*sig,ECPCSPIK/gainf);
     if(val>(ped+spikethr)){//spike(>~1mips in higain chan)
       hiamap[sl][pm]=1;//put it into map
       accept=false;//mark as bad for filling
     }
     else{//candidate for "fill"
       if(pm>0)pml=pm-1;
       else pml=0;
       if(pm<(ECPMSMX-1))pmr=pm+1;
       else pmr=ECPMSMX-1;
       accept=(hiamap[sl][pml]==0 && hiamap[sl][pmr]==0);//not accept if there is any neighbour(horizontal)
     }
   }
//
//   accept=true;//tempor to switch-off spike algorithm
//
   if(val>lohil[0] && val<lohil[1] && accept){//check "in_limits/not_spike"
     if(nev<ECPCEVMX){//limit statistics(to keep max-stack size small)
       adc[ch][pix][gn]+=val;
       adc2[ch][pix][gn]+=(val*val);
       nevt[ch][pix][gn]+=1;
       for(is=0;is<nstacksz;is++){//try to position val in stack of nstacksz highest max-values
          if(val>adcm[ch][pix][gn][is]){
	    for(i=nstacksz-1;i>is;i--)adcm[ch][pix][gn][i]=adcm[ch][pix][gn][i-1];//move stack -->
	    adcm[ch][pix][gn][is]=val;//store max.val
	    break;
	  }
       }
     }
   }//-->endof "in limits" check
   if(sl==4 && pm==17){
     if(pix==1 && gn==0)HF1(ECHISTC+190,val,1.);
     if(pix==1 && gn==1)HF1(ECHISTC+191,val,1.);
     if(pix==4 && gn==0)HF1(ECHISTC+192,val,1.);
     if(ped>0 && val>lohil[0] && val<lohil[1] && accept){
       if(pix==1 && gn==0)HF1(ECHISTC+199,val,1.);
       if(pix==1 && gn==1)HF1(ECHISTC+200,val,1.);
       if(pix==4 && gn==0)HF1(ECHISTC+201,val,1.);
     }
   } 
 }
//-----
 void ECPedCalib::outp(int flg){//called in ecalendjob
// flg=0/1/2=>HistosOnly/write2DB+File/write2file
   int i,j;
   geant pdiff;
   geant por2rem,p2r;
   geant pedmin,pedmax,sigmin,sigmax,smn2;
   integer statmin(9999);
   int16u sl,pm,pix,gn,gnm,pln,cll,ch;
   time_t begin=BTime();//begin time = 1st_event_time(filled at 1st "ped-block" arrival) 
   uinteger runn=BRun();//1st event run# 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   int totchs(0),goodchs(0);
   geant goodchp(0);
   geant sigdef(0.4);//to use when sig=0
//   strcpy(DataDate,asctime(localtime(&begin)));
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer evs2rem;
//
   por2rem=ECCAFFKEY.pedcpr;//def port. of hi-ampl to remove for DownScaled(in trigger)
   pedmin=ECCAFFKEY.pedlim[0];
   pedmax=ECCAFFKEY.pedlim[1];
   sigmin=ECCAFFKEY.siglim[0];
   sigmax=ECCAFFKEY.siglim[1];
   smn2=sigmin*sigmin;
//
   cout<<endl;
   cout<<"=====> ECPedCalib-Report:"<<endl<<endl;
   for(sl=0;sl<ECSLMX;sl++){
     for(pm=0;pm<ECPMSMX;pm++){
       ch=ECPMSMX*sl+pm;//seq.# of sl*pm
       for(pix=0;pix<5;pix++){
         gnm=2;
	 if(pix==4)gnm=1;//only hi-gain for dynodes
         for(gn=0;gn<gnm;gn++){
	   totchs+=1;
           if(port2r[ch][pix][gn]<0)p2r=por2rem;//use default for suspicious channel
           else{
	     if(pm==0)p2r=port2r[ch][pix][gn]/10;//use reduced value because of the ped+-n*sig limits
	     else p2r=port2r[ch][pix][gn]/15;
	   }
	   if(nevt[ch][pix][gn]>=ECPCEVMN){//statistics ok
	     evs2rem=floor(p2r*nevt[ch][pix][gn]+0.5);
	     if(evs2rem>nstacksz)evs2rem=nstacksz;
             if(evs2rem<1)evs2rem=1;
	     for(i=0;i<evs2rem;i++){//remove highest amplitudes
	       adc[ch][pix][gn]=adc[ch][pix][gn]-adcm[ch][pix][gn][i];
	       adc2[ch][pix][gn]=adc2[ch][pix][gn]-adcm[ch][pix][gn][i]*adcm[ch][pix][gn][i];
	     }
	     adc[ch][pix][gn]/=number(nevt[ch][pix][gn]-evs2rem);//truncated average
	     adc2[ch][pix][gn]/=number(nevt[ch][pix][gn]-evs2rem);
	     adc2[ch][pix][gn]=adc2[ch][pix][gn]-adc[ch][pix][gn]*adc[ch][pix][gn];//truncated rms**2
	     if(adc2[ch][pix][gn]<=smn2 && adc2[ch][pix][gn]>-smn2){
	       cout<<"       SuspCh:Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	       cout<<"                  Nevents="<<nevt[ch][pix][gn]<<endl;    
	       cout<<"                  ped/sig**2="<<adc[ch][pix][gn]<<" "<<adc2[ch][pix][gn]<<endl;
	       cout<<"                  Set sig to rms default !!!"<<endl;           
	       adc2[ch][pix][gn]=smn2;
	     }
//
	     if(adc2[ch][pix][gn]>=(sigmin*sigmin) && adc2[ch][pix][gn]<=(sigmax*sigmax)
	                           && adc[ch][pix][gn]>pedmin && adc[ch][pix][gn]<=pedmax){//chan.OK
	       peds[ch][pix][gn]=geant(adc[ch][pix][gn]);
	       sigs[ch][pix][gn]=geant(sqrt(adc2[ch][pix][gn]));
	       stas[ch][pix][gn]=0;//ok
	       goodchs+=1;
//update ped-object in memory:
	       if(pix<4){//anodes
	         pdiff=peds[ch][pix][gn]-ECPMPeds::pmpeds[sl][pm].ped(pix,gn);
	         ECPMPeds::pmpeds[sl][pm].ped(pix,gn)=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sig(pix,gn)=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sta(pix,gn)=stas[ch][pix][gn];
		 pln=sl*2+(pix/2);//0-17
		 cll=pm*2+(pix%2);//0-71
		 HF1(ECHISTC+100+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].ped(pix,gn));
		 HF1(ECHISTC+136+2*pln+gn,geant(cll+1),ECPMPeds::pmpeds[sl][pm].sig(pix,gn));
		 if(gn==0)HF1(ECHISTC+193,sigs[ch][pix][gn],1.);
		 else HF1(ECHISTC+194,sigs[ch][pix][gn],1.);
		 if(gn==0)HF1(ECHISTC+196,pdiff,1.);
		 else HF1(ECHISTC+197,pdiff,1.);
	       }
	       else{//dynodes
	         pdiff=peds[ch][pix][gn]-ECPMPeds::pmpeds[sl][pm].pedd();
	         ECPMPeds::pmpeds[sl][pm].pedd()=peds[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].sigd()=sigs[ch][pix][gn];
	         ECPMPeds::pmpeds[sl][pm].stad()=stas[ch][pix][gn];
		 HF1(ECHISTC+172+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].pedd());
		 HF1(ECHISTC+181+sl,geant(pm+1),ECPMPeds::pmpeds[sl][pm].sigd());
		 HF1(ECHISTC+195,sigs[ch][pix][gn],1.);
		 HF1(ECHISTC+198,pdiff,1.);
	       }
	       if(statmin>nevt[ch][pix][gn])statmin=nevt[ch][pix][gn];
	     }
	     else{//bad chan
	       cout<<"       BadCh:Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	       cout<<"                     Nevents="<<nevt[ch][pix][gn]<<endl;    
	       cout<<"                  ped/sig**2="<<adc[ch][pix][gn]<<" "<<adc2[ch][pix][gn]<<endl;    
	     }
	   }//--->endof "good statistics" check
	   else{
	     cout<<"       LowStatCh:Slay/Pmt/Pix/Gn="<<sl<<" "<<pm<<" "<<pix<<" "<<gn<<endl;
	     cout<<"                         Nevents="<<nevt[ch][pix][gn]<<endl;    
	   } 
	 }//--->endof gain-loop
       }//--->endof pixel-loop
     }//--->endof pmt-loop
   }//--->endof Slayer-loop
   goodchp=geant(goodchs)/totchs;
   cout<<"       MinAcceptableStatistics/channel was:"<<statmin<<" GoodChcPort="<<goodchp<<endl; 
//   
// ---> prepare update of DB :
   if(flg==1 && goodchp>=0.5){
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
   }
   else{
     if(flg==1 && goodchp<0.5)cout<<" <-- GoogChsPortion is too small - block writing to DB !"<<endl;
   }
// ---> write RD default-peds file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[1024];
     char name[80];
     char buf[20];
//
     if(ECREFFKEY.relogic[1]==4)strcpy(name,"eclp_cl");
     if(ECREFFKEY.relogic[1]==5)strcpy(name,"eclp_ds");
     if(AMSJob::gethead()->isMCData())           // for MC-event
     {
       cout <<"       new MC peds-file will be written..."<<endl;
       strcat(name,"_mc.");
     }
     else                                       // for Real events
     {
       cout <<"       new RD peds-file will be written..."<<endl;
       strcat(name,"_rl.");
     }
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new pedestals-file "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write HighGain peds/sigmas/stat:
//
     icfile.width(6);
     icfile.precision(1);// precision
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][0];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write LowGain peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
       for(pix=0;pix<4;pix++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][pix][1];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][pix][1];
         }
         icfile << endl;
       }
       icfile << endl;
     } 
//
// ---> write Dynode peds/sigmas/stats:
//
     for(sl=0;sl<ECSLMX;sl++){   
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << peds[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << sigs[ch][4][0];
         }
         icfile << endl;
         for(pm=0;pm<ECPMSMX;pm++){  
           ch=ECPMSMX*sl+pm;//seq.# of sl*pm
           icfile.width(6);
           icfile.precision(1);// precision
           icfile << stas[ch][4][0];
         }
         icfile << endl;
     } 
     icfile << endl;
//
// ---> write EndFileLabel :
//
     icfile << endflab;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<102;i++)HPRINT(ECHISTC+100+i);
   cout<<endl;
   cout<<"====================== ECPedCalib: job is completed ! ======================"<<endl;
   cout<<endl;
//
 }
//--------------
void ECPedCalib::ntuple_close(){
//
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//ecpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("ecpedsig");
  CLOSEF(IOPA.hlun+1);
//
}
//-----------------------------------------------------------
/*
#include "timeid.h"

// This PED-calibration is used when both (Raw and Compressed) format are
// present event-by-event. The Ped-object in memory can be updated and saved
// into DB and/or into ped-file at the end of the job.

// peds

integer  AMSECIdCalib::_Count[ECPMSMX][ECSLMX][4][3];
integer  AMSECIdCalib::_BadCh[ECPMSMX][ECSLMX][4][3];
number   AMSECIdCalib::_ADC[ECPMSMX][ECSLMX][4][3];
number   AMSECIdCalib::_ADCMax[ECPMSMX][ECSLMX][4][3];
number  AMSECIdCalib::_ADC2[ECPMSMX][ECSLMX][4][3];
time_t AMSECIdCalib::_BeginTime;
uinteger AMSECIdCalib::_CurRun=0;
AMSECIdCalib::ECCalib_def  AMSECIdCalib::ECCALIB;


void AMSECIdCalib::clear(){
for(int i=0;i<ECPMSMX;i++){
  for(int j=0;j<ECSLMX;j++){
   for(int k=0;k<4;k++){
    for(int l=0;l<3;l++){
     _Count[i][j][k][l]=0;
     _ADC[i][j][k][l]=0;
     _ADCMax[i][j][k][l]=0;
     _ADC2[i][j][k][l]=0;
    }
     ECPMPeds::pmpeds[j][i].sta(k,0)=0;
     ECPMPeds::pmpeds[j][i].sta(k,1)=0;
     if(i==1 && j==5){
       ECPMPeds::pmpeds[j][i].sta(k,0)|=AMSDBc::BAD;
       ECPMPeds::pmpeds[j][i].sta(k,1)|=AMSDBc::BAD;
     }
   }
  }
}
}


void AMSECIdCalib::updADC(uinteger adc, uinteger gain){
 if(!adc && gain<1)return;
 if(gain<3){
    (_Count[_pmt][_slay][_pixel][gain])++;
    (_ADC[_pmt][_slay][_pixel][gain])+=adc;
    (_ADC2[_pmt][_slay][_pixel][gain])+=adc*adc;
    if(_ADCMax[_pmt][_slay][_pixel][gain]<adc)_ADCMax[_pmt][_slay][_pixel][gain]=adc;
    HF1(-(gain+1)*10000-makeshortid(),adc-getped(gain),1.);
 }
 else{
  cerr <<"AMSECIdCalib::updADC-S-WrongGain "<<gain<<endl;
 }
}

void AMSECIdCalib::init(){

// clear pedestals
for(int i=0;i<ECPMSMX;i++){
  for(int j=0;j<ECSLMX;j++){
     ECcalib::ecpmcal[j][i].adc2mev()=1;
     ECcalib::ecpmcal[j][i].an2dyr()=30;
     ECcalib::ecpmcal[j][i].pmrgain()=1;
  for(int k=0;k<4;k++){
     ECcalib::ecpmcal[j][i].pmscgain(k)=1;
    for(int l=0;l<2;l++){
       ECPMPeds::pmpeds[j][i].ped(k,l)=4095;
       ECPMPeds::pmpeds[j][i].sta(k,l)=0;
       AMSECIds ids(i,j,k,l);
       if((ids.getlayer()==4 && ids.getcell()==4) ||
          (ids.getlayer()==5 && ids.getcell()==4) ||
          (ids.getlayer()==17 && ids.getcell()==13)){
          ECPMPeds::pmpeds[j][i].sta(k,l)=AMSDBc::BAD;
       }
   }
     if(i==0 && j==0 && k==0 )cout <<" hi2lowr **** "<<ECcalib::ecpmcal[j][i].hi2lowr(k)<<" "<<ECcalib::ecpmcal[j][i].adc2mev()<<" "<<ECcalib::ecpmcal[j][i].an2dyr()<<endl;
     ECcalib::ecpmcal[j][i].hi2lowr(k)=36;
      AMSECIds ids(i,j,k,0);
     if(ids.getlayer()==14 && ids.getcell()==6){
      ECcalib::ecpmcal[j][i].hi2lowr(k)=12;
     }  
    }
  }
}

{
geant gains[18][14];
ifstream fbin;
fbin.open("gains.china");
if(!fbin){
cerr<<"UnableToOpenfile gains.china"<<endl;
abort();
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

   
   for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<7;j++){
              ECcalib::ecpmcal[i][j].pmrgain()=1;
         for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<endl;             
              ECcalib::ecpmcal[i][j].pmscgain(k)=ECcalib::ecpmcal[i][j].pmscgain(k)*gains[ids.getlayer()][ids.getcell()];
           cout <<"  new gain "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].pmscgain(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}
{
geant gains[18][14];
ifstream fbin;
fbin.open("h2lr.dat");
if(!fbin){
 cerr<<" Unable to open fbin "<<endl;
 abort();
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)fbin >> gains[i][j];
}
for (int i=0;i<18;i++){
   for(int j=0;j<14;j++)cout<< gains[i][j]<<" ";
cout <<endl;
}

fbin.close();
   
   for(int i=0;i<ecalconst::ECSLMX;i++){
       for(int j=0;j<7;j++){
          for (int k=0;k<4;k++){
             AMSECIds ids(i,j,k,0);
           cout <<"  old hi2lowr "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].hi2lowr(k)<<endl;             
              ECcalib::ecpmcal[i][j].hi2lowr(k)=36./gains[ids.getlayer()][ids.getcell()];
           cout <<"  new hi2lowr "<<i<<" "<<j<<" "<<k<<" "<< ECcalib::ecpmcal[i][j].hi2lowr(k)<<" "<<ids.getlayer()<<" "<<ids.getcell()<<endl;             
              
         }
       }
      } 

}

    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);  
    char filename[256];
    strcpy(filename,hfile);
    char tmp[80];
    if(_CurRun<1000){
     sprintf(tmp,".0%d",_CurRun);
    }
    else{
     sprintf(tmp,".%d",_CurRun);
    }
//    strcat(filename,tmp);
    integer iostat;
    integer rsize=1024;
    char event[80];  
    HROPEN(IOPA.hlun+1,"ecpedsig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening ecpedsig ntuple file "<<filename<<endl;
     exit(1);
    }
    else cout <<"ecpedsig ntuple file "<<filename<<" opened."<<endl;
   HBNT(IOPA.ntuple,"EcalPedSigmas"," ");
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<7;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){
             AMSECIds ids(i,j,k,g);
             int id=ids.makeshortid(); 
             HBOOK1(-(g+1)*10000-id,"peds",100,-100.,100.,0.);
           }
         }
       }
     }
   HBNAME(IOPA.ntuple,"ECPedSig",(int*)(&ECCALIB),"Run:I,SLayer:I,PMTNo:I,Channel:I,Gain:I, Ped:R,ADCMax:R,Sigma:R,BadCh:I");
}

void AMSECIdCalib::getaverage(){


     int acount=0;
     int bad=0;
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<ECPMSMX;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){ 
            if(g==2 && k)continue;
            AMSECIds cid(i,j,k,g);
            AMSECIdCalib id(cid);
            if(id.dead())continue;
            if(id.getcount(g)>1){
              acount++;
              id.setADC(g)=(id.getADC(g)-id.getADCMax(g))/(id.getcount(g)-1);
              id.setADC2(g)=(id.getADC2(g)-id.getADCMax(g)*id.getADCMax(g))/(id.getcount(g)-1);
              id.setADC2(g)=sqrt(id.getADC2(g)-id.getADC(g)*id.getADC(g)); 
              if(id.getADC2(g)>10){
               cerr<<"AMSECIdCalib::getaverage-I-SigmaTooHigh "<<id.getADC2(g)<<" "<<g<<" "<<id.getADC(g)<<" "<<id.getADCMax(g)<<" "<<id.getcount(g)<<" "<<i<<" "<<j<<" "<<k<<endl;
              }
              if(g<2){
                // update pedestals & sigmas here
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].ped(id.getpixel(),g)=id.getADC(g);
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sig(id.getpixel(),g)=id.getADC2(g);
                // update status
                if(id.getADC(g)>4000){
                 ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sta(id.getpixek(),g)|=AMSDBc::BAD;
                 bad++;
                }                
              }
              else{
                // update pedestals & sigmas here
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].pedd()=id.getADC(g);
                ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sigd()=id.getADC2(g);
              }
             }
             else if(id.getped(g)<0){
                 ECPMPeds::pmpeds[id.getslay()][id.getpmt()].sta(id.getpixel(),g)|=AMSDBc::BAD;
                 bad++;
             }
            }
           }
          }
      }


    cout <<"  AMSECUdCalib::getaverage-I-"<<acount<< " Pedestals/Sigmas Calculated"<<endl;
    cout <<"  AMSECUdCalib::getaverage-I-"<<bad<< " Bad Pedestals/Sigmas Found"<<endl;


   //update db
{
    AMSTimeID *ptdv;
     time_t begin,end,insert;
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVped());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=_BeginTime;
      end=_BeginTime+86400*30;
      ptdv->SetTime(insert,begin,end);
      cout <<" updating db 2"<<begin<<endl;
}
{
    AMSTimeID *ptdv;
     time_t begin,end,insert;
      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSEcalRawEvent::getTDVcalib());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      begin=_BeginTime;
      end=_BeginTime+86400*30;
      ptdv->SetTime(insert,begin,end);
}

    if (AMSFFKEY.Update==2 ){
     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
      cerr <<"AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }

   // write ntuple


     int count=0;
     for(int i=0;i<ECSLMX;i++){
       for(int j=0;j<ECPMSMX;j++){
         for (int k=0;k<4;k++){
           for( int g=0;g<3;g++){ 
            if(g==2 && k)continue;
            AMSECIds cid(i,j,k,g);
            AMSECIdCalib id(cid);
            if(id.dead() || !id.getcount(g))continue;
            count++;
          ECCALIB.Run=_CurRun;
          ECCALIB.SLayer=i+1;
          ECCALIB.PmtNo=j+1;
          ECCALIB.Channel=k+1;
          ECCALIB.Gain=g+1;
          ECCALIB.Ped=id.getADC(g);
          ECCALIB.ADCMax=id.getADCMax(g);
          ECCALIB.Sigma=id.getADC2(g);
          ECCALIB.BadCh=ECPMPeds::pmpeds[j][i].sta(k,g) & AMSDBc::BAD>0;
          HFNT(IOPA.ntuple);
         }
        }
       }
     }
    cout <<"  AMSECUdCalib::write-I-"<<count<< " Pedestals/Sigmas Written"<<endl;
     }

void AMSECIdCalib::write(){
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//ecpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("ecpedsig");
  CLOSEF(IOPA.hlun+1);

  

}



void AMSECIdCalib::buildSigmaPed(integer n, int16u *p){

  integer ic=AMSEcalRawEvent::checkdaqid(*p)-1;
   
  int leng=0;
  int16u * ptr;
  int count=0; 
  int dynode=0;
  int dead=0;
// first pass :  get total amplitude

  geant sum=0;
  int pmtn;
  int slay;
  int chan;
{
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead() && gain==0){
             // high gain only
              if(sum<value-id.getped(gain)){
               chan=id.getchannel();
               pmtn=id.getpmtno();
               slay=id.getslay();
               sum=value-id.getped(gain);
             }
           }
   count++;
  }
//  cout << "sum "<<sum<<endl;
}
  number Threshold=100;
  if(sum<Threshold ){
  count=0;
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead()){
             AMSECIdCalib idc(id);
                         
             idc.updADC(value,gain);
            }
 count++;
           }

}
else{
 cout <<"  sum "<<sum<<" "<<pmtn+1<<" "<<slay+1<<" "<<chan+1<<endl;
}
}


void AMSECIdCalib::buildPedDiff(integer n, int16u *p){

  integer ic=AMSEcalRawEvent::checkdaqid(*p)-1;
   
  int leng=0;
  int16u * ptr;
  int count=0; 
  int dynode=0;
  int dead=0;
  for(ptr=p+1;ptr<p+n;ptr++){  
   int16u pmt=count%36;
            int16u anode=(*ptr>>15)& 1;
            int16u channel=((*ptr)>>12)&3;
            int16u gain=((*ptr)>>14)&1;
            int16u value=( (*ptr))&4095; 
            if(!anode){
               channel=4;
               gain=2;
            }
            else gain=1-gain;
  
           AMSECIds id(ic,pmt,channel);
           if(!id.dead()){
             AMSECIdCalib idc(id);
              idc.updADC(uinteger((value-id.getped(gain))*8),gain);
           }
 count++;
}
}
*/
