// $Id: TrRawCluster.h,v 1.7 2009/12/28 15:58:24 oliva Exp $ 
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
/// $Date: 2009/12/28 15:58:24 $
///
/// $Revision: 1.7 $
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
  /// Signal over Noise Ratio (from DSP code) 
  float     _seedsn;
  /// Cluster status word (from DSP code)    
  short int _status;
  /// Cluster status word (for reconstruction and analysis purposes)
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
  /// Generic Constructor 
  TrRawClusterR(short int tkid, short int address, short int nelem, float *adc);
  /// Constructor from real data
  TrRawClusterR(short int tkid, short int rawadd, short int rawnelem, short int *adc);
  /// Destructor
  virtual ~TrRawClusterR() { Clear(); }
  /// Clears the content of the class
  void Clear();


  /// Returns the ladder TkId identifier (LayerNumber*100 + SlotNumber)*SideSign[-1 or 1]
  int GetTkId()      const { return _tkid; }
  /// Returns the ladder HwId identifier (CrateNumber*100 + TDRNumber)
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
  int GetSide()      const { return (GetAddress()>639) ? 0 : 1; }
 
  /// Returns the strip number of the cluster first strip   
  int GetAddress()   const { return _address; }
  /// Returns the address of the ii-th strip of the cluster
  int GetAddress(int ii)   { return GetAddress() + ii; } 
  /// Returns the Seed SN from DAQ
  float GetDSPSeedSN() const { return _seedsn; }
  /// Returns the cluster strip multiplicity
  int GetNelem()     const { return _nelem; }
  /// Returns the DSP cluster status word 
  /*!
    - Bit 0: common noise status from DSP of the i-nth VA containing the first address strip (0: good, 1: bad)
    - Bit 1: common noise status from DSP of the (i+1)-th VA 
    - Bit 2: common noise status from DSP of the (i+2)-th VA 
    - Bit 3: common noise status from DSP of the (i+3)-th VA 
    - Bit 4: p-side power supply status from DSP (0: good, 1: bad)
    - Bit 5: n-side power supply status from DSP (0: good, 1: bad)
  */  
  int GetDSPStatus() const { return _status; }
  /// Returns the DSP cluster status word 
  int GetDSPStatus(int ii) const { return (_status>>ii)&1; }
  /// Set the DSP cluster status word 
  int SetDSPStatus(int ii) const { return _status|(1<<ii); }
  /// Returns the DSP common noise status bits
  int GetCNStatus() const { return _status&0xf;}
  /// Returns the DSP common noise status bit of the ii-th VA 
  /*! Starting from the VA which belongs the address strip,
      ii takes values from 0 to 3.
  */
  int GetCNStatus(int ii) const { return GetDSPStatus(ii); }
  /// Returns the DSP power status bits
  int GetPowerStatus() const { return (_status>>4)&0x3;}
  /// Returns p-side DSP power status bits
  int GetPSidePowerStatus() const { return GetDSPStatus(4); }
  /// Returns n-side DSP power status bits
  int GetNSidePowerStatus() const { return GetDSPStatus(5); }

  /// Returns the signal of the ii-th strip of the cluster
  float GetSignal(int ii) const { return _signal.at(ii); }
  /// Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetSigma (int ii) { return GetNoise(ii); }
  ///  Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetNoise(int ii);   
  /// Returns the signal to noise ratio of the ii-th strip of the cluster (using calibration file)
  float GetSN(int ii, const char* options = "AG") { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii)/GetNoise(ii); }

  /// Returns the ii-th strip status (from calibration)
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

