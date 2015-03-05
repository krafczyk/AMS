// $Id$

#ifndef __TrMipDB__
#define __TrMipDB__


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


#define MIP_ZMAX 26


//! \c TrMipElem 
class TrMipElem : public TObject {

 private: 

  //! Element index 
  int    fIndex;
  //! Number of points
  int    fN;
  //! Z 
  double fZ[MIP_ZMAX];
  //! Measured value 
  double fMPV[MIP_ZMAX];

 public:
  
  //! c-tor
  TrMipElem(int index);
  //! d-tor
  ~TrMipElem() { Clear(); }
  //! Clear
  void   Clear(Option_t* option = "");
  //! Info
  void   Info(int verbosity=0);
  //! Get index
  int    GetIndex() { return fIndex; } 
  //! Get Z 
  double GetZ(int i) { return ( (i>0)&&(i<GetN()) ) ? fZ[i] : 0; } 
  //! Get MPV value 
  double GetMPV(int i) { return ( (i>0)&&(i<GetN()) ) ? fMPV[i] : 0; } 
  //! Get number of points
  int    GetN() { return fN; } 
  //! Add point
  void   AddPoint(double z, double mpv);
  //! Is the element valid (good description of the correction) ?
  bool   IsValid();
  //! Return corrected value
  double GetMipCorrectedValue(double Q);

  using TObject::Info;
  using TObject::Clear;
  ClassDef(TrMipElem,1);
};


//! \c TrMipDB 
class TrMipDB : public TObject {

 private:

  //! Map for fast binary search
  static map<int,TrMipElem*> fTrMipElemIndexMap;
  //! Singleton
  static TrMipDB* fHead;
  //! Init done
  static bool fInitDone;

 public:

  //! Disable MIP correction 
  static bool fDisableMipCorrection; 

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrMipDB() {} // { Clear(); }
  //! Destructor
  ~TrMipDB(); 
  //! Get self-pointer
  static TrMipDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void   Init(bool force = false);
  //! Clean up the table database
  void   Clear(Option_t* option = "");
  //! Info
  void   Info(int verbosity = 0);
  //! Create index from TkId and VA number for requested depth 
  int    CreateIndex(int tkid, int iva, int depth);
  //! Get element from index
  TrMipElem* GetElem(int index);
  //! Get element from TkId, VA and depth
  TrMipElem* GetElem(int tkid, int iva, int depth);
  //! Get valid element from TkId, VA and depth
  TrMipElem* GetValidElem(int tkid, int iva, int depth);

  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer1(char* dirname = getenv("AMSDataDir"));

  //! Get the mip correction 
  double GetMipCorrectedValue(double Q, int tkid, int iva, int depth);

  using TObject::Info;
  ClassDef(TrMipDB,1);
};


#endif

