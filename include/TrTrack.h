//  $Id: TrTrack.h,v 1.24 2010/05/26 14:18:35 pzuccon Exp $
#ifndef __TrTrackR__
#define __TrTrackR__

//////////////////////////////////////////////////////////////////////////
///
///\class TrTrackPar
///\brief A class to manage reconstructed track parameters
///\ingroup tkrec
///
/// TrTrackRPar contains a set of track parameters correponding one 
/// fitting method, which are contained as a vector by TrTrackR class
///
///\date  2008/11/20 SH  A new structure introduced
///
///
///\class TrTrackR
///\brief A class to manage track reconstruction in AMS Tracker
///\ingroup tkrec
///
/// TrTrackR consists of some fitting parameter sets and hits vector.
/// Each parameter set can be obtained by specifing fitting method ID 
/// defined as a combination of one of EFitMethods plus any of EFitOptions.
/// Track finding/Reconstruction parts are implemented in TrRecon.
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/20 SH  TkFit support
///\date  2007/12/28 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/03/01 AO  Preliminary new data format
///\date  2008/03/12 SH  Getting back to SH version
///\date  2008/07/01 PZ  New Tracker Lib Standalone but to be emebedded
///\date  2008/09/16 PZ  Rename as TrTrackR to be gbatch compatible
///\date  2008/10/23 PZ  Modifications for GBATCH compatibility
///\date  2008/11/05 PZ  New data format to be more compliant
///\date  2008/11/13 SH  Some updates for the new TrRecon
///\date  2008/11/20 SH  A new structure introduced
///\date  2010/03/03 SH  Advanced fits updated 
///$Date: 2010/05/26 14:18:35 $
///
///$Revision: 1.24 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include "TrFit.h"
#include "TkDBc.h"

#include "TrElem.h"
#include <cmath>

class TrRecHitR;

//! Class Used to represent the parameters coming as a result of a Track Fit
class TrTrackPar {
public:
  /// Fit done flag
  bool FitDone;
  /// Bits of hits used for the fitting e.g. 0x7f: Layer1 missing
  short int HitBits;
  /// Chisquare in X (Not normalized)
  Double32_t ChisqX;
  /// Chisquare in Y (Not normalized)
  Double32_t ChisqY;

  /// Ndof in X
  short int NdofX;
  /// Ndof in Y
  short int NdofY;
  /// Normalized chisquare, Chisq(x+y)/Ndof(x+y)
  /*!
   * Note: In case of kChoutko fitting _Chisq is not exactly the same 
   * as (_ChisqX+_ChisqY)/(_NdofX+_NdofY) because of X-Y correlation */
  Double32_t Chisq;
  /// Rigidity in GV
  Double32_t Rigidity;
  /// Fitting error on 1/rigidity in 1/GV
  Double_t ErrRinv;
  /// Track positon (normally defined at Z=0 plane)
  /*!
   * Note: Definition of P0 is different from original Gbatch 
   *       Use AMSTrTarck::GetPentry() for the entry position */
  AMSPoint P0;
  /// Track direction at P0
  /*!
   * Note: Theta and Phi can be obtained through AMSDir */
  AMSDir Dir;
  /// Fitting residual at each layer
  AMSPoint Residual[trconst::maxlay];

  /// Default constructor to fill default values
  TrTrackPar()
    : FitDone(false), HitBits(0), ChisqX(-1), ChisqY(-1), 
      NdofX(0), NdofY(0), Chisq(-1), Rigidity(0), ErrRinv(0), 
      P0(AMSPoint()), Dir(AMSPoint(0, 0, -1)) {}
  ~TrTrackPar(){}
  void Print(int full=0);
  void Print_stream(std::string &ostr,int full=0);
  ClassDef(TrTrackPar,1);
} ; 


//==============================================================================================================================
//===================  TRACK CLASS =============================================================================
//==============================================================================================================================

class TrTrackR : public TrElem {

  //==== Fit Methods ===
public:
  /// enum of fitting methods; trying to keep compatible with TKFITG
  // if you ADD a new type please UPDATE TrTrack::GetFitName
  enum EFitMethods { 
    /// Track witthout hits (for RICH Compatibility)
    kDummy         = -1, 
    /// V. Choutko  fit (5X5 matrix inv, mscat included) : Default final fit
    kChoutko       = 1,
    /// GEANE banded matrix inv  : To be implemented
    kGEANE         = 2, 
    /// GEANE Kalman filter      : To be implemented
    kGEANE_Kalman  = 3, 
    /// Juan algo
    kAlcaraz       = 4,
    /// A. Chikanian algo
    kChikanian     = 5,
    
    /// Linear fit for non-magnetic-field runs
    kLinear        = 10,
    /// Circle fit for YZ plane, Linear fit fot XS plane
    kCircle        = 11,
    /// Simple path integral method : Default fit for scanning
    kSimple        = 12
  };
  
  /// enum of fitting options
  enum EFitOptions { 
    /// Multiple scattering switch
    kMultScat   = 0x10,
    /// Upper half option
    kUpperHalf  = 0x20,
    /// Lower half option
    kLowerHalf  = 0x40,
    /// One drop option
    kOneDrop    = 0x100,
    /// Noise drop option
    kNoiseDrop  = 0x200,
    /// **AMS-B: fitting with layer 8**
    kFitLayer8  = 0x400,
    /// **AMS-B: fitting with layer 9**
    kFitLayer9  = 0x800
  };

  /// Advanced fit flags to be specified by TRCLFFKEY_DEF::AdvancedFitFlag
  enum EAdvancedFitFlags {
    kChoutkoFit   = 0x001, ///< Choutko fitting w/o scattering
    kChoutkoMsct  = 0x002, ///< Choutko fitting w/  scattering
    kChoutkoHalf  = 0x004, ///< Choutko fitting w/o scattering half layers
    kChoutkoDef   = 0x007, ///< Default all Choutko fitting

    kChoutkoDrop  = 0x008, ///< Choutko fitting w/o scattering with one drop
    kChoutkoNdrp  = 0x010, ///< Choutko fitting w/o scattering with noise drop
    kChoutkoAll   = 0x01F, ///< All Choutko fitting

    kAlcarazFit   = 0x020, ///< Alcaraz fitting w/o scattering
    kAlcarazMsct  = 0x040, ///< Alcaraz fitting w/  scattering
    kAlcarazAll   = 0x060, ///< All Alcaraz fitting

    kChikanianFit = 0x080, ///< Chikanian fitting w/ scattering

    kLayer8Fit    = 0x100, ///< AMS-B fitting with layer 8
    kLayer9Fit    = 0x200, ///< AMS-B fitting with layer 9
    kLayer89Fit   = 0x400, ///< AMS-B fitting with layer 8 and 9
    kLayer89All   = 0x700, ///< All AMS-B fitting

    kAllAdvanced  = 0x7FF  ///< All advanced fitting
  };
  /// Default advanced fit flags : keep it "thread-common"
#define DEF_ADVFIT_NUM 8
  static const int DefaultAdvancedFitFlags[DEF_ADVFIT_NUM];


  //==== Static members ===


    
  // --- data members ---
protected:
  /// Vector of hit pointers, to be not stored in ROOT Tree
  TrRecHitR* _Hits[trconst::maxlay]; //!
  /// Vector of hit values of magnetic field at track hits
  AMSPoint  _BField[trconst::maxlay]; 
  /// Vector of hit index, to be stored in ROOT Tree instead of _Hits
  short int _iHits[trconst::maxlay];
  /// Vector of multiplicty index (to fix x-coord) 
  short int _iMult[trconst::maxlay];
  /// Track pattern ID
  short int _Pattern;
  /// Number of hits
  short int _Nhits;
  /// Track pattern ID for hits with X(n)-cluster
  short int _PatternX;
  /// Track pattern ID for hits with Y(n)-cluster
  short int _PatternY;
  /// Track pattern ID for hits with X- and Y-cluster
  short int _PatternXY;
  /// Number of hits with X(n)-cluster
  short int _NhitsX;
  /// Number of hits with Y(p)-cluster
  short int _NhitsY;
  /// Number of hits with X- and Y-clusters
  short int _NhitsXY;
  /// The last successful TrFit object (not stored in ROOT Tree)
  TrFit _TrFit;  //!
  /// maps of track parameters with the key as fitting method ID
  map<int, TrTrackPar> _TrackPar;
  //! Flag if the track was reconstructed with or without Magfield
  bool _MagFieldOn;
  /// Rigidity & chi2 without alignment
  /*!
   * (for compatibility with Gbatch) */
  float DBase[2];
  //! the default fit method used for this track
  int trdefaultfit;
  //! Track Status word
  int Status;

  /// Number of points for half fit (default: 4) : keep it "thread-common"
  static int NhitHalf;

  /// load the std::string sout with the info for a future output
  void _PrepareOutput(int full=0);


public:
  /// Default fit method ID to retrive parameters
  static int DefaultFitID;
#pragma omp threadprivate(DefaultFitID)
public:

//############### CONSTRUCTORS & C. ############################
  /// Default constructor
  TrTrackR();
  /// Constructor with hits
  TrTrackR(int pattern, 
	     int nhits = 0, TrRecHitR *phit[] = 0,AMSPoint bfield[]=0, int *imult = 0,int fithmethod=0);

  /// Dummy track for RICH compatibility (filled at [kDummy])
  TrTrackR(number theta, number phi, AMSPoint pref);
  /// Dummy track for RICH compatibility (filled at [kDummy])
  TrTrackR(AMSDir dir, AMSPoint pref, number rig = 1e7, number errig = 1e7);

  /// Destructor
  virtual ~TrTrackR();

  /// Add a hit with multiplicity index if specified, B-field is taken from coo
  void AddHit(TrRecHitR *hit, int imult = -1);

  /// Set hit patterns
  void SetPatterns(int patx, int paty, int patxy) {
    _PatternX = patx; _PatternY = paty; _PatternXY = patxy;
  }


//#####################  ACCESSORS #########################

  // Access functions
  //! returns the pattern
  int getpattern() const { return _Pattern; }
  //! returns the pattern
  int GetPattern() const { return _Pattern; }
  //! returns the Number of Hits
  int GetNhits  () const { return _Nhits;   }
  //! returns the Number of Hits
  int getnhits  () const { return _Nhits;   }

  //! returns the Number of Hits
  int NTrRecHit  () const { return _Nhits;   }

  //! Returns the pattern on X
  int GetPatternX () const { return _PatternX;  }
  //! Returns the pattern on Y
  int GetPatternY () const { return _PatternY;  }
  //! Returns the pattern on XY
  int GetPatternXY() const { return _PatternXY; }
  //! Number of  X hits
  int GetNhitsX   () const { return _NhitsX;    }
  //! Number of  Y hits
  int GetNhitsY   () const { return _NhitsY;    }
  //! Number of  XY hits
  int GetNhitsXY  () const { return _NhitsXY;   }

  // Get fitting parameter with a key as Fitting method ID 
  //! True if the fit was succeful (fit id from EFitMehthods
  bool     FitDone     (int id= 0) { return ParExists(id) &&
                                            GetPar(id).FitDone;  }
  int      GetHitBits  (int id= 0) { return GetPar(id).HitBits;  }
  //! Returns Chi2 on X
  double   GetChisqX   (int id= 0) { return GetPar(id).ChisqX;   }
  //! Returns Chi2 on Y
  double   GetChisqY   (int id= 0) { return GetPar(id).ChisqY;   }
  //! Returns Global Chi2 
  double   GetChisq    (int id= 0) { return GetPar(id).Chisq;    }
  //! returns Ndof on X
  int      GetNdofX    (int id= 0) { return GetPar(id).NdofX;    }
  //! returns Ndof on Y
  int      GetNdofY    (int id= 0) { return GetPar(id).NdofY;    }
  //! Returnt the fitted Rigidity
  double   GetRigidity (int id= 0) { return GetPar(id).Rigidity; }
  //! Returns the error on 1/R
  double   GetErrRinv  (int id= 0) { return GetPar(id).ErrRinv;  }
  /// Get track entry point (first layer of the fitting)
  AMSPoint GetPentry(int id = 0);
  ///Returns the point of passage on the Z=0 XY plane
  AMSPoint GetP0       (int id= 0) { return GetPar(id).P0;       }
  /// Get track entry point direction (first layer of the fitting)
  AMSDir GetPdir(int id = 0);
  ///Returns the direction at the point of passage on the Z=0 XY plane
  AMSDir   GetDir      (int id= 0) { return GetPar(id).Dir;      }
  /// Return the 3D residual at layer ilay (0-7)
  AMSPoint GetResidual (int ilay, int id= 0) { 
    return ((id == 0 || ParExists(id)) && 0 <= ilay && ilay < trconst::maxlay)
      ? GetPar(id).Residual[ilay] : AMSPoint(0, 0, 0);
  }
  ///  Get back the string corresponding to a fit ID
  char* GetFitNameFromID(int fitnum);
  /// Get the fit ID of the pos-th fit method or zero if pos is invalid
  int    GetFitID(int pos);
  /// Print the string IDs of all the performed fits
  void   PrintFitNames();
  /// Get track position at layer ilay (0-7)
  AMSPoint GetPlayer(int ilay, int id = 0);

  // Aliases
  double   GetP0x      (int id= 0) { return GetP0(id).x(); }
  double   GetP0y      (int id= 0) { return GetP0(id).y(); }
  double   GetP0z      (int id= 0) { return GetP0(id).z(); }
  double   GetChi2     (int id= 0) { return GetChisq(id);  }
  double   GetTheta    (int id= 0) { return GetDir(id).gettheta();}
  double   GetPhi      (int id= 0) { return GetDir(id).getphi();  }

  /// Check if _TrackPar[id]
  bool ParExists(int id) { return (_TrackPar.find(id) != _TrackPar.end()); }

  /// Get TrTrackPar with id
  TrTrackPar &GetPar(int id = 0);

  /// Test HitBits
  bool TestHitBits(int layer, int id = 0) { 
    return (GetHitBits(id) & (1 << (trconst::maxlay-layer)));
  }

  /// Get the pointer to the i-th in the track
  TrRecHitR *GetHit(int i);
  /// Get the pointer of hit at Layer, ilay(0-7), or returns 0 if not exists
  TrRecHitR *GetHitL(int ilay);
  /// Get areferemce to the i-th in the track
  TrRecHitR& TrRecHit(int i);
  /// Get the pointer to the i-th in the track
  TrRecHitR *pTrRecHit(int i){ return GetHit(i);}
  /// Get the index of the i-th hit in the track within the hit vector
  int iTrRecHit(int i){return _iHits[i];}
  

  /// Get tan(theta) on XZ projection
  double GetThetaXZ(int id = 0) { 
    AMSDir dir = GetDir(id);
    return (dir.z() != 0) ? dir.x()/dir.z() : 0;
  }
  /// Get tan(theta) on YZ projection
  double GetThetaYZ(int id = 0) { 
    AMSDir dir = GetDir(id);
    return (dir.z() != 0) ? dir.y()/dir.z() : 0;
  }

  /// Get normalized chi2 on X with ErrX = 50 um
  double GetNormChisqX(int id= 0);

  /// Get normalized chi2 on Y with ErrY = 30 um
  double GetNormChisqY(int id= 0);

  /// Get TrFit object of the last fit (not to be used outside GBATCH)
  TrFit *GetTrFit() { return &_TrFit; }

  //!Returns the default fit method used for this track
  int Gettrdefaultfit(){return trdefaultfit;}

  //!Set the default fit method to be used for this track
  void Settrdefaultfit(int def ){trdefaultfit=def;}

//############## VARIUOS METHODS ###############

  /// Perform 3D fitting with the method specified by ID
  /*!
    \param[in] id    Fitting method, defined as a combination of
    one of EFitMethods and any of EFitOptions, 
    e.g. ID= 0x31= kChoutko+kMultScat+kUpperHalf. 
    Fitting parameters will be overwritten on the map[ID] 
    unless trfit is specified. 

    \param[in] layer Layer to be excluded in the fitting (if specified) 

    \param[in] update Track parameters are overwritten if update=true

    \param[in] err   Fitting errors (0:x,1:y,2:z) to be used. 
    If they are not specified, default values 
    are taken from TRFITFFKEY.ErrX, ErrY, and ErrZ

    \param[in] mass  Particle mass assumed for multiple scattering. 
    Ignored if kMultScat option is not set.

    \param[in] chrg  Particle charge (in unit of e) assumed for multiple 
    scattering. Ignored if kMultScat option is not set.
    
    \return          Chisq(X+Y)/Ndof if succeeded, or -1 if failed
  */
  float Fit(int id = 0,
	    int layer = -1, bool update = true, const float *err = 0, 
	    float mass = 0.938272297, float chrg = 1);

  void ReFit( const float *err = 0, 
	      float mass = 0.938272297, float chrg = 1);


  /// Perform simple fitting with a constant position error of 0.03 cm
  float SimpleFit(void) {
    static const float err[3] = { 0.03, 0.03, 0.03 };
#pragma omp threadprivate(err)
    return Fit(kSimple, -1, 0, err);
  }
  /// Perform simple fitting with a constant position error
  float SimpleFit(float *err) {
    return Fit(kSimple, -1, 0, err);
  }

  /// Check if advanced fits specified by flag are done
  int AdvancedFitDone(int add_flags=0);

  /// Do advanced fits specified in the default list and add in OR the add_flags
  int DoAdvancedFit(int add_flags=0);

  void GetMaxShift(int& left,int &right);
  void Move(int shift);

  /// Build index vector (_iHits) from hits vector (_Hits)
  void BuildHitsIndex();
  
  /// Interpolation onto Z=const. plane
  /*!
   * \param[in]  zpl  Plane position (Z=zpl)
   * \param[out] pnt  Track position  at Z=zpl
   * \param[out] dif  Track direction at Z=zpl
   * \param[in]  id   Fitting method ID
   * \return          Path length between Z=P0z(usually 0) and Z=zpl
   */
  double Interpolate(double zpl, AMSPoint &pnt, AMSDir &dir, 
		     int id = 0);

  /// Interpolation onto Tracker layer with alignment correction 
  /*!
   * \param[in]  ily  Tracker layer index (Layer_number-1, [0-7])
   * \param[out] pnt  Track position  at the layer
   * \param[out] dif  Track direction at the layer
   * \param[in]  id   Fitting method ID
   * \return          Path length between Z=P0z(usually 0) and Z=zpl
   */
  double InterpolateLayer(int ily, AMSPoint &pnt, AMSDir &dir, 
			  int id = 0);

  /// Interpolation onto Tracker layer with alignment correction 
  /*!
   * \param[in]  ily  Tracker layer index (Layer_number-1, [0-7])
   * \param[in]  id   Fitting method ID
   * \return          Track position  at the layer
   */
  AMSPoint InterpolateLayer(int ily, int id = 0);

  /// Build interpolation vectors onto Z=zpl[i] (0<=i<nz) planes
  /*!
   * \param[in]  nz   Number of planes
   * \param[in]  zpl  Plane Z positions vector
   * \param[out] pvec Track positions   vector
   * \param[out] dvec Track directions  vector, not filled if 0
   * \param[out] lvec Path lengths      vector, not filled if 0
   * \param[in]  id   Fitting method ID
   * lvec[0] is length between Z=P0z and Z=zpl[0],
   * lvec[i] is length between Z=zpl[i] and Z=zpl[i-1] for i>0
   */
  void Interpolate(int nz, double *zpl, 
		   AMSPoint *pvec, AMSDir *dvec = 0, double *lvec = 0,
		   int id = 0);

  /// General interpolation (for the compatibility with Gbatch)
  void interpolate(AMSPoint pnt, AMSDir dir,  AMSPoint &P1, 
		   number &theta, number &phi, number &length, 
		   int id = 0);

  /// Interpolation to cylindrical surface
  bool interpolateCyl(AMSPoint pnt, AMSDir dir, number rad, number idir, 
		      AMSPoint &P1, number &theta, number &phi,
		      number &length, int id = 0);

  /// Interception (for the compatibility with Gbatch)
  int intercept(AMSPoint &pnt, int layer, 
		number &theta, number &phi, number &local, int id = 0);

  /// For compatibility with Gbatch
  void getParFastFit(number& Chi2,  number& Rig, number& Err, 
		     number& Theta, number& Phi, AMSPoint& X0);

  
  /// Print Track basic information on a given stream 
  std::ostream& putout(std::ostream &ostr = std::cout);
 
  /// Print Track info (verbose if opt !=0 )
  void  Print(int opt=0);
  /// Return a string with hit infos (used for the event display)
  char *Info(int iRef=0);

  /// Stream out operator
  friend std::ostream &operator << (std::ostream &ostr,  TrTrackR &Tr){
    return Tr.putout(ostr);}

  /// For Gbatch compatibility
  uinteger checkstatus(integer checker) const{return Status & checker;}
  uinteger getstatus() const{return Status;}
  void     setstatus(uinteger status){Status=Status | status;}
  void     clearstatus(uinteger status){Status=Status & ~status;} 


  /// --- Std gbatch compatibility functions
  geant Rigidityf(){ return GetRigidity();}

  geant Chi2FastFitf() {return GetChi2(kChoutko| kMultScat );}

  geant HChi2f(int half){ return (half==0)?
      GetChi2(kChoutko| kMultScat|kUpperHalf):
      GetChi2(kChoutko| kMultScat|kLowerHalf);}

  geant FChi2MSf(){return  GetChi2(kChoutko);}

  geant HRigidityf(int half){ return (half==0)?
      GetRigidity(kChoutko| kMultScat|kUpperHalf):
      GetRigidity(kChoutko| kMultScat|kLowerHalf);}

  

  /// ROOT definition
  ClassDef(TrTrackR, 1);
};




#endif
