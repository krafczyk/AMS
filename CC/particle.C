
// Author V. Choutko 6-june-1996
 
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
#include <particle.h>
#include <mccluster.h>
#include <amsstl.h>
#include <ctcdbc.h>
#include <ntuple.h>
#include <antirec.h>
#include <mceventg.h>


extern "C" void atcrec_(const integer & run, integer & nctcht, geant cooctc[1][2][3], int la[], int co[], int ro[], geant sig[],
                   int atcnbcel[], geant atcnbphe[], int atcidcel[], int atcdispm[], int atcdaero[], int atcstatu[], float & atcbeta);


PROTOCCALLSFFUN2(FLOAT,PROB,prob,FLOAT,INT)
#define PROB(A2,A3)  CCALLSFFUN2(PROB,prob,FLOAT,INT,A2,A3)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void *alfun, void * monit, int & maxcal, int &ifail, void * p);

integer sign(number a){
if(a>=0)return 1;
else return -1;
}

integer AMSParticle::build(integer refit){
  //Particle mass rec, momentum etc.
     number mass,emass,charge,momentum,emomentum,theta(0),phi(0);
     AMSPoint coo;
     AMSParticle * ppart(0);
     AMSAntiMCCluster * pcl(0);
     AMSCharge *pcharge=(AMSCharge*)AMSEvent::gethead()->
     getheadC("AMSCharge",0);
      while(pcharge) {
        {
          AMSBeta * pbeta=pcharge->getpbeta();
          AMSTrTrack * ptrack=pbeta->getptrack();
#ifdef __AMSDEBUG__
          assert (ptrack && pbeta && pcharge);
#endif   
         
          if(pbeta->checkstatus(AMSDBc::AMBIG)==0 ||BETAFITFFKEY.FullReco ){
          number chi2(0),rid(0),err(0);
          ptrack->getParFastFit( chi2, rid, err, theta,  phi,  coo);
          // Add new element
          
          int index;
          charge=pcharge->getvotedcharge(index);
          _build(rid,err,charge,pbeta,mass,emass,momentum,emomentum);
          ppart=new AMSParticle(pbeta, pcharge, ptrack,
          mass,emass,momentum,emomentum,charge,theta,phi,coo);
          ptrack->setstatus(AMSDBc::USED);
          AMSgObj::BookTimer.start("ReAxPid");
          ppart->pid();
          AMSgObj::BookTimer.stop("ReAxPid");
           
          AMSgObj::BookTimer.start("ReAxRefit");
           ppart->refit(AMSJob::gethead()->isCalibration() & AMSJob::CTracker);
          if(!(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)){
           if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
            ppart->ctcfit();
           }
           ppart->toffit();
           ppart->antifit();
          }
          AMSgObj::BookTimer.stop("ReAxRefit");
          AMSEvent::gethead()->addnext(AMSID("AMSParticle",ppart->contnumber()),ppart);
          }
        }
out:
       pcharge=pcharge->next();
      }
      return 1;
}

void  AMSParticle::_build(number rid,number err,number charge,AMSBeta * pbeta,number & mass,number & emass,number & momentum,number & emomentum){
          number beta=pbeta->getbeta();
          number ebeta=pbeta->getebeta()*beta*beta;
          momentum=rid*charge;
          emomentum=err*rid*rid*charge;
          if(fabs(beta)<=1.e-10 ){
           cerr<< " beta too low " <<beta<<endl;
           mass=FLT_MAX;
           emass=FLT_MAX;
          }
          else{ 
           number one=1;
           number xb=1/fabs(beta);
           if(xb<one)xb=2-xb;
           number gamma2=one/(one-beta*beta); 
           number mass2=momentum*momentum*(xb*xb-one);
           mass=gamma2>0? sqrt(mass2) : -sqrt(mass2);
           emass=fabs(mass)*sqrt((emomentum/momentum)*(emomentum/momentum)+
           (gamma2*ebeta/beta)*(gamma2*ebeta/beta));
          }
          if(beta<0)momentum=-momentum;
}

void AMSParticle::ctcfit(){
if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
  AMSPoint SearchReg(BETAFITFFKEY.SearchReg[0],BETAFITFFKEY.SearchReg[1],
  BETAFITFFKEY.SearchReg[2]);
AMSDir dir(0,0,1.);
AMSPoint outp(0,0,0);
number theta, phi, sleng;
number signal,beta,ebeta;
  // Find CTC hits 
integer kk;
for(kk=0;kk<CTCDBc::getnlay();kk++){  
  AMSCTCCluster *pctc=(AMSCTCCluster*)AMSEvent::gethead()->
  getheadC("AMSCTCCluster",kk);
  while(pctc){
    if(pctc->checkstatus(AMSDBc::USED)==0 ){
        AMSPoint coo=pctc->getcoo();
        _ptrack->interpolate(coo,dir,outp,theta,phi,sleng);
        if(((outp-coo)/pctc->getecoo()).abs() < SearchReg)break;
    }
    pctc=pctc->next();
  }

 if(pctc){
      pctc->setstatus(AMSDBc::USED);
      pctc->correct(cos(theta));
      signal=pctc->getsignal()/_Charge/_Charge;
 number error=pctc->geterror()/_Charge/_Charge;
 number A1=CTCMCFFKEY.Path2PhEl[0];
 number A2=CTCMCFFKEY.Path2PhEl[1];
 number N1=CTCMCFFKEY.Refraction[0];
 number N2=CTCMCFFKEY.Refraction[1];
 number L1=CTCDBc::getagsize(2);
 number L2=CTCDBc::getgeom()? 0 : CTCDBc::getwlsth(); 
// no wls in vertical readout
 number b2=(A1*L1/N1/N1+A2*L2/N2/N2)/(A1*L1+A2*L2-signal);
   if(b2 < 0){
    beta=0;
    ebeta=1;
   }
   else{
    beta=sqrt(b2);
    b2=(A1*L1/N1/N1+A2*L2/N2/N2)/(A1*L1+A2*L2-signal+error);
    ebeta=beta-sqrt(b2);
   }
 }
 else{
   // No hits, but still TrackExtrapolation needed
   signal=0;
   beta=0;
   ebeta=1;

   AMSCTCRawHit d(1,kk+1,1);
   AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(2));
   if(p)
   _ptrack->interpolate(p->loc2gl(AMSPoint(0,0,0)),dir,outp,theta,phi,sleng);
   else {
   cerr << " ctcfit-S- No layer no " << kk+1<<endl ;
   outp=AMSPoint(0,0,0);
   }
}
    _Value[kk]=CTC(signal,beta,ebeta, outp);
    _pctc[kk]=pctc;
}

}
else{
     AMSPoint outp(0,0,0);
    _Value[0]=CTC(0,0,0,outp);
    _Value[1]=CTC(0,0,0,outp);
    _pctc[0]=0;
    _pctc[1]=0;
}
}




void AMSParticle::toffit(){
AMSDir dir(0,0,1.);
number theta, phi, sleng;
number signal,beta,ebeta;
  // Find TOF hits
  integer ntof;
  integer fitdone[4]={0,0,0,0};
  if(_pbeta->getpattern()==0){
    ntof=4;
  }
  else if(_pbeta->getpattern()<5){
    ntof=3;
  }
  else {
   ntof=2;
  }
   
integer kk;
for(kk=0;kk<ntof;kk++){
  AMSTOFCluster* pcl=_pbeta->getpcluster(kk);
  _ptrack->interpolate(pcl->getcoo(),dir,_TOFCoo[pcl->getntof()-1],theta,phi,sleng);    
 fitdone[pcl->getntof()-1]=1;
}
   // No hits, but still TrackExtrapolation needed
for(kk=0;kk<4;kk++){
  if(!fitdone[kk]){
   AMSTOFCluster d(0,kk+1,1);
   AMSgvolume *p=AMSJob::gethead()->getgeomvolume(d.crgid());
   if(p)
   _ptrack->interpolate(p->loc2gl(AMSPoint(0,0,0)),dir,_TOFCoo[kk],theta,phi,sleng);
   else {
   cerr << " toffit-S- No layer no " << kk+1<<endl ;
   _TOFCoo[kk]=AMSPoint(0,0,0);
   }

  }
}
}



void AMSParticle::antifit(){
number theta, phi, sleng;
for(int kk=0;kk<2;kk++){
   AMSAntiCluster d(0,1);
   AMSgvolume *p=AMSJob::gethead()->getgeomvolume(d.crgid());
   if(p){
      AMSPoint coo(p->getcooA(0),p->getcooA(1),p->getcooA(2));
      number rad=(p->getpar(0)+p->getpar(1))/2.;
      AMSDir dir(p->getnrmA(2,0),p->getnrmA(2,1),p->getnrmA(2,2));
     _ptrack->interpolateCyl(coo,dir,rad,2*kk-1,_AntiCoo[kk],theta,phi,sleng);
   }
   else {
   cerr << " antifit-S- No sector no " << kk+1<<endl ;
   _AntiCoo[kk]=AMSPoint(0,0,0);
   }

}
}






void AMSParticle::_writeEl(){


if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){

  ParticleNtuple* PN = AMSJob::gethead()->getntuple()->Get_part();
  if (PN->Npart>=MAXPART) return;
  if((AMSEvent::gethead()->getC("AMSParticle",0)->getnelem()>0 || LVL3FFKEY.Accept) && _ptrack->checkstatus(AMSDBc::NOTRACK))return;
// Fill the ntuple 
  PN->ChargeP[PN->Npart]=_pcharge->getpos();
  PN->BetaP[PN->Npart]=_pbeta->getpos();
  integer pat=_pbeta->getpattern();
  int i;
  for(i=0;i<pat;i++){
    AMSContainer *pc=AMSEvent::gethead()->getC("AMSBeta",i);
    #ifdef __AMSDEBUG__
      assert(pc != NULL);
    #endif
    PN->BetaP[PN->Npart]+=pc->getnelem();
  }
 
  pat=_ptrack->getpattern();
  if(_ptrack->checkstatus(AMSDBc::NOTRACK))PN->TrackP[PN->Npart]=-1;
  else PN->TrackP[PN->Npart]=_ptrack->getpos();
  PN->Particle[PN->Npart]=_gpart[0];
  PN->ParticleVice[PN->Npart]=_gpart[1];
  PN->FitMom[PN->Npart]=_fittedmom[0];
 for(i=0;i<2;i++){
  PN->Prob[PN->Npart][i]=_prob[i];
 }
  PN->Mass[PN->Npart]=_Mass;
  PN->ErrMass[PN->Npart]=_ErrMass;
  PN->Momentum[PN->Npart]=_Momentum;
  PN->ErrMomentum[PN->Npart]=_ErrMomentum;
  PN->Charge[PN->Npart]=_Charge;
  PN->Theta[PN->Npart]=_Theta;
  PN->Phi[PN->Npart]=fmod(_Phi+AMSDBc::twopi,AMSDBc::twopi);
  PN->ThetaGl[PN->Npart]=_ThetaGl;
  PN->PhiGl[PN->Npart]=fmod(_PhiGl+AMSDBc::twopi,AMSDBc::twopi);
  for(i=0;i<3;i++)PN->Coo[PN->Npart][i]=_Coo[i];
  for(i=0;i<CTCDBc::getnlay();i++){
    PN->CooCTC[PN->Npart][i][0]=_Value[i].getcoo()[0];
    PN->CooCTC[PN->Npart][i][1]=_Value[i].getcoo()[1];
    PN->CooCTC[PN->Npart][i][2]=_Value[i].getcoo()[2];
  }
  for(i=0;i<4;i++){
    for(int j=0;j<3;j++){
      PN->TOFCoo[PN->Npart][i][j]=_TOFCoo[i][j];
    }
  }
  for(i=0;i<2;i++){
    for(int j=0;j<3;j++){
      PN->AntiCoo[PN->Npart][i][j]=_AntiCoo[i][j];
//      cout <<i<<" "<<j<<" "<<_AntiCoo[i][j]<<endl;
    }
  }
  for(i=0;i<TKDBc::nlay();i++){
    for(int j=0;j<2;j++){
      PN->TrCoo[PN->Npart][i][j]=_TrCoo[i][j];
    }
      PN->TrCoo[PN->Npart][i][2]=_Local[i];
  }

  PN->Cutoff[PN->Npart]=_CutoffMomentum;


// New ATC association
  const integer maxh=100;
  integer ctchitstatus[maxh];
  integer ctchitlayer[maxh];
  integer ctchitcolumn[maxh];
  integer ctchitrow[maxh];
  geant ctchitsignal[maxh]; 
  integer nhits=0;
 for(integer kk=0;kk<CTCDBc::getnlay();kk++){
  AMSCTCRawHit *ptr=(AMSCTCRawHit*)AMSEvent::gethead()->
  getheadC("AMSCTCRawHit",kk);
  while (ptr){
   ctchitsignal[nhits]=ptr->getsignal();
   ctchitstatus[nhits]=ptr->getstatus();
   ctchitlayer[nhits]=ptr->getlayno();
   ctchitrow[nhits]=ptr->getrowno();
   ctchitcolumn[nhits]=ptr->getcolno();
   if(nhits<maxh)nhits++;
   ptr=ptr->next();
  }
 }
/*


********************************************************************
      SUBROUTINE atcrec(run,nctcht,cooctc,ctchitlayer,ctchitcolumn,ctchitrow,c
     +                  tchitsignal,
     +                  atcnbcel,atcnbphe,atcidcel,atcdispm,
     +                  atcdaero,atcstatu,atcbeta)
********************************************************************
*   F. Barao (30/Nov/98)
*
*   AIMS: get ATC detailed information per event
*     IN:
*    OUT: atcnbcel(1:2) = nb cells acrossed
*         atcnbphe(1:2) =
*         atcidcel(1:2) =
*         atcdispm(1:2) =
*         atcdaero(1:2) =
*         atcstatu(1:2) =
*         atcbeta       =
********************************************************************
      real cooctc(3,2,1)
      real ctchitsignal(1)
      integer ctchitcolumn(1),ctchitrow(1),ctchitlayer(1),run,nctcht



*/

/*
  int   ATCnbcel[MAXPART][2];
  float ATCnbphe[MAXPART][2];
  int   ATCidcel[MAXPART][2];
  int   ATCdispm[MAXPART][2];
  int   ATCdaero[MAXPART][2];
  int   ATCstatu[MAXPART][2];
  float ATCbeta[MAXPART];
*/ 

 float atcbeta;
 atcrec_(AMSEvent::gethead()->getrun(), nhits, PN->CooCTC, ctchitlayer, ctchitcolumn, ctchitrow, ctchitsignal,  
         PN->ATCnbcel[PN->Npart], PN->ATCnbphe[PN->Npart],   PN->ATCidcel[PN->Npart],  PN->ATCdispm[PN->Npart], 
         PN->ATCdaero[PN->Npart], PN->ATCstatu[PN->Npart], atcbeta); 
 
 // cout << "ATC debug" << PN->ATCnbcel[PN->Npart][1] << PN->ATCnbcel[PN->Npart][1] << endl;
 
 //next

  PN->Npart++;
}
else{
  ParticleNtuple02* PN = AMSJob::gethead()->getntuple()->Get_part02();
  if (PN->Npart>=MAXPART) return;
  if((AMSEvent::gethead()->getC("AMSParticle",0)->getnelem()>0 || LVL3FFKEY.Accept) && _ptrack->checkstatus(AMSDBc::NOTRACK))return;
// Fill the ntuple 
  PN->ChargeP[PN->Npart]=_pcharge->getpos();
  PN->BetaP[PN->Npart]=_pbeta->getpos();
  integer pat=_pbeta->getpattern();
  int i;
  for(i=0;i<pat;i++){
    AMSContainer *pc=AMSEvent::gethead()->getC("AMSBeta",i);
    #ifdef __AMSDEBUG__
      assert(pc != NULL);
    #endif
    PN->BetaP[PN->Npart]+=pc->getnelem();
  }
 
  pat=_ptrack->getpattern();
  if(_ptrack->checkstatus(AMSDBc::NOTRACK))PN->TrackP[PN->Npart]=-1;
  else PN->TrackP[PN->Npart]=_ptrack->getpos();
  PN->Particle[PN->Npart]=_gpart[0];
  PN->ParticleVice[PN->Npart]=_gpart[1];
  PN->FitMom[PN->Npart]=_fittedmom[0];
 for(i=0;i<2;i++){
  PN->Prob[PN->Npart][i]=_prob[i];
 }
  PN->Mass[PN->Npart]=_Mass;
  PN->ErrMass[PN->Npart]=_ErrMass;
  PN->Momentum[PN->Npart]=_Momentum;
  PN->ErrMomentum[PN->Npart]=_ErrMomentum;
  PN->Charge[PN->Npart]=_Charge;
  PN->Theta[PN->Npart]=_Theta;
  PN->Phi[PN->Npart]=fmod(_Phi+AMSDBc::twopi,AMSDBc::twopi);
  PN->ThetaGl[PN->Npart]=_ThetaGl;
  PN->PhiGl[PN->Npart]=fmod(_PhiGl+AMSDBc::twopi,AMSDBc::twopi);
  for(i=0;i<3;i++)PN->Coo[PN->Npart][i]=_Coo[i];
  for(i=0;i<4;i++){
    for(int j=0;j<3;j++){
      PN->TOFCoo[PN->Npart][i][j]=_TOFCoo[i][j];
    }
  }
  for(i=0;i<2;i++){
    for(int j=0;j<3;j++){
      PN->AntiCoo[PN->Npart][i][j]=_AntiCoo[i][j];
//      cout <<i<<" "<<j<<" "<<_AntiCoo[i][j]<<endl;
    }
  }
  for(i=0;i<6;i++){
    for(int j=0;j<2;j++){
      PN->TrCoo[PN->Npart][i][j]=_TrCoo[i][j];
    }
      PN->TrCoo[PN->Npart][i][2]=_Local[i];
  }

  PN->Cutoff[PN->Npart]=_CutoffMomentum;



  PN->Npart++;

}
}


void AMSParticle::_copyEl(){
}


void AMSParticle::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSParticle",0);
 if(p)p->printC(cout);
}


void AMSParticle::pid(){
 void (*palfun)(int &n, double x[], double &f, AMSParticle *p)=
 &AMSParticle::alfun;
 void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
 &AMSParticle::monit;


  const int maxp=38;
  number prob[maxp];
  number pfit[maxp];
    _beta=1/fabs(_pbeta->getbeta());
    _ebeta=_pbeta->getebeta();
    _ebeta=_ebeta*_ebeta;
    int i;
  for (i=0;i<maxp;i++){
    integer itr;
    geant xmass,chrg,tlt7,uwb[1];
    integer nwb=0;
    char chdum[21]="";
    GFPART(_partP[i],chdum,itr,xmass,chrg,tlt7,uwb,nwb);
    _mass=xmass*xmass;
    const int mp=2;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
    integer n=1;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=2000;
    number tol=2.99e-2;
    int j;
    x[0]=0;
    _mom=1/(_Momentum/_Charge*chrg);
    _emom=_ErrMomentum/_Momentum/_Momentum*_Charge/chrg;
    _emom=_emom*_emom;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,palfun,pmonit,maxcal,ifail,this);
    geant chi2=f;
    prob[i]=PROB(chi2,1);
    prob[i]*=_pcharge->getprobcharge(chrg);
    //linux bug
    if(ifail)prob[i]=0;
    if(fabs(prob[i])>2.){
      cerr<<"AMSPArticle::pid-E-Proberror " <<chi2<<" " <<prob[i]<<endl;
     prob[i]=0;
    }
    if(x[0]!=0)pfit[i]=1./fabs(x[0]);
    else pfit[i]=FLT_MAX;
  }
  _prob[1]=-2;
  _prob[0]=-1;
  for(i=0;i<maxp;i++){
   if(prob[i]>_prob[0]){
     _gpart[1]=_gpart[0];
     _fittedmom[1]=_fittedmom[0];
     _prob[1]=_prob[0];
     _prob[0]=prob[i];
     _gpart[0]=_partP[i];
     _fittedmom[0]=pfit[i]; 
   }
   else if(prob[i]>_prob[1]){
     _prob[1]=prob[i];
     _gpart[1]=_partP[i];
     _fittedmom[1]=pfit[i]; 
   }
  }
  _GPart=_gpart[0];
    // CheckNotAproton
    for(i=0;i<maxp;i++){
      if(_partP[i]==14){
        if(prob[i]<0.1 && !_ptrack->checkstatus(AMSDBc::NOTRACK)){
          AMSEvent::gethead()->addnext(AMSID("NotAProton",0),new AntiMatter(_GPart));
          break;
        }
      }
    }

   //Check AntiMatter
  integer antimatter=0;
  for( int patt=0;patt<npat;patt++){
    AMSTrTrack *ptrack=(AMSTrTrack*)AMSEvent::gethead()->
      getheadC("AMSTrTrack",patt);
    while(ptrack ){   
      if(ptrack->getrid() <0 || ptrack->getgrid()<0){
        antimatter=1;
        break;
      }
      ptrack=ptrack->next();
    }
  }
  if((antimatter || _Momentum <0) && !_ptrack->checkstatus(AMSDBc::NOTRACK)){
   
   AMSEvent::gethead()->addnext(AMSID("AntiMatter",0),new AntiMatter(_GPart));
   
  }
  if(_Charge>1)AMSEvent::gethead()->addnext(AMSID("HeavyIon",0),new AntiMatter(_GPart));

}
void AMSParticle::refit(int fast){
    for(int layer=0;layer<TKDBc::nlay();layer++){
       number theta,phi;
      if(_ptrack->intercept(_TrCoo[layer],layer,theta,phi,_Local[layer])!=1)
      setstatus(AMSDBc::BADINTERPOL);
// Change theta,phi,coo 
      if(_pbeta->getbeta()>0 && layer==0){
          _Theta=theta;
          _Phi=phi;
          _Coo=_TrCoo[0];
         // change theta according to beta
          if(_Theta<AMSDBc::pi/2){
           _Theta=AMSDBc::pi-_Theta;
           _Phi+=AMSDBc::pi;
          }
     }       
      else if(_pbeta->getbeta()<=0 && layer==TKDBc::nlay()-1){
          _Theta=theta;    
         // change theta according to beta
          _Phi=phi;
          _Coo=_TrCoo[TKDBc::nlay()-1];
          if(_Theta>AMSDBc::pi/2){
            _Theta=AMSDBc::pi-_Theta;
            _Phi+=AMSDBc::pi;
          }
      }
    }
    if(fast || _ptrack->checkstatus(AMSDBc::NOTRACK)){
      _loc2gl();
      return;
    }
      geant dummy;
      integer dorefit=TRFITFFKEY.ForceAdvancedFit==1 ||
      (TRFITFFKEY.ForceAdvancedFit==2  &&
      !TRFITFFKEY.FastTracking);
  if(_GPart !=14 || dorefit){
        if(!_ptrack->AdvancedFitDone()){
          _ptrack->AdvancedFit();
        }
     _ptrack->Fit(0,_GPart);
// Changed - never use geanerigidity to build mom
     if(TRFITFFKEY.ForceAdvancedFit==1 && MISCFFKEY.G3On)_ptrack->Fit(_pbeta->getbeta()>0?3:-3,_GPart);
     if(0 && _ptrack->GeaneFitDone() && fabs(_ptrack->getgrid())>TRFITFFKEY.RidgidityMin/2 ){
      _build(_pbeta->getbeta()>0?_ptrack->getgrid():-_ptrack->getgrid(),
       _ptrack->getegrid(),_Charge,_pbeta,_Mass,_ErrMass,_Momentum,_ErrMomentum);
       }
     else{
      _build(_ptrack->getrid(),_ptrack->geterid(),_Charge,
       _pbeta,_Mass,_ErrMass,_Momentum,_ErrMomentum);
     }
      integer oldpart=_GPart;
      pid();
#ifdef __AMSDEBUG__
      if(_GPart != oldpart){ 

        cout <<"AMSParticle::refit-W-ParticleIdChanged: was "<<oldpart <<" "<<
        " now "<<_GPart<<endl; 
      }
#endif
     }
     _loc2gl();
  }
   
integer AMSParticle::_partP[38]={2,3,5,6,8,9,11,12,14,15,45,145,
                                 46,146,49,149,47,147,61,161,62,162,
                                 63,163,64,164,65,165,66,166,67,167,68,
                                 168,69,169,70,170};


void AMSParticle::_loc2gl(){
          AMSgObj::BookTimer.start("part::loc2gl");
 // Get current station position from event bank
  number polephi,theta,phi;

  AMSEvent::gethead()->GetGeographicCoo(polephi, theta, phi);
  geant StationRad=AMSEvent::gethead()->GetStationRad();
  geant pitch=AMSEvent::gethead()->getpitch();
  geant roll=AMSEvent::gethead()->getroll();
  geant yaw=AMSEvent::gethead()->getyaw();

  number EarthR       =AMSmceventg::Orbit.EarthR;
  number DipoleR      =AMSmceventg::Orbit.DipoleR;
  number DipoleTheta  =AMSmceventg::Orbit.DipoleTheta;
  number DipolePhi    =AMSmceventg::Orbit.DipolePhi;
  number PoleTheta    =AMSmceventg::Orbit.PoleTheta;
  number PolePhiStatic=AMSmceventg::Orbit.PolePhiStatic;

  AMSDir amszg(AMSDBc::pi/2-theta,phi);
  AMSDir amsxg(AMSDBc::pi/2-AMSEvent::gethead()->getveltheta(),
  AMSEvent::gethead()->getvelphi());
  AMSDir amsyg=amszg.cross(amsxg);
  number prod=amsxg.prod(amszg);
  if(fabs(prod)>0.03 ){
   cerr<<"AMSParticle::_loc2gl-E-AMSGlobalCoosystemIllDefined "<<prod<<endl;
   AMSEvent::gethead()->_printEl(cerr);
   AMSEvent::gethead()->seterror();
  }
  number cp=cos(pitch);
  number sp=sin(pitch);
  number cy=cos(yaw);
  number sy=sin(yaw);
  number cr=cos(roll);
  number sr=sin(roll);
  number l1=cy*cp;
  number m1=-sy;
  number n1=cy*sp;
  number l2=cr*sy*cp-sr*sp;
  number m2=cr*cy;
  number n2=cr*sy*sp+sr*cp;
  number l3=-sr*sy*cp-cr*sp;
  number m3=-sr*cy;
  number n3=-sr*sy*sp+cr*cp;
  number amsx[3],amsy[3],amsz[3];
  // (-) due to fact that ams coo system is rotated by 180^o with
  //    respect to shuttle one
  amsx[0]=-(l1*amsxg[0]+l2*amsyg[0]+l3*amszg[0]);
  amsx[1]=-(l1*amsxg[1]+l2*amsyg[1]+l3*amszg[1]);
  amsx[2]=-(l1*amsxg[2]+l2*amsyg[2]+l3*amszg[2]);
  amsy[0]=-(m1*amsxg[0]+m2*amsyg[0]+m3*amszg[0]);
  amsy[1]=-(m1*amsxg[1]+m2*amsyg[1]+m3*amszg[1]);
  amsy[2]=-(m1*amsxg[2]+m2*amsyg[2]+m3*amszg[2]);
  amsz[0]=n1*amsxg[0]+n2*amsyg[0]+n3*amszg[0];
  amsz[1]=n1*amsxg[1]+n2*amsyg[1]+n3*amszg[1];
  amsz[2]=n1*amsxg[2]+n2*amsyg[2]+n3*amszg[2];
  AMSDir _dir(_Theta,_Phi);
  number ue=_dir[0]*amsx[0]+_dir[1]*amsy[0]+_dir[2]*amsz[0];
  number ve=_dir[0]*amsx[1]+_dir[1]*amsy[1]+_dir[2]*amsz[1];
  number we=_dir[0]*amsx[2]+_dir[1]*amsy[2]+_dir[2]*amsz[2];
  AMSDir global(ue,ve,we);
  _ThetaGl=global.gettheta();
  _PhiGl=global.getphi()-(polephi-PolePhiStatic);

  //
  // Dipole direction
  //
  number um=sin(AMSDBc::pi/2-PoleTheta)*cos(polephi);
  number vm=sin(AMSDBc::pi/2-PoleTheta)*sin(polephi);
  number wm=cos(AMSDBc::pi/2-PoleTheta);

  //
  // Direction Dipole to Station
  //
  number dphi=DipolePhi+polephi-PolePhiStatic;
  number xc=StationRad*sin(AMSDBc::pi/2-theta)*cos(phi)-
   DipoleR*sin(AMSDBc::pi/2-DipoleTheta)*cos(dphi);
  number yc=StationRad*sin(AMSDBc::pi/2-theta)*sin(phi)-
   DipoleR*sin(AMSDBc::pi/2-DipoleTheta)*sin(dphi);
  number zc=StationRad*cos(AMSDBc::pi/2-theta)-
   DipoleR*cos(AMSDBc::pi/2-DipoleTheta);
  number rl=sqrt(xc*xc+zc*zc+yc*yc);
  number  up=xc/rl;
  number  vp=yc/rl;
  number  wp=zc/rl;

  number cts=um*up+vm*vp+wm*wp;
  number xl=acos(cts);
  number cl=fabs(sin(xl));
  number rgm=rl;

  // Magnetic East
  xc=vm*wp-wm*vp;
  yc=wm*up-um*wp;
  zc=um*vp-vm*up;
  rl=sqrt(xc*xc+yc*yc+zc*zc);

  number  uv=xc/rl;
  number  vv=yc/rl;
  number  wv=zc/rl;

  number cth=ue*uv+ve*vv+we*wv;
  number xfac=57.576*EarthR/rgm*EarthR/rgm;
  number chsgn=_Momentum/fabs(_Momentum);
  number cl3=cl*cl*cl;
  number cl4=cl*cl*cl*cl;
  number mom=xfac*cl4/(sqrt(1.-chsgn*cth*cl3)+1)/(sqrt(1.-chsgn*cth*cl3)+1)*_Charge;
  _CutoffMomentum=chsgn*mom;
/*
  integer rgcutoff=(int)(mom/_Charge*10+.5)<(1<<10)-1?(int)(mom/_Charge*10+.5):(1<<10)-1;
  _pbeta->setstatus(rgcutoff<<20);
*/    
          AMSgObj::BookTimer.stop("part::loc2gl");

}


void AMSParticle::alfun(integer & n , number xc[], number &fc, AMSParticle *p){
 number zbeta=p->_beta;
 if(zbeta<1)zbeta=2-zbeta;
 fc=(fabs(xc[0])-p->_mom)/p->_emom*(fabs(xc[0])-p->_mom)+
   (sqrt(1+xc[0]*xc[0]*p->_mass)-zbeta)/p->_ebeta*
   (sqrt(1+xc[0]*xc[0]*p->_mass)-zbeta);
}
