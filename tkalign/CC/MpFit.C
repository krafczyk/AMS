
#include "MpFit.h"
#include "TTree.h"
#include "TMath.h"
#include <iostream>

MpFit::MpFit(TTree *tree, Int_t nfill) : TFcnAdapter(0), fCopy(0), fData(0)
{
  if (!tree) return;

  Int_t nent = tree->GetEntries();
  if (nfill <= 0 || nfill > nent) nfill = nent;

  Init(nfill);
  Fill(tree, nfill);
}

MpFit::MpFit(Int_t size) : TFcnAdapter(0), fCopy(0), fData(0)
{
  Init(size);
}

MpFit::MpFit() : TFcnAdapter(0), fCopy(0), fNent(0), fData(0)
{
}

MpFit::~MpFit()
{
  if (!fCopy) delete [] fData;
}

Double_t MpFit::GetParameter(Int_t i) const
{
  if (0 <= i && i < 3) return fPosA[i];
  Double_t a, b, c;
  RotAngle(fRotA, a, b, c);
  if (i == 3) return a;
  if (i == 4) return b;
  if (i == 5) return c;
  return 0;
}

void MpFit::Init(Int_t size)
{
  if (fData) delete [] fData;

  fData = new Float_t[size*NDAT];
  fNent = 0;
}

void MpFit::Clear(Option_t *opt)
{
  fNent = 0;
  fPosA = TVector3(0, 0, 0);
  fRotA = RotMat  (0, 0, 0);
}

void MpFit::Fill(TTree *tree, Int_t nfill, Float_t range)
{
  if (!tree) return;

  Int_t nent = tree->GetEntries();
  if (nfill <= 0 || nfill > nent) nfill = nent;

  Float_t fdata[NDAT];
  tree->SetBranchAddress("fdata", fdata);

  for (Int_t i = 0; i < nfill; i++) {
    tree->GetEntry(i);
    Fill(fdata, range);
  }
}

void MpFit::Fill(Float_t *fdata, Float_t range)
{
  Int_t i = fNent++;
  for (Int_t j = 0; j < NDAT; j++) fData[i*NDAT+j] = fdata[j];

  if (range > 0) {
    TVector3 vr = GetRes(i);
    if (TMath::Abs(vr.x()) > range ||
	TMath::Abs(vr.y()) > range) fNent--;
  }
}

TVector3 MpFit::GetRes(Int_t ient, TVector3 posa, TRotation rota) const
{
  if (ient < 0 || fNent <= ient) return TVector3(0, 0, 0);

  Float_t *fdata = &fData[ient*NDAT];
  return GetRes(&fdata[1], posa, rota);
}

Double_t MpFit::fFactP =  1e4;
Double_t MpFit::fFactA = 30e4;
Double_t MpFit::fWx    =  0.1;
Double_t MpFit::fWy    =    1;

Double_t MpFit::Chisq(const PVec &par, Bool_t pset) const
{
  Double_t dx = par[0]/fFactP;
  Double_t dy = par[1]/fFactP;
  Double_t dz = par[2]/fFactP;
  Double_t da = par[3]/fFactA;
  Double_t db = par[4]/fFactA;
  Double_t dc = par[5]/fFactA;

  TVector3  posa(dx, dy, dz);
  TRotation rota = RotMat(da, db, dc);

  if (pset) {
    fPosA = posa;
    fRotA = rota;
  }

  Double_t chisq = 0;

  for (Int_t i = 0; i < fNent; i++) {
    Float_t rgt = fData[i*NDAT];
    Float_t sig = 100e-4;//TMath::Sqrt(100e-4*100e-4+0.5*0.5/rgt/rgt);
    TVector3 vr = GetRes(i, posa, rota);
    chisq += vr.x()*vr.x()/sig/sig*fWx +vr.y()*vr.y()/sig/sig*fWy;
  }

  return (fNent > 0) ? chisq : 0;
}

Double_t MpFit::Chisq(Double_t p0, Double_t p1, Double_t p2,
		      Double_t p3, Double_t p4, Double_t p5,
		      Bool_t pset) const
{
  PVec pvec;
  pvec.push_back(p0); pvec.push_back(p1); pvec.push_back(p2);
  pvec.push_back(p3); pvec.push_back(p4); pvec.push_back(p5);

  return Chisq(pvec, pset);  
}

Double_t MpFit::Chisq(TFitterMinuit &minfit, Bool_t pset) const
{
  PVec pvec;
  for (Int_t i = 0; i < minfit.GetNumberTotalParameters(); i++)
    pvec.push_back(minfit.GetParameter(i));

  return Chisq(pvec, pset);
}

Int_t MpFit::Minimize(Int_t fix, Int_t verb, Int_t nfcn)
{
  MpFit *spc = new MpFit;
  spc->fCopy = kTRUE;
  spc->fNent = fNent;
  spc->fData = fData;

  TFitterMinuit minfit;
  minfit.CreateMinimizer();
  minfit.SetMinuitFCN(spc);

  TString  stn[NPAR] = { "dx", "dy", "dz", "dth", "dph", "dps" };
  Double_t err[NPAR] = {   10,   10,  100,    50,   500,   500 };

  for (Int_t i = 0; i < NPAR; i++)
    minfit.SetParameter(i, stn[i], 0, err[i], 0, 0);

  for (Int_t i = 0; i < NPAR; i++) {
    if (fix & (1 << i)) {
      minfit.FixParameter(i);
      std::cout << "Parameter " << stn[i].Data() << " fixed" << std::endl;
    }
  }

  Double_t csq0 = Chisq(minfit);
  if (csq0 < 0) return -1;
  std::cout << "chisq= " << csq0/fNent << std::endl;

  Int_t ret = minfit.Minimize(nfcn, 0.001);
  std::cout << "#### ret= " << ret << std::endl;
  if (verb > 0) minfit.PrintResults(verb, 0);

  Double_t csq1 = Chisq(minfit, kTRUE);
  std::cout << "chisq= " << csq1/fNent << std::endl;

  for (Int_t i = 0; i < NPAR; i++) 
    fParError[i] = minfit.GetParError(i)/((i < 3) ? fFactP : fFactA);

  Double_t raa, rab, rac;
  RotAngle(fRotA, raa, rab, rac);
  std::cout << Form("posA= %6.1f %6.1f %6.1f rotA= %6.1f %6.1f %6.1f",
		    fPosA.x()*fFactP, fPosA.y()*fFactP, fPosA.z()*fFactP,
		          raa*fFactA,       rab*fFactA,       rac*fFactA)
	    << std::endl;

  return 0;
}


TVector3 MpFit::GetRes(Float_t *fdata, TVector3 posa, TRotation rota)
{
  // T = (x0+dx*t, y0+ty*t, z0+t)  // Track vector
  //   = T0 + Td*t
  // h = (     hx,      hy,   hz)  // Hit point on plane
  //
  //     T0 +     Td*t = R*h +     A
  // R-1*T0 + R-1*Td*t =   h + R-1*A
  //
  // t = (h + R-1*(A-T0)) / (R-1*Td)
  //
  // h = R-1*(T0-A+Td*t)

  TVector3 vp(fdata[0], fdata[1], 0);
  TVector3 vd(fdata[2], fdata[3], 1);
  TVector3 vh(fdata[4], fdata[5], fdata[6]);

  posa += TVector3(fdata[ 7], fdata[ 8], fdata[ 9]);
  rota *= RotMat  (fdata[10], fdata[11], fdata[12]);

  Bool_t nox = kFALSE, noy = kFALSE;
  if (TMath::Abs(vh[0]) > 999) {
    nox = kTRUE; vh[0] -= TMath::Sign(1000., vh[0]);
  }
  if (TMath::Abs(vh[1]) > 999) {
    noy = kTRUE; vh[1] -= TMath::Sign(1000., vh[1]);
  }

  TRotation rinv = rota.Inverse();
  Double_t     t = (vh+rinv*(posa-vp)).z()/(rinv*vd).z();
  TVector3    vt = vp+t*vd;
  TVector3    vr = (rota*vh+posa)-vt;
  if (nox) vr[0] = 0;
  if (noy) vr[1] = 0;

  return vr;
}

Double_t *MpFit::RotAngle(const TRotation &rotm)
{
  static Double_t angles[3];
  RotAngle(rotm, angles[0], angles[1], angles[2]);
  return angles;
}

void MpFit::RotAngle(const TRotation &rotm,
		     Double_t &a, Double_t &b, Double_t &c)
{
  c = TMath::ATan2(   rotm.YZ(), rotm.ZZ());
  a = TMath::ATan2(   rotm.XY(), rotm.XX());
  b = TMath::ATan2(-1*rotm.XZ(), rotm.ZZ()/TMath::Cos(c));
}

TRotation MpFit::RotMat(Double_t a, Double_t b, Double_t c)
{
  TRotation rt;
  rt.RotateZ(-a);
  rt.RotateY(-b);
  rt.RotateX(-c);
  return rt;
}

using namespace std;

void MpFit::Print(const TRotation &rt)
{
  cout << Form(" %8.5f %8.5f %8.5f", rt.XX(), rt.XY(), rt.XZ()) << endl;
  cout << Form(" %8.5f %8.5f %8.5f", rt.YX(), rt.YY(), rt.YZ()) << endl;
  cout << Form(" %8.5f %8.5f %8.5f", rt.ZX(), rt.ZY(), rt.ZZ()) << endl;
  cout << endl;
}

