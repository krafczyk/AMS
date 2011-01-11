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

#include "root.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TObject.h"
#include "TNamed.h"
#include "TDirectory.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TObjArray.h"
#include "TObjString.h"

//! Class TrHistoManHeader, the Tracker Histogram Manager Header
/*!
  The class TrHistoManHeader is used to store the histogram manager 
  useful generic informations.
*/

using namespace trconst;

class TrHistoManHeader : public TNamed {

 public:

  //! Run number
  int  fRunNumber;
  //! File list 
  TObjArray fFileList;

 public:

  //! Default constructor
  TrHistoManHeader() {}  
  //! Constructor
  TrHistoManHeader(char* name, char* title) : TNamed(name, title) {}
  //! Copy constructor 
  TrHistoManHeader(const TrHistoManHeader &);
  //! Destructor 
  ~TrHistoManHeader() { Clear(); }
  //! Clear
  void Clear(); 
  //! Adding
  void Add(TrHistoManHeader* tobeadded) {} 
  //! Merge ... 

  //! Add file name to the list 
  void        AddFileName(char* name) { fFileList.Add(new TObjString(name)); }
  //! Get number of files used
  int         GetNFileNames() { return fFileList.GetEntries(); }
  //! Return the i-th file name
  const char* GetFileName(int i) { return (i<GetNFileNames()) ? fFileList.At(i)->GetName() : 0; }
  //! Set run number
  void        SetRunNumber(int runnumber) { fRunNumber = runnumber; } 
  //! Get the file name list (TObjString objs)
  TObjArray   GetFileNameList() { return fFileList; }
  //! Get run number
  int         GetRunNumber() { return fRunNumber; }

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
  map<Int_t,TNamed*> fHashTable;

 public:

  //! Constructor (file, folder-name, folder-title)
  TrHistoMan(TFile* file, char* name, char* title);
  //! Copy constructor 
  TrHistoMan(const TrHistoMan &);
  //! Destructor 
  virtual ~TrHistoMan();

  //! Write the histogram manager on a file
  void Write();
  //! Add an histogram to the manager
  void Add(TNamed* obj);
  //! Returns the pointer to the histogram with the required name
  TNamed* Get(char* name);

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
  void Define(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void Define(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void Define(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d tracker histo sets 
  void DefineTracker(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineTracker(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineTracker(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d layers histo sets 
  void DefineLayers(char* name, char*title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineLayers(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineLayers(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books 1d/2d/3d ladders histo sets (192 or 384!) 
  void DefineLadders(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx, 
		Bool_t bothsides = kFALSE);
  void DefineLadders(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy, 
		Bool_t bothsides = kFALSE);
  void DefineLadders(char* name, char* title, 
		Int_t binx, Double_t lowx, Double_t upx,
		Int_t biny, Double_t lowy, Double_t upy,
		Int_t binz, Double_t lowz, Double_t upz, 
		Bool_t bothsides = kFALSE);
  //! Books an entries histogram (entries for each ladder in a layer vs slot plot)
  void DefineEntries(char* name, Bool_t bothsides = kFALSE);

  /////////////////////////
  // Filling methods
  /////////////////////////

  //! Fills a 1d/2d/3d histogram (entries/tracker/layers/ladders) 
  void Fill(Bool_t bothsides, TrRawClusterR* cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Bool_t bothsides, TrClusterR*    cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Bool_t bothsides, TrRecHitR*     hit,     char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {} 
  void Fill(Bool_t bothsides, TrTrackR*      track,   char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void Fill(Int_t side, Int_t tkid, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(Int_t tkid, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d tracker histograms 
  void FillTracker(Bool_t bothsides, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(Bool_t bothsides, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(Bool_t bothsides, TrRecHitR*     hit,     char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {} 
  void FillTracker(Bool_t bothsides, TrTrackR*      track,   char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillTracker(Int_t side, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d layer histograms (by cluster) 
  void FillLayer(Bool_t bothsides, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Bool_t bothsides, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Bool_t bothsides, TrRecHitR*     hit,     char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLayer(Bool_t bothsides, TrTrackR*      track,   char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLayer(Int_t side, Int_t layer, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer(Int_t layer, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d/2d/3d ladder histograms 
  void FillLadder(Bool_t bothsides, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Bool_t bothsides, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Bool_t bothsides, TrRecHitR*     hit,     char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLadder(Bool_t bothsides, TrTrackR*      track,   char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillLadder(Int_t side, Int_t tkid, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder(Int_t tkid, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills an Entries histogram 
  void FillEntry(Bool_t bothsides, TrRawClusterR* cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);  
  void FillEntry(Bool_t bothsides, TrClusterR*    cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillEntry(Bool_t bothsides, TrRecHitR*     hit,     char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}
  void FillEntry(Bool_t bothsides, TrTrackR*      track,   char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.) {}  
  void FillEntry(Int_t side, Int_t tkid, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);  
  void FillEntry(Int_t tkid, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);  

  /////////////////////////
  // Other methods
  /////////////////////////

  //! Resets an histogram (via TObject)
  void ResetHisto(TNamed* object); 
  //! Reset a Tracker histogram
  void ResetTracker(char* name); 

 protected:

  //! Request the error calculation for all the histograms
  void Sumw2();

  ClassDef(TrHistoMan,1);
};


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

 public:

  //! Constructor 
  TrOnlineMonitor(TFile* file, int flag = 0, float dtmin = 200 /* us */) : 
    TrHistoMan(file, "TrOnlineMon", "Tracker Online Monitor"),
    fFlag(flag), fDtMin(dtmin) {}
  //! Copy constructor 
  TrOnlineMonitor(const TrOnlineMonitor &);
  //! Destructor
  ~TrOnlineMonitor() { fFlag = 0; fDtMin = 0; }

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
