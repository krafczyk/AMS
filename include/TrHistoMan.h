#ifndef __TrHistoMan_h__
#define __TrHistoMan_h__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrHistoMan.h
///\brief Header file of TrHistoMan class
///
///\class TrHistoMan
///\brief The Histogram Manager 
///
//////////////////////////////////////////////////////////////////////////

#include "TkPlane.h"
#include "TkLadder.h"
#include "TkDBc.h"
#include "TrRawCluster.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrCharge.h"

#include "root.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TObject.h"
#include "TNamed.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TList.h"
#include "TCollection.h"

#ifndef __DARWIN__
#include <stdint.h>
#endif

#include <vector>
#include <map>

//! Class TrHistoManHeader, the Tracker Histogram Manager Header
/*!
  The class TrHistoManHeader is used to store the histogram manager 
  useful generic informations.
*/

using namespace trconst;

class TrHistoManHeader : public TNamed {

 public:

  //! Run number list
  vector<int> fRunList;
  //! File list 
  TObjArray   fFileList;

 public:

  //! Default constructor
  TrHistoManHeader() {}  
  //! Constructor
  TrHistoManHeader(const char* name, const char* title) : TNamed(name, title) {}
  //! Copy constructor 
  TrHistoManHeader(const TrHistoManHeader &);
  //! Destructor 
  ~TrHistoManHeader() { Clear(); }
  //! Clear
  void Clear(Option_t* oo=""); 
  //! Merging (needed for hadd) 
  Long64_t Merge(TCollection* list);

  //! Add file name to the list 
  void        AddFileName(const char* name) { fFileList.Add(new TObjString(name)); }
  //! Get number of files used
  int         GetNFileNames() { return fFileList.GetEntries(); }
  //! Return the i-th file name
  const char* GetFileName(int i) { return (i<GetNFileNames()) ? fFileList.At(i)->GetName() : 0; }
  //! Get the file name list (TObjString objs)
  TObjArray   GetFileNameList() { return fFileList; }
  //! Add run number
  void        AddRunNumber(int runnumber) { fRunList.push_back(runnumber); } 
  //! Get the number of runs used 
  int         GetNRunNumbers() { return int(fRunList.size()); }
  //! Get run number
  int         GetRunNumber(int i) { return fRunList.at(i); }

  ClassDef(TrHistoManHeader,1);
};


//! Class TrHistoMan, the Tracker Histogram Manager
/*!
  The class TrHistoMan implements the histogram manager.

  The method Define() provides a way to book histograms 
  for the different Tracker objects (tracker, layers, ladders, ...)
  put in a ordered subdirectories.
  If the bool variable bothsides is false a cumulative plot for
  the X(N) and Y(P) side is produced. If bothsides is true separate
  histograms for X and Y will be created.

  All created histograms are TH?D.
*/

class TrHistoMan : public TObject {

 protected:

  //! Header 
  TrHistoManHeader* fHeader;
  //! Output file 
  TFile* fFile;
  //! Main directory 
  TDirectory* fDir;  
  //! Directories for the layers; 
  TDirectory* fDirLayers[maxlay];  
  //! TObject hash table containing the histos
  map<uint32_t,TNamed*> fHashTable;
  //! Who is the Owner?
  Bool_t fOwnerItself;

 public:

  //! Constructor (file, folder-name, folder-title)
  TrHistoMan(TFile* file, const char* name, const char* title);
  //! Copy constructor 
  TrHistoMan(const TrHistoMan &);
  //! Destructor 
  virtual ~TrHistoMan();

  //! Write the histogram manager on a file
  int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0);
  int Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) const;
  //! Add an histogram to the manager
  void Add(TNamed* obj);
  //! Returns the pointer to the histogram with the required name
  TNamed* Get(const char* name);
  //! Static function for the hash calculation
  static uint32_t CalcHash(const char *str);

  //! Returns the pointer to the histogram header
  inline TrHistoManHeader* GetHeader() const { return fHeader; } 
  //! Returns the pointer to the file
  inline TFile* GetFile() const { return fFile; }
  //! Returns the pointer to the directory holding the histograms
  inline TDirectory* GetDir() const { return fDir; }

  /////////////////////////
  // Booking methods
  /////////////////////////

  //! Books 1d/2d/3d histo sets (tracker/layers/ladders)
  void Define(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void Define(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void Define(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d tracker histo sets 
  void DefineTracker(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineTracker(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineTracker(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d layers histo sets 
  void DefineLayers(const char* name, const char*title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineLayers(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineLayers(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d ladders histo sets (192 or 384!) 
  void DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineLadders(const char* name, const char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books an entries histogram (entries for each ladder in a layer vs slot plot)
  void DefineEntries(const char* name, Bool_t bothsides = kFALSE);

  /////////////////////////
  // Filling methods
  /////////////////////////

  //! Fills a 1d/2d/3d histogram (entries/tracker/layers/ladders) 
  void Fill(Bool_t bothsides, TrRawClusterR* cluster, const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Bool_t bothsides, TrClusterR*    cluster, const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Bool_t bothsides, TrRecHitR*     hit,     const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {} 
  void Fill(Bool_t bothsides, TrTrackR*      track,   const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void Fill(Int_t side, Int_t tkid, const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Int_t tkid, const char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d tracker histograms 
  void FillTracker(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(Bool_t bothsides, TrClusterR*    cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(Bool_t bothsides, TrRecHitR*     hit,     const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {} 
  void FillTracker(Bool_t bothsides, TrTrackR*      track,   const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillTracker(Int_t side, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d layer histograms (by cluster) 
  void FillLayer(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Bool_t bothsides, TrClusterR*    cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Bool_t bothsides, TrRecHitR*     hit,     const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLayer(Bool_t bothsides, TrTrackR*      track,   const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLayer(Int_t side, Int_t layer, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Int_t layer, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d ladder histograms 
  void FillLadder(Bool_t bothsides, TrRawClusterR* cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Bool_t bothsides, TrClusterR*    cluster, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Bool_t bothsides, TrRecHitR*     hit,     const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLadder(Bool_t bothsides, TrTrackR*      track,   const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLadder(Int_t side, Int_t tkid, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Int_t tkid, const char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills an Entries histogram 
  void FillEntry(Bool_t bothsides, TrRawClusterR* cluster, const char* name);  
  void FillEntry(Bool_t bothsides, TrClusterR*    cluster, const char* name);
  void FillEntry(Bool_t bothsides, TrRecHitR*     hit,     const char* name) {}
  void FillEntry(Bool_t bothsides, TrTrackR*      track,   const char* name) {}  
  void FillEntry(Int_t side, Int_t tkid, const char* name);  
  void FillEntry(Int_t tkid, const char* name);  

  /////////////////////////
  // Other methods
  /////////////////////////

  //! Resets an histogram (via TObject)
  void ResetHisto(TNamed* object); 
  //! Reset a Tracker histogram
  void ResetTracker(const char* name); 

 protected:

  //! Request the error calculation for all the histograms
  void Sumw2();

  ClassDef(TrHistoMan,1);
};


//! Time event structure 
/*! 
    Time dependent information storing 
*/

typedef struct {
  // Unix time [event]
  UInt_t   Time; 
  // Fine time [event]
  Float_t  FineTime;
  // Interevent distance (us) [event]
  Float_t  dT;
  // Livetime [event]
  Float_t  LiveTime;
  // Rigidity [particle] 
  Float_t  Rigidity;
  // log10(ChiSq) [particle]
  Float_t  logChiSq;
  // Truncated mean inner tracker X-Side [particle]
  Float_t  TruncMean;
  // TOF Beta [particle]
  Float_t  Beta;
  // Tracker Event Size (max 24000) [event] 
  UShort_t Size; 
  // Number of Raw Clusters [event]
  UShort_t nRaw; 
  // Number of Clusters [event]
  UShort_t nClu;  
  // Number of Hits [event]
  UShort_t nHit; 
  // Number of Tracks [event]
  UShort_t nTrk;
} TIME_EVENT;


//! Class TrOnlineMonitor, the Tracker Online Monitor
/*!
  The class TrOnlineMonitor is a TrHistoMan used for the Online data
  processing (inside/outside of GBATCH).
*/

class TrOnlineMonitor : public TrHistoMan {

 private:

  //! Level of detail (0 = high detailed, 1 = low detailed)
  int fFlag;  
  //! Low Dt definition
  float fDtMin;
  //! TTree for time dependent quantities
  TTree* fTimeNtuple;
  //! Structure for the time dependent quantities storing
  static TIME_EVENT time_event;

 public:

  //! Constructor 
  TrOnlineMonitor(TFile* file, int flag = 0, float dtmin = 200 /* us */) : 
    TrHistoMan(file, "TrOnlineMon", "Tracker Online Monitor"),
    fFlag(flag), fDtMin(dtmin) {}
  //! Copy constructor 
  TrOnlineMonitor(const TrOnlineMonitor &);
  //! Destructor
  virtual ~TrOnlineMonitor();

  //! GetTree
  TTree* GetTimeNtuple() { return fTimeNtuple; }

  //! Set level of detail (0 = high detailed, 1 = low detailed)
  void SetFlag(int flag) { fFlag = flag; }
  //! Set low dt definition
  void SetDtMin(float dtmin) { fDtMin = dtmin; }

  //! Histograms booking
  void Book();
  //! Histograms filling
  void Fill(AMSEventR* event);

  ClassDef(TrOnlineMonitor,1);
};

extern TrOnlineMonitor* ptrman;

#endif
