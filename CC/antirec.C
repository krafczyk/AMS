//
// May 27, 1997 "zero" version by V.Choutko
// June 9, 1997 E.Choumilov: 'siantidigi' replaced by
//                           (RawEvent + validate + RawCluster) + trpattern;
//                           complete DAQ-section added; Cluster modified 
//
#include <typedefs.h>
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
#include <antidbc.h>
#include <daqblock.h>
#include <antirec.h>
#include <ntuple.h>
//
extern ANTIPcal antisccal[MAXANTI];
//
 integer AMSAntiRawEvent::trpatt=0;
 integer AMSAntiRawCluster::_trpatt=0;
//----------------------------------------------------
void AMSAntiRawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u ntdca[2],tdca1[ANAHMX*2],tdca2[ANAHMX*2];
  int16u ntdct[2],tdct1[ANAHMX*2],tdct2[ANAHMX*2];
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idN,stat[2];
  integer sector,isid,isds;
  integer i,j,im,nhit,chnum,am[2],sta,st;
  int bad;
  AMSAntiRawEvent *ptr;
  AMSAntiRawEvent *ptrN;
//
  pbitn=SCPHBP;// as for TOF
  pbanti=pbitn-1;
  status=1;//bad
  bad=1;// means no good atdc sequences
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(AMSAntiRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSAntiRawEvent",0,1);//last 1 to sort
  isds=0;
  if(ptr>0){
//
#ifdef __AMSDEBUG__
  if(ANTIRECFFKEY.reprtf[1]>=1)
  cout<<endl<<"=======> Anti::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//                             <---- loop over ANTI RawEvent hits -----
  while(ptr){
#ifdef __AMSDEBUG__
    if(ANTIRECFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    id=ptr->getid();// BBS
    sector=id/10-1;//0-15
    isid=id%10-1;//0-1 (top/bot)
    chnum=sector*2+isid;//channels numbering
    stat[isid]=ptr->getstat();
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      ANTIJobStat::addch(chnum,5);
//---> check A-TDC :
      ntdca[isid]=ptr->gettdca(tdca1);
      nhit=0;
      im=ntdca[isid];
      for(i=0;i<im;i++)tdca2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(tdca1[i]&pbitn);//check p-bit of down-edge (come first)
        pbup=(tdca1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOFDBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        tdca2[nhit]=tdca1[i];
        nhit+=1;
        tdca2[nhit]=tdca1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good hits
      }
      if(nhit<im){//something was wrong (bad sequence)
        ptr->puttdca(int16u(nhit),tdca2);// refill object with corrected data
        ANTIJobStat::addch(chnum,6);
      }
      if(nhit>0)bad=0;//at least one good sequence
//---> check FT-TDC :
      ntdct[isid]=ptr->gettdct(tdct1);
      nhit=0;
      im=ntdct[isid];
      for(i=0;i<im;i++)tdct2[i]=0;
      for(i=0;i<im;i++){// find correct(up) edges(FT-channel has only up-edges)
        pbup=(tdct1[i]&pbitn);//check p-bit of current edge
        if(TOFDBc::pbonup()==1){
          if(pbup==0)continue;//wrong(down) edge, take next one
        }
        else{
          if(pbup!=0)continue;//wrong(up) edge, take next one
        }
        tdct2[nhit]=tdct1[i];
        nhit+=1;
      }
      if(nhit<im){//something was wrong (bad sequence)
        ptr->puttdct(int16u(nhit),tdct2);// refill object with corrected data
        ANTIJobStat::addch(chnum,7);
      }
//-----      
    } 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//  ---- end of RawEvent hits loop ------->
//
  }// end of ptr>0 check
//
  if(bad==0)status=0;//good anti-event(was good tdca sequence)
}
//----------------------------------------------------
void AMSAntiRawEvent::mc_build(int &stat){
  geant up(0),down(0),slope[2],athr[2],tau,q2pe;
  geant eup(0),edown(0),tup(0),tdown(0);
  geant thresh[2];
  integer i,j,nup,ndown,sector,ierr,trflag(0),it,sta[2];
  integer sbt,trpatt,lsbit,hcount[4];
  int16u phbit,maxv,id,chsta,ntdca,tdca[ANAHMX],ntdct,tdct[ANAHMX],itt;
  number edep,ede,edept(0),time,z,c0,c1,tlev1,ftrig,t1,t2,dt,tovt;
  static number esignal[2][MAXANTI];
  static number tsignal[2][MAXANTI];
  VZERO(esignal,2*MAXANTI*sizeof(esignal[0][0])/sizeof(geant));
  VZERO(tsignal,2*MAXANTI*sizeof(tsignal[0][0])/sizeof(geant));
  AMSAntiMCCluster * ptr;
//
  trpatt=0;
  stat=1;//bad
  trflag=AMSTOFRawEvent::gettrfl();
  AMSAntiRawEvent::setpatt(trpatt);// reset trigger-pattern in AMSAntiRawEvent::
  if(trflag<=0)return;// because needs FTrigger abs. time
  stat=0;// ok
  ftrig=AMSTOFRawEvent::gettrtime();// FTrigger abs.time (ns)(incl. fixed delay)
  tlev1=ftrig+TOFDBc::accdel();// Lev-1 accept-signal abs.time
  c0=exp(-ANTIMCFFKEY.PMulZPos/ANTIMCFFKEY.LZero);
  c1=0.5*ANTIDBc::scleng();
  phbit=SCPHBP;// phase bit position as for TOF !!!
  maxv=phbit-1;// max. number
  ptr=(AMSAntiMCCluster*)AMSEvent::gethead()->
               getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
//
  edept=0.;
  while(ptr){ // <--- geant-hits loop:
    sector=ptr->getid();
    ede=ptr->getedep()*1000;
    edept+=ede;
    edep=ede*antisccal[sector-1].getmip();// Edep Mev -> p.e.
    z=ptr->getcoo(2);
    time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
    eup=0.5*edep*exp(z/ANTIMCFFKEY.LZero);// one side signal(pe)
    edown=0.5*edep*exp(-z/ANTIMCFFKEY.LZero);
    up+=eup;
    down+=edown;
    tup+=(time+(c1-z)/ANTIMCFFKEY.LSpeed)*eup;
    tdown+=(time+(c1+z)/ANTIMCFFKEY.LSpeed)*edown;
// 
    if(ptr->testlast()){// next or last id (sector)
      POISSN(up,nup,ierr); 
      POISSN(down,ndown,ierr); 
#ifdef __AMSDEBUG__
      assert(sector >0 && sector <= MAXANTI);
#endif
      esignal[0][sector-1]=nup; // save number of p.e.'s in buffer
      esignal[1][sector-1]=ndown;
      if(up>0)tsignal[0][sector-1]=tup/up; // averaged up_side time (ns)
      if(down>0)tsignal[1][sector-1]=tdown/down; // averaged down_side time (ns)
      up=0;
      down=0;
      tup=0.;
      tdown=0.;
    } 
   ptr=ptr->next();  
  } // ---> end of geant-hits loop
//
  if(ANTIMCFFKEY.mcprtf)HF1(2000,edept,1.);
//-----------------
// create Anti-trigger pattern:
//                         
  for(i=0;i<4;i++)hcount[i]=0;
  trpatt=0;
  for(i=0;i<MAXANTI;i++){
    up=esignal[0][i];
    down=esignal[1][i];
    antisccal[i].getstat(sta);// =0/1-> alive/dead
    antisccal[i].gettthr(thresh);
    if((i<4)||(i>11)){ // <-- x>0
      if(up>thresh[1] && sta[0]==0)hcount[3]+=1; // z>0
      if(down>thresh[0] && sta[1]==0)hcount[2]+=1; // z<0
    }
    else{              // <-- x<0
      if(up>thresh[1] && sta[0]==0)hcount[1]+=1; // z>0
      if(down>thresh[0] && sta[1]==0)hcount[0]+=1; // z<0
    }
  }
//
  for(i=0;i<4;i++){
    if(hcount[i]>3)hcount[i]=3;
    trpatt|=hcount[i]<<(16+i*2);
  }
  trpatt|=(min(hcount[0],hcount[1])+min(hcount[2],hcount[3]));
//
  AMSAntiRawEvent::setpatt(trpatt);// add trigger-pattern to AMSAntiRawEvent::
//----------------
// convert p.e.->TovT(ns)->TovT(TDCchan):
//
  tau=ANTIDBc::shprdt();
  for (j=0;j<MAXANTI;j++){ // <--- sector loop
    antisccal[j].gettthr(athr);// threshold in TovT(p.e.!!!)
    q2pe=antisccal[j].getq2pe();//Q->pe conv.factor (pe/pC)
    for (i=0;i<2;i++){ // <--- top/bot loop
      edep=esignal[i][j]; // p.e.
      time=tsignal[i][j]; // ns
      tovt=0;
      ede=edep/q2pe;// convert signal pe->pC 
      antisccal[j].q2t2q(0,i,tovt,ede);// Q(pC)->TovT(ns)(threshold is applied automatically !)
      if(tovt>0){ // non-zero hit -> create pulse/TDCA/TDCT
        t1=time;// TovT-pulse up-edge (ns) abs.time
        t2=time+tovt;// TovT-pulse down-edge (ns) abs.time
//
        dt=tlev1-t2;// first take second(down)-edge of pulse(LIFO mode of readout !!!)
        it=integer(dt/ANTIDBc::tdcabw());
        if(it>maxv){
          cout<<"ANTIRawEvent_mc: warning : down-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdca[0]=itt;
//
        dt=tlev1-t1;// now take first(up)-edge 
        it=integer(dt/ANTIDBc::tdcabw());
        if(it>maxv){
          cout<<"ANTIRawEvent_mc: warning : up-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdca[1]=itt;
//--------
        tovt=ANTIDBc::ftpulw();// FTrigger pulse width in ns
        t1=ftrig;// FT-pulse up-edge (ns) abs.time
//        t2=t1+tovt;// FT-pulse down-edge (ns) abs.time
//
//        dt=tlev1-t2;// first take second(down)-edge of pulse(LIFO mode of readout !!!)
//        it=integer(dt/ANTIDBc::tdcabw());
//        if(it>maxv){
//          cout<<"ANTIRawEvent_mc: warning : FT-down-time out of range"<<endl;
//          it=maxv;
//        }
//        itt=int16u(it);
//        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
//        tdct[0]=itt;
//
        dt=tlev1-t1;// now take first(up)-edge (only 1 edge for FT in SFEA !!!)
        it=integer(dt/ANTIDBc::tdcabw());
        if(it>maxv){
          cout<<"ANTIRawEvent_mc: warning : FT-up-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
//        tdct[1]=itt;
        tdct[0]=itt;
//--------
        ntdca=2;
//        ntdct=2;
        ntdct=1;
        id=int16u(10*(j+1)+i+1);//BBS (Bar/Side)
        chsta=0;// good
        AMSEvent::gethead()->addnext(AMSID("AMSAntiRawEvent",0),
                     new AMSAntiRawEvent(id,chsta,ntdca,tdca,ntdct,tdct));
      }// ---> end of TovT>0 check
    }// ---> end of top/bot loop
  }// ---> end of sector loop
}
//---------------------------------------------------
void AMSAntiRawCluster::build(int &status){
//(this is still 1-sided object)
  int16u ntdca,tdca[ANAHMX*2],ntdct,tdct[ANAHMX*2];
  int16u id,idN,sta;
  int statdb[2];
  int16u pbitn;
  int16u pbanti;
  integer i,j,jmax,sector,isid,isds,stat,chnum;
  number fttm,atm,dt,signal,qs;
  geant ftdel[2],athr[2],slope[2],twin,tau,q2pe;
  AMSAntiRawEvent *ptr;
//
  ptr=(AMSAntiRawEvent*)AMSEvent::gethead()
                                    ->getheadC("AMSAntiRawEvent",0);
  pbitn=SCPHBP;//phase bit position as for TOF
  pbanti=pbitn-1;// mask to avoid it.
  status=0;// good
  twin=ANTIRECFFKEY.ftwin;
  tau=ANTIDBc::shprdt();
  isds=0;
  ntdca=0;
  ntdct=0;
  while(ptr){ // <--- RawEvent hits loop
    id=ptr->getid();
    sector=id/10-1;
    isid=id%10-1;
    sta=ptr->getstat();
    chnum=sector*2+isid;//channels numbering
    antisccal[sector].getstat(statdb); // "alive" status from DB
    antisccal[sector].getftd(ftdel); // FTrig delay wrt ADCA-hit
    q2pe=antisccal[sector].getq2pe();// q(pC)->pe conv.factor
    if(statdb[isid] == 0){  // channel alive
//channel statistics :
      ANTIJobStat::addch(chnum,0); 
      ntdca=ptr->gettdca(tdca);
      if(ntdca>0)ANTIJobStat::addch(chnum,1);
      if(ntdca==2)ANTIJobStat::addch(chnum,2);
//
      ntdct=ptr->gettdct(tdct);
      if(ntdct>0)ANTIJobStat::addch(chnum,3);
      if(ntdct==1)ANTIJobStat::addch(chnum,4);
//--------
      signal=0.;
//<--- Use first (last on TDC input) FT-single_hit(1-edge signal !!!) as true hit 
      fttm=(pbanti&tdct[0])*ANTIDBc::tdcabw();//FT-up_egde(ns)(from 1st single-hit))
      jmax=ntdca/2;// dbl_hits
      for(j=0;j<jmax;j++){ // <--- TDCA-dbl_hit loop
        atm=(pbanti&tdca[2*j+1])*ANTIDBc::tdcabw();//TDCA-up_egde(ns)(2nd in readout)
        dt=atm-fttm;//should be >0 due to "common stop" mode
        if(ANTIRECFFKEY.reprtf[0])HF1(2501,geant(dt),1.);
        if(fabs(dt-ftdel[isid])<twin){//correlated FT and ADCA hits -> sum energy(p.e.)
          dt=((pbanti&tdca[2*j+1])-(pbanti&tdca[2*j]))*ANTIDBc::tdcabw();// TovT in ns
          antisccal[sector].q2t2q(1,isid,dt,qs);// TovT->q(pC)
          signal+=qs*q2pe;//  summing Edep in p.e.
        }
      }// ---> end of TDCA-hit loop
// create AntiRawCluster object :
      stat=integer(sta); 
      if(signal>0)AMSEvent::gethead()->addnext(AMSID("AMSAntiRawCluster",isid),
                                new AMSAntiRawCluster(stat,sector+1,isid,signal));
//    
    }
//
    ptr=ptr->next();// take next RawEvent hit
  }// ---> end of RawEvent hits loop
}
//---------------------------------------------------
void AMSAntiRawCluster::siantidigi(){
  integer sbt,trpatt(0),lsbit(1);
  number upam,downam;
  integer sta[2];
  geant thresh[2];
  static number counter[2][MAXANTI];
  VZERO(counter,2*MAXANTI*sizeof(counter[0][0])/sizeof(geant));
  AMSAntiMCCluster * ptr;
//
  ptr=(AMSAntiMCCluster*)AMSEvent::gethead()->
   getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
   geant up=0;
   geant down=0;
   number c0=exp(-ANTIMCFFKEY.PMulZPos/ANTIMCFFKEY.LZero);
  while(ptr){
   integer sector=ptr->getid();
   number edep=ptr->getedep()*1000.*antisccal[sector-1].getmip();// Edep Mev -> p.e.
   number z=ptr->getcoo(2);
   up+=edep*c0*exp(z/ANTIMCFFKEY.LZero);
   down+=edep*exp(-z/ANTIMCFFKEY.LZero)*c0;
   if(ptr->testlast()){
    integer nup,ndown,ierr;
    POISSN(up,nup,ierr); 
    POISSN(down,ndown,ierr); 
    up=0;
    down=0;
    #ifdef __AMSDEBUG__
      assert(sector >0 && sector <= MAXANTI);
    #endif
    counter[0][sector-1]=nup;
    counter[1][sector-1]=ndown;
   } 
 
   ptr=ptr->next();  
  }
   siantinoise(counter[0],counter[1],MAXANTI);

  int i,j;
  for (i=0;i<2;i++){
    for (j=0;j<MAXANTI;j++){ 
     if(counter[i][j] > 0)
     AMSEvent::gethead()->addnext(AMSID("AMSAntiRawCluster",i),
     new AMSAntiRawCluster(0,j+1,i,counter[i][j]));
     
    }
  }
// create trigger pattern:
  for(i=0;i<MAXANTI;i++){
    upam=counter[0][i];
    downam=counter[1][i];
    antisccal[i].getstat(sta);
    antisccal[i].gettthr(thresh);
    if((upam>thresh[1] && sta[0]==0) || (downam>thresh[0] && sta[1]==0)){// require OR of both ends
      sbt=lsbit<<i;
      trpatt|=sbt;
    }
  }
  AMSAntiRawCluster::setpatt(trpatt);// add trigger-pattern to AMSAntiRawCluster::
}

void AMSAntiRawCluster::siantinoise(number counter1[], number counter2[], integer nm){
 int i,j;
   for(j=0;j<nm;j++){
    integer ns=0;
    integer ierr=0;
    POISSN(ANTIMCFFKEY.SigmaPed,ns,ierr);
    counter1[j]=counter1[j]+ns-ANTIMCFFKEY.SigmaPed;
    POISSN(ANTIMCFFKEY.SigmaPed,ns,ierr);
    counter2[j]=counter2[j]+ns-ANTIMCFFKEY.SigmaPed;
   } 
}


void AMSAntiRawCluster::_writeEl(){
  AntiRawClusterNtuple* TN=AMSJob::gethead()->getntuple()->Get_antiraw();

  if (TN->Nantiraw>=MAXANTIRAW) return;

// fill the ntuple

  if(AMSAntiRawCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Nantiraw]=_status;
    TN->Sector[TN->Nantiraw]=_sector;
    TN->UpDown[TN->Nantiraw]=_updown;
    TN->Signal[TN->Nantiraw]=_signal;
    TN->Nantiraw++;
  }
}

void AMSAntiRawCluster::_copyEl(){
}


void AMSAntiRawCluster::_printEl(ostream & stream){
  stream <<"AMSAntiRawCluster "<<_sector<<" "<<_updown<<" "<<_signal<<endl;
}



void AMSAntiCluster::_writeEl(){
  AntiClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_anti();

  if (TN->Nanti>=MAXANTICL) return;

// fill the ntuple
  if(AMSAntiCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
    TN->Status[TN->Nanti]=_status;
    TN->Sector[TN->Nanti]=_sector;
    TN->Edep[TN->Nanti]=_edep;
    int i;
    for(i=0;i<3;i++)TN->Coo[TN->Nanti][i]=_coo[i];
    for(i=0;i<3;i++)TN->ErrorCoo[TN->Nanti][i]=_ecoo[i];
    TN->Nanti++;
  }
}



void AMSAntiCluster::_copyEl(){
}


void AMSAntiCluster::_printEl(ostream & stream){
  stream <<"AMSAntiCluster "<<_status<<" "<<_sector<<" "<<_edep<<" "<<_coo<<  " "<<_ecoo<<endl;
}


 
void AMSAntiCluster::print(){
AMSContainer *p =AMSEvent::gethead()->getC("AMSAntiCluster",0);
 if(p)p->printC(cout);
}

//===========================================================================



void AMSAntiCluster::build(){
    static number counter[2][MAXANTI];
    number edept(0);
    VZERO(counter,2*MAXANTI*sizeof(counter[0][0])/sizeof(geant));
    int i,j;
    for(i=0;i<2;i++){ 
     AMSAntiRawCluster *ptr=(AMSAntiRawCluster*)AMSEvent::gethead()->
     getheadC("AMSAntiRawCluster",i);
     while(ptr){
      integer sector=ptr->getsector()-1;
      #ifdef __AMSDEBUG__
       assert(sector >=0 && sector < MAXANTI);
      #endif
      counter[i][sector]=counter[i][sector]+ptr->getsignal();
      ptr=ptr->next();
     }
    }
    for (j=0;j<MAXANTI;j++){
     number sup=counter[0][j];
     number sdown=counter[1][j];
     if(sup>0 || sdown>0)ANTIJobStat::addbr(j,0);// have signal on any side
//
     if(sup+sdown > ANTIRECFFKEY.ThrS){
      ANTIJobStat::addbr(j,1);// total signal above the threshold
      AMSgvolume *pg=AMSJob::gethead()->getgeomvolume(AMSID("ANTS",j+1));
      #ifdef __AMSDEBUG__
       assert (pg != NULL);
      #endif     
      //
      // Geometry dependent
      // Should be changed if not "TUBS"
      //
      number par[5];
      for(i=0;i<5;i++)par[i]=pg->getpar(i);
      number r=(par[0]+par[1])/2;
      number er=(par[1]-par[0])/2;
      number phi=(par[3]+par[4])/2;
      number ephi=(par[4]-par[3])/2;
      number delta=(sup-sdown)/(sup+sdown);
      number z;
      if(delta > -1. && delta < 1.)z=ANTIMCFFKEY.LZero/2*log((1.+delta)/(1.-delta));
      else if (delta >=1)z=par[2];
      else z=-par[2];
      number edep=(sup+sdown)/antisccal[j].getmip()  // p.e.->Mev
      *2/(exp(z/ANTIMCFFKEY.LZero)+exp(-z/ANTIMCFFKEY.LZero));// useless(low z-accur)
      edept+=edep;
      number ddelta=1/sqrt(sup+sdown);
      number z1,z2;
      number d1=delta+ddelta;
      if(d1 >=1)z1=par[2];      
      else if(d1 <=-1)z1=-par[2];      
      else z1=ANTIMCFFKEY.LZero/2*log((1.+d1)/(1.-d1));
      number d2=delta-ddelta;
      if(d2 <=-1)z2=-par[2];      
      else if(d2 >=1)z2=par[2];      
      else z2=ANTIMCFFKEY.LZero/2*log((1.+d2)/(1.-d2));
      number ez=fabs(z1-z2)/2;
      AMSPoint coo(r,phi,z);
      AMSPoint ecoo(er,ephi,ez);
      integer status=0;
      if (sup ==0 || sdown == 0)status=AMSDBc::BAD;
      AMSEvent::gethead()->addnext(AMSID("AMSAntiCluster",0),
      new     AMSAntiCluster(status,j+1,edep,coo,ecoo));

      
     }
    }
    if(ANTIRECFFKEY.reprtf[0])HF1(2500,geant(edept),1.);
}





integer AMSAntiCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSAntiCluster",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}

integer AMSAntiRawCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=1;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSAntiRawCluster",AMSJob::gethead()->gettriggerC(n))==0){
        WriteAll=1;
        break;
      }
    }
  }
  return (WriteAll || status);
}

//===================================================================================
//  DAQ-interface functions :
//
// function returns number of Anti_data-words ( incl. FT-hits)
// for given block/format (one(max) SFEA card/crate is implied !!!)
//
integer AMSAntiRawEvent::calcdaqlength(int16u blid){
  AMSAntiRawEvent *ptr;
  int16u id,ibar,isid;
  int16u crate,rcrat,antic,hwid,fmt;
  integer len(0),nhits(0),nzchn(0),cntw;
  ptr=(AMSAntiRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSAntiRawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
//
  if((2&(DAQSBlock::getdmsk(rcrat))) > 0){ // AntiSubdet. is in this crate 
  while(ptr){
    id=ptr->getid();// BBS
    ibar=id/10-1;
    isid=id%10-1;
    hwid=AMSAntiRawEvent::sw2hwid(ibar,isid);// CAA, always >0 here
    antic=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      nhits+=ptr->getntdca();// counts hits (edges) of TDCA
      nzchn+=ptr->getnzchn();// counts nonzero TDC-channels (only TDCA)
    } 
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
    len+=1; // hit_bitmask
    if(nhits>0){// nonempty SFEA
      nzchn+=2;// add 2 TDCT(=FT) channels
      nhits+=(2*1);// add 2x1 TDCT-hits(for MC each FT-channel contains 1 hit(edge))
      cntw=nzchn/4;
      if(nzchn%4 > 0)cntw+=1;// hit-counter words
      len+=(cntw+nhits);
    } 
  }
//
  else{ // =====> Raw format :
    if(nhits>0){
      nhits+=(2*1);// add 2x1 TDCT-hits(for MC each FT-channel contains 1 hit(edge))
      len=2*nhits;// each hit(edge) require 2 words (header+TDCvalue)
    }
  }// end of format check
//
  }// end of SFEA-presence test
//
  return len;
//
}
//-------------------------------------------------------------------------
void AMSAntiRawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_ANTI_data
// on output: len=ANTI_data_length; 
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nanti;
  int16u ibar,isid,id;
  int16u phbit,maxv,ntdc,tdc[ANAHMX*2],ntdct,tdct[ANAHMX*2];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,tdcc,chip,tdccm;
  AMSAntiRawEvent *ptr;
  AMSAntiRawEvent *ptlist[ANCHSF];
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
  len=0;
  if(2&DAQSBlock::getdmsk(rcrat) == 0)return;//no AntiSubdet. in this crate (len=0)
//
#ifdef __AMSDEBUG__
  if(ANTIRECFFKEY.reprtf[1]==2)cout<<"Anti::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
// ---> prepare ptlist[tdc_channel] - list of AntiRawEvent pointers :
//
  ptr=(AMSAntiRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSAntiRawEvent",0);
  for(tdcc=0;tdcc<ANCHSF;tdcc++)ptlist[tdcc]=0;// clear pointer array
  nanti=0;
  while(ptr){
    id=ptr->getid();// BBS
    ibar=id/10-1;
    isid=id%10-1;
    hwid=AMSAntiRawEvent::sw2hwid(ibar,isid);// CAA, always >0 here (if valid ibar/isid) 
    tdcc=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      if(ptr->getnzchn())nanti+=1;
      ptlist[tdcc]=ptr;// store pointer
      ntdct=ptr->gettdct(tdct);// get FT-tdc (in MC these values are the same for all objects)
#ifdef __AMSDEBUG__
      if(ANTIRECFFKEY.reprtf[1]==2)ptr->_printEl(cout);
#endif
    }
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
//
// ---> encoding of tdc-data :
//
    ic=0;
    htmsk=0;// tdc bitmask word
    ichm=ic;// bias to bitmask word (where nonzero tdc-channels bits are set)
    ic+=1;
    if(nanti>0){ // nonempty SFEA
      shft=0;
      nzch=0;//counts all nonzero TDC-channels (incl. FT)
      nanti=0;// counts only Anti TDC-channels
      for(tdcc=0;tdcc<ANCHSF;tdcc++){// <--- TDC-channels loop (16) (some of them are FT)
        mskb=1<<tdcc;
        ntdc=0;// hits(edges) in given tdcc
        mtyp=1; // measurement type (1-> TDCA)
        if(tdcc==7 || tdcc==15){  //these TDC-channels contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =1 in MC
          for(i=0;i<ntdct;i++)tdc[i]=tdct[i];// FT measurement
        }
        ptr=ptlist[tdcc];// =0 for FT (sw2hwid-function should provide that)
        if(ptr>0)ntdc=ptr->gettdca(tdc);// TDCA measurement
        if(ntdc>0){
          htmsk|=mskb;
          shft=nzch%4;// hit-counter position in the hit-counters word (0-3)
          if(shft==0){
            hitc=0;
            ichc=ic;// bias for hit-counter
            ic+=1;
          }
          for(i=0;i<ntdc;i++)*(p+ic++)=tdc[i];//fill TDC-hit words
          hitc|=(ntdc-1)<<(shft*4);
          if(shft==3){// it was last hit-counter in current hit-counters word
            *(p+ichc)=hitc;// add hit-counters word
            ichc=ic;// bias for next hit-counters word
          }
          nzch+=1;
          if(mtyp==1)nanti+=1;//counts only ANTI-edges (not FT)
        }
      } // end of TDC-channels loop
      if((shft<3) && (nzch>0))*(p+ichc)=hitc;// add uncompleted hit-counters word
    }// ---> end of nonempty SFEA check
//
    *(p+ichm)=htmsk;// add SFEA-channels bitmask word
//
//
  } // ---> end of "Reduced" format
//---------------
  else{ // =====> Raw format :
//
//
// ---> encoding of tdc-data :
//
    ic=0;
    if(nanti>0){ // nonempty SFEA check
      for(tdcc=0;tdcc<ANCHSF;tdcc++){// <--- TDC-channels loop (0-15)
        chip=tdcc/8;//0-1
        chipc=tdcc%8;//chann.inside chip (0-7)
        chc=((chipc&1)<<2);//convert chipc to reverse bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        ntdc=0;
        slad=DAQSBlock::sladdr(int16u(ANSLOT-1));//slot-addr(h/w addr of SFEA in crate)
        adrw=slad;// put slot-addr in address word
        adrw|=(chc<<12);// add channel number(inside of TDC-chip)
        mtyp=1;// measur.type (1->TDCA)
        if(chipc==7){  //this TDC-channel contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =1 in MC
          for(i=0;i<ntdct;i++)tdc[i]=tdct[i];// FT measurement to write-buffer
        }
        ptr=ptlist[tdcc];
        if(ptr>0)ntdc=ptr->gettdca(tdc);// ANTI-meas. to write-buffer
        if(ntdc>0){
          for(i=0;i<ntdc;i++){
            adr=adrw;
            if((tdc[i]&phbit)>0)adr|=phbtp;// add phase bit to address-word
            tdcw=tdc[i]&maxv;// put hit-value to tdc-word
            tdcw|=((1-chip)<<15);// add chip number (1-chip to be compartible with TOF) 
            *(p+ic++)=tdcw; // write hit-value
            *(p+ic++)=adr; // write hit-address
          }
        }
      } // end of TDC-channels loop in SFEA
//
    }// ---> end of nonempty SFEA check
//
  } // ---> end of "Raw" format
//
  len=ic;
//
#ifdef __AMSDEBUG__
  if(ANTIRECFFKEY.reprtf[1]==2){
    cout<<"AntiDAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
    cout<<dec<<endl<<endl;
  }
#endif
}
//------------------------------------------------------------------------------
// function to build Raw: 
//  
void AMSAntiRawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
// on input:  *p=pointer_to_beginning_of_block (to block-id word)
//            len=bias_to_first_Anti_data_word
// on output: len=Anti_data_length.
//
  integer i,j,jj,ic,ibar,isid,lentot,bias;
  integer val,warnfl;
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hwch,hmsk,slad,chip,chipc,chc;
  int16u swid[ANCHCH],mtyp,hcnt,shft,nhit,nzch,nzcch,sbit;
  int16u phbit,maxv,phbt,phbtp; 
  int16u ids,stat,chan;
  int16u ntdca[ANCHCH],tdca[ANCHCH][ANAHMX*2],ntdct,tdct[ANAHMX*2],dummy;
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
  lentot=*(p-1);// total length of the block(incl. block_id)
  bias=len;
//
// decoding of block-id :
//
  crate=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=1-(blid&63);// data_type ("0"->RawTDC; "1"->ReducedTDC)
#ifdef __AMSDEBUG__
  if(ANTIRECFFKEY.reprtf[1]>=1){
    cout<<"Anti::decoding: crate/format="<<crate<<" "<<dtyp<<"  bias="<<bias<<endl;
  }
#endif
//
  if(dtyp==1){ // =====> reduced TDC data decoding :
      ic=bias;//jamp to first data word (bias=1+previous_detectors_data_length) 
//
      hmsk=*(p+ic++);// SFEA nonzero-channels bit mask
      nzch=0;//nonzero TDCchannels in SFEA (upto 15)
      if(hmsk>0){
        for(chip=0;chip<2;chip++){ // <--- TDC-chip loop (0-1)
          nzcch=0;// nonzero anti-channels in chip
          ntdct=0;// nonzero FT-channels in chip (0,1)
          tdct[0]=0;
          for(tdcc=0;tdcc<8;tdcc++){ // <-- TDC-chan. in chip loop (0-7)
            hwch=8*chip+tdcc;// TDC-ch numbering inside SFEA (0-15)
            mtyp=0;
            if(tdcc<ANCHCH)mtyp=1;// measurement_type (1-TDCA)
            if(tdcc==7){  //this TDC-channels contain FT-hits
              mtyp=2;// means FT measurements
            }
//
            if( (hmsk & (1<<hwch)) > 0){// nonzero SFEA-channel processing:
              shft=nzch%4;
              if(shft==0){// take new counters_word
                hcnt=*(p+ic++);
              }
              nhit=((hcnt>>(4*shft))&15)+1;// numb.of hits for current mtyp
              for(i=0;i<nhit;i++){//    <-- Hit loop
                if(mtyp==0)dummy=*(p+ic++);// ignore nonAnti/nonFT hits (if any)
                if(mtyp==1)tdca[nzcch][i]=*(p+ic++);
                if(mtyp==2)tdct[i]=*(p+ic++);
              } //                      --> end of hit loop
              if(mtyp==1)ntdca[nzcch]=nhit;
              if(mtyp==2)ntdct=nhit;
              nzch+=1;//count !=0 TDC-ch inside of one SFEA
              if(mtyp==1){
                swid[nzcch]=AMSAntiRawEvent::hw2swid(crate,hwch);// BBS or 0
                nzcch+=1;//count !=0 Anti-TDC-ch inside of one TDC-chip
              }
            }
//
          }// ---> end of TDCchip channels loop
//
          if(nzcch>0){// <--- create AMSAntiRawEvent objects for nonempty TDC-chip
            for(i=0;i<nzcch;i++){// <--- loop over non-empty anti-channels
              ids=swid[i];//BBS
              if(ids>0){
                stat=0; // tempor 
                AMSEvent::gethead()->addnext(AMSID("AMSAntiRawEvent",0),
                new AMSAntiRawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
              }
              else{
           cerr<<"AntiDAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
              }
            }// ---> end of object creation loop
          }// ---> end of nonempty chip
//
        }//---> end of TDC-chip loop
      }//---> end of !=0 mask check
//
      len=ic-len;//return pure Anti_data_length
  }// end of reduced data decoding
//
//---------------
//
  else if(dtyp==0){ // =====> raw TDC data decoding :
//
    integer lent(0);
    int16u hits[ANCHSF][16];// array to store hit-values(upto 16) for each h/w channel
    integer nhits[ANCHSF];// number of hits in given h/w channel
    int16u tdcw,adrw,hitv;
//
    for(i=0;i<ANCHSF;i++){// clear buffer
      nhits[i]=0;
      for(j=0;j<16;j++)hits[i][j]=0;
    }
//
    ic=1;// for Raw format start from the beginning of block + 1
//    ic=bias;// tempor
    while(ic<lentot){ // <---  words loop
      tdcw=*(p+ic++);// chip# + tdc_value
      chip=(tdcw>>15);// TDC-chip number(TRUE,0-1)
      if(ic>=lentot){
        cout<<"ANTI:RawFmt:read_error: attempt to read Extra-word ic="<<ic<<" blocklength="<<lentot<<endl;
        break;   
      }
      adrw=*(p+ic++);// phbit + chipc + slotaddr.
      slad=adrw&15;// get SFEx h/w address(card-id) ((0,1,2,3)-TOF, (5)-C, (4)-A)
      sfet=DAQSBlock::slnumb(slad);// sequential slot number (0-5, or =DAQSSLT if slad invalid)) 
      if(sfet==DAQSSLT)continue; //---> invalid slad: try to take next pair
      if(DAQSBlock::isSFEA(slad)){ // SFEA data : write to buffer
        lent+=2;
        chipc=(adrw>>12)&7;// channel inside TDC-chip (0-7)(reverse bit pattern!!!)
        chc=((chipc&1)<<2);//convert chipc to normal bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        if((adrw & phbtp)>0)
                            phbt=1; // check/set phase-bit flag
        else
                            phbt=0;
        tdcc=8*(1-chip)+chc; // channel inside SFEA(0-15) (1-chip to be unified with TOF)
        hitv=(tdcw & maxv)|(phbt*phbit);// tdc-value with phase bit set as for RawEvent
        if(nhits[tdcc]<16){
          warnfl=0;
          hits[tdcc][nhits[tdcc]]=hitv;
          nhits[tdcc]+=1;
        }
        else{
          if(warnfl==0){
          cout<<"ANTI:RawFmt:read_warning: > 16 hits in channel: crate= "<<crate
          <<" sfet="<<sfet<<" chip="<<chip<<" chipch="<<chc<<endl;
          cout<<"event="<<(AMSEvent::gethead()->getid())<<endl;
          warnfl=1;
          }
        }
      }
//
      else{
        continue;// nonSFEA data, take next pair
      }// ---> end of SFEA data check
//
    }// ---> end of words loop
//-----------------------------
// Now extract ANTI+FT data from buffer:
//
    for(chip=0;chip<2;chip++){ // <--- TDC-chip loop (0-1)
      nzcch=0;// nonzero anti-channels in chip
      ntdct=0;// nonzero FT-channels in chip (0,1)
      tdct[0]=0;
      for(tdcc=0;tdcc<8;tdcc++){ // <-- TDC-chan. in chip loop (0-7)
        hwch=8*chip+tdcc; // channel inside SFEA(0-15)
        mtyp=0;
        if(tdcc<ANCHCH)mtyp=1;// measurement_type (1-TDCA)
        if(tdcc==7)mtyp=2; //this TDC-channels contain FT-hits (mtyp=2)
        nhit=nhits[hwch];
        if(nhit>0){
          for(i=0;i<nhit;i++){//    <-- Hit loop
            if(mtyp==0)dummy=hits[hwch][i];// ignore nonAnti/nonFT hits (if any)
            if(mtyp==1)tdca[nzcch][i]=hits[hwch][i];
            if(mtyp==2)tdct[i]=hits[hwch][i];
          } //                      --> end of hit loop
          ntdct=0;
          if(mtyp==1)ntdca[nzcch]=nhit;
          if(mtyp==2)ntdct=nhit;
          if(mtyp==1){
            swid[nzcch]=AMSAntiRawEvent::hw2swid(crate,hwch);// store BBS (or 0)
            nzcch+=1;//count !=0 Anti-TDC-ch inside of one TDC-chip
          }
        }
      }// ---> end of TDCchip channels loop
//
      if(nzcch){// <--- create AMSAntiRawEvent objects for nonempty TDC-chip
        for(i=0;i<nzcch;i++){// <--- loop over non-empty anti-channels
            ids=swid[i];//BBS
            if(ids>0){
              ids=swid[i];// BBS
              stat=0; // tempor 
              AMSEvent::gethead()->addnext(AMSID("AMSAntiRawEvent",0),
              new AMSAntiRawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
            }
            else{
           cerr<<"AntiDAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
            }
        }// ---> end of object creation loop
      }// ---> end of nonempty chip test
//
    }//---> end of TDC-chip loop
//
  len=lent;
  }// ---> end of Raw format decoding
//-------------------------
  else{
    cout<<"AntiDaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
#ifdef __AMSDEBUG__
  if(ANTIRECFFKEY.reprtf[1]>=1){
    cout<<"AntiRawEventBuild::decoding: FOUND "<<len<<" good words, hex/bit dump follows:"<<endl;
    int16u tstw,tstb;
    if(dtyp==1){ // only for red.data
      for(i=0;i<len;i++){
        tstw=*(p+i+bias);
        cout<<hex<<setw(4)<<tstw<<"  |"<<dec;
        for(j=0;j<16;j++){
          tstb=(1<<(15-j));
          if((tstw&tstb)!=0)cout<<"x"<<"|";
          else cout<<" "<<"|";
        }
        cout<<endl;
      }
      cout<<dec<<endl<<endl;
    }
  }
#endif
}
//------------------------------------------------------------------------------
//  function to get sofrware-id (BBS) for given hardware-channel (crate,antich):
//  (imply only one (max) SFEA card per crate)                    0-7   0-15
//
int16u AMSAntiRawEvent::hw2swid(int16u a1, int16u a2){
  int16u swid,hwch;
//
  static int16u sidlst[SCCRAT*ANCHSF]={// 14 BBS's + 2 FT's  per CRATE (per SFEA):
// mycrate-1(cr-01) = (1 SFEA)x(2x(4 ANTICs +FT)) :
    21, 41,161,151,  0,  0,  0,  0,141, 31,131, 11,  0,  0,  0,  0,
// mycrate-2 = (no SFEA card) :
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// mycrate-3(cr-41) = (1 SFEA)x(2x(4 ANTICs +FT)) :
    91, 81,101, 51,  0,  0,  0,  0, 61,121, 71,111,  0,  0,  0,  0,
// mycrate-4 = (no SFEA card) :
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// mycrate-5(cr-03) = (1 SFEA)x(2x(4 ANTICs +FT)) :
    42,162, 32,142,  0,  0,  0,  0,132, 12,152, 22,  0,  0,  0,  0, 
// mycrate-6 = (no SFEA card) :
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
// mycrate-7(cr-43) = (1 SFEA)x(2x(4 ANTICs +FT)) :
   112,102,122, 92,  0,  0,  0,  0, 72, 62, 52, 82,  0,  0,  0,  0,
// mycrate-8 = (no SFEA card) :
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<SCCRAT);//crate(0-7)
  assert(a2>=0 && a2<ANCHSF);//antich(0-15)
#endif
  hwch=int16u(ANCHSF*a1+a2);// hardware-channel
  swid=sidlst[hwch]; // software-id BBS (S=1->up(+z), =2->down(-z))
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;//tempor
  return swid;
}
//-----------------------------------------------------------------------
//  function to get hardware-id (CAA) for given software-chan (bar, side)
//                                                             0-15  0-1
//
int16u AMSAntiRawEvent::sw2hwid(int16u a1, int16u a2){
  static int16u first(0);
  integer i;
  int16u ibar,isid,swch,swid,hwid,crate,antic;
  static int16u hidlst[MAXANTI*2]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<MAXANTI);// bar(0-15)
  assert(a2>=0 && a2<2); // side(0-1)
#endif
//
  if(first==0){ // create hardw.id list:
    first=1;
    for(i=0;i<MAXANTI*2;i++)hidlst[i]=0;
    for(crate=0;crate<SCCRAT;crate++){
      for(antic=0;antic<ANCHSF;antic++){
        hwid=100*(crate+1)+antic+1;// CrateAntich (CAA)
        swid=AMSAntiRawEvent::hw2swid(crate,antic);// BBS
        if(swid>0 && swid<=162){// legal swid (FT's are =0 here)
          ibar=swid/10-1;
          isid=swid%10-1;
          swch=2*ibar+isid;
          hidlst[swch]=hwid;
        }
      }// end of antich-loop
    }// end of crate-loop
  }// end of first
// 
  swch=2*a1+a2;
  hwid=hidlst[swch];// hardware-id CST
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;
  return hwid;
}
