// $Id: tkalign.C,v 1.1 2010/10/14 09:28:04 shaino Exp $
#include "TkDBc.h"
#include "TkCoo.h"
#include "TrFit.h"

#include "TObjArray.h"
#include "TDirectory.h"
#include "TH3.h"
#include "TH2.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TROOT.h"


// All the scales are in cm

class TkAlign;
typedef std::map<int, TkAlign*> TkAlignMap;
typedef std::map<int, TkAlign*>::const_iterator TkAlignIT;

class TkAlign {

public:
  enum { NLAY = 9, NLAD = 15, NHIST = 5 };

  TkLadder   *fLadder;
  TDirectory *fDir;
  TObjArray   fHarray;
  Double_t    fXref;

  static Double_t fDefRange;

  TkAlign(TkLadder *lad);
 ~TkAlign();

  Int_t GetTkId (void) const { return            fLadder->GetTkId ();  }
  Int_t GetLayer(void) const { return            fLadder->GetLayer();  }
  Int_t GetSlot (void) const { return TMath::Abs(fLadder->GetSlot ()); }
  Int_t GetSide (void) const { return            fLadder->GetSide ();  }
  Int_t GetId   (void) const;

  void FillHist(Int_t i, Int_t id, Double_t x, Double_t y);

  void InitHists(TDirectory *ldir, Double_t rng = fDefRange);
  void FillHists(TrFit &fit, Int_t ih);
  void CopyHists(void);

  enum { kXpFit = 1, kYpFit = 2, kZyFit = 4, kYtFit = 8, kSensY = 16,
	 kZtFit = 32};
  void ProcHists(Int_t bits);


  static TObjArray fSarray;
  static Int_t     fTbNpos;
  static Int_t     fTbPos;
  static void InitShists(Double_t rng = fDefRange);
  static void CopyShists(TFile &file);

  static TkAlignMap fTkMap;
  static void  InitMap(Double_t rng = fDefRange);
  static TkAlign *Find(Int_t tkid);
  static void     Fill(Int_t tkid, TrFit &fit, Int_t ih);
  static void     Proc(Int_t bits);

  static Int_t    fCheckID;
  static TFile   *fFile;

  static Int_t    fNmin;
  static Double_t fRfix;
  static Double_t fRref;
  static Double_t fRfit;
  static Double_t fCmax;
  static Double_t fErrX[NLAY], fErrY[NLAY], fErrZ[NLAY];
  static Double_t fZlay[NLAY];

  static void InitPar(void);
  static Double_t Fit(Int_t *tkid, Int_t *imlt, Float_t *xcog, Float_t *ycog);

  static Int_t fProfMode;
  static void  ProjSig(TH2 *hist, Double_t &par, Double_t &err);
  static TH1D *Profile(TH2 *hist);
  static TH2F *Project(TH3 *hist, Int_t ibx1, Int_t ibx2);
  static Int_t Fit2D(Int_t n, Double_t *x, Double_t *y, 
		              Double_t *w, Double_t *p); 
  static Int_t Inv3x3(Double_t mtx[3][3]);
};

Double_t TkAlign::fDefRange = 0.1;  // in cm

TkAlign::TkAlign(TkLadder *lad) : fLadder(lad), fDir(0)
{
}

TkAlign::~TkAlign()
{
}

Int_t TkAlign::GetId(void) const
{
  Int_t tkid  = GetTkId();
  Int_t layer = GetLayer();
  Int_t slot  = GetSlot();  // slot > 0
  Int_t side  = GetSide();  // 0(-) 1(+)
  Int_t sign  = (side == 0) ? -1 : 1;

  return (layer-1)*32+slot*sign+16-side;
}

void TkAlign::FillHist(Int_t i, Int_t id, Double_t x, Double_t y)
{
  if (i < 0 || fHarray.GetEntriesFast() <= i) return;

  TH2F *hist1 = (TH2F*)fHarray.At(i);
  TH3F *hist2 = (TH3F*)fSarray.At(i);

  if (hist1) hist1->Fill(    x, y);
  if (hist2) hist2->Fill(id, x, y);
}

void TkAlign::InitHists(TDirectory *ldir, Double_t rng)
{
  fHarray.Delete();
  ldir->cd();

  Int_t tkid = GetTkId();
  Int_t slot = GetSlot();
  Int_t sign = (tkid < 0) ? -1 : 1;

  fXref = TkCoo::GetGlobalA(tkid, 0, 0).x();

  fDir = new TDirectory(Form("dir%c%02d", ((sign > 0) ? 'p' : 'm'), slot),
			Form("TkID %d", tkid));
  fDir->cd();

  Int_t nb = 200;
  Int_t nd = 30;
  Int_t ng = 20;
  Int_t ns = 15;

  Double_t rgx = rng*2;
  fHarray.Add(new TH2F("hist1", "dX VS dX/dZ", ng,  -1,  1, nb, -rgx, rgx));
  fHarray.Add(new TH2F("hist2", "dY VS dY/dZ", ng,  -1,  1, nb, -rng, rng));
  fHarray.Add(new TH2F("hist3", "dY VS X",     nd, -60, 60, nb, -rng, rng));
  fHarray.Add(new TH2F("hist4", "dY VS Sen",   ns,   0, ns, nb, -rng, rng));
  fHarray.Add(new TH2F("hist5", "dZ VS X",     nd, -60, 60, nb, -rgx, rgx));
}

void TkAlign::FillHists(TrFit &fit, Int_t ih)
{
  Int_t  sign = (GetSide() == 0) ? -1 : 1;
  Double_t xh = fit.GetXh(ih);
  Double_t xr = fit.GetXr(ih);
  Double_t yr = fit.GetYr(ih);
  Double_t xd = fit.GetDxDz();
  Double_t yd = fit.GetDyDz();
  Double_t xs = fLadder->GetPlane()->GetRotMat().GetEl(0, 0)*xh;
  Double_t sp = TkDBc::Head->_SensorPitchK;
  Double_t sn = -(xs-fLadder->pos[0])*sign/sp;

  Int_t il = GetLayer()-1;
  if (il == 7 || il == 8) {
    if (fErrX[il] == 0) xr *= 0.1;
    if (fErrY[il] == 0) yr *= 0.1;
  }

  Int_t id = GetId();
  if (fit.GetXs(ih) >= 0)
  FillHist(0, id, xd, xr);   // dX VS dX/dZ
  FillHist(1, id, yd, yr);   // dY VS dY/dZ
  FillHist(2, id, xh, yr);   // dY VS X
  FillHist(3, id, sn, yr);   // dY VS Sen

  if (TMath::Abs(yd) > 0.15)
    FillHist(4, id, xh, yr/yd); // dZ VS X

  TH3F *hist = (TH3F *)fSarray.FindObject("hist0");
  hist->Fill(fit.GetXh(ih), fit.GetYh(ih), GetLayer());
}

void TkAlign::CopyHists(void)
{
  Int_t id = GetId();

  for (Int_t i = 0; i < NHIST; i++) {
    TH2F *hist1 = (TH2F*)fHarray.At(i);
    TH3F *hist2 = (TH3F*)fSarray.At(i);
    for (Int_t j = 0; j < hist1->GetNbinsX(); j++)
      for (Int_t k = 0; k < hist1->GetNbinsY(); k++)
	hist1->SetBinContent(j+1, k+1, hist2->GetBinContent(id, j+1, k+1));
  }
}

Int_t  TkAlign::fCheckID = -1;
TFile *TkAlign::fFile    =  0;

void TkAlign::ProcHists(Int_t bits)
{
  fDir->cd();

  Double_t par[NHIST], per[NHIST];
  for (Int_t i = 0; i < NHIST; i++) par[i] = per[i] = 0;

  Int_t nfit = 0;

  for (Int_t i = 0; i < 5; i++) {
    TH2F *hist = (TH2F *)fHarray.At(i);
    TString str = hist->GetTitle();
    str += Form(" : %4d %3d", GetTkId(), GetId());
    hist->SetTitle(str);
  }

  TH2F *hist1 = (TH2F *)fHarray.At(0);   // dX VS dX/dZ
  TH2F *hist2 = (TH2F *)fHarray.At(1);   // dY VS dY/dZ
  TH2F *hist3 = (TH2F *)fHarray.At(2);   // dY VS X
  TH2F *hist5 = (TH2F *)fHarray.At(4);   // dZ VS X

  for (Int_t i = 0; i < 3; i++) {
    TH2F *hist = (TH2F *)fHarray.At(i);
    if (!hist) {
      cerr << "Error: hist does exist: " << i
	   << " tkid= " << GetTkId() << endl;
      return;
    }
    if (hist->GetSumOfWeights() > 0 &&
	hist->GetSumOfWeights() < 100) {
      cout << "Warning: entries of hist too few: " << i << " "
	   << hist->GetSumOfWeights()
	   << " tkid= " << GetTkId() << " id= " << GetId() << endl;
      return;
    }
  }

//if (bits & kXpFit) ProjSig(hist1, par[0], per[0]);
  if (bits & kXpFit) {
    TH1D *prof = Profile(hist1);
    if (prof->GetEntries() >= 2) {
      prof->Fit("pol1", "q0");

      TF1 *func = prof->GetFunction("pol1");
      par[0] = -func->GetParameter(0);
      per[0] =  func->GetParError (0);
    }
  }

  if (bits & kYpFit) ProjSig(hist2, par[1], per[1]);

  if (bits & kZyFit) {
    TH1D *prof = Profile(hist2);
    if (prof->GetEntries() >= 2) {
      prof->Fit("pol1", "q0");

      TF1 *func = prof->GetFunction("pol1");
      par[2] = func->GetParameter(1);
      per[2] = func->GetParError (1);
    }
  }

  if (bits & kYtFit) {
    TH1D *prof = Profile(hist3);
    if (prof->GetEntries() >= 2) {
      prof->Fit("pol1", "q0");

      TF1 *func = prof->GetFunction("pol1");
      par[1] = -func->Eval(fXref);
      per[1] =  func->GetParError (0);
      par[3] = -func->GetParameter(1);
      per[3] =  func->GetParError (1);
    }
  }

  if (bits & kZtFit) {
    TH1D *prof = Profile(hist5);
    if (prof->GetEntries() >= 2) {
      prof->Fit("pol1", "q0");

      TF1 *func = prof->GetFunction("pol1");
      par[2] =  func->Eval(fXref);
      per[2] =  func->GetParError (0);
      par[4] = -func->GetParameter(1);
      per[4] =  func->GetParError (1);
    }
  }


  Int_t il = GetLayer()-1;
  if (il == 7 || il == 8) {
    if (fErrX[il] == 0) { par[0] *= 10; per[0] *= 10; }
    if (fErrY[il] == 0) { par[1] *= 10; per[1] *= 10;
                          par[2] *= 10; per[2] *= 10;
			  par[3] *= 10; per[3] *= 10;
			  par[4] *= 10; per[4] *= 10; }
  }

  Int_t id = GetId();

  for (Int_t i = 0; i < NHIST; i++) {
    Double_t factor = (i >= 3) ? 600 : 1;
    TH1F *hist = (TH1F *)fSarray.FindObject(Form("hpar%d", i+1));
    if (hist) {
      hist->SetBinContent(id, par[i]*factor);
      hist->SetBinError  (id, per[i]*factor);
    }
  }

  Int_t sgnx = 1;
  Int_t sgnz = 1;
  if (fLadder->GetPlane()->rot.GetEl(0,0) < 0) sgnx = -1;
  if (fLadder->GetPlane()->rot.GetEl(2,2) < 0) sgnz = -1;

  AMSRotMat nrm;
  nrm.SetRotAngles(-sgnx*par[3], -sgnx*sgnz*par[4], 0);

  AMSPoint pref = fLadder->posA;

  fLadder->posA = fLadder->posA + AMSPoint(par[0]*sgnx, par[1], par[2]*sgnz);
  fLadder->rotA = fLadder->rotA * nrm;

  if (id == fCheckID && gFile) {
    cout << "Check " << GetTkId() << " " << fLadder->GetTkId() << " "
	 << par[0]*1e4 << " " 
	 << par[1]*1e4 << " " 
	 << par[2]*1e4 << " : "
	 << pref << " => "
	 << fLadder->posA << endl;
    if (fFile) {
      fFile->cd();
      fDir->GetList()->Write();
    }
  }
}


TObjArray TkAlign::fSarray;
Int_t TkAlign::fTbNpos = 0;
Int_t TkAlign::fTbPos  = 0;

void TkAlign::InitShists(Double_t rng)
{
  Int_t    nbx = NLAY*32;
  Int_t    nbz = 200;
  Int_t    nd  = 30;
  Int_t    ng  = 20;
  Int_t    ns  = 15;
  Double_t bx  = nbx+0.5;
  Double_t rgx = rng*2;

  fSarray.Clear();
  fSarray.Add(new TH3F("hist1", "Residual (X) VS dX/dZ", 
		       nbx, 0.5, bx, ng,  -1,  1, nbz, -rgx, rgx));
  fSarray.Add(new TH3F("hist2", "Residual (Y) VS dY/dZ", 
		       nbx, 0.5, bx, ng,  -1,  1, nbz, -rng, rng));
  fSarray.Add(new TH3F("hist3", "Residual (Y) VS Xhit", 
		       nbx, 0.5, bx, nd, -60, 60, nbz, -rng, rng));
  fSarray.Add(new TH3F("hist4", "Residual (Y) VS Sensor", 
		       nbx, 0.5, bx, ns,   0, ns, nbz, -rng, rng));
  fSarray.Add(new TH3F("hist5", "Residual (Z) VS Xhit",
		       nbx, 0.5, bx, nd, -60, 60, nbz, -rgx, rgx));

  fSarray.Add(new TH3F("hist0", "Track pos",
		       280, -70, 70, 280, -70, 70, 9, 0.5, 9.5));

  fSarray.Add(new TH1F("hpar1", "Fitpar (X)",     nbx, 0.5, bx));
  fSarray.Add(new TH1F("hpar2", "Fitpar (Y)",     nbx, 0.5, bx));
  fSarray.Add(new TH1F("hpar3", "Fitpar (Z)",     nbx, 0.5, bx));
  fSarray.Add(new TH1F("hpar4", "Fitpar (dY/dX)", nbx, 0.5, bx));
  fSarray.Add(new TH1F("hpar5", "Fitpar (dZ/dX)", nbx, 0.5, bx));

  if (fRfix == 0) {
    Int_t ntb = fTbNpos;
    if (ntb > 0)
      fSarray.Add(new TH2F("histr", "R0/R", ntb, 0.5, ntb+0.5, 800, -20, 20));
    else
      fSarray.Add(new TH3F("histr", "1/R",
			   9, 0.5, 9.5, 100, -50, 50, 201, -100.5, 100.5));
  }
}

void TkAlign::CopyShists(TFile &file)
{
  for (Int_t i = 0; i < NHIST; i++) {
    TH3F *hist = (TH3F *)fSarray.At(i);
    TH3F *htmp = (TH3F *)file.Get(hist->GetName());
    if (!htmp) {
      cerr << "Error: hist not found: " << hist->GetName() << endl;
      continue;
    }
    hist->Add(htmp);
  }

  for (TkAlignIT it = fTkMap.begin(); it != fTkMap.end(); it++)
    if (it->second) it->second->CopyHists();
}

TkAlignMap TkAlign::fTkMap;

void TkAlign::InitMap(Double_t rng)
{
  fTkMap.clear();

  Int_t nlad = TkDBc::Head->GetEntries();

  for (Int_t i = 0; i < nlad; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad || lad->GetHwId() < 0) continue;

    Int_t tkid = lad->GetTkId();

    TkAlign *alg = new TkAlign(lad);
    fTkMap[tkid] = alg;
  }

  for (Int_t i = 0; i < NLAY; i++) {
    gROOT->cd();
    TDirectory *dir 
      = new TDirectory(Form("dir%d", i+1), Form("Layer %d", i+1));

    for (Int_t j = 0; j < 2; j++) 
      for (Int_t k = 0; k < NLAD; k++) {
	Int_t slot = k+1;
	Int_t sign = (j == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);

	TkAlign *alg = Find(tkid);
	if (!alg) continue;

	alg->InitHists(dir, rng);
      }
  }
}

TkAlign *TkAlign::Find(Int_t tkid)
{
  TkAlignIT iter = fTkMap.find(tkid);
  return (iter == fTkMap.end() ? 0 : iter->second);
}

void TkAlign::Fill(Int_t tkid, TrFit &fit, Int_t ih)
{
  TkAlign *alg = Find(tkid);
  if (alg) alg->FillHists(fit, ih);
}

void TkAlign::Proc(Int_t bits)
{
  for (TkAlignIT it = fTkMap.begin(); it != fTkMap.end(); it++)
    if (it->second) it->second->ProcHists(bits);
}


Int_t    TkAlign::fNmin = 6;
Double_t TkAlign::fRfix = 400;
Double_t TkAlign::fRref = 400;
Double_t TkAlign::fRfit = 0;
Double_t TkAlign::fCmax = 10;
Double_t TkAlign::fErrX[TkAlign::NLAY];
Double_t TkAlign::fErrY[TkAlign::NLAY];
Double_t TkAlign::fErrZ[TkAlign::NLAY];
Double_t TkAlign::fZlay[TkAlign::NLAY];

void TkAlign::InitPar(void)
{
  for (Int_t i = 0; i < NLAY; i++) {
    fZlay[i] = TkDBc::Head->GetZlayer(i+1);
    fErrX[i] = fErrY[i] = fErrZ[i] = 300e-4;
  }
}

Double_t TkAlign::Fit(Int_t *tkid, Int_t *imlt, Float_t *xcog, Float_t *ycog)
{
  Double_t zhit[NLAY];
  Int_t nhit = 0;

  for (Int_t i = 0; i < NLAY; i++) {
    Int_t ily = TMath::Abs(tkid[i])/100-1;
    if (0 <= ily && ily < NLAY) {
      zhit[i] = fZlay[ily];
      nhit++;
    }
    else zhit[i] = -999;
  }
  if (nhit < fNmin) return -2;

  Int_t idx[NLAY];
  TMath::Sort<Double_t,Int_t>(NLAY, zhit, idx);

  TrFit trfit;
  Int_t fixr = 0;
  if (fRfix != 0) {
    trfit.SetRigidity(fRfix);
    fixr = 1;
  }

  TrFit ttmp;
  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0 || xcog[j] < 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], xcog[j], imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j], ycog[j], imlt[j]), 0);
    AMSPoint err(fErrX[ily], fErrY[ily], fErrZ[ily]);
    ttmp.Add(TkCoo::GetGlobalA(tkid[j], loc), err);
  }
  if (ttmp.LinearFit() <  0) return -1;
  if (ttmp.GetChisqX() > 50) return -1;

  ttmp.SetRigidity(0);
  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], TMath::Abs(xcog[j]), imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j],            ycog[j],  imlt[j]), 0);
    AMSPoint err(((xcog[j] > 0) ? fErrX[ily] : -1), fErrY[ily], fErrZ[ily]);

    AMSPoint coo = TkCoo::GetGlobalA(tkid[j], loc);
    if (xcog[j] < 0) {
      ttmp.Propagate(coo.z());
      coo[0] = ttmp.GetP0x();
    }

    trfit.Add(coo, err);
  }
  if (trfit.AlcarazFit(fixr) < 0) return -1;

  Double_t chisq = trfit.GetChisqY(); //trfit.GetChisqX()+trfit.GetChisqY();
  fRfit = 0;

  if (!fixr && trfit.GetRigidity() != 0) {
    if (fTbNpos > 0 && fTbPos > 0) {
      if (chisq < fCmax) {
	TH2F *hist = (TH2F*)fSarray.FindObject("histr");
	hist->Fill(fTbPos, fRref/trfit.GetRigidity()-1);
	fRfit = trfit.GetRigidity();
      }
    }
    else {
      Double_t rgt  = trfit.GetRigidity();
      Double_t argt = TMath::Abs(rgt);
      Double_t csqx = trfit.GetChisqX();
      Double_t csqy = trfit.GetChisqY();
      Double_t logr = TMath::Log10(argt);
      Double_t csrx = TMath::Power(10, 2.3-2.5*logr)+0.07;
      Double_t csry = TMath::Power(10, 1.7-2.5*logr)+0.01;
      Double_t rsr8 = 1.5/argt+100e-4;
      Double_t rsr9 = 1.5/argt+130e-4;

      if (csqx/csrx < 100 && csqy/csry < 100) {
	Int_t i8 = -1, i9 = -1;
	for (Int_t i = 0, k = 0; i < nhit; i++) {
	  Int_t j = idx[i];
	  if (tkid[j] == 0) continue;
	  Int_t layer = TMath::Abs(tkid[j])/100;
	  if (layer == 8) i8 = k;
	  if (layer == 9) i9 = k;
	  k++;
	}

	Double_t ry8 = 0;
	Double_t ry9 = 0;
	trfit.SetErr(i8,        0,        0, fErrZ[7]);
	trfit.SetErr(i9, fErrX[8], fErrY[8], fErrZ[8]);
	if (trfit.AlcarazFit() > 0) ry8 = trfit.GetYr(i8);

	trfit.SetErr(i8, fErrX[7], fErrY[7], fErrZ[7]);
	trfit.SetErr(i9,        0,        0, fErrZ[8]);
	if (trfit.AlcarazFit() > 0) ry9 = trfit.GetYr(i9);

	if (ry8 != 0 && ry9 != 0 &&
	    TMath::Abs(ry8/rsr8) < 1 && TMath::Abs(ry9/rsr9) < 1) {
	  fRfit = rgt;

	  TH3F *hist = (TH3F*)fSarray.FindObject("histr");

	  for (Int_t i = 0, k = 0; i < nhit; i++) {
	    Int_t j = idx[i];
	    if (tkid[j] == 0) continue;
	    Int_t layer = TMath::Abs(tkid[j])/100;
	    hist->Fill(layer, trfit.GetYh(k++), 1e3/trfit.GetRigidity());
	  }
	}
      }
    }
  }
  if (chisq > fCmax) return -chisq;

  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] != 0) Fill(tkid[j], trfit, i);
  }
  return chisq;
}

Int_t TkAlign::fProfMode = 2;

void TkAlign::ProjSig(TH2 *hist, Double_t &par, Double_t &err)
{
  if (!hist) return;

  par = err = 0;

  TH1D *hprj = hist->ProjectionY("hprj");

  Double_t nmin = 100;

  if (fProfMode == 1) {
    Int_t nx = hprj->GetNbinsX();

    Double_t *hx = new Double_t[nx];
    Double_t *hy = new Double_t[nx];
    Double_t *hw = new Double_t[nx];

    Int_t neff = 0;
    Double_t sum = 0;
    for (Int_t i = 0; i < nx; i++) {
      Double_t c = hprj->GetBinContent(i+1);
      hx[i] = hprj->GetBinCenter(i+1);
      hy[i] = hw[i] = 0;

      if (c > 10) {
	Double_t lc =  TMath::Log(c);
	Double_t er = (TMath::Log(c+TMath::Sqrt(c))-lc)/c;
	hy[i] = lc;
	hw[i] = 1/er/er;
	sum += c;
	neff++;
      }
    }

    if (sum > nmin && neff > 3) {
      Double_t pp[3];
      Fit2D(nx, hy, hx, hw, pp);

      if (pp[2] < 0) {
	Double_t gpar1 = -pp[1]/pp[2]/2;
	Double_t gpar2 = TMath::Sqrt(-0.5/pp[2]);
	Double_t gerr1 = gpar2/TMath::Sqrt(sum);
	par = -gpar1;
	err =  gerr1;
      }
    }

    delete [] hx;
    delete [] hy;
    delete [] hw;
  }

  else if (fProfMode == 2) {
    if (hprj->GetSumOfWeights() > nmin) {
      hprj->Fit("gaus", "q0");
      TF1 *func = hprj->GetFunction("gaus");
      par = -func->GetParameter(1);
      err =  func->GetParError (1);
    }
  }

  delete hprj;
}

TH1D *TkAlign::Profile(TH2 *hist)
{
  if (!hist) return 0;

  TString hname = hist->GetName();
  hname.ReplaceAll("hist", "prof");
  hname.ReplaceAll("proj", "prof");
  if (hname == "hist") hname += "_pf";

  TH1D *prof = hist->ProjectionX(hname);
  TH1D *htmp = 0;
  Int_t nmin = 50;

  prof->Reset();

  Int_t nx = hist->GetNbinsX();
  Int_t ny = hist->GetNbinsY();

  Double_t *hx = new Double_t[ny];
  Double_t *hy = new Double_t[ny];
  Double_t *hw = new Double_t[ny];

  Int_t nfill = 0;

  for (Int_t i = 0; i < nx; i++) {
    Int_t neff = 0;
    Double_t sum = 0;
    for (Int_t j = 0; j < ny; j++) {
      hx[j] = hist->GetYaxis()->GetBinCenter(j+1);
      hy[j] = hw[j] = 0;

      Double_t c = hist->GetBinContent(i+1, j+1);
      sum += c;

      if (c > 10) {
	Double_t lc =  TMath::Log(c);
	Double_t er = (TMath::Log(c+TMath::Sqrt(c))-lc)/c;
	hy[j] = lc;
	hw[j] = 1/er/er;
	neff++;
      }
    }

    Double_t par = 0, per = 0;
    if (sum > nmin && neff > 3) {

      // Fast gaussian fit
      if (fProfMode == 1) {
	Double_t pp[3];
	Fit2D(ny, hy, hx, hw, pp);

	if (pp[2] < 0) {
	  Double_t gpar1 = -pp[1]/pp[2]/2;
	  Double_t gpar2 = TMath::Sqrt(-0.5/pp[2]);
	  Double_t gerr1 = gpar2/TMath::Sqrt(sum);
	  par = gpar1;
	  per = gerr1;
	}
      }

      // Normal fit (slow and memory consumption)
      else if (fProfMode == 2) {
	htmp = hist->ProjectionY("htmp", i+1, i+1);
	htmp->Fit("gaus", "q0");
	TF1 *func = htmp->GetFunction("gaus");
	par = func->GetParameter(1);
	per = func->GetParError (1);
      }

      // Fitting on cosmic-ray data with long tail
      else if (fProfMode == 3) {
	htmp = hist->ProjectionY("htmp", i+1, i+1);

	TF1 *func = new TF1("func",
			    "[0]/(exp((x-[1])/[2])+exp(-(x-[1])/[2]))+"
			    "[3]/(exp((x-[1])/[4])+exp(-(x-[1])/[4]))");
	func->SetParameters(htmp->GetMaximum(), 0, 10e-4,
			    htmp->GetMaximum()/10, 50e-4);
	if (htmp->Fit(func, "q0") != 0) htmp->Fit(func, "q0");

	if (TMath::Abs(func->GetParameter(2)) < 1e-4 ||
	    TMath::Abs(func->GetParameter(4)) < 1e-4 || 
	    func->GetParError(1) > 10e-4) {
	  cout << "Warning: fitting maybe failed: "
	       << Form("%s %2d %5.2f %5.2f %6.1f %6.1f %.0f",
		       hist->GetTitle(), i+1, 
		       func->GetChisquare()/func->GetNDF(),
		       func->GetParError(1)*1e4,
		       TMath::Abs(func->GetParameter(2))*1e4,
		       TMath::Abs(func->GetParameter(4))*1e4,
		       htmp->GetSumOfWeights()) << endl;
	  
	  htmp->Fit("gaus", "q0");
	  func = htmp->GetFunction("gaus");
	}

	par = func->GetParameter(1);
	per = func->GetParError (1);
      }
    }

    if (sum > nmin/10 && par == 0 && per == 0) {
      htmp = hist->ProjectionY("htmp", i+1, i+1);
      // Simple mean
      if (htmp->GetMeanError() > 1e-4) {
	par = htmp->GetMean();
	per = htmp->GetMeanError();
      }
    }

    prof->SetBinContent(i+1, par);
    prof->SetBinError  (i+1, per);
    if (per > 0) nfill++;
  }
  prof->SetEntries(nfill);

  delete htmp;

  delete [] hx;
  delete [] hy;
  delete [] hw;

  return prof;
}

TH2F *TkAlign::Project(TH3 *hist, Int_t ibx1, Int_t ibx2)
{
  TString hname = hist->GetName();
  hname.ReplaceAll("hist", "proj");
  if (hname == "hist") hname += "_pj";

  Int_t  ny = hist->GetNbinsY();
  Int_t  nz = hist->GetNbinsZ();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  TH2F *proj = new TH2F(hname, hist->GetTitle(), 
			ny, ay->GetXmin(), ay->GetXmax(),
			nz, az->GetXmin(), az->GetXmax());

  for (Int_t i = 0; i < ny; i++)
    for (Int_t j = 0; j < nz; j++)
      proj->SetBinContent(i+1, j+1, hist->Integral(ibx1, ibx2, 
						   i+1, i+1, j+1, j+1));
  return proj;
}

Int_t TkAlign::Fit2D(Int_t n, Double_t *x, Double_t *y, 
		              Double_t *w, Double_t *par)
{
  // Analytic parabola fitting method

  // check number of hits
  if (n < 4) return -1;

  Int_t np = 3;

  Double_t mtx[3][3], vec[3];
  for (Int_t i = 0; i < 9; i++) mtx[i/3][i%3] = vec[i/3] = 0;

  for (Int_t i = 0; i < n; i++) {
    Double_t m0 = (w) ? w[i] : 1;
    for (Int_t j = 0; j < np; j++) {
      Double_t m = m0;
      for (Int_t k = j; k < np; k++) {
	mtx[j][k] += m;
	if (j == 0) vec[k] += m*x[i];
	m *= y[i];
      }
      m0 *= y[i]*y[i];
    }
  }
  mtx[1][0] = mtx[0][1];
  mtx[2][0] = mtx[0][2];
  mtx[2][1] = mtx[1][2];

  if (Inv3x3(mtx) < 0) return -1;

  for (Int_t i = 0; i < np; i++)
    par[i] = mtx[i][0]*vec[0]+mtx[i][1]*vec[1]+mtx[i][2]*vec[2];
  return 0;
}

Int_t TkAlign::Inv3x3(Double_t mtx[3][3])
{
  Double_t pM[9];
  for (Int_t i = 0; i < 9; i++) pM[i] = mtx[i/3][i%3];

  const Double_t c00 = pM[4] * pM[8] - pM[5] * pM[7];
  const Double_t c01 = pM[5] * pM[6] - pM[3] * pM[8];
  const Double_t c02 = pM[3] * pM[7] - pM[4] * pM[6];
  const Double_t c10 = pM[7] * pM[2] - pM[8] * pM[1];
  const Double_t c11 = pM[8] * pM[0] - pM[6] * pM[2];
  const Double_t c12 = pM[6] * pM[1] - pM[7] * pM[0];
  const Double_t c20 = pM[1] * pM[5] - pM[2] * pM[4];
  const Double_t c21 = pM[2] * pM[3] - pM[0] * pM[5];
  const Double_t c22 = pM[0] * pM[4] - pM[1] * pM[3];

  const Double_t t0 = TMath::Abs(pM[0]);
  const Double_t t1 = TMath::Abs(pM[3]);
  const Double_t t2 = TMath::Abs(pM[6]);
  Double_t det;
  Double_t tmp;
  if (t0 >= t1) {
    if (t2 >= t0) {
    tmp = pM[6];
    det = c12*c01-c11*c02;
    } else {
      tmp = pM[0];
      det = c11*c22-c12*c21;
    }
  } else if (t2 >= t1) {
    tmp = pM[6];
    det = c12*c01-c11*c02;
  } else {
    tmp = pM[3];
    det = c02*c21-c01*c22;
  }

  if ( det == 0 || tmp == 0) {
    std::cout << "Inv3x3 matrix is singular" << std::endl;
    return -1;
  }

  const Double_t s = tmp/det;

  pM[0] = s*c00;
  pM[1] = s*c10;
  pM[2] = s*c20;
  pM[3] = s*c01;
  pM[4] = s*c11;
  pM[5] = s*c21;
  pM[6] = s*c02;
  pM[7] = s*c12;
  pM[8] = s*c22;

  for (Int_t i = 0; i < 9; i++) mtx[i/3][i%3] = pM[i];

  return 0;
}
