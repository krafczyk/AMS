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
//  start[1]=TOFCAFFKEY.fixstr;// def.inv.str.ratio
  start[1]=1.;// now str.ratio is known and already included in input-times !!!
  step[1]=0.01;
  plow[1]=0.001;
  phigh[1]=1.;
//  ifit[1]=TOFCAFFKEY.ifstr;// fix/release strr-parameter
  ifit[1]=0;// fixed(known) now !!!
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
    cout<<"Slope and individual T0's will be written !"<<'\n';
    slope=geant(x[0]);
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
  number shft;
  geant strr[2];
  AMSTOFRawCluster *ptr;
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
//----
  TOFJobStat::addre(6);
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
    scbrcal[ilay][ibar].gtstrat(strr);
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
  TOFJobStat::addre(7);
//------>
    number t1,t2,t3,t4,t13,t24;
       ilay=0; //        <-- some hist. for calibration run
    shft=TOFDBc::shftim();
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t1=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1510,geant(tm[0]),1.);
      HF1(1511,geant(tm[1]),1.);
      HF1(1512,geant(ama[0]),1.);
      HF1(1513,geant(ama[1]),1.);
      HF2(1502,geant(tamp),geant(time),1.);
    }
       ilay=1;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t2=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1514,geant(tm[0]),1.);
      HF1(1515,geant(tm[1]),1.);
      HF1(1516,geant(ama[0]),1.);
      HF1(1517,geant(ama[1]),1.);
      HF2(1503,geant(tamp),geant(time),1.);
    }
       ilay=2;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t3=time;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1518,geant(tm[0]),1.);
      HF1(1519,geant(tm[1]),1.);
      HF1(1520,geant(ama[0]),1.);
      HF1(1521,geant(ama[1]),1.);
      HF2(1504,geant(tamp),geant(time),1.);
    }
       ilay=3;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    t4=time;
    t13=t1-t3;
    t24=t2-t4;
    tamp=exp(-ama[0]/shft)+exp(-ama[1]/shft);
    if(TOFRECFFKEY.reprtf[2]>0){
      HF1(1522,geant(tm[0]),1.);
      HF1(1523,geant(tm[1]),1.);
      HF1(1524,geant(ama[0]),1.);
      HF1(1525,geant(ama[1]),1.);
      HF2(1505,geant(tamp),geant(time),1.);
      HF1(1508,geant(t13),1.);
      HF1(1509,geant(t24),1.);
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
    TOFJobStat::addre(8);
//
    int il,ib,ix,iy;
    geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cosi;
    number ram[4],dum[3],tld[3],tdi[3];
    ix=0;
    iy=0;
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
    cosi=sqrt(1.+tgx*tgx+tgy*tgy);// this is 1/cos(theta) !!!
    tld[0]=(zc[0]-zc[1])*cosi;
    tld[1]=(zc[0]-zc[2])*cosi;
    tld[2]=(zc[0]-zc[3])*cosi;
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
//=============================================================================
//
geant TOFAMPLcalib::amsd[2][SCBLMX];
integer TOFAMPLcalib::rbls[SCBTPN];
geant TOFAMPLcalib::btamp[SCBTPN][SCPRBM]; 
geant TOFAMPLcalib::profb[SCBTPN][SCPRBM];
geant TOFAMPLcalib::profp[SCBTPN][SCPRBM];
integer TOFAMPLcalib::nprbn[SCBTPN]={14,16,17,18,19};//profile bins vs bar-type
//--------------------------
void TOFAMPLcalib::init(){ // ----> initialization for AMPL-calibration 
  int i,j,il,ib,ii,jj,id,nadd,nbnr;
  geant blen,dd,bw,bl,bh;
  geant bwid[6]={2.,3.,3.,4.,5.,8.};// bin width (first 6 standard bins) 
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
  for(i=0;i<SCBLMX;i++){
    amsd[0][i]=0.;
    amsd[1][i]=0.;
  }
  for(i=0;i<SCBTPN;i++){
    for(j=0;j<SCPRBM;j++){
      btamp[i][j]=0.;
    }
  }
//
  for(il=0;i<SCLRS;il++){   // book hist. for side-signals
    for(ib=0;j<SCMXBR;ib++){
      for(i=0;i<2;i++){
        strcpy(htit1,"Center signals distr. for chan(LBBS) ");
        in[0]=inum[il];
        strcat(htit1,in);
        ii=ib/10;
        jj=ib%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        in[0]=inum[i];
        strcat(htit1,in);
        id=1600+il*SCMXBR*2+ib*2+i;
        HBOOK1(id,htit1,80,0.,480.,0.);
      }
    }
  }
//
  for(i=0;i<SCBTPN;i++){   // book hist. for ref.bars bin signals
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
    for(j=0;j<nbnr;j++){
      strcpy(htit2,"Bin signals distribution for bar(LBB)/bin(NN) ");
      strcat(htit2,htit3);
      ii=j/10;
      jj=j%10;
      in[0]=inum[ii];
      strcat(htit2,in);
      in[0]=inum[jj];
      strcat(htit2,in);
      id=1601+SCLRS*SCMXBR*2+i*SCPRBM+j;
      HBOOK1(id,htit2,80,0.,960.,0.);
    }
  }
//
  for(i=0;i<SCBTPN;i++){ // prepare profile bin width array
    id=rbls[i];
    il=id/100;
    ib=id%100;
    blen=TOFDBc::brlen(il,ib);
    nbnr=nprbn[i];//real numb. of bins
    dd=0.;
    for(j=0;j<6;j++){ // bin width for the first/last 6 standard ones
      dd+=bwid[j];
      profb[i][j]=bwid[j];
      profb[i][nbnr-j-1]=bwid[j];
    }
    nadd=nbnr-2*6;// number of additional bins of THE SAME WIDTH !
    bw=(blen-dd)/nadd;// width of additional bins
    for(j=0;j<nadd;j++){ // complete bin width array
      profb[i][6+j]=bw;
    }
  }
//
  for(i=0;i<SCBTPN;i++){ // prepare mid.points positions (loc.r.s) of profile bins
    id=rbls[i];
    il=id/100;
    ib=id%100;
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
    blen=TOFDBc::brlen(il,ib);
    bl=-0.5*blen;
    bh=bl+blen;
    HBOOK1(1220+i,htit1,70,bl,bh,0.);
  }
//
}
//--------------------------------------
//
void TOFAMPLcalib::select(){ // ------> event selection for AMPL-calibration
  integer i,ilay,ibar,nbrl[SCLRS],brnl[SCLRS],bad;
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc;
  number ama[2],coo[SCLRS],coot[SCLRS],cstr[SCLRS],dx,dy;
  number am1[SCLRS],am2[SCLRS],am[2];
  geant ainp[2],cinp;
  AMSTOFRawCluster *ptr;
  ptr=(AMSTOFRawCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFRawCluster",0);
//----
  TOFJobStat::addre(10);
  for(i=0;i<SCLRS;i++)nbrl[i]=0;
//
  while (ptr){ // <--- loop over AMSTOFRawCluster hits
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    nbrl[ilay]+=1;
    brnl[ilay]=ibar;
    ptr->gettovta(ama);// get raw anode-ampl(TovT in ns)
    scbrcal[ilay][ibar].ama2q(ama,am);// convert to charge
    am1[ilay]=am[0];
    am2[ilay]=am[1];
    coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOFJobStat::addre(11);
//
//------> get parameters from tracker:
//
    static number pmas(0.938);
    number pmom,bet,chi2,betm;
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
    if(ntrk!=1 && TOFCAFFKEY.truse==1)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();//get pointer of the track, used in given particle
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    } 
    else rid=0;
    pmom=fabs(rid);
    if(TOFRECFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(pmom<TOFCAFFKEY.plhc[0] || pmom>TOFCAFFKEY.plhc[1]){
      if(TOFCAFFKEY.truse=1)return;//rem. low/high mom.
    }
    TOFJobStat::addre(12);
//
    if(TOFCAFFKEY.truse=1){// tracker: find crossing points/angles with counters
    C0[0]=0.;
    C0[1]=0.;
    for(il=0;il<SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      cstr[il]=cos(the);
      if(TOFDBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take Y-cross
        ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take X-cross
        coot[il]=-coot[il];// go to bar local r.s.
        ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
      }
      dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc
      if(TOFRECFFKEY.reprtf[2]>0)HF1(1200+il,geant(dy),1.);
      dx=ctran-TOFDBc::gettsc(il,ib);//Transv.coo_track-Transv.coo_sc
      if(TOFRECFFKEY.reprtf[2]>0)HF1(1210+il,geant(dx),1.);
    }
    cost=geant(0.25*(cstr[0]+cstr[1]+cstr[2]+cstr[3]));// average cos from track
    }
//
//-------> get track impact angle using scint-made long.coord :
//
    ix=0;
    iy=0;                                
    for(il=0;il<4;il++){
      ib=brnl[il];
      zc[il]=TOFDBc::getzsc(il,ib);
      if(TOFDBc::plrotm(il)==0){// unrotated (Long->Y) planes
        y[iy]=coo[il];
        zy[ix]=zc[il];
        iy+=1;
      }
      else{                    // rotated (Long->-X) planes
        x[ix]=-coo[il];
        zx[ix]=zc[il];
        ix+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);
//
    if(TOFRECFFKEY.reprtf[2]>0){
      if(TOFCAFFKEY.truse == 1){
        HF1(1215,(cost-cosc)/cost,1.);
        HF1(1216,cost,1.);
      }
      else{
        HF1(1217,cosc,1.);
      }
    }
//
// ------> normalize charge to normal incidence :
//
    for(il=0;il<4;il++){
      if(TOFCAFFKEY.truse == 1){
        am1[il]*=cstr[il];
        am2[il]*=cstr[il];
      }
      else{
        am1[il]*=cosc;
        am2[il]*=cosc;
      }
    }
// ------> fill working arrays and histograms :
//
    for(il=0;il<4;il++){
      ainp[0]=geant(am1[il]);
      ainp[1]=geant(am2[il]);
      if(TOFCAFFKEY.truse == 1)
                               cinp=coot[il];
      else
                               cinp=coo[il];
      TOFAMPLcalib::fill(il,ib,ainp,cinp);
    }
}
//--------------------------------------
//            ---> program to accumulate data for AMPL-calibration:
void TOFAMPLcalib::fill(integer il, integer ib, geant am[2], geant coo){
//
  integer id,idr,idh,ibt,nbn,nb;
  geant bl,bh;
  static geant cbin(10.);// centr. bin width for gain calibr.
//
//                             ---> fill hist. for gains:
//
  if(fabs(coo) < cbin){// select only central incidence(+-10cm)
    idh=1600+il*SCMXBR*2+ib*2+0;// side-1
    HF1(idh,am[0],1.);
    idh=1600+il*SCMXBR*2+ib*2+1;// side-2
    HF1(idh,am[1],1.);
  }
//                             ---> fill profile hist.:
  ibt=TOFDBc::brtype(il,ib);// 1-5
  idr=rbls[ibt-1];
  id=100*(il+1)+ib+1;
  if(id == idr){// only for ref. sc. bars
    HF1(1219+ibt,coo,1.);// longit.imp.point distribution
    if(ibt == 5 && fabs(coo) < cbin){
      HF1(1250,am[0],1.);// Q-distr. for ref.bar type=5, s=1
      HF1(1251,am[1],1.);// Q-distr. for ref.bar type=5, s=2
    }
    nbn=nprbn[ibt-1];
    for(nb=0;nb<nbn;nb++){
      bl=profp[ibt-1][nb]-0.5*profb[ibt-1][nb];
      bh=bl+profb[ibt-1][nb];
      if(coo>=bl && coo<bh){
        idh=1601+SCLRS*SCMXBR*2+(ibt-1)*SCPRBM+nb;
        HF1(idh,(am[0]+am[1]),1.);
      }
    }
  }
}
//--------------------------------------
//            ---> program to get final AMPL-calibration:
void TOFAMPLcalib::fit(){
//
  integer id,idr,idh,ibt,nbn,nb;
}
//=========================================================================




