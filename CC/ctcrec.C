// E.Choumilov, 10.23.97, RawEvent/RawHit-build and DAQ-interface sections are added.
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <daqblock.h>
#include <tofdbc.h>
#include <tofsim.h>
#include <ctcdbc.h>
#include <ctcrec.h>
#include <ctcsim.h>
#include <ntuple.h>
//
extern CTCCCcal ctcfcal[CTCCCMX]; // CTC calibr.parameters
//
//----------------------------------------------------
void AMSCTCRawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u ntdca,tdca1[CTCEPMX*2],tdca2[CTCEPMX*2];
  int16u ntdct,tdct1[CTCEPMX*2],tdct2[CTCEPMX*2];
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idN,stat;
  integer i,j,im,nhit,chnum,am[2],sta,st;
  int bad(1);
  AMSCTCRawEvent *ptr;
//
  pbitn=SCPHBP;// as for TOF
  pbanti=pbitn-1;
  status=1;//bad
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(AMSCTCRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSCTCRawEvent",0);
  if(ptr>0){// presence of RawEvent
//
#ifdef __AMSDEBUG__
  if(CTCRECFFKEY.reprtf[1]>=1)
  cout<<endl<<"=======> CTC::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//                             <---- loop over CTC RawEvent hits -----
  while(ptr){
#ifdef __AMSDEBUG__
    if(CTCRECFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    id=ptr->getid();// comb.number CC (1-CTCCCMX)
    chnum=integer(id-1);//channels numbering
    stat=ptr->getstat();
    if(stat == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      CTCJobStat::addch(chnum,0);
//---> check A-TDC :
      ntdca=ptr->gettdca(tdca1);
      nhit=0;
      im=ntdca;
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
        CTCJobStat::addch(chnum,1);
      }
//---> check FT-TDC :
      ntdct=ptr->gettdct(tdct1);
      nhit=0;
      im=ntdct;
      for(i=0;i<im;i++)tdct2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(tdct1[i]&pbitn);//check p-bit of down-edge (come first)
        pbup=(tdct1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOFDBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        tdct2[nhit]=tdct1[i];
        nhit+=1;
        tdct2[nhit]=tdct1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good hits
      }
      if(nhit<im){//something was wrong (bad sequence)
        ptr->puttdct(int16u(nhit),tdct2);// refill object with corrected data
        CTCJobStat::addch(chnum,2);
      }
      if(nhit>0)bad=0;//at least one good TDCT-sequence
//-----      
    }// ---> end of status check 
//
//---------------
    ptr=ptr->next();// next RawEvent hit
  }//  ---- end of RawEvent hits loop ------->
//
  }// end of ptr>0 check (RawEvent presence)
//
  if(bad==0)status=0;//good CTC-event
}
//------------------------------------------------------------
void AMSCTCRawEvent::mc_build(int &stat){//to build from CTCRawHit
  static integer first(1);
  integer dbs,combn,gid,gidg,i,j,il,ix,iy,geoch,gchmx,nmemb,mblst[CTCCMMX],lmax;
  integer it,trflag(0);
  static integer geo2comn[2*CTCXDMX*CTCYDMX];//combination-number vs geo-channel
  number sig,tm,signal[CTCCCMX],timesig[CTCCCMX],sigtot(0);
  number t1,t2,dt,tlev1,ftrig,tovt;
  int16u phbit,maxv,id,chsta,ntdca,tdca[CTCEPMX],ntdct,tdct[CTCEPMX],itt;
  geant ftdel,q2pe,gain;
  AMSCTCRawHit *ptr;
//
//----------
  stat=1;//bad
  trflag=AMSTOFRawEvent::gettrfl();
  if(trflag<=0)return;// because CTC needs FTrigger abs. time from TOF
  stat=0;// ok
  ftrig=AMSTOFRawEvent::gettrtime();// FTrigger abs.time (ns)(incl. fixed delay)
  tlev1=ftrig+TOFDBc::accdel();// Lev-1 accept-signal abs.time
  phbit=SCPHBP;// phase bit position as for TOF !!!
  maxv=phbit-1;// max. number
  lmax=CTCDBc::getnlay();
  gchmx=lmax*CTCXDMX*CTCYDMX;
//--------
  if(first){ // create array geo2comn for geoch->comb.number transformation
    first=0;
    for(i=0;i<gchmx;i++)geo2comn[i]=0;
    for(i=0;i<CTCCCMX;i++){
      nmemb=ctcfcal[i].getmembers(mblst);
      for(j=0;j<nmemb;j++){
        gid=mblst[j];       //LXXY=LCCR
        il=gid/1000-1;      // layer (0-1)
        ix=((gid%1000)/10)-1; // cell number in X
        iy=((gid%1000)%10)-1; //  ..........    Y
        geoch=il*CTCXDMX*CTCYDMX+ix*CTCYDMX+iy;
        geo2comn[geoch]=i;
      }
    }
  }
//---------
  for(i=0;i<CTCCCMX;i++){
    signal[i]=0;
    timesig[i]=0;
  }
//--------> fill signal/time buffers :
//
  for(il=0;il<lmax;il++){ // <--- loop over layer
    ptr=(AMSCTCRawHit*)AMSEvent::gethead()->getheadC("AMSCTCRawHit",il);
    while (ptr){ // <--- loop over RawHits in layer
      ix=(ptr->getcolno())-1;
      iy=(ptr->getrowno())-1;
      geoch=il*CTCXDMX*CTCYDMX+CTCYDMX*ix+iy;
      gidg=1000*(il+1)+10*(ix+1)+iy+1;//LXXY=LCCR
#ifdef __AMSDEBUG__
      assert(geoch >=0 && geoch < gchmx);
#endif
      combn=geo2comn[geoch];// CC-1 !!!
#ifdef __AMSDEBUG__
      assert(combn >=0 && combn < CTCCCMX);
#endif
      nmemb=ctcfcal[combn].getmembers(mblst);
      dbs=1;// status fr. DB (0/1->alive/not)
      for(j=0;j<nmemb;j++){
        gid=mblst[j];       //LXXY=LCCR
        if(gid==gidg){
          dbs=ctcfcal[combn].getmstat(j);// memb.status from DB
          gain=ctcfcal[combn].getgain(j);// memb.gain from DB
        }
      }
      if(dbs==0){// sum signals/times for alive cells (geo-channels)
        sig=ptr->getsignal();//signal in p.e.
        tm=(1.e+9)*(ptr->gettime());// time in ns
        signal[combn]+=(sig*gain);// sum gain-"incorrected" signal
        timesig[combn]+=tm*sig;
      }
      ptr=ptr->next();
    }
  }
//-------> create RawEvent objects :
//
  for(i=0;i<CTCCCMX;i++){ // <--- loop over combination numbers (buffer)
    sig=signal[i];// p.e.
    sigtot+=sig;
//--------  digitize anode-signal :
      tm=0;
      if(sig>0)tm=timesig[i]/sig; // signal-weighted time (ns)
      q2pe=ctcfcal[i].getq2pe();//Q->pe conv.factor (pe/pC)
      sig/=q2pe;// convert signal p.e.->pC 
      ctcfcal[i].q2t2q(0,tovt,sig);// Q(pC)->TovT(ns)(thresh. is applied automatically !)
      if(tovt>0){// nonzero TovT: create pulse/TDCA/TDCT
        t1=tm;// TovT-pulse up-edge (ns) abs.time
        t2=tm+tovt;// TovT-pulse down-edge (ns) abs.time
//
        dt=tlev1-t2;// first take second(down)-edge of pulse(LIFO mode of readout !!!)
        it=integer(dt/CTCDBc::tdcabw());
        if(it>maxv){
          cout<<"CTCRawEvent_mc: warning : TDCA-down-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdca[0]=itt;
//
        dt=tlev1-t1;// now take first(up)-edge 
        it=integer(dt/CTCDBc::tdcabw());
        if(it>maxv){
          cout<<"CTCRawEvent_mc: warning : TDCA-up-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdca[1]=itt;
//--------  digitize FT-signal :
        tovt=CTCDBc::ftpulw();// FTrigger pulse width in ns
        t1=ftrig;// FT-pulse up-edge (ns) abs.time
        t2=t1+tovt;// FT-pulse down-edge (ns) abs.time
//
        dt=tlev1-t2;// first take second(down)-edge of pulse(LIFO mode of readout !!!)
        it=integer(dt/CTCDBc::tdcabw());
        if(it>maxv){
          cout<<"CTCRawEvent_mc: warning : FT-down-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(!TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdct[0]=itt;
//
        dt=tlev1-t1;// now take first(up)-edge 
        it=integer(dt/CTCDBc::tdcabw());
        if(it>maxv){
          cout<<"CTCRawEvent_mc: warning : FT-up-time out of range"<<endl;
          it=maxv;
        }
        itt=int16u(it);
        if(TOFDBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        tdct[1]=itt;
//--------
        ntdca=2;
        ntdct=2;
        id=int16u(i+1);//comb.number
        chsta=0;// good
        AMSEvent::gethead()->addnext(AMSID("AMSCTCRawEvent",0),
                       new AMSCTCRawEvent(id,chsta,ntdca,tdca,ntdct,tdct));
      }//---> end of nonzero TovT test
  }// ---> end of comb.loop
//
  if(CTCMCFFKEY.mcprtf>0)HF1(3500,geant(sigtot),1.);
}
//----------------------------------------------------------------------
void AMSCTCRawHit::build(int &stat){// build from CTCRawEvent
  int16u id,ntdca,tdca[CTCEPMX*2],ntdct,tdct[CTCEPMX*2];
  int16u pbitn,pbanti;
  integer nmemb,mblist[CTCCMMX],mbstat[CTCCMMX],ngmem,status;
  integer jmax,chnum,i,j,gid,row,col,layer;
  number atm,fttm,dt,q,signal,msignal,totsig;
  geant twin,ftdel,q2pe,gain;
  AMSCTCRawEvent *ptr;
//
  ptr=(AMSCTCRawEvent*)AMSEvent::gethead()
                                    ->getheadC("AMSCTCRawEvent",0);
  pbitn=SCPHBP;//phase bit position as for TOF
  pbanti=pbitn-1;// mask to kill it.
  twin=CTCRECFFKEY.ftwin;
//
  stat=1;//bad
  ntdca=0;
  ntdct=0;
  totsig=0;
  while(ptr){ // <--- RawEvent hits(readout-channels) loop
    id=ptr->getid();// comb. number
    chnum=integer(id-1);
    nmemb=ctcfcal[chnum].getmembers(mblist);// memb.list (LXXY's) 
    for(i=0;i<nmemb;i++)mbstat[i]=ctcfcal[chnum].getmstat(i);// memb.status fr.DB
    ngmem=0;
    gain=0;
    for(i=0;i<nmemb;i++){
      if(mbstat[i]==0){ 
        ngmem+=1;// count alive members
        gain+=(ctcfcal[chnum].getgain(i));// add gains for averaging
      }
    }
    if(ngmem>0)gain=gain/geant(ngmem);// readout-channel average gain
    status=0;
    if(ngmem==0)status=1;//bad according to DB (all members are bad)
    if(status==1){
      ptr=ptr->next();// skip current and take next RawEvent hit
      continue;
    } 
//channel statistics :
    CTCJobStat::addch(chnum,4);
    ntdca=ptr->gettdca(tdca);
    if(ntdca>0)CTCJobStat::addch(chnum,5);
    if(ntdca==2)CTCJobStat::addch(chnum,6);
//
    ntdct=ptr->gettdct(tdct);
    if(ntdct>0)CTCJobStat::addch(chnum,7);
    if(ntdct==2)CTCJobStat::addch(chnum,8);
//
    ftdel=ctcfcal[chnum].getftdel();//FT-delay wrt anode signal
    q2pe=ctcfcal[chnum].getq2pe();//Q->pe conv.factor (pe/pC)
//--------
      signal=0.;
//<--- Use first (last on TDC input) FT-double_hit(2edges) as true hit 
      fttm=(pbanti&tdct[1])*CTCDBc::tdcabw();//FT-up_egde(ns)(got from 1st dbl-hit))
      jmax=ntdca/2;// pairs of edges
      for(j=0;j<jmax;j++){ // <--- TDCA-dbl_hit loop
        atm=(pbanti&tdca[2*j+1])*CTCDBc::tdcabw();//TDCA-up_egde(ns)(2nd in readout)
        dt=atm-fttm;//should be >0 due to "common stop" mode
        if(CTCRECFFKEY.reprtf[0])HF1(3001,geant(dt),1.);
        if(fabs(dt-ftdel)<twin){//correlated FT and ADCA hits -> sum energy(pC)
          dt=((pbanti&tdca[2*j+1])-(pbanti&tdca[2*j]))*CTCDBc::tdcabw();// TovT in ns
          ctcfcal[chnum].q2t2q(1,dt,q);// TovT(ns)->Q(pC)
          signal+=(q*q2pe);// Q(pC)-> Signal(pe) and sum up TDCA-hits (true)
        }
      }// ---> end of TDCA-hit loop
      totsig+=signal/gain;
//
// create CTCRawHit objects :
//
    if(ngmem>0 && signal>0){ // good comb.channel
      msignal=signal/number(ngmem)/gain;//gain-corrected signal per good member
      for(i=0;i<nmemb;i++){
        gid=mblist[i];//LXXY=LCCR
        layer=gid/1000;
        col=(gid%1000)/10;
        row=(gid%1000)%10;
        status=mbstat[i];
        if(status==0){
          stat=0;//event ok ( at least one object created)
          AMSEvent::gethead()->addnext(AMSID("AMSCTCRawHit",layer-1),
                        new AMSCTCRawHit(status,row,layer,col,msignal,0));
        }
//    
      }
    }// ---> end of good comb.channel check
//
    ptr=ptr->next();// take next RawEvent hit
  }// ---> end of RawEvent hits loop
  if(CTCRECFFKEY.reprtf[0]>0)HF1(3000,geant(totsig),1.);
}
//------------------------------------------------------------
void AMSCTCCluster::_writeEl(){

  CTCClusterNtuple* CTCCLN = AMSJob::gethead()->getntuple()->Get_ctccl();

  if (CTCCLN->Nctccl>=MAXCTCCL) return;
  
// Fill the ntuple
  if(AMSCTCCluster::Out( IOPA.WriteAll==1 ||  checkstatus(AMSDBc::USED))){
    CTCCLN->Status[CTCCLN->Nctccl]=_status; 
    CTCCLN->Layer[CTCCLN->Nctccl]=_Layer;
    int i;
    for(i=0;i<3;i++)CTCCLN->Coo[CTCCLN->Nctccl][i]=_Coo[i];
    for(i=0;i<3;i++)CTCCLN->ErrCoo[CTCCLN->Nctccl][i]=_ErrorCoo[i];
    CTCCLN->RawSignal[CTCCLN->Nctccl]=_Signal;
    CTCCLN->Signal[CTCCLN->Nctccl]=_CorrectedSignal;
    CTCCLN->ErrorSignal[CTCCLN->Nctccl]=_ErrorSignal;
    CTCCLN->Nctccl++;
  }

}
void AMSCTCCluster::_copyEl(){
}


void AMSCTCCluster::_printEl(ostream & stream){
  stream <<"AMSCTCCluster "<<_status<<" "<<_Layer<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<
 _Signal<<" " <<_ErrorSignal<<endl;
}


 
void AMSCTCCluster::print(){
int i;
for(i=0;;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCTCCluster",i);
 if(p)p->printC(cout);
 else break;
}
}



void AMSCTCCluster::build(){
if(CTCDBc::getgeom()<2){
  cerr << " Non Annecy setups are not supported any more "<<endl;
  exit(1);
}
else {
 for(integer kk=0;kk<CTCDBc::getnlay();kk++){

  AMSCTCRawHit *ptr=(AMSCTCRawHit*)AMSEvent::gethead()->
  getheadC("AMSCTCRawHit",kk);
  integer const maxpl=200;
  static number xplane[maxpl];
  static integer xstatus[maxpl];
  VZERO(xplane,maxpl*sizeof(number)/4);
  while (ptr){
    if(ptr->getlayno()==kk+1){
    integer plane=2*CTCDBc::getnx(kk+1)*(ptr->getrowno()-1)+ptr->getcolno()-1;
#ifdef __AMSDEBUG__
    assert(plane>=0 && plane < maxpl);
#endif
    xplane[plane]+=ptr->getsignal();
    xstatus[plane]+=ptr->getstatus();
    }
   ptr=ptr->next();
  }
  addnew(xstatus,xplane,maxpl,kk+1);
 }
}
}

void AMSCTCCluster::addnew(integer xstatus[],number xplane[],integer maxpl,integer layer){
number smax=0;
integer imax=-1;
for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > smax){
  smax=xplane[i];
  imax=i;
 }
}
if(imax >=0 && smax >0 && smax>=CTCRECFFKEY.Thr1){
  number edep=0;
  AMSPoint cofg(0,0,0);
  number xsize=1000;
  number ysize=1000;
  number zsize=1000;
  integer status=xstatus[imax];
  integer row=imax/CTCDBc::getnx(layer)/2+1;
  integer col=imax%(CTCDBc::getnx(layer)*2)+1;
  int i,j;
  for(i=-1;i<2;i++){ 
   for(j=-1;j<2;j++){
    integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
    if(col-1+j >=0 && col-1+j <CTCDBc::getnx(layer)*2 && row-1+i >=0 
    && row-1+i < CTCDBc::getny()*2){ 
    if(k>=0 && k<maxpl && xplane[k]>0){
     AMSCTCRawHit d(row+i,layer,col+j);
     AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
     if(p){
      cofg=cofg+p->loc2gl(AMSPoint(0,0,0))*xplane[k];
      edep+=xplane[k];
     }
     else cerr << "AMSCTCCluster::build-S-GeomVolumeNotFound "<<row-1+i<<" "
     <<col-1+j<<endl;
    }
   }  
   }
  }
  if(edep>0)cofg=cofg/edep;
  // GetTypical Error Size - 
  AMSCTCRawHit d(1,1,1);
  AMSgvolume *p0= AMSJob::gethead()->getgeomvolume(d.crgid(0));
  if(p0 ){
          xsize=p0->getpar(0);
          ysize=p0->getpar(1);
          zsize=p0->getpar(2)*2;
  }
  AMSPoint ecoo(xsize,ysize,zsize);
  if(edep>=TOFRECFFKEY.ThrS){
   AMSEvent::gethead()->addnext(AMSID("AMSCTCCluster",layer-1),
   new     AMSCTCCluster(status,layer,cofg,ecoo,edep,sqrt(edep)));
   for(i=-1;i<2;i++){ 
    for(j=-1;j<2;j++){
      integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
      if(k>=0 && k<maxpl && xplane[k]>0)xplane[k]=0;
    }
   }
   addnew(xstatus,xplane,maxpl,layer);
  }
}
}


integer AMSCTCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSCTCCluster",AMSJob::gethead()->gettriggerC(n))==0){
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
// function returns number of CTC_data-words ( incl. FT-hits)
// for given block/format (one(max) SFEC card/crate is implied !!!)
//
integer AMSCTCRawEvent::calcdaqlength(int16u blid){
  AMSCTCRawEvent *ptr;
  int16u id;
  int16u crate,rcrat,sfcc,hwid,fmt;
  integer len(0),nhits(0),nzchn(0),cntw;
  ptr=(AMSCTCRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSCTCRawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
//
  if((4&(DAQSBlock::getdmsk(rcrat))) > 0){ // CTCSubdet. is in this crate 
  while(ptr){
    id=ptr->getid();// CC (config.combination number)
    hwid=AMSCTCRawEvent::sw2hwid(id);//CAA, always>0 here
//                                                      if id is legal    
    sfcc=hwid%100-1;//SFEC channel (0-15)
    crate=hwid/100-1;//crate (0-7)
    if(crate==rcrat){
      nhits+=ptr->getntdca();// counts hits (edges) of TDC-A (anode TovT)
      nzchn+=ptr->getnzchn();// counts nonzero TDC-A-channels (not TDC-T (FT))
    } 
//
    ptr=ptr->next();
  }
//
  if(fmt==1){ // =====> Reduced format :
    len+=1; // hit_bitmask
    if(nhits>0){// nonempty SFEC
      nzchn+=2;// add 2 TDCT(=FT) channels
      nhits+=(2*2);// add 2x2 TDCT-hits(for MC each FT-channel contains 2 hits(edges))
      cntw=nzchn/4;
      if(nzchn%4 > 0)cntw+=1;// hit-counter words
      len+=(cntw+nhits);
    } 
  }
//
  else{ // =====> Raw format :
    if(nhits>0){
      nhits+=(2*2);// add 2x2 TDCT-hits(for MC each FT-channel contains 2 hits(edges))
      len=2*nhits;// each hit(edge) require 2 words (header+TDCvalue)
    }
  }// end of format check
//
  }// end of SFEC-presence test
//
  return len;
//
}
//-------------------------------------------------------------------------
void AMSCTCRawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_CTC_data
// on output: len=CTC_data_length; 
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nctcs;
  int16u id;
  int16u phbit,maxv,ntdc,tdc[CTCEPMX*2],ntdct,tdct[CTCEPMX*2];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,tdcc,chip,tdccm;
  AMSCTCRawEvent *ptr;
  AMSCTCRawEvent *ptlist[CTCCHSF];
//
  phbit=SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
  len=0;
  if(4&DAQSBlock::getdmsk(rcrat) == 0)return;//no CTCSubdet. in this crate (len=0)
//
#ifdef __AMSDEBUG__
  if(CTCRECFFKEY.reprtf[1]==2)cout<<"CTC::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
// ---> prepare ptlist[tdc_channel] - list of CTCRawEvent pointers :
//
  ptr=(AMSCTCRawEvent*)AMSEvent::gethead()
                        ->getheadC("AMSCTCRawEvent",0);
  for(tdcc=0;tdcc<CTCCHSF;tdcc++)ptlist[tdcc]=0;// clear pointer array
  nctcs=0;
  while(ptr){
    id=ptr->getid();// CC (1-CTCCCMX)
    hwid=AMSCTCRawEvent::sw2hwid(id);// CAA, always >0 here
    tdcc=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      if(ptr->getnzchn())nctcs+=1;//counts number of RawEv. objects with nonzero TovT
      ptlist[tdcc]=ptr;// store pointer
      ntdct=ptr->gettdct(tdct);// get FT-tdc (in MC these values are the same for all objects)
#ifdef __AMSDEBUG__
      if(CTCRECFFKEY.reprtf[1]==2)ptr->_printEl(cout);
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
    if(nctcs>0){ // nonempty SFEC
      shft=0;
      nzch=0;//counts all nonzero TDC-channels (incl. FT)
      nctcs=0;// counts only CTC TDC-channels
      for(tdcc=0;tdcc<CTCCHSF;tdcc++){// <--- TDC-channels loop (16) (some of them are FT)
        mskb=1<<tdcc;
        ntdc=0;// hits(edges) in given tdcc
        mtyp=1; // measurement type (1-> TDCA)
        if(tdcc==7 || tdcc==15){  //these TDC-channels contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =2 in MC
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
          if(mtyp=1)nctcs+=1;//counts only CTC-edges (not FT)
        }
      } // end of TDC-channels loop
      if((shft<3) && (nzch>0))*(p+ichc)=hitc;// add uncompleted hit-counters word
    }// ---> end of nonempty SFEC check
//
    *(p+ichm)=htmsk;// add SFEC-channels bitmask word
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
    if(nctcs>0){ // nonempty SFEC check
      for(tdcc=0;tdcc<CTCCHSF;tdcc++){// <--- TDC-channels loop (0-15)
        chip=tdcc/8;//0-1
        chipc=tdcc%8;//chann.inside chip (0-7)
        chc=((chipc&1)<<2);//convert chipc to reverse bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        ntdc=0;
        slad=DAQSBlock::sladdr(int16u(CTCSLOT-1));//slot-addr(h/w addr of SFEC in crate)
        adrw=slad;// put slot-addr in address word
        adrw|=(chc<<12);// add channel number(inside of TDC-chip)
        mtyp=1;// measur.type (1->TDCA)
        if(chipc==7){  //this TDC-channel contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =2 in MC
          for(i=0;i<ntdct;i++)tdc[i]=tdct[i];// FT measurement to write-buffer
        }
        ptr=ptlist[tdcc];
        if(ptr>0)ntdc=ptr->gettdca(tdc);// put CTC-measurements to buffer (tdc)
        if(ntdc>0){
          for(i=0;i<ntdc;i++){
            adr=adrw;
            if((tdc[i]&phbit)>0)adr|=phbtp;// add phase bit to address-word
            tdcw=tdc[i]&maxv;// put hit-value to tdc-word
            tdcw|=((1-chip)<<15);// add chip number(1-chip to be uniform with TOF) 
            *(p+ic++)=tdcw; // write hit-value
            *(p+ic++)=adr; // write hit-address
          }
        }
      } // end of TDC-channels loop in SFEC
//
    }// ---> end of nonempty SFEC check
//
  } // ---> end of "Raw" format
//
  len=ic;
//
#ifdef __AMSDEBUG__
  if(CTCRECFFKEY.reprtf[1]==2){
    cout<<"CTCDAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
    cout<<dec<<endl<<endl;
  }
#endif
}
//-------------------------------------------------------------------------
// function to build Raw: 
//  
void AMSCTCRawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
// on input:  *p=pointer_to_beginning_of_block (to block-id word)
//            len=bias_to_first_CTC_data_word
// on output: len=CTC_data_length.
//
  integer i,j,jj,ic,ibar,isid,lentot,bias;
  integer val,warnfl;
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hwch,hmsk,slad,chip,chipc,chc;
  int16u swid[CTCCHCH],mtyp,hcnt,shft,nhit,nzch,nzcch,sbit;
  int16u phbit,maxv,phbt,phbtp; 
  int16u ids,stat,chan;
  int16u ntdca[CTCCHCH],tdca[CTCCHCH][CTCEPMX*2],ntdct,tdct[CTCEPMX*2],dummy;
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
  if(CTCRECFFKEY.reprtf[1]>=1){
    cout<<"CTC::decoding: crate/format="<<crate<<" "<<dtyp<<"  bias="<<bias<<endl;
  }
#endif
//
  if(dtyp==1){ // =====> reduced TDC data decoding :
      ic=bias;//jamp to first data word (bias=1+previous_detectors_data_length) 
//
      hmsk=*(p+ic++);// SFEC nonzero-channels bit mask
      nzch=0;//nonzero TDCchannels in SFEC (upto 15)
      if(hmsk>0){
        for(chip=0;chip<2;chip++){ // <--- TDC-chip loop (0-1)
          nzcch=0;// nonzero anti-channels in chip
          for(tdcc=0;tdcc<8;tdcc++){ // <-- TDC-chan. in chip loop (0-7)
            hwch=8*chip+tdcc;// TDC-ch numbering inside SFEC (0-15)
            mtyp=0;
            if(tdcc<CTCCHCH)mtyp=1;// measurement_type (1-TDCA)
            if(tdcc==7){  //this TDC-channels contain FT-hits
              mtyp=2;// means FT measurements
            }
//
            if( (hmsk & (1<<hwch)) > 0){// nonzero SFEC-channel processing:
              shft=nzch%4;
              if(shft==0){// take new counters_word
                hcnt=*(p+ic++);
              }
              nhit=((hcnt>>(4*shft))&15)+1;// numb.of hits for current mtyp
              for(i=0;i<nhit;i++){//    <-- Hit loop
                if(mtyp==0)dummy=*(p+ic++);// ignore nonCTC/nonFT hits (if any)
                if(mtyp==1)tdca[nzcch][i]=*(p+ic++);
                if(mtyp==2)tdct[i]=*(p+ic++);
              } //                      --> end of hit loop
              if(mtyp==1)ntdca[nzcch]=nhit;
              if(mtyp==2)ntdct=nhit;
              nzch+=1;//count !=0 TDC-ch inside of one SFEC
              if(mtyp==1){
                swid[nzcch]=AMSCTCRawEvent::hw2swid(crate,hwch);// CC(1-CTCCCMX) or 0
                nzcch+=1;//count !=0 CTC-TDC-channels inside of one TDC-chip
              }
            }
//
          }// ---> end of TDCchip channels loop
//
          if(nzcch>0){// <--- create AMSCTCRawEvent objects for nonempty TDC-chip
            for(i=0;i<nzcch;i++){// <--- loop over non-empty anti-channels
              ids=swid[i];//CC
              if(ids>0){
                stat=0; // tempor 
                AMSEvent::gethead()->addnext(AMSID("AMSCTCRawEvent",0),
                new AMSCTCRawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
              }
              else{
           cerr<<"CTCDAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
              }
            }// ---> end of object creation loop
          }// ---> end of nonempty chip
//
        }//---> end of TDC-chip loop
      }//---> end of !=0 mask check
//
      len=ic-len;//return pure CTC_data_length
  }// end of reduced data decoding
//
//---------------
//
  else if(dtyp==0){ // =====> raw TDC data decoding :
//
    integer lent(0);
    int16u hits[CTCCHSF][16];//array to store hit-values(upto 16) for each channel in SFEC
    integer nhits[CTCCHSF];// number of hits in given channel
    int16u tdcw,adrw,hitv;
//
    for(i=0;i<CTCCHSF;i++){// clear buffer
      nhits[i]=0;
      for(j=0;j<16;j++)hits[i][j]=0;
    }
//
    ic=1;// for Raw format start from the beginning of block + 1
//    ic=bias;// tempor
    while(ic<lentot){ // <---  words loop
      tdcw=*(p+ic++);// chip# + tdc_value
      chip=(tdcw>>15);// TDC-chip number(0-1)
      if(ic>=lentot){
        cout<<"CTC:RawFmt:read_error: attempt to read Extra-word ic="<<ic<<" blocklength="<<lentot<<endl;
        break;   
      }
      adrw=*(p+ic++);// phbit + chipc + slotaddr.
      slad=adrw&15;// get SFEx h/w address(card-id) ((0,1,2,3)-TOF, (5)-C, (4)-A)
      sfet=DAQSBlock::slnumb(slad);// sequential slot number (0-5, or =DAQSSLT if slad invalid)) 
      if(sfet==DAQSSLT)continue; //---> invalid slad: try to take next pair
      if(DAQSBlock::isSFEC(slad)){ // SFEC data : write to buffer
        lent+=2;
        chipc=(adrw>>12)&7;// channel inside TDC-chip (0-7)(reverse bit pattern!!!)
        chc=((chipc&1)<<2);//convert chipc to normal bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        if((adrw & phbtp)>0)
                            phbt=1; // check/set phase-bit flag
        else
                            phbt=0;
        tdcc=8*(1-chip)+chc; // channel inside SFEC(0-15)(1-chip to be uniform with TOF)
        hitv=(tdcw & maxv)|(phbt*phbit);// tdc-value with phase bit set as for RawEvent
        if(nhits[tdcc]<16){
          warnfl=0;
          hits[tdcc][nhits[tdcc]]=hitv;
          nhits[tdcc]+=1;
        }
        else{
          if(warnfl==0)cout<<"CTC:RawFmt:read_warning: > 16 hits in channel: crate= "<<crate
          <<" sfet="<<sfet<<" chip="<<chip<<" chipch="<<chc<<endl;
          warnfl=1;
        }
      }
//
      else{
        continue;// nonSFEC data, take next pair
      }// ---> end of SFEC data check
//
    }// ---> end of words loop
//-----------------------------
// Now extract CTC+FT data from buffer:
//
    for(chip=0;chip<2;chip++){ // <--- TDC-chip loop (0-1)
      nzcch=0;// nonzero CTC-channels in chip
      for(tdcc=0;tdcc<8;tdcc++){ // <-- TDC-chan. in chip loop (0-7)
        hwch=8*chip+tdcc; // channel inside SFEC(0-15)
        mtyp=0;
        if(tdcc<CTCCHCH)mtyp=1;// measurement_type (1-TDCA)
        if(tdcc==7)mtyp=2; //this TDC-channels contain FT-hits (mtyp=2)
        nhit=nhits[hwch];
        if(nhit>0){
          for(i=0;i<nhit;i++){//    <-- Hit loop
            if(mtyp==0)dummy=hits[hwch][i];// ignore nonCTC/nonFT hits (if any)
            if(mtyp==1)tdca[nzcch][i]=hits[hwch][i];
            if(mtyp==2)tdct[i]=hits[hwch][i];
          } //                      --> end of hit loop
          if(mtyp==1)ntdca[nzcch]=nhit;
          if(mtyp==2)ntdct=nhit;
          if(mtyp==1){
            swid[nzcch]=AMSCTCRawEvent::hw2swid(crate,hwch);// store LXYD (or 0)
            nzcch+=1;//count !=0 CTC-TDC-channels inside of one TDC-chip
          }
        }
      }// ---> end of TDCchip channels loop
//
      if(nzcch){// <--- create AMSCTCRawEvent objects for nonempty TDC-chip
        for(i=0;i<nzcch;i++){// <--- loop over non-empty CTC-channels
            ids=swid[i];//CC
            if(ids>0){
              ids=swid[i];// CC
              stat=0; // tempor 
              AMSEvent::gethead()->addnext(AMSID("AMSCTCRawEvent",0),
              new AMSCTCRawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
            }
            else{
           cerr<<"CTCDAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
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
    cout<<"CTCDaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
#ifdef __AMSDEBUG__
  if(CTCRECFFKEY.reprtf[1]>=1){
    cout<<"CTCRawEventBuild::decoding: FOUND "<<len<<" good words, hex/bit dump follows:"<<endl;
    int16u tstw,tstb;
    for(i=0;i<len;i++){
      tstw=*(p+i+bias);
      cout<<hex<<tstw<<"  |"<<dec;
      for(j=0;j<16;j++){
        tstb=(1<<(15-j));
        if((tstw&tstb)!=0)cout<<"x"<<"|";
        else cout<<" "<<"|";
      }
      cout<<endl;
    }
    cout<<dec<<endl<<endl;
  }
#endif
}
//------------------------------------------------------------------------------
//  function to get sofrware-id (CC) for given hardware-channel (crate,sfech):
//  (imply only one (max) SFEC card per crate)                      0-7   0-15
//
int16u AMSCTCRawEvent::hw2swid(int16u a1, int16u a2){
  int16u swid,hwch;
//
  static int16u sidlst[SCCRAT*CTCCHSF]={// 14 CC's + 2 FT's  per CRATE (per SFEC):
//
// mycrate-1 = (no SFEC card) :
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//
// mycrate-2(cr31) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
   6,  5,  8,  7, 14, 13, 16,  0, 15, 46, 45, 48, 47, 56, 55,  0,
//
// mycrate-3 = (no SFEC card) :
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
//
// mycrate-4(cr71) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
  26, 25, 28, 27, 34, 33, 36,  0, 35, 66, 65, 74, 73, 82, 81,  0,
//
// mycrate-5(cr03) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
  32, 31, 38, 37, 40, 39, 72,  0, 71, 78, 77, 80, 79, 84, 83,  0,
//
// mycrate-6(cr33) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
  54, 53, 62, 61, 64, 63, 70,  0, 69, 22, 21, 24, 23, 30, 29,  0,
//
// mycrate-7(cr43) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
  42, 41, 44, 43, 52, 51, 50,  0, 49,  2,  1,  4,  3, 10,  9,  0,
//
// mycrate-8(cr73) = (1 SFEC)x(2x(7xTDCA +1xTDCT(FT))) :
  12, 11, 18, 17, 20, 19, 58,  0, 57, 60, 59, 68, 67, 76, 75,  0};
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<SCCRAT);//crate(0-7)
  assert(a2>=0 && a2<CTCCHSF);//sfech(0-15)
#endif
  hwch=int16u(CTCCHSF*a1+a2);// hardware-channel
  swid=sidlst[hwch]; // software-id CC (conf.combination number)
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;//tempor
  return swid;
}
//-----------------------------------------------------------------------
//function to get hardware-id(CAA) FROM software-ID==CC (1-CTCCCMX): 
//
int16u AMSCTCRawEvent::sw2hwid(int16u a1){
  static int16u first(0);
  integer i;
  int16u swch,swid,hwid,crate,sfech;
  static int16u hidlst[CTCCCMX]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>0 && a1<=CTCCCMX); // CC (conf.combination number)
#endif
//
  if(first==0){ // create hardw.id list:
    first=1;
    for(i=0;i<CTCCCMX;i++)hidlst[i]=0;
    for(crate=0;crate<SCCRAT;crate++){// crates (0-7)
      for(sfech=0;sfech<CTCCHSF;sfech++){// SFEC channels (0-15)
        hwid=100*(crate+1)+sfech+1;//  (CAA)
        swid=AMSCTCRawEvent::hw2swid(crate,sfech);// CC
        if(swid>0 && swid<=CTCCCMX){// legal swid (FT's and dummies = 0 here)
          swch=swid-1;
          hidlst[swch]=hwid;
        }
      }// end of sfech-loop
    }// end of crate-loop
  }// end of first
// 
  swch=a1-1;
  hwid=hidlst[swch];// hardware-id CAA
//  cout<<"swch->hwid: "<<swch<<" "<<hwid<<endl;//tempor
  return hwid;
}




