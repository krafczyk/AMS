//  $Id: TrTrack.h,v 1.58 2011/02/22 09:22:29 shaino Exp $
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
///$Date: 2011/02/22 09:22:29 $
///
///$Revision: 1.58 $
///
//////////////////////////////////////////////////////////////////////////


class AMSTrTrackError;
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
  /// Bitsfield of layers used for the fitting; bit_num <==> layer-1
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
  Double32_t ErrRinv;
  /// Track positon (normally defined at Z=0 plane)
  /*!
   * Note: Definition of P0 is different from original Gbatch 
   *       Use AMSTrTarck::GetPentry() for the entry position */
  AMSPoint P0;
  /// Track direction at P0
  /*!
   * Note: Theta and Phi can be obtained through AMSDir */
  AMSDir Dir;
  /// Fitting residual at each layer 0-X  1-Y
  float Residual[trconst::maxlay][2];

  /// Default constructor to fill default values
  TrTrackPar()
    : FitDone(false), HitBits(0), ChisqX(-1), ChisqY(-1), 
      NdofX(0), NdofY(0), Chisq(-1), Rigidity(0), ErrRinv(0), 
      P0(AMSPoint()), Dir(AMSPoint(0, 0, -1)) {}
  ~TrTrackPar(){}
  void Print(int full=0) const;
  void Print_stream(std::string &ostr,int full=0) const;

  ClassDef(TrTrackPar,2);
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
    /// A. Chikanian algo (C++ imported)
    kChikanian     = 5,
    /// A. Chikanian algo (Original Fortran, available only with gbatch.exe)
    kChikanianF    = 6,
    
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
    /// Set the fitting weight of all the points same
    kSameWeight = 0x100000,
    /// Upper half option
    kUpperHalf  = 0x20,
    /// Lower half option
    kLowerHalf  = 0x40,
    /// External 2 + 2 (1N + top internal) +( bottom internal + 9) 
    kExternal   = 0x80,
    /// One drop option
    kOneDrop    = 0x100,
    /// Noise drop option
    kNoiseDrop  = 0x200,
    /// **AMS-B: fitting with layer 8**
    kFitLayer8  = 0x00400,
    /// **AMS-B: fitting with layer 9**
    kFitLayer9  = 0x00800,
    /// **AMS-B: fitting with layer 1**
    kFitLayer1  = 0x01000,
    /// **AMS-B: fitting with layer 2**

    kFitLayer2  = 0x02000,
    /// **AMS-B: fitting with layer 3**
    kFitLayer3  = 0x04000,
    /// **AMS-B: fitting with layer 4**
    kFitLayer4  = 0x08000,
    /// **AMS-B: fitting with layer 5**
    kFitLayer5  = 0x10000,
    /// **AMS-B: fitting with layer 6**
    kFitLayer6  = 0x20000,
    /// **AMS-B: fitting with layer 7**
    kFitLayer7  = 0x40000,
    /// Specific pattern selected
    kPattern    = 0x80000
  };

#define DEF_ADVFIT_NUM 8
  static const int DefaultAdvancedFitFlags[DEF_ADVFIT_NUM];
  /// Advanced fit bits
  static int AdvancedFitBits;

  //==== Static members ===


    
  // --- data members ---
protected:
  /// Vector of hit pointers, to be not stored in ROOT Tree
  TrRecHitR* _Hits[trconst::maxlay]; //!
//   /// Vector of hit values of magnetic field at track hits
//   AMSPoint  _BField[trconst::maxlay]; 
  /// Vector of hit index, to be stored in ROOT Tree instead of _Hits
  short int _iHits[trconst::maxlay];
//   /// Vector of multiplicty index (to fix x-coord) 
//   short int _iMult[trconst::maxlay];
  /// The real bitted Track Pattern
  unsigned short int _bit_pattern;
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
  /// Service function to check if a layer is included in a fit
  bool CheckLayFit(int fittype,int lay) const;
public:
  /// Default fit method ID to retrive parameters
  static int DefaultFitID;

  /// Default mass for fitting
  static float DefaultMass;

  /// Default charge for fitting
  static float DefaultCharge;
public:
	/** @name CONSTRUCTORS & C.
	 */
	/**@{*/ 
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

  TrTrackR(const TrTrackR& orig);

  /// Destructor
  virtual ~TrTrackR();

  /**@}*/
	
  /** @name Track Data Accessors	 */
  /**@{*/
	
	//! returns the BIT Mask of the layers (1-9) with hit. bit_num <==> layer_num-1
	unsigned short int GetBitPattern() const { return _bit_pattern; }
	//! returns the Number of Hits
	int GetNhits  () const { return _Nhits;   }
	//! returns the Number of Hits
	int getnhits  () const { return _Nhits;   }
	//! returns the Number of Hits
	int NTrRecHit  () const { return _Nhits;   }
	
	//! Number of  X hits
	int GetNhitsX   () const { return _NhitsX;    }
	//! Number of  Y hits
	int GetNhitsY   () const { return _NhitsY;    }
	//! Number of  XY hits
	int GetNhitsXY  () const { return _NhitsXY;   }
	
	/// Get areferemce to the i-th in the track
	TrRecHitR& TrRecHit(int i ); 
	/// Get the index of the i-th hit in the track within the hit vector
	int iTrRecHit(int i){return _iHits[i];}
	/// Get the pointer to the i-th in the track
	TrRecHitR *pTrRecHit(int i){ return GetHit(i);}
	/// Get the pointer of hit at Layer, ilay(0-7), or returns 0 if not exists
	TrRecHitR *GetHitL(int ilay) const;
	/// For Gbatch compatibility
	uinteger checkstatus(integer checker) const{return Status & checker;}
	uinteger getstatus() const{return Status;}
	void     setstatus(uinteger status){Status=Status | status;}
	void     clearstatus(uinteger status){Status=Status & ~status;} 
  /**@}*/
	
  /** @name Fit type Accessors
  */
 /**@{*/

/*!
   \brief It gives you the integer number (fit code) to be used to access the fit results (TrTrackPar obj) 
    Advanced TrTrackPar accessor
    \param algo Fitting algorithm= 
                          \li 0 The default algorithm choosen at recon stage (if you select this, you cannot use the other param)
                          \li 1 Choutko;
                          \li 2 Alcaraz;
                          \li 3 ChikanianF (the original A. Chikanian algo not for refit);
                          \li 4 ChikanianC (Chikanin C version, in development not for general use);
                        \li +10 mscattering off;
                        \li +20 same weight;
    \param pattern    Hit Pattern= 
                     \li  0   all hits belonginf to track; (maximum span)
					 \li  1   inner upper half;
					 \li  2   inner lower half;
                     \li  3   Inner Tracker only (aka drop 2 external hits);
					 \li  4   only 2 + 2 external hits;
                     \li  5   inner + Layer 1N (kFitLayer8)
                     \li  6   inner + Layer 9  (kFitLayer9)
                     \li  7   inner + Layer 1N + layer 9 (kFitLayer8 & kFitLayer9)
                     \li  OR
					 \li mmmmmmmmm    where m=0 or 1 for 
                                    TrRecHit layer GetLayer()
                                     from right to the left such as 
                                     100110010  corresponds to layers
                                     2,5,6,9
     \param refit    
                    0   do not refit
                    1    refit if does not exist
                    2  refit
     \param mass (optional) the particle mass. (default= 0.938272297)
     
     \param chrg (optional) the particle charge. (default = 1)

     \return  the code to access the TrTrackPar object corresponding to the selected fit or <0  if errors
             \retval  >=0  --> The code corresponding to the requested fittype
              \retval -1 --> The requested fit cannot be performed on this track
              \retval -2 --> The requested fit it is not available without refitting    
              \retval -3 --> The refit failed
              \retval -4 --> Should not happen!! contact the developpers

     To correctly perform the refit, the FieldMap file is needed.
     If not loaded elsewhere the program try load the file $AMSDataDir/v5.00/MagneticFieldMapPM_NEW.bin
     check to have this file on your disk.
!*/
  int   iTrTrackPar(int algo=0, int pattern=0, int refit=0, float mass = DefaultMass, float chrg = DefaultCharge);

  /*!\brief it returns the TrTrackPar object selected with the code given by  iTrTrackPar(...)
 
     \return TrTrackPar object or  trow an exception "TrTrackPar-E-NotFound "
  !*/
  const TrTrackPar&  gTrTrackPar(int fit_type)  throw (string);
 //!Return the number of store fit results (TrTrackPar objects)
  int nTrTrackPar(){return _TrackPar.size();}
  /// Print the string IDs of all the performed fits
  void   PrintFitNames();
///@}
	
/** @name Accessor to fit dependent quantiies */
///@{

  //! Returnt the fitted Rigidity from TrTrackPar corresponding to id
  double   GetRigidity (int id= 0) const { return GetPar(id).Rigidity; }
  //! Returns the error on 1/R from TrTrackPar corresponding to id
  double   GetErrRinv  (int id= 0) const { return GetPar(id).ErrRinv;  }
  //! Returns Global Chi2 from TrTrackPar corresponding to id
  double   GetChisq    (int id= 0) const { return GetPar(id).Chisq;    }
  //! Returns Chi2 on X from TrTrackPar corresponding to id
  double   GetChisqX   (int id= 0) const { return GetPar(id).ChisqX;   }
  //! returns Ndof on X from TrTrackPar corresponding to id
  int      GetNdofX    (int id= 0) const { return GetPar(id).NdofX;    }
  //! Returns Chi2 on Y from TrTrackPar corresponding to id
  double   GetChisqY   (int id= 0) const { return GetPar(id).ChisqY;   }
  //! returns Ndof on Y from TrTrackPar corresponding to id
  int      GetNdofY    (int id= 0) const { return GetPar(id).NdofY;    }
  /// Get normalized chisquare in X
  double GetNormChisqX(int id= 0);
  /// Get normalized chisquare in Y
  double GetNormChisqY(int id= 0);

  /// Get track entry point (first layer of the fitting) from TrTrackPar corresponding to id
  AMSPoint GetPentry(int id = 0);
  ///Returns the point of passage on the Z=0 XY plane from TrTrackPar corresponding to id
  AMSPoint GetP0       (int id= 0) const { return GetPar(id).P0;       }
  /// Get track entry point direction (first layer of the fitting) from TrTrackPar corresponding to id
  AMSDir GetPdir(int id = 0);
  ///Returns the direction at the point of passage on the Z=0 XY plane from TrTrackPar corresponding to id
  AMSDir   GetDir      (int id= 0) const { return GetPar(id).Dir;      }
  /// Returns the Theta angle at the point of passage on the Z=0 XY plane from TrTrackPar corresponding to id
  double   GetTheta    (int id= 0) const { return GetDir(id).gettheta();}
  /// Returns the Phi angle at the point of passage on the Z=0 XY plane from TrTrackPar corresponding to id
  double   GetPhi      (int id= 0) const { return GetDir(id).getphi();  }


  /// Return the  proj (0=X,1=Y) residual at layer ilay (0-8) from TrTrackPar corresponding to id
  AMSPoint  GetResidual (int ilay, int id= 0) const { 
    if (ilay < 0 || ilay >= trconst::maxlay || !ParExists(id)) return AMSPoint(0,0,0);
    return AMSPoint(GetPar(id).Residual[ilay][0],
		    GetPar(id).Residual[ilay][1],
		    TkDBc::Head->GetZlayer(ilay+1));
  }


  /// Get track position at layer ilay (0-7)
  AMSPoint GetPlayer(int ilay, int id = 0);


 


  /// Checks if an it on layer(1-9) is used for the fit_type id
  bool TestHitBits(int layer, int id = 0) const { 
    return (GetHitBits(id) & (1 << (layer-1)));
  }

 


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

  /// Get TrFit object of the last fit (not to be used outside GBATCH)
  TrFit *GetTrFit() { return &_TrFit; }

  //!Returns the default fit method used for this track
  int Gettrdefaultfit(){return trdefaultfit;}

  //!Set the default fit method to be used for this track
  void Settrdefaultfit(int def ){trdefaultfit=def;}

  /// Interpolation onto Z=const. plane
  /*!
   * \param[in]  zpl  Plane position (Z=zpl)
   * \param[out] pnt  Track position  at Z=zpl
   * \param[out] dif  Track direction at Z=zpl
   * \param[in]  id   Fitting method ID
   * \return          Path length between Z=P0z(usually 0) and Z=zpl
   */
  double Interpolate(double zpl, AMSPoint &pnt, AMSDir &dir, 
		     int id = 0) const;

  /// Interpolation onto Tracker layer with alignment correction 
  /*!
   * \param[in]  ily  Tracker layer index (Layer_number-1, [0-7])
   * \param[out] pnt  Track position  at the layer
   * \param[out] dif  Track direction at the layer
   * \param[in]  id   Fitting method ID
   * \return          Path length between Z=P0z(usually 0) and Z=zpl
   */
  double InterpolateLayer(int ily, AMSPoint &pnt, AMSDir &dir, 
			  int id = 0) const;

  /// Interpolation onto Tracker layer with alignment correction 
  /*!
   * \param[in]  ily  Tracker layer index (Layer_number-1, [0-7])
   * \param[in]  id   Fitting method ID
   * \return          Track position  at the layer
   */
  AMSPoint InterpolateLayer(int ily, int id = 0) const;

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
		   int id = 0) const;

  /// General interpolation (for the compatibility with Gbatch)
  void interpolate(AMSPoint pnt, AMSDir dir,  AMSPoint &P1, 
		   number &theta, number &phi, number &length, 
		   int id = 0) const;

  /// Interpolation to cylindrical surface
  bool interpolateCyl(AMSPoint pnt, AMSDir dir, number rad, number idir, 
		      AMSPoint &P1, number &theta, number &phi,
		      number &length, int id = 0);
  /// Print Track info (verbose if opt !=0 )
  void  Print(int opt=0);
  /// Return a string with hit infos (used for the event display)
  char *Info(int iRef=0);
/**@}*/

/** @name Alternative and deprecated methods
 */
///@{
	
  double   GetP0x      (int id= 0) const { return GetP0(id).x(); }
  double   GetP0y      (int id= 0) const { return GetP0(id).y(); }
  double   GetP0z      (int id= 0) const { return GetP0(id).z(); }
  double   GetChi2     (int id= 0) const { return GetChisq(id);  }

  /// Check if _TrackPar[id]
  bool ParExists(int id) const { return (_TrackPar.find(id) != _TrackPar.end()); }

  /// Get TrTrackPar with id (const operator)
  const TrTrackPar &GetPar(int id = 0) const;

  /// Get TrTrackPar with id (non-const operator, only for experts)
  TrTrackPar &GetPar(int id);
  
  /// Get the pointer to the i-th in the track
  TrRecHitR *GetHit(int i);
  /// Get the pointer to the i-th in the track (const version)
  TrRecHitR *GetHit(int i) const;
  
	
  //! returns the pattern ID
  int getpattern() const { return _Pattern; }
  //! returns the pattern
  int GetPattern() const { return _Pattern; }

  int    GetFitID(int pos);
 
  /// --- Std gbatch compatibility functions
  geant Rigidityf() const { return GetRigidity();}

  geant Chi2FastFitf() const {return GetChi2(kChoutko| kMultScat );}

  geant HChi2f(int half) const { return (half==0)?
      GetChi2(kChoutko| kUpperHalf):
      GetChi2(kChoutko| kLowerHalf);}

  geant FChi2MSf() const {return  GetChi2(kChoutko);}

  geant HRigidityf(int half) const { return (half==0)?
      GetRigidity(kChoutko|kUpperHalf):
      GetRigidity(kChoutko|kLowerHalf);}
  
  
  // Get fitting parameter with a key as Fitting method ID 
  //! True if the fit was succeful (fit id from EFitMehthods
  bool     FitDone     (int id= 0) const { return ParExists(id) &&
      GetPar(id).FitDone;  }
  
  int Pattern(int input=111111111) ; ///< \return full track  pattern hit suitable for iTrTrackPar
  //! Returns the pattern on X
  int GetPatternX () const { return _PatternX;  }
  //! Returns the pattern on Y
  int GetPatternY () const { return _PatternY;  }
  //! Returns the pattern on XY
  int GetPatternXY() const { return _PatternXY; }
  //! Returns the TrTrackPar HitBits
  int      GetHitBits  (int id= 0) const { return GetPar(id).HitBits;  }
  /// Get the fit ID of the pos-th fit method or zero if pos is invalid
  
  /// For compatibility with Gbatch
  void getParFastFit(number& Chi2,  number& Rig, number& Err, 
		     number& Theta, number& Phi, AMSPoint& X0);

  /// Interception (for the compatibility with Gbatch)
  int intercept(AMSPoint &pnt, int layer, 
		number &theta, number &phi, number &local, int id = 0);
///@}
/** @name A.F: RECONSTRUCTION  METHODS  
 */
///@{


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
  float FitT(int id = 0,
	    int layer = -1, bool update = true, const float *err = 0, 
	    float mass = DefaultMass, float chrg = DefaultCharge);

  void ReFit( const float *err = 0, 
	      float mass = DefaultMass, float chrg = DefaultCharge);


  /// Perform simple fitting with a constant position error of 0.03 cm
  //  float SimpleFit(void) {
  //    static const float err[3] = { 0.03, 0.03, 0.03 };
  //#pragma omp threadprivate(err)
  //    return Fit(kSimple, -1, 0, err);
  //  }
  /// Perform simple fitting with a constant position error
  // float SimpleFit(float *err) {
  //   return Fit(kSimple, -1, 0, err);
  //  }

  /// Check if advanced fits specified by flag are done
  int AdvancedFitDone(int add_flags=0);

  /// Do advanced fits specified in the default list and add in OR the add_flags
  int DoAdvancedFit(int add_flags=0);

  /// Get range of maximum shift in multiplicity
  void GetMaxShift(int &left, int &right);

  /// Move the track in X direction
  void Move(int shift, int fit_flags = 0);

  /// Estimate dummyX and multiplicity for OnlyY hits
  void EstimateDummyX(int fitid = 0);

  /// Fill residual of external layers and internal track
  void FillExRes(int idsel = -1);

  /// Build index vector (_iHits) from hits vector (_Hits)
  void BuildHitsIndex();
  
  /// Set index vector (_iHits)
  void SetHitsIndex(int *ihit);
  /// Add a hit with multiplicity index if specified, B-field is taken from coo
  void AddHit(TrRecHitR *hit, int imult = -1);
  /// Remove the hit on the selected layer (1-9) 
  bool RemoveHitOnLayer( int layer);
  

  /// Set hit patterns
  void SetPatterns(int patx, int paty, int patxy, int pat = -1) {
    _PatternX = patx; _PatternY = paty; _PatternXY = patxy;
    if (pat >= 0) _Pattern = pat;
  }
  ///@}  
	
  /// Print Track basic information on a given stream 
  std::ostream& putout(std::ostream &ostr = std::cout);

	///  Get back the string corresponding to a fit ID
	static char* GetFitNameFromID(int fitnum);

	/// Get bit pattern as a string
	static const char *HitBitsString(int aa, char con = 'X', char coff = '_')
	{
		static char ss[10];
		for (int ii = 0; ii < 9; ii++) ss[8-ii] = (aa & (1<<ii)) ? con : coff;
		ss[9]='\0';
		return ss;
	}
	
  /// Stream out operator
  friend std::ostream &operator << (std::ostream &ostr,  TrTrackR &Tr){
    return Tr.putout(ostr);}
	
	
ClassDef(TrTrackR, 4); 


  
};




#endif
