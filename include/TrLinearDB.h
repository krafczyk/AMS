// $Id: TrLinearDB.h,v 1.1 2013/02/15 15:20:22 oliva Exp $

#ifndef __TrLinearDB__
#define __TrLinearDB__


#include "TkDBc.h"
#include "TkLadder.h"


#include "TObject.h"
#include "TFile.h"
#include "typedefs.h"
#include "TH2D.h"


#include <iostream>
#include <fstream>
#include <string>
#include <map>


//! \c TrLinearElem contains informations about p-strip linearization of a single element (VA, ladder, layer or tracker)
class TrLinearElem : public TObject {

 private: 

  //! Element index (0: whole tracker; 1, ...,9: layer; (0, ..., 192)+10: ladder+10, ((0, ..., 192)+10)*100 + (0, ..., 9): VA)
  int    fIndex;
  //! Performed fit was succesfull 
  int    fSucc;
  //! Chi squared of the fit 
  double fChisq;
  //! Parameters
  double fPars[5];

 public:
  
  //! c-tor
  TrLinearElem(int index, int succ, double chisq, double* pars);
  //! c-tor
  TrLinearElem(int index, int succ, float chisq, float* pars);
  //! d-tor
  ~TrLinearElem() {}
  //! Info
  void   Info();
  //! Get index
  int    GetIndex() { return fIndex; } 
  //! Is the correction OK?
  int    GetSucc()  { return fSucc; } 
  //! Get chisquare of the fit 
  double GetChisq() { return fChisq; } 
  //! Get parameter pointer
  double GetPar(int ipar)  { return ( (ipar>=0)&&(ipar<5) ) ? fPars[ipar] : -9999; } 
  //! Is the element valid (good description of the correction) ?
  bool   IsValid(); 
  //! Return corrected value
  double GetLinearityCorrected(double ADC); 

  using TObject::Info;
  ClassDef(TrLinearElem,1);
};


//! \c TrLinearDB contains informations about p-strip linearization (mostly for the ions resolution improvement).
class TrLinearDB : public TObject {

 private:

  //! Map for fast binary search
  static map<int,TrLinearElem*> fTrLinearElemIndexMap;
  //! Singleton
  static TrLinearDB* fHead;

 public:

  //! Default correction depth 
  static int DefaultCorrDepth;

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrLinearDB() { Clear(); }
  //! Destructor
  ~TrLinearDB(); 
  //! Get self-pointer
  static TrLinearDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void   Init();
  //! Clean up the table database
  void   Clear(Option_t* option = "");
  //! Info
  void   Info();
  //! Create index from TkId and VA number for requested depth 
  int    CreateIndex(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Add element to the database
  bool   AddElem(int index, TrLinearElem* elem);
  //! Get element from index
  TrLinearElem* GetElem(int index); 
  //! Get element from TkId, VA and depth
  TrLinearElem* GetElem(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Get valid element from TkId, VA and depth
  TrLinearElem* GetValidElem(int tkid, int iva, int depth = DefaultCorrDepth);
  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer0(char* dirname = getenv("AMSDataDir"));
  //! Get the linearity correction at different level (depth: 0: tracker; 1: layer; 2: ladder; 3: VA) 
  double GetLinearityCorrected(double ADC, int tkid, int iva, int depth = DefaultCorrDepth);

  using TObject::Info;
  ClassDef(TrLinearDB,1);
};


//! Functional description of the p-side strip behavior
double p_strip_behavior(double* x, double* par);


#endif

