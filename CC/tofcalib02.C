//  $Id: tofcalib02.C,v 1.57 2011/06/24 09:16:20 choumilo Exp $
#include "tofdbc02.h"
#include "tofid.h"
#include "point.h"
#include "typedefs.h"
#include "event.h"
#include "amsgobj.h"
#include "commons.h"
#include "cern.h"
#include "mccluster.h"
#include <math.h>
#include "extC.h"
#include "antidbc02.h"
#include "tofsim02.h"
#include "trrec.h"
#include "tofrec02.h"
#include "antirec02.h"
#include "daqs2block.h"
#include "tofcalib02.h"
#include "particle.h"
#include "trigger102.h"
#include "user.h"
#include <time.h>
#include "timeid.h"
//
//
extern TOFBPeds scbrped[TOF2GC::SCLRS][TOF2GC::SCMXBR];// TOF peds/sigmas/...
//extern uinteger TOF2Brcal::CFlistC[11];
//
//
//--------------------------------------------------------------------
//Tdelv:
integer TofTmAmCalib::_nbins[TOF2GC::SCBTPN]={15,16,16,16,15,
                                    16,17,14,15,16,17};//#coord-bins vs bar-type(<=TOF2GC::SCTDBM)
//                                                   to have bin width = 8-9cm
number TofTmAmCalib::_tdiff[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
number TofTmAmCalib::_tdif2[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
number TofTmAmCalib::_clong[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
integer TofTmAmCalib::_nevnt[TOF2GC::SCBLMX][TOF2GC::SCTDBM];
//Txslw:
geant TofTmAmCalib::slope;
geant TofTmAmCalib::tzero[TOF2GC::SCLRS][TOF2GC::SCMXBR];
number TofTmAmCalib::s0;
number TofTmAmCalib::s1;
number TofTmAmCalib::s3[TOF2GC::SCLRS][TOF2GC::SCMXBR];
number TofTmAmCalib::s30[TOF2GC::SCLRS][TOF2GC::SCMXBR];
number TofTmAmCalib::s4;
number TofTmAmCalib::s6[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
number TofTmAmCalib::s7[TOF2GC::SCMXBR];
number TofTmAmCalib::s8;
number TofTmAmCalib::s12[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TofTmAmCalib::s13[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TofTmAmCalib::s14[TOF2GC::SCMXBR][TOF2GC::SCMXBR];
number TofTmAmCalib::s15[TOF2GC::SCLRS-1][TOF2GC::SCMXBR];
number TofTmAmCalib::s16[TOF2GC::SCMXBR];
number TofTmAmCalib::events;
number TofTmAmCalib::resol;
number TofTmAmCalib::resol1;
//Ampl:
number TofTmAmCalib::ambin1[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s1-signals for each ref_bar/bin/event
integer TofTmAmCalib::nevenb1[TOF2GC::SCBTBN];// s1 events, accumulated per ref_bar/bin
number TofTmAmCalib::ambin2[TOF2GC::SCBTBN][TOF2GC::SCACMX];// s2-signals for each ref_bar/bin/event
integer TofTmAmCalib::nevenb2[TOF2GC::SCBTBN];// s2 events, accumulated per ref_bar/bin
number TofTmAmCalib::amchan[TOF2GC::SCCHMX][TOF2GC::SCACMX];// side-signals for each channel/event
integer TofTmAmCalib::nevenc[TOF2GC::SCCHMX];// numb.of events accum. per channel
geant TofTmAmCalib::SideMPA[TOF2GC::SCLRS][TOF2GC::SCMXBR][2];//side MostProb signals(ADCch,for PM-equiliz)
geant TofTmAmCalib::gains[TOF2GC::SCCHMX];// ch.signals ("0" impact) relat. to ref.ones
geant TofTmAmCalib::btamp[2][TOF2GC::SCBTBN];// MostProb bar-signals for each bin(ref.bars,s1/s2) 
geant TofTmAmCalib::ebtamp[2][TOF2GC::SCBTBN];// MostProb errors for each bin(ref.bars,s1/s2) 
integer TofTmAmCalib::rbls[TOF2GC::SCBTPN]={101,105,401,404,201,
                                    202,204,301,302,303,305};//ref.bar id's (for bar types 1-11)
geant TofTmAmCalib::profb[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// bin width for each bar type
geant TofTmAmCalib::profp[TOF2GC::SCBTPN][TOF2GC::SCPRBM];// middle of the bin .........
integer TofTmAmCalib::nprbn[TOF2GC::SCBTPN]={16,16,16,16,15,16,16,14,15,16,16};//profile-bins vs bar-type

number TofTmAmCalib::a2dr[TOF2GC::SCCHMX];//sum of An/Sum(Dyn(pmt))-ratios for each channel
number TofTmAmCalib::a2dr2[TOF2GC::SCCHMX];//sum of ratios**2 for each channel
integer TofTmAmCalib::neva2d[TOF2GC::SCCHMX];//number of events/channel for above sum
number TofTmAmCalib::d2sdr[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of Dyn(pm)/Sum(Dyn(pm))-gains for LBBS
number TofTmAmCalib::d2sdr2[TOF2GC::SCCHMX][TOF2GC::PMTSMX];//sum of ratios**2 
integer TofTmAmCalib::nevdgn[TOF2GC::SCCHMX];//number of events/channel for above sum
  
number TofTmAmCalib::ammrfc[TOF2GC::SCBTPN];//sum for mean charge calculation (ref.c.,"0"-bin)
integer TofTmAmCalib::nevrfc[TOF2GC::SCBTPN];//events in above sum
geant TofTmAmCalib::fpar[10]={0,0,0,0,0,0,0,0,0,0};// parameters to fit
integer TofTmAmCalib::nbinr;//working var. for A-profile fit
integer TofTmAmCalib::iside;//working var. for A-profile fit
geant TofTmAmCalib::fun2nb;//working var. for  fit
number TofTmAmCalib::mcharge[TOF2GC::SCPRBM];
number TofTmAmCalib::mcoord[TOF2GC::SCPRBM];
number TofTmAmCalib::emchar[TOF2GC::SCPRBM];
number TofTmAmCalib::aprofp[TOF2GC::SCBTPN][2*TOF2GC::SCPROFP];// A-profile parameters(ref.bars)
number TofTmAmCalib::clent;
integer TofTmAmCalib::cbtyp;//bar-type (1-11)
integer TofTmAmCalib::fitflg;//(0,1)to communicate with FCN
number TofTmAmCalib::arefb[TOF2GC::SCBTPN][TOF2GC::SCACMX];// tot-signals for each ref_bar/event
integer TofTmAmCalib::nrefb[TOF2GC::SCBTPN];// numb.of events accum. per ref_bar
//
geant TofTmAmCalib::binsta[50];
geant TofTmAmCalib::bincoo[50];
integer TofTmAmCalib::elbt;
geant TofTmAmCalib::elfitp[TOF2GC::SCELFT];
char TofTmAmCalib::eltit[60];
//
//------------------------------
void TofTmAmCalib::initjob(){
  integer mode=TFREFFKEY.relogic[0];
  integer hprtf=TFCAFFKEY.hprintf;
// common Tdelv/Tzslw histograms:
if(hprtf>0){
  if(hprtf>1)HBOOK1(1500,"TmAmC:Anticounter cluster-energy(mev)",80,0.,20.,0.);//
  HBOOK1(1501,"TmAmC:Qmax ratio",80,0.,16.,0.);//
  HBOOK1(1502,"TmAmC:Aver.Q(pC,truncated)",60,0.,600.,0.);//
  HBOOK1(1503,"TmAmC:MyTofBeatFit Beta(prev.calib, Npoints=4)",100,-1.2,1.2,0.);//
  if(hprtf>1)HBOOK1(1504,"TmAmC:Particle multipl. in calib.events",10,0.,10.,0.);
  HBOOK1(1505,"TmAmC:Part.rigidity from TRK(gv)",100,0.,50.,0.);//
  HBOOK1(1506,"TmAmC:PartBeta(prev.calib)",100,0.7,1.2,0.);// 
  HBOOK1(1507,"TmAmC:PartBetaChi2(based on prev calib)",80,0.,16.,0.);//
  HBOOK1(1508,"TmAmC:PartBetaChi2S(based on prev calib)",80,0.,16.,0.);//
  HBOOK1(1509,"TmAmC:PartTrackChi2",80,0.,40.,0.);//
  if(hprtf>0)HBOOK1(1512,"TmAmC:Effective Beta used as reference for Tzslw-calib",80,0.8,1.,0.);//
      
  HBOOK1(1510,"TmAmC:MyTofBetaFit Chisq(prev.calib)",100,0.,25.,0.);
  HBOOK1(1511,"TmAmC:MyTofBetaFit Tzero(prev.calib)",50,-1.,1.,0.);
  HBOOK1(1513,"TmAmC:Primitive(noFit, noTrack required) TofBeta(prev.calib)",100,-1.25,1.25,0.);//
  HBOOK1(1514,"TmAmC:MyTofBeatFit Npoints(prev.calib)",10,0.,10.,0.);//
  HBOOK1(1516,"TmAmC:MyTofBeatFit Beta(prev.calib, Npoint<4)",100,-1.2,1.2,0.);//
      
  HBOOK1(1200,"TmAmC:Res_long.coo(track-sc),L=1",60,-15.,15.,0.);//
  HBOOK1(1201,"TmAmC:Res_long.coo(track-sc),L=2",60,-15.,15.,0.);//
  HBOOK1(1202,"TmAmC:Res_long.coo(track-sc),L=3",60,-15.,15.,0.);//
  HBOOK1(1203,"TmAmC:Res_long.coo(track-sc),L=4",60,-15.,15.,0.);//
//  HBOOK2(1204,"TmAmC:Res_long. vs track coord.,L1",70,-70.,70.,60,-30.,30.,0.);// spare
//  HBOOK2(1205,"TmAmC:Res_long. vs track coord.,L2",70,-70.,70.,60,-30.,30.,0.);//
//  HBOOK2(1206,"TmAmC:Res_long. vs track coord.,L3",70,-70.,70.,60,-30.,30.,0.);//
//  HBOOK2(1207,"TmAmC:Res_long. vs track coord.,L4",70,-70.,70.,60,-30.,30.,0.);//
  if(hprtf>1){
    HBOOK1(1210,"TmAmC:Res_transv.coo(track-sc),L=1",50,-20.,20.,0.);//
    HBOOK1(1211,"TmAmC:Res_transv.coo(track-sc),L=2",50,-20.,20.,0.);//
    HBOOK1(1212,"TmAmC:Res_transv.coo(track-sc),L=3",50,-20.,20.,0.);//
    HBOOK1(1213,"TmAmC:Res_transv.coo(track-sc),L=4",50,-20.,20.,0.);//
  }
  if(hprtf>1)HBOOK1(1215,"TmAmC:TrackImpactCos(averaged over planes)",50,-1.,1.,0.);//
  HBOOK1(1216,"TmAmC:MeanEdep(truncated,cos/beta corrected)",100,0.,25.,0.);
//  HBOOK1(1217,"TmAmC:Cos_sc",50,0.5,1.,0.);//spare
//  HBOOK1(1218,"TmAmC:TOF track-fit chi2-x",50,0.,5.,0.);
//  HBOOK1(1219,"TmAmC:TOF track-fit chi2-y",50,0.,5.,0.);
}
//
  if(mode==2 || mode==23 || mode==234)inittd();//incl. specific hist-booking(1600-1613,1620-1653)
  if(mode==3 || mode==23 || mode==234 || mode==34)inittz();//incl. specific hist-booking(1515-1518)
  if(mode==4 || mode==34 || mode==234)initam();//incl. specific hist-booking(1225-1229,1250-1289)
}
//------------------------------
void TofTmAmCalib::endjob(){
  integer mode=TFREFFKEY.relogic[0];
  cout<<"====> Enter TofTimeAmplCalib::endjob in mode:"<<mode<<endl;
//
//---> print common hist:
if(TFCAFFKEY.hprintf>0){
  if(TFCAFFKEY.hprintf>1)HPRINT(1500);
  HPRINT(1501);
  HPRINT(1502);
  HPRINT(1513);
  HPRINT(1503);
  HPRINT(1516);
  HPRINT(1510);
  HPRINT(1511);
  HPRINT(1514);
  if(TFCAFFKEY.hprintf>1)HPRINT(1504);
  HPRINT(1505);
  HPRINT(1506);
  HPRINT(1507);
  HPRINT(1508);
  HPRINT(1509);
  if(TFCAFFKEY.hprintf>0)HPRINT(1512);
  
  HPRINT(1200);
  HPRINT(1201);
  HPRINT(1202);
  HPRINT(1203);
//  HPRINT(1204);//spare
//  HPRINT(1205);
//  HPRINT(1206);
//  HPRINT(1207);
  if(TFCAFFKEY.hprintf>1){
    HPRINT(1210);
    HPRINT(1211);
    HPRINT(1212);
    HPRINT(1213);
  }
  if(TFCAFFKEY.hprintf>1)HPRINT(1215);
  HPRINT(1216);
//  HPRINT(1217);
//  HPRINT(1218);
//  HPRINT(1219);
}
//Tdelv:
  if(mode==2 || mode==23 || mode==234)fittd();//Tdelv-fits + print(1600-1605,1610-..) + write2file
//Tzslw:
  if(mode==3 || mode==23 || mode==234 || mode==34){
    if(TFCAFFKEY.hprintf>1){
      HPRINT(1515);
    }
    fittz();//Tzslw-fits + print(1517-1518) + write2file
  }
//Ampl:
  if(mode==4 || mode==34 || mode==234){
    endjam();// call fitam and print specific hists + write2file
  }
//
  if(TFCAFFKEY.spares[0]>0){
    cout<<endl;
    cout<<"<============= TofPmtEquilization table is prepared !!!"<<endl;
    cout<<endl;
    return;
  }
//-----------
  int ctyp,ntypes,cnum;
  char datt[3];
  char ext[80];
  char name[80];
  int date[2],year,mon,day,hour,mins,sec;
  uinteger yyyymmdd,hhmmss;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[11];
  int ic,entr,noft,nolt,noht,noan,nody,stat[TOF2GC::SCLRS][TOF2GC::SCMXBR][2];
  number bpor,bpormx(0.8);
//
  char frdate[30];
  uinteger StartRun,overid,verid;
  time_t StartTime;
  integer npmts;
//
//
//   --->  Create TOF-channels status params using JobStat-data:
//
   for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
     for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
       npmts=TOF2Brcal::scbrcal[il][ib].getnpm();
       for(int ip=0;ip<2;ip++){
         stat[il][ib][ip]=0;//ok
         ic=il*TOF2GC::SCMXBR*2+ib*2+ip;//ch-numbering in TofJobStat arrays
	 entr=TOF2JobStat::getch(ic,12);
	 if(entr==0)continue;
	 noht=TOF2JobStat::getch(ic,18);//noSumHT(history)
	 bpor=number(noht)/entr;
	 if(TFREFFKEY.relogic[1]==1 && bpor>bpormx)stat[il][ib][ip]+=100000;//no SumHT
	 nolt=TOF2JobStat::getch(ic,15);//noLT
	 bpor=number(nolt)/entr;
	 if(bpor>bpormx)stat[il][ib][ip]+=10000;//no LT
	 noan=TOF2JobStat::getch(ic,16);//noAn
	 bpor=number(noan)/entr;
	 if(bpor>bpormx)stat[il][ib][ip]+=1000;//no Anode
	 nody=TOF2JobStat::getch(ic,17);//noDy
	 bpor=number(nody)/entr;
	 if(bpor>bpormx){
           if(npmts==2)stat[il][ib][ip]+=11;//no 2-Dynodes
	   else stat[il][ib][ip]+=111;//no 3-Dynodes
	 }
       }
       if(s30[il][ib]<20.)stat[il][ib][0]+=1000000;//set bad BAR for timing (according to Tzslw-results) 
     }
   }
//
//---> create TofCh-status file TofCStatMC(RD):
//
  StartRun=AMSUser::JobFirstRunN();
  StartTime=time_t(StartRun);
  uinteger StartTimeE=uinteger(AMSUser::JobFirstEventT());
  strcpy(frdate,asctime(localtime(&StartTime)));
//
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    overid=TOF2Brcal::CFlistC[2];
    verid=overid+1;//get new MC-versn = old+1 
    TOF2Brcal::CFlistC[2]=verid;//update CStatMC-versn in static store
  }
  else{
    strcpy(datt,"RD");
    overid=TOF2Brcal::CFlistC[1];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    TOF2Brcal::CFlistC[1]=verid;//update CStatRD utc in static store
  }
  strcpy(name,"TofCStat");//generic name
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  cout<<"  <-- Opening TofCalibStatus-file for writing : "<<name<<'\n';
  ofstream stfile(name,ios::out|ios::trunc); // open file for writing
  if(!stfile){
    cout <<"<---- Error: Can't write TofCalibStatus-file: "<<name<<endl;
    exit(1);
  }
//
  for(int il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
  for(int ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
    for(int ip=0;ip<2;ip++){
      stfile << " "<< stat[il][ib][ip];//stat(s1),stat(s2)
    }
    stfile << endl;
  } // --- end of bar loop --->
  stfile << endl;
  } // --- end of layer loop --->
//
  stfile << 12345 << endl;//endoffile label
  stfile << endl;
  stfile << endl<<"======================================================"<<endl;
  stfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  stfile << endl<<" Date of the first event : "<<frdate<<endl;
  stfile.close();
//
  cout <<"  <-- new TofCStat-file "<<name<<" is created !!!"<<endl;
//--------------------------------------------
//
//---> create  TofCflistMC(RD) file:
//
  ntypes=integer(TOF2Brcal::CFlistC[0]);//7/6->MC/RD
  strcpy(name,"TofCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    overid=TOF2Brcal::CFlistC[ntypes+1];
    verid=overid+1;//new vers = old+1
    sprintf(ext,"%d",verid);
  }
  else{
    strcpy(datt,"RD");
    overid=TOF2Brcal::CFlistC[ntypes+1];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(ext,"%d",verid);//new RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
//
  cout<<"  <--  writing new Calib_vers_list-file... "<<name<<'\n';
  ofstream vlfile(name,ios::out|ios::trunc);
  if(!vlfile){
    cout <<"  <-- Error:Can't write new vers.list-file !!? "<<name<<endl;
    exit(1);
  }
  vlfile << ntypes;// total number of calibr. file types in the list
  vlfile << endl;
  for(int i=0;i<ntypes;i++){
    vlfile << TOF2Brcal::CFlistC[i+1];
    vlfile << endl;
  }
  if(AMSJob::gethead()->isMCData()){//add begin date/time for MC
    year=1900+TFREFFKEY.year[0];//tempor use current DataCard begin time
    mon=TFREFFKEY.mon[0]+1;
    day=TFREFFKEY.day[0];
    hour=TFREFFKEY.hour[0];
    mins=TFREFFKEY.min[0];
    sec=1;
    yyyymmdd=year*10000+mon*100+day;
    hhmmss=hour*10000+mins*100+sec;
    vlfile << yyyymmdd;//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile << endl;
    vlfile << hhmmss;//HHMMSS 
    vlfile << endl;
  }
  vlfile.close();
  cout <<"  <-- New CFlist-file "<<name<<" is created !!!"<<endl<<endl;
//---
  printf("<==========Tof CALIB Results:\n");
  for(int i=0;i<4;i++){
    if(i<3)for(int j=0;j<5;j++)printf("% 7.3f",TOF2JobStat::cqual(i,j));//format for TAUC-members
    else for(int j=0;j<5;j++)printf("% 7.1f",TOF2JobStat::cqual(i,j));// format for TDCL
    printf("\n");
  }
  printf("<==========End of Results\n\n");
//
//--- RD: update on flight if requested:
//  
  if(!AMSJob::gethead()->isMCData() && TFCAFFKEY.updbrcaldb==1
//                                                              && AMSFFKEY.Update>0
							                          ){
    int calresok(1);
    time_t begin,end,insert;
    if(mode==2 || mode==23 || mode==234){
      if(!(TOF2JobStat::cqual(0,0)>0.8 && TOF2JobStat::cqual(0,1)>0.6))calresok=0;
    }
    if(mode==3 || mode==23 || mode==234 || mode==34){
      if(!(TOF2JobStat::cqual(1,0)>0.8 && TOF2JobStat::cqual(1,1)<0.5))calresok=0;
    }
    if(mode==24 || mode==34 || mode==234){
      if(!(TOF2JobStat::cqual(2,0)>0.8 && TOF2JobStat::cqual(2,1)>0.7 && TOF2JobStat::cqual(2,2)>0.1))calresok=0;
    }
    if(calresok==1){
      int retstat=TOF2Brcal::setpars(StartRun);//bad if >0
//
      if(retstat==0){
        AMSTimeID *ptdv;
        ptdv = AMSJob::gethead()->gettimestructure(AMSID("Tofbarcal2",AMSJob::gethead()->isRealData()));
        ptdv->UpdateMe()=1;
        ptdv->UpdCRC();
        time(&insert);
        if(CALIB.InsertTimeProc)insert=StartRun;//redefine insert time as runNumber
//        ptdv->SetTime(insert,StartRun-1,StartRun-1+86400*30);
        ptdv->SetTime(insert,min(StartRun-1,StartTimeE),StartRun-1+86400*60);//beg+60days
        cout <<"      <--- Tof Time/Ampl DB-info has been prepared for TDV "<<*ptdv<<endl;
        ptdv->gettime(insert,begin,end);
        cout<<"           Time ins/beg/end: "<<endl;
        cout<<"           "<<ctime(&insert);
        cout<<"           "<<ctime(&begin);
        cout<<"           "<<ctime(&end);
      }
      else{
        cout<<"<==== TofTimeAmplCalib::DB on-flight update was requested but not done : problems to read new calib-files ! "<<retstat<<endl;
      }
//
    }
    else{
      cout<<"<==== TofTimeAmplCalib::DB on-flight update was requested but not done : bad calib.quality !"<<endl;
    }
  }
//
  cout <<"<==== TofTimeAmplCalib::endjob completed ==="<<endl;
}
//------------------------------
void TofTmAmCalib::inittz(){
  int i,j,il,ib;
  slope=0.;
  s0=0.;
  s1=0.;
  s4=0.;
  s8=0.;
  events=0.;
  for(ib=0;ib<TOF2GC::SCMXBR;ib++){
    for(il=0;il<TOF2GC::SCLRS;il++){
      tzero[il][ib]=0.;
      s3[il][ib]=0.;
      s30[il][ib]=0.;
    }
    for(i=0;i<(TOF2GC::SCLRS-1);i++){
      s6[i][ib]=0.;
      s15[i][ib]=0.;
    }
    s7[ib]=0.;
    s16[ib]=0.;
    for(j=0;j<TOF2GC::SCMXBR;j++){
      s12[ib][j]=0.;
      s13[ib][j]=0.;
      s14[ib][j]=0.;
    }
  }
//
  if(TFCAFFKEY.hprintf>1){
    HBOOK1(1515,"TRlen13-TRlen24",80,-4.,4.,0.);//
    HBOOK1(1517,"T0-difference inside bar-types 2",80,-0.4,0.4,0.);
    HBOOK1(1518,"T0-difference between ref. and bar-types 4",80,-0.4,0.4,0.);
  }
}
//------------------------------
void TofTmAmCalib::fittz(){  // Tzslw-calibr. fit procedure, f.results->slope,tzero[][]->file
  int i,id,ii,j,ier,il,ib,npar(TOF2GC::SCBLMX+2),nparr,seqnum;
//
  char pnm[6];
  char p1nam[6];
  char p2nam[3];
  char inum[11];
  char in[2]="0";
  int ifit[TOF2GC::SCBLMX+1];
  char pnam[TOF2GC::SCBLMX+1][6];
  number argl[10];
  int iargl[10];
  number start[TOF2GC::SCBLMX+1],step[TOF2GC::SCBLMX+1];
  number plow[TOF2GC::SCBLMX+1],phigh[TOF2GC::SCBLMX+1];
//
  strcpy(p1nam,"tslop");
  strcpy(p2nam,"tz");
  strcpy(inum,"0123456789");
//
// -----------> set parameter defaults:
//slope:
  strcpy(pnam[0],p1nam); // for slope
  start[0]=TFCAFFKEY.fixsl;// def. slope 
  step[0]=1.;
  plow[0]=0.;
  phigh[0]=40.;
  ifit[0]=TFCAFFKEY.ifsl;// fix/release slope 
  if(TFCAFFKEY.idref[1]==2)ifit[0]=0;
//T0's
  ii=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      id=(il+1)*100+ib+1;
      start[ii+1]=TOF2Brcal::scbrcal[il][ib].gettzero();// def.T0's from current calibration
      if(id == TFCAFFKEY.idref[0] && (TFCAFFKEY.idref[0]/100)==1)
                                 start[ii+1]=TFCAFFKEY.tzref[0];//def.T0(0.) for ref.counter
      step[ii+1]=1.;
      plow[ii+1]=-40.;
      phigh[ii+1]=40.;
      ifit[ii+1]=0;//means fix
      strcpy(pnm,p2nam);
      in[0]=inum[il];
      strcat(pnm,in);
      i=ib/10;
      j=ib%10;
      in[0]=inum[i];
      strcat(pnm,in);
      in[0]=inum[j];
      strcat(pnm,in);
      strcpy(pnam[1+ii],pnm);
      ii+=1;//use sequential solid numbering of counters
    }
  }
// ------------> release T0-parameters with good statistics:
// 
  printf("====> TofTimeAmplCalib::fittz:Collected events (total) : %9.0f\n",events);
  printf("      Collected events per sc. bar : \n");
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=1;ib<=TOF2DBc::getbppl(il);ib++){
      if(ib%TOF2DBc::getbppl(il) !=0)
                     printf("% 7.0f",s30[il][ib-1]);
      else
                     printf("% 7.0f\n",s30[il][ib-1]);
    }
  }
//
  nparr=0;
  seqnum=0;
  bool missrefc(false);
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      id=(il+1)*100+ib+1;
      if(s30[il][ib]>=20.){
        if(TFCAFFKEY.idref[1]==0 
	     || (TFCAFFKEY.idref[1]==1 && (ib>0 || (ib+1)<TOF2DBc::getbppl(il)))
	     || (TFCAFFKEY.idref[1]==2 && (ib==0 || (ib+1)==TOF2DBc::getbppl(il)))){
          ifit[1+seqnum]=1;//bar with high statist.-> release
          nparr+=1;
	}
        if(id == TFCAFFKEY.idref[0]){
          ifit[1+seqnum]=0;//fix, if ref counter
          nparr-=1;
        }
      }
      else{
        if(id == TFCAFFKEY.idref[0]){
          cout<<" <--- Too low statistics in ref.counter "<<id<<" "<<s30[il][ib]<<'\n';
	  missrefc=true;
        }
        ifit[1+seqnum]=0;//bar with low statist.-> fix
      }
      seqnum+=1;
    }
  }
//
  printf("      Sufficient statistics to fit  %d counters(+1ref) \n",nparr);
//
  if(nparr<2 || missrefc){
    cout<<"<---- Not enough counters for Minuit-Fit !!!"<<'\n';
    return;// not enough counters for FIT -> return
  }
  TOF2JobStat::cqual(1,0)=geant(nparr+1)/seqnum;//% counters with good stat (for cal-quality evaluation)
//
  npar=seqnum+1;//T0's_of_counters + slope_parameter
// ------------> initialize parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("      TZSL-calibration for TOF-system");
  for(i=0;i<npar;i++){
    strcpy(pnm,pnam[i]);
    ier=0;
    MNPARM((i+1),pnm,start[i],step[i],plow[i],phigh[i],ier);
    if(ier!=0){
      cout<<"<---- Fatal: Param-init problem for par-id="<<pnam[i]<<'\n';
      exit(10);
    }
    argl[0]=number(i+1);
    if(ifit[i]==0){
      ier=0;
      MNEXCM(mfuntz,"FIX",argl,1,ier,0);
      if(ier!=0){
        cout<<"<---- Fatal: Param-fix problem for par-id="<<pnam[i]<<'\n';
        exit(10);
      }
    }
  }
//----
  argl[0]=0.;
  ier=0;
  MNEXCM(mfuntz,"MINIMIZE",argl,0,ier,0);
  if(ier!=0){
    cout<<"<---- Fatal: MINIMIZE problem !"<<'\n';
    exit(10);
  }  
  MNEXCM(mfuntz,"MINOS",argl,0,ier,0);
  if(ier!=0){
    cout<<"<---- Fatal: MINOS problem !"<<'\n';
    exit(10);
  }
  argl[0]=number(3);
  ier=0;
  MNEXCM(TofTmAmCalib::mfuntz,"CALL FCN",argl,1,ier,0);
  if(ier!=0){
    cout<<"<---- Fatal: final CALL_FCN problem !"<<'\n';
    exit(10);
  }
//----
  printf("<==== TofTimeAmplCalib::fittz: Minuit ended with parameters:\n");
  printf("      -----------------------------\n");
  printf("      Resolution(ns) : %6.3e\n",resol);
  printf("      Resolution1(ns): %6.3e\n",resol1);
  printf("      Slope          : %6.3e\n",slope);
  TOF2JobStat::cqual(1,1)=geant(resol1);//system resolution (for cal-quality evaluation)
}
//-----------------------------------------------------------------------
// This is standard Minuit FCN for Tzslw-calib:
void TofTmAmCalib::mfuntz(int &np, number grad[], number &f, number x[], int &flg, int &dum){
  int i,j,il,ib,seqnum;
  integer id,ibt,idr,ibtr;
  int refl=(TFCAFFKEY.idref[0]/100);//reflayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
  int nprsmx=4-refl;//max pairs in sums (=3 for normal case lref=1, =2 if lref=2)
//  static int first(0);
  number f3(0.),f6(0.),f7(0.),f8(0.),f10(0.);
  number f1[TOF2GC::SCLRS];
  number f2[TOF2GC::SCLRS-1];
  number f9[TOF2GC::SCLRS-1];
  geant w,tz,tzr;
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  integer cfvn;
  uinteger StartRun,overid,verid;
  time_t StartTime;
  int dig;
  strcpy(inum,"0123456789");
//
//
  for(i=0;i<TOF2GC::SCLRS;i++)f1[i]=0.;
  for(i=0;i<TOF2GC::SCLRS-1;i++){
    f2[i]=0.;
    f9[i]=0.;
  }
//
  seqnum=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    f1[il]=0.;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      f1[il]+=s3[il][ib]*x[1+seqnum]*x[1+seqnum];//l=1,4 (s3[0][ib]=0 when ref.layer=2) 
      seqnum+=1;
    }
  }
//
  for(i=0;i<TOF2DBc::getnplns()-1;i++){//loop over pairs Lr->Lk, k=(2),3,4
    f2[i]=0.;    
    il=i+1;
    if(il>(refl-1)){//skip missing pair Lr->L2 if refl=2
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++)f2[i]+=s6[i][ib]*x[1+TOF2DBc::barseqn(il,ib)];//l=(2),3,4
    }
  }
//
  for(ib=0;ib<TOF2DBc::getbppl(refl-1);ib++)f3+=s7[ib]*x[1+TOF2DBc::barseqn(refl-1,ib)];//l=r
//
  if(refl==1){// f6=0(skipped) if refl=2
    for(i=0;i<TOF2DBc::getbppl(refl-1);i++){
      for(j=0;j<TOF2DBc::getbppl(1);j++)f6+=s12[i][j]*x[1+TOF2DBc::barseqn(refl-1,i)]*x[1+TOF2DBc::barseqn(1,j)];//Lr*(2)
    }
  }
//
  for(i=0;i<TOF2DBc::getbppl(refl-1);i++){//loop over bars in refl
    for(j=0;j<TOF2DBc::getbppl(2);j++)f7+=s13[i][j]*x[1+TOF2DBc::barseqn(refl-1,i)]*x[1+TOF2DBc::barseqn(2,j)];//Lr*3
  }
//
  for(i=0;i<TOF2DBc::getbppl(refl-1);i++){
    for(j=0;j<TOF2DBc::getbppl(3);j++)f8+=s14[i][j]*x[1+TOF2DBc::barseqn(refl-1,i)]*x[1+TOF2DBc::barseqn(3,j)];//Lr*4
  }
//
  for(i=0;i<TOF2DBc::getnplns()-1;i++){ 
    f9[i]=0.;
    il=i+1;
    if(il>(refl-1)){//skip missing pair Lr->L2 if refl=2
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++)f9[i]+=s15[i][ib]*x[1+TOF2DBc::barseqn(il,ib)];//l=(2),3,4
    }
  }
//
  for(ib=0;ib<TOF2DBc::getbppl(refl-1);ib++)f10+=s16[ib]*x[1+TOF2DBc::barseqn(refl-1,ib)];//l=r
//
  f=s1
   +f1[0]+f1[1]+f1[2]+f1[3]
   +s4*x[0]*x[0]
   +2.*(f2[0]+f2[1]+f2[2])
   -2.*f3
   +2.*s8*x[0]
   -2.*(f6+f7+f8)
   +2.*(f9[0]+f9[1]+f9[2])*x[0]
   -2.*f10*x[0];
//----------------------------------
  if(flg==3){
    cout<<"----> TofTimeAmplCalib::fittz: Final call to FCN(user actions):"<<endl;
//
//--> get run/time of the first event(set at 1st event valid-stage using related AMSEvent values) 
//
    StartRun=AMSUser::JobFirstRunN();
    StartTime=time_t(StartRun);
    strcpy(frdate,asctime(localtime(&StartTime)));
//
//--> create name for output file
// 
    strcpy(fname,"TofTzslw");
    if(AMSJob::gethead()->isMCData()){
      strcat(fname,vers1);
      overid=TOF2Brcal::CFlistC[4];
      verid=overid+1;//new MC-versn = old+1
      sprintf(fext,"%d",verid);
      TOF2Brcal::CFlistC[4]=verid;//update Tzslw-calib version in static store
    }
    else{
      strcat(fname,vers2);
      overid=TOF2Brcal::CFlistC[3];
      if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
      else verid=StartRun;
      sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
      TOF2Brcal::CFlistC[3]=verid;//update Tzslw-calib version in static store
    }
    strcat(fname,".");
    strcat(fname,fext);
//
// some histograms:
    idr=TFCAFFKEY.idref[0];
    tzr=TFCAFFKEY.tzref[0];
    il=idr/100-1;
    ib=idr%100-1;
    ibtr=TOF2DBc::brtype(il,ib);// ref. bar type (1->...)
    seqnum=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        tz=geant(x[1+seqnum]);
        ibt=TOF2DBc::brtype(il,ib);// bar type (1->...)
        if(ibt==ibtr && TFCAFFKEY.hprintf>1)HF1(1517,(tz-tzr),1.);
        if(ibt==4 && TFCAFFKEY.hprintf>1)HF1(1518,(tz-tzr),1.);
	seqnum+=1;
      }
    }
    if(TFCAFFKEY.hprintf>1){
      HPRINT(1517);
      HPRINT(1518);
    }
// write parameters to ext.file:
    resol=-1.;
    resol1=-1.;
    if(f>=0. && events>0)resol=sqrt(f/events);
    if(f>=0. && s0>0)resol1=sqrt(f/s0);//to have rms/pair
//
    cout<<"<---- Open file for Tzslw-calibration output, fname:"<<fname<<'\n';
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cout<<"<---- error opening file for output"<<fname<<'\n';
      exit(8);
    }
    cout<<"      Slope and individual T0's will be written !"<<'\n';
    cout<<"      First run used for calibration is "<<StartRun<<endl;
    cout<<"      Date of the first event : "<<frdate<<endl;
    tcfile.setf(ios::fixed);
    tcfile.width(6);
    tcfile.precision(2);// precision for slope
    slope=geant(x[0]);
    tcfile << slope<<endl;
    tcfile.precision(3);// precision for T0
    seqnum=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        tzero[il][ib]=geant(x[1+seqnum]);
        tcfile <<" "<<tzero[il][ib];
	seqnum+=1;
      }
      tcfile << endl;
    }
    tcfile << endl;
//
    tcfile << 12345 << endl;//endoffile label
    tcfile << endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
    cout<<"<==== TofTimeAmplCalib(Tzslw) completed, output file is written !!!"<<endl;
  }
}
//-----------------------------------------------------------------------
// To fill arrays, used by FCN :
void TofTmAmCalib::filltz(int ib[TOF2GC::SCLRS],number dtr[TOF2GC::SCLRS-1], 
                                               number du[TOF2GC::SCLRS-1]){
  int i,j,ibr,refl,tarl;
// ---> note: imply missing layer has ib=-1; corresponding diffs=0
// du[i]=dtr[i]=0 simultaneously (when any r->2,3,4 pair is missing)
//
  refl=(TFCAFFKEY.idref[0]/100);//reflayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
  ibr=ib[refl-1];//bar in ref.layer
  if((ib[0]<0 && refl==1) || (ib[1]<0 && refl==2)){//ref-layer have to be present
    cout<<"<--- TzslClid: Error - ref.layer missing:refl="<<refl<<" BarL1/2="<<ib[0]+1<<" "<<ib[1]+1<<endl;
    goto Exit;
  }
  if(refl==2)ib[0]=-1;//to avoid summing L1 when ref.layer=2 (important for s30-counting)
//cout<<" refl/b="<<refl<<" "<<ibr+1<<endl;
//cout<<"ib:"<<endl<<ib[0]<<" "<<ib[1]<<" "<<ib[2]<<" "<<ib[3]<<endl;
//cout<<"dtr:"<<endl<<dtr[0]<<" "<<dtr[1]<<" "<<dtr[2]<<endl;
//cout<<"du:"<<endl<<du[0]<<" "<<du[1]<<" "<<du[2]<<endl;
  events+=1.;
//---
    for(i=0;i<TOF2DBc::getnplns();i++)if(ib[i]>=0)s30[i][ib[i]]+=1.;
    for(int ipr=0;ipr<TOF2DBc::getnplns()-1;ipr++){
      if(dtr[ipr]!=0){//given pair is exist
        tarl=2+ipr;//target layer in given pair(refl->tarl)
        s0+=1;//counts all !=0 pairs (r->2,r->3,r->4)
        s1+=dtr[ipr]*dtr[ipr];
        s3[tarl-1][ib[tarl-1]]+=1;
        s3[refl-1][ibr]+=1;
        s4+=du[ipr]*du[ipr];
        s6[ipr][ib[tarl-1]]+=dtr[ipr];
        s7[ibr]+=dtr[ipr];
        s8+=dtr[ipr]*du[ipr];
        if(tarl==2)s12[ibr][ib[tarl-1]]+=1.;//r->2 (s12=0(not filled) if refl=2 because dtr[0]=0 & tarl>2)
        if(tarl==3)s13[ibr][ib[tarl-1]]+=1.;//r->3
        if(tarl==4)s14[ibr][ib[tarl-1]]+=1.;//r->4
	s15[ipr][ib[tarl-1]]+=du[ipr];
	s16[ibr]+=du[ipr];
      }
    }
Exit:
    refl=0;//dummy action
}
//========================================================================
void TofTmAmCalib::select(){  // calibr. event selection
  integer i,j,ilay,ibar,nbrl[TOF2GC::SCLRS],brnl[TOF2GC::SCLRS],bad,status,sector,conf,nanti(0);
  integer il,ib,ix,iy;
  integer cref[2],lref[2];
  integer badx,bady;
  number ltim[4],tdif[4],trle[4],tm[2],am[2],ama[2],amd[2],time,timeD,tamp,edepa,edepd,relt;
  geant x[2],y[2],zx[2],zy[2],zc[4],tgx,tgy,cosc,cosi,cost,xtr[TOF2GC::SCLRS],ytr[TOF2GC::SCLRS];
  number edep[TOF2GC::SCLRS];
  geant ainp[2],dinp[2][TOF2GC::PMTSMX],cinp;
  number amdr[TOF2GC::PMTSMX];
  number am1[TOF2GC::SCLRS],am2[TOF2GC::SCLRS],am1d[TOF2GC::SCLRS],am2d[TOF2GC::SCLRS];
  number fpnt,bci,sut,sul,sul2,sutl,sud,sit2,tzer,chsq;
  int ip,npmf,npmts,nzpm;
  number adca[TOF2GC::SCLRS][2],adcd[TOF2GC::SCLRS][2];
  number dpma1[TOF2GC::SCLRS][TOF2GC::PMTSMX],dpma2[TOF2GC::SCLRS][TOF2GC::PMTSMX];
  number coo[TOF2GC::SCLRS],trp1[TOF2GC::SCLRS],trp2[TOF2GC::SCLRS],arp1[TOF2GC::SCLRS],arp2[TOF2GC::SCLRS];
  number tmsd[TOF2GC::SCLRS],tmsdc[TOF2GC::SCLRS],t14,tmss[TOF2GC::SCLRS];
  geant slops[2],eacl;
  number shft,ftdel,qtotl[TOF2GC::SCLRS],qsd1[TOF2GC::SCLRS],qsd2[TOF2GC::SCLRS],eanti(0),meanq,rr,qmax;
  number sigt[4]={0.17,0.17,0.17,0.17};// approx. time meas.accuracy in TOF-layers 
  number betof,lflgt,cvel(29.979);
  number eacut=2.5;// cut on E-anti (mev). tempor 
  number qrcut=6.;// cut on max/mean-charge ratio
  number qtcut=400.;// cut on max mean-charge 
  number edecut=4.5;// max. TruncAver energy(mev)(to avoid ions in Ampl-calib of abs.norm)
  number adcmin=10;//min 2xAnodes-signal(ADC-ch) for counter to be selected for calibration
  number *pntr[TOF2GC::SCLRS];
  bool PadInTrig;
  TOF2RawCluster *ptr;
  AMSAntiCluster *ptra;
  Trigger2LVL1 *plvl1;
//
  ptr=(TOF2RawCluster*)AMSEvent::gethead()->
                           getheadC("TOF2RawCluster",0);
  ptra=(AMSAntiCluster*)AMSEvent::gethead()->
                           getheadC("AMSAntiCluster",0);
//----
  integer mode=TFREFFKEY.relogic[0];//calibr.type
  bool RelaxCut=(TFCAFFKEY.spares[1]==1);
  bool PMEQmode=(TFCAFFKEY.spares[0]==1);
  int Bref=(TFCAFFKEY.idref[0]%100);//refbar(1-8)
  int Lref=(TFCAFFKEY.idref[0]/100);//ref1ayer(1)/2, =2 means 1st layer in not functional, 2nd used as ref
  bool StrictLPatt=(TFCAFFKEY.spares[2]==0);
  if(Lref!=1)StrictLPatt=false;//reset to false if 1st layer bad
//----
  TOF2JobStat::addre(10);
//
  for(i=0;i<TOF2GC::SCLRS;i++){
    nbrl[i]=0;
    qtotl[i]=0;
    ltim[i]=0;
    tmss[i]=0;
    edep[i]=0;
    brnl[i]=-1;
    adca[i][0]=0;
    adca[i][1]=0;
    am1[i]=0;
    am2[i]=0;
    tmsdc[i]=0;
  }
//
//----
  while (ptr){ // <--- loop over TOF2RawCluster hits
    status=ptr->getstatus();
    if((status&TOFGC::SCBADB3)==0  || PMEQmode){ //use only good for t-measurement(2sides t-meas ok) according to DB 
      if((status&TOFGC::SCBADB2)==0  || PMEQmode
//both sides really have complete set of measurement in given event (i.e. not 1-sided)
//                                   || (status&TOFGC::SCBADB5)!=0
//was not (1-sided + recovered)
				                                 ){//use only 2-sided (
        ilay=(ptr->getntof())-1;
        ibar=(ptr->getplane())-1;
//	if(TFCAFFKEY.idref[1]==0 || TFCAFFKEY.idref[1]==2 ||
//	  (TFCAFFKEY.idref[1]==1 && (ibar>0 && (ibar+1)<TOF2DBc::getbppl(ilay)))){//skip trapez.c if requested
          ptr->getadca(ama);
          if((ama[0]>adcmin && ama[1]>adcmin)
	                     || ((ama[0]>adcmin || ama[1]>adcmin) && PMEQmode)
	                                                                      ){// require min. anode signal
            TOF2Brcal::scbrcal[ilay][ibar].adc2q(0,ama,am);//Anode_adc->charge(no gain corr)
            qtotl[ilay]=am[0]+am[1];
            qsd1[ilay]=am[0];//side q(pC) for Tzslw-calib
            qsd2[ilay]=am[1];
            ptr->getsdtm(tm);// get raw side-times(A-noncorrected)
            nbrl[ilay]+=1;
            brnl[ilay]=ibar;//<--- store bar#
            trp1[ilay]=tm[0];//store side raw time(noncorrected by q)
            trp2[ilay]=tm[1];
            arp1[ilay]=ama[0];
            arp2[ilay]=ama[1];
            coo[ilay]=ptr->gettimeD();// get local Y-coord., got from time-diff(prev.calib)
            tmsd[ilay]=0.5*(tm[0]-tm[1]);// slew-noncorr. side time difference
            tmss[ilay]=0.5*(tm[0]+tm[1]);// ............. side time sum
	    if(!PMEQmode){
	      TOF2Brcal::scbrcal[ilay][ibar].td2ctd(tmsd[ilay],ama,0,tmsdc[ilay]);//get slew-corrected t-diff for Tdelv
//	      tmsdc[ilay]=tmsd[ilay];// use raw side-times(running first time,when slop unknown)
	    }
            else{
	      tmsdc[ilay]=tmsd[ilay];// use raw side-times(running first time,when slop unknown)
	    }
// --> some vars for ampl.cal:
            ltim[ilay]=ptr->gettime();//raw-cluster corrected time(previous calibration was used)
            am1[ilay]=am[0];//store Anode-charge(pC) for Anode rel.gains, abs.norm(
            am2[ilay]=am[1];
            edepa=ptr->getedepa();//energy in Anode channel(prev calibration !)
            edepd=ptr->getedepd();//energy in Dynode channel
            edep[ilay]+=edepa;
//
// ===> store Anode-adcs and Dynode_pmts-adcs(to select later centr.bin for a2dr-calc
//                                                                  and for gaind[ipm]-calc):
            adca[ilay][0]=ama[0];
            adca[ilay][1]=ama[1];
//    
            ptr->getadcdr(0,amdr);//Dyn-pmts ampls, s1
            for(ip=0;ip<TOF2GC::PMTSMX;ip++)dpma1[ilay][ip]=amdr[ip];
            ptr->getadcdr(1,amdr);//Dyn-pmts ampls, s2
            for(ip=0;ip<TOF2GC::PMTSMX;ip++)dpma2[ilay][ip]=amdr[ip];
//------
	  }
//	}
      }
    }
    ptr=ptr->next();
  }// --- end of raw-cluster hits loop --->
//
//----->    Select events with enough layers with bars/layer=1 :
//
  int ngdlrs=0;
  for(i=0;i<TOF2DBc::getnplns();i++){
    if(nbrl[i]==1)ngdlrs+=1;
    else{
      nbrl[i]=0;//reset non-empty, but bad(>1 fired bars) layers
      brnl[i]=-1;
    }  
  }
  if(!PMEQmode){//<---normal(not PMEquilization) calibration
    if(StrictLPatt){
      if(ngdlrs<TOF2DBc::getnplns()){
//cout<<"<--- Rejected bad LPatt(Strict): nbrl="<<nbrl[0]<<" "<<nbrl[1]<<" "<<nbrl[2]<<" "<<nbrl[3]<<" ngdl="<<ngdlrs<<endl;
        return;//require 4x1bar layers
      }
    }
    else{
      bad=0;
      if(nbrl[0]==0 && Lref==1)bad=1; // always require 1st layer if it is ref-layer
      if(nbrl[1]==0 && Lref==2)bad=1; // always require 2nd layer if it is ref-layer
      if((nbrl[2]==0 || nbrl[3]==0) && Lref==2)bad=1; // always require 2 layer for bottom if 1st one is bad
      if(ngdlrs<TOF2DBc::getnplns()-1)bad=1;//require at least 3 planes
      if(bad==1){
//cout<<"<--- Rejected bad LPatt: nbrl="<<nbrl[0]<<" "<<nbrl[1]<<" "<<nbrl[2]<<" "<<nbrl[3]<<" ngdl="<<ngdlrs<<endl;
        return;
      }
    }
  }
  else{//<---PMEquilization calib
    if(ngdlrs<2)return;
  } 
  TOF2JobStat::addre(11);
//
// -----> check Anti-counter :
//
  eanti=0;
  nanti=0;
  while (ptra){ // <--- loop over ANTICluster hits
    status=ptra->getstatus();
    if(status==0){ //select only good hits
      sector=(ptra->getsector())-1;
      eacl=ptra->getedep();
      eanti=eanti+(ptra->getedep());
      if(eacl>eacut)nanti+=1;
      if(TFCAFFKEY.hprintf>1){
#pragma omp critical (hf1)
{
        HF1(1500,geant(eacl),1.);
}
      }
    }
    ptra=ptra->next();
  }// --- end of hits loop --->
  if(nanti>1 && !(RelaxCut || PMEQmode)){
//cout<<"<--- rejected due to ACC presence: nanti="<<nanti<<endl; 
    return;// remove events with >1 sector(e>ecut) in Anti
  }
//
  TOF2JobStat::addre(12);
//
// -----> check spike-amplitudes in layers :
//
  for(i=0;i<TOF2DBc::getnplns();i++)pntr[i]=&qtotl[i];
  AMSsortNAG(pntr,TOF2DBc::getnplns());// sort in increasing order
  meanq=0;
  for(i=0;i<(TOF2DBc::getnplns()-1);i++)meanq+=(*pntr[i]);
  meanq/=number(TOF2DBc::getnplns()-1);
  qmax=*pntr[TOF2DBc::getnplns()-1];
  rr=qmax/meanq;//Qmax/Qaverage_of_the_rest
  if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
    HF1(1501,geant(rr),1.);
    HF1(1502,geant(meanq),1.);
}
  }
  if(rr>qrcut && !(RelaxCut || PMEQmode)){
//cout<<"<--- rejected due to Spike:qmax/meanq="<<qmax<<"/"<<meanq<<" rr="<<rr<<endl; 
//    return; // remove events with "spike" Edep layers(normal calib)
  } 
 
  TOF2JobStat::addre(13);
//
// -----> remove albedo, very slow and Beta>>1 particles(using only TOF) :
//
  if(ngdlrs==4){//this is only for 4-layers TOF config
    ix=0;
    iy=0;                                
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      zc[il]=TOF2DBc::getzsc(il,ib);
      if(TOF2DBc::plrotm(il)==0){// unrotated (Long->Yabs) planes(2,3)
        y[iy]=coo[il];
        zy[iy]=zc[il];
        xtr[il]=TOF2DBc::gettsc(il,ib);
        ytr[il]=y[iy];
        iy+=1;
      }
      else{                    // rotated (Long->Xabs) planes(1,4)
        x[ix]=coo[il];
        zx[ix]=zc[il];
        xtr[il]=x[ix];
        ytr[il]=TOF2DBc::gettsc(il,ib);
        ix+=1;
      }
    }
    tgx=(x[0]-x[1])/(zx[0]-zx[1]);
    tgy=(y[0]-y[1])/(zy[0]-zy[1]);
    cosc=1./sqrt(1.+tgx*tgx+tgy*tgy);//track impact cos 
//
    lflgt=TOF2DBc::supstr(1)-TOF2DBc::supstr(2)-
        (TOF2DBc::plnstr(1)+TOF2DBc::plnstr(2)
        +TOF2DBc::plnstr(3)+TOF2DBc::plnstr(6));// aver. top/bot flight distance(norm.inc)
    geant ttop=0;
    geant tbot=0;
    if(ltim[0]!=0 && ltim[1]!=0)ttop=ltim[0]+ltim[1];
    if(ltim[2]!=0 && ltim[3]!=0)tbot=ltim[2]+ltim[3];
    betof=0;
    if(ttop!=0 && tbot!=0){
      betof=2.*lflgt/(ttop-tbot)/cosc/cvel;//primitive(noFit) TOFbeta based on prev.calibr.
      if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
        HF1(1513,geant(betof),1.);
}
      }
    }
    if(!PMEQmode){//calib-mode
      if(!RelaxCut){
        if(betof<0.7 || betof>1.3){
//cout<<"<--- rejected due to PrimitiveBet:beta="<<betof<<endl; 
	  return;
	}
      }
      else{
        if(betof<0.1 || betof>1.8)return;//relaxed
      }
    }
  }
//
  TOF2JobStat::addre(14);
//------> prepare times for Tzslw-calib:
  number times[TOF2GC::SCLRS];
//
  ftdel=TOF2Varp::tofvpar.ftdelf()+40;//tempor incr by 40 to have mean instead of min
  for(i=0;i<TOF2DBc::getnplns();i++){//prepare times-array for Tzslw-calib
    times[i]=0.;
    if(nbrl[i]==0)continue;//skip missing layers
    times[i]=tmss[i];//measured bar's raw times,used for Tzslw-calib
    ama[0]=arp1[i];//................ Q
    ama[1]=arp2[i];
  }
//
//------> Look for the TRACK :
//
    number pmas(0.938),mumas(0.1057);
    number momentum;
    number pmom,bet,chi2,betm,beta;
    number chi2t,chi2s,crc;
    number the,phi,rigid,err,trl;
    geant scchi2[2],xer[TOF2GC::SCLRS],yer[TOF2GC::SCLRS],lcerr,lvel;
    number trlr[TOF2GC::SCLRS],trlen[TOF2GC::SCLRS-1];
    number ram[TOF2GC::SCLRS],ramm[TOF2GC::SCLRS];
    number dum[TOF2GC::SCLRS-1],tld[TOF2GC::SCLRS-1],tdm[TOF2GC::SCLRS-1];
    number ctran,coot[TOF2GC::SCLRS],cstr[TOF2GC::SCLRS],dx,dy;
    number cool[TOF2GC::SCLRS];
    number sl[2],t0[2],sumc,sumc2,sumt,sumt2,sumct,sumid,zco,tco,dis;
    integer chargeTOF,chargeTracker(1),betpatt,trpatt,trhits(0);
    uintl traddr(0,0);
    AMSPoint C0,Cout;
    AMSDir dir(0,0,1.);
    AMSContainer *cptr;
    AMSParticle *ppart;
    AMSTrTrack *ptrack;
    AMSTRDTrack *ptrd;
    AMSCharge  *pcharge;
    AMSBeta * pbeta;
    int npart(0),ipatt,envindx(0);
    bool trktr,trdtr,ecaltr,nottr,badint;
//
    for(i=0;i<2;i++){//i=0->keeps parts.with true(Trk)-track, i=1->...false(nonTrk)-track
      cptr=AMSEvent::gethead()->getC("AMSParticle",i);// get pointer to part-envelop "i"
      if(cptr){
        if(cptr->getnelem()!=0){
          npart+=cptr->getnelem();
	  envindx=i;
          break;//use 1st non-empty envelop
        }
      }
    }
    if(TFCAFFKEY.hprintf>1){
#pragma omp critical (hf1)
{
      HF1(1504,geant(npart),1.);
}
    }
    if(npart<1){
//cout<<"<--- rejected due to missing Part,npart="<<npart<<endl; 
      return;// require events with 1 particle at least
    }
// 
    TOF2JobStat::addre(15);
//
    ppart=(AMSParticle*)AMSEvent::gethead()->
                                      getheadC("AMSParticle",envindx);
    bool TrkTrPart=false;
    bool AnyTrPart=false;
    bool GoodTrPart=false;
    bool GoodTrkTrack=false;
//			      
    while(ppart){//<---loop over particles in envindx-envelop
      ptrack=ppart->getptrack();//get pointer of the Track used in given particle
      ptrd=ppart->getptrd();//get pointer of the TRD-track, used in given particle
      if(ptrd)TOF2JobStat::addre(17);
      if(ptrack){
        AnyTrPart=true;
        TOF2JobStat::addre(16);
        trdtr=(ptrack->checkstatus(AMSDBc::TRDTRACK)!=0);
        ecaltr=(ptrack->checkstatus(AMSDBc::ECALTRACK)!=0);
        nottr=(ptrack->checkstatus(AMSDBc::NOTRACK)!=0);
        badint=(ptrack->checkstatus(AMSDBc::BADINTERPOL)!=0);
        if(trdtr)TOF2JobStat::addre(18);
        if(ecaltr)TOF2JobStat::addre(19);
        if(nottr)TOF2JobStat::addre(20);
        if(badint)TOF2JobStat::addre(21);
	if(nottr || ecaltr || badint)goto Nextp;//looking only for TRK/TRD-track particle
	GoodTrPart=true;
	TOF2JobStat::addre(22);//good track part
	if(!trdtr){//tracker-track based part
	  TrkTrPart=true;
	  TOF2JobStat::addre(23);
          trpatt=ptrack->getpattern();//TRK-track pattern
	  if(trpatt>=0){//trk-track ok
            GoodTrkTrack=true;
#ifdef _PGTRACK_
	    //PZ FIXME UNUSED traddr=ptrack->getaddress();//TRK-track ladders combination id
	    trhits=ptrack->GetNhits();
#else
	    traddr=ptrack->getaddress();//TRK-track ladders combination id
	    trhits=ptrack->getnhits();
#endif
            ptrack->getParFastFit(chi2,rigid,err,the,phi,C0);
            status=ptrack->getstatus();
            pcharge=ppart->getpcharge();// get pointer to charge, used in given particle
            pbeta=ppart->getpbeta();//pointer to tof beta
            betpatt=pbeta->getpattern();
            beta=pbeta->getbeta();//tof beta
	    momentum=ppart->getmomentum();
            chi2t=pbeta->getchi2();
            chi2s=pbeta->getchi2S();
            chargeTracker=pcharge->getchargeTracker();
	    TOF2JobStat::addre(24);
          }
	}//--->endof "trk-track particle" check
      }//--->endof "any track particle" check
//      if(GoodTrPart)break;//accept 1st particle with any good(not ecal-based) track
      if(GoodTrkTrack)break;//accept 1st particle with good Trk-track
Nextp:
      ppart=ppart->next();
    }//--->endof particles loop
//==================================================
    if(!GoodTrPart){
//cout<<"<--- rejected due to NotGoodTrPart"<<endl; 
      return;//require part. with good(good interpol, TRD|TRK-track based) particle
    }
    TOF2JobStat::addre(25);
//
    if(GoodTrkTrack)TOF2JobStat::addre(26);
//==================================================
//
    if(!GoodTrkTrack && TFCAFFKEY.truse>=0){
//cout<<"<--- rejected due to NotGoodTrkTrack"<<endl; 
      return;//require part. with TRK-track
    }
    TOF2JobStat::addre(27);
//==================================================
// define ref.beta for TZSL-calib and check mom-range(if measured, indiv. for tzsl/ampl):
    bool TzslMomOK(true);
    bool AmplMomOK(true);
    bool MomMeasExist(false);
    number imass;//implied mass
//
    if(TFCAFFKEY.caltyp==0)imass=pmas;//space calib (pr)
    else imass=mumas;//earth calib(mu)
//
    if(!GoodTrkTrack)rigid=10;//dummy value when no tracker(or magnet off)
    bet=1; 
    if(GoodTrkTrack){
//      pmom=fabs(rigid);
      pmom=momentum;//from part(>0=>pos.q)
      if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
        HF1(1505,geant(pmom),1.);
        HF1(1506,beta,1.);
        HF1(1507,chi2t,1.);
        HF1(1508,chi2s,1.);
        HF1(1509,chi2,1.);
}
      }
      if(TFCAFFKEY.truse>0){//use momentum-info from tracker for ref.beta calculation
        MomMeasExist=true;
	bet=pmom/sqrt(pmom*pmom+imass*imass); 
        TzslMomOK=(pmom>=TFCAFFKEY.pcut[0] && pmom<=TFCAFFKEY.pcut[1]);//check low/too_high mom, sel.pos.charge
//
	if(TFCAFFKEY.caltyp==0)AmplMomOK=(pmom>=TFCAFFKEY.plhc[0] && pmom<=TFCAFFKEY.plhc[1]);
	else AmplMomOK=(pmom>=TFCAFFKEY.plhec[0] && pmom<=TFCAFFKEY.plhec[1]);
      }
      else{//no momentum info from tracker (magnet off) - use implied average beta
        if(TFCAFFKEY.caltyp==0)bet=TFCAFFKEY.bmeanpr;// aver.beta for space Tzsl-calib(protons)
        else bet=TFCAFFKEY.bmeanmu;//aver.beta for earth Tzsl-calib(muons)
      }
    }
    betm=bet;//used by Tzslw-calib
    pmom=fabs(pmom);
//
    
//=================================================
//
// -----> Check TOF-Track match, get track length
//
    geant barw,dtcut,dlcut,hlen;
    number trlr0(0);
    if(!RelaxCut)dlcut=12;//about 3sigma
    else dlcut=30;//relaxed
    for(il=0;il<TOF2GC::SCLRS-1;il++)trlen[il]=0.;
    int nzlrs=0;
    int n2dml=0;//2d-matched layers
    int reflr=0;//really used as ref.layer (=1/2,from 2 possible on top)
    int bad4tdcal=1;//to count good events for TdelvCalib
    if(TFCAFFKEY.truse>=-1){
      C0[0]=0.;
      C0[1]=0.;
//
      for(il=0;il<TOF2GC::SCLRS;il++){//<-------- layers loop
        badx=0;
        bady=0;
        trlr[il]=0.;
	cstr[il]=0.;
        if(nbrl[il]==0)continue;//skip missing layer
	nzlrs+=1;
        ib=brnl[il];
        if(ib>0 && (ib+1)<TOF2DBc::getbppl(il))barw=TOF2DBc::plnstr(5);//stand.(not outer) bar width
        else barw=TOF2DBc::outcp(il,1);//outer counter width
        dtcut=barw/2-1.;//"-1" to be shure in good impact
        zc[il]=TOF2DBc::getzsc(il,ib);
        C0[2]=zc[il];
        ptrack->interpolate(C0,dir,Cout,the,phi,trl);
        cstr[il]=cos(the);
        trlr[il]=trl;
        if(TOF2DBc::plrotm(il)==0){
          coot[il]=Cout[1];// unrot. (X-meas) planes -> take trk Y-cross as long.coo
          ctran=Cout[0];// transv. coord.(abs. r.s.)(X-cross) 
          crc=Cout[1];//long.cr.point for Tdiff-calib
        }
        else {
          coot[il]=Cout[0];// rot. (Y-meas) planes -> take trk X-cross as long.coo
          ctran=Cout[1];// transv. coord.(abs. r.s.)(Y-cross) 
          crc=Cout[0];
        }
        dy=coot[il]-coo[il];//Long.coo_track-Long.coo_sc(based on prev.TdelvCalib)
        dx=ctran-TOF2DBc::gettsc(il,ib);//Transv.coo_tracker-Transv.coo_scint
        if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
          HF1(1200+il,geant(dy),1.);
          if(TFCAFFKEY.hprintf>1)HF1(1210+il,geant(dx),1.);
}
//#pragma omp critical (hf2)
//{
//          HF2(1204+il,geant(coot[il]),geant(dy),1.);
//}
        }
        hlen=0.5*TOF2DBc::brlen(il,ib);
        if(fabs(dx)>dtcut)badx=1;//bad transv.coo match->bad for any calib
	if(fabs(crc)>=hlen)bady=1;//cros.point out of bar-length(neglect plane's y-offset)->bad for any calib
	if(fabs(dy)>dlcut && !PMEQmode)bady=1;//bad longit.coo match(ignore for PM-equiliz run)
        if(badx==0 && fabs(crc)<hlen){//ok for Tdelv
	  bad4tdcal=0;//good event for TdelvCal(at least one layer was used)
#pragma omp critical (ctof_filltd)
{
	  TofTmAmCalib::filltd(il,ib,tmsdc[il],crc);//<---- TdelvCalib: need only transversal matching
}
	}
	if(badx>0 || bady>0){//mismatch in given layer, set "0" for related time/trlen values
	  brnl[il]=-1;//<=== mark bad layer (used for pm-equil mode, for Tz/Am calib "bad" is checked later)
	  nbrl[il]=0;
	  trlr[il]=0;
	  ltim[il]=0;
	  times[il]=0;
	  edep[il]=0;
	  am1[il]=0;
	  am2[il]=0;
	}
	else n2dml+=1;//count 2d-matched layers
//
      }//---------> endof layers loop
//
      cost=geant((fabs(cstr[0])+fabs(cstr[1])+fabs(cstr[2])+fabs(cstr[3]))/nzlrs);//average cos 
      if(TFCAFFKEY.hprintf>1){
#pragma omp critical (hf1)
{
        HF1(1215,cost,1.);
}
      }
//      
    }//-->endof "use TOF/TRD-track"mode check
//----------
    if(bad4tdcal==0)TOF2JobStat::addre(28);//just to count good events for Tdelv-calib
//---
    if(mode==2)return;//all is done for Tdelv-calib "alone" mode, exit
//---
    if(!PMEQmode){//normal TsAm-calib: cut on matched layers pattern
      if((Lref==1 && trlr[0]==0)||(Lref==2 && trlr[1]==0)||n2dml<3||(StrictLPatt && n2dml<4)){
//cout<<"<--- Rejected due to bad LPatt after match:"<<endl;
//cout<<"  nbrl:"<<endl<<nbrl[0]<<" "<<nbrl[1]<<" "<<nbrl[2]<<" "<<nbrl[3]<<endl;
//cout<<"  n2dml="<<n2dml<<endl<<" trlr="<<trlr[0]<<" "<<trlr[1]<<" "<<trlr[2]<<" "<<trlr[3]<<endl;
        return;//missing ref-layer
      }
    }
    TOF2JobStat::addre(29);
//
//---> create trlen[3] array(ref->2,ref->3,ref->4(ref=1 when L1-ok(otherwise ref=2), trlen[i]=0 when related pair is missing)
//
      if(Lref==1)trlr0=trlr[0];
      else trlr0=trlr[1];
      for(i=0;i<TOF2GC::SCLRS-1;i++){
        if(trlr[i+1]!=0.)trlen[i]=(trlr[i+1]-trlr0);//r->2,r->3,r->4(=0 for missing pair)
      }
//===============================================
//
//--------> more accurate TOF-beta fit(still using prev.calib times) :
//
    bool TofBetaFitOK(true);
    number ltim0(0);
    betof=0;
    if(!PMEQmode){//means at least 3 layers have to be present and are present
      trle[0]=0;
      trle[1]=trlen[0];
      trle[2]=trlen[1];
      trle[3]=trlen[2];
      tdif[0]=0;
      tdif[1]=0;
      tdif[2]=0;
      tdif[3]=0;
      
      if(Lref==1)ltim0=ltim[0];
      else ltim0=ltim[1];
      for(i=0;i<TOF2GC::SCLRS-1;i++){
        if(ltim[i+1]!=0.)tdif[i+1]=(ltim0-ltim[i+1]);//r->2,r->3,r->4(=0 for missing pair); >0 for bet>0
      }
      
      fpnt=0;
      sul=0;
      sut=0;
      sutl=0;
      sul2=0;
      sud=0.;
      for(il=0;il<TOF2GC::SCLRS;il++){
        if(nbrl[il]==0)continue;
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
      for(il=0;il<TOF2GC::SCLRS;il++)if(nbrl[il]>0)chsq+=pow((tzer+bci*trle[il]-tdif[il])/sigt[il],2);
      if(fpnt>2)chsq/=number(fpnt-2);
      betof=1./bci/cvel;
      if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
        if(fpnt==4)HF1(1503,betof,1.);
	if(fpnt<4)HF1(1516,betof,1.);
        HF1(1510,chsq,1.);
        HF1(1511,tzer,1.);
        HF1(1514,geant(fpnt),1.);
}
      }
      if(!RelaxCut)TofBetaFitOK=(chsq<25. && betof>0.6 && betof<1.3);//check on chi2/beta
      else TofBetaFitOK=(chsq<50. && betof>0.1 && betof<1.8);//relaxed check on chi2/beta
    }//--->endof "normal calib" mode check(imply 4 layers)
//---
    if(!TofBetaFitOK && !PMEQmode){
//cout<<"<--- Rejected by MyBetaFit:fpnt="<<fpnt<<" chi2="<<chsq<<" bet="<<betof<<endl;
      return;
    }
//---
    TOF2JobStat::addre(30);
//=======================================================
//
// -----> Identify events with high(ions) or too low edep.:
//
    bool IonEvent(false);
    number meanedep(0),maxedep(0);
    number betnor,absbet;
//    
    absbet=fabs(betof);// beta from TOF  
    if(absbet<0.94)betnor=pow(absbet,number(1.83))
                       /pow(number(0.94),number(1.83));//norm.factor to MIP(pr/mu)
    else betnor=1;
//
    for(il=0;il<TOF2GC::SCLRS;il++){//correct edep for impact-angle and beta!=1
      edep[il]*=(fabs(cstr[il])*betnor);//edep is based on previous calibr.
    }
//    
    for(i=0;i<TOF2DBc::getnplns();i++)pntr[i]=&edep[i];
    AMSsortNAG(pntr,TOF2DBc::getnplns());// sort in increasing order
    for(i=0;i<(TOF2DBc::getnplns()-1);i++){
      meanedep+=(*pntr[i]);//one can sum all layers - missing layers have edep=0
    }
    meanedep/=number(n2dml-1);//truncated("-" one layer with max Edep) average
    maxedep=*pntr[TOF2DBc::getnplns()-1];
    rr=maxedep/meanedep;//EDmax/EDaverage_of _the_rest
    if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
      HF1(1216,geant(meanedep),1.);
}
    }
    IonEvent=((meanedep > edecut) || chargeTracker>1); 
    if(IonEvent)TOF2JobStat::addre(31);
//=======================================================
//
// ---> look at mass/beta*gamma (and set flags for AbsNorm-calib) :
//
    number betg(4.);//default p/m(no momexist)
    number massq,mcut[2];
    geant prcut[2]={-10.,10.};//cuts on mass**2
    geant mucut[2]={-1.5,0.75};
    geant amt;
    integer id;
    if(TFCAFFKEY.caltyp==0){// space (prot) calibr.
      mcut[0]=prcut[0];
      mcut[1]=prcut[1];
    }
    else{                    // earth (mu) calibr.
      mcut[0]=mucut[0];
      mcut[1]=mucut[1];
    }
//
    massq=imass*imass;//implied m**2(if no mom.measurement)
    badx=0;
    bady=0;
    if(MomMeasExist){
      massq=pmom*pmom*(1.-betof*betof)/(betof*betof);//measured(TOFvsTRK) mass**2
      betg=pmom/imass;//use "tracker-defined" betg
      if(TFCAFFKEY.hprintf>1){ 
#pragma omp critical (hf1)
{
        HF1(1225,geant(massq),1.);
        if(fabs(betof)<0.88)HF1(1226,geant(massq),1.);
        HF1(1227,geant(betg),1.);
}
#pragma omp critical (hf2)
{
        HF2(1228,geant(pmom),geant(fabs(betof)),1.);
}
      }
//
      if(TFCAFFKEY.hprintf>1){ 
        for(il=0;il<TOF2GC::SCLRS;il++){
          ib=brnl[il];
	  if(ib>=0){
            id=100*(il+1)+ib+1;
            amt=am1[il]+am2[il];
            if(id==TofTmAmCalib::rbls[1]){
#pragma omp critical (hf2)
{
	      HF2(1229,geant(betg),amt,1.);//Atot vs bg(ref.bar type=2)
}
	    }
	  }
        }
      }
//
      if(massq<mcut[0] || massq>mcut[1])badx=1;
      if(betg<TFCAFFKEY.bgcut[0] || betg>TFCAFFKEY.bgcut[1])bady=1;
    }//--->endof "MomMeasExist" check
//
//=======================================================
    if(PMEQmode)goto SkipTzsl;//skip Tzsl for pm-equil.
//
// -----> prepare/fill arrays for TZSL-calibration:
//
    if((mode==3 || mode==23 || mode==234 || mode==34) && !PMEQmode){//<=== Tzsl-calib requested
      TOF2JobStat::addre(32);
//---
      if(!TzslMomOK)goto SkipTzsl; 
      TOF2JobStat::addre(33);
    if(TFCAFFKEY.hprintf>0){
#pragma omp critical (hf1)
{
      HF1(1512,betm,1.);
}
    }
//---
      bad=0;
      for(il=0;il<TOF2GC::SCLRS;il++){//check presence of trapez.counters
        ib=brnl[il];
        if(TFCAFFKEY.idref[1]==1
	                         && (ib==0 || (ib+1)==TOF2DBc::getbppl(il)))bad=1;
	if((ib==0 || (ib+1)==TOF2DBc::getbppl(il)) && fabs(coot[il])>20)bad=1;//trap.counter,use only central Longit-impact
      }
      if(bad==1)goto SkipTzsl;//ignore events with trapez.counters, when requested
      TOF2JobStat::addre(34);
      if(IonEvent)goto SkipTzsl;
   betm=0.998;//tempor
      if(TOF2JobStat::getre(35)>150000)goto SkipTzsl;//to limit statistics
      TOF2JobStat::addre(35);
//
//---> measured track-lengthes:
      for(il=0;il<TOF2GC::SCLRS-1;il++)tld[il]=trlen[il];//r->2,r->3,r->4(=0 for missing pair), tld>0
//
      if(n2dml==4){
        if(TFCAFFKEY.hprintf>1){
#pragma omp critical (hf1)
{
          HF1(1515,geant(tld[1]-tld[2]+tld[0]),1.);//Trlen13-Trlen24
}
	}
      }
//---> measured time-didderences:
      number times0(0);
      if(Lref==1)times0=times[0];//r=1
      else times0=times[1];//r=2
      for(i=0;i<TOF2GC::SCLRS-1;i++){
        tdm[i]=0.;
        if(times[i+1]!=0.)tdm[i]=times0-times[i+1];//r->2,r->3,r->4(=0 for missing pair), tdm>0 for bet>0
      }
//----
      for(il=0;il<TOF2GC::SCLRS;il++){//new parametrization(also ready for indiv.slopes)
        ramm[il]=0.;
        if(nbrl[il]==0)continue;//skip missing layer
        ib=brnl[il];
        TOF2Brcal::scbrcal[il][ib].getslops(slops);
        ramm[il]=(slops[0]/sqrt(qsd1[il])+slops[1]/sqrt(qsd2[il]));//works sl.better
      }
//cout<<"In ramm="<<ramm[0]<<" "<<ramm[1]<<" "<<ramm[2]<<" "<<ramm[3]<<endl;
//----
      ilay=0;
      tm[0]=trp1[ilay];
      tm[1]=trp2[ilay];
      time=0.5*(tm[0]+tm[1]);
      relt=time-ftdel;// subtract FT fix.delay
//----
      number ramm0(0);
      if(Lref==1)ramm0=ramm[0];//r=1
      else ramm0=ramm[1];//r=2
      for(i=0;i<TOF2GC::SCLRS-1;i++){
        dum[i]=0.;
        if(ramm[i+1]!=0.)dum[i]=ramm[i+1]-ramm0;//2-r,3-r,4-4 (=0 for missing pairs)
      }
//
      number tdr[TOF2GC::SCLRS-1];
      for(i=0;i<TOF2GC::SCLRS-1;i++)tdr[i]=tld[i]/betm/cvel;//real(reference) time-diffs
      number tdrm[TOF2GC::SCLRS-1];
      for(i=0;i<TOF2GC::SCLRS-1;i++)tdrm[i]=tdr[i]-tdm[i];//(ref.-meas.) time-diffs
//----
//
      integer brnlw[TOF2GC::SCLRS];
#pragma omp critical (ctof_filltz)
{
//cout<<"-----> Entry2filltz:event="<<(AMSEvent::gethead()->getid())<<" Bars:"<<brnl[0]<<" "<<brnl[1]<<" "<<brnl[2]<<" "<<brnl[3]<<endl;
//cout<<"       trlen:"<<trlr[0]<<" "<<trlr[1]<<" "<<trlr[2]<<" "<<trlr[3]<<endl;
//cout<<"       times:"<<times[0]<<" "<<times[1]<<" "<<times[2]<<" "<<times[3]<<endl;
//cout<<"          zc:"<<zc[0]<<" "<<zc[1]<<" "<<zc[2]<<" "<<zc[3]<<endl;
      for(i=0;i<TOF2GC::SCLRS;i++)brnlw[i]=brnl[i];//copy for Tzslw(because it can change brnl from inside)
//cout<<"       brnlw:"<<brnlw[0]<<" "<<brnlw[1]<<" "<<brnlw[2]<<endl;
//cout<<"       tdrm:"<<tdrm[0]<<" "<<tdrm[1]<<" "<<tdrm[2]<<endl;
//cout<<"       dum:"<<dum[0]<<" "<<dum[1]<<" "<<dum[2]<<"  betm="<<betm<<endl;
      TofTmAmCalib::filltz(brnlw,tdrm,dum); // fill calib.working arrays
}
    }//--->endof Tzsl-submode check
//
//===========================================================
SkipTzsl:
//
//=========> prepare/fill arrays for AMPL-calibration:
//
  if(mode==4 || mode==234 || mode==24 || mode==34){//<=== Ampl-calib requested
//
    TOF2JobStat::addre(36);
    if(!PMEQmode){//skip a2d for PMEQmode
//
// ---> Fill arrays for a2dr/gaind calculations(no mip-selection upto here,
//                                                 will use bar mid-bin only):
      for(il=0;il<TOF2GC::SCLRS;il++){
        ib=brnl[il];
        if(ib >= 0){//non-empty layer
          cinp=coot[il];// loc.r.s.!!!
	  ainp[0]=adca[il][0];//ADC-ch
	  ainp[1]=adca[il][1];
	  for(ip=0;ip<TOF2GC::PMTSMX;ip++){
	    dinp[0][ip]=dpma1[il][ip];
	    dinp[1][ip]=dpma2[il][ip];
	  }
          if(ainp[0]>adcmin && ainp[1]>adcmin){
#pragma omp critical (ctof_filla2dg)
{
	    TofTmAmCalib::filla2dg(il,ib,cinp,ainp,dinp);//for Anode/Sum(Dynode(ip)), Dgain(ip)
} 
	  }
        }
      }
//----
    }//--->endof "!PMEQmode" check
//
    if(IonEvent && !RelaxCut)return;
    TOF2JobStat::addre(37);
//
// ---> normalize charge to normal incidence :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      am1[il]*=fabs(cstr[il]);
      am2[il]*=fabs(cstr[il]);
    }
//
// ------> fill working arrays for Anode relat.gains calib :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      if(ib>=0){
        ainp[0]=geant(am1[il]);//q(pC)
        ainp[1]=geant(am2[il]);
        cinp=coot[il];// longit.coo of trk crossing, paddle loc.r.s.!!!
        if((ainp[0]>adcmin && ainp[1]>adcmin) || ((ainp[0]>adcmin || ainp[1]>adcmin) && PMEQmode)){
#pragma omp critical (ctof_fillam)
{
	  TofTmAmCalib::fillam(il,ib,ainp,cinp);//for relat.gains(using Anode) and PMEquilization
}
	} 
      }//--->endof nonempty layer check
    }//--->endof layer loop
//-------
    if(PMEQmode)return;//<===== All is done for PMEquilisation mode
//-----------------------
//
// ---> Normalize low beta Edep to MIP(p/m=4) :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      am1[il]*=betnor;
      am2[il]*=betnor;
    }
//
// ---> check mass/beta*gam for abs.normalization calib:
// 
    if(badx!=0 && !RelaxCut)return;//fail m**2 cut
    TOF2JobStat::addre(38);
    if(bady!=0 && !RelaxCut)return;//fail bet*gam(mip) cut
    TOF2JobStat::addre(39);
//
// ------> fill working arrays for Mip abs.normalization calib :
//
    for(il=0;il<TOF2GC::SCLRS;il++){
      ib=brnl[il];
      if(ib>=0){
        ainp[0]=geant(am1[il]);//q(pC)
        ainp[1]=geant(am2[il]);
        cinp=coot[il];// loc.r.s.!!!
        if(MomMeasExist){
          if(ainp[0]>adcmin && ainp[1]>adcmin){
#pragma omp critical (ctof_fillabs)
{
            TofTmAmCalib::fillabs(il,ib,ainp,cinp);//for abs.normalization(using Anode)
}
	  }
        }
        else{//no mom-info, put LowBeta-cut to select mip(raw approximation)
          if(ainp[0]>adcmin && ainp[1]>adcmin){
            if(fabs(betof)>0.85 || RelaxCut){
#pragma omp critical (ctof_fillabs)
{
	      TofTmAmCalib::fillabs(il,ib,ainp,cinp);
}
	    }
	  }
        }
      }//--->endof nonempty layer check
    }//--->endof layer loop
//----
  }//--->endof Ampl-calib section
//-------------------------------
}
//=============================================================================
void TofTmAmCalib::inittd(){ // ----> initialization for TDIF-calibration
  int i,j,id,il,ib,ii,jj,cnum;
  int hprtf=TFCAFFKEY.hprintf;
  char htit1[60];
  char htit2[60];
  char htit3[7];
  char inum[11];
  char in[2]="0";
//
  for(i=0;i<TOF2GC::SCBLMX;i++){
    for(j=0;j<TOF2GC::SCTDBM;j++){
      _tdiff[i][j]=0.;
      _tdif2[i][j]=0.;
      _clong[i][j]=0.;
      _nevnt[i][j]=0;
    }
  }
//
  if(hprtf>0){ 
// ---> book histograms for "Bar raw time" (data quality check for TimeCalib):
//  HBOOK2(1780,"T vs 1/Q, L=1",50,0.,0.1,80,40.,120.,0.);
//  HBOOK2(1781,"T vs 1/Q, L=4",50,0.,0.1,80,40.,120.,0.);
//
// ---> book histograms for "Tdiff_mean vs Clong"
//
  strcpy(inum,"0123456789");
  cnum=0;
  if(hprtf>1){
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
        id=1620+cnum;
        HBOOK1(id,htit1,TOF2GC::SCTDBM,0.,geant(TOF2GC::SCTDBM),0.);
        cnum+=1;//sequential solid numbering
      }
    }
//
    HBOOK2(1600,"L=1,B=6, Raw Tdif vs Coord (inp2fit)",35,-70.,70.,50,-20.,20.,0.);
    HBOOK2(1601,"L=3,B=7, Raw Tdif vs Coord (inp2fit)",35,-70.,70.,50,-20.,20.,0.);
    HBOOK2(1602,"L=3,B=8, Raw Tdif vs Coord (inp2fit)",35,-70.,70.,50,-20.,20.,0.);
    HBOOK2(1603,"L=3,B=9, Raw Tdif vs Coord (inp2fit)",35,-70.,70.,50,-20.,20.,0.);
  }
//---
  HBOOK1(1604,"BinTdiff/Coo_Fit T0(all layers/bars)",100,-15.,15.,0.);
  HBOOK1(1605,"BinTdiff/Coo_Fit Slope (all layers/bars)",80,0.045,0.085,0.);
  HBOOK1(1606,"Tdiff BinRms(all lay/bar/bins)",50,0.,2.,0.);
  HBOOK1(1607,"BinTdiff/Coo_Fit Chi2(all layers/bars)",50,0.,20.,0.);
  
  HBOOK1(1610,"BinTdiff/Coo_Fit Slope (L1 all bars)",80,0.045,0.085,0.);
  HBOOK1(1611,"BinTdiff/Coo_Fit Slope (L2 all bars)",80,0.045,0.085,0.);
  HBOOK1(1612,"BinTdiff/Coo_Fit Slope (L3 all bars)",80,0.045,0.085,0.);
  HBOOK1(1613,"BinTdiff/Coo_Fit Slope (L4 all bars)",80,0.045,0.085,0.);
  }
}
//------------------------- 
void TofTmAmCalib::filltd(integer il,integer ib, number td, number co){//--->fill arrays
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
  bin=2*hlen/_nbins[btyp-1];
// convert abs.coord. to local one:
  if(TOF2DBc::plrotm(il)==0){// unrotated plane
    col=co-coh[1];// local Y-coo(longit)
  }
  else{
    col=co-coh[0];
  }
  if((col < -(hlen-3)) || (col >= (hlen-3)))goto Exit;// cr.point is out of range(paddle length)
  if(TFCAFFKEY.hprintf>1){
    if(il==0 && ib==5)HF2(1600,col,td,1.);
    if(il==2 && ib==6)HF2(1601,col,td,1.);
    if(il==2 && ib==7)HF2(1602,col,td,1.);
    if(il==2 && ib==8)HF2(1603,col,td,1.);
  }
  if(fabs(td)>19.)goto Exit;//remove obviously unrealistic td
  nbin=integer(floor((col+hlen)/bin));
  if((ib==0 || (ib+1)==TOF2DBc::getbppl(il))){//trepezoidal counters
    if(nbin<=2 || nbin>=(_nbins[btyp-1]-3))goto Exit;//do not use 1st/last 3 bins
  }
  else{
    if(nbin<=0 || nbin>=(_nbins[btyp-1]-1))goto Exit;//do not use 1st/last bins
  }
  chan=TOF2DBc::barseqn(il,ib);
  if(chan<0 || chan>33){
    cout<<"<---- TofTmAmCalib::filltd:-F- WrongChannel,il/ib="<<il+1<<" "<<ib+1<<" ch="<<chan<<endl;
    goto Exit;
  }
  if(_nevnt[chan][nbin]<999){
    _clong[chan][nbin]+=col;
    _tdiff[chan][nbin]=_tdiff[chan][nbin]+td;
    _tdif2[chan][nbin]+=(td*td);
    _nevnt[chan][nbin]+=1;
  }
Exit:
  nbin=0;
} 
//------------------------- 
void TofTmAmCalib::fittd(){//--->Tdelv-calib: get the slope,td0,chi2
  int lspflg(1);//0/1->use single/array  for Lspeed
  integer il,ib,chan,nb,btyp,nev,bins,binsl[TOF2GC::SCLRS];
  integer ich;
  number bin,len,co,t,t2,dis,sig,sli,meansl(0),bintot(0),speedl,avsll[TOF2GC::SCLRS];
  number sl[TOF2GC::SCBLMX],t0[TOF2GC::SCBLMX],sumc,sumc2,sumt,sumt2,sumct,sumid,chi2[TOF2GC::SCBLMX];
  geant tdf[TOF2GC::SCTDBM];
  integer gchan,gsbins;
  geant gsbchan; 
  char fname[80];
  char frdate[30];
  char in[2]="0";
  char inum[11];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  integer cfvn;
  uinteger StartRun,overid,verid;
  time_t StartTime;
  int dig;
  strcpy(inum,"0123456789");
//
//--> get run/time of the first event(set at 1st event valid-stage using related AMSEvent values) 
//
  StartRun=AMSUser::JobFirstRunN();
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//--> create name for output file
// 
  strcpy(fname,"TofTdelv");
  if(AMSJob::gethead()->isMCData()){
    strcat(fname,vers1);
    overid=TOF2Brcal::CFlistC[3];
    verid=overid+1;// new=old+1
    sprintf(fext,"%d",verid);//MC-versn
    TOF2Brcal::CFlistC[3]=verid;//update Tdelv-calib version in static store
  }
  else{
    strcat(fname,vers2);
    overid=TOF2Brcal::CFlistC[2];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
    TOF2Brcal::CFlistC[2]=verid;//update Tdelv-calib version in static store
  }
  strcat(fname,".");
  strcat(fname,fext);
//
//
  if(TFCAFFKEY.hprintf>1){
    HPRINT(1600);
    HPRINT(1601);
    HPRINT(1602);
    HPRINT(1603);
  }
  chan=0;
  gchan=0;
  gsbchan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    avsll[il]=0.;
    binsl[il]=0;
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//      ich=TOF2DBc::barseqn(il,ib);
      len=TOF2DBc::brlen(il,ib);
      btyp=TOF2DBc::brtype(il,ib);//1->...
      bin=len/_nbins[btyp-1];
      sumc=0;
      sumt=0;
      sumct=0;
      sumc2=0;
      sumt2=0;
      sumid=0;
      bins=0;
      gsbins=0;
      sl[chan]=0;
      t0[chan]=0;
      chi2[chan]=0;
      cout<<"<===== Events/bin in lay/bar="<<(il+1)<<" "<<(ib+1)<<"  ===> ";
      for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_nevnt[chan][nb]<<" ";
      cout<<endl;
      for(nb=0;nb<TOF2GC::SCTDBM;nb++)tdf[nb]=0.;
//
      for(nb=0;nb<_nbins[btyp-1];nb++){//<--- bin-loop
        nev=_nevnt[chan][nb];
	if(nev>0){
	  _tdiff[chan][nb]/=nev;// mean td
          _tdif2[chan][nb]/=nev;//mean td2
	  _clong[chan][nb]/=nev;// mean co
	  tdf[nb]=geant(_tdiff[chan][nb]);
	}
        if(nev>=20){//min.cut on event number in bin
	  gsbins+=1;
          t=_tdiff[chan][nb];
          co=_clong[chan][nb];
	  t2=_tdif2[chan][nb];
          dis=t2-(t*t);
          if(dis>=0. && TFCAFFKEY.hprintf>0)HF1(1606,geant(sqrt(dis)),1.);
          if(dis>=0. && sqrt(dis)<=0.8){//max.cut on bin-rms
            dis=dis/(nev-1);
            sumc+=(co/dis);
            sumt+=(t/dis);
            sumid+=(1/dis);
            sumct+=(co*t/dis);
            sumc2+=(co*co/dis);
            sumt2+=(t*t/dis);
            bins+=1;
          }
	  else{
	    cout<<"   BadBinRms:td/td2="<<t<<" "<<t2<<" dis="<<dis<<" coo="<<co<<" nev="<<nev<<endl;
	  }
        }
      }// ---> end of bins loop
//
      for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_tdiff[chan][nb]<<" ";
      cout<<endl;
      for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_tdif2[chan][nb]<<" ";
      cout<<endl;
      for(nb=0;nb<_nbins[btyp-1];nb++)cout<<_clong[chan][nb]<<" ";
      cout<<endl;
      gsbchan+=(geant(gsbins)/_nbins[btyp-1]);//% of good statistics bins in given channel
      cout<<"  ---> NgoodBins="<<bins<<" sumc/sumt="<<sumc<<" "<<sumt<<" sumct="<<sumct<<" sumc2/t2="<<sumc2<<" "<<sumt2<<" sumis="<<sumid<<endl;
      if(TFCAFFKEY.hprintf>1)HPAK(1620+chan,tdf);
//
      if(bins>=5){
        t0[chan]=(sumt*sumc2-sumct*sumc)/(sumid*sumc2-(sumc*sumc));
        sl[chan]=(sumct*sumid-sumc*sumt)/(sumid*sumc2-(sumc*sumc));
        chi2[chan]=sumt2+t0[chan]*t0[chan]*sumid+sl[chan]*sl[chan]*sumc2
         -2*t0[chan]*sumt-2*sl[chan]*sumct+2*t0[chan]*sl[chan]*sumc;
        chi2[chan]/=number(bins-2);
        if(chi2[chan]<18. &&
                     fabs(sl[chan])>0.05 &&
                     fabs(sl[chan])<0.08){//only good for averaging
          bintot+=1;
          meansl+=sl[chan];
          avsll[il]+=sl[chan];
          binsl[il]+=1;
	  gchan+=1;
        }
        if(TFCAFFKEY.hprintf>0)HF1(1604,geant(t0[chan]),1.);
        if(TFCAFFKEY.hprintf>0  && fabs(sl[chan])>0.){
	  HF1(1605,geant(fabs(sl[chan])),1.);
	  if(il==0)HF1(1610,geant(fabs(sl[chan])),1.);
	  if(il==1)HF1(1611,geant(fabs(sl[chan])),1.);
	  if(il==2)HF1(1612,geant(fabs(sl[chan])),1.);
	  if(il==3)HF1(1613,geant(fabs(sl[chan])),1.);
	}
        if(TFCAFFKEY.hprintf>0)HF1(1607,geant(chi2[chan]),1.);
	cout<<"  ---> Chi2="<<chi2[chan]<<"  t0/slop="<<t0[chan]<<" "<<sl[chan]<<endl;
      }
      else{
        cout<<"  ---> Not enought bins for L/B="<<il+1<<" "<<ib+1<<" nbins="<<bins<<endl;
      }
      chan+=1;
    }//<------ end of bar loop
//
    if(binsl[il]>0)avsll[il]=fabs(avsll[il])/geant(binsl[il]);
    if(avsll[il]>0.)avsll[il]=1./avsll[il];
    else avsll[il]=15.2;//def.value
  }//<------ end of layer loop
//
  if(chan>0){//fill cal-quality array
    TOF2JobStat::cqual(0,0)=geant(gchan)/chan;//% of good chan(Nb>=4,Chi2=ok,slope in range)
    TOF2JobStat::cqual(0,1)=gsbchan/chan;//% of bins with good statistics
  }
  if(bintot>0)meansl/=bintot; // mean slope
  if(meansl!=0)speedl=fabs(1./meansl);// mean light speed
//
//---> print Td vs Coo histograms:
  if(TFCAFFKEY.hprintf>1){  
    chan=0;
    for(il=0;il<TOF2DBc::getnplns();il++){
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        if(chi2[chan]>=0)HPRINT(1620+chan);
        chan+=1;
      }
    }
  }
//---  
  cout<<endl<<endl;
  cout<<"TofTimeAmplCalib::fit: for bar 1-12  Tdiff@center (ns):"<<endl<<endl;
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      cout<<" "<<t0[chan];
      chan+=1;
    }
    cout<<endl;
  }
  cout<<endl;
  cout<<"TofTimeAmplCalib::fit: for bar 1-12  Light speed (cm/ns):"<<endl<<endl;
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
  cout<<"TofTimeAmplCalib::fit: for bar 1-12  Chi2 :"<<endl<<endl;
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
  if(TFCAFFKEY.hprintf>0){  
    HPRINT(1604);
    HPRINT(1606);
    HPRINT(1607);
  
    HPRINT(1605);//all layers slope
    HPRINT(1610);//slope by layers
    HPRINT(1611);//slope by layers
    HPRINT(1612);//slope by layers
    HPRINT(1613);//slope by layers
//
//---> print T vs 1/Q histograms:
//  HPRINT(1780);
//  HPRINT(1781);
  }
//
// ---> write mean light speed and Tdif's to file:
// 
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TofTimeAmplCalib:error opening file for output"<<fname<<'\n';
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
  tcfile << endl;
//
  tcfile << 12345 << endl;//endoffile label
  tcfile << endl;
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOFTDLVcalib:output file closed !"<<endl;
}
//=============================================================================
//
void TofTmAmCalib::initam(){ // ----> initialization for AMPL-calibration 
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
  if(TFCAFFKEY.truse == -1){
    cout<<"AMPLcalib::init: Use TRK/TRD-track, no Momentum knowledge !!!"<<endl;
  }
  else if(TFCAFFKEY.truse == 0){
    cout<<"AMPLcalib::init: Use TRK-track, no Momentum knowledge !!!"<<endl;
  }
  else if(TFCAFFKEY.truse == 1){
    cout<<"AMPLcalib::init: Use TRK-track and Momentum knowledge !!!"<<endl;
  }
  else{
    cout<<"AMPLcalib::init: Unknown mode of track usage !!!"<<endl;
    exit(1);
  }
  strcpy(inum,"0123456789");
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    nevenc[i]=0;
    gains[i]=0.;
    a2dr[i]=0;
    a2dr2[i]=0;
    neva2d[i]=0;
    nevdgn[i]=0;
    for(int ip=0;ip<TOF2GC::PMTSMX;ip++){
      d2sdr[i][ip]=0;
      d2sdr2[i][ip]=0;
    }
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
        id=1700+chan;
    
//        if(TFCAFFKEY.hprintf>0)HBOOK1(id,htit1,80,hll,hhl,0.);
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
      id=1701+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+i*TOF2GC::SCPRBM+j;
//      if(TFCAFFKEY.hprintf>0)HBOOK1(id,htit2,80,0.,960.,0.);
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
    cout<<"AMPLcalib::init:Bartype="<<i<<" equal_bins width="<<bw<<endl;
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
  if(TFCAFFKEY.hprintf>1){
    HBOOK1(1225,"Mass**2",80,-1.,7.,0.);
    HBOOK1(1226,"Mass**2 for beta<0.92",80,-1.,7.,0.);
    HBOOK1(1227,"Momentum/impl.mass(beta*gamma)",80,0.,24.,0.);
    HBOOK2(1228,"My TOF-beta vs TrkTrack-momentum",80,0.,4.,70,0.5,1.2,0.);
    HBOOK2(1229,"Q(ref.btyp=2) vs (beta*gamma)",80,0.,40.,60,0.,300.,0.);
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
    if(TFCAFFKEY.hprintf>0)HBOOK1(1230+i,htit1,70,bl,bh,0.);
    aprofp[i][0]=50.;//defaults param.(A) for A-profile fit(s1)
    aprofp[i][1]=195.;//              (Lh)
    aprofp[i][2]=0.;//                (Wl)
    aprofp[i][3]=10.;//               (Ll)
    aprofp[i][4]=0.;//                (Abk)
    aprofp[i][5]=10.;//               (Lbk)
    aprofp[i][6]=aprofp[i][0];//defaults param. for A-profile fit(s2)
    aprofp[i][7]=aprofp[i][1];
    aprofp[i][8]=aprofp[i][2];
    aprofp[i][9]=aprofp[i][3];
    aprofp[i][10]=aprofp[i][4];               
    aprofp[i][11]=aprofp[i][5];               
  }
//
  if(TFCAFFKEY.hprintf>0){
    HBOOK1(1250,"Instant Ah/Dh(pm-sum)(LBBS=1041,midd.bin)",50,5.,15.,0.);
    HBOOK1(1251,"Instant Dh(pm) rel.gain(LBBS=1041,PM=1,midd.bin)",50,0.5,1.5,0.);
    HBOOK1(1252,"Instant Dh(pm) rel.gain(LBBS=1041,PM=2,midd.bin)",50,0.5,1.5,0.);
//    HBOOK1(1254,"Instant Ah/Al for LBBS=1041",60,2.,8.,0.);//spare
//    HBOOK1(1255,"Instant Dh(pm)/Dl(pm) for LBBS=1041,PM=1",50,0.,10.,0.);//spare
//    HBOOK1(1253,"Instant Dh(pm)/Dl(pm) for LBBS=1042,PM=1",50,0.,10.,0.);//spare
    HBOOK1(1256,"Average Ah/Dh(pm-sum) (all chan, midd.bin)",80,3.,11.,0.);
    HBOOK1(1257,"RelatRMS of  aver. Ah/Dh(pm-sum) (all chan, midd.bin)",50,0.,1.,0.);
    HBOOK1(1258,"Average Dh(pm) rel.gains(all chan/pm,m.bin)",50,0.5,1.5,0.);
    HBOOK1(1259,"RelatRMS of aver. Dh(pm) rel.gains(all chan/pm,m.bin)",50,0.,1.,0.);
// hist.1800-1911 are booked in init-function for D(h) vs A(h) correlation!!!
//
//    HBOOK1(1260,"Ref.bar(type=2) Q-distr.(s=1,centre)",80,0.,160.,0.);//spare        
//    HBOOK1(1261,"Ref.bar(type=2) Q-distr.(s=2,centre)",80,0.,160.,0.);//spare
    HBOOK1(1262,"Relative anode-gains(all channels)",80,0.3,2.5,0.);
    if(TFCAFFKEY.hprintf>1){
      HBOOK1(1264,"Ref.bar A-profile (s1,type-1)",70,-70.,70.,0.);        
      HBOOK1(1265,"Ref.bar A-profile (s2,type-1)",70,-70.,70.,0.);        
      HBOOK1(1266,"Ref.bar A-profile (s1,type-2)",70,-70.,70.,0.);        
      HBOOK1(1267,"Ref.bar A-profile (s2,type-2)",70,-70.,70.,0.);        
      HBOOK1(1268,"Ref.bar A-profile (s1,type-3)",70,-70.,70.,0.);        
      HBOOK1(1269,"Ref.bar A-profile (s2,type-3)",70,-70.,70.,0.);        
      HBOOK1(1270,"Ref.bar A-profile (s1,type-4)",70,-70.,70.,0.);        
      HBOOK1(1271,"Ref.bar A-profile (s2,type-4)",70,-70.,70.,0.);        
      HBOOK1(1272,"Ref.bar A-profile (s1,type-5)",70,-70.,70.,0.);        
      HBOOK1(1273,"Ref.bar A-profile (s2,type-5)",70,-70.,70.,0.);        
      HBOOK1(1274,"Ref.bar A-profile (s1,type-6)",70,-70.,70.,0.);        
      HBOOK1(1275,"Ref.bar A-profile (s2,type-6)",70,-70.,70.,0.);        
      HBOOK1(1276,"Ref.bar A-profile (s1,type-7)",70,-70.,70.,0.);        
      HBOOK1(1277,"Ref.bar A-profile (s2,type-7)",70,-70.,70.,0.);        
      HBOOK1(1278,"Ref.bar A-profile (s1,type-8)",70,-70.,70.,0.);        
      HBOOK1(1279,"Ref.bar A-profile (s2,type-8)",70,-70.,70.,0.);        
      HBOOK1(1280,"Ref.bar A-profile (s1,type-9)",70,-70.,70.,0.);        
      HBOOK1(1281,"Ref.bar A-profile (s2,type-9)",70,-70.,70.,0.);        
      HBOOK1(1282,"Ref.bar A-profile (s1,type-10)",70,-70.,70.,0.);        
      HBOOK1(1283,"Ref.bar A-profile (s2,type-10)",70,-70.,70.,0.);        
      HBOOK1(1284,"Ref.bar A-profile (s1,type-11)",70,-70.,70.,0.);        
      HBOOK1(1285,"Ref.bar A-profile (s2,type-11)",70,-70.,70.,0.);
    }          
    HBOOK1(1286,"Average Ah/Al(all chan, midd.bin)",50,2.5,7.5,0.);
    HBOOK1(1287,"RelatRMS of aver. Ah/Al(all chan, midd.bin)",50,0.,5.,0.);
    HBOOK1(1288,"Average Dh/Dl(all chan/pm, midd.bin)",50,2.5,7.5,0.);
    HBOOK1(1289,"RelatRMS of aver. Dh/Dl(all chan/pm, midd.bin)",50,0.,2.5,0.);
  }
//
}
//--------------------------
void TofTmAmCalib::endjam(){ // ----> print Ampl-hists 
  if(TFCAFFKEY.hprintf>1){
    HPRINT(1225);//filled in select
    HPRINT(1226);
    HPRINT(1227);
    HPRINT(1228);
    HPRINT(1229);
//    HPRINT(1260);//spare (filled in fillam)
//    HPRINT(1261);//........................
  }
  TofTmAmCalib::fitam();//fill h#1250-1259,1264-1285(ref.bar-A-profiles)
  if(TFCAFFKEY.hprintf>0){
    for(int i=0;i<TOF2GC::SCBTPN;i++)HPRINT(1230+i);//Ref.bars Imp.p.distr,filled in fillam
    HPRINT(1250);
    HPRINT(1251);
    HPRINT(1252);
//    HPRINT(1254);
//    HPRINT(1255);
//    HPRINT(1253);
    HPRINT(1256);
    HPRINT(1257);
    HPRINT(1258);
    HPRINT(1259);
  
    HPRINT(1262);
    HPRINT(1286);
    HPRINT(1287);
    HPRINT(1288);
    HPRINT(1289);
    if(TFCAFFKEY.hprintf>1){	    
      for(int i=0;i<2*TOF2GC::SCBTPN;i++)HPRINT(1264+i);//ref.bars s1/s2 A-profs, filled in fitam
    }
  }
} 
//--------------------------------------
//   ---> program to accumulate data for relat.gains-calibration:
void TofTmAmCalib::fillam(integer il, integer ib, geant am[2], geant coo){
//
  integer i,id,idr,idh,ibt,btyp,nbn,nb,nbc,isb,chan,nev,bchan;
  geant r;
  geant bl,bh,qthrd;
  geant cbin;// centr. bin half-width for gain calibr.
  if(TFCAFFKEY.spares[0]>0)cbin=20;//relaxed range for PM-equilization procedure
  else cbin=15;
//
  isb=TOF2DBc::barseqn(il,ib);//bar sequential number(0->...)
  chan=2*isb;//side-1 seq. number(2*isb+1 for side-2)
  ibt=TOF2DBc::brtype(il,ib);// bar type (1-11)
  if(ibt<1 || ibt>11){
    cout<<"<------- TofTmAmCalib::fillam: wrong btype="<<ibt<<endl;
    goto Exit;
  }
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
  if(idr==0){
    cout<<"<------- TofTmAmCalib::fillam:illegal ref.bar id ! "<<id<<endl;
    goto Exit;
  }
  nbn=nprbn[btyp];// number of long.bins
//
  nbc=-1;
  for(nb=0;nb<nbn;nb++){ // define bin number for current coo
    bl=profp[btyp][nb]-0.5*profb[btyp][nb];
    bh=bl+profb[btyp][nb];
    if(coo>=bl && coo<bh){
      nbc=nb;
      break;
    }
  }
  if(nbc<0){
    bl=profp[btyp][0]-0.5*profb[btyp][0];
    bh=profp[btyp][nbn-1]+0.5*profb[btyp][nbn-1];
    if((bl-coo)>2. || (coo-bh)>2.){//big "outrange"->give warning...
      cout<<"<------- TofTmAmCalib::fillam:-W- LongCrosCoo out_of_Blen !,coo="<<coo<<" l/h="<<bl<<" "<<bh<<endl;
    }
    goto Exit;
  }
  bchan=TOF2GC::SCPRBM*btyp+nbc;
//                             ---> fill arrays/hist. for gains:
//
  if(fabs(coo) < cbin){// select only central incidence(+- cbin cm)
    nev=nevenc[chan];
    if(nev<TOF2GC::SCACMX && am[0]>0){  
      amchan[chan][nev]=am[0];
      nevenc[chan]+=1;
    }
    nev=nevenc[chan+1];
    if(nev<TOF2GC::SCACMX && am[1]>0){  
      amchan[chan+1][nev]=am[1];
      nevenc[chan+1]+=1;
    }
    if(TFCAFFKEY.hprintf>0){
      idh=1700+chan+0;// side-1
//      HF1(idh,geant(am[0]),1.);
      idh=1700+chan+1;// side-2
//      HF1(idh,geant(am[1]),1.);
    }
  }
//                             ---> fill profile arrays/hist. for ref. bars:
  if(TFCAFFKEY.spares[0]>0)goto Exit;//skip for PMEQmode
//
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
    if(TFCAFFKEY.hprintf>0){
      HF1(1230+btyp,coo,1.);// longit.imp.point distribution
//      if(ibt == 2 && fabs(coo) < cbin){
//        HF1(1260,am[0],1.);// Q-distr. for ref.bar type=2, s=1
//        HF1(1261,am[1],1.);// Q-distr. for ref.bar type=2, s=2
//      }
      idh=1701+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+bchan;
//      HF1(idh,geant(am[0]+am[1]),1.);
    }
  }
Exit:
  cbin=0;//dummy action
}
//--------------------------------------
//            ---> program to accumulate data for abs.normalization:
void TofTmAmCalib::fillabs(integer il, integer ib, geant am[2], geant coo){
//
  integer i,id,idr,ibt,btyp,nev;
  geant cbin(15.);// centr. bin half-width for abs-norm calibr.
  geant amt,mcut[2];
//
  ibt=TOF2DBc::brtype(il,ib);// bar type (1-10)
  btyp=ibt-1;
  id=100*(il+1)+ib+1;
  idr=rbls[btyp];// ref.bar id for given bar
//
  if(id != idr)goto Exit;//only for ref.counters
  if(fabs(coo) > cbin)goto Exit;// select only central incidence(+- cbin cm)
//
  amt=(am[0]+am[1]);
//
  nevrfc[btyp]+=1;
  ammrfc[btyp]+=amt;
  nev=nrefb[btyp];
  if(nev<TOF2GC::SCACMX){
    arefb[btyp][nev]=(am[0]+am[1]);
    nrefb[btyp]+=1;
  }
Exit:
  ibt=1;
}
//
//--------------------------------------
// (prog.to fill arrays for a2dr and Dgain[ip] calculations)
//
void TofTmAmCalib::filla2dg(int il, int ib, geant cin,
                                     geant ain[2], geant din[2][TOF2GC::PMTSMX]){
//
  integer ip,is,chnum,npmts,ngdp[2],chn;
  geant dsum[2],mina,maxa,mind,maxd;
  geant r,cbin(10);
//
  npmts=TOF2Brcal::scbrcal[il][ib].getnpm();
  chnum=2*TOF2DBc::barseqn(il,ib);//sequential channels numbering (s=1)
//
  for(is=0;is<2;is++){//shift to the middle of the adc-bin
    if(ain[is]>0)ain[is]+=0.5;
    for(ip=0;ip<npmts;ip++){
      if(din[is][ip]>0)din[is][ip]+=0.5;
    }
  }
//
  if(fabs(cin) < cbin){// <-- select only central incidence(+- cbin cm)
    for(is=0;is<2;is++){//<-- side loop
      chn=chnum+is;
      dsum[is]=0;
      ngdp[is]=0;
      mina=5*TOFBPeds::scbrped[il][ib].asiga(is);//5sig
      maxa=TOF2GC::SCPUXMX-mina;//to avoid ovfls(anode saturation area)
      mina=8*20;//tempor(to have(at def.a2d=6) Ad1,2>10 to avoid nonlin of beg.of D-signals scale)
      if(ain[is]>mina && ain[is]<maxa){// Ah ok
        for(ip=0;ip<npmts;ip++){
          mind=5*TOFBPeds::scbrped[il][ib].asigd(is,ip);//5sig
	  maxd=TOF2GC::SCPUXMX-mind;
          if(din[is][ip]>mind && din[is][ip]<maxd)ngdp[is]+=1;//Dh[ip] ok
        }
      }
      if(ngdp[is]==npmts){// <-- all adc's OK -> fill arrays
        for(ip=0;ip<npmts;ip++){
	  dsum[is]+=din[is][ip];
        }
	r=ain[is]/dsum[is];
        if(chn==6 && TFCAFFKEY.hprintf>0)HF1(1250,r,1.);//inst.Ah/Dh(pm-sum) for LBBS=1041
	a2dr[chn]+=r;
	a2dr2[chn]+=r*r;
	neva2d[chn]+=1;
	dsum[is]/=geant(npmts);//Dh average over npmts
	for(ip=0;ip<npmts;ip++){
	  r=din[is][ip]/dsum[is];
          if(TFCAFFKEY.hprintf>0){
            if(chn==6 && ip==0)HF1(1251,r,1.);//inst.Dg(pm) rel.gain for LBBS=1041, pm=1
            if(chn==6 && ip==1)HF1(1252,r,1.);//inst.Dg(pm) rel.gain for LBBS=1041, pm=2
	  }
	  d2sdr[chn][ip]+=r;
	  d2sdr2[chn][ip]+=r*r;
	}
	nevdgn[chn]+=1;
      }//-->endof "adc's OK" check
    }//--->endof side-loop
  }//-->endof centr.bin check
}
//--------------------------------------
//            ---> program to get final AMPL-calibration:
void TofTmAmCalib::fitam(){
//
  integer il,ib,is,i,j,k,n,ii,jj,id,idr,btyp;
  integer glosta[TOF2GC::SCCHMX];
  int ic,ich;
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
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  integer cfvn;
  uinteger StartRun,overid,verid;
  time_t StartTime;
  int dig;
  strcpy(inum,"0123456789");
//
//--> get run/time of the first event
//
  StartRun=AMSUser::JobFirstRunN();
  StartTime=time_t(StartRun);
  strcpy(frdate,asctime(localtime(&StartTime)));
//
//--> create name for output file
// 
  strcpy(fname,"TofAmplf");
  if(AMSJob::gethead()->isMCData()){
    strcat(fname,vers1);
    overid=TOF2Brcal::CFlistC[5];
    verid=overid+1;//new MC-versn old+1
    sprintf(fext,"%d",verid);
    TOF2Brcal::CFlistC[5]=verid;//update Ampl-calib version in static store
  }
  else{
    strcat(fname,vers2);
    overid=TOF2Brcal::CFlistC[4];
    if(overid==StartRun)verid=overid+1;//for safety when rerun over the same Data
    else verid=StartRun;
    sprintf(fext,"%d",verid);//RD-Run# = UTC-time of 1st "on-board" event
    TOF2Brcal::CFlistC[4]=verid;//update Ampl-calib version in static store
  }
  strcat(fname,".");
  strcat(fname,fext);
//
//
// ---> print "gain"-hist. (side-signals for center bin)
  if(TFCAFFKEY.hprintf>0){
    chan=0;
    for(il=0;il<TOF2DBc::getnplns();il++){   
      for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
        for(i=0;i<2;i++){
          glosta[chan]=0;
          id=1700+chan;
//          HPRINT(id);
          chan+=1;
        }
      }
    }
  }
//
//
// ---> print "profile"-hist. (tot.signal vs long.bin for ref.counters):
//
  if(TFCAFFKEY.hprintf>0){
//    for(i=0;i<TOF2GC::SCBTPN;i++){ // loop over bar types  
//      nbnr=nprbn[i];//real numb. of bins
//      if(nbnr==0)continue;
//      for(j=0;j<nbnr;j++){ // loop over longit.bins
//        id=1701+TOF2GC::SCLRS*TOF2GC::SCMXBR*2+i*TOF2GC::SCPRBM+j;
//        HPRINT(id);
//      }
//    }
  }
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
  int goodch;
  geant rbnn,bnw,bnl,bnh;
  ic=0;
  goodch=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(is=0;is<2;is++){
        SideMPA[il][ib][is]=0;
        nev=nevenc[ic];
        aver=0;
        if(nev>=TFCAFFKEY.minev){
          for(k=0;k<nev;k++)pntr[k]=&amchan[ic][k];//pointers to event-signals of chan=i 
          AMSsortNAG(pntr,nev);//sort in increasing order
          nmax=int(floor(nev*TFCAFFKEY.trcut));// to keep (100*trcut)% of lowest amplitudes
          nmin=int(floor(nev*0.015));// to remove 1.5 % of lowest amplitudes
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
          if(TFCAFFKEY.hprintf>1)HPRINT(1599);
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
          MNEXCM(TofTmAmCalib::melfun,"CALL FCN",arglp,1,ierp,0);
          if(ierp!=0){
            cout<<"TOF:Mp-side-calib: final CALL_FCN problem !"<<'\n';
            continue;
          }
          gains[ic]=elfitp[1];
	  SideMPA[il][ib][is]=elfitp[1];//store for PM-equilization
	  goodch+=1;
          HDELET(1599);
//
        }// ---> endof min.events check
//
	ic+=1;//sequential numbering of bar sides
//
      }// ---> endof side loop
    }// ---> endof bar loop
  }// ---> endof layer loop
//---
  if(ic>0)TOF2JobStat::cqual(2,0)=geant(goodch)/ic;//for cal-quality
//---
  if(TFCAFFKEY.spares[0]>0){//for PM-equilization procedure
    ic=0;
    printf("----------------------------------------------------------\n");
    printf("Collected events per layer/paddle/side:\n");
    printf("\n");
    for(il=0;il<TOF2DBc::getnplns();il++){
      printf("Layer= %2d\n",(il+1));
      for(is=0;is<2;is++){
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          chan=ic+2*ib+is;
          printf("%6d",nevenc[chan]);
	}
        printf("\n");
      }
      printf("\n");
      ic+=2*TOF2DBc::getbppl(il);
    }
//
    printf("----------------------------------------------------------\n");
    printf("MostProb Amplitude(ADC-ch) per layer/paddle/side:\n");
    printf("\n");
    for(il=0;il<TOF2DBc::getnplns();il++){
      printf("Layer= %2d\n",(il+1));
      for(is=0;is<2;is++){
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          printf("%6.1f",SideMPA[il][ib][is]);
	}
        printf("\n");
      }
      printf("\n");
    }
//
    strcpy(fname,"TofSideMPA.");
    strcat(fname,fext);
    ofstream tcfile(fname,ios::out|ios::trunc);
    if(!tcfile){
      cerr<<"TofTmAmCalib::PmtEquil:error opening file for output "<<fname<<'\n';
      exit(8);
    }
    cout<<"Open file for PmtEquilizTable output, fname:"<<fname<<'\n';
    cout<<" First run used for calibration is "<<StartRun<<endl;
    cout<<" Date of the first event : "<<frdate<<endl;
//
    tcfile.setf(ios::fixed);
    tcfile.width(6);
//
    tcfile.precision(1);// precision for MPA
    for(il=0;il<TOF2DBc::getnplns();il++){
      tcfile << "Layer="<< (il+1) << endl;;
      for(is=0;is<2;is++){
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
          tcfile << SideMPA[il][ib][is] << " ";
	}
        tcfile << endl;
      }
      tcfile << endl;
    }
    tcfile << endl;
    tcfile << endl<<"======================================================"<<endl;
    tcfile << endl<<" First run used for PmtEquilizationTable is "<<StartRun<<endl;
    tcfile << endl<<" Date of the first event : "<<frdate<<endl;
    tcfile.close();
//
    return;
  } 
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
        if(id != idr && TFCAFFKEY.hprintf>0)HF1(1262,gains[chan+i],1.);
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
        nmax=int(floor(nev*TFCAFFKEY.trcut));// to keep (100*trcut)% of lowest amplitudes
        nmin=int(floor(nev*0.02));// to remove 2 % of lowest amplitudes
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
        if(TFCAFFKEY.hprintf>1 || (ibt==0 || ibt==2 || ibt==4 || ibt==7)&&(ibn<3 || ibn>nbnr-4))HPRINT(1599);
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
        MNEXCM(TofTmAmCalib::melfun,"CALL FCN",arglp,1,ierp,0);
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
      if(TFCAFFKEY.hprintf>1)HF1(1264+2*ibt+isd,profp[ibt][ibn],btamp[isd][bchan]);
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
  strcpy(pnam[0],"aforw");
  strcpy(pnam[1],"lenhi");
  strcpy(pnam[2],"wmilo");
  strcpy(pnam[3],"lenlo");
  strcpy(pnam[4],"aback");
  strcpy(pnam[5],"lenbk");
//
  start[0]=50.;
  start[1]=195.;
  start[2]=0.;
  start[3]=18.;
  start[4]=0.;
  start[5]=11.;
//
//  if(AMSJob::gethead()->isMCData()){ // tempor solution for MC
//    start[1]=190.;
//    start[3]=5.;
//  }
//
  pstep[0]=5.;
  pstep[1]=10.;
  pstep[2]=0.1;
  pstep[3]=3.;
  pstep[4]=5.;
  pstep[5]=3.;
//
  plow[0]=1.;
  plow[1]=50.;
  plow[2]=-0.001;
  plow[3]=2.;
  plow[4]=0.;
  plow[5]=2.;
//
  phigh[0]=500.;
  phigh[1]=800.;
  phigh[2]=1.;
  phigh[3]=50.;
  phigh[4]=500.;
  phigh[5]=50.;
//
  for(i=0;i<TOF2GC::SCPROFP;i++)ifit[i]=1;//release all parms
// ------------> initialize some parameters for Minuit:
  MNINIT(5,6,6);
  MNSETI("Ampl<->Position dependence calibration for TOF-system");
  argl[0]=number(-1);
  MNEXCM(mfunam,"SET PRINT",argl,1,ier,0);
  argl[0]=number(1);//UP(=1-> chi2 methode, =0.5->likelihood)
  MNEXCM(mfunam,"SET ERR",argl,1,ier,0);
//
//------> start fit for each bar-type:
//
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
          cout<<"       q/err="<<mcharge[nbinr]<<" "<<emchar[nbinr]<<" coo="<<mcoord[nbinr]<<endl;
          nbinr+=1;
        }
      }// ---> endof bins loop
//
      cout<<"    Good statistics bins:"<<nbinr<<endl;
      if(nbinr<5)goto nextsd;//too few bins, skip this side
//
      start[0]=50.;
      start[1]=195.;
      start[2]=0.;
      start[3]=18.;//Llow= specific to normal count.
      if(ibt==0 || ibt==2 || ibt==4 || ibt==7)start[3]=7.;//Llow= specific to trapezoidal
      start[4]=0.;
      start[5]=11.;//Lback= specific to trapez. count
      for(i=0;i<TOF2GC::SCPROFP;i++){//<--- initialize all parameters
        strcpy(pnm,pnam[i]);
	if(i==0){// more realistic "start" value for 1st param.
	  if(isd==0)start[i]=mcharge[0];
	  if(isd==1)start[i]=mcharge[nbinr-1];
	}
	if(i==4){// more realistic "start" value for 5th param("aback").
	  if(ibt==2 || ibt==4 || ibt==7){//non-zero 5th param("aback") for trapez.counters
	    if(isd==0)start[i]=mcharge[nbinr-1];
	    if(isd==1)start[i]=mcharge[0];
	  }
	}
	cout<<"Init:StartParN/val="<<i+1<<" "<<start[i]<<" pl/ph="<<plow[i]<<" "<<phigh[i]<<endl;
        ier=0;
        MNPARM((i+1),pnm,start[i],pstep[i],plow[i],phigh[i],ier);//init
        if(ier!=0){
          cout<<"TOF-calib:LprofileFit: Param-init problem for par-id="<<pnam[i]<<'\n';
          exit(10);
        }
      }// ---> endof initialize
//
      if(ibt==0 || ibt==2 || ibt==4 || ibt==7){//<--- fix/rel some pars. for trapezoidal counters
        fitflg=1;//use formula with "back" (reflection) part
	if(ifit[1]==0){//rel
          ifit[1]=1;
          argl[0]=number(2);//lenhi
          ier=0;
          MNEXCM(mfunam,"RELEASE",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-fix problem for par-id="<<pnam[1]<<'\n';
            exit(10);
          }
	}
	if(ifit[3]==1){//fix
          ifit[3]=0;
          argl[0]=number(4);//"lenlo"
          ier=0;
          MNEXCM(mfunam,"FIX",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[3]<<'\n';
            exit(10);
          }
	}
	if(ifit[4]==0){//release
          ifit[4]=1;
          argl[0]=number(5);//"aback"
          ier=0;
          MNEXCM(mfunam,"RELEASE",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[4]<<'\n';
            exit(10);
          }
	}
	if(ifit[5]==1){//fix
          ifit[5]=0;
          argl[0]=number(6);//"lenbk
          ier=0;
          MNEXCM(mfunam,"FIX",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[5]<<'\n';
            exit(10);
          }
	}
      }
//
      else{//                          <--- Fix/rel some params. for other types counters
        fitflg=0;//use formula with "front" second slope
	
	if(ifit[1]==0){//rel
          ifit[1]=1;
          argl[0]=number(2);//"lenhi"
          ier=0;
          MNEXCM(mfunam,"RELEASE",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[1]<<'\n';
            exit(10);
          }
	}
	
	if(ifit[3]==1){//fix
          ifit[3]=0;
          argl[0]=number(4);//"lenlo"
          ier=0;
          MNEXCM(mfunam,"FIX",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-release problem for par-id="<<pnam[3]<<'\n';
            exit(10);
          }
	}
	if(ifit[4]==1){//fix
          ifit[4]=0;
          argl[0]=number(5);//"aback"
          ier=0;
          MNEXCM(mfunam,"FIX",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-fix problem for par-id="<<pnam[4]<<'\n';
            exit(10);
          }
	}
	if(ifit[5]==1){//fix
          ifit[5]=0;
          argl[0]=number(6);//"lenbk"
          ier=0;
          MNEXCM(mfunam,"FIX",argl,1,ier,0);
          if(ier!=0){
            cout<<"TOF-calib: LProfileParam-fix problem for par-id="<<pnam[5]<<'\n';
            exit(10);
          }
	}
      }
      cout<<"Param-release mask:"<<100000*ifit[0]+10000*ifit[1]+1000*ifit[2]+100*ifit[3]+10*ifit[4]+ifit[5]<<endl;
//
// ------> start minimization:
      argl[0]=number(1000);//max calls
      argl[1]=number(1);//tolerance, minim. stops when dist<0.001*tolerance*UP
      ier=0;
      MNEXCM(mfunam,"MINIMIZE",argl,2,ier,0);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: MINIMIZE problem !"<<'\n';
        goto nextsd;
      }  
      MNEXCM(mfunam,"MINOS",argl,0,ier,0);
      if(ier!=0){
        cout<<"TOF-calib:LprofileFit: MINOS problem !"<<'\n';
        goto nextsd;
      }
      argl[0]=number(3);
      ier=0;
      MNEXCM(TofTmAmCalib::mfunam,"CALL FCN",argl,1,ier,0);
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
  integer btypeok(0),btypes(0);
  for(ibt=0;ibt<TOF2GC::SCBTPN;ibt++){//loop over bar types
    id=rbls[ibt];//ref.bar id
    if(id==0)continue;//skip dummy bar types
    btypes+=1;
    aabs[ibt]=0.;
    mip2q[ibt]=100.;//default value
    nev=nrefb[ibt];
    cout<<endl;
    cout<<"Abs.calib.fit: ibt/events="<< (ibt+1)<<" "<<nev<<endl;
    cout<<endl;
    if(nev>=TFCAFFKEY.minev){
      for(k=0;k<nev;k++)pntr[k]=&arefb[ibt][k];//pointers to event-signals of chan=bchan
      AMSsortNAG(pntr,nev);//sort in increasing order
      nmax=int(floor(nev*TFCAFFKEY.trcut));// to keep (100*trcut)% of lowest amplitudes
      nmin=int(floor(nev*0.015));// to remove 1.5 % of lowest amplitudes
      if(nmin==0)nmin=1;
      strcpy(htit1,"Q-tot(X=0) for ref. bar type");
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
      MNEXCM(TofTmAmCalib::melfun,"CALL FCN",arglp,1,ierp,0);
      if(ierp!=0){
        cout<<"TOF:Abs-calib: final CALL_FCN problem !"<<'\n';
        continue;
      }
      aabs[ibt]=elfitp[1];
      mip2q[ibt]=aabs[ibt]/elref;//(pC/mev)
      btypeok+=1;
      HDELET(1599);
//
    }
  }
//
  if(btypes>0)TOF2JobStat::cqual(2,1)=geant(btypeok)/btypes;//for cal-quality
//-------------------------------------------------------
//
// ---> calculate/print Anode/Dynode chan.gain ratios:
//
  number a2d[TOF2GC::SCCHMX],a2ds[TOF2GC::SCCHMX],avr,avr2,a2dsig,rsig;
//
// ---> calculate/print Ah2Dh ratios:
//
//
  integer gchan(0);
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      for(i=0;i<2;i++){
        a2d[chan]=6;//default value
        a2ds[chan]=0;
        if(neva2d[chan]>=10){
	  avr=a2dr[chan]/neva2d[chan];//aver x
	  avr2=a2dr2[chan]/neva2d[chan];//aver x**2
	  a2dsig=avr2-avr*avr;
	  if(a2dsig>0){
	    a2dsig=sqrt(a2dsig);//rms
	    rsig=a2dsig/avr;//rel. rms
	    if(TFCAFFKEY.hprintf>0){
	      HF1(1256,geant(avr),1.);
	      HF1(1257,geant(rsig),1.);
	    }
	    if(rsig<0.25){//good measurement, tempor
	      a2d[chan]=avr;
	      a2ds[chan]=a2dsig;
	      gchan+=1;
	    }
	    else{
	      a2ds[chan]=99;
	    }
	  }
	}
	chan+=1;//seq.numbering of bar sides
      }
    }
  }
//
  if(chan>0)TOF2JobStat::cqual(2,2)=geant(gchan)/chan;//for cal-quality
//
  printf("\n");
  printf(" =================> An/Dyn(pm-sum)  distributions :\n");
  printf("\n");
//
//
  printf("\n");
  printf(" ------->  An/Dyn  nevents/ratios/sigmas :\n");
  printf("\n");
  ich=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6d",neva2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6d",neva2d[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",a2d[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      printf("%6.2f",a2ds[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      printf("%6.2f",a2ds[chan]);
    }
    printf("\n");
    printf("\n");
    ich+=2*TOF2DBc::getbppl(il);//skip to next layer
  }
    
//-------------------------------------------------------
//
// ---> calculate/print Dynode-pmts relat(to side aver) gains:
//
  number dpmg[TOF2GC::SCCHMX][TOF2GC::PMTSMX],dpmgs[TOF2GC::SCCHMX][TOF2GC::PMTSMX];
  number dpmsig,dnor;
  integer npmts;
//
// ---> calculate/print Dyn rel.gains:
//
//chan-sequence: L1:B1(s1,s2),...Bn(s1,s2),  L2:.......  L4:B1(s1,s2),...Bn(s1,s2)
  chan=0;
  for(il=0;il<TOF2DBc::getnplns();il++){//<---plane-loop
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){//<---bar-loop
      npmts=TOF2Brcal::scbrcal[il][ib].getnpm();
      for(i=0;i<2;i++){//<---side-loop
//
	for(ip=0;ip<TOF2GC::PMTSMX;ip++){
	  dpmg[chan][ip]=1;
	  dpmgs[chan][ip]=0;
	}
        if(nevdgn[chan]>5){//<--check stat
          for(ip=0;ip<npmts;ip++){//<--pmt-loop
	    avr=d2sdr[chan][ip]/nevdgn[chan];//aver x
	    avr2=d2sdr2[chan][ip]/nevdgn[chan];//aver x**2
	    dpmsig=avr2-avr*avr;
	    if(dpmsig>0 && avr>0){
	      dpmsig=sqrt(dpmsig);//rms
	      rsig=dpmsig/avr;//rel. rms
	      if(TFCAFFKEY.hprintf>0){
	        HF1(1258,geant(avr),1.);
	        HF1(1259,geant(rsig),1.);
	      }
	      if(rsig<2){//good measurement
	        dpmg[chan][ip]=avr;
	        dpmgs[chan][ip]=dpmsig;
	      }
	      else{
	        dpmg[chan][ip]=1;
	        dpmgs[chan][ip]=99;
	      }
	    }
	    else{
	      dpmg[chan][ip]=1;
	      dpmgs[chan][ip]=99;
	    }
	  }//-->endof pm-loop
	  dnor=0;
	  for(ip=0;ip<npmts;ip++)dnor+=dpmg[chan][ip];
	  if(dnor>0){//normalize to have Sum(dpmg[ip])=npmts
	    dpmg[chan][ip]*=(geant(npmts)/dnor);
	  }
	}//-->endof stat-check
	chan+=1;//seq.numbering of bar sides
      }//-->endof side-loop
//
    }//-->endof bar-loop
  }//-->endof layer-loop
//
  printf("\n");
  printf(" =================> Dynode(pm) rel.gains distributions :\n");
  printf("\n");
//
//-----
  printf("\n");
  printf(" ------->  DynodeH(pmt) rel.gains :\n");
  printf("\n");
  ich=0;
  for(il=0;il<TOF2DBc::getnplns();il++){
    printf("Layer= %2d\n",(il+1));
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){//s1
      chan=ich+2*ib;
      printf("%6d",nevdgn[chan]);
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){//s2
      chan=ich+2*ib+1;
      printf("%6d",nevdgn[chan]);
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      for(ip=0;ip<TOF2GC::PMTSMX;ip++)printf("%5.2f",dpmg[chan][ip]);
      printf(" ");
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      for(ip=0;ip<TOF2GC::PMTSMX;ip++)printf("%5.2f",dpmg[chan][ip]);
      printf(" ");
    }
    printf("\n");
    printf("\n");
    
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib;
      for(ip=0;ip<TOF2GC::PMTSMX;ip++)printf("%5.2f",dpmgs[chan][ip]);
      printf(" ");
    }
    printf("\n");
    for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
      chan=ich+2*ib+1;
      for(ip=0;ip<TOF2GC::PMTSMX;ip++)printf("%5.2f",dpmgs[chan][ip]);
      printf(" ");
    }
    printf("\n");
    printf("\n");
    ich+=2*TOF2DBc::getbppl(il);
  }
  printf("\n");
  printf("\n");
//
//------------------------------------------------------
//
  printf("\n");
  printf(" ===============>  Anode relative gains :\n");
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
// ------------------------------------------------------------- 
//
// ---> write Anode-gains,A/D slops/offs,Anode-mip2q's to file:
//
  ofstream tcfile(fname,ios::out|ios::trunc);
  if(!tcfile){
    cerr<<"TofTmAmCalib::Ampl:error opening file for output "<<fname<<'\n';
    exit(8);
  }
  cout<<"Open file for AMPL-calibration output, fname:"<<fname<<'\n';
  cout<<"Indiv.channel gain's,a2dr's,h2l's,mip2q's,profile_par's will be written !"<<'\n';
  cout<<" First run used for calibration is "<<StartRun<<endl;
  cout<<" Date of the first event : "<<frdate<<endl;
//
  tcfile.setf(ios::fixed);
  tcfile.width(6);
//                       <------- write Anode relat. gains:
//
  tcfile.precision(3);// precision for gains
  ic=0;
  for(il=0;il<TOF2DBc::getnplns();il++){ 
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
//                      <------ write Anode/Sum(Dynode(ipm))-ratios 
//
    tcfile.precision(2);// precision for A/D ratio
    ic=0;
    for(il=0;il<TOF2DBc::getnplns();il++){ //
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
//                      <------ write Dynode-pmts rel.gains :
//
    tcfile.precision(2);// precision for  D-gains
    ic=0;
    for(il=0;il<TOF2DBc::getnplns();il++){//<---layer-loop
      for(is=0;is<2;is++){//<--- side-loop
        for(ib=0;ib<TOF2DBc::getbppl(il);ib++){//<---bar-loop
	  chan=ic+2*ib+is;
          for(ip=0;ip<TOF2GC::PMTSMX;ip++)tcfile << dpmg[chan][ip]<<" ";
	  tcfile << " ";
        }//--->endof bar-loop
        tcfile << endl;
      }//--->endof side-loop
      ic+=2*TOF2DBc::getbppl(il);
    }//--->endof layer-loop
    tcfile << endl;
//
//                       <------ write mip2q's :
//
  tcfile.precision(2);// precision for mip2q's
  for(btyp=0;btyp<TOF2GC::SCBTPN;btyp++){ // <--- mip2q's
    tcfile << mip2q[btyp]<<" ";
  }  
  tcfile << endl;
  tcfile << endl;
//
//                       <-------- write A-profile fit-parameters:
//
  for(btyp=0;btyp<TOF2GC::SCBTPN;btyp++){
    for(i=0;i<2*TOF2GC::SCPROFP;i++)tcfile << aprofp[btyp][i]<<" ";
    tcfile << endl;
  }  
  tcfile << endl;
//
  tcfile << 12345;// write end-file label
  tcfile << endl;
//
//
  tcfile << endl<<"======================================================"<<endl;
  tcfile << endl<<" First run used for calibration is "<<StartRun<<endl;
  tcfile << endl<<" Date of the first event : "<<frdate<<endl;
  tcfile.close();
  cout<<"TOF2AMPLcalib:output file closed !"<<endl;
//
}
//---------------------
// This is standard Minuit FCN for uniformity fit:
void TofTmAmCalib::mfunam(int &np, number grad[], number &f, number x[]
                                                        , int &flg, int &dum){
  int i,j;
  number ff;
  f=0.;
  if(iside==0){
    for(i=0;i<nbinr;i++){
      if(fitflg==0)ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.+mcoord[i])/x[3])))
		                                            )/emchar[i];//normal counters
      else         ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.+mcoord[i])/x[3]))
		     +x[4]*exp(-(clent/2.-mcoord[i])/x[5]))
		                                            )/emchar[i];//some trapez.counters
      f+=(ff*ff);
    }
  }
  else{
    for(i=0;i<nbinr;i++){
      if(fitflg==0)ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.-mcoord[i])/x[3])))
		                                            )/emchar[i];//normal counters
      else         ff=(mcharge[i]-(x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                     +x[2]*exp(-(clent/2.-mcoord[i])/x[3]))
		     +x[4]*exp(-(clent/2.+mcoord[i])/x[5]))
		                                            )/emchar[i];//some trapez.counters
      f+=(ff*ff);
    }
  }
  if(flg==3){
    f=sqrt(f/number(nbinr));
    cout<<"    FitResult::Btype="<<cbtyp<<" Fitflg="<<fitflg<<" side="<<iside+1<<"  funct:nbins="<<f<<endl;
    for(i=0;i<TOF2GC::SCPROFP;i++){
      aprofp[cbtyp-1][iside*TOF2GC::SCPROFP+i]=x[i];
      cout<<"    parnumb/par="<<i<<" "<<x[i]<<endl;
    }
    cout<<"      Function values vs coo:"<<endl;
    for(i=0;i<nbinr;i++){
      if(iside==0){
       if(fitflg==0)ff=x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                      +x[2]*exp(-(clent/2.+mcoord[i])/x[3]));//normal counters
       else         ff=x[0]*((1-x[2])*exp(-(clent/2.+mcoord[i])/x[1])
                      +x[2]*exp(-(clent/2.+mcoord[i])/x[3]))
		      +x[4]*exp(-(clent/2.-mcoord[i])/x[5]);       
      }
      else{
       if(fitflg==0)ff=x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                      +x[2]*exp(-(clent/2.-mcoord[i])/x[3]));//normal counters
       else         ff=x[0]*((1-x[2])*exp(-(clent/2.-mcoord[i])/x[1])
                      +x[2]*exp(-(clent/2.-mcoord[i])/x[3]))
		      +x[4]*exp(-(clent/2.+mcoord[i])/x[5]);
      }
      cout<<"      ff/coo="<<ff<<" "<<mcoord[i]<<endl;
    }
  }
}
//---------------------
// This is standard Minuit FCN for Landau fit:
void TofTmAmCalib::melfun(int &np, number grad[], number &f, number x[]
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
//=============================================================================
number TOFPedCalib::adc[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store Anode/Dynode adc sum
number TOFPedCalib::adc2[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//store adc-squares sum
number TOFPedCalib::adcm[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1][TFPCSTMX];//max. adc-values stack
number TOFPedCalib::port2r[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];//portion of hi-ampl to remove
integer TOFPedCalib::nevt[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];// events in sum
geant TOFPedCalib::peds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
geant TOFPedCalib::dpeds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
geant TOFPedCalib::thrs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
geant TOFPedCalib::sigs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
uinteger TOFPedCalib::stas[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
//geant TOFPedCalib::rpeds[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
//geant TOFPedCalib::rsigs[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
//uinteger TOFPedCalib::rstas[TOF2GC::SCCHMX][TOF2GC::PMTSMX+1];
integer TOFPedCalib::hiamap[TOF2GC::SCLRS][TOF2GC::SCMXBR];//high signal Paddles map (1 event) 
integer TOFPedCalib::nstacksz;//really needed stack size (ev2rem*TFPCEVMX)
time_t TOFPedCalib::BeginTime;
uinteger TOFPedCalib::BeginRun;
TOFPedCalib::TOFPedCal_ntpl TOFPedCalib::TOFPedCalNT;
//-------------------------------------------------------
void TOFPedCalib::initb(){ //called in retof2initjob() if TOF+AC is requested for OnBoard-calib data proc 
// histograms booking / reset vars
  integer i,j,k,il,ib,id,ii,jj,chan;
  char htit1[60];
  char inum[11];
  char in[2]="0";
  char hmod[2]=" ";
//
  strcpy(inum,"0123456789");
//
//
  if(TFREFFKEY.reprtf[1]>0)cout<<endl;
//
//
//  ---> book hist.  :
//
  if((CALIB.SubDetInCalib/1000)%10>1){//histograms are requested
    for(i=0;i<2;i++){
      strcpy(htit1,"Anode peds vs paddle for Side-");
      in[0]=inum[i+1];
      strcat(htit1,in);
      id=1790+i;
      HBOOK1(id,htit1,50,1.,51.,0.);
      HMINIM(id,50.);
      HMAXIM(id,400.);
    }
    for(i=0;i<2;i++){
      strcpy(htit1,"Anode ped-rms vs paddle for Side-");
      in[0]=inum[i+1];
      strcat(htit1,in);
      id=1792+i;
      HBOOK1(id,htit1,50,1.,51.,0.);
      HMINIM(id,0.);
      HMAXIM(id,5.);
    }
    for(i=0;i<2;i++){
      for(j=0;j<TOF2GC::PMTSMX;j++){
        strcpy(htit1,"Dynode peds vs paddle for Side/Pmt-");
        in[0]=inum[i+1];
        strcat(htit1,in);
        in[0]=inum[j+1];
        strcat(htit1,in);
        id=1794+3*i+j;
        HBOOK1(id,htit1,50,1.,51.,0.);
        HMINIM(id,100.);
        HMAXIM(id,800.);
      }
    }
    for(i=0;i<2;i++){
      for(j=0;j<TOF2GC::PMTSMX;j++){
        strcpy(htit1,"Dynode ped-rms vs paddle for Side/Pmt-");
        in[0]=inum[i+1];
        strcat(htit1,in);
        in[0]=inum[j+1];
        strcat(htit1,in);
        id=1800+3*i+j;
        HBOOK1(id,htit1,50,1.,51.,0.);
        HMINIM(id,0.);
        HMAXIM(id,5.);
      }
    }
    HBOOK1(1808,"All Anode-channels PedRms",50,0.,25.,0.);
    HBOOK1(1809,"All Anode-channels PedDiff",50,-10.,10.,0.);
    HBOOK1(1810,"All Dynode-channels PedRms",50,0.,25.,0.);
    HBOOK1(1811,"All Dynode-channels PedDiff",50,-10.,10.,0.);
  }
// ---> clear arrays:
//
   DAQS2Block::PrevRunNum()=0;
   DAQS2Block::NReqEdrs()=0;
   DAQS2Block::GoodPedBlks()=0;
   DAQS2Block::FoundPedBlks()=0;
   DAQS2Block::FirstPedBlk()=true;
   DAQS2Block::CalFirstSeq()=true;
//
   for(int cr=0;cr<TOF2GC::SCCRAT;cr++){
     DAQS2Block::PedBlkCrat(cr)=-1;
   }
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    for(j=0;j<TOF2GC::PMTSMX+1;j++){
      peds[i][j]=0;
      dpeds[i][j]=-1;//means missing
      thrs[i][j]=-1;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
    }
  }
//
  if(TFREFFKEY.reprtf[1]>0)cout<<"<---- TOF OnBoardPedCalib: init done..."<<endl<<endl;;
}
//-----
//
void TOFPedCalib::resetb(){ // run-by-run reset for OnBoardPedTable processing 
//called in buildonbP
  integer i,j,k,il,ib,id,ii,jj,chan;
  char hmod[2]=" ";
//
  cout<<endl;
//
  if((CALIB.SubDetInCalib/1000)%10>1){//reset hist
    for(i=0;i<22;i++)HRESET(1790+i,hmod);
  }
//
// ---> clear arrays:
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    for(j=0;j<TOF2GC::PMTSMX+1;j++){
      peds[i][j]=0;
      dpeds[i][j]=-1;//means missing
      thrs[i][j]=-1;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
    }
  }
//
  DAQS2Block::GoodPedBlks()=0;//start new calib.sequence
  DAQS2Block::FoundPedBlks()=0;
  DAQS2Block::NReqEdrs()=0;
//
  for(i=0;i<TOF2GC::SCCRAT;i++)DAQS2Block::PedBlkCrat(i)=-1;
//
  cout<<"====> TOFPedCalib::OnBoardPedTable:  Reset done..."<<endl<<endl;;
}
//----
void TOFPedCalib::filltb(int il, int ib, int is, int pm, geant ped,geant dped,geant thr,geant sig){
//For storing of OnBoardPedTable elements, pm=0/1-3 => anod2/dynode1-3
  int ch;
//
  ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
  peds[ch][pm]=ped;
  dpeds[ch][pm]=dped;
  thrs[ch][pm]=thr;
  sigs[ch][pm]=sig;
}
//----
//
void TOFPedCalib::outptb(int flg){//called in buildonbP
// flg=1/2/3=>/write2DB/NoAction(hist only)/(write2file+hist)
   int i,il,ib,is,pm,ch;
   int totchs(0),goodtbch(0),goodchs(0);
   geant pedo,sigo;
   geant pedmin,pedmax,sigmin,sigmax;
   int stao;
   geant pdiff;
   uinteger runn=BRun();//run# 
   time_t begin=time_t(runn);//begin time = run-time(=runn) 
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   geant goodchp(0);
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer spatt=TFCAFFKEY.onbpedspat;//bit-patt for onb.ped-table sections (bit set if section is present)
   bool dpedin=((spatt&16)==1);//dyn.peds-section present(90 words)
   bool thrsin=((spatt&2)==1);//thresholds ..............(90...)
   geant rthrs,rdped;
//
   if(TFREFFKEY.reprtf[1]>0)cout<<endl<<"=====> TofOnbPedCalib::EndOfRun: OnBoardTable-Report:"<<endl<<endl;
//---- fill ntuple:
//   TOFPedCalNT.Run=BRun();
//   for(il=0;il<TOF2DBc::getnplns();il++){
//     TOFPedCalNT.Layer=il+1;
//     for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
//       TOFPedCalNT.Paddle=ib+1;
//       for(is=0;is<2;is++){
//         ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
//
//         for(pm=0;pm<TOF2DBc::npmtps(il,ib)+1;pm++){//<--- pm-loop(0/1-3 => an/dyn1-3)
//	   if(pm==0){
//	     TOFPedCalNT.PedA[is]=peds[ch][pm];
//	     TOFPedCalNT.SigA[is]=sigs[ch][pm];
//	     TOFPedCalNT.StaA[is]=stas[ch][pm];
//	   }
//	   else{
//	     TOFPedCalNT.PedD[is][pm-1]=peds[ch][pm];
//	     TOFPedCalNT.SigD[is][pm-1]=sigs[ch][pm];
//	     TOFPedCalNT.StaD[is][pm-1]=stas[ch][pm];
//	   }
//	 }
//       }//side
//       HFNT(IOPA.ntuple);
//     }//paddle
//   }// layer
//   cout<<"      <-- Ntuple filled..."<<endl<<endl;
//----
   pedmin=TFCAFFKEY.pedlim[0];
   pedmax=TFCAFFKEY.pedlim[1];
   sigmin=TFCAFFKEY.siglim[0];
   sigmax=TFCAFFKEY.siglim[1];
//
   for(il=0;il<TOF2DBc::getnplns();il++){
     for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
       for(is=0;is<2;is++){
         ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
//
         for(pm=0;pm<TOF2DBc::npmtps(il,ib)+1;pm++){//<--- pm-loop(0/1-3 => an/dyn1-3)
	   totchs+=1;
	   if(pm==0){
	     pedo=TOFBPeds::scbrped[il][ib].apeda(is);//anodes,extract prev.calib ped/sig/sta for comparison
	     sigo=TOFBPeds::scbrped[il][ib].asiga(is);
	     stao=TOFBPeds::scbrped[il][ib].astaa(is);
	   }
	   else{
	     pedo=TOFBPeds::scbrped[il][ib].apedd(is,pm-1);//dynodes,extract prev.calib ped/sig/sta for comparison
	     sigo=TOFBPeds::scbrped[il][ib].asigd(is,pm-1);
	     stao=TOFBPeds::scbrped[il][ib].astad(is,pm-1);
	   }
	   pdiff=peds[ch][pm]-pedo;
	   if(DAQS2Block::CalFirstSeq() || stao==1)pdiff=0;//to exclude pdiff-check for 1st run or bad prev.channel
//
           if(thrsin)rthrs=thrs[ch][pm];
	   else rthrs=5;
	   if(dpedin)rdped=dpeds[ch][pm];
	   else rdped=50;
//
	   if(peds[ch][pm]>0 && rthrs>1 && rdped>0){// channel OK in table ?
	     goodtbch+=1;
	     if(sigs[ch][pm]>sigmin && sigs[ch][pm]<=sigmax &&
		        peds[ch][pm]>pedmin && peds[ch][pm]<=pedmax && fabs(pdiff)<20){//MyCriteria:chan.OK
	       stas[ch][pm]=0;//ch ok
	       goodchs+=1;
	       if(pm==0){//anodes
	         TOFBPeds::scbrped[il][ib].apeda(is)=peds[ch][pm];//save current values in PedCal-obj
	         TOFBPeds::scbrped[il][ib].asiga(is)=sigs[ch][pm];
	         TOFBPeds::scbrped[il][ib].astaa(is)=stas[ch][pm];
		 if(flg>1){
		   HF1(1790+is,geant(il*10+ib+1),peds[ch][pm]);
		   HF1(1792+is,geant(il*10+ib+1),sigs[ch][pm]);
		   HF1(1808,sigs[ch][pm],1.);
		   HF1(1809,pdiff,1.);
		 }
	       }
	       else{//dynodes
	         TOFBPeds::scbrped[il][ib].apedd(is,pm-1)=peds[ch][pm];//save current values in PedCal-obj
	         TOFBPeds::scbrped[il][ib].asigd(is,pm-1)=sigs[ch][pm];
	         TOFBPeds::scbrped[il][ib].astad(is,pm-1)=stas[ch][pm];
		 if(flg>1){
		   HF1(1794+3*is+pm-1,geant(il*10+ib+1),peds[ch][pm]);
		   HF1(1800+3*is+pm-1,geant(il*10+ib+1),sigs[ch][pm]);
		   HF1(1810,sigs[ch][pm],1.);
		   HF1(1811,pdiff,1.);
		 }
	       }
	     }
	     else{//MyCriteria: bad chan
	       if(TFREFFKEY.reprtf[1]>0){
	         cout<<"       MyCriteriaBadCh: Lay/Pad/Side/Pmi="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
	         cout<<"                        ped/sig="<<peds[ch][pm]<<" "<<sigs[ch][pm]<<endl;    
	         cout<<"                        PedDiff="<<pdiff<<endl;
	       }    
	     }
	   }//--->endof "channel OK in table ?" check
	   else{
	     if(TFREFFKEY.reprtf[1]>0){
	       cout<<"       BadTableChan:Lay/Pad/Side/Pmi="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
	       cout<<"       ped/sig/sta="<<peds[ch][pm]<<" "<<sigs[ch][pm]<<" "<<stas[ch][pm]<<endl;
	     }    
	   }
	}//--->endof pm-loop 
      }//--->endof side-loop
    }//--->endof paddle-loop
  }//--->endof layer-loop
//
  goodchp=geant(goodchs)/totchs;
// 
  if(TFREFFKEY.reprtf[1]>0)
    cout<<"       GoogChs(Table/My)="<<goodtbch<<" "<<goodchs<<" from total="<<totchs<<" GoodChsPort="<<goodchp<<endl;
//      
// ---> prepare update of DB :
//
   if(goodchp>=0.5 && flg==1){//Update DB "on flight"
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Tofpeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();//redefine according to VC.
     ptdv->SetTime(insert,min(AMSEvent::gethead()->getrun()-1,uinteger(AMSEvent::gethead()->gettime())),AMSEvent::gethead()->getrun()-1+86400*30);
     cout <<"      <--- TofOnBoardPeds DB-info has been updated for "<<*ptdv<<endl;
     ptdv->gettime(insert,begin,end);
     cout<<"           Time ins/beg/end: "<<endl;
     cout<<"           "<<ctime(&insert);
     cout<<"           "<<ctime(&begin);
     cout<<"           "<<ctime(&end);
//
//     if(AMSFFKEY.Update==2 ){
//       AMSTimeID * offspring = 
//         (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());//get 1st timeid instance
//       while(offspring){
//         if(offspring->UpdateMe())cout << "         Start update TOF-peds DB "<<*offspring; 
//         if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatabase))
//         cerr <<"<---- Problem To Update TOF-peds in DB"<<*offspring;
//         offspring=(AMSTimeID*)offspring->next();//get one-by-one
//       }
//     }
   }
   else{
     if(goodchp<0.5)cout<<"      <--- GoodCh% is too low("<<goodchp<<") - No DB-writing !"<<endl;
   }
//
// ---> write OnBoardPedTable to ped-file:
//
   if(flg==3 && AMSFFKEY.Update==0){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     strcpy(name,"tofp_tb_rl.");//from OnBoardTable
     sprintf(buf,"%d",runn);
     strcat(name,buf);
     if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
     if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
     strcat(fname,name);
     cout<<"       Open file : "<<fname<<'\n';
     cout<<"       Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<"<---- Problems to write new ONBT-Peds file !!? "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write Anodes peds/sigmas/stat:
//
     for(il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
       for(ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
         for(is=0;is<2;is++){// sequence: side1,side2
           ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
           icfile.width(2);
           icfile.precision(1);
           icfile << stas[ch][0];//stat
           icfile.width(6);
           icfile.precision(1);
           icfile << peds[ch][0];//ped
           icfile.width(5);
           icfile.precision(1);
           icfile << sigs[ch][0];//sig
	   icfile << "  ";
         }
         icfile << endl;
       } // --- end of bar loop --->
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//---> write dynodes:
     for(il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
       for(ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
         for(is=0;is<2;is++){// sequence: side1,side2
           ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
	   for(pm=0;pm<TOF2GC::PMTSMX;pm++){
             icfile.width(2);
             icfile.precision(1);
             icfile << stas[ch][pm+1];//stat
             icfile.width(6);
             icfile.precision(1);
             icfile << peds[ch][pm+1];//ped
             icfile.width(5);
             icfile.precision(1);
             icfile << sigs[ch][pm+1];//sig
	     icfile << "  ";
           }
	   icfile << "  ";
	 }
         icfile << endl;
       } // --- end of bar loop --->
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   if(flg>1 && AMSFFKEY.Update==0){
     for(i=0;i<22;i++)HPRINT(1790+i);
   }
//
//
   cout<<endl;
   cout<<"<========= TofOnbPedCalib: run "<<runn<<" is processed, goodpeds%="<<goodchp<<endl;
   cout<<endl;
//
//
}
//--------------------------
void TOFPedCalib::init(){ // ----> initialization for TofPed-calibration(Classic,DS)
//called in catofinitjob() 
  integer i,j,k,il,ib,id,ii,jj,chan;
  char htit1[60];
  char inum[11];
  char in[2]="0";
  geant por2rem;
//
  strcpy(inum,"0123456789");
//
   cout<<endl;
   if(TFREFFKEY.relogic[0]==7){//open Ntuple file (for OnBoardTable only for the moment)
//     char hfile[161];
//     UHTOC(IOPA.hfile,40,hfile,160);  
//     char filename[256];
//     strcpy(filename,hfile);
//     integer iostat;
//     integer rsize=1024;
//     char event[80];  
//     HROPEN(IOPA.hlun+1,"tofpedsig",filename,"NP",rsize,iostat);
//     if(iostat){
//       cerr << "<==== TOFPedCalib::init: Error opening tofpedsig ntuple file "<<filename<<endl;
//       exit(1);
//     }
//     else cout <<"====> TOFPedCalib::init: Ntuple file "<<filename<<" opened..."<<endl;
//     HBNT(IOPA.ntuple,"TofPedSigmas"," ");
//     HBNAME(IOPA.ntuple,"TOFPedSig",(int*)(&TOFPedCalNT),"Run:I,Layer:I,Paddle:I,PedA(2):R,SigA(2):R,"
//                                                                            "PedD(3,2):R,SigD(3,2):R,"
//									       "StaA(2):I,StaD(3.2):I");
//     return;
   }
//
  if(TFREFFKEY.relogic[0]==5)por2rem=TFCAFFKEY.pedcpr[0];//def ClassPed(random)
  else if(TFREFFKEY.relogic[0]==6)por2rem=TFCAFFKEY.pedcpr[1];//def DownScaled(in trigger)
//  nstacksz=integer(floor(por2rem*TFPCEVMX+0.5));
  nstacksz=TFPCSTMX;
  cout<<"====> TOFPedCalib::init: real stack-size="<<nstacksz<<endl;
//
//  ---> book hist.  :
//
  for(i=0;i<2;i++){
    strcpy(htit1,"Anode peds vs paddle for Side-");
    in[0]=inum[i+1];
    strcat(htit1,in);
    id=1790+i;
    HBOOK1(id,htit1,50,1.,51.,0.);
    HMINIM(id,10.);
    HMAXIM(id,510.);
  }
  for(i=0;i<2;i++){
    strcpy(htit1,"Anode ped-rms vs paddle for Side-");
    in[0]=inum[i+1];
    strcat(htit1,in);
    id=1792+i;
    HBOOK1(id,htit1,50,1.,51.,0.);
    HMINIM(id,0.);
    HMAXIM(id,10.);
  }
  for(i=0;i<2;i++){
    for(j=0;j<TOF2GC::PMTSMX;j++){
      strcpy(htit1,"Dynode peds vs paddle for Side/Pmt-");
      in[0]=inum[i+1];
      strcat(htit1,in);
      in[0]=inum[j+1];
      strcat(htit1,in);
      id=1794+3*i+j;
      HBOOK1(id,htit1,50,1.,51.,0.);
      HMINIM(id,10.);
      HMAXIM(id,510.);
    }
  }
  for(i=0;i<2;i++){
    for(j=0;j<TOF2GC::PMTSMX;j++){
      strcpy(htit1,"Dynode ped-rms vs paddle for Side/Pmt-");
      in[0]=inum[i+1];
      strcat(htit1,in);
      in[0]=inum[j+1];
      strcat(htit1,in);
      id=1800+3*i+j;
      HBOOK1(id,htit1,50,1.,51.,0.);
      HMINIM(id,0.);
      HMAXIM(id,10.);
    }
  }
  HBOOK1(1808,"All Anode-channels PedRms",50,0.,25.,0.);
  HBOOK1(1809,"All Anode-channels PedDiff",50,-10.,10.,0.);
  HBOOK1(1810,"All Dynode-channels PedRms",50,0.,25.,0.);
  HBOOK1(1811,"All Dynode-channels PedDiff",50,-10.,10.,0.);
//
  HBOOK1(1820,"Anode peds for LBBS=1042(raw)",100,100.,300.,0.);
  HBOOK1(1821,"Anode peds for LBBS=1042(inlim)",100,100.,300.,0.);
  HBOOK1(1830,"Dynode-1 peds for LBBS=2011(raw)",100,100.,300.,0.);
  HBOOK1(1831,"Dynode-1 peds for LBBS=2011(inlim)",100,100.,300.,0.);
// ---> clear arrays:
//
  for(i=0;i<TOF2GC::SCCHMX;i++){
    for(j=0;j<TOF2GC::PMTSMX+1;j++){
      nevt[i][j]=0;
      adc[i][j]=0;
      adc2[i][j]=0;
      port2r[i][j]=0;
      peds[i][j]=0;
      sigs[i][j]=0;
      stas[i][j]=1;//bad
      port2r[i][j]=0;
      for(k=0;k<TFPCSTMX;k++)adcm[i][j][k]=0;
    }
  }
  cout<<"<==== TOFPedCalib::init done..."<<endl<<endl;;
}
//-----
void TOFPedCalib::fill(int il, int ib, int is, int pm, geant val){//pm=0/1-3 => anod2/dynode1-3
   int i,ist,nev,ch,evs2rem,ibl,ibr;
   geant lohil[2]={0,9999};//means no limits on val, if partial ped is bad
   geant ped,sig,sig2,gainf,spikethr,gnf[2];
   geant pedmin,pedmax,sigmin,sigmax;
   bool accept(true);
   geant por2rem,p2r;
   geant apor2rm[10]={0.,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,};
   number ad,ad2,dp,ds;
   geant pedi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   geant sigi[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
//
   if(TFREFFKEY.relogic[0]==5)por2rem=TFCAFFKEY.pedcpr[0];//ClassPed(random)
   else if(TFREFFKEY.relogic[0]==6)por2rem=TFCAFFKEY.pedcpr[1];//DownScaled(in trigger)
//
   TOF2Brcal::scbrcal[il][ib].geta2dr(gnf);
   if(pm==0)gainf=1.;//an
   else gainf=gnf[is]*TOF2DBc::npmtps(il,ib);//dyn (aver. per 1pm)
   sigmin=TFCAFFKEY.siglim[0];
   sigmax=TFCAFFKEY.siglim[1];
//
   ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
   nev=nevt[ch][pm];
// peds[ch][pm];//SmalSample(SS) ped (set to "0" at init)
   if(peds[ch][pm]==0 && nev==TFPCEVMN){//calc. SubSet-ped/sig when TFPCEVMN events is collected
//
//     cout<<"<----- start SS-peds calculation for L/B/S/PM="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
//     for(i=0;i<nstacksz;i++){
//       cout<<adcm[ch][pm][i]<<" ";
//       if((i+1)%10==0)cout<<endl;
//     }
//     cout<<endl;   
     int llindx(-1);   
     for(int ip2r=0;ip2r<10;ip2r++){//<--- portion to remove loop
       p2r=apor2rm[ip2r];
       evs2rem=int(floor(p2r*nev+0.5));
       if(evs2rem>nstacksz)evs2rem=nstacksz;
       ad=adc[ch][pm];
       ad2=adc2[ch][pm];
       for(i=0;i<evs2rem;i++){//remove "evs2rem" highest amplitudes
         ad=ad-adcm[ch][pm][i];
         ad2=ad2-adcm[ch][pm][i]*adcm[ch][pm][i];
       }
       ped=ad/number(nev-evs2rem);//truncated average
       sig2=ad2/number(nev-evs2rem);
       sig2=sig2-ped*ped;// truncated rms**2
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
       if((sig < sigmax && sig>sigmin)
                                      && (dp < 1.0)
                                                   && (ds < 0.5)
		                                                && ip2r > 0){
         port2r[ch][pm]=p2r;
         llindx=ip2r;
	 break;
       }
//       cout<<"   pedi/sigi="<<pedi[ip2r]<<" "<<sigi[ip2r]<<endl;
     }//--->endof portion to remove loop
//
     if(llindx<0){//fail to find SubSet-ped/sig - suspicious channel
       sig=0;
       ped=0;
       port2r[ch][pm]=-1;
     }
//
     sigs[ch][pm]=sig;
     peds[ch][pm]=ped;//is used now as flag that SS-PedS ok
     adc[ch][pm]=0;//reset to start new life(with real selection limits)
     adc2[ch][pm]=0;
     nevt[ch][pm]=0;
     for(i=0;i<TFPCSTMX;i++)adcm[ch][pm][i]=0;
   }//--->endof SS(1st 100evs) check
   ped=peds[ch][pm];//now !=0 or =0 
   sig=sigs[ch][pm];
//
   if(ped>0){//set val-limits if partial ped OK
     lohil[0]=ped-3.5*sig;
     lohil[1]=ped+5*sig;
     spikethr=max(5*sig,TFPCSPIK/gainf);
     if(val>(ped+spikethr)){//spike(>~1mips in higain chan)
       hiamap[il][ib]=1;//put it into map
       accept=false;//mark as bad for filling
     }
     else{//candidate for "fill" - check neigbours
       if(ib>0)ibl=ib-1;
       else ibl=0;
       if(ib<(TOF2DBc::getbppl(il)-1))ibr=ib+1;
       else ibr=TOF2DBc::getbppl(il)-1;
       accept=(hiamap[il][ibl]==0 && hiamap[il][ibr]==0);//not accept if there is any neighbour(horizontal)
     }
   }
//
//   accept=true;//tempor to switch-off spike algorithm
//
   if(val>lohil[0] && val<lohil[1] && accept){//check "in_limits/not_spike"
     if(nev<TFPCEVMX){//limit statistics(to keep max-stack size small)
       adc[ch][pm]+=val;
       adc2[ch][pm]+=(val*val);
       nevt[ch][pm]+=1;
       for(ist=0;ist<nstacksz;ist++){//try to position val in stack of nstacksz highest max-values
          if(val>adcm[ch][pm][ist]){
	    for(i=nstacksz-1;i>ist;i--)adcm[ch][pm][i]=adcm[ch][pm][i-1];//move stack -->
	    adcm[ch][pm][ist]=val;//store max.val
	    break;
	  }
       }
     }
   }//-->endof "in limits" check
//
   if(il==0 && ib==3 && is==1 && nev<TFPCEVMX){
     if(pm==0){
       HF1(1820,val,1.);
       if(ped>0 && val>lohil[0] && val<lohil[1] && accept)HF1(1821,val,1.);
     }
     if(pm==1){
       HF1(1830,val,1.);
       if(ped>0 && val>lohil[0] && val<lohil[1] && accept)HF1(1831,val,1.);
     }
   } 
}
//-------------------------------------------
void TOFPedCalib::outp(int flg){// very preliminary
// flg=0/1/2/3=>HistosOnly/write2DB+file/write2file
   int i,il,ib,is,pm,ch,statmin(9999);
   geant pdiff,por2rem,p2r;
   geant pedmin,pedmax,sigmin,sigmax;
   uinteger runn=AMSUser::JobFirstRunN();//job 1st run# 
   time_t begin=time_t(runn);//begin time = runn
   time_t end,insert;
   char DataDate[30],WrtDate[30];
   strcpy(DataDate,asctime(localtime(&begin)));
   time(&insert);
   strcpy(WrtDate,asctime(localtime(&insert)));
//
   integer evs2rem;
   integer totchs(0),goodchs(0);
   geant goodchp(0);
   if(TFREFFKEY.relogic[0]==5)por2rem=TFCAFFKEY.pedcpr[0];//ClassPed(random)
   else if(TFREFFKEY.relogic[0]==6)por2rem=TFCAFFKEY.pedcpr[1];//DownScaled(in trigger)
   pedmin=TFCAFFKEY.pedlim[0];
   pedmax=TFCAFFKEY.pedlim[1];
   sigmin=TFCAFFKEY.siglim[0];
   sigmax=TFCAFFKEY.siglim[1];
//
   cout<<endl;
   cout<<"=====> TOFPedCalib-Report:"<<endl<<endl;
   for(il=0;il<TOF2DBc::getnplns();il++){
     for(ib=0;ib<TOF2DBc::getbppl(il);ib++){
       for(is=0;is<2;is++){
         ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
//
         for(pm=0;pm<TOF2DBc::npmtps(il,ib)+1;pm++){//<--- pm-loop(0/1-3 => an/dyn1-3)
	   totchs+=1;
           if(port2r[ch][pm]<0)p2r=por2rem;//use default for suspicious channel
           else{
	     if(pm==0)p2r=port2r[ch][pm]/10;//use reduced value because of the ped+-n*sig limits
	     else p2r=port2r[ch][pm]/15;
	   }
	   if(nevt[ch][pm]>=TFPCEVMN){//statistics ok
	     evs2rem=integer(floor(p2r*nevt[ch][pm]+0.5));
	     if(evs2rem>nstacksz)evs2rem=nstacksz;
             if(evs2rem<1)evs2rem=1;
	     for(i=0;i<evs2rem;i++){//remove highest amplitudes
	       adc[ch][pm]=adc[ch][pm]-adcm[ch][pm][i];
	       adc2[ch][pm]=adc2[ch][pm]-adcm[ch][pm][i]*adcm[ch][pm][i];
	     }
	     adc[ch][pm]/=number(nevt[ch][pm]-evs2rem);//truncated average
	     adc2[ch][pm]/=number(nevt[ch][pm]-evs2rem);
	     adc2[ch][pm]=adc2[ch][pm]-adc[ch][pm]*adc[ch][pm];//truncated rms**2
//	     cout<<" ped/rms2="<<adc[ch][pm]<<" "<<adc2[ch][pm]<<endl;
	     if(adc2[ch][pm]>(sigmin*sigmin) && adc2[ch][pm]<=(sigmax*sigmax)
	                         && adc[ch][pm]>pedmin && adc[ch][pm]<=pedmax){//chan.OK
	       peds[ch][pm]=geant(adc[ch][pm]);
	       sigs[ch][pm]=geant(sqrt(adc2[ch][pm]));
	       stas[ch][pm]=0;//ok
	       goodchs+=1;
//update ped-object in memory:
	       if(pm==0){//anodes
	         pdiff=peds[ch][pm]-TOFBPeds::scbrped[il][ib].apeda(is);
	         TOFBPeds::scbrped[il][ib].apeda(is)=peds[ch][pm];
	         TOFBPeds::scbrped[il][ib].asiga(is)=sigs[ch][pm];
	         TOFBPeds::scbrped[il][ib].astaa(is)=stas[ch][pm];
		 HF1(1790+is,geant(il*10+ib+1),TOFBPeds::scbrped[il][ib].apeda(is));
		 HF1(1792+is,geant(il*10+ib+1),TOFBPeds::scbrped[il][ib].asiga(is));
		 HF1(1808,sigs[ch][pm],1.);
		 HF1(1809,pdiff,1.);
	       }
	       else{//dynodes
	         pdiff=peds[ch][pm]-TOFBPeds::scbrped[il][ib].apedd(is,pm-1);
	         TOFBPeds::scbrped[il][ib].apedd(is,pm-1)=peds[ch][pm];
	         TOFBPeds::scbrped[il][ib].asigd(is,pm-1)=sigs[ch][pm];
	         TOFBPeds::scbrped[il][ib].astad(is,pm-1)=stas[ch][pm];
		 HF1(1794+3*is+pm-1,geant(il*10+ib+1),peds[ch][pm]);
		 HF1(1800+3*is+pm-1,geant(il*10+ib+1),sigs[ch][pm]);
		 HF1(1810,sigs[ch][pm],1.);
		 HF1(1811,pdiff,1.);
	       }
	       if(statmin>nevt[ch][pm])statmin=nevt[ch][pm];
	     }
	     else{//bad chan
	       cout<<"TOFPedCalib:BadCh=Lay/Pad/Side/Pmi="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
	       cout<<"                        Nevents="<<nevt[ch][pm]<<endl;    
	       cout<<"                        ped/sig**2="<<adc[ch][pm]<<" "<<adc2[ch][pm]<<endl;    
	     }
	   }//--->endof "good statistics" check
	   else{
	     if(nevt[ch][pm]>0){
	       cout<<"TOFPedCalib:LowStatCh=Lay/Pad/Side/Pmi="<<il<<" "<<ib<<" "<<is<<" "<<pm<<endl;
	       cout<<"                             Nevents="<<nevt[ch][pm]<<endl;
	     }    
	   }
	}//--->endof pm-loop 
      }//--->endof side-loop
    }//--->endof paddle-loop
  }//--->endof layer-loop
  goodchp=geant(goodchs)/totchs;
   cout<<"TOFPedCalib: MinAcceptableStatistics/channel was:"<<statmin<<" GoodChsPort="<<goodchp<<endl; 
//   
// ---> prepare update of DB :
   if(flg==1 && goodchp>=0.5){
     AMSTimeID *ptdv;
     ptdv = AMSJob::gethead()->gettimestructure(AMSID("Tofpeds",AMSJob::gethead()->isRealData()));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     end=begin+86400*30;
     ptdv->SetTime(insert,begin,end);
   }
   else{
     if(flg==1 && goodchp<0.5){
       cout<<"<---- TOFPedCalib: Not enough good channels - abort automatic writing to DB !!!"<<endl;
     }
   }
// ---> write MC/RD ped-file:
   if(flg==1 || flg==2){
     integer endflab(12345);
     char fname[80];
     char name[80];
     char buf[20];
//
     if(TFREFFKEY.relogic[0]==5)strcpy(name,"tofp_cl");//classic(all_events rundom trig)
     if(TFREFFKEY.relogic[0]==6)strcpy(name,"tofp_ds");//down_scaled events
     if(AMSJob::gethead()->isMCData())           // for MC-event
     {
       cout <<" TOFPedCalib: new MC peds-file will be written..."<<endl;
       strcat(name,"_mc.");
     }
     else                                       // for Real events
     {
       cout <<" TOFPedCalib: new RD peds-file will be written..."<<endl;
       strcat(name,"_rl.");
     }
     sprintf(buf,"%d",runn);
     strcat(name,buf);
//     if(TFCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//     if(TFCAFFKEY.cafdir==1)strcpy(fname,"");
     strcpy(fname,"");
     strcat(fname,name);
     cout<<"Open file : "<<fname<<'\n';
     cout<<" Date of the first used event : "<<DataDate<<endl;
     ofstream icfile(fname,ios::out|ios::trunc); // open pedestals-file for writing
     if(!icfile){
       cerr <<" TOFPedCalib: Problems to write new pedestals-file "<<fname<<endl;
       exit(1);
     }
     icfile.setf(ios::fixed);
//
// ---> write Anodes peds/sigmas/stat:
//
     for(il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
       for(ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
         for(is=0;is<2;is++){// sequence: side1,side2
           ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
           icfile.width(2);
           icfile.precision(1);
           icfile << stas[ch][0];//stat
           icfile.width(6);
           icfile.precision(1);
           icfile << peds[ch][0];//ped
           icfile.width(5);
           icfile.precision(1);
           icfile << sigs[ch][0];//sig
	   icfile << "  ";
         }
         icfile << endl;
       } // --- end of bar loop --->
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
//---> write dynodes:
     for(il=0;il<TOF2DBc::getnplns();il++){   // <-------- loop over layers
       for(ib=0;ib<TOF2DBc::getbppl(il);ib++){  // <-------- loop over bar in layer
         for(is=0;is<2;is++){// sequence: side1,side2
           ch=il*TOF2GC::SCMXBR*2+ib*2+is;//seq. channels numbering
	   for(pm=0;pm<TOF2GC::PMTSMX;pm++){
             icfile.width(2);
             icfile.precision(1);
             icfile << stas[ch][pm+1];//stat
             icfile.width(6);
             icfile.precision(1);
             icfile << peds[ch][pm+1];//ped
             icfile.width(5);
             icfile.precision(1);
             icfile << sigs[ch][pm+1];//sig
	     icfile << "  ";
           }
	   icfile << "  ";
	 }
         icfile << endl;
       } // --- end of bar loop --->
       icfile << endl;
     } // --- end of layer loop --->
     icfile << endl;
//
     icfile << endl<<"======================================================"<<endl;
     icfile << endl<<" Date of the 1st used event : "<<DataDate<<endl;
     icfile << endl<<" Date of the file writing   : "<<WrtDate<<endl;
     icfile.close();
//
   }//--->endof file writing 
//
   for(i=0;i<22;i++)HPRINT(1790+i);
   HPRINT(1820);
   HPRINT(1821);
   HPRINT(1830);
   HPRINT(1831);
   cout<<endl;
   cout<<"====================== TOFPedCalib: job is completed ! ======================"<<endl;
   cout<<endl;
//
}
//
//-------------------------------------------
//--------------
void TOFPedCalib::ntuple_close(){
//
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tofpedsig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tofpedsig");
  CLOSEF(IOPA.hlun+1);
//
}
//=============================================================================
//======> TOFTdcCalib:
integer TOFTdcCalib::evpch[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2];//events/tdc_chan
number TOFTdcCalib::diflin[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//chann's diff.nonlin
geant TOFTdcCalib::intlin[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//chann's integr.nonlin
number TOFTdcCalib::avtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//average chip's temperature(use 1st ch entries only)
integer TOFTdcCalib::tempev[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//temper.statistics(use 1st ch. entries only)
geant TOFTdcCalib::mntemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//min ...................... 
geant TOFTdcCalib::mxtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//max ...................... 
geant TOFTdcCalib::iavtemp[TOF2GC::SCCRAT][TOF2GC::SCFETA-1];//interm.aver ......................
integer TOFTdcCalib::iupnum; 
time_t TOFTdcCalib::BeginTime;
uinteger TOFTdcCalib::BeginRun;
integer TOFTdcCalib::istore[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2][1024];//statistics store
//--------------------------
void TOFTdcCalib::init(){ // ----> initialization for TofTdc-calibration
//called in catofinitjob()
  int crt,ssl,ich,bin,csl,i,j;
  char fname[20];
  strcpy(fname,"TofTdcIStore.dat");
//
  cout<<"====> TOFTdcCalib::init, use Mode:"<<TFCAFFKEY.tdccum<<endl;
  if(TFCAFFKEY.tdccum/10==1 && TFCAFFKEY.tdccum%10>0){//not valid, correct it for economy mode priority !
    i=TFCAFFKEY.tdccum/10;
    TFCAFFKEY.tdccum=i*10;
    cout<<"     Mode is not valid, corrected to Mode:"<<TFCAFFKEY.tdccum<<endl;
  }
// book hist:
  HBOOK1(1600,"Bin-statistics for Cr0SL3Ch1(LBBS=1041)",1024,0.,1024.,0.);
  HBOOK1(1601,"Bin-statistics for Cr2SL2Ch4(LBBS=3092)",1024,0.,1024.,0.);
  HBOOK1(1602,"Integr.Linearity for Cr0SL3Ch1(LBBS=1041)",256,0.,256.,0.);
  HBOOK1(1603,"Integr.Linearity for Cr0SL3Ch5(same crt FT)",256,0.,256.,0.);
  HBOOK1(1604,"Integr.Linearity for Cr1SL3Ch1(LBBS=1022)",256,0.,256.,0.);
  if(AMSJob::gethead()->isMCData()){
    HBOOK1(1610,"Integr.Linearity SD-MC for Cr2Sl2,Ch=0",50,-5.,5.,0.);
    HBOOK1(1611,"Integr.Linearity SD-MC for Cr2Sl2,Ch=1",50,-5.,5.,0.);
    HBOOK1(1612,"Integr.Linearity SD-MC for Cr2Sl2,Ch=2",50,-5.,5.,0.);
    HBOOK1(1613,"Integr.Linearity SD-MC for Cr2Sl2,Ch=3",50,-5.,5.,0.);
    HBOOK1(1614,"Integr.Linearity SD-MC for Cr2Sl2,Ch=4",50,-5.,5.,0.);
    HBOOK1(1615,"Integr.Linearity SD-MC for Cr2Sl2,Ch=5",50,-5.,5.,0.);
    
    HBOOK1(1616,"Integr.Linearity SD-MC for Cr1Sl2,Ch=0",50,-5.,5.,0.);
    HBOOK1(1617,"Integr.Linearity SD-MC for Cr1Sl2,Ch=1",50,-5.,5.,0.);
    HBOOK1(1618,"Integr.Linearity SD-MC for Cr1Sl2,Ch=2",50,-5.,5.,0.);
    HBOOK1(1619,"Integr.Linearity SD-MC for Cr1Sl2,Ch=3",50,-5.,5.,0.);
    HBOOK1(1620,"Integr.Linearity SD-MC for Cr1Sl2,Ch=4",50,-5.,5.,0.);
    HBOOK1(1621,"Integr.Linearity SD-MC for Cr1Sl2,Ch=5",50,-5.,5.,0.);
  }
  HBOOK1(1700,"Crate min.statistics status (ok/not->0/1) in slot/chan, Crt=1",40,1.,41.,0.);
  HBOOK1(1701,"Crate min.statistics status (ok/not->0/1) in slot/chan, Crt=2",40,1.,41.,0.);
  HBOOK1(1702,"Crate min.statistics status (ok/not->0/1) in slot/chan, Crt=3",40,1.,41.,0.);
  HBOOK1(1703,"Crate min.statistics status (ok/not->0/1) in slot/chan, Crt=4",40,1.,41.,0.);
//
  iupnum=0; 
  for(crt=0;crt<TOF2GC::SCCRAT;crt++){
    for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
      avtemp[crt][ssl]=0;
      iavtemp[crt][ssl]=0;
      tempev[crt][ssl]=0;
      mntemp[crt][ssl]=999;
      mxtemp[crt][ssl]=-999;
      csl=crt*(TOF2GC::SCFETA-1)+ssl;
      for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
        evpch[csl][ich]=0;
        for(bin=0;bin<1024;bin++){
	  diflin[csl][ich][bin]=0;
	  intlin[csl][ich][bin]=0;
	}
      }
    }
  }
//
  if(TFCAFFKEY.tdccum%10>0){//use ext.file to collect/keep statistics
    if(TFCAFFKEY.tdccum%10==1){//start from scratch(clear array and later rewrite storage file)
      for(crt=0;crt<TOF2GC::SCCRAT;crt++){
        for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
          csl=crt*(TOF2GC::SCFETA-1)+ssl;
          for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
            for(bin=0;bin<1024;bin++)istore[csl][ich][bin]=0;
          }
        }
      }
    }
//
    else{//read existing interm.storage file
      ifstream icfile(fname,ios::in);
      if(!icfile){
        cerr<<"<---- Error opening interm.storage file"<<fname<<'\n';
        exit(8);
      }
      cout<<"      Open file for TOFTdc-calibration interm.storage: "<<fname<<'\n';
      icfile >> iupnum;
      for(crt=0;crt<TOF2GC::SCCRAT;crt++){//crate loop
        for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){//SFET(A) seq.slot(link#) loop
          csl=crt*(TOF2GC::SCFETA-1)+ssl;
	  icfile >> mntemp[crt][ssl];
	  icfile >> iavtemp[crt][ssl];
	  icfile >> mxtemp[crt][ssl];
          for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){//channel-loop:
            for(bin=0;bin<1024;bin++){//bin loop
              icfile >> istore[csl][ich][bin];
            }//--->endof bin-loop
          }//--->endof chan-loop
        }//--->endof SFET(A) seq.slot(link#) loop
      }//--->endof crate-loop
      icfile.close();
//    
    }
  }
  cout<<"<==== TOFTdcCalib::init done !"<<endl;
}
//-----------------------
void TOFTdcCalib::outp(int flg){
  int crt,ssl,ich,bin,csl,i,j,binmin;
  int sstat[TOF2GC::SCCRAT*(TOF2GC::SCFETA-1)][TOF2GC::SCTDCCH-2];
  int16u rdch,mtyp;
  int16 slot;
  integer chbmap; 
  uinteger BeginRun=AMSUser::JobFirstRunN();//job 1st run# 
  time_t BeginTime=time_t(BeginRun);//begin time = BeginRun
  time_t end,insert;
  char fname[80],ifname[80];
  char frdate[30];
  char vers1[3]="MC";
  char vers2[3]="RD";
  char fext[20];
  char DataDate[30],WrtDate[30];
  geant bincon[1024],lincor[1024],lincor4[256];
  integer endflab(12345);
//
  cout<<endl<<endl;
  cout<<"====> TOFTdcCalibration results:"<<endl;
  strcpy(DataDate,asctime(localtime(&BeginTime)));
  time(&insert);
  strcpy(WrtDate,asctime(localtime(&insert)));
  strcpy(ifname,"TofTdcIStore.dat");
//
//--> calc. run's and interm. aver.temperatures:
//
  for(crt=0;crt<TOF2GC::SCCRAT;crt++){
    for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
      if(tempev[crt][ssl]>0)avtemp[crt][ssl]/=tempev[crt][ssl];
      cout<<"      Crate/SSlot:"<<crt<<" "<<ssl<<" Run Aver.temper="<<avtemp[crt][ssl]<<
                                            " min/max="<<mntemp[crt][ssl]<<" "<<mxtemp[crt][ssl]<<endl<<endl;
      iavtemp[crt][ssl]=(iupnum*iavtemp[crt][ssl]+avtemp[crt][ssl])/(iupnum+1);//imply equal statistics updates
    }
  }
//
//--> write interm.output file (if requested):
//
  if(TFCAFFKEY.tdccum%10>0){//store temper/diff.lin. data into interm.storage file
    ofstream osfile(ifname,ios::out|ios::trunc);
    if(!osfile){
      cerr<<"<---- Error opening interm.storage file for output"<<ifname<<'\n';
      exit(8);
    }
    cout<<"      Open file for TOFTdc-calibration interm.storage: "<<ifname<<'\n';
    cout<<"      First run used for calibration is "<<BeginRun<<endl;
    cout<<"      Date of the first event : "<<DataDate<<endl;
    osfile.setf(ios::dec);
    osfile << (iupnum+1)<<endl;
    for(crt=0;crt<TOF2GC::SCCRAT;crt++){//crate loop
      for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){//SFET(A) seq.slot(link#) loop
        osfile << mntemp[crt][ssl]<<" ";
        osfile << iavtemp[crt][ssl]<<" ";
        osfile << mxtemp[crt][ssl];
        osfile <<endl;
        csl=crt*(TOF2GC::SCFETA-1)+ssl;
        for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){//channel-loop:
          for(bin=0;bin<1024;bin++){//bin loop
            osfile << istore[csl][ich][bin]<<" ";
            if(bin%32==31)osfile << endl;
          }//--->endof bin-loop
          osfile << endl;
        }//--->endof chan-loop
        osfile << endl;
        osfile << endl;
      }//--->endof SFET(A) seq.slot(link#) loop
    }//--->endof crate-loop
    osfile << endl<<"======================================================"<<endl;
    osfile << endl<<"     First run used for calibration is "<<BeginRun<<endl;
    osfile << endl<<"     Date of the first event : "<<DataDate<<endl;
    osfile.close();
    cout<<"<---- TOFTdcCalib:output interm.storage file is closed !"<<endl;
  }
//-----
  if(TFCAFFKEY.tdccum%10>2 || TFCAFFKEY.tdccum%10==0){//was single-pass or last pass of interm.storage run(any econ/norm mode) : 
    strcpy(fname,"TofTdcor");
    if(AMSJob::gethead()->isMCData()){
      strcat(fname,vers1);
      sprintf(fext,"%d",(TFMCFFKEY.calvern+1));//MC-versn
    }
    else{
      strcat(fname,vers2);
      sprintf(fext,"%d",BeginRun);//tempor RD-Run# = UTC-time of 1st "on-board" event
    }
    strcat(fname,".");
    strcat(fname,fext);
    if(TFCAFFKEY.tdccum/10==1)strcat(fname,"_ecom");
    else strcat(fname,"_norm");
//
//---> prepare standard evpch/diflin arrays:
//
    if(TFCAFFKEY.tdccum%10>2){//last pass of interm.store run: fill standard arrays with data from interm.store
      for(crt=0;crt<TOF2GC::SCCRAT;crt++){
        for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
          csl=crt*(TOF2GC::SCFETA-1)+ssl;
          for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
            for(bin=0;bin<1024;bin++){
	      evpch[csl][ich]+=istore[csl][ich][bin];
	      diflin[csl][ich][bin]=number(istore[csl][ich][bin]);
	    }
	  }
	}
      }
    }
//---> do nothing special for non_econom(=norm=1slot)-mode: evpch/diflin arrays are already prepared by store-routine !
//
//----
    if(TFCAFFKEY.tdccum/10==1){//economy(1slot) mode: copy crt/ssl=0 data to other crt/ssl
      for(crt=0;crt<TOF2GC::SCCRAT;crt++){
        for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
	  if(crt==0 && ssl==0)continue;//skip crt/ssl=0 because it is already filled by fill-routine
          csl=crt*(TOF2GC::SCFETA-1)+ssl;
          for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
	    evpch[csl][ich]=evpch[0][ich];
            for(bin=0;bin<1024;bin++){
	      diflin[csl][ich][bin]=diflin[0][ich][bin];
	    }
	  }
	}
      }
    }
//
    for(bin=0;bin<1024;bin++)bincon[bin]=geant(diflin[3][1][bin]);//cr=0,sl=3,ch=1(LBBS=1041)
    HPAK(1600,bincon);
    for(bin=0;bin<1024;bin++)bincon[bin]=geant(diflin[10][4][bin]);//cr=2,sl=2,ch=4(LBBS=3092)
    HPAK(1601,bincon);
//
//---> prepare D/I linearity corrections from standard arrays :
//
    number totchan(0),totgchan(0),avstat(0),avmnstat(0),badbinch(0),gchanperc(0);
    number cref,dnl,inl;
    geant evbinmn,evbinmx;
    cref=0;
    for(crt=0;crt<TOF2GC::SCCRAT;crt++){
      cout<<"--->Crate:"<<crt<<endl;
      for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
        slot=AMSSCIds::crdid2sl(int16u(crt),int16u(ssl));//SFET(A)seq.slt(link#)=>glob.seq.slot#(0-10)
        if(slot<0){
	  cout<<"<--- Illegal glob.seq.Slot !"<<endl;
	  exit(1);
        }
        csl=crt*(TOF2GC::SCFETA-1)+ssl;
	cout<<"    SSlot:"<<ssl<<endl;
        for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){//<--- chan-loop (8-2 channels because don't need sumHT,sumSHT)
	  mtyp=0;//here means any time-type chan (LT,FT,...)
          rdch=AMSSCIds::ich2rdch(int16u(crt),int16u(slot),int16u(ich),mtyp);//1,...(=0 if ich empty)
          if(rdch==0)continue;//skip empty(not_connected=not_used) input channel
	  totchan+=1;//counts connected channels
	  cout<<"      Chan:"<<ich<<endl;
          sstat[csl][ich]=0;//clear statistics status array(0=ok)
          cref=number(evpch[csl][ich])/1024;
	  if(cref<TFCAFFKEY.minstat)continue;//skip dead channel(aver stat/bin < min.accepted)
	  totgchan+=1;//counts good channels (aver.stat/bin > min)
	  avstat+=cref;
	  evbinmn=1000000;
	  evbinmx=0;
	  for(i=0;i<1024;i++){
	    if(diflin[csl][ich][i] < evbinmn){
	      evbinmn=diflin[csl][ich][i];
	      binmin=i;
	    }
	  }
	  for(i=0;i<1024;i++)if(diflin[csl][ich][i] > evbinmx)evbinmx=diflin[csl][ich][i];
	  avmnstat+=evbinmn;
	  cout<<"      Aver.events/ch/bin="<<cref<<" Evs/bin min/max="<<evbinmn<<" "<<evbinmx<<endl;
	  if(evbinmn < TFCAFFKEY.minstat){
	    sstat[csl][ich]=1;
	    badbinch+=1;
	    cout<<"   <--Warning:Low-stats chan/bin:"<<ich<<" "<<binmin<<endl;
	  }
          inl=0;
          for(bin=0;bin<1024;bin++){
	    dnl=1.-diflin[csl][ich][bin]/cref;
	    diflin[csl][ich][bin]=dnl;
	    inl+=dnl;
	    intlin[csl][ich][bin]=geant(inl);
	  }
        }//--->endof chan-loop
      }//--->endof SFET(slot)-loop
    }//--->endof crate-loop
//
    if(totchan>0){
      avstat/=totgchan;//aver.stat/bin for good channels(aver. stat/bin(cref) > min)
      avmnstat/=totgchan;//aver. minstat/bin(..........................)
      badbinch/=totgchan;//portion of channels having at least 1 bin with low statistics
      badbinch*=100;//in %
      gchanperc=100*(totchan-totgchan)/totchan;// % bad channels (connected but having too low aver. stat/bin)
      cout<<endl<<"<--- TDCLcalSummary: TotNonEmptyCh="<<totchan<<" TotGoodCh(WithBinAverOK)="<<totgchan<<endl;
      cout<<"     LowBinStatCh%="<<badbinch<<" TotBadCh(WithBinAverBad)="<<gchanperc<<endl<<endl; 
    }
//-- fill 4th(->TDCL) line in CALIB results block:
    TOF2JobStat::cqual(3,0)=avstat;
    TOF2JobStat::cqual(3,1)=avmnstat;
    TOF2JobStat::cqual(3,2)=badbinch;
    TOF2JobStat::cqual(3,3)=gchanperc;
    TOF2JobStat::cqual(3,4)=0;
//
//---  histograms of integr.nonlinearity in scale*4 for few channels:
//
    geant sum4;
    crt=0;
    ssl=3;
    ich=1;
    csl=crt*(TOF2GC::SCFETA-1)+ssl;
    for(i=0;i<256;i++){
      sum4=0;
      for(j=0;j<4;j++){
        bin=i*4+j;
        sum4+=intlin[csl][ich][bin];
      }  
      lincor4[i]=sum4;
    }
    HPAK(1602,lincor4);
//
    crt=0;
    ssl=3;
    ich=5;
    csl=crt*(TOF2GC::SCFETA-1)+ssl;
    for(i=0;i<256;i++){
      sum4=0;
      for(j=0;j<4;j++){
        bin=i*4+j;
        sum4+=intlin[csl][ich][bin];
      }  
      lincor4[i]=sum4;
    }
    HPAK(1603,lincor4);
//
    crt=1;
    ssl=3;
    ich=1;
    csl=crt*(TOF2GC::SCFETA-1)+ssl;
    for(i=0;i<256;i++){
      sum4=0;
      for(j=0;j<4;j++){
        bin=i*4+j;
        sum4+=intlin[csl][ich][bin];
      }  
      lincor4[i]=sum4;
    }
    HPAK(1604,lincor4);
//
//    
    for(crt=0;crt<TOF2GC::SCCRAT;crt++){
      for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){
        csl=crt*(TOF2GC::SCFETA-1)+ssl;
        for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++)if(sstat[csl][ich]>0)HF1(1700+crt,geant(ssl*10+ich+1),1.);  
      }
      HPRINT(1700+crt);
    }
//-------- hist of bin-by-bin (seed - calc) lin.corr for 2 crt and 2 slt in each channel:
    geant diff;
    if(AMSJob::gethead()->isMCData()){//check 6 channels of cr/sl=2/2,3(where ch4 is present)
      crt=2;
      ssl=2;
      csl=crt*(TOF2GC::SCFETA-1)+ssl;
      for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
        if(!TofTdcCorMS::tdccor[crt][ssl].truech(ich))continue;//illegal(empty) channel for nonlin-procedure
        for(bin=0;bin<1024;bin++){
          diff=TofTdcCorMS::tdccor[crt][ssl].getinval(ich,bin)-intlin[csl][ich][bin];
//	  if(ich==4){
//	    cout<<"bin/seed="<<bin<<" "<<TofTdcCorMS::tdccor[crt][ssl].getinval(ich,bin)<<" ";
//	    cout<<"calc="<<intlin[csl][ich][bin]<<" diff="<<diff<<endl;
//	  }
	  HF1(1610+ich,diff,1.);
        }
        HPRINT(1610+ich);
      }
//
      crt=1;
      ssl=2;
      csl=crt*(TOF2GC::SCFETA-1)+ssl;
      for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){
        if(!TofTdcCorMS::tdccor[crt][ssl].truech(ich))continue;//illegal(empty) channel for nonlin-procedure
        for(bin=0;bin<1024;bin++){
          diff=TofTdcCorMS::tdccor[crt][ssl].getinval(ich,bin)-intlin[csl][ich][bin];
	  HF1(1616+ich,diff,1.);
        }
        HPRINT(1616+ich);
      }
    }
//--- write TDCL-calibration resume in standard CALIB-block:
    printf("<==========Tof CALIB Results:\n");
    for(i=0;i<4;i++){
      if(i<3)for(j=0;j<5;j++)printf("% 7.3f",TOF2JobStat::cqual(i,j));
      else for(j=0;j<5;j++)printf("% 7.1f",TOF2JobStat::cqual(i,j));// format for TDCL
      printf("\n");
    }
    printf("<==========End of Results\n\n");
//---
//
// ---> write TDC-corr to file:
//
    ofstream ocfile(fname,ios::out|ios::trunc);
    if(!ocfile){
      cerr<<"<----- Error opening file for output"<<fname<<'\n';
      exit(8);
    }
    cout<<"     Open file for TOFTdc-calibration output, fname:"<<fname<<'\n';
    cout<<"     TDC linearity-corrections will be written !"<<'\n';
    cout<<"     First run used for calibration is "<<BeginRun<<endl;
    cout<<"     Date of the first event : "<<DataDate<<endl;
//
    ocfile.setf(ios::fixed);
    ocfile.width(6);
    ocfile.precision(2);// precision 
    for(crt=0;crt<TOF2GC::SCCRAT;crt++){//crate loop
      ocfile << (crt+1);
      ocfile << endl;
      for(ssl=0;ssl<TOF2GC::SCFETA-1;ssl++){//SFET(A) seq.slot(link#) loop
        slot=AMSSCIds::crdid2sl(int16u(crt),int16u(ssl));//SFET(A)seq.slt(link#)=>glob.seq.slot#(0-10)
        if(slot<0){
	  cout<<"<--- Illegal glob.seq.Slot !"<<endl;
	  exit(1);
        }
        chbmap=0;
        for(i=0;i<TOF2GC::SCTDCCH-2;i++){//loop to prepare non-empty inp.ch map
	  mtyp=0;//here means any time-type chan (LT,FT,...)
          rdch=AMSSCIds::ich2rdch(int16u(crt),int16u(slot),int16u(i),mtyp);//1,...(=0 if ich empty)
          if(rdch!=0)(chbmap|=(1<<i));
        }
        ocfile <<"  "<<(ssl+1)<<endl;
        csl=crt*(TOF2GC::SCFETA-1)+ssl;
        ocfile <<"   "<<chbmap<<endl;
        for(ich=0;ich<TOF2GC::SCTDCCH-2;ich++){//channel-loop:
          if((chbmap&(1<<ich))==0)continue;//skip empty inp.channel
          for(bin=0;bin<1024;bin++){//bin loop
            ocfile << intlin[csl][ich][bin]<<" ";
            if(bin%32==31)ocfile << endl;
          }//--->endof bin-loop
          ocfile << endl;
        }//--->endof chan-loop
        ocfile << endl;
        ocfile << endl;
      }//--->endof SFET(A) seq.slot(link#) loop
    }//--->endof crate-loop
    ocfile << endflab;
//
    ocfile << endl;
    ocfile << endl<<"======================================================"<<endl;
    ocfile << endl<<" First run used for calibration is "<<BeginRun<<endl;
    ocfile << endl<<" Date of the first event : "<<DataDate<<endl;
    ocfile.close();
    cout<<"<--- TOFTdcCalib:output file is closed !"<<endl;
    cout<<endl<<endl;
    HPRINT(1600);
    HPRINT(1601);
    HPRINT(1602);
    HPRINT(1603);
    HPRINT(1604);
  }
  cout<<"<==== TOFTdcCalibration is finished !"<<endl;
  cout<<endl<<endl;
}
//-------------------------- 
void TOFTdcCalib::fill(int cr, int sl, int ch, int tdc, geant temp){//for normal/economy mode
//crate=0-3, slot=0-3, ch=0-5(total 6ch, 4(or 5)-LTch + 1-FTch)
  int crt,ssl;
  integer time10=(tdc&(0x3FFL));//10 lsb
  if(!TofTdcCor::tdccor[cr][sl].truech(ch)){//illegal(empty) channel
    cout<<"  TOFTdcCalib::fill: Illegal ch="<<ch<<"  call in Crate/SSl="<<cr<<" "<<sl<<endl;
    return; 
  }
  if(TFCAFFKEY.tdccum/10==0){//normal(non-economy) mode
    crt=cr;
    ssl=sl;
  }
  else {
    crt=0;
    ssl=0;
  }
  diflin[crt*(TOF2GC::SCFETA-1)+ssl][ch][time10]+=1.;
  evpch[crt*(TOF2GC::SCFETA-1)+ssl][ch]+=1;
  if(ch==0 && fabs(temp)<999.){
    avtemp[cr][sl]+=temp;
    tempev[cr][sl]+=1;
    if(temp > mxtemp[cr][sl])mxtemp[cr][sl]=temp;
    if(temp < mntemp[cr][sl])mntemp[cr][sl]=temp;
  }
//
}
//-------------------------- 
void TOFTdcCalib::ifill(int crt, int ssl, int ch, int tdc, geant temp){//for interm.storage mode
  integer time10=(tdc&(0x3FFL));//10 lsb
  istore[crt*(TOF2GC::SCFETA-1)+ssl][ch][time10]+=1;
  if(ch==0 && fabs(temp)<999.){
    avtemp[crt][ssl]+=temp;
    tempev[crt][ssl]+=1;
    if(temp > mxtemp[crt][ssl])mxtemp[crt][ssl]=temp;
    if(temp < mntemp[crt][ssl])mntemp[crt][ssl]=temp;
  }
}
//=============================================================================
