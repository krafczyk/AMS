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

#include "amsdbc.h"

#include <cmath>
#include <vector>
#include <string>

/*!
 \class TrClusterR
 \brief A class to manage reconstructed cluster in AMS Tracker
 \ingroup tkrec

 TrClusterR, i.e. Tracker reconstructed cluster, is the core of the Tracker reconstruction. 
 This class gives accessors and memebers to retrieve all the reconstructed cluster 
 properties: signal (data members), calibration parameters (via TrCalDB), gains (via TrParDB),
 coordinates (via TkCoo). 

 $Date: 2011/03/30 13:19:17 $

 $Revision: 1.23 $

*/


class TrClusterR :public TrElem{

 public:
  
  enum CorrectionOptions {
    /// No Correction Applied
    kNoCorr       = 0x00,       
    /// Signal Corr.: Cluster Asymmetry Correction (left/right)
    kAsym         = 0x01, 
    /// Signal Corr.: P-Strip Correction (inactive)
    kPStrip       = 0x02,
    /// Total Signal Corr.: Energy Loss Normalization at 300 um [cos(Theta)^-1]
    kAngle        = 0x04,
    /// Total Signal Corr.: Gain Correction
    kGain         = 0x08, 
    /// Total Signal Corr.: VA Gain Correction 
    kVAGain       = 0x10, 
    /// Total Signal Corr.: Charge Loss Correction 
    kLoss         = 0x20,
    /// Total Signal Corr.: Normalization to P-Side      
    kPN           = 0x40,
    /// Total Signal Corr.: Normalization to number of MIP
    kMIP          = 0x80,
    /// Coordinate Corr.: Flip the eta used strips
    kFlip         = 0x100,
    /// Coordinate Corr.: Correct for the charge coupling (4%)
    kCoupl        = 0x200,
    /// Coordinate Corr.: Belau correction
    kBelau        = 0x400
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
  void _PrepareOutput(int full=0);
  
 public:

  /// Default signal correction option
  static int DefaultCorrOpt;
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
	
  /// Is a TAS cluster? (check the status bit)
  bool  TasCls() const { return checkstatus(TASCLS); }
  /// Used for track AMSDBc::USED = 32; (0x0020)
  bool  Used() const { return checkstatus(AMSDBc::USED); }	
	
  /**@}*/


  /** @name   SIGNALS & AMPLITUDE */
  /**@{*/	

  /// Get cluster amplitude
  float GetTotSignal(int opt = DefaultCorrOpt);
  /// Convert signal to number of MIPs (each MIP is approximately 81 keV)
  float GetNumberOfMIPs(int opt = DefaultCorrOpt);

  /// Get i-th strip signal
  float GetSignal(int ii, int opt = DefaultCorrOpt);
  /// Get i-th strip noise (from calibration)
  float GetSigma(int ii) { return GetNoise(ii);  }
  /// Get i-th strip noise (from calibration)
  float GetNoise(int ii);   
  /// Get i-th signal to noise ratio 
  float GetSN(int ii, int opt = DefaultCorrOpt) { 
    return (GetNoise(ii)<=0.) ? -9999. : GetSignal(ii,opt)/GetNoise(ii); 
  }
 
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
  float GetCofG(int nstrips = DefaultUsedStrips, int opt = DefaultCorrOpt);
  /// Get local coordinate with center of gravity on nstrips 
  float GetXCofG(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultCorrOpt);
  /// Get local coordinate with center of gravity on nstrips 
  float GetXCofG_old(int nstrips = DefaultUsedStrips, int imult = 0, const int opt = DefaultCorrOpt) { 
    return TkCoo::GetLocalCoo(GetTkId(),GetSeedAddress(opt)+GetCofG(nstrips,opt),imult); 
  }  
  /// Get eta (center of gravity with the two higher strips) 
  float GetEta(int opt = DefaultCorrOpt);
  /// Get eta (center of gravity with the two higher strips) by CofG algorythm  
  float GetEta_CofG(int opt = DefaultCorrOpt) { float eta = GetCofG(2,opt); return (eta>0.) ? eta : eta + 1.; }

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
  /// Get track interpolation angle theta
  inline float GetTheta() { return acos(1./(1.+_dxdz*_dxdz+_dydz*_dydz)); }
  /// Get track impact angle XZ (degrees) 
  inline float GetImpactAngle() { return (GetSide()==0) ? atan(_dxdz)*180./3.14159265 : atan(_dydz)*180./3.14159265; }

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
  char* Info(int iRef);

  /**@}*/


  /// ROOT definition
  ClassDef(TrClusterR, 4)
};


#endif
