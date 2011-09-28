//  $Id: particle.C,v 1.240 2011/09/28 07:50:04 choutko Exp $

// Author V. Choutko 6-june-1996

#include "commons.h"
#include <math.h>
#ifndef __DARWIN__
#include <values.h>
#else
#define MINDOUBLE DBL_MIN
#endif
#include <limits.h>
#include "amsgobj.h"
#include "trigger102.h"
#include "extC.h"
#include "upool.h"
#include "charge.h"
#include <stdlib.h>
#include "particle.h"
#include "mccluster.h"
#include "amsstl.h"
#include "tofrec02.h"
#include "ntuple.h"
#include "antirec02.h"
#include "ecaldbc.h"
#include "ecalrec.h"
#include "mceventg.h"
#include "ecalrec.h"
#ifdef _PGTRACK_
//#include "patt.h"
#include "tkdcards.h"
//#include "TkSens.h"
#else
#include "vtx.h"
#endif
#include "TROOT.h"
#include "TMath.h"
#include "TrdHRecon.h"
#include "TrdHCharge.h"

// Normalized TRD probabilities (preliminary)
number AMSParticle::trdpspect[30]={
  0.133822,0.254755,0.243698,0.168653,0.0885082,0.0419913,0.018962,0.0105981,0.00641613,0.00412466,0.00360907,0.00337993,0.00240605,0.00183318,0.00148946,0.00154675,0.00137489,0.000630156,0.00091659,0.000973877,0.000859303,0.000572869,0.00074473,0.000687443,0.000515582,0.000572869,0.000859303,0.000229148,0.000687443,0.000401008};

number AMSParticle::trdespect[30]={
  0.065593,0.113031,0.120425,0.101903,0.072328,0.0493411,0.0367496,0.038287,0.0353587,0.0377013,0.0333089,0.0302343,0.0296486,0.025183,0.0237921,0.0173499,0.0167643,0.0169839,0.0127379,0.0140556,0.0122987,0.011347,0.00944363,0.00812592,0.00629575,0.00702782,0.00600293,0.00592972,0.00387994,0.00461201};

PROTOCCALLSFFUN2(FLOAT,PROB,prob,FLOAT,INT)
#define PROB(A2,A3)  CCALLSFFUN2(PROB,prob,FLOAT,INT,A2,A3)

  extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void *alfun, void * monit, int & maxcal, int &ifail, void * p);


#ifdef _PGTRACK_
bool TkTRDMatch(AMSTrTrack* ptrack,AMSTRDTrack *ptrd){
  number SearchReg[2]={12,4};
  number MaxCos(0.95);
  double zpl=63.;//approximate utof height
  //83.5; //Z low of TRD in cm;

  // TRD point and direction at Z=  zpl
  AMSPoint trd_pnt0     = ptrd->getCooStr();
  AMSDir   trd_dir  = ptrd->getCooDirStr();

  double X_TRD= (zpl-trd_pnt0[2])*trd_dir[0]/trd_dir[2]+trd_pnt0[0];
  double Y_TRD= (zpl-trd_pnt0[2])*trd_dir[1]/trd_dir[2]+trd_pnt0[1];
  AMSPoint trd_pnt(X_TRD,Y_TRD,zpl);

  // Tracker point and direction at Z= zpl
  AMSPoint tk_pnt;
  AMSDir   tk_dir;
  ptrack->Interpolate(zpl, tk_pnt, tk_dir); 

  // angle between the tracks
  number c=tk_dir.prod(trd_dir);
  //distance 
  number d=(tk_pnt-trd_pnt).norm();
  
  //PZ DEBUG  printf(" TRDTK MATCH  cos %f dist %f\n",c,d);

// Changed by VC take care about tracker amb

  if(fabs(c)>MaxCos && fabs((tk_pnt-trd_pnt)[0]) <SearchReg[0] && fabs((tk_pnt-trd_pnt)[1]) <SearchReg[1]) return true;
//  if(fabs(c)>MaxCos && fabs(d) <SearchReg) return true;
  
  return false;
}

#endif


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
  AMSCharge *pcharge=(AMSCharge*)AMSEvent::gethead()->getheadC("AMSCharge",0);
  int partfound=0;


  //   Make vertex particle

  number mbig=100000;
  AMSVtx *pcand=0;
  for( AMSVtx *pvert=(AMSVtx*)AMSEvent::gethead()->getheadC("AMSVtx",0);pvert!=NULL;pvert=pvert->next()){   
    // VC pvert
    if(abs(pvert->getcharge())<2 && !pvert->checkstatus(AMSDBc::BAD)){    
      if(pvert->getmass()/pvert->getmom()<mbig){
	mbig=pvert->getmass()/pvert->getmom();
	pcand=pvert;
      }
    }
  }
  if(pcand){
    ppart=new AMSParticle(pcand);
    pcand->setstatus(AMSDBc::USED);
    ppart->pid();
    AMSEvent::gethead()->addnext(AMSID("AMSParticle",ppart->contnumber()),ppart);
    
    //cerr <<"  Added a VERTEX Particle cont number "<<ppart->contnumber()<<endl;
    partfound++;
  }
  //
  //  change here if other particles after vtx particles should be allowed
  //
  if(!partfound){      
    int evt=AMSEvent::gethead()->getid();
    while(pcharge) {
      {
	AMSBeta * pbeta=pcharge->getpbeta();
	AMSTrTrack * ptrack=pbeta->getptrack();
#ifdef __AMSDEBUG__
	//          assert (ptrack && pbeta && pcharge);
#endif   
	if(!ptrack || !pbeta || !pcharge){
	  cerr<<" AMSParticle-F-problem "<<endl;
	}         
	if(1 || pbeta->checkstatus(AMSDBc::AMBIG)==0 ||BETAFITFFKEY.FullReco ){
          number chi2(0),rid(0),err(0);
          ptrack->getParFastFit( chi2, rid, err, theta,  phi,  coo);
          // Add new element
          
          int index;
          charge=pcharge->getvotedcharge(index);
          number beta=pbeta->getbeta();
          number ebeta=pbeta->getebeta()*beta*beta;
          _build(rid,err,charge,beta,ebeta,mass,emass,momentum,emomentum);
          ppart=new AMSParticle(pbeta, pcharge, ptrack,
				beta,ebeta,mass,emass,momentum,emomentum,charge,theta,phi,coo);
          ptrack->setstatus(AMSDBc::USED);
          
          AMSgObj::BookTimer.start("ReAxPid");
          ppart->pid();
          AMSgObj::BookTimer.stop("ReAxPid");
           
          AMSEvent::gethead()->addnext(AMSID("AMSParticle",ppart->contnumber()),ppart);
	  //cerr <<"  Added a Particle cont number "<<ppart->contnumber()<<endl;
	  partfound++;
	}
      }
      pcharge=pcharge->next();
    }
    
  }

  if(!partfound){
    //      Make ECAL Particle if exists from the highest available ecals
    AMSEcalShower * pecal=(AMSEcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0,2);
    if(pecal){
      for (int i=-1;i<2;i+=2){
	// make false tracks (+-)

	//PZ FPE FIX often ecal ene is very large double > 1e209 or NAN
        double ecal_ene=pecal->getEnergyC();
        double ecal_ene_err=pecal->getEnergyErr();
	if(!isfinite(ecal_ene)){
	  //	  cerr<<" AMSParticle::build-W- ECAL ENE FIT Result is Not a Good floating number "<< ecal_ene<<" !!!"<<endl;
	  continue;
	}
	if(fabs(ecal_ene) > 1e6) ecal_ene=1e6;
	if(fabs(ecal_ene_err) > 1e7) ecal_ene_err=1e7;
	//PZ FPE FIX often ecal ene is very small double skip in this case
	
	if(ecal_ene<=MINDOUBLE) {
	  //  cerr<<" AMSParticle::build-W- ECAL ENE FIT Result small double near to Zero!!!"<<endl;
	  continue;
	}
	
	AMSTrTrack *ptrack=new AMSTrTrack(pecal->getDir(), pecal->getEntryPoint(),ecal_ene*i,ecal_ene_err);
         Trigger2LVL1 *ptr2=(Trigger2LVL1*)AMSEvent::gethead()->getheadC("TriggerLVL1",0);
         int charge=0;
         if(ptr2 && ptr2->TofFasTrigOK())charge=1;
	ptrack->setstatus(AMSDBc::ECALTRACK); 
	ppart=new AMSParticle(0, 0, ptrack,
			      pecal->getDirection(),1,0,100000,ecal_ene*i*pecal->getDirection(),ecal_ene_err,charge,pecal->getDir().gettheta(),pecal->getDir().getphi(),pecal->getEntryPoint());
	//          cout <<" ecal particle done "<<AMSEvent::gethead()->getid()<<endl;
	AMSgObj::BookTimer.start("ReAxPid");
	ppart->pid();
	AMSgObj::BookTimer.stop("ReAxPid");
	
	AMSEvent::gethead()->addnext(AMSID("AMSParticle",ppart->contnumber()),ppart);
	
	//cerr <<"  Added a ECAL Particle cont number "<<ppart->contnumber()<<endl;

      }
    }
    
    

  }
  AMSgObj::BookTimer.start("ReAxRefit");
  for (int id=0;id<2;id++){
    AMSParticle *ppart=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",id);
    while(ppart){
      ppart->refit(AMSJob::gethead()->isCalibration() & AMSJob::CTracker);
      AMSgObj::BookTimer.start("ReRICHRefit"); 
      ppart->richfit();
      AMSgObj::BookTimer.stop("ReRICHRefit"); 
      AMSgObj::BookTimer.start("ReTOFRefit"); 
      ppart->toffit();
      ppart->antifit();
      AMSgObj::BookTimer.stop("ReTOFRefit"); 
      AMSgObj::BookTimer.start("ReECRefit"); 
      ppart->ecalfit();
      AMSgObj::BookTimer.stop("ReECRefit"); 
      AMSgObj::BookTimer.start("ReTRDRefit"); 
      ppart->trdfit();
      ppart->trd_likelihood();
      if(TRDFITFFKEY.FitMethod)ppart->trd_Hlikelihood();
      AMSgObj::BookTimer.stop("ReTRDRefit"); 
      ppart=ppart->next();
    }
  }   
  AMSgObj::BookTimer.stop("ReAxRefit");


  return 1;
}



void AMSParticle::_calcmass(number momentum,number emomentum, number beta, number ebeta, number &mass, number &emass){
  if(fabs(beta)<=1.e-10 ){
    cerr<< " beta too low " <<beta<<endl;
    mass=FLT_MAX;
    emass=FLT_MAX;
  }
  else{ 
    number one=1;
    number xb=1/fabs(beta);
    if(xb<one)xb=2-xb;
    number gamma2=fabs(beta)!=1?one/(one-beta*beta):FLT_MAX; 
    number mass2=momentum*momentum*(xb*xb-one);
    mass=gamma2>0? sqrt(mass2) : -sqrt(mass2);
    emass=fabs(mass)*sqrt((emomentum/momentum)*(emomentum/momentum)+
			  (gamma2*ebeta/beta)*(gamma2*ebeta/beta));
  }
}

void  AMSParticle::_build(number rid,number err,number charge,number beta, number ebeta,number & mass,number & emass,number & momentum,number & emomentum){
  momentum=rid*charge;
  emomentum=err*rid*rid*charge;
  _calcmass(momentum,emomentum,beta,ebeta,mass,emass);
  if(beta<0)momentum=-momentum;
}


void AMSParticle::toffit(){
  AMSDir dir(0,0,1.);
  number theta, phi, sleng;
  number signal,beta,ebeta;
  // Find TOF hits
  integer ntof;
  integer fitdone[4]={0,0,0,0};
  if(!_pbeta){
    ntof=0;
  }
  else if(_pbeta->getpattern()==0){
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
    _TOFTLength[pcl->getntof()-1]=sleng;
  }
  // No hits, but still TrackExtrapolation needed
  for(kk=0;kk<4;kk++){
    if(!fitdone[kk]){
      AMSTOFCluster d(0,kk+1,1);
      AMSgvolume *p=AMSJob::gethead()->getgeomvolume(d.crgid());
      if(p){
	_ptrack->interpolate(p->loc2gl(AMSPoint(0,0,0)),dir,_TOFCoo[kk],theta,phi,sleng);
	_TOFTLength[kk]=sleng;
      }
      else {
	cerr << " toffit-S- No layer no " << kk+1<<endl ;
	_TOFCoo[kk]=AMSPoint(0,0,0);
	_TOFTLength[kk]=0;
      }

    }
  }
}



void AMSParticle::antifit(){
  number theta, phi, sleng;
  _AntiCoo[0]=AMSPoint(0,0,10000);
  _AntiCoo[1]=AMSPoint(0,0,10000);
  _AntiCrAngle[0][0]=AMSDBc::pi/2;
  _AntiCrAngle[1][0]=AMSDBc::pi/2;
  _AntiCrAngle[0][1]=0;
  _AntiCrAngle[1][1]=0;
  for(int kk=0;kk<2;kk++){
    AMSAntiCluster d(0,1);
    AMSgvolume *p=AMSJob::gethead()->getgeomvolume(d.crgid());
    if(p){
      AMSPoint coo(p->getcooA(0),p->getcooA(1),p->getcooA(2));
      number rad=(p->getpar(0)+p->getpar(1))/2.;
      AMSDir dir(p->getnrmA(2,0),p->getnrmA(2,1),p->getnrmA(2,2));
      if(!_ptrack->interpolateCyl(coo,dir,rad,2*kk-1,_AntiCoo[kk],_AntiCrAngle[kk][0],_AntiCrAngle[kk][1],sleng)){
	_AntiCoo[kk]=AMSPoint(0,0,10000);
	break;
      }
    }
    else {
      cerr << " antifit-S- No sector no " << kk+1<<endl ;
      _AntiCoo[kk]=AMSPoint(0,0,0);
    }
  }
}




void AMSParticle::ecalfit(){

  {
    // Fit to first & last ecal planes as well as to shower max ( or 1/2 if no sho)

  
    AMSEcalShower *pbest=0;
    number dmin=FLT_MAX;
    AMSDir dir(0,0,1.);
    number theta, phi, sleng;
    {
      AMSEcalShower *p=(AMSEcalShower*)AMSEvent::gethead()->getheadC("EcalShower",0,1);
      while(p){
	_ptrack->interpolate(p->getEntryPoint(),dir,_EcalSCoo[0],theta,phi,sleng);
	number d=(p->getEntryPoint()-_EcalSCoo[0]).norm();
	if(d<dmin){
	  dmin=d;
	  pbest=p;
	}
	p=p->next();
      }
    }
    if(pbest){
      _ptrack->interpolate(pbest->getEntryPoint(),dir,_EcalSCoo[0],theta,phi,sleng);
      _ptrack->interpolate(pbest->getCofG(),dir,_EcalSCoo[1],theta,phi,sleng);
      _ptrack->interpolate(pbest->getExitPoint(),dir,_EcalSCoo[2],theta,phi,sleng);
      _pShower=pbest;
      _pShower->setstatus(AMSDBc::USED);
    }
    else _pShower=0;
    integer pr,pl,ce;
    number cl,ct;
    number EcalFirstPlaneZ,EcalLastPlaneZ;
    ECALDBc::getscinfoa(0,0,0,pr,pl,ce,ct,cl,EcalFirstPlaneZ);
    ECALDBc::getscinfoa(ECALDBc::slstruc(3)-1,2,0,pr,pl,ce,ct,cl,EcalLastPlaneZ);
    _ptrack->interpolate(AMSPoint(0,0,EcalFirstPlaneZ),dir,_EcalSCoo[0],theta,phi,sleng);
    _ptrack->interpolate(AMSPoint(0,0,(EcalFirstPlaneZ+EcalLastPlaneZ)/2),dir,_EcalSCoo[1],theta,phi,sleng);
    _ptrack->interpolate(AMSPoint(0,0,EcalLastPlaneZ),dir,_EcalSCoo[2],theta,phi,sleng);






  }

  //special Eugeni's calibration

  if(AMSJob::gethead()->isCalibration() & AMSJob::CEcal){
    AMSDir dir(0,0,1.);
    AMSPoint coo;
    number theta, phi, sleng;
    Ecal1DCluster * ptr;   
    integer maxpl,ipl;
    maxpl=2*ECALDBc::slstruc(3);
    for(ipl=0;ipl<2;ipl++){ //loop over containers(proj)
      ptr=(Ecal1DCluster*)AMSEvent::gethead()->getheadC("Ecal1DCluster",ipl,0);
      if(ptr){
	coo=ptr->getcoo();//use only first cluster in container(really need only Z-coo)
	_ptrack->interpolate(coo,dir,_EcalCoo[ipl],theta,phi,sleng);
      }
      else { // no clusters in this plane -> put "0"
	_EcalCoo[ipl]=AMSPoint(0,0,0);
      }
    }
  }

}

// Find TrdHTrack matching TrTrack of ParticleR and get
// -loglikelihood of the event to be electron-like
void AMSParticle::trd_Hlikelihood(){
  int debug=0;
  _TRDHLikelihood=0.;
  _TRDHElik=0.;
  _TRDHPlik=0.;
  _TRDCCnhit=0;
  _phtrd=0;

  // TOF layers would probably make more sense (->mult.scatt.)
  double zpl=83.5; //Z low of TRD in cm;
  number SearchReg(6);
  number MaxCos(0.9);

  number theta,phi,sleng;
  AMSPoint coo(0,0,zpl);
  AMSDir   tmp(0,0,-1.);
  AMSDir   tk_pnt;
  _ptrack->interpolate(coo,tmp,tk_pnt,theta,phi,sleng);
  AMSDir tk_dir(theta,phi);

  AMSTRDHTrack* ptr=(AMSTRDHTrack*)AMSEvent::gethead()->getheadC("AMSTRDHTrack",0);
  while(ptr){
    AMSPoint trd_pnt=ptr->getCooStr();
    AMSDir trd_dir=AMSDir(ptr->Dir[0],ptr->Dir[1],ptr->Dir[2]);
    bool matched =0;

    float dz=zpl-ptr->Coo[2];
    float x=ptr->Coo[0]+ ptr->Dir[0]/ptr->Dir[2]*dz;
    float y=ptr->Coo[1]+ ptr->Dir[1]/ptr->Dir[2]*dz;

    // angle between the tracks
    number c=tk_dir.prod(trd_dir);

    //distance
    number d=sqrt(pow((tk_pnt.x()-x),2)+pow((tk_pnt.y()-y),2));

    if(debug) printf(" TRDHTK MATCH  cos %f dist %f\n",c,d);
    if(fabs(c)>MaxCos && fabs(d) <SearchReg) matched=1;

    if( matched ){
      _phtrd=ptr;
      _phtrd->status=2;
      break;
    }
    ptr=ptr->next();
  }
  if(_phtrd){
    if(debug)printf("matching track found\n");
    _phtrd->setstatus(AMSDBc::USED);
  }
  else{ // look for hsegments matching trtrack
    if(debug)printf("looking for new track\n");
    AMSTRDHSegment* seg=(AMSTRDHSegment*)AMSEvent::gethead()->getheadC("AMSTRDHSegment",0);
    
    int sptr[2]={-1,-1};
    int siter=0;
    
    while(seg){
      TrdHSegmentR* s=seg;
      float dz=zpl-s->z;
      float expos=s->r+ s->m*dz;
      if(debug)printf("seg d %i r %.2f m %.2f z %.2f\n",s->d,s->r,s->m,s->z);
      if(debug)printf("tk %.2f %.2f %.2f dir %.2f %.2f %.2f\n",
                      tk_pnt[0],tk_pnt[1],tk_pnt[2],
                      tk_dir[0],tk_dir[1],tk_dir[2]);
      if(debug)printf("dz %.2f expos %.2f c %.2f d %.2f\n",dz,expos,
                      tk_dir[s->d]/tk_dir[2]-s->m,
                      tk_pnt[s->d]-expos);
      if(fabs(tk_dir[s->d]/tk_dir[2]-s->m)<1-MaxCos&&
         fabs(tk_pnt[s->d]-expos)<SearchReg){
	//      change by vc
        //            delete sptr[s->d];
        sptr[s->d]=siter;
      }

      siter++;
      seg=seg->next();
    }

    if(debug)printf("s1 %d s2 %d\n",sptr[0],sptr[1]);

    if(sptr[0]>=0&&sptr[1]>=0){
      if(debug)printf("try to build new TrdHTrackR \n");
      TrdHTrackR* tr=TrdHReconR::gethead(AMSEvent::get_thread_num())->SegToTrack(sptr[0],sptr[1]);
      if(tr){
	if(debug)printf("generate AMSTRDHTrack\n");
	if(tr){
	  tr->status=3;
	  TrdHReconR::gethead(AMSEvent::get_thread_num())->AddTrack(tr);
	  _TRDHLikelihood=TrdHChargeR::gethead()->GetELikelihood(tr);
	  _TRDHElik=TrdHChargeR::gethead()->GetELikelihood(tr,0.,1);
	  _TRDHPlik=TrdHChargeR::gethead()->GetELikelihood(tr,0.,2);
	  _TRDCCnhit=TrdHChargeR::gethead()->GetNCC(tr);


	  AMSTRDHTrack* ptr=(AMSTRDHTrack*)AMSEvent::gethead()->getC("AMSTRDHTrack",0)->getlast();
	  _phtrd=ptr;
	}
      }
    }
  }


  if(!_phtrd)return;
  _TRDHLikelihood=_phtrd->elikelihood;
  _TRDHElik=TrdHChargeR::gethead()->GetELikelihood(_phtrd,0.,1);
  _TRDHPlik=TrdHChargeR::gethead()->GetELikelihood(_phtrd,0.,2);
  _TRDCCnhit=TrdHChargeR::gethead()->GetNCC(_phtrd);
  if(debug)printf("likelihood %.2f\n",_TRDHLikelihood);
}

void AMSParticle::trd_likelihood(){

  //Crude likelihood for TRD-based pid.  Right now this just
  // calculates likelihood globally for all hits in an event.
  // To be upgraded by GPC to associate hits with TRD tracks

  number loglikelihood=0.;

  for (int n=0;n<AMSTRDIdSoft::ncrates();n++){
    AMSTRDRawHit * ptr=
      (AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",n,2); 

    while(ptr)
      {
	AMSTRDIdSoft id(ptr->getidsoft());
	float dE=(ptr->Amp())/((id.getgain()>0?id.getgain():1)*TRDMCFFKEY.GeV2ADC)*1.e6;

	// Get the corresponding probs

	// For this array index in keV roughly correct
        // Deal with overflow later
	integer index=(int)dE;
  
	if (index>=0 &&index<30){
	  number pprob =trdpspect[index];
	  number eprob =trdespect[index];
	  loglikelihood += log(eprob/pprob);
	}

	ptr=ptr->next();
      }
  }

  _TRDLikelihood=loglikelihood;
}


void AMSParticle::trdfit(){
  _ptrd=0;
  AMSDir dir(0,0,1.);
  number theta, phi, sleng;
  number dist=FLT_MAX;
  for(AMSTRDTrack* ptr=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,0);ptr;ptr=ptr->next()){
  if(ptr->checkstatus(AMSDBc::DELETED))continue;
    AMSPoint coo=ptr->getCooStr();
    AMSPoint tmp;
#ifdef _PGTRACK_
    bool matched =TkTRDMatch(_ptrack,ptr);
    if( matched ){
      _ptrd=ptr;
      break;
    }
#else
    _ptrack->interpolate(coo,dir,tmp,theta,phi,sleng);
    number d2=(coo-tmp).norm();
    number cd=AMSDir(theta,phi).crossp(AMSDir(ptr->gettheta(),ptr->getphi())).norm();
    d2*=fabs(cd);
    AMSPoint error=ptr->getECooStr();
    number d3;
    if(_ptrack->checkstatus(AMSDBc::NOTRACK)){
      integer ntof;
      if(_pbeta->getpattern()==0){
	ntof=4;
      }
      else if(_pbeta->getpattern()<5){
	ntof=3;
      }
      else {
	ntof=2;
      }
      AMSPoint tofe(0,0,0);
      for(int kk=0;kk<ntof;kk++){
	AMSTOFCluster* pcl=_pbeta->getpcluster(kk);
	tofe=tofe+pcl->getecoo();
      }
      tofe=tofe/ntof;
      d3=5*sqrt(error.prod(error)+tofe.prod(tofe)); 
    }
    else d3=5*error.norm();
    if(_ptrack->TRDMatch(ptr) && d2<d3){
	if(d2<dist){
	  dist=d2;
	  _TRDCoo[0]=tmp;
	  _ptrd=ptr;
	}
	else if(!_ptrd){
	  _TRDCoo[0]=tmp;
	}
      }
#endif
  }
  if(_ptrd){
    _ptrd->setstatus(AMSDBc::USED);
    AMSPoint aa=_ptrd->getCooStr();
#ifdef _PGTRACK_
    _ptrack->Interpolate(aa[2], _TRDCoo[0],dir);
#else
      _ptrack->interpolate(aa,dir,_TRDCoo[0],theta,phi,sleng);
#endif
  }
  else{
     
    AMSTRDIdGeom ida(0,0,0);
    AMSTRDIdGeom idb(TRDDBc::nlay()-1,0,0);
    AMSgvolume *pa=AMSJob::gethead()->getgeomvolume(ida.crgid());
    AMSgvolume *pb=AMSJob::gethead()->getgeomvolume(idb.crgid());
    if(pa && pb){
      number z=0.5*(pa->loc2gl(AMSPoint(0,0,0))[2]+pb->loc2gl(AMSPoint(0,0,0))[2]);
      AMSPoint coo(0,0,z);
#ifdef _PGTRACK_
      _ptrack->Interpolate(z, _TRDCoo[0],dir);
#else
      _ptrack->interpolate(coo,dir,_TRDCoo[0],theta,phi,sleng);
#endif
    }
    else {
      cerr << " trdfit-S- NoLayerFoundThenExpected " << pa<<" "<<pb<<endl ;
      _TRDCoo[0]=AMSPoint(0,0,0);
    }

  }

  //add trdcoo2 on top of trd
  AMSTRDIdGeom idb(TRDDBc::nlay()-1,0,0);
  AMSgvolume *pb=AMSJob::gethead()->getgeomvolume(idb.crgid());
  if(pb){
    number z=pb->loc2gl(AMSPoint(0,0,0))[2]+2;

    AMSPoint coo(0,0,z);
#ifdef _PGTRACK_
      _ptrack->Interpolate(z, _TRDCoo[1],dir);
#else
    _ptrack->interpolate(coo,dir,_TRDCoo[1],theta,phi,sleng);
#endif
  }
  else {
    cerr << " trdfit-S- NoLayerFoundThenExpected " <<pb<<endl ;
    _TRDCoo[1]=AMSPoint(0,0,0);
  }
  


/* // <- trdcharge computation moved to charge.C
if(_ptrd && _pbeta){
// Calculate trdcharge 

_ptrd->ComputeCharge(_pbeta->GetTRDBetaCorr());

}
*/
}


void AMSParticle::richfit(){
  AMSDir dir(0,0,1.);
  number theta, phi, sleng;
  AMSTrTrack *real_track=_ptrack;
  
  real_track->interpolate(AMSPoint(0,0,RICHDB::RICradpos()),dir,_RichCoo[0],theta,phi,sleng);
  real_track->interpolate(AMSPoint(0,0,RICHDB::RICradpos()-RICHDB::pmt_pos()-RICHDB::cato_pos()),dir,_RichCoo[1],theta,phi,sleng);

  geant direct,reflected,length;
  AMSRichRing::ring_fraction(real_track,direct,reflected,length,1.0);
  
  _RichPath[0]=direct;
  _RichPath[1]=reflected;
  _RichLength=length;
  _RichParticles=AMSRichRawEvent::Npart();
  
  if(real_track->checkstatus(AMSDBc::TOFFORGAMMA)){ // MAKE RICH VTX PARTICLE AWARE
    _prich=0;
    
    if(_pvert){         // Probably there are new tracks there ... so add them to the rings stuff
#ifdef _PGTRACK_
      int maxtk=_pvert->NTrTrack();
#else 
      int maxtk=_pvert->getntracks();
#endif
	
      for(int i=0;i<maxtk;i++){
#ifdef _PGTRACK_
	AMSTrTrack *track=dynamic_cast<AMSTrTrack*>(_pvert->pTrTrack(i));
#else
	AMSTrTrack *track=_pvert->gettrack(i);
#endif
	//       AMSRichRing *ring=AMSRichRing::rebuild(track);
	AMSRichRing *ring=AMSRichRing::build(track,10);
	if(!ring){ring=AMSRichRing::build(track,0);if(ring)ring->setstatus(dirty_ring);}
	
	if(ring){
	  // Put a flag in this ring
	  ring->setstatus(AMSDBc::TOFFORGAMMA);
	  if(ring->next())
	    if(ring->next()->gettrack()==track) ring->next()->setstatus(AMSDBc::TOFFORGAMMA);
	}
      }
    }
    
    return;  // Do not try to assign a ring to the current particle
  }

  // Deal with AMSRichRing
  {
    //  Add more rings
    //AMSRichRing::rebuild(real_track);
    _prich=0;
    AMSRichRing* ptr=AMSRichRing::rebuild(real_track);
    if(!ptr && real_track==_ptrack)
      ptr=(AMSRichRing*)AMSEvent::gethead()->getheadC("AMSRichRing",0);
  

    //  AMSRichRing* ptr=(AMSRichRing*)AMSEvent::gethead()->getheadC("AMSRichRing",0);

    while(ptr){
      if(ptr->gettrack()==real_track){
	if(ptr->getused()>2) _prich=ptr;
	if(! ptr->IsGood() && ptr->next())
	  if(ptr->next()->gettrack()==real_track && ptr->next()->getused()>2)
	    if(RICRECFFKEY.recon[1]%10){
	      if(ptr->next()->getprob()>ptr->getprob() && ptr->next()->getused()>2) _prich=ptr->next();
	    }else _prich=ptr->next();
	//	  if(ptr->getused()>2 & ptr->IsGood())_prich=ptr;
	break;
      }
      ptr=ptr->next();
    }


    if(_prich){
      AMSRichRing::ring_fraction(real_track,direct,reflected,length,_prich->getbeta());
      _RichPathBeta[0]=direct;
      _RichPathBeta[1]=reflected;
      _RichLength=length;
      _prich->setstatus(AMSDBc::USED);
    }
  }

  // Deal with AMSRichRingNew
  {
    _prichB=0;
    AMSRichRingNew* ptr=(AMSRichRingNew*)AMSEvent::gethead()->getheadC("AMSRichRingNew",0);
    while(ptr){
      if(ptr->gettrack()==real_track && ptr->getUsed()>2){
	if((ptr->getStatus()%10)==2) {_prichB=ptr;break;}
      }
      ptr=ptr->next();
    }
  }

}

void AMSParticle::_writeEl(){
  int evt=AMSEvent::gethead()->getid();
  if((AMSEvent::gethead()->getC("AMSParticle",0)->getnelem()>0 || LVL3FFKEY.Accept) && (_ptrack->checkstatus(AMSDBc::NOTRACK) || _ptrack->checkstatus(AMSDBc::TRDTRACK)|| _ptrack->checkstatus(AMSDBc::ECALTRACK)))return;
#ifdef __WRITEROOT__
  // Fill Root class
  float phi  =fmod(_Phi+AMSDBc::twopi,AMSDBc::twopi);
  float phigl=fmod(_PhiGl+AMSDBc::twopi,AMSDBc::twopi);
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this, phi, phigl);
#endif
}


void AMSParticle::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  ParticleR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->Particle(_vpos);
  if (_pbeta)   ptr.fBeta  =_pbeta  ->GetClonePointer();
  else ptr.fBeta=-1;
  if (_pcharge) ptr.fCharge=_pcharge->GetClonePointer();
  else ptr.fCharge=-1;
  if (_ptrack)  ptr.fTrTrack =_ptrack ->GetClonePointer();
  else ptr.fTrTrack=-1;
  if (_ptrd)    ptr.fTrdTrack   =_ptrd   ->GetClonePointer();
  else ptr.fTrdTrack=-1;
  if (_phtrd)   ptr.fTrdHTrack   =_phtrd   ->GetClonePointer();
  else ptr.fTrdHTrack=-1;
  if (_prich)   ptr.fRichRing  =_prich  ->GetClonePointer();
  else ptr.fRichRing=-1;
  if (_prichB)   ptr.fRichRingB  =_prichB  ->GetClonePointer();
  else ptr.fRichRingB=-1;
  if (_pShower) ptr.fEcalShower=_pShower->GetClonePointer();
  else ptr.fEcalShower=-1;
  if (_pvert) ptr.fVertex=_pvert->GetClonePointer();
  else ptr.fVertex=-1;
#endif
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
  if(_Charge==0){    
    _GPart=1;  // photon
    _gpart[0]=1;  // photon
    _gpart[1]=0;  // nothing
    _fittedmom[0]=_Momentum;
    _fittedmom[1]=0;
    _prob[0]=1;
    _prob[1]=0;
    return;
  }
  const int maxp=38;
  number prob[maxp];
  number pfit[maxp];
  if(_pbeta && !_pvert){
    _beta=1/fabs(_pbeta->getbeta());
    _ebeta=_pbeta->getebeta();
    _ebeta=_ebeta*_ebeta;
    if(_prich  ){
      number b1=1./_prich->getbeta();
      number b2=_prich->geterrorbeta()*b1*b1;
      b2=b2*b2;
      if(fabs(b1-_beta)<3*sqrt(b2+_ebeta) || _beta<RICHDB::rad_index){
	_beta=(_beta/_ebeta+b1/b2)/(1/_ebeta+1/b2);
	_ebeta=1./(1/_ebeta+1/b2);
	_Beta=1/_beta;
	_ErrBeta=sqrt(_ebeta)/_Beta/_Beta;
	_calcmass(_Momentum,_ErrMomentum,_Beta,_ErrBeta,_Mass,_ErrMass);
      }
      else{
	static int fprint=0; 
	if(fprint++<100)cerr<<"RICH & TOF Disagree TOF Says velocity is "<<1/_beta<<" RICH "<<1/b1<<endl;
	_beta=b1;
	_beta=(_beta/_ebeta+b1/b2)/(1/_ebeta+1/b2);
	_ebeta=1./(1/_ebeta+1/b2);
	_Beta=1/_beta;
	_ErrBeta=sqrt(_ebeta)/_Beta/_Beta;
	_calcmass(_Momentum,_ErrMomentum,_Beta,_ErrBeta,_Mass,_ErrMass);


      }
    }
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
#ifndef  NO_NAG	
      e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
#endif
      geant chi2=f;
      if(chi2==chi2){
	prob[i]=PROB(chi2,1);
      }
      else prob[i]=0;
      if(_pcharge){
	prob[i]*=_pcharge->getprobcharge(int(chrg));   // work around case when charge defined by rich
	//      cout<<" probCharge="<<_pcharge->getprobcharge(int(chrg))<<" chrg="<<chrg<<endl;
      }
      //linux bug
      if(ifail)prob[i]=0;
      if(fabs(prob[i])>2.){
	cout<<"AMSPArticle::pid-E-Proberror " <<chi2<<" " <<prob[i]<<endl;
	prob[i]=0;
      }
      if(x[0]!=0)pfit[i]=1./fabs(x[0]);
      else pfit[i]=FLT_MAX;
    }
    _prob[1]=-2;
    _prob[0]=-1;
    _fittedmom[0]=_fittedmom[1]=0;
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
	if(prob[i]<0.1 && !_ptrack->checkstatus(AMSDBc::NOTRACK) && !_ptrack->checkstatus(AMSDBc::TRDTRACK)&& !_ptrack->checkstatus(AMSDBc::ECALTRACK)){
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
	if(
#ifdef _PGTRACK_
	   // PZ FIXME Track format
	   //	   ptrack->GetRigidity(AMTrTrack::kChoutko) <0 || ptrack->GetRigidity(AMTrTrack::kGeane)<0
	   ptrack->GetRigidity(AMSTrTrack::kChoutko) <0 || ptrack->GetRigidity(AMSTrTrack::kChoutko)<0
#else
	   ptrack->getrid() <0 || ptrack->getgrid()<0
#endif
	   ){
	  antimatter=1;
	  break;
	}
	ptrack=ptrack->next();
      }
    }
    if((antimatter || _Momentum <0) && !_ptrack->checkstatus(AMSDBc::NOTRACK) && !_ptrack->checkstatus(AMSDBc::TRDTRACK)&& !_ptrack->checkstatus(AMSDBc::ECALTRACK)){
   
      AMSEvent::gethead()->addnext(AMSID("AntiMatter",0),new AntiMatter(_GPart));
   
    }
    if(_Charge>1)AMSEvent::gethead()->addnext(AMSID("HeavyIon",0),new AntiMatter(_GPart));
  }
  else{
    if(_Momentum<0){
      _gpart[0]=3;
      _gpart[1]=3;
      _GPart=3;
      _fittedmom[0]=_Momentum;
      _prob[0]=0;
      _prob[1]=0;
    }
    else{ 
      _gpart[0]=2;
      _gpart[1]=2;
      _GPart=2;
      _fittedmom[0]=_Momentum;
      _prob[0]=0;
      _prob[1]=0;
    }
  }
}

void AMSParticle::refit(int fast){
  AMSgObj::BookTimer.start("ReTKRefit");
#ifndef _PGTRACK_  
  if(_ptrack->getpattern()>=0){
    AMSTrTrack *pfalse= AMSTrTrack::AddFalseX(_ptrack);
    if(pfalse){
         pfalse->setstatus(AMSDBc::USED);
        _ptrack=pfalse;
        _ptrack->AdvancedFit();
         //cout <<_ptrack->getpattern()<<endl;
        //cout << " falsex added "<<endl; 
   

}}
#endif
  number beta=_pbeta?_pbeta->getbeta():_Beta;
#ifdef _PGTRACK_
  for(int layer=0;layer<TkDBc::Head->nlay();layer++)
#else
    for(int layer=0;layer<TKDBc::nlay();layer++)
#endif
      {
	number theta,phi;
	bool bad=false;
#ifdef  _PGTRACK_
	//PZ FIXME REFIT //SH FIXED
	_TrCoo[layer]=_ptrack->GetResidualO(layer+1);
	if (_TrCoo[layer].norm() == 0 || layer==0 || layer==TkDBc::Head->nlay()-1){
	  bad=_ptrack->intercept(_TrCoo[layer],layer,theta,phi,_Local[layer])!=1;       }
	AMSDir dir;
        _ptrack->InterpolateLayerO(layer+1, _TrCoo[layer],dir);
#else
	if(!_ptrack->getres(layer,_TrCoo[layer]) || layer==0 || layer==TKDBc::nlay()-1){
	  bad=_ptrack->intercept(_TrCoo[layer],layer,theta,phi,_Local[layer])!=1;       }
#endif
	if(bad)setstatus(AMSDBc::BADINTERPOL);
	// Change theta,phi,coo 
	if(beta>=0 && layer==0){
	  _Theta=theta;
	  _Phi=phi;
	  _Coo=_TrCoo[0];
	  // change theta according to beta
	  if(_Theta<AMSDBc::pi/2){
	    _Theta=AMSDBc::pi-_Theta;
	    _Phi+=AMSDBc::pi;
	  }
	}       
	else if
#ifdef _PGTRACK_
	  (beta<0 && layer==TkDBc::Head->nlay()-1)
#else
	  (beta<0 && layer==TKDBc::nlay()-1)
#endif
	    {
	      _Theta=theta;    
	      // change theta according to beta
	      _Phi=phi;
#ifdef _PGTRACK_
	      _Coo=_TrCoo[TkDBc::Head->nlay()-1];
#else
	      _Coo=_TrCoo[TKDBc::nlay()-1];
#endif
	      if(_Theta>AMSDBc::pi/2){
		_Theta=AMSDBc::pi-_Theta;
		_Phi+=AMSDBc::pi;
	      }
	    }
      }
  if(fast || _ptrack->checkstatus(AMSDBc::NOTRACK) 
     || _ptrack->checkstatus(AMSDBc::TRDTRACK) 
     || _ptrack->checkstatus(AMSDBc::ECALTRACK)
     || _ptrack->checkstatus(AMSDBc::TOFFORGAMMA)){
    _loc2gl();
    AMSgObj::BookTimer.stop("ReTKRefit");  
    return;
  }

#ifndef _PGTRACK_
  integer dorefit=TRFITFFKEY.ForceAdvancedFit==1 ||
    (TRFITFFKEY.ForceAdvancedFit==2  &&
     !TRFITFFKEY.FastTracking);
#else
  integer dorefit=0;
#endif
  if(_GPart !=14 || dorefit){
    if(!_ptrack->AdvancedFitDone()){
#ifdef _PGTRACK_
      // It is done in TrRecon !!!!  _ptrack->DoAdvancedFit();//PZ FIXME
      //      if(_GPart!=14)_ptrack->FitP(AMSTrTrack::kChoutko,_GPart);
#else
      _ptrack->AdvancedFit();
#endif
    }
    //if(_GPart!=14)_ptrack->Fit(0,_GPart);
    AMSgObj::BookTimer.stop("ReTKRefit");    
    // Changed - never use geanerigidity to build mom

#ifndef _PGTRACK_
//    // PZ FIXME what is the purpose ?????
//    if(TRFITFFKEY.ForceAdvancedFit==1 && MISCFFKEY.G3On){
//      AMSgObj::BookTimer.start("ReGeaneRefit");
//      _ptrack->Fit(beta>0?3:-3,_GPart);
//      AMSgObj::BookTimer.stop("ReGeaneRefit"); 
//    }
#endif	
#ifdef _PGTRACK_
    _build(_ptrack->GetRigidity(),_ptrack->GetErrRinv(),_Charge,
	   _Beta,_ErrBeta,_Mass,_ErrMass,_Momentum,_ErrMomentum);
#else
    if(0 && _ptrack->GeaneFitDone() && fabs(_ptrack->getgrid())>TRFITFFKEY.RidgidityMin/2 ){
      _build(beta>0?_ptrack->getgrid():-_ptrack->getgrid(),
	     _ptrack->getegrid(),_Charge,_Beta,_ErrBeta,_Mass,_ErrMass,_Momentum,_ErrMomentum);
    }
    else{
      _build(_ptrack->getrid(),_ptrack->geterid(),_Charge,
	     _Beta,_ErrBeta,_Mass,_ErrMass,_Momentum,_ErrMomentum);
    }
#endif
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
  integer _errors=0;
  if(fabs(prod)>0.03 && !MISCFFKEY.BeamTest){
    cerr<<"AMSParticle::_loc2gl-E-AMSGlobalCoosystemIllDefined "<<prod<<" "<<amsxg<<" "<<amsyg<<" "<<amszg<<endl;
    AMSEvent::gethead()->_printEl(cerr);
    AMSEvent::gethead()->seterror(_errors++>100?3:2);
  }
  else{
    _errors=0;
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




// additional roll 12 deg

    for(int i=0;i<3;i++)amsxg[i]=amsx[i];
    for(int i=0;i<3;i++)amsyg[i]=amsy[i];
    for(int i=0;i<3;i++)amszg[i]=amsz[i];
   pitch=0;
   yaw=0;
   roll=CCFFKEY.Angle/180*3.1415926;
   cp=cos(pitch);
   sp=sin(pitch);
   cy=cos(yaw);
   sy=sin(yaw);
   cr=cos(roll);
   sr=sin(roll);
   l1=cy*cp;
   m1=-sy;
   n1=cy*sp;
   l2=cr*sy*cp-sr*sp;
   m2=cr*cy;
   n2=cr*sy*sp+sr*cp;
   l3=-sr*sy*cp-cr*sp;
   m3=-sr*cy;
   n3=-sr*sy*sp+cr*cp;

  amsx[0]=(l1*amsxg[0]+l2*amsyg[0]+l3*amszg[0]);
  amsx[1]=(l1*amsxg[1]+l2*amsyg[1]+l3*amszg[1]);
  amsx[2]=(l1*amsxg[2]+l2*amsyg[2]+l3*amszg[2]);
  amsy[0]=(m1*amsxg[0]+m2*amsyg[0]+m3*amszg[0]);
  amsy[1]=(m1*amsxg[1]+m2*amsyg[1]+m3*amszg[1]);
  amsy[2]=(m1*amsxg[2]+m2*amsyg[2]+m3*amszg[2]);
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
  if(rgm/EarthR>2){

//   work around stupid bug

     rgm=rgm/100;
     
     xfac=57.576*EarthR/rgm*EarthR/rgm;
  
 }
  number chsgn=_Momentum>0?1:-1;
  number cl3=cl*cl*cl;
  number cl4=cl*cl*cl*cl;
  number mom=xfac*cl4/(sqrt(1.-chsgn*cth*cl3)+1)/(sqrt(1.-chsgn*cth*cl3)+1)*_Charge;
  _CutoffMomentum=chsgn*mom;
  AMSgObj::BookTimer.stop("part::loc2gl");

}


void AMSParticle::alfun(integer & n , number xc[], number &fc, AMSParticle *p){
  number zbeta=p->_beta;
  if(zbeta<1)zbeta=2-zbeta;
  fc=(fabs(xc[0])-p->_mom)/p->_emom*(fabs(xc[0])-p->_mom)+
    (sqrt(1+xc[0]*xc[0]*p->_mass)-zbeta)/p->_ebeta*
    (sqrt(1+xc[0]*xc[0]*p->_mass)-zbeta);
}
 
 
AMSParticle::AMSParticle(AMSVtx *pvert):_pvert(pvert),_ptrack(0),
					_ptrd(0),_phtrd(0),_prich(0),_prichB(0),_pShower(0),_pcharge(0),_pbeta(0){
  int i;
  for(i=0;i<4;i++)_TOFCoo[i]=AMSPoint(0,0,0);
  for(i=0;i<2;i++)_AntiCoo[i]=AMSPoint(0,0,0);
  for(i=0;i<2*ecalconst::ECSLMX;i++)_EcalCoo[i]=AMSPoint(0,0,0);
  for(i=0;i<3;i++)_EcalSCoo[i]=AMSPoint(0,0,0);
  for(i=0;i<2;i++)_RichCoo[i]=AMSPoint(0,0,0);
  for(i=0;i<2;i++)_RichPath[i]=0.;
  for(i=0;i<2;i++)_RichPathBeta[i]=0.;
  _RichLength=0.;
#ifdef _PGTRACK_
  for(i=0;i<TkDBc::Head->nlay();i++)
#else
    for(i=0;i<TKDBc::nlay();i++)
#endif
      {
	_TrCoo[i]=AMSPoint(0,0,0);
	_Local[i]=0;
      }
  
  _Charge=pvert->getcharge();
  _Mass=pvert->getmass();
  _Momentum=pvert->getmom();
  _ErrMomentum=pvert->geterrmom();
  _Theta=pvert->gettheta();
  _Phi=pvert->getphi();
  _Coo=pvert->getvert();
  _ErrMass=_ErrMomentum/_Momentum*_Mass;
  //
  // Set Charge according to other stuff i.e. put - sign to momentum
  if(_Charge<0){
    _Charge=-_Charge;
    _Momentum=-_Momentum;
  }
  
  //   find beta
  _Beta=0;
  _ErrBeta=1;
  number betamin = 999.;
  for(int patb=0; patb<npatb; patb++){
    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){
      AMSTrTrack *ptrbeta = pbeta->getptrack();
      AMSTrTrack *ptr = NULL;
#ifdef _PGTRACK_
      for (i=0;i<pvert->NTrTrack();i++) {
	if (ptrbeta==pvert->pTrTrack(i)) {
	  ptr = ptrbeta;
	  break;
	}
      }
#else
      for (i=0;i<pvert->getntracks();i++) {
	if (ptrbeta==pvert->gettrack(i)) {
	  ptr = ptrbeta;
	  break;
	}
      }
#endif
      if (ptr && fabs(pbeta->getbeta())<betamin) { 
	betamin = fabs(pbeta->getbeta());
	_Beta = pbeta->getbeta();
	_pbeta = pbeta;
	_ErrBeta = pbeta->getebeta()*_Beta*_Beta;
      }
      pbeta=pbeta->next();
    }
  }
  
  if(!_pbeta){
    for(int patb=0; patb<npatb; patb++){
      AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
      while(pbeta){
	if(pbeta->getptrack()->checkstatus(AMSDBc::NOTRACK)){
	  _Beta=pbeta->getbeta();
	  _pbeta=pbeta;
	  _ErrBeta=pbeta->getebeta()*_Beta*_Beta;
	  break;
	}
	pbeta=pbeta->next();
      }
      if(_pbeta)break;
    }
  }
  
  //try to find richring beta
  number oldbeta=_Beta;
  number oldebeta=_ErrBeta;
  _Beta=0;
  _ErrBeta=0;
  number beta=0;
  number beta2=0;
  number nbfound=0;
#ifdef _PGTRACK_
  for(int i=0;i<pvert->NTrTrack();i++)
#else
    for(int i=0;i<pvert->getntracks();i++)
#endif
      {
	for(AMSRichRing* ptr=(AMSRichRing*)AMSEvent::gethead()->getheadC("AMSRichRing",0);ptr;ptr=ptr->next()){
#ifdef _PGTRACK_
	  if(ptr->gettrack()==pvert->pTrTrack(i) && ptr->getused()>2)
#else
	    if(ptr->gettrack()==pvert->gettrack(i) && ptr->getused()>2)
#endif
	      {
		// found beta
		nbfound++;
		beta+=ptr->getbeta();
		beta2+=ptr->getbeta()*ptr->getbeta();
		number err=1./ptr->geterrorbeta()/ptr->geterrorbeta();
		_Beta+=ptr->getbeta()*err;
		_ErrBeta+=err;
		break;
	      }
	}
      }
  if(_ErrBeta!=0){
    beta/=nbfound;
    beta2/=nbfound;
    beta2=sqrt(beta2-beta*beta)/sqrt(nbfound);
    
    _Beta/=_ErrBeta;
    if(oldbeta<0)_Beta=-_Beta;
    _ErrBeta=1/sqrt(_ErrBeta);
    if(_ErrBeta<beta2)_ErrBeta=beta2;
    //     cout <<" rich beta found "<<_Beta<<" "<<_ErrBeta<<" "<<beta2<<" "<<nbfound<<endl;
  }
  else{
    _Beta=oldbeta;
    _ErrBeta=oldebeta; 
  } 
  // make false track
  if(!_Charge)_ptrack=new AMSTrTrack(_Theta,_Phi,_Coo);
  else _ptrack= new AMSTrTrack(AMSDir(_Theta,_Phi),_Coo,_Momentum/_Charge,_ErrMomentum/_Momentum/_Momentum); 
  _ptrack->setstatus(AMSDBc::TOFFORGAMMA); 
  
}
  

