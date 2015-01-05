#ifndef __TrPdfDB__
#define __TrPdfDB__

/*! 
 \file  TrPdfDB.h
 \brief Header file of TrPdfDB class

 \class TrPdfDB
 \brief The tracker PDF manager
 
 $Date$

 $Revision$
*/

#include "tkdcards.h"

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
  enum PdfType { 
    // ver01: pdf for single layers from MC [sqrt(ADC)] for x, y and xy
    kPdf01_SingleLayer   = 0, 
    // ver01: pdf for truncated mean from MC [sqrt(ADC)] for x
    kPdf01_TruncatedMean = 1,
    // ver02: pdf for single layers from data layer-by-layer [ADC/Z^2] for x
    kPdf02_SingleLayer   = 2
  };

  /////////////////////////
  // Accessors and management
  /////////////////////////

  //! Constructor (declared public because of ROOT I/O)
  TrPdfDB() {}
  //! Destructor
  ~TrPdfDB() { Clear(); }
  //! Get self-pointer
  static TrPdfDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Clear
  void   Clear(Option_t* option="");
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
  //! Get the pdf from the map entry number
  TrPdf* GetEntryPdf(int ii);
  //! Get the pdf index from map entry number
  int    GetEntryIndex(int ii);
  //! Get the pdf from Z, side and layer
  TrPdf* GetPdf(int Z, int iside, int itype, int layer = 0) { return GetPdf(GenerateIndex(Z,iside,itype,layer)); }
  //! Get the pdf by index 
  TrPdf* GetPdf(int index);
  //! Add or override a pdf 
  void   AddPdf(TrPdf* pdf, int Z, int iside, int itype, int layer = 0) { AddPdf(pdf,GenerateIndex(Z,iside,itype,layer)); } 
  //! Add or override a pdf 
  void   AddPdf(TrPdf* pdf, int index);
  //! Generate index
  /*  + Z (charge up to TRCHAFFKEY.MaxCharge) 
      + iside*100 (TrCharge::SideType) 
      + itype*1000 (0, ..., 9: PdfType) 
      + layer*10000 (0: no dependence from layer, from 1 to 9: layers)
  */
  int    GenerateIndex(int Z, int iside, int itype, int layer = 0) { return Z + iside*100 + itype*1000 + layer*10000; } 
  //! Get Z from index
  int    GetZ(int index) { return index%100; }
  //! Get side from index
  int    GetSide(int index) { return int(index/100)%10; }
  //! Get type from index
  int    GetType(int index) { return int(index/1000)%10; }
  //! Get layer from index
  int    GetLayer(int index) { return int(index/10000); }

  //////////////////////////
  /// Reading and loading
  //////////////////////////

  //! Load the Pierre's ROOT file with PDFs (extracted from MC)
  void   LoadPierrePdf01(char* filename);
  //! Load the Pierre's ROOT file with PDFs (extracted from MC)
  void   LoadPierrePdf01(TFile* file = 0);
  //! Load the layer-by-layer Pierre PDFs (from data, corrections from KSC)
  void   LoadPierrePdf02(); 

  //////////////////////////
  /// Service
  //////////////////////////

  //! Function used for Pdf02 fitting
  static double newlangauexpfun(double *x, double *par);
  //! Service function for the PDF management
  TH1D*  GetSliceY(TH2D* th2, int ixbin);

  //! ROOT stuff
  using TObject::Info;

  ClassDef(TrPdfDB,1);
};


#endif

