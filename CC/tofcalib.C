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
#include <particle.h>
#include <iostream.h>
#include <fstream.h>
//#include <string.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//
extern TOFBrcal scbrcal[SCLRS][SCMXBR];// TZSL-calibration data
//
geant TOFTZSLcalib::strrat;
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
//--------------------------------------------------------------------
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
  strcpy(p2nam,"istrr");
  strcpy(p3nam,"tz");
  strcpy(inum,"0123456789");
//
// -----------> set parameter defaults:
  strcpy(pnam[0],p1nam); // for slope
  start[0]=TOFCAFFKEY.fixsl;// def. slope
  step[0]=0.5;
  plow[0]=0.;
  phigh[0]=10.;
  ifit[0]=TOFCAFFKEY.ifsl;// fix/release slope 
//
  strcpy(pnam[1],p2nam); // for inverse stratcher ratio
  start[1]=TOFCAFFKEY.fixstr;// def. for inv. str.ratio
  step[1]=0.05;
  plow[1]=0.001;
  phigh[1]=1.;
  ifit[1]=TOFCAFFKEY.ifstr;// fix/release istrr-parameter
//
  for(il=0;il<SCLRS;il++){
    for(ib=0;ib<SCMXBR;ib++){
      id=(il+1)*100+ib+1;
      ii=il*SCMXBR+ib;
      for(j=0;j<2;j++){
        if(id == TOFCAFFKEY.idref[j])
          start[ii+2]=TOFCAFFKEY.tzref[j];//def. t0 for ref.counters
        else 
          start[ii+2]=6.;
      }
      step[ii+2]=1.;
      plow[ii+2]=0.;
      phigh[ii+2]=30.;
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
      if(s3[il][ib]>=10.){
        ifit[2+il*SCMXBR+ib]=1;//bar with high statist.- release
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
        ifit[2+il*SCMXBR+ib]=0;//bar with low statist.- fix
      }
    }
  }
  nparr+=2;
  printf(" Statistics enough to fit  %d  parameters\n",nparr);
  if(nparr<8){
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
  printf("Strrat     : %6.3e\n",strrat);
}
//-----------------------------------------------------------------------
// This is standard Minuit FCN :
void TOFTZSLcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j,il,ib;
  static int first(0);
  number f1[4],f2[3],f3(0.),f4[3],f5(0.),f6(0.),f7(0.),f8(0.);
  number f9[3],f10(0.);
  geant w;
  char fname[80]="tzslcalib.dat";
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
//
  if(flg==3){
    resol=-1.;
    if(f>=0. && events>0)resol=sqrt(f/events);
    ofstream tcfile(fname,ios::out);
    if(!tcfile){
      cerr<<"TOFTZSLcalib:error opening file for output"<<fname<<'\n';
      exit(8);
    }
    cout<<"Open file for TzSl-calibration output, fname:"<<fname<<'\n';
    cout<<"Strrat,slope and individual T-thr,Qa-thr,T0 will be written !"<<'\n';
    strrat=geant(1./x[1]);
    slope=geant(x[0]);
    tcfile << strrat;
    tcfile << slope;
    for(il=0;il<SCLRS;il++){
      for(ib=0;ib<SCMXBR;ib++){
        tzero[il][ib]=geant(x[2+il*SCMXBR+ib]);
        tcfile << tzero[il][ib];
      }
    }
    tcfile.close();
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
  integer i,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad;
  number tm[2],ama[2],amd[2],time,timeD,tamp,edepa,edepd;
  number trp1[SCLRS],trp2[SCLRS],arp1[SCLRS],arp2[SCLRS];
  number strr,shft;
  AMSTOFRawCluster *ptr;
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
//----
  TOFJobStat::addre(2);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//----
  while (ptr){ // <--- loop over AMSTOFRawCluster hits
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    ptr->gettovta(ama);
    ptr->gettovtd(amd);
    ptr->getsdtm(tm);
    nbrl[ilay]+=1;
    brnl[ilay]=ibar;
    trp1[ilay]=tm[0];
    trp2[ilay]=tm[1];
    arp1[ilay]=ama[0];
    arp2[ilay]=ama[1];
    ptr=ptr->next();
  }// --- end of hits loop --->
//----
//    Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOFJobStat::addre(3);
//------>
    geant t1,t2,t3,t4,t13,t24;
       ilay=0; //        <-- some hist. for calibration run
    strr=scbrcal[ilay][brnl[ilay]].gtstrat();
    shft=TOFDBc::shftim();
    tm[0]=trp1[ilay]/strr;
    tm[1]=trp2[ilay]/strr;
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t1=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1510,tm[0],1.);
      HF1(1511,tm[1],1.);
      HF1(1512,ama[0],1.);
      HF1(1513,ama[1],1.);
      HF2(1502,tamp,time,1.);
    }
       ilay=1;
    strr=scbrcal[ilay][brnl[ilay]].gtstrat();
    tm[0]=trp1[ilay]/strr;
    tm[1]=trp2[ilay]/strr;
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t2=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1514,tm[0],1.);
      HF1(1515,tm[1],1.);
      HF1(1516,ama[0],1.);
      HF1(1517,ama[1],1.);
      HF2(1503,tamp,time,1.);
    }
       ilay=2;
    strr=scbrcal[ilay][brnl[ilay]].gtstrat();
    tm[0]=trp1[ilay]/strr;
    tm[1]=trp2[ilay]/strr;
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t3=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1518,tm[0],1.);
      HF1(1519,tm[1],1.);
      HF1(1520,ama[0],1.);
      HF1(1521,ama[1],1.);
      HF2(1504,tamp,time,1.);
    }
       ilay=3;
    strr=scbrcal[ilay][brnl[ilay]].gtstrat();
    tm[0]=trp1[ilay]/strr;
    tm[1]=trp2[ilay]/strr;
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t4=time;
    t13=t1-t3;
    t24=t2-t4;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1522,tm[0],1.);
      HF1(1523,tm[1],1.);
      HF1(1524,ama[0],1.);
      HF1(1525,ama[1],1.);
      HF2(1505,tamp,time,1.);
      HF1(1508,t13,1.);
      HF1(1509,t24,1.);
    }
//------> get parameters from tracker:
    static number pmas(0.938);
    number pmom,bet,chi2,betm;
    number the,phi,rid,err;
    AMSPoint C0;
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    int ntrk,ipatt;
    ntrk=0;
    cptr=AMSEvent::gethead()->getC("AMSParticle",0);// get TOF-matched track
    if(cptr)
           ntrk+=cptr->getnelem();
    HF1(1506,geant(ntrk),1.);
    if(ntrk!=1)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    } 
    else rid=0;
    pmom=fabs(rid);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(pmom<TOFCAFFKEY.pcut[0] || pmom>TOFCAFFKEY.pcut[1])return;//remove low/too_high mom events
    bet=pmom/sqrt(pmom*pmom+pmas*pmas);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1501,bet,1.);
    TOFJobStat::addre(4);
//
    int il,ib,ix(0),iy(0);
    geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,tg;
    number ram[4],dum[3],tld[3],tdi[3];
    for(il=0;il<4;il++){
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
    tg=sqrt(1.+tgx*tgx+tgy*tgy);
    tld[0]=(zc[0]-zc[1])*tg;
    tld[1]=(zc[0]-zc[2])*tg;
    tld[2]=(zc[0]-zc[3])*tg;
    t1=0.5*(trp1[0]+trp2[0]);
    t2=0.5*(trp1[1]+trp2[1]);
    t3=0.5*(trp1[2]+trp2[2]);
    t4=0.5*(trp1[3]+trp2[3]);
    tdi[0]=t1-t2;
    tdi[1]=t1-t3;
    tdi[2]=t1-t4;
    for(i=0;i<SCLRS;i++)ram[i]=exp(-arp1[i]/shft)+exp(-arp2[i]/shft);
    dum[0]=ram[1]-ram[0];
    dum[1]=ram[2]-ram[0];
    dum[2]=ram[3]-ram[0];
    betm=TOFCAFFKEY.bmean;// tempor! use mean value for beta
//
    TOFTZSLcalib::fill(betm,brnl,tld,tdi,dum); // fill calib.working arrays
}
