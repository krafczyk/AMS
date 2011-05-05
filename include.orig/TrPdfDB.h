#ifndef __TrPdfDB__
#define __TrPdfDB__

/*! 
 \file  TrPdfDB.h
 \brief Header file of TrPdfDB class

 \class TrPdfDB
 \brief The tracker PDF manager
 
 $Date: 2011/05/05 23:14:39 $

 $Revision: 1.2 $
*/

#include "TkDBc.h"
#include "TrPdf.h"

#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

class TrPdfDB {

 private:
  
  //! Map for fast binary search
  map<int,TrPdf*> fTrPdfMap;
  //! Singleton
  static TrPdfDB* fHead;
  //! Private contructor
  TrPdfDB() {}

 public:

  /////////////////////////
  // Enumerators
  /////////////////////////

  //! Available PDF types
  enum PdfType { kSingleLayer = 0, kTruncatedMean = 1 };

  /////////////////////////
  // Accessors 
  /////////////////////////
  
  //! Destructor
  ~TrPdfDB() { Clear(); }
  //! Get self-pointer
  static TrPdfDB* GetHead();
  //! Clear
  void   Clear();
  //! Get a list of available pdfs
  void   Info(); 

  //! Entries in the database 
  int    GetEntries() { return fTrPdfMap.size(); }
  //! Get a pdf by map index
  TrPdf* GetEntryPdf(int ii);
  //! Get the pdf index [Z (from 0 to 26) + side*100 (0/1/2 = x/y/xy) + type*1000 (0/1/... = single/truncated/...)]
  int    GetEntryIndex(int ii);

  //! Add or override a pdf 
  void   Add(TrPdf* pdf, int Z, int iside, int itype) { Add(pdf,GenerateIndex(Z,iside,itype)); } 
  //! Add or override a pdf 
  void   Add(TrPdf* pdf, int index);
  //! Get the pdf from Z and side
  TrPdf* Get(int Z, int iside, int itype) { return Get(GenerateIndex(Z,iside,itype)); } 
  //! Get the pdf by index (Z + side*100 + type*1000)
  TrPdf* Get(int index); 
  //! Generate index Z (from 0 to 26) + side*100 (0/1/2 = x/y/xy) + type*1000 (0/1/... = single/truncated/...)
  int    GenerateIndex(int Z, int iside, int itype) { return Z + iside*100 + itype*1000; } 
  //! Get Z from index
  int    GetZ(int index) { return index%100; }
  //! Get side from index
  int    GetSide(int index) { return int(index/100)%10; }
  //! Get type from index
  int    GetType(int index) { return int(index/1000); }

  //////////////////////////
  /// Reading 
  //////////////////////////

  /// Load the Pierre's ROOT file with PDFs
  void   LoadPierrePdf(TFile* file);

  //! Service function for the PDF management
  TH1D*  GetSliceY(TH2D* th2, int ixbin);

};

#endif

