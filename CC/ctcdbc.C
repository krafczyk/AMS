#include <typedefs.h>
#include <commons.h>
#include <math.h>
#include <job.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <tofdbc.h>
#include <ctcdbc.h>
//
// initialize some parameters :
//
integer CTCDBc::_geomId=0;
integer CTCDBc::_nlay=0;
geant CTCDBc::_tdcabw=1.;// TDC-A(T) bin width (ns)
geant CTCDBc::_ftpulw=50.;// FT-pusle width(ns) an TDC input
//----------------------------------------------------------
number CTCDBc::getagsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.agsize[index];
}
  number CTCDBc::getwlsth()  {return CTCGEOMFFKEY.wlsth;}
  integer CTCDBc::getnblk()   {return CTCGEOMFFKEY.nblk;}
  integer CTCDBc::getnwls()   {
    if(_geomId <2)return CTCGEOMFFKEY.nwls;
    else return getnagel();
  }
  void CTCDBc::setnwls(integer nwls) {CTCGEOMFFKEY.nwls=nwls;}
  number CTCDBc::getwallth()         {return CTCGEOMFFKEY.wallth;}
  number CTCDBc::getsupzc() {return CTCGEOMFFKEY.supzc;}
  number CTCDBc::getagap() {return CTCGEOMFFKEY.agap;}
  number CTCDBc::getwgap() {return CTCGEOMFFKEY.wgap;}
  number CTCDBc::getptfeth() {return CTCGEOMFFKEY.ptfx;}
  
  number CTCDBc::getptfesize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.ptfe[index];
}

 number CTCDBc::getthcsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.thcsize[index];
}

 number CTCDBc::getupsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.upper[index];
}
number CTCDBc::getwallsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.wall[index];
} 
number CTCDBc::getcellsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.cell[index];
} 
number CTCDBc::getygapsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.ygap[index];
} 
number CTCDBc::getagelsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.agel[index];
} 
number CTCDBc::getpmtsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.pmt[index];
} 
  number CTCDBc::gethcsize(integer index) {
#ifdef __AMSDEBUG__
assert (index >=0 && index <=2);
#endif
return CTCGEOMFFKEY.hcsize[index];
  }

void CTCDBc::setgeom(integer iflag){
_geomId=iflag;
  if(iflag==2){
   CTCDBc::setnlay(2);
   CTCMCFFKEY.Edep2Phel[1]=0;
  }
  else if(iflag==1){
    CTCDBc::setnwls(CTCDBc::getnblk());
    CTCDBc::setnlay(2);     // Hardwired no of layers
  }
  else {
   CTCDBc::setnlay(1);
   CTCMCFFKEY.Edep2Phel[1]=0;
  }
}

  integer CTCDBc::getnagel()   {
   if(_geomId==0)return getnblk();
   else if(_geomId==1)return 2*getnblk();
   else return getnx()*getny();
  }
//======================================================================
//
CTCCCcal ctcfcal[CTCCCMX];// reserve array of CTC calibr. objects
//                 (for each config. combination(softid in RawEvent class)) 
// CTCCCcal class member functions:
//
void CTCCCcal::build(){
  integer i,j,nm;
  integer ncomb,nmem[CTCCCMX];
  integer mem[CTCCMMX],memb[CTCCCMX][CTCCMMX];
  integer sta[CTCCMMX],stat[CTCCCMX][CTCCMMX];
  geant gn[CTCCMMX],gain[CTCCCMX][CTCCMMX];
  geant q2pe=0.2;//tempor q(pC)->pe conv.factor (pe/pC)
  geant ftdl;// TDCT(FTrig)_hit delay wrt TDCA_hit delay (ns)
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  geant aipa[CTCCCMX][SCIPAR];
  geant aip[SCIPAR]={50.,62.6,1.3};
//      ( def.param. for anode integrator(shft,t0(qthr=exp(t0/shft)),qoffs))
//------------------------------------
  char in[2]="0";
  char inum[11];
  char verlst[20]="ctcverslist.dat";
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read cal.file-versions file :
//
  integer cfvn;
  cfvn=CTCCAFFKEY.cfvers%100;
  if(AMSJob::gethead()->isMCData())strcpy(name,"ctcverlistmc");
  else strcpy(name,"ctcverlistrl");
  dig=cfvn/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
  strcpy(fname,AMSDATADIR.amsdatadir);
//  strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/ctcca/");//tempor
  strcat(fname,name);
  cout<<"CTCCCcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cerr <<"CTCCCcal_build:: missing verslist-file "<<fname<<endl;
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
// ------> read config/gain file:
  ctyp=1;
  strcpy(name,"ctcconfg");
  mcvn=mcvern[ctyp-1]%100;
  rlvn=rlvern[ctyp-1]%100;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
    cout <<" CTCCCcal_build: MC-config/gain file is used"<<endl;
    dig=mcvn/10;
    in[0]=inum[dig];
    strcat(name,in);
    dig=mcvn%10;
    in[0]=inum[dig];
    strcat(name,in);
    strcat(name,vers1);
  }
  else                              //      for Real events
  {
    cout <<" CTCCCcal_build: REAL-config/gain file is used"<<endl;
    dig=rlvn/10;
    in[0]=inum[dig];
    strcat(name,in);
    dig=rlvn%10;
    in[0]=inum[dig];
    strcat(name,in);
    strcat(name,vers2);
  }
//
  strcat(name,".dat");
  strcpy(fname,AMSDATADIR.amsdatadir);    
//  strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/ctcca/");//tempor
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream ctccfile(fname,ios::in); // open  file for reading
  if(!ctccfile){
   cerr <<"CTCCCcal_build: Error opening  "<<fname<<endl;
   exit(1);
  }
  ctccfile >> ncomb; // real number of combinations(readout channels)
  for(i=0;i<ncomb;i++){// <-- comb. loop
    ctccfile >> nm; // number of members
    nmem[i]=nm;
    for(j=0;j<nm;j++){// <-- memb. loop
      ctccfile >> memb[i][j];// member (LXXY=LCCR)
      ctccfile >> stat[i][j];// status 
      ctccfile >> gain[i][j];// gain 
    }
  }
//--------------------------------------------------
// -------> read integrator parameters file :
//
 ctyp=2;
 strcpy(name,"ctcincal");
 mcvn=mcvern[ctyp-1]%100;
 rlvn=rlvern[ctyp-1]%100;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" CTCCCcal_build: integrator-calibration for MC-events selected."<<endl;
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
   cout <<" CTCCCcal_build: integrator-calibration for Real-events selected."<<endl;
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
// strcpy(fname,"/afs/cern.ch/user/c/choumilo/public/ams/AMS/ctcca/");//tempor
 strcat(fname,name);
 cout<<"Open file : "<<fname<<'\n';
 ifstream icfile(fname,ios::in); // open integrator_param-file for reading
 if(!icfile){
   cerr <<"CTCCCcal_build: missing integrator_param-file "<<fname<<endl;
   exit(1);
 }
  icfile >> nm; // number of combinations in integr.file
  if(nm != ncomb){
    cerr <<"CTCCCcal_build: comb/integr.file mismatch(readout-channels number !)"<<endl; 
    exit(1);
  }
  for(i=0;i<ncomb;i++){// <-- comb. loop
    for(j=0;j<SCIPAR;j++){// <-- param. loop
      icfile >> aipa[i][j];// 
    }
  }
//
//--------------------------------------------------
// -------> now create calibr. objects:
  for(i=0;i<ncomb;i++){
    nm=nmem[i];
    for(j=0;j<nm;j++){
      mem[j]=memb[i][j];
    }
    for(j=0;j<nm;j++){
      sta[j]=stat[i][j];
    }
    for(j=0;j<nm;j++){
      gn[j]=gain[i][j];
    }
    for(j=0;j<SCIPAR;j++){
      aip[j]=aipa[i][j];
    }
    ftdl=TOFDBc::ftdelf();// tempor (as for TOF)
    ctcfcal[i]=CTCCCcal(i,nm,mem,sta,ftdl,q2pe,gn,aip);//create calibr. object
  }
}
//---
void CTCCCcal::q2t2q(int cof, number &tovt, number &q){  
// Q(pC) <-> Tovt(ns) to use in sim./rec. programs (cof=0/1-> Q->Tovt/Tovt->Q)
  number qoffs,shft,qthr;
  shft=aipar[0];
  qthr=exp(aipar[1]/shft);//to match old parametrization
  qoffs=aipar[2];
// 
  if(cof==0){ // q->tovt
    if(q>qoffs)tovt=shft*log((q-qoffs)/qthr);
    else tovt=0.;
  }
  else{       // tovt->q
    q=qoffs+qthr*exp(tovt/shft);
  }
}
//----------------------------------------------------------------------------
//=====================================================================  
//
//   CTCJobStat static variables definition (memory reservation):
//
integer CTCJobStat::mccount[CTCJSTA];
integer CTCJobStat::recount[CTCJSTA];
integer CTCJobStat::chcount[CTCCCMX][CTCCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void CTCJobStat::print(){
  int i,j,ic;
  integer nmemb,mblst[CTCCMMX];
  geant rc,rc1,rc2,rc3,rc4;
//
  cout<<endl<<endl;;
  cout<<"    ============ JOB CTC-statistics ============="<<endl<endl;
  cout<<" MC: entries             : "<<mccount[0]<<endl;
  cout<<" MC: Ghits->RawHit OK    : "<<mccount[1]<<endl;
  cout<<" MC: RawHit->RawEvent OK : "<<mccount[2]<<endl;
  cout<<" RECO-entries            : "<<recount[0]<<endl;
  cout<<" Lev-1 trigger OK        : "<<recount[1]<<endl;
  cout<<" RawEvent-validation OK  : "<<recount[2]<<endl;
  cout<<" RawEvent->RawHits OK    : "<<recount[3]<<endl;
  cout<<" RawHits->Cluster OK     : "<<recount[4]<<endl;
  cout<<endl;
//
//
  cout<<"        ===========> Channels validation report :"<<endl<<endl;
//
  cout<<"comb.#"<<" id(LXXY)         "<<"h/w-stat_OK"<<" A-bad_seq(%)  FT-bad_seq(%)"<<endl;
  cout.setf(ios::fixed);
    for(ic=0;ic<CTCCCMX;ic++){
      cout<<setw(6)<<(ic+1)<<"   ";
      nmemb=ctcfcal[ic].getmembers(mblst);
      for(j=0;j<nmemb;j++)cout<<mblst[j]<<" ";
      cout<<"        "<<setw(6)<<chcount[ic][0]<<"        ";
      rc=geant(chcount[ic][0]);
      if(rc>0.){
        rc1=100.*geant(chcount[ic][1])/rc;
        rc2=100.*geant(chcount[ic][2])/rc;
      }
      cout<<setw(6)<<setprecision(2)<<rc1<<"           "<<rc2<<endl;
    }
    cout<<endl;
//
  cout<<"============> Channels reconstruction report :"<<endl<<endl;
//
  cout<<"comb.#"<<" id's (LXXY)   "<<"h/w-stat_OK"<<" A-TDC_ON(%)  Nhit=1(pair)";
  cout<<"  T-TDC_ON(%)   Nhit=1(pair)"<<endl;
    for(ic=0;ic<CTCCCMX;ic++){
      cout<<setw(6)<<(ic+1)<<"   ";
      nmemb=ctcfcal[ic].getmembers(mblst);
      for(j=0;j<nmemb;j++)cout<<mblst[j]<<" ";
      cout<<"  "<<setw(6)<<chcount[ic][4]<<"         ";
      rc=geant(chcount[ic][4]);
      if(rc>0.){
        rc1=100.*geant(chcount[ic][5])/rc;
        rc2=100.*geant(chcount[ic][6])/rc;
        rc3=100.*geant(chcount[ic][7])/rc;
        rc4=100.*geant(chcount[ic][8])/rc;
      }
      cout<<setw(6)<<setprecision(2)<<rc1<<"          "<<setw(6)<<rc2<<"         "
                                            <<setw(6)<<rc3<<"        "<<setw(6)<<rc4<<endl;
    }
    cout<<endl;
//
}
//======================================================================

