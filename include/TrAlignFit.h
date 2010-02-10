//  $Id: TrAlignFit.h,v 1.2 2010/02/10 12:07:07 shaino Exp $
#ifndef __TrAlignFit__
#define __TrAlignFit__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrAlignFit.h
///\brief Header file of TrAlignFit class
///
///\class TrAlignFit
///\brief Tracker alignment fit
///\ingroup gbint
///
///\date  2007/04/02 SH  First test version
///$Date: 2010/02/10 12:07:07 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "point.h"

#include "TFile.h"
#include "TDirectory.h"
#include "TObjArray.h"

#include <map>

class TH1D;
class TH2F;
class TH2;
class TH3;

class TrAlignObject {

public:
  enum { Npar = 2, Ndim = 3, Nalg = 6 };
  enum { XYcorr1 = 0001, Zcorr1 = 0002, dYXcorr = 0004, 
	 XYcorr2 = 0010, Zcorr2 = 0020, dZXcorr = 0040,
	 XYcorr3 = 0100 };
	 
  Float_t cool[Ndim];
  Int_t   fixp[Nalg];

  Double_t param[Ndim];
  Double_t perr [Ndim];
  Double_t phi;
  Double_t pitch;
  Double_t roll;

  static TObjArray sharray;

  static Int_t  ProfMode;
  static Bool_t fLcommon;

protected:
  Int_t tkid;

  TObjArray harray;

  TDirectory *ldir;
  TDirectory *cdir;

public:
  TrAlignObject(Int_t id);
 ~TrAlignObject();

  Int_t GetTkId(void) const { return tkid; }

  void Reset(void);

  void InitHists (TDirectory *dir);
  void ResetHists(void);
  void FillHist  (Int_t i, Double_t x, Double_t y);
  void ProcHists (Int_t bits);

  void SigmaProj(TH2 *hist, Double_t &par, Double_t &err);

  AMSPoint GetCoord(Double_t x, Double_t y, Double_t z);

  static void  InitSHists(void);
  static TH1D *Profile(TH2 *hist);
  static TH2F *Project(TH3 *hist, Int_t ibx1, Int_t ibx2);
  static Int_t Fit2D(Int_t n, Double_t *x, Double_t *y, 
		              Double_t *w, Double_t *p); 
  static Int_t Inv3x3(Double_t mtx[3][3]);
};


typedef std::map<int,TrAlignObject*>::const_iterator aligIT;

class TrAlignFit : public TObject {

public:
  enum { Nplan = 8, Nlad = 15, Npar = 2, 
	 Ndim = 11, Narr = 3 };

  static TString fNameOut;

  enum { kCosmicRay = 1, kTestBeam = 2 };
  static Int_t fMode;

protected:
  Int_t    fMaxCases;     //!
  Int_t    fNbuf;         //!
  Int_t    fNcbf;         //!
  Int_t    fNcases;       //!
  Float_t *fArray;        //!
  char    *fCrray;        //!

  Float_t  fSigma[Npar];  //!

  std::map<int,TrAlignObject*> fAlignMap;  //!

  TFile      *fFile;  //!
  TDirectory *fDir;   //!

  Float_t fSfactor[1+Nplan+Nplan*(Nplan-1)][Nplan];   //!

  Int_t    fIcase;                 //!
  Double_t fResidual[2][Nplan];    //!
  Double_t fGradient[2];           //!
  Double_t fChisq[2];              //!

  static Int_t    fMaxIter;
  static Int_t    fMinIter;
  static Double_t fMaxChisq;

public:
  TrAlignFit(Int_t maxcases = 1000000);
 ~TrAlignFit();

  void Init(void);
  void Set (Float_t arr[Ndim][8], Int_t prm[Ndim][8]);
  void Fit (void);
  void Get (Float_t arr[Ndim][8], Int_t what);

protected:
  Int_t FitPoints(void);
  void  InitHists(Int_t iter);
  void  FillHists(Int_t side);
  void  ProcHists(Int_t iter);
  void  ParmHists(void);
  void  PrintStat(Int_t iter, Double_t chisq, Int_t ntot, Double_t time);

  void Sfactor(Int_t nhit, Float_t *zpos, Float_t *xsig, Float_t *xfac);
  void SfFill (Int_t layer1, Int_t layer2, Float_t *zdef);

  Int_t ArrayIndex(Int_t icase, Int_t ilayr, Int_t type) const {
    return (0 <= icase && icase < fMaxCases &&
	    0 <= ilayr && ilayr < Nplan &&
	    0 <=  type && type  < Narr) 
      ? (icase*Nplan+ilayr)*Narr+type : -1; 
  }

  Float_t GetArray(Int_t icase, Int_t ilayr, Int_t type) const {
    Int_t idx = ArrayIndex(icase, ilayr, type);
    return (fArray && 0 <= idx && idx < fNbuf) ? fArray[idx] : 0;
  }

  void SetArray(Int_t icase, Int_t ilayr, Int_t type, Float_t data) {
    Int_t idx = ArrayIndex(icase, ilayr, type);
    if (fArray && 0 <= idx && idx < fNbuf) fArray[idx] = data;
  }

  Int_t CrrayIndex(Int_t icase, Int_t ilayr) const {
    return (0 <= icase && icase < fMaxCases &&
	    0 <= ilayr && ilayr < Nplan) ? icase*Nplan+ilayr : -1; 
  }

  char GetCrray(Int_t icase, Int_t ilayr) const {
    Int_t idx = CrrayIndex(icase, ilayr);
    return (fCrray && 0 <= idx && idx < fNcbf) ? fCrray[idx] : 0;
  }

  void SetCrray(Int_t icase, Int_t ilayr, char data) {
    Int_t idx = CrrayIndex(icase, ilayr);
    if (fCrray && 0 <= idx && idx < fNcbf) fCrray[idx] = data;
  }

  TrAlignObject *FindTkId(int tkid) const {
    aligIT iter = fAlignMap.find(tkid);
    return (iter == fAlignMap.end() ? 0 : iter->second);
  }

  Int_t GetSfIndex(Int_t lymis1, Int_t lymis2) const {
    if (lymis1 <= 0      || Nplan < lymis1) return 0;
    if (lymis2 <= lymis1 || Nplan < lymis2) return lymis1;
    return (Int_t)((8-lymis1*0.5)*(lymis1-1))+8+lymis2-lymis1;
  }

public:
  Float_t GetSfactor(Int_t lymis1, Int_t lymis2, Int_t layer) const {
    return (1 <= layer && layer <= Nplan) 
      ? fSfactor[GetSfIndex(lymis1, lymis2)][layer-1] : 0;
  }

  ClassDef(TrAlignFit, 1)  // Tracker alignment fit
};

#endif
