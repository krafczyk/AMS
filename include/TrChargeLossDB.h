// $Id$

#ifndef __TrChargeLossDB__
#define __TrChargeLossDB__


#include "TkDBc.h"
#include "TkLadder.h"


#include "TObject.h"
#include "TFile.h"
#include "typedefs.h"
#include "TH2D.h"
#include "TGraph.h"


#include <iostream>
#include <fstream>
#include <string>
#include <map>


//! \c TrChargeLossTable contains single charge table for the charge loss correction.
class TrChargeLossTable : public TObject {

 private:

  //! Table index: Type(0: n, 1: p, 2: n-special) + 10*Charge(1, ..., 26)
  int     fIndex;
  //! Number of impact poin bins 
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

  //! Bad value level;
  static double fBadValue; 
  
 public:

  //! Constructor 
  TrChargeLossTable();
  //! Constructobinsr
  TrChargeLossTable(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref);
  //! Default destructor
  ~TrChargeLossTable() { Clear(); }
  //! Clear
  void Clear(Option_t* option = "");
  //! Clear and initialization
  void Init(int ipnbins, double ipmin, double ipmax, int ianbins, double iamin, double iamax, double ref);
  //! Info
  void Info(); 

  //! Create type number from side and number of intermediate implanted strips
  static int CreateType(int iside, int inter, int ver);
  //! Create an index (version*1000 + z*10 + type)
  /* - type = 0: K; 1: S(=3S), 2: 1K(=1KB), 3: 0K, 4: 1KA
   * - z    = charge associated to the table
   * - ver  = 0: ver0, generic table, only K and S; 1 = ver1, more specific, different correction scheme
   */
  static int CreateIndex(int type, int z, int ver = 0) { return type + z*10 + ver*1000; }
  //! Get type from index 
  static int GetType(int index) { return index%10; }
  //! Get Z from index
  static int GetZ(int index) { return int(index/10)%100; }
  //! Get version from index  
  static int GetVersion(int index) { return int(index/1000)%10; } 
 
  //! Get the table index
  int    GetIndex()   { return fIndex; }
  //! Get the table type (0: n, 1:p, 2: n-special)
  int    GetType()    { return GetType(GetIndex()); }
  //! Get the table associated Z 
  int    GetZ()       { return GetZ(GetIndex()); } 
  //! Get version
  int    GetVersion() { return GetVersion(GetIndex()); } 
  //! Set index 
  void   SetIndex(int index) { fIndex = index; }
  //! Set index 
  void   SetIndex(int type, int z, int ver) { fIndex = CreateIndex(type,z,ver); }

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
  //! Get MPV value of the closest point on grid
  double GetMPVValueOnGrid(double ip, double ia);

  //! Get the impact point bin for a given IP
  int    GetIPBin(double ip) { return int(floor((ip - GetIPMin())*GetIPNBins()/(GetIPMax()-GetIPMin()))); }
  //! Get the impact angle bin for a given IA
  int    GetIABin(double ia) { return int(floor((ia - GetIAMin())*GetIANBins()/(GetIAMax()-GetIAMin()))); }

  //! Get the impact point for a given bin (bin center)
  double GetIP(int ipbin)   { return GetIPMin() + (0.5 + ipbin)*(GetIPMax()-GetIPMin())/GetIPNBins(); }
  //! Get the impact angle for a given bin (bin center)
  double GetIA(int iabin)   { return GetIAMin() + (0.5 + iabin)*(GetIAMax()-GetIAMin())/GetIANBins(); }

  //! Get MPV value from interpolation
  double GetMPVValueBilinearExtrapolation(double ip, double ia, bool old = false);
  //! Get best MPV value evaluation for a given impact point and impact angle
  double GetMPVValue(double ip, double ia) { return GetMPVValueBilinearExtrapolation(ip,ia,false); }
  //! Get best MPV value evaluation for a given impact point and impact angle (old version for backward comp.)
  double GetMPVValueOld(double ip, double ia) { return GetMPVValueBilinearExtrapolation(ip,ia,true); }
 
  //! Set the MPV reference
  void   SetMPVRef(double mpvref) { fMPVRef = mpvref; }
  //! Set a single element of the matrix
  bool   SetMPVValue(int ipbin, int iabin, double mpv); 

  //! Multiply by a factor (testing purposes)
  void   Multiply(double factor); 

  //! Read table from a txt file
  bool   InitTableFromTxtFile(char* filename);

  //! Fill gaps of the table (used only for the last table)
  void   FillGaps();

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
  //! Init done
  static bool fInitDone;

 public: 

  //! Default version
  static int fUsedVersion;
 
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
  void  Init(bool force = false);
  //! Clean up the table database
  void  Clear(Option_t* option = "");
  //! Info
  void  Info();
 
  //! Add table to the database
  bool   AddTable(int index, TrChargeLossTable* table);
  //! Add table to the database by type and Z
  bool   AddTable(TrChargeLossTable* table, int type, int z, int ver) { return AddTable(TrChargeLossTable::CreateIndex(type,z,ver),table); }
  //! Retrieve table 
  TrChargeLossTable* GetTable(int index);
  //! Retrieve table by type, Z and version
  TrChargeLossTable* GetTable(int type, int z, int ver) { return GetTable(TrChargeLossTable::CreateIndex(type,z,ver)); }

  //! Get the corrected value (old method used in ver0)
  double GetChargeLossCorrectedValue(int type, double ip, double ia, double adc);
  //! Alternative method for correcting the value (used by ver>0)
  double GetChargeLossCorrectedValue(int iside, int inter, double ip, double ia, double adc, int ver = -1);

  //! Load the ver0 tables from directory 
  bool   LoadDefaultTablesVer0(char* dirname = getenv("AMSDataDir"));
  //! Load the ver1 tables from directory 
  bool   LoadDefaultTablesVer1(char* dirname = getenv("AMSDataDir"));
  //! Load the ver2 tables (MC) from directory
  bool   LoadDefaultTablesVer2(char* dirname = getenv("AMSDataDir"));

  //! Get a graph with correction function for a given CofG and impact point
  TGraph* GetCorrectionGraph(int type, double ip, double ia, int ver);

  //! Fill gaps by interpolation 
  void    FillGaps(int type);
  //! Method used to fill gaps 
  double  GetMPVValueChargeInterpolation(int type, double ip, double ia, int z, int ver);

  using TObject::Info;
  ClassDef(TrChargeLossDB,1);
};


//! Monotonic cubic interpolation 
double monotonic_cubic_interpolation(double xx, int n, double* x, double* y);


#endif

