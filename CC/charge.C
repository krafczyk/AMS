//  $Id: charge.C,v 1.95 2013/11/09 14:14:07 oliva Exp $
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
#include <algorithm>
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

static TString detector_list[7] = {
  AMSChargeTrackerUpper::ClassID(),
  AMSChargeTRD::ClassID(),
  AMSChargeTOFUpper::ClassID(),
  AMSChargeTrackerInner::ClassID(),
  AMSChargeTOFLower::ClassID(),
  AMSChargeRich::ClassID(),
  AMSChargeTrackerLower::ClassID()
};


/////////////////////////////////////////////////////////////////
// AMSChargeBase
/////////////////////////////////////////////////////////////////


void AMSChargeBase::copyvalues(const AMSChargeBase& that) {
  _Q = that._Q;
  _Indxz = that._Indxz;
  _Lkhdz = that._Lkhdz;
  _Probz = that._Probz;
  _ChargeI = that._ChargeI;
  _Lkhd = that._Lkhd;
  _Prob = that._Prob;  
}


void AMSChargeBase::clear() {
  _Q = 0;
  _Indxz.clear();
  _Lkhdz.clear();
  _Probz.clear();
  _ChargeI.clear();
  _Lkhd.clear();
  _Prob.clear();
}


void AMSChargeBase::addelectron() {
  if (isvalid(1)) {
    _Indxz.push_back(0);
    _Lkhdz.push_back(getlkhdindex(1));
    _Probz.push_back(getprobindex(1));
  }
}


struct order_indexes_with_vector {
  vector<double>& values;
  order_indexes_with_vector(vector<double>& val) : values(val) {}
  bool operator() (const int& a, const int& b) const {
    return values[a]<values[b];
  }
};


bool AMSChargeBase::sortandfill(bool uselike, double maxLogLike, double minProb, int maxRootSize) {
  // checks
  if (isempty()) return false;
  if (!isconsistent()) {
    printf("AMSChargeBase::sortandfill-W error on size of containers (nIndxz:%2d,nLkhdz:%2d,nProbz:%2d), clean up.\n",(int)_Indxz.size(),(int)_Lkhdz.size(),(int)_Probz.size());
    clear();
    return false; 
  }
  // reordering
  vector<int> indexes;
  for (int i=0; i<getn(); i++) indexes.push_back(i);
  if (uselike) sort(indexes.begin(),indexes.end(),order_indexes_with_vector(_Lkhdz));
  else {
    sort(indexes.begin(),indexes.end(),order_indexes_with_vector(_Probz));
    reverse(indexes.begin(),indexes.end());
  }
  // probability normalization
  _ProbSum = 0;
  for (int i=0; i<(int)_Probz.size(); i++) _ProbSum += _Probz[i];
  if (_ProbSum<=0) {
    printf("AMSChargeBase::sortandfill(2)-W proability sum is <=0 (%f), clean up.\n",_ProbSum);
    clear();
    return false;
  } 
  if (CHARGEFITFFKEY.NormalizeProbs) {
    for (int i=0; i<(int)_Probz.size(); i++) {
      _Probz[i] /= _ProbSum;
    }
  }
  // fill and swap
  for (int i=0; i<getn(); i++) {
    _ChargeI.push_back(_Indxz[indexes[i]]);
    _Lkhd.push_back(_Lkhdz[indexes[i]]);
    _Prob.push_back(_Probz[indexes[i]]);
  }
  copy(_ChargeI.begin(),_ChargeI.end(),_Indxz.begin());
  copy(_Lkhd.begin(),_Lkhd.end(),_Lkhdz.begin());
  copy(_Prob.begin(),_Prob.end(),_Probz.begin());
  // clean from too small values
  for (int i=getn()-1; i>=0; i--) {
    if ( (_Lkhdz[i]>maxLogLike)||(_Probz[i]<minProb) ) {
      _Indxz.erase(_Indxz.begin()+i);
      _Lkhdz.erase(_Lkhdz.begin()+i);
      _Probz.erase(_Probz.begin()+i);
      _ChargeI.erase(_ChargeI.begin()+i);
      _Lkhd.erase(_Lkhd.begin()+i);
      _Prob.erase(_Prob.begin()+i);
    }
  }
  if (isempty()) return false;
  // probability normalization (again)
  _ProbSum = 0;
  for (int i=0; i<(int)_Probz.size(); i++) _ProbSum += _Probz[i];
  if (_ProbSum<=0) {
    printf("AMSChargeBase::sortandfill(2)-W proability sum is <=0 (%f), clean up.\n",_ProbSum);
    clear();
    return false;
  }
  if (CHARGEFITFFKEY.NormalizeProbs) {
    for (int i=0; i<(int)_Probz.size(); i++) {
      _Probz[i] /= _ProbSum;
      _Prob[i] /= _ProbSum;
    }
  }
  // restrit to maxRootSize elements to be stored on ROOT file 
  for (int i=getn()-1; i>=0; i--) {
    if ( (maxRootSize>=0)&&(i>=maxRootSize) ) {
      _ChargeI.erase(_ChargeI.begin()+i);
      _Lkhd.erase(_Lkhd.begin()+i);
      _Prob.erase(_Prob.begin()+i);
    }
  }
  // charge 
  return true;
}


bool AMSChargeBase::isvalid(int index, double maxLogLike, double minProb) {
  for (int i=0; i<getn(); i++) 
    if (index==_Indxz[i]) 
      if ( (_Probz[i]>=minProb)&&(_Lkhdz[i]<=maxLogLike) ) return true; 
  return false;
} 


double AMSChargeBase::getprobindex(int index) {
  float prob = 0;
  for (int i=0; i<getn(); i++) 
    if (index==_Indxz[i]) 
      return (i<(int)_Probz.size()) ? _Probz[i] : prob;
  return prob;
}


double AMSChargeBase::getlkhdindex(int index) {
  float like = 1000; 
  for (int i=0; i<getn(); i++) 
    if (index==_Indxz[i]) 
      return (i<(int)_Lkhdz.size()) ? _Lkhdz[i] : like; 
  return like;
}


void AMSChargeBase::getminmaxindex(int& minindex, int& maxindex) {
  minindex = -1;
  maxindex = -1;
  if (isempty()) return;
  minindex = *min_element(_Indxz.begin(),_Indxz.end());
  maxindex = *max_element(_Indxz.begin(),_Indxz.end());
}


bool AMSChargeBase::isok() { 
  return (!isempty())&&(isconsistent());
}

/////////////////////////////////////////////////////////////////
// AMSChargeSubD
/////////////////////////////////////////////////////////////////


void AMSChargeSubD::Init(int refit){
  _ProbSum = 0;
  if (!refit) {
    clear(); 
    clearstatus(0xFFFFFFFF);
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

  int index = -1;

  if(fit>=0){
    Init();
    for(int i=0; i<MaxZTypes; i++) _Lkhdz.push_back(lkhd[i]);
    index = sortlkhd();
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
  // _i, CHARGEFITFFKEY.NormalizeProbs) << " " << prob[_i] << endl;

  return prob[index]; // /(CHARGEFITFFKEY.NormalizeProbs?_ProbSum:1);
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


void AMSChargeSubD::print() {
  if (isempty()) printf("AMSChargeSubD::print-V %25s empty.\n",getID().Data());
  for (int i=0; i<getn(); i++) {
    printf("AMSChargeSubD::print-V %25s i:%02d iz:%2d -LL:%10.5f P:%10.5g\n",
      getID().Data(),i,_Indxz[i],_Lkhdz[i],_Probz[i]);
  }
}


/////////////////////////////////////////////////////////////////
// AMSCharge
/////////////////////////////////////////////////////////////////


void AMSCharge::print() {
  printf("AMSCharge::print-V - Start ------------------------------------------------------ \n");
  printf("AMSCharge::print-V - Beta? %1d    BetaH? %1d\n",(_pbeta!=0),(_pbetah!=0));
  if (isempty()) printf("AMSCharge::print-V     %25s empty.\n","AMSCharge");
  for (int i=0; i<getn(); i++) {
    printf("AMSCharge::print-V     %25s i:%02d iz:%2d -LL:%10.5f P:%10.5g\n",
      "AMSCharge",i,_Indxz[i],_Lkhdz[i],_Probz[i]);
  }
  map<TString,AMSChargeSubD*>::iterator i;
  for (i=_charges.begin(); i!=_charges.end(); ++i) {
    AMSChargeSubD *chargeSubD = i->second;
    chargeSubD->print();
  }
  printf("AMSCharge::print-V - End -------------------------------------------------------- \n");
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


int AMSCharge::BuildTOF() {

  // checks
  if (_pbeta == NULL) return 0;
  // whole TOF
  if (((CHARGEFITFFKEY.RecEnable[kTOF]/tofAll)%10) != 0) {
    AMSChargeTOF *chargetof = new AMSChargeTOF(_pbeta,_pbetah);
    if ((chargetof->ComputeCharge())&&chargetof->isok()) _charges[chargetof->_ID] = chargetof;  
    else delete chargetof; 
  }
  // upper TOF
  if (((CHARGEFITFFKEY.RecEnable[kTOF]/tofUpper)%10) != 0) {
    AMSChargeTOFUpper *chargetofupper = new AMSChargeTOFUpper(_pbeta,_pbetah);
    if (chargetofupper->ComputeCharge()&&chargetofupper->isok()) _charges[chargetofupper->_ID] = chargetofupper;
    else delete chargetofupper;
  }
  // lower TOF
  if (((CHARGEFITFFKEY.RecEnable[kTOF]/tofLower)%10) != 0) {
    AMSChargeTOFLower *chargetoflower = new AMSChargeTOFLower(_pbeta,_pbetah);
    if (chargetoflower->ComputeCharge()&&chargetoflower->isok()) _charges[chargetoflower->_ID] = chargetoflower;
    else delete chargetoflower;
  }
  return 1;
}


int AMSCharge::BuildTracker() {
  // checks 
  if (_pbeta == NULL) return 0;
  AMSTrTrack *ptrtk = _pbeta->getptrack();
  if (ptrtk == NULL) return 0;
  if (ptrtk->getnhits() == 0) return 0; 
  // whole tracker
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkAll)%10) != 0) {
    AMSChargeTracker *chargetrk = new AMSChargeTracker(_pbeta,ptrtk,_pbetah);
    if (chargetrk->ComputeCharge()&&chargetrk->isok()) _charges[chargetrk->_ID] = chargetrk;
    else delete chargetrk;
  }
  // inner tracker
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkInner)%10) != 0) {
    AMSChargeTrackerInner *chargetrkinner = new AMSChargeTrackerInner(_pbeta,ptrtk,_pbetah);
    if (chargetrkinner->ComputeCharge()&&chargetrkinner->isok()) _charges[chargetrkinner->_ID] = chargetrkinner;
    else delete chargetrkinner;
  }
  // upper layer
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkUpper)%10) != 0) {
    AMSChargeTrackerUpper *chargetrkupper = new AMSChargeTrackerUpper(_pbeta,ptrtk,_pbetah);
    if (chargetrkupper->ComputeCharge()&&chargetrkupper->isok()) _charges[chargetrkupper->_ID] = chargetrkupper;
    else delete chargetrkupper;
  }
  // lower layer
  if (((CHARGEFITFFKEY.RecEnable[kTracker]/trkLower)%10) != 0) {
    AMSChargeTrackerLower *chargetrklower = new AMSChargeTrackerLower(_pbeta,ptrtk,_pbetah);
    if(chargetrklower->ComputeCharge()&&chargetrklower->isok()) _charges[chargetrklower->_ID] = chargetrklower;
    else delete chargetrklower;
  }
  return 1; 
}


int AMSCharge::BuildTRD() {       
  // checks
  if (_pbeta == NULL) return 0;
  AMSTrTrack *ptrtk = _pbeta->getptrack();
  if (ptrtk == NULL) return 0; 
  // search for best trd-track associated with trk-track
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
  // whole TRD 
  AMSChargeTRD *chargetrd = new AMSChargeTRD(_pbeta,ptrd,_pbetah);
  if (chargetrd->ComputeCharge()&&chargetrd->isok()) _charges[chargetrd->_ID] = chargetrd;
  else delete chargetrd;
  //---Use TRD Match for BetaH
  if(ptrd!=NULL){
    if(_pbetah==0){ //no tracker match->try to use trdtrack match
       AMSBetaH * betah=(AMSBetaH*)AMSEvent::gethead()->getheadC("AMSBetaH",0,0);
       while(betah){
         if(betah->gettrdtrack()==ptrd){_pbetah=betah;_pbetah->setcharge(this);break;}
         betah=betah->next();
       }
    }
    else _pbetah->settrdtrack(ptrd); //reset this trd to betah
  } 
  return 1; 
}


int AMSCharge::BuildRich() {
  // check
  AMSTrTrack *ptrtk = _pbeta->getptrack();
  if (ptrtk == NULL) return 0;
  // search ring associated with track
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
  // RICH
  AMSChargeRich *chargerich = new AMSChargeRich(_pbeta,pring,_pbetah);
  if (chargerich->ComputeCharge()&&chargerich->isok()) _charges[chargerich->_ID]=chargerich;
  else delete chargerich;

  return 1; 
}


int AMSCharge::BuildCombine() {
  clear();
  int indmx = -1; 
  int charge = getvotedcharge(indmx);
  if (indmx<0) {
    cout << "AMSCharge::BuildCombine-W-NoValidAMSChargeFound " << endl;
    print();
    return 0;
  }
  if (_ChargeI[0]==65535) {
    cout << "AMSCharge::BuildCombine-W-ChargeI overflow " << endl;
    // print();
    return 0;     
  }
  return 1;
}


AMSChargeBase AMSCharge::getvotedcharge(vector<TString> detectors) {

  AMSChargeBase result;
  result.clear();

  // calculate index range 
  int minindex = -1;
  int maxindex = -1;
  map<TString,AMSChargeSubD*>::iterator i;
  for (i=_charges.begin(); i!=_charges.end(); ++i) {
    AMSChargeSubD *chargeSubD = i->second;
    if (find(detectors.begin(),detectors.end(),chargeSubD->getID())==detectors.end()) continue;
    int subdminindex = -1;
    int subdmaxindex = -1;
    chargeSubD->getminmaxindex(subdminindex,subdmaxindex);
    if (minindex<0) minindex = subdminindex;
    if (maxindex<0) maxindex = subdmaxindex;
    minindex = min(minindex,subdminindex);
    maxindex = max(maxindex,subdmaxindex);
  }
  int size = maxindex - minindex + 1;
  if ( (minindex<0)&&(maxindex<0) ) return result;
  
  // calculate global likelihood 
  result._Indxz.assign(size,-1);
  result._Lkhdz.assign(size,0);
  result._Probz.assign(size,1);
  for (int index=minindex; index<=maxindex; index++) {
    int Nused = 0;
    for (i=_charges.begin();i!=_charges.end();++i) {
      AMSChargeSubD *chargeSubD = i->second;
      if (find(detectors.begin(),detectors.end(),chargeSubD->getID())==detectors.end()) continue;
      if ( (!chargeSubD->isvalid(index))||(chargeSubD->getprobindex(chargeSubD->getindex())==0) ) continue;
      Nused++;
      result._Indxz[index-minindex] = index;
      result._Lkhdz[index-minindex] += chargeSubD->getlkhdindex(index);
      result._Probz[index-minindex] *= chargeSubD->getprobindex(index)/chargeSubD->getprobindex(chargeSubD->getindex());
    }
    if (Nused>0) result._Lkhdz[index-minindex] /= Nused;
    else {
      // if no detector reset to bad values
      result._Lkhdz[index-minindex] = HUGE_VAL;
      result._Probz[index-minindex] = 0;
    }
  }

  // sort and fill ROOT vectors 
  result.sortandfill(); // CHARGEFITFFKEY.UseLikelihood);

  return result;
}


int AMSCharge::getvotedcharge(int& index) {

  // return value if already computed
  if (getcharge()>0) {
    index = getindex();
    return getcharge();
  }

  // calculate if new
  vector<TString> list; 
  for (int i=0; i<7; i++) list.push_back(detector_list[i]);
  copyvalues(getvotedcharge(list));

  index = getindex();
  return getcharge();  
}


int AMSCharge::BuildUpper() {

  // upper AMS detector list 
  vector<TString> list;
  for (int i=0; i<3; i++) list.push_back(detector_list[i]);

  // combine charges
  AMSChargeUpper *chargeupper = new AMSChargeUpper();
  int nused = 0;
  chargeupper->copyvalues(getvotedcharge(list));
  if (chargeupper->isok()) _charges[chargeupper->_ID] = chargeupper;
  else delete chargeupper;

  return 1;
}


int AMSCharge::BuildFragmentation() {
  return 1;
}


double AMSCharge::getprobcharge(int charge) {
  // init
  charge = abs(charge);
  int indmx;
  int voted = getvotedcharge(indmx);
  int index = charge;
  double prob = 1;
  // loop
  for (map<TString,AMSChargeSubD*>::iterator i=_charges.begin();i!=_charges.end();++i) {
    AMSChargeSubD *chargeSubD = i->second;
    if (!chargeSubD->checkstatus(AMSDBc::USED)) continue;
    prob *= chargeSubD->getprobindex(index)/chargeSubD->getprobindex(voted);
  }
  return prob;
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
  // init 
  clear();
  _pbeta = pbeta;
  // associate betah (needed) 
  _pbetah = 0;
  AMSTrTrack* ptrack=pbeta->getptrack();
  if (ptrack!=NULL) {
    AMSBetaH* betah=(AMSBetaH*)AMSEvent::gethead()->getheadC("AMSBetaH",0,0);
    while(betah){
      if(betah->gettrack()==ptrack){_pbetah=betah;_pbetah->setcharge(this);break;}
      betah=betah->next();
    }
  }
  // build (the order is important in order to find proper betah)
  if (CHARGEFITFFKEY.RecEnable[kTRD]>0)     BuildTRD();
  if (CHARGEFITFFKEY.RecEnable[kTracker]>0) BuildTracker();
  if (CHARGEFITFFKEY.RecEnable[kTOF]>0)     BuildTOF();
  if (CHARGEFITFFKEY.RecEnable[kRich]>0)    BuildRich();
  BuildCombine();
  BuildUpper();
  BuildFragmentation();

  // print();
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
  ptr.setBetaH(_pbetah==0?-1:_pbetah->GetClonePointer());
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


/////////////////////////////////////////////////////////////////
// AMSChargeSubD derived types
/////////////////////////////////////////////////////////////////


int AMSChargeTOF::ComputeCharge(int refit){
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }
  Init(refit);
  if (!Fill(refit)) return 0;
  return 1;
}



int AMSChargeTracker::ComputeCharge(int refit){
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }
  if (_ptrtk == NULL) {
    cout << "AMSChargeTracker::ComputeCharge-W-NullAMSTrTrackPointer" << endl;
    return 0;
  }
  Init(refit);
  if (!Fill(refit)) return 0;
  return 1;
}


int AMSChargeRich::ComputeCharge(int refit) {
  if (_pbeta == NULL) {
    cout << "AMSChargeTOF::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }
  if (_pring == NULL) {
    cout << "AMSChargeRich::ComputeCharge-W-NullRichRingPointer" << endl;
    return 0;
  }
  Init(refit);
  if (!Fill(refit)) return 0;
  return 1;
}


int AMSChargeTRD::ComputeCharge(int refit) {
  if (_pbeta == NULL) {
    cout << "AMSChargeTRD::ComputeCharge-W-NullAMSBetaPointer" << endl;
    return 0;
  }
  if (_ptrd == NULL) {
    cout << "AMSChargeTRD::ComputeCharge-W-NullTRDTrackPointer" << endl;
    return 0;
  }
  Init(refit);
  if (!Fill(refit)) return 0;
  return 1;
}


int AMSChargeTRD::Fill(int refit) {
  // init
  _ptrd->ComputeCharge(_pbeta->GetTRDBetaCorr()); // <<< THIS IS NOT CLEAR FOR ME
  if (_ptrd->_Charge.Nused <= 0) return 0;
  // charge
  _Q = _ptrd->_Charge.Q;
  // likelihood 
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
      _Lkhdz[ind]=_ptrd->_Charge.ChargeP[i]/_ptrd->_Charge.Nused;
      _Probz[ind]=1./exp(min(_Lkhdz[ind],powmx))/probmx;
      //cout << "AMSChargeTRD::ComputeCharge " <<  _Lkhdz.back() << " " << _Probz.back() << " " << _ProbSum << endl;
    }
    else {
      cout << "AMSChargeTRD::ComputeCharge-W-TRDChargeIndexOutOfRange : " << ind << endl;
      return 0;
    }
  }
  sortandfill();
  if (refit) setstatus(AMSDBc::REFITTED);
  return 1;
}


int AMSChargeRich::Fill(int refit) {
  // photon counting
  double exp = _pring->getnpexp();
  double use = _pring->getcollnpe();
  if (AMSJob::gethead()->isRealData()) exp*=0.836; // average correction factor 
  if(exp<=0) return 0;
  // floating charge
  _Q = sqrt(use/exp);
  // calculation
  float minLogLike = HUGE_VAL;
  int maxindex = int(_Q+15);
  for (int i=0; i<maxindex; i++){
    double z=i+1;
    double zz=z*z;
    _Indxz.push_back(z);
    double LogLike = exp*zz-use*log(exp*zz);
    _Lkhdz.push_back(LogLike);
    if (LogLike<minLogLike) minLogLike = LogLike;
  }
  for (int i=0; i<maxindex; i++)  
    _Probz.push_back(PROB(2.*max(_Lkhdz[i]-minLogLike,0.),1)); // (float)(_Lkhdz[i]-minLogLike),1));
  addelectron();
  sortandfill();
  if (refit) setstatus(AMSDBc::REFITTED);
  return 1;
}


#ifdef _PGTRACK_
int AMSChargeTracker::Fill(int refit) {

  // check
  if (_ID.CompareTo(AMSChargeTracker::ClassID())      &&
      _ID.CompareTo(AMSChargeTrackerInner::ClassID()) && 
      _ID.CompareTo(AMSChargeTrackerLower::ClassID()) && 
      _ID.CompareTo(AMSChargeTrackerUpper::ClassID()) 
     )
    return 0;

  // beta
  float beta = _pbeta->getbeta();
  if (_pbetah) beta = _pbetah->GetBeta(); 

  // for results
  vector<like_t> likelihood;

  // AMSChargeTrackerInner
  if (_ID.CompareTo(AMSChargeTrackerInner::ClassID())==0) {
    _Q = _ptrtk->GetInnerQ(beta);
    _TruncatedMean = _ptrtk->GetInnerQ(1);
    _ptrtk->GetInnerZ(likelihood,beta);
  }

  // AMSChargeTrackerUpper
  else if (_ID.CompareTo(AMSChargeTrackerUpper::ClassID())==0) {
    _Q = _ptrtk->GetLayerJQ(1,beta);
    _TruncatedMean = _ptrtk->GetLayerJQ(1,1);
    _ptrtk->GetLayerJZ(likelihood,1,beta); 
  }

  // AMSChargeTrackerLower
  else if (_ID.CompareTo(AMSChargeTrackerLower::ClassID())==0) {
    _Q = _ptrtk->GetLayerJQ(9,beta);
    _TruncatedMean = _ptrtk->GetLayerJQ(9,1);
    _ptrtk->GetLayerJZ(likelihood,9,beta);
  }

  // AMSChargeTracker
  else if (_ID.CompareTo(AMSChargeTracker::ClassID())==0) {
    // _ProbAllTracker = 0;
    _Q = _ptrtk->GetQ(beta);
    _TruncatedMean = _ptrtk->GetQ(1);
    _ptrtk->GetZ(likelihood,beta);
  }

  // fill base vectors 
  for (int i=0; i<(int)likelihood.size(); i++) {
    _Indxz.push_back(likelihood[i].Z);
    _Lkhdz.push_back(-likelihood[i].GetNormLogLike()); // convention
    _Probz.push_back(likelihood[i].GetNormProb());
  }

  // sort and fill root vectors
  addelectron();
  sortandfill();

  if (refit) setstatus(AMSDBc::REFITTED);
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
  _Q = sqrt ( _TruncatedMean * EdepBetaCorrection(getindex(),beta) / CHARGEFITFFKEY.TrkkeVperMip );
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

  if ( (_ID.CompareTo(AMSChargeTOF::ClassID())) &&   
       (_ID.CompareTo(AMSChargeTOFUpper::ClassID())) &&
       (_ID.CompareTo(AMSChargeTOFLower::ClassID())) ) return 0;

#ifdef _PGTRACK_
  if (_pbetah) {
    FakeBetaHR fakebetah(_pbetah); 
    TofChargeHR charbetah(&fakebetah);
    int pattern = -2; // -2 drop max-q bad pathlenght, -10 bad pathlenght
    if      (_ID.CompareTo(AMSChargeTOFUpper::ClassID())==0) pattern = 1100;
    else if (_ID.CompareTo(AMSChargeTOFLower::ClassID())==0) pattern = 11;
    for (int i=0; i<charbetah.GetNZ(pattern); i++) {
      TofLikelihoodPar* likelihood = charbetah.GetTofLikelihoodPar(i,pattern);
      int nhit = likelihood->GetnLayer();
      if (nhit<1) continue;
      int charge = likelihood->Z;
      double loglike = -likelihood->Likelihood/nhit; // convention 
      double prob = likelihood->Prob;
      if ((prob<0)||isnan(prob)||isnan(loglike)) continue;
      _Indxz.push_back(charge);
      _Lkhdz.push_back(loglike);   
      _Probz.push_back(prob);
    }
    int npoints = 0;
    float rms = 0;
    _Q = charbetah.GetQ(npoints,rms,pattern);
    _TruncatedMean = fakebetah.GetQ(npoints,rms,2,TofClusterHR::DefaultQOpt,pattern,1); // no beta correction
    // sort and fill root vectors
    addelectron();
    sortandfill();
    if (refit) setstatus(AMSDBc::REFITTED);
    return 1; 
  }
#endif 

  // if no available betah charge go to old algo. based on beta

  // no upper/lower for old algo. 
  if ( (_ID.CompareTo(AMSChargeTOFUpper::ClassID()==0))||
       (_ID.CompareTo(AMSChargeTOFLower::ClassID()==0)) ) return 0;

  // old algo.
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
  sortandfill();
  _Q = sqrt ( _TruncatedMean * EdepBetaCorrection(getindex(),beta) / CHARGEFITFFKEY.TOFMeVperMip );
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
    _ProbSum=0;
    for (int i=0; i<_Probz.size(); _ProbSum+=_Probz[i++]); 
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


#ifdef _PGTRACK_
/////////////////////////////////////////////////////////////////
// FakeBetaHR support class
////////////////////////////////////////////////////////////////

FakeBetaHR::FakeBetaHR() {
  _pclusterh.clear();
  _ptrtrack = 0;
}

FakeBetaHR::FakeBetaHR(AMSBetaH* ptr) : BetaHR(ptr) {
  fTrTrack = -1;
  for (int i=0; i<4; i++) 
    if (ptr->_phith[i])
      _pclusterh.push_back((TofClusterHR*) ptr->_phith[i]);
  if (ptr->_ptrack) {
    _ptrtrack = (TrTrackR*) ptr->_ptrack;
    fTrTrack = 0; // to let the people believe
  }
}

FakeBetaHR::~FakeBetaHR() { 
  _pclusterh.clear();
  _ptrtrack = 0;
}

TofClusterHR* FakeBetaHR::GetClusterHL(int ilay) {
  for (int i=0; i<NTofClusterH(); i++) 
    if (pTofClusterH(i)&&(pTofClusterH(i)->Layer==ilay)) 
      return pTofClusterH(i);
  return 0;
}
#endif 
