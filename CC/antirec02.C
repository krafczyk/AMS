//  $Id: antirec02.C,v 1.15 2003/06/26 13:12:20 choumilo Exp $
//
// May 27, 1997 "zero" version by V.Choutko
// June 9, 1997 E.Choumilov: 'siantidigi' replaced by
//                           (RawEvent + validate + RawCluster) + trpattern;
//                           complete DAQ-section added; Cluster modified
//
//    18.03.03 E.Choumilov Simu/Reco-logic completely changed according to 
//                                         AMS02 design. 
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
#include <tofdbc02.h>
#include <tofsim02.h>
#include <antidbc02.h>
#include <daqs2block.h>
#include <antirec02.h>
#include <ecalrec.h>
#include <ntuple.h>
#include <mceventg.h>
using namespace std;
//
//
 integer Anti2RawEvent::trpatt=0;
//----------------------------------------------------
void Anti2RawEvent::validate(int &status){ //Check/correct RawEvent-structure
  int16u nadca[2],adca1[ANTI2C::ANAHMX];
  int16u ntdct[2],tdct1[ANTI2C::ANTHMX*2],tdct2[ANTI2C::ANTHMX*2];
  int16u pbitn;
  int16u pbanti;
  int16u pbup,pbdn,pbup1,pbdn1;
  int16u id,idN,stat[2];
  integer sector,isid,isds;
  integer i,j,im,nhit,chnum,am[2],sta,st;
  int bad;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  pbitn=TOF2GC::SCPHBP;// as for TOF
  pbanti=pbitn-1;
  status=1;//bad
  bad=1;// means NO both Charge and History measurements 
//
// =============> check/correct logical "up/down" sequence :
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0,1);//last 1 to sort
  isds=0;
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]>=1)
  cout<<endl<<"=======> Anti::validation: for event "<<(AMSEvent::gethead()->getid())<<endl;
#endif
//                             
  while(ptr){//<---- loop over ANTI RawEvent hits
#ifdef __AMSDEBUG__
    if(ATREFFKEY.reprtf[1]>=1)ptr->_printEl(cout);
#endif
    id=ptr->getid();// BBarSide
    sector=id/10-1;//bar 0-7
    isid=id%10-1;//0-1 (top/bot)
    chnum=sector*2+isid;//channels numbering
    stat[isid]=ptr->getstat();
    if(stat[isid] == 0){ // still no sense(?) ( =0 upto now by definition)
//       fill working arrays for given side:
      isds+=1;
      ANTI2JobStat::addch(chnum,5);
//
//-----> check Charge-ADC :
//
      nadca[isid]=ptr->getadca(adca1);
      im=nadca[isid];
      if(im==0)ANTI2JobStat::addch(chnum,6);
      if(im>1)ANTI2JobStat::addch(chnum,7);
      nhit=im;
      if(nhit>0)bad=0;//found charge measurement
//
//-----> check Hist-TDC :
//
      ntdct[isid]=ptr->gettdct(tdct1);
      nhit=0;
      im=ntdct[isid];
      for(i=0;i<im;i++)tdct2[i]=0;
      for(i=0;i<im-1;i++){// find all correct pairs of up/down bits settings
        pbdn=(tdct1[i]&pbitn);//check p-bit of down-edge (come first, LIFO mode !!!)
        pbup=(tdct1[i+1]&pbitn);//check p-bit of up-edge (come second)
        if(TOF2DBc::pbonup()==1){
          if(pbup==0 || pbdn!=0)continue;//wrong sequence, take next pair
        }
        else{
          if(pbup!=0 || pbdn==0)continue;//wrong  sequence, take next pair
        }
        tdct2[nhit]=tdct1[i];
        nhit+=1;
        tdct2[nhit]=tdct1[i+1];
        nhit+=1;
        i+=1;//to bypass current 2 good edges
      }
      if(nhit<im){//something was wrong (lost edge, overflow ?)
        ptr->puttdct(int16u(nhit),tdct2);// refill object with corrected data
        ANTI2JobStat::addch(chnum,8);
      }
      if(nhit>0)bad=0;//found history measurement
//-----      
    }//--->endof stat check 
//
//---------------
    ptr=ptr->next();// take next RawEvent hit
  }//----> endof RawEvent hits loop
//
//
  if(bad==0)status=0;//good anti-event(were ADC+TDC measurements)
}
//----------------------------------------------------
void Anti2RawEvent::mc_build(int &stat){
  geant up(0),down(0),slope[2],athr[2],tau,q2pe;
  geant eup(0),edown(0),tup(0),tdown(0);
  geant thresh[2];
  int i,ii,nup,ndown,nupt,ndownt,sector,sectorN,ierr,trflag(0),it,sta[2];
  uinteger j,ibmn[2],ibmx[2],nsides(0);
  uinteger ectrfl(0),trpatt(0),hcount[4],cbit,lsbit(1);
  int16u atrpat[2]={0,0};
  int16u phbit,maxv,id,chsta,nadca,adca[ANTI2C::ANAHMX];
  int16u nhtdch,htdc[2*ANTI2C::ANTHMX],itt;
  number edep,ede,edept(0),time,z,tlev1,ftrig,t1,t2,dt;
  geant fadcb[2][ANTI2C::ANFADC+1];
  geant hdthr,tdthr,gain[2],fladcb,htdcb,htdpr,hdmn;
  geant ped,sig,am,amp,tm,tmp,tmd1u,tmd1d,td1b1u,td1b1d,td1b2u,td1b2d,tmark;
  int upd1,upd11,upd12,stackof;
  integer nhtdc,iamp,ncoinc,ncoinct(0);
  number esignal[2][ANTI2C::MAXANTI];
  number htup[ANTI2C::ANTHMX],htdw[ANTI2C::ANTHMX];
  number ptup[ANTI2C::ANTHMX];
  integer nptr;
  geant pwid,pdbr,pgate,tg1,tg2;
  AMSAntiMCCluster * ptr;
  AMSAntiMCCluster * ptrN;
  geant * parr;
//
  static integer first=0;
  static integer nshap;//real length of (light+PMT)-pulse(in bins=ANTI2DBc::fadcbw())
  static geant pshape[ANTI2C::ANFADC];//store PM single ph.el. pulse shape
//(+ result of time-dispersion of original light flash due to light collection) 
  if(first++==0){
    ANTI2DBc::inipulsh(nshap,pshape); // prep.PM-pulse shape arr.(in ANTI2DBc::fadcbw() bins)
  }
//
  VZERO(esignal,2*ANTI2C::MAXANTI*sizeof(esignal[0][0])/sizeof(geant));
  stat=1;//bad
  fladcb=ANTI2DBc::fadcbw();//flash-adc bin-width
  htdcb=ANTI2DBc::htdcbw();//hist-tdc bin width
  htdpr=ANTI2DBc::htdcdr();//h-tdc double pulse resol.
  hdmn=ANTI2DBc::hdpdmn();//h-discr. min. pulse-duration
  pwid=ANTI2DBc::ppfwid();//"pattern" pulse fixed width
  pdbr=ANTI2DBc::pbdblr();// .............. dbl-resol(dead time)
  pgate=ANTI2DBc::pbgate();// gate, applied to "pattern" pulse
  trflag=TOF2RawEvent::gettrfl();
  Anti2RawEvent::setpatt(trpatt);// reset trigger-pattern in Anti2RawEvent::
//
  if(TGL1FFKEY.trtype<256){//<==== NotExternalTrigger"
//
    if(trflag>=0){// use FT from TOF
      ftrig=TOF2RawEvent::gettrtime();//Time when FT came to S-crate(incl. fixed delay)
      tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
    }
    else{// have to use FT from ECAL
      ectrfl=AMSEcalRawEvent::gettrfl(); 
      if(ectrfl<=0)return;//no EC trigger also -> no chance to digitize ANTI
      ftrig=AMSEcalRawEvent::gettrtm();
      tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
    }
//
  }//===>endof "NotExtTrig" check
//
  else{//<=== ExtTrigger
    geant ttrig1=0.;//tempor (true ExtTrigSignal-time)
    ftrig=ttrig1+TOF2Varp::tofvpar.ftdelf();// FTrigger abs time (fixed delay added)
    tlev1=ftrig+TOF2DBc::accdel();// "common_stop"-signal abs.time
  }
//-----------
  geant padl=0.5*ANTI2DBc::scleng();
  phbit=TOF2GC::SCPHBP;// phase bit position as for TOF !!!
  maxv=phbit-1;// max possible TDC value (16383)
  ptr=(AMSAntiMCCluster*)AMSEvent::gethead()->
               getheadC("AMSAntiMCCluster",0,1); // last 1  to test sorted container
//
  edept=0.;
  ncoinct=0;
  for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
    fadcb[0][i]=0.;
    fadcb[1][i]=0.;
  }
  nupt=0;
  ndownt=0;
  ibmn[0]=ANTI2C::ANFADC;
  ibmn[1]=ANTI2C::ANFADC;
  ibmx[0]=0;
  ibmx[1]=0;
//-------------
  AMSContainer *cptr;
  cptr=AMSEvent::gethead()->getC("AMSAntiMCCluster",0);//MC container-pointer
  ii=0;
  if(cptr)ii+=cptr->getnelem();
  if(ATMCFFKEY.mcprtf)HF1(2607,geant(ii),1.);
//----
  while(ptr){ // <--- geant-hits loop:
    sector=ptr->getid();
    ede=ptr->getedep();
    edep=ede*1000;
    edept+=edep;
    z=ptr->getcoo(2);
    time=(1.e+9)*(ptr->gettime());// geant-hit time in ns
//
    eup=0.5*edep*exp(z/ATMCFFKEY.LZero)
       *ANTI2Pcal::antisccal[sector-1].getmev2pe(1);// aver. up-side(+z) signal(pe)
//                                                  (relat.gains are applied later)
    POISSN(eup,nup,ierr);//real number of up-side p.e's
    nupt+=nup;
    if(nup>0){ 
      tup=(time+(padl-z)/ATMCFFKEY.LSpeed);
      j=uinteger(floor(tup/ANTI2DBc::fadcbw()));//time-bin number
      if(j<ibmn[1])ibmn[1]=j;//min.bin
      for(i=0;i<nshap;i++){//"dispersion"
        ii=i+j;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;//ovfl-bin
        fadcb[1][ii]+=nup*pshape[i];
      }
      if(ii>ibmx[1])ibmx[1]=ii;//max.bin
    }
//
    edown=0.5*edep*exp(-z/ATMCFFKEY.LZero)
         *ANTI2Pcal::antisccal[sector-1].getmev2pe(0);//aver. down-side(-z) signal(pe)
    POISSN(edown,ndown,ierr);//real number of down-side p.e's
    ndownt+=ndown;
    if(ndown>0){ 
      tdown=(time+(padl+z)/ATMCFFKEY.LSpeed);
      j=uinteger(floor(tdown/ANTI2DBc::fadcbw()));
      if(j<ibmn[0])ibmn[0]=j;//min.bin
      for(i=0;i<nshap;i++){//"dispersion"
        ii=i+j;
        if(ii>ANTI2C::ANFADC)ii=ANTI2C::ANFADC;
        fadcb[0][ii]+=ndown*pshape[i];
      }
      if(ii>ibmx[0])ibmx[0]=ii;//max.bin
    }
//--------------
    ptrN=ptr->next();     
    sectorN=0;
    if(ptrN)sectorN=ptrN->getid(); //next hit sector-number
//
    if(sectorN != sector){//<--- next(last) sector: create signals for current sector
#ifdef __AMSDEBUG__
      assert(sector >0 && sector <= ANTI2C::MAXANTI);
#endif
    ANTI2Pcal::antisccal[sector-1].getgain(gain);//2-sides gains
    hdthr=ANTI2Pcal::antisccal[sector-1].getathr();//hist-discr thresh(pe's)
    esignal[1][sector-1]=nupt*gain[1]; // save tot.number of p.e.'s in buffer
    esignal[0][sector-1]=ndownt*gain[0];
//
//--->upply threshold to flash-adc signal to get times
//
    for(j=0;j<2;j++){//<---------------- side loop(down->up)
      ncoinc=0;
      nptr=0;
      id=sector*10+j+1;//BBS (BB=paddle, S=side(1->down,2->up))
      parr=&fadcb[j][0];
      if(ATMCFFKEY.mcprtf==3)ANTI2DBc::displ_a(id,1,parr);//print pulse
//
//--->loop over flash-adc buffer,upply threshold to signal to get times
//
      hdthr/=gain[j];//to avoid multiplying by gain of each buffer ampl.
      nhtdc=0;
      upd1=0; //up/down flag for discr-1
      tmd1u=-9999.;//up-time of discr-1
      tmd1d=-9999.;//down-time of discr-1
//
      upd11=0;//up/down flag for hist-TDC(branche-1 of discr-1)
      td1b1u=-9999.;//discr1_branch1(hist-TDC) up-time
      td1b1d=-9999.;// ......................down-time
//
      upd12=0;//up/down flag for "pattern"(branche-2 of discr-1)
      td1b2u=-9999.;//discr1_branch2(pattern) up-time
      td1b2d=-9999.;// ......................down-time
      stackof=0;//stack overfl.flag
      amp=0.;
      tmp=fladcb*ibmn[j];
      tm=tmp;
      if(ibmx[j]==ANTI2C::ANFADC){
        ANTI2JobStat::addmc(1);
#ifdef __AMSDEBUG__
        cout<<"AntiRawEvent::mc_build-W: Flash-TDC buffer ovfl,id/PElast="
	                                <<id<<" "<<fadcb[j][ibmx[j]]<<endl;
#endif
	ibmx[j]=ANTI2C::ANFADC-1;//don't use overflow-bin
      }
//====>
      for(i=ibmn[j];i<=ibmx[j];i++){//<====== flash-adc buffer loop
        tm+=fladcb;
        am=fadcb[j][i];//instant ampl. from flash-adc buffer
//----------
// discr-1(anode,fast comparator for h-TDC ) up/down setting with LOW thr.:
        if(am>=hdthr){
          if(upd1 ==0){
            tmd1u=tmp+fladcb*(hdthr-amp)/(am-amp);// up time of discr.1(bin-width compencated)
            tmark=tmd1u;// no  t-dispersion by discr.itself
            upd1=1;
          }
        }
        else{
          if(upd1!=0 && (tm-tmd1u)>hdmn){ // down time (+min.duration check)
            upd1=0;
            tmd1d=tm;
          }
        }
//----------
        amp=am;// store ampl to use as "previous" one in next i-loop
        tmp=tm;// .......
//----------
//--->try set all branches of discr  when it is up:
// 
        if(upd1>0){//<--- disc-1 up ?
//"history-TDC branche" :        
          if(upd11==0){
            if((tm-td1b1d)>htdpr){ //dead-time(=h_TDC dbl.resol ?)-check
              upd11=1;  // set flag for h-TDC branch
              td1b1u=tmark;//store up-time
            }
          }
//"pattern-branch" :
            if(upd12==0){
              if((tm-td1b2u)>(pwid+pdbr)){//buzy time(pwid+dead) check for "pattern"
                upd12=1;  // set flag 
		td1b2u=tm;//store up-time to use as previous in next "up" set
                if(nptr<ANTI2C::ANTHMX){//store upto ANTHMX(=8) up-edges
                  ptup[nptr]=tm;
                  nptr+=1;
                }
                else{
                  cerr<<"AntiRawEvent::mc_build-W: PatternPulse ovfl(hits number)"<<nptr<<endl;
                  upd12=2;//blockade on overflow for safety
                }
              } 
            }
	}//<-- end of discr-1 up-check     
//
//----------
//--->try reset all branches:
//
//"history-TDC branche":
        if(upd11==1){ 
          if(upd1==0 || i==ibmx[j]){//reset "h-TDC" if discr-1 down, or out-of-time 
            upd11=0;
            td1b1d=tm;
	    for(ii=nhtdc-1;ii>=0;ii--){//move up/down-times in the LIFO-stack
	      if(ii+1<ANTI2C::ANTHMX){
	        htup[ii+1]=htup[ii];
	        htdw[ii+1]=htdw[ii];
	      }
	    }
	    htup[0]=td1b1u;//write latest up-time
	    htdw[0]=td1b1d;//write latest down-time
            if(nhtdc<ANTI2C::ANTHMX)nhtdc+=1;
            else{
              if(stackof==0){//1st stack ovfl for given id
                ANTI2JobStat::addmc(2);
#ifdef __AMSDEBUG__
	        cout<<"AntiRawEvent::mc_build-W: h-TDC stack ovfl,id="<<id<<endl;
#endif
	      }
	      stackof=1;
            }
          }
        }
//"pattern branch":
        if(upd12==1){// "pattern pulse" self-reset(independ.of discr-1 state)
            if((tm-td1b2u)>pwid){//min duration (fixed pulse width) check
              upd12=0;                    // for self-clear
            }
        }
//-------------
      }//======> endof flash-adc buffer loop
//-------------
      bool anchok=(ANTIPeds::anscped[sector-1].PedStOK(j) &&
                  ANTI2Pcal::antisccal[sector-1].CalStOK(j));//Chan-OK
//
//---> Create charge raw-hits:
//
      geant adc2pe=ANTI2Pcal::antisccal[sector-1].getadc2pe();
      geant daqthr=ANTI2Pcal::antisccal[sector-1].getdqthr();//DAQ-readout thresh(adc-ch)    
      ped=ANTIPeds::anscped[sector-1].apeda(j);// in adc-chan. units(float)
      sig=ANTIPeds::anscped[sector-1].asiga(j);
      if(ATMCFFKEY.mcprtf)HF1(2602+j,geant(esignal[j][sector-1]),1.);
      amp=esignal[j][sector-1]/adc2pe;//pe -> ADC-ch
      amp=amp+ped+sig*rnormx();// adc+ped(no "integerization")
      iamp=integer(floor(amp));//go to real ADC-channels("integerization")
      if(iamp>TOF2GC::SCADCMX){//check ADC-overflow
        ANTI2JobStat::addmc(3);
#ifdef __AMSDEBUG__
        cout<<"ANTI2RawEvent_mcbuld-W: ADC-range overflow,id="<<id<<endl;
#endif
        iamp=TOF2GC::SCADCMX;
      }
      amp=number(iamp)-ped;// subtract pedestal (loose "integerization" !)
      if(ATMCFFKEY.mcprtf)HF1(2604+j,amp,1.);
      nadca=0;
      if(amp>daqthr*sig){// DAQ readout threshold check
	iamp=floor(amp*ANTI2DBc::daqscf()+0.5);// floatADC -> internal DAQ bins
        itt=int16u(iamp);
        adca[nadca]=itt;
        nadca+=1;
      }
//
//---> Create history raw-hits:
//
      if(ATMCFFKEY.mcprtf)HF1(2606,geant(nhtdc),1.);
      nhtdch=0;
      for(i=0;i<nhtdc;i++){//        <--- htdc-hits loop ---
        ii=i;// htup/dw have LIFO readout order(1st elem = last stored(first read))
        t1=htup[ii]+ANTI2Pcal::antisccal[sector-1].gettzer();//add cl-fiber delay for given sector 
        t2=htdw[ii]+ANTI2Pcal::antisccal[sector-1].gettzer();
        dt=tlev1-t2;// follow LIFO mode of readout : down-edge - first hit
        it=integer(floor(dt/ANTI2DBc::htdcbw())); // conv. to hist-TDC binning
        if(it>maxv){//out of range
          ANTI2JobStat::addmc(4);
#ifdef __AMSDEBUG__
          cout<<"ANTI2RawEvent_mcbuild-W : Hist-TDC overflow(down edge) !!!"<<'\n';
#endif
          it=maxv;
        }
        itt=int16u(it);
        if(!TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        htdc[nhtdch]=itt;
        nhtdch+=1;
        dt=tlev1-t1;// follow LIFO mode of readout : leading(up) edge - second
        it=integer(floor(dt/ANTI2DBc::htdcbw())); // conv. to hist-TDC binning
        if(it>maxv){//out of range
          it=maxv;
        }
        itt=int16u(it);
        if(TOF2DBc::pbonup())itt=itt|phbit;//add phase bit if necessary
        htdc[nhtdch]=itt;
        nhtdch+=1;
      }//--- end of htdc-hits loop --->
//
//----> create RawEvent-object and store trig-patt:
//
      if(anchok && nadca>0 && nhtdch>0){
        chsta=0;// good
	id=sector*10+j+1;//BBarSide(BBS)
//cout<<"MCFillRaw:id="<<id<<" nadc/ntdc="<<nadca<<" "<<nhtdch<<" nhtdc="<<nhtdc<<endl;
//for(int ih=0;ih<nadca;ih++)cout<<adca[ih]<<" ";
//cout<<endl;
//for(int ih=0;ih<nhtdch;ih++)cout<<htdc[ih]<<" ";
//cout<<endl;
        AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                       new Anti2RawEvent(id,chsta,nadca,adca,nhtdch,htdc));
        tg1=ftrig-ATREFFKEY.ftwin;//GateUpTime~FTime(subtr. some adjustment-delay because pattern
//                                             is made in TgBox and FTtime is time at S-crate)
        tg2=tg1+pgate;//gate_end_time
	for(i=0;i<nptr;i++){// check up-time of each "pattern" pulse for coinc.with FT
          t1=ptup[i];//"pattern" pulse up-time
	  t2=t1+pwid;//"pattern" pulse down-time
	  dt=tg1-t1;
	  if(nptr==1 && ATMCFFKEY.mcprtf>0)HF1(2601,geant(dt),1.);
	  if(tg2<=t1 || tg1>=t2)continue;
	  ncoinc+=1;
	}
	if(ncoinc){
	  atrpat[j]|=1<<(sector-1);//set bit
	  ncoinct+=1;
	}
      }
//------------
    }//--->endof side loop
//
    nupt=0;
    ndownt=0;
    ibmn[0]=ANTI2C::ANFADC;
    ibmn[1]=ANTI2C::ANFADC;
    ibmx[0]=0;
    ibmx[1]=0;
    for(i=0;i<=ANTI2C::ANFADC;i++){//reset MC Flash-ADC buffer
      fadcb[0][i]=0.;
      fadcb[1][i]=0.;
    }
    }//------------------> endof next(last) sector check 
    ptr=ptr->next();
//  
  } // ---> end of geant-hits loop
//
  if(ATMCFFKEY.mcprtf)HF1(2600,edept,1.);
  if(ncoinct>0)ANTI2JobStat::addmc(5);
//----------------------
//----> create Anti-trigger pattern:
//(bits 1-8->sector's s1, 8-16-> s2; 17-24-> 4 trig.counters)
//                         
  for(i=0;i<4;i++)hcount[i]=0;
  trpatt=0;//reset patt.
  for(i=0;i<ANTI2C::MAXANTI;i++){
    cbit=lsbit<<i;
    if((i<2)||(i>5)){ // <-- x>0
      if(atrpat[1]&cbit)hcount[3]+=1; // z>0
      if(atrpat[0]&cbit)hcount[2]+=1; // z<0
    }
    else{              // <-- x<0
      if(atrpat[1]&cbit)hcount[1]+=1; // z>0
      if(atrpat[0]&cbit)hcount[0]+=1; // z<0
    }
//    if(atrpat[0]&cbit>0 || atrpat[1]&cbit>0)trpatt|=cbit;//two side OR
  }
//
  trpatt=(atrpat[0] | (atrpat[1]<<8));//1-8=>s1(bot), 9-16=>s2(top)
  for(i=0;i<4;i++){//add 4 trig.counters
    if(hcount[i]>3)hcount[i]=3;
    trpatt|=hcount[i]<<(16+i*2);
  }
//
  Anti2RawEvent::setpatt(trpatt);// add trigger-pattern to Anti2RawEvent::
  if(trpatt!=0 || edept>0)stat=0;//found anti activity
//
}
//
//===========================================================================
//
void AMSAntiCluster::_writeEl(){
//
  if(AMSAntiCluster::Out( IOPA.WriteAll%10==1 ||  checkstatus(AMSDBc::USED ))){
   int i;
#ifdef __WRITEROOT__
   AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// fill the ntuple
    AntiClusterNtuple* TN = AMSJob::gethead()->getntuple()->Get_anti();
    if (TN->Nanti>=MAXANTICL) return;
    TN->Status[TN->Nanti]=_status;
    TN->Sector[TN->Nanti]=_sector;
    TN->Ntimes[TN->Nanti]=_ntimes;
    TN->Npairs[TN->Nanti]=_npairs;
    for(i=0;i<_ntimes;i++)TN->Times[TN->Nanti][i]=_times[i];;
    for(i=0;i<_ntimes;i++)TN->Timese[TN->Nanti][i]=_etimes[i];;
    TN->Edep[TN->Nanti]=_edep;
    for(i=0;i<3;i++)TN->Coo[TN->Nanti][i]=_coo[i];
    for(i=0;i<3;i++)TN->ErrorCoo[TN->Nanti][i]=_ecoo[i];
    TN->Nanti++;
  }
}
//---
void AMSAntiCluster::_copyEl(){
}
//---
void AMSAntiCluster::_printEl(ostream & stream){
  stream <<"AMSAntiCluster: sta/sect= "<<_status<<" "<<_sector
         <<" ntm/npr="<<_ntimes<<" "<<_npairs
         <<" time1,2"<<_times[0]<<" "<<_times[1]
         <<" etime1,2"<<_etimes[0]<<" "<<_etimes[1]
         <<" Edep"<<_edep<<" Coo/err"<<_coo<<  " "<<_ecoo<<endl;
}
//--
void AMSAntiCluster::print(){
AMSContainer *p =AMSEvent::gethead()->getC("AMSAntiCluster",0);
 if(p)p->printC(cout);
}
//
//=============================
//
void AMSAntiCluster::build2(){
//(combine 2-sides of single paddle)
  bool anchok;
  int16u nadca,adca[ANTI2C::ANAHMX],ntdct,tdct[ANTI2C::ANTHMX*2];
  int16u id,idN,sta;
  int16u pbitn;
  int16u pbanti;
  number edep[2],uptm[2][ANTI2C::ANTHMX],edep2,ass,zc,erz,erfi,err;
  integer nuptm[2];
  integer i,j,jmax,sector,isid,nsds,isdn[2],stat,chnum,n1,n2,i1min,i2min;
  number zcoo[2*ANTI2C::ANTHMX],times[2*ANTI2C::ANTHMX],etimes[2*ANTI2C::ANTHMX];
  number edept,t1,t2,t1mn,t2mn,dt,z1,z2;
  integer ntimes,npairs,nclust(0),nclustp(0),status(0);
  geant ftdel[2],padlen,padrad,padfi,paddfi,zcer1,zcer2,ped,sig,dtmin;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptrN;
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                  ->getheadC("Anti2RawEvent",0);// already sorted after validation
//
  pbitn=TOF2GC::SCPHBP;//phase bit position as for TOF
  pbanti=pbitn-1;// mask to avoid it.
  padlen=ANTI2DBc::scleng();
  padrad=ANTI2DBc::scradi()+0.5*ANTI2DBc::scinth();
  paddfi=360./ANTI2C::MAXANTI;
  zcer1=ATREFFKEY.zcerr1;//Z-coo err. for true pair case
  nsds=0;
  nadca=0;
  ntdct=0;
  edept=0.;
  edep[0]=0;
  edep[1]=0;
  while(ptr){ // <------ RawEvent hits loop
    id=ptr->getid();//BBS
    sector=id/10-1;
    isid=id%10-1;
    chnum=sector*2+isid;//channels numbering
    anchok=(ANTIPeds::anscped[sector].PedStOK(isid) &&
                  ANTI2Pcal::antisccal[sector].CalStOK(isid));//Chan-DBStat OK
    if(anchok){// <--- channel alive
//channel statistics :
      ANTI2JobStat::addch(chnum,0); 
      nadca=ptr->getadca(adca);
      if(nadca>0)ANTI2JobStat::addch(chnum,1);
      if(nadca==1)ANTI2JobStat::addch(chnum,2);
//
      ntdct=ptr->gettdct(tdct);
      if(ntdct>0)ANTI2JobStat::addch(chnum,3);
      if(ntdct==2)ANTI2JobStat::addch(chnum,4);//1hit=2edges
      isdn[nsds]=isid+1;
//DAQ-ch-->edep(mev):
      ped=ANTIPeds::anscped[sector-1].apeda(isid);//adc-chan
      sig=ANTIPeds::anscped[sector-1].asiga(isid);//adc-ch sigmas
      edep[nsds]=number(adca[0])/ANTI2DBc::daqscf()       //DAQ-ch-->ADC-ch
                /ANTI2Pcal::antisccal[sector].sgain(isid)//gain correction
                *ANTI2Pcal::antisccal[sector].getadc2pe() //ADC-ch-->p.e.
                /ANTI2Pcal::antisccal[sector].getmev2pe(isid); //p.e.-->Edep(Mev)
//cout<<"    decoded edep="<<edep[nsds]<<endl; 
//TDC-ch-->time(ns):
      nuptm[nsds]=0;
      for(i=0;i<ntdct;i++){
        if(i%2==1){//use up-edges (i=1,3,...)
         uptm[nsds][nuptm[nsds]]=(pbanti&tdct[i])*ANTI2DBc::htdcbw()    //TDC-ch-->ns
	                        +ANTI2Pcal::antisccal[sector].gettzer();//compens.tzero
//cout<<"    decoded Up-time="<<uptm[nsds][nuptm[nsds]]<<endl;
         nuptm[nsds]+=1;
        }
      }
//
      nsds+=1;
    }//--->endof alive-check
//--------
    ptrN=ptr->next();
    idN=0;
    if(ptrN)idN=(ptrN->getid())/10; //next hit short_id (BBS)
//
    if(idN != id/10){//next hit is new sector: create 2-sides signal for current sect
      padfi=paddfi*(0.5+sector);//current paddle phi
      erfi=paddfi/sqrt(12.);
      err=ANTI2DBc::scinth()/sqrt(12.);
      ntimes=0;
      npairs=0;
      edep2=0;
      if(edep[0]>0 || edep[1]>0)ANTI2JobStat::addbr(sector,0);
//                                                   <-------- case1=2sides:
      if(nsds==2){
//                        <-- search for true SideTime-pairs(giving best zcoo):
	n1=nuptm[0];
	n2=nuptm[1];
	while(n1>0 && n2>0){//<-- try next pair ?
	  dtmin=9999.;
	  for(int i1=0;i1<nuptm[0];i1++){//<-- combinations loop
	    t1=uptm[0][i1];
	    if(t1<0)continue;
	    for(int i2=0;i2<nuptm[1];i2++){
	      t2=uptm[1][i2];
	      if(t2<0)continue;
              dt=fabs(t1-t2);
	      if(dt<dtmin){
	        dtmin=dt;
	        i1min=i1;
		i2min=i2;
	      }
	    }
	  }//-->endof combin. loop
	  t1=uptm[0][i1min];//best pair
	  t2=uptm[1][i2min];
//
	  zc=-0.5*(t1-t2)*ATMCFFKEY.LSpeed;//abs.Z(neglect by possible ANTI Z-shift)
	  if(fabs(zc)<(0.5*padlen+3*zcer1)){//match found(Zc is within paddle size)
	    if(zc>0.5*padlen)zc=0.5*padlen;
	    if(zc<-0.5*padlen)zc=-0.5*padlen;
	    zcoo[npairs]=zc;
	    times[npairs]=0.5*(t1+t2);
	    etimes[npairs]=zcer1/ATMCFFKEY.LSpeed;
	    npairs+=1;
	    n1-=1;
	    n2-=1;
	    uptm[0][i1min]=-1;//mark used hit(all times are positive)
	    uptm[1][i2min]=-1;
	  }
	  else break;//next pair will be bad(because current one was the best) 
//
	}//-->endof "try next pair" 
//
        if(npairs==1){//imply, that single Edep correlated with this single pair
	  zc=zcoo[0];
	  edep2=(edep[0]+edep[1])
               *2/(exp(zc/ATMCFFKEY.LZero)+exp(-zc/ATMCFFKEY.LZero));
	  erz=zcer1;
	}
	else{//npair=0,>1; not possible to associate single Edep and time
//        (to have something, let us calc.zc using Edep(is) assimetry)
          ass=(edep[1]-edep[0])/(edep[1]+edep[0]);
          zc=ATMCFFKEY.LZero/2*log((1.+ass)/(1.-ass));
          edep2=(edep[0]+edep[1])
          *2/(exp(zc/ATMCFFKEY.LZero)+exp(-zc/ATMCFFKEY.LZero));
          number ddelta=1/sqrt(edep[1]+edep[0]);
          number z1,z2;
          number d1=ass+ddelta;
          if(d1 >=1)z1=0.5*padlen;      
          else if(d1 <=-1)z1=-0.5*padlen;      
          else z1=ATMCFFKEY.LZero/2*log((1.+d1)/(1.-d1));
          number d2=ass-ddelta;
          if(d2 <=-1)z2=-0.5*padlen;      
          else if(d2 >=1)z2=0.5*padlen;      
          else z2=ATMCFFKEY.LZero/2*log((1.+d2)/(1.-d2));
          erz=fabs(z1-z2)/2;
	}
	ntimes+=npairs;
//
      }//--->endof 2s-case
//                                                  <--------- case1=1side:
      if(nsds==1){
        isid=isdn[nsds];//=1,2
        edep2=edep[nsds]*2;//rough estimation to compensate missing side
	zc=0.;
	erz=padlen/sqrt(12.);
        status|=TOFGC::SCBADB2;// set bit for one-side paddles
        if(isid==2)status|=TOFGC::SCBADB4;// set missing-side bit(s=2 if set)
      }//--->endof 1s-case
      if(ATREFFKEY.reprtf[0] && nsds==2)HF1(2508,geant(edep2),1.);
//---
      if(nsds>0 && edep2>ATREFFKEY.Edthr){//non-empty paddle -> create object
        for(i=0;i<nsds;i++){// add all unpaired times
	  for(j=0;j<nuptm[i];j++){
	    t1=uptm[i][j];
	    if(t1>0){
	      times[ntimes]=t1;
	      etimes[ntimes]=padlen/sqrt(12.)/ATMCFFKEY.LSpeed;
	      ntimes+=1;
	    }
	  }
	}
//
        AMSPoint coo(padrad,padfi,zc);
        AMSPoint ecoo(err,erfi,erz);
        ANTI2JobStat::addbr(sector,1);
//
//cout<<"   FillClustSect="<<sector+1<<" nsds/ntimes/npair="<<nsds<<" "<<ntimes<<" "<<npairs<<endl;
//cout<<"     edep2="<<edep2<<" coo="<<coo<<" err="<<ecoo<<endl;
//for(i=0;i<ntimes;i++)cout<<" times="<<times[i]<<" err="<<etimes[i]<<endl;
        AMSEvent::gethead()->addnext(AMSID("AMSAntiCluster",0),
         new AMSAntiCluster(status,sector+1,ntimes,npairs,times,etimes,edep2,coo,ecoo));
//
	nclust+=1;
	if(npairs>0)nclustp+=1;
        edept+=edep2;
        if(ATREFFKEY.reprtf[0]){
          HF1(2502,geant(ntimes),1.);
          HF1(2503,geant(npairs),1.);
          if(npairs==1)HF1(2504,geant(zc),1.);
	  if(nsds==2 && npairs!=1)HF1(2505,geant(zc),1.);
          HF1(2506,geant(sector+1),1.);
        }
      }//--->endof non-empty paddle check
      
//
      nsds=0;
      nadca=0;
      ntdct=0;
      edep[0]=0;
      edep[1]=0;
    }//--->endof next sector check
//---
    ptr=ptr->next();// take next RawEvent hit
  }//------>endof RawEvent hits loop
//
  if(ATREFFKEY.reprtf[0]){
    HF1(2500,geant(edept),1.);
    HF1(2501,geant(nclust),1.);
    HF1(2507,geant(nclustp),1.);
  }
//
}

//===============================

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

//===================================================================================
//  DAQ-interface functions :
//
// function returns number of Anti_data-words ( incl. FT-hits)
// for given block/format (one(max) SFEA card/crate is implied !!!)
//
integer Anti2RawEvent::calcdaqlength(int16u blid){
  Anti2RawEvent *ptr;
  int16u id,ibar,isid;
  int16u crate,rcrat,antic,hwid,fmt;
  integer len(0),nhits(0),nzchn(0),cntw;
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0);
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
//
  if((2&(DAQS2Block::getdmsk(rcrat))) > 0){ // AntiSubdet. is in this crate 
  while(ptr){
    id=ptr->getid();// BBS
    ibar=id/10-1;
    isid=id%10-1;
    hwid=Anti2RawEvent::sw2hwid(ibar,isid);// CAA, always >0 here
    antic=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      nhits+=ptr->getnadca();// counts hits (edges) of TDCA
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
void Anti2RawEvent::builddaq(int16u blid, integer &len, int16u *p){
//
// on input: *p=pointer_to_beginning_of_ANTI_data
// on output: len=ANTI_data_length; 
//
  integer i,j,stat,ic,icc,ichm,ichc,nzch,nanti;
  int16u ibar,isid,id;
  int16u phbit,maxv,ntdc,tdc[ANTI2C::ANAHMX*2],ntdct,tdct[ANTI2C::ANAHMX*2];
  int16u mtyp,hwid,hwch,swid,swch,htmsk,mskb,fmt,shft,hitc;
  int16u slad,tdcw,adrw,adr,chipc,chc;
  int16u phbtp;  
  int16u crate,rcrat,tdcc,chip,tdccm;
  Anti2RawEvent *ptr;
  Anti2RawEvent *ptlist[ANTI2C::ANCHSF];
//
  phbit=TOF2GC::SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=TOF2GC::SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
//
  rcrat=(blid>>6)&7;// requested crate #
  fmt=1-(blid&63);// 0-raw, 1-reduced
  len=0;
  if(2&DAQS2Block::getdmsk(rcrat) == 0)return;//no AntiSubdet. in this crate (len=0)
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]==2)cout<<"Anti::encoding: crate/format="<<rcrat<<" "<<fmt<<endl;
#endif
//
// ---> prepare ptlist[tdc_channel] - list of AntiRawEvent pointers :
//
  ptr=(Anti2RawEvent*)AMSEvent::gethead()
                        ->getheadC("Anti2RawEvent",0);
  for(tdcc=0;tdcc<ANTI2C::ANCHSF;tdcc++)ptlist[tdcc]=0;// clear pointer array
  nanti=0;
  while(ptr){
    id=ptr->getid();// BBS
    ibar=id/10-1;
    isid=id%10-1;
    hwid=Anti2RawEvent::sw2hwid(ibar,isid);// CAA, always >0 here (if valid ibar/isid) 
    tdcc=hwid%100-1;
    crate=hwid/100-1;
    if(crate==rcrat){
      if(ptr->getnzchn())nanti+=1;
      ptlist[tdcc]=ptr;// store pointer
      ntdct=ptr->gettdct(tdct);// get FT-tdc (in MC these values are the same for all objects)
#ifdef __AMSDEBUG__
      if(ATREFFKEY.reprtf[1]==2)ptr->_printEl(cout);
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
      for(tdcc=0;tdcc<ANTI2C::ANCHSF;tdcc++){// <--- TDC-channels loop (16) (some of them are FT)
        mskb=1<<tdcc;
        ntdc=0;// hits(edges) in given tdcc
        mtyp=1; // measurement type (1-> TDCA)
        if(tdcc==7 || tdcc==15){  //these TDC-channels contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =1 in MC
          for(i=0;i<ntdct;i++)tdc[i]=tdct[i];// FT measurement
        }
        ptr=ptlist[tdcc];// =0 for FT (sw2hwid-function should provide that)
        if(ptr>0)ntdc=ptr->getadca(tdc);// TDCA measurement
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
      for(tdcc=0;tdcc<ANTI2C::ANCHSF;tdcc++){// <--- TDC-channels loop (0-15)
        chip=tdcc/8;//0-1
        chipc=tdcc%8;//chann.inside chip (0-7)
        chc=((chipc&1)<<2);//convert chipc to reverse bit pattern
        chc|=(chipc&2);
        chc|=((chipc&4)>>2);
        ntdc=0;
        slad=DAQS2Block::sladdr(int16u(ANTI2C::ANSLOT-1));//slot-addr(h/w addr of SFEA in crate)
        adrw=slad;// put slot-addr in address word
        adrw|=(chc<<12);// add channel number(inside of TDC-chip)
        mtyp=1;// measur.type (1->TDCA)
        if(chipc==7){  //this TDC-channel contain FT-hits
          mtyp=2;// means FT measurements
          ntdc=ntdct;// =1 in MC
          for(i=0;i<ntdct;i++)tdc[i]=tdct[i];// FT measurement to write-buffer
        }
        ptr=ptlist[tdcc];
        if(ptr>0)ntdc=ptr->getadca(tdc);// ANTI-meas. to write-buffer
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
  if(ATREFFKEY.reprtf[1]==2){
    cout<<"Anti2DAQBuild::encoding: WRITTEN "<<len<<" words, hex dump follows:"<<endl;
    for(i=0;i<len;i++)cout<<hex<<(*(p+i))<<endl;
    cout<<dec<<endl<<endl;
  }
#endif
}
//------------------------------------------------------------------------------
// function to build Raw: 
//  
void Anti2RawEvent::buildraw(int16u blid, integer &len, int16u *p){
//
// on input:  *p=pointer_to_beginning_of_block (to block-id word)
//            len=bias_to_first_Anti_data_word
// on output: len=Anti_data_length.
//
  integer i,j,jj,ic,ibar,isid,lentot,bias;
  integer val,warnfl;
  int16u btyp,ntyp,naddr,dtyp,crate,sfet,tdcc,hwch,hmsk,slad,chip,chipc,chc;
  int16u swid[ANTI2C::ANCHCH],mtyp,hcnt,shft,nhit,nzch,nzcch,sbit;
  int16u phbit,maxv,phbt,phbtp; 
  int16u ids,stat,chan;
  int16u ntdca[ANTI2C::ANCHCH],tdca[ANTI2C::ANCHCH][ANTI2C::ANAHMX*2],ntdct,tdct[ANTI2C::ANAHMX*2],dummy;
//
  phbit=TOF2GC::SCPHBP;//take uniq phase-bit position used in Reduced format and TOFRawEvent
  maxv=phbit-1;// max TDC value
  phbtp=TOF2GC::SCPHBPA;//take uniq phase-bit position used in Raw format for address-word
  lentot=*(p-1);// total length of the block(incl. block_id)
  bias=len;
//
// decoding of block-id :
//
  crate=(blid>>6)&7;// node_address (0-7 -> DAQ crate #)
  dtyp=1-(blid&63);// data_type ("0"->RawTDC; "1"->ReducedTDC)
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]>=1){
    cout<<"Anti2::decoding: crate/format="<<crate<<" "<<dtyp<<"  bias="<<bias<<endl;
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
            if(tdcc<ANTI2C::ANCHCH)mtyp=1;// measurement_type (1-TDCA)
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
                swid[nzcch]=Anti2RawEvent::hw2swid(crate,hwch);// BBS or 0
                nzcch+=1;//count !=0 Anti-TDC-ch inside of one TDC-chip
              }
            }
//
          }// ---> end of TDCchip channels loop
//
          if(nzcch>0){// <--- create Anti2RawEvent objects for nonempty TDC-chip
            for(i=0;i<nzcch;i++){// <--- loop over non-empty anti-channels
              ids=swid[i];//BBS
              if(ids>0){
                stat=0; // tempor 
                AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
                new Anti2RawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
              }
              else{
           cerr<<"Anti2DAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
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
    int16u hits[ANTI2C::ANCHSF][16];// array to store hit-values(upto 16) for each h/w channel
    integer nhits[ANTI2C::ANCHSF];// number of hits in given h/w channel
    int16u tdcw,adrw,hitv;
//
    for(i=0;i<ANTI2C::ANCHSF;i++){// clear buffer
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
        cout<<"ANTI2:RawFmt:read_error: attempt to read Extra-word ic="<<ic<<" blocklength="<<lentot<<endl;
        break;   
      }
      adrw=*(p+ic++);// phbit + chipc + slotaddr.
      slad=adrw&15;// get SFEx h/w address(card-id) ((0,1,2,3)-TOF, (5)-C, (4)-A)
      sfet=DAQS2Block::slnumb(slad);// sequential slot number (0-5, or =TOF2GC::DAQSSLT if slad invalid)) 
      if(sfet==TOF2GC::DAQSSLT)continue; //---> invalid slad: try to take next pair
      if(DAQS2Block::isSFEA(slad)){ // SFEA data : write to buffer
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
          cout<<"ANTI2:RawFmt:read_warning: > 16 hits in channel: crate= "<<crate
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
        if(tdcc<ANTI2C::ANCHCH)mtyp=1;// measurement_type (1-TDCA)
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
            swid[nzcch]=Anti2RawEvent::hw2swid(crate,hwch);// store BBS (or 0)
            nzcch+=1;//count !=0 Anti-TDC-ch inside of one TDC-chip
          }
        }
      }// ---> end of TDCchip channels loop
//
      if(nzcch){// <--- create Anti2RawEvent objects for nonempty TDC-chip
        for(i=0;i<nzcch;i++){// <--- loop over non-empty anti-channels
            ids=swid[i];//BBS
            if(ids>0){
              ids=swid[i];// BBS
              stat=0; // tempor 
              AMSEvent::gethead()->addnext(AMSID("Anti2RawEvent",0),
              new Anti2RawEvent(ids,stat,ntdca[i],tdca[i],ntdct,tdct));
            }
            else{
           cerr<<"Anti2DAQ:read_error: smth wrong in softw_id<->crate/chan map !"<<endl;
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
    cout<<"Anti2DaqRead: Unknown data type "<<dtyp<<endl;
    return;
  }
//
#ifdef __AMSDEBUG__
  if(ATREFFKEY.reprtf[1]>=1){
    cout<<"Anti2RawEventBuild::decoding: FOUND "<<len<<" good words, hex/bit dump follows:"<<endl;
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
int16u Anti2RawEvent::hw2swid(int16u a1, int16u a2){
  int16u swid,hwch;
//
  static int16u sidlst[TOF2GC::SCCRAT*ANTI2C::ANCHSF]={// 14 BBS's + 2 FT's  per CRATE (per SFEA):
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
  assert(a1>=0 && a1<TOF2GC::SCCRAT);//crate(0-7)
  assert(a2>=0 && a2<ANTI2C::ANCHSF);//antich(0-15)
#endif
  hwch=int16u(ANTI2C::ANCHSF*a1+a2);// hardware-channel
  swid=sidlst[hwch]; // software-id BBS (S=1->up(+z), =2->down(-z))
//  cout<<"hwch->swid: "<<hwch<<" "<<swid<<endl;//tempor
  return swid;
}
//-----------------------------------------------------------------------
//  function to get hardware-id (CAA) for given software-chan (bar, side)
//                                                             0-15  0-1
//
int16u Anti2RawEvent::sw2hwid(int16u a1, int16u a2){
  static int16u first(0);
  integer i;
  int16u ibar,isid,swch,swid,hwid,crate,antic;
  static int16u hidlst[ANTI2C::MAXANTI*2]; // hardw.id list
//
#ifdef __AMSDEBUG__
  assert(a1>=0 && a1<ANTI2C::MAXANTI);// bar(0-15)
  assert(a2>=0 && a2<2); // side(0-1)
#endif
//
  if(first==0){ // create hardw.id list:
    first=1;
    for(i=0;i<ANTI2C::MAXANTI*2;i++)hidlst[i]=0;
    for(crate=0;crate<TOF2GC::SCCRAT;crate++){
      for(antic=0;antic<ANTI2C::ANCHSF;antic++){
        hwid=100*(crate+1)+antic+1;// CrateAntich (CAA)
        swid=Anti2RawEvent::hw2swid(crate,antic);// BBS
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
