#ifndef __TrLikeDB__
#define __TrLikeDB__

#include "tkdcards.h"
#include "TkDBc.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrCharge.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TObject.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>


using namespace std;

class TrLikeDB : public TObject {

 private:
  
  //! Map for fast binary search
  map<int,TH1*> fTrPdfMap;
  //! Singleton
  static TrLikeDB* fHead;

 public:

  //! default value for log-probability (const) 
  static double default_logprob; 

 public:

  //! Constructor (declared public because of ROOT I/O)
  TrLikeDB() {}
  //! Destructor
  ~TrLikeDB() { Clean(); }
  //! Get self-pointer
  static TrLikeDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void   Init();
  //! Clean
  void   Clean();
  //! Print
  void   Print();   
  //! Load PDF
  bool   LoadPdfVer0(char* dirname = getenv("AMSDataDir"));
  //! Index used to store and retrieve PDFs (index = RSCZZT)
  // - type = 0: 2D X vs Y; 1: 1D X or Y only; 2: 1D Eta 
  // - Z = 1, ..., 28
  // - category/side = 0/0: K7 (or K7 vs S); 1/0: K5 (or K5 vs S); x/1: S  
  static int CreateIndex(int type, int Z, int icat, int iside, int irig) { return type + Z*10 + icat*1000 + iside*10000 + irig*100000; }
  //! Add or override a PDF
  void AddPdf(TH1* pdf, int index);
  //! Get the PDF by index 
  TH1* GetPdf(int index);
  //! Get the PDF 
  TH1* GetPdf(int type, int Z, int icat, int iside, int irig) { return GetPdf(CreateIndex(type,Z,icat,iside,irig)); }
  //! Get PDF value 
  static double Interpolate(TH1* pdf, double x);
  //! Get PDF value 
  static double Interpolate(TH1* pdf, double x, double y);

  using TObject::Print;
  ClassDef(TrLikeDB,1);
};


#endif

