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
  integer i,j;
  integer sta[2]={0,0}; // all  are alive for now
  geant tthr[2]; // trigger threshold for one side.(p.e. for now)
  geant athr[2]; // TovT threshold for one side.(p.e.)
  geant q2pe=1.;//tempor q(pC)->pe conv.factor (pe/pC)
  geant mip2q;   // conv.factor for Mev->pe (Pe/Mev)
  geant gain[2];
  geant ftdl[2];// TDCT(FTrig)_hit delay wrt TDCA_hit delay (ns)
  geant aip[2][3]={
    {50.,62.6,1.3},
    {50.,62.6,1.3}
  }; 
// (def.param. for anode integrator(shft,t0(qthr=exp(t0/shft)),qoffs))
//
  if(AMSJob::gethead()->isMCData()){ //            =====> For MC data:
    for(i=0;i<MAXANTI;i++){
      tthr[0]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      tthr[1]=ANTIRECFFKEY.dtthr; // take trig. threshold from data card for now
      athr[0]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      athr[1]=ANTIRECFFKEY.dathr; // take TovT threshold from data card for now
      mip2q=ANTIMCFFKEY.MeV2PhEl; // (pe/mev)
      gain[0]=1.; // tempor
      gain[1]=1.;
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
      mip2q=1./ANTIRECFFKEY.PhEl2MeV; // (pe/mev)
      gain[0]=1.; // tempor
      gain[1]=1.;
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
  printf(" RawEvent-validation OK  : % 6d\n",recount[1]);
  printf(" RawEvent->RawCluster OK : % 6d\n",recount[2]);
  printf(" RawCluster->Cluster OK  : % 6d\n",recount[3]);
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
