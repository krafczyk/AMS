// $Id: TrOccDB.h,v 1.1 2013/01/02 19:41:40 oliva Exp $

#ifndef __TrOccDB__
#define __TrOccDB__


#include "TrCalDB.h"
#include "TrLadCal.h"
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
#include <algorithm>
#include <ctime>


//! \c TrLadOcc contains occupancy informations of each ladder.
/*!
  \author alberto.oliva@cern.ch
*/
class TrLadOcc : public TObject {

 private:

  //! HwId of the ladder
  int   fHwId;
  //! Median values
  float fMedian[2];
  //! Normalized deviation 
  float fDeviation[2];
  //! Normalized occupancy 
  float fOccupancy[1024];

 private:

  //! Strips with occupancy below this value are declared dead
  static float MinimumNormalizedOccupancy;
  //! Strips with occupancy above this value are declared noisy
  static float MaximumNormalizedOccupancy;

 public:

  //! Default constructor
  TrLadOcc(int hwid = 0) { Clear(); SetHwId(hwid); }
  //! Constructor 
  TrLadOcc(int hwid, float* median, float* deviation, float* occupancy);
  //! Default destructor
  ~TrLadOcc() {}
  //! Clear
  void Clear(Option_t* option = "");
  //! Info
  void Info(int verbosity = 0);
 
  //! Get ladder HwId
  int   GetHwId()                 { return fHwId; }
  //! Get ladder occupancy median value
  float GetMedian(int iside)      { return ( (iside<0)||(iside>1) ) ? -1 : fMedian[iside]; }
  //! Get ladder normalized occupancy median deviation  
  float GetDeviation(int iside)   { return ( (iside<0)||(iside>1) ) ? -1 : fDeviation[iside]; }  
  //! Get strip normalized occupancy value
  float GetOccupancy(int address) { return ( (address<0)||(address>1023) ) ? -1 : fOccupancy[address]; }
  //! Get occupancy status (-1 if invalid address)
  /* - bit0: dead  (Occupancy<TrOccDB::MinimumNormalizedOccupancy) 
   * - bit1: noisy (Occupancy>TrOccDB::MinimumNormalizedOccupancy)
   */
  int   GetStatus(int address);
  //! Set Ladder HwId
  void  SetHwId(int hwid)                { fHwId = hwid; }
  //! Set ladder occupancy median value
  void  SetMedian(float* median)         { for (int iside=0; iside<2; iside++) fMedian[iside] = median[iside]; }
  //! Set ladder normalized occupancy median deviation 
  void  SetDeviation(float* deviation)   { for (int iside=0; iside<2; iside++) fDeviation[iside] = deviation[iside]; }
  //! Set all channels normalized occupancy values
  void  SetOccupancy(float* occupancy)   { for (int address=0; address<1024; address++) fOccupancy[address] = occupancy[address]; }

  //! Size of the TDV entry 
  static int GetSize() { return 1+2+2+1024; }

  //! Interface to TDV linear database
  bool OccDBToLinear(float* offset);
  //! Interface to TDV linear database
  bool LinearToOccDB(float* offset);

  using TObject::Info;
  ClassDef(TrLadOcc,1);
};


//! \c TrOccDB contains occupancy informations of each of the 192 AMS Ladders.
/*!
  \author alberto.oliva@cern.ch
*/
class TrOccDB : public TObject {

 private:

  //! Run number 
  static unsigned int fRun;  
  //! Map for fast binary search
  map<int,TrLadOcc*> fTrOccHwIdMap;
  //! Singleton
  static TrOccDB* fHead;
  //! Interface for TDV storing
  static float* fLinear; 

  //! Default value for bad occupancy
  static float BadOccupancyValue;

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrOccDB() {}
  //! Destructor
  ~TrOccDB(); 
  //! Get self-pointer
  static TrOccDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Init the database 
  bool Init(); 
  //! Clear
  void Clear(Option_t* option = ""); 
  //! Info 
  void Info(int verbosity = 0);

  //! Get run number
  unsigned int GetRun() { return fRun; }
  //! Set run number
  void SetRun(unsigned int run) { fRun = run; } 

  //! Get the ladder gain element by HwId 
  TrLadOcc* FindOccHwId(int hwid);
  //! Get the ladder gain element by TkId 
  TrLadOcc* FindOccTkId(int tkid);

  //! Save the database into a ROOT file
  bool  Save(const char* filename);
  //! Load the database from a ROOT file (by filename)
  bool  Load(const char* filename);
  //! Load the database from a ROOT file (by TFile pointer)
  bool  Load(TFile* rootfile);

  //! Create database from raw occupancies
  bool  CreateFromRawOccupanciesHisto(TH2F* histo, float norm = 1);
  //! Occupancy histogram map
  TH2D* GetOccupancyMapHisto();
  //! Occupancy histogram per ladder/side
  TH2D* GetOccupancyLadderHisto();
  //! Calibration comparison 
  void  PrintCalibrationComparisonTable();
  //! Get global number of bad strips 
  int   GetNBadStrips();

  //! Linear database pointer
  static float* GetLinear() { return fLinear; }
  //! Size of the linear database 
  static int GetSize() { return 1 + 192*TrLadOcc::GetSize(); }
  //! Size of the TDV entry (bytes)
  static int GetLinearSize() { return GetSize()*sizeof(float); }
  //! Interface to TDV linear database
  bool OccDBToLinear();
  //! Interface to TDV linear database
  bool LinearToOccDB();
  //! Dump the linear database
  void PrintLinear(); 

  //! Save in TDV 
  bool SaveInTDV(unsigned int start_time, unsigned int validity, unsigned int insert_time, int isReal);
  //! Load from TDV
  int  LoadFromTDV(unsigned int time, int isReal);

  using TObject::Info;
  ClassDef(TrOccDB,1);
};


//! TrOccDB iterator
typedef map<int,TrLadOcc*>::const_iterator TrOccIt;

//! Function for the conversion (needed for the TDV unpack)
void FunctionLinearToOccDB();

#endif

