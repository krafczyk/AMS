// $Id: TrRawCluster.h,v 1.17 2013/02/17 16:32:18 mduranti Exp $ 
#ifndef __TrRawClusterR__
#define __TrRawClusterR__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrRawClusterR
///\brief A class implementing the Tracker Raw Clusters
///\ingroup tkrec
///
/// The TrRawCluster are just the translation of the tracker RAW data and 
/// they contains the clusters as produced by the TDRs DSP.
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  Some comments and utils are added
///\date  2008/02/13 SH  ADC,sigma,status arrays are changed as vector
///\date  2008/02/16 AO  Renaming and new methods
///\date  2008/06/19 AO  Using TrCalDB instead of data member
///\date  2009/08/16 PZ  General revision --  modified inheritance, clean up docs 
///
/// $Date: 2013/02/17 16:32:18 $
///
/// $Revision: 1.17 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TrElem.h"
#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TkDBc.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <algorithm>


class TrRawClusterR : public TrElem {

 protected:

  /// TkLadder ID (layer *100 + slot)*side[-1 or 1]
  short int _tkid;
  /// The cluster address word (as given by DSP code)
  short int _addressword;
  /// The cluster length word (as given by the DSP code)
  short int _lengthword;
  /// Cluster status word (for analysis purposes)
  int Status;
  /// ADC data array
  std::vector<float> _signal;

  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  /// load the std::string sout with the info for a future output
  void _PrepareOutput(int full=0);

 public:
/** @name   CONSTRUCTORS & C. */
  /**@{*/	
  //################    CONSTRUCTORS & C.   ################################
  /// Default constructor
  TrRawClusterR(void);
  /// Copy constructor
  TrRawClusterR(const TrRawClusterR& orig);
  /// Generic Constructor 
  TrRawClusterR(int tkid, int address, int nelem, float *adc);
  /// Constructor for real data
  TrRawClusterR(int tkid, int clsaddwrd, int clslenwrd, short int *adc);
  /// Destructor
  virtual ~TrRawClusterR() { Clear(); }
  /// Clears the content of the class
  void Clear();
 
/**@}*/	

  /// Returns the ladder TkId identifier (LayerNumber(OLD)*100 + SlotNumber)*SideSign[-1 or 1]
  int   GetTkId()      const { return _tkid; }
  /// Return the layer (J Numbering) to which the cluster belong
  int   GetLayerJ()     const { return TkDBc::Head->GetJFromLayer(abs(GetTkId()/100)); }
  /// Returns the ladder HwId identifier (CrateNumber*100 + TDRNumber)
  int   GetHwId()      const;
  /// Return +1 or -1 dependig on which side of the X plane is located the ladder holding the cluster
  int   GetLayerSide() const { return (GetTkId()>0) ? 1 : -1; }
  /// Returns  the slot identifier of the ladder holding the cluster 
  int   GetSlot()      const { return (abs(GetTkId())-GetLayer()*100)*GetLayerSide(); }
  /// Get ladder slot Side (0: negative X, 1: positive X)
  int   GetSlotSide()  const { return (_tkid>=0) ? 1 : 0; }
  /// Returns the silicon face on which the cluster is:
  /*! 
     - Value = 0: n-side aka K-side, X-coordinate 
     - Value = 1: p-side aka S-side, Y-coordinate
  */
  int   GetSide() { return (GetAddress()>639) ? 0 : 1; }
 
  /// Returns the strip number of the cluster first strip   
  int   GetAddress(){return _addressword&0x3ff;}
  /// Returns the address of the ii-th strip of the cluster
  int   GetAddress(int ii) { return GetAddress() + ii; } 
  /// Returns the Seed SN from DSP (if exists)
  float GetDSPSeedSN();
  /// Returns the cluster strip multiplicity
  int   GetNelem() { return _signal.size();}
  /// Returns the DSP common noise status bits (if exists)  
  /*!
    - Bit 0: common noise status from DSP of the i-nth VA containing the first address strip (0: good, 1: bad)
    - Bit 1: common noise status from DSP of the (i+1)-nth VA (0: good, 1: bad)
    - Bit 2: common noise status from DSP of the (i+2)-nth VA (0: good, 1: bad)
    - Bit 3: common noise status from DSP of the (i+3)-nth VA (0: good, 1: bad)
  */  
  int   GetCNStatus(){return (_addressword>>10)&0xf;}
  /// Returns the DSP common noise status bit of the ii-th VA (if exists) 
  /*! \param ii takes values from 0 to 3.
      \sa GetCNStatus()
  */
  int   GetCNStatus(int ii) { return (GetCNStatus()>=0) ? (GetCNStatus()>>ii)&1 : -1; }
  /// Returns the DSP power status bits (if exists)
  /*!
    - Bit 0: p-side power supply status from DSP (0: good, 1: bad)
    - Bit 1: n-side power supply status from DSP (0: good, 1: bad)
  */    
  int   GetPowerStatus(){return (_addressword>>14)&0x3;}
  /// Returns p-side DSP power status bits (if exists)
  int   GetPSidePowerStatus() { return (GetPowerStatus()>=0) ? (GetPowerStatus()>>0)&1 : -1; }
  /// Returns n-side DSP power status bits (if exists)
  int   GetNSidePowerStatus() { return (GetPowerStatus()>=0) ? (GetPowerStatus()>>1)&1 : -1; }

  /// Returns the signal of the ii-th strip of the cluster
  float GetSignal(int ii) const { return _signal.at(ii); }
  /// Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetSigma (int ii) { return GetNoise(ii); }
  ///  Returns the noise of the ii-th strip of the cluster (from calibration file)
  float GetNoise(int ii);   
  /// Returns the signal to noise ratio of the ii-th strip of the cluster (using calibration file)
  float GetSN(int ii) { return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii)/GetNoise(ii); }

  /// Returns the ii-th strip status (from calibration)
  short GetStatus(int ii);
  /// Check if the cluster is within the side boundary ( p ==> .lt. 640; n ==>  .ge. 640)
  int   CheckSide(int side) { return (side == 0) ? (640 <= GetAddress()+GetNelem()) : (GetAddress() < 640); }

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


  /// Return the layer (OLD Numbering) to which the cluster belong
  int   GetLayer()     const { return abs(GetTkId()/100); }


	
  /// Print information
  std::ostream& putout(std::ostream &ostr = std::cout);


  /// Print raw cluster strip variables 
  void   Print(int full=0);
  /// Return a string with some info (used for event display)
  const char*  Info( int fRef=0);
   
  /// ostream operator
  friend std::ostream &operator <<(std::ostream &ostr,  TrRawClusterR &cls) { 
    return cls.putout(ostr);
  } 
  /// Using this calibration file
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Get the current calibration database
  TrCalDB*    GetTrCalDB()    { return _trcaldb; }
  /// Get the current ladder calibration
  TrLadCal*   GetTrLadCal()   { return (GetTrCalDB()) ? TrCalDB::Head->FindCal_TkId(GetTkId()) : 0; } 

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
  ClassDef(TrRawClusterR, 2)
};





#endif

