// $Id: TrTasPar.h,v 1.1 2009/12/17 16:11:12 shaino Exp $

#ifndef __TrTasPar__
#define __TrTasPar__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasPar.h
///\brief Header file of TrTasPar class
///
///\class TrTasPar
///\brief The TAS parameters data base
///\ingroup tkdbc
///
///\date  2009/12/10 SH  First version
///$Date: 2009/12/17 16:11:12 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class TrTasPar : public TObject {

public:
  enum { NLAD = 8*2, NADR = 4, NPAR = 5, NLAS = 4 };

public:
  /// Laser intensity (mA)
  int   _ival;
  /// LDDR (Laser Diode DriveR)
  int   _lddr;
  /// TkId
  int   _tkid[NLAD];
  /// Minimum ADC address of signal window
  int   _amin[NLAD*NADR];
  /// Maximum ADC address of signal window
  int   _amax[NLAD*NADR];
  /// Signal fitting parameters
  float _fpar[NLAD*NADR*NPAR];
  /// Residual mean offset (cm)
  float _resm[NLAD*NADR];
  /// Laser center position in X (cm) in the global coordinate
  float _lasx[NLAS];
  /// Laser center position in Y (cm) in the global coordinate
  float _lasy[NLAS];

public:
  TrTasPar() : _ival(0), _lddr(0) {}
 ~TrTasPar() {}

  static TrTasPar *Head;

  int GetIlad(int tkid) const;
  int GetIpk (int ilad, int addr) const;

  int GetIval() const { return _ival; }
  int GetLDDR() const { return _lddr; }

  int GetTkId(int i) const { return (0 <= i && i < NLAD) ? _tkid[i] : 0; }

  int GetAmin(int i, int j) const 
  { return (0 <= i && i < NLAD && 0 <= j && j < NADR) ? _amin[i*NADR+j] : 0; }
  int GetAmax(int i, int j) const 
  { return (0 <= i && i < NLAD && 0 <= j && j < NADR) ? _amax[i*NADR+j] : 0; }

  float GetFpar(int i, int j, int k) const
  { return (0 <= i && i < NLAD && 0 <= j && j < NADR && 0 <= k && k < NPAR) 
      ? _fpar[(i*NADR+j)*NPAR+k] : 0; }

  void SetDisabled(int i, int j) 
  { if (0 <= i && i < NLAD && 0 <= j && j < NADR) 
    _amin[i*NADR+j] = _amax[i*NADR+j] = -1;
  }

  float GetResm(int i, int j) const 
  { return (0 <= i && i < NLAD && 0 <= j && j < NADR) ? _resm[i*NADR+j] : 0; }

  void SetResm(int i, int j, float res)
  { if (0 <= i && i < NLAD && 0 <= j && j < NADR) _resm[i*NADR+j] = res; }

  float GetLasx(int i) const { return (0 <= i && i < NLAS) ? _lasx[i] : 0; }
  float GetLasy(int i) const { return (0 <= i && i < NLAS) ? _lasy[i] : 0; }

  /// Initialize TrTasPar, called from TrTasDB::Init
  int Init(int ival, int lddr, const char *dname = "dat");

  /// Output contents into a text file, called from TrTasDB::Init
  void Output(const char *fname = 0) const;

  /// Print information
  /*
   * option= "rpk": print signal ranges and saturaton flags
   *         "tkp": print fitting parameters
   *      default : print TkID list and laser positions
   */
  void Print(Option_t *option = "") const;

  /// Returns linear buffer size
  static int GetLinearSize(void);

  /// Copy contents to a linear vector 
  int Par2Lin(float *ptr);

  /// Get contents to a linear vector 
  int Lin2Par(float *ptr);

  ClassDef(TrTasPar, 1);
};

#endif
