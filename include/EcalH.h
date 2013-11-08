//  $Id: EcalH.h,v 1.2 2013/11/08 18:07:07 shaino Exp $
#ifndef __EcalHR__
#define __EcalHR__

//////////////////////////////////////////////////////////////////////////
///
///\class EcalHR
///\brief A class to manage hadron shower in Ecal
///
///\date  2013/11/06 SH  Introduction of the class
///
///$Date: 2013/11/08 18:07:07 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrElem.h"
#include "point.h"
#include <vector>
#include <cmath>

class AMSEventR;
class TrTrackR;
class TH2;
class TF1;

class EcalHR : public TrElem {

public:
  enum { NL = 18,           ///< Number of layers
	 NC = 72,           ///< Number of cells
	 AP = 20            ///< Indicator for Apex layer
  };

protected:
  // --- data members ---
  AMSPoint _tpnt;           ///< Track extrapolated position
  AMSDir   _tdir;           ///< Track extrapolated direction
  float    _tcsq;           ///< GetChisq(1, 100, 1) of the track

  int      _nhit;           ///< Number of EcalHits
  int      _hidx [NL][5];   ///< Index  of EcalHits for S5 cells
  int      _apex [3];       ///< Apex for different criteria
  float    _ecen [3];       ///< S1,3,5 energy
  float    _elast[3];       ///< S1,3,5 energy in the last two layers (L16,17)
  float    _lmax [3];       ///< Shower max (in layer)   for S1,3,5 energy
  float    _lsum [3];       ///< Sum of longitudinal fit for S1,3,5 energy
  float    _etot;           ///< Total Edep sum

  // --- Used only for mini-DST ---
  std::vector<int>   _hid;  ///< EcalHit ID (Layer-Cell-Proj)
  std::vector<float> _hxy;  ///< EcalHit coo in projection (x or y)
  std::vector<float> _hz;   ///< EcalHit coo in z
  std::vector<float> _de;   ///< Measured energy (MeV, all corrections)
  std::vector<float> _sc;   ///< Saturation1 correction applied (MeV)
  std::vector<float> _ac;   ///< Attenuation correction applied (MeV)

public:
  // --- static parameters ---
  static float fZref;       ///< Reference Z coordinate
  static float fZtop;       ///< Ecal top
  static float fZbot;       ///< Ecal bottom
  static float fXmin;       ///< Ecal fiducial volume in X
  static float fXmax;       ///< Ecal fiducial volume in X
  static float fYmin;       ///< Ecal fiducial volume in Y
  static float fYmax;       ///< Ecal fiducial volume in Y
  static float fCell;       ///< Ecal cell pitch

  static float fEmin;       ///< Minimum energy sum (GeV) needed for FitL
  static float fEthd[3];    ///< Energy threshold (MeV) for apex

  // --- constructors ---
  EcalHR(const EcalHR *org);
  EcalHR();
 ~EcalHR();

  // --- accessors ---

  AMSPoint Tpnt(void)    const { return _tpnt; }
  AMSDir   Tdir(void)    const { return _tdir; }
  float    Tcsq(void)    const { return _tcsq; }

  AMSPoint TrkP(float z) const { return _tpnt+_tdir/_tdir.z()*(z-_tpnt.z()); }
  float    TrkX(float z) const { return TrkP(z).x(); }
  float    TrkY(float z) const { return TrkP(z).y(); }

  int   Nhit (void)  const { return _nhit; }
  int   Hidx (int i,
	      int j) const { return (0 <= i && i < NL && 
				     0 <= j && j < 5) ? _hidx [i][j] : -1; }
  int   Apex (int i) const { return (0 <= i && i < 3) ? _apex [i] : -1; }
  float Ecen (int i) const { return (0 <= i && i < 3) ? _ecen [i] : -1; }
  float Elast(int i) const { return (0 <= i && i < 3) ? _elast[i] : -1; }
  float Lmax (int i) const { return (0 <= i && i < 3) ? _lmax [i] : -1; }
  float Lsum (int i) const { return (0 <= i && i < 3) ? _lsum [i] : -1; }
  float Etot (void)  const { return _etot; }
  float Ecell(int i,
	      int j) const { return dE(Hidx(i, j)); }

  bool    Chk(int i) const { return (0 <= i && i < _hid.size()); }
  int     Hid(int i) const { return Chk(i) ? _hid.at(i) : -1; }
  int    Proj(int i) const { return Chk(i) ? Hid(i)%10  : -1; }
  int    Cell(int i) const { return Chk(i) ?(Hid(i)/10)    %100 : -1; }
  int   Plane(int i) const { return Chk(i) ?(Hid(i)/10/100)%100 : -1; }
  int   ClsId(int i) const { return Chk(i) ? Hid(i)/10/100 /100 : -1; }

  static int Hid(int plane, int cell, int proj, int cid = 0) {
    return cid*100*100*10+plane*100*10+cell*10+proj;
  }

  float Hxy(int i) const { return  Chk(i) ? _hxy.at(i) : 0; }
  float Hz (int i) const { return  Chk(i) ? _hz .at(i) : 0; }
  float dE (int i) const { return (Chk(i) && _de.at(i) == 
				             _de.at(i)) ? _de.at(i) : 0; }
  float dXt(int i) const { return (Proj(i) == 0) ? Hxy(i)-TrkX(Hz(i)) 
                                                 : Hxy(i)-TrkY(Hz(i)); }

  // --- methods ---

  /*!
    \brief Clear data member
    \param[in] mode  0: clear only vectors  1: clear all
  */
  void Clear(int mode = 1);

  /*!
    \brief Get chisquare of the residual between S1 hits and track
    \param[in] sigma in unit of cell
    \param[in] emin  min. Edep (MeV) to be included
    \param[in] norm  1:normalized by Nlayer or 0:not
  */
  float GetChisq(float sigma = 1, float emin = 100, int norm = 1) const;

  /*!
    \brief Process current event
    \retval  0 success
    \retval  1 not enough Ecal hits
  */
  int Process(void);

  /*!
    \brief Fit longitudinal shower shape
    \param[in]  s    use 0:S1, 1:S3, 2:S5 energy
    \param[out] par  fitting parameters
    \param[out] err  fitting errors
    \retval  0 success
    \retval -1 failure
  */
  int FitL(int s, float *par, float *err);

  // --- static methods ---

  /// Check if a linear track is inside Ecal fiducial volume or not
  static bool IsInside(AMSPoint pnt, AMSDir dir);

  /// Check if a Tracker track is inside Ecal fiducial volume or not
  static bool IsInside(TrTrackR *track);

  /// Get number of EcalHR object in the current event
  static int GetN(void);

  /// Get i-th EcalHR pointer in the current event
  static EcalHR *Get(int i);

  /// Build EcalHR object for the current event
  static int Build(int clear = 1);

  /*
    \brief Longitudinal shower fitting function
    \param[in] norm  Energy deposit (MeV) at shower max
    \param[in] apex  Shower start layer
    \param[in] lmax  Shower max   layer
  */
  static TF1 *Lfun(float norm, float apex, float lmax);


  // --- TrElem overoverriders ---

protected:
  void _PrepareOutput(int opt  = 0);

public:
  void          Print(int opt  = 0);
  const char   *Info (int iref = 0);
  std::ostream &putout(std::ostream &ostr = std::cout);

  ClassDef(EcalHR, 1)
};

#ifndef __ROOTSHAREDLIBRARY__

class AMSEcalH : public EcalHR, public AMSlink {

public:
  AMSEcalH() : EcalHR(), AMSlink() {}
  AMSEcalH(const AMSEcalH &org) : EcalHR(org), AMSlink(org) {}
 ~AMSEcalH() {}
 
  void _copyEl ();
  void _writeEl();
  void _printEl(std::ostream &str) { putout(str); }

  AMSEcalH *next() { return (AMSEcalH *)_next; }
};

#endif
#endif
