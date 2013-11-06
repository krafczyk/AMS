//  $Id: EcalH.h,v 1.1 2013/11/06 20:24:13 shaino Exp $
#ifndef __EcalHR__
#define __EcalHR__

//////////////////////////////////////////////////////////////////////////
///
///\class EcalHR
///\brief A class to manage hadron shower in Ecal
///
///\date  2013/11/06 SH  Introduction of the class
///
///$Date: 2013/11/06 20:24:13 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrElem.h"
#include "point.h"
#include <vector>
#include <cmath>

class AMSEventR;
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

  int      _nhit;           ///< Number of EcalHits
  int      _hidx [5][NL];   ///< Index  of EcalHits for S5 cells
  int      _apex [3];       ///< Apex for different criteria
  float    _ecen [3];       ///< S1,3,5 energy
  float    _elast[3];       ///< S1,3,5 energy in the last two layers (L16,17)
  float    _lmax [3];       ///< Shower max (layer) for S1,3,5 energy

  // --- Used only for mini-DST ---
  std::vector<int>   _hid;  ///< EcalHit ID (Layer-Cell-Proj)
  std::vector<float> _hxy;  ///< EcalHit coo in projection (x or y)
  std::vector<float> _hz;   ///< EcalHit coo in z
  std::vector<float> _de;   ///< Measured energy (MeV, all corrections)
  std::vector<float> _sc;   ///< Saturation1 correction applied (MeV)
  std::vector<float> _ac;   ///< Attenuation correction applied (MeV)

  // --- static parameters ---
public:
  static double fZref;      ///< Reference Z coordinate
  static double fZtop;      ///< Ecal top
  static double fZbot;      ///< Ecal bottom
  static double fXmin;     ///< Ecal fiducial volume in X
  static double fXmax;     ///< Ecal fiducial volume in X
  static double fYmin;     ///< Ecal fiducial volume in Y
  static double fYmax;     ///< Ecal fiducial volume in Y

  // --- constructors ---
  EcalHR(const EcalHR *org) { *this = *org; }
  EcalHR() { Clear(); }
 ~EcalHR() {}

  // --- methods ---
  AMSPoint TrkP(double z) const { return _tpnt+_tdir/_tdir.z()*(z-_tpnt.z()); }
  double   TrkX(double z) const { return TrkP(z).x(); }
  double   TrkY(double z) const { return TrkP(z).y(); }

  bool IsInside(void) const;

  int   Nhit (void)  const { return _nhit; }
  int   Hidx (int i,
	      int j) const { return (0 <= i && i < NL && 
				     0 <= j && j < 5) ? _hidx [i][j] : -1; }
  int   Apex (int i) const { return (0 <= i && i < 3) ? _apex [i] : -1; }
  float Ecen (int i) const { return (0 <= i && i < 3) ? _ecen [i] : -1; }
  float Elast(int i) const { return (0 <= i && i < 3) ? _elast[i] : -1; }
  float Lmax (int i) const { return (0 <= i && i < 3) ? _lmax [i] : -1; }

  bool    Chk(int i) const { return (0 <= i && i < _hid.size()); }
  int     Hid(int i) const { return Chk(i) ? _hid.at(i) : -1; }
  int    Proj(int i) const { return Chk(i) ? Hid(i)%10  : -1; }
  int    Cell(int i) const { return Chk(i) ?(Hid(i)/10)    %100 : -1; }
  int   Plane(int i) const { return Chk(i) ?(Hid(i)/10/100)%100 : -1; }
  int   ClsId(int i) const { return Chk(i) ? Hid(i)/10/100 /100 : -1; }

  float   Hxy(int i) const { return  Chk(i) ? _hxy.at(i) : 0; }
  float   Hz (int i) const { return  Chk(i) ? _hz .at(i) : 0; }
  float   dE (int i) const { return (Chk(i) && _de.at(i) == 
				             _de.at(i)) ? _de.at(i) : 0; }
  float dXt(int i) const { return (Proj(i) == 0) ? Hxy(i)-TrkX(Hz(i)) 
                                                 : Hxy(i)-TrkY(Hz(i)); }

  void Clear(int mode = 1);

  int Process(void);

  static int Build(void);
  static TF1 *Lfun(double norm, double apex, double ldep);


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
