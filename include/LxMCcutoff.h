//  $Id$
#ifndef LXMCCUTOFF_H
#define LXMCCUTOFF_H

//////////////////////////////////////////////////////////////////////////
///
///\class LxMCcutoff || QBincutoff
///\brief A class to simulate cutoff on MC data
///
///\date  2014/07/17 L.Derome
///\date  2014/10/29 L.Derome new version
///\date  2014/12/01 Q.Yan QBincutoff introduced(for bin-by-bin unfolding||MC bin-by-bin exposure-time reweight)
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TH2.h"

class LxMCcutoff {
public:
  enum eRcutDist { _RcutVert, _RcutMax25, _RcutMin25, _RcutAcc25, _RcutAccC25};
  /* Filename should contain the TH2F with the Rcut vs Rmax distribution 
   * h is used to get the binning
   * SafetyFactor is the safety factor used to compute the threshold applied to Rrec  
   * dist  is the method used to estimate the  Rcut vs Rmax distribution:
   * _RcutVert : vertical direction in AMS is used to get Rcut
   * _RcutMin25 : Min value within 25 deg is used to get Rcut
   * _RcutMax25 : Max value within 25 deg is used to get Rcut
   * _RcutAcc25 : Isotropically distributed directions within 25 deg is used to get Rcut
   */
LxMCcutoff (const char *FileName, TH1D *h, double safetyfactor=.2, LxMCcutoff::eRcutDist dist = LxMCcutoff::_RcutAcc25 );
  virtual ~LxMCcutoff ();
  void SetExperimentalCutoff(double ExperimentalCutoff_val) {ExperimentalCutoff = ExperimentalCutoff_val;}
  double GetRcut();
  double GetRmax(double Rcut);
  TH2F *GetEff(double N0 = 1000000);
  double GetWeight(double Rgen,double Rrec,int randseed=-1);

  TH2F* GetRcutI()  { return RcutI; }
  TH1D* GetExp()    { return hExp; } 
  TH1D* GethRcut1D(){ return hRcut1D; }

  static LxMCcutoff *GetHead() { return Head; }
  static void  SetBinning(TH1 *hbin) { Hbin = (TH1D *)hbin; }
  static TH1D *GetBinning(void) { return Hbin; }

public:
  double SafetyFactor;
  LxMCcutoff::eRcutDist RcutDist;
  bool ExperimentalCutoff;

protected:
  TH2F* RcutI; 
  TH1D* hExp ;
  TH1D* hRcut1D;

  static LxMCcutoff *Head;
  static TH1D *Hbin;
  
public:
  ClassDef(LxMCcutoff, 2);
};

//////////////////////////////////////////////////////////////////////////
#include "TSpline.h"

class QBincutoff {

public:
   QBincutoff();

   QBincutoff(const char *fname);
   QBincutoff(TH1* hexp); 
/// Expsure-Time(Cutoff) weight for flux based on BinLowEdge
/*!
 *   @param[in] lbv:          meausred rigidity(R)-BinLowEdge[GV] 
 *   @param[in] Rgen:         flux generated rigidity[GV] 
 *   @param[in] margin:       cutoff safety factor used for bin-by-bin(1.2cutoff...)
 *   @param[in] cutoffoffset: ture cutoff=meausred cutoff/cutoffoffset 
 */
   double GetMCTimeWeight(double lbv,double Rgen,double margin=1.2,double cutoffoffset=1.2);
/// Expsure-Time(Cutoff) weight for MC event
/*!
  *   @param[in] hev:          MC events Histogram(meausred rigidity[R]-Xaxis)
  *   @param[in] Rrec:         MC reconstructed rigidity[GV]
  *   @param[in] Rgen:         MC generated rigidity[GV] 
  *   @param[in] margin:       cutoff safety factor used for bin-by-bin(1.2cutoff...)
  *   @param[in] cutoffoffset: ture cutoff=meausred cutoff/cutoffoffset 
*/
   double GetMCTimeWeight(TH1 *hev,double Rrec,double Rgen,double margin=1.2,double cutoffoffset=1.2); 

private:
   int Init();
   int Clear();
   double qmargin;
   double qcutoffset;
   TH1 *qhev;
   TH1 *qhexp;
   TSpline3 *qsexp;
    ~QBincutoff(){Clear();}

  ClassDef(QBincutoff, 1);  
};

#endif /* end of include guard: LXMCCUTOFF_H */
