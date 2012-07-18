// $Id: TrGainDB.h,v 1.5 2012/07/18 09:40:56 oliva Exp $

#ifndef __TrGainDB__
#define __TrGainDB__


#include "TrParDB.h"
#include "TrLadPar.h"
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


//! \c TrGainLad contains gain informations of each one of the 16 VAs.
class TrLadGain : public TObject {

 private:

  //! HwId of the ladder
  int   fHwId;
  //! Gain 
  float fGain[16];
  //! Offset
  float fOffset[16];
  //! Systematic Error
  float fSysErr[16];
  //! Status  
  int   fStatus[16];

 public:

  //! Default constructor
  TrLadGain(int hwid = 0) { Clear(); SetHwId(hwid); }
  //! Constructor 
  TrLadGain(int hwid, float* gain, float* offset, float* syserr, int* status);
  //! Default destructor
  ~TrLadGain() {}
  //! Clear
  void Clear(Option_t* option = "");
  //! Info (nice format)
  void Info(int verbosity = 0);
  //! Dump (ntuple format)
  void Dump(int iva);
 
  //! Get Ladder HwId
  int   GetHwId() { return fHwId; }
  //! Get VA gain value
  float GetGain(int iva) { return ( (iva<0)||(iva>15) ) ? -1 : fGain[iva]; }
  //! Get VA offset value
  float GetOffset(int iva) { return ( (iva<0)||(iva>15) ) ? -1 : fOffset[iva]; }
  //! Get VA systematic error
  float GetSysErr(int iva) { return ( (iva<0)||(iva>15) ) ? -1 : fSysErr[iva]; }
  //! Get VA status
  /*  
   * VA classification coming from the gain calibration procedure:
   * - bit0: 0, good; 1, very bad quality (pathological)
   * - bit1: 0, good; 1, no statistics (dead, out of acceptance)
   * - bit2: 0, good; 1, bad quality (low amp contamination, strange distributions) 
   * - bit3: 0, good; 1, low gain
   * - bit4: 0, good; 1, low statistics for high-Z 
   */
  int  GetStatus(int iva) { return ( (iva<0)||(iva>15) ) ? -1 : fStatus[iva]; }
  //! Gold VA (high quality) 
  bool IsGold(int iva)    { return (GetStatus(iva)==0); }
  //! Silver VA (good quality)
  bool IsSilver(int iva)  { return ((GetStatus(iva)&0x1)==0); }

  //! Set Ladder HwId
  void  SetHwId(int hwid)                { fHwId = hwid; }
  //! Set all VAs gain values
  void  SetGain(float* gain)             { for (int iva=0; iva<16; iva++) fGain[iva] = gain[iva]; }
  //! Set a VA gain value
  void  SetGain(int iva, float gain)     { if ( (iva>=0)&&(iva<16) ) fGain[iva] = gain; } 
  //! Set all VAs offset values
  void  SetOffset(float* offset)         { for (int iva=0; iva<16; iva++) fOffset[iva] = offset[iva]; }
  //! Set a VA offset value
  void  SetOffset(int iva, float offset) { if ( (iva>=0)&&(iva<16) ) fOffset[iva] = offset; }
  //! Set all VAs systematic errors
  void  SetSysErr(float* syserr)         { for (int iva=0; iva<16; iva++) fSysErr[iva] = syserr[iva]; }
  //! Set a VA systematic errors
  void  SetSysErr(int iva, float syserr) { if ( (iva>=0)&&(iva<16) ) fSysErr[iva] = syserr; }
  //! Set all VAs statuses
  void  SetStatus(int* status)           { for (int iva=0; iva<16; iva++) fStatus[iva] = status[iva]; }
  //! Set a VA status
  void  SetStatus(int iva, int status)   { if ( (iva>=0)&&(iva<16) ) fStatus[iva] = status; }
  //! Set all VAs statuses (for database purposes)
  void  SetStatus(float* status)         { for (int iva=0; iva<16; iva++) fStatus[iva] = (int) status[iva]; }

  //! Size of the TDV entry 
  static int GetSize() { return 1+16+16+16+16; }
  //! Interface to TDV linear database
  bool GainDBToLinear(float* offset);
  //! Interface to TDV linear database
  bool LinearToGainDB(float* offset);

  //! Apply correction to a strip (very bad VA are excluded)
  float GetGainCorrected(float adc, int iva);

  using TObject::Dump;
  using TObject::Info;
  ClassDef(TrLadGain,1);
};


//! \c TrGainDB contains gain informations of each of the 192 AMS Ladders.
class TrGainDB : public TObject {

 private:
  
  //! Map for fast binary search
  map<int,TrLadGain*> fTrGainHwIdMap;

  //! Singleton
  static TrGainDB* fHead;

  //! Interface for TDV storing
  static float* fLinear; 

 public:

  //! Constructor (declared public because of ROOT I/O), not to be used
  TrGainDB() {}
  //! Destructor
  ~TrGainDB(); 
  //! Get self-pointer
  static TrGainDB* GetHead();
  //! Existence checker
  static bool IsNull() { return (fHead==0) ? true : false; } 
  //! Init the database 
  bool Init(); 
  //! Clear
  void Clear(Option_t* option = ""); 
  //! Info (nice format)
  void Info(int verbosity = 0);
  //! Dump (ntuple format)
  void Dump();

  //! Get the ladder gain element by HwId 
  TrLadGain* FindGainHwId(int hwid);
  //! Get the ladder gain element by TkId 
  TrLadGain* FindGainTkId(int tkid);

  //! Save the database into a ROOT file
  bool  Save(const char* filename);
  //! Load the database from a ROOT file (by filename)
  bool  Load(const char* filename);
  //! Load the database from a ROOT file (by TFile pointer)
  bool  Load(TFile* rootfile);

  //! Read from txt file (Pierre's format)
  bool  LoadFromTxtFile(const char* filename);
  //! Read from TrParDB (old database) in a ROOT file
  bool  LoadFromTrParDBFile(const char* filename);

  //! Linear database pointer
  static float* GetLinear() { return fLinear; }
  //! Size of the linear database 
  static int GetSize() { return 192*TrLadGain::GetSize(); }
  //! Size of the TDV entry (bytes)
  static int GetLinearSize() { return GetSize()*sizeof(float); }
  //! Interface to TDV linear database
  bool GainDBToLinear();
  //! Interface to TDV linear database
  bool LinearToGainDB();
  //! Dump the linear database
  void PrintLinear(); 

  //! Save in TDV 
  bool SaveInTDV(long int start_time, long int validity = 31536000, int isReal = 1);
  //! Load from TDV
  int  LoadFromTDV(long int time, int isReal = 1);

  //! Apply correction to a strip (very bad VA are excluded)
  float GetGainCorrected(float adc, int tkid, int iva); 

  //! Display gains as a 2D histogram 
  TH2D* GetGainHistogram();

  using TObject::Dump;
  using TObject::Info;
  ClassDef(TrGainDB,1);
};


//! TrGainDB iterator
typedef map<int,TrLadGain*>::const_iterator TrGainIt;

//! Function for the conversion (needed for the TDV unpack)
void FunctionLinearToGainDB();

#endif

