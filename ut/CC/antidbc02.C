//  $Id: antidbc02.C,v 1.6 2003/03/18 09:04:06 choumilo Exp $
// Author E.Choumilov 2.06.97
//
#include <typedefs.h>
#include <commons.h>
#include <job.h>
#include <amsdbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <tofdbc02.h>
#include <antidbc02.h>
#include <antirec02.h>
//
ANTI2Pcal ANTI2Pcal::antisccal[ANTI2C::MAXANTI];// init array of antipaddles calibr. objects
ANTIPeds ANTIPeds::anscped[ANTI2C::MAXANTI];//mem.reserv. ANTI-paddles pedestals/sigmas
//
//======> just memory reservation for ANTI2DBc class variables:
// (real values are initialized at run-time from data cards in setgeom() or in...)
// 
  geant ANTI2DBc::_scradi=0.;
  geant ANTI2DBc::_scinth=0.;
  geant ANTI2DBc::_scleng=0.;
  geant ANTI2DBc::_wrapth=0.;
  geant ANTI2DBc::_groovr=0.;
  geant ANTI2DBc::_pdlgap=0.;
  geant ANTI2DBc::_stradi=0.;
  geant ANTI2DBc::_stleng=0.;
  geant ANTI2DBc::_stthic=0.;
//
  geant ANTI2DBc::_fadcbw=1.;//Flash-ADC bin width(ns)
  geant ANTI2DBc::_htdcdr=10.;//Hist-discrim. double pulse resolution(ns)
  geant ANTI2DBc::_htdcbw=1.;  // bin width in Hist-TDC (ns)
  integer ANTI2DBc::_daqscf=2; //daq scaling factor for charge
  geant ANTI2DBc::_ftcoinw=50.;//not used
  geant ANTI2DBc::_hdpdmn=2.;// hist-discrim. min pulse duration
//
//  ANTI2DBc class member functions :
//
  geant ANTI2DBc::scradi(){return _scradi;}
  geant ANTI2DBc::scinth(){return _scinth;}
  geant ANTI2DBc::scleng(){return _scleng;}
  geant ANTI2DBc::wrapth(){return _wrapth;}
  geant ANTI2DBc::groovr(){return _groovr;}
  geant ANTI2DBc::pdlgap(){return _pdlgap;}
  geant ANTI2DBc::stradi(){return _stradi;};
  geant ANTI2DBc::stleng(){return _stleng;};
  geant ANTI2DBc::stthic(){return _stthic;}
  geant ANTI2DBc::fadcbw(){return _fadcbw;}
  geant ANTI2DBc::htdcdr(){return _htdcdr;}
  geant ANTI2DBc::htdcbw(){return _htdcbw;}
  integer ANTI2DBc::daqscf(){return _daqscf;}
  geant ANTI2DBc::ftcoinw(){return _ftcoinw;}
  geant ANTI2DBc::hdpdmn(){return _hdpdmn;}
//
  void ANTI2DBc::setgeom(){ //get parameters from data cards (for now)
    _scradi=ATGEFFKEY.scradi;
    _scinth=ATGEFFKEY.scinth;
    _scleng=ATGEFFKEY.scleng;
    _wrapth=ATGEFFKEY.wrapth;
    _groovr=ATGEFFKEY.groovr;
    _pdlgap=ATGEFFKEY.pdlgap;
    _stradi=ATGEFFKEY.stradi;
    _stleng=ATGEFFKEY.stleng;
    _stthic=ATGEFFKEY.stthic;
    if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<" ANTIGeom-I-AMS02 setup selected."<<endl;
    }
    else
    {
          cout <<" ANTIGeom-I-UNKNOWN setup !!!!"<<endl;
    }
  }
//======================================================================
// ANTI2Pcal class member functions:
//
void ANTI2Pcal::build(){ // fill array of objects with data
  integer i,j,k,ip,cnum,ibr,isd;
  integer sta[2]={0,0}; // all  are alive as default
  integer status[ANTI2C::MAXANTI][2];
  geant athr; // hist-discr threshold(p.e.)
  geant dqthr;//daq-readout thresh(adc-ch)
  geant gain[2],gains[ANTI2C::MAXANTI][2];
  geant m2p[2],mev2pe[ANTI2C::MAXANTI][2];
  geant ftdl[2],ftdel[ANTI2C::MAXANTI][2];
  geant t0,tzer[ANTI2C::MAXANTI],a2p,adc2pe[ANTI2C::MAXANTI];
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
//------------------------------
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  strcpy(inum,"0123456789");
//
// ---> read cal.file-versions file :
//
  integer cfvn;
  cfvn=ATCAFFKEY.cfvers%1000;
  strcpy(name,"antiverlist");
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ANTI2Pcal::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed verslist-file for reading
  if(!vlfile){
    cerr <<"ANTI2Pcal_build:: missing verslist-file "<<fname<<endl;
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
//   --->  Read abs_normalization/gain/status calib. file :
//
 ctyp=1;
 strcpy(name,"anticpf");
 mcvn=mcvern[ctyp-1]%1000;
 rlvn=rlvern[ctyp-1]%1000;
 if(AMSJob::gethead()->isMCData())           // for MC-event
 {
   cout <<" ANTI2Pcal_build: status/calib.const for MC-events "<<endl;
   dig=mcvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(mcvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=mcvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers1);
 }
 else                                       // for Real events
 {
   cout <<" ANTI2Pcal_build: status/calib.const for Real-events"<<endl;
   dig=rlvn/100;
   in[0]=inum[dig];
   strcat(name,in);
   dig=(rlvn%100)/10;
   in[0]=inum[dig];
   strcat(name,in);
   dig=rlvn%10;
   in[0]=inum[dig];
   strcat(name,in);
   strcat(name,vers2);
 }
 strcat(name,".dat");
 if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
 if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
 strcat(fname,name);
 cout<<"ANTI2Pcal_build:Open file : "<<fname<<'\n';
 ifstream acfile(fname,ios::in); // open abs.norm/gain/stat-file for reading
 if(!acfile){
   cerr <<"ANTI2Pcal_build: missing status/calib.const file "<<fname<<endl;
   exit(1);
 }
//
 for(i=0;i<ANTI2C::MAXANTI;i++){
   acfile >> status[i][0];
   acfile >> mev2pe[i][0];
   acfile >> gains[i][0];
   acfile >> ftdel[i][0];
   acfile >> status[i][1];
   acfile >> mev2pe[i][1];
   acfile >> gains[i][1];
   acfile >> ftdel[i][1];
   acfile >> adc2pe[i];// read adc->pe factor (p.e/adc_ch)
   acfile >> tzer[i]; // read Hist-time delay(ns, mainly due to clear fiber length)
 }
//
 acfile.close();
//
//----------------------------------------------------------------------
// create ANTI2Pcal objects:
//
    athr=ATREFFKEY.dathr; // take Hist-discr threshold from DataCard
    dqthr=ATREFFKEY.daqthr;//DAQ-readout-threshold from DataCard
//
    for(i=0;i<ANTI2C::MAXANTI;i++){
      gain[0]=gains[i][0];// Relat. gain from CalibOutput-file(usage is not clear now,
      gain[1]=gains[i][1];// because depends on calib.procedure for mev2pe-parameter)
      sta[0]=status[i][0];// alive status from CalibOutput-file
      sta[1]=status[i][1];
      ftdl[0]=ftdel[i][0];//True hist-hit delay wrt FT from CalibOutput-file(not used now)
      ftdl[1]=ftdel[i][1];
      m2p[0]=mev2pe[i][0];// mev->pe conv.factor(incl.clfib.atten. and may be PM-gain)
      m2p[1]=mev2pe[i][1];
      t0=tzer[i];//ClfFiber+Cable delay from CalibOutput-file
      a2p=adc2pe[i];//adc->pe conv.factor from CalibOutput-file
//
      antisccal[i]=ANTI2Pcal(i,sta,athr,ftdl,m2p,gain,a2p,
                                                 dqthr,t0);// create ANTI2Pcal object
    }
//
}
//
//==========================================================================
//
void ANTIPeds::mcbuild(){// create MC ANTIPeds-objects for each counter
//
  int i,j,is;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antiped");
  cout <<" ANTIPeds_mcbuild: COPY of current RealData peds-file is used..."<<endl;
  strcat(name,"mc");
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<" ANTIPEDS-I-AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<" ANTIPeds:mcbuild-E-Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"ANTIPeds_mcbuild: missing MC default pedestals-file "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
}
//==========================================================================
//
void ANTIPeds::build(){//tempor solution for RealData peds. 
//
  int i,j,is;
  integer sid;
  char fname[80];
  char name[80];
  integer asta[ANTI2C::MAXANTI][2];// array of counter stat
  geant aped[ANTI2C::MAXANTI][2];// array of counter peds
  geant asig[ANTI2C::MAXANTI][2];// array of counter sigmas
  integer stata[2];
  geant peda[2],siga[2];
//
//
//   --->  Read  pedestals file :
//
  strcpy(name,"antiped");
  cout <<" ANTIPeds_build: Current RealData peds-file is used..."<<endl;
  strcat(name,"rl");
// ---> check setup:
//
  if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    cout<<" ANTIPEDS-I-AMS02 setup selected."<<endl;
  }
  else
  {
        cout <<" ANTIPeds:build-E-Unknown setup !!!"<<endl;
        exit(10);
  }
//
  strcat(name,".dat");
  if(ATCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ATCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"ANTIPeds_build: missing real pedestals file "<<fname<<endl;
    exit(1);
  }
//
//---> Read anode stats/peds/sigs:
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <---- loop over sectors
    for(i=0;i<2;i++)icfile >> asta[is][i];// sequence: side1,side2
    for(i=0;i<2;i++)icfile >> aped[is][i];
    for(i=0;i<2;i++)icfile >> asig[is][i];
  } // ---> endof sectors loop
//
//
  icfile.close();
//---------------------------------------------
//   ===> fill ANTIPeds bank :
//
  for(is=0;is<ANTI2C::MAXANTI;is++){  // <--- loop over sectors
    sid=(is+1);
    for(i=0;i<2;i++){
      stata[i]=asta[is][i];
      peda[i]=aped[is][i];
      siga[i]=asig[is][i];
    }
//
    anscped[is]=ANTIPeds(sid,stata,peda,siga);
//
  } // ---> endof sector loop
}
//=====================================================================  
//
//   ANTIJobStat static variables definition (memory reservation):
//
integer ANTI2JobStat::mccount[ANTI2C::ANJSTA];
integer ANTI2JobStat::recount[ANTI2C::ANJSTA];
integer ANTI2JobStat::chcount[ANTI2C::ANCHMX][ANTI2C::ANCSTA];
integer ANTI2JobStat::brcount[ANTI2C::MAXANTI][ANTI2C::ANCSTA];
//
// function to print Job-statistics at the end of JOB(RUN):
//
void ANTI2JobStat::print(){
  int il,ib,ic;
  geant rc;
  printf("\n");
  printf("    ============ JOB ANTI2-statistics =============\n");
  printf("\n");
  printf(" MC: entries             : % 6d\n",mccount[0]);
  printf(" MC: Ghits->RawEvent OK  : % 6d\n",mccount[6]);
  printf(" MC: Flash-ADC buff.ovfl : % 6d\n",mccount[1]);
  printf(" MC: Hist-TDC stack ovfl : % 6d\n",mccount[2]);
  printf(" MC: ADC range ovfl      : % 6d\n",mccount[3]);
  printf(" MC: H-TDC range ovfl    : % 6d\n",mccount[4]);
  printf(" MC: FT/Anti coincidence : % 6d\n",mccount[5]);
  printf(" RECO-entries            : % 6d\n",recount[0]);
  printf(" Lev-1 trigger OK        : % 6d\n",recount[1]);
  printf(" Usage of TOF in LVL1    : % 6d\n",recount[5]);
  printf(" Usage of EC  in LVL1    : % 6d\n",recount[6]);
  printf(" RawEvent-validation OK  : % 6d\n",recount[2]);
  printf(" RawEvent->Cluster OK    : % 6d\n",recount[3]);
  printf("\n\n");
//
  printf("==========> Bars reconstruction report :\n\n");
//
  printf("Have signal on any side :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      printf(" % 6d",brcount[ib][0]);
    }
    printf("\n\n");
//
  printf("Have signal > threshold :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      rc=geant(brcount[ib][0]);
      if(rc>0.)rc=geant(brcount[ib][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("===========> Channels validation report :\n\n");
//
  printf("Validation entries :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][5]);
    }
    printf("\n\n");
//
  printf("Missing Charge-ADC Hit (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][6])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Multiple Charge-ADC Hits(percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][7])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("Hist-TDC wrong up/down sequence (percentage) :\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][5]);
      if(rc>0.)rc=100.*geant(chcount[ic][8])/rc;
      printf("% 5.2f",rc);
    }
    printf("\n\n");
//
  printf("============> Channels reconstruction report :\n\n");
//
  printf("DB-statusOK hit found :\n\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      printf(" % 6d",chcount[ic][0]);
    }
    printf("\n\n");
//
  printf("ADC-ch 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][1])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("ADC-ch '1 hit' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][2])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
  printf("TDC-ch 'ON' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][3])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
//
  printf("TDC-ch '1 hit' (wrt total):\n");
  printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n");
    for(ib=0;ib<ANTI2C::MAXANTI;ib++){
      ic=ib*2+1;
      rc=geant(chcount[ic][0]);
      if(rc>0.)rc=geant(chcount[ic][4])/rc;
      printf("% 5.3f",rc);
    }
    printf("\n\n");
//
}
//======================================================================
