
#include "SpFold.h"
#include "TF1.h"
#include "TH1.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TVirtualFitter.h"
#include "typedefs.h"

#ifdef _OPENMP
#include <omp.h>
#endif
#define NTHR 8

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
Double_t   SpFold::fRmax  = 30e3;
Int_t      SpFold::fVcont =   -1;
Int_t      SpFold::fCrChk =    1;
Bool_t     SpFold::fMT    =    1;

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
  Double_t Xn[9] = { 1.2, 2, 5, 8, 15, 30, 50, 300, 1000 };
  if (nn == 0 || !xn) { nn = 9; xn = Xn; }

  SpFold::fGacc = gac;
  SpFold::fRPDF = rpdf;
  Bool_t invx = (fXtype == RINV) ? 1 : 0;

  TGraph *gr = HtoG(hrt, 0, invx);
  TF1  *func = SpFold::GetF(model, gr, nn, xn);
  if (!opt) return func;

  TString sopt = opt; sopt.ToLower();
  if (sopt.Contains("i")) hrt->Fit(func, opt, "", xmin, xmax);
  else                    gr ->Fit(func, opt, "", xmin, xmax);

  return func;
}

Double_t SpFold::GetX(TF1 *fflx, Double_t bl, Double_t bu)
{
  Double_t bw = bu-bl;
  if (bw == 0) return bl;

  Double_t bi = fflx->Integral(bl, bu);
  Double_t x  = fflx->GetX(bi/bw, bl, bu);
  if (x <= bl || bu <= x) {
    x = TMath::Sqrt(bl*bu);
    if (bu < 0) x = -1;
  }

  return x;
}

Double_t SpFold::PDF(Double_t *xp, Double_t *par)
{
  if (!fRPDF) return Flux(xp, par);

  Double_t rrec = 0;
  if (fXtype == RREC) rrec =  xp[0];
  if (fXtype == RINV) rrec = (xp[0] != 0) ? 1/xp[0] : 1e6;
  if (rrec == 0) return 0;

  Double_t l1 = TMath::Log10(fRmin);
  Double_t l2 = TMath::Log10(fRmax);
  Double_t ss = 0;

  if (fMT) {
    Prod(fRmin, rrec, par);
    fCrChk = 0;
    Int_t nd = fNdiv*20;
    Double_t *sw = new Double_t[nd];
#ifdef _OPENMP
#pragma omp parallel for num_threads(NTHR)
#endif
    for (Int_t i = 0; i < nd; i++) {
      Double_t r1 = TMath::Power(10, l1+(l2-l1)/nd* i   );
      Double_t r2 = TMath::Power(10, l1+(l2-l1)/nd*(i+1));
      Double_t rg = TMath::Sqrt(r1*r2);
      Double_t dr = (r2-r1)*((fXtype == RINV) ? 1 : 1/rrec/rrec);
      sw[i] = Prod(rg, rrec, par)*dr;
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
  Double_t c = (fRtype == RG_RR) ? rgen      : 1;
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
  Double_t Xn[9] = { 1.2, 2, 5, 8, 15, 30, 50, 300, 1000 };
  if (n <= 0 || !xn) { n = 9; xn = Xn; }

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
  Int_t  vc = fVcont; fVcont = -1;

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

  if (x1 > 0 && x2 > 0 && x3 > 0 && x4 > 0) {
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
    TGraphErrors gf;
    for (Int_t i = 0; i < gr->GetN(); i++) {
      gf.SetPoint(i, gr->GetX()[i], gr->GetY()[i]/GetA(gr->GetX()[i]));
      gf.SetPointError(i, 0, gf.GetY()[i]*0.01);
    }

    TF1 ff("ff", SpFold::Flux, 1, 3000, np);
    ff.SetParameters(func->GetParameters());

    gf.Fit(&ff, "q0");
    func->SetParameters(ff.GetParameters());

    Double_t *pc = new Double_t[n];
    for (Int_t iter = 0; iter <= 3; iter++) {
      for (Int_t i = 0; i < n; i++) pc[i] = gr->Eval(xn[i])/func->Eval(xn[i]);
      for (Int_t i = 0; i < n; i++)
	func->SetParameter(i, func->GetParameter(i)*pc[i]);
    }
    delete pc;
  }

  fXtype = svx;
  fVcont = vc;

  return func;
}

TGraph *SpFold::Plot(TF1 *func, Double_t pwr, Int_t n, Double_t xmin,
		                                       Double_t xmax)
{
  TGraph *gr = new TGraph;

  Int_t  logx = 0;
  Double_t x1 = xmin;
  Double_t x2 = xmax;

  if (xmin > 0 && xmax > 0) {
    x1 = TMath::Log10(xmin);
    x2 = TMath::Log10(xmax);
    logx = 1;
  }

  for (Int_t i = 0; i <= n; i++) {
    Double_t x = (logx) ? TMath::Power(10, x1+(x2-x1)*i/n)
                                         : x1+(x2-x1)*i/n;
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
  TGraphErrors *g = new TGraphErrors;
  Int_t np = 0;

  TF1 fpw("fpw", "x^[0]");
  fpw.SetParameter(0, -2.7);

  Double_t emin = 0.002;

  for (Int_t i = 0; i < hist->GetNbinsX(); i++) {
    Int_t j = i;
    if (invx) j = hist->GetNbinsX()-i-1;

    Double_t bw = (norm) ? hist->GetBinWidth(j+1) : 1;
    Double_t bx = GetX(&fpw, hist->GetBinLowEdge(j+1),
		             hist->GetBinLowEdge(j+2));
    Double_t bc = hist->GetBinContent(j+1);
    Double_t be = hist->GetBinError  (j+1);
    if (bc == 0) continue;
    if (be < bc*emin) be = bc*emin;

    if (invx) {
      if (bx <= 0) continue;
      bx = 1/bx;
    }
    if (bw != 0) {
      g->SetPoint(np, bx, bc/bw);
      g->SetPointError(np, 0, be/bw);
      np++;
    }
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
  // Broken power law (aka Grenoble function, parameterization by MH)
  // x     : Rigidity in GV
  // par[0]: gamma
  // par[1]: delta gamma
  // par[2]: smooth
  // par[3]: 1/break (1/GV)
  // par[4]: norm

  Double_t phi = fSolMod[0];
  Double_t r   = xp[0];
  Double_t rf  = r+phi;
  Double_t rn  = rf*par[3];
  return par[4]*(r/rf)*(r/rf)*TMath::Power(rn, par[0])
              *TMath::Power(1+TMath::Power(rn, par[1]/par[2]), par[2]);
/*
  Double_t rm = xp [0];
  Double_t gm = par[1];
  Double_t rb = 1/par[2];
  Double_t dg = par[3];
  Double_t s  = par[4];
  Double_t sm = 1;
  if (fSolMod[0] > 0) sm = SolMod(&rm, fSolMod);

  return par[0]*TMath::Power(rm/rb, gm)*
 TMath::Power(1+TMath::Power(rm/rb, dg/s), s)/TMath::Power(2, s);
*/
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
      cout << Form("%5d", fVcont) << "\033[5D" << flush;
    }
  }
 }}

  Double_t x = TMath::Log10(TMath::Abs(xp[0]));
  Double_t y = SplFit::Splint(nn, xn, yn, d2, x, SplFit::NORMAL);
//if (x > xn[nn-1]) y = yn[nn-1]+par[nn+1]*(x-xn[nn-1]);
//else y = SplFit::Splint(nn, xn, yn, d2, x, SplFit::NORMAL);
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

Double_t SpReso::fXn[SpReso::Nx] = { -10, -6, -4, -2, 2, 4, 6, 15 };
Double_t SpReso::fRn[SpReso::Nr] = { 2, 13, 24, 42, 100, 500, 4000 };

// Default parameters with R<400 GV MCscat and MC cutoff
Double_t SpReso::fSp[SpReso::Nt] = 
 {  0.0159, 0.0171, 0.0152, 0.0143, 0.0122, 0.0139, 0.0155, 0.0073, 0.0000, 
   -0.013, -0.038, -0.064, -0.031,  0.014, -0.006,  0.033, -0.236,  0.026, 
    1.535,  1.237,  3.702,  4.131,  7.186,  8.857,  7.585,  1.778, -0.156, 
    1.477,  1.632,  2.194,  2.507,  3.473,  3.789,  3.222,  0.491, -0.033, 
    1.297,  1.260,  1.493,  1.646,  2.033,  2.121,  2.004, -0.612, -0.071, 
    1.029,  0.907,  1.076,  1.197,  1.575,  1.224,  1.076, -0.919, -0.035, 
    1.028,  0.920,  1.125,  1.256,  1.589,  1.228,  1.073, -0.857, -0.056, 
    1.491,  1.421,  1.598,  1.801,  2.100,  2.124,  1.963, -0.174, -0.145, 
    2.665,  2.540,  2.611,  2.808,  3.534,  3.810,  3.221,  1.390, -0.150, 
   13.654, 12.175, 12.001, 11.659, 14.657, 16.568, 15.447,  8.691, -0.661, 
   -0.700, -0.700, -0.705, -0.728, -1.019, -1.502, -1.517,  0.000,  0.045, 
    1.602,  1.635,  1.546,  1.480,  1.518,  1.761,  1.756,  0.011, -0.001 };

static const Double_t Rmin = 0.8;

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
  /*  PDF of xp[0]= 1/Rrec-1/Rgen with par[]= Norm,Mean,Sig_i
   * \param[in] xp [0]     1/Rgen-1/Rrec
   * \param[in] par[0]     Norm
   * \param[in] par[1]     Mean
   * \param[in] par[2-9]   Sigma^2 spline position in Y
   * \param[in] par[10,11] Sigma^2 spline dY/dX at lower/upper boundary
   * \param[in] par[12]    x scaling
   * \return    PDF or sigma^2 (if par[0]=0)
   */

  Int_t  n = Nx-1;
  Int_t i0 = 2;
  Int_t in = 2+Nx-1;
  Int_t is = in+3;

  Double_t x = xp[0];
  if (par[is] != 0) x *= par[is];

  x -= par[1];
  if (x == 0) x = 1e-9;

  Double_t d2[Nx];
  SplFit::Spline(Nx, fXn, &par[i0], &par[in+1], d2);

  Double_t sig2 = (x < fXn[0]) ? par[i0]+par[in+1]*(x-fXn[0])
               : ((x > fXn[n]) ? par[in]+par[in+2]*(x-fXn[n])
	       : SplFit::Splint(Nx, fXn, &par[i0], d2, x, SplFit::NORMAL));

  if (par[0] == 0) return (par[is] != 0) ? sig2/par[is] : sig2;

  return par[0]*TMath::Exp(-0.5*x*x/sig2);
}

Double_t SpReso::PARrg(Double_t *xp, Double_t *par)
{
  // x      : Rgen
  // par[0] : Index
  return GetPar(xp[0], (Int_t)par[0]);
}

Double_t SpReso::GetNorm(Double_t rgen, Int_t type)
{
  if (TMath::Abs(rgen) < Rmin) rgen = Rmin;
  Double_t rr = TMath::Sqrt(fPref[0]*fPref[0]+fPref[1]*fPref[1]/rgen/rgen);
  if (rr > 0) {
    if (type == 1) return 1/rr/rgen;
    if (type == 2) return 1/rr;
  }
  return 1;
}

Double_t SpReso::GetPDF(Double_t rgen, Double_t x)
{
  if (TMath::Abs(rgen) < Rmin) return 0;

  Double_t p0 = GetPar(rgen, 0)*fNorm;
  Double_t p1 = GetPar(rgen, 1);

  Double_t rr = TMath::Sqrt(fPref[0]*fPref[0]+fPref[1]*fPref[1]/rgen/rgen);
  if (rr > 0) x /= rr;

  Double_t sig2 = GetSig(rgen, x-p1);
  if (sig2 <= 0) return 0;

  Double_t ee = -0.5*(x-p1)*(x-p1)/sig2;
  return (ee > -20) ? p0*TMath::Exp(ee) : 0;
}

Double_t SpReso::GetSig(Double_t rgen, Double_t x)
{
  Double_t par[Nx+2];
  for (Int_t i = 0; i < Nx+2; i++) par[i] = GetPar(rgen, i+2);

  static Double_t Rgen = 0;
  static Double_t D2[Nx];
#ifdef _OPENMP
#pragma omp threadprivate(Rgen,D2)
#endif
  if (Rgen == 0 || rgen == 0 || Rgen != rgen) {
    SplFit::Spline(Nx, fXn, par, &par[Nx], D2);
    Rgen = rgen;
  }

  Int_t n = Nx-1;
  return (x < fXn[0]) ? par[0]+par[n+1]*(x-fXn[0])
      : ((x > fXn[n]) ? par[n]+par[n+2]*(x-fXn[n])
      : SplFit::Splint(Nx, fXn, par, D2, x, SplFit::NORMAL));
}

Double_t SpReso::Satur(Double_t x, Double_t s0, Double_t s1)
{
  Double_t y = (x-s0)/s1;
  Double_t e = TMath::Exp(y);

  return s0+(1-(e+1/e)/(e-1/e))*y/2*s1;
}

Double_t SpReso::GetPar(Double_t rgen, Int_t i)
{
  enum { np = Nr+2 };

  Double_t ln[Nr];
  for (Int_t j = 0; j < Nr; j++) ln[j] = TMath::Log10(fRn[j]);

  Double_t rs = TMath::Abs(rgen); if (rs < 0.8) rs = 0.8;
  Double_t x  = TMath::Log10(rs);
  Double_t x1 = ln[0];
  Double_t x2 = ln[Nr-1];

  static Double_t *D2 = 0;
  if (!D2 || i < 0) {
#ifdef _OPENMP
#pragma omp critical (d2chk)
#endif
   if (!D2 || i < 0) {
     if (!D2) D2 = new Double_t[Nr*Np];
     for (Int_t j = 0; j < Np; j++)
       SplFit::Spline(Nr, ln, &fSp[j*np], &fSp[j*np+Nr], &D2[j*Nr]);
   }
  }
  if (i < 0 || i >= Np) return 0;

  Double_t y = SplFit::Splint(Nr, ln, &fSp[i*np], &D2[i*Nr], x);
  if (x < x1) y = fSp[i*np]     +fSp[i*np+Nr]  *(x-x1);
  if (x > x2) y = fSp[i*np+Nr-1]+fSp[i*np+Nr+1]*(x-x2);

  return y;
}

TF1 *SpReso::Fit(TH1 *hist, Option_t *option, Double_t range)
{
  hist->Fit("gaus", "q0", "", -range/20, range/20);
  TF1 *fg0 = hist->GetFunction("gaus");

  Double_t p0 = fg0->GetParameter(0);
  Double_t p1 = fg0->GetParameter(1);

  TF1 *func = new TF1("func", SpReso::PDFpar, -range, range, 13);
  func->SetParameters(p0, p1);

  Double_t pl = 0;
  Double_t ph = 0;

  for (Int_t i = 0; i < Nx; i++) {
    Int_t   ib = hist->FindBin(fXn[i]);
    Double_t x = hist->GetBinCenter (ib);
    Double_t y = hist->GetBinContent(ib);
    if (y <= 0) y = hist->Integral(ib-1, ib+1)/3;
    if (y <= 0) y = hist->Integral(ib-2, ib+2)/5;
    if (y <= 0) {
      func->SetParameter(i+2, 0);
      continue;
    }
    Double_t xp = -0.5*(x-p1)*(x-p1);
    Double_t pr = xp/TMath::Log(y/p0);
    func->SetParameter(i+2, pr);
    if (x < 0 && pl == 0) pl = pr;
    if (x > 0)            ph = pr;
  }
  func->SetParameter(Nx+2,  -0.7);
  func->SetParameter(Nx+3,   1.6);
  func->FixParameter(Nx+4,     1);
  func->SetParLimits(Nx+2, -5, 0);
  func->SetParLimits(Nx+3,  0, 5);

  if (func->GetParameter(2) == 0) {
    func->FixParameter(2,       0);
    func->FixParameter(Nx+2, -0.7);
  }
  if (func->GetParameter(Nx+1) == 0)
    func->FixParameter(Nx+3, 1.7);

  for (Int_t i = 0; i < Nx/2; i++)
    if (func->GetParameter(i+2) == 0) func->SetParameter(i+2, pl);

  for (Int_t i = Nx/2; i < Nx; i++)
    if (func->GetParameter(i+2) == 0) func->SetParameter(i+2, ph);

  if (option) hist->Fit(func, option, "", -range, range);

  return func;
}

void SpReso::SetPar(Int_t part, Int_t cfw)
{
  // Proton with MC cutoff,  TRMCFFKEY.OuterSmearingC = 0.25;
  Double_t sp1[Nt] =
  {  0.0154, 0.0165, 0.0149, 0.0134, 0.0122, 0.0139, 0.0154, 0.0102, -0.0000, 
     0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     1.268,  1.474,  2.735,  4.910,  7.320,  8.899,  7.562, -0.317,  0.032, 
     0.865,  1.682,  2.308,  2.824,  3.499,  3.793,  3.198,  2.402, -0.077, 
     1.207,  1.292,  1.518,  1.802,  2.039,  2.125,  2.000, -0.588, -0.049, 
     0.862,  0.917,  1.140,  1.339,  1.561,  1.227,  1.045, -0.279, -0.060, 
     1.348,  1.017,  1.151,  1.402,  1.614,  1.215,  1.106, -2.632, -0.027, 
     1.626,  1.495,  1.709,  1.959,  2.107,  2.125,  1.965, -0.456, -0.141, 
     2.951,  2.598,  2.747,  3.089,  3.576,  3.802,  3.225,  0.896, -0.104, 
    15.301, 12.274, 13.187, 12.809, 15.056, 16.509, 15.455, 17.536, -0.975, 
    -0.701, -0.701, -0.720, -0.788, -1.027, -1.510, -1.517, -0.006,  0.083, 
     1.700,  1.700,  1.700,  1.700,  1.700,  1.700,  1.700,  0.000,  0.000 };

  // Proton without MC cutoff,  TRMCFFKEY.OuterSmearingC = 0.25;
  Double_t sp2[Nt] =
  {  0.0154, 0.0165, 0.0148, 0.0134, 0.0122, 0.0139, 0.0154, 0.0103, 0.0000, 
     0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000,  0.000, 
     1.398,  1.460,  2.666,  4.916,  7.322,  8.903,  7.559, -0.607,  0.047, 
     1.036,  1.731,  2.303,  2.824,  3.500,  3.794,  3.198,  1.663, -0.074, 
     1.268,  1.296,  1.518,  1.802,  2.039,  2.125,  2.000, -0.770, -0.049, 
     0.861,  0.920,  1.143,  1.339,  1.561,  1.227,  1.045, -0.282, -0.061, 
     1.357,  1.020,  1.152,  1.402,  1.614,  1.215,  1.106, -2.664, -0.028, 
     1.624,  1.496,  1.710,  1.959,  2.107,  2.125,  1.965, -0.469, -0.141, 
     2.956,  2.584,  2.748,  3.089,  3.576,  3.802,  3.225,  1.099, -0.106, 
    15.314, 12.394, 13.198, 12.796, 15.054, 16.508, 15.456, 16.823, -0.977, 
    -0.700, -0.701, -0.718, -0.788, -1.027, -1.510, -1.517, -0.004,  0.083, 
     1.700,  1.700,  1.700,  1.700,  1.700,  1.700,  1.700,  0.000,  0.000 };

  // Helium (without MC cutoff) 
  Double_t sp3[Nt] = 
  {  0.0113, 0.0137, 0.0166, 0.0136, 0.0119, 0.0136, 0.0251, 0.0192, -0.0006, 
     0.604,  0.234,  0.067,  0.038,  0.020,  0.004, -0.003, -4.423, -0.002, 
     2.023,  1.425,  1.556,  3.985,  6.877,  8.018,  7.243, -3.429,  0.107, 
     1.888,  1.615,  1.600,  2.976,  3.914,  4.183,  2.950, -4.102,  0.174, 
     1.932,  1.529,  1.310,  1.984,  2.458,  2.340,  1.510, -4.758, -0.046, 
     1.796,  1.256,  0.932,  1.368,  1.657,  1.267,  0.589, -6.222, -0.024, 
     2.282,  1.351,  1.000,  1.295,  1.587,  1.243,  0.588, -11.616, -0.007, 
     2.907,  2.118,  1.785,  2.051,  2.475,  2.340,  1.497, -9.666, -0.057, 
     3.830,  3.346,  3.033,  3.312,  4.007,  4.160,  2.927, -4.569,  0.224, 
    11.604, 11.846, 11.476, 13.222, 13.626, 14.625, 14.599,  4.825,  0.364, 
    -0.701, -0.697, -0.703, -0.789, -0.974, -1.157, -1.466, -0.010,  0.034, 
     1.550,  1.550,  1.550,  1.550,  1.550,  1.550,  1.550,  0.000,  0.000 };

  Double_t rn3[Nr] = { 2, 3, 8, 50, 100, 250, 4000 };

  Double_t *rn = 0;
  Double_t *sp = 0;
  if (part == 1 &&  cfw) sp = sp1;
  if (part == 1 && !cfw) sp = sp2;
  if (part == 2)       { sp = sp3; rn = rn3; }

  if (!sp) {
    cout << "SpReso::SetPar-E-Not supported: part= " << part
	 << " cfw= " << cfw << endl;
    return;
  }
  for (Int_t i = 0; i < Nt;       i++) fSp[i] = sp[i];
  for (Int_t i = 0; i < Nr && rn; i++) fRn[i] = rn[i];

  GetSig(0,  0);
  GetPar(0, -1);
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

  TString sopt = opt; sopt.ToLower();
  sopt.ReplaceAll("bfl", "");
  sopt.ReplaceAll("bfu", "");
  gr->Fit(func, sopt, "", xmin, xmax);
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

  TString sopt = opt; sopt.ToLower();
  sopt.ReplaceAll("bfl", "");
  sopt.ReplaceAll("bfu", "");
  hist->Fit(func, sopt, "", xmin, xmax);
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
