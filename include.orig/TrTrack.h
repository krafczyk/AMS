//  $Id: TrTrack.h,v 1.1 2008/12/18 11:19:24 pzuccon Exp $
#ifndef __AMSTrTrack__
#define __AMSTrTrack__

//////////////////////////////////////////////////////////////////////////
///
///
///\class AMSTrTrack
///\brief A class to manage track reconstruction in AMS Tracker
///\ingroup tkrec
///
/// AMSTrTrack consists of some fitting parameter sets and hits vector.
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
///$Date: 2008/12/18 11:19:24 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "link.h"
#include "point.h"
#include "VCon.h"
#include "TObject.h"

#include <cmath>

class AMSTrRecHit;
class TrFit;

class AMSTrTrackPar {
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

  /// Default constructor to fill default values
  AMSTrTrackPar()
    : FitDone(false), HitBits(0), ChisqX(0), ChisqY(0), 
      NdofX(0), NdofY(0), Chisq(0), Rigidity(0), ErrRinv(0), 
      P0(AMSPoint()), Dir(AMSPoint(0, 0, -1)) {}
  ~AMSTrTrackPar(){}
  void Print(){
    printf("Fit Done: %d, HitBits: %06d, ChisqX: %f, ChisqY: %f, Chisq: %f, NdofX: %d NdofY: %d \n",
	   FitDone,HitBits,ChisqX,ChisqY,Chisq,NdofX,NdofY);
    printf("Rigidity:  %f Err(1/R):  %f P0: %f %f %f  Dir:  %f %f %f\n",
	   Rigidity,ErrRinv,P0[0],P0[1],P0[2],Dir[0],Dir[1],Dir[2]);
  }
ClassDef(AMSTrTrackPar,1);
} ; 


class AMSTrTrack : public AMSlink {

private:
  static geant _TimeLimit; //!
public:
  /// Default fit method ID to retrive parameters
  static int DefaultFitID;

  /// enum of fitting methods; trying to keep compatible with TKFITG
  enum EFitMethods { 
    /// Track without hits (for RICH Compatibility)
    kDummy         = 0, 
    /// V. Choutko  fit (5X5 matrix inv, mscat included) : Default final fit
    kChoutko       = 1,
    /// GEANE banded matrix inv  : To be implemented
    kGEANE         = 2, 
    /// GEANE Kalman filter      : To be implemented
    kGEANE_Kalman  = 3, 
    /// Juan algo                : Partially implemented, to be completed
    kJuan          = 4,
    /// A. Chikanian algo        : Will NOT be implemented without request
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
    kLowerHalf  = 0x40
  };

  /// Number of points for half fit (default: 4)
  static int NhitHalf;

protected:
  /// maps of track parameters with the key as fitting method ID
  map<int, AMSTrTrackPar> _TrackPar;

  /// The last successful TrFit object (not stored in ROOT Tree)
  TrFit *_TrFit;  //!
  uinteger _Status;

public:


  /// Virtual container
  static VCon* vcon;

  /// For Gbatch compatibility
  static void  SetTimeLimit(geant time) { _TimeLimit = time; }
  /// For Gbatch compatibility
  static geant GetTimeLimit() { return _TimeLimit; }
  
  /// Vector of hit pointers, to be not stored in ROOT Tree
  AMSTrRecHit* _Hits[trconst::maxlay]; //!
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

  /// Rigidity & chi2 without alignment
  /*!
   * (for compatibility with Gbatch) */
  float DBase[2];

public:
  /// Default constructor
  AMSTrTrack();
  /// Constructor with hits
  AMSTrTrack(int pattern, 
	     int nhits = 0, AMSTrRecHit *phit[] = 0, int *imult = 0);
  /// Dummy track for RICH compatibility (filled at [kDummy])
  AMSTrTrack(number theta, number phi, AMSPoint pref);
  /// Dummy track for RICH compatibility (filled at [kDummy])
  AMSTrTrack(AMSDir dir, AMSPoint pref, number rig = 1e7, number errig = 1e7);

  /// Destructor
  ~AMSTrTrack();
  AMSTrTrack( const AMSTrTrack & orig);

  AMSTrTrack& operator=( const AMSTrTrack & orig);
    
  /// Add a hit with multiplicity index (if specified)
  void AddHit(AMSTrRecHit *hit, int imult = -1);

  /// For Gbatch compatibility
  uinteger checkstatus(integer checker) const{return _Status & checker;}
  uinteger getstatus() const{return _Status;}
  void     setstatus(uinteger status){_Status=_Status | status;}
  void     clearstatus(uinteger status){_Status=_Status & ~status;} 
    int IsGood   () { return 1; }
  AMSTrTrack *next(){ return (AMSTrTrack*)_next; }

  /// Build index vector (_iHits) from hits vector (_Hits)
  void BuildHitsIndex();

  // Access functions
  int getpattern() const { return _Pattern; }
  int GetPattern() const { return _Pattern; }
  int GetNhits  () const { return _Nhits;   }

  int GetPatternX () const { return _PatternX;  }
  int GetPatternY () const { return _PatternY;  }
  int GetPatternXY() const { return _PatternXY; }
  int GetNhitsX   () const { return _NhitsX;    }
  int GetNhitsY   () const { return _NhitsY;    }
  int GetNhitsXY  () const { return _NhitsXY;   }

  /// Check if _TrackPar[id]
  bool ParExists(int id) { return (_TrackPar.find(id) != _TrackPar.end()); }

  /// Get TrTrackPar with id
  AMSTrTrackPar &GetPar(int id = DefaultFitID) {
    if (ParExists(id)) return _TrackPar[id];
    cerr << "Warning in AMSTrTrack::GetPar, Parameter not exists " 
	 << id << endl;
    int aa=10/0; //for debug
    static AMSTrTrackPar parerr;
    return parerr;
  }

  // Get fitting parameter with a key as Fitting method ID
  bool   FitDone    (int id = DefaultFitID) { return GetPar(id).FitDone;  }
  int    GetHitBits (int id = DefaultFitID) { return GetPar(id).HitBits;  }
  double GetChisqX  (int id = DefaultFitID) { return GetPar(id).ChisqX;   }
  double GetChisqY  (int id = DefaultFitID) { return GetPar(id).ChisqY;   }
  double GetChisq   (int id = DefaultFitID) { return GetPar(id).Chisq;    }
  int    GetNdofX   (int id = DefaultFitID) { return GetPar(id).NdofX;    }
  int    GetNdofY   (int id = DefaultFitID) { return GetPar(id).NdofY;    }
  double GetRigidity(int id = DefaultFitID) { return GetPar(id).Rigidity; }
  double GetErrRinv (int id = DefaultFitID) { return GetPar(id).ErrRinv;  }
  AMSPoint GetP0    (int id = DefaultFitID) { return GetPar(id).P0;       }
  AMSDir   GetDir   (int id = DefaultFitID) { return GetPar(id).Dir;      }
  // PZ FIXME 
  bool getres(int layer,AMSPoint &pnt,int fitid=DefaultFitID );

  // Aliases
  double GetP0x  (int id = DefaultFitID) { return GetP0(id).x(); }
  double GetP0y  (int id = DefaultFitID) { return GetP0(id).y(); }
  double GetP0z  (int id = DefaultFitID) { return GetP0(id).z(); }
  double GetChi2 (int id = DefaultFitID) { return GetChisq(id);  }
  double GetTheta(int id = DefaultFitID) { return GetDir(id).gettheta(); }
  double GetPhi  (int id = DefaultFitID) { return GetDir(id).getphi();   }

  /// Get track entry point (first layer of the fitting)
  AMSPoint GetPentry(int id = DefaultFitID);

  /// Test HitBits
  bool TestHitBits(int layer, int id = DefaultFitID) { 
    return (GetHitBits(id) & (1 << (trconst::maxlay-layer)));
  }

  /// Get a hit at i in the vector
  AMSTrRecHit *GetHit(int i);

  /// Get tan(theta) on XZ projection
  double GetThetaXZ(int id = DefaultFitID) { 
    AMSDir dir = GetDir(id);
    return (dir.z() != 0) ? dir.x()/dir.z() : 0;
  }
  /// Get tan(theta) on YZ projection
  double GetThetaYZ(int id = DefaultFitID) { 
    AMSDir dir = GetDir(id);
    return (dir.z() != 0) ? dir.y()/dir.z() : 0;
  }

  /// Perform 3D fitting with the method specified by ID
  /*!
    \param[in] id    Fitting method, defined as a combination of
                     one of EFitMethods and any of EFitOptions, 
		     e.g. ID= 0x31= kChoutko+kMultScat+kUpperHalf. 
		     Fitting parameters will be overwritten on the map[ID] 
		     unless trfit is specified. 

    \param[in] layer Layer to be excluded in the fitting (if specified) 

    \param[in] trfit TrFit object to which fitting parameters are kept. 
                     If it is not specified, map[ID] is overwritten. 

    \param[in] err   Fitting errors (0:x,1:y,2:z) to be used. 
		     If they are not specified, default values 
		     are taken from TRFITFFKEY.ErrX, ErrY, and ErrZ

    \param[in] mass  Particle mass assumed for multiple scattering. 
                     Ignored if kMultScat option is not set.

    \param[in] chrg  Particle charge (in unit of e) assumed for multiple 
                     scattering. Ignored if kMultScat option is not set.
    
    \return          Chisq(X+Y)/Ndof if succeeded, or -1 if failed
   */
  
  double FitP(int id = kChoutko,int part=14){
    int charge=1;
    double mass=0.938272297;
    if(part==14||part==15)   {charge=1;mass=0.938272297;}
    else if(part==2||part==3){charge=1;mass=0.000510998;}
    else if(part==5||part==6){charge=1;mass=0.105658357;}
    else if(part==47)        {charge=2;mass=3.727417;}
    else return -1;
    return  Fit(id ,-1,  0,mass,  1);
  }    
  double Fit(int id = kChoutko,
	     int layer = -1, const double *err = 0, 
	     double mass = 0.938272297, double chrg = 1);

  /// Fill residual with TrFit if specified, if not with the last fit 
  void FillResidual();

  /// Perform simple fitting with a constant position error of 0.03 cm
  double SimpleFit(void) {
    static const double err[3] = { 0.03, 0.03, 0.03 };
    return Fit(kSimple, -1,  err);
  }
  /// Perform simple fitting with a constant position error
  double SimpleFit(double *err) {
    return Fit(kSimple, -1,  err);
  }
  
  bool interpolateToZ(double Zint,AMSPoint &pnt,int id= DefaultFitID);
  /// Interpolation
  void interpolate(AMSPoint pnt, AMSDir dir,  AMSPoint &P1, 
		   number &theta, number &phi, number &length, 
		   int id = DefaultFitID);
  
  /// Interpolation to cylinder surface: TO_BE_IMPLEMENTED
  bool interpolateCyl(AMSPoint  pnt,  AMSDir dir, number rad, number idir, 
		      AMSPoint & P1, number & theta, number & phi, 
		      number & length){
    P1[0]=P1[1]=P1[2]=theta=phi=length=0;
    return true;
  }


  /// Print tracks
  static void print();
  void myprint();

  /// For compatibility with vtx.C
  double getpichi2() { return 1.; }

  /// For compatibility with ecalcalib.C
  int GeaneFitDone   () { return FitDone(kGEANE); }

  /// For compatibility with ecalcalib.C
  /*! "Advanced fit" is assumed to perform all the fitting: 
   *   1: 1st harf, 2: 2nd harf, 6: with nodb, 4: "Fast fit" ims=0 and 
   *   5: Juan with ims=1 */
  int AdvancedFitDone() { 
    return FitDone(kChoutko|kUpperHalf) & FitDone(kChoutko|kLowerHalf) & 
           FitDone(kChoutko) & FitDone(kJuan);
  }
  /// For compatibility with ecalcalib.C
  int DoAdvancedFit();
  
  /// For compatibility with Gbatch : TO_BE_IMPLEMENTED
  void getParAdvancedFit(number& GChi2,  number& GRid, number&   GErr,
			 number& GTheta, number& GPhi, AMSPoint& GP0,
			 number HChi2 [2], number HRid[2], number  HErr[2], 
			 number HTheta[2], number HPhi[2], AMSPoint HP0[2]) {
    GChi2 = GRid = GErr = GTheta = GPhi = 0;
    GP0 = AMSPoint();
    for (int i = 0; i < 2;i++) {
      HChi2[i] = HRid[i] = HErr[i] = HTheta[i] = HPhi[i] = 0;
      HP0[i] = AMSPoint();
    }
  }

  /// For compatibility with Gbatch
  void getParFastFit(number& Chi2,  number& Rig, number& Err, 
		     number& Theta, number& Phi, AMSPoint& X0) {
    /// FastFit is assumed as normal (Choutko) fit without MS
    int id = kChoutko;
    Chi2 = GetChisq(id); Rig = GetRigidity(id); Err = GetErrRinv(id); 
    Theta = GetTheta(id); Phi = GetPhi(id); X0 = GetP0(id);
  }

  void *operator new   (size_t t) { return TObject::operator new(t); }
  void  operator delete(void  *p) { TObject::operator delete(p); p = 0; }
  void _printEl(std::ostream&);
  void _printEl(std::string&);

protected:
  void _printEl(void) {};
  void _copyEl (){}
  void _writeEl(){}

  /// ROOT definition
  ClassDef(AMSTrTrack, 1)
};

typedef AMSTrTrack TrTrackR;
typedef AMSTrTrackPar TrTrackParR;


/// Class to handle some error messages
class AMSTrTrackError{

private:
  char msg[256];

public:
  AMSTrTrackError(char*);
  char *getmessage();
};

#endif
