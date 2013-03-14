// $Id: TrEDepDB.h,v 1.1 2013/03/14 09:29:38 oliva Exp $

#ifndef __TrEDepDB__
#define __TrEDepDB__


#include "TkDBc.h"
#include "TkLadder.h"


#include "TObject.h"
#include "TFile.h"
#include "typedefs.h"
#include "TH2D.h"
#include "TMath.h"
#include "TGraph.h" 


#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>


//! \c TrEDepTable contains single charge table for the betagamma correction.
class TrEDepTable : public TObject {

 private:

  //! Index 
  int     fIndex;
  //! Number of EDep profiles in the table
  int     fNCharges;
  //! Number of parameters for EDep profile functional description
  int     fNPars;
  //! Charges array (fNCharges)
  int*    fZ;
  //! Parameters array (fNPars*fNCharges)
  double* fPars; 

 public:

  //! Constructor 
  TrEDepTable();
  //! Constructobinsr
  TrEDepTable(int index, int nch, int npar); 
  //! Default destructor
  ~TrEDepTable() { Clear(); }
  //! Clear
  void    Clear(Option_t* option = "");
  //! Clear and initialization
  void    Init(int index, int nch, int npar);
  //! Info
  void    Info(); 
  //! Is empty?
  bool    IsEmpty() { return ( (fNCharges==0)||(fNPars==0) ); }

  //! Get table index 
  int     GetIndex() { return fIndex; }
  //! Get number of EDep profiles in the table
  int     GetNCharges() { return fNCharges; }
  //! Get number of parameters for EDep profile functional description
  int     GetNPars() { return fNPars; } 
  //! Get i-th profile charge value
  int     GetZ(int ich);
  //! Get parameters of a specific profile
  double  GetPar(int ich, int ipar);

  //! Set i-th profile charge value
  bool    SetZ(int ich, int z);
  //! Set parameters of a specific profile
  bool    SetPar(int ich, int ipar, double par);

  //! Read table from a txt file
  bool    InitTableFromTxtFile(char* filename);

  //! Interpolate for a given Q, beta or beta/rigidity correction (if mass_on_Z <= 0 make an educated guess for mass_on_Z) [c.u.]
  double  GetCorrectedValue(double Q, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0);
  //! Get the Q for a give Z in case of beta or beta/rigidity parametrization [c.u.]
  double  GetQ(int Z, double beta, double rigidity = 0, double mass_on_Z = 0, int type = 0);
  //! Find the beta corresponding to Q along Z line
  double  FindBeta(int Z, double Q, int type = 0, double beta_min = 0.2, double beta_max = 0.94);

  //! Naive evaluation of mass over Z from Z (minimize the mass problem)
  static double GetNaiveMassOnZ(double Q);
  //! Calculate the log10(betagamma) used for fitting/correcting values  
  static double GetLogBetaGamma(double beta, double rigidity, double mass_on_Z, int type);

  using TObject::Info;
  ClassDef(TrEDepTable,1);
};


//! \c TrEDepDB contains all the charge loss informations.
class TrEDepDB : public TObject {

 private:
  
  //! Map for fast binary search
  static map<int,TrEDepTable*> fTrEDepMap;
  //! Singleton
  static TrEDepDB* fHead;
  //! Init done
  static bool fInitDone;

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrEDepDB() { Clear(); }
  //! Destructor
  ~TrEDepDB(); 
  //! Get self-pointer
  static TrEDepDB* GetHead();
  //! Existence checker
  static bool  IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void  Init(bool force = false);
  //! Clean up the table database
  void  Clear(Option_t* option = "");
  //! Info
  void  Info();
 
  //! Create index
  //! Create an index (index = ver*10000 + jlayer*10 + iside) 
  /* - iside  = 0: N; 1: P 
   * - jlayer = 1, ..., 9
   * - ver    = 1: ver1 calibration 
   */ 
  int   CreateIndex(int iside, int jlayer, int ver = 1) { return iside + jlayer*10 + ver*10000; }
  //! Add table to the database
  bool  AddTable(int index, TrEDepTable* table);
  //! Add table to the database by type and Z
  bool  AddTable(TrEDepTable* table, int iside, int jlayer, int ver = 1) { return AddTable(CreateIndex(iside,jlayer,ver),table); }
  //! Retrieve table 
  TrEDepTable* GetTable(int index);
  //! Retrieve table by type and Z
  TrEDepTable* GetTable(int iside, int jlayer, int ver = 1) { return GetTable(CreateIndex(iside,jlayer,ver)); }

  //! Get the corrected value
  double GetEDepCorrectedValue(int jlayer, double Q, double beta, double rigidity = 0, double mass_on_Z = 0, int iside = 0, int ver = 1);

  //! Top of the intrument correction (top of layer 1), by default use layer 1 and 5 Z=1 tables
  double GetTOIBeta(double beta, int Z = 1, int inn_jlayer = 5, int iside = 0, int ver = 1);

  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer1(char* dirname = getenv("AMSDataDir"));

  using TObject::Info;
  ClassDef(TrEDepDB,1);
};


//! line + plolynomial + line
double line_to_line_fun(double *x, double *par);
//! log line-to-line + exponential tail
double edep_correction_function(double *x, double *par);

#endif

