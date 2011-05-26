#ifndef __TrPdfDB__
#define __TrPdfDB__

/*! 
 \file  TrPdfDB.h
 \brief Header file of TrPdfDB class

 \class TrPdfDB
 \brief The tracker PDF manager
 
 $Date: 2011/05/26 01:30:29 $

 $Revision: 1.4 $
*/

#include "TkDBc.h"
#include "TrPdf.h"

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

class TrPdfDB : public TObject {

 private:
  
  //! Map for fast binary search
  map<int,TrPdf*> fTrPdfMap;
  //! Singleton
  static TrPdfDB* fHead;

 public:

  /////////////////////////
  // Enumerators
  /////////////////////////

  //! Available PDF types
  enum PdfType { kSingleLayer = 0, kTruncatedMean = 1 };

  /////////////////////////
  // Accessors 
  /////////////////////////

  //! Constructor (declared public because of ROOT I/O)
  TrPdfDB() {}
  //! Destructor
  ~TrPdfDB() { Clear(); }
  //! Get self-pointer
  static TrPdfDB* GetHead();
  //! Clear
  void   Clear(Option_t* oo="");
  //! Get a list of available pdfs
  void   Info();
  //! Default setting for GBATCH (called in job.C) 
  void   LoadDefaults();
  //! Load the database from a ROOT file
  static void Load(char* filename);
  //! Load the database from a ROOT file
  static void Load(TFile* file);
  //! Load the database from a ROOT file
  static void ReadDB(char* filename) { Load(filename); }

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

  //! Load the Pierre's ROOT file with PDFs
  void   LoadPierrePdf(char* filename);
  //! Load the Pierre's ROOT file with PDFs
  void   LoadPierrePdf(TFile* file = 0);
  //! Service function for the PDF management
  TH1D*  GetSliceY(TH2D* th2, int ixbin);
  using TObject::Info;

  /// ROOT definition
  ClassDef(TrPdfDB,1);

};

#endif

