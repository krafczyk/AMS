// Author E.Choumilov 2.06.97
//
#include <typedefs.h>
#include <commons.h>
#include <job.h>
#include <amsdbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <tofdbc.h>
#include <antidbc.h>
#include <antirec.h>
//
ANTIPcal antisccal[MAXANTI];// create empty array of antipaddles calibr. objects
//
//======> just memory reservation for ANTIDBc class variables:
// (real values are initialized at run-time from data cards in setgeom() or in...)
// 
  geant ANTIDBc::_scradi=0.;
  geant ANTIDBc::_scinth=0.;
  geant ANTIDBc::_scleng=0.;
  geant ANTIDBc::_wrapth=0.;
  geant ANTIDBc::_groovr=0.;
  geant ANTIDBc::_pdlgap=0.;
  geant ANTIDBc::_stradi=0.;
  geant ANTIDBc::_stleng=0.;
  geant ANTIDBc::_stthic=0.;
//
  geant ANTIDBc::_tdcabw=1.;  // bin width in TovT TDC (ns)
  geant ANTIDBc::_shprdt=50.; // shaper(integrator) decay time (ns)
  geant ANTIDBc::_ftpulw=30.; // FTrigger pulse width (ns)
//
//  ANTIDBc class member functions :
//
  geant ANTIDBc::scradi(){return _scradi;}
  geant ANTIDBc::scinth(){return _scinth;}
  geant ANTIDBc::scleng(){return _scleng;}
  geant ANTIDBc::wrapth(){return _wrapth;}
  geant ANTIDBc::groovr(){return _groovr;}
  geant ANTIDBc::pdlgap(){return _pdlgap;}
  geant ANTIDBc::stradi(){return _stradi;}
  geant ANTIDBc::stleng(){return _stleng;}
  geant ANTIDBc::stthic(){return _stthic;}
  geant ANTIDBc::tdcabw(){return _tdcabw;}
  geant ANTIDBc::shprdt(){return _shprdt;}
  geant ANTIDBc::ftpulw(){return _ftpulw;}
//
  void ANTIDBc::setgeom(){ //get parameters from data cards (for now)
    _scradi=ANTIGEOMFFKEY.scradi;
    _scinth=ANTIGEOMFFKEY.scinth;
    _scleng=ANTIGEOMFFKEY.scleng;
    _wrapth=ANTIGEOMFFKEY.wrapth;
    _groovr=ANTIGEOMFFKEY.groovr;
    _pdlgap=ANTIGEOMFFKEY.pdlgap;
    _stradi=ANTIGEOMFFKEY.stradi;
    _stleng=ANTIGEOMFFKEY.stleng;
    _stthic=ANTIGEOMFFKEY.stthic;
  }
//======================================================================
// ANTIPcal class member functions:
//
void ANTIPcal::build(){ // fill array of objects with data
  integer i,j,k,ip,cnum,nrch,ibr,isd;
  int16u swid,crat,sfea,antic;
  integer sta[2]={0,0}; // all  are alive as default
  integer status[MAXANTI][2];
  geant tthr[2]; // trigger threshold for one side.(p.e. for now)
  geant athr[2]; // TovT threshold for one side.(p.e.)
  geant q2pe=1.;//tempor q(pC)->pe conv.factor (pe/pC)
  geant mip2q;   // conv.factor for Mev->pe (Pe/Mev)
  geant gain[2],gains[MAXANTI][2];
  geant ftdl[2];// TDCT(FTrig)_hit delay wrt TDCA_hit delay (ns)
  geant ipara[ANCHMX][SCIPAR];// the same number of integr. parameters as for TOF
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  geant aip[2][SCIPAR]={// default
    {50.,62.6,1.3},
    {50.,62.6,1.3}
  }; 
// (def.param. for anode integrator(shft,t0(qthr=exp(t0/shft)),qoffs))
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
  nrch=2*ANCHCH;// real number of anti-ch per SFEA (2chip*4chan)
//
// ---> read cal.file-versions file :
//
  integer cfvn;
  cfvn=ANTICAFFKEY.cfvers%100;
  strcpy(name,"antiverlist");
  dig=cfvn/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
  strcpy(fname,AMSDATADIR.amsdatadir);
//  strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/antica/");//tempor
  strcat(fname,name);
  cout<<"ANTIPcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cerr <<"ANTIPcal_build:: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//
//---------------------------------------------
//
//   --->  Read integrator parameters calibr. file :
//
 ctyp=1;
 strcpy(name,"antiincal");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" ANTIPcal_build: integrator-calibration for MC-events selected."<<endl;
   dig=mcvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" ANTIPcal_build: integrator-calibration for Real-events selected."<<endl;
   dig=rlvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
 strcat(name,".dat");
   strcpy(fname,AMSDATADIR.amsdatadir);    
// strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/antica/");//tempor
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream icfile(fname,ios::in); // open integrator_param-file for reading
 if(!icfile){
   cerr <<"ANTIPcal_build: missing integrator_param-file "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANCRAT;i++){ //<--- crate loop (0-max7)
   icfile >> crat;// crate-number
   for(j=0;j<ANSFEA;j++){ //<--- SFEA card loop (0)
     icfile >> sfea;// sfea-number
     for(k=0;k<nrch;k++){ //<--- real anti-chan. loop (0-max15)
       icfile >> antic; // antic-number(tdc-ch numbering accross sfea)
       swid=AMSAntiRawEvent::hw2swid(crat-1,antic-1);//BBS
       if(swid==0)continue;// non-existing antic 
       ibr=swid/10-1;
       isd=swid%10-1;
       cnum=2*ibr+isd;
       for(ip=0;ip<SCIPAR;ip++)icfile >> ipara[cnum][ip];//read anode parameters
     }
   }
 }
 icfile.close();
//
//---------------------------------------------
//
//   --->  Read abs_normalization/gain/status calib. file :
//
 ctyp=2;
 strcpy(name,"antiancal");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" ANTIPcal_build: abs.norm/gain/stat-calib. for MC-events selected."<<endl;
   dig=mcvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" ANTIPcal_build: abs.norm/gain/stat-calib. for Real-events selected."<<endl;
   dig=rlvn/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
 strcat(name,".dat");
   strcpy(fname,AMSDATADIR.amsdatadir);    
// strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/antica/");//tempor
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream acfile(fname,ios::in); // open abs.norm/gain/stat-file for reading
 if(!acfile){
   cerr <<"ANTIPcal_build: missing abs.norm/gain/status file "<<fname<<endl;
   exit(1);
 }
//
 acfile >> mip2q;// read mip2q factor (p.e/mev)
 acfile >> q2pe; // read q2pe factor (p.e./pC)
 for(i=0;i<MAXANTI;i++){
   acfile >> gains[i][0];
   acfile >> status[i][0];
   acfile >> gains[i][1];
   acfile >> status[i][1];
 }
//
 acfile.close();
//
//----------------------------------------------------------------------
// create ANTIPcal objects:
//
  if(AMSJob::gethead()->isMCData()){ //            =====> For MC data:
    for(i=0;i<MAXANTI;i++){
      tthr[0]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      tthr[1]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      athr[0]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      athr[1]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      for(ip=0;ip<SCIPAR;ip++)aip[0][ip]=ipara[2*i][ip];// int.param.from file
      for(ip=0;ip<SCIPAR;ip++)aip[1][ip]=ipara[2*i+1][ip];
//      mip2q=ANTIMCFFKEY.MeV2PhEl; // (pe/mev)
      gain[0]=gains[i][0];// gain from file
      gain[1]=gains[i][1];
      sta[0]=status[i][0];// alive status from file
      sta[1]=status[i][1];
      ftdl[0]=TOFDBc::ftdelf();// tempor (as for TOF)
      ftdl[1]=TOFDBc::ftdelf();// tempor
      antisccal[i]=ANTIPcal(i,sta,tthr,athr,ftdl,mip2q,q2pe,
                                         gain,aip);// create ANTIPcal object
    }
  }
//---------------------------------------------------------------------
  else{ //                                         =====> For Real Data :
    for(i=0;i<MAXANTI;i++){
      tthr[0]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      tthr[1]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      athr[0]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      athr[1]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      for(ip=0;ip<SCIPAR;ip++)aip[0][ip]=ipara[2*i][ip];// int.param.from file
      for(ip=0;ip<SCIPAR;ip++)aip[1][ip]=ipara[2*i+1][ip];
//      mip2q=1./ANTIRECFFKEY.PhEl2MeV; // (pe/mev)
      gain[0]=gains[i][0];// gain from file
      gain[1]=gains[i][1];
      sta[0]=status[i][0];// alive status from file
      sta[1]=status[i][1];
      ftdl[0]=TOFDBc::ftdelf();// tempor (as for TOF)
      ftdl[1]=TOFDBc::ftdelf();// tempor
      antisccal[i]=ANTIPcal(i,sta,tthr,athr,ftdl,mip2q,q2pe,
                                         gain,aip);// create ANTIPcal object
    }
  }
}
//----
void ANTIPcal::q2t2q(int cof, int sdf, number &tovt, number &q){  
// Q(pC) <-> Tovt(ns) to use in sim./rec. programs (cof=0/1-> Q->Tovt/Tovt->Q)
//                                                 (sdf=0/1-> bar side 1/2   )
  number qoffs,shft,qthr;
  shft=aipar[sdf][0];
  qthr=exp(aipar[sdf][1]/shft);//to match old parametrization
  qoffs=aipar[sdf][2];
// 
  if(cof==0){ // q->tovt
    if(q>qoffs)tovt=shft*log((q-qoffs)/qthr);
    else tovt=0.;
  }
  else{       // tovt->q
    q=qoffs+qthr*exp(tovt/shft);
  }
}
//=====================================================================  
//
//   ANTIJobStat static variables definition (memory reservation):
//
integer ANTIJobStat::mccount[ANJSTA];
integer ANTIJobStat::recount[ANJSTA];
integer ANTIJobStat::chcount[ANCHMX][ANCSTA];
integer ANTIJobStat::brcount[MAXANTI][ANCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void ANTIJobStat::print(){
  int il,ib,ic;
  geant rc;
  printf("\n");
  printf("    ============ JOB ANTI-statistics =============\n");
  printf("\n");
  printf(" MC: entries             : % 6d\n",mccount[0]);
  printf(" MC: Ghits->RawEvent OK  : % 6d\n",mccount[1]);
  printf(" RECO-entries            : % 6d\n",recount[0]);
  printf(" Lev-1 trigger OK        : % 6d\n",recount[1]);
  printf(" RawEvent-validation OK  : % 6d\n",recount[2]);
  printf(" RawEvent->RawCluster OK : % 6d\n",recount[3]);
  printf(" RawCluster->Cluster OK  : % 6d\n",recount[4]);
  printf("\n\n");
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Have signal on any side :\n\n");
    for(ib=0;ib<MAXANTI;ib++){
      printf(" % 6d",brcount[ib][0]);
    }
    printf("\n\n");
//
  printf("Have signal > threshold :\n\n");
    for(ib=0;ib<MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  if(!AMSJob::gethead()->isRealData() && TOFMCFFKEY.fast==1)return;
//
  printf("===========> Channels validation report :\n\n");
//
  printf("H/w-status OK (validation) :\n\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n\n");
//
  printf("Anode-TovT TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("FTrigger TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("============> Channels reconstruction report :\n\n");
//
  printf("H/w-status OK :\n\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
//
  printf("TDCA 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("TDCA '1 hit' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("TDCT 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("TDCT '1 hit' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
}
//======================================================================
