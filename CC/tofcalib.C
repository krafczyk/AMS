#include <point.h>
#include <typedefs.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <antidbc.h>
#include <tofsim.h>
#include <trrec.h>
#include <tofrec.h>
#include <antirec.h>
#include <tofcalib.h>
#include <particle.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// TOF indiv. bars parameters
extern TOFVarp tofvpar; // TOF general parameters
//
//--------------------------------------------------------------------
geant TOFTZSLcalib::slope;
geant TOFTZSLcalib::tzero[SCLRS][SCMXBR];
number TOFTZSLcalib::s1;
number TOFTZSLcalib::s2;
number TOFTZSLcalib::s3[SCLRS][SCMXBR];
number TOFTZSLcalib::s4;
number TOFTZSLcalib::s5;
number TOFTZSLcalib::s6[SCLRS-1][SCMXBR];
number TOFTZSLcalib::s7[SCMXBR];
number TOFTZSLcalib::s8;
number TOFTZSLcalib::s9[SCLRS-1][SCMXBR];
number TOFTZSLcalib::s10[SCMXBR];
number TOFTZSLcalib::s11;
number TOFTZSLcalib::s12[SCMXBR][SCMXBR];
number TOFTZSLcalib::s13[SCMXBR][SCMXBR];
number TOFTZSLcalib::s14[SCMXBR][SCMXBR];
number TOFTZSLcalib::s15[SCLRS-1][SCMXBR];
number TOFTZSLcalib::s16[SCMXBR];
number TOFTZSLcalib::events;
number TOFTZSLcalib::resol;
//-----
void TOFTZSLcalib::mfit(){  // calibr. fit procedure
  int i,id,ii,j,ier,il,ib,npar(SCBLMX+2),nparr;
//
  char pnm[6];
  char p1nam[6];
  char p2nam[6];
  char p3nam[3];
  char inum[11];
  char in[2]="0";
  int ifit[SCBLMX+2];
  char pnam[SCBLMX+2][6];
  number argl[10];
  int iargl[10];
  number start[SCBLMX+2],step[SCBLMX+2],plow[SCBLMX+2],phigh[SCBLMX+2];
//
  strcpy(p1nam,"tslop");
  strcpy(p2nam,"dummy");
  strcpy(p3nam,"tz");
  strcpy(inum,"0123456789");
//
// -----------> set parameter defaults:
  strcpy(pnam[0],p1nam); // for slope
  start[0]=TOFCAFFKEY.fixsl;// def. slope
  step[0]=1.;
  plow[0]=0.;
  phigh[0]=40.;
  ifit[0]=TOFCAFFKEY.ifsl;// fix/release slope 
//
  strcpy(pnam[1],p2nam); // for inverse stratcher ratio (now dummy)
  start[1]=1.;// now str.ratio is known and already included in input-times !!!
  step[1]=0.01;
  plow[1]=0.001;
  phigh[1]=2.;
  ifit[1]=0;// fixed(known) now !!!
//
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      id=(il+1)*100+ib+1;
      ii=il*SCMXBR+ib;
      start[ii+2]=0.;// def.T0 for all counters
      for(j=0;j<2;j++){
        if(id == TOFCAFFKEY.idref[j])
          start[ii+2]=TOFCAFFKEY.tzref[j];//def. t0 for ref.counters
      }
      step[ii+2]=1.;
      plow[ii+2]=-40.;
      phigh[ii+2]=40.;
      ifit[ii+2]=0;
      strcpy(pnm,p3nam);
      in[0]=inum[il];
      strcat(pnm,in);
      i=ib/10;
      j=ib%10;
      in[0]=inum[i];
      strcat(pnm,in);
      in[0]=inum[j];
      strcat(pnm,in);
      strcpy(pnam[2+ii],pnm);
    }
  }
// ------------> release T0-parameters with good statistics:
// 
  printf(" Collected events (total) : %9.0f\n",events);
  printf(" Collected events per sc. bar : \n");
  for(il=0;il<SCLRS;il++){
    for(ib=1;ib<=SCMXBR;ib++){
      if(ib%SCMXBR !=0)
                     printf("% 7.0f",s3[il][ib-1]);
      else
                     printf("% 7.0f\n",s3[il][ib-1]);
    }
  }
//
  nparr=0;
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      id=(il+1)*100+ib+1;
      if(s3[il][ib]>=1.){
        ifit[2+il*SCMXBR+ib]=1;//bar with high statist.-> release
        nparr+=1;
        for(j=0;j<2;j++){
          if(id == TOFCAFFKEY.idref[j]){
            ifit[2+il*SCMXBR+ib]=0;//fix, if ref counter
            nparr-=1;
          }
        }
      }
      else{
        for(j=0;j<2;j++){
          if(id == TOFCAFFKEY.idref[j]){
            cerr<<"Too low statistics in ref.counter "<<id<<" "<<s3[il][ib]<<'\n';
            return;
          }
        }
        ifit[2+il*SCMXBR+ib]=0;//bar with low statist.-> fix
      }
    }
  }
  nparr+=2;
  printf(" Statistics enough to fit  %d  parameters\n",nparr);
  if(nparr<10){
    cerr<<"   Not enough counters for Minuit-Fit !!!"<<'\n';
    return;// not enough counters for FIT -> return
  }
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("TZSL-calibration for TOF-system");
  for(i=0;i<npar;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],step[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cerr<<"TOF-calib: Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cerr<<"TOF-calib: Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
//----
  argl[0]=0.;
  ier=0;
  MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
  if(ier!=0){
    cerr<<"TOF-calib: MINIMIZE problem !"<<'\n';
    exit(10);
  }  
  MNEXCM(mfun,"MINOS",argl,0,ier,0);
  if(ier!=0){
    cerr<<"TOF-calib: MINOS problem !"<<'\n';
    exit(10);
  }
  argl[0]=number(3);
  ier=0;
  MNEXCM(TOFTZSLcalib::mfun,"CALL FCN",argl,1,ier,0);
  if(ier!=0){
    cerr<<"TOF-calib: final CALL_FCN problem !"<<'\n';
    exit(10);
  }
//----
  printf("     Minuit ended with parameters:\n");
  printf("    -------------------------------\n");
  printf("Resolution : %6.3e\n",resol);
  printf("Slope      : %6.3e\n",slope);
}
//-----------------------------------------------------------------------
// This is standard Minuit FCN :
void TOFTZSLcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j,il,ib;
  integer id,ibt,idr,ibtr;
  static int first(0);
  number f1[4],f2[3],f3(0.),f4[3],f5(0.),f6(0.),f7(0.),f8(0.);
  number f9[3],f10(0.);
  geant w,tz,tzr;
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create name for output file
// 
  strcpy(inum,"0123456789");
  cfvn=TOFCAFFKEY.cfvers%100;
  strcpy(fname,"tzcalib");
  dig=cfvn/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".dat");
//
//--> get run/time of the first event
//
  StartRun=AMSTOFRawEvent::getsrun();
  StartTime=AMSTOFRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
//
  for(il=0;il<SCLRS;il++){
    f1[il]=0.;
    for(ib=0;ib<SCMXBR;ib++)f1[il]+=s3[il][ib]*x[2+il*SCMXBR+ib]
                                              *x[2+il*SCMXBR+ib];
  }
//
  for(i=0;i<3;i++){
    f2[i]=0.;
    il=i+1;
    for(ib=0;ib<SCMXBR;ib++)f2[i]+=s6[i][ib]*x[2+il*SCMXBR+ib];
  }
//
  for(ib=0;ib<SCMXBR;ib++)f3+=s7[ib]*x[2+ib];
//
  for(i=0;i<3;i++){
    f4[i]=0.;
    il=i+1;
    for(ib=0;ib<SCMXBR;ib++)f4[i]+=s9[i][ib]*x[2+il*SCMXBR+ib];
  }
//
  for(ib=0;ib<SCMXBR;ib++)f5+=s10[ib]*x[2+ib];
//
  for(i=0;i<SCMXBR;i++){
    for(j=0;j<SCMXBR;j++)f6+=s12[i][j]*x[2+i]*x[2+1*SCMXBR+j];
  }
//
  for(i=0;i<SCMXBR;i++){
    for(j=0;j<SCMXBR;j++)f7+=s13[i][j]*x[2+i]*x[2+2*SCMXBR+j];
  }
//
  for(i=0;i<SCMXBR;i++){
    for(j=0;j<SCMXBR;j++)f8+=s14[i][j]*x[2+i]*x[2+3*SCMXBR+j];
  }
//
  for(i=0;i<3;i++){
    f9[i]=0.;
    il=i+1;
    for(ib=0;ib<SCMXBR;ib++)f9[i]+=s15[i][ib]*x[2+il*SCMXBR+ib];
  }
//
  for(ib=0;ib<SCMXBR;ib++)f10+=s16[ib]*x[2+ib];
//
  f=s1+s2*x[1]*x[1]+3.*f1[0]+f1[1]+f1[2]+f1[3]+s4*x[0]*x[0]
   -2.*s5*x[1]+2.*(f2[0]+f2[1]+f2[2])-2.*f3+2.*s8*x[0]
   -2.*(f4[0]+f4[1]+f4[2])*x[1]+2.*f5*x[1]-2.*s11*x[0]*x[1]
   -2.*(f6+f7+f8)+2.*(f9[0]+f9[1]+f9[2])*x[0]-2.*f10*x[0];
//----------------
  if(flg==3){
// some histograms:
    idr=TOFCAFFKEY.idref[0];
    tzr=TOFCAFFKEY.tzref[0];
    il=idr/100-1;
    ib=idr%100-1;
    ibtr=TOFDBc::brtype(il,ib);// ref. bar type (1-5)
    for(il=0;il<SCLRS;il++){
      for(ib=0;ib<SCMXBR;ib++){
        tz=geant(x[2+il*SCMXBR+ib]);
        ibt=TOFDBc::brtype(il,ib);// bar type (1-5)
        if(ibt==ibtr)HF1(1507,(tz-tzr),1.);
      }
    }
// write parameters to ext.file:
    resol=-1.;
    if(f>=0. && events>0)resol=sqrt(f/events);
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"TOFTZSLcalib:error opening file for output"<<fname<<'\n';
      exit(8);
    }
    cout<<"Open file for TZSL-calibration output, fname:"<<fname<<'\n';
    cout<<"Slope and individual T0's will be written !"<<'\n';
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(6);
    tcfile.precision(2);// precision for slope
    slope=geant(x[0]);
    tcfile << slope<<endl;
    tcfile.precision(3);// precision for T0
    for(il=0;il<SCLRS;il++){
      for(ib=0;ib<SCMXBR;ib++){
        tzero[il][ib]=geant(x[2+il*SCMXBR+ib]);
        tcfile <<" "<<tzero[il][ib];
      }
      tcfile << endl;
    }
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"TOFTZSLcalib:output file closed !"<<endl;
  }
}
//-----------------------------------------------------------------------
// To fill arrays, used by FCN :
void TOFTZSLcalib::fill(number bet, int ib[4], number tld[3]
                                             ,number dt[3],number du[3]){
  static int first(0);
  static number vl(29.979); // light velocity [cm/ns]
  number dtr[3];
  int i,j;
//
  for(i=0;i<3;i++)dtr[i]=tld[i]/bet/vl;
//
  events+=1.;
  for(i=0;i<3;i++)s1+=dtr[i]*dtr[i];
  for(i=0;i<3;i++)s2+=dt[i]*dt[i];
  for(i=0;i<4;i++)s3[i][ib[i]]+=1.;
  for(i=0;i<3;i++)s4+=du[i]*du[i];
  for(i=0;i<3;i++)s5+=dtr[i]*dt[i];
  for(i=0;i<3;i++)s6[i][ib[i+1]]+=dtr[i];
  for(i=0;i<3;i++)s7[ib[0]]+=dtr[i];
  for(i=0;i<3;i++)s8+=dtr[i]*du[i];
  for(i=0;i<3;i++)s9[i][ib[i+1]]+=dt[i];
  for(i=0;i<3;i++)s10[ib[0]]+=dt[i];
  for(i=0;i<3;i++)s11+=dt[i]*du[i];
  s12[ib[0]][ib[1]]+=1.;
  s13[ib[0]][ib[2]]+=1.;
  s14[ib[0]][ib[3]]+=1.;
  for(i=0;i<3;i++)s15[i][ib[i+1]]+=du[i];
  for(i=0;i<3;i++)s16[ib[0]]+=du[i];
}
//========================================================================
void TOFTZSLcalib::select(){  // calibr. event selection
  integer i,j,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad,status,sector,conf;
  integer cref[2],lref[2];
  number tm[2],am[2],ama[2],amd[2],time,timeD,tamp,edepa,edepd,relt;
  number coo[SCLRS],trp1[SCLRS],trp2[SCLRS],arp1[SCLRS],arp2[SCLRS];
  geant slops[2];
  number shft,ftdel,qtotl[SCLRS],qsd1[SCLRS],qsd2[SCLRS],eanti(0),meanq,rr,qmax;
  number eacut=0.6;// cut on E-anti (mev). tempor (no calibration)
  number qrcut=8.;// cut on max/mean-charge ratio
  number dscut=8.;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  number *pntr[SCLRS];
  AMSTOFRawCluster *ptr;
  AMSAntiCluster *ptra;
//
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  TOFJobStat::addre(6);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
  for(i=0;i<SCLRS;i++)qtotl[i]=0;
  cref[0]=TOFCAFFKEY.idref[0]%100-1;//BB-ref1(0-13)
  lref[0]=TOFCAFFKEY.idref[0]/100-1;//L-ref1(0-3)
//----
  while (ptr){ // <--- loop over AMSTOFRawCluster hits
    status=ptr->getstatus();
    if(status==0){ //select only 2-sided "good_history/good_strr" hits
      ilay=(ptr->getntof())-1;
      ibar=(ptr->getplane())-1;
      ptr->gettovta(ama);
      scbrcal[ilay][ibar].ama2q(ama,am);// convert side-TovT to charge
      qtotl[ilay]=am[0]+am[1];
      qsd1[ilay]=am[0];
      qsd2[ilay]=am[1];
      ptr->gettovtd(amd);
      ptr->getsdtm(tm);// raw side-times(A-noncorrected)
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      trp1[ilay]=tm[0];
      trp2[ilay]=tm[1];
      arp1[ilay]=ama[0];
      arp2[ilay]=ama[1];
      coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//----
//    Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOFJobStat::addre(7);
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
  HF1(1503,geant(eanti),1.);
  if(eanti>eacut)return;// remove events with big signal in Anti. tempor commented
//
// -----> check amplitudes :
//
  for(i=0;i<SCLRS;i++)pntr[i]=&qtotl[i];
  AMSsortNAG(pntr,SCLRS);// sort in increasing order
  meanq=0;
  for(i=0;i<(SCLRS-1);i++)meanq+=(*pntr[i]);
  meanq/=number(SCLRS-1);
  qmax=*pntr[SCLRS-1];
  rr=qmax/meanq;
  HF1(1505,geant(rr),1.);
  if(rr>qrcut)return; // remove events with "spike" dE/dX.  tempor commented
  TOFJobStat::addre(8);
//------>
    number t1,t2,t3,t4,t13,t24;
    conf=1;
//
       ilay=0; //        <-- some hist. for calibration run
    shft=TOFDBc::shftim();
    ftdel=TOFDBc::ftdelf();
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    t1=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      if(brnl[0]==6)HF2(1502,geant(tamp),geant(relt),1.);
    }
//
       ilay=1;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t2=time;
       ilay=2;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    t3=time;
//
       ilay=3;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    t4=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      if(brnl[3]==6)HF2(1504,geant(tamp),geant(relt),1.);
    }
//
    t13=t1-t3;
    t24=t2-t4;
    if(TOFRECFFKEY.reprtf[2]>0){
//      HF1(1550,geant(t1),1.);
//      HF1(1551,geant(t2),1.);
//      HF1(1552,geant(t3),1.);
//      HF1(1553,geant(t4),1.);
    }
//------> soft usage of tracker : get momentum to select beta~1
//
    static number pmas(0.938),mumas(0.1057);
    number pmom,bet,chi2,betm;
    number the,phi,rid,err,trl;
    int il,ib,ix,iy;
    geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cosc,cosi,cost,xtr[SCLRS],ytr[SCLRS];
    geant scchi2[2],xer[SCLRS],yer[SCLRS],lcerr,lvel;
    number ram[4],ramm[4],dum[3],tld[3],tdi[3],trlr[SCLRS],trlen[SCLRS-1];
    number ctran,coot[SCLRS],cstr[SCLRS],dx,dy;
    number sl[2],t0[2],sumc,sumc2,sumt,sumt2,sumct,sumid,zco,tco,dis;
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
    HF1(1506,geant(ntrk),1.);
    if(ntrk!=1 && TOFCAFFKEY.truse>=0)return;// require events with 1 track.
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    } 
    else rid=0;
    pmom=fabs(rid);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(TOFCAFFKEY.truse>=0)
       if(pmom<TOFCAFFKEY.pcut[0] || pmom>TOFCAFFKEY.pcut[1])return;//remove low/too_high mom.
    if(TOFCAFFKEY.caltyp==0)bet=pmom/sqrt(pmom*pmom+pmas*pmas);// space calibration
    else bet=pmom/sqrt(pmom*pmom+mumas*mumas);// earth calibration
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1501,bet,1.);
//
    TOFJobStat::addre(9);
//
// -----> hard usage of tracker: get track length
//
    bad=0;
    if(TOFCAFFKEY.truse==1){
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
      cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);//average cos from track
      trlen[0]=trlr[0]-trlr[1];//1->2
      trlen[1]=trlr[0]+trlr[2];//1->3
      trlen[2]=trlr[0]+trlr[3];//1->4
    }
//
    if(bad)return;// remove events with big difference of TOF-Tracker coord.
    TOFJobStat::addre(10);
//
//------> find track impact angle using scint-made transv.coord :
  if(TOFCAFFKEY.tofcoo==0){
    ix=0;
    iy=0;
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      if(TOFDBc::plrotm(il)==0){// unrotated (X-meas) planes
        x[ix]=TOFDBc::gettsc(il,ib);
        zx[ix]=zc[il];
        ix+=1;
      }
      else{                    // rotated (Y-meas) planes
        y[iy]=TOFDBc::gettsc(il,ib);
        zy[iy]=zc[il];
        iy+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
  }
//
//
//-------> get track impact angle using scint-made long.coord :
//
  bad=0;
  if(TOFCAFFKEY.tofcoo==1){
    ix=0;
    iy=0;                                
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      scbrcal[il][ib].getd2p(lvel,lcerr);
      if(TOFDBc::plrotm(il)==0){// unrotated (Long->Yabs) planes(2,3)
        y[iy]=coo[il];
        zy[iy]=zc[il];
        xtr[il]=TOFDBc::gettsc(il,ib);
        xer[il]=3.18;// bar_wid/sqrt(12.)
        ytr[il]=y[iy];
        yer[il]=lcerr;
        iy+=1;
      }
      else{                    // rotated (Long->-Xabs) planes(1,4)
        x[ix]=-coo[il];
        zx[ix]=zc[il];
        xtr[il]=x[ix];
        xer[il]=lcerr;
        ytr[il]=TOFDBc::gettsc(il,ib);
        yer[il]=3.18;
        ix+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
//--- get track from TOF(coord.fit):
//
    for(j=0;j<2;j++){//proj.loop
      scchi2[j]=0.;
      sl[j]=0.;
      t0[j]=0.;
      sumc=0.;
      sumc2=0.;
      sumt=0.;
      sumt2=0.;
      sumct=0.;
      sumid=0.;
      for(il=0;il<SCLRS;il++){//layer loop
        zco=zc[il];
        if(j==0){//x-proj
          tco=xtr[il];
          dis=xer[il];
        }
        else{
          tco=ytr[il];
          dis=yer[il];
        }
        dis=dis*dis;
        sumc+=(zco/dis);
        sumt+=(tco/dis);
        sumid+=(1/dis);
        sumct+=(zco*tco/dis);
        sumc2+=(zco*zco/dis);
        sumt2+=(tco*tco/dis);
      }
      t0[j]=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl[j]=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      scchi2[j]=sumt2+t0[j]*t0[j]*sumid+sl[j]*sl[j]*sumc2
         -2*t0[j]*sumt-2*sl[j]*sumct+2*t0[j]*sl[j]*sumc;
      scchi2[j]/=2.;
    }
    HF1(1218,geant(scchi2[0]),1.); 
    HF1(1219,geant(scchi2[1]),1.);
    if(scchi2[0]>4. || scchi2[1]>4.)bad=1; 
//
  }
  if(bad)return;// remove events with bad chi2 of the TOF-track
  TOFJobStat::addre(20);
//------
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);
    HF1(1217,cosc,1.);
    cosi=sqrt(1.+tgx*tgx+tgy*tgy);// this is 1/cos(theta) !!!
//
    if(TOFCAFFKEY.truse<=0){ // use track-length from TOF
      tld[0]=(zc[0]-zc[1])*cosi;//1->2
      tld[1]=(zc[0]-zc[2])*cosi;//1->3
      tld[2]=(zc[0]-zc[3])*cosi;//1->4
    }
    else{                    // use track-length from TRACKER
      tld[0]=trlen[0];//1->2
      tld[1]=trlen[1];//1->3
      tld[2]=trlen[2];//1->4
    }
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1524,geant(tld[1]-tld[2]+tld[0]),1.);//Trlen13-Trlen24
      if(TOFCAFFKEY.truse > 0){
        HF1(1215,(cost-cosc)/cost,1.);
        HF1(1216,cost,1.);
      }
    }
    tdi[0]=t1-t2;
    tdi[1]=t1-t3;
    tdi[2]=t1-t4;
//
    for(i=0;i<SCLRS;i++)ram[i]=exp(-arp1[i]/shft)+exp(-arp2[i]/shft);//old parametr.
//
    for(il=0;il<SCLRS;il++){// new parametrization (also ready for indiv.slopes)
      ib=brnl[il];
      scbrcal[il][ib].getslops(slops);
//      ramm[il]=(slops[0]/qsd1[il]+slops[1]/qsd2[il]);
      ramm[il]=(slops[0]/sqrt(qsd1[il])+slops[1]/sqrt(qsd2[il]));//works sl.better
    }
//
    ilay=0;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    if(TOFRECFFKEY.reprtf[2]>0){
      if(conf==1)HF2(1514,geant(ramm[ilay]),geant(relt),1.);
    }
//-----
//    for(i=0;i<3;i++)dum[i]=ram[i+1]-ram[0];// old parametrization
    for(i=0;i<3;i++)dum[i]=ramm[i+1]-ramm[0];// new parametrization
//
    if(TOFCAFFKEY.caltyp==0)betm=TOFCAFFKEY.bmeanpr;// tempor! better use measured one ?
    else betm=TOFCAFFKEY.bmeanmu;
//
    TOFTZSLcalib::fill(betm,brnl,tld,tdi,dum); // fill calib.working arrays
}
//=============================================================================
//
number TOFTDIFcalib::tdiff[SCBLMX][SCTDBM];
number TOFTDIFcalib::tdif2[SCBLMX][SCTDBM];
number TOFTDIFcalib::clong[SCBLMX][SCTDBM];
integer TOFTDIFcalib::nevnt[SCBLMX][SCTDBM];
integer TOFTDIFcalib::nbins[SCBTPN]={7,9,11,13,13};//#coord-bins vs bar-type(<=SCTDBM)
//                                                   to have bin width = 10-11cm
//--------------------------
void TOFTDIFcalib::init(){ // ----> initialization for TDIF-calibration
  int i,j,id,il,ib,ii,jj;
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  for(i=0;i<SCBLMX;i++){
    for(j=0;j<SCTDBM;j++){
      tdiff[i][j]=0.;
      tdif2[i][j]=0.;
      clong[i][j]=0.;
      nevnt[i][j]=0;
    }
  }
// ---> book histograms for "Bar raw time" (data quality check for TZSL-cal):
//  strcpy(inum,"0123456789");
//  for(il=0;il<SCLRS;il++){  
//    for(ib=0;ib<SCMXBR;ib++){
//      strcpy(htit1,"Bar raw time (4Lx1Bar events) for chan(LBB) ");
//      in[0]=inum[il+1];
//      strcat(htit1,in);
//      ii=(ib+1)/10;
//      jj=(ib+1)%10;
//      in[0]=inum[ii];
//      strcat(htit1,in);
//      in[0]=inum[jj];
//      strcat(htit1,in);
//      id=1720+il*SCMXBR+ib;
//      HBOOK1(id,htit1,80,40.,120.,0.);
//    }
//  }
//  HBOOK2(1780,"T vs 1/Q, L=1",50,0.,0.1,80,40.,120.,0.);
//  HBOOK2(1781,"T vs 1/Q, L=4",50,0.,0.1,80,40.,120.,0.);
// ---> book histograms for "Tdiff_mean vs Clong"
  strcpy(inum,"0123456789");
  for(il=0;il<SCLRS;il++){  
    for(ib=0;ib<SCMXBR;ib++){
      strcpy(htit1,"Mean time_difference(ns) vs coord(cm) for chan(LBB) ");
      in[0]=inum[il+1];
      strcat(htit1,in);
      ii=(ib+1)/10;
      jj=(ib+1)%10;
      in[0]=inum[ii];
      strcat(htit1,in);
      in[0]=inum[jj];
      strcat(htit1,in);
      id=1610+il*SCMXBR+ib;
      HBOOK1(id,htit1,SCTDBM,0.,geant(SCTDBM),0.);
    }
  }
  HBOOK2(1600,"L=1,B=7, Tdif vs Coord",35,-70.,70.,40,-5.,5.,0.);
  HBOOK2(1601,"L=2,B=7, Tdif vs Coord",35,-70.,70.,40,-5.,5.,0.);
  HBOOK1(1602,"Mean Tdiff (all layers/bars)",80,-0.4,0.4,0.);
  HBOOK1(1603,"Slope (all layers/bars)",80,0.02,0.1,0.);
  HBOOK1(1604,"Tdiff sigma in bin",50,0.,2.,0.);
}
//------------------------- 
void TOFTDIFcalib::select(){ // ------> event selection for TDIF-calibration
  integer i,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad,status;
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,crz;
  number sdtm[2],tmsd[SCLRS],tmsdc[SCLRS],ama[2],crc,t14,tmss[SCLRS];
  number am[2],time,qsd1[SCLRS],qsd2[SCLRS],q1,q2,qinv,tmin;
  AMSTOFRawCluster *ptr;
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
//----
  TOFJobStat::addre(17);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
  while (ptr){ // <--- loop over AMSTOFRawCluster hits
    status=ptr->getstatus();
    if(status==0){ //select only 2-sided "good_matching/history/strr" hits
      ilay=(ptr->getntof())-1;
      ibar=(ptr->getplane())-1;
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      ptr->gettovta(ama);// raw TovT (ns)
      scbrcal[ilay][ibar].ama2q(ama,am);// convert side-TovT to charge
      qsd1[ilay]=am[0];
      qsd2[ilay]=am[1];
      ptr->getsdtm(sdtm);// get raw side-times(ns)
      tmsd[ilay]=0.5*(sdtm[0]-sdtm[1]);// side time difference
      tmss[ilay]=0.5*(sdtm[0]+sdtm[1]);// side time sum
      scbrcal[ilay][ibar].td2ctd(tmsd[ilay],ama,tmsdc[ilay]);//use A-corrected times
//      tmsdc[ilay]=tmsd[ilay];// use raw side-times(running first time,when slop unknown)
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOFJobStat::addre(18);
  t14=tmsdc[0]-tmsdc[3];
//
//------> fill histogr. for indiv. bar time (quality check for TZSL-calibr):
//  for(il=0;il<SCLRS;il++){
//    ib=brnl[il];
//    time=tmss[il];
//    chan=SCMXBR*il+ib;
//    HF1(1720+chan,geant(time),1.);
//  }
  q1=qsd1[0];// layer=1
  q2=qsd2[0];
  time=tmss[0];
  ib=brnl[0];
  if(ib==7 && q1>0. && q2>0.){ // for bar=8
    qinv=1./q1+1./q2;
//    HF2(1780,geant(qinv),geant(time),1.);
  }
  q1=qsd1[3];// layer=4
  q2=qsd2[3];
  time=tmss[3];
  ib=brnl[3];
  if(ib==7 && q1>0. && q2>0.){ // for bar=8
    qinv=1./q1+1./q2;
//    HF2(1781,geant(qinv),geant(time),1.);
  }
//------> find track slope(in projection) using only scint.pos.info :
  ix=0;
  iy=0;
  for(il=0;il<SCLRS;il++){
    ib=brnl[il];
    zc[il]=TOFDBc::getzsc(il,ib);
    if(TOFDBc::plrotm(il)==0){// unrotated (X-meas) planes
      x[ix]=TOFDBc::gettsc(il,ib);
      zx[ix]=zc[il];
      ix+=1;
    }
    else{                    // rotated (Y-meas) planes
      y[iy]=TOFDBc::gettsc(il,ib);
      zy[iy]=zc[il];
      iy+=1;
    }
  }
  tgx=(x[0]-x[1])/(zx[0]-zx[1]);
  tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
//-----> find track crossing points (long.coord):
  for(il=0;il<SCLRS;il++){
    ib=brnl[il];
    crz=TOFDBc::getzsc(il,ib);
    if(TOFDBc::plrotm(il)==0){// unrotated (X-meas) planes
      crc=y[1]+tgy*(crz-zy[1]);// long. crossing coord. in abs. ref.syst.
    }
    else{                     // rotated (Y-meas) planes
      crc=x[1]+tgx*(crz-zx[1]);// long. crossing coord. in abs. ref.syst.
    }
    TOFTDIFcalib::fill(il,ib,tmsdc[il],crc);
  }
}
//------------------------- 
void TOFTDIFcalib::fill(integer il,integer ib, number td, number co){//--->fill arrays
  integer chan,nbin,btyp;
  number bin,hlen,col;
//
  hlen=0.5*TOFDBc::brlen(il,ib);
  btyp=TOFDBc::brtype(il,ib);//1-5 !!!
  bin=2*hlen/nbins[btyp-1];
// convert abs.coord. to local one:
  if(TOFDBc::plrotm(il)==0){// unrotated plane
    col=co;// local Y-coo is abs. Y-coo for unrot.planes(tempor. neglect by shifts !!!)
  }
  else{
    col=-co;// local Y-coo is opposit to abs. X-coo for rotated planes
  }
  if((col<-hlen) || (col>=hlen))return;// out of range
  nbin=(col+hlen)/bin;
  chan=SCMXBR*il+ib;
  nevnt[chan][nbin]+=1;
  clong[chan][nbin]+=col;
  tdiff[chan][nbin]+=td;
  tdif2[chan][nbin]+=(td*td);
  if(il==0 && ib==6)HF2(1600,col,td,1.);
  if(il==1 && ib==6)HF2(1601,col,td,1.);
} 
//------------------------- 
void TOFTDIFcalib::fit(){//---> get the slope,td0,chi2
  integer il,ib,chan,nb,btyp,nev,bins;
  number bin,len,co,t,dis,sig,sli,meansl(0),bintot(0),speedl;
  number sl[SCBLMX],t0[SCBLMX],sumc,sumc2,sumt,sumt2,sumct,sumid,chi2[SCBLMX];
  geant td[SCTDBM];
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create name for output file
// 
  strcpy(inum,"0123456789");
  cfvn=TOFCAFFKEY.cfvers%100;
  strcpy(fname,"tdcalib");
  dig=cfvn/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".dat");
//
//--> get run/time of the first event
//
  StartRun=AMSTOFRawEvent::getsrun();
  StartTime=AMSTOFRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
  HPRINT(1600);
  HPRINT(1601);
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      len=TOFDBc::brlen(il,ib);
      btyp=TOFDBc::brtype(il,ib);//1-5 !!!
      bin=len/nbins[btyp-1];
      chan=SCMXBR*il+ib;
      sumc=0;
      sumt=0;
      sumct=0;
      sumc2=0;
      sumt2=0;
      sumid=0;
      bins=0;
      sl[chan]=0;
      t0[chan]=0;
      chi2[chan]=0;
   cout<<"Events/bin in layer/bar="<<(il+1)<<" "<<(ib+1)<<"  ===> ";
      for(nb=0;nb<SCTDBM;nb++){
        td[nb]=0;
        nev=nevnt[chan][nb];
   cout<<" "<<nev;
        if(nev>=20){//min.cut on event number in bin
          t=tdiff[chan][nb]/number(nev);// mean td
          tdiff[chan][nb]=t;
          td[nb]=geant(t);
          tdif2[chan][nb]/=number(nev);
          dis=tdif2[chan][nb]-(t*t);
          if(dis>=0.)HF1(1604,geant(sqrt(dis)),1.);
          if(dis>=0. && sqrt(dis)<=0.56){//max.cut on bin-rms
            dis=dis/(nev-1);
            tdif2[chan][nb]=dis;//now store sigmas**2 of mean-values
            co=clong[chan][nb]/number(nev);// mean co
            clong[chan][nb]=co;
            sumc+=(co/dis);
            sumt+=(t/dis);
            sumid+=(1/dis);
            sumct+=(co*t/dis);
            sumc2+=(co*co/dis);
            sumt2+=(t*t/dis);
            bins+=1;
          }
        }
      }// ---> end of bins loop
   cout<<endl;
      HPAK(1610+chan,td);
      if(bins>=4){
        t0[chan]=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
        sl[chan]=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
        chi2[chan]=sumt2+t0[chan]*t0[chan]*sumid+sl[chan]*sl[chan]*sumc2
         -2*t0[chan]*sumt-2*sl[chan]*sumct+2*t0[chan]*sl[chan]*sumc;
        chi2[chan]/=number(bins-2);
        if(chi2[chan]<6. &&
                     fabs(sl[chan])>0.058 &&
                     fabs(sl[chan])<0.071){//only good for averaging
          bintot+=1;
          meansl+=sl[chan];
        }
        HF1(1602,geant(t0[chan]),1.);
        if(fabs(sl[chan])>0.)HF1(1603,geant(fabs(sl[chan])),1.);
      }
    }
  }
  if(bintot>0)meansl/=bintot; // mean slope
  if(meansl!=0)speedl=fabs(1/meansl);// mean light speed
//
//---> print Td vs Coo histograms:
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      chan=SCMXBR*il+ib;
      if(chi2[chan]>0)HPRINT(1610+chan);
    }
  }
//---  
  cout<<endl<<endl;
  cout<<"TOFTDIFcalib::fit: for bar 1-14  Tdiff@center (ns):"<<endl<<endl;
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      chan=SCMXBR*il+ib;
      cout<<" "<<t0[chan];
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TOFTDIFcalib::fit: for bar 1-14  Light speed (cm/ns):"<<endl<<endl;
  cout<<"            Mean light speed = "<<speedl<<endl<<endl;
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      chan=SCMXBR*il+ib;
      sli=0;
      if(sl[chan]!=0)sli=1/sl[chan];
      cout<<" "<<sli;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TOFTDIFcalib::fit: for bar 1-14  Chi2 :"<<endl<<endl;
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      chan=SCMXBR*il+ib;
      cout<<" "<<chi2[chan];
    }
    cout<<endl;
  }
  cout<<endl;
//
  HPRINT(1602);
  HPRINT(1603);
  HPRINT(1604);
//
//---> print T vs 1/Q histograms:
//  for(il=0;il<SCLRS;il++){
//    for(ib=0;ib<SCMXBR;ib++){
//      chan=SCMXBR*il+ib;
//      HPRINT(1720+chan);
//    }
//  }
//  HPRINT(1780);
//  HPRINT(1781);
//
// ---> write mean light speed and Tdif's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOFTDIFcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for TDLV-calibration output, fname:"<<fname<<'\n';
  cout<<"Lspeed and individual Tdif's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
  tcfile.setf(ios::fixed);
  tcfile.width(6);
  tcfile.precision(2);// precision for Lspeed and Tdiff's
  tcfile << geant(speedl)<<endl;
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      chan=SCMXBR*il+ib;
      tcfile << geant(t0[chan])<<" ";
    }
    tcfile << endl;
  }
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOFTDLVcalib:output file closed !"<<endl;
}
//=============================================================================
//
number TOFAMPLcalib::ambin[SCBTBN][SCACMX];// tot-signals for each ref_bar/bin/event
integer TOFAMPLcalib::nevenb[SCBTBN];// numb.of events accum. per ref_bar/bin
number TOFAMPLcalib::amchan[SCCHMX][SCACMX];// side-signals for each channel/event
integer TOFAMPLcalib::nevenc[SCCHMX];// numb.of events accum. per channel
geant TOFAMPLcalib::gains[SCCHMX];//trunc. mean ch.signals ("0" impact) relat. to ref.ones
geant TOFAMPLcalib::btamp[SCBTBN];// trunc. mean. bar-signals for each bin(ref.bars) 
integer TOFAMPLcalib::rbls[SCBTPN];//ref.bar id's (for bar types 1-5)
geant TOFAMPLcalib::profb[SCBTPN][SCPRBM];// bin width for each bar type
geant TOFAMPLcalib::profp[SCBTPN][SCPRBM];// middle of the bin .........
integer TOFAMPLcalib::nprbn[SCBTPN]={10,12,13,14,14};//profile-bins vs bar-type
number TOFAMPLcalib::a2dr[SCCHMX];// sum of a/d signal-ratios for each channel
number TOFAMPLcalib::a2dr2[SCCHMX];// sum of a/d (signal-ratios)**2 
integer TOFAMPLcalib::nevenr[SCCHMX];// number of accumulated events for above sums
number TOFAMPLcalib::ammrfc[SCBTPN];//sum for mean charge calculation (ref.c.,"0"-bin)
integer TOFAMPLcalib::nevrfc[SCBTPN];//events in above sum
geant TOFAMPLcalib::fpar[10]={0,0,0,0,0,0,0,0,0,0};// parameters to fit
integer TOFAMPLcalib::nbinr;//working arrays for A-profile fit
number TOFAMPLcalib::mcharge[SCPRBM];
number TOFAMPLcalib::mcoord[SCPRBM];
number TOFAMPLcalib::aprofp[SCBTPN][SCPROFP];// A-profile parameters(ref.bars)
number TOFAMPLcalib::clent;
integer TOFAMPLcalib::cbtyp;//bar-type
//--------------------------
void TOFAMPLcalib::init(){ // ----> initialization for AMPL-calibration 
  integer i,j,il,ib,ii,jj,id,nadd,nbnr;
  geant blen,dd,bw,bl,bh;
  integer stbns(4);// number of standard bins
  geant bwid[4]={4.,5.,7.,9.};// bin width (first "stbns" standard bins) 
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  if(TOFCAFFKEY.truse < 0){
    cout<<"TOFAMPLcalib::init: Only TOF info will be used !!!"<<endl;
  }
  else {
    cout<<"TOFAMPLcalib::init: Tracker info will be used !!!"<<endl;
  }
  strcpy(inum,"0123456789");
  for(i=0;i<SCBTPN;i++)rbls[i]=TOFCAFFKEY.refbid[i];
//
  for(i=0;i<SCCHMX;i++){
    nevenc[i]=0;
    gains[i]=0.;
    nevenr[i]=0;
    a2dr[i]=0.;
    a2dr2[i]=0.;
  for(j=0;j<SCACMX;j++){
    amchan[i][j]=0.;
  }
  }
//
  for(i=0;i<SCBTPN;i++){
    nevrfc[i]=0;
    ammrfc[i]=0;
  }
//
  for(i=0;i<SCBTBN;i++){
    nevenb[i]=0;
    btamp[i]=0.;
  for(j=0;j<SCACMX;j++){
    ambin[i][j]=0.;
  }
  }
//
//  ---> book hist. for side-signals:
//
  for(il=0;il<SCLRS;il++){   
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        strcpy(htit1,"Center signals distr. for chan(LBBS) ");
        in[0]=inum[il+1];
        strcat(htit1,in);
        ii=(ib+1)/10;
        jj=(ib+1)%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        in[0]=inum[i+1];
        strcat(htit1,in);
        id=1600+il*SCMXBR*2+ib*2+i;
//        HBOOK1(id,htit1,80,0.,480.,0.);
      }
    }
  }
// ---> book hist. for ref.bars bin-signals (profiles):
  for(i=0;i<SCBTPN;i++){ // loop over bar types  
    id=rbls[i];
    il=id/100;
    in[0]=inum[il];
    strcpy(htit3,in);
    ib=id%100;
    ii=ib/10;
    in[0]=inum[ii];
    strcat(htit3,in);
    jj=ib%10;
    in[0]=inum[jj];
    strcat(htit3,in);
    strcat(htit3,"/");
    nbnr=nprbn[i];//real numb. of bins
    for(j=0;j<nbnr;j++){ // loop over longit.bins
      strcpy(htit2,"Bin signals distribution for bar(LBB)/bin(NN) ");
      strcat(htit2,htit3);
      ii=(j+1)/10;
      jj=(j+1)%10;
      in[0]=inum[ii];
      strcat(htit2,in);
      in[0]=inum[jj];
      strcat(htit2,in);
      id=1601+SCLRS*SCMXBR*2+i*SCPRBM+j;
//      HBOOK1(id,htit2,80,0.,960.,0.);
    }
  }
//
  for(i=0;i<SCBTPN;i++){ // prepare profile bin width array
    id=rbls[i];
    il=id/100-1;
    ib=id%100-1;
    blen=TOFDBc::brlen(il,ib);
    nbnr=nprbn[i];//real numb. of bins
    dd=0.;
    for(j=0;j<stbns;j++){ // bin width for the first/last "stbns" standard ones
      dd+=bwid[j];
      profb[i][j]=bwid[j];
      profb[i][nbnr-j-1]=bwid[j];
    }
    nadd=nbnr-2*stbns;// number of additional bins of THE SAME WIDTH !
    bw=(blen-2*dd)/nadd;// width of additional bins
    for(j=0;j<nadd;j++){ // complete bin width array
      profb[i][stbns+j]=bw;
    }
  }
//
  for(i=0;i<SCBTPN;i++){ // prepare mid.points positions (loc.r.s) of profile bins
    id=rbls[i];
    il=id/100-1;
    ib=id%100-1;
    blen=TOFDBc::brlen(il,ib);
    nbnr=nprbn[i];//real numb. of bins
    dd=0.;
    for(j=0;j<nbnr;j++){
      profp[i][j]=dd+0.5*profb[i][j]-0.5*blen;// -blen/2 to go to loc.r.s.
      dd+=profb[i][j];
    }
  }
//
  for(i=0;i<SCBTPN;i++){// book hist. for longit.imp.point distr.(ref.bar only)
    strcpy(htit1,"Impact point distr.for ref.bar(LBB) ");
    id=rbls[i];
    il=id/100;
    in[0]=inum[il];
    strcat(htit1,in);
    ib=id%100;
    ii=ib/10;
    in[0]=inum[ii];
    strcat(htit1,in);
    jj=ib%10;
    in[0]=inum[jj];
    strcat(htit1,in);
    blen=TOFDBc::brlen(il-1,ib-1);
    bl=-0.5*blen;
    bh=bl+blen;
    HBOOK1(1220+i,htit1,70,bl,bh,0.);
    aprofp[i][0]=120.;//defaults param. for A-profile fit
    aprofp[i][1]=120.;
    aprofp[i][2]=0.28;
    aprofp[i][3]=195.;
    aprofp[i][4]=10.4;
  }
//
}
//--------------------------------------
//
void TOFAMPLcalib::select(){ // ------> event selection for AMPL-calibration
  integer i,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad,status,sector;
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number ama[2],amd[2],coo[SCLRS],coot[SCLRS],cstr[SCLRS],dx,dy;
  number am1[SCLRS],am2[SCLRS],am1d[SCLRS],am2d[SCLRS],am[2],eanti(0);
  geant ainp[2],dinp[2],cinp,trlen[SCLRS];
  number ltim[4],tdif[4],trle[4],fpnt,bci,sut,sul,sul2,sutl,tzer,chsq,betof;
  number sigt(0.121),cvel(29.979);// time meas.accuracy and light velocity 
  number eacut=0.6;// cut on E-anti (mev)
  number dscut=5.6;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  AMSTOFRawCluster *ptr;
  AMSAntiCluster *ptra;
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  TOFJobStat::addre(11);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
  while(ptr){ // <--- loop over AMSTOFRawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    ptr->gettovta(ama);// get raw anode-ampl(TovT in ns)
    ptr->gettovtd(amd);// get raw dinode-ampl(TovT in ns)
    scbrcal[ilay][ibar].ama2q(ama,am);// convert to charge
    am1[ilay]=am[0];
    am2[ilay]=am[1];
    if(status>=0 && TOFCAFFKEY.dynflg==0){//for dyn-calibr.(old,not Contin's)
      scbrcal[ilay][ibar].amd2q(amd,am);// convert to charge
      am1d[ilay]=am[0];
      am2d[ilay]=am[1];
      ainp[0]=geant(am1[ilay]);
      ainp[1]=geant(am2[ilay]);
      dinp[0]=geant(am1d[ilay]);
      dinp[1]=geant(am2d[ilay]);
      TOFAMPLcalib::filla2d(ilay,ibar,ainp,dinp);
    }
    if(status==0){ //select only 2-sided "good_matching/history/..." bars
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
      ltim[ilay]=ptr->gettime();// get ampl-corrected time
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
  HF1(1503,geant(eanti),1.);
  if(eanti>eacut)return;// remove events with big signal in Anti
  TOFJobStat::addre(13);
//
//
//------> get parameters from tracker:
//
    static number pmas(0.938),mumas(0.1057),imass;
    number pmom,mom,bet,chi2,betm,pcut[2];
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
    if(ntrk!=1 && TOFCAFFKEY.truse>=0)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();//get pointer of the track, used in given particle
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
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
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1500,geant(pmom),1.);
      HF1(1501,bet,1.);
    }
//
    bad=0;
    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
    if(TOFCAFFKEY.truse>=0 && bad==1)return;
    TOFJobStat::addre(14);
//
    bad=0;
    if(TOFCAFFKEY.truse==1){
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
        trlen[il]=trl;
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
      cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);// average cos from track
    }
//
    if(bad)return;//too big difference of TOF-Tracker coord.
    TOFJobStat::addre(15);
//
//-------> get track impact angle using scint-made long.coord :
//
    ix=0;
    iy=0;                                
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      if(TOFDBc::plrotm(il)==0){// unrotated (Long->Yabs) planes
        y[iy]=coo[il];
        zy[iy]=zc[il];
        iy+=1;
      }
      else{                    // rotated (Long->-Xabs) planes
        x[ix]=-coo[il];
        zx[ix]=zc[il];
        ix+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);
    HF1(1217,cosc,1.);
//
//--------> find beta from TOF :
//
    trle[0]=0;
    trle[1]=(zc[0]-zc[1])/cosc;
    trle[2]=(zc[0]-zc[2])/cosc;
    trle[3]=(zc[0]-zc[3])/cosc;
    tdif[0]=0;
    tdif[1]=ltim[0]-ltim[1];
    tdif[2]=ltim[0]-ltim[2];
    tdif[3]=ltim[0]-ltim[3];
    fpnt=0;
    sul=0;
    sut=0;
    sutl=0;
    sul2=0;
    for(il=0;il<SCLRS;il++){
      fpnt+=1;
      sut+=tdif[il];
      sul+=trle[il];
      sul2+=(trle[il]*trle[il]);
      sutl+=(tdif[il]*trle[il]);
    }
    bci=(fpnt*sutl-sut*sul)/(fpnt*sul2-sul*sul);
    tzer=(sut-bci*sul)/fpnt;
    chsq=0;
    for(il=0;il<SCLRS;il++)chsq+=pow((tzer+bci*trle[il]-tdif[il]),2);
    chsq/=pow(sigt,2);
    chsq/=number(fpnt-2);
    betof=1./bci/cvel;
    if(chsq>8.)betof=-1.;//cut on chi2
    if(TOFRECFFKEY.reprtf[2]>0){
      if(chsq<=8.)HF1(1502,betof,1.);
      HF1(1205,chsq,1.);
      HF1(1206,tzer,1.);
    }
//
// ---> look at mass :
//
    number betg(0.000001);
    geant mass(-1.);
    if(betof<1. && betof>=0.)betg=betof/sqrt(1.-betof*betof);
    if(TOFCAFFKEY.truse == 1){
      if(betof<1. && betof>=0.)mass=geant(pmom/betg);
      if(TOFRECFFKEY.reprtf[2]>0){
        HF1(1204,mass,1.);
        if(betg>TOFCAFFKEY.bgcut[0] && betg<TOFCAFFKEY.bgcut[1])HF1(1207,mass,1.);
        HF1(1215,(cost-cosc)/cost,1.);
        HF1(1216,cost,1.);
        HF2(1218,geant(pmom),geant(betof),1.);
      }
    }
//
// ------> normalize charge to normal incidence :
//
    for(il=0;il<SCLRS;il++){
      if(TOFCAFFKEY.truse == 1){
        am1[il]*=fabs(cstr[il]);
        am2[il]*=fabs(cstr[il]);
      }
      else{
        am1[il]*=fabs(cosc);
        am2[il]*=fabs(cosc);
      }
    }
// ------> fill working arrays and histograms :
//
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      ainp[0]=geant(am1[il]);
      ainp[1]=geant(am2[il]);
      if(TOFCAFFKEY.truse == 1){
        cinp=coot[il];// loc.r.s.!!!
        TOFAMPLcalib::fill(il,ib,ainp,cinp);
        TOFAMPLcalib::fillabs(il,ib,ainp,cinp,pmom,betof);
      }
      else{
        cinp=coo[il];// loc.r.s.!!!
        TOFAMPLcalib::fill(il,ib,ainp,cinp);
        mom=imass*betg;// fictitious mom. to give implied mass
        TOFAMPLcalib::fillabs(il,ib,ainp,cinp,mom,betof);
      }
    }
}
//--------------------------------------
//            ---> program to accumulate data for AMPL-calibration:
void TOFAMPLcalib::fill(integer il, integer ib, geant am[2], geant coo){
//
  integer i,id,idr,idh,ibt,btyp,nbn,nb,nbc,isb,chan,nev,bchan;
  geant r;
  geant bl,bh,qthrd;
  static geant cbin(15.);// centr. bin half-width for gain calibr.
//
  chan=2*SCMXBR*il+2*ib;
  isb=SCMXBR*il+ib;
  ibt=TOFDBc::brtype(il,ib);// bar type (1-5)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
  nbn=nprbn[btyp];// number of long.bins
//
  nbc=-1;
  for(nb=0;nb<nbn;nb++){ // define bin number for current coo
    bl=profp[btyp][nb]-0.5*profb[btyp][nb];
    bh=bl+profb[btyp][nb];
    if(coo>=bl && coo<bh)nbc=nb;
  }
  if(nbc<0){
    bl=profp[btyp][0]-0.5*profb[btyp][0];
    bh=profp[btyp][nbn-1]+0.5*profb[btyp][nbn-1];
    if((bl-coo)>4. || (coo-bh)>4.)
//    cerr<<"TOFAMPLcalib::fill:out_of_range !, coo="<<coo<<" l/h="<<bl<<" "<<bh<<endl;
    return;
  }
  bchan=SCPRBM*btyp+nbc;
//                             ---> fill arrays/hist. for gains:
//
  if(fabs(coo) < cbin){// select only central incidence(+- cbin cm)
    nev=nevenc[chan];
    if(nev<SCACMX){  
      amchan[chan][nev]=am[0];
      nevenc[chan]+=1;
    }
    nev=nevenc[chan+1];
    if(nev<SCACMX){  
      amchan[chan+1][nev]=am[1];
      nevenc[chan+1]+=1;
    }
    idh=1600+chan+0;// side-1
//    HF1(idh,geant(am[0]),1.);
    idh=1600+chan+1;// side-2
//    HF1(idh,geant(am[1]),1.);
  }
//                             ---> fill profile arrays/hist. for ref. bars:
  if(id == idr){// only for ref. sc. bars
    nev=nevenb[bchan];
    if(nev<SCACMX){
      ambin[bchan][nev]=(am[0]+am[1]);
      nevenb[bchan]+=1;
    }
    HF1(1219+ibt,coo,1.);// longit.imp.point distribution
    if(ibt == 5 && fabs(coo) < cbin){
      HF1(1250,am[0],1.);// Q-distr. for ref.bar type=5, s=1
      HF1(1251,am[1],1.);// Q-distr. for ref.bar type=5, s=2
    }
    idh=1601+SCLRS*SCMXBR*2+(ibt-1)*SCPRBM+nbc;
//    HF1(idh,geant(am[0]+am[1]),1.);
  }
}
//--------------------------------------
//            ---> program to accumulate data for abs.normalization:
void TOFAMPLcalib::fillabs(integer il, integer ib, geant am[2], geant coo,
                                     number mom, number btof){
//
  integer i,id,idr,ibt,btyp,mflg(0),bgflg(0);
  static geant cbin(15.);// centr. bin half-width for gain calibr.
  static geant prcut[2]={0.5,1.5};
  static geant mucut[2]={0.,0.5};
  number betg(0.000001);
  geant amt,mass(-1),mcut[2];
//
  ibt=TOFDBc::brtype(il,ib);// bar type (1-5)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
//
  if(id != idr)return;//only for ref.counters
  if(fabs(coo) > cbin)return;// select only central incidence(+- cbin cm)
//
  if(btof<1. && btof>=0.){
    betg=btof/sqrt(1-btof*btof);
    mass=geant(mom/betg);
  }
//  
  amt=(am[0]+am[1]);
//
  if(TOFCAFFKEY.caltyp==0){// space (prot) calibr.
    mcut[0]=prcut[0];
    mcut[1]=prcut[1];
  }
  else{                    // earth (mu) calibr.
    mcut[0]=mucut[0];
    mcut[1]=mucut[1];
  }
  if(mass>mcut[0] && mass<mcut[1])mflg=1;
  if(betg>TOFCAFFKEY.bgcut[0] && betg<TOFCAFFKEY.bgcut[1])bgflg=1;
//
  if(id==rbls[4])HF2(1219,geant(log10(betg)),amt,1.);// for ref.bar type=5
  if(bgflg==1 && mflg==1){// mass-mip region ok
    nevrfc[btyp]+=1;
    ammrfc[btyp]+=amt;
  }
}
//
//-------------------------------------------
//            ---> program to fill a2d-arrays:
void TOFAMPLcalib::filla2d(integer il, integer ib, geant am[2], geant amd[2]){
//
  integer i,id,idr,ibt,btyp,chan;
  geant r,qthrd,q0,tauf,t0;
//
  chan=2*SCMXBR*il+2*ib;
  ibt=TOFDBc::brtype(il,ib);// bar type (1-5)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
//
//                             ---> fill arrays for anode/dinode ratios:
  for(i=0;i<2;i++){
    r=0;
    if(amd[i]>0){
      r=am[i]/amd[i];
      if(id==idr && ibt==5 && r>0){
//      HF2(1248,am[i],amd[i],1.);
        HF2(1249,am[i],r,1.);
      }
      tauf=scbrcal[il][ib].getdipar(i,0);
      t0=scbrcal[il][ib].getdipar(i,1);
      q0=scbrcal[il][ib].getdipar(i,2);// D-integrator parameters
      qthrd=q0+exp(t0/tauf);// eff.threshold for given D-channel
      if(amd[i]>(2.5*qthrd)){//50% above the D-thershold (to avoid possible nonlinearity)
        a2dr[chan+i]+=r;
        a2dr2[chan+i]+=(r*r);
        nevenr[chan+i]+=1;
      }
    }
  }
}
//--------------------------------------
//            ---> program to get final AMPL-calibration:
void TOFAMPLcalib::fit(){
//
  integer il,ib,i,j,k,id,idr,btyp,nbnr,chan,bchan,nev,nm;
  geant aref[SCBTPN][2],ar,aabs[SCBTPN],mip2q[SCBTPN];
  number *pntr[SCACMX];
  number aver;
  geant step[10],pmin[10],pmax[10],sigp[10];
  integer nev1,nev2,npar=2;
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create name for output file
// 
  strcpy(inum,"0123456789");
  cfvn=TOFCAFFKEY.cfvers%100;
  strcpy(fname,"ancalib");
  dig=cfvn/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".dat");
//
//--> get run/time of the first event
//
  StartRun=AMSTOFRawEvent::getsrun();
  StartTime=AMSTOFRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
// ---> print "gain"-hist. (side-signals for center bin)
  for(il=0;il<SCLRS;il++){   
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        id=1600+il*SCMXBR*2+ib*2+i;
//        HPRINT(id);
      }
    }
  }
//
// ---> print impact point hist. for ref. counters:
//
  for(i=0;i<SCBTPN;i++){
    HPRINT(1220+i);
  }
//
// ---> print "profile"-hist. (tot.signal vs long.bin for ref.counters):
//
  for(i=0;i<SCBTPN;i++){ // loop over bar types  
    nbnr=nprbn[i];//real numb. of bins
    for(j=0;j<nbnr;j++){ // loop over longit.bins
      id=1601+SCLRS*SCMXBR*2+i*SCPRBM+j;
//      HPRINT(id);
    }
  }
// ---> prepare truncated mean ampl. for each channel:
//
  for(i=0;i<SCCHMX;i++){
    nev=nevenc[i];
    aver=0;
    if(nev>=TOFCAFFKEY.minev){
      for(k=0;k<nev;k++)pntr[k]=&amchan[i][k];//pointers to event-signals of chan=i 
      AMSsortNAG(pntr,nev);//sort in increasing order
      nm=floor(nev*TOFCAFFKEY.trcut);// to keep (100*trcut)% of lowest amplitudes
      for(j=0;j<nm;j++)aver+=(*pntr[j]);
      if(nm>0)gains[i]=geant(aver/nm);
    }
  }
//
// ---> extract trunc.mean ampl for ref.bar:
//
  for(i=0;i<SCBTPN;i++){
    id=rbls[i];
    il=id/100-1;
    ib=id%100-1;
    chan=2*SCMXBR*il+2*ib;
    nev1=nevenc[chan];
    nev2=nevenc[chan+1];
    if(nev1>=(2*TOFCAFFKEY.minev) && nev2>=(2*TOFCAFFKEY.minev)){
      aref[i][0]=gains[chan];
      aref[i][1]=gains[chan+1];
    }
    else{
      aref[i][0]=0;
      aref[i][1]=0;
    }
  }
// ---> calc. relative gains:
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      btyp=TOFDBc::brtype(il,ib)-1;
      id=100*(il+1)+ib+1;
      idr=rbls[btyp];// ref.bar id for given bar
      chan=2*SCMXBR*il+2*ib;
      for(i=0;i<2;i++){
        ar=aref[btyp][i];
        if(ar>0. && gains[chan+i]>0.)gains[chan+i]/=ar;
        else gains[chan+i]=1.;//default value
        if(id != idr)HF1(1252,gains[chan+i],1.);
      }
    }
  }
// ---> prepare truncated mean ampl. for each bin of ref.bars:
//
  integer j1,j2;
  for(i=0;i<SCBTPN;i++){//loop over bar-types
    nbnr=nprbn[i];//real numb. of bins
    for(j=0;j<nbnr;j++){ // loop over longit.bins
      bchan=i*SCPRBM+j;
      nev=nevenb[bchan];
      aver=0;
      if(nev>=TOFCAFFKEY.minev){
        for(k=0;k<nev;k++)pntr[k]=&ambin[bchan][k];//pointers to event-signals of chan=bchan
        AMSsortNAG(pntr,nev);//sort in increasing order
        nm=floor(nev*TOFCAFFKEY.trcut);// to keep (100*trcut)% of lowest amplitudes
        for(k=0;k<nm;k++)aver+=(*pntr[k]);
        if(nm>0)btamp[bchan]=geant(aver/nm);
      }
      HF1(1254+i,profp[i][j],btamp[bchan]);
    }
  }
//---> fit A-profiles for ref. bars:
  int ier;  
  int ifit[SCPROFP];
  char pnam[SCPROFP][6],pnm[6];
  number argl[10];
  int iargl[10];
  number start[SCPROFP],pstep[SCPROFP],plow[SCPROFP],phigh[SCPROFP];
  strcpy(pnam[0],"aleft");
  strcpy(pnam[1],"arigh");
  strcpy(pnam[2],"amixt");
  strcpy(pnam[3],"lenhi");
  strcpy(pnam[4],"lenlo");
  start[0]=120.;
  start[1]=120.;
  start[2]=0.28;
  start[3]=195.;
  start[4]=10.4;
  pstep[0]=10.;
  pstep[1]=10.;
  pstep[2]=0.1;
  pstep[3]=10.;
  pstep[4]=1.;
  plow[0]=1.;
  plow[1]=1.;
  plow[2]=0.01;
  plow[3]=10.;
  plow[4]=1.;
  phigh[0]=500.;
  phigh[1]=500.;
  phigh[2]=10.;
  phigh[3]=500.;
  phigh[4]=150.;
  for(i=0;i<SCPROFP;i++)ifit[1]=1;
  ifit[3]=0;
  ifit[4]=0;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("Pos.correction-calibration for TOF-system");
  for(i=0;i<SCPROFP;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"TOF-calib: Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfun,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"TOF-calib: Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
//----
  for(i=0;i<SCBTPN;i++){//loop over bar-types
    nbnr=nprbn[i];//numb. of bins for given bar type
    id=rbls[i];
    il=id/100-1;
    ib=id%100-1;
    clent=TOFDBc::brlen(il,ib);
    cbtyp=i;
    cout<<"Start minim. for btyp/length="<<i<<" "<<clent<<" nbins="<<nbnr<<endl;
    nbinr=0;
    for(j=0;j<nbnr;j++){ // loop over longit.bins to load coo,charge..
      bchan=i*SCPRBM+j;
      nev=nevenb[bchan];
     cout<<"events/bin="<<nev<<endl;
      if(nev>=TOFCAFFKEY.minev){
        mcharge[nbinr]=btamp[bchan];
        mcoord[nbinr]=profp[i][j];
        cout<<"q/c="<<mcharge[nbinr]<<" "<<mcoord[nbinr]<<endl;
        nbinr+=1;
      }
    }
    cout<<"Good statistics bins:"<<nbinr<<endl;
    if(nbinr<5)continue;//too few bins, skip this bar-type
    argl[0]=0.;
    ier=0;
    MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
    if(ier!=0){
      cout<<"TOF-calib: MINIMIZE problem !"<<'\n';
      continue;
    }  
    MNEXCM(mfun,"MINOS",argl,0,ier,0);
    if(ier!=0){
      cout<<"TOF-calib: MINOS problem !"<<'\n';
      continue;
    }
    argl[0]=number(3);
    ier=0;
    MNEXCM(TOFAMPLcalib::mfun,"CALL FCN",argl,1,ier,0);
    if(ier!=0){
      cout<<"TOF-calib: final CALL_FCN problem !"<<'\n';
      continue;
    }
  }//---> end of btype loop
//-----------------------------------------------
//
//----> calc. mean charge for ref.counters (for abs.calibr):
//
  geant elref(2.);// ref. Elos(mev,norm.incidence) for mip-region
  for(i=0;i<SCBTPN;i++){
    aabs[i]=0.;
    mip2q[i]=100.;//default valie
    nev=nevrfc[i];
    if(nev>=TOFCAFFKEY.minev){
      aabs[i]=geant(ammrfc[i]/nev);
      mip2q[i]=aabs[i]/elref;//(pC/mev)
    }
  }
//----------------------------------------------
//
// ---> calculate/print mean a/d ratios and errors (old dyn-calibr):
//
  geant a2d[SCCHMX],ea2d[SCCHMX],mval,mval2,err,r;
//
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        chan=2*SCMXBR*il+2*ib+i;
        a2d[chan]=5.;//just default value
      }
    }
  }
  if(TOFCAFFKEY.dynflg==0){// <--- stand(old) dyn.calibr.
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        chan=2*SCMXBR*il+2*ib+i;
        nev=nevenr[chan];
        mval=0;
        err=-1.;
        if(nev>=TOFCAFFKEY.minev){
          mval=geant(a2dr[chan]/nev);
          mval2=geant(a2dr2[chan]/nev);
          r=mval2-mval*mval;
          if(r>=0)err=sqrt(r/nev);
        }
        a2d[chan]=mval;
        ea2d[chan]=err;
        HF1(1259,mval,1.);
        HF1(1260,err,1.);
      }
    }
  }
  printf("\n");
  printf(" ===============>  Anode_to_dinode ratios:\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6d",nevenr[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6d",nevenr[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.3f",ea2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.3f",ea2d[chan]);
    }
    printf("\n");
    printf("\n");
  }
  }
//--------------------------------------------------
//
  printf("\n");
  printf(" ===============>  Relative gains :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    printf("\n");
  }
//
// ---------
  cout<<" Absolute calibration results :"<<endl;
  cout<<endl;
  cout<<"Events in ref.bars :"<<endl;
  cout<<nevrfc[0]<<" "<<nevrfc[1]<<" "<<nevrfc[2]<<" "<<nevrfc[3]<<" "<<nevrfc[4]<<endl;
  cout<<"Mean charge in ref.bars (centr.bin):"<<endl;
  cout<<aabs[0]<<" "<<aabs[1]<<" "<<aabs[2]<<" "<<aabs[3]<<" "<<aabs[4]<<endl;
  cout<<"Convertion factors mip2q (pC/MeV):"<<endl;
  cout<<mip2q[0]<<" "<<mip2q[1]<<" "<<mip2q[2]<<" "<<mip2q[3]<<" "<<mip2q[4]<<endl;
  cout<<endl;
// --------- 
//
// ---> write a2d-ratios(or slop/offs), gains and mip2q's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOFAMPLcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for AMPL-calibration output, fname:"<<fname<<'\n';
  cout<<"Indiv. channel a2dr/gain-param., mip2q's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
//
  tcfile.setf(ios::fixed);
  tcfile.width(6);
//                         <-- for standard(old) dynode calibr.
    tcfile.precision(2);// precision for a2dr
    for(il=0;il<SCLRS;il++){ // <--- a2d-ratios:
      for(ib=0;ib<SCMXBR;ib++){
        chan=2*SCMXBR*il+2*ib;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
      for(ib=0;ib<SCMXBR;ib++){
        chan=2*SCMXBR*il+2*ib+1;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
    }
    tcfile << endl;
//
//
  tcfile.precision(3);// precision for gains
  for(il=0;il<SCLRS;il++){ // <--- gains:
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
  }
  tcfile << endl;
//
//
  tcfile.precision(2);// precision for mip2q's
  for(btyp=0;btyp<SCBTPN;btyp++){ // <--- mip2q's
    tcfile << mip2q[btyp]<<" ";
  }  
  tcfile << endl;
  tcfile << endl;
//
  for(btyp=0;btyp<SCBTPN;btyp++){ // <--- A-profile fit-parameters
    for(i=0;i<SCPROFP;i++)tcfile << aprofp[btyp][i]<<" ";
    tcfile << endl;
  }  
  tcfile << endl;
//
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOFAMPLcalib:output file closed !"<<endl;
//
}
//-----
// This is standard Minuit FCN :
void TOFAMPLcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
  for(i=0;i<nbinr;i++){
    ff=mcharge[i]-(x[0]*(exp(-(clent/2.+mcoord[i])/x[3])
      +x[2]*exp(-(clent/2.+mcoord[i])/x[4]))
      +x[1]*(exp(-(clent/2.-mcoord[i])/x[3])
      +x[2]*exp(-(clent/2.-mcoord[i])/x[4])));
    f+=(ff*ff);
  }
  if(flg==3){
    f=sqrt(f/number(nbinr));
    cout<<"Bar-type:"<<cbtyp<<"  funct:nbins="<<f<<endl;
    for(i=0;i<SCPROFP;i++){
      aprofp[cbtyp][i]=x[i];
      cout<<" np/par="<<i<<" "<<x[i]<<endl;
    }
  }
}
//
//=========================================================================
number TOFSTRRcalib::dtin[SCCHMX][SCSRCHB];//for mean Tin calc. in bin (for each channel)
number TOFSTRRcalib::dtinq[SCCHMX][SCSRCHB];//for sigma Tin calc. in bin (for each channel)
integer TOFSTRRcalib::nevnt[SCCHMX][SCSRCHB];// events in bin,channel
integer TOFSTRRcalib::nevtot[SCCHMX];       // total number of analized events
integer TOFSTRRcalib::nevnt2[SCCHMX];// for f/s-tdc time difference
number TOFSTRRcalib::fstdif[SCCHMX];
number TOFSTRRcalib::fstdif2[SCCHMX];
number TOFSTRRcalib::sbins[SCCHMX];// for "points-fit" method
number TOFSTRRcalib::ssumc[SCCHMX];
number TOFSTRRcalib::ssumt[SCCHMX];
number TOFSTRRcalib::ssumid[SCCHMX];
number TOFSTRRcalib::ssumct[SCCHMX];
number TOFSTRRcalib::ssumc2[SCCHMX];
number TOFSTRRcalib::ssumt2[SCCHMX];
//----------
void TOFSTRRcalib::init(){
  integer i,j,il,ib,id,ii,jj;
  char htit1[60];
  char inum[11];
  char in[2]="0";
  geant til[2]={15.,15.};// hist.limits for Tin
  geant tih[2]={95.,95.};
  geant tol[2]={2000.,2000.};// hist.limits for Tout
  geant toh[2]={5500.,5500.};
  integer mrf;
//
  strcpy(inum,"0123456789");
//
//  ---> book hist. Tovt-D vs Tovt-A :
//
  mrf=0;
  if(AMSJob::gethead()->isRealData())mrf=1;
//
  for(il=0;il<SCLRS;il++){   
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        strcpy(htit1,"Stretcher Tout vs Tin for chan(LBBS) ");
        in[0]=inum[il+1];
        strcat(htit1,in);
        ii=(ib+1)/10;
        jj=(ib+1)%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        in[0]=inum[i+1];
        strcat(htit1,in);
        id=1600+il*SCMXBR*2+ib*2+i;
        HBOOK2(id,htit1,50,til[mrf],tih[mrf],50,tol[mrf],toh[mrf],0.);
      }
    }
  }
//
// ---> clear arrays:
//
  for(i=0;i<SCCHMX;i++){
    nevnt2[i]=0;
    fstdif[i]=0.;
    fstdif2[i]=0.;
    sbins[i]=0.;
    ssumc[i]=0.;
    ssumt[i]=0.;
    ssumid[i]=0.;
    ssumct[i]=0.;
    ssumc2[i]=0.;
    ssumt2[i]=0.;
    for(j=0;j<SCSRCHB;j++){
      dtin[i][j]=0.;
      dtinq[i][j]=0.;
      nevnt[i][j]=0;
    }
    nevtot[i]=0;
  }
} 
//----------
void TOFSTRRcalib::fill(integer ichan, number tm[3]){
  number dti,dto;
  integer idto,idtol[2];
//
  assert(ichan>=0 && ichan<SCCHMX);
  if(AMSJob::gethead()->isRealData()){
    idtol[0]=SCSRCLB;
    idtol[1]=SCSRCHB;
  }
  else{
    idtol[0]=SCSRCLBMC;
    idtol[1]=SCSRCHBMC;
  }
  nevtot[ichan]+=1;
  dti=tm[0]-tm[1];
  dto=tm[1]-tm[2];
  idto=integer(dto/SCSRCTB);// time bin = SCSRCTB ns
  if(idto>=idtol[0] && idto<idtol[1] && dti>20. && dti<130.){
    nevnt[ichan][idto]+=1;
    dtin[ichan][idto]+=dti;
    dtinq[ichan][idto]+=(dti*dti);
    sbins[ichan]+=1.;// for "points-method"
    ssumid[ichan]+=1.;
    ssumc[ichan]+=dto;
    ssumt[ichan]+=dti;
    ssumct[ichan]+=(dto*dti);
    ssumc2[ichan]+=(dto*dto);
    ssumt2[ichan]+=(dti*dti);
  }
  HF2(1600+ichan,geant(dti),geant(dto),1.);
}
//-----------
void TOFSTRRcalib::fill2(integer ichan, number tdif){
  nevnt2[ichan]+=1;
  fstdif[ichan]+=tdif;
  fstdif2[ichan]+=(tdif*tdif);
}
//-----------
void TOFSTRRcalib::outp(){
  integer i,j,il,ib,id,ic,stat(0),idtol[2],dbsta;
  number t0,sl,chi2[SCCHMX],t,tq,co,dis,nev,bins;
  number sumc,sumt,sumct,sumc2,sumt2,sumid;
  number strr[SCCHMX],offs[SCCHMX];
  int method(1);// <------ method number to use for fit (0/1->bins/points)
  number pndis(0.27);// rms**2 (ns) for points-method
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="mc";
  char vers2[3]="rl";
  integer cfvn;
  uinteger StartRun;
  time_t StartTime;
  int dig;
//
//--> create name for output file
// 
  strcpy(inum,"0123456789");
  cfvn=TOFCAFFKEY.cfvers%100;
  strcpy(fname,"srcalib");
  dig=cfvn/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".dat");
//
//--> get run/time of the first event
//
  StartRun=AMSTOFRawEvent::getsrun();
  StartTime=AMSTOFRawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  if(AMSJob::gethead()->isRealData()){
    idtol[0]=SCSRCLB;
    idtol[1]=SCSRCHB;
  }
  else{
    idtol[0]=SCSRCLBMC;
    idtol[1]=SCSRCHBMC;
  }
if(method==0){// <----- "bin-method"
  for(ic=0;ic<SCCHMX;ic++){ // <-- chan. loop
    sumc=0.;
    sumt=0.;
    sumct=0.;
    sumc2=0.;
    sumt2=0.;
    sumid=0.;
    bins=0.;
    t0=0.;
    sl=0.;
    strr[ic]=0.;
    offs[ic]=0.;
    chi2[ic]=0.;
    for(j=idtol[0];j<idtol[1];j++){//<-- Tout bin loop
      nev=number(nevnt[ic][j]);
      co=(number(j)+0.5)*SCSRCTB;// mid. of Tout bin
      if(nev>=3.){ // min. 3 events
        t=dtin[ic][j]/nev; //get mean
        tq=dtinq[ic][j]/nev; // mean square
        dis=tq-t*t;// rms**2
        if(dis>=0.)HF1(1204,geant(sqrt(dis)),1.);
        if(dis>=0. && sqrt(dis)<2.){// apply max.cut on rms (2ns)
          if(dis==0.)dis=1./12.;// if all events was in single Tin bin=1ns
          dis/=(nev-1.);// rms**2 of the mean
          bins+=1.;
          sumc+=(co/dis);
          sumt+=(t/dis);
          sumid+=(1./dis);
          sumct+=(co*t/dis);
          sumc2+=(co*co/dis);
          sumt2+=(t*t/dis);
        }
      }
    }//--> end of bin loop
//
    if(bins>=15.){
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[ic]=sumt2+t0*t0*sumid+sl*sl*sumc2
        -2.*t0*sumt-2.*sl*sumct+2.*t0*sl*sumc;
      chi2[ic]/=(bins-2.);
      if(sl>0.){
        strr[ic]=1./sl;
        offs[ic]=-t0/sl;
      }
      HF1(1200,geant(strr[ic]),1.);
      HF1(1201,geant(offs[ic]),1.);
      HF1(1202,geant(chi2[ic]),1.);
    }
  }//--> end of chan. loop
}
//---
else{// <--------"points=method"
  dis=pndis;// rms**2 (ns**2)
  for(ic=0;ic<SCCHMX;ic++){ // <-- chan. loop
    t0=0.;
    sl=0.;
    strr[ic]=0.;
    offs[ic]=0.;
    chi2[ic]=0.;
    HF1(1204,geant(sqrt(dis)),1.);
    sumc=ssumc[ic]/dis;
    sumt=ssumt[ic]/dis;
    sumid=ssumid[ic]/dis;
    sumct=ssumct[ic]/dis;
    sumc2=ssumc2[ic]/dis;
    sumt2=ssumt2[ic]/dis;
    bins=sbins[ic];
//
    if(bins>=100.){
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[ic]=sumt2+t0*t0*sumid+sl*sl*sumc2
        -2.*t0*sumt-2.*sl*sumct+2.*t0*sl*sumc;
      chi2[ic]/=(bins-2.);
      if(sl>0.){
        strr[ic]=1./sl;
        offs[ic]=-t0/sl;
      }
      HF1(1200,geant(strr[ic]),1.);
      HF1(1201,geant(offs[ic]),1.);
      HF1(1202,geant(chi2[ic]),1.);
    }
  }//--> end of chan. loop
}
//------------
  printf("\n\n");
  printf("===========> Channels STRR-calibration report :\n\n");
  printf("\n");
  printf("Event/channel collected :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6d",nevtot[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6d",nevtot[ic]);
    }
    printf("\n\n");
  }
//---
  printf("Stretcher ratios :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n\n");
  }
//---
  printf("Offsets (ns) :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n\n");
  }
//
//  printf("Stretcher ratio errors :\n\n");
//  for(il=0;il<SCLRS;il++){
//    for(ib=0;ib<SCMXBR;ib++){
//      ic=il*SCMXBR*2+ib*2;
//      printf(" % 5.2f",s1[ic]);
//    }
//    printf("\n");
//    for(ib=0;ib<SCMXBR;ib++){
//      ic=il*SCMXBR*2+ib*2+1;
//      printf(" % 5.2f",s1[ic]);
//    }
//    printf("\n\n");
//  }
//---
  printf("Chi2 :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n\n");
  }
//
//---
  for(il=0;il<SCLRS;il++){ // print 2-D histogr.  
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        id=1600+il*SCMXBR*2+ib*2+i;
        HPRINT(id);
      }
    }
  }
//------------------------------------------------
// ===> f/s-tdc time difference calculation :
//
  number fstdm[SCCHMX],fstds[SCCHMX];
  number nttd,tdfs;
  for(i=0;i<SCCHMX;i++){
    fstdm[i]=0.;
    fstds[i]=0.;
    if(nevnt2[i]>5){
      nttd=number(nevnt2[i]);
      fstdm[i]=fstdif[i]/nttd;
      fstds[i]=fstdif2[i]/nttd;
      tdfs=fstds[i]-fstdm[i]*fstdm[i];
      if(tdfs<0.)tdfs=0.;
      fstds[i]=sqrt(tdfs);
    }
  }
  printf("\n\n");
  printf("===========> Channels f/s-tdc t-differences :\n\n");
  printf("\n");
  printf("Event/channel collected :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 6d",nevnt2[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 6d",nevnt2[ic]);
    }
    printf("\n\n");
  }
//
  printf("Time differences :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 5.1f",fstdm[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 5.1f",fstdm[ic]);
    }
    printf("\n\n");
  }
//
  printf("RMS of time differences :\n\n");
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2;
      printf(" % 5.1f",fstds[ic]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      ic=il*SCMXBR*2+ib*2+1;
      printf(" % 5.1f",fstds[ic]);
    }
    printf("\n\n");
  }
//------------------------------------------------
//
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOFSTRRcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for STRR-calibration output, fname:"<<fname<<'\n';
  cout<<"Stretcher ratios for indiv.channels will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
  tcfile.setf(ios::fixed);
  tcfile.width(6);
  tcfile.precision(2);// precision 
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){// str-ratios
      ic=il*SCMXBR*2+ib*2;
      dbsta=-1;
      if(chi2[ic]>0.)dbsta=0; 
      if(chi2[ic]>20.)dbsta=10;//problem with t-measurement
      tcfile <<strr[ic]<<" "<<offs[ic]<<" "<<dbsta<<"  ";// side-1
      ic+=1;
      dbsta=-1;
      if(chi2[ic]>0.)dbsta=0; 
      if(chi2[ic]>20.)dbsta=10;//problem with t-measurement
      tcfile <<strr[ic]<<" "<<offs[ic]<<" "<<dbsta<<endl;// side-2
    }
    tcfile << endl;
  }
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOFSTRRcalib:output file closed !"<<endl;
}
//=============================================================================
number TOFAVSDcalib::dtdyn[SCCHMX][SCACHB];// to calc.mean TovT-dynode per chan/bin 
number TOFAVSDcalib::dtdyn2[SCCHMX][SCACHB];// to calc.mean (TovT-dynode)**2 per chan/bin
integer TOFAVSDcalib::nevdyn[SCCHMX][SCACHB];// events in above sums per chan/bin 
integer TOFAVSDcalib::nevdynt[SCCHMX];// events in above sums per chan
//--------------------------
void TOFAVSDcalib::init(){ // ----> initialization for AVSD-calibration 
  integer i,j,il,ib,id,ii,jj;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
//  ---> book hist. Tovt-D vs Tovt-A :
//
  for(il=0;il<SCLRS;il++){   
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        strcpy(htit1,"TovT-D vs TovT-A for chan(LBBS) ");
        in[0]=inum[il+1];
        strcat(htit1,in);
        ii=(ib+1)/10;
        jj=(ib+1)%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        in[0]=inum[i+1];
        strcat(htit1,in);
        id=1800+il*SCMXBR*2+ib*2+i;
        HBOOK2(id,htit1,50,100.,450.,50,0.,300.,0.);
      }
    }
  }
// ---> clear arrays:
//
  for(i=0;i<SCCHMX;i++){
    for(j=0;j<SCACHB;j++){
      dtdyn[i][j]=0.;
      dtdyn2[i][j]=0.;
      nevdyn[i][j]=0.;
    }
    nevdynt[i]=0;
  }
}
//-------------------------------------------
//            ---> program to fill TovT-arrays:
void TOFAVSDcalib::filltovt(integer chan, geant tma[2], geant tmd[2]){
//
  integer ic;
  geant tota,totd;
//
  tota=tma[0]-tma[1];//front_edge - back_edge
  totd=tmd[0]-tmd[1];
  HF2(1800+chan,tota,totd,1.);
      ic=integer(tota/SCACBW);// get TovT-anode bin
      if(ic<SCACHB && totd>30.){// use events with TovT-d > 30ns and ic<SCACHB
        dtdyn[chan][ic]+=totd;
        dtdyn2[chan][ic]+=(totd*totd);
        nevdyn[chan][ic]+=1;
        nevdynt[chan]+=1;
      }
}
//--------------------------------------
//            ---> program to get final  parameters :
void TOFAVSDcalib::fit(){
//
  integer i,j,il,ib,idd,is,chan;
  int16u id,cr,sf,tc;
  number t0,sl,t,tq,co,dis,nevf,bins;
  number sumc,sumt,sumct,sumc2,sumt2,sumid;
  number slop[SCCHMX],offs[SCCHMX],chi2[SCCHMX];
  char fname[80]="avcalib.dat";
//
// ---> calculate/print TovT-d vs TovT-a param.(special(A.Contin's) dyn-calibr):
//
//
  for(i=0;i<SCCHMX;i++){ // <-- chan. loop
    sumc=0.;
    sumt=0.;
    sumct=0.;
    sumc2=0.;
    sumt2=0.;
    sumid=0.;
    bins=0.;
    t0=0.;
    sl=0.;
    slop[i]=0.;
    offs[i]=0.;
    chi2[i]=0.;
    for(j=0;j<SCACHB;j++){//<-- Tout bin loop
      nevf=number(nevdyn[i][j]);
      co=(number(j)+0.5)*SCACBW;// mid. of TovT-anode bin
      if(nevf>10.){ // min. 10 events
        t=dtdyn[i][j]/nevf; // get mean
        tq=dtdyn2[i][j]/nevf; // mean square
        dis=tq-t*t;// rms**2
        if(dis>=0.){
        if(dis==0.)dis=1./12.;// if all events are in single TovT-d bin=1ns
          dis/=(nevf-1.);// rms**2 of the mean
          bins+=1.;
          sumc+=(co/dis);
          sumt+=(t/dis);
          sumid+=(1./dis);
          sumct+=(co*t/dis);
          sumc2+=(co*co/dis);
          sumt2+=(t*t/dis);
        }
      }
    }//--> end of bin loop
//
    if(bins>=5){
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[i]=sumt2+t0*t0*sumid+sl*sl*sumc2
        -2.*t0*sumt-2.*sl*sumct+2.*t0*sl*sumc;
      chi2[i]/=(bins-2.);
      slop[i]=sl;
      offs[i]=t0;
      HF1(1240,geant(slop[i]),1.);
      HF1(1241,geant(offs[i]),1.);
      HF1(1242,geant(chi2[i]),1.);
    }
  }//--> end of chan. loop
//
  printf("\n");
  printf(" ===============>  Dynode-Anode correlation parameters :\n");
  printf("\n");
  for(il=0;il<SCLRS;il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6d",nevdynt[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6d",nevdynt[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.2f",slop[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.0f",offs[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib;
      printf("%6.2f",chi2[chan]);
    }
    printf("\n");
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.2f",slop[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.0f",offs[chan]);
    }
    printf("\n");
    for(ib=0;ib<SCMXBR;ib++){
      chan=2*SCMXBR*il+2*ib+1;
      printf("%6.2f",chi2[chan]);
    }
    printf("\n");
    printf("\n");
  }
//
//---
  for(il=0;il<SCLRS;il++){ // print 2-D histogr.  
    for(ib=0;ib<SCMXBR;ib++){
      for(i=0;i<2;i++){
        idd=1800+il*SCMXBR*2+ib*2+i;
        HPRINT(idd);
      }
    }
  }
//
// ---> write slop/offs to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOFAVSDcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for AVSD-calibration output, fname:"<<fname<<'\n';
  cout<<"Indiv. channel dynode-parameters(A.Contin's ones) will be written !"<<'\n';
//
  tcfile.setf(ios::fixed);
  for(cr=0;cr<SCCRAT;cr++){
    tcfile.width(1);
    tcfile <<(cr+1)<<endl;
    for(sf=0;sf<SCSFET;sf++){
      tcfile.width(1);
      tcfile <<"  "<<(sf+1)<<endl;
      for(tc=0;tc<SCTOFC;tc++){
        tcfile.width(1);
        tcfile <<"   "<<(tc+1)<<endl;
        id=AMSTOFRawEvent::hw2swid(cr,sf,tc);
        if(id==0)continue;
        is=id%10-1;
        ib=(id/10)%100-1;
        il=id/1000-1;
        chan=2*SCMXBR*il+2*ib+is;
        tcfile.width(6);
        tcfile.precision(1);// precision for offs
        tcfile <<"     "<<offs[chan]<<" ";
        tcfile.precision(2);// precision for slop
        tcfile <<slop[chan]<<endl;
      }
    }
  }
//  
//  for(il=0;il<SCLRS;il++){ // <--- slop/offs :
//    tcfile.precision(2);// precision for slop
//    for(ib=0;ib<SCMXBR;ib++){
//      chan=2*SCMXBR*il+2*ib;
//      tcfile << slop[chan]<<" ";
//    }
//    tcfile << endl;
//    tcfile.precision(1);// precision for offs
//    for(ib=0;ib<SCMXBR;ib++){
//      chan=2*SCMXBR*il+2*ib;
//      tcfile << offs[chan]<<" ";
//    }
//    tcfile << endl<<endl;
//
//    tcfile.precision(2);// precision for slop
//    for(ib=0;ib<SCMXBR;ib++){
//      chan=2*SCMXBR*il+2*ib+1;
//      tcfile << slop[chan]<<" ";
//    }
//    tcfile << endl;
//    tcfile.precision(1);// precision for offs
//    for(ib=0;ib<SCMXBR;ib++){
//      chan=2*SCMXBR*il+2*ib+1;
//      tcfile << offs[chan]<<" ";
//    }
//    tcfile << endl<<endl;
//  }
  tcfile << endl;
  tcfile.close();
  cout<<"TOFAVSDcalib:output file closed !"<<endl;
}
//--------------------------------------------------
//
//=============================================================================
