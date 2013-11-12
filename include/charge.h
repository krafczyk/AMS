//  $Id: charge.h,v 1.34 2013/11/12 09:28:46 qyan Exp $
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
#include "tofhit.h"

using namespace AMSChargConst;

const number powmx=50.;
enum SubDiD { kTOF=0, kTracker=1, kTRD=2, kRich=3 };


/* Base class for subdetector charge calculation */
class AMSChargeBase {

 public:

  // floating point charge estimator 
  float _Q; 

  ////////////////////////////////////////////////////////  
  // vector of charge hypothesys ordered from highest to lowest (old scheme still in use)
  ////////////////////////////////////////////////////////  

  // charge indexes vector (0: electron, >0: Z)
  vector <int>    _Indxz;     
  // -logLikelihood vector
  vector <double> _Lkhdz; 
  // normalized probability vector, used to fill ROOT variables  
  vector <double> _Probz;  
  // normalization of probability vector
  double          _ProbSum;                 

  ////////////////////////////////////////////////////////  
  // interface to ROOT
  ////////////////////////////////////////////////////////  

  // charge indexes vector (0: electron, >0: Z), used to fill ROOT variables 
  vector <unsigned short> _ChargeI;  
  // -logLikelihood vector, used to fill ROOT variables 
  vector <float> _Lkhd;              
  // noramlized probability vector, used to fill ROOT variables  
  vector <float> _Prob;              

 public: 

  // c-tor 
  AMSChargeBase() { clear(); }
  // d-tor
  virtual ~AMSChargeBase() { clear(); }    
  // copy 
  void   copyvalues(const AMSChargeBase& that);    
  // get charge estimator
  float  getq() { return _Q; }
  // get most probable charge index 
  int    getindex() { return (!isempty()) ? _Indxz[0] : -1; }  
  // return charge value 
  int    getcharge() { return (getindex()==0) ? 1 : getindex(); }
  // get size of index vector
  int    getn() { return (int)_Indxz.size(); }
  // get size of index vector
  int    getsize() {return _Indxz.size();}
  // get size of likelihood vector
  int    getlkhdsize() {return _Lkhdz.size();}
  // get size of prbability vector
  int    getprobsize() {return _Probz.size();}
  // clear everything
  void   clear();
  // patch to treat the electron case
  void   addelectron();
  // sort vectors and copy to ROOT copies (eliminate hypothesis with low probability, and shrink the ROOT interface to some maximum size) 
  bool   sortandfill(bool uselike = true, double maxLogLike = 1000, double minProb = 1e-100, int maxRootSize = -1);
  // is the charge index present in the list 
  bool   isvalid(int index, double maxLogLike = 50, double minProb = 1e-50);
  // search probability by charge index
  double getprobindex(int index);
  // search likelihood by charge index
  double getlkhdindex(int index);
  // get the charge index span of the vectors
  void   getminmaxindex(int& minindex, int& maxindex);
  // check if vectors are empty 
  bool   isempty() { return _Indxz.empty(); }
  // check if vector sizes are consistent with each other 
  bool   isconsistent() { return ((int)_Indxz.size()==(int)_Lkhdz.size())&&((int)_Indxz.size()==(int)_Probz.size()); }
  // return an overall goodness check 
  bool   isok();

};


/* Base class for subdetector charge reconstruction */
class AMSChargeSubD : public AMSChargeBase, public AMSlink {
 
 public:

  // Subdetector ID
  TString _ID;     
  // Pointer to beta 
  AMSBeta* _pbeta;    
  // Pointer to betaH 
  AMSBetaH* _pbetah; 

 public: 

  // name
  static TString ClassID() { return ""; }
  // c-tor 
  AMSChargeSubD(const char *id, AMSBeta *pbeta, AMSBetaH* pbetah):_ID(id),_pbeta(pbeta),_pbetah(pbetah) {}
  // d-tor 
  virtual ~AMSChargeSubD() {}
  // get name 
  const TString &getID() { return _ID; }
  // get beta
  AMSBeta* getpbeta() { return _pbeta;}
  // get betaH
  AMSBetaH* getpbetah() { return _pbetah; }
  // a simple dump
  void print();

  // init
  void Init(int refit = false);
  // virtual method for charge calculation 
  virtual int ComputeCharge(int refit = false){ return 0; }

  // old charge evaluation related routines 
  double        probcalc(int fit, int nhit,double lkhd[]);
  // old charge evaluation related routines 
  static double resmax(double x[],int ntot,int refit,double rescut,int &imax,double &mean,double &trres,double &trmax);
  // old charge evaluation related routines 
  static void   lkhcalc(int mode, double beta, int nhit, double ehit[], int typeh[], double lkh[]);
  // old charge evaluation related routines 
  int           sortlkhd(); 
 
  // copy element 
  void _copyEl(){}
  // print element
  void _printEl(ostream & stream){ stream << _ID << " Charge: " << getcharge() <<" Probs : "; for (int i=0; i<min(4,getprobsize()); i++) stream << _Probz[i]<<" "; stream << endl;}
  // write element
  void _writeEl(){}

  // additional attribute map 
  virtual void _addAttr(map<TString,float> &attr) {} 

};


/* Base class for TOF charge reconstruction */
class AMSChargeTOF : public AMSChargeSubD {

 public:

  // truncated mean  
  float _TruncatedMean;  
  // betaH is used to calculate charge
  int _isBetaH;
  // parameter for old fitting 
  static integer _chargeTOF[MaxZTypes];

 public:

  // name
  static TString ClassID() { return "AMSChargeTOF"; }
  // c-tor
  AMSChargeTOF(AMSBeta *pbeta, AMSBetaH* pbetah):AMSChargeSubD(ClassID(),pbeta,pbetah){}
  // d-tor
  ~AMSChargeTOF() {}
  // virtual
  int ComputeCharge(int refit = false);
  // truncated mean
  float getTruncatedMean() {return _TruncatedMean;}
  // code used to produce charge
  int getversion() { return _isBetaH; }
  // filler
  int Fill(int refit);     
  // old charge computation
  int Fit(int refit, double beta, int bstatus, int nhit, AMSTOFCluster *pTOFc[], double etof[TOF2GC::SCLRS]);
  // energy deposition correction
  double EdepBetaCorrection(int ichar, double beta);
  // attributes
  void _addAttr(map<TString,float> &attr){ 
    attr["TruncatedMean"] = getTruncatedMean();
    attr["isBetaH"] = getversion();
  }
};


/* Class for Upper TOF charge reconstruction */
class AMSChargeTOFUpper : public AMSChargeTOF {

 public:

  // name
  static TString ClassID() { return "AMSChargeTOFUpper"; }
  // c-tor
  AMSChargeTOFUpper(AMSBeta *pbeta, AMSBetaH* pbetah):AMSChargeTOF(pbeta,pbetah){_ID=ClassID();}
  // d-tor
  ~AMSChargeTOFUpper() {}
};


/* Class for Lower TOF charge reconstruction */
class AMSChargeTOFLower : public AMSChargeTOF {

 public:

  // name 
  static TString ClassID() { return "AMSChargeTOFLower"; }
  // c-tor
  AMSChargeTOFLower(AMSBeta *pbeta, AMSBetaH* pbetah):AMSChargeTOF(pbeta,pbetah){_ID=ClassID();}
  // d-tor
  ~AMSChargeTOFLower(){}
};


/* Base class for Tracker charge reconstruction */
class AMSChargeTracker : public AMSChargeSubD {

 public:

  // track pointer  
  AMSTrTrack *_ptrtk;
  // truncated mean
  float _TruncatedMean;
  // parameters for old fitting 
  static integer _chargeTracker[MaxZTypes];
  // v5 is used to calculate charge
  int _isPGTRACK;

 public: 

  // name 
  static TString ClassID() { return "AMSChargeTracker"; }
  // c-tor
  AMSChargeTracker(AMSBeta *pbeta, AMSTrTrack *ptrtk, AMSBetaH* pbetah):AMSChargeSubD(ClassID(),pbeta,pbetah),_ptrtk(ptrtk){}
  // d-tor
  ~AMSChargeTracker() {}
  // virtual
  int ComputeCharge(int refit=false);
  // truncated mean (no beta correction)
  float getTruncatedMean() { return _TruncatedMean; }
  // code used to produce charge
  int getversion() { return _isPGTRACK; } 
  // fill
  int Fill(int refit);
#ifndef _PGTRACK_
  // old charge computation (v4)
  int Fit(int fit, double beta, int bstatus, int nhit,AMSTrCluster *pTrackerc[trconst::TrkTypes-1][trconst::maxlay],double etrk[trconst::TrkTypes-1][trconst::maxlay]);
  // beta correction
  double EdepBetaCorrection(int ichar, double beta);
#endif
  // attribute map
  void _addAttr(map<TString,float> &attr) {
    attr["TruncatedMean"] = getTruncatedMean();
    attr["isPGTRACK"] = getversion();
  }
};


/* Class for Tracker Inner Charge Reconstruction */
class AMSChargeTrackerInner : public AMSChargeTracker {

 public:

  // name
  static TString ClassID() { return "AMSChargeTrackerInner"; }
  // c-tor  
  AMSChargeTrackerInner(AMSBeta *pbeta, AMSTrTrack *ptrtk, AMSBetaH* pbetah):AMSChargeTracker(pbeta,ptrtk,pbetah){_ID=ClassID();}
  // d-tor
  ~AMSChargeTrackerInner() {}
};


/* Class for Tracker Upper Charge Reconstruction */
class AMSChargeTrackerUpper : public AMSChargeTracker {

 public:

  // name 
  static TString ClassID() { return "AMSChargeTrackerUpper"; }
  // c-tor
  AMSChargeTrackerUpper(AMSBeta *pbeta, AMSTrTrack *ptrtk, AMSBetaH* pbetah):AMSChargeTracker(pbeta,ptrtk,pbetah){_ID=ClassID();}
  // d-tor
  ~AMSChargeTrackerUpper() {}
};


/* Class for Tracker Lower Charge Reconstruction */
class AMSChargeTrackerLower : public AMSChargeTracker {

 public:

  // name
  static TString ClassID() { return "AMSChargeTrackerLower"; }
  // c-tor
  AMSChargeTrackerLower(AMSBeta *pbeta, AMSTrTrack *ptrtk, AMSBetaH* pbetah):AMSChargeTracker(pbeta,ptrtk,pbetah){_ID=ClassID();}
  // d-tor
  ~AMSChargeTrackerLower() {}
};


/* Class for TRD Charge Reconstruction */
class AMSChargeTRD : public AMSChargeSubD {

 public:

  // trd track
  AMSTRDTrack *_ptrd;

 public:
  
  // name
  static TString ClassID() { return "AMSChargeTRD"; }
  // c-tor
  AMSChargeTRD(AMSBeta *pbeta, AMSTRDTrack *ptrd, AMSBetaH* pbetah):AMSChargeSubD(ClassID(),pbeta,pbetah),_ptrd(ptrd){};
  // d-tor
  ~AMSChargeTRD(){}
  // virtual
  int ComputeCharge(int refit=false);
  // filler
  int Fill(int refit);
};


/* Class for Rich Charge Reconstruction */
class AMSChargeRich : public AMSChargeSubD {

 public:

  // rich ring 
  AMSRichRing *_pring;

 public: 

  // name
  static TString ClassID() { return "AMSChargeRich"; }
  // c-tor
  AMSChargeRich(AMSBeta *pbeta, AMSRichRing *pring, AMSBetaH* pbetah):AMSChargeSubD(ClassID(),pbeta,pbetah),_pring(pring){};
  // d-tor
  ~AMSChargeRich(){}
  // virtual
  int ComputeCharge(int refit = false);
  // filler
  int Fill(int refit);
  // is bad
  bool isbad(){ return (_pring->getprob()<CHARGEFITFFKEY.ProbklMin); }
};


/* Class for AMS Upper Charge Reconstruction */
class AMSChargeUpper : public AMSChargeSubD {

 public:

  // name
  static TString ClassID(){return "AMSChargeUpper";}
  // c-tor
  AMSChargeUpper() : AMSChargeSubD(ClassID(),0,0) {};
  // d-tor
  ~AMSChargeUpper(){}
  // virtual
  int ComputeCharge(int refit = false) { return 1; }
};



/* Class for AMS Upper Charge Reconstruction */
class AMSChargeFragmentation : public AMSChargeSubD {

 public:

  // name
  static TString ClassID(){return "AMSChargeFragmentation";}
  // c-tor
   AMSChargeFragmentation() : AMSChargeSubD(ClassID(),0,0) {};
  // d-tor
  ~AMSChargeFragmentation(){}
  // virtual
  int ComputeCharge(int refit = false) { return 1; }
};



/* Class for Global AMS Charge Reconstruction */
class AMSCharge : public AMSChargeBase, public AMSlink {

 public:

  // Subdetector ID
  TString _ID;
  // Pointer to beta 
  AMSBeta* _pbeta;
  // Pointer to betaH 
  AMSBetaH* _pbetah;

  // map of all estimators 
  map <TString,AMSChargeSubD*> _charges;  

  // to-be-implemented: vector of used detectors for each charge? 

  // available multiple estimators 
  enum TrackerCombType { trkAll = 1, trkInner = 10, trkUpper = 100, trkLower = 1000};
  enum TofCombType { tofAll = 1, tofUpper = 10, tofLower = 100};

 public:

  // c-tor 
  AMSCharge(AMSBeta *pbeta);
  // d-tor 
  ~AMSCharge() { for(map<TString,AMSChargeSubD*>::iterator i=_charges.begin();i!=_charges.end();++i) delete i->second; _charges.clear(); }

  // get beta
  AMSBeta* getpbeta() { return _pbeta;}
  // get betaH
  AMSBetaH* getpbetah() { return _pbetah; }
  // a simple dump
  void print();

  // build
  static int build(int refit);
  // refit is summy for now
  static int refit() {return 0;}
  // move
  AMSCharge* next() { return (AMSCharge *)_next; }

  // build TOF estimators (all, upper, lower)
  int BuildTOF();
  // build Tracker estimators (all, inner, upper, lower)
  int BuildTracker();
  // build TRD estimator
  int BuildTRD();  
  // build RICH estimator
  int BuildRich();
  // build ECAL estimator (to be implemented)
  int BuildECAL() { return 0; } 
  // Build overall charge 
  int BuildCombine();
  // Build upper AMS estimator 
  int BuildUpper(); 
  // Build fragmentation estimator 
  int BuildFragmentation();

  // calculate overall charge given a detector list
  AMSChargeBase getvotedcharge(const vector<TString> &detectors);
  // calculate overall charge
  int getvotedcharge(int& index); 
  // evaluate probability given Z
  double getprobcharge(int charge);

  // charge from TOF
  int getchargeTOF();
  // charge from Tracker
  int getchargeTracker();

  // copy all elements 
  void _copyEl();
  // print all elements
  void _printEl(ostream & stream){for(map<TString,AMSChargeSubD*>::iterator i=_charges.begin();i!=_charges.end();++i) i->second->_printEl(stream);}
  // write all elements
  void _writeEl();

  // kept for backward compatibility 
  static int ind2charge(int idet, int ind) { return max(1,ind); } 
};


#ifdef _PGTRACK_
// A fake BetaHR class that replaces methods based on indexes with methods based on pointers.
class FakeBetaHR : public BetaHR {
 public:
  /// pointers to clusters
  vector<TofClusterHR*> _pclusterh;
  /// pointer to track
  TrTrackR*             _ptrtrack;
  ///  trdtrack
   TrdTrackR            _trdtrack;
  // c-tor  
  FakeBetaHR();
  // c-tor with AMSBetaH 
  FakeBetaHR(AMSBetaH* ptr);
  // d-tor
  virtual ~FakeBetaHR();
  /// overload access to TofClusterHR objects used
  int           NTofClusterH() const { return (int)_pclusterh.size(); }
  /// overload access to TofClusterHR object 
  TofClusterHR* pTofClusterH(unsigned int i) { return _pclusterh[i]; }
  /// overload access et pointer to TOF BetaH iLayer(0-3) 
  TofClusterHR* GetClusterHL (int ilay); 
  /// if TOF BetaH iLayer(0-3) ClusterH exists Return true
  bool          TestExistHL(int ilay) { return (ilay>=0&&ilay<4)&&(GetClusterHL(ilay)!=0); }  
  /// overload access to TrTrackR
  TrTrackR*     pTrTrack() { return _ptrtrack; }
  /// overload access to TrdTrackR (not implementable)
  TrdTrackR*    pTrdTrack() { return (fTrdTrack>=0)?&_trdtrack:0; }
  /// overload access to EcalShowerR (not implementable)
  EcalShowerR*  pEcalShower() { return 0; }
};
#endif


#endif
