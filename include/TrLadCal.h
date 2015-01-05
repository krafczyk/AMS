#ifndef __TrLadCal__
#define __TrLadCal__

//////////////////////////////////////////////////////////////////////////
///
///
///\class TrLadCal
///\brief Class used to store a Tracker Ladder Calibration
///\ingroup tkdbc
///
/// author: P.Zuccon -- INFN Perugia 20/11/2007
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 AO  Some methods are added 
///\date  2010/08/14 AO  New occupancy table added (not written in the LinearDB)
///
//////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "TObject.h"
#include "TH1F.h"

//! structure used internally to store the Calib data
typedef struct CaloutDSP{
  float  ped[1024];
  float  rsig[1024];
  float  sig[1024];
  unsigned short int occupancy[1024];
  unsigned short int occupgaus[1024];
  short int status[1024];
  float CNmean[16];
  float CNrms[16];
  short int dspver;
  float S1_lowthres;
  float S2_lowthres;
  float K_lowthres;
  float S1_highthres;
  float S2_highthres;
  float K_highthres;
  float sigrawthres;
  short int Power_failureS;
  short int Power_failureK;
  short int calnum;
  short int usedev;
  short int calstatus;
  short int tdrstatus;
} CaloutDSP;


class TrLadCal :public TObject {
  
public:  

  static int dead; // = 0x1 | 0x4 = 0x5;

  /// Hardware ID (HwId = iTDR + iCrate*100)
  short int HwId;
  /// Version DSP
  int dspver;
  /// low threshold parameter S1(0-319) (i.e. the parameter to recover the sigma value)
  float S1_lowthres;
  /// high threshold parameter S1(0-319) (i.e. the parameter to recover the sigma value) 
  float S1_highthres;
  /// low threshold parameter S2(320-639)( (i.e. the parameter to recover the sigma value)
  float S2_lowthres;
  /// high threshold parameter S2(320-639) (i.e. the parameter to recover the sigma value) 
  float S2_highthres;
  /// low threshold parameter K (i.e. the parameter to recover the sigma value)
  float K_lowthres;
  /// high threshold parameter K (i.e. the parameter to recover the sigma value) 
  float K_highthres;
  /// raw sigma parameter (i.e. the parameter to recover
  float sigrawthres;
  /// \brief The Calibration status
  /*
    0 calibration finished
    1 internal trigger is on
    2 error in step 1
    3 error in step 2
    4 error in step 3
    5 error in step 4
    6 internal trigger mode: an external trigger happened
    7 internal trigger mode: no event received
  */
  int calstatus;
  /// p-side Power Failure 
  short int Power_failureS;
  /// n-side Power Failure
  short int Power_failureK;

private:

  /// Database version 
  static int version;
  /// Pedestals  
  geant     _Pedestal[1024];
  /// Pedestals sigma
  geant     _SigmaRaw[1024];
  /// Common noise subtracted pedestal sigma
  geant     _Sigma[1024];
  /// Strip Status 
  /* 
     Bit  0 to 1 (0x0001): dead  channel (sigma raw < mediane sigma raw on VA/2)
     Bit  1 to 1 (0x0002): noisy channel (sigma raw > mediane sigma raw on VA*3)
     Bit  2 to 1 (0x0004): dead  channel (sigma < mediane sigma on ADC/2)
     Bit  3 to 1 (0x0008): noisy channel (sigma > mediane sigma on ADC*1.5)
     Bit  4 to 1 (0x0010): non gaussian channel (occupancy>?)
     Bit  9 to 1 (0x0100): double-pulsed calibration bad channel
     Bit 15 to 1 (0x8000): user defined bad region channel
  */
  short int _Status[1024];
  /// Occupancy table for double-trigger calibration step
  unsigned short int _Occupancy[1024]; 
  /// Occupancy table for non-gaussianity calibration step
  unsigned short int _OccupancyGaus[1024];
  /// Average CN value in CAL
  geant _CNmean[16];
  ///  CN RMS  value in CAL
  geant _CNrms[16];
  /// 1 if the ladder calibration is acquired, 0 if not
  short int _filled;

  /// Private getting method
  geant _getnum( geant *,int ii);
  /// Private getting method
  short int _getnum( short int *,int ii);
  /// Private getting method
  unsigned short int _getnum( unsigned short int *,int ii);

  /// Private setting method
  void _setnum( geant *,int ii, geant val);
  /// Private setting method
  void _setnum( short int *,int ii, short int val);
  
public:

  /// Default constructor
  TrLadCal(short int hwid=0);
  /// Copy constructor
  TrLadCal(const TrLadCal& orig);
  /// Default destructor
  ~TrLadCal(){}
  /// Clear data members
  void Clear(const Option_t*aa=0);
  /// Fill the data members by the DSP block (all members - sigma)
  void Fill_same(CaloutDSP* cc);
  /// Fill the data members by the DSP block (sigma from sigmalow)
  void Fill_old(CaloutDSP* cc);
  /// Fill the data members by the DSP block (sigma from sigma table) 
  void Fill_new(CaloutDSP* cc);

  /// Get the DSP code version
  int GetDSPCodeVersion()   { return dspver&0xFFFF; } 
  /// Get ii-th strip pedestal ADC value
  geant Pedestal(int ii)    { return short(int(_getnum(_Pedestal,ii)*8)&0xFFFF)/8.; } // Apply the formula also here for bkward comp.  
  /// Get ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  geant Sigma(int ii)       { return _getnum(_Sigma,ii);}
  /// Get ii-th strip sigma-pedestal ADC value
  geant SigmaRaw(int ii)    { return _getnum(_SigmaRaw,ii);}
  /// Get ii-th status 
  short int Status(int ii)  { return _getnum(_Status,ii);}
  /// Get ii-th occupancy value (double step calibration) 
  unsigned short int Occupancy(int ii) { return _getnum(_Occupancy,ii);}
  /// Get ii-th occupancy value (non-gaussian strips)  
  unsigned short int OccupancyGaus(int ii); 
  ///Get CN mean
  geant GetCNmean(int va){ return _CNmean[va];}
  ///Get CN rms
  geant GetCNrms(int va){ return _CNrms[va];}
  /// Get the HwId
  short int GetHwId() const   { return HwId; }
  /// Get ii-th strip pedestal ADC value
  geant GetPedestal(int ii)   { return Pedestal(ii); }
  /// Get ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  geant GetSigma(int ii);
  /// Get ii-th strip sigma-pedestal ADC value
  geant GetSigmaRaw(int ii)   { return _getnum(_SigmaRaw,ii); }
  /// Get ii-th status 
  unsigned short int GetStatus(int ii) { return (unsigned short int) _getnum(_Status,ii); }

  /// Set ii-th strip pedestal ADC value
  void SetPedestal(int ii,geant val)   { _setnum(_Pedestal,ii,val);}
  /// Set ii-th strip sigma (CN subtracted pedestal sigma) ADC value
  void SetSigma(int ii,geant val)      { _setnum(_Sigma,ii,val);}
  /// Set ii-th strip sigma-pedestal ADC value
  void SetSigmaRaw(int ii,geant val)   { _setnum(_SigmaRaw,ii,val);}
  /// Set ii-th status 
  void SetStatus(int ii,short int val) { _setnum(_Status,ii,val);}

  /// The ladder calibration is acquired?
  bool IsFilled()  { return _filled==1; }
  /// Set the ladder calibration acquisition status
  void SetFilled() { _filled = 1; } 

  /// Simulate the DSP mediane calculation 
  short int DSPSmallest(short int a[], int n, int k);
  /// Get the sigma raw on the mediane (on a VA) using the DSP code
  geant GetDSPSigmaRawOnMediane(int ii); 
  /// Get the sigma on the mediane (on an ADC) using the DSP code 
  geant GetDSPSigmaOnMediane(int ii); 

  /// Get the mean sigma (strips with status==0) for the p (1) and n (0) sides 
  geant GetSigmaMean(int side);
  /// Get the sigma RMS (strips with status==0) for the p (1) and n (0) sides  
  geant GetSigmaRMS(int side);
  /// Get the number of channels for a given status
  int   GetnChannelsByStatus(int statusbit); 

  /// Print info (long format if long_format>0)
  void PrintInfo(int long_format = 0);
  /// Copy the data part from another calibration
  void CopyCont(TrLadCal& orig);
  /// Operator - 
  TrLadCal& operator-(TrLadCal &);
  /// Operator =  
  TrLadCal& operator=(TrLadCal& orig);

  static  void SetVersion(int ver); 
  static int GetVersion(){return version;} 

  /// Additional noise in K side
  static float AddNoiseK[9];
  /// Additional noise in S side
  static float AddNoiseS[9];

  /// It copy the calibration data to a linear vector 
  ///  in memory starting at address offset
  int Cal2Lin(float* offset);

  /// It fill  the calibration from o a linear vector 
  ///  in memory starting at address offset
  int Lin2Cal(float* offset);

  TH1F* DrawOccupancy();
  TH1F* DrawStatus(unsigned short  mask=0xffff);

  static int GetSize() {
    if(version==1) return (1024*4+6);
    else return (1024*5+12+32);
  }
  
  ClassDef(TrLadCal,5);
};


#endif
