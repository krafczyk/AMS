
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
         
          if(pbeta->checkstatus(AMSDBc::AMBIG)==0){
          number chi2(0),rid(0),err(0);
          ptrack->getParFastFit( chi2, rid, err, theta,  phi,  coo);
            if(ptrack->AdvancedFitDone() && ptrack->GeaneFitDone()){
              number gchi2(0),grid(0),gerr(0),gtheta(0),gphi(0);
              AMSPoint gcoo;
              number hchi2[2],hrid[2],herr[2],htheta[2],hphi[2];
              AMSPoint hcoo[2];
              ptrack-> getParAdvancedFit(gchi2, grid, gerr, gtheta, gphi, 
              gcoo, hchi2, hrid, herr, htheta, hphi,   hcoo );
              if(sign(rid) != sign(grid) || sign(rid) != sign(hrid[0]) ||
                 sign(rid) != sign(hrid[1])){
#ifdef __AMSDEBUG__
                 cout <<" Fast & Advanced Fits disagree. "<<rid<<" "<<grid<<
                 " "<<hrid[0]<< " "<<hrid[1]<<endl;
#endif             
//                 goto out;
              } 
              else {rid=grid; err=gerr; }
            }
          // Add new element
          charge=(pcharge->getchargeTOF()+pcharge->getchargeTracker())/2.;
          momentum=rid*charge;
          emomentum=err*rid*rid*charge;
          number beta=pbeta->getbeta();
          number ebeta=pbeta->getebeta()*beta*beta;
          if(fabs(beta)>=1){
           mass=0;
           emass=FLT_MAX;
          }
          else if(fabs(beta)<=1.e-10 ){
           cerr<< " beta too low " <<beta<<endl;
           mass=FLT_MAX;
           emass=FLT_MAX;
          }
          else{
           number one=1;
           number gamma=sqrt(one/(one-beta*beta)); 
           mass=fabs(momentum/gamma/beta);
           emass=mass*sqrt(pow(emomentum/momentum,2.)+pow(gamma,4)*
           pow(ebeta/beta,2));
           if(beta<0)momentum=-momentum;
          }
          ppart=new AMSParticle(pbeta, pcharge, ptrack,
          mass,emass,momentum,emomentum,charge,theta,phi,coo);
          ptrack->setstatus(AMSDBc::USED);
          ppart->pid();
          ppart->refit();
          ppart->ctcfit();
          ppart->toffit();
          AMSEvent::gethead()->addnext(AMSID("AMSParticle",0),ppart);
          }
        }
out:
       pcharge=pcharge->next();
      }
      return 1;
}

void AMSParticle::ctcfit(){
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
   AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
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






void AMSParticle::_writeEl(){
  // fill the ntuple 
static integer init=0;
static ParticleNtuple PN;
int i;

if(init++==0){
  // get memory
  //book the ntuple block
  HBNAME(IOPA.ntuple,"Particle",PN.getaddress(),
  "ParticleEvent:I*4, PCTCPointer(2):I*4,PBetaPointer:I*4, PChargePointer:I*4, PtrackPointer:I*4,   ParticleId:I*4,  PMass:R*4, PErrMass:R*4, PMom:R*4, PErrMom:R*4, PCharge:R*4, PTheta:R*4, PPhi:R*4, PCoo(3):R*4, SignalCTC(2):R*4, BetaCTC(2):R*4, ErrorBetaCTC(2):R*4, CooCTC(3,2):R*4,COOTOF(3,4):R");

}
 PN.ChargeP=_pcharge->getpos();
 PN.BetaP=_pbeta->getpos();
 integer pat=_pbeta->getpattern();
 for(i=0;i<pat;i++){
   AMSContainer *pc=AMSEvent::gethead()->getC("AMSBeta",i);
   #ifdef __AMSDEBUG__
     assert(pc != NULL);
   #endif
   PN.BetaP+=pc->getnelem();
 }

 PN.TrackP=_ptrack->getpos();
 pat=_ptrack->getpattern();
     if(AMSTrTrack::Out(IOPA.WriteAll)){
       // Writeall
     
        for(i=0;i<pat;i++){
         AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrTrack",i);
         #ifdef __AMSDEBUG__
          assert(pc != NULL);
         #endif
         PN.TrackP+=pc->getnelem();
        }
     }
     else {
       //WriteUsedOnly
        for(i=0;i<pat;i++){
         AMSTrTrack *ptr=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",i);
          while(ptr && ptr->checkstatus(AMSDBc::USED)){ 
            PN.TrackP++;
            ptr=ptr->next();
          }
        }
     }
 PN.Event()=AMSEvent::gethead()->getid();
 
 PN.Particle=_GPart;
 PN.Mass=_Mass;
 PN.ErrMass=_ErrMass;
 PN.Momentum=_Momentum;
 PN.ErrMomentum=_ErrMomentum;
 PN.Charge=_Charge;
 PN.Theta=_Theta;
 PN.Phi=_Phi;
 for(i=0;i<3;i++)PN.Coo[i]=_Coo[i];
 for(i=0;i<CTCDBc::getnlay();i++){
  PN.CTCP[i]=_pctc[i]?_pctc[i]->getpos():0;
  PN.Value[0][i]=_Value[i].getsignal();
  PN.Value[1][i]=_Value[i].getbeta();
  PN.Value[2][i]=_Value[i].geterbeta();
  PN.CooCTC[i][0]=_Value[i].getcoo()[0];
  PN.CooCTC[i][1]=_Value[i].getcoo()[1];
  PN.CooCTC[i][2]=_Value[i].getcoo()[2];
 }
  for(i=0;i<4;i++){
    for(int j=0;j<3;j++){
     PN.TOFCoo[i][j]=_TOFCoo[i][j];
    }
  }


  HFNTB(IOPA.ntuple,"Particle");
}

void AMSParticle::_copyEl(){
}


void AMSParticle::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSParticle",0);
 if(p)p->printC(cout);
}


void AMSParticle::pid(){
  integer num;
  if(_Charge < 1.5){
    num=AMSbiel(_massP,(geant)_Mass,_chargeP[0]);
   if(num<1)num=1;
   if(num>_chargeP[0]-1)num=_chargeP[0]-1;
   if(fabs(_Mass-_massP[num])>fabs(_Mass-_massP[num-1]))num--;
  }
  else if(_Charge < 2.5){
   num=_chargeP[1]-1;
   if(fabs(_Mass-_massP[num])>fabs(_Mass-_massP[num-1]))num--;
  }
  else if(_Charge < 3.5){
   num=_chargeP[2]-1;
   if(fabs(_Mass-_massP[num])>fabs(_Mass-_massP[num-1]))num--;
  }
  else if(_Charge < 4.5){
   num=_chargeP[3]-1;
   if(fabs(_Mass-_massP[num])>fabs(_Mass-_massP[num-1]))num--;
  }
  else if(_Charge < 5.5){
   num=_chargeP[4]-1;
   if(fabs(_Mass-_massP[num])>fabs(_Mass-_massP[num-1]))num--;
  }
  else if(_Charge < 6.5){
   num=_chargeP[5]-1;
  }
  else if(_Charge < 7.5){
   num=_chargeP[6]-1;
  }
  else if(_Charge < 8.5){
   num=_chargeP[7]-1;
  }
  else if(_Charge < 9.5){
   num=_chargeP[8]-1;
  }
  else if(_Charge < 10.5){
   num=_chargeP[9]-1;
  }
  else{
    cerr <<"AMSParticle::_pid-E-TooBigParticleCharge "<<_Charge<<endl;
    num=_chargeP[9];
}
  _GPart=_partP[num]; 
  if(_Momentum <0){
   if(_GPart < 40 )_GPart++;
   else _GPart=_GPart+100;
   
   AMSEvent::gethead()->addnext(AMSID("AntiMatter",0),new AntiMatter(_GPart));
   
  }

}
void AMSParticle::refit(){
  if(_GPart !=14 && _Momentum*sign(_pbeta->getbeta()) < 0){
   if(_Charge >= 1.5 || fabs(_Mass-0.938)>1.5*_ErrMass){
    if(_Mass > 0){
     _ptrack->Fit(3,_GPart);
     if(_ptrack->GeaneFitDone()){
      number fac=_ptrack->getgrid()/_Momentum/sign(_pbeta->getbeta());
      integer itr;
      geant xmass,chrg,tlt7,uwb[1];
      integer nwb=0;
      char chdum[21];
      GFPART(_GPart,chdum,itr,xmass,chrg,tlt7,uwb,nwb);
      fac=fac*fabs(chrg);
      _Mass=_Mass*fac;
      _ErrMass=_ErrMass*fac;
      _Momentum=_Momentum*fac;
      _ErrMomentum=_ErrMomentum*fac;
      if(_Mass>FLT_MAX)_Mass=FLT_MAX;
      if(_ErrMass>FLT_MAX)_ErrMass=FLT_MAX;
      if(_Momentum>FLT_MAX)_Momentum=FLT_MAX;
      if(_Momentum<-FLT_MAX)_Momentum=-FLT_MAX;
      if(_ErrMomentum>FLT_MAX)_ErrMomentum=FLT_MAX;
      integer oldpart=_GPart;
      pid();
      if(_GPart != oldpart){ 
        cout <<"AMSParticle::refit-W-ParticleIdChanged: was "<<oldpart <<" "<<
        " now "<<_GPart<<endl; 
      }
     }
    }
   }
  }   
}  
geant AMSParticle::_massP[19]={0.,0.106,0.5,0.938,1.8756,2.81,2.81,3.727,5.6,
                             6.5,6.5,8.4,9.3,10.3,11.2,13.04,14.9,17.7,18.6};
integer AMSParticle::_chargeP[10]={6,8,10,12,14,15,16,17,18,19};
integer AMSParticle::_partP[20]={2,5,11,14,45,46,49,47,61,62,63,64,
                                              65,66,67,68,69,70,71,72};
