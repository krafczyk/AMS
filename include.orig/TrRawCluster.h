// $Id: TrRawCluster.h,v 1.4 2009/08/17 12:53:47 pzuccon Exp $ 
#ifndef __TrRawClusterR__
#define __TrRawClusterR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRawClusterR
///\brief A class implemnting the Tracker Raw Clusters
///\ingroup tkrec
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  Some comments and utils are added
///\date  2008/02/13 SH  ADC,sigma,status arrays are changed as vector
///\date  2008/02/16 AO  Renaming and new methods
///\date  2008/06/19 AO  Using TrCalDB instead of data members 
///
/// $Date: 2009/08/17 12:53:47 $
///
/// $Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////////
#include "TObject.h"
#include "TrElem.h"
#include "TrCalDB.h"
#include "TrLadCal.h"

#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>



class TrRawClusterR :public TrElem   {
 public:
  /// TkLadder ID (layer *100 + slot)*side
  short int _tkid;
  /// First strip number
  short int _address;
  /// Number of strips
  short int _nelem;
   
  /// ADC data array
  std::vector<float> _signal;
 protected:
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  int Status;
  
 public:
  /// Default constructor
  TrRawClusterR(void);
  /// Copy constructor
  TrRawClusterR(const TrRawClusterR& orig);
  /// Constructors with raw data
  TrRawClusterR(int tkid, int add, int nelem, short int *adc);
  TrRawClusterR(int tkid, int add, int nelem, float *adc);
  /// Destructor
  virtual ~TrRawClusterR(){Clear();}
  /// Clear
  void Clear();

  /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }

  /// Get ladder TkId identifier 
  int GetTkId()      const { return _tkid; }
  /// Get ladder HwId identifier 
  int GetHwId()      const{
    TkLadder* lad=TkDBc::Head->FindTkId(_tkid);
    if(lad) return lad->GetHwId();
    else return -1;
  }
  /// Get ladder plane identifier 
  int GetLayer()     const { return abs(_tkid/100); }
  /// Get ladder X side on plane identifier 
  int GetLayerSide() const { return (_tkid>0)?1:-1; }
  /// Get the slotidentifier 
  int GetSlot()      const { return (abs(_tkid)-GetLayer()*100)*GetLayerSide(); }
  /// Get cluster side (0 -> p, 1 -> n)
  int GetSide()      const { return(GetAddress()>639) ? 0 : 1; }
  /// Get the cluster first strip number  
  int GetAddress()   const { return _address; }
  /// Get i-th strip address
  int GetAddress(int ii)   { return GetAddress() + ii; } 
  /// Get the cluster strip multiplicity
  int GetNelem()     const { return _nelem; }
  //  /// Get the gain
  //   double GetGain()   const { return _gain; }
  /// Get i-th strip signal
  float GetSignal(int ii) const { return _signal.at(ii); }
  /// Same as GetNoise() 
  float GetSigma (int ii) { return GetNoise(ii); }
  /// Get i-th strip sigma (from calibration file)
  float GetNoise(int ii);   
  /// Get i-th signal to noise ratio
  float GetSN(int ii, const char* options = "AG") { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii)/GetNoise(ii); }

  /// Get i-th strip status (from calibration)
  short GetStatus(int ii);
  /*
    Status definition (thresholds...???):
    bit 0 set to 1: dead channel on the sigma raw base (pedestal subtraction)
    bit 1 set to 1: noisy channel on the sigma raw base (pedestal subtraction) 
    bit 2 set to 1: dead channel on the sigma base (pedestal and CN subtraction)
    bit 3 set to 1: noisy channel on the sigma base (pedestal and CN subtraction)
    bit 4 set to 1: non gaussian channel 
  */
  /// Check if the cluster contains strip for the side
  int CheckSide(int side) const { return (side == 0) ? (640 <= GetAddress()+GetNelem()) : (GetAddress() < 640); }

  /// Print information
  std::ostream& putout(std::ostream &ostr = std::cout) const;
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr, const TrRawClusterR &cls) { 
    return cls.putout(ostr);
  }
  /// Print raw cluster strip variables 
  void Print(int full=0);
  void Info(int full=0);

  /// Get the seed index in the cluster with re-clustering (<0 if wrong)
  int   GetSeedIndex(float thseed = 3.);
  /// Get the seed address with re-clustering (<0 if wrong)
  int   GetSeedAddress(float thseed = 3.);
  /// Get seed signal with re-clustering (<0 if wrong)
  float GetSeedSignal(float thseed = 3.);
  /// Get seed signal with re-clustering (<0 if wrong)
  float GetSeedNoise(float thseed = 3.);
  /// Get seed signal to noise ratio with re-clustering (<0 if wrong)
  float GetSeedSN(float thseed = 3.);
  /// Get the numeber of strips on the left of the seed strip with re-clustering (<0 if wrong)
  int   GetLeftLength(float thseed = 3.,float thneig = 1.);
  /// Get the number of strips on the right of the seed strip with re-clustering (<0 if wrong)
  int   GetRightLength(float thseed = 3.,float thneig = 1.);
  /// Get cluster length with re-clustering (<0 if wrong)
  int   GetLength(float thseed = 3.,float thneig = 1.);
  /// Get cluster amplitude with re-clustering (<0 if wrong)
  float GetTotSignal(float thseed = 3.,float thneig = 1.);
  float GetTotSignal2(float thseed = 3.,float thneig = 1.);
  /// Get the status of the n higher strips of the cluster with re-clustering (<0 if wrong)
  int   GetStatusnStrips(int nstrips, float thseed = 3., float thneig = 1.);
  /// Get cluster eta (center of gravity with the two higher strips)
  float GetEta(float thseed = 3.,float thneig = 1.);
  /// Get cluster address with eta
  float GetEtaAddress(float thseed = 3., float thneig = 1.);
  /// For compatibility with trigger lvl3
  integer lvl3format(int16 * adc, integer nmax,int lvl3dcard_par=1  ,integer matchedonly=0);

  static std::string sout;

  static void lvl3CompatibilityAddress(
      int16u address,
      integer & strip, integer & va, integer & side,  integer &drp){
    drp=(address>>10)&31;
    va=(address>>6)&15;
    strip=((address)&63)+(va%10)*64; 
    //     drp=address%100;
    //     strip=address/1000;
    //     va=strip/64;
    side=va>9 ? 0 : 1;

  //   printf("----------------->>>>>>   address  %d,  strip %d , va %d ,  side %d,  drp %d\n",
    //   address,	   strip,va, side,drp);
  }
  
 /// chek some bits into cluster status
  uinteger checkstatus(integer checker) const{return Status & checker;}
  /// Get cluster status
  uinteger getstatus() const{return Status;}
  /// Set cluster status
  void     setstatus(uinteger status){Status=Status | status;}
  /// Clear cluster status
  void     clearstatus(uinteger status){Status=Status & ~status;}

 
  /// ROOT definition
  ClassDef(TrRawClusterR, 1)
};





#endif

