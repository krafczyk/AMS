//  $Id$
#ifndef LXMCCUTOFF_H
#define LXMCCUTOFF_H

//////////////////////////////////////////////////////////////////////////
///
///\class LxMCcutoff
///\brief A class to simulate cutoff on MC data
///
///\date  2014/07/17 L.Derome
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TH2.h"

class LxMCcutoff {
public:
  enum eRcutDist { _RcutVert, _RcutMin25, _RcutIso25};
  /*
   * Filename should contain the TH2F with the Rcut vs Rmax distribution 
   * h is used to get the binning
   * SafetyFactor is the safety factor used to compute the threshold applied to Rrec  
   * dist  is the method used to estimate the  Rcut vs Rmax distribution:
   * _RcutVert : vertical direction in AMS is used to get Rcut
   * _RcutMin25 : Min direction within 25 deg is used to get Rcut
   * _RcutIso25 : Isotropically distributed directions within 25 deg is used to get Rcut (not implemented yet)
   */
  LxMCcutoff (const char *FileName, TH1D *h, double safetyfactor=.2, LxMCcutoff::eRcutDist dist = LxMCcutoff::_RcutVert );
  virtual ~LxMCcutoff ();
  double GetWeight(double Rgen,double Rrec,int randseed=-1);
  TH1D* GetExp();

  static LxMCcutoff *GetHead() { return Head; }

public:
  double SafetyFactor;
  LxMCcutoff::eRcutDist RcutDist;

protected:
  TH2F* RcutI; 
  TH1D* hExp0;
  TH1D* hExp ;
  TH1D* Rcut1D;

  static LxMCcutoff *Head;
  
public:
  ClassDef(LxMCcutoff, 1);
};


#endif /* end of include guard: LXMCCUTOFF_H */
