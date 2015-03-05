#ifndef __TrClusterR__
#define __TrClusterR__

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrRawCluster.h"
#include "TrElem.h"
#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TrParDB.h" 
#include "TrLadPar.h" 
#include "TrGainDB.h"
#include "TrChargeLossDB.h"
#include "TrOccDB.h"
#include "TrEDepDB.h"
#include "TrMipDB.h"
#include "TrLinearDB.h"

#include "edep.h"
#include "amsdbc.h"

#include "TSpline.h"

#include <cmath>
#include <vector>
#include <string>

/*!
 \class TrClusterR
 \brief A class to manage reconstructed cluster in AMS Tracker
 \ingroup tkrec

 TrClusterR, i.e. Tracker reconstructed cluster, is the core of the Tracker reconstruction. 
 This class gives accessors and memebers to retrieve all the reconstructed cluster 
 properties: signal (data members), calibration parameters (via TrCalDB), gains (via TrGainDB),
 charge corrections (via TrChargeLossDB) and coordinates (via TkCoo). 

 $Date$

 $Revision$

*/


class TrClusterR :public TrElem{

 public:
  
  enum CorrectionOptions {
    /// No Correction Applied
    kNoCorr       =     0x0,       
    /// Signal Corr.: Cluster Asymmetry Correction (left/right)
    kAsym         =     0x1, 
    /// Signal Corr.: P-Strip Correction 
    kPStrip       =     0x2,
    /// Total Signal Corr.: Energy Loss Normalization at 300 um [cos(Theta)^-1]
    kAngle        =     0x4,
    /// Total Signal Corr.: Gain Correction
    kGain         =     0x8, 
    /// Total Signal Corr.: Charge Loss Correction 
    kLoss         =    0x10,
    /// Total Signal Corr.: Charge Loss Correction (alternative to kLoss)
    kLoss2        =    0x20, 
    /// Total Signal Corr.: Normalization to P-Side (probably not working, however not really needed)    
    kPN           =    0x40,
    /// Total Signal Corr.: Normalization to number of MIP
    kMIP          =    0x80,
    /// Total Signal Corr.: Multiply by 300 um MIP energy deposition (estimated to be 81 keV)
    kMeV          =   0x100,
    /// Total Signal Corr.: Beta correction 
    kBeta         =   0x200,
    /// Total Signal Corr.: Rigidity correction 
    kRigidity     =   0x400,
    /// Coordinate Corr.: Flip the eta used strips
    kFlip         =   0x800,
    /// Coordinate Corr.: Correct for the charge coupling (4%) 
    kCoupl        =  0x1000,
    /// Coordinate Corr.: Belau correction 
    kBelau        =  0x2000,
    /// Use old charge calibration 
    kOld          =  0x4000
  };

  enum { TASCLS = 0x400 };
 
 protected:
  
  /// TkLadder ID (layer *100 + slot)*side 
  short int    _tkid;
  /// First strip address (0-639 for p-side, 640-1023 for n-side)
  short int    _address;
  /// Number of strips
  short int    _nelem;
  /// Seed index in the cluster (without correction, 0<=_seedind<_nelem)
  short int    _seedind;
  /// ADC data array
  std::vector<float> _signal;
  /// tan(ThetaXZ) of the incoming track (used for optimization of the track fitting)
  float        _dxdz;
  /// tan(ThetaYZ) of the incoming track (used for optimization of the track fitting)
  float        _dydz;
  /// charge of the incoming track (used for optimization of the track fitting)
  float        _qtrk;
  /// Multiplicity (on p side should be 1 on n side it is ladder dependent)
  int8         _mult;   
  /// Cluster status 
  unsigned int Status;

 protected:
  
  /// Pointer to the calibration database
  static TrCalDB* _trcaldb;
  /// Pointer to the parameters database
  static TrParDB* _trpardb;
  /// load the std::string sout with the info for a future output
  std::string _PrepareOutput(int full=0);

  /// Conversion between sqrt(ADC) and number of MIPs
  static TSpline3* sqrtadc_to_sqrtmip_spline[2];
  /// Conversion between "almost" MIPs and MIPs (refinement of the previous) 
  static TSpline3* sqrtref_to_sqrtmip_spline[2][9];
 
 public:

  /// Cluster asymmetry (strip cross talk)
  static float Asymmetry[2];
  /// Silicon intrinsic corrections
  static int DefaultCorrOpt;
  /// Corrections for coordinate calculation considering non-linearity on Y
  static int DefaultLinearityCorrOpt;
  /// Best set of options for coordinate determination
  static int DefaultBestResidualOpt;
  /// Normalization up to Z^2 scale 
  static int DefaultChargeCorrOpt;
  /// Energy deposition corrections      
  static int DefaultEdepCorrOpt;
  /// Default number of strips used for CofG
  static int DefaultUsedStrips;
  /// _dxdz threshold for 2/3-steip CofG
  static float TwoStripThresholdX;
  /// _dydz threshold for 2/3-steip CofG
  static float TwoStripThresholdY;

 public:

  /** @name   CONSTRUCTORS & C. */
  /**@{*/	

  /// Default constructor 
  TrClusterR(void);
  /// Constructor with data
  TrClusterR(int tkid, int side, int add, int nelem, int seedind, float* adc, unsigned int status);
  /// Constructor divided is several instructions 
  TrClusterR(int tkid, int side, int add, int seedind, unsigned int status);
  
  /// Copy constructor
  TrClusterR(const TrClusterR& orig);
  /// Destructor
  virtual ~TrClusterR();
  /// Clear
  void Clear();

  /**@}*/


  /** @name   Cluster Structure  */
  /**@{*/	

  /// Get ladder TkId identifier 
  int   GetTkId()          const { return _tkid; }
  /// Get ladder layer J number scheme
  int   GetLayerJ()        const { return TkDBc::Head->GetJFromLayer(abs(_tkid/100)); }
  /// Is a K7 cluster?
  int   IsK7()             const { return ( (TkDBc::Head->FindTkId(GetTkId())->IsK7())&&(GetSide()==0) ); } 
  /// Get ladder slot
  int   GetSlot()          const { return abs(_tkid%100); }
  /// Get ladder slot Side (0: negative X, 1: positive X)
  int   GetSlotSide()      const { return (_tkid>=0) ? 1 : 0; }
  /// Returns the silicon face on which the cluster is:
  /*! 
    - Value = 0: n-side aka K-side, X-coordinate 
    - Value = 1: p-side aka S-side, Y-coordinate
  */
  int   GetSide()          const { return (_address<640) ? 1 : 0; }
  /// Get the cluster first strip number   
  int   GetAddress()       const { return _address; }
  /// Get i-th strip address
  int   GetAddress(int ii);
  /// Get the i-th strip address on sensor (S: 0-639, K5: 0-191, K7: 0-223), if wrong gives back -1
  int   GetSensorAddress(int& sens, int ii = 0, int mult = 0, int verbose = 1);
  /// Get the local coordinate for i-th strip
  float GetX(int ii, int imult = 0) { 
    return TkCoo::GetLocalCoo(GetTkId(),GetAddress(ii),imult); 
  }
  /// Get the cluster strip multiplicity
  int   GetNelem()         const { return _nelem; }
  /// Get the cluster strip multiplicity
  int   GetLength()              { return GetNelem(); }

  /// Get the seed index 
  int   GetSeedIndex(int opt = DefaultCorrOpt);
  /// Get the seed address 
  int   GetSeedAddress(int opt = DefaultCorrOpt) { return GetAddress(GetSeedIndex(opt)); }
  /// Get seed signal 
  float GetSeedSignal(int opt = DefaultCorrOpt)  { return GetSignal(GetSeedIndex(opt),opt); }
  /// Get seed noise 
  float GetSeedNoise(int opt = DefaultCorrOpt)   { return GetNoise(GetSeedIndex(opt)); }
  /// Get seed signal to noise ratio 
  float GetSeedSN(int opt = DefaultCorrOpt)      { return GetSN(GetSeedIndex(opt)); }
  /// Get the numeber of strips on the left of the seed strip
  int   GetLeftLength(int opt = DefaultCorrOpt)  { return GetSeedIndex(opt); }
  /// Get the number of strips on the right of the seed strip
  int   GetRightLength(int opt = DefaultCorrOpt) { return GetNelem() - GetSeedIndex(opt) - 1; } 
  /// Get i-th strip status (from calibration)
  short GetStatus(int ii);

  /// Get the strip cross talk factor 
  static float GetAsymmetry(int iside) { return ( (iside>=0)&&(iside<=1) ) ? Asymmetry[iside] : 0; }
	
  /// Is a TAS cluster? (check the status bit)
  bool  TasCls() const { return checkstatus(TASCLS); }
  /// Used for track AMSDBc::USED = 32; (0x0020)
  bool  Used() const { return checkstatus(AMSDBc::USED); }	

  /// Check cluster consistency 
  bool Check(int verbosity=0);	
  /// Check if a K7 cluster is well shaped (it can happen that is not)
  bool CheckK7(int mult = -1, int verbosity=0);

  /**@}*/


  /** @name   SIGNALS & AMPLITUDE */
  /**@{*/	

  /// Get cluster amplitude, with corrections
  float         GetTotSignal(int opt = DefaultCorrOpt, float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult = -1);

  /// Convert an ADC signal to the ADC scale of p-side
  float         ConvertToPSideScale(float adc/*n-side*/);
  /// Convert an ADC signal to the ADC scale of n-side 
  float         ConvertToNSideScale(float adc/*p-side*/);
  /// Conversion between ADC and MIPs 
  float         GetNumberOfMIPs(float adc) { return GetNumberOfMIPs_ISS(adc); }
  /// Conversion between ADC and MIPs (derived from 2003 Ion Test Beam data)
  float         GetNumberOfMIPs_TB_2003(float adc);
  /// Conversion between ADC and MIPs (derived from ISS data)
  float         GetNumberOfMIPs_ISS(float adc);
  /// Beta correction 
  float         BetaCorrection(float beta) { return BetaCorrection_ISS_2011(beta); } 
  /// Beta correction (estimated with on ground Muons, 2010)
  float         BetaCorrection_Muons_2010(float beta);
  /// Beta correction (derived from ISS proton data 2011)
  float         BetaCorrection_ISS_2011(float beta);
  /// Rigidity correction (estimated with ISS data), dependent on the A/Z guess
  float         RigidityCorrection(float rigidity, float mass_on_Z = 0.938); 
  /// Beta and rigidity correction (best ranges selected), dependent on the A/Z guess
  float         BetaRigidityCorrection(float beta, float rigidity, float mass_on_z = 0.938);

  /// Get energy deposition (MeV)
  float GetEdep() { return GetTotSignal(TrClusterR::DefaultEdepCorrOpt); }
  /// Get floating charge estimation
  float GetQ(float beta = 1, float rigidity = 0, float mass_on_Z = 0, int res_mult = -1) { 
    return sqrt(GetTotSignal(TrClusterR::DefaultChargeCorrOpt,beta,rigidity,mass_on_Z,res_mult)); 
  }

  /// Get i-th strip signal
  float GetSignal(int ii, int opt = DefaultCorrOpt);
  /// Get i-th strip noise (from calibration)
  float GetSigma(int ii) { return GetNoise(ii);  }
  /// Get i-th strip noise (from calibration)
  float GetNoise(int ii);   
  /// Get i-th signal-to-noise ratio 
  float GetSN(int ii, int opt = DefaultCorrOpt) { 
    return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii,opt)/GetNoise(ii); 
  }
  /// Get the signal-to-noise ratio of the cluster (sum-signals/sqrt(sum-sigma^2))
  float GetClusterSN(int opt = 0);

  /**@}*/	


  /** @name  Coordinates */
  /**@{*/	
	
  /// Get multiplicity
  int   GetMultiplicity();
  /// Get local coordinate by multiplicity index
  float GetCoord(int imult) { return GetXCofG(DefaultUsedStrips,imult); }
  /// Get global coordinate by multiplicity index
  float GetGCoord(int imult);

  /// Get cluster bounds for a given number of strips (gerarchic order)  
  void  GetBoundsAsym(int &leftindex, int &rightindex, int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get cluster bounds for a given number of strips (symmetric order)  
  void  GetBoundsSymm(int &leftindex, int &rightindex, int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get the Center of Gravity with the n highest consecutive strips 
  float GetCofG(int nstrips = DefaultUsedStrips, int opt = DefaultBestResidualOpt);
  /// Get local coordinate with center of gravity on nstrips 
  float GetXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt);
  /// Get local coordinate with center of gravity on nstrips 
  float GetXCofG_old(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultBestResidualOpt) { 
    return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetCofG(nstrips,opt),imult); 
  }  
  /// Get eta (center of gravity with the two higher strips) 
  float GetEta(int opt = DefaultBestResidualOpt);
  /// Get eta (center of gravity with the two higher strips) by CofG algorythm  
  float GetEta_CofG(int opt = DefaultBestResidualOpt) { float eta = GetCofG(2,opt); return (eta>0.) ? eta : eta + 1.; }

  /// Set linearity correction (brute force method)
  static void SetLinearityCorrection() { DefaultCorrOpt = kAsym|kAngle|kGain|kPStrip; DefaultBestResidualOpt = kAsym|kAngle|kGain|kPStrip; }
  /// Unset linearity correction (brute force method)
  static void UnsetLinearityCorrection() { DefaultCorrOpt = kAsym|kAngle; DefaultBestResidualOpt = -1; } 

  /**@}*/ 


  /** @name Reconstruction & Special Methods (Experts only) */
  /**@{*/	

  /// Get ladder layer OLD Numbering
  int   GetLayer()         const { return abs(_tkid/100); }

  /// Insert a strip in the cluster
  void  push_back(float adc);

  /// Set track interpolation angle tan(thetaXZ) (rad)
  inline void  SetDxDz(float dxdz) { _dxdz = dxdz; }
  /// Set track interpolation angle tan(thetaYZ) (rad)
  inline void  SetDyDz(float dydz) { _dydz = dydz; }
  /// Get track interpolation angle tan(thetaXZ) (rad)
  inline float GetDxDz()  { return _dxdz; }
  /// Get track interpolation angle tan(thetaYZ) (rad)
  inline float GetDyDz()  { return _dydz; }
  /// Get track impact angle (degrees) 
  inline float GetImpactAngle() { return (GetSide()==0) ? atan(_dxdz)*180./3.14159265 : atan(_dydz)*180./3.14159265; }
  /// Get the zenith angle cosine
  inline float GetCosTheta() { return sqrt(1./(1.+GetDxDz()*GetDxDz()+GetDyDz()*GetDyDz())); } 

  /// Get track charge (used for optimization of the track fitting) 
  float GetQtrk() { return _qtrk; }
  /// Set track charge (used for optimization of the track fitting)  
  void SetQtrk(float q) { _qtrk = q; }

  /// chek some bits into cluster status
  uinteger checkstatus(integer checker) const { return Status & checker; }
  /// Get cluster status
  uinteger getstatus() const            { return Status; }
  /// Set cluster status
  void     setstatus(uinteger status)   { Status = Status | status; }
  /// Clear cluster status
  void     clearstatus(uinteger status) { Status = Status & ~status; }
  /// Set as used
  void     SetUsed()    { setstatus(AMSDBc::USED); }
  /// Clear used status
  void     ClearUsed()  { clearstatus(AMSDBc::USED); }
  /// change back and forth the flip status
  void ChangeFlipStatus(){
    if (checkstatus(kFlip))
      clearstatus(kFlip);
    else 
      setstatus(kFlip);
  }
     
      
  /// Get the current parameter database
  TrParDB* GetTrParDB() { return _trpardb; }
  /// Get the current calibration database
  TrCalDB* GetTrCalDB() { return _trcaldb; }

  /**@}*/	


  /// Using this calibration database
  static void UsingTrCalDB(TrCalDB* trcaldb) { _trcaldb = trcaldb; }
  /// Using this parameter database
  static void UsingTrParDB(TrParDB* trpardb) { _trpardb = trpardb; }
 	
  ///  Get DefaultCorrOpt
  static int  GetDefaultCorrOpt()        { return DefaultCorrOpt; }
  /// Get DefaultUsedStrips
  static int  GetDefaultUsedStrips()     { return DefaultUsedStrips; }
  /// Set DefaultCorrOpt
  static void SetDefaultCorrOpt(int def) { DefaultCorrOpt = def; }
  /// Set DefaultUsedStrips
  static void SetDefaultUsedStrips(int def) {DefaultUsedStrips=def;}

		
  /** @name Printout */
  /**@{*/	

  /// Print cluster basic information
  std::ostream& putout(std::ostream &ostr = std::cout);
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr,  TrClusterR &cls) { 
    return cls.putout(ostr); 
  }

  /// Print cluster strip variables (printop >0 --> verbose) 
  void Print(int printopt =0);
  /// Return a string with some info (used for event display)
  const char* Info(int iRef);

  /**@}*/


  /** @name More cluster quantities */
  /**@{*/

  /// Get number of implant strips between the two highest readout strips
  /// N%10 = n. interstrip (ex. S: 3, K5: 1 or 2 (edges), K7: 1 or 0 ...)
  /// N/10 = structure of K7 (1|2|3)   
  int   GetNInterstrip(int mult = 0); 

  /// Get number of strips with a particular calibration status mask for this cluster (checks also outside of cluster) 
  int   GetNStripWithCalibrationStatus(int nstrip_from_seed, int mask, int mult = 0);
  /// Get number of strips with a particular occupancy status mask for this cluster (checks also outside of cluster)
  int   GetNStripWithOccupancyStatus(int nstrip_from_seed, int mask, int mult = 0);
  /// Get number of strips with a particular gain status mask for this cluster (checks also outside of cluster)
  int   GetNStripWithGainStatus(int nstrip_from_seed, int mask, int mult = 0);
  /// Get number of strips on the edge of the sensor (checks also outside of cluster)
  int   GetNStripOnTheEdgeOfSensor(int nstrip_from_seed, int mult = 0);
  /// Get number of strips on the edge of VA (checks also outside of cluster)
  int   GetNStripOnTheEdgeOfVA(int nstrip_from_seed, int mult = 0);

  /// Check cluster morfology: monothonic behaviour? (extend to how many strips you want around the seed)
  bool  IsMonotonic(int nstrip_from_seed);
  /// Check cluster morfology: monothonic behaviour? (extend to all the strips around seed over threshold)
  bool  IsMonotonicWithThreshold(float threshold);
  /// Check cluster morfology: are the strips over threshold?
  bool  IsOverThreshold(int nstrip_from_seed, float threshold = 1);

  /// Get the ratio between signal of few strips around seed with respect to total signal (no corrections).
  float GetSignalToSignalRatio(int nstrip_from_seed);

  /// \brief Create a cluster status with information about the cluster quality 
  /// \return 
  /// \retval  0-bit: >0 dead strips 
  /// \retval  1-bit: >0 noisy strips 
  /// \retval  2-bit: >0 second step bad strips
  /// \retval  3-bit: >0 strips with bad region flag
  /// \retval  4-bit: >0 dead occupancy strips 
  /// \retval  5-bit: >0 noisy occupancy strips
  /// \retval  6-bit: >0 bad strips from gain database (not gold)
  /// \retval  7-bit: >0 bad strips from gain database (not silver)
  /// \retval  8-bit: >0 strips on the edge of the sensor 
  /// \retval  9-bit: >0 strips on the edge of VA
  /// \retval 10-bit: check if the cluster is well constructed
  /// \retval 11-bit: check if the cluster K7 is well constructed
  int   GetQStatus(int nstrip_from_seed = 1, int mult = -1);

  /// \brief Create a cluster status with valuable information about cluster morfology
  /// \return 
  /// \retval 0-bit: 2 strip around the seed have a non-monotonic behaviour
  /// \retval 1-bit: 3 strip around the seed have a non-monotonic behaviour
  /// \retval 2-bit: strips over SN=0.0 thereshold have a non-monotonic behaviour
  /// \retval 3-bit: strips over SN=1.5 thereshold have a non-monotonic behaviour 
  /// \retval 4-bit: strips over SN=3.0 thereshold have a non-monotonic behaviour 
  /// \retval 5-bit: strips over SN=4.5 thereshold have a non-monotonic behaviour
  int   GetMorfologyStatus();

  /**@}*/


  /// ROOT definition
  ClassDef(TrClusterR, 5)
};


#endif
