
#include "TFitterMinuit.h"
#include "TFcnAdapter.h"
#include "TVector3.h"
#include "TRotation.h"

class TTree;

class MpFit : public TFcnAdapter {

public:
  enum { NDAT = 8+6, NPAR = 6 };
  typedef std::vector<Double_t> PVec;

protected:
  Bool_t   fCopy;
  Int_t    fNent;
  Float_t *fData;

  mutable TVector3  fPosA;
  mutable TRotation fRotA;
  mutable Double_t  fParError[NPAR];

  static Double_t fFactP;
  static Double_t fFactA;

public:
  static Double_t fWx;
  static Double_t fWy;

  MpFit(TTree *tree, Int_t nfill = 0);
  MpFit(Int_t size);
  MpFit();
 ~MpFit();

  TVector3  GetPosA() const { return fPosA; }
  TRotation GetRotA() const { return fRotA; }
  Double_t  GetParameter(Int_t i) const;
  Double_t  GetParError (Int_t i) const{
    return (0 <= i && i < NPAR) ? fParError[i] : 0;
  }

  Int_t GetNent() const { return fNent; }
  void  GetData(Int_t ient, Float_t *fdata) const {
    if (0 <= ient && ient < fNent) 
      for (Int_t i = 0; i < NDAT; i++) fdata[i] = fData[ient*NDAT+i];
  }

  Float_t *GetData(Int_t ient) {
    return (0 <= ient && ient < fNent) ? &fData[ient*NDAT] : 0;
  }

  void Init (Int_t size);
  void Clear(Option_t *opt = "");
  void Fill (TTree *tree, Int_t nfill = 0, Float_t range = 0);
  void Fill (Float_t *fdata, Float_t range = 0);

  TVector3 GetRes(Int_t ient) const {
    return GetRes(ient, fPosA, fRotA);
  }
  TVector3 GetRes(Int_t ient, TVector3 posa, TRotation rotm) const;

  Double_t operator() (const PVec &par) const { return Chisq(par); }
  Double_t Chisq(const PVec &par,       Bool_t pset = kFALSE) const;
  Double_t Chisq(TFitterMinuit &minfit, Bool_t pset = kFALSE) const;
  Double_t Chisq(Double_t p0, Double_t p1, Double_t p2,
		 Double_t p3, Double_t p4, Double_t p5,
		 Bool_t pset = kFALSE) const;

  Int_t Minimize(Int_t fix = 0, Int_t verb = 0, Int_t nfcn = 1000);

  static TVector3 GetRes(Float_t *fdata, TVector3 posa, TRotation rotm);

  static Double_t *RotAngle(const TRotation &rotm);
  static void      RotAngle(const TRotation &rotm,
			    Double_t &a, Double_t &b, Double_t &c);
  static TRotation RotMat  (Double_t  a, Double_t  b, Double_t  c);

  static void Print(const TRotation &rotm);
};

