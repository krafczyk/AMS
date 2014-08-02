//  $Id$
#ifndef __EcalHR__
#define __EcalHR__

//////////////////////////////////////////////////////////////////////////
///
///\class EcalHR
///\brief A class to manage hadron shower in Ecal
///
///\date  2013/11/06 SH  Introduction of the class
///\date  2013/11/08 SH  Methods implemented
///\date  2013/11/10 SH  Parameters added
///
///$Date$
///
///$Revision$
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

#ifdef __ROOTSHAREDLIBRARY__
public:
  // --- User fuctions ---
  /*!
    \brief Get hadron shower parameters of current event
    \param[in]  z         Integer charge estimated by TrTrack (1 or 2)
    \param[out] rrec      Estimated rigidity (GV)
    \param[out] smax      Estimated shower max (in unit of Ecal layer)
    \param[out] tr_chisq  TrTrack-EcalShower matching chisquare
    \retval  0  success
    \retval -1  error
  */
  static int Get(int z, float &rrec, float &smax, float &tr_chisq);

  /*!
    \brief Get MIP energy deposit from 3 adjacent cells (S3)
    \param[out] p_entry   Entry point
    \param[out] p_last    Point before APEX (the last layer before the shower)
    \return     S3 Energy deposit in GeV from p_entry and p_last
    \retval -1  error
  */
  static double GetMipEdep(AMSPoint &p_entry, AMSPoint &p_last);
#endif

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
  float    _ecap [3];       ///< S1,3,5 energy (after apex[1])
  float    _elast[3];       ///< S1,3,5 energy in the last two layers (L16,17)
  float    _lapex[3];       ///< Shower apex (in layer)  for S1,3,5 energy
  float    _lmax [3];       ///< Shower max  (in layer)  for S1,3,5 energy
  float    _ltop [3];       ///< Shower max energy (MeV) for S1,3,5 energy
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

  static float fEmip;       ///< Minimum energy dep (MeV) needed for Tr-match
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
  float Ecap (int i) const { return (0 <= i && i < 3) ? _ecap [i] : -1; }
  float Elast(int i) const { return (0 <= i && i < 3) ? _elast[i] : -1; }
  float Lapex(int i) const { return (0 <= i && i < 3) ? _lapex[i] : -1; }
  float Lmax (int i) const { return (0 <= i && i < 3) ? _lmax [i] : -1; }
  float Ltop (int i) const { return (0 <= i && i < 3) ? _ltop [i] : -1; }
  float Lsum (int i) const { return (0 <= i && i < 3) ? _lsum [i] : -1; }
  float Etot (void)  const { return _etot; }
  float Ecell(int i,
	      int j) const { return dE(Hidx(i, j)); }
  float ES1  (int i) const { return Ecell(i, 2); }
  float ES3  (int i) const { return Ecell(i, 1)+Ecell(i, 2)+Ecell(i, 3); }

  bool    Chk(int i) const { return (0 <= i && i < int(_hid.size())); }
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
    \brief Fill S energy for each layer
    \param[in]  s     use 0:S1, 1:S3, 2:S5 energy
    \param[out] es    S1/3/5 energy at each layer
    \retval     imax  layer number with maximum es value
  */
  int FillS(int s, float es[NL]);

  /*!
    \brief Fit longitudinal shower shape
    \param[in]  s       use 0:S1, 1:S3, 2:S5 energy
    \param[out] par[3]  fitting parameters
    \param[out] err[3]  fitting errors
    \param[in]  method  1:Minuit 2:Analytical 3:TH1F(ROOTSHARED only)
    \retval  0 success
    \retval -1 failure
  */
  int FitL(int s, float par[3], float err[3], int method = 1);

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

  /// Peak locator with pol2 from three bins
  static double Peak(double x1, double y1, double x2, double y2,
		     double x3, double y3);

  // --- TrElem overoverriders ---

protected:
  void _PrepareOutput(int opt  = 0);

public:
  void          Print(int opt  = 0);
  const char   *Info (int iref = 0);
  std::ostream &putout(std::ostream &ostr = std::cout);

  ClassDef(EcalHR, 2)
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
