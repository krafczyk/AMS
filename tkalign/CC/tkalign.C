// $Id: tkalign.C,v 1.5 2011/06/17 00:30:49 shaino Exp $
#include "TObjArray.h"
#include "TDirectory.h"
#include "TH3.h"
#include "TH2.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TROOT.h"

#include "TFitterMinuit.h"
#include "TFcnAdapter.h"

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrFit.h"

#include <vector>
#include <fstream>
#include <iostream>

// All the scales are in cm

class TrFit;

class PlAlign : public TFcnAdapter {

public:
  enum { NPLA = 6, NLAY = 9, NPAR = 6 };

protected:
  Int_t fPlane;

  TObjArray   fHarray;
  TDirectory *fDir;

  mutable AMSPoint  fPosA;
  mutable AMSRotMat fRotA;
  mutable Int_t     fNDF;

  Double_t fPerr[NPAR];

public:
  static Double_t fRange;
  static Int_t    fMode;  // 1:Plane 2:Layer 3:ExPlane

public:
  PlAlign(Int_t plane) : fPlane(plane), TFcnAdapter(0) { Init(); }
 ~PlAlign() {}

  Int_t GetPlane(void) const { return fPlane; }

  void Init(void);
  void Copy(TDirectory *dir);
  void Fill(TrFit &trfit, Int_t ih);
  void Prof(Int_t mode = 0);

  static TH2F *Prof(TH3F *hist, Int_t mode, Int_t wthd = 500,
		    Double_t pmax = 500e-4, Double_t emax = 50e-4,
		    Double_t emin = 0.5e-4,
		    Double_t sig1 = 0.03,   Double_t sig2 = 0.1, 
		    Double_t rpk  = 1);

  typedef std::vector<Double_t> PVec;

  Double_t operator() (const PVec &par) const { return Chisq(par); }
  Double_t Chisq(const PVec &par,       Bool_t pset = kFALSE) const;
  Double_t Chisq(TFitterMinuit &minfit, Bool_t pset = kFALSE) const;

  static Int_t Minimize(PlAlign *plalg, Int_t fix  = 0,
			Int_t verb = 0, Int_t nfcn = 1000);
};

typedef std::vector<PlAlign*> PlAlignVec;

class TkAlign;
typedef std::map<int, TkAlign*> TkAlignMap;
typedef std::map<int, TkAlign*>::const_iterator TkAlignIT;

class TkAlign {

public:
  enum { NPLA = 6, NLAY = 9, NLAD = 15, NHIST = 5 };

  enum { kXpFit = 0x0001, kYpFit = 0x0002, kZyFit = 0x0004, kYtFit = 0x0008, 
	 kSensY = 0x0010, // 16
	 kZtFit = 0x0020, // 32
	 kPlFit = 0x1000, // 4096
	 kLyFit = 0x2000  // 8192
       };

protected:
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
  Int_t GetId   (void) const { return GetId(GetTkId()); }

  void FillHist(Int_t i, Double_t x, Double_t y);

  void InitHists(TDirectory *ldir, Double_t rng = fDefRange);
  void CopyHists(void);

  void ProcHists(Int_t bits);

// Static members
protected:
  static TObjArray fSarray;
  static TkAlignMap fTkMap;
  static PlAlignVec fPlVec;

  friend class PlAlign;

public:
  static Int_t     fTbNpos;
  static Int_t     fTbPos;

  static Int_t    fCheckID;
  static TFile   *fFile;

  static Int_t    fNmin;
  static Double_t fRfix;
  static Double_t fRref;
  static Double_t fRfit;
  static Double_t fCmax;
  static Double_t fErrX[NLAY], fErrY[NLAY], fErrZ[NLAY];
  static Double_t fZlay[NLAY];
  static Int_t    fMsc;

  static Int_t fProfMode;

public:
  static Int_t GetId(Int_t tkid);
  static Int_t GetSn(Int_t tkid,    Double_t xh);
  static Int_t GetSn(TkLadder *lad, Double_t xh);

  static void InitShists(Double_t rng = fDefRange);
  static void CopyShists(TFile &file);

  static void InitMap(Double_t rng = fDefRange);
  static void InitVec();
  static void CopyVec(TFile &file);

  static TkAlign *Find(Int_t tkid);
  static void InitPar(void);
  static Double_t Fit(Int_t *tkid, Int_t *imlt, Float_t *xcog, Float_t *ycog);

  static void Proc(Int_t bits);

protected:
  static void Fill (Int_t tkid, TrFit &fit, Int_t ih);
  static void FillR(TrFit &trfit, Int_t *idx, Int_t *tkid);

  static void FillHist(Int_t i, Int_t tkid, Double_t x, Double_t y);

public:
  static void  ProjSig(TH2 *hist, Double_t &par, Double_t &err);
  static TH1D *Profile(TH2 *hist);
  static TH2F *Profile(TH3 *hist);
  static TH2F *Project(TH3 *hist, Int_t ibx1, Int_t ibx2);
  static Int_t Fit2D(Int_t n, Double_t *x, Double_t *y, 
		              Double_t *w, Double_t *p); 
  static Int_t Inv3x3(Double_t mtx[3][3]);
};


Double_t PlAlign::fRange = 1;
Int_t    PlAlign::fMode  = 1;

void PlAlign::Init(void)
{
  Double_t rng = fRange;
  if (fMode != 2 && fPlane >= 5) rng *= 20;

  fDir = gDirectory;

  fHarray.Clear();
  fHarray.Add(new TH3F("hist1", "dx VS x",
		       20, -50, 50, 12, -0.6, 0.6, 100, -rng, rng));
  fHarray.Add(new TH3F("hist2", "dx VS y",
		       20, -50, 50, 12, -0.6, 0.6, 100, -rng, rng));
  fHarray.Add(new TH3F("hist3", "dy VS x",
		       20, -50, 50, 12, -0.6, 0.6, 100, -rng, rng));
  fHarray.Add(new TH3F("hist4", "dy VS y",
		       20, -50, 50, 12, -0.6, 0.6, 100, -rng, rng));
}

void PlAlign::Copy(TDirectory *dir)
{
  for (Int_t i = 0; i < fHarray.GetEntries(); i++) {
    TH3F *hist = (TH3F *)fHarray.At(i);
    if (!hist) continue;

    TH3F *href = (TH3F *)dir->Get(hist->GetName());
    if (!href) {
      cerr << Form("%s does not exist", hist->GetName()) << endl;
      continue;
    }

    hist->Reset();
    hist->Add(href);
  }
}

void PlAlign::Fill(TrFit &trfit, Int_t ih)
{
  TH3F *hist1 = (TH3F *)fHarray.At(0);
  TH3F *hist2 = (TH3F *)fHarray.At(1);
  TH3F *hist3 = (TH3F *)fHarray.At(2);
  TH3F *hist4 = (TH3F *)fHarray.At(3);

  Int_t pln = (fPlane == 1) ? 1 : fPlane/2+1; // fPlane;
  if ( pln == 3  && TMath::Abs(trfit.GetP0x()) > 40) return;
  if ((pln == 2 ||
       pln == 4) && TMath::Abs(trfit.GetP0x()) > 45) return;

  if (trfit.GetXs(ih) >= 0) {
    hist1->Fill(trfit.GetP0x(), trfit.GetDxDz(), trfit.GetXr(ih));
    hist2->Fill(trfit.GetP0y(), trfit.GetDxDz(), trfit.GetXr(ih));
  }
  if (trfit.GetYs(ih) >= 0) {
    hist3->Fill(trfit.GetP0x(), trfit.GetDyDz(), trfit.GetYr(ih));
    hist4->Fill(trfit.GetP0y(), trfit.GetDyDz(), trfit.GetYr(ih));
  }
}

void PlAlign::Prof(Int_t mode)
{
  if (fDir && fDir->IsWritable()) fDir->cd();

  Int_t wthd = 500;

  if (mode == 0) {
    mode = (fRange > 0.05) ?   1 :   2;
    wthd = (fRange > 0.05) ? 500 : 100;
  }

  Double_t pmax = 500e-4;
  Double_t emax = 100e-4;
  Double_t emin = 0.5e-4;

  Int_t      ii[6] = { 0, 1, 1, 2, 3, 3 };
  Double_t sig1[8] = {  30,   5,   30,  15,   30,  10,   300,  300 };
  Double_t sig2[8] = {  80,  20,   60,  40,   80,  20,  1300, 1300 };
  Double_t rpk [8] = { 0.4, 0.4,  0.4, 0.4,  0.1, 0.3,   1.0,  1.0 };

  cout << endl;
  if (fMode == 2)
    cout << "##### Layer: " << fPlane << endl;
  else
    cout << "##### Plane: " << fPlane << endl;

  for (Int_t i = 0; i < 4; i++) {
    TH3F *hist = (TH3F *)fHarray.At(i);
    if (hist) {
     if (mode == 2)
      fHarray.Add(Prof(hist, mode, wthd, pmax, emax, emin));
     else {
      Double_t wsum = hist->GetSumOfWeights();
      if (wsum < 1e5) { emax = 100e-4; wthd = 200; }
      if (wsum > 1e6)   emax =  20e-4;

      Int_t j = ii[fPlane-1]*2+i/2;

      if (fPlane == 1 && TkAlign::fErrX[0] == 0) {
	sig1[j] =  80;
	sig2[j] = 300;
      }
      else if (fPlane == 4 && TkAlign::fErrX[5] == 0
	                   && TkAlign::fErrX[6] == 0) {
	sig1[j] =  80;
	sig2[j] = 300;
	rpk [j] = 0.4;
      }
      else if (fPlane <= 4) {
	sig2[j] = 1;
	rpk [j] = -1;
      }

      fHarray.Add(Prof(hist, mode, wthd, pmax, emax, emin, 
		       sig1[j]*1e-4, sig2[j]*1e-4, rpk[j]));
     }
    }
    else
      cerr << Form("hist%d does not exist", i+1) << endl;
  }
}

TH2F *PlAlign::Prof(TH3F *hist, Int_t mode, Int_t wthd,
		    Double_t pmax, Double_t emax, Double_t emin,
		    Double_t sig1, Double_t sig2, Double_t rpk)
{
  Int_t nbx = hist->GetNbinsX();
  Int_t nby = hist->GetNbinsY();
  Int_t nbz = hist->GetNbinsZ();
  TAxis *ax = hist->GetXaxis();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  Int_t wcut = (Int_t)(hist->GetSumOfWeights()/nbx/nby);
  if (wcut > wthd) wthd = wcut;

  TString shn = hist->GetName();
  shn.ReplaceAll("hist", "prof");

  TH1F *hchk = (TH1F *)gDirectory->Get("hchk");
  if (!hchk) hchk = new TH1F("hchk", "Prof.check", 4*nbx*nby, 0, 4*nbx*nby);


  TH2F *prof = new TH2F(shn, hist->GetName(), 
			nbx, ax->GetXmin(), ax->GetXmax(),
			nby, ay->GetXmin(), ay->GetXmax());

  shn.ReplaceAll("prof", "hprj");
  TH1F *hprj = new TH1F(shn, hist->GetTitle(),
			nbz, az->GetXmin(), az->GetXmax());
  for (Int_t k = 0; k < nbz; k++)
    hprj->SetBinContent(k+1, hist->Integral(1, nbx, 1, nby, k+1, k+1));

  shn.ReplaceAll("hprj", "");
  Int_t hid = shn.Atoi();

  TF1 *func;
  Double_t pfix[6];

  if (mode == 1) {
    func = new TF1("func",
		   "[0]/(exp((x-[1])/[2])+exp(-(x-[1])/[2]))+"
		   "[3]*[0]/(exp((x-[4])/[5])+exp(-(x-[4])/[5]))");
    if (rpk > 0)
      func->SetParameters(hprj->GetMaximum(), 0, sig1, rpk, 0, sig2);
    else {
      func->FixParameter(3, 0);
      func->FixParameter(4, 0);
      func->FixParameter(5, 1);
      func->SetParameters(hprj->GetMaximum(), 0, sig1, 0, 0, 1);
    }
    hprj->Fit(func, "q0");

    pfix[1] =            func->GetParameter(1);
    pfix[2] = TMath::Abs(func->GetParameter(2));
    pfix[3] =            func->GetParameter(3);
    pfix[4] =            func->GetParameter(4);
    pfix[5] = TMath::Abs(func->GetParameter(5));

    Double_t perr[6];
    for (Int_t i = 0; i < 6; i++) perr[i] = func->GetParError(i);

    if (pfix[2] > pfix[5]) {
      pfix[1] =            func->GetParameter(4);
      pfix[2] = TMath::Abs(func->GetParameter(5));
      pfix[3] =          1/func->GetParameter(3);
      pfix[4] =            func->GetParameter(1);
      pfix[5] = TMath::Abs(func->GetParameter(2));
      perr[1] = func->GetParError(4);
      perr[2] = func->GetParError(5);
      perr[4] = func->GetParError(1);
      perr[5] = func->GetParError(2);
    }

    func->FixParameter(2, pfix[2]); func->FixParameter(3, pfix[3]);
    func->FixParameter(5, pfix[4]); func->FixParameter(4, pfix[5]);

    cout << Form("s: %5.1f(%4.1f) %6.1f(%5.1f) "
		 "m: %6.1f(%4.1f) %6.1f(%4.1f) r: %4.2f(%4.2f)",
		 pfix[2]*1e4, perr[2]*1e4,
		 pfix[5]*1e4, perr[5]*1e4,
		 pfix[1]*1e4, perr[1]*1e4,
		 pfix[4]*1e4, perr[4]*1e4,
		 pfix[3],     perr[3]) << endl;
  }

  TH1F *htmp = new TH1F("htmp", "htmp", nbz, az->GetXmin(), az->GetXmax());

  for (Int_t i = 0; i < nbx; i++) {
    for (Int_t j = 0; j < nby; j++) {

      htmp->Reset();
      for (Int_t k = 0; k < nbz; k++)
	htmp->SetBinContent(k+1, hist->GetBinContent(i+1, j+1, k+1));

      if (htmp->GetSumOfWeights() < wthd) continue;

      if (mode == 1) {
	Double_t sig1 = 0.05;
	Double_t sig2 = 0.25;

	if (htmp->GetRMS() < 0.02) {
	  sig1 = 0.002;
	  sig2 = 0.010;
	}

	func->SetParameters(htmp->GetMaximum(), pfix[1], 
			    pfix[2], pfix[3], pfix[4], pfix[5]);
	htmp->Fit(func, "q0");
      }
      else {
	htmp->Fit("gaus", "q0");
	func = htmp->GetFunction("gaus");
      }

      Double_t par = func->GetParameter(1);
      Double_t per = func->GetParError (1);

      if (per < emax && TMath::Abs(par) < pmax) {
	if (per < emin) per = emin;

	prof->SetBinContent(i+1, j+1, par);
	prof->SetBinError  (i+1, j+1, per);

	Int_t ib = (hid-1)*nbx*nby+j*nbx+i+1;
	hchk->SetBinContent(ib, par);
	hchk->SetBinError  (ib, per);
      }
    }
  }
  delete htmp;

  return prof;
}


Double_t PlAlign::Chisq(const PVec &par, Bool_t pset) const
{
  Double_t dx = par[0]*1e-4;
  Double_t dy = par[1]*1e-4;
  Double_t dz = par[2]*1e-4;
  Double_t da = par[3]*1e-4/40;
  Double_t db = par[4]*1e-4/40;
  Double_t dc = par[5]*1e-4/40;

  if (pset) {
    fPosA.setp        (-dx, -dy, -dz);
    fRotA.SetRotAngles(-da, -db, -dc);
  }

  Double_t chisq = 0;
  fNDF = 0;

  for (Int_t i = 0; i < 4; i++) {
    TH2F *prof = (TH2F *)fHarray.At(i+4);
    if (!prof) {
      cerr << Form("prof%d not found", i+1) << endl;
      return -1;
    }

    for (Int_t j = 0; j < prof->GetNbinsX(); j++) {
      for (Int_t k = 0; k < prof->GetNbinsY(); k++) {
	if (prof->GetBinError(j+1, k+1) <= 0) continue;

	Double_t pos = prof->GetXaxis()->GetBinCenter(j+1);
	Double_t dtr = prof->GetYaxis()->GetBinCenter(k+1);
	Double_t res = prof->GetBinContent(j+1, k+1);
	Double_t rer = prof->GetBinError  (j+1, k+1);
	Double_t ddz = (i%2 == 0) ? db*pos : -dc*pos;
	Double_t drs = (i/2 == 0) ? dx : dy;
	Double_t drt = (i == 1) ?  da*pos 
                    : ((i == 2) ? -da*pos : 0);
      //Double_t drt = (i == 2) ? -da*pos : 0;

	Double_t ddr = res -drs-drt +(dz+ddz)*dtr;
	chisq += ddr*ddr/rer/rer;
	fNDF++;
      }
    }
  }

  return chisq;
}

Double_t PlAlign::Chisq(TFitterMinuit &minfit, Bool_t pset) const
{
  PVec pvec;
  for (Int_t i = 0; i < minfit.GetNumberTotalParameters(); i++)
    pvec.push_back(minfit.GetParameter(i));

  return Chisq(pvec, pset);
}

Int_t PlAlign::Minimize(PlAlign *plfit, Int_t fix, Int_t verb, Int_t nfcn)
{
  TFitterMinuit minfit;

  minfit.CreateMinimizer();
  minfit.SetMinuitFCN(plfit);

  TString  stn[NPAR] = { "dx", "dy", "dz", "dth", "dph", "dps" };
  Double_t err[NPAR] = {   10,   10,  100,    50,   100,   100 };

  for (Int_t i = 0; i < NPAR; i++)
    minfit.SetParameter(i, stn[i], 0, err[i], 0, 0);

  for (Int_t i = 0; i < NPAR; i++) {
    if (fix & (1 << i)) {
      minfit.FixParameter(i);
      cout << "Parameter " << stn[i].Data() << " fixed" << endl;
    }
  }

  Double_t csq0 = plfit->Chisq(minfit);
  if (csq0 < 0) return -1;
  cout << "chisq= " << csq0/plfit->fNDF << endl;

  Int_t ret = minfit.Minimize(nfcn, 0.001);
  cout << "#### ret= " << ret << endl;
  if (verb > 0) minfit.PrintResults(verb, 0);

  Double_t csq1 = plfit->Chisq(minfit, kTRUE);
  cout << "chisq= " << csq1/plfit->fNDF << endl;

  Double_t par[NPAR], per[NPAR];
  for (Int_t i = 0; i < NPAR; i++) {
    par[i] = minfit.GetParameter(i);
    per[i] = minfit.GetParError (i);
  }

  if (TkDBc::Head) {
    if (fMode == 2) {
      plfit->fRotA.GetRotAngles(par[3], par[4], par[5]);
      par[0] = plfit->fPosA[0]/1e-4;
      par[1] = plfit->fPosA[1]/1e-4;
      par[2] = plfit->fPosA[2]/1e-4;
      par[3] /= 1e-4/40;
      par[4] /= 1e-4/40;
      par[5] /= 1e-4/40;

      for (Int_t i = 0; i < TkDBc::Head->GetEntries(); i++) {
	TkLadder *lad = TkDBc::Head->GetEntry(i);
	if (lad->GetLayer() == plfit->fPlane)
	  lad->posA = lad->posA + plfit->fPosA;
      }
    }
    else {
      Int_t  plane  = plfit->GetPlane();
      Bool_t update = ((fMode == 1 && plane <= 4) ||
		       (fMode == 3 && plane >= 5)) ? kTRUE : kFALSE;

      AMSPoint posa;
      
      if (update) {
	TkPlane *pl = TkDBc::Head->GetPlane(plane);
	pl->posA = pl->posA + plfit->fPosA;
	pl->rotA = pl->rotA * plfit->fRotA;
	pl->rotA.GetRotAngles(par[3], par[4], par[5]);
	posa = pl->posA;
      }
      else {
	posa = plfit->fPosA;
	plfit->fRotA.GetRotAngles(par[3], par[4], par[5]);
      }

      par[0] = posa[0]/1e-4;
      par[1] = posa[1]/1e-4;
      par[2] = posa[2]/1e-4;
      par[3] /= 1e-4/40;
      par[4] /= 1e-4/40;
      par[5] /= 1e-4/40;
    }
  }

  for (Int_t i = 0; i < NPAR; i++) {
    TString shn = Form("hpar%d", i+1);
    TH1F *hist = (TH1F *)TkAlign::fSarray.FindObject(shn);
    if (hist) {
      hist->SetBinContent(plfit->GetPlane(), par[i]);
      hist->SetBinError  (plfit->GetPlane(), per[i]);
    }
  }

  return ret;
}


Double_t TkAlign::fDefRange = 0.1;  // in cm

TkAlign::TkAlign(TkLadder *lad) : fLadder(lad), fDir(0)
{
}

TkAlign::~TkAlign()
{
}

void TkAlign::FillHist(Int_t i, Double_t x, Double_t y)
{
  if (i < 0 || fHarray.GetEntriesFast() <= i) return;
  TH2F *hist = (TH2F*)fHarray.At(i);
  if (hist) hist->Fill(x, y);
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
      std::cout << "Warning: entries of hist too few: " << i << " "
		<< hist->GetSumOfWeights()
		<< " tkid= " << GetTkId() << " id= " << GetId() << std::endl;
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
    std::cout << "Check " << GetTkId() << " " << fLadder->GetTkId() << " "
	      << par[0]*1e4 << " " 
	      << par[1]*1e4 << " " 
	      << par[2]*1e4 << " : "
	      << pref << " => "
	      << fLadder->posA << std::endl;
    if (fFile) {
      fFile->cd();
      fDir->GetList()->Write();
    }
  }
}


Int_t TkAlign::GetId(Int_t tkid)
{
  Int_t layer = TMath::Abs(tkid)/100;
  Int_t slot  = TMath::Abs(tkid)%100;
  Int_t side  = (tkid  < 0) ?  0 : 1;
  Int_t sign  = (side == 0) ? -1 : 1;

  return (layer-1)*32+slot*sign+16-side;
}

Int_t TkAlign::GetSn(Int_t tkid, Double_t xh)
{
  return GetSn(TkDBc::Head->FindTkId(tkid), xh);
}

Int_t TkAlign::GetSn(TkLadder *lad, Double_t xh)
{
  if (!lad) return -1;

  Int_t  sign = (lad->GetSide() == 0) ? -1 : 1;
  Double_t xs = lad->GetPlane()->GetRotMat().GetEl(0, 0)*xh;
  Double_t sp = TkDBc::Head->_SensorPitchK;
  return (Int_t)(-(xs-lad->pos[0])*sign/sp);
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

  Double_t bin[61];
  for (Int_t i = 0; i <= 60; i++) bin[i] = TMath::Power(10, -3+i*0.1);

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
  fSarray.Add(new TH3F("hist6", "Chisquare",
		       30, &bin[30], 60, bin, 60, bin));
  fSarray.Add(new TH3F("hist7", "Chisquare",
		       30, &bin[30], 60, bin, 60, bin));

  Int_t npx = nbx;
  if (fTkMap.size() == 0 && fPlVec.size() == NPLA) npx = NPLA;
  if (fTkMap.size() == 0 && fPlVec.size() == NLAY) npx = NLAY;

  Double_t bpx = npx+0.5;
  fSarray.Add(new TH1F("hpar1", "Fitpar (X)",     npx, 0.5, bpx));
  fSarray.Add(new TH1F("hpar2", "Fitpar (Y)",     npx, 0.5, bpx));
  fSarray.Add(new TH1F("hpar3", "Fitpar (Z)",     npx, 0.5, bpx));
  fSarray.Add(new TH1F("hpar4", "Fitpar (dY/dX)", npx, 0.5, bpx));
  fSarray.Add(new TH1F("hpar5", "Fitpar (dZ/dX)", npx, 0.5, bpx));
  fSarray.Add(new TH1F("hpar6", "Fitpar (dZ/dY)", npx, 0.5, bpx));

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

PlAlignVec TkAlign::fPlVec;

void TkAlign::InitVec()
{
  fPlVec.clear();

  TDirectory *fsave = gDirectory;
  if (!fsave) fsave = gROOT;

  Int_t mode = PlAlign::fMode;
  Int_t nv   = (mode == 2) ? NLAY : NPLA;
  for (Int_t i = 0; i < nv; i++) {
    TDirectory *dir;

    fsave->cd();

    TString sdn = Form("%s %d", ((mode == 2) ? "Layer" : "Plane"), i+1);

    if (fsave != gROOT)
      dir = new TDirectoryFile(Form("dir%d", i+1), sdn);
    else
      dir = new TDirectory    (Form("dir%d", i+1), sdn);
    dir->cd();
    fPlVec.push_back(new PlAlign(i+1));
  }

  fsave->cd();
}

void TkAlign::CopyVec(TFile &file)
{
  Int_t nv = fPlVec.size();
  for (Int_t i = 0; i < nv; i++) {
    PlAlign *plalg = fPlVec.at(i);
    TDirectory *dref = (TDirectory *)file.Get(Form("dir%d", i+1));
    if (dref) plalg->Copy(dref);
  }
}

TkAlign *TkAlign::Find(Int_t tkid)
{
  TkAlignIT iter = fTkMap.find(tkid);
  return (iter == fTkMap.end() ? 0 : iter->second);
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
Int_t    TkAlign::fMsc  = 0;

void TkAlign::InitPar(void)
{
  for (Int_t i = 0; i < NLAY; i++) {
    fZlay[i] = TkDBc::Head->GetZlayer(i+1);

    fErrZ[i] = 300e-4;
    if (fMsc) {
      fErrX[i] = 40e-4;
      fErrY[i] = 15e-4;
      fCmax    = 50;
    }
    else
      fErrX[i] = fErrY[i] = 40e-4;
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
    if (tkid[j] == 0 || xcog[j] < 0 || ycog[j] < 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], xcog[j], imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j], ycog[j], imlt[j]), 0);
    AMSPoint err(fErrX[ily], fErrY[ily], fErrZ[ily]);
    ttmp.Add(TkCoo::GetGlobalA(tkid[j], loc), err);
  }
  if (ttmp.LinearFit() <  0) return -1;
  if (ttmp.GetChisqX() > 50) return -1;

  Int_t nfit = 0;

  ttmp.SetRigidity(0);
  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], TMath::Abs(xcog[j]), imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j], TMath::Abs(ycog[j]), imlt[j]), 0);
    AMSPoint err(((xcog[j] > 0) ? fErrX[ily] : -1), 
		 ((ycog[j] > 0) ? fErrY[ily] :  0), fErrZ[ily]);

    AMSPoint coo = TkCoo::GetGlobalA(tkid[j], loc);
    if (xcog[j] < 0) {
      ttmp.Propagate(coo.z());
      coo[0] = ttmp.GetP0x();
    }

    trfit.Add(coo, err);
    if (err.y() > 0) nfit++;
  }
  trfit._mscat = 0;
  if (trfit.AlcarazFit(fixr) < 0) return -1;

  if (!fixr && fMsc) {
    trfit.SetRigidity(trfit.GetRigidity()*fMsc);
    trfit._mscat = 1;
    if (trfit.AlcarazFit() < 0) return -1;
  }

  Double_t chisq = trfit.GetChisqY()/trfit.GetNdofY();
  fRfit = 0;

  if (!fixr && trfit.GetRigidity() != 0) {
    Double_t argt = TMath::Abs(trfit.GetRigidity());
    Double_t logr = TMath::Log10(argt);
    Double_t csqx = trfit.GetChisqX()/trfit.GetNdofX();
    Double_t csqy = trfit.GetChisqY()/trfit.GetNdofY();

    TH3F *hist0 = (TH3F *)fSarray.FindObject("hist6");
    TH3F *hist1 = (TH3F *)fSarray.FindObject("hist7");
    hist0->Fill(argt, csqx, csqy);

    if (!fMsc && (csqx > 100 || csqy > 100)) return -2;
    FillR(trfit, idx, tkid);

    if (chisq < fCmax) hist1->Fill(argt, csqx, csqy);
  }
  if (chisq > fCmax) return -chisq;

  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] != 0) Fill(tkid[j], trfit, i);

    Int_t ily = TMath::Abs(tkid[j])/100-1;
    Int_t ipl = (PlAlign::fMode == 2) ? ily
                                      : TkDBc::Head->_plane_layer[ily]-1;

    PlAlign *palg = (0 <= ipl && ipl < fPlVec.size()) ? fPlVec.at(ipl) : 0;
    if (palg) {
      if (i > 0) trfit.PropagateFast(i-1, i, 4);
      palg->Fill(trfit, i);
    }
  }

  return chisq;
}

void TkAlign::Proc(Int_t bits)
{
  if (fPlVec.size() >= NPLA) {
    for (Int_t i = 0; i < fPlVec.size(); i++) {
      fPlVec.at(i)->Prof(2); //();

      if (!(bits & kPlFit)) continue;

      if (0) { //(PlAlign::fMode == 1) {
	Int_t bpla = bits & 0x3f;
	if (bpla && !(bpla & (1 << i))) continue;
      }
      else if (i >= 7) continue;

      Int_t fix = (bits >> 6) & 0x3f;
      PlAlign::Minimize(fPlVec.at(i), fix);
    }
  }

  if (fTkMap.size() > 0) {
    for (TkAlignIT it = fTkMap.begin(); it != fTkMap.end(); it++)
      if (it->second) it->second->ProcHists(bits);
  }
}


void TkAlign::Fill(Int_t tkid, TrFit &fit, Int_t ih)
{
  Double_t xh = fit.GetXh(ih);
  Double_t xr = fit.GetXr(ih);
  Double_t yr = fit.GetYr(ih);
  Double_t xd = fit.GetDxDz();
  Double_t yd = fit.GetDyDz();
  Double_t sn = GetSn(tkid, xh);

  Int_t layer = TMath::Abs(tkid)/100;
  Int_t il    = layer-1;
  if (layer == 8 || layer == 9) {
    if (fErrX[il] == 0) xr *= 0.1;
    if (fErrY[il] == 0) yr *= 0.1;
  }

  if (fit.GetXs(ih) >= 0)
  FillHist(0, tkid, xd, xr);   // dX VS dX/dZ
  FillHist(1, tkid, yd, yr);   // dY VS dY/dZ
  FillHist(2, tkid, xh, yr);   // dY VS X
  FillHist(3, tkid, sn, yr);   // dY VS Sen

  if (TMath::Abs(yd) > 0.15)
    FillHist(4, tkid, xh, yr/yd); // dZ VS X

  TH3F *hist = (TH3F *)fSarray.FindObject("hist0");
  hist->Fill(fit.GetXh(ih), fit.GetYh(ih), layer);
}

void TkAlign::FillR(TrFit &trfit, Int_t *idx, Int_t *tkid)
{
  if (fTbNpos > 0 && fTbPos > 0) {
    if (trfit.GetChisqY() < fCmax) {
      TH2F *hist = (TH2F*)fSarray.FindObject("histr");
      hist->Fill(fTbPos, fRref/trfit.GetRigidity()-1);
      fRfit = trfit.GetRigidity();
    }
  }

  else {
    TH3F *hist = (TH3F*)fSarray.FindObject("histr");

    fRfit = trfit.GetRigidity();

    Int_t nhit = trfit.GetNhit();
    for (Int_t i = 0, k = 0; i < nhit; i++) {
      Int_t j = idx[i];
      if (tkid[j] == 0) continue;

      Int_t layer = TMath::Abs(tkid[j])/100;
      hist->Fill(layer, trfit.GetYh(k++), 1e3/trfit.GetRigidity());
    }
  }
}

void TkAlign::FillHist(Int_t i, Int_t tkid, Double_t x, Double_t y)
{
  TkAlign *tkalg = Find(tkid);
  if (tkalg) tkalg->FillHist(i, x, y);

  if (i < 0 || fSarray.GetEntriesFast() <= i) return;
  TH3F *hist = (TH3F*)fSarray.At(i);
  if (hist) hist->Fill(GetId(tkid), x, y);
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

  Double_t pfix[3] = { 0, 0, 0 };

  if (fProfMode == 3) {
    htmp = hist->ProjectionY("htmp");

    Double_t sig1 = 800e-4;
    Double_t sig2 = 600e-4;

    if (htmp->GetRMS() < 0.01) {
      sig1 = 20e-4;
      sig2 = 80e-4;
    }

    TF1 *func = new TF1("func",
			"[0]/(exp((x-[1])/[2])+exp(-(x-[1])/[2]))+"
		    "[3]*[0]/(exp((x-[1])/[4])+exp(-(x-[1])/[4]))");
    func->SetParameters(htmp->GetMaximum(), 0, sig1, 0.2, sig2);
    if (htmp->Fit(func, "q0") != 0) htmp->Fit(func, "q0");
    pfix[0] = TMath::Abs(func->GetParameter(2));
    pfix[1] = TMath::Abs(func->GetParameter(4));
    pfix[2] = func->GetParameter(3);

    if (pfix[0] > pfix[1]) {
      pfix[0] = TMath::Abs(func->GetParameter(4));
      pfix[1] = TMath::Abs(func->GetParameter(2));
      pfix[2] = 1/func->GetParameter(3);
    }

    if (pfix[0] < 1e-4 || pfix[1] < 1e-4 || pfix[2] < 0.01 ||
	func->GetParError(1) > 10e-4) {
      std::cout << Form("chk: %22s %6.1f %6.1f %5.2f | %5.2f %.0f",
			hist->GetTitle(), pfix[0]*1e4, pfix[1]*1e4, pfix[2],
			func->GetParError(1)*1e4,
			htmp->GetSumOfWeights()) << std::endl;
      pfix[0] = pfix[1] = pfix[2] = -1;
    }
  }

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
      else if (fProfMode == 2 || (fProfMode == 3 && pfix[0] < 0)) {
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
	func->SetParameters(htmp->GetMaximum(), 0, pfix[0], pfix[2], pfix[1]);
	func->FixParameter(2, pfix[0]);
	func->FixParameter(3, pfix[2]);
	func->FixParameter(4, pfix[1]);

	if (htmp->Fit(func, "q0") != 0) htmp->Fit(func, "q0");

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

TH2F *TkAlign::Profile(TH3 *hist)
{
  if (!hist) return 0;

  TString hname = hist->GetName();
  hname.ReplaceAll("hist", "prof");
  hname.ReplaceAll("proj", "prof");
  if (hname == "hist") hname += "_pf";

  Int_t nbx = hist->GetNbinsX();
  Int_t nby = hist->GetNbinsY();
  Int_t nbz = hist->GetNbinsZ();

  TAxis *ax = hist->GetXaxis();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  TH1F *htmp = new TH1F("htmp", "Res(Y)", 
			nbz, az->GetXmin(), az->GetXmax());
  for (Int_t i = 0; i < nbz; i++)
    htmp->SetBinContent(i+1, hist->Integral(1, nbx, 1, nby, i+1, i+1));

  TH2F *prof = new TH2F("prof", hist->GetTitle(),
			nbx, ax->GetXmin(), ax->GetXmax(),
			nby, ay->GetXmin(), ay->GetXmax());

  Int_t nmin = 50;

  Double_t pfix = 0;

  TF1  *func = 0;
  TH1F *hchk = 0;

  if (fProfMode == 3) {
    func = new TF1("func",
		   "[0]/(exp((x-[1])/[2])+exp(-(x-[1])/[2]))");
    hchk = new TH1F("hchk", "hchk", nbx*nby, 0.5, 9.5);
  }

  prof->Reset();

  Int_t nfill = 0;

  for (Int_t i = 0; i < nbx; i++) {
    for (Int_t j = 0; j < nby; j++) {

      htmp->Reset();
      for (Int_t k = 0; k < nbx; k++)
	htmp->SetBinContent(k+1, hist->GetBinContent(i+1, j+1, k+1));

      if (htmp->GetSumOfWeights() < nmin) continue;

      Double_t par = 0, per = 0;

      // Fitting on cosmic-ray data with long tail
      if (fProfMode == 3) {
	htmp->Fit("gaus", "q0");
	Double_t sig = htmp->GetFunction("gaus")->GetParameter(2);
	if (sig > htmp->GetRMS()) sig = htmp->GetRMS();

	func->SetParameters(htmp->GetMaximum(), 0, sig);

	if (htmp->Fit(func, "q0") != 0) htmp->Fit(func, "q0");
	if (func->GetParError(2) > 10e-4) continue;

	par = func->GetParameter(1);
	per = func->GetParError (1);

	hchk->SetBinContent(i*nby+j+1, TMath::Abs(func->GetParameter(2)));
	hchk->SetBinError  (i*nby+j+1, func->GetParError(2));

	if (TMath::Abs(par) > 50e-4) {
	  cout << Form("Check %3d %2d %6.1f %5.1f %5.1f",
		       i, j+1, par*1e4, per*1e4, 
		       TMath::Abs(func->GetParameter(2))*1e4) << endl;
	  htmp->Clone(Form("htmp%03d%02d", i, j+1));
	}
      }
      else {
	htmp->Fit("gaus", "q0");
	TF1 *func = htmp->GetFunction("gaus");
	par = func->GetParameter(1);
	per = func->GetParError (1);
      }

      prof->SetBinContent(i+1, j+1, par);
      prof->SetBinError  (i+1, j+1, per);
      if (per > 0) nfill++;
    }
  }
  prof->SetEntries(nfill);

  delete htmp;

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
