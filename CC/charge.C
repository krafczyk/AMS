//  $Id: charge.C,v 1.88 2012/01/17 13:32:45 oliva Exp $
// Author V. Choutko 5-june-1996
//
//
// Lat Edit : Mar 20, 1997. ak. AMSCharge::Build() check if psen == NULL
//
// Modified by E.Choumilov 17.01.2005 for AMS02 
// Modified by J.Casaus    10.05.2011 
//
#include "beta.h"
#include "commons.h"
#include <math.h>
#include <limits.h>
#include "amsgobj.h"
#include "extC.h"
#include "upool.h"
#include "charge.h"
#include <stdlib.h>
#include "amsstl.h"
#include "ntuple.h"
#include "cern.h"
#ifdef _PGTRACK_
#include "trrec.h"
#include "tkdcards.h"
#else
#include "trrawcluster.h"
#endif
#include "job.h"

int AMSCharge::_debug=0;     // debug level for AMSCharge
int AMSChargeSubD::_debug=0; // debug level for AMSChargeSubD

using namespace std;
using namespace trconst;
using namespace AMSChargConst;

PROTOCCALLSFFUN2(FLOAT,PROB,prob,FLOAT,INT)
#define PROB(A2,A3)  CCALLSFFUN2(PROB,prob,FLOAT,INT,A2,A3)

#ifdef _PGTRACK_
bool TkTRDMatch(AMSTrTrack* ptrack,AMSTRDTrack *ptrd);
#endif

integer AMSChargeTOF::_chargeTOF[MaxZTypes]={1,1,2,3,4,5,6,7,8,9};
integer AMSChargeTracker::_chargeTracker[MaxZTypes]={1,1,2,3,4,5,6,7,8,9};

int AMSChargeTOF::ComputeCharge(int refit){

  // Check pointers
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }

  // Initialize
  Init(refit);

  // flag bad betas
  FlagBad();

  // Fill Charge info
  if(!Fill(refit)) return 0;

  // Fill vectors
  FillRootVectors();

  return 1;
}



int AMSChargeTracker::ComputeCharge(int refit){

  // Check pointers
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }

  if (_ptrtk == NULL) {
    cout << "AMSChargeTracker::ComputeCharge-W-NullAMSTrTrackPointer" << endl;
    return 0;
  }

  // Initialize
  Init(refit);

  // flag bad tracks
  FlagBad();

  // Fill charge
  if(!Fill(refit)) return 0;

  // Fill vectors
  FillRootVectors();

  return 1;
}

int AMSChargeRich::ComputeCharge(int refit) {

  // Check pointers
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }

  if (_pring == NULL) {
    cout << "AMSChargeRich::ComputeCharge-W-NullRichRingPointer" << endl;
    return 0;
  }

  // Initialize
  Init(refit);

  // flag bad rings
  FlagBad();

  // Fill charge 
  if (!Fill(refit)) return 0;

  if (_debug) 
    cout << "AMSChargeRich::ComputeCharger-I Charge : " << _Charge << endl;
    //for (i=0; i<MaxZTypes; i++)
    //  cout << "i lkhd prob " << i << " " << _Lkhdz[i] << " " << _Probz[i] << endl;

  // Fill vectors
  FillRootVectors();

  return 1;
}


int AMSChargeTRD::ComputeCharge(int refit) {

  // Check pointers
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }

  if (_ptrd == NULL) {
    cout << "AMSChargeTRD::ComputeCharge-W-NullTRDTrackPointer" << endl;
    return 0;
  }

  // Initialize
  Init(refit);

  // flag bad tracks
  FlagBad();

  // Fill charge
  if (!Fill(refit)) return 0; 

  // Fill vectors
  FillRootVectors();

  return 1;
}


void AMSChargeSubD::Init(int refit){

  _ProbSum = 0;

  if(!refit){
    _i=0;
    _Charge=0;
    _Probz.clear();
    _Indxz.clear();
    _Lkhdz.clear();
    clearstatus(0xFFFFFFFF);
    _ChargeI.clear();
    _Lkhd.clear();
    _Prob.clear();
  }

}

void AMSChargeSubD::lkhcalc(int mode, double beta, int nhit, double ehit[], int typeh[], double lkh[]){
  for(int i=0; i<MaxZTypes; i++){
    lkh[i]=0;
    if(mode==0)lkh[i]+=TofElosPDF::TofEPDFs[i].getlkhd(nhit,typeh,ehit,beta);//TOF
#ifndef _PGTRACK_
    else if(mode==1)lkh[i]+=TrkElosPDF::TrkEPDFs[i].getlkhd(nhit,typeh,ehit,beta);//TRK
#endif
  }
}


double AMSChargeSubD::resmax(double x[],int ntot,int refit,double rescut,int &imax,double &mean,double &trres,double &trmax){

  imax=-1;
  mean=ntot?x[0]:0;
  trres=ntot?x[0]:0;
  trmax=ntot?x[0]:0;

  if(ntot<2) return 0;

  double sigmin=CHARGEFITFFKEY.SigMin;
  double sigmax=refit?1:CHARGEFITFFKEY.SigMax;
  assert(sigmin>0 && sigmax>0 && sigmax>sigmin);

  double rsmx=0;
  for(int i=0; i<ntot; i++){
    if (x[i]>0){
      double xm=0, xxm=0;
      int n=0;
      for(int j=0; j<ntot; j++){
        if(j!=i && x[j]>0){
          n++;
          xm+=x[j];
          xxm+=pow(x[j],2);
        }
      }
      if (n>1){//>=2hits in the rest group)
        double sig,rs;
        sig=sqrt(max((n*xxm-pow(xm,2))/n/(n-1),0.));
        sig=max(sig,sigmin*xm/n);
        sig=min(sig,sigmax*xm/n);
        rs=(x[i]-xm/n)/sig;//deviation in sigmas of the rest
        if (fabs(rs)>rsmx){
          rsmx=fabs(rs);
          imax=i;//store index of hit with max.deviation from the average of the rest
        }
      }
    }
  }

  int cut=(rsmx>rescut&&rescut>=0)?1:0;//=0, if incomp.hit removal was not requested
  imax=cut?imax:-1;//imax =-1 if incomp.hit removal was not requested

  mean=0;
  trres=0;
  double xmx=0;
  int imx=-1;
  int n=0;
  for(int i=0; i<ntot; i++){//ntot>=2
    if(x[i]>0){
      n++;
      mean+=x[i];
      if(i!=imax) trres+=x[i];
      if(x[i]>xmx){
        xmx=x[i];//store max.hit value
	imx=i;//index
      }
    }
  }

  trmax=mean;
  if(n>1) {
    mean=mean/n;//mean
    trmax=(trmax-xmx)/(n-1);//normal("-max.hit") trunc.mean
    trres=cut?trres/(n-1):trres/n;//"-incomp.hit" trunc.mean/normal_mean
  }

  return rsmx;//return max deviation (if incomp.hit removal requested) or 0
}


double AMSChargeSubD::probcalc(int fit, int nhit, double lkhd[]){

  vector<double> prob(MaxZTypes,0);

  if(fit>=0){
    Init();
    for(int i=0; i<MaxZTypes; i++) _Lkhdz.push_back(lkhd[i]);
    _i=sortlkhd();
  }

  double probmx=1;
//  probmx=0.;
//  for(i=0; i<types; i++){
//    double lkhdnorm=mode?TrkElosPDF::TrkEPDFs[_iTracker].getstep(i):TofElosPDF::TofEPDFs[_iTOF].getstep();
//    if (nhittyp[i]>0) probmx+=nhittyp[i]*log(lkhdnorm);
//  }
//  probmx=1./exp(min(probmx/nhit,powmx));

  double probs(0);
  for(int i=0; i<MaxZTypes; i++){
    prob[i]=1./exp(min(lkhd[i]/nhit,powmx))/probmx;
    probs+=prob[i];
  }

  if (fit>=0) {
    _Probz = prob;
    _ProbSum = probs;
  }

  //cout << " _i _Probz[i] prob[i] " << _i << " " <<
  // getprobcharge(_i, CHARGEFITFFKEY.NormalizeProbs) << " " << prob[_i] << endl;

  return prob[_i]/(CHARGEFITFFKEY.NormalizeProbs?_ProbSum:1);
} 


int AMSChargeSubD::sortlkhd() {

  vector<double> lkhs=_Lkhdz;
  int imax = -1;
  _Indxz.clear();
  _Indxz.resize(_Lkhdz.size(),-1);
  sort(lkhs.begin(),lkhs.end());//sort in increasing order(1st place - min(-logPw(i)) 
  //                                                                ie max.prob)

  // unfortunate heritage from previous versions ... (TO BE REMOVED) 
  // here we fill index for the unsorted vectors 
  for(int i=0;i<lkhs.size();i++)
    for(int j=0;j<_Lkhdz.size();j++) {
      if(lkhs[i] == _Lkhdz[j] && _Indxz[j]<0) {
        _Indxz[j]=i;
        lkhs[i] = -HUGE_VAL;
        if (!i) imax=j; //store Z-index with max prob. 
      }
    }

  // check for negative indexes (unwanted feature on repeated values)
  int size = _Indxz.size(); 
  for (int i=0; i<size; i++) {
    if (_Indxz[i]<0) {
      if (i>0&&_Indxz[i-1]>0)_Indxz[i]=_Indxz[i-1]-1; 
      else {
	cout << "AMSChargeSubD::sortlkhd-W-UnexpectedSortedIndexSequence";
	for (int j=0; j<size; j++)cout <<" "<< _Indxz[j];
	cout << endl;
      }
    }
  }

  return imax;
}


void AMSChargeSubD::FillRootVectors() {

  if (_debug)
    cout << "AMSChargeSubD::FillRootVector-I-ID " << _ID << endl;

  _ChargeI.resize(_Indxz.size());
  _Lkhd.resize(_Indxz.size());
  _Prob.resize(_Indxz.size());

  for (int i=0; i<_Indxz.size(); i++) {
    int j=_Indxz[i];
    if (_debug)
      cout << "Index " << i << " in position " << j << " like " << _Lkhdz[i] << endl;
    if (j>=0 && j<_Indxz.size()) {
      _ChargeI[j] = i;
      _Lkhd[j] = _Lkhdz[i];
      //_Prob[j] = _Probz[i];
      _Prob[j] = getprobcharge(i,CHARGEFITFFKEY.NormalizeProbs);
    }
  }

  return; 
}


int AMSChargeSubD::getsplitlevel() {

  if      (_ID.Contains(AMSChargeTOF::ClassID())) 
    return CHARGEFITFFKEY.SplitLevel[kTOF];
  else if (_ID.Contains(AMSChargeTracker::ClassID()))
    return CHARGEFITFFKEY.SplitLevel[kTracker];
  else if (_ID.Contains(AMSChargeTRD::ClassID()))
    return CHARGEFITFFKEY.SplitLevel[kTRD];
  else if (_ID.Contains(AMSChargeRich::ClassID()))
    return CHARGEFITFFKEY.SplitLevel[kRich];

  cout << "AMSChargeSubD::getsplitlevel-W-UnknownID " << _ID << endl;

  return 0; 
}


int AMSChargeSubD::getchargemax() {

  if      (_ID.Contains(AMSChargeTOF::ClassID())) 
    return CHARGEFITFFKEY.ChargeMax[kTOF]; 
  else if (_ID.Contains(AMSChargeTracker::ClassID()))
    return CHARGEFITFFKEY.ChargeMax[kTracker];
  else if (_ID.Contains(AMSChargeTRD::ClassID()))
    return CHARGEFITFFKEY.ChargeMax[kTRD];
  else if (_ID.Contains(AMSChargeRich::ClassID()))
    return CHARGEFITFFKEY.ChargeMax[kRich];

  cout << "AMSChargeSubD::getchargemax-W-UnknownID " << _ID << endl;

  return 0;
}


double AMSChargeSubD::getprobmin() {

  if      (_ID.Contains(AMSChargeTOF::ClassID())) 
    return CHARGEFITFFKEY.ProbMin[kTOF];
  else if (_ID.Contains(AMSChargeTracker::ClassID()))
    return CHARGEFITFFKEY.ProbMin[kTracker];
  else if (_ID.Contains(AMSChargeTRD::ClassID()))
    return CHARGEFITFFKEY.ProbMin[kTRD];
  else if (_ID.Contains(AMSChargeRich::ClassID()))
    return CHARGEFITFFKEY.ProbMin[kRich];

  cout << "AMSChargeSubD::getprobmin-W-UnknownID " << _ID << endl;

  return 0;
}


int AMSCharge::build(int refit){

  if (refit) return AMSCharge::refit(); // no refit implemented yet 

  for(int patb=0; patb<npatb; patb++){

    AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->getheadC("AMSBeta",patb);
    while(pbeta){

      AMSCharge *pcharge = new AMSCharge(pbeta);

      AMSEvent::gethead()->addnext(AMSID("AMSCharge",0),pcharge);
   
      pbeta=pbeta->next();

    }

  }

  return 0;
}


int AMSCharge::BuildTOF(AMSBeta *pbeta) {

  if (pbeta == NULL) return 0;
  
  AMSChargeTOF *chargetof = new AMSChargeTOF(pbeta);

  if (chargetof == NULL) return 0;

  if (chargetof->ComputeCharge()) 
    _charges[chargetof->_ID]=chargetof;
  else {
    delete chargetof;
    return 0;
  }
  if (_debug) chargetof->_printEl(cout);

  return 1;
}


int AMSCharge::BuildTracker(AMSBeta *pbeta) {

  if (pbeta == NULL) return 0;

  AMSTrTrack *ptrtk=pbeta->getptrack();
  if (ptrtk == NULL) return 0;

  if (ptrtk->getnhits() == 0) return 0; // fake track check

  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkAll)%10) != 0) {

    AMSChargeTracker *chargetrk = new AMSChargeTracker(pbeta,ptrtk);

    if (chargetrk != NULL) {
      if (chargetrk->ComputeCharge()) 
	_charges[chargetrk->_ID]=chargetrk;
      else delete chargetrk;
      if (_debug) chargetrk->_printEl(cout);
    }
  }

  // Inner planes
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkInner)%10) != 0) {

    AMSChargeTrackerInner *chargetrkinner = new AMSChargeTrackerInner(pbeta,ptrtk);

    if (chargetrkinner != NULL) {
      if (chargetrkinner->ComputeCharge())
	_charges[chargetrkinner->_ID]=chargetrkinner;
      else delete  chargetrkinner;
      if (_debug) chargetrkinner->_printEl(cout);
    }

  }

  // Upper planes
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkUpper)%10) != 0) {

    AMSChargeTrackerUpper *chargetrkupper = new AMSChargeTrackerUpper(pbeta,ptrtk);
    if (chargetrkupper != NULL) {
      if (chargetrkupper->ComputeCharge())
	_charges[chargetrkupper->_ID]=chargetrkupper;
      else delete chargetrkupper;
      if (_debug) chargetrkupper->_printEl(cout);
    }

  }

  // Lower plane
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkLower)%10) != 0) {

    AMSChargeTrackerLower *chargetrkbottom = new AMSChargeTrackerLower(pbeta,ptrtk);

    if (chargetrkbottom != NULL) {
      if(chargetrkbottom->ComputeCharge())
	_charges[chargetrkbottom->_ID]=chargetrkbottom;
      else delete chargetrkbottom;

      if (_debug) chargetrkbottom->_printEl(cout);
    }

  }

  return 1; 
}


int AMSCharge::BuildTRD(AMSBeta *pbeta) {       

  if (pbeta == NULL) return 0;

  AMSTrTrack *ptrtk=pbeta->getptrack();
  if (ptrtk == NULL) return 0; 

  AMSTRDTrack *ptrd=NULL;
   bool notrefitted=true;
again:
  int ntrd=0;
  AMSTRDTrack* p=(AMSTRDTrack*)AMSEvent::gethead()->getheadC("AMSTRDTrack",0,0);
  while(p){
#ifndef _PGTRACK_
    if (ptrtk->TRDMatch(p)) {
#else
    if (TkTRDMatch(ptrtk,p)) {
#endif
      ptrd=p;
      break;
    }
    p=p->next();
  }   

  if (ptrd == NULL) {

   if(notrefitted){
     notrefitted=false;
     if(TRDRECFFKEY.ResolveAmb && AMSTRDTrack::ResolveAmb(ptrtk))goto again;
     else return 0;
   }

    
    else return 0; 
  }

  AMSChargeTRD *chargetrd = new AMSChargeTRD(pbeta,ptrd);

  if (chargetrd == NULL) return 0;

  if (chargetrd->ComputeCharge()) 
    _charges[chargetrd->_ID]=chargetrd;
  else {
    delete chargetrd;
    return 0;
  }
  if (_debug) chargetrd->_printEl(cout);

  return 1; 
}


int AMSCharge::BuildRich(AMSBeta *pbeta) {

  AMSTrTrack *ptrtk=pbeta->getptrack();
  if (ptrtk == NULL) return 0;

  AMSRichRing *pring=NULL;
  AMSRichRing *p=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
  while(p){
    AMSTrTrack *prctk=p->gettrack();
    if(prctk==ptrtk) {
      pring=p;
      break;
    }
    p=p->next(); 
  }

  if (pring == NULL) return 0;

  AMSChargeRich *chargerich = new AMSChargeRich(pbeta,pring);

  if (chargerich == NULL) return 0;

  if (chargerich->ComputeCharge()) 
    _charges[chargerich->_ID]=chargerich;
  else {
     delete chargerich;
     return 0;
  }
  if (_debug) chargerich->_printEl(cout);

  return 1; 
}


int AMSCharge::BuildCombine() {

  _i=-1;
  _Charge = 0;
  _Nused = 0;

  _Probz.clear();
  _Indxz.clear();
  _Lkhdz.clear();

  clearstatus(0xFFFFFFFF);

  if (!SelectSubDCharges()) {
    setstatus(AMSDBc::WEAK);
    if (!SelectSubDCharges()) 
      setstatus(AMSDBc::BAD);
  }

  if (_Nused<=0) {
    cout << "AMSCharge::BuildCombine-W-NoValidSubDChargesFound " << endl;
    return 0;
  }

  int indmx;
  getvotedcharge(indmx);

  if (_Charge<=0) {
    cout << "AMSCharge::BuildCombine-W-NoValidAMSChargeFound " << endl;
    return 0;
  }

  if (_debug)
    cout << "AMSCharge::BuildCombine-I-Charge : _Charge " << _Charge << endl;
  //for (int ind=0; ind<_Probz.size(); ind++) getprobcharge(ind);

  FillRootVectors();

  return 1;
}


int AMSCharge::SelectSubDCharges() {

  _Nused = 0;

  map<TString,AMSChargeSubD*>::iterator i;

  AMSChargeSubD* bestTrCharge=NULL;

  bool weak = checkstatus(AMSDBc::WEAK);

  for(i=_charges.begin();i!=_charges.end();++i) {

    AMSChargeSubD *chargeSubD = i->second;

    int charge = chargeSubD->getcharge();
    float prob = chargeSubD->getprobcharge(chargeSubD->getindex(), CHARGEFITFFKEY.NormalizeProbs);
    int chargemax = chargeSubD->getchargemax();
    float probmin = chargeSubD->getprobmin();
    int splitlevel = chargeSubD->getsplitlevel();
    
    bool bad = chargeSubD->checkstatus(AMSDBc::BAD);

    chargeSubD->clearstatus(AMSDBc::USED);

    if(charge>0 && !bad &&
       ( weak ||
	 (chargemax<0 || charge<=chargemax) && prob>probmin ) ) {

      if(chargeSubD->_ID.Contains(AMSChargeTracker::ClassID())){

	if (splitlevel>0) {
	  if (chargeSubD->_ID.CompareTo(AMSChargeTracker::ClassID()))
	    chargeSubD->setstatus(AMSDBc::USED);
	}
	else {
	  if (!chargeSubD->_ID.CompareTo(AMSChargeTracker::ClassID())) {
	    if(bestTrCharge==NULL || chargeSubD->_Priority>bestTrCharge->_Priority) {
	      if (bestTrCharge!=NULL) bestTrCharge->clearstatus(AMSDBc::USED);
	      bestTrCharge = chargeSubD;
	      bestTrCharge->setstatus(AMSDBc::USED);
	    }
	  }
	}

      }
      else chargeSubD->setstatus(AMSDBc::USED);
    }

    if (_debug) 
      cout << "AMSCharge::SelectSubDCharges-I-chargeSubDID : " << chargeSubD->getID()
	   << " used : " << (chargeSubD->checkstatus(AMSDBc::USED) > 0) << endl;

    if (chargeSubD->checkstatus(AMSDBc::USED)) _Nused++;
  }

  if (_debug)
    cout << "AMSCharge::SelectSubDCharges-I-_Nused : " << _Nused << endl;

  return (_Nused>0);
}


int AMSCharge::getvotedcharge(int &iVoted) {

  if (_Charge >0) {
    iVoted=_i;
    return _Charge;
  }

  _Charge = 0;

  if (_Nused<=0) return 0;

  map<TString,AMSChargeSubD*>::iterator i;

  vector<int> ind2charge;

  for (i=_charges.begin();i!=_charges.end();++i) {

    AMSChargeSubD *chargeSubD = i->second;

    if (!chargeSubD->checkstatus(AMSDBc::USED)) continue;

    int indmx = chargeSubD->getindex();
    int size = chargeSubD->getsize();

    if (_Lkhdz.empty()) {
      _Indxz.assign(size,-1);
      _Lkhdz.assign(size,0);
      _Probz.assign(size,1);
    }
    else if (size != _Lkhdz.size()) {
      cout << "AMSCharge::getvotedcharge-W-InconsistentSubDVectorSizes : "
	   << chargeSubD->getID() << " size " << size << " RefSize " << _Lkhdz.size() << endl;
      return 0;
    }

    if (_debug)
      cout << "AMSCharge::getvotedcharge-I-chargeSubDID : " << chargeSubD->getID()
	   << " size " << size << " indmx " << indmx << endl;

    for (int index=0; index<size; index++) {

      //cout << "index " << index << " lkhd " << chargeSubD->getlkhdcharge(index)<< endl;
      _Lkhdz[index] += chargeSubD->getlkhdcharge(index);
      _Probz[index] *= chargeSubD->getprobcharge(index, CHARGEFITFFKEY.NormalizeProbs)/
	              chargeSubD->getprobcharge(indmx, CHARGEFITFFKEY.NormalizeProbs);
      //cout << "_Lkhdz " << _Lkhdz[index] << " _Probz " <<  _Probz[index] << endl;

    }

    if (ind2charge.empty()) {
      ind2charge.assign(size,0);
      for (int ind=0; ind<size; ind++) ind2charge[ind] = chargeSubD->ind2charge(ind);
    }

  }

  // Find voted & Fill vectors
  vector<double> test, save;
  if (CHARGEFITFFKEY.UseLikelihood) test=_Lkhdz;
  else {
    test=_Probz;
    for (int i=0; i<test.size(); i++) test[i]*=-1;
  }
  save=test;
  sort(test.begin(),test.end());

  // again the unfortunate heritage ... again to be removed
  _i=-1;
  for(int i=0;i<test.size();i++)
    for(int j=0;j<save.size();j++) {
      if(test[i] == save[j] && _Indxz[j]<0) {
	_Indxz[j]=i;
	test[i] = -HUGE_VAL;
        if (!i) _i=j; //store Z-index with max prob. 
      }
    }

  iVoted = _i;
  if (iVoted>=0) _Charge = ind2charge[iVoted];

  if (_debug)
    cout << "AMSCharge::getvotedcharge-I-VotedCharge : charge " << _Charge << " index " << iVoted << endl;

  return _Charge;
}


double AMSCharge::getprobcharge(int charge) {

  double prob=0;

  charge=abs(charge);
  if(charge>MaxZTypes){
    cerr <<" AMSCharge::getprobcharge-E-charge too big "<<charge<<endl;
    return 0;
  }

  int indmx;
  int voted=getvotedcharge(indmx);
  int index=charge;

  map<TString,AMSChargeSubD*>::iterator i;

  vector<int> ind2charge;

  prob = 1;

  for (i=_charges.begin();i!=_charges.end();++i) {

    AMSChargeSubD *chargeSubD = i->second;

    if (!chargeSubD->checkstatus(AMSDBc::USED)) continue;

    prob *= chargeSubD->getprobcharge(index, CHARGEFITFFKEY.NormalizeProbs)/
            chargeSubD->getprobcharge(indmx, CHARGEFITFFKEY.NormalizeProbs);

  }

  //cout << "AMSCharge::getprobcharge : charge " << charge << " voted " << voted << " index " << index
  //     << " prob " << prob << endl;

 return prob;
}


void AMSCharge::FillRootVectors() {

  if (_debug)
    cout << "AMSCharge::FillRootVector-I- size : " << _Indxz.size() << endl;

  _ChargeI.resize(_Indxz.size());
  _Lkhd.resize(_Indxz.size());
  _Prob.resize(_Indxz.size());

  for (int i=0; i<_Indxz.size(); i++) {
    int j=_Indxz[i];
    if (_debug)
      cout << "Index " << i << " in position " << j << " prob " << _Probz[i] << endl;
    if (j>=0 && j<_Indxz.size()) {
      _ChargeI[j] = i;
      _Lkhd[j] = _Lkhdz[i];
      _Prob[j] = _Probz[i];
    }
  }

  return; 
}


int AMSCharge::getchargeTOF() {

  map<TString,AMSChargeSubD*>::iterator i=_charges.find("AMSChargeTOF");

  if(i==_charges.end()) return -1;
  else return i->second->getcharge();
}


int AMSCharge::getchargeTracker() {

  map<TString,AMSChargeSubD*>::iterator i=_charges.find("AMSChargeTracker");

  if(i==_charges.end()) return -1;
  else return i->second->getcharge();
}


AMSCharge::AMSCharge(AMSBeta *pbeta) {

  _i=-1;
  _Charge = 0;
  _pbeta = pbeta;

  if (CHARGEFITFFKEY.RecEnable[kTOF] > 0) {
    if (_debug) cout << "AMSCharge::AMSCharge-I-BuildTOF" << endl;
    BuildTOF(pbeta);
  }

  if (CHARGEFITFFKEY.RecEnable[kTracker] > 0) {
    if (_debug) cout << "AMSCharge::AMSCharge-I-BuildTracker" << endl;
    BuildTracker(pbeta);
  }

  if (CHARGEFITFFKEY.RecEnable[kTRD] > 0) {
    if (_debug) cout << "AMSCharge::AMSCharge-I-BuildTRD" << endl;
    BuildTRD(pbeta);
  }

  if (CHARGEFITFFKEY.RecEnable[kRich] > 0) {
    if (_debug) cout << "AMSCharge::AMSCharge-I-BuildRich" << endl;
    BuildRich(pbeta);
  }

  if (_debug) cout << "AMSCharge::AMSCharge-I-BuildCombine" << endl;
  BuildCombine();

}


int AMSChargeTRD::Fill(int refit) {

  _ptrd->ComputeCharge(_pbeta->GetTRDBetaCorr());

  if (_ptrd->_Charge.Nused <= 0) return 0;

  _Q = _ptrd->_Charge.Q;
  _i = _ptrd->_Charge.Charge[0];
  _Charge = ind2charge(_i);

  // Fill vectors
  double probmx = 1;
  int nc = sizeof(_ptrd->_Charge.Charge)/sizeof(_ptrd->_Charge.Charge[0]);
  _Probz.assign(nc,0);
  _Indxz.assign(nc,0);
  _Lkhdz.assign(nc,HUGE_VALF);
  _ProbSum=0;
  for (int i=0; i<nc; i++) {
    int ind = _ptrd->_Charge.Charge[i];
    if (ind<nc) {
      _Indxz[ind]=i;
      _Lkhdz[ind]=_ptrd->_Charge.ChargeP[i];
      _Probz[ind]=1./exp(min(_Lkhdz[ind]/_ptrd->_Charge.Nused,powmx))/probmx;
      _ProbSum+=_Probz[ind];
      //cout << "AMSChargeTRD::ComputeCharge " <<  _Lkhdz.back() << " " << _Probz.back() << " " << _ProbSum << endl;
    }
    else {
      cout << "AMSChargeTRD::ComputeCharge-W-TRDChargeIndexOutOfRange : " << ind << endl;
      return 0;
    }
  }

  if (refit) setstatus(AMSDBc::REFITTED);

  return 1;
}


int AMSChargeRich::Fill(int refit) {

  double exp = _pring->getnpexp();
  double use = _pring->getcollnpe();

  if(exp<=0) return 0;

  for (int i=0; i<MaxZTypes; i++){
    double z=ind2charge(i);
    double zz=z*z;
    double f=(use-zz*exp)*(use-zz*exp)/zz/exp;
    _Lkhdz.push_back(exp*zz-use*log(exp*zz));
    _Probz.push_back(PROB((float)f,1));
    //cout << "AMSChargeRich::Fill " <<  _Lkhdz.back() << " " << _Probz.back() << " " << _ProbSum << endl;
  }

  _ProbSum=0;
  for (int i=0; i<_Probz.size(); _ProbSum+=_Probz[i++]); 
  _i=sortlkhd();
  _Charge=ind2charge(_i);

  _Q = sqrt(use/exp);

  if (refit) setstatus(AMSDBc::REFITTED);

  return 1;
}


#ifdef _PGTRACK_
int AMSChargeTracker::Fill(int refit) {

  // First Implementation : Use only Inner Tracker for Charge Estimation
  if (_ID.CompareTo(AMSChargeTracker::ClassID()) &&
      _ID.CompareTo(AMSChargeTrackerInner::ClassID()))
    return 0;

  // loop on charge indexes
  like_t like;
  for (int ind=0; ind<MaxZTypes; ind++) {

    float beta = ind>0?_pbeta->getbeta():1;
    int Z = ind2charge(ind);

    if (_debug) 
      cout << "AMSChargeTracker::Fill-I- " <<
	"_ptrtk " << _ptrtk << " Z " << Z <<  " beta " << beta << endl;

    //! Truncated mean probability (inner tracker)
    like = TrCharge::GetTruncMeanProbToBeZ(_ptrtk, Z, beta);

    // Fill 
    Fill(ind, like);
  }

  // Compute Sum Probabilities for all charges & check
  _ProbSum=0;
  for (int i=0; i<_Probz.size(); _ProbSum+=_Probz[i++]); 
  if (_ProbSum<=0) return 0; // GetTruncMeanProbToBeZ failed? 

  //! Best available Q evaluation (charge units) with beta correction 
  float beta = _pbeta->getbeta();
  _Q = TrCharge::GetQ(_ptrtk, TrCharge::kXY, beta);

  //! Truncated mean charge (inner tracker), no beta correction
  _TruncatedMean = TrCharge::GetMean(TrCharge::kTruncMean|TrCharge::kInner, _ptrtk, TrCharge::kX).Mean;

  //! ProbAlltracker not yet available
  _ProbAllTracker = 0;

  if (_debug)
    cout << "AMSChargeTracker::Fill-I- " <<
      "_Q " << _Q << " _TruncatedMean " << _TruncatedMean <<  " _ProbAllTracker " << _ProbAllTracker << endl;

  // Sort Likelihoods  
  _i=sortlkhd();

  // Assign Most likely charge
  _Charge=ind2charge(_i);

  if (_debug)
    _printEl(cout);

  if (refit) setstatus(AMSDBc::REFITTED);

  return 1;
}

int AMSChargeTracker::Fill(int ind, like_t like) {

  if (!_Probz.size()) {
    _Probz.assign(MaxZTypes,0);
    _Indxz.assign(MaxZTypes,0);
    _Lkhdz.assign(MaxZTypes,HUGE_VALF);
    _ProbSum=0;
  }

  if (ind>MaxZTypes) {
    cout << "AMSChargeTracker::Fill-W-ChargeIndexAboveLimits: ind " << endl;
    return 0;
  }

  if (like.NPoints>0) {
 
    //! Likelihood
    if (like.Prob>0)
      _Lkhdz[ind] = -log(like.Prob);  // use convention -logPi;

    //! Probability
    _Probz[ind] = like.Prob;    

    if (_debug)
      cout << " AMSChargeTracker::Fill-I-ChargeIndex " << ind 
	   << " Lkhd " << _Lkhdz[ind] << " Prob " << _Probz[ind] << endl;
   }
  else {

    // I comment this out as it  seems to be more frequent than expected
    //cout << " AMSChargeTracker::Fill-W-ZeroNPoints " << endl; 
    return 0;
  }

  return 1;
}

#else

int AMSChargeTracker::Fill(int refit) {

  // init
  AMSTrTrack *ptrack = _ptrtk;
  AMSBeta    *pbeta  = _pbeta;
  double etrk[trconst::maxlay];
  double EdepTracker[trconst::TrkTypes-1][trconst::maxlay];
  AMSTrCluster *pTrackerc[trconst::TrkTypes-1][trconst::maxlay];
  const double fac=AMSTrRawCluster::ADC2KeV();

  int nhitTracker=0, nallTracker=0;

  double beta=pbeta->getbeta();
  double theta, phi, sleng;
  AMSPoint P1;
  double pathcor;

  clearstatus(AMSDBc::WEAK);
  while(1 ){

    nhitTracker=0;
    nallTracker=0;

    if (_debug)
      cout << "AMSChargeTracker::Fill-I- ID " << _ID << endl;

    for(int i=0; i<ptrack->getnhits(); i++){
      AMSTrRecHit *phit=ptrack->getphit(i);
      if(phit){
	int layer = phit->getLayer();
	if (!_ID.CompareTo(AMSChargeTrackerInner::ClassID()) && (layer==1 || layer==9)) continue;
	if (!_ID.CompareTo(AMSChargeTrackerUpper::ClassID()) &&  layer!=1 ) continue;
	if (!_ID.CompareTo(AMSChargeTrackerLower::ClassID()) &&  layer!=9 ) continue;
	//cout << " Hit : " << i << " Layer : " << phit->getLayer() << " z " <<  phit->getHit().z() << endl;  
	if (phit->getpsen()){
	  for(int j=0; j<trconst::TrkTypes-1; j++) EdepTracker[j][nhitTracker]=0;
	  AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
			(phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2));
	  AMSPoint SenPnt=phit->getHit();
	  ptrack->interpolate(SenPnt, SenDir, P1, theta, phi, sleng);
	  AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
	  pathcor=fabs(SenDir.prod(DTr));
	  int etaok[2];
	  for(int j=0; j<2; j++){
	    etaok[j]=0;
	    AMSTrCluster *pcls=phit->getClusterP(j);
	    if(pcls){
	      double eta=max(pcls->geteta(),0.);
	      double etamin=CHARGEFITFFKEY.EtaMin[j];
	      double etamax=CHARGEFITFFKEY.EtaMax[j];
	      if( ((eta>etamin||etamin<=0)&&(eta<etamax||etamax>=1)) || checkstatus(AMSDBc::WEAK))
		etaok[j]=1;
	      pTrackerc[j][nhitTracker]=pcls;
	    }
	  }
	  double sums=phit->getsonly()/2;
	  double sumk=phit->getkonly()/2;
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

    if(nhitTracker<2 && 
       _ID.CompareTo(AMSChargeTrackerUpper::ClassID()) && _ID.CompareTo(AMSChargeTrackerUpper::ClassID())){
      if(!checkstatus(AMSDBc::WEAK) && nallTracker) setstatus(AMSDBc::WEAK);
      else break;
    }
    else break;

  }

  if (_debug)
    cout << "AMSChargeTracker::Fill-I- nallTracker " << nallTracker << " nhitTracker " << nhitTracker <<  endl;

//====> Compute path corrected truncated mean (use K side)
  int imx;
  double mean, trunres, trunmax;
  double rescut=CHARGEFITFFKEY.ResCut[1];
  //double resmx=resmax(etrk,nallTracker,0,rescut,imx,mean,trunres,trunmax);
  double resmx=resmax(EdepTracker[0],nhitTracker,0,rescut,imx,mean,trunres,trunmax);
  if(!CHARGEFITFFKEY.TrMeanRes) _TruncatedMean=trunmax;
  else _TruncatedMean=trunres;

  int bstatus = !pbeta->checkstatus(AMSDBc::AMBIG);

  if (!Fit(0,beta,bstatus,nhitTracker,pTrackerc,EdepTracker)) return 0;

  _Q = sqrt ( _TruncatedMean * EdepBetaCorrection(_i,beta) / CHARGEFITFFKEY.TrkkeVperMip );

  if (refit) setstatus(AMSDBc::REFITTED); // not considered yet 

  return 1;
}

int AMSChargeTracker::Fit(int trkfit, double beta, int bstatus, int nhitTracker,
			  AMSTrCluster *pTrackerc[trconst::TrkTypes-1][trconst::maxlay],
			  double etrk[trconst::TrkTypes-1][trconst::maxlay]) {

  static double ETRK[trconst::TrkTypes-1][trconst::maxlay];
#pragma omp threadprivate(ETRK)
  int typetrk[trconst::maxlay], nhittrktyp[trconst::TrkTypes];
  double Trackerresmax[trconst::TrkTypes-1], etrkh[trconst::maxlay], x[trconst::maxlay];
  int Trackerhitmax[trconst::TrkTypes-1];
  int i,j;

  if (_debug)
    cout << "AMSChargeTracker::Fit-I-trkfit : " << trkfit << endl;

// init
  if(!trkfit){
    Init();
    _ProbAllTracker=0;
    UCOPY(etrk[0],ETRK[0],(trconst::TrkTypes-1)*trconst::maxlay*sizeof(etrk[0][0])/4);
  }

  if (nhitTracker<=0) return 0;

// determine furthest hits
  if(trkfit>=0){
    double rescut=!trkfit?CHARGEFITFFKEY.ResCut[1]:0;
    for(i=0; i<trconst::TrkTypes-1; i++){
      double mean,trunres,trunmax;
      int hitmax;
      for(j=0; j<nhitTracker; j++) x[j]=etrk[i][j];
      Trackerresmax[i]=resmax(x,nhitTracker,trkfit,rescut,hitmax,mean,trunres,trunmax);
      Trackerhitmax[i]=hitmax;
    }
    for(i=0; i<trconst::TrkTypes-1; i++){
      for(j=0; j<nhitTracker; j++) if(j==Trackerhitmax[i]) etrk[i][j]=0;
    }
  }

// Use either S+K, S or K energies
  int failtrk=0;
  int nhittrk=0;
  for(i=0; i<trconst::TrkTypes; i++) nhittrktyp[i]=0;
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
    double lkhtrk[MaxZTypes];
    lkhcalc(1,beta,nhitTracker,etrkh,typetrk,lkhtrk);
    double probtrk=probcalc(trkfit,nhittrk,lkhtrk);
    _Charge=ind2charge(_i);
    _ProbSum=0;
    for (int i=0; i<_Probz.size(); _ProbSum+=_Probz[i++]); 

// refit tracker if required
    if(!trkfit && probtrk<CHARGEFITFFKEY.ProbTrkRefit){
      for(int i=0; i<nhitTracker; i++){
        if(etrk[1][i]>0 && (typetrk[i]==0 || typetrk[i]==2) && bstatus)
         pTrackerc[1][i]->clearstatus(AMSDBc::CHARGEUSED);
        if(etrk[0][i]>0 && (typetrk[i]==1 || typetrk[i]==2) && bstatus)
         pTrackerc[0][i]->clearstatus(AMSDBc::CHARGEUSED);
     }
      trkfit++;
      failtrk=!Fit(trkfit,beta,bstatus,nhitTracker,pTrackerc,etrk);
    }
    else if(trkfit>0) setstatus(AMSDBc::REFITTED);
    else _ProbAllTracker=probtrk;

// get tracker probability using all the hits
    if(trkfit!=-1) failtrk=!Fit(-1,beta,bstatus,nhitTracker,pTrackerc,ETRK);
    //cout << "AMSCharge::FitTracker : " << _Charge << endl;
    //for (i=0; i<MaxZTypes; i++)
    //  cout << "i lkhd prob " << i << " " << _Lkhdz[i] << " " << _Probz[i] << endl;

  }
  else{
    //cerr<<"AMSCharge::Fit -E- no Tracker hit found"<<endl;
  }

  return !failtrk;
}


double AMSChargeTracker::EdepBetaCorrection(int ichar, double beta) {

  if (ichar<0 || ichar>=MaxZTypes) return 0;

  double betamax = 0.94;
  double betapow = 1.78;//my estimation, works better than 5/3, the same for all nucl<=C12)
  double betacor = ichar?pow(min(fabs(beta/betamax),1.),betapow):1;//corr to "mip"(=1 for ichar=0(electrons))

  return betacor;
}

#endif


int AMSChargeTOF::Fill(int refit) {

  AMSBeta *pbeta = _pbeta;
  double etof[TOF2GC::SCLRS];
  double EdepTOF[TOF2GC::SCLRS];
  AMSTOFCluster *pTOFc[TOF2GC::SCLRS];
  int TypeTOF[TOF2GC::SCLRS];

// init
  int nhitTOF=0, nallTOF=0;

  AMSTrTrack *ptrack=pbeta->getptrack();
  double beta=pbeta->getbeta();
  double theta, phi, sleng;
  AMSPoint P1;
  double pathcor;


  clearstatus(AMSDBc::WEAK);
  while(1 ){
    nhitTOF=0;
    nallTOF=0;
    for(int i=0; i<TOF2GC::SCLRS; i++){
      EdepTOF[i]=0;
      AMSTOFCluster *pcluster=pbeta->getpcluster(i);
      if(pcluster){
	double edep=pcluster->getedep();
	if(edep>0) etof[nallTOF++]=pcluster->getedep();//store/counts all raw hits
	if(pcluster->getnmemb()<=CHARGEFITFFKEY.NmembMax || checkstatus(AMSDBc::WEAK)){
	  EdepTOF[nhitTOF]=0;
	  AMSDir ScDir(0,0,1); // good approximation for TOF
	  AMSPoint SCPnt=pcluster->getcoo();
	  ptrack->interpolate(SCPnt, ScDir, P1, theta, phi, sleng);
	  AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
	  pathcor=fabs(ScDir.prod(DTr));
	  pTOFc[nhitTOF]=pcluster;
	  EdepTOF[nhitTOF]=edep*pathcor;//store selected and pass-corrected hits
	  nhitTOF++;//count selected hits
	}
      }
    }//---> endof hits loop
    if(nhitTOF<2){//too little, try to add "nmemb>NmembMax"(weak) hits
      if(!checkstatus(AMSDBc::WEAK) && nallTOF>0 && CHARGEFITFFKEY.NmembMax<2)
	setstatus(AMSDBc::WEAK); // (try to use clust with nmemb>NmembMax(if<2), by TOF-clust definition nmemb<=2 !!!)
      else{
	cerr<<"AMSCharge::build:TOF -E- Low clust.mult. and No weak clusters to add"<<endl;
	break;
      }
    }
    else break;
  }//-->endof while

  if (_debug)
    cout << "AMSChargeTOF::Fill-I- nhitTOF " << nhitTOF <<  endl;

//====> Compute path corrected truncated mean
  int imx;
  double mean, trunres, trunmax;
  double rescut=CHARGEFITFFKEY.ResCut[0];//use/not(>=0/-1) incomp.clus exclusion
  //double resmx=resmax(etof,nallTOF,0,rescut,imx,mean,trunres,trunmax);//TOF(raw(non-corr!) hits trunc.mean)
  double resmx=resmax(EdepTOF,nhitTOF,0,rescut,imx,mean,trunres,trunmax);//TOF hits trunc.mean)
  if(!CHARGEFITFFKEY.TrMeanRes) _TruncatedMean=trunmax;//normal("-highest hit") TruncMean
  else _TruncatedMean=trunres;//"-incomp.cluster" TruncMean

  int bstatus = !pbeta->checkstatus(AMSDBc::AMBIG);

  if (!Fit(0,beta,bstatus,nhitTOF,pTOFc,EdepTOF)) return 0;

  _Q = sqrt ( _TruncatedMean * EdepBetaCorrection(_i,beta) / CHARGEFITFFKEY.TOFMeVperMip );

  if (refit) setstatus(AMSDBc::REFITTED);

  return 1;
}


int AMSChargeTOF::Fit(int refit, double beta, int bstatus, int nhitTOF, AMSTOFCluster *pTOFc[], double etof[TOF2GC::SCLRS]){
  static double ETOF[TOF2GC::SCLRS];
#pragma omp threadprivate(ETOF)
  int typetof[TOF2GC::SCLRS], nhittoftyp;
  double TOFresmax, etofh[TOF2GC::SCLRS], x[TOF2GC::SCLRS];
  int TOFhitmax;
  int i,j;

// init
  if (!refit){
    Init();
    UCOPY(etof,ETOF,TOF2GC::SCLRS*sizeof(etof[0])/4);//save initial edep-array
  }

// find/remove furthest(incomp) hits(if requested)
  if(!refit){
    double rescut=CHARGEFITFFKEY.ResCut[0];//>=0/-1->use/not incomp.clus exclusion
    double mean,trunres,trunmax;
    int hitmax;
    for(j=0; j<nhitTOF; j++) x[j]=etof[j];
    TOFresmax=resmax(x,nhitTOF,refit,rescut,hitmax,mean,trunres,trunmax);
    TOFhitmax=hitmax;//incomp.clus.index/-1(if not requested)
    for(j=0; j<nhitTOF; j++) if(j==TOFhitmax) etof[j]=0;//delete incomp.hit(if requested)
  }

// Mark good(used)/deleted hits:
  int failtof=0;
  int nhittof=0;
  nhittoftyp=0;
  for(i=0; i<nhitTOF; i++){
    typetof[i]=-1;
    etofh[i]=0;
    if(refit<=0 && etof[i]>0){
      typetof[i]=0;//mark good(not_incomp) hits with "0"
      etofh[i]=etof[i];//store hits for lkhc-calc(incomp.hits with etof=0 will be mark typetof=-1) 
    }
    if(typetof[i]>=0){
      nhittoftyp++;//counts all good hits
      if(refit>=0 && bstatus) pTOFc[i]->setstatus(AMSDBc::CHARGEUSED);
      nhittof++;
    }
  }
  if(!nhittof) failtof=1;//no good hits left
  if(!failtof){
// likelihood values and charge probabilities
    double lkhtof[MaxZTypes];
    lkhcalc(0,beta,nhitTOF,etofh,typetof,lkhtof);//"0" means TOF
    double probtof=probcalc(refit,nhittoftyp,lkhtof);
    _Charge=ind2charge(_i);
    _ProbSum=0;
    for (int i=0; i<_Probz.size(); _ProbSum+=_Probz[i++]); 
    if (_debug)
      cout << "AMSCharge::FitTOF : " << _Charge << endl;
    //for (int i=0; i<MaxZTypes; i++)
    //   cout << "i lkhd prob " << i << " " << _Lkhdz[i] << " " << _Probz[i] << endl;
  }
  else{
    cerr<<"AMSCharge::TofFit -E- no TOF cluster found"<<endl;
  }

  return !failtof;
}


double AMSChargeTOF::EdepBetaCorrection(int ichar, double beta) {

  if (ichar<0 || ichar>=MaxZTypes) return 0;

  double betamax = 0.95;//now depends on Z-index
  double betapow = TofElosPDF::TofEPDFs[ichar].getbpow();
  double betacor = ichar?pow(min(fabs(beta/betamax),1.),betapow):1;//corr to "mip"(=1 for ichar=0(electrons))

  return betacor;
}


#include "root.h"

void AMSCharge::_writeEl(){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

void AMSCharge::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet()) return;
  ChargeR &ptr=AMSJob::gethead()->getntuple()->Get_evroot02()->Charge(_vpos);
  ptr.setBeta(_pbeta==NULL?-1:_pbeta->GetClonePointer());

  for(  map <TString,AMSChargeSubD*> :: iterator i=_charges.begin();i!=_charges.end();++i){

    ChargeSubDR &subPtr=ptr.Charges[i->first];

    /*subPtr.setParent(i->second->_getParent());*/

    subPtr.setParent(-1);

    if((i->first).Contains(AMSChargeTOF::ClassID())){
      AMSChargeTOF *obj=(AMSChargeTOF*)i->second;
      subPtr.setParent(obj->_pbeta==NULL?-1:obj->_pbeta->GetClonePointer());
      continue;
    }

    if((i->first).Contains(AMSChargeTracker::ClassID())){
      AMSChargeTracker *obj=(AMSChargeTracker*)i->second;
      subPtr.setParent(obj->_ptrtk==NULL?-1:obj->_ptrtk->GetClonePointer());
      continue;
    }

    if((i->first).Contains(AMSChargeTRD::ClassID())){
      AMSChargeTRD *obj=(AMSChargeTRD*)i->second;
      subPtr.setParent(obj->_ptrd==NULL?-1:obj->_ptrd->GetClonePointer());
      continue;
    }

    if((i->first).Contains(AMSChargeRich::ClassID())){
      AMSChargeRich *obj=(AMSChargeRich*)i->second;
      subPtr.setParent(obj->_pring==NULL?-1:obj->_pring->GetClonePointer());
      continue;
    }
    
  }
#endif
}
