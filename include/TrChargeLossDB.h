// $Id: TrChargeLossDB.h,v 1.1 2012/09/13 15:54:20 oliva Exp $

#ifndef __TrChargeLossDB__
#define __TrChargeLossDB__


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


//! \c TrChargeLossTable contains single charge table for the charge loss correction.
class TrChargeLossTable : public TObject {

 private:

  //! Table index: Type(0: n, 1: p, 2: n-special) + 10*Charge(1, ..., 26)
  int     fIndex;
  //! Number of impact point bins 
  int     fIPNBins;
  //! Min impact point
  double  fIPMin;
  //! Max impact point
  double  fIPMax;
  //! Number of impact angle bins
  int     fIANBins;
  //! Min impact angle
  double  fIAMin;
  //! Max impact angle
  double  fIAMax;
  //! MPV reference   
  double  fMPVRef;
  //! MPV values (ADC gain corrected)  
  double* fMPVTable;

 public:

  //! Constructor 
  TrChargeLossTable();
  //! Constructobinsr
  TrChargeLossTable(int type, int z, int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref);
  //! Default destructor
  ~TrChargeLossTable() { Clear(); }
  //! Clear
  void Clear(Option_t* option = "");
  //! Clear and initialization
  void Init(int type, int z, int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref);
  //! Info
  void Info(); 
 
  //! Get the table associated Z 
  int    GetZ()       { return int(fIndex/10); }
  //! Get the table type (0: n, 1:p, 2: n-special)
  int    GetType()    { return (fIndex%10); }
  //! Get the number of bins for impact point 
  int    GetIPNBins() { return fIPNBins; }
  //! Get the impact point minimum
  double GetIPMin()   { return fIPMin;   }
  //! Get the impact point maximum
  double GetIPMax()   { return fIPMax;   }
  //! Get the number of bins for IA
  int    GetIANBins() { return fIANBins; }
  //! Get the impact point minimum
  double GetIAMin()   { return fIAMin;   }
  //! Get the impact point maximum
  double GetIAMax()   { return fIAMax;   }
  //! Get the MPV reference  
  double GetMPVRef()  { return fMPVRef; } 
  //! Get MPV value on the table
  double GetMPVValue(int ipbin, int iabin);
  //! Get MPV value on the table (impact point folding, extrapolation on impact angle)
  double GetMPVValueWithExtrapolation(int ipbin, int iabin);

  //! Get the impact point bin for a given IP
  int    GetIPBin(double ip) { return int(0.5 + (ip - GetIPMin())*GetIPNBins()/(GetIPMax()-GetIPMin())); }
  //! Get the impact angle bin for a given IA
  int    GetIABin(double ia) { return int(0.5 + (ia - GetIAMin())*GetIANBins()/(GetIAMax()-GetIAMin())); }
  //! Get the impact point for a given bin
  double GetIP(int ipbin)   { return GetIPMin() + ipbin*(GetIPMax()-GetIPMin())/GetIPNBins(); }
  //! Get the impact angle for a given bin
  double GetIA(int iabin)   { return GetIAMin() + iabin*(GetIAMax()-GetIAMin())/GetIANBins(); }

  //! Get MPV value from interpolation
  double GetMPVValueBilinearExtrapolation(double ip, double ia);
  //! Get best MPV value evaluation for a given impact point and impact angle
  double GetMPVValue(double ip, double ia) { return GetMPVValueBilinearExtrapolation(ip,ia); }

  //! Set index 
  void   SetIndex(int type, int z) { fIndex = type + z*10; }  
  //! Set the MPV reference
  void   SetMPVRef(double mpvref) { fMPVRef = mpvref; }
  //! Set a single element of the matrix
  bool   SetMPVValue(int ipbin, int iabin, double mpv); 

  //! Multiply by a factor (testing purposes)
  void   Multiply(double factor); 

  //! Read table from a txt file
  bool   InitTableFromTxtFile(char* filename);

  //! Get an histogram of the table
  TH2D*  GetHistogram();
  //! Get an histogram of the table (extended)
  TH2D*  GetHistogramExtended();
  //! Get an histogram with interpolation
  TH2D*  GetHistogram(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax);

  using TObject::Info;
  ClassDef(TrChargeLossTable,1);
};


//! \c TrChargeLossDB contains all the charge loss informations.
class TrChargeLossDB : public TObject {

 private:
  
  //! Map for fast binary search
  static map<int,TrChargeLossTable*> fTrChargeLossMap;
  //! Singleton
  static TrChargeLossDB* fHead;

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrChargeLossDB() { Clear(); }
  //! Destructor
  ~TrChargeLossDB(); 
  //! Get self-pointer
  static TrChargeLossDB* GetHead();
  //! Existence checker
  static bool  IsNull() { return (fHead==0) ? true : false; } 
  //! Needed init (loading interface for GBATCH)  
  void  Init();
  //! Clean up the table database
  void  Clear(Option_t* option = "");
  //! Info
  void  Info();
  
  //! Add table to the database
  bool  AddTable(int index, TrChargeLossTable* table);
  //! Add table to the database by type and Z
  bool  AddTable(int type, int z, TrChargeLossTable* table) { return AddTable(type + z*10, table); }
  //! Retrieve table 
  TrChargeLossTable* GetTable(int index);
  //! Retrieve table by type and Z
  TrChargeLossTable* GetTable(int type, int z) { return GetTable(type + z*10); }

  //! Get the corrected value
  double GetChargeLossCorrectedValue(int type, double ip, double ia, double adc);

  //! Load the ver0 tables from directory 
  bool   LoadDefaultTablesVer0(char* dirname = getenv("AMSDataDir"));

  using TObject::Info;
  ClassDef(TrChargeLossDB,1);
};


#endif

