

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
          charge=pcharge->getvotedcharge();
          momentum=rid*charge;
          emomentum=err*rid*rid*charge;
          number beta=pbeta->getbeta();
          number ebeta=pbeta->getebeta()*beta*beta;
          if(fabs(beta)>=1.99){
           mass=0;
           emass=FLT_MAX;
          }
          else if(fabs(beta)<=1.e-10 ){
           cerr<< " beta too low " <<beta<<endl;
           mass=FLT_MAX;
           emass=FLT_MAX;
          }
          else if(fabs(beta)<1.){
           number one=1;
           number gamma=sqrt(one/(one-beta*beta)); 
           mass=fabs(momentum/gamma/beta);
//           emass=mass*sqrt(pow(emomentum/momentum,2.)+pow(gamma,4)*
//           pow(ebeta/beta,2));
           emass=mass*sqrt((emomentum/momentum)*(emomentum/momentum)+
           (gamma*gamma*ebeta/beta)*(gamma*gamma*ebeta/beta));
          }
          else{
           number one=1;
           if(beta>0)beta=2-beta;
           else beta=-2-beta;
           number gamma=sqrt(one/(one-beta*beta)); 
           mass=-fabs(momentum/gamma/beta);
           emass=fabs(mass)*sqrt((emomentum/momentum)*(emomentum/momentum)+
           (gamma*gamma*ebeta/beta)*(gamma*gamma*ebeta/beta));
          }
          if(beta<0)momentum=-momentum;
          ppart=new AMSParticle(pbeta, pcharge, ptrack,
          mass,emass,momentum,emomentum,charge,theta,phi,coo);
          ptrack->setstatus(AMSDBc::USED);
          ppart->pid();
           
          AMSgObj::BookTimer.start("ReAxRefit");
           ppart->refit(AMSJob::gethead()->isCalibration() & AMSJob::CTracker);
          if(!(AMSJob::gethead()->isCalibration() & AMSJob::CTracker)){
           ppart->ctcfit();
           ppart->toffit();
           ppart->antifit();
          }
          AMSgObj::BookTimer.stop("ReAxRefit");
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
  ParticleNtuple* PN = AMSJob::gethead()->getntuple()->Get_part();

  if (PN->Npart>=MAXPART) return;

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
 
  PN->TrackP[PN->Npart]=_ptrack->getpos();
  pat=_ptrack->getpattern();
  if(AMSTrTrack::Out(IOPA.WriteAll%10==1)){
    // Writeall
    for(i=0;i<pat;i++){
      AMSContainer *pc=AMSEvent::gethead()->getC("AMSTrTrack",i);
      #ifdef __AMSDEBUG__
       assert(pc != NULL);
      #endif
      PN->TrackP[PN->Npart]+=pc->getnelem();
    }
  }
  else {
    //WriteUsedOnly
    for(i=0;i<pat;i++){
      AMSTrTrack *ptr=(AMSTrTrack*)AMSEvent::gethead()->getheadC("AMSTrTrack",i);
      while(ptr && ptr->checkstatus(AMSDBc::USED)){ 
        PN->TrackP[PN->Npart]++;
        ptr=ptr->next();
      }
    }
  }
  PN->Particle[PN->Npart]=_GPart;
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
    PN->CTCP[PN->Npart][i]=_pctc[i]?_pctc[i]->getpos():0;
    PN->Value[0][PN->Npart][i]=_Value[i].getsignal();
    PN->Value[1][PN->Npart][i]=_Value[i].getbeta();
    PN->Value[2][PN->Npart][i]=_Value[i].geterbeta();
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
  for(i=0;i<6;i++){
    for(int j=0;j<3;j++){
      PN->TrCoo[PN->Npart][i][j]=_TrCoo[i][j];
    }
  }

  PN->Npart++;
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
  if( _Momentum <0){
   if(_GPart < 40 )_GPart++;
   else _GPart=_GPart+100;
  }
  if(antimatter || _Momentum <0){
   
   AMSEvent::gethead()->addnext(AMSID("AntiMatter",0),new AntiMatter(_GPart));
   
  }
  if(_Charge>1)AMSEvent::gethead()->addnext(AMSID("HeavyIon",0),new AntiMatter(_GPart));

}
void AMSParticle::refit(int fast){
    for(int layer=0;layer<nl;layer++){
       number theta,phi;
      _ptrack->intercept(_TrCoo[layer],layer,theta,phi);
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
      else if(_pbeta->getbeta()<=0 && layer==nl-1){
          _Theta=theta;    
         // change theta according to beta
          _Phi=phi;
          _Coo=_TrCoo[nl-1];
          if(_Theta>AMSDBc::pi/2){
            _Theta=AMSDBc::pi-_Theta;
            _Phi+=AMSDBc::pi;
          }
      }
    }
    _loc2gl();
    if(fast)return;

  if(_GPart !=14 ){
   if(_Charge >= 1.5 || fabs(_Mass-0.938)>1.5*_ErrMass){
      if(_Charge>=1.5){
        if(!_ptrack->AdvancedFitDone()){
          _ptrack->AdvancedFit(1);
        }
      }

    if(_Mass > 0){
     _ptrack->Fit(3,_GPart);
     if(_ptrack->GeaneFitDone()){
      number fac=_ptrack->getgrid()*_Charge/_Momentum/sign(_pbeta->getbeta());
      integer itr;
      geant xmass,chrg,tlt7,uwb[1];
      integer nwb=0;
      char chdum[21]="";
      GFPART(_GPart,chdum,itr,xmass,chrg,tlt7,uwb,nwb);
      _Mass=_Mass*fabs(fac);
      _ErrMass=_ErrMass*fabs(fac);
      _Momentum=_Momentum*fac;
      _ErrMomentum=_ErrMomentum*fabs(fac);
      if(_Mass>FLT_MAX)_Mass=FLT_MAX;
      if(_ErrMass>FLT_MAX)_ErrMass=FLT_MAX;
      if(_Momentum>FLT_MAX)_Momentum=FLT_MAX;
      if(_Momentum<-FLT_MAX)_Momentum=-FLT_MAX;
      if(_ErrMomentum>FLT_MAX)_ErrMomentum=FLT_MAX;
      integer oldpart=_GPart;
      pid();
#ifdef __AMSDEBUG__
      if(_GPart != oldpart){ 

        cout <<"AMSParticle::refit-W-ParticleIdChanged: was "<<oldpart <<" "<<
        " now "<<_GPart<<endl; 
      }
#endif
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


void AMSParticle::_loc2gl(){
          AMSgObj::BookTimer.start("part::loc2gl");
 // Get current station position from event bank
  number polephi,theta,phi;

  AMSEvent::gethead()->GetGeographicCoo(polephi, theta, phi);
  geant pitch=AMSEvent::gethead()->getpitch();
  geant roll=AMSEvent::gethead()->getroll();
  geant yaw=AMSEvent::gethead()->getyaw();

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
  number cp=cos(-pitch);
  number sp=sin(-pitch);
  number cy=cos(-yaw);
  number sy=sin(-yaw);
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
  _PhiGl= global.getphi();
          AMSgObj::BookTimer.stop("part::loc2gl");


  // Get mag east-west 


  AMSDir magpole(AMSDBc::pi/2-AMSmceventg::Orbit.PoleTheta,polephi);
  AMSDir magmeridian=magpole.cross(amszg);
  _SinMagMeridian=magmeridian.prod(global);
}
