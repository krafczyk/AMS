//Create by Qi Yan 2012/05/01
// ------------------------------------------------------------
//      History
//        Modified:  Adding Geant3 Support 2012/05/16
// -----------------------------------------------------------

#include "tofdbc02.h"
#include "Tofdbc.h"
#include <stdio.h>
#include "typedefs.h"
#include "cern.h"
#include "time.h"
#include "extC.h"
#include <math.h>
#include "point.h"
#include "amsgobj.h"
#include "event.h"
#include "amsstl.h"
#include "commons.h"
#include "Tofsim02.h"
#include "mccluster.h"
#include "trigger102.h"
#include "trigger302.h"
#include "mceventg.h"
#include "ecalrec.h"
#include "tofid.h"
#include "root.h"
#include "ntuple.h"
#ifdef _PGTRACK_
#include "HistoMan.h"
#endif
#include "tofsim02.h"
//
using namespace std;
//------------------------------------------------------------------
void TOF2TovtN::covtoph(integer idsoft, geant vect[], geant edep,geant tofg, geant tofdt,geant stepl,integer parentid){

  integer id,pmtid,ibar,ilay,is,ipm,ibtyp,cnum;
  integer i,kk,i1,i2;
  geant edep1,x,y,z,time,dtime,sharep,eff,r,rand;
  geant nel0,nels,nelp;
  int neles,photongen=0;
  integer idivx,nwdivs,npmts;
  geant phtiml=0,phtim=0,phtims=0,phtimd=0,phtral=0,phene=0,tfpos[3]={0,0,0},tfdir[3]={0,0,0};
  char vname[5];
  integer ierr(0);
  static geant bthick=0.5*TOF2DBc::plnstr(6);
  static geant convr =TOF2DBc::edep2ph();//

  //--
  id=idsoft;
  x=vect[0];y=vect[1];z=vect[2]; 
  edep1=edep*1000.;//GeV->MeV
  time=(1.e+9)*tofg; // conv. to "ns"
  dtime=tofdt;
  ilay=id/100-1;
  ibar=id%100-1;
  //--
  AMSPoint cglo(x,y,z);//gcoo
  kk=ilay*TOF2GC::SCMXBR+ibar+1;//bar ID used in the volume name
  if(kk>=10)sprintf(vname,"TF%d",kk);
  else      sprintf(vname,"TF0%d",kk);
  AMSgvolume *p=AMSJob::gethead()->getgeomvolume(AMSID(vname,id));// pointer to volume "TFnn",id
  AMSPoint cloc=p->gl2loc(cglo);// convert global coord. to local
  x=cloc[0];y=cloc[1];z=cloc[2];
  //cout<<"loc x="<<x<<" loc y="<<y<<" locz="<<z<<endl;
  TOF2JobStat::addmc(12);
  nwdivs=TOFWScanN::scmcscan1[ilay][ibar].getndivs();//wdivs-x
  npmts=TOFWScanN::scmcscan1[ilay][ibar].getnpmts();//pmts per side
  idivx=TOFWScanN::scmcscan1[ilay][ibar].getwbin(x);//wdivs-x-idiv
  cnum=TOF2DBc::barseqn(ilay,ibar);// solid sequential numbering(0->33)
  if(idivx<0 || (fabs(z)-bthick)>0.01){
    cout<<"---> TOF2TovtN::build: OutOfVolError:ID="<<id<<" cnum="<<cnum<<" Vol.name:"<<vname<<endl;
    if(idivx<0)cout<<"     Out-of-width hit !"<<endl;
    if((fabs(z)-bthick)>0.01)cout<<"     Out-of-thickness hit !"<<endl;
    TOF2JobStat::addmc(3);
    return;
  }
  TOFWScanN::scmcscan1[ilay][ibar].getybin(idivx,y,i1,i2,r);//y-bin #
  nel0=edep1*convr;
  //      cout<<"edep="<<edep<<" sum generate phton="<<nel0<<endl;
  for(is=0;is<2;is++){
     if(is==0)eff=TOFWScanN::scmcscan1[ilay][ibar].getef1(idivx,r,i1,i2);//side eff
     else     eff=TOFWScanN::scmcscan1[ilay][ibar].getef2(idivx,r,i1,i2);
     if(eff>0.1*npmts)eff=0.1*npmts;
     else if(eff<0)   eff=0.;
//        cout<<"ilay="<<ilay<<" ibar="<<ibar<<" is="<<is<<" idivx="<<idivx<<" i1="<<i1<<" i2="<<i2<<" eff="<<eff<<endl;
     nels=nel0*eff;// mean total number of photoelectrons side
//--for pmt
    for(ipm=0;ipm<npmts;ipm++){
      pmtid =ilay*1000+ibar*100+is*10+ipm;
      if(is==0)sharep=TOFWScanN::scmcscan1[ilay][ibar].getps1(ipm,idivx,r,i1,i2);//share 0side
      else    sharep=TOFWScanN::scmcscan1[ilay][ibar].getps2(ipm,idivx,r,i1,i2);
      nelp=nels*sharep;
      //            cout<<"nelp="<<nelp<<endl;
      POISSN(nelp,neles,ierr);
      //            cout<<"pmt="<<ipm<<" sum pmt phton="<<neles<<endl;
      for(i=0;i<neles;i++){//for each side
         rand=RNDM(-1); 
	 phtims=dtime*rand;//photon begin step time 
         tfpos[0]=vect[0]+stepl*rand*vect[3];
         tfpos[1]=vect[1]+stepl*rand*vect[4];
         tfpos[2]=vect[2]+stepl*rand*vect[5];
	 phtimd=TOFPMT::phriset();//photon rise time+decay time
	 if(is==0)phtiml=TOFWScanN::scmcscan1[ilay][ibar].gettm1(idivx,r,i1,i2);
	 else     phtiml=TOFWScanN::scmcscan1[ilay][ibar].gettm2(idivx,r,i1,i2);
	 if(G4FFKEY.TFNewGeant4>0)phtim=time+phtims+phtimd+phtiml;
         else                     phtim=time+phtiml; 
/*         if(phtim>500){
             cout<<"ilay ibar is ipm idiv i1 i2"<<ilay<<" "<<ibar<<" "<<is<<" "<<ipm<<" "<<idivx<<" "<<i1<<" "<<i2<<endl;
             cout<<" time="<<time<<" phtims="<<phtims<<" phtimd="<<phtimd<<" phtiml="<<phtiml<<endl;
          }*/
	 AMSTOFMCPmtHit::sitofpmthits(pmtid,parentid,phtim,phtiml,phtral,phene,tfpos,tfdir);
	 photongen++;
      }
    }//end pmt
  }//end side

 
}    
//------
void TOF2TovtN::build()
{
  integer id,idd,ibar,ilay,is,ipm;
  integer i,j,ij;
  uinteger ii;
  geant edep,edepb,time,pmtime,am;
  geant tslice[TOF2GC::PMTSMX][TOF2GC::SCTBMX+1]; //  flash ADC array
//---
  geant dummy(-1);int ierr(0);
  integer nhitl[TOF2GC::SCLRS];
  static geant ifadcb=1./TOF2DBc::fladctb();
  static int prlevel=1;

  if(prlevel==1){
     cout<<"using tof Tofsim02"<<endl;
     prlevel=0;
   }
 //---
   AMSTOFMCPmtHit *ptrpm=(AMSTOFMCPmtHit *)AMSEvent::gethead()->
                                   getheadC("AMSTOFMCPmtHit",0,1); 
   AMSTOFMCCluster *ptr=(AMSTOFMCCluster*)AMSEvent::gethead()->
                                    getheadC("AMSTOFMCCluster",0,1);

    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
       for(i=0;i<=TOF2GC::SCTBMX;i++){
         tslice[ipm][i]=0;
      }
    }
    //cout<<"begin totovtn build"<<endl;
    int nphoton=0;
    while(ptrpm){
      time=ptrpm->getphtime();//ns
      id=  ptrpm->getpmtid();
      ilay=id/1000%10; 
      ibar=id/100%10;
      is=  id/10%10;
      ipm= id%10;
      nphoton++;
      if(ptrpm==0)cout<<"Error TOF PMT Hit loop"<<endl;
      pmtime=0;
      if(G4FFKEY.TFNewGeant4>0){//LTRANS already include this item
        pmtime+=TOFPMT::pmttm[ilay][ibar][is][ipm];//Transmit time Mean about 7.2ns
        pmtime+=TOFPMT::pmtts[ilay][ibar][is][ipm]*rnormx()-1.9;//Transmit time Spread (-1.9) convert to pulse begin time  
      }
      time+=pmtime;
//--
      //cout<<"phtime="<<time<<endl;
     ii=uinteger(time*ifadcb+0.5);//arrive time bin/compesate
//    if(ii>1000)cout<<"ilay ibar is ipm"<<ilay<<" "<<ibar<<" "<<is<<" "<<ipm<<" pmtime="<<pmtime<<" neg_time="<<time<<endl;
      am=TOFPMT::phseamp();
      ptrpm->sitofpmtpar(pmtime,(am*TOFPMT::pmgain[ilay][ibar][is][ipm]));
      uinteger npulseb=TOFPMT::pmpulse.getnb();
      for(i=0;i<npulseb;i++){
        ij=i+ii;
        if(ij>TOF2GC::SCTBMX)break;//max time
        tslice[ipm][ij]+=am*TOFPMT::pmpulse.gety(i+1); 
      //  cout<<"am="<<am<<endl;
      }
///----
      if(ptrpm->next()==0||(ptrpm->next()->getpmtid()/10!=ptrpm->getpmtid()/10)){//isid different
        //cout<<"begin tofcluster edep"<<endl;
        edepb=0.;//GeV
        while(ptr){
          id=ptr->idsoft;
          if(id>(100*(ilay+1)+ibar+1))break;
          else if(id==(100*(ilay+1)+ibar+1))edepb+=ptr->edep*1000;//GeV->MeV
          ptr=ptr->next();
        }
        if(edepb>0.)TOF2JobStat::addmc(4);//count fired bars
//        idd=id*10+1;//LBBS already change
        idd=1000*(ilay+1)+10*(ibar+1)+(is+1);//LBBS
        //cout<<"begin proces totovtn"<<endl;
        if(edepb>TFMCFFKEY.Thr)TOF2TovtN::totovtn(idd,edepb,tslice);//put PMT for side
        //cout<<"end process totovtn"<<endl;
        for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
          for(i=0;i<=TOF2GC::SCTBMX;i++)tslice[ipm][i]=0;
        }
        //cout<<"end tofcluster edep="<<edepb<<endl;
      }
//---
      ptrpm=ptrpm->next();//to next
    }//out of loop

    cout<<"<<---TOF New MC Event nphoton="<<nphoton<<endl;

         
//-------
// <--- arrange in incr.order TOF2TovtN::SumHT/SumSHT-arrays,created by all calls to TOF2TovtN::totovt(...)-routine:
  int nhth;
  for(int ic=0;ic<TOF2GC::SCCRAT;ic++){
    for(int sl=0;sl<TOF2GC::SCFETA-1;sl++){
      nhth=TOF2Tovt::SumHTh[ic][sl];
      if(nhth>0){//<-- non-empty slot with SumHTt-hits
	AMSsortNAGa(TOF2Tovt::SumHTt[ic][sl],nhth);
	if(TFMCFFKEY.mcprtf[3]>1){
          cout<<"     TOF2TovT:SumHT>0:cr/sl_seq="<<ic<<" "<<sl<<" Nhits="<<nhth<<endl;
          for(int ih=0;ih<nhth;ih++)cout<<"       "<<TOF2Tovt::SumHTt[ic][sl][ih]<<endl;
	}
      }
      nhth=TOF2Tovt::SumSHTh[ic][sl];
      if(nhth>0){//<-- non-empty slot with SumSHTt-hits
	AMSsortNAGa(TOF2Tovt::SumSHTt[ic][sl],nhth);
	if(TFMCFFKEY.mcprtf[3]>1){
          cout<<"     TOF2TovT:SumSHT>0:cr/ssl="<<ic<<" "<<sl<<" Nhits="<<nhth<<endl;
          for(int ih=0;ih<nhth;ih++)cout<<"       "<<TOF2Tovt::SumSHTt[ic][sl][ih]<<endl;
	}
      }
    }
  }

//---
  geant edepl[4];
  if(TFMCFFKEY.mcprtf[2]!=0){
    for(i=0;i<TOF2GC::SCLRS;i++)HF1(1050+i,geant(nhitl[i]),1.);
    HF1(1060,1000.*edepl[0],1.);
    HF1(1061,1000.*edepl[1],1.);
    HF1(1062,1000.*edepl[2],1.);
    HF1(1063,1000.*edepl[3],1.);
  }

//
}
//=========================================================================
// function below simulate Anode saturation
geant TOF2TovtN::pmsatur(geant am,int ilay,int ibar,int is,int ipm){
  geant satuindex=1./(1.-TOFPMT::ansat[ilay][ibar][is][1]);
  geant satuthr=pow(TOFPMT::ansat[ilay][ibar][is][0],satuindex);
  geant satuam;
  if(am<satuthr)satuam=am;
  else          satuam=TOFPMT::ansat[ilay][ibar][is][0]*pow(am,TOFPMT::ansat[ilay][ibar][is][1]);
  return satuam;         
}

//--------------------------------------------------------------------------
void TOF2TovtN::totovtn(integer idd, geant edepb, geant tslice1[][TOF2GC::SCTBMX+1])
{
//!!! Logic is preliminary:wait for detailed SFET slecrtrical logic from Diego(Guido)
  integer i,k,ii,j,ij,ipm,ilay,ibar,isid,id,_sta,stat(0);
  geant tm,a,am,am0,amd,tmp,amp,amx,amxq;
  geant iq0,it0,itau;
  integer _ntr1,_ntr2,_ntr3,_nftdc,_nstdc,_nadca,_nadcd;
  number _ttr1u[TOF2GC::SCTHMX1],_ttr2u[TOF2GC::SCTHMX1],_ttr3u[TOF2GC::SCTHMX1];
  number _ttr1d[TOF2GC::SCTHMX1],_ttr2d[TOF2GC::SCTHMX1],_ttr3d[TOF2GC::SCTHMX1];
  number _tftdc[TOF2GC::SCTHMX2],_tftdcd[TOF2GC::SCTHMX2];
  number _tstdc[TOF2GC::SCTHMX3];
  number _adca;
  number _adcd[TOF2GC::PMTSMX];
  number tovt,aqin;
  int updsh;
  int imax,imin;
  geant a2dr[2],adc2q;
  static geant a2ds[2],adc2qs;
  geant tshd,tshup,saturf;
  number charge,charged[TOF2GC::PMTSMX];
  geant tbn,w,bo1,bo2,bn1,bn2,tmark;
  static integer first=0;
  static integer nshbn,mxcon,mxshc,mxshcg;
  static geant fladcb,cconv;
  geant daqt0,daqt1,daqt2,daqt3,daqt4,fdaqt0;
  static geant daqp0,daqp1,daqp2,daqp3,daqp4,daqp5,daqp6,daqp7,daqp8,daqp9,daqp10;
  static geant daqp11,daqp12;
  number adcs;
  int16u otyp,mtyp,crat,slot,tsens;
//
//cout<<"======>Enter TOF2TovtN::totovt with id="<<idd<<endl;
  id=idd/10;// LBB
  isid=idd%10-1;// side
  ilay=id/100-1;
  ibar=id%100-1;
  mtyp=0;
  otyp=0;
  AMSSCIds tofid(ilay,ibar,isid,otyp,mtyp);//otyp=0(anode),mtyp=0(LT-time)
  crat=tofid.getcrate();//current crate#
  slot=tofid.getslot();//sequential slot#(0,1,...9)(2 last are fictitious for d-adcs)
  tsens=tofid.gettempsn();//... sensor#(1,2,...,5 == sequential SFET(A)'s number !!!)
//
  if(first++==0){
    fladcb=TOF2DBc::fladctb();          //prepare some time-stable parameters
    cconv=fladcb/50.; // for mV->pC (50 Ohm load)
    daqp0=TOF2DBc::daqpwd(0);// fixed PW of "FTC(HT)"-branch output pulse(ACTEL-outp going to SPT)
    daqp1=TOF2DBc::daqpwd(1);// minimal inp.pulse width(TovT) to fire discr.(its rise time)
    daqp2=TOF2DBc::daqpwd(2);// fixed PW of "FTZ(SHT)"-branch output pulse(ACTEL-outp going to SPT) 
    daqp3=TOF2DBc::daqpwd(3);// min.outPW of discr.(outPW=inpTovT-daqp1 when outPW>daqp3)
    daqp4=TOF2DBc::daqpwd(4);// inp dead time of generic discr(outpDT=daqp4+daqp1)
    daqp7=TOF2DBc::daqpwd(7);// dead time of TDC-inputs(ns,the same for LT-/FT-/Sum-inputs) 
    daqp8=TOF2DBc::daqpwd(8);// dead time of "HT/SHT-trig"-branch on ACTEL-output(going to SPT-inp)
//                                      (Guido: ACTEL-inp is faster than Discr, so no ACTEL-inp DT check)
    daqp9=TOF2DBc::daqpwd(9);// dead time of "SumHT(SHT)"-branch on ACTEL-output(going to TDC-inp) 
//                                      (Guido: ACTEL-inp is faster than Discr, so no ACTEL-inp DT check)
    daqp10=TOF2DBc::daqpwd(10);// generic discr. intrinsic accuracy
    daqp11=TOF2DBc::daqpwd(11);// fixed PW of "SumHT(SHT)"-branch on ACTEL-output(going to TDC)
  }
   daqt0=TOFPMT::daqthr[ilay][ibar][isid][0];
   fdaqt0=0.1*daqt0;// lowered threshold to select "working" part of pulse(m.b. loose some charge !!!)
   daqt1=TOFPMT::daqthr[ilay][ibar][isid][1];
   daqt2=TOFPMT::daqthr[ilay][ibar][isid][2];
//
// -----> create/fill summary Tovt-object for idsoft=idd :
  geant tslice[TOF2GC::SCTBMX+1];
  geant gainref=1;
  bool gainflag=1;
  if(TFMCFFKEY.anodesat==2)gainflag=0;

pmtansat:
  charge=0;
  for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++)charged[ipm]=0;
//---
  for(i=0;i<=TOF2GC::SCTBMX;i++){
     tslice[i]=0;
     for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
        tslice[i]+=   tslice1[ipm][i]*TOFPMT::pmgain[ilay][ibar][isid][ipm]*gainref;//all pmt together
        if(gainflag)charged[ipm]+=tslice1[ipm][i]*TOFPMT::pmgaind[ilay][ibar][isid][ipm];//integral signal together
       }
       if(gainflag)tslice[i]+=TFMCFFKEY.g4hfnoise*rnormx();//tempor high freq. noise
       charge+=tslice[i];
    }
//---total charge(PC)
    charge*=fladcb/50.; // get total charge (pC)
//---Anode PMT saturation simulation
    if(!gainflag){
        if(charge>0)gainref=pmsatur(charge,ilay,ibar,isid)/charge;
        else        gainref=0;
        gainflag=1;goto pmtansat;
    }

    //cout<<"charge an="<<charge<<"pC"<<endl;
    for(ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
       charged[ipm]*=fladcb/50.;
    } 
  
        if(tslice[TOF2GC::SCTBMX]>daqt0){//test last bin of flash-ADC ("FADC")
          TOF2JobStat::addmc(5);
//#ifdef __AMSDEBUG__
          cout<<"SITOFTovt-W: MC_FADC time-range overflow, id="<<idd<<
             "  AmplOfLastBin(mV)= "<<tslice[TOF2GC::SCTBMX]<<'\n';
          if(TFMCFFKEY.mcprtf[1]>1)TOF2Tovt::displ_a("FADC-ovfl, PM-pulse id=",idd,100,tslice);//print PMT pulse
//#endif
        }
        for(i=TOF2GC::SCTBMX;i>0;i--){
	  imax=i;
          if(tslice[i]>fdaqt0 && tslice[i-1]>fdaqt0)break;//find high limit as highest 2bins above thresh.
	}
        for(i=0;i<TOF2GC::SCTBMX;i++){
	  imin=i;
	  if(tslice[i]>fdaqt0)break;//find low limit
	}
// max possible index range: 0,1,...,TOF2GC::SCTBMX !!!
//
//cout<<"---->InTovT:id/imax="<<idd<<" "<<imax<<endl;
        _ntr1=0;
        _ntr2=0;
        _ntr3=0;
        _nftdc=0;
        _nstdc=0;
//--->
//   Based on available from Guido information, i created the following logic of
//      SFET front part(simple and fast but may be wrong a little):
//    1) for each logical channel(LT,HT,SHT,SumHT) there is a generic discriminator(fast core)
//      which require: min input pulse duration(~5ns) to go UP, have small dead time(~10ns),
//      have extending (proportional to input TovT) outp. pulse duration with some small 
//      minimum pulse width(~7ns)
//    2) the core is followed by slower output branch(s) where its own dead time(buzy) state
//      is controlled, core up-times are stored in the FIFO buffer. Branch up-setting is driven
//      by discriminator, but is branch's own dead time controlled !. Branch may be self-resetted to
//      prowide nesessary outp.pulse width(independently on discr. state !). 
//<---
     int upd1=0; //up-flag for discr-1(LowThr, followed by 1 branch: LT-time)
     geant tmd1u=-9999.;//up-time of discr-1 
     geant tmd1d=-9999.;// down-time of discr-1
     int pupd1=0;//pre_up-flag (to manage minimal inp.pulse duration(rise time) requirement)
     geant tpupd1=0;//time of the 1st pre_up (.................................)
     geant td1ref=-9999;
     
     int upd11=0;//up-flag for branche-1
     geant tmd11u=-9999.;//branche-1 up-time
     geant tmd11d=-9999.;//branche-1 down-time
//
     int upd2=0; //up-flag for discr-2(HiThr, followed by 1 branche: Z>=1(FTC)-trig)
     geant tmd2u=-9999.;//up-time of discr-2 
     geant tmd2d=-9999.;//down-time of discr-2 
     int pupd2=0;//pre_up-flag (to manage minimal inp.pulse duration(rise time) requirement)
     geant tpupd2=0;//time of the 1st pre_up (...........................................)
     
     int upd21=0;// up-flag for FTC(z>=1) branch(#1) of discr-2
     geant tmd21u=-9999;//branche-1 up-time
     geant tmd21d=-9999;//branche-1 down-time
//
     int upd3=0;//up-flag for discr-3(SHiThr,  followed by 1 branch: Z>=2 trig(FTZ))
     geant tmd3u=-9999.;//up-time of discr-3 
     geant tmd3d=-9999.;//down-time of discr-3
     int pupd3=0;//pre_up-flag (to manage minimal inp.pulse duration requirement)
     geant tpupd3=0;//time of the 1st pre_up (.................................)
     
     int upd31=0;// up-flag for FTZ(z>=2) branch of discr.3
     geant tmd31u=-9999.;// branch-1 up-time
     geant tmd31d=-9999.;// branch-1 up-time
     geant maxtu;
//    _| |_
//___|     |___  PMT-pulse
        amx=0.;
        amp=0.;
        tmp=fladcb*imin;//low edge of imin-bin
        tm=tmp;
        for(i=imin;i<=imax;i++){  //  <--- time bin loop to simulate time measurements ---
          tm+=fladcb;//high edge of the current bin
          am=tslice[i];
          if(am>amx)amx=am;//to find max amplitude
//-------------------          
// generic discr-1 up/down setting (used by LT-time branch):
          if(am>=daqt0){// <=== Am>LTthr
            if(pupd1==0&& (tm-tmd1d)>daqp4){//--D1(dead time 11 ns after previous LT down)
              pupd1=1;//already over
              tpupd1=tm;
              tmark=tmp+fladcb*(daqt0-amp)/(am-amp);//D1 raw
            }
            if(pupd1==1 && upd1==0){//check no previous dead
              if((tm-tpupd1)>daqp1){ //D1 should keep 5ns for fire//min pulse width
                upd1=1;//already up
                tmd1u=tm;// up time of discr.1
                td1ref=tmark+daqp1;//D1 raw+up keep time(more accurate)
               }
             }
          }
          else{//Am<LTthr
	    pupd1=0;//reset pre-up state
            if(upd1!=0 && (tm-tmd1u)>daqp3){ //D1 up state width delay 7ns (min. outPW check)
              upd1=0;
              tmd1d=tm;//D1 down time of discr.1
            }
          }
  
//------------------
// generic discr-2 up/down setting (used by z>=1(FTC)-trig branch):
          if(am>=daqt1){// <=== Am>HTthr 80mV
	    if(pupd2==0 && (tm-tmd2d)>daqp4){// try set D2 pre-up state(inp dead time check)
	      pupd2=1;
	      tpupd2=tm;
	    }
	    if(pupd2==1 && upd2==0){// try set D2 "up" if it is "down" and pre-up state OK 
              if((tm-tpupd2)>daqp1){// min inpTovT check
                upd2=1;
                tmd2u=tm;// up time of discr-2(delayed wrt tpupd2 according to min inpTovT (rise time))
//		cout<<"D2up:tmd2u/d="<<tmd2u<<" "<<tmd2d<<endl;
              }
	    }
          }
          else{//Am<HTthr
	    pupd2=0;//reset pre-up state
            if(upd2!=0 && (tm-tmd2u)>daqp3){ // try reset D2 (min. outPW check)
              upd2=0;
              tmd2d=tm;
//		cout<<"D2down:tmd2u/d="<<tmd2u<<" "<<tmd2d<<endl;
            }
          }
//------------------
// generic discr-3 up/down setting (used by "z>=2(FTZ)" branch:
          if(am>=daqt2){// <=== Am>SHTthr 940mV
	    if(pupd3==0 && (tm-tmd3d)>daqp4){// try set D3 pre-up state(inp dead time check)
	      pupd3=1;
	      tpupd3=tm;
	    }
	    if(pupd3==1 && upd3==0){// try set D3 "up" if it is "down" and pre-up state OK 
              if((tm-tpupd3)>daqp1){// min inpTovT check
                upd3=1;
                tmd3u=tm;// up time of discr-3(delayed wrt tpupd3 according to min inpTovT (rise time))
              }
	    }
          }
          else{//Am<SHTthr
	    pupd3=0;//reset pre-up state
            if(upd3!=0 && (tm-tmd2u)>daqp3){ // try reset D3 (min. outPW check)
              upd3=0;
              tmd3d=tm;
            }
          }
//----------
          amp=am;// store ampl to use as "previous" one in next i-loop(for interpolation)
          tmp=tm;// ......time .........................................................
//
//------------------------------------
//
// =========> try set branch-1 of discr-1(LT)  just on discr. up-edge: 
          if(upd1>0 && tm==tmd1u){//<-- D1-up moment
// ---> D1,branch-1(LT-time TDC) :        
            if(upd11==0){//try to set branch up
              if((tm-tmd11d)>daqp7){ //previous down time+20ns can get new signal  inputs branch-1(TDC-input itself) dead time(buzy) check
                upd11=1;  // set up-state 
                tmd11u=tm;//store up-time 
                if(_nstdc<TOF2GC::SCTHMX3){//store upto SCTHMX3 up-edges
                  _tstdc[_nstdc]=td1ref+daqp10*rnormx();//store precise up-time + intrinsic fluct.
                  _nstdc+=1;
                }
                else{
                  TOF2JobStat::addmc(25);
                  cout<<"TOF2Tovt::build-W: LT-time buffer ovfl, nhits="<<_nstdc<<endl;
                  upd11=2;//to block buffer input
                }
              } 
            }
          }//<-- end of discr-1 up-check
//
// =========> try set branch-1 of discr-2(HT)  when it is up:
// 
          if(upd2>0 && tm==tmd2u){//<-- D2-up moment
            if(TOF2Tovt::SumHTh[crat][tsens-1]<TOF2GC::SCTHMX2){//store upto SCTHMX2 D2 up-edges
	      TOF2Tovt::SumHTt[crat][tsens-1][TOF2Tovt::SumHTh[crat][tsens-1]]=tmd2u;//store D2 up-times
	      TOF2Tovt::SumHTh[crat][tsens-1]+=1;// for later simulation of SumHT-time channel
	    }
            else{
              TOF2JobStat::addmc(26);
              cout<<"TOF2Tovt::build-W: SumHT-time buffer ovfl, nhits="<<TOF2Tovt::SumHTh[crat][tsens-1]<<endl;
            }
// ---> D2,branch-1(FTC-trig) :
            if(upd21==0){
              if((tm-tmd21d)>daqp8){//ACTELoutp=SPTinp dead time check, imply that Actel can go UP only on D2 front edge !
                upd21=1;  // set up-state
		tmd21u=tm;//store up-time to use at self-reset(outpPW) check
//		cout<<"D2B1up:tmd21u/d="<<tmd21u<<" "<<tmd21d<<endl;
              } 
            }
	  }
//--------
// ========> try set branch-1 of discr-3(SHT)  when it is up: 
          if(upd3>0 && tm==tmd3u){//<-- D3-up moment
            if(TOF2Tovt::SumSHTh[crat][tsens-1]<TOF2GC::SCTHMX2){//store upto SCTHMX2 D3 up-edges
	      TOF2Tovt::SumSHTt[crat][tsens-1][TOF2Tovt::SumSHTh[crat][tsens-1]]=tmd3u;//store D3 up-times
	      TOF2Tovt::SumSHTh[crat][tsens-1]+=1;// for later simulation of SumSHT-time channel
	    }
            else{
              TOF2JobStat::addmc(27);
              cout<<"TOF2Tovt::build-W: SumSHT-time buffer ovfl, nhits="<<TOF2Tovt::SumSHTh[crat][tsens-1]<<endl;
            }
// ---> D3,branch-1(FTZ-trig) :        
            if(upd31==0){// try set branch up
              if((tm-tmd31d)>daqp8){//ACTELoutp=SPTinp dead time check 
                upd31=1;  // set up state
		tmd31u=tm;//store up-time to use at self-reset(outpPW) check
              } 
            }
          }
//------------------------------------------
// try reset all branches:
//--------
// "LT-time TDC"-branche(#1) of Discr1
          if(upd11==1){ // branch is up - try reset it
            if(tm==tmd1d || i==imax){//reset(go-to-ready) on D1 "down"-edge or on "time-out"
	      upd11=0;
	      tmd11d=tm;//store down-time to use in next i-loop set-up (DT) check
	    }
          }
//--------
// "FTC-trig"-branch(#1) of Discr2(imply fixed outp.pulse width made by ACTEL)
          if(upd21==1){// branch is up - try reset it(check also discr-2 extra pulse during branch "up"-state)
	    maxtu=max(tmd21u,tmd2u);//latest between br21-up and d2-up
            if((tm-maxtu)>daqp0 || i==imax){//self-reset in daqp0 after the latest event: d2-up or br21-up
              upd21=0;
	      tmd21d=tm;//store down-time to use in next i-loop set-up stage
              if(_ntr1<TOF2GC::SCTHMX1){//store upto SCTHMX1 up/down-edges pairs
                _ttr1u[_ntr1]=tmd21u;//don't need accurate up-time for trigger
                if(i<imax)_ttr1d[_ntr1]=tmd21d;
		else _ttr1d[_ntr1]=maxtu+daqp0;//"internal time-out" case, use fixed pwid starting from
//                                         max(tmd21u,tmd2u)(if there is exra D2up during bran21 "up"state, tmd2u > tmd21u)
                _ntr1+=1;
              }
              else{
                cout<<"TOF2TovtN::build-W: HT(FTC)-buffer ovfl, nhits="<<_ntr1<<endl;
                upd21=2;//to block buffer input
              }
            }
          }
//--------
// "FTZ-trig"-branch(#1) of Discr3(imply fixed outp.pulse width made by ACTEL)
          if(upd31==1){// branch is up - try reset it(check also discr-3 extra pulse during branch "up"-state)
	    maxtu=max(tmd31u,tmd3u);//latest between br21-up and d2-up
            if((tm-maxtu)>daqp2 || i==imax){//self-reset in daqp0 after the latest event: d3-up or br31-up
              upd31=0;
	      tmd31d=tm;//store down-time to use in next i-loop set-up stage
              if(_ntr3<TOF2GC::SCTHMX1){//store upto SCTHMX1(=8) up/down-edges pairs
                _ttr3u[_ntr3]=tmd31u;//up-time
                if(i<imax)_ttr3d[_ntr3]=tmd31d;//down-time
		else _ttr3d[_ntr3]=maxtu+daqp2;//"internal time-out" case, use fixed pwid starting from
//                                         max(tmd31u,tmd3u)(if there is exra D3up during bran31 "up"state, tmd3u > tmd31u)
                _ntr3+=1;
              }
              else{
                cout<<"TOF2TovtN::build-W: FTZ-buffer ovfl, nhits="<<_ntr3<<endl;
                upd31=2;//to block buffer input
              }
            }
          }
//---------------------------        
        } //      --- end of time bin loop for time measurements --->
//---------------------------        
#ifdef _PGTRACK_
       char histn[1000];
       sprintf(histn,"Tof_MIP_Pulse_hight_%d",idd); 
       hman.Fill(histn,amx,1.);
#endif
       //HF1(idd+10000,amx,1.); //amx.spectrum   
       if(TFMCFFKEY.mcprtf[2]!=0){
	  if(idd==1041)HF1(1070,float(charge),1.);
	  for(i=0;i<_ntr1;i++){
	    HF1(1072,geant(_ttr1d[i]-_ttr1u[i]),1.);
	  }
	  for(i=0;i<_ntr3;i++){
	    HF1(1073,geant(_ttr3d[i]-_ttr3u[i]),1.);
	  }
	  HF1(1071,geant(_ntr1),1.);
	  HF1(1071,geant(_ntr3+10),1.);
	}
//**************************************************************************
        number ped,sig,gain,eqs;
	geant pmgn,dsum(0),asum(0),rrr;
	integer brtyp,npmts;
	brtyp=TOF2DBc::brtype(ilay,ibar)-1;//0-10
        npmts=TOFWScanN::scmcscan1[ilay][ibar].getnpmts();//real # of pmts per side
        _adca=0;
        _nadcd=0;
	for(int ipm=0;ipm<TOF2GC::PMTSMX;ipm++){
	  _adcd[ipm]=0.;
	}
//anode:
        adcs=charge;// Qa(pC)->Anode(adc,float)
        //cout<<"anode adc="<<adcs<<endl;
//----Anode Electronic saturation
        if(TFMCFFKEY.anodesat==1)adcs=pmsatur(adcs,ilay,ibar,isid);
        if(adcs>TOF2GC::SCPUXMX)adcs=TOF2GC::SCPUXMX;//PUX-chip saturation
        ped=TOFBPeds::scbrped[ilay][ibar].apeda(isid);// aver.ped in adc-chann. units(float)
        sig=TOFBPeds::scbrped[ilay][ibar].asiga(isid);// .... sig
        _adca=adcs+ped+sig*rnormx();// Anode adc+ped (float)
//	cout<<"  Aped/sig="<<ped<<" "<<sig<<" _adca="<<_adca<<endl;
//
//
//----Dynode:
	for(int ipm=0;ipm<npmts;ipm++){
          adcs=charged[ipm];//a(pC)->(adc,float)//Dynode same as anode
	  //cout<<"ipm="<<ipm<<" dynode adc="<<adcs<<endl;  
          if(adcs>TOF2GC::SCPUXMX)adcs=TOF2GC::SCPUXMX;//PUX-chip saturation
	  ped=TOFBPeds::scbrped[ilay][ibar].apedd(isid,ipm);
	  sig=TOFBPeds::scbrped[ilay][ibar].asigd(isid,ipm);
          _adcd[_nadcd]=adcs+ped+sig*rnormx();// Dynode(pm) adc+ped (float)
          _nadcd+=1;//really it is number of PMTs/side because all adcd's accepted
	}
//
//------------------------------
// now create/fill TOF2Tovt object (id=idd) with above digi-data:
//      if(_ntr1>0){// if counter_side FT-signal(HT) exists->create object
      if(_nstdc>0){//if counter_side LT-signal exists->create object 
        _sta=0;    
        stat=0;
	if(TFMCFFKEY.mcprtf[3]>1){
          cout<<"  --->TOFTovT-obj created for id="<<idd<<endl;
          cout<<"      nLThits="<<_nstdc<<endl;
          for(int ih=0;ih<_nstdc;ih++)cout<<"      "<<_tstdc[ih]<<endl;;
          cout<<"      nFThits="<<_ntr1<<endl;
          for(int ih=0;ih<_ntr1;ih++)cout<<"Tup/down="<<_ttr1u[ih]<<" "<<_ttr1d[ih]<<endl;
          cout<<"      nFTZhits="<<_ntr3<<endl;
          for(int ih=0;ih<_ntr3;ih++)cout<<"Tup/down="<<_ttr3u[ih]<<" "<<_ttr1d[ih]<<endl;
          cout<<"      adca="<<_adca<<" nadcd="<<_nadcd<<" "<<_adcd[0]<<" "<<_adcd[1]<<" "<<_adcd[2]<<endl;
	}
        if(AMSEvent::gethead()->addnext(AMSID("TOF2Tovt",ilay), new
             TOF2Tovt(idd,_sta,charge,edepb,
             _ntr1,_ttr1u,_ttr1d,_ntr3,_ttr3u,_ttr3d,
             _nstdc,_tstdc,
             _adca,
	     _nadcd,_adcd)))stat=1;
      }
      return;
//
}
