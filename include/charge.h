//  $Id: charge.h,v 1.27 2011/05/10 19:10:06 jorgec Exp $
// V. Choutko 5-june-96
//
// July 12, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. implementation of friend class
//
// Last Edit Oct  10, 1996.  ak. 
//
#ifndef __AMSCHARGE__
#define __AMSCHARGE__
#include "link.h"
#include "tofdbc02.h"
#ifndef _PGTRACK_
#include "tkdbc.h"
#else
#include "TrCharge.h"
#endif
#include "trrec.h"
#include "beta.h"
#include "richrec.h"
#include "trdrec.h"
#include "TString.h"

using namespace AMSChargConst;

const number powmx=50.;
enum SubDiD { kTOF=0, kTracker=1, kTRD=2, kRich=3 };

/* Base class for subdetector charge reconstruction */

class AMSChargeSubD : public AMSlink {
public:

  TString _ID;                       // Subdetector ID

  AMSBeta * _pbeta;                  // pointer to beta 
  float _Q;                          // Charge magnitude from truncated mean or similar

  vector <unsigned short> _ChargeI;  // Charge indexes ordered in decreasing probability
  vector <float> _Lkhd;              // logLikelihood values from PDF
  vector <float> _Prob;              // estimated probabilities for different charges

  double _ProbSum;                   // Sum of probs for all chare hypotheses 
  int _Priority;                     // Priority in charge combination (among all with same ClassID)
 
  static int   _debug;         // debug level  

  /********** (old scheme...but  still in use) */
  double _Charge;
  vector <double> _Probz;  // prob  to be e,p,He,...,F
  vector <double> _Lkhdz;  // -loglikhd value for e,p,...
  vector <int>    _Indxz;  // index 0,...,9 from most to least prob charge
  int    _i;               // index of most probable charge
  /*********/ 
 
  void Init(int refit=false);
  virtual int ComputeCharge(int refit=false){return 0;}
  void FillRootVectors();
  
  double probcalc(int fit, int nhit,double lkhd[]);
  int sortlkhd();

  float getQ(){return _Q;}

  double getprobcharge(int index, bool normalized=1){if (index<_Probz.size())return _Probz[index]/(normalized?_ProbSum:1); return 0;}
  double getlkhdcharge(int index){return _Lkhdz[index];}
  int getindex(){return _i;}
  int getcharge(){return _Charge;}
  int getsize(){return _Indxz.size();}
  int getlkhdsize(){return _Lkhdz.size();}
  int getprobsize(){return _Probz.size();}
  int getsplitlevel();
  int getchargemax();
  double getprobmin();

  AMSBeta * getpbeta(){return _pbeta;}

  const TString &getID(){return _ID; }

  // Static methods
  static double resmax(double x[],int ntot,int refit,double rescut,int &imax,double &mean,double &trres,double &trmax);
  static void lkhcalc(int mode, double beta, int nhit, double ehit[], int typeh[], double lkh[]);
  static int ind2charge(int ind) { return max(1,ind); }
  static TString ClassID() {return "";}
  
  void _copyEl(){};
  void _printEl(ostream & stream){ stream << _ID << " Charge: " << _Charge <<" Probs : "; for (int i=0; i<min(4,getprobsize()); i++) stream <<getprobcharge(i)<<" "; stream << endl;}
  void _writeEl(){};
  virtual int _getParent(){return -1;};
  virtual void _addAttr(map<TString,float> &attr){};
  
  AMSChargeSubD(const char *id,AMSBeta *pbeta):_ID(id),_Q(0),_pbeta(pbeta),_ProbSum(0){};
    virtual ~AMSChargeSubD(){}; 
};


/* Class for TOF charge reconstruction */

class AMSChargeTOF : public AMSChargeSubD {
 public:
  
  float _TruncatedMean;       // truncated mean of the energy depositions (no beta corrections)
  
  static integer _chargeTOF[MaxZTypes];

  int ComputeCharge(int refit=false);

  static TString ClassID(){return "AMSChargeTOF";}

  float getTruncatedMean() {return _TruncatedMean;}
  
  void FlagBad(){clearstatus(AMSDBc::BAD);}  // placeholder for beta selection
  int Fill(int refit);     
  int Fit(int refit, double beta, int bstatus, int nhit,
	  AMSTOFCluster *pTOFc[], double etof[TOF2GC::SCLRS]);
  double EdepBetaCorrection(int ichar, double beta);

  int _getParent(){return _pbeta==NULL?-1:_pbeta->GetClonePointer();}
  void _addAttr(map<TString,float> &attr){attr["TruncatedMean"]=getTruncatedMean();}

  AMSChargeTOF(AMSBeta *pbeta):AMSChargeSubD(ClassID(),pbeta){};
    ~AMSChargeTOF(){};
};


/* Base Class for Tracker charge reconstruction */

class AMSChargeTracker : public AMSChargeSubD {
 public:
  
  AMSTrTrack *_ptrtk;
  
  float _TruncatedMean;       // truncated mean of the energy depositions (no beta corrections)
  float _ProbAllTracker;      // probability of most probable charge using all tracker hits

  static integer _chargeTracker[MaxZTypes];
  
  int ComputeCharge(int refit=false);
  static TString ClassID(){return "AMSChargeTracker";}

  float getTruncatedMean() {return _TruncatedMean;}
  float getProbAllTracker() {return _ProbAllTracker;}
  
  void FlagBad(){clearstatus(AMSDBc::BAD);}  // placeholder for track selection
  
  int Fill(int refit);
#ifndef _PGTRACK_
  int Fit(int fit, double beta, int bstatus, int nhit,
	  AMSTrCluster *pTrackerc[trconst::TrkTypes-1][trconst::maxlay], 
	  double etrk[trconst::TrkTypes-1][trconst::maxlay]);
  double EdepBetaCorrection(int ichar, double beta);
#else
  int Fill(int index, like_t lkhd);
#endif
  
  int _getParent(){return _ptrtk==NULL?-1:_ptrtk->GetClonePointer();}  
  void _addAttr(map<TString,float> &attr){attr["TruncatedMean"]=getTruncatedMean();attr["ProbAllTracker"]=getProbAllTracker();}
  
  AMSChargeTracker(AMSBeta *pbeta, AMSTrTrack *ptrtk):AMSChargeSubD(ClassID(),pbeta),_ptrtk(ptrtk){};
    ~AMSChargeTracker(){};
};


/* Class for Tracker Inner Charge Reconstruction (provision) */

class AMSChargeTrackerInner : public AMSChargeTracker {
 public:
  
  static TString ClassID(){return "AMSChargeTrackerInner";}
  AMSChargeTrackerInner(AMSBeta *pbeta, AMSTrTrack *ptrtk):AMSChargeTracker(pbeta, ptrtk){_ID=ClassID();}
    ~AMSChargeTrackerInner(){};
};


/* Class for Tracker Upper Charge Reconstruction (provision) */

class AMSChargeTrackerUpper : public AMSChargeTracker {
 public:

  static TString ClassID(){return "AMSChargeTrackerUpper";}
  AMSChargeTrackerUpper(AMSBeta *pbeta, AMSTrTrack *ptrtk):AMSChargeTracker(pbeta, ptrtk){_ID=ClassID();}
    ~AMSChargeTrackerUpper(){};
};


/* Class for Tracker Lower Charge Reconstruction (provision) */

class AMSChargeTrackerLower : public AMSChargeTracker {
 public:

  static TString ClassID(){return "AMSChargeTrackerLower";}
  AMSChargeTrackerLower(AMSBeta *pbeta, AMSTrTrack *ptrtk):AMSChargeTracker(pbeta, ptrtk){_ID=ClassID();}
    ~AMSChargeTrackerLower(){};
};


/* Class for TRD Charge Reconstruction */

class AMSChargeTRD : public AMSChargeSubD {
 public:

  AMSTRDTrack *_ptrd;

  int ComputeCharge(int refit=false);
  static TString ClassID(){return "AMSChargeTRD";}

  void FlagBad(){clearstatus(AMSDBc::BAD);}  // placeholder for track selection
  int Fill(int refit);

  AMSChargeTRD(AMSBeta *pbeta, AMSTRDTrack *ptrd):AMSChargeSubD(ClassID(),pbeta),_ptrd(ptrd){};
    ~AMSChargeTRD(){};
};


/* Class for Rich Charge Reconstruction */

class AMSChargeRich : public AMSChargeSubD {
 public:
 
  AMSRichRing *_pring;

  int ComputeCharge(int refit=false);
  static TString ClassID(){return "AMSChargeRich";}

  void FlagBad(){if (_pring->getprob()<CHARGEFITFFKEY.ProbklMin) setstatus(AMSDBc::BAD); else clearstatus(AMSDBc::BAD);}
  int Fill(int refit);

  AMSChargeRich(AMSBeta *pbeta, AMSRichRing *pring):AMSChargeSubD(ClassID(),pbeta),_pring(pring){};
    ~AMSChargeRich(){};
};


/* Class for Global AMS Charge Reconstruction */

class AMSCharge : public AMSlink{
 public:
  AMSBeta * _pbeta;            // pointer to beta 

  map <TString,AMSChargeSubD*> _charges;  // map storing all SubQ charges

  vector <unsigned short> _ChargeI; //Charge indexes ordered in decreasing probability  
  vector <float>     _Lkhd;         //loglikelihood values from PDFs
  vector <float>     _Prob;         //estimated probabilities for different charges

  int _Charge;           //most probable charge
  int _Nused;            //number of SubD charges used

  static int   _debug;         // debug level  

  /********** (old scheme...but  still in use) */ 
  vector <double>  _Probz;  // prob  to be e,p,He,...,F
  vector <double>  _Lkhdz;  // -loglikhd value for e,p,...
  vector <int>     _Indxz;  // index 0,...,9 from most to least prob charge
  int    _i;                // index of most probable charge
  /**********/ 

  enum TrackerCombType { trkAll = 1, trkInner = 10, trkUpper = 100, trkLower = 1000};

  static int build(int refit);
  static int refit(){return 0;}
  AMSCharge *next(){ return (AMSCharge *)_next; }

  int BuildTOF(AMSBeta *pbeta);
  int BuildTracker(AMSBeta *pbeta);
  int BuildTRD(AMSBeta *pbeta);  
  int BuildRich(AMSBeta *pbeta);

  int BuildCombine();
  void FillRootVectors();

  int SelectSubDCharges();
  int getvotedcharge(int & index);
  double getprobcharge(int charge);

  AMSBeta* getpbeta(){return _pbeta;}

  int getchargeTOF();
  int getchargeTracker();

  static int ind2charge(int idet, int ind) { return AMSChargeSubD::ind2charge(ind); } /* back compat */

  void _copyEl();
  void _printEl(ostream & stream){for(map<TString,AMSChargeSubD*>::iterator i=_charges.begin();i!=_charges.end();++i) i->second->_printEl(stream);}
  void _writeEl();

 AMSCharge(AMSBeta *pbeta);
  ~AMSCharge(){for(map<TString,AMSChargeSubD*>::iterator i=_charges.begin();i!=_charges.end();++i) delete i->second;}

};

#endif
