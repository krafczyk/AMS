// $Id: TrRawCluster.h,v 1.6 2009/12/22 16:30:13 oliva Exp $ 
#ifndef __TrRawClusterR__
#define __TrRawClusterR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRawClusterR
///\brief A class implementing the Tracker Raw Clusters
///\ingroup tkrec
///
/// The Tr Raw Cluster are just the translation of the tracker RAW data and 
/// they contains the clusters as produced by the TDRs DSP.
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  Some comments and utils are added
///\date  2008/02/13 SH  ADC,sigma,status arrays are changed as vector
///\date  2008/02/16 AO  Renaming and new methods
///\date  2008/06/19 AO  Using TrCalDB instead of data member
///\date  2009/08/16 PZ  General revision --  modified inheritance, clean up docs 
///
/// $Date: 2009/12/22 16:30:13 $
///
/// $Revision: 1.6 $
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
 protected:
  /// TkLadder ID (layer *100 + slot)*side[-1 or 1]
  short int _tkid;
  /// First strip number
  short int _address;
  /// Number of strips
  short int _nelem;
  /// Cluster Status Word
  int Status;
  /// ADC data array
  std::vector<float> _signal;

  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  /// load the std::string sout with the info for a future output
  void _PrepareOutput(int full=0);

  
 public:
  /// Default constructor
  TrRawClusterR(void);
  /// Copy constructor
  TrRawClusterR(const TrRawClusterR& orig);
  /// Constructor with from data
  TrRawClusterR(int tkid, int address, int nelem, short int *adc);
  /// Constructor with from data
  TrRawClusterR(int tkid, int address, int nelem, float *adc);
  /// Destructor
  virtual ~TrRawClusterR(){Clear();}
  /// Clears the content of the class
  void Clear();


  /// Returns the ladder TkId identifier (layer *100 + slot)*side[-1 or 1]
  int GetTkId()      const { return _tkid; }
  /// Returns the ladder HwId identifier (Crate# * 100 + TDR# )
  int GetHwId()      const{
    TkLadder* lad=TkDBc::Head->FindTkId(_tkid);
    if(lad) return lad->GetHwId();
    else return -1;
  }
  /// Return the layer to which the cluster belong
  int GetLayer()     const { return abs(_tkid/100); }
  /// Return +1 or -1 dependig on which side of the X plane is located the ladder holding the cluster
  int GetLayerSide() const { return (_tkid>0)?1:-1; }
  /// Returns  the slot identifier of the ladder holding the cluster 
  int GetSlot()      const { return (abs(_tkid)-GetLayer()*100)*GetLayerSide(); }
  /// Returns the silicon face on which the cluster is (0 -> p, 1 -> n)
  int GetSide()      const { return(GetAddress()>639) ? 0 : 1; }
 
  /// Returns the strip number of the cluster first strip   
  int GetAddress()   const { return _address&0x3ff; }
  /// Returns the address of the ii-th strip of the cluster
  int GetAddress(int ii)   { return GetAddress() + ii; } 
  /// Returns the Seed SN from DAQ
  int GetDAQSeedSN()   const { return (_nelem>>7)&0x3ff; }
  /// Returns the cluster strip multiplicity
  int GetNelem()     const { return (_nelem&0x7f)+1; }
  /// Returns the CN status bits
  int GetCNStatus() const { return (_address>>10)&0xf;}
  /// Returns the power status bits
  int GetPowerStatus() const { return (_address>>14)&0x3;}
  //  /// Get the gain
  //   double GetGain()   const { return _gain; }
  /// Returns the signal of the ii-th strip of the cluster
  float GetSignal(int ii) const { return _signal.at(ii); }
  /// Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetSigma (int ii) { return GetNoise(ii); }
  ///  Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetNoise(int ii);   
  /// Returns the signal to noise ratio of the ii-th strip of the cluster (using calibration file)
  float GetSN(int ii, const char* options = "AG") { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii)/GetNoise(ii); }

  /// \brief Returns the  ii-th strip status (from calibration)
  /*!
    Status definition (thresholds...???):
    bit 0 set to 1: dead channel on the sigma raw base (pedestal subtraction)
    bit 1 set to 1: noisy channel on the sigma raw base (pedestal subtraction) 
    bit 2 set to 1: dead channel on the sigma base (pedestal and CN subtraction)
    bit 3 set to 1: noisy channel on the sigma base (pedestal and CN subtraction)
    bit 4 set to 1: non gaussian channel 
  */
  short GetStatus(int ii);
  /// Check if the cluster is within the side boundary ( p ==> .lt. 640; n ==>  .ge. 640)
  int CheckSide(int side) const { return (side == 0) ? (640 <= GetAddress()+GetNelem()) : (GetAddress() < 640); }

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

	
  /// Print information
  std::ostream& putout(std::ostream &ostr = std::cout);


  /// Print raw cluster strip variables 
  void   Print(int full=0);
  /// Return a string with some info (used for event display)
  char*  Info( int fRef=0);
   
  /// ostream operator
  friend std::ostream &operator <<(std::ostream &ostr,  TrRawClusterR &cls) { 
    return cls.putout(ostr);
  } 
  /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB() { return _trcaldb; }
  
  static void lvl3CompatibilityAddress(
				       int16u address,
				       integer & strip, integer & va, integer & side,  integer &drp){
    drp=(address>>10)&31;
    va=(address>>6)&15;
    strip=((address)&63)+(va%10)*64; 
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

