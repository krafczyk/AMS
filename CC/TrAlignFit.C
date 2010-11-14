// $Id: TrAlignFit.C,v 1.8 2010/11/14 07:51:48 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrAlignFit.C
///\brief Source file of TrAlignFit
///
///\date  2007/04/02 SH  First test version
///$Date: 2010/11/14 07:51:48 $
///
///$Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrAlignFit.h"
#include "TrFit.h"
#include "TkDBc.h"

#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "TMath.h"
#include "TString.h"
#include "TStopwatch.h"
#include "TSystem.h"

#include <iostream>
#include <fstream>

TObjArray TrAlignObject::sharray;
Int_t   TrAlignObject::ProfMode = 0;
Int_t   TrAlignObject::fLcommon = 0;
Float_t TrAlignObject::fRange   = 1;

TrAlignObject::TrAlignObject(Int_t id) : tkid(id)
{
  for (Int_t i = 0; i < Nalg; i++) fixp[i] = 0;
}

TrAlignObject::~TrAlignObject()
{
}

void TrAlignObject::Reset(void)
{
  for (Int_t i = 0; i < Ndim; i++) param[i] = 0;
  phi = pitch = roll = 0;
}

void TrAlignObject::InitSHists(void)
{
  Int_t    nb = TrAlignFit::Nplan*32;
  Double_t bx = nb+0.5;

  sharray.Clear();
  sharray.Add(new TH1F("hist1", "Fitpar (X)",     nb, 0.5, bx));
  sharray.Add(new TH1F("hist2", "Fitpar (Y)",     nb, 0.5, bx));
  sharray.Add(new TH1F("hist3", "Fitpar (Z)",     nb, 0.5, bx));
  sharray.Add(new TH1F("hist4", "Fitpar (dY/dX)", nb, 0.5, bx));
  sharray.Add(new TH1F("hist5", "Fitpar (dZ/dX)", nb, 0.5, bx));
  sharray.Add(new TH1F("hist0", "X-Scale",        nb, 0.5, bx));
  sharray.Add(new TH3F("histx", "Residual VS angle (X)", 
		       nb, 0.5, bx, 20, -1, 1, 100, -fRange, fRange));
  sharray.Add(new TH3F("histy", "Residual VS angle (Y)", 
		       nb, 0.5, bx, 20, -1, 1, 100, -fRange, fRange));
  sharray.Add(new TH3F("hists", "Residual (X) VS Sensor", 
		       nb, 0.5, bx, 15, 0, 15, 100, -fRange, fRange));
  sharray.Add(new TH3F("histp", "Residual (Y) VS Sensor", 
		       nb, 0.5, bx, 15, 0, 15, 100, -fRange, fRange));

  if (TrAlignFit::fFitM == TrAlignFit::kCurved) {
    sharray.Add(new TH2F("hfit1", "1/R VS X0", 100, -50, 50, 1000, -0.1, 0.1));
    sharray.Add(new TH2F("hfit2", "1/R VS Y0", 100, -50, 50, 1000, -0.1, 0.1));
    sharray.Add(new TH2F("hfit3", "1/R VS dX", 100,  -1,  1, 1000, -0.1, 0.1));
    sharray.Add(new TH2F("hfit4", "1/R VS dY", 100,  -1,  1, 1000, -0.1, 0.1));

    Double_t bin[31];
    Double_t rng = fRange*2;
    for (Int_t i = 0; i <= 30; i++) bin[i] = TMath::Power(10, i*0.1);
    sharray.Add(new TH2F("hfit5", "ResY8 VS Rfit", 30, bin, 100, -rng, rng));
    sharray.Add(new TH2F("hfit6", "ResY9 VS Rfit", 30, bin, 100, -rng, rng));

    if (TrAlignFit::fMode/10 == TrAlignFit::kMCSim/10) {
     sharray.Add(new TH2F("hdfr1", "dR VS X0", 100, -50, 50, 1000, -0.3, 0.3));
     sharray.Add(new TH2F("hdfr2", "dR VS Y0", 100, -50, 50, 1000, -0.3, 0.3));
     sharray.Add(new TH2F("hdfr3", "dR VS dX", 100,  -1,  1, 1000, -0.3, 0.3));
     sharray.Add(new TH2F("hdfr4", "dR VS dY", 100,  -1,  1, 1000, -0.3, 0.3));
    }
  }
}

void TrAlignObject::InitHists(TDirectory *dir)
{
  if (harray.GetEntries() > 0) {
    Int_t nh = harray.GetEntries();
    for (Int_t i = 0; i < nh; i++)
      ((TH1 *)harray.At(i))->Reset();
    return;
  }

  ldir = dir;
  ldir->cd();

  Int_t slot = TMath::Abs(tkid)%100;
  Int_t sign = (tkid < 0) ? -1 : 1;
  cdir = new TDirectory(Form("dir%c%02d", (sign > 0) ? 'p' : 'n', slot),
			Form("TkId %d", tkid));
  cdir->cd();

  Float_t rng = fRange;
  harray.Add(new TH2F("hist001", "dX VS X",     30, -60, 60, 100, -rng, rng));
  harray.Add(new TH2F("hist002", "dY VS X",     30, -60, 60, 100, -rng, rng));
  harray.Add(new TH2F("hist003", "dX VS dX/dZ", 20,  -1,  1, 100, -rng, rng));
  harray.Add(new TH2F("hist004", "dY VS dY/dZ", 20,  -1,  1, 100, -rng, rng));
  harray.Add(new TH1F("hist005", "dZ(X) VS X",  30, -60, 60));
  harray.Add(new TH1F("hist006", "dZ(Y) VS X",  30, -60, 60));

  TH2F *hist1 = (TH2F *)harray.At(0);
  for (Int_t i = 0; i < 30; i++) {
    Double_t x1 = hist1->GetXaxis()->GetBinLowEdge(i+1);
    Double_t x2 = hist1->GetXaxis()->GetBinLowEdge(i+2);
    harray.Add(new TH2F(Form("hist%02d1", i+1), 
			Form("dX VS dX/dZ (%.0f<X<%.0f)", x1, x2), 
			20, -1, 1, 100, -rng, rng));
    harray.Add(new TH2F(Form("hist%02d2", i+1), 
			Form("dY VS dX/dZ (%.0f<X<%.0f)", x1, x2), 
			20, -1, 1, 100, -rng, rng));
  }
}

void TrAlignObject::ResetHists(void)
{
  Int_t nh = harray.GetEntries();
  for (Int_t i = 0; i < nh; i++)
    ((TH1 *)harray.At(i))->Reset();
}

void TrAlignObject::FillHist(Int_t i, Double_t x, Double_t y)
{
  TH2F *hist = (TH2F*)harray.At(i);
  if (hist) hist->Fill(x, y);
}

void TrAlignObject::ProcHists(Int_t bits)
{
  if (TMath::Abs(cool[2]) > 60 && !fLcommon) return;

  cdir->cd();

  Double_t par[6], per[6];
  for (Int_t i = 0; i < 6; i++) par[i] = per[i] = 0;

  TH1F *hdzx = (TH1F *)harray.At(4);
  TH1F *hdzy = (TH1F *)harray.At(5);
  Int_t nfit = 0;

  Int_t nh = harray.GetEntries();
  for (Int_t i = 0; i < nh; i++) {
    if (i == 4 || i == 5) continue;

    TH2F  *hist = (TH2F *)harray.At(i);
    Bool_t hdel = kFALSE;

    if (fLcommon == 1) {
      if (i != 2 && i != 3) continue;
      Int_t lay = TMath::Abs(tkid)/100;
      Int_t ix1 = lay*32-31;
      Int_t ix2 = lay*32;
      hist = Project((TH3F *)sharray.At(4+i), ix1, ix2);
      hdel = kTRUE;
    }
    if (fLcommon == 2) {
      if (i != 0 && i != 1) continue;
      Int_t lay = TMath::Abs(tkid)/100;
      if (lay == 8 || lay == 9) {
	Int_t ix1 = lay*32-31;
	Int_t ix2 = lay*32;
	hist = Project((TH3F *)sharray.At(6+i), ix1, ix2);
	hdel = kTRUE;
      }
    }
    if (hist->GetSumOfWeights() < 100) continue;

    TH1D *prof = Profile(hist);
    Int_t npt = 0;
    for (Int_t j = 0; j < prof->GetNbinsX(); j++)
      if (prof->GetBinError(j+1) > 0) npt++;
    if (npt < 2) continue;

    prof->Fit("pol1", "q0");
    TF1 *func = prof->GetFunction("pol1");

    if (i == 0) {
      if (bits & XYcorr1) {
	par[0] = -func->GetParameter(0);
	per[0] =  func->GetParError (0);
      }
      else if (bits & XYcorr3)
	SigmaProj(hist, par[0], per[0]);

      par[5] = func->GetParameter(1);
      per[5] = func->GetParError (1);
    }

    else if (i == 1) {
      if (bits & XYcorr1) {
	par[1] = -func->Eval(cool[0]);
	per[1] =  func->GetParError (0);
      }
      else if (bits & XYcorr3)
	SigmaProj(hist, par[1], per[1]);

      if (bits & dYXcorr) {
	par[3] = -func->GetParameter(1);
	per[3] =  func->GetParError (1);
      }
    }

    else if (i == 2) {
      if (bits & XYcorr2) {
	par[0] = -func->GetParameter(0);
	per[0] =  func->GetParError (0);
      }
      if (bits & Zcorr1) {
	par[2] = func->GetParameter(1);
	per[2] = func->GetParError (1);
      }
    }

    else if (i == 3) {
      if (bits & XYcorr2) {
	par[1] = -func->GetParameter(0);
	per[1] =  func->GetParError (0);
      }
      if (bits & Zcorr2) {
	par[2] = func->GetParameter(1);
	per[2] = func->GetParError (1);
      }
    }

    else {
      TH1F *hdz = (i%2 == 0) ? hdzx : hdzy;
      hdz->SetBinContent((i-6)/2+1, func->GetParameter(1));
      hdz->SetBinError  ((i-6)/2+1, func->GetParError (1));
      if (i%2 == 1) nfit++;
    }

    if (hdel) delete hist;
  }

  if (nfit > 2 && (bits & dZXcorr)) {
    hdzy->Fit("pol1", "q0");
    TF1 *func2 = hdzy->GetFunction("pol1");
    par[4] = -func2->GetParameter(1);
    per[4] =  func2->GetParError (1);
    par[2] =  func2->Eval(cool[0]);
    per[2] =  func2->GetParError(0);
  }

  Int_t ly = TMath::Abs(tkid)/100;
  Int_t sl = tkid%100;
  Int_t id = (ly-1)*32+sl+16-((tkid > 0) ? 1 : 0);

  for (Int_t i = 0; i < 6; i++) {
    Double_t factor = (i >= 3) ? 600 : 1;
    TH1F *hist = (TH1F *)sharray.At(i);
    hist->SetBinContent(id, par[i]*factor);
    hist->SetBinError  (id, per[i]*factor);

    if (i < 3 && fixp[i] == 1) param[i] += par[i]*0.1;
  }
  if (fixp[4] == 1) phi   += par[3]*0.1;
  if (fixp[3] == 1) pitch += par[4]*0.1;
  if (fixp[5] == 1) roll  += par[5]*0.1;
}

void TrAlignObject::SigmaProj(TH2 *hist, Double_t &par, Double_t &err)
{
  par = err = 0;

  TH1D *hprj = hist->ProjectionY("hprj");

  Double_t nmin = 100;

  if (ProfMode == 1) {
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

  else if (ProfMode == 2) {
    if (hprj->GetSumOfWeights() > nmin) {
      hprj->Fit("gaus", "q0");
      TF1 *func = hprj->GetFunction("gaus");
      par = -func->GetParameter(1);
      err =  func->GetParError (1);
    }
  }

  delete hprj;
}

AMSPoint TrAlignObject::GetCoord(Double_t xx, Double_t yy, Double_t zz)
{
  Double_t sy = TMath::Sin(phi),   cy = TMath::Cos(phi);
  Double_t sp = TMath::Sin(pitch), cp = TMath::Cos(pitch);
  Double_t sr = TMath::Sin(roll),  cr = TMath::Cos(roll);

  Double_t mtx[3][3];
  mtx[0][0] =  cy*cp; mtx[0][1] = cr*sy*cp-sr*sp; mtx[0][2] = -sr*sy*cp-cr*sp;
  mtx[1][0] = -sy;    mtx[1][1] = cr*cy;          mtx[1][2] = -sr*cy;
  mtx[2][0] =  cy*sp; mtx[2][1] = cr*sy*sp+sr*cp; mtx[2][2] = -sr*sy*sp+cr*cp;

  Double_t s1 = xx-cool[0];
  Double_t s2 = yy-cool[1];
  Double_t s3 = zz-cool[2]+mtx[0][2]*s1+mtx[1][2]*s2;

  AMSPoint coo;
  coo[0] = mtx[0][0]*s1+mtx[1][0]*s2+mtx[2][0]*s3+cool[0]+param[0];
  coo[1] = mtx[0][1]*s1+mtx[1][1]*s2+mtx[2][1]*s3+cool[1]+param[1];
  coo[2] = mtx[0][2]*s1+mtx[1][2]*s2+zz                  +param[2];

  return coo;
}

TH1D *TrAlignObject::Profile(TH2 *hist)
{
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
      if (ProfMode == 1) {
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
      else if (ProfMode == 2) {
	htmp = hist->ProjectionY("htmp", i+1, i+1);
	htmp->Fit("gaus", "q0");
	TF1 *func = htmp->GetFunction("gaus");
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
  }
  prof->SetEntries(hist->GetSumOfWeights());

  delete htmp;

  delete [] hx;
  delete [] hy;
  delete [] hw;

  return prof;
}

TH2F *TrAlignObject::Project(TH3 *hist, Int_t ibx1, Int_t ibx2)
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

Int_t TrAlignObject::Fit2D(Int_t n, Double_t *x, Double_t *y, 
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

Int_t TrAlignObject::Inv3x3(Double_t mtx[3][3])
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

ClassImp(TrAlignFit)

TString  TrAlignFit::fNameOut = "alignfit.root";
Int_t    TrAlignFit::fMode    = TrAlignFit::kCosmicRay;
Int_t    TrAlignFit::fFitM    = TrAlignFit::kLinear;
Double_t TrAlignFit::fFixR    = 0;

Int_t    TrAlignFit::fMaxIter  = 20;
Int_t    TrAlignFit::fMinIter  =  5;
Int_t    TrAlignFit::fFinIter  = 10;//15;
Double_t TrAlignFit::fMaxChisq = 10;

TrAlignFit::TrAlignFit(Int_t maxcases)
{
  fMaxCases = maxcases;
  fNbuf = Nplan*Narr*fMaxCases;
  fNcbf = Nplan*fMaxCases;

  fArray = new Float_t[fNbuf];
  fCrray = new char[fNcbf];
  fPrray = 0;
  
  if (TrAlignFit::fMode/10 == TrAlignFit::kMCSim/10)
    fPrray = new Float_t[fMaxCases];

  Init();

  fSigma[0] = 500e-4;
  fSigma[1] = 200e-4;

  fFile = 0;
}

TrAlignFit::~TrAlignFit()
{
  if (fFile) {
    fFile->Write();
    delete fFile;
  }

  delete [] fArray;
  delete [] fCrray;
  delete [] fPrray;
}

void TrAlignFit::Init(void)
{
  fNcases = 0;

  fFile = 0;

  Float_t zdef[Nplan] = { 53.03,  29.258,  25.182,  1.728,
			 -2.348, -25.182, -29.258, -53.03, -135 };
  if (TkDBc::Head) {
    for (Int_t i = 0; i < Nplan; i++) {
      zdef[i] = TkDBc::Head->GetZlayer(i+1);
    }
  }

  SfFill(0, 0, zdef);
  for (Int_t i = 0; i < Nplan; i++) SfFill(i+1, 0, zdef);
  for (Int_t i = 0; i < Nplan; i++) 
    for (Int_t j = i+1; j < Nplan; j++) SfFill(i+1, j+1, zdef);
}

void TrAlignFit::Set(Float_t arr[Ndim][Nplan], Int_t prm[Ndim][Nplan], 
		     Float_t mcp)
{
  if (fNcases >= fMaxCases) return;

  for (Int_t i = 0; i < Nplan; i++) {

    if (arr[3][i] == 0) {
      for (Int_t j = 0; j < Narr; j++)
	SetArray(fNcases, i, j, 0);
      SetCrray(fNcases, i, 0);
      continue;
    }

    Int_t slot = (Int_t)arr[3][i];
    Int_t sign = (arr[4][i] == 1) ? -1 : 1;
    Int_t tkid = sign*((i+1)*100+slot);
    SetCrray(fNcases, i, slot*sign);

    TrAlignObject *obj = FindTkId(tkid);
    if (!obj) {
      obj = new TrAlignObject(tkid);
      fAlignMap[tkid] = obj;
    }

    for (Int_t j = 0; j < 6; j++) {
      if (j < Narr) SetArray(fNcases, i, j, arr[j][i]);
      if (j < 3) obj->cool[j] = arr[7+j][i];
      if (j < 6) obj->fixp[j] = prm[j][i];
    }
  }

  SetPrray(fNcases, mcp);

  fNcases++;
}

void TrAlignFit::Fit(void)
{
  std::cout << " fNcases " << fNcases << std::endl;

  if (fFitM != kLinear) {
    std::cout << " FitMethod= " << fFitM;
    if (fFitM == kRgtFix) std::cout << " RigidityFix= " << fFixR;
    std::cout << std::endl;
  }

  TStopwatch timer;
  timer.Start();
  Float_t ptime = 0;

  if (!fFile) fFile = new TFile(fNameOut, "recreate");

  for (aligIT it = fAlignMap.begin(); it != fAlignMap.end(); it++) {
    TrAlignObject *algp = it->second;
    if (algp) algp->Reset();
  }

  gROOT->cd();
  for (Int_t i = 0; i < Nplan; i++) {
    TDirectory *ldir 
      = new TDirectory(Form("dir%d", i+1), Form("Layer %d", i+1));

    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < 2; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	TrAlignObject *algp = FindTkId(tkid);
	if (!algp) continue;

	algp->InitHists(ldir);
      }
    }
  }

  for (Int_t iter = 0; iter <= fMaxIter; iter++) {

    InitHists(iter);

    Double_t chisqsum = 0;
    Int_t ntot = 0;

    // Loop for each case
    for (fIcase = 0; fIcase < fNcases; fIcase++) {
      if (FitPoints() < 0) continue;

      for (Int_t i = 0; i < 2; i++) {
	if (fChisq[i] < fMaxChisq ||
	   (fChisq[i] < fMaxChisq*10 && iter < fMinIter)) {
	  FillHists(i);
	  chisqsum += fChisq[i];
	  ntot++;
	}
      }
    }

    chisqsum /= ntot;
    PrintStat(iter, chisqsum, ntot, timer.CpuTime()-ptime);
    ptime = timer.CpuTime();
    timer.Continue();

    if (iter < fMaxIter) ProcHists(iter);
  }

  ParmHists();

  std::cout << "CpuTime " << timer.CpuTime() << std::endl;
}

void TrAlignFit::Get(Float_t arr[Ndim][Nplan], Int_t what)
{
  for (Int_t j = what/2-1; j < Nlad; j++) {
    for (Int_t k = what%2; k < 2; k++) {
      for (Int_t i = 0; i < Nplan; i++) {

	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	TrAlignObject *algp = FindTkId(tkid);

	for (Int_t l = 0; l < 3; l++) {
	  arr  [l][i] = (algp) ? algp->param[l] : 0;
	  arr[8+l][i] = (algp) ? algp->cool [l] : 0;
	}
	arr[3][i] = (algp) ? algp->pitch : 0;
	arr[4][i] = (algp) ? algp->phi   : 0;
	arr[5][i] = (algp) ? algp->roll  : 0;
	arr[6][i] = j+1;
	arr[7][i] = k+1;
      }
      return;
    }
  }
}

Int_t TrAlignFit::FitPoints(void)
{
  Int_t lmiss[2] = { 0, 0 }, nm = 0;
  for (Int_t i = 0; i < Nplan; i++) {
    Int_t slot = GetCrray(fIcase, i);
    if (slot == 0) {
      if (nm < 2) lmiss[nm] = i+1;
      nm++;
    }
  }
  if (nm > 2) return -1;

  TrFit fit;
  if (fFitM == kRgtFix) fit.SetRigidity(fFixR);

  Int_t    idx[Nplan];
  Double_t zht[Nplan];
  for (Int_t i = 0; i < Nplan; i++) {
    Int_t slot = GetCrray(fIcase, i);
    zht[i] = (slot != 0) ? GetArray(fIcase, i, 2) : -200-i;
  }
  TMath::Sort<Double_t,Int_t>(Nplan, zht, idx);

  for (Int_t i = 0; i < Nplan; i++) {
    Int_t slot = GetCrray(fIcase, idx[i]);
    if (slot == 0) continue;

    Int_t sign = TMath::Sign(1, slot);
    Int_t tkid = sign*((idx[i]+1)*100+TMath::Abs(slot));

    TrAlignObject *algp = FindTkId(tkid);
    if (!algp) {
      std::cout << "WARNING algp not found: " << tkid << std::endl;
      continue;
    }

    AMSPoint coord = algp->GetCoord(GetArray(fIcase, idx[i], 0), 
				    GetArray(fIcase, idx[i], 1), 
				    GetArray(fIcase, idx[i], 2));
    if (TMath::Abs(coord.z()) < 1e-3) continue;

    AMSPoint sig(fSigma[0], fSigma[1], 1);
    if (TMath::Abs(coord.z()) > 60) sig.setp(0, 0, 0);
    fit.Add(coord, sig);
  }

  Double_t ret = 0;
  if      (fFitM == kLinear) ret = fit.Fit(TrFit::LINEAR);
  else if (fFitM == kCurved) ret = fit.AlcarazFit(0);
  else if (fFitM == kRgtFix) ret = fit.AlcarazFit(1);

  if (ret < 0) return -1;

  if (fFitM == kCurved) fit.Propagate(0);

  for (Int_t i = 0, j = 0; i < Nplan; i++) {
    if (GetCrray(fIcase, idx[i]) == 0) continue;
    fResidual[0][idx[i]] = fit.GetXr(j);
    fResidual[1][idx[i]] = fit.GetYr(j);
    j++;
  }
  fChisq[0] = fit.GetChisqX();
  fChisq[1] = fit.GetChisqY();

  fGradient[0] = fit.GetDxDz();
  fGradient[1] = fit.GetDyDz();

  if (fFitM == kCurved) {
    TH2F *hist1 = (TH2F*)TrAlignObject::sharray.At(10);
    TH2F *hist2 = (TH2F*)TrAlignObject::sharray.At(11);
    TH2F *hist3 = (TH2F*)TrAlignObject::sharray.At(12);
    TH2F *hist4 = (TH2F*)TrAlignObject::sharray.At(13);
    TH2F *hist5 = (TH2F*)TrAlignObject::sharray.At(14);
    TH2F *hist6 = (TH2F*)TrAlignObject::sharray.At(15);

    Double_t rgt = fit.GetRigidity();
    if (rgt != 0) {
      if (hist1) hist1->Fill(fit.GetP0x (), 1/rgt);
      if (hist2) hist2->Fill(fit.GetP0y (), 1/rgt);
      if (hist3) hist3->Fill(fit.GetDxDz(), 1/rgt);
      if (hist4) hist4->Fill(fit.GetDyDz(), 1/rgt);

      for (Int_t i = 0; i < Nplan; i++) {
	if (GetCrray(fIcase, i) == 0 || fResidual[1][i] == 0) continue;
	if (i == 7 && hist5) hist5->Fill(TMath::Abs(rgt), fResidual[1][i]);
	if (i == 8 && hist6) hist6->Fill(TMath::Abs(rgt), fResidual[1][i]);
//	if ((i == 7 || i == 8) && TMath::Abs(rgt) < 5)
//	  fResidual[0][i] = fResidual[1][i] = 0;
      }

      if (TrAlignFit::fMode/10 == TrAlignFit::kMCSim/10 && 
	  GetPrray(fIcase) != 0) {
	TH2F *hist1 = (TH2F*)TrAlignObject::sharray.At(16);
	TH2F *hist2 = (TH2F*)TrAlignObject::sharray.At(17);
	TH2F *hist3 = (TH2F*)TrAlignObject::sharray.At(18);
	TH2F *hist4 = (TH2F*)TrAlignObject::sharray.At(19);

	Float_t pref = GetPrray(fIcase);
	if (hist1) hist1->Fill(fit.GetP0x (), 1/rgt-1/pref);
	if (hist2) hist2->Fill(fit.GetP0y (), 1/rgt-1/pref);
	if (hist3) hist3->Fill(fit.GetDxDz(), 1/rgt-1/pref);
	if (hist4) hist4->Fill(fit.GetDyDz(), 1/rgt-1/pref);
      }
    }
  }

  return 0;
}

void TrAlignFit::InitHists(Int_t iter)
{
  for (Int_t i = 0; i < Nplan; i++) {
    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < 2; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	TrAlignObject *algp = FindTkId(tkid);
	if (!algp) continue;
	algp->ResetHists();
      }
    }
  }

  fFile->cd();
  fDir = new TDirectoryFile(Form("iter%d", iter), 
			    Form("Iteration %d", iter));
  fDir->cd();
  TrAlignObject::InitSHists();

  Double_t bin[51];
  for (Int_t i = 0; i <= 50; i++) bin[i] = TMath::Power(10, -1+i*0.1);
  new TH1F("hcsqx", "ChisqX", 50, bin);
  new TH1F("hcsqy", "ChisqY", 50, bin);
}

void TrAlignFit::FillHists(Int_t i)
{
  Double_t rmax = 0.1;

  for (Int_t j = 0; j < Nplan; j++) {
    Int_t slot = GetCrray(fIcase, j);
    if (slot == 0) continue;

    Int_t sign = TMath::Sign(1, slot);
    Int_t tkid = sign*((j+1)*100+TMath::Abs(slot));

    TrAlignObject *algp = FindTkId(tkid);
    if (!algp) {
      std::cout << "WARNING algp not found: " << tkid << std::endl;
      continue;
    }
    if (TMath::Abs(fResidual[i][j]) > rmax || fResidual[i][j] == 0) continue;

    Double_t xh = GetArray(fIcase, j, 0);
    Int_t m = (Int_t)((xh+60)/60*15);
    if (0 <= m && m < 30) {
      algp->FillHist(i, xh, fResidual[i][j]*10);
      algp->FillHist(i+2,       fGradient[i], fResidual[i][j]*10);
      algp->FillHist(i+(m+3)*2, fGradient[i], fResidual[i][j]*10);
    }

    TkLadder *lad = TkDBc::Head->FindTkId(tkid);
    Int_t id = j*32+slot+16-((tkid > 0) ? 1 : 0);
    Double_t sn = -(xh-lad->pos[0])*sign/TkDBc::Head->_SensorPitchK;
    TH3F *hist1 = (TH3F*)TrAlignObject::sharray.At(6+i);
    TH3F *hist2 = (TH3F*)TrAlignObject::sharray.At(8+i);
    hist1->Fill(id, fGradient[i], fResidual[i][j]*10);
    hist2->Fill(id,           sn, fResidual[i][j]*10);
  }

  TH1F *hcsq = (TH1F *)fDir->Get((i == 0) ? "hcsqx" : "hcsqy");
  hcsq->Fill(fChisq[i]);
}

void TrAlignFit::PrintStat(Int_t iter, Double_t chisq, 
			   Int_t ntot, Double_t time)
{
  // Get memory
  ifstream ftmp(Form("/proc/%d/status", gSystem->GetPid()));
  TString sbuf;
  Float_t vmsize = 0;
  while (!sbuf.ReadToken(ftmp).eof()) {
    if (sbuf == "VmSize:") {
      ftmp >> vmsize;
      break;
    }
  }

  std::cout << Form(" iter %3d %8.3f %7d %4.0f sec %5.1f MB" ,
		    iter, chisq, ntot, time, vmsize/1024) << std::endl;
}

void TrAlignFit::Sfactor(Int_t nhit, Float_t *zpos, 
			 Float_t *xsig, Float_t *xfac)
{
  Double_t s = 0;
  for (Int_t i = 0; i < nhit; i++) s += 1/xsig[i]/xsig[i];

  Double_t z0 = 0;
  for (Int_t i = 0; i < nhit; i++) z0 += zpos[i]/xsig[i]/xsig[i]/s;

  Double_t zz = 0;
  for (Int_t i = 0; i < nhit; i++)
    zz += (zpos[i]-z0)*(zpos[i]-z0)/xsig[i]/xsig[i]/s;

  for (Int_t i = 0; i < nhit; i++) {
    Double_t xf = 0;

    for (Int_t j = 0; j < nhit; j++) {
      Double_t ww  = 1/xsig[j]/xsig[j]/s;
      Double_t zzp = (zpos[i]-z0)*(zpos[j]-z0)*ww/zz;
      xf += (i == j) ? (1-ww-zzp)*(1-ww-zzp) : (ww+zzp)*(ww+zzp);
    }
    xfac[i] = TMath::Sqrt(xf);
  }
}

void TrAlignFit::SfFill(Int_t layer1, Int_t layer2, Float_t *zdef)
{
  Int_t niter = 20;
  Float_t zpos[Nplan], xsig[Nplan], xfac[Nplan];

  Int_t nhit = 0;
  for (Int_t i = 0, j = 0; i < Nplan; i++) {
    if (i+1 != layer1 && i+1 != layer2) {
      zpos[j++] = zdef[i];
      nhit++;
    }
  }

  for (Int_t i = 0; i < nhit; i++) xsig[i] = 1;

  for (Int_t iter = 0; iter < niter; iter++) {
    Sfactor(nhit, zpos, xsig, xfac);
    Float_t xss = 0;
    for (Int_t i = 0; i < nhit; i++) {
      xsig[i] /= xfac[i];
      xss += xsig[i]/nhit;
    }
    for (Int_t i = 0; i < nhit; i++) xsig[i] /= xss;
  }

  Int_t index = GetSfIndex(layer1, layer2);
  for (Int_t i = 0, j = 0; i < Nplan; i++)
    fSfactor[index][i] = (i+1 != layer1 && i+1 != layer2) ? xsig[j++] : 0;
}

void TrAlignFit::ProcHists(Int_t iter)
{
  Int_t bits0 = TrAlignObject::XYcorr2;
  Int_t bits1 = TrAlignObject::Zcorr2;
  Int_t bits2 = TrAlignObject::XYcorr1 | 
                TrAlignObject::Zcorr2  | TrAlignObject::dYXcorr;
  Int_t bits3 = TrAlignObject::XYcorr1 | 
                TrAlignObject::dYXcorr | TrAlignObject::dZXcorr;
  Int_t bits4 = TrAlignObject::XYcorr3;

  if (fNcases < 500000) bits3 = bits2;

  Int_t bits = 0;
  if      (iter == 0) bits = bits0;
  else if (iter == 1) bits = bits1;
  else if (iter == 2) bits = bits2;
  else if (iter < fFinIter) bits = bits3;
  else bits = bits4;

  TrAlignObject::fLcommon = 0;
  if (fMode%10 == kTestBeam) {
    if (iter == 0 || iter == 2 || iter == 4) {
      TrAlignObject::fLcommon = 1;
      bits = TrAlignObject::Zcorr2;
    }
    else bits = TrAlignObject::XYcorr3;
  }
  else if (TkDBc::Head->GetSetup() == 3) {
    if (iter == fMaxIter-1) TrAlignObject::fLcommon = 2;
  }

  TrAlignObject::ProfMode = 1;
  if (iter == fMaxIter-1) TrAlignObject::ProfMode = 2;

  for (Int_t i = 0; i < Nplan; i++) {
    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < 2; k++) {
	Int_t slot = j+1;
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+slot);
	TrAlignObject *algp = FindTkId(tkid);
	if (algp) algp->ProcHists(bits);
      }
    }
  }

  if (iter == 0 || iter == 1 || iter == fFinIter) {
    TDirectory *sdir = gDirectory;

    TString sfn = fNameOut.Data();
    sfn.ReplaceAll(".root", Form("_%02d.root", iter));

    TFile of(sfn, "recreate");
    if (of.IsOpen()) {
      for (Int_t i = 0; i < TrAlignObject::sharray.GetEntries(); i++)
	if (TrAlignObject::sharray.At(i))
	  TrAlignObject::sharray.At(i)->Write();
    }

    if (sdir) sdir->cd();
  }
}

void TrAlignFit::ParmHists(void)
{
  TDirectory *dir1 = (TDirectory *)fFile->Get(Form("iter%d", fFinIter-1));
  TDirectory *dir2 = (TDirectory *)fFile->Get(Form("iter%d", fMaxIter-1));
  if (!dir1 || !dir2) return;

  fFile->cd();

  TDirectory *dir = new TDirectoryFile("params", "Parameters");
  dir->cd();

  Int_t    nb = Nplan*32;
  Double_t bx = Nplan+0.5;

  TH1F *hist01 = new TH1F("hist01", "NevtX", nb, 0.5, bx);
  TH1F *hist02 = new TH1F("hist02", "NevtY", nb, 0.5, bx);
  TH1F *hist11 = new TH1F("hist11", "dX",    nb, 0.5, bx);
  TH1F *hist12 = new TH1F("hist12", "dY",    nb, 0.5, bx);
  TH1F *hist13 = new TH1F("hist13", "dZ",    nb, 0.5, bx);
  TH1F *hist14 = new TH1F("hist14", "dY/dX", nb, 0.5, bx);
  TH1F *hist15 = new TH1F("hist15", "dZ/dX", nb, 0.5, bx);
  TH1F *hist22 = new TH1F("hist22", "dYc",   nb, 0.5, bx);
  TH1F *hist23 = new TH1F("hist23", "dZc",   nb, 0.5, bx);

  TH1F *hist1 = (TH1F *)dir2->Get("hist1");
  TH1F *hist2 = (TH1F *)dir2->Get("hist2");
  TH1F *hist3 = (TH1F *)dir2->Get("hist3");
  TH1F *hist4 = (TH1F *)dir1->Get("hist4");
  TH1F *hist5 = (TH1F *)dir1->Get("hist5");
  TH3F *histx = (TH3F *)dir2->Get("histx");
  TH3F *histy = (TH3F *)dir2->Get("histy");

  Int_t nby = histx->GetNbinsY();
  Int_t nbz = histx->GetNbinsZ();

  for (Int_t i = 0; i < Nplan; i++) {
    for (Int_t j = 0; j < Nlad; j++) {
      for (Int_t k = 0; k < 2; k++) {
	Int_t sign = (k == 0) ? -1 : 1;
	Int_t slot = sign*(j+1);
	Int_t tkid = sign*((i+1)*100+j+1);
	TrAlignObject *algp = FindTkId(tkid);
	if (!algp) continue;

	Int_t id = i*32+slot+16-((tkid > 0) ? 1 : 0);
	Double_t pcy = algp->param[1]+algp->cool[0]*algp->phi;
	Double_t pcz = algp->param[2]+algp->cool[0]*algp->pitch;
	hist01->SetBinContent(id, histx->Integral(id, id, 1, nby, 1, nbz));
	hist02->SetBinContent(id, histy->Integral(id, id, 1, nby, 1, nbz));
	hist11->SetBinContent(id, algp->param[0]*10);
	hist12->SetBinContent(id, algp->param[1]*10);
	hist13->SetBinContent(id, algp->param[2]*10);
	hist14->SetBinContent(id, algp->phi  *600);
	hist15->SetBinContent(id, algp->pitch*600);
	hist22->SetBinContent(id, pcy*10);
	hist23->SetBinContent(id, pcz*10);

	hist11->SetBinError(id, hist1->GetBinError(id));
	hist12->SetBinError(id, hist2->GetBinError(id));
	hist13->SetBinError(id, hist3->GetBinError(id));
	hist14->SetBinError(id, hist4->GetBinError(id));
	hist15->SetBinError(id, hist5->GetBinError(id));
	hist22->SetBinError(id, hist2->GetBinError(id));
	hist23->SetBinError(id, hist3->GetBinError(id));
      }
    }
  }
}
