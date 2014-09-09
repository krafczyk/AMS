
#include "SpFold.h"
#include "TF1.h"
#include "TH1.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TVirtualFitter.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#define NTHR 8

#include <iostream>
using namespace std;

ClassImp(SplFit);
ClassImp(SpFold);
ClassImp(SpReso);

Int_t      SpFold::fModel = 0;
Int_t      SpFold::fNnode = 0;
Double_t  *SpFold::fXnode = 0;

Double_t (*SpFold::fRPDF)(Double_t*, Double_t*) = 0;
Int_t      SpFold::fRtype = SpFold::RG_RR;
Int_t      SpFold::fXtype = SpFold::RREC;

TGraph    *SpFold::fGacc  = 0;
Double_t  *SpFold::fPacc  = 0;
Int_t      SpFold::fNacc  = 0;

Int_t      SpFold::fNdiv  =    8;
Double_t   SpFold::fRmin  =  0.6;
Double_t   SpFold::fRmax  = 6000;
Int_t      SpFold::fVcont =   -1;
Int_t      SpFold::fCrChk =    1;
Bool_t     SpFold::fMT    =    0;

Double_t   SpFold::fSolMod[3] = { 0.6, 0.9383, 1 };


#include "Math/IFunction.h"
#include "Math/GaussIntegrator.h"

class ProdF : public ROOT::Math::IGenFunction {
public:
  Double_t  fRrec;
  Double_t *fPar;
  ProdF(Double_t rrec, Double_t *par) : fRrec(rrec), fPar(par) {}

  virtual ROOT::Math::IGenFunction* Clone() const { 
    return new ProdF(fRrec, fPar);
  }
  virtual Double_t DoEval(const Double_t rgen) const {
    return SpFold::Prod(rgen, fRrec, fPar);
  }
};

TF1 *SpFold::Fit(TH1 *hrt, TGraph *gac, 
		 Double_t (*rpdf)(Double_t*, Double_t*),
		 Option_t *opt, Double_t xmin, Double_t xmax,
		 Int_t model, Int_t nn, Double_t *xn)
{
  Double_t Xn[5] = { 1, 20, 50, 300, 1000 };
  if (nn == 0 || !xn) { nn = 5; xn = Xn; }

  SpFold::fGacc = gac;
  SpFold::fRPDF = rpdf;
  Bool_t invx = (fXtype == RINV) ? 1 : 0;

  TF1 *func = SpFold::GetF(model, HtoG(hrt, 1, invx), nn, xn);
  if (opt) {
    Norm(hrt);
    hrt->Fit(func, opt, "", xmin, xmax);
    Norm(hrt, 1);
  }

  return func;
}

Double_t SpFold::GetX(TF1 *fflx, Double_t bl, Double_t bu)
{
  Double_t bw = bu-bl;
  if (bw == 0) return bl;

  Double_t bi = fflx->Integral(bl, bu);
  Double_t x  = fflx->GetX(bi/bw, bl, bu);
  if (x <= bl || bu <= x) x = TMath::Sqrt(bl*bu);

  return x;
}

Double_t SpFold::PDF(Double_t *xp, Double_t *par)
{
  if (!fRPDF) return Flux(xp, par);

  Double_t rrec = 0;
  if (fXtype == RREC) rrec = xp[0];
  if (fXtype == RINV && xp[0] != 0) rrec = 1/xp[0];
  if (rrec == 0) return 0;

  Double_t l1 = TMath::Log10(fRmin);
  Double_t l2 = TMath::Log10(fRmax);
  Double_t ss = 0;

  if (fMT) {
    Flux(xp, par); fCrChk = 0;
    Int_t nd = fNdiv*20;
    Double_t *sw = new Double_t[nd];
#ifdef _OPENMP
#pragma omp parallel for num_threads(NTHR)
#endif
    for (Int_t i = 0; i < nd; i++) {
      Double_t r1 = TMath::Power(10, l1+(l2-l1)/nd* i   );
      Double_t r2 = TMath::Power(10, l1+(l2-l1)/nd*(i+1));
      Double_t rg = TMath::Sqrt(r1*r2);
      sw[i] = Prod(rg, rrec, par)*(r2-r1);
    }
    fCrChk = 1;
    for (Int_t i = 0; i < nd; i++) ss += sw[i];
    delete [] sw;
  }
  else {
    ProdF prod(rrec, par);
    ROOT::Math::GaussIntegrator gi;
    gi.SetFunction(prod);
    gi.SetRelTolerance(1e-4);

    Double_t r1 = fRmin;
    for (Int_t i = 0; i < fNdiv; i++) {
      Double_t r2 = TMath::Power(10, l1+(l2-l1)/fNdiv*(i+1));
      ss += gi.Integral(r1, r2);
      r1 = r2;
    }
  }

  return ss;
}

Double_t SpFold::Ratio(Double_t *xp, Double_t *par)
{
  Double_t acc = GetA(xp[0]);
  Double_t flx = Flux(xp, par);
  return (acc > 0 && flx > 0) ? PDF(xp, par)/flx/acc : 0;
}

Double_t SpFold::Prod(Double_t rgen, Double_t rrec, Double_t *par)
{
  if (rgen == 0 || rrec == 0 || !fRPDF) return 0;

  Double_t x = (fRtype == RG_RR) ? rgen/rrec : 1/rrec-1/rgen;
  Double_t c = (fRtype == RG_RR) ? 1/rrec/rrec*rgen : 1/rrec/rrec;
  Double_t p = (*fRPDF)(&x, &rgen)*c;
  return p*Flux(&rgen, par)*GetA(rgen);
}

Double_t SpFold::GetA(Double_t r)
{
  Double_t ac = 1;
  if (fPacc && fNacc > 0) {
    Double_t lr = TMath::Log10(r);
    ac = SplFit::Splint(fNacc, fPacc, &fPacc[fNacc],
			              &fPacc[fNacc*2], lr, SplFit::NORMAL);
    if (lr > fPacc[fNacc-1]) ac = fPacc[fNacc*2-1]+
			          fPacc[fNacc*3+1]*(lr-fPacc[fNacc-1]);
    if (lr < fPacc[0])       ac = fPacc[fNacc]+
			          fPacc[fNacc*3]  *(lr-fPacc[0]);
  }
  else ac = (fGacc) ? fGacc->Eval(r) : 1;
  if (ac <= 0) ac = 1;

  return ac;
}

TH1 *SpFold::GetCi(TF1 *func, const char *hname, Double_t cl, Double_t pwr,
		   Bool_t norm, Int_t n, Double_t xmin, Double_t xmax)
{
  Int_t svc = fVcont;
  fVcont = -1;

  TH1 *hci = SplFit::GetCi(func, hname, cl, norm, n, xmin, xmax);
  fVcont = svc;

  for (Int_t i = 0; i < hci->GetNbinsX(); i++) {
    Double_t bx = TMath::Sqrt(hci->GetBinLowEdge(i+1)*
			      hci->GetBinLowEdge(i+2));
    Double_t p = (pwr != 0) ? TMath::Power(bx, pwr) : 1;
    hci->SetBinContent(i+1, p*hci->GetBinContent(i+1));
    hci->SetBinError  (i+1, p*hci->GetBinError  (i+1));
  }

  return hci;
}

TF1 *SpFold::GetF(Int_t model, TGraph *gr, Int_t n, Double_t *xn)
{
  Double_t xx[5] = { 1, 10, 50, 200, 1000 };
  if (n <= 0 || !xn) { n = 5; xn = xx; }

  if (model > 10) {
    if (fXnode && fNnode != n) { delete [] fXnode; fXnode = 0; }
    if (!fXnode) fXnode = new Double_t[n];
    fNnode = n;
  }

  Int_t np = 0;
  if (model == PWR)   np = 2;
  if (model == PWRBR) np = 4;
  if (model == PWGRE) np = 5;
  if (model == PWSUM) np = 4;
  if (model == SPGRE) np = 8;
  if (model == SPLOG) np = n+2;
  if (model == SPINT) np = n+3;

  if (np == 0) return 0;
  fModel = model;

  TF1 *func = new TF1("func", SpFold::PDF, 1, 3000, np);

  if (model < 10) {
    Double_t x1 = 30, x2 = gr->GetX()[gr->GetN()-5];
    Double_t y1 = gr->Eval(x1);
    Double_t y2 = gr->Eval(x2);
    if (x2 <= 0 || y1 <= 0 || y2 <= 0) return func;

    Double_t p1 = TMath::Log(y2/y1)/TMath::Log(x2/x1);

    if (model == PWSUM) func->SetParameters(1, p1, 0, 0);
    else                func->SetParameters(1, p1, 200, 0.05, 1, -3, 3, 6);
    Double_t p0 = y1/func->Eval(x1);

    func->SetParameter(0, p0);
    if (model == PWRBR) func->SetParameters(p0, p1, 300, 0.05);
    if (model == PWGRE) func->SetParameters(p0, p1, 300, 0.05, 1);
    if (model == PWSUM) func->SetParameters(p0, p1, p0*0.1, p1+0.1);
    if (model == SPGRE) func->SetParameters(p0, p1, 200, 0.05, 1, -3, 3, 6);
    return func;
  }

  TGraph gg = *gr;
  if (fModel == SPINT) {
    TGraph g; gg = g;
    for (Int_t i = 0; i < gr->GetN()-1; i++)
      gg.SetPoint(i, gr->GetX()[i],
		     TMath::Log(gr->GetY()[i+1]/gr->GetY()[i])/
		     TMath::Log(gr->GetX()[i+1]/gr->GetX()[i]));
  }

  for (Int_t i = 0; i < n; i++) {
    fXnode[i] = xn[i];
    func->SetParameter(i, gg.Eval(xn[i])/GetA(xn[i]));
  }
  Double_t x1 = xn[0],   x2 = xn[0]  *1.1;
  Double_t x3 = xn[n-1], x4 = xn[n-1]*0.5;
  Double_t y1 = func->GetParameter(0),   y2 = gg.Eval(x2)/GetA(x2);
  Double_t y3 = func->GetParameter(n-1), y4 = gg.Eval(x4)/GetA(x4);
  Double_t u1 = x1, u2 = x2, u3 = x3, u4 = x4;

  if (x1 > 0 && x2 > 0 & x3 > 0 && x4 > 0) {
    u1 = TMath::Log10(x1); u2 = TMath::Log10(x2);
    u3 = TMath::Log10(x3); u4 = TMath::Log10(x4);
  }
  if (fModel == SPLOG && y1 > 0 && y2 > 0 && y3 > 0 && y4 > 0) {
    y1 = TMath::Log10(y1); y2 = TMath::Log10(y2);
    y3 = TMath::Log10(y3); y4 = TMath::Log10(y4);
  }
  func->SetParameter(n,   (y2-y1)/(u2-u1));
  func->SetParameter(n+1, (y4-y3)/(u4-u3));

  Int_t svx = fXtype;
  fXtype = RREC;

  if (fModel == SPINT) {
    func->SetParameter(n+2, 0);
    Double_t scl = gr->Eval(10)/func->Eval(10);
    if (scl > 0) func->SetParameter(n+2, TMath::Log10(scl));
  }

  if (fModel == SPLOG) {
    Double_t *pc = new Double_t[n];
    for (Int_t i = 0; i < n; i++) pc[i] = gr->Eval(xn[i])/func->Eval(xn[i]);
    for (Int_t i = 0; i < n; i++)
      func->SetParameter(i, func->GetParameter(i)*pc[i]);
    delete pc;

    Double_t c1 = TMath::Log10(func->Eval(x1)/GetA(x1));
    Double_t c2 = TMath::Log10(func->Eval(x2)/GetA(x2));
    Double_t c3 = TMath::Log10(func->Eval(x3)/GetA(x3));
    Double_t c4 = TMath::Log10(func->Eval(x4)/GetA(x4));
    func->SetParameter(n,   (c2-c1)/(u2-u1));
    func->SetParameter(n+1, (c4-c3)/(u4-u3));
  }

  fXtype = svx;

  return func;
}

TGraph *SpFold::Plot(TF1 *func, Double_t pwr, Int_t n, Double_t xmin,
		                                       Double_t xmax)
{
  TGraph *gr = new TGraph;

  Double_t l1 = TMath::Log10(xmin);
  Double_t l2 = TMath::Log10(xmax);

  for (Int_t i = 0; i <= n; i++) {
    Double_t x = TMath::Power(10, l1+(l2-l1)*i/n);
    Double_t p = (pwr != 0) ? TMath::Power(x, pwr) : 1;
    gr->SetPoint(i, x, func->Eval(x)*p);
  }
  gr->SetLineStyle(func->GetLineStyle());
  gr->SetLineWidth(func->GetLineWidth());
  gr->SetLineColor(func->GetLineColor());

  return gr;
}

TGraph *SpFold::HtoG(TH1 *hist, Bool_t norm, Bool_t invx)
{
  TGraph *g = new TGraph;
  Int_t np = 0;

  for (Int_t i = 0; i < hist->GetNbinsX(); i++) {
    Int_t j = i;
    if (invx) j = hist->GetNbinsX()-i-1;

    Double_t bw = (norm) ? hist->GetBinWidth(j+1) : 1;
    Double_t bx = hist->GetBinCenter (j+1);
    Double_t bc = hist->GetBinContent(j+1);
    if (bc == 0) continue;

    if (invx) {
      if (bx <= 0) continue;
      bx = 1/bx;
    }
    if (bw != 0) g->SetPoint(np++, bx, bc/bw);
  }
  return g;
}

void SpFold::Norm(TH1 *hist, Bool_t rev)
{
  for (Int_t i = 0; i < hist->GetNbinsX(); i++) {
    Double_t bw = hist->GetBinWidth(i+1);
    if (bw == 0) continue;

    if (rev) {
      hist->SetBinContent(i+1, hist->GetBinContent(i+1)*bw);
      hist->SetBinError  (i+1, hist->GetBinError  (i+1)*bw);
    }
    else {
      hist->SetBinContent(i+1, hist->GetBinContent(i+1)/bw);
      hist->SetBinError  (i+1, hist->GetBinError  (i+1)/bw);
    }
  }
}

void SpFold::Scale(TH1 *hist, Double_t pwr, TF1 *fflx)
{
  for (Int_t i = 0; i < hist->GetNbinsX(); i++) {
    Double_t bl = hist->GetBinLowEdge(i+1);
    Double_t bu = hist->GetBinLowEdge(i+2);
    Double_t bc = hist->GetBinContent(i+1);
    Double_t be = hist->GetBinError  (i+1);
    Double_t x  = GetX(fflx, bl, bu);
    Double_t p  = TMath::Power(x, pwr);
    hist->SetBinContent(i+1, bc*p);
    hist->SetBinError  (i+1, be*p);
  }
}

Double_t SpFold::Flux(Double_t *xp, Double_t *par)
{
  if (fModel == PWR)   return FluxPwr(xp, par);
  if (fModel == PWRBR) return FluxPbr(xp, par);
  if (fModel == PWGRE) return FluxGre(xp, par);
  if (fModel == PWSUM) return FluxSum(xp, par);
  if (fModel == SPGRE) return FluxSgr(xp, par);
  if (fModel == SPLOG) return FluxSpl(xp, par);
  if (fModel == SPINT) return FluxSpi(xp, par);

  return 0;
}

Double_t SpFold::FluxPwr(Double_t *xp, Double_t *par)
{
  // x     : Rigidity in GV
  // par[0]: norm
  // par[1]: gamma
  Double_t rm = xp[0];
  Double_t sm = 1;
  if (fSolMod[0] > 0) sm = SolMod(&rm, fSolMod);

  return par[0]*TMath::Power(rm, par[1])*sm;
}

Double_t SpFold::FluxPbr(Double_t *xp, Double_t *par)
{
  // x     : Rigidity in GV
  // par[0]: norm
  // par[1]: gamma
  // par[2]: break (GV)
  // par[3]: delta gamma

  Double_t x  = xp [0];
  Double_t xb = par[2];
  if (x < xb) return FluxPwr(&x, par);

  Double_t pp[2] = { 1, par[1]+par[3] };
  Double_t f0 = FluxPwr(&xb, par);
  Double_t f1 = FluxPwr(&xb, pp);

  return FluxPwr(&x, pp)*f0/f1;;
}

Double_t SpFold::FluxGre(Double_t *xp, Double_t *par)
{
  // x     : Rigidity in GV
  // par[0]: norm
  // par[1]: gamma
  // par[2]: break (GV)
  // par[3]: delta gamma
  // par[4]: smooth

  Double_t rm = xp [0];
  Double_t rb = par[2];
  Double_t dg = par[3];
  Double_t s  = par[4];
  Double_t sm = 1;
  Double_t sp = TMath::Power(1+TMath::Power(rm/rb, s), -dg/s);
  if (fSolMod[0] > 0) sm = SolMod(&rm, fSolMod);

  return par[0]*TMath::Power(rm/rb, par[1])/sp*sm;
}

Double_t SpFold::FluxSum(Double_t *xp, Double_t *par)
{
  // x     : Rigidity in GV
  // par[0]: norm
  // par[1]: gamma
  // par[2]: norm2
  // par[3]: gamma2

  Double_t x = xp[0];
  return FluxPwr(&x, par)+FluxPwr(&x, &par[2]);
}

Double_t SpFold::FluxSgr(Double_t *xp, Double_t *par)
{
  // x     : Rigidity in GV
  // par[0]: norm
  // par[1]: gamma
  // par[2]: break (GV)
  // par[3]: delta gamma
  // par[4]: smooth
  // par[5-7]: Spline Solar modulation: 

  Double_t rm = xp [0];
  Double_t rb = par[2];
  Double_t dg = par[3];
  Double_t s  = par[4];
  Double_t sm = 1;
  Double_t sp = TMath::Power(1+TMath::Power(rm/rb, s), -dg/s);

  if (rm > 0) {
    Double_t xn[2] = { 0, 1.6 };
    Double_t bb[2] = { 0, 0 };
    Double_t d2[2];
    SplFit::Spline(2, xn, &par[6], bb, d2);

    Double_t lr = TMath::Log10(rm);
    Double_t sp;
    if      (lr < 0)     sp = par[6];
    else if (lr > xn[1]) sp = par[7];
    else sp = SplFit::Splint(2, xn, &par[6], d2, lr, SplFit::NORMAL);

    sm = 1/(1+TMath::Exp(sp)*TMath::Power(rm, par[5]));
  }
  return par[0]*TMath::Power(rm/rb, par[1])/sp*sm;
}

Double_t SpFold::FluxSpl(Double_t *xp, Double_t *par)
{
  enum { Nmax = SplFit::Nmax };

  static Double_t ps[Nmax+2] = { 0, 0 };
  static Double_t xn[Nmax];
  static Double_t yn[Nmax];
  static Double_t d2[Nmax];

  Int_t nn = fNnode;
  Int_t np = nn+2;

  if (fCrChk) {
#ifdef _OPENMP
#pragma omp critical (d2chk)
#endif
 {
  Int_t chk = 1;
  for (Int_t i = 0; i < np; i++) if (ps[i] != par[i]) { chk = 0; break; }

  if (!chk) {
    for (Int_t i = 0; i < np; i++) ps[i] = par[i];
    for (Int_t i = 0; i < nn; i++) xn[i] = TMath::Log10(TMath::Abs(fXnode[i]));
    for (Int_t i = 0; i < nn; i++) yn[i] = TMath::Log10(TMath::Abs(par[i]));
    SplFit::Spline(nn, xn, yn, &par[nn], d2);

    if (fVcont >= 0) {
      fVcont++;
      cout << Form("%4d", fVcont) << "\033[4D" << flush;
    }
  }
 }}

  Double_t x = TMath::Log10(TMath::Abs(xp[0]));
  Double_t y = SplFit::Splint(nn, xn, yn, d2, x, SplFit::NORMAL);
  return TMath::Power(10, y);
}

Double_t SpFold::FluxSpi(Double_t *xp, Double_t *par)
{
  enum { Nmax = SplFit::Nmax };

  static Double_t ps[Nmax+2] = { 0, 0 };
  static Double_t xn[Nmax];
  static Double_t yn[Nmax];
  static Double_t d2[Nmax];

  Int_t nn = fNnode;
  Int_t np = nn+2;

  Int_t chk = 1;
  for (Int_t i = 0; i < np; i++)
    if (ps[i] != par[i]) { chk = 0; break; }

  if (!chk) {
    for (Int_t i = 0; i < np; i++) ps[i] = par[i];
    for (Int_t i = 0; i < nn; i++) xn[i] = TMath::Log10(TMath::Abs(fXnode[i]));
    for (Int_t i = 0; i < nn; i++) yn[i] = par[i];
    SplFit::Spline(nn, xn, yn, &par[nn], d2);

    if (fVcont >= 0) {
      fVcont++;
      cout << Form("%4d", fVcont) << "\033[4D" << flush;
    }
  }

  Double_t x = TMath::Log10(TMath::Abs(xp[0]));
  Double_t y = SplFit::Splint(nn, xn, yn, d2, x, SplFit::INTEG);

  if (x>xn[nn-1])
    y = SplFit::Splint(nn, xn, yn, d2, xn[nn-1], SplFit::INTEG)
      +yn[nn-1]*(x-xn[nn-1])
      +(x-xn[nn-1])*(x-xn[nn-1])*par[nn+1]/2;

  return TMath::Power(10, y+par[np]);
}

Double_t SpFold::SolMod(Double_t *xp, Double_t *par)
{
  Double_t rgt  = xp [0];
  Double_t phi  = par[0];
  Double_t mass = par[1];
  Double_t chrg = par[2];

  Double_t ek   = TMath::Sqrt(chrg*chrg*rgt*rgt+mass*mass)-mass;
  Double_t ekm  = ek+chrg*phi;
  Double_t etm  = ekm+mass;
  Double_t beta = TMath::Sqrt(1-mass*mass/etm/etm);

  rgt = beta*etm/chrg;
  return (ek*ek+2*ek*mass)/(ekm*ekm+2*ekm*mass);
}


Double_t SpReso::fPref[2] = { 5e-4, 0.1 };
Double_t SpReso::fNorm = 1/4e-2;

Double_t SpReso::fSp0[Np*2+2] = { -0.1, 0.1, 0.5, 1.0, 2.0, 2.9, 3.6,
				    0.0117, 0.0137, 0.0172, 0.0170,
				    0.0124, 0.0146, 0.0151, 0.020, 0 };
Double_t SpReso::fSp1[Np*2+2] = { -0.219, -4.36, 0.939 };
Double_t SpReso::fSp2[Np*2+2] = { -0.1,    1.0,   2.0,   2.9,   3.6,    0, 0,
				    1.31,   0.870, 1.13,  0.829, 0.884,  0, 0,
				   -1.25,   0 };
Double_t SpReso::fSp3[Np*2+2] = { -0.1,    1.0,   2.0,   2.9,   3.6,    0, 0,
				    0.0884, 0.200, 0.253, 0.362, 0.284,  0, 0,
				    0.29,  -0.19 };
Double_t SpReso::fSp4[Np*2+2] = { -0.1,    1.0,   2.0,   2.9,   3.6,    0, 0,
				    2.1,    1.89,  1.97,  1.78,  1.91,   0, 0,
				    0.0,    0.46 };
Double_t SpReso::fSp5[Np*2+2] = {  2.4,    2.7,   3.0,   3.4,   3.7,    0, 0,
				    0.00152, -0.000354, -0.00525,
				    0.00391, -0.0362,    0, 0,
				    0.04, -0.26 };
Double_t SpReso::fSp6[Np*2+2] = { -0.2,    0.0,   2.2,   2.4,   3.7,    0, 0,
				    2.9,    3.56,  9.45,  11.4,  6.84,
				    6.1,   -4.3 };

Double_t SpReso::PDFvR(Double_t *xp, Double_t *par)
{
  // x      : Rgen/Rrec
  // par[0] : Rgen
  Double_t rg = par[0];
  return (rg != 0) ? GetPDF(rg, (xp[0]-1)/rg)*GetNorm(rg, 1) : 0;
}

Double_t SpReso::PDFdR(Double_t *xp, Double_t *par)
{
  // x      : 1/Rrec-1/Rgen
  // par[0] : Rgen
  return GetPDF(par[0], xp[0])*GetNorm(par[0], 2);
}

Double_t SpReso::PDFpar(Double_t *xp, Double_t *par)
{
  // This should be applied after dividing x by
  //  rr = TMath::Sqrt(fPref[0]*fPref[0]+fPref[1]*fPref[1]/rgen/rgen);
  //
  // x      : 1/Rrec-1/Rgen
  // par[0] : Norm
  // par[1] : Mean
  // par[2] : Sig0
  // par[3] : Sig1
  // par[4] : Sig2

  Double_t x  =  xp[0];
  Double_t s2 = par[2]*par[2]+
                par[3]*par[3]*TMath::Power(TMath::Abs(x), par[4]);

  return par[0]*TMath::Exp(-0.5*(x-par[1])*(x-par[1])/s2);
}

Double_t SpReso::PARrg(Double_t *xp, Double_t *par)
{
  // x      : Rgen
  // par[0] : Index
  return GetPar(xp[0], (Int_t)par[0]);
}

Double_t *SpReso::GetSp(Int_t i, Int_t type)
{
  Double_t *sp = 0;
  if (i == 0) sp = fSp0;
  if (i == 2) sp = fSp2;
  if (i == 3) sp = fSp3;
  if (i == 4) sp = fSp4;
  if (i == 5) sp = fSp5;
  if (i == 6) sp = fSp6;

  if (sp) {
    if (type == 0) return  sp;
    if (type == 1) return &sp[Np];
    if (type == 2) return &sp[Np*2];
  }
  return 0;
}

Double_t SpReso::GetNorm(Double_t rgen, Int_t type)
{
  Double_t rr = TMath::Sqrt(fPref[0]*fPref[0]+fPref[1]*fPref[1]/rgen/rgen);
  if (rr > 0) {
    if (type == 1) return 1/rr/rgen;
    if (type == 2) return 1/rr;
  }
  return 1;
}

Double_t SpReso::GetPDF(Double_t rgen, Double_t x)
{
  if (rgen == 0) return 0;

  Double_t p0 = GetPar(rgen, 0);
  Double_t p1 = GetPar(rgen, 1);

  Double_t rr = TMath::Sqrt(fPref[0]*fPref[0]+fPref[1]*fPref[1]/rgen/rgen);
  if (rr > 0) x /= rr;

  if (rgen > 220) p0 /= 1+GetPar(rgen, 5);
  p0 *= fNorm;

  Double_t ee = -0.5*(x-p1)*(x-p1)/GetSig(rgen, x-p1);
  return (ee > -20) ? p0*TMath::Exp(ee) : 0;
}

Double_t SpReso::GetSig(Double_t rgen, Double_t x)
{
  Double_t p[4];
  for (Int_t i = 0; i < 3; i++) p[i] = GetPar(rgen, i+2);

  p[3] = GetPar(rgen, 6);

  Double_t bx = TMath::Power(TMath::Abs(rgen), 0.4)+4;
  x = TMath::Abs(Satur(TMath::Abs(x), p[3]+bx, 5));

  Double_t sig = p[0]*p[0]+p[1]*p[1]*TMath::Power(x, p[2]);
  return sig;
}

Double_t SpReso::Satur(Double_t x, Double_t s0, Double_t s1)
{
  Double_t y = (x-s0)/s1;
  Double_t e = TMath::Exp(y);

  return s0+(1-(e+1/e)/(e-1/e))*y/2*s1;
}

Double_t SpReso::GetPar(Double_t rgen, Int_t i)
{
  static Double_t *D2 = 0;
  if (!D2) {
#ifdef _OPENMP
#pragma omp critical (d2chk)
#endif
   if (!D2) {
    D2 = new Double_t[Np*6];
    SplFit::Spline(7, fSp0, &fSp0[Np], &fSp0[Np*2], &D2[ 0]);
    SplFit::Spline(5, fSp2, &fSp2[Np], &fSp2[Np*2], &D2[Np]);
    SplFit::Spline(5, fSp3, &fSp3[Np], &fSp3[Np*2], &D2[Np*2]);
    SplFit::Spline(5, fSp4, &fSp4[Np], &fSp4[Np*2], &D2[Np*3]);
    SplFit::Spline(5, fSp5, &fSp5[Np], &fSp5[Np*2], &D2[Np*4]);
    SplFit::Spline(5, fSp6, &fSp6[Np], &fSp6[Np*2], &D2[Np*5]);
   }
  }

  if (rgen == 0) return 0;

  Double_t x = TMath::Log10(TMath::Abs(rgen));
  if (i == 1) return TMath::Exp(fSp1[0]+fSp1[1]*x+fSp1[2]*x*x);

  Int_t     np = (i == 0) ?  7 : 5;
  Double_t *d2 = (i == 0) ? D2 : &D2[Np*(i-1)];
  Double_t *sp = GetSp(i, 0);
  if (!sp) return 0;

  return SplFit::Splint(np, sp, &sp[Np], d2, x, SplFit::NORMAL);
}

void SpReso::REtest(Double_t x)
{
  Double_t r[NTHR], p[NTHR];
  for (Int_t i = 0; i < NTHR; i++) {
    r[i] = TMath::Power(10, (i+0.5)*4/NTHR);
    p[i] = 0;
  }

#ifdef _OPENMP
#pragma omp parallel for num_threads(NTHR)
  for (Int_t i = 0; i < NTHR; i++)
    p[i] = PDFvR(&x, &r[i]);
#endif

  for (Int_t i = 0; i < NTHR; i++)
    cout << r[i] << " " << p[i] << " " << p[i]-PDFvR(&x, &r[i]) << endl;
}


Int_t  SplFit::fN     = 0;
Int_t  SplFit::fMode  = SplFit::NORMAL;
Int_t  SplFit::fXfree = 0;
Bool_t SplFit::fLogX  = 0;
Bool_t SplFit::fLogY  = 0;
Bool_t SplFit::fBlxL  = 0;
Bool_t SplFit::fBlxU  = 0;

TF1 *SplFit::Fit(TGraph *gr, Int_t n, Double_t *x, Double_t *b,
		 Option_t *opt, Double_t xmin, Double_t xmax)
{
  TF1 *func = GetF(gr, n, x, b, opt, xmin, xmax);
  if (!func) return 0;

  gr->Fit(func, opt, "", xmin, xmax);
  return func;
}

TF1 *SplFit::Fit(TH1 *hist, Int_t n, Double_t *x, Double_t *b,
		 Option_t *opt, Double_t xmin, Double_t xmax)
{
  TGraphErrors *gr = new TGraphErrors;
  for (Int_t i = 0; i < hist->GetNbinsX(); i++) {
    gr->SetPoint(i, hist->GetBinCenter(i+1), hist->GetBinContent(i+1));
    gr->SetPointError(i, 0, hist->GetBinError(i+1));
  }

  TF1 *func = GetF(gr, n, x, b, opt, xmin, xmax);
  if (!func) return 0;
  delete gr;

  hist->Fit(func, opt, "", xmin, xmax);
  return func;
}

TF1 *SplFit::GetF(TGraph *gr, Int_t n, Double_t *x, Double_t *b,
		  Option_t *opt, Double_t xmin, Double_t xmax)
{
  if (fMode == INTEG && (!fLogX || !fLogY)) {
    cerr << "SplFit::Fit-E-INTEG mode is supported only with LogXY" << endl;
    return 0;
  }

  fN = n;
  Int_t npar = n*2+2;
  if (fMode == INTEG) npar++;

  Double_t xmn = x[0];
  Double_t xmx = x[n-1];
  if (xmin < xmax) { xmn = xmin; xmx = xmax; }

  TF1 *func = new TF1("spfun", SplFit::SpFunc, xmn, xmx, npar);

  for (Int_t i = 0; i < n; i++)
    if (fXfree & (1<<i)) func->SetParameter(i, x[i]);
    else                 func->FixParameter(i, x[i]);

  for (Int_t i = 0; i < n; i++) {
    if (fMode == INTEG && fLogX && fLogY) {
      Double_t x1 = x[i]*0.9;
      Double_t x2 = x[i]*1.1;
      Double_t y1 = gr->Eval(x1);
      Double_t y2 = gr->Eval(x2);
      Double_t dl = (TMath::Log10(y2/y1))/(TMath::Log10(x2/x1));
      func->SetParameter(i+n, TMath::Power(10, dl));
    }
    else func->SetParameter(i+n, gr->Eval(x[i]));
  }
  if (fLogY) {
    for (Int_t i = 0; i < n; i++)
      func->SetParameter(i+n, TMath::Log10(func->GetParameter(i+n)));
  }

  Int_t ib1 = n*2;
  Int_t ib2 = n*2+1;
  func->SetParameter(ib1, (b) ? b[0] : 0);
  func->SetParameter(ib2, (b) ? b[1] : 0);

  TString sopt = opt; sopt.ToLower();
  if (b) {
   if (sopt.Contains("bfl")) func->FixParameter(ib1, func->GetParameter(ib1));
   if (sopt.Contains("bfu")) func->FixParameter(ib2, func->GetParameter(ib2));
  }
  sopt.ReplaceAll("bfl", "");
  sopt.ReplaceAll("bfu", "");

  if (fMode == INTEG && fLogX && fLogY) {
    func->SetParameter(npar-1, 0);
    func->SetParameter(npar-1, TMath::Log10(gr->Eval(x[n/2])
					 /func->Eval(x[n/2])));
  }

  return func;
}

TH1 *SplFit::GetCi(TF1 *func, const char *hname, Double_t cl, Bool_t norm,
		   Int_t n, Double_t xmin, Double_t xmax)
{
  Double_t *bn = new Double_t[n+1];
  Double_t *bx = new Double_t[n];
  Double_t *ci = new Double_t[n];
  Double_t  l1 = TMath::Log10(xmin);
  Double_t  l2 = TMath::Log10(xmax);

  for (Int_t i = 0; i <= n; i++) {
    bn[i] = TMath::Power(10, l1+(l2-l1)*i/n);
    if (i > 0) bx[i-1] = TMath::Sqrt(bn[i]*bn[i-1]);
  }

  Double_t cdf = (norm) ? 1 : TMath::Sqrt(func->GetChisquare()/func->GetNDF());
  (TVirtualFitter::GetFitter())
    ->GetConfidenceIntervals(n, 1, bx, ci, cl);

  TH1F *hci = new TH1F(hname, "Confidence interval", n, bn);
  for (Int_t i = 0; i < n; i++) {
    hci->SetBinContent(i+1, func->Eval(bx[i]));
    hci->SetBinError  (i+1, ci[i]/cdf);
  }

  delete [] bn;
  delete [] bx;
  delete [] ci;

  return hci;
}

TGraph *SplFit::Plot(TF1 *func, Int_t n, Double_t xmin, Double_t xmax)
{
  TGraph *gr = new TGraph;

  Double_t l1 = TMath::Log10(xmin);
  Double_t l2 = TMath::Log10(xmax);

  for (Int_t i = 0; i <= n; i++) {
    Double_t x = TMath::Power(10, l1+(l2-l1)*i/n);
    gr->SetPoint(i, x, func->Eval(x));
  }
  gr->SetLineStyle(func->GetLineStyle());
  gr->SetLineWidth(func->GetLineWidth());
  gr->SetLineColor(func->GetLineColor());

  return gr;
}

Double_t SplFit::SpFunc(Double_t *xp, Double_t *par)
{
  // Spline function
  // par[ 0:fN-1]   : Nodes x
  // par[fN:fN*2-1] : Nodes y
  // par[fN*2]      : Derivative (dy/dx) of lower boundary
  // par[fN*2+1]    : Derivative (dy/dx) of upper boundary
  // par[fN*2+2]    : (only for fMode==INTEG) Constant offset at xn[0]

  static Double_t ps[Nmax*2+2] = { 0, 0 };
  static Double_t xn[Nmax];
  static Double_t d2[Nmax];

  Int_t chk = 1;
  for (Int_t i = 0; i < fN*2+2; i++)
    if (ps[i] != par[i]) { chk = 0; break; }

  if (!chk) {
    for (Int_t i = 0; i < fN*2+2; i++) ps[i] = par[i];
    for (Int_t i = 0; i < fN;     i++) 
      xn[i] = (fLogX && par[i] > 0) ? TMath::Log10(par[i]) : par[i];

    Spline(fN, xn, &par[fN], &par[fN*2], d2);
  }

  Double_t x = xp[0];
  if (fLogX && x > 0) x = TMath::Log10(x);

  Double_t y = Splint(fN, xn, &par[fN], d2, x, fMode);

  if (fBlxL && x < xn[0]) {
    Double_t dx = x-xn[0];
    if (fMode == NORMAL) y = par[fN]   +par[fN*2]*dx;
    if (fMode == INTEG)  y = par[fN]*dx+par[fN*2]*dx*dx/2
			    +Splint(fN, xn, &par[fN], d2, xn[0], fMode);
  }
  if (fBlxU && x > xn[fN-1]) {
    Double_t dx = x-xn[fN-1];
    if (fMode == NORMAL) y = par[fN*2-1]   +par[fN*2+1]*dx;
    if (fMode == INTEG)  y = par[fN*2-1]*dx+par[fN*2+1]*dx*dx/2
			    +Splint(fN, xn, &par[fN], d2, xn[fN-1], fMode);
  }

  if (fMode == INTEG) y += par[fN*2+2];
  return (fLogY) ? TMath::Power(10, y) : y;
}

Double_t SplFit::Splint(Int_t n, Double_t *xn,
		                Double_t *yn, Double_t *d, Double_t x,
		       Int_t mode)
{
  Int_t i1 = 0;
  Int_t i2 = n-1;
  while (i2-i1 > 1) {
    Int_t i = (i2+i1)/2;
    if (xn[i] > x) i2 = i;
    else i1 = i;
  }

  Double_t h = xn[i2]-xn[i1];
  if (h == 0) return 0;

  Double_t a  = (xn[i2]-x)/h; 
  Double_t b  = (x-xn[i1])/h;
  Double_t a3 = a*a*a;
  Double_t b3 = b*b*b;
  Double_t c  = 0;

  // 1st. derivative
  if (mode == DERIV1) {
    a3 = -3*a*a/h; a = -1/h;
    b3 =  3*b*b/h; b =  1/h;
  }

  // 2nd. derivative
  if (mode == DERIV2) return a*d[i1]+b*d[i2];

  // Integral from xn[0]
  if (mode == INTEG) {
    Double_t h0 = xn[1]-xn[0];
    c = -(-h0*yn[0]/2+(h0*h0*h0)*d[0]/24);

    for (Int_t i = 1; i <= i1; i++) {
      Double_t h1 = xn[i]-xn[i-1];
      Double_t h2 = xn[i+1]-xn[i];
      c += (h1+h2)*yn[i]/2-(h1*h1*h1+h2*h2*h2)*d[i]/24;
    }
    a3 = -h/4*a*a*a*a; a = -h/2*a*a;
    b3 =  h/4*b*b*b*b; b =  h/2*b*b;
  }
  return a*yn[i1]+b*yn[i2]+((a3-a)*d[i1]+(b3-b)*d[i2])*(h*h)/6+c;
}

void SplFit::Spline(Int_t n, Double_t *x,
		            Double_t *y, Double_t *b, Double_t *d)
{
  Double_t u[Nmax];
  d[0] = u[0] = 0; 

  if (b) {
    d[0] = -0.5;
    u[0] = (3/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-b[0]);
  }

  for (Int_t i = 1; i < n-1; i++) {
    Double_t s = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
    Double_t p = s*d[i-1]+2;
    d[i] = (s-1)/p;
    u[i] = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
    u[i] = (6*u[i]/(x[i+1]-x[i-1])-s*u[i-1])/p;
  }

  Double_t q = 0, v = 0;
  if (b) {
    q = 0.5;
    v = (3/(x[n-1]-x[n-2]))*(b[1]-(y[n-1]-y[n-2])/(x[n-1]-x[n-2]));
  }
  d[n-1] = (v-q*u[n-2])/(q*d[n-2]+1);

  for (Int_t i = n-2; i >= 0; i--) d[i] = d[i]*d[i+1]+u[i];
}
