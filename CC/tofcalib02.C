//  $Id: tofcalib02.C,v 1.7 2002/12/06 14:43:19 choumilo Exp $
#include <tofdbc02.h>
#include <point.h>
#include <typedefs.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <antidbc02.h>
#include <tofsim02.h>
#include <trrec.h>
#include <tofrec02.h>
#include <antirec02.h>
#include <tofcalib02.h>
#include <particle.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
//
//
extern TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];// TOF peds/sigmas/...
//
//
//--------------------------------------------------------------------
geant TOF2TZSLcalib::slope;
geant TOF2TZSLcalib::tzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s1;
number TOF2TZSLcalib::s2;
number TOF2TZSLcalib::s3[TOF2GC::SCLRS][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s4;
number TOF2TZSLcalib::s5;
number TOF2TZSLcalib::s6[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s7[TOF2GC::SCMXBR];
number TOF2TZSLcalib::s8;
number TOF2TZSLcalib::s9[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s10[TOF2GC::SCMXBR];
number TOF2TZSLcalib::s11;
number TOF2TZSLcalib::s12[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s13[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s14[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s15[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
number TOF2TZSLcalib::s16[TOF2GC::SCMXBR];
number TOF2TZSLcalib::events;
number TOF2TZSLcalib::resol;
//-----
void TOF2TZSLcalib::mfit(){  // calibr. fit procedure
  int i,id,ii,j,ier,il,ib,npar(TOF2GC::SCBLMX+2),nparr,seqnum;
//
  char pnm[6];
  char p1nam[6];
  char p2nam[6];
  char p3nam[3];
  char inum[11];
  char in[2]="0";
  int ifit[TOF2GC::SCBLMX+2];
  char pnam[TOF2GC::SCBLMX+2][6];
  number argl[10];
  int iargl[10];
  number start[TOF2GC::SCBLMX+2],step[TOF2GC::SCBLMX+2],plow[TOF2GC::SCBLMX+2],phigh[TOF2GC::SCBLMX+2];
//
  strcpy(p1nam,"tslop");
  strcpy(p2nam,"dummy");
  strcpy(p3nam,"tz");
  strcpy(inum,"0123456789");
//
// -----------> set parameter defaults:
  strcpy(pnam[0],p1nam); // for slope
  start[0]=TFCAFFKEY.fixsl;// def. slope 
  step[0]=1.;
  plow[0]=0.;
  phigh[0]=40.;
  ifit[0]=TFCAFFKEY.ifsl;// fix/release slope 
//
  strcpy(pnam[1],p2nam); // for inverse stratcher ratio (now dummy)
  start[1]=1.;// now str.ratio is known and already included in input-times !!!
  step[1]=0.01;
  plow[1]=0.001;
  phigh[1]=2.;
  ifit[1]=0;// fixed(known) now !!!
//
  ii=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      id=(il+1)*100+ib+1;
      start[ii+2]=TOF2Brcal::scbrcal[il][ib].gettzero();// def.T0's from current calibration
      for(j=0;j<2;j++){
        if(id == TFCAFFKEY.idref[j])
          start[ii+2]=TFCAFFKEY.tzref[j];//def. t0 for ref.counters (0.)
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
      ii+=1;//use sequential solid numbering of counters
    }
  }
// ------------> release T0-parameters with good statistics:
// 
  printf(" Collected events (total) : %9.0f\n",events);
  printf(" Collected events per sc. bar : \n");
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=1;ib<=TOF2DBc::getbppl(il);ib++){
      if(ib%TOF2DBc::getbppl(il) !=0)
                     printf("% 7.0f",s3[il][ib-1]);
      else
                     printf("% 7.0f\n",s3[il][ib-1]);
    }
  }
//
  nparr=0;
  seqnum=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      id=(il+1)*100+ib+1;
      if(s3[il][ib]>=1.){
        ifit[2+seqnum]=1;//bar with high statist.-> release
        nparr+=1;
        for(j=0;j<2;j++){
          if(id == TFCAFFKEY.idref[j]){
            ifit[2+seqnum]=0;//fix, if ref counter
            nparr-=1;
          }
        }
      }
      else{
        for(j=0;j<2;j++){
          if(id == TFCAFFKEY.idref[j]){
            cerr<<"Too low statistics in ref.counter "<<id<<" "<<s3[il][ib]<<'\n';
            return;
          }
        }
        ifit[2+seqnum]=0;//bar with low statist.-> fix
      }
      seqnum+=1;
    }
  }
  nparr+=2;
  printf(" Statistics enough to fit  %d  parameters\n",nparr);
  if(nparr<10){
    cerr<<"   Not enough counters for Minuit-Fit !!!"<<'\n';
    return;// not enough counters for FIT -> return
  }
  npar=seqnum+2;
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
  MNEXCM(TOF2TZSLcalib::mfun,"CALL FCN",argl,1,ier,0);
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
void TOF2TZSLcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j,il,ib,seqnum;
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
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(fname,"tzcalib");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=TOF2RawEvent::getsrun();
  StartTime=TOF2RawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
//
  seqnum=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    f1[il]=0.;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      f1[il]+=s3[il][ib]*x[2+seqnum]*x[2+seqnum];//l=1,4
      seqnum+=1;
    }
  }
//
  for(i=0;i<3;i++){//loop over difference L1->Lk, k=2,3,4
    f2[i]=0.;
    il=i+1;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++)f2[i]+=s6[i][ib]*x[2+TOF2DBc::barseqn(il,ib)];//l=2,4
  }
//
  for(ib=0;ib<TOF2DBc::getbppl(0);ib++)f3+=s7[ib]*x[2+ib];//l=1
//
  for(i=0;i<3;i++){
    f4[i]=0.;
    il=i+1;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++)f4[i]+=s9[i][ib]*x[2+TOF2DBc::barseqn(il,ib)];//l=2,4
  }
//
  for(ib=0;ib<TOF2DBc::getbppl(0);ib++)f5+=s10[ib]*x[2+ib];//l=1
//
  for(i=0;i<TOF2DBc::getbppl(0);i++){
    for(j=0;j<TOF2DBc::getbppl(1);j++)f6+=s12[i][j]*x[2+i]*x[2+TOF2DBc::barseqn(1,j)];//L12
  }
//
  for(i=0;i<TOF2DBc::getbppl(0);i++){
    for(j=0;j<TOF2DBc::getbppl(2);j++)f7+=s13[i][j]*x[2+i]*x[2+TOF2DBc::barseqn(2,j)];//L13
  }
//
  for(i=0;i<TOF2DBc::getbppl(0);i++){
    for(j=0;j<TOF2DBc::getbppl(3);j++)f8+=s14[i][j]*x[2+i]*x[2+TOF2DBc::barseqn(3,j)];//L14
  }
//
  for(i=0;i<3;i++){
    f9[i]=0.;
    il=i+1;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++)f9[i]+=s15[i][ib]*x[2+TOF2DBc::barseqn(il,ib)];//l=2,4
  }
//
  for(ib=0;ib<TOF2DBc::getbppl(0);ib++)f10+=s16[ib]*x[2+ib];//l=1
//
  f=s1+s2*x[1]*x[1]+3.*f1[0]+f1[1]+f1[2]+f1[3]+s4*x[0]*x[0]
   -2.*s5*x[1]+2.*(f2[0]+f2[1]+f2[2])-2.*f3+2.*s8*x[0]
   -2.*(f4[0]+f4[1]+f4[2])*x[1]+2.*f5*x[1]-2.*s11*x[0]*x[1]
   -2.*(f6+f7+f8)+2.*(f9[0]+f9[1]+f9[2])*x[0]-2.*f10*x[0];
//----------------
  if(flg==3){
// some histograms:
    idr=TFCAFFKEY.idref[0];
    tzr=TFCAFFKEY.tzref[0];
    il=idr/100-1;
    ib=idr%100-1;
    ibtr=TOF2DBc::brtype(il,ib);// ref. bar type (1->...)
    seqnum=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        tz=geant(x[2+seqnum]);
        ibt=TOF2DBc::brtype(il,ib);// bar type (1->...)
        if(ibt==ibtr)HF1(1507,(tz-tzr),1.);
        if(ibt==4)HF1(1508,(tz-tzr),1.);
	seqnum+=1;
      }
    }
// write parameters to ext.file:
    resol=-1.;
    if(f>=0. && events>0)resol=sqrt(f/events);
//
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"TOF2TZSLcalib:error opening file for output"<<fname<<'\n';
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
    seqnum=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        tzero[il][ib]=geant(x[2+seqnum]);
        tcfile <<" "<<tzero[il][ib];
	seqnum+=1;
      }
      tcfile << endl;
    }
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"TOF2TZSLcalib:output file closed !"<<endl;
  }
}
//-----------------------------------------------------------------------
// To fill arrays, used by FCN :
void TOF2TZSLcalib::fill(number bet, int ib[4], number tld[3]
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
void TOF2TZSLcalib::select(){  // calibr. event selection
  integer i,j,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector,conf,nanti(0);
  integer cref[2],lref[2];
  number ltim[4],tm[2],am[2],ama[2],amd[2],time,timeD,tamp,edepa,edepd,relt;
  number coo[TOF2GC::SCLRS],trp1[TOF2GC::SCLRS],trp2[TOF2GC::SCLRS],arp1[TOF2GC::SCLRS],arp2[TOF2GC::SCLRS];
  geant slops[2],eacl;
  number shft,ftdel,qtotl[TOF2GC::SCLRS],qsd1[TOF2GC::SCLRS],qsd2[TOF2GC::SCLRS],eanti(0),meanq,rr,qmax;
  number betof,lflgt,cvel(29.979);
  number eacut=0.6;// cut on E-anti (mev). tempor (no calibration)
  number qrcut=6.;// cut on max/mean-charge ratio
  number qtcut=200.;// cut on max mean-charge 
  number dscut=8.;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  number *pntr[TOF2GC::SCLRS];
  static int first(0);
  TOF2RawCluster *ptr;
  AMSAntiCluster *ptra;
//
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  TOF2JobStat::addre(6);
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
  for(i=0;i<TOF2GC::SCLRS;i++)qtotl[i]=0;
  for(i=0;i<TOF2GC::SCLRS;i++)ltim[i]=0;
  cref[0]=TFCAFFKEY.idref[0]%100-1;//BB-ref1(0-11)
  lref[0]=TFCAFFKEY.idref[0]/100-1;//L-ref1(0-3)
//----
  while (ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    if((status&TOFGC::SCBADB3)==0){ //select "good_strr" hits
      if((status&TOFGC::SCBADB2)==0 || (status&TOFGC::SCBADB5)!=0){// 2-sided or recovered
      ilay=(ptr->getntof())-1;
      ibar=(ptr->getplane())-1;
      ptr->getadca(ama);
      TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// convert anode-adc to charge
      qtotl[ilay]=am[0]+am[1];
      qsd1[ilay]=am[0];
      qsd2[ilay]=am[1];
      ptr->getadcd(amd);
      ptr->getsdtm(tm);// raw side-times(A-noncorrected)
      ltim[ilay]=ptr->gettime();// get ampl-corrected time
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      trp1[ilay]=tm[0];
      trp2[ilay]=tm[1];
      arp1[ilay]=ama[0];
      arp2[ilay]=ama[1];
      coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff
      }
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//----
//    Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOF2JobStat::addre(7);
//
// -----> check Anti-counter :
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over ANTICluster hits
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
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
//
// -----> check amplitudes in layers :
//
  for(i=0;i<TOF2GC::SCLRS;i++)pntr[i]=&qtotl[i];
  AMSsortNAG(pntr,TOF2GC::SCLRS);// sort in increasing order
  meanq=0;
  for(i=0;i<(TOF2GC::SCLRS-1);i++)meanq+=(*pntr[i]);
  meanq/=number(TOF2GC::SCLRS-1);
  qmax=*pntr[TOF2GC::SCLRS-1];
  rr=qmax/meanq;
  HF1(1505,geant(rr),1.);
  HF1(1509,geant(meanq),1.);
  if(rr>qrcut)return; // remove events with "spike" dE/dX. 
  if(meanq>qtcut)return; // remove events with z>1 dE/dX. 
//
// -----> remove albedo and very slow part. :
//
  lflgt=TOF2DBc::supstr(1)-TOF2DBc::supstr(2)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(2)
        +TOF2DBc::plnstr(3)+TOF2DBc::plnstr(6));// aver. top/bot flight distance(norm.inc)
  geant ttop=0;
  geant tbot=0;
  if(ltim[0]!=0 && ltim[1]!=0)ttop=ltim[0]+ltim[1];
  if(ltim[2]!=0 && ltim[1]!=3)tbot=ltim[3]+ltim[3];
  betof=0;
  if(ttop!=0 && tbot!=0){
    betof=2.*lflgt/(ttop-tbot)/cvel;//underest.TOFbeta based on prev.calibr.
    HF1(1502,geant(betof),1.);
  }
  if(betof<0.6)return;
  TOF2JobStat::addre(8);
//------>
    number t1,t2,t3,t4,t13,t24;
    conf=1;
//
       ilay=0; //        <-- some hist. for calibration run
    ftdel=TOF2DBc::ftdelf();
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    ama[0]=arp1[ilay];
    ama[1]=arp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    t1=time;
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
//
    t13=t1-t3;
    t24=t2-t4;
    if(TFREFFKEY.reprtf[2]>0){
//      HF1(1550,geant(t1),1.);
//      HF1(1551,geant(t2),1.);
//      HF1(1552,geant(t3),1.);
//      HF1(1553,geant(t4),1.);
    }
//
//------> soft usage of tracker : get momentum to select beta~1
//
    static number pmas(0.938),mumas(0.1057);
    number pmom,bet,chi2,betm;
    number the,phi,rid,err,trl;
    int il,ib,ix,iy;
    geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cosc,cosi,cost,xtr[TOF2GC::SCLRS],ytr[TOF2GC::SCLRS];
    geant scchi2[2],xer[TOF2GC::SCLRS],yer[TOF2GC::SCLRS],lcerr,lvel;
    number ram[4],ramm[4],dum[3],tld[3],tdi[3],trlr[TOF2GC::SCLRS],trlen[TOF2GC::SCLRS-1];
    number ctran,coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
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
    if(ntrk!=1 && TFCAFFKEY.truse>=0)return;// require events with 1 track.
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    } 
    else rid=0;
    pmom=fabs(rid);
    HF1(1500,geant(pmom),1.);
    if(TFCAFFKEY.truse>=0)
       if(pmom<TFCAFFKEY.pcut[0] || pmom>TFCAFFKEY.pcut[1])return;//remove low/too_high mom.
    if(TFCAFFKEY.caltyp==0)bet=pmom/sqrt(pmom*pmom+pmas*pmas);// space calibration
    else bet=pmom/sqrt(pmom*pmom+mumas*mumas);// earth calibration
    HF1(1501,bet,1.);
//
    TOF2JobStat::addre(9);
//
// -----> hard usage of tracker: get track length
//
    bad=0;
    if(TFCAFFKEY.truse==1){
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
          coot[il]=Cout[1];// unrot. (X-meas) planes -> take trk Y-cross as long.coo
          ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
        }
        else {
          coot[il]=Cout[0];// rot. (Y-meas) planes -> take trk X-cross as long.coo
          ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
        }
        dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc
        HF1(1200+il,geant(dy),1.);
        if(TFREFFKEY.reprtf[2]>0){
          HF2(1204+il,geant(coot[il]),geant(dy),1.);
        }
        dx=ctran-TOF2DBc::gettsc(il,ib);//Transv.coo_tracker-Transv.coo_scint
        if(TFREFFKEY.reprtf[2]>0)HF1(1210+il,geant(dx),1.);
        if(fabs(dx)>dscut || fabs(dy)>dscut)bad=1;//too big dist. of tof-tracker
      }
      cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);//average cos from track
      trlen[0]=trlr[0]-trlr[1];//1->2
      trlen[1]=trlr[0]+trlr[2];//1->3
      trlen[2]=trlr[0]+trlr[3];//1->4
    }
//
    if(bad)return;// remove events with big difference of TOF-Tracker coord.
    TOF2JobStat::addre(10);
//
//------> find track impact angle using scint-made transv.coord :
  if(TFCAFFKEY.tofcoo==0){
    ix=0;
    iy=0;
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
        x[ix]=TOF2DBc::gettsc(il,ib);
        zx[ix]=zc[il];
        ix+=1;
      }
      else{                    // rotated (Y-meas) planes
        y[iy]=TOF2DBc::gettsc(il,ib);
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
  if(TFCAFFKEY.tofcoo==1){
    ix=0;
    iy=0;                                
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      TOF2Brcal::scbrcal[il][ib].getd2p(lvel,lcerr);
      if(TOF2DBc::plrotm(il)==0){// unrotated (Long->Yabs) planes(2,3)
        y[iy]=coo[il];
        zy[iy]=zc[il];
        xtr[il]=TOF2DBc::gettsc(il,ib);
        xer[il]=3.46;// bar_wid/sqrt(12.)
        ytr[il]=y[iy];
        yer[il]=lcerr;
        iy+=1;
      }
      else{                    // rotated (Long->Xabs) planes(1,4)
        x[ix]=coo[il];
        zx[ix]=zc[il];
        xtr[il]=x[ix];
        xer[il]=lcerr;
        ytr[il]=TOF2DBc::gettsc(il,ib);
        yer[il]=3.46;
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
      for(il=0;il<TOF2GC::SCLRS;il++){//layer loop
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
  TOF2JobStat::addre(20);
//------
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);
    HF1(1217,cosc,1.);
    HF1(1221,betof/cosc,1.);
    cosi=sqrt(1.+tgx*tgx+tgy*tgy);// this is 1/cos(theta) !!!
//
    if(TFCAFFKEY.truse<=0){ // use track-length from TOF
      tld[0]=(zc[0]-zc[1])*cosi;//1->2
      tld[1]=(zc[0]-zc[2])*cosi;//1->3
      tld[2]=(zc[0]-zc[3])*cosi;//1->4
    }
    else{                    // use track-length from TRACKER
      tld[0]=trlen[0];//1->2
      tld[1]=trlen[1];//1->3
      tld[2]=trlen[2];//1->4
    }
    if(TFREFFKEY.reprtf[2]>0){
      HF1(1524,geant(tld[1]-tld[2]+tld[0]),1.);//Trlen13-Trlen24
      if(TFCAFFKEY.truse > 0){
        HF1(1215,(cost-cosc)/cost,1.);
      }
    }
    tdi[0]=t1-t2;
    tdi[1]=t1-t3;
    tdi[2]=t1-t4;
//
    for(il=0;il<TOF2GC::SCLRS;il++){// new parametrization (also ready for indiv.slopes)
      ib=brnl[il];
      TOF2Brcal::scbrcal[il][ib].getslops(slops);
//      ramm[il]=(slops[0]/qsd1[il]+slops[1]/qsd2[il]);
      ramm[il]=(slops[0]/sqrt(qsd1[il])+slops[1]/sqrt(qsd2[il]));//works sl.better
    }
//
    ilay=0;
    tm[0]=trp1[ilay];
    tm[1]=trp2[ilay];
    time=0.5*(tm[0]+tm[1]);
    relt=time-ftdel;// subtract FT fix.delay
    if(TFREFFKEY.reprtf[2]>0){
      if(conf==1)HF2(1514,geant(ramm[ilay]),geant(relt),1.);
    }
//-----
    for(i=0;i<3;i++)dum[i]=ramm[i+1]-ramm[0];// new parametrization
//
//    if(TFCAFFKEY.caltyp==0)betm=TFCAFFKEY.bmeanpr;// tempor! better use measured one ???
    if(TFCAFFKEY.caltyp==0)betm=bet;
    else betm=TFCAFFKEY.bmeanmu;
//
    TOF2TZSLcalib::fill(betm,brnl,tld,tdi,dum); // fill calib.working arrays
}
//=============================================================================
//=============================================================================
//
number TOF2TDIFcalib::tdiff[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
number TOF2TDIFcalib::tdif2[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
number TOF2TDIFcalib::clong[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
integer TOF2TDIFcalib::nevnt[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
integer TOF2TDIFcalib::nbins[TOF2GC::SCBTPN]={15,16,16,16,15,
                                    16,17,14,15,16,17};//#coord-bins vs bar-type(<=TOF2GC::SCTDBM)
//                                                   to have bin width = 8-9cm
//--------------------------
void TOF2TDIFcalib::init(){ // ----> initialization for TDIF-calibration
  int i,j,id,il,ib,ii,jj,cnum;
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  for(i=0;i<TOF2GC::SCBLMX;i++){
    for(j=0;j<TOF2GC::SCTDBM;j++){
      tdiff[i][j]=0.;
      tdif2[i][j]=0.;
      clong[i][j]=0.;
      nevnt[i][j]=0;
    }
  }
// ---> book histograms for "Bar raw time" (data quality check for TZSL-cal):
//  strcpy(inum,"0123456789");
//  for(il=0;il<TOF2GC::SCLRS;il++){  
//    for(ib=0;ib<TOF2GC::SCMXBR;ib++){
//      strcpy(htit1,"Bar raw time (4Lx1Bar events) for chan(LBB) ");
//      in[0]=inum[il+1];
//      strcat(htit1,in);
//      ii=(ib+1)/10;
//      jj=(ib+1)%10;
//      in[0]=inum[ii];
//      strcat(htit1,in);
//      in[0]=inum[jj];
//      strcat(htit1,in);
//      id=1720+il*TOF2GC::SCMXBR+ib;
//      HBOOK1(id,htit1,80,40.,120.,0.);
//    }
//  }
//  HBOOK2(1780,"T vs 1/Q, L=1",50,0.,0.1,80,40.,120.,0.);
//  HBOOK2(1781,"T vs 1/Q, L=4",50,0.,0.1,80,40.,120.,0.);
//
// ---> book histograms for "Tdiff_mean vs Clong"
//
  strcpy(inum,"0123456789");
  cnum=0;
  for(il=0;il<TOF2DBc::getnplns();il++){  
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      strcpy(htit1,"Mean time_difference(ns) vs coord(cm) for chan(LBB) ");
      in[0]=inum[il+1];
      strcat(htit1,in);
      ii=(ib+1)/10;
      jj=(ib+1)%10;
      in[0]=inum[ii];
      strcat(htit1,in);
      in[0]=inum[jj];
      strcat(htit1,in);
      id=1610+cnum;
      HBOOK1(id,htit1,TOF2GC::SCTDBM,0.,geant(TOF2GC::SCTDBM),0.);
      cnum+=1;//sequential solid numbering
    }
  }
  HBOOK2(1600,"L=1,B=4, Tdif vs Coord",35,-70.,70.,40,-5.,5.,0.);
  HBOOK2(1601,"L=2,B=4, Tdif vs Coord",35,-70.,70.,40,-5.,5.,0.);
  HBOOK1(1602,"Mean Tdiff (all layers/bars)",80,-0.4,0.4,0.);
  HBOOK1(1603,"Slope (all layers/bars)",80,0.02,0.1,0.);
  HBOOK1(1604,"Tdiff sigma in bin",50,0.,2.,0.);
  HBOOK1(1605,"Chi2",50,0.,20.,0.);
}
//------------------------- 
void TOF2TDIFcalib::select(){ // ------> event selection for TDIF-calibration
  integer i,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status;
  integer il,ib,ix,iy,chan;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,crz;
  number sdtm[2],tmsd[TOF2GC::SCLRS],tmsdc[TOF2GC::SCLRS],ama[2],crc,t14,tmss[TOF2GC::SCLRS];
  number am[2],time,qsd1[TOF2GC::SCLRS],qsd2[TOF2GC::SCLRS],q1,q2,qinv,tmin;
  TOF2RawCluster *ptr;
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
//----
  TOF2JobStat::addre(17);
  for(i=0;i<TOF2GC::SCLRS;i++)nbrl[i]=0;
//
  while (ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    if((status&TOFGC::SCBADB1)==0 &&
       (status&TOFGC::SCBADB3)==0){ //select "good_history/good_strr" hits
      if((status&TOFGC::SCBADB2)==0 || (status&TOFGC::SCBADB5)!=0){// 2-sided or recovered
      ilay=(ptr->getntof())-1;
      ibar=(ptr->getplane())-1;
      nbrl[ilay]+=1;
      brnl[ilay]=ibar;
      ptr->getadca(ama);//use  AnodeADC for calibr.(Most are mu or Pr)
      if(ama[0]>0 && ama[1]>0){
        TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// convert anode-adc to charge
        qsd1[ilay]=am[0];
        qsd2[ilay]=am[1];
        ptr->getsdtm(sdtm);// get raw side-times(ns)
        tmsd[ilay]=0.5*(sdtm[0]-sdtm[1]);// side time difference
        tmss[ilay]=0.5*(sdtm[0]+sdtm[1]);// side time sum
        TOF2Brcal::scbrcal[ilay][ibar].td2ctd(tmsd[ilay],ama,0,tmsdc[ilay]);//slew-corr times(from high-ch)
//        tmsdc[ilay]=tmsd[ilay];// use raw side-times(running first time,when slop unknown)
        }
      }
    }
    ptr=ptr->next();
  }// --- end of hits loop --->
//
//------> Select events with bars/layer=1 :
  bad=0;
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOF2JobStat::addre(18);
  t14=tmsdc[0]-tmsdc[3];
//
//------> fill histogr. for indiv. bar time (quality check for TZSL-calibr):
//  for(il=0;il<TOF2GC::SCLRS;il++){
//    ib=brnl[il];
//    time=tmss[il];
//    chan=TOF2GC::SCMXBR*il+ib;
//    HF1(1720+chan,geant(time),1.);
//  }
  q1=qsd1[0];// layer=1
  q2=qsd2[0];
  time=tmss[0];
  ib=brnl[0];
  if(ib==3 && q1>0. && q2>0.){ // for bar=4
    qinv=1./q1+1./q2;
//    HF2(1780,geant(qinv),geant(time),1.);
  }
  q1=qsd1[3];// layer=4
  q2=qsd2[3];
  time=tmss[3];
  ib=brnl[3];
  if(ib==3 && q1>0. && q2>0.){ // for bar=4
    qinv=1./q1+1./q2;
//    HF2(1781,geant(qinv),geant(time),1.);
  }
//---------------------------------------
if(TFCAFFKEY.truse < 0){// <------  use only TOF to find track crossing points
//
//--- find track slope(in projection) using scint.transv.position :
  ix=0;
  iy=0;
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    zc[il]=TOF2DBc::getzsc(il,ib);
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      x[ix]=TOF2DBc::gettsc(il,ib);
      zx[ix]=zc[il];
      ix+=1;
    }
    else{                    // rotated (Y-meas) planes
      y[iy]=TOF2DBc::gettsc(il,ib);
      zy[iy]=zc[il];
      iy+=1;
    }
  }
  tgx=(x[0]-x[1])/(zx[0]-zx[1]);
  tgy=(y[0]-y[1])/(zy[0]-zy[1]);
//
//-----> find track crossing points(i.e. long.coord for 2 other counters):
  for(il=0;il<TOF2GC::SCLRS;il++){
    ib=brnl[il];
    crz=TOF2DBc::getzsc(il,ib);
    if(TOF2DBc::plrotm(il)==0){// unrotated (X-meas) planes
      crc=y[1]+tgy*(crz-zy[1]);// long. crossing coord. in abs. ref.syst.
    }
    else{                     // rotated (Y-meas) planes
      crc=x[1]+tgx*(crz-zx[1]);// long. crossing coord. in abs. ref.syst.
    }
    TOF2TDIFcalib::fill(il,ib,tmsdc[il],crc);
  }
 }
//
else{// <------- use Tracker to find track crossing points:
//
    static number pmas(0.938),mumas(0.1057);
    number pmom,bet,chi2;
    number the,phi,rid,err,trl;
    number coot[TOF2GC::SCLRS];
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
    if(ntrk!=1)return;// require events with 1 track.
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
    } 
    else rid=0;
    pmom=fabs(rid);
    if(TFREFFKEY.reprtf[2]>0)HF1(1500,geant(pmom),1.);
    if(pmom<=1. || pmom>=50.)return;//remove events with suspicious mom.
    TOF2JobStat::addre(19);
    C0[0]=0.;
    C0[1]=0.;
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      C0[2]=zc[il];
      ptrack->interpolate(C0,dir,Cout,the,phi,trl);
      if(TOF2DBc::plrotm(il)==0){
        coot[il]=Cout[1];// unrot. (X-meas) planes -> take Y-cross as long.coo
        crc=Cout[1];
      }
      else {
        coot[il]=Cout[0];// rot. (Y-meas) planes -> take X-cross as long.coo
        crc=Cout[0];
      }
      TOF2TDIFcalib::fill(il,ib,tmsdc[il],crc);
    }
//
 }
}
//------------------------- 
void TOF2TDIFcalib::fill(integer il,integer ib, number td, number co){//--->fill arrays
  integer chan,nbin,btyp;
  number bin,hlen,col;
  geant coh[2];
  if(il<2){//top counters
    coh[0]=TOF2DBc::supstr(3);//center of top honeyc.plate
    coh[1]=TOF2DBc::supstr(4);//center of top honeyc.plate
  }
  else{//bot counters
    coh[0]=TOF2DBc::supstr(5);//center of bot honeyc.plate
    coh[1]=TOF2DBc::supstr(6);//center of bot honeyc.plate
  }
//
  hlen=0.5*TOF2DBc::brlen(il,ib);
  btyp=TOF2DBc::brtype(il,ib);//1->...
  bin=2*hlen/nbins[btyp-1];
// convert abs.coord. to local one:
  if(TOF2DBc::plrotm(il)==0){// unrotated plane
    col=co-coh[1];// local Y-coo(longit)
  }
  else{
    col=co-coh[0];
  }
  if((col<-hlen) || (col>=hlen))return;// out of range
  nbin=(col+hlen)/bin;
  chan=TOF2DBc::barseqn(il,ib);
  nevnt[chan][nbin]+=1;
  clong[chan][nbin]+=col;
  tdiff[chan][nbin]+=td;
  tdif2[chan][nbin]+=(td*td);
  if(il==0 && ib==3)HF2(1600,col,td,1.);
  if(il==1 && ib==3)HF2(1601,col,td,1.);
} 
//------------------------- 
void TOF2TDIFcalib::fit(){//---> get the slope,td0,chi2
  int lspflg(1);//0/1->use single/array  for Lspeed
  integer il,ib,chan,nb,btyp,nev,bins,binsl[TOF2GC::SCLRS];
  number bin,len,co,t,dis,sig,sli,meansl(0),bintot(0),speedl,avsll[TOF2GC::SCLRS];
  number sl[TOF2GC::SCBLMX],t0[TOF2GC::SCBLMX],sumc,sumc2,sumt,sumt2,sumct,sumid,chi2[TOF2GC::SCBLMX];
  geant td[TOF2GC::SCTDBM];
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
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(fname,"tdcalib");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=TOF2RawEvent::getsrun();
  StartTime=TOF2RawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
  HPRINT(1600);
  HPRINT(1601);
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    avsll[il]=0.;
    binsl[il]=0;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      len=TOF2DBc::brlen(il,ib);
      btyp=TOF2DBc::brtype(il,ib);//1->...
      bin=len/nbins[btyp-1];
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
      for(nb=0;nb<TOF2GC::SCTDBM;nb++){
        td[nb]=0;
        nev=nevnt[chan][nb];
   cout<<" "<<nev;
        if(nev>=40){//min.cut on event number in bin
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
        if(chi2[chan]<18. &&
                     fabs(sl[chan])>0.051 &&
                     fabs(sl[chan])<0.071){//only good for averaging
          bintot+=1;
          meansl+=sl[chan];
          avsll[il]+=sl[chan];
          binsl[il]+=1;
        }
        HF1(1602,geant(t0[chan]),1.);
        if(fabs(sl[chan])>0.)HF1(1603,geant(fabs(sl[chan])),1.);
        HF1(1605,geant(chi2[chan]),1.);
      }
      chan+=1;
    }//<------ end of bar loop
//
    if(binsl[il]>0)avsll[il]=fabs(avsll[il])/geant(binsl[il]);
    if(avsll[il]>0.)avsll[il]=1./avsll[il];
    else avsll[il]=15.45;//def.value
  }//<------ end of layer loop
//
  if(bintot>0)meansl/=bintot; // mean slope
  if(meansl!=0)speedl=fabs(1./meansl);// mean light speed
//
//---> print Td vs Coo histograms:
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//      if(chi2[chan]>0)HPRINT(1610+chan);// tempor commented
    chan+=1;
    }
  }
//---  
  cout<<endl<<endl;
  cout<<"TOF2TDIFcalib::fit: for bar 1-12  Tdiff@center (ns):"<<endl<<endl;
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      cout<<" "<<t0[chan];
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TOF2TDIFcalib::fit: for bar 1-12  Light speed (cm/ns):"<<endl<<endl;
  cout<<"Average Lspeed = "<<speedl<<"  , vs layer : "<<avsll[0]<<" "<<avsll[1]
                                   <<" "<<avsll[2]<<" "<<avsll[3]<<endl<<endl;
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      sli=0;
      if(sl[chan]!=0)sli=1./fabs(sl[chan]);
      cout<<" "<<sli;
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TOF2TDIFcalib::fit: for bar 1-12  Chi2 :"<<endl<<endl;
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      cout<<" "<<chi2[chan];
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
//
  HPRINT(1602);
  HPRINT(1603);
  HPRINT(1604);
  HPRINT(1605);
//
//---> print T vs 1/Q histograms:
//  chan=0;
//  for(il=0;il<TOF2DBc::getnplns();il++){
//    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//      HPRINT(1720+chan);
//      chan+=1;
//    }
//  }
//  HPRINT(1780);
//  HPRINT(1781);
//
// ---> write mean light speed and Tdif's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOF2TDIFcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for TDLV-calibration output, fname:"<<fname<<'\n';
  cout<<"Lspeed and individual Tdif's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
  tcfile.setf(ios::fixed);
  tcfile.width(6);
  tcfile.precision(2);// precision for Lspeed and Tdiff's
  if(lspflg){
    chan=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        if(sl[chan]!=0.)tcfile << 1./geant(fabs(sl[chan]))<<" ";
        else tcfile << geant(avsll[il])<<" ";
	chan+=1;
      }
      tcfile << endl;
    }
  }
  else tcfile << geant(speedl)<<endl;
  tcfile << endl;
//
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      tcfile << geant(t0[chan])<<" ";
      chan+=1;
    }
    tcfile << endl;
  }
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOF2TDLVcalib:output file closed !"<<endl;
}
//=============================================================================
//=============================================================================
//
number TOF2AMPLcalib::ambin1[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s1-signals for each ref_bar/bin/event
integer TOF2AMPLcalib::nevenb1[TOF2GC::SCBTBN];// s1 events, accumulated per ref_bar/bin
number TOF2AMPLcalib::ambin2[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s2-signals for each ref_bar/bin/event
integer TOF2AMPLcalib::nevenb2[TOF2GC::SCBTBN];// s2 events, accumulated per ref_bar/bin
number TOF2AMPLcalib::amchan[TOF2GC::SCCHMX][TOF2GC::SCACMX];// side-signals for each channel/event
integer TOF2AMPLcalib::nevenc[TOF2GC::SCCHMX];// numb.of events accum. per channel
geant TOF2AMPLcalib::gains[TOF2GC::SCCHMX];// ch.signals ("0" impact) relat. to ref.ones
geant TOF2AMPLcalib::btamp[2][TOF2GC::SCBTBN];// MostProb bar-signals for each bin(ref.bars,s1/s2) 
geant TOF2AMPLcalib::ebtamp[2][TOF2GC::SCBTBN];// MostProb errors for each bin(ref.bars,s1/s2) 
integer TOF2AMPLcalib::rbls[TOF2GC::SCBTPN]={101,104,401,404,201,
                                    202,204,301,302,303,305};//ref.bar id's (for bar types 1-11)
geant TOF2AMPLcalib::profb[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// bin width for each bar type
geant TOF2AMPLcalib::profp[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// middle of the bin .........
integer TOF2AMPLcalib::nprbn[TOF2GC::SCBTPN]={16,16,16,16,15,16,16,14,15,16,16};//profile-bins vs bar-type
number TOF2AMPLcalib::a2dr[TOF2GC::SCCHMX];// sum of a/d signal-ratios for each channel
number TOF2AMPLcalib::a2dr2[TOF2GC::SCCHMX];// sum of a/d (signal-ratios)**2 
integer TOF2AMPLcalib::nevenr[TOF2GC::SCCHMX];// number of accumulated events for above sums
number TOF2AMPLcalib::ammrfc[TOF2GC::SCBTPN];//sum for mean charge calculation (ref.c.,"0"-bin)
integer TOF2AMPLcalib::nevrfc[TOF2GC::SCBTPN];//events in above sum
geant TOF2AMPLcalib::fpar[10]={0,0,0,0,0,0,0,0,0,0};// parameters to fit
integer TOF2AMPLcalib::nbinr;//working var. for A-profile fit
integer TOF2AMPLcalib::iside;//working var. for A-profile fit
geant TOF2AMPLcalib::fun2nb;//working var. for  fit
number TOF2AMPLcalib::mcharge[TOF2GC::SCPRBM];
number TOF2AMPLcalib::mcoord[TOF2GC::SCPRBM];
number TOF2AMPLcalib::emchar[TOF2GC::SCPRBM];
number TOF2AMPLcalib::aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP];// A-profile parameters(ref.bars)
number TOF2AMPLcalib::clent;
integer TOF2AMPLcalib::cbtyp;//bar-type (1-11)
number TOF2AMPLcalib::arefb[TOF2GC::SCBTPN][TOF2GC::SCACMX];// tot-signals for each ref_bar/event
integer TOF2AMPLcalib::nrefb[TOF2GC::SCBTPN];// numb.of events accum. per ref_bar
//
geant TOF2AMPLcalib::binsta[50];
geant TOF2AMPLcalib::bincoo[50];
integer TOF2AMPLcalib::elbt;
geant TOF2AMPLcalib::elfitp[TOF2GC::SCELFT];
char TOF2AMPLcalib::eltit[60];
//
//--------------------------
void TOF2AMPLcalib::init(){ // ----> initialization for AMPL-calibration 
  integer i,j,il,ib,ii,jj,id,nadd,nbnr,chan;
  geant blen,dd,bw,bl,bh,hll,hhl;
  integer stbns(4);// number of standard bins
  geant bwid[4]={5.,6.,7.,8.};// bin width (first "stbns" bins, closed to the edge(sum=26cm)
//                                (other bin width should be < or about 10cm) 
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  if(TFCAFFKEY.truse < 0){
    cout<<"TOF2AMPLcalib::init: Only TOF info will be used !!!"<<endl;
  }
  else {
    cout<<"TOF2AMPLcalib::init: Tracker info will be used !!!"<<endl;
  }
  strcpy(inum,"0123456789");
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    nevenc[i]=0;
    gains[i]=0.;
    nevenr[i]=0;
    a2dr[i]=0.;
    a2dr2[i]=0.;
  for(j=0;j<TOF2GC::SCACMX;j++){
    amchan[i][j]=0.;
  }
  }
//
  for(i=0;i<TOF2GC::SCBTPN;i++){//bar type loop
    nevrfc[i]=0;
    ammrfc[i]=0;
    nrefb[i]=0;
  for(j=0;j<TOF2GC::SCACMX;j++){
    arefb[i][j]=0.;
  }
  }
//
  for(i=0;i<TOF2GC::SCBTBN;i++){
    nevenb1[i]=0;
    nevenb2[i]=0;
    btamp[0][i]=0.;
    btamp[1][i]=0.;
  for(j=0;j<TOF2GC::SCACMX;j++){
    ambin1[i][j]=0.;
    ambin2[i][j]=0.;
  }
  }
//
//  ---> book hist. for side-signals:
//
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){   
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        hll=25.;
        hhl=345.;
        if((il+1)==2 && (ib+1)==3 && (i+1)==1)hhl=665.;// individ. settings
        if((il+1)==2 && (ib+1)==8)hhl=505.;
        if((il+1)==3 && (ib+1)==10)hhl=1305.;
        if((il+1)==3 && (ib+1)==11 && (i+1)==1)hhl=825.;
        if((il+1)==4 && (ib+1)==3 && (i+1)==2)hhl=505.;
        if((il+1)==4 && (ib+1)==11 && (i+1)==2)hhl=665.;
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
        id=1600+chan;
//        HBOOK1(id,htit1,80,hll,hhl,0.);
        chan+=1;
      }
    }
  }
// ---> book hist. for ref.bars bin-signals (profiles):
  for(i=0;i<TOF2GC::SCBTPN;i++){ // loop over bar types  
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
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
      id=1601+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+i*TOF2GC::SCPRBM+j;
//      HBOOK1(id,htit2,80,0.,960.,0.);
    }
  }
//
  for(i=0;i<TOF2GC::SCBTPN;i++){ // prepare profile bin width array
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
    il=id/100-1;
    ib=id%100-1;
    blen=TOF2DBc::brlen(il,ib);
    nbnr=nprbn[i];//real tot.numb. of bins
    dd=0.;
    for(j=0;j<stbns;j++){ // bin width for the first/last "stbns" standard ones
      dd+=bwid[j];
      profb[i][j]=bwid[j];
      profb[i][nbnr-j-1]=bwid[j];
    }
    nadd=nbnr-2*stbns;// number of additional bins of THE SAME WIDTH !
    bw=(blen-2*dd)/nadd;// width of additional bins
    cout<<"TOF2AMPLcalib::init:Bartype="<<i<<" equal_bins width="<<bw<<endl;
    for(j=0;j<nadd;j++){ // complete bin width array
      profb[i][stbns+j]=bw;
    }
  }
//
  for(i=0;i<TOF2GC::SCBTPN;i++){ // prepare mid.points positions (loc.r.s) of profile bins
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
    il=id/100-1;
    ib=id%100-1;
    blen=TOF2DBc::brlen(il,ib);
    nbnr=nprbn[i];//real tot.numb. of bins
    dd=0.;
    for(j=0;j<nbnr;j++){
      profp[i][j]=dd+0.5*profb[i][j]-0.5*blen;// -blen/2 to go to loc.r.s.
      dd+=profb[i][j];
    }
  }
//
  for(i=0;i<TOF2GC::SCBTPN;i++){// book hist. for longit.imp.point distr.(ref.bar only)
    strcpy(htit1,"Impact point distr.for ref.bar(LBB) ");
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types(<-> ref.bars)
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
    blen=TOF2DBc::brlen(il-1,ib-1);
    bl=-0.5*blen;
    bh=bl+blen;
    HBOOK1(1220+i,htit1,70,bl,bh,0.);
    aprofp[i][0]=50.;//defaults param.(A) for A-profile fit(s1)
    aprofp[i][1]=195.;//              (Lh)
    aprofp[i][2]=0.;//                (Wl)
    aprofp[i][3]=10.;//               (Ll)
    aprofp[i][4]=aprofp[i][0];//defaults param. for A-profile fit(s2)
    aprofp[i][5]=aprofp[i][1];
    aprofp[i][6]=aprofp[i][2];
    aprofp[i][7]=aprofp[i][3];
  }
//
}
//--------------------------------------
//
void TOF2AMPLcalib::select(){ // ------> event selection for AMPL-calibration
  integer i,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector,nanti(0);
  integer il,ib,ix,iy,chan,chnum;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cost,cosc,eacl,edepa,edepd,edep[TOF2GC::SCLRS];
  number ama[2],amd[2],coo[TOF2GC::SCLRS],coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
  number am1[TOF2GC::SCLRS],am2[TOF2GC::SCLRS],am1d[TOF2GC::SCLRS],am2d[TOF2GC::SCLRS],am[2],eanti(0);
  geant ainp[2],dinp[2],cinp,trlen[TOF2GC::SCLRS];
  number ltim[4],tdif[4],trle[4];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq,betof,lflgt;
  number cvel(29.979);// light velocity
  number sigt[4]={0.15,0.15,0.15,0.15};// time meas.accuracy in TOF-layers 
  number eacut=0.6;// cut on E-anti (mev)
  number dscut=8.;// TOF/Tracker-coord. dist.cut (hard usage of tracker)
  number edecut=10.;// max.energy(mev) per layer(to avoid ions)
  TOF2RawCluster *ptr;
  AMSAntiCluster *ptra;
  uinteger Runum(0);
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  Runum=AMSEvent::gethead()->getrun();// current run number
  TOF2JobStat::addre(11);
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrl[i]=0;
    edep[i]=0.;
  }
//
  while(ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    ilay=(ptr->getntof())-1;
    ibar=(ptr->getplane())-1;
    ptr->getadca(ama);// get anode-adc (for 2 sides)
    ptr->getadcd(amd);// get dynode(h)-adc
    TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);// convert anode-adc to charge
    am1[ilay]=am[0];//charge(pC)
    am2[ilay]=am[1];
    edepa=ptr->getedepa();//energy in Anode channel(prev calibration !)
    edepd=ptr->getedepd();//energy in Dynode(h) channel
    edep[ilay]+=edepa;
    if(edepa>0. && edepd>0.){// fill arrays for AVSD-calibration
      ainp[0]=geant(ama[0]);//ADC-channels !!!
      ainp[1]=geant(ama[1]);
      dinp[0]=geant(amd[0]);
      dinp[1]=geant(amd[1]);
      chnum=2*TOF2DBc::barseqn(ilay,ibar);//sequential channels numbering (s=1)
      TOF2AVSDcalib::filltovt(chnum,ainp[0],dinp[0]);
      chnum=2*TOF2DBc::barseqn(ilay,ibar)+1;//sequential channels numbering (s=2)
      TOF2AVSDcalib::filltovt(chnum,ainp[1],dinp[1]);
    }
    if((status&TOFGC::SCBADB1)==0 &&
                  (status&TOFGC::SCBADB3)==0){ //select "good_history/good_strr" hits
      if((status&TOFGC::SCBADB2)==0 || (status&TOFGC::SCBADB5)!=0){// 2-sided or recovered
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
  for(i=0;i<TOF2GC::SCLRS;i++)if(nbrl[i] != 1)bad=1;
  if(bad==1)return; // remove events with bars/layer != 1
  TOF2JobStat::addre(12);
//
// -----> check Anti-counter :
//
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over ANTIRawCluster hits
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
  if(nanti>1)return;// remove events with >1 sector(e>ecut) in Anti
// -----> remove albedo and very slow part. :
//
  lflgt=TOF2DBc::supstr(1)-TOF2DBc::supstr(2)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(2)
        +TOF2DBc::plnstr(3)+TOF2DBc::plnstr(6));// aver. top/bot flight distance(norm.inc)
  geant ttop=0;
  geant tbot=0;
  if(ltim[0]!=0 && ltim[1]!=0)ttop=ltim[0]+ltim[1];
  if(ltim[2]!=0 && ltim[1]!=3)tbot=ltim[3]+ltim[3];
  betof=0;
  if(ttop!=0 && tbot!=0){
    betof=2.*lflgt/(ttop-tbot)/cvel;//under.TOFbeta based on prev.calibr.
//    HF1(1502,geant(betof),1.);
  }
  if(betof<(TFCAFFKEY.tofbetac-0.1))return;// mainly to remove albedo-particles
//
  TOF2JobStat::addre(13);
//
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
    if(ntrk!=1 && TFCAFFKEY.truse>=0)return;// require events with 1 track 
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",0);
    if(ppart){
      ptrack=ppart->getptrack();//get pointer of the track, used in given particle
      ptrack->getParFastFit(chi2,rid,err,the,phi,C0);
      pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
      chargeTracker=pcharge->getchargeTracker();
      chargeTOF=pcharge->getchargeTracker();
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
    HF1(1500,geant(pmom),1.);
    HF1(1501,bet,1.);
//
    bad=0;
    if(pmom<=pcut[0] || pmom>=pcut[1])bad=1;// out of needed mom.range
    if(TFCAFFKEY.truse>=0 && bad==1)return;
    TOF2JobStat::addre(14);
//
    bad=0;
    if(TFCAFFKEY.truse==1){
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
        trlen[il]=trl;
        if(TOF2DBc::plrotm(il)==0){
          coot[il]=Cout[1];// unrot. (X-meas) planes -> take Y-cross as long.coo
          ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
        }
        else {
          coot[il]=Cout[0];// rot. (Y-meas) planes -> take X-cross as long.coo
          ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
        }
        dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc
        HF1(1200+il,geant(dy),1.);
        dx=ctran-TOF2DBc::gettsc(il,ib);//Transv.coo_tracker-Transv.coo_scint
        if(TFREFFKEY.reprtf[2]>0)HF1(1210+il,geant(dx),1.);
        if(fabs(dx)>dscut || fabs(dy)>dscut)bad=1;//too big dist. of tof-tracker
      }
      cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/4);// average cos from track
    }
//
    if(bad)return;//too big difference of TOF-Tracker coord.
    TOF2JobStat::addre(15);
//
//-------> get track impact angle using scint-made long.coord :
//
    ix=0;
    iy=0;                                
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      if(TOF2DBc::plrotm(il)==0){// unrotated (Long->Yabs) planes
        y[iy]=coo[il];
        zy[iy]=zc[il];
        iy+=1;
      }
      else{                    // rotated (Long->-Xabs) planes
        x[ix]=coo[il];
        zx[ix]=zc[il];
        ix+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);
    if(TFREFFKEY.reprtf[2]>0)HF1(1217,cosc,1.);
//
//--------> find beta from TOF :
//
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
    HF1(1502,betof,1.);
    HF1(1205,chsq,1.);
    HF1(1206,tzer,1.);
    if(chsq>8. || betof<TFCAFFKEY.tofbetac)return;//cut on chi2/beta
    TOF2JobStat::addre(30);
//
// ---> look at mass :
//
    number betg(4.);//default (no tracker)
    massq=imass*imass;//default m**2 (no tracker)
    if(TFCAFFKEY.truse == 1){
      massq=pmom*pmom*(1.-betof*betof)/betof/betof;
      betg=pmom/imass;//use "tracker-defined" betg
      HF1(1204,geant(massq),1.);
      if(betof<0.92)HF1(1207,geant(massq),1.);
      HF1(1208,geant(betg),1.);
      HF1(1215,(cost-cosc)/cost,1.);
      HF2(1218,geant(pmom),geant(betof),1.);
    }
//
// ------> normalize charge to normal incidence :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      if(TFCAFFKEY.truse == 1){
        am1[il]*=fabs(cstr[il]);
        am2[il]*=fabs(cstr[il]);
	edep[il]*=fabs(cstr[il]);
      }
      else{
        am1[il]*=fabs(cosc);
        am2[il]*=fabs(cosc);
	edep[il]*=fabs(cosc);
      }
    }
// ------> <-- Remove events with high(ions) or too low edep.:
//
    bad=0;
    for(il=0;il<TOF2GC::SCLRS;il++){ // 
      if(edep[il]<0.1 || edep[il]>edecut)bad+=1;
    }
    if(bad>1)return;// not more than 1 bad layer
    TOF2JobStat::addre(31);
//
// ------> fill working arrays and histograms :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      ainp[0]=geant(am1[il]);
      ainp[1]=geant(am2[il]);
      if(TFCAFFKEY.truse == 1){
        cinp=coot[il];// loc.r.s.!!!
        if(ainp[0]>0. && ainp[1]>0.){
	  TOF2AMPLcalib::fill(il,ib,ainp,cinp);
          TOF2AMPLcalib::fillabs(il,ib,ainp,cinp,massq,betg);
	}
      }
      else{
        cinp=coo[il];// loc.r.s.!!!
        if(ainp[0]>0. && ainp[1]>0.){
          TOF2AMPLcalib::fill(il,ib,ainp,cinp);
          if(betof>0.85)TOF2AMPLcalib::fillabs(il,ib,ainp,cinp,massq,betg);
	}
      }
    }
}
//--------------------------------------
//            ---> program to accumulate data for AMPL-calibration:
void TOF2AMPLcalib::fill(integer il, integer ib, geant am[2], geant coo){
//
  integer i,id,idr,idh,ibt,btyp,nbn,nb,nbc,isb,chan,nev,bchan;
  geant r;
  geant bl,bh,qthrd;
  static geant cbin(15.);// centr. bin half-width for gain calibr.
//
  isb=TOF2DBc::barseqn(il,ib);//bar sequential number(0->...)
  chan=2*isb;//side seq. number
  ibt=TOF2DBc::brtype(il,ib);// bar type (1-11)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
  if(idr==0){
    cerr<<"TOF2AMPLcalib::fill:illegal ref.bar id ! "<<id<<endl;
    return;
  }
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
//    cerr<<"TOF2AMPLcalib::fill:out_of_range !, coo="<<coo<<" l/h="<<bl<<" "<<bh<<endl;
    return;
  }
  bchan=TOF2GC::SCPRBM*btyp+nbc;
//                             ---> fill arrays/hist. for gains:
//
  if(fabs(coo) < cbin){// select only central incidence(+- cbin cm)
    nev=nevenc[chan];
    if(nev<TOF2GC::SCACMX){  
      amchan[chan][nev]=am[0];
      nevenc[chan]+=1;
    }
    nev=nevenc[chan+1];
    if(nev<TOF2GC::SCACMX){  
      amchan[chan+1][nev]=am[1];
      nevenc[chan+1]+=1;
    }
    idh=1600+chan+0;// side-1
    HF1(idh,geant(am[0]),1.);
    idh=1600+chan+1;// side-2
    HF1(idh,geant(am[1]),1.);
  }
//                             ---> fill profile arrays/hist. for ref. bars:
  if(id == idr){// only for ref. sc. bars
    nev=nevenb1[bchan];
    if(nev<TOF2GC::SCACMX && am[0]>0){
      ambin1[bchan][nev]=am[0];
      nevenb1[bchan]+=1;
    }
    nev=nevenb2[bchan];
    if(nev<TOF2GC::SCACMX && am[1]>0){
      ambin2[bchan][nev]=am[1];
      nevenb2[bchan]+=1;
    }
    HF1(1219+ibt,coo,1.);// longit.imp.point distribution
    if(ibt == 2 && fabs(coo) < cbin){
      HF1(1250,am[0],1.);// Q-distr. for ref.bar type=2, s=1
      HF1(1251,am[1],1.);// Q-distr. for ref.bar type=2, s=2
    }
    idh=1601+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+bchan;
//    HF1(idh,geant(am[0]+am[1]),1.);
  }
}
//--------------------------------------
//            ---> program to accumulate data for abs.normalization:
void TOF2AMPLcalib::fillabs(integer il, integer ib, geant am[2], geant coo,
                                     number massq, number betg){
//
  integer i,id,idr,ibt,btyp,mflg(0),bgflg(0),nev;
  static geant cbin(15.);// centr. bin half-width for gain calibr.
  static geant prcut[2]={-10.,10.};
  static geant mucut[2]={-0.3,0.3};
  geant amt,mcut[2];
//
  ibt=TOF2DBc::brtype(il,ib);// bar type (1-10)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
//
  if(id != idr)return;//only for ref.counters
  if(fabs(coo) > cbin)return;// select only central incidence(+- cbin cm)
//
  amt=(am[0]+am[1]);
//
  if(TFCAFFKEY.caltyp==0){// space (prot) calibr.
    mcut[0]=prcut[0];
    mcut[1]=prcut[1];
  }
  else{                    // earth (mu) calibr.
    mcut[0]=mucut[0];
    mcut[1]=mucut[1];
  }
  if(massq>mcut[0] && massq<mcut[1])mflg=1;
  if(betg>TFCAFFKEY.bgcut[0] && betg<TFCAFFKEY.bgcut[1])bgflg=1;
//
  if(id==rbls[1])HF2(1219,geant(betg),amt,1.);// for ref.bar type=2
  if(bgflg==1 && mflg==1){// mass-mip region ok
    nevrfc[btyp]+=1;
    ammrfc[btyp]+=amt;
    nev=nrefb[btyp];
    if(nev<TOF2GC::SCACMX){
      arefb[btyp][nev]=(am[0]+am[1]);
      nrefb[btyp]+=1;
    }
  }
}
//
//-------------------------------------------
//            ---> program to fill H/L channel ratio arrays:
void TOF2AMPLcalib::filla2d(integer il, integer ib, geant am[2], geant amd[2]){
//
  integer i,id,idr,ibt,btyp,chan,isb;
  geant r,qthrd,q0,tauf,t0;
//
  for(i=0;i<2;i++){
    am[i]+=0.5;// tempor  +0.5 to be at the middle of ADC-bin=1
    amd[i]+=0.5;
  }
  isb=TOF2DBc::barseqn(il,ib);//bar sequential number
  chan=2*isb;//side seq. number
  ibt=TOF2DBc::brtype(il,ib);// bar type (1-5)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar

//                             ---> fill arrays for high/low ratios:
  for(i=0;i<2;i++){
    r=0;
    if(amd[i]>5. && am[i]>0.){
      r=am[i]/amd[i];
        a2dr[chan+i]+=r;
        a2dr2[chan+i]+=(r*r);
        nevenr[chan+i]+=1;
    }
  }
}
//--------------------------------------
//            ---> program to get final AMPL-calibration:
void TOF2AMPLcalib::fit(){
//
  integer il,ib,is,i,j,k,n,ii,jj,id,idr,btyp;
  int ic;
  integer ibt,ibn,nbnr,chan,bchan,nev,nm,nmax,nmin;
  geant aref[TOF2GC::SCBTPN][2],ar,aabs[TOF2GC::SCBTPN],mip2q[TOF2GC::SCBTPN];
  number *pntr[TOF2GC::SCACMX];
  number aver;
  geant step[10],pmin[10],pmax[10],sigp[10];
  integer nev1,nev2,npar=2;
  char htit1[60];
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
  cfvn=TFCAFFKEY.cfvers%1000;
  strcpy(fname,"ancalib");
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=TOF2RawEvent::getsrun();
  StartTime=TOF2RawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//
// ---> print "gain"-hist. (side-signals for center bin)
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){   
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        id=1600+chan;
//        HPRINT(id);
        chan+=1;
      }
    }
  }
//
// ---> print impact point hist. for ref. counters:
//
  for(i=0;i<TOF2GC::SCBTPN;i++){
    HPRINT(1220+i);
  }
//
// ---> print "profile"-hist. (tot.signal vs long.bin for ref.counters):
//
//  for(i=0;i<TOF2GC::SCBTPN;i++){ // loop over bar types  
//    nbnr=nprbn[i];//real numb. of bins
//    if(nbnr==0)continue;
//    for(j=0;j<nbnr;j++){ // loop over longit.bins
//      id=1601+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+i*TOF2GC::SCPRBM+j;
//      HPRINT(id);
//    }
//  }
//---------------------------------------------------------------------
// ---> Calculate(fit) most prob. ampl. for each channel (X=0):
//
  int ierp,ip;  
  int ifitp[TOF2GC::SCELFT];
  number llim,hlim,pval,perr;
  char prnam[TOF2GC::SCELFT][6],prnm[6];
  number arglp[10];
  int iarglp[10];
  number pri[TOF2GC::SCELFT],prs[TOF2GC::SCELFT],prl[TOF2GC::SCELFT],prh[TOF2GC::SCELFT],maxv;
  strcpy(prnam[0],"anor1");
  strcpy(prnam[1],"mprob");
  strcpy(prnam[2],"scalf");
  strcpy(prnam[3],"speed");
  pri[0]=10.;
  pri[1]=40.;
  pri[2]=10.;
  pri[3]=0.5;
//
  prs[0]=5.;
  prs[1]=10.;
  prs[2]=1.;
  prs[3]=0.25;
//
  prl[0]=1.;
  prl[1]=1.;
  prl[2]=0.1;
  prl[3]=0.;
//
  prh[0]=1500.;
  prh[1]=500.;
  prh[2]=100.;
  prh[3]=10.;
//
  for(i=0;i<TOF2GC::SCELFT;i++)ifitp[i]=1;
  ifitp[3]=0;
  cout<<endl;
  cout<<"-------------> Start Mp-fit for side signals (X=0): <--------------"<<endl;
  cout<<endl;
// ------------> initialize Minuit:
  MNINIT(5,6,6);
  MNSETI("TOF:Mp-calibration for side-signals");
  arglp[0]=number(-1);
  MNEXCM(melfun,"SET PRINT",arglp,1,ierp,0);
//---
  for(ip=0;ip<TOF2GC::SCELFT;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"TOF:Mp-side-calib: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=number(ip+1);
    if(ifitp[ip]==0){
      ierp=0;
      MNEXCM(melfun,"FIX",arglp,1,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Mp-side-calib: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//---
//----
  char choice[5]=" ";
  int bnn,jmax;
  geant rbnn,bnw,bnl,bnh;
  ic=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(is=0;is<2;is++){
        nev=nevenc[ic];
        aver=0;
        if(nev>=TFCAFFKEY.minev){
          for(k=0;k<nev;k++)pntr[k]=&amchan[ic][k];//pointers to event-signals of chan=i 
          AMSsortNAG(pntr,nev);//sort in increasing order
          nmax=floor(nev*TFCAFFKEY.trcut);// to keep (100*trcut)% of lowest amplitudes
          nmin=floor(nev*0.015);// to remove 1.5 % of lowest amplitudes
          if(nmin==0)nmin=1;
//          for(j=nmin;j<nmax;j++)aver+=(*pntr[j]);
//          if((nmax-nmin)>0)gains[i]=geant(aver/(nmax-nmin));
//
          strcpy(htit1,"X=0 signals distr. for chan(LBBS) ");
          in[0]=inum[il+1];
          strcat(htit1,in);
          ii=(ib+1)/10;
          jj=(ib+1)%10;
          in[0]=inum[ii];
          strcat(htit1,in);
          in[0]=inum[jj];
          strcat(htit1,in);
          in[0]=inum[is+1];
          strcat(htit1,in);
          bnn=20;//        <<--- select limits/binwidth for hist.
          bnl=(*pntr[nmin]);
          bnh=(*pntr[nmax]);
          bnw=(bnh-bnl)/bnn;
          if(bnw<1.){
            bnw=1.;
            bnn=int((bnh-bnl)/bnw);
            bnh=bnl+bnw*bnn;
          }
          HBOOK1(1599,htit1,bnn,bnl,bnh,0.);
          for(j=nmin;j<nmax;j++)HF1(1599,geant((*pntr[j])),1.);
          HUNPAK(1599,binsta,choice,0);
          for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
          maxv=0.;
          jmax=0;
          for(j=0;j<bnn;j++){// find bin with max.stat.
            if(binsta[j]>maxv){
              maxv=binsta[j];
              jmax=j;
            }
          }
          HPRINT(1599);
          strcpy(eltit,htit1);
//---
          prl[1]=bnl;// some more realistic init.values from histogr.
          prh[1]=bnh;
          pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
          prs[1]=bnw;
          pri[0]=maxv;// A
	  prl[0]=1.;
	  prh[0]=2.*maxv;
	  prs[0]=maxv/10;
          elbt=bnn;
//---
          for(ip=0;ip<2;ip++){// <----- reinitialize these 2 parameters:
            strcpy(prnm,prnam[ip]);
            ierp=0;
            MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
            if(ierp!=0){
              cout<<"TOF:Mp-side-calib: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
              exit(10);
            }
          }// <----- end of param. reinit.
//---
          arglp[0]=0.;
          ierp=0;
          MNEXCM(melfun,"MINIMIZE",arglp,0,ierp,0);
          if(ierp!=0){
            cout<<"TOF:Mp-side-calib: MINIMIZE problem !"<<'\n';
            continue;
          }  
          MNEXCM(melfun,"MINOS",arglp,0,ierp,0);
          if(ierp!=0){
            cout<<"TOF:Mp-side-calib: MINOS problem !"<<'\n';
            continue;
          }
          arglp[0]=number(3);
          ierp=0;
          MNEXCM(TOF2AMPLcalib::melfun,"CALL FCN",arglp,1,ierp,0);
          if(ierp!=0){
            cout<<"TOF:Mp-side-calib: final CALL_FCN problem !"<<'\n';
            continue;
          }
          gains[ic]=elfitp[1];
          HDELET(1599);
//
        }// ---> endof min.events check
//
	ic+=1;//sequential numbering of bar sides
//
      }// ---> endof side loop
    }// ---> endof bar loop
  }// ---> endof layer loop
//
// ---> extract most prob. ampl for ref.bar:
//
  for(i=0;i<TOF2GC::SCBTPN;i++){
    id=rbls[i];
    if(id==0)continue;//skip dummy bar types
    il=id/100-1;
    ib=id%100-1;
    chan=2*TOF2DBc::barseqn(il,ib);
    nev1=nevenc[chan];
    nev2=nevenc[chan+1];
    if(nev1>=(2*TFCAFFKEY.minev) && nev2>=(2*TFCAFFKEY.minev)){
      aref[i][0]=gains[chan];
      aref[i][1]=gains[chan+1];
    }
    else{
      aref[i][0]=0;
      aref[i][1]=0;
    }
  }
//
// ---> calc. relative gains:
//
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      btyp=TOF2DBc::brtype(il,ib)-1;
      id=100*(il+1)+ib+1;
      idr=rbls[btyp];// ref.bar id for given bar
      if(idr==0)continue;//skip dummy bar types
      chan=2*TOF2DBc::barseqn(il,ib);
      for(i=0;i<2;i++){
        ar=aref[btyp][i];
        if(ar>0. && gains[chan+i]>0.)gains[chan+i]/=ar;
        else gains[chan+i]=1.;//default value
        if(id != idr)HF1(1252,gains[chan+i],1.);
      }
    }
  }
//-------------------------------------------------------------
//
// ---> Calculate(fit) most prob. ampl. for each X-bin of ref.bars(s1/s2):
//
  integer j1,j2;
  pri[1]=80.;
  pri[2]=20.;
  prh[1]=1000.;
  prh[2]=100.;
  cout<<endl;
  cout<<"-------------> Start Mp-fit in ref.bar bins: <--------------"<<endl;
  cout<<endl;
// ------------> initialize Minuit:
  MNINIT(5,6,6);
  MNSETI("TOF:Mp-calib. for ref.bar bin-signals");
  arglp[0]=number(-1);
  MNEXCM(melfun,"SET PRINT",arglp,1,ierp,0);
//---
  for(ip=0;ip<TOF2GC::SCELFT;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"TOF:Mp-bin-calib: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=number(ip+1);
    if(ifitp[ip]==0){
      ierp=0;
      MNEXCM(melfun,"FIX",arglp,1,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Mp-bin-calib: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//---
//----
  for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){// <---loop over bar-types
    nbnr=nprbn[ibt];//real numb. of bins for bar-type ibt
    if(nbnr==0)continue;//skip dummy bar types
    for(int isd=0;isd<2;isd++){// <---side loop
    iside=isd;
    for(ibn=0;ibn<nbnr;ibn++){ // <--- loop over longit.bins
      bchan=ibt*TOF2GC::SCPRBM+ibn;
      if(isd==0)nev=nevenb1[bchan];
      else nev=nevenb2[bchan];
      aver=0;
      if(nev>=TFCAFFKEY.minev){
        if(isd==0)for(k=0;k<nev;k++)pntr[k]=&ambin1[bchan][k];//pointers to event-signals of chan=bchan
        else for(k=0;k<nev;k++)pntr[k]=&ambin2[bchan][k];//pointers to event-signals of chan=bchan
        AMSsortNAG(pntr,nev);//sort in increasing order
        nmax=floor(nev*TFCAFFKEY.trcut);// to keep (100*trcut)% of lowest amplitudes
        nmin=floor(nev*0.015);// to remove 1.5 % of lowest amplitudes
        if(nmin==0)nmin=1;
//        for(k=nmin;k<nmax;k++)aver+=(*pntr[k]);
//        if((nmax-nmin)>0)btamp[bchan]=geant(aver/(nmax-nmin));
//
        if(isd==0)strcpy(htit1,"Q-s1 for ref.bar type/bin(TBB) ");
	else strcpy(htit1,"Q-s2 for ref.bar type/bin(TBB) ");
	ii=ibt+1;
        in[0]=inum[ii/10];
        strcat(htit1,in);
	in[0]=inum[ii%10];
        strcat(htit1,in);
        ii=(ibn+1)/10;
        jj=(ibn+1)%10;
        in[0]=inum[ii];
        strcat(htit1,in);
        in[0]=inum[jj];
        strcat(htit1,in);
        bnn=20;//        <<--- select limits/binwidth for hist.
        bnl=(*pntr[nmin]);
        bnh=(*pntr[nmax]);
        bnw=(bnh-bnl)/bnn;
        if(bnw<1.){
          bnw=1.;
          bnn=int((bnh-bnl)/bnw);
          bnh=bnl+bnw*bnn;
        }
        HBOOK1(1599,htit1,bnn,bnl,bnh,0.);
        for(n=nmin;n<nmax;n++)HF1(1599,geant((*pntr[n])),1.);
        HUNPAK(1599,binsta,choice,0);
        for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
        maxv=0.;
        jmax=0;
        for(j=0;j<bnn;j++){// find bin with max.stat.
          if(binsta[j]>maxv){
            maxv=binsta[j];
            jmax=j;
          }
        }
        HPRINT(1599);
        strcpy(eltit,htit1);
//---
        prl[1]=bnl;// some more realistic init.values from histogr.
        prh[1]=bnh;
        pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
        prs[1]=bnw;
        pri[0]=maxv;// A
	prl[0]=1.;
	prh[0]=2.*maxv;
        prs[0]=maxv/10;
        elbt=bnn;
//---
        for(ip=0;ip<2;ip++){// <----- reinitialize these 2 param.
          strcpy(prnm,prnam[ip]);
          ierp=0;
          MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
          if(ierp!=0){
            cout<<"TOF:Mp-bin-calib: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
            exit(10);
          }
        }// <----- end of param. reinit.
//---
        arglp[0]=0.;
        ierp=0;
        MNEXCM(melfun,"MINIMIZE",arglp,0,ierp,0);
        if(ierp!=0){
          cout<<"TOF:Mp-bin-calib: MINIMIZE problem !"<<'\n';
          continue;
        }  
        MNEXCM(melfun,"MINOS",arglp,0,ierp,0);
        if(ierp!=0){
          cout<<"TOF:Mp-bin-calib: MINOS problem !"<<'\n';
          continue;
        }
        arglp[0]=number(3);
        ierp=0;
        MNEXCM(TOF2AMPLcalib::melfun,"CALL FCN",arglp,1,ierp,0);
        if(ierp!=0){
          cout<<"TOF:Mp-bin-calib: final CALL_FCN problem !"<<'\n';
          continue;
        }
	MNPOUT(2,prnm,pval,perr,llim,hlim,ierp);//error on "mprob" parameter
	if(perr<=0){
	  cout<<"TOF:Mp-bin-calib:ZeroMprobError!!"<<endl;
	  perr=1;
	}
        btamp[isd][bchan]=elfitp[1];//store Mp 
        ebtamp[isd][bchan]=perr;//store Mp error 
        HDELET(1599);
//
      }
      HF1(1254+2*ibt+isd,profp[ibt][ibn],btamp[isd][bchan]);
    }//---> end of loop for longit.bins
    }//---> end of side loop
  }//---> end of loop for bar-types
//
//--------> fit Impact-point profiles for ref. bars(for nonuniform.corr.):
//
  int ier;  
  int ifit[TOF2GC::SCPROFP];
  char pnam[TOF2GC::SCPROFP][6],pnm[6];
  number argl[10];
  int iargl[10];
  number start[TOF2GC::SCPROFP],pstep[TOF2GC::SCPROFP],plow[TOF2GC::SCPROFP],phigh[TOF2GC::SCPROFP];
  strcpy(pnam[0],"aside");
  strcpy(pnam[1],"lenhi");
  strcpy(pnam[2],"wmixt");
  strcpy(pnam[3],"lenlo");
//
  start[0]=100.;
  start[1]=195.;
  start[2]=0.;//in AMS01 was 0.28
  start[3]=15.;
//
//  if(AMSJob::gethead()->isMCData()){ // tempor solution for MC
//    start[1]=190.;
//    start[3]=5.;
//  }
//
  pstep[0]=10.;
  pstep[1]=10.;
  pstep[2]=0.1;
  pstep[3]=1.;
//
  plow[0]=1.;
  plow[1]=1.;
  plow[2]=-0.001;
  plow[3]=1.;
//
  phigh[0]=500.;
  phigh[1]=1000.;
  phigh[2]=0.5;
  phigh[3]=50.;
//
  for(i=0;i<TOF2GC::SCPROFP;i++)ifit[i]=1;
  ifit[2]=0;
  ifit[3]=0;
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("Ampl<->Position dependence calibration for TOF-system");
  argl[0]=number(-1);
  MNEXCM(mfun,"SET PRINT",argl,1,ier,0);
  for(i=0;i<TOF2GC::SCPROFP;i++){
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
  for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){// <------ loop over bar-types
    id=rbls[ibt];
    if(id==0)continue;//skip dummy bar types
    nbnr=nprbn[ibt];//numb. of bins for given bar type
    il=id/100-1;
    ib=id%100-1;
    clent=TOF2DBc::brlen(il,ib);
    cbtyp=ibt+1;
    cout<<endl;
    cout<<"---> Start minim. for btyp/length="<<cbtyp<<" "<<clent<<" nbins="<<nbnr<<endl<<endl;
//
    for(int isd=0;isd<2;isd++){// <----- side loop
      iside=isd;
      cout<<"    side="<<isd+1<<"--->"<<endl<<endl;
//
      nbinr=0;
      for(j=0;j<nbnr;j++){ // <--- loop over longit.bins to load coo,charge..
        bchan=ibt*TOF2GC::SCPRBM+j;
        if(isd==0)nev=nevenb1[bchan];
        else nev=nevenb2[bchan];
       cout<<"    events/bin="<<nev<<endl;
        if(nev>=TFCAFFKEY.minev){
          mcharge[nbinr]=btamp[isd][bchan];
          emchar[nbinr]=ebtamp[isd][bchan];
          mcoord[nbinr]=profp[ibt][j];
          cout<<"       q/coo="<<mcharge[nbinr]<<" "<<mcoord[nbinr]<<endl;
          nbinr+=1;
        }
      }// ---> endof bins loop
//
      cout<<"    Good statistics bins:"<<nbinr<<endl;
      if(nbinr<5)goto nextsd;//too few bins, skip this side
//
      for(i=0;i<TOF2GC::SCPROFP;i++){//<--- reinitialize all non-fixed parameters
        if(ifit[i]==1){
          strcpy(pnm,pnam[i]);
	  if(i==0){// better "0" value for 1st param.
	    if(isd==0)start[i]=mcharge[0];
	    if(isd==1)start[i]=mcharge[nbinr-1];
	  }
//	  cout<<"Prof.fit:StartParN/val="<<i+1<<" "<<start[i]<<" pl/ph="<<plow[i]<<" "<<phigh[i]<<endl;
          ier=0;
          MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);
          if(ier!=0){
            cout<<"TOF-calib:LprofileFit: Param-init problem for par-id="<<pnam[i]<<'\n';
            exit(10);
          }
	}
      }// ---> endof reinitialize
//
// ------> start minimization:
      argl[0]=0.;
      ier=0;
      MNEXCM(mfun,"MINIMIZE",argl,0,ier,0);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: MINIMIZE problem !"<<'\n';
        goto nextsd;
      }  
      MNEXCM(mfun,"MINOS",argl,0,ier,0);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: MINOS problem !"<<'\n';
        goto nextsd;
      }
      argl[0]=number(3);
      ier=0;
      MNEXCM(TOF2AMPLcalib::mfun,"CALL FCN",argl,1,ier,0);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: final CALL_FCN problem !"<<'\n';
        goto nextsd;
      }
 nextsd:
      cout<<endl<<endl;
    } // -----> endof side loop
  }// ------> endof btype loop
//------------------------------------------------------------------
//
//----> calc. mean charge for ref.counters (for abs.calibr):
//
  geant elref(1.65);// ref. Elos(Mp(mev),norm.incidence) for mip-region
  pri[1]=80.;
  pri[2]=10.;
  prh[1]=1000.;
  prh[2]=100.;
  cout<<endl;
  cout<<"-------------> Start Mp-fit(X=0) for ref.bars: <---------------"<<endl;
  cout<<endl;
// ------------> initialize Minuit:
  MNINIT(5,6,6);
  MNSETI("TOF:Mp-fit for ref.bar abs. calibration");
  arglp[0]=number(-1);
  MNEXCM(melfun,"SET PRINT",arglp,1,ierp,0);
//---
  for(ip=0;ip<TOF2GC::SCELFT;ip++){// <----- initialize param.
    strcpy(prnm,prnam[ip]);
    ierp=0;
    MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
    if(ierp!=0){
      cout<<"TOF:Abs-calib: Param-init problem for par-id="<<prnam[ip]<<'\n';
      exit(10);
    }
    arglp[0]=number(ip+1);
    if(ifitp[ip]==0){
      ierp=0;
      MNEXCM(melfun,"FIX",arglp,1,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Abs-calib: Param-fix problem for par-id="<<prnam[ip]<<'\n';
        exit(10);
      }
    }
  }// <----- end of param. init.
//---
//----
  for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){//loop over bar types
    id=rbls[ibt];//ref.bar id
    if(id==0)continue;//skip dummy bar types
    aabs[ibt]=0.;
    mip2q[ibt]=100.;//default value
    nev=nrefb[ibt];
    cout<<endl;
    cout<<"Abs.calib.fit: ibt/events="<< (ibt+1)<<" "<<nev<<endl;
    cout<<endl;
    if(nev>=TFCAFFKEY.minev){
      for(k=0;k<nev;k++)pntr[k]=&arefb[ibt][k];//pointers to event-signals of chan=bchan
      AMSsortNAG(pntr,nev);//sort in increasing order
      nmax=floor(nev*TFCAFFKEY.trcut);// to keep (100*trcut)% of lowest amplitudes
      nmin=floor(nev*0.015);// to remove 1.5 % of lowest amplitudes
      if(nmin==0)nmin=1;
      strcpy(htit1,"Q-tot(X=0) for ref.bar type ");
      in[0]=inum[ibt+1];
      strcat(htit1,in);
      bnn=20;//        <<--- select limits/binwidth for hist.
      bnl=(*pntr[nmin]);
      bnh=(*pntr[nmax]);
      bnw=(bnh-bnl)/bnn;
      if(bnw<1.){
        bnw=1.;
        bnn=int((bnh-bnl)/bnw);
        bnh=bnl+bnw*bnn;
      }
      HBOOK1(1599,htit1,bnn,bnl,bnh,0.);
      for(n=nmin;n<nmax;n++)HF1(1599,geant((*pntr[n])),1.);
      HUNPAK(1599,binsta,choice,0);
      for(j=0;j<bnn;j++)bincoo[j]=bnl+0.5*bnw+bnw*j;
      maxv=0.;
      jmax=0;
      for(j=0;j<bnn;j++){// find bin with max.stat.
        if(binsta[j]>maxv){
          maxv=binsta[j];
          jmax=j;
        }
      }
      HPRINT(1599);
      strcpy(eltit,htit1);
//---
      prl[1]=bnl;// some more realistic init.values from histogr.
      prh[1]=bnh;
      pri[1]=bnl+0.5*bnw+bnw*jmax;//Mp
      prs[1]=bnw;
      pri[0]=maxv;// A
      prl[0]=1.;
      prh[0]=2.*maxv;
      prs[0]=maxv/10;
      elbt=bnn;
//---
      for(ip=0;ip<2;ip++){// <----- reinitialize these 2 param.
        strcpy(prnm,prnam[ip]);
        ierp=0;
        MNPARM((ip+1),prnm,pri[ip],prs[ip],prl[ip],prh[ip],ierp);
        if(ierp!=0){
          cout<<"TOF:Abs-calib: Param-reinit problem for par-id="<<prnam[ip]<<'\n';
          exit(10);
	}
      }// <----- end of param. reinit.
//---
      arglp[0]=0.;
      ierp=0;
      MNEXCM(melfun,"MINIMIZE",arglp,0,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Abs-calib: MINIMIZE problem !"<<'\n';
        continue;
      }  
      MNEXCM(melfun,"MINOS",arglp,0,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Mp-bin-calib: MINOS problem !"<<'\n';
        continue;
      }
      arglp[0]=number(3);
      ierp=0;
      MNEXCM(TOF2AMPLcalib::melfun,"CALL FCN",arglp,1,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Abs-calib: final CALL_FCN problem !"<<'\n';
        continue;
      }
      aabs[ibt]=elfitp[1];
      mip2q[ibt]=aabs[ibt]/elref;//(pC/mev)
      HDELET(1599);
//
      
    }
  }
//-------------------------------------------------------
//
// ---> calculate/print mean hihg/low chan.gain ratios (AVSD-calib):
//
  number a2d[TOF2GC::SCCHMX],a2do[TOF2GC::SCCHMX],avsl,avof;
//
// ---> calculate/print Low-High channel correl. param.(AVSD-calibr):
//
  TOF2AVSDcalib::fit(a2d,a2do,avsl,avof);
//
//
//         <-- put aver.values for low statist.channels
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        if(a2d[chan]==0.)a2d[chan]=avsl;
        if(a2do[chan]==0.)a2do[chan]=avof;
	chan+=1;//seq.numbering of bar sides
      }
    }
  }
//
//--------------------------------------------------
//
  printf("\n");
  printf(" ===============>  Relative gains :\n");
  printf("\n");
  ic=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib+1;
      printf("%6d",nevenc[chan]);
    }
    printf("\n");
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib+1;
      printf("%6.3f",gains[chan]);
    }
    printf("\n");
    printf("\n");
    ic+=2*TOF2DBc::getbppl(il);
  }
//
// ---------
  cout<<" Absolute calibration results :"<<endl;
  cout<<endl;
  cout<<"Events in ref.bars :"<<endl;
  for(i=0;i<TOF2GC::SCBTPN;i++)printf("%6d",nrefb[i]);
  printf("\n");
  printf("\n");
  cout<<"Mean charge in ref.bars (centr.bin):"<<endl;
  for(i=0;i<TOF2GC::SCBTPN;i++)printf("%7.2f",aabs[i]);
  printf("\n");
  printf("\n");
  cout<<"Convertion factors mip2q (pC/MeV):"<<endl;
  for(i=0;i<TOF2GC::SCBTPN;i++)printf("%7.2f",mip2q[i]);
  printf("\n");
  printf("\n");
  cout<<endl;
// --------- 
//
// ---> write H/L-ratios(or slop/offs), gains and mip2q's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOF2AMPLcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for AMPL-calibration output, fname:"<<fname<<'\n';
  cout<<"Indiv. channel a2dr/gain-param., mip2q's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
//
  tcfile.setf(ios::fixed);
  tcfile.width(6);
//                      <-- for High_to_Low chan. calibr.
    tcfile.precision(2);// precision for H/L ratio
    ic=0;
    for(il=0;il<TOF2DBc::getnplns();il++){ // <--- H/L-ratios:
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        chan=ic+2*ib;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        chan=ic+2*ib+1;
        tcfile << a2d[chan]<<" ";
      }
      tcfile << endl;
      ic+=2*TOF2DBc::getbppl(il);
    }
    tcfile << endl;
//
//
  tcfile.precision(3);// precision for gains
  ic=0;
  for(il=0;il<TOF2DBc::getnplns();il++){ // <--- gains:
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ic+2*ib+1;
      tcfile << gains[chan]<<" ";
    }
    tcfile << endl;
    ic+=2*TOF2DBc::getbppl(il);
  }
  tcfile << endl;
//
//
  tcfile.precision(2);// precision for mip2q's
  for(btyp=0;btyp<TOF2GC::SCBTPN;btyp++){ // <--- mip2q's
    tcfile << mip2q[btyp]<<" ";
  }  
  tcfile << endl;
  tcfile << endl;
//
  for(btyp=0;btyp<TOF2GC::SCBTPN;btyp++){ // <--- A-profile fit-parameters
    for(i=0;i<2*TOF2GC::SCPROFP;i++)tcfile << aprofp[btyp][i]<<" ";
    tcfile << endl;
  }  
  tcfile << endl;
//
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOF2AMPLcalib:output file closed !"<<endl;
//
}
//-----
// This is standard Minuit FCN for uniformity fit:
void TOF2AMPLcalib::mfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
  if(iside==0){
    for(i=0;i<nbinr;i++){
      ff=(mcharge[i]-(x[0]*(exp(-(clent/2.+mcoord[i])/x[1])
        +x[2]*exp(-(clent/2.+mcoord[i])/x[3]))))/emchar[i];
      f+=(ff*ff);
    }
  }
  else{
    for(i=0;i<nbinr;i++){
      ff=(mcharge[i]-(x[0]*(exp(-(clent/2.-mcoord[i])/x[1])
        +x[2]*exp(-(clent/2.-mcoord[i])/x[3]))))/emchar[i];
      f+=(ff*ff);
    }
  }
  if(flg==3){
    f=sqrt(f/number(nbinr));
    cout<<"    Fit::Btype="<<cbtyp<<" side="<<iside+1<<"  funct:nbins="<<f<<endl;
    for(i=0;i<TOF2GC::SCPROFP;i++){
      aprofp[cbtyp-1][iside*TOF2GC::SCPROFP+i]=x[i];
      cout<<"    parnumb/par="<<i<<" "<<x[i]<<endl;
    }
  }
}
//-----
// This is standard Minuit FCN for Landau fit:
void TOF2AMPLcalib::melfun(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number fun,sig2,lam;
  f=0.;
  for(i=0;i<elbt;i++){// loop over hist.bins
    sig2=binsta[i];
    if(sig2==0.)sig2=1.;
    lam=(bincoo[i]-x[1])/x[2];
    fun=x[0]*exp(x[3])*exp(-x[3]*(lam+exp(-lam)));
    f+=pow((binsta[i]-fun),2)/sig2;
  }
  if(flg==3){
    f=sqrt(f/number(elbt));
    cout<<eltit<<"  Quality of fit (funct/nbins)->"<<f<<endl;
    for(i=0;i<TOF2GC::SCELFT;i++){
      elfitp[i]=x[i];
      cout<<" par.number/par="<<i<<" "<<elfitp[i]<<endl;
    }
    cout<<endl<<endl;
  }
}
//
//=========================================================================
number TOF2STRRcalib::dtin[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];//for mean Tin calc. in bin (for each channel)
number TOF2STRRcalib::dtinq[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];//for sigma Tin calc. in bin (for each channel)
integer TOF2STRRcalib::nevnt[TOF2GC::SCCHMX][TOF2GC::SCSRCHB];// events in bin,channel
integer TOF2STRRcalib::nevtot[TOF2GC::SCCHMX];       // total number of analized events
integer TOF2STRRcalib::nevnt2[TOF2GC::SCCHMX];// for f/s-tdc time difference
number TOF2STRRcalib::fstdif[TOF2GC::SCCHMX];
number TOF2STRRcalib::fstdif2[TOF2GC::SCCHMX];
number TOF2STRRcalib::sbins[TOF2GC::SCCHMX];// for "points-fit" method
number TOF2STRRcalib::ssumc[TOF2GC::SCCHMX];
number TOF2STRRcalib::ssumt[TOF2GC::SCCHMX];
number TOF2STRRcalib::ssumid[TOF2GC::SCCHMX];
number TOF2STRRcalib::ssumct[TOF2GC::SCCHMX];
number TOF2STRRcalib::ssumc2[TOF2GC::SCCHMX];
number TOF2STRRcalib::ssumt2[TOF2GC::SCCHMX];
//----------
void TOF2STRRcalib::init(){
  integer i,j,il,ib,id,ii,jj,chan;
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
//  ---> book hist. stretcher Tout vs Tin :
//
  mrf=0;
  if(AMSJob::gethead()->isRealData())mrf=1;
//
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){   
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
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
        id=1600+chan;
        HBOOK2(id,htit1,50,til[mrf],tih[mrf],50,tol[mrf],toh[mrf],0.);
	chan+=1;
      }
    }
  }
//
// ---> clear arrays:
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
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
    for(j=0;j<TOF2GC::SCSRCHB;j++){
      dtin[i][j]=0.;
      dtinq[i][j]=0.;
      nevnt[i][j]=0;
    }
    nevtot[i]=0;
  }
} 
//----------
void TOF2STRRcalib::fill(integer ichan, number tm[3]){
  number dti,dto;
  integer idto,idtol[2];
//
  assert(ichan>=0 && ichan<TOF2GC::SCCHMX);
  if(AMSJob::gethead()->isRealData()){
    idtol[0]=TOF2GC::SCSRCLB;
    idtol[1]=TOF2GC::SCSRCHB;
  }
  else{
    idtol[0]=TOF2GC::SCSRCLBMC;
    idtol[1]=TOF2GC::SCSRCHBMC;
  }
  nevtot[ichan]+=1;
  dti=tm[0]-tm[1];
  dto=tm[1]-tm[2];
  idto=integer(dto/TOF2GC::SCSRCTB);// time bin = SCSRCTB ns (Tout)
  if(idto>=idtol[0] && idto<idtol[1] && dti>20. && dti<100.){
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
void TOF2STRRcalib::fill2(integer ichan, number tdif){
  nevnt2[ichan]+=1;
  fstdif[ichan]+=tdif;
  fstdif2[ichan]+=(tdif*tdif);
}
//-----------
void TOF2STRRcalib::outp(){
  integer i,j,il,ib,id,ic,stat(0),idtol[2],dbsta,chan,chmax;
  number t0,sl,t,tq,co,dis,nev,bins;
  number sumc,sumt,sumct,sumc2,sumt2,sumid;
  number strr[TOF2GC::SCCHMX],offs[TOF2GC::SCCHMX],chi2[TOF2GC::SCCHMX];
  number strr2[TOF2GC::SCCHMX],offs2[TOF2GC::SCCHMX],chi22[TOF2GC::SCCHMX];
  number chi2mx(50.);//max.acceptable chi2
  int replf(0),method(0);// <------ method number to use for fit (0/1->bins/points)
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
  strcpy(fname,"srcalib");
  cfvn=TFCAFFKEY.cfvers%1000;
  dig=cfvn/100;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig];
  strcat(fname,in);
  dig=cfvn%10;
  in[0]=inum[dig];
  strcat(fname,in);
  if(AMSJob::gethead()->isMCData())strcat(fname,vers1);
  else strcat(fname,vers2);
  strcat(fname,".cof");
//
//--> get run/time of the first event
//
  StartRun=TOF2RawEvent::getsrun();
  StartTime=TOF2RawEvent::getstime();
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  if(AMSJob::gethead()->isRealData()){
    idtol[0]=TOF2GC::SCSRCLB;
    idtol[1]=TOF2GC::SCSRCHB;
  }
  else{
    idtol[0]=TOF2GC::SCSRCLBMC;
    idtol[1]=TOF2GC::SCSRCHBMC;
  }
//
  chmax=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    chmax+=TOF2DBc::getbppl(il);
  }
  chmax*=2;//max.number of sc.channels
//
// <-------------------------- "bin-method"
  for(ic=0;ic<chmax;ic++){ // <-- chan. loop
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
      co=(number(j)+0.5)*TOF2GC::SCSRCTB;// mid. of Tout bin
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
//---
  printf("\n\n");
  printf("Stretcher ratios: bin-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Chi2: bin-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Offsets(ns),bin-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
//
// <----------------------------"points=method"
//
  dis=pndis;// rms**2 (ns**2)
  for(ic=0;ic<chmax;ic++){ // <-- chan. loop
    t0=0.;
    sl=0.;
    strr2[ic]=0.;
    offs2[ic]=0.;
    chi22[ic]=0.;
    sumc=ssumc[ic]/dis;
    sumt=ssumt[ic]/dis;
    sumid=ssumid[ic]/dis;
    sumct=ssumct[ic]/dis;
    sumc2=ssumc2[ic]/dis;
    sumt2=ssumt2[ic]/dis;
    bins=sbins[ic];
//
    if(bins>=20.){// really number of events in channel ic
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi22[ic]=sumt2+t0*t0*sumid+sl*sl*sumc2
        -2.*t0*sumt-2.*sl*sumct+2.*t0*sl*sumc;
      chi22[ic]/=(bins-2.);
      if(sl>0.){
        strr2[ic]=1./sl;
        offs2[ic]=-t0/sl;
      }
      HF1(1205,geant(strr2[ic]),1.);
      HF1(1206,geant(offs2[ic]),1.);
      HF1(1207,geant(chi22[ic]),1.);
    }
  }//--> end of chan. loop
//---
  printf("\n\n");
  printf("Stretcher ratios: points-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",strr2[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",strr2[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Chi2: points-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",chi22[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",chi22[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Offsets(ns),pnt-methode :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 6.1f",offs2[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 6.1f",offs2[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
//
//<----- combine two methodes(bin-meth. is main):
//
  for(ic=0;ic<chmax;ic++){ // <-- chan. loop
    replf=0;
    if(chi2[ic]>6. || strr[ic]==0.){//replace by better chi2 from pnt.methode
      if(strr[ic]==0. && chi22[ic]>0.)replf=1;
      if(chi2[ic]>0. && chi22[ic]<chi2[ic] && chi22[ic]>0.)replf=1;
    }
    if(replf==1){//cancel replacement if have bad strr in point-methode
      if(strr2[ic]>55. || strr2[ic]<40.)replf=0;
    }
    if(replf){
      chi2[ic]=chi22[ic];
      strr[ic]=strr2[ic];
      offs[ic]=offs2[ic];
    }
    if(strr[ic]>0.){
      HF1(1208,geant(strr[ic]),1.);
      HF1(1209,geant(offs[ic]),1.);
      HF1(1210,geant(chi2[ic]),1.);
    }
  }
//
//<----- combine two methodes(points-meth. is main):
//
//  for(ic=0;ic<chmax;ic++){ // <-- chan. loop
//    replf=0;
//    if(chi22[ic]>3.){//replace by better chi2 from bin-methode
//      if(chi2[ic]<chi22[ic] && chi2[ic]>0.)replf=1;
//    }
//    if(replf==0){//force replacement if bad strr
//      if(strr2[ic]>47. || strr2[ic]<40.5)replf=1;
//    }
//    if(replf){
//      chi22[ic]=chi2[ic];
//      strr2[ic]=strr[ic];
//      offs2[ic]=offs[ic];
//    }
//    chi2[ic]=chi22[ic];
//    strr[ic]=strr2[ic];
//    offs[ic]=offs2[ic];
//    if(strr[ic]>0.){
//      HF1(1208,geant(strr[ic]),1.);
//      HF1(1209,geant(offs[ic]),1.);
//      HF1(1210,geant(chi2[ic]),1.);
//    }
//  }
//------------
  printf("\n\n");
  printf("===========> Channels STRR-calibration report :\n\n");
  printf("\n");
  printf("Event/channel collected :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 6d",nevtot[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 6d",nevtot[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Stretcher ratios :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",strr[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//---
  printf("Offsets(ns),comb. :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 6.1f",offs[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
//  printf("Stretcher ratio errors :\n\n");
//  chan=0;
//  for(il=0;il<TOF2DBc::getnplns();il++){
//    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//      ic=chan+ib*2;
//      printf(" % 5.2f",s1[ic]);
//    }
//    printf("\n");
//    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//      ic=chan+ib*2+1;
//      printf(" % 5.2f",s1[ic]);
//    }
//    printf("\n\n");
//    chan+=2*TOF2DBc::getbppl(il);
//  }
//---
  printf("Chi2 :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.2f",chi2[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
//---
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){ // print 2-D histogr.  
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        id=1600+chan;
        HPRINT(id);
	chan+=1;
      }
    }
  }
//------------------------------------------------
// ===> f/s-tdc time difference calculation :
//
  number fstdm[TOF2GC::SCCHMX],fstds[TOF2GC::SCCHMX];
  number nttd,tdfs;
  for(i=0;i<chmax;i++){
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
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 6d",nevnt2[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 6d",nevnt2[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
  printf("Time differences :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.1f",fstdm[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.1f",fstdm[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//
  printf("RMS of time differences :\n\n");
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2;
      printf(" % 5.1f",fstds[ic]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      ic=chan+ib*2+1;
      printf(" % 5.1f",fstds[ic]);
    }
    printf("\n\n");
    chan+=2*TOF2DBc::getbppl(il);
  }
//------------------------------------------------
//
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TOF2STRRcalib:error opening file for output"<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for STRR-calibration output, fname:"<<fname<<'\n';
  cout<<"Stretcher ratios for indiv.channels will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
  tcfile.setf(ios::fixed);
  tcfile.width(6);
  tcfile.precision(2);// precision
  chan=0; 
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){// str-ratios
      ic=chan+ib*2;
      dbsta=-1;
      if(chi2[ic]>0.)dbsta=0; 
      if(chi2[ic]>chi2mx)dbsta=10;//problem with t-measurement
      tcfile <<strr[ic]<<" "<<offs[ic]<<" "<<dbsta<<"  ";// side-1
      ic+=1;
      dbsta=-1;
      if(chi2[ic]>0.)dbsta=0; 
      if(chi2[ic]>chi2mx)dbsta=10;//problem with t-measurement
      tcfile <<strr[ic]<<" "<<offs[ic]<<" "<<dbsta<<endl;// side-2
    }
    tcfile << endl;
    chan+=2*TOF2DBc::getbppl(il);
  }
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOF2STRRcalib:output file closed !"<<endl;
}
//=============================================================================
number TOF2AVSDcalib::dtdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean TovT-dynode per chan/bin 
number TOF2AVSDcalib::dtdyn2[TOF2GC::SCCHMX][TOF2GC::SCACHB];// to calc.mean (TovT-dynode)**2 per chan/bin
integer TOF2AVSDcalib::nevdyn[TOF2GC::SCCHMX][TOF2GC::SCACHB];// events in above sums per chan/bin 
integer TOF2AVSDcalib::nevdynt[TOF2GC::SCCHMX];// events in above sums per chan
//--------------------------
void TOF2AVSDcalib::init(){ // ----> initialization for AVSD-calibration 
  integer i,j,il,ib,id,ii,jj,chan;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  strcpy(inum,"0123456789");
//
//  ---> book hist. Tovt-D vs Tovt-A :
//
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){   
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        strcpy(htit1,"Low-gain signal vs High-gain for chan(LBBS) ");
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
        id=1800+chan;
        HBOOK2(id,htit1,50,0.,500.,50,0.,100.,0.);
	chan+=1;
      }
    }
  }
// ---> clear arrays:
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    for(j=0;j<TOF2GC::SCACHB;j++){
      dtdyn[i][j]=0.;
      dtdyn2[i][j]=0.;
      nevdyn[i][j]=0.;
    }
    nevdynt[i]=0;
  }
}
//-------------------------------------------
//            ---> program to fill TovT-arrays:
void TOF2AVSDcalib::filltovt(integer chan, geant adch, geant adcl){
//( called from TOFRawEvent::validate)
//
  integer ic;
//
  adch+=0.5;//tempor +0.5 to be at the middle of ADC-bin=1
  adcl+=0.5;
  HF2(1800+chan,adch,adcl,1.);
      ic=integer(adch/TOF2GC::SCACBW);// get High(anode) bin #
      if(ic<TOF2GC::SCACHB && adcl>=5.){// use events with adcl >= 5 and ic<TOF2GC::SCACHB
        dtdyn[chan][ic]+=adcl;
        dtdyn2[chan][ic]+=(adcl*adcl);
        nevdyn[chan][ic]+=1;
        nevdynt[chan]+=1;
      }
}
//--------------------------------------
//            ---> program to get final  parameters :
void TOF2AVSDcalib::fit(number slop[TOF2GC::SCCHMX], number offs[TOF2GC::SCCHMX], number &avs, number &avo){
//
  integer i,j,il,ib,idd,is,chan,chmax,ich;
  number t0,sl,t,tq,co,dis,nevf,bins;
  number sumc,sumt,sumct,sumc2,sumt2,sumid,nonzer;
  number chi2[TOF2GC::SCCHMX];
//
// ---> calculate/print Low-High channel correl. param.(AVSD-calibr):
//
//
//
  chmax=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    chmax+=TOF2DBc::getbppl(il);
  }
  chmax*=2;//max.number of sc.channels
//
  avs=0.;
  avo=0.;
  nonzer=0.;
  for(i=0;i<chmax;i++){ // <-- chan. loop
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
    for(j=0;j<TOF2GC::SCACHB;j++){//<-- Tout bin loop
      nevf=number(nevdyn[i][j]);
      co=(number(j)+0.5)*TOF2GC::SCACBW;// mid. of High(anode)chan. bin
      if(nevf>5.){ // min. 5 events
        t=dtdyn[i][j]/nevf; // get mean
        tq=dtdyn2[i][j]/nevf; // mean square
        dis=tq-t*t;// rms**2
        if(dis>=0.){
        if(dis==0.)dis=1./12.;// if all events are in single Low-chan bin=1
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
    if(bins>=4){
      t0=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
      sl=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
      chi2[i]=sumt2+t0*t0*sumid+sl*sl*sumc2
        -2.*t0*sumt-2.*sl*sumct+2.*t0*sl*sumc;
      chi2[i]/=(bins-2.);
      slop[i]=1./sl;
      offs[i]=t0;
      nonzer+=1.;
      avs+=slop[i];
      avo+=offs[i];
      HF1(1240,geant(slop[i]),1.);
      HF1(1241,geant(offs[i]),1.);
      HF1(1242,geant(chi2[i]),1.);
    }
  }//--> end of chan. loop
//
  if(nonzer>0){
    avs/=nonzer;
    avo/=nonzer;
  }
//
  printf("\n");
  printf(" ===============>  Low-High signal correlation parameters :\n");
  printf("\n");
  printf("Average HvsL slop: %6.2f\n",avs);
  printf("Average LvsH offset: %6.2f\n",avo);
  printf("\n");
  ich=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6d",nevdynt[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6d",nevdynt[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6.2f",slop[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6.1f",offs[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6.2f",chi2[chan]);
    }
    printf("\n");
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",slop[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6.1f",offs[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",chi2[chan]);
    }
    printf("\n");
    printf("\n");
    ich+=2*TOF2DBc::getbppl(il);
  }
//
//---
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){ // print 2-D histogr.  
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        idd=1800+chan;
        HPRINT(idd);// tempor commented
	chan+=1;
      }
    }
  }
  HPRINT(1240);
  HPRINT(1241);
  HPRINT(1242);
//
}
//--------------------------------------------------
//
//=============================================================================
