//  $Id: charge.C,v 1.62 2003/01/08 17:34:54 jorgec Exp $
// Author V. Choutko 5-june-1996
//
//
// Lat Edit : Mar 20, 1997. ak. AMSCharge::Build() check if psen == NULL
//
#include <beta.h>
#include <commons.h>
#include <math.h>
#include <limits.h>
#include <amsgobj.h>
#include <extC.h>
#include <upool.h>
#include <charge.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <amsstl.h>
#include <ntuple.h>
#include <cern.h>
#include <trrawcluster.h>
#include <job.h>
using namespace std;
integer AMSCharge::_sec[2]={0,0};
integer AMSCharge::_min[2]={0,0};
integer AMSCharge::_hour[2]={0,0};
integer AMSCharge::_day[2]={0,0};
integer AMSCharge::_mon[2]={0,0};
integer AMSCharge::_year[2]={97,96};
geant AMSCharge::_lkhdTOF[TOFTypes][ncharge][nbins];
geant AMSCharge::_lkhdTracker[TrackerTypes][ncharge][nbins];
geant AMSCharge::_lkhdStepTOF[TOFTypes][ncharge];
geant AMSCharge::_lkhdStepTracker[TrackerTypes][ncharge];
geant AMSCharge::_lkhdNormTOF[TOFTypes][ncharge];
geant AMSCharge::_lkhdNormTracker[TrackerTypes][ncharge];
geant AMSCharge::_lkhdSlopTOF[TOFTypes][ncharge];
geant AMSCharge::_lkhdSlopTracker[TrackerTypes][ncharge];
integer AMSCharge::_chargeTracker[ncharge]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeTOF[ncharge]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeRich[ncharge]={1,1,2,3,4,5,6,7,8,9};
char AMSCharge::_fnam[128]="lkhd_v216.data";

PROTOCCALLSFFUN2(FLOAT,PROB,prob,FLOAT,INT)
#define PROB(A2,A3)  CCALLSFFUN2(PROB,prob,FLOAT,INT,A2,A3)

number AMSCharge::getprobcharge(integer charge){
charge=abs(charge);
if(charge>_chargeTracker[ncharge-1]){
  cerr <<" AMSCharge::getprobcharge-E-charge too big "<<charge<<endl;
  return 0;
}
 int index;
 int voted=getvotedcharge(index);
 int i=charge;
 if(_ChargeTracker){
   if(voted<=CHARGEFITFFKEY.TrackerOnly && voted<=CHARGEFITFFKEY.TrackerProbOnly) {
     return _ProbTOF[i]*_ProbTracker[i]/_ProbTOF[index]/_ProbTracker[index];
   }
   else return _ProbTracker[i]/_ProbTracker[_iTracker];
 } else return _ProbTOF[i]/_ProbTOF[_iTOF];
}

integer AMSCharge::getvotedcharge(int & index){
  int i;
  int charge=0;

// Only tracker above this value
  if (_ChargeTOF>CHARGEFITFFKEY.TrackerOnly && _ChargeTracker){
    index=_iTracker;
    return _ChargeTracker;
  }
// Exclude bad measurement if possible
  number probmin=CHARGEFITFFKEY.ProbMin;
  int usetof=_ProbTOF[_iTOF]>probmin?1:0;
  int usetrk=_ProbTracker[_iTracker]>probmin?1:0;
  if(!_ChargeTracker) usetof=1;
  else if(!usetof && !usetrk){
    usetof=1;
    usetrk=1;
  }

  number minlkhd=FLT_MAX;
  for(i=0; i<ncharge; i++){
    number lkhdall=(usetof?_LkhdTOF[i]:0)+(usetrk?_LkhdTracker[i]:0);
    if(lkhdall<minlkhd){
      minlkhd=lkhdall;
      charge=_chargeTracker[i];
      index=i;
    }
  }
  return charge;
}


integer AMSCharge::build(integer refit){
  number etof[TOFMaxHits],etofd[TOFMaxHits],etrk[TrackerMaxHits];
  number EdepTOF[TOFTypes][TOFMaxHits],EdepTracker[TrackerTypes-1][TrackerMaxHits];
  AMSTOFCluster *pTOFc[TOFMaxHits];
  AMSTrCluster  *pTrackerc[TrackerTypes-1][TrackerMaxHits];
  integer TypeTOF[TOFMaxHits];
  integer TypeTracker[TrackerMaxHits];
  const number fac=AMSTrRawCluster::ADC2KeV();

// Temporary fix for simulation
  if (!AMSJob::gethead()->isRealData()){
    CHARGEFITFFKEY.ResCut[0]=-1.;     // no incompatible TOF clus exclusion
    CHARGEFITFFKEY.ResCut[1]=-1.;     // no incompatible Tracker clus exclusion
    CHARGEFITFFKEY.TrMeanRes=0;       // calculate truncated mean
    CHARGEFITFFKEY.ChrgMaxAnode=10;   // no use of dynodes for TOF charge 
    CHARGEFITFFKEY.BetaPowAnode=0;    // no corr. on anode beta dependence for z>1
    CHARGEFITFFKEY.TrackerForceSK=1;  // force tracker hit energies to be x+y
    CHARGEFITFFKEY.TrackerKSRatio=1.; // average x/y tracker energy ratio
    static integer first=1;
    if(first){
      first=0;
      cout<<"AMSCharge::build - MC forced with following datacards:"<<endl;
      cout<<" ResCut[0]: "<<CHARGEFITFFKEY.ResCut[0]<<endl
          <<" ResCut[1]: "<<CHARGEFITFFKEY.ResCut[1]<<endl
          <<" TrMeanRes: "<<CHARGEFITFFKEY.TrMeanRes<<endl
          <<" ChrgMaxAnode: "<<CHARGEFITFFKEY.ChrgMaxAnode<<endl
          <<" BetaPowAnode: "<<CHARGEFITFFKEY.BetaPowAnode<<endl
          <<" TrackerForceSK: "<<CHARGEFITFFKEY.TrackerForceSK<<endl
          <<" TrackerKSRatio: "<<CHARGEFITFFKEY.TrackerKSRatio<<endl;
    }
  }

  int patb;
  for(patb=0; patb<npatb; patb++){
    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){

// init
      integer nhitTOF=0, nhitTracker=0;
      integer nallTOF=0, nallTOFD=0, nallTracker=0;
      number expRich=0, useRich=0;
      AMSTrTrack *ptrack=pbeta->getptrack();
      number rid=ptrack->getrid();
      number beta=pbeta->getbeta();
      number theta, phi, sleng;
      AMSPoint P1;
      int i,j,weak;
      number pathcor;

// TOF hits
      weak=0;
      while(1){
        nhitTOF=0;
        nallTOF=0;
        nallTOFD=0;
        for(i=0; i<TOFMaxHits; i++){
          AMSTOFCluster *pcluster=pbeta->getpcluster(i);
          if(pcluster){
            number edep=pcluster->getedep();
            if(edep>0) etof[nallTOF++]=pcluster->getedep();
            number edepd=pcluster->getedepd();
            if(edepd>0) etofd[nallTOFD++]=pcluster->getedepd();
            if(pcluster->getnmemb()<=CHARGEFITFFKEY.NmembMax || weak){
              for(j=0; j<TOFTypes; j++) EdepTOF[j][nhitTOF]=0;
              AMSDir ScDir(0,0,1); // change when eugeni writes the proper code
              AMSPoint SCPnt=pcluster->getcoo();
              ptrack->interpolate(SCPnt, ScDir, P1, theta, phi, sleng);
              AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
              pathcor=fabs(ScDir.prod(DTr));
              pTOFc[nhitTOF]=pcluster;
              EdepTOF[0][nhitTOF]=edep>0?edep*pathcor:0;
              EdepTOF[1][nhitTOF]=edepd>0?edepd*pathcor:0;
              nhitTOF++;
            }
          }
        }
        if(nhitTOF<2){
          if(!weak && nallTOF) weak=1;
          else{
            cerr<<"AMSCharge::build -E- nallTOF = 0"<<endl;
            break;
          }
        }else break;
      }

// Tracker hits
      weak=0;
      while(1 ){
        nhitTracker=0;
        nallTracker=0;
        for(i=0; i<ptrack->getnhits(); i++){
          AMSTrRecHit *phit=ptrack->getphit(i);
          if(phit){
            if (phit->getpsen()){
              for(j=0; j<TrackerTypes-1; j++) EdepTracker[j][nhitTracker]=0;
              AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
               (phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2));
              AMSPoint SenPnt=phit->getHit();
              ptrack->interpolate(SenPnt, SenDir, P1, theta, phi, sleng);
              AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
              pathcor=fabs(SenDir.prod(DTr));
              AMSTrCluster *pcls;
              int etaok[2];
              for(j=0; j<2; j++){
                etaok[j]=0;
                pcls=phit->getClusterP(j);
                if(pcls){
                  number eta=max(pcls->geteta(),0.);
                  number etamin=CHARGEFITFFKEY.EtaMin[j];
                  number etamax=CHARGEFITFFKEY.EtaMax[j];
                  if( ((eta>etamin||etamin<=0)&&(eta<etamax||etamax>=1)) || weak)
                   etaok[j]=1;
                  pTrackerc[j][nhitTracker]=pcls;
                }
              }
              number sums=phit->getsonly()/2;
              number sumk=phit->getkonly()/2;
              if (sums>0) etrk[nallTracker++]=2*sums;
              if (etaok[0]) EdepTracker[0][nhitTracker]=sumk>0?fac*sumk*pathcor:0;
              if (etaok[1]) EdepTracker[1][nhitTracker]=sums>0?fac*sums*pathcor:0;
              nhitTracker++;
            }
            else{
             cout<<"AMSCharge::build -E- phit -> getpsen == NULL "<<" for hit wit pos "<<phit ->getpos();
            }
          }
        }
        if(nhitTracker<2){
          if(!weak && nallTracker) weak=1;
          else{
//            cerr<<"AMSCharge::build -E- nallTracker = 0"<<endl;
            break;
          }
        }else break;
      }

// RICH Ring
      AMSRichRing *pring=NULL;
      AMSRichRing *prcri=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
      while(prcri){
       AMSTrTrack *prctk=prcri->gettrack();
       if(prctk==ptrack){
         pring=prcri;
         expRich=pring->getnpexp();
         useRich=pring->getcollnpe();
       }
       prcri=prcri->next(); 
      }
      

// Compute truncated means
      int imx;
      number TrMeanTOF, TrMeanTOFD, TrMeanTracker;
      number resmx,mean,trunres,trunmax,rescut;
      rescut=CHARGEFITFFKEY.ResCut[0];
      resmx=resmax(etof,nallTOF,0,rescut,imx,mean,trunres,trunmax);
      if(!CHARGEFITFFKEY.TrMeanRes) TrMeanTOF=trunmax;
      else TrMeanTOF=trunres;
      rescut=CHARGEFITFFKEY.ResCut[1];
      resmx=resmax(etrk,nallTracker,0,rescut,imx,mean,trunres,trunmax);
      if(!CHARGEFITFFKEY.TrMeanRes) TrMeanTracker=trunmax;
      else TrMeanTracker=trunres;
      resmx=resmax(etofd,nallTOFD,0,rescut,imx,mean,trunres,trunmax);
      TrMeanTOFD=mean;

// Add new entry
      addnext(pbeta,pring,nhitTOF,nhitTracker,pTOFc,EdepTOF,pTrackerc,EdepTracker,TrMeanTracker,TrMeanTOF,TrMeanTOFD,expRich,useRich);
      pbeta=pbeta->next();
    }    
  }
  return 1;
}


void AMSCharge::addnext(AMSBeta *pbeta, AMSRichRing *pring, integer nhitTOF, integer nhitTracker, AMSTOFCluster *pTOFc[], number EdepTOF[TOFTypes][TOFMaxHits], AMSTrCluster  *pTrackerc[TrackerTypes-1][TrackerMaxHits], number EdepTracker[TrackerTypes-1][TrackerMaxHits], number trtr, number trtof, number trtofd, number expRich, number useRich){
  AMSCharge *pcharge=new AMSCharge(pbeta, pring, trtr, trtof, trtofd);
  number beta=pbeta->getbeta();
  int bstatus=!pbeta->checkstatus(AMSDBc::AMBIG);
  int tofok=pcharge->FitTOF(0,beta,bstatus,nhitTOF,pTOFc,EdepTOF);
  int trkok=pcharge->FitTracker(0,beta,bstatus,nhitTracker,pTrackerc,EdepTracker);
  int ricok=pcharge->FitRich(0,expRich,useRich);
  number probmin=CHARGEFITFFKEY.ProbMin;
  int tofgood=pcharge->_ProbTOF[pcharge->_iTOF]>probmin?1:0;
  int trkgood=pcharge->_ProbTracker[pcharge->_iTracker]>probmin?1:0;
  if((!tofok&&!trkok) || (!tofgood&&!trkgood)) pcharge->setstatus(AMSDBc::BAD);
  AMSEvent::gethead()->addnext(AMSID("AMSCharge",0),pcharge);
}

integer AMSCharge::FitTOF(int toffit, number beta, int bstatus, int nhitTOF, AMSTOFCluster *pTOFc[], number etof[TOFTypes][TOFMaxHits]){
  static number ETOF[TOFTypes][TOFMaxHits];
  int typetof[TOFMaxHits], nhittoftyp[TOFTypes];
  number TOFresmax[TOFTypes], etofh[TOFMaxHits], x[TOFMaxHits];
  int TOFhitmax[TOFTypes];
  int i,j;

// init
  if (!toffit){
    _iTOF=0;
    _ChargeTOF=0;
    for(i=0; i<ncharge; i++) _ProbTOF[i]=0;
    for(i=0; i<ncharge; i++) _IndxTOF[i]=i;
    UCOPY(etof[0],ETOF[0],TOFTypes*TOFMaxHits*sizeof(etof[0][0])/4);
  }

// determine furthest hits
  if(!toffit){
    number rescut=CHARGEFITFFKEY.ResCut[0];
    for(i=0; i<TOFTypes; i++){
      number mean,trunres,trunmax;
      int hitmax;
      for(j=0; j<nhitTOF; j++) x[j]=etof[i][j];
      TOFresmax[i]=resmax(x,nhitTOF,toffit,rescut,hitmax,mean,trunres,trunmax);
      TOFhitmax[i]=hitmax;
    }
    for(i=0; i<TOFTypes; i++){
      for(j=0; j<nhitTOF; j++) if(j==TOFhitmax[i]) etof[i][j]=0;
    }
  }

// Use either Anode or Dynode energies
  int failtof=0;
  int nhittof=0, nhittofd=0;
  for(i=0; i<TOFTypes; i++) nhittoftyp[i]=0;
  for(i=0; i<nhitTOF; i++){
    if (etof[1][i]>0) nhittofd++;
    typetof[i]=-1;
    if(toffit>0 && etof[1][i]>0){
      typetof[i]=1;
      etofh[i]=etof[1][i];
    }
    else if(toffit<=0 && etof[0][i]>0){
      typetof[i]=0;
      etofh[i]=etof[0][i];
    }
    if(typetof[i]>=0){
      nhittoftyp[typetof[i]]++;
      if(toffit>=0 && bstatus) pTOFc[i]->setstatus(AMSDBc::CHARGEUSED);
      nhittof++;
    }
  }
  if(!nhittof) failtof=1;

  if(!failtof){

// likelihood values and charge probabilities
    number lkhtof[ncharge];
    lkhcalc(0,beta,nhitTOF,etofh,typetof,lkhtof);
    number probtof=_probcalc(0,toffit,nhittoftyp,lkhtof);

// refit using dynodes if required
    _ChargeTOF=_chargeTOF[_iTOF];
    if (!toffit && _ChargeTOF>CHARGEFITFFKEY.ChrgMaxAnode && nhittofd){
      for(i=0; i<nhitTOF; i++)
       if(typetof[i]>=0 && bstatus) pTOFc[i]->clearstatus(AMSDBc::CHARGEUSED);
      toffit++;
      failtof=!FitTOF(toffit,beta,bstatus,nhitTOF,pTOFc,ETOF);
    }
    else if(toffit>0) setstatus(AMSDBc::REFITTED);
  }
  else{
    cerr<<"AMSCharge::Fit -E- no TOF cluster found"<<endl;
  }

  return !failtof;
}

 
integer AMSCharge::FitTracker(int trkfit, number beta, int bstatus, int nhitTracker, AMSTrCluster  *pTrackerc[TrackerTypes-1][TrackerMaxHits], number etrk[TrackerTypes-1][TrackerMaxHits]){
  static number ETRK[TrackerTypes-1][TrackerMaxHits];
  int typetrk[TrackerMaxHits], nhittrktyp[TrackerTypes];
  number Trackerresmax[TrackerTypes-1], etrkh[TrackerMaxHits], x[TrackerMaxHits];
  int Trackerhitmax[TrackerTypes-1];
  int i,j;

// init
  if(!trkfit){
    _iTracker=0;
    _ChargeTracker=0;
    _ProbAllTracker=0;
    for(i=0; i<ncharge; i++) _ProbTracker[i]=0;
    for(i=0; i<ncharge; i++) _IndxTracker[i]=i;
    UCOPY(etrk[0],ETRK[0],(TrackerTypes-1)*TrackerMaxHits*sizeof(etrk[0][0])/4);
  }

// determine furthest hits
  if(trkfit>=0){
    number rescut=!trkfit?CHARGEFITFFKEY.ResCut[1]:0;
    for(i=0; i<TrackerTypes-1; i++){
      number mean,trunres,trunmax;
      int hitmax;
      for(j=0; j<nhitTracker; j++) x[j]=etrk[i][j];
      Trackerresmax[i]=resmax(x,nhitTracker,trkfit,rescut,hitmax,mean,trunres,trunmax);
      Trackerhitmax[i]=hitmax;
    }
    for(i=0; i<TrackerTypes-1; i++){
      for(j=0; j<nhitTracker; j++) if(j==Trackerhitmax[i]) etrk[i][j]=0;
    }
  }

// Use either S+K, S or K energies
  int failtrk=0;
  int nhittrk=0;
  for(i=0; i<TrackerTypes; i++) nhittrktyp[i]=0;
  for(i=0; i<nhitTracker; i++){
    typetrk[i]=-1;
    if (CHARGEFITFFKEY.TrackerForceSK){
      if(etrk[1][i]>0){
        typetrk[i]=2;
        etrkh[i]=etrk[0][i]>0?etrk[1][i]+etrk[0][i]:etrk[1][i]*(1.+CHARGEFITFFKEY.TrackerKSRatio);
        if(trkfit>=0 && bstatus){
          pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
          if(etrk[0][i]>0) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
        }
      }
    }
    else{
      if (CHARGEFITFFKEY.Tracker>0 && etrk[1][i]>0 && etrk[0][i]>0){
        typetrk[i]=2;
        etrkh[i]=etrk[1][i]+etrk[0][i];
        if(trkfit>=0 && bstatus) pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
        if(trkfit>=0 && bstatus) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
      }
      else if(etrk[1][i]>0){
        typetrk[i]=0;
        etrkh[i]=etrk[1][i];
        if(trkfit>=0 && bstatus) pTrackerc[1][i]->setstatus(AMSDBc::CHARGEUSED);
      }
      else if(CHARGEFITFFKEY.Tracker>1 && etrk[0][i]>0){
        typetrk[i]=1;
        etrkh[i]=etrk[0][i];
        if(trkfit>=0 && bstatus) pTrackerc[0][i]->setstatus(AMSDBc::CHARGEUSED);
      }
    }
    if(typetrk[i]>=0){
      nhittrktyp[typetrk[i]]++;
      nhittrk++;
    }
  }
  if(!nhittrk) failtrk=1;

  if(!failtrk){

// likelihood values and charge probabilities
    number lkhtrk[ncharge];
    lkhcalc(1,beta,nhitTracker,etrkh,typetrk,lkhtrk);
    number probtrk=_probcalc(1,trkfit,nhittrktyp,lkhtrk);

// refit tracker if required
    _ChargeTracker=_chargeTracker[_iTracker];
    if(!trkfit && probtrk<CHARGEFITFFKEY.ProbTrkRefit){
      for(i=0; i<nhitTracker; i++){
        if(etrk[1][i]>0 && (typetrk[i]==0 || typetrk[i]==2) && bstatus)
         pTrackerc[1][i]->clearstatus(AMSDBc::CHARGEUSED);
        if(etrk[0][i]>0 && (typetrk[i]==1 || typetrk[i]==2) && bstatus)
         pTrackerc[0][i]->clearstatus(AMSDBc::CHARGEUSED);
      }
      trkfit++;
      failtrk=!FitTracker(trkfit,beta,bstatus,nhitTracker,pTrackerc,etrk);
    }
    else if(trkfit>0) setstatus(AMSDBc::REFITTED);
    else _ProbAllTracker=probtrk;

// get tracker probability using all the hits
    if(trkfit!=-1) failtrk=!FitTracker(-1,beta,bstatus,nhitTracker,pTrackerc,ETRK);
  }
  else{
    //cerr<<"AMSCharge::Fit -E- no Tracker hit found"<<endl;
  }

  return !failtrk;
}

integer AMSCharge::FitRich(int ricfit, number expRich, number useRich){
  number lkhrich[ncharge],probrich;
  int i;
  int failrich=0;

// init
  if(!ricfit){
    _iRich=0;
    _ChargeRich=0;
    for(i=0; i<ncharge; i++) _ProbRich[i]=0;
    for(i=0; i<ncharge; i++) _IndxRich[i]=i;
  }

  if(expRich>0){
   for (i=0; i<ncharge; i++){
    number zz=_chargeRich[i]*_chargeRich[i];
    lkhrich[i]=expRich*zz-useRich*log(expRich*zz);}
   probrich=_probrich(expRich,useRich,lkhrich);
   _ChargeRich=_chargeRich[_iRich];}
  else failrich=1;

  return !failrich;
}

void AMSCharge::lkhcalc(int mode, number beta, int nhit, number ehit[], int typeh[], number lkh[]){
  number betamax=0.95;
  int i,j;

  for(i=0; i<ncharge; i++){
    lkh[i]=0;
    number chg=mode?_chargeTracker[i]:_chargeTOF[i];
    for(j=0; j<nhit; j++){
      int type=typeh[j];
      if(type>=0){
        number step,slop,norm,betapow,betacor;
        step=mode?_lkhdStepTracker[type][i]:_lkhdStepTOF[type][i];
        slop=mode?_lkhdSlopTracker[type][i]:_lkhdSlopTOF[type][i];
        norm=mode?_lkhdNormTracker[type][i]:_lkhdNormTOF[type][i];
        betapow=(!mode && !type && chg>1 && CHARGEFITFFKEY.BetaPowAnode)?7./6:5./3;
        betapow=5./3;
        betacor=i?pow(min(fabs(beta/betamax),1.),betapow):1;
        int ia=(int)floor(ehit[j]*betacor/step);
        if(ia<0) ia=0;
        number cor=0;
        if(ia>=nbins){
          cor=(ia+1-nbins)*slop*step;
          ia=nbins-1;
        }
        number fac=0;
        if(CHARGEFITFFKEY.PdfNorm) fac=log(norm);
        number fval=mode?_lkhdTracker[type][i][ia]:_lkhdTOF[type][i][ia];
        lkh[i]+=-log(fval)+fac+cor;
      }
    }
  }

}

number AMSCharge::resmax(number x[],int ntot,int refit,number rescut,int &imax,number &mean,number &trres,number &trmax){
  int i,j,n;
  imax=-1;
  mean=ntot?x[0]:0;
  trres=ntot?x[0]:0;
  trmax=ntot?x[0]:0;

  if(ntot<2) return 0;

  number sigmin=CHARGEFITFFKEY.SigMin;
  number sigmax=refit?1:CHARGEFITFFKEY.SigMax;
  assert(sigmin>0 && sigmax>0 && sigmax>sigmin);

  number rsmx=0;
  for(i=0; i<ntot; i++){
    if (x[i]>0){
      number xm=0, xxm=0;
      n=0;
      for(j=0; j<ntot; j++){
        if(j!=i && x[j]>0){
          n++;
          xm+=x[j];
          xxm+=pow(x[j],2);
        }
      }
      if (n>1){
        number sig,rs;
        sig=sqrt(max((n*xxm-pow(xm,2))/n/(n-1),0.));
        sig=max(sig,sigmin*xm/n);
        sig=min(sig,sigmax*xm/n);
        rs=(x[i]-xm/n)/sig;
        if (fabs(rs)>rsmx){
          rsmx=fabs(rs);
          imax=i;
        }
      }
    }
  }

  int cut=(rsmx>rescut&&rescut>=0)?1:0;
  imax=cut?imax:-1;

  mean=0;
  trres=0;
  n=0;
  number xmx=0;
  for(i=0; i<ntot; i++){
    if(x[i]>0){
      n++;
      mean+=x[i];
      if(i!=imax) trres+=x[i];
      if(x[i]>xmx) xmx=x[i];
    }
  }

  trmax=mean;
  if(n>1) {
    mean=mean/n;
    trmax=(trmax-xmx)/(n-1);
    trres=cut?trres/(n-1):trres/n;
  }

  return rsmx;
}


number AMSCharge::_probcalc(int mode, int fit, int nhittyp[],number lkhd[]){
  number prob[ncharge];
  int i;

  if(fit>=0){
    if(!mode){ 
      for(i=0; i<ncharge; i++) _LkhdTOF[i]=lkhd[i];
      _iTOF=_sortlkhd(mode);
    }
    else{ 
      for(i=0; i<ncharge; i++) _LkhdTracker[i]=lkhd[i];
      _iTracker=_sortlkhd(mode);
    }
  }

  int types=mode?TrackerTypes:TOFTypes;

  int nhit=0;
  for(i=0; i<types; i++) nhit+=nhittyp[i];

  number probmx=1;
  if (CHARGEFITFFKEY.PdfNorm){
    probmx=0.;
    for(i=0; i<types; i++){
      number lkhdnorm=mode?_lkhdNormTracker[i][_iTracker]:_lkhdNormTOF[i][_iTOF];
      if (nhittyp[i]>0) probmx+=nhittyp[i]*log(lkhdnorm);
    }
    probmx=1./exp(min(probmx/nhit,powmx));
  }

  for(i=0; i<ncharge; i++) prob[i]=1./exp(min(lkhd[i]/nhit,powmx))/probmx;

  if(fit>=0){
    if(!mode){ for(i=0; i<ncharge; i++) _ProbTOF[i]=prob[i];}
    else{ for(i=0; i<ncharge; i++) _ProbTracker[i]=prob[i];}
  }

  int index=mode?_iTracker:_iTOF;
  return prob[index];
} 

number AMSCharge::_probrich(number expRich, number useRich, number lkhd[]){
  number prob[ncharge];
  int i;

  for(i=0; i<ncharge; i++) _LkhdRich[i]=lkhd[i];
  _iRich=_sortlkhd(2);

  for (i=0; i<ncharge; i++){
    number zz=_chargeRich[i]*_chargeRich[i];
    number f=(useRich-zz*expRich)*(useRich-zz*expRich)/zz/expRich;
    _ProbRich[i]=PROB((geant)f,1);
  }

  return _ProbRich[_iRich];
}

int AMSCharge::_sortlkhd(int sort){
  number lkhd[ncharge];
  number *pntr[ncharge];
  int index[ncharge];
  int i,j,imax;

  if(!sort){ for(i=0; i<ncharge; i++) lkhd[i]=_LkhdTOF[i];}
  else if (sort==1){ for(i=0; i<ncharge; i++) lkhd[i]=_LkhdTracker[i];}
  else { for(i=0; i<ncharge; i++) lkhd[i]=_LkhdRich[i];}

  for (i=0; i<ncharge; i++) pntr[i]=&lkhd[i];

  AMSsortNAG(pntr,ncharge);

  for(i=0; i<ncharge; i++) index[i]=-1;
  for(i=0; i<ncharge; i++){
    for(j=0; j<ncharge; j++){
      if (lkhd[j]==(*pntr[i])) {
        index[j]=i;
        lkhd[j]=-999;
        if(!i) imax=j;
        break;
      }
    }
    if(index[j]==-1) {
      cerr << " AMSCharge::sortlkhd-E-badly sorted "<<endl;
      return 0;
    }
  }

  if (!sort) { for(i=0; i<ncharge; i++) _IndxTOF[i]=index[i]; }
  else if (sort==1){ for(i=0; i<ncharge; i++) _IndxTracker[i]=index[i]; }
  else{ for(i=0; i<ncharge; i++) _IndxRich[i]=index[i]; }

  return imax;
}


void AMSCharge::_writeEl(){

   int i,j;
#ifdef __WRITEROOTCLONES__
    float probtof[4];
    int   chintof[4];
    float probtr[4];
    int   chintr[4];
    float probrc[4];
    int   chinrc[4];
    float proballtr;
  for(i=0; i<4; i++){
    for(j=0; j<ncharge; j++){
      if(_IndxTOF[j]==i){
        probtof[i]=_ProbTOF[j];
        chintof[i]=j+1;
      }
      if(_IndxTracker[j]==i){
        probtr[i]=_ProbTracker[j];
        chintr[i]=j+1;
        if(!i) proballtr=_ProbAllTracker;
      }
      if(_IndxRich[j]==i){
        probrc[i]=_ProbRich[j];
        chinrc[i]=j+1;
      }
    }
  }
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,
     probtof, chintof, probtr, chintr, probrc, chinrc, proballtr);
#endif
  ChargeNtuple02* CN = AMSJob::gethead()->getntuple()->Get_charge02();

  if (CN->Ncharge>=MAXCHARGE02) return;
// Fill the ntuple
  CN->Status[CN->Ncharge]=_status;
  CN->BetaP[CN->Ncharge]=_pbeta->getpos();
  CN->RingP[CN->Ncharge]=_pring?_pring->getpos():-1;
  CN->ChargeTOF[CN->Ncharge]=_ChargeTOF;
  CN->ChargeTracker[CN->Ncharge]=_ChargeTracker;
  CN->ChargeRich[CN->Ncharge]=_ChargeRich;
  for(i=0; i<4; i++){
    for(j=0; j<ncharge; j++){
      if(_IndxTOF[j]==i){
        CN->ProbTOF[CN->Ncharge][i]=_ProbTOF[j];
        CN->ChInTOF[CN->Ncharge][i]=j+1;
      }
      if(_IndxTracker[j]==i){
        CN->ProbTracker[CN->Ncharge][i]=_ProbTracker[j];
        CN->ChInTracker[CN->Ncharge][i]=j+1;
        if(!i) CN->ProbAllTracker[CN->Ncharge]=_ProbAllTracker;
      }
      if(_IndxRich[j]==i){
        CN->ProbRich[CN->Ncharge][i]=_ProbRich[j];
        CN->ChInRich[CN->Ncharge][i]=j+1;
      }
    }
  }
  CN->TrunTOF[CN->Ncharge]=_TrMeanTOF;
  CN->TrunTOFD[CN->Ncharge]=_TrMeanTOFD;
  CN->TrunTracker[CN->Ncharge]=_TrMeanTracker;
  CN->Ncharge++;
}


void AMSCharge::_copyEl(){
#ifdef __WRITEROOT__
  ChargeRoot02 *cptr = (ChargeRoot02*)_ptr;
  if (cptr) {
    // AMSBeta* _pbeta;
    if (_pbeta) cptr->fBeta=_pbeta->GetClonePointer();
    if (_pring) cptr->fRich=_pring->GetClonePointer();
  } else {
    cout<<"AMSCharge::_copyEl -I-  AMSCharg::ChargeRoot02 *ptr is NULL "<<endl;
  }
#endif

}


void AMSCharge::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCharge",0);
 if(p)p->printC(cout);
}


void AMSCharge::init(){
  //#ifdef __ALPHA__
  //  // yet another dec cxx compiler bug
  //  AMSCommonsI cmni;
  //  cmni.init();
  //#endif
int typ,ich,bin,j;
char fnam[256]="";                 
strcpy(fnam,AMSDATADIR.amsdatadir);
strcat(fnam,_fnam);
if(AMSJob::gethead()->isRealData())strcat(fnam,".1");
else strcat(fnam,".0");
  ifstream iftxt(fnam,ios::in);
  if(!iftxt){
     cerr <<"AMSCharge::init-F-Error open file "<<fnam<<endl;
     exit(1);
  }
  else cout <<"AMSCharge::init-I-Open file "<<fnam<<endl;

// Read TOF pdfs
  for(typ=0; typ<TOFTypes; typ++){
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdStepTOF[typ][ich];
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdNormTOF[typ][ich];
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdSlopTOF[typ][ich];
    for(ich=0; ich<ncharge; ich++){
      for(bin=0; bin<nbins; bin++) iftxt >> _lkhdTOF[typ][ich][bin];
      if(iftxt.eof()){
        cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
        exit(1);
      }
    }
  }

// Read Tracker pdfs
  for(typ=0; typ<TrackerTypes; typ++){
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdStepTracker[typ][ich];
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdNormTracker[typ][ich];
    for(ich=0; ich<ncharge; ich++) iftxt >> _lkhdSlopTracker[typ][ich];
    for(ich=0; ich<ncharge; ich++){
      for(bin=0; bin<nbins; bin++) iftxt >> _lkhdTracker[typ][ich][bin];
      if(iftxt.eof()){
        cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
        exit(1);
      }
    }
  }

// Validity time
  for(j=0;j<2;j++) iftxt >> _sec[j];
  if(iftxt.eof()){
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  for(j=0;j<2;j++) iftxt >> _min[j];
  if(iftxt.eof()){ 
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  for(j=0;j<2;j++) iftxt >> _hour[j];
  if(iftxt.eof()){
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  for(j=0;j<2;j++) iftxt >> _day[j];
  if(iftxt.eof()){
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  for(j=0;j<2;j++) iftxt >> _mon[j];
  if(iftxt.eof()){
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  for(j=0;j<2;j++) iftxt >> _year[j];
  if(iftxt.eof()){
    cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
    exit(1);
  }

  iftxt.close();

  cout << "AMSCharge::init()-I-Completed"<<endl;
}


AMSChargeI::AMSChargeI(){
  // if(_Count++==0)AMSCharge::init();
}
integer AMSChargeI::_Count=0;

