//  $Id$
#ifndef __GausBF__
#define __GausBF__

//////////////////////////////////////////////////////////////////////////
///
///\file  GausBF.h
///\brief Header file of GausBF class
///
///\class GausBF
///\brief Gaussianized magnetic field model
///
///\date  2014/08/10 SH  First version
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include "TObject.h"
#include "TString.h"

class TkDBc;
class TF1;
class TGraph;

class GausBF : public TObject {
public:
  enum { Ns1 = 98, Ns9 = 11*16, Np = 3*12 };

protected:
  Int_t    _np [Ns1*Ns9];
  Double_t _par[Ns1*Ns9*Np];

  static Double_t _Zref;
  static GausBF  *_Head;

  Double_t *GetPar(Int_t is1, Int_t is9);

public:
  /// Constructor
  GausBF(void);
  /// Destructor
 ~GausBF();

  /// Initialize parameters
  void Init (void);
  /// Check and fix parameters
  void Check(void);
  /// Print data contains
  void Print(Option_t *option = "") const;

  /// Get parameter pointer
  const Double_t *GetPar(Int_t is1, Int_t is9) const;
  /// Get parameter 
  Double_t  GetPar(Int_t is1, Int_t is9, Int_t ip) const;
  /// Set parameter 
  void      SetPar(Int_t is1, Int_t is9, Int_t ip, Double_t par);

  /// Get Magnetic field function
  TF1 *GetBf (Int_t is1, Int_t is9) const;
  /// Get Deflection function
  TF1 *GetDf (Int_t is1, Int_t is9, Double_t rgt) const;
  /// Get Propagation function
  TF1 *GetPr (Int_t is1, Int_t is9, Double_t rgt) const;

  static ::TkDBc *TkDBc();

  static AMSPoint GetP1(Int_t is1);
  static AMSPoint GetP9(Int_t is9);
  static Int_t    Find (AMSPoint p1, AMSPoint p2);

  static Double_t Zref() { return _Zref; }
  static GausBF  *Head();
  static GausBF  *Load(const char *fname);

  static TF1     *Bfit(TGraph *gr);
  static Double_t Gfit(Int_t is1, Int_t is9, Double_t rgt, TGraph *gr = 0,
		                                           TGraph *gb = 0);
  static TString FB(void);
  static TString SE(Int_t i);
  static TString SV(Int_t i);

  ClassDef(GausBF, 1);
};

#endif
