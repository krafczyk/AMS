//  $Id$

#include "GausBF.h"
#include "TkDBc.h"
#include "TrFit.h"
#include "TMath.h"
#include "TGraph.h"
#include "TF1.h"

ClassImp(GausBF);

Double_t GausBF::_Zref = 0;
GausBF  *GausBF::_Head = 0;

static Int_t Nsen1[7] = { 15, 15, 15, 15, 14, 13, 11 };

GausBF::GausBF(void)
{
  Init();
}

GausBF::~GausBF()
{
}

void GausBF::Init(void)
{
  for (Int_t i = 0; i < Ns1*Ns9*Np; i++) _par[i] = 0;
}

void GausBF::Check(void)
{
  for (Int_t i = 0; i < Ns1; i++) 
    for (Int_t j = 0; j < Ns9; j++) {
      for (Int_t k = 0; k < Np/3; k++) {
	if (GetPar(i, j, k*3) == 0) continue;
	if (TMath::Abs(GetPar(i, j, k*3+2)) <   1 ||
	    TMath::Abs(GetPar(i, j, k*3+1)) > 200)
	  GetPar(i, j)[k*3] = GetPar(i, j)[k*3+1] = 0;
      }
    }
}

void GausBF::Print(Option_t *option) const
{
  TString sopt = option; sopt.ToLower();
  if (sopt == "dump") {
    for (Int_t i = 0; i < Ns1*Ns9*Np; i++) {
      cout << Form(" %10.3e,", _par[i]);
      if (i%6 == 5) cout << endl;
    }
    cout << endl;
  }
  if (sopt.BeginsWith("p")) {
    TString ss = sopt(1, 2);
    if (ss.IsDigit()) {
      Int_t ip = ss.Atoi();
      if (0 <= ip && ip < Np)
	for (Int_t i = 0; i < Ns1; i++) {
	  for (Int_t j = 0; j < Ns9; j++) {
	    AMSPoint p1 = GetP1(i);
	    AMSPoint p9 = GetP9(j);
	    cout << Form("%3d %3d %6.2f %6.2f %7.2f %7.2f %10.3e",
			 i, j, p1.x(), p1.y(),
			       p9.x(), p9.y(), _par[(i*Ns9+j)*Np+ip]) << endl;
	  }
	}
    }
  }
}

Double_t *GausBF::GetPar(Int_t is1, Int_t is9)
{
  return (0 <= is1 && is1 < Ns1 &&
	  0 <= is9 && is9 < Ns9) ? &_par[(is1*Ns9+is9)*Np] : 0;
}

const Double_t *GausBF::GetPar(Int_t is1, Int_t is9) const
{
  return (0 <= is1 && is1 < Ns1 &&
	  0 <= is9 && is9 < Ns9) ? &_par[(is1*Ns9+is9)*Np] : 0;
}

Double_t GausBF::GetPar(Int_t is1, Int_t is9, Int_t ip) const
{
  const Double_t *p = GetPar(is1, is9);
  return (p && 0 <= ip && ip < Np) ? p[ip] : 0;
}

void GausBF::SetPar(Int_t is1, Int_t is9, Int_t ip, Double_t par)
{
  Double_t *p = GetPar(is1, is9);
  if (p && 0 <= ip && ip < Np) p[ip] = par;
}

TF1 *GausBF::GetBf(Int_t is1, Int_t is9) const
{
  TF1 *func = new TF1("fbf", FB(), -200, 200);
  for (Int_t i = 0; i < func->GetNpar() && i < Np; i++)
    func->SetParameter(i, GetPar(is1, is9, i));

  return func;
}

TF1 *GausBF::GetDf(Int_t is1, Int_t is9, Double_t rgt) const
{
  AMSPoint  p1 = GetP1(is1);
  AMSPoint  p9 = GetP9(is9);
  AMSPoint pnt = p1;
  AMSDir   dir = p9-p1;

  TString str = "[1]+1/[2]*TMath::C()/1e12*(0";
  for (Int_t i = 0; i < 12; i++) str += SV(i*3+3);
  str += ")";

  static TF1 *func = 0;
  if (!func) func = new TF1("fdf", str);
  func->FixParameter(0, 0);
  func->FixParameter(1, dir.y()/dir.z());
  func->FixParameter(2, rgt);

  for (Int_t i = 0; i < Np; i++) {
    Double_t par = GetPar(is1, is9, i);
    if (par != 0 && (i < 3 || i%3 == 0)) func->SetParameter(i+3, par);
    else                                 func->FixParameter(i+3, par);
  }

  return func;
}

TF1 *GausBF::GetPr(Int_t is1, Int_t is9, Double_t rgt) const
{
  AMSPoint  p1 = GetP1(is1);
  AMSPoint  p9 = GetP9(is9);
  AMSPoint pnt = p1;
  AMSDir   dir = p9-p1;

  TF1 *fdr = GetDf(is1, is9, rgt);
  Double_t dy0 = fdr->Eval(p1.z())-dir.y()/dir.z();

  Int_t ip = 11;
  TString str = Form("[0]+[1]*(x-%.4f)-(%f)*(x-27)-[3]", p1.z(), dy0);
  str += SE(5)+SE(8);
  str += "+[4]*[2]*TMath::C()/1e12*(0";
  for (Int_t i = 0; i < 7; i++) str += SE(i*3+ip);
  str += ")";

  static TF1 *func = 0;
  if (!func) func = new TF1("fpr", str);
  func->SetParameter(0, pnt.y());
  func->SetParameter(1, dir.y()/dir.z());
  func->SetParameter(2, 1/rgt);
  func->SetParameter(3, 0);
  func->FixParameter(4, TMath::Sqrt(1.+(dir[0]*dir[0]
				       +dir[1]*dir[1])/dir[2]/dir[2])*
		                    (1.+dir[1]*dir[1] /dir[2]/dir[2]));

  func->SetParameter(5,   0); func->SetParameter( 8,   0);
  func->FixParameter(6,  65); func->FixParameter( 9, -65);
  func->FixParameter(7, 0.1); func->FixParameter(10, 0.1);

  for (Int_t i = 0; i < 21; i++) func->FixParameter(i+ip, GetPar(is1, is9, i));

  func->FixParameter(3, func->Eval(pnt.z())-p1.y());

  return func;
}


TkDBc *GausBF::TkDBc()
{
  return TrProp::TkDBc();
}

AMSPoint GausBF::GetP1(Int_t is1)
{
  Int_t l = -1, s = -1;
  for (Int_t i = 0; i < 7; i++)
    if  (is1 <  Nsen1[i]) { l = i; s = is1; break; }
    else is1 -= Nsen1[i];

  AMSPoint pnt;
  if (l >= 0 && s >= 0) {
    pnt[0] = TkDBc()->_SensorPitchK*(s+0.5);
    pnt[1] = TkDBc()->_ladder_Ypitch*l;
    pnt[2] = TkDBc()->GetZlayerAJ(1);
  }
  return pnt;
}

AMSPoint GausBF::GetP9(Int_t is9)
{
  Int_t l = is9/11;
  Int_t s = is9%11;
  Int_t g = 1;

  AMSPoint pnt;
  if (0 <= l && l < 16) {
    if (l < 8) g = -1; else l = 15-l;
    pnt[0] = TkDBc()->_SensorPitchK *(s+0.5)*g;
    pnt[1] = TkDBc()->_ladder_Ypitch*(l-3.5);
    pnt[2] = TkDBc()->GetZlayerAJ(9);
  }
  return pnt;
}

Int_t GausBF::Find(AMSPoint pt1, AMSPoint pt2)
{
  AMSDir   d  = pt2-pt1;
  AMSPoint p1 = pt1+d*(TkDBc()->GetZlayerAJ(1)-pt1.z())/d.z();
  AMSPoint p9 = pt1+d*(TkDBc()->GetZlayerAJ(9)-pt1.z())/d.z();

  if (p1.x() < 0) { p1[0] *= -1; p9[0] *= -1; }
  if (p1.y() < 0) { p1[1] *= -1; p9[1] *= -1; }

  Int_t l1 = TMath::Floor(p1.y()/TkDBc()->_ladder_Ypitch+0.5);
  Int_t l9 = TMath::Floor(p9.y()/TkDBc()->_ladder_Ypitch);
  Int_t s1 = TMath::Floor(p1.x()/TkDBc()->_SensorPitchK);
  Int_t s9 = TMath::Floor(p9.x()/TkDBc()->_SensorPitchK);

  if (l1 <  0 || l1 > 5 || s1 <   0 || s1 >= Nsen1[l1] ||
      l9 < -4 || l9 > 3 || s9 < -11 || s9 >= 11) return -1;

  Int_t is1 = s1;
  for (Int_t l = 0; l < l1; l++) is1 += Nsen1[l];

  if (s9 >= 0) l9 = 11-l9; else { l9 += 4; s9 = -s9-1; }
  Int_t is9 = l9*11+s9;

  if ((is1 == 59 && is9 == 98) || (is1 == 59 && is9 == 109) ||
      (is1 == 73 && is9 == 98) || (is1 == 86 && is9 ==  97) ||
      (is1 == 86 && is9 == 98) || (is1 == 97 && is9 ==  98)) return -1;

  return is9*100+is1;
}


GausBF *GausBF::Head()
{
  if (!_Head) {
    TString sbf = getenv("AMSDataDir");
    sbf += "/v5.01/GausBF.root";
    if (getenv("GausBF")) sbf = getenv("GausBF");

    if (!GausBF::Load(sbf)) _Head = new GausBF;
  }
  return _Head;
}

GausBF *GausBF::Load(const char *fname)
{
  TFile f(fname);
  if (!f.IsOpen()) return 0;

  _Head = (GausBF *)f.Get("GausBF");
  if (_Head) {
    cout << "GausBF::Load-I-Loaded from: " << fname << endl;
  //_Head->Check();
  }
  return _Head;
}

TF1 *GausBF::Bfit(TGraph *gr)
{
  TF1 *func = new TF1("func", FB(), -200, 200);
  gr->Fit("gaus", "q0");
  TF1 *fg0 = gr->GetFunction("gaus");

  Int_t ip = 3;
  for (Int_t i = 0; i < 3; i++) func->SetParameter(i, fg0->GetParameter(i));

  TGraph gd;
  TGraph gm;

  Double_t min = 0, max = 0;
  Double_t xmn = gr->GetY()[0];
  Double_t xmx = gr->GetY()[0];
  Double_t xb  = gr->GetX()[0];
  Double_t db  = 0;

  Double_t mmin = 0.002;

  for (Int_t i = 0; i < gr->GetN(); i++) {
    Double_t x = gr->GetX()[i];
    Double_t y = gr->GetY()[i];
    Double_t d = y-func->Eval(x);

    gd.SetPoint(i, x,  d);
    gm.SetPoint(i, x, -d);

    if (db*d < 0) {
      if ((d > 0 && min < -mmin) || (d < 0 && max > mmin)) {
	Double_t xm = (d > 0) ? xmn : xmx;
	Double_t w1 = x-xm;
	Double_t w2 = xm-xb;
	Double_t w  = (w1 > w2) ? w2 : w1;
	Double_t par[3];
	if (d < 0) {
	  gd.Fit("gaus", "q0", "", xm-w, xm+w);
	  TF1 *fg = gd.GetFunction("gaus");
	  for (Int_t j = 0; j < 3; j++) par[j] = fg->GetParameter(j);
	}
	else {
	  gm.Fit("gaus", "q0", "", xm-w, xm+w);
	  TF1 *fg = gm.GetFunction("gaus");
	  for (Int_t j = 0; j < 3; j++) par[j] = fg->GetParameter(j);
	  par[0] = -par[0];
	}

	for (Int_t j = 0; j < 3; j++) func->SetParameter(ip+j, par[j]);
	ip += 3;
      }
      xb = x;
      if (d < 0) min = 0;
      if (d > 0) max = 0;
    }
    db = d;

    if (d < min) { min = d; xmn = x; }
    if (d > max) { max = d; xmx = x; }
  }
  for (Int_t i = ip; i < Np; i++) func->FixParameter(i, (i%3 == 2) ? 1 : 0);

  gr->Fit(func, "q0");

  return func;
}

Double_t GausBF::Gfit(Int_t is1, Int_t is9, Double_t rgt, TGraph *gr,
		                                          TGraph *gb)
{
  if ((is1 == 59 && is9 == 98) || (is1 == 59 && is9 == 109) ||
      (is1 == 73 && is9 == 98) || (is1 == 86 && is9 ==  97) ||
      (is1 == 86 && is9 == 98) || (is1 == 97 && is9 ==  98)) return -1;

  AMSPoint p1 = GetP1(is1);
  AMSPoint p9 = GetP9(is9);

  if (rgt == 0) {
    TGraph grb; if (!gb) gb = &grb;
    TrProp trp(p1, p9-p1, rgt);
    for (Int_t i = 0; i < 200; i++) {
      Double_t z = -200+(i+0.5)*2;
      trp.Propagate(z);
      gb->SetPoint(i, z, trp.GuFld(trp.GetP0()).x());
    }

    TF1 *func = Bfit(gb);

    Double_t *par = GausBF::Head()->GetPar(is1, is9);
    for (Int_t i = 0; i < func->GetNpar() && i < Np; i++)
      par[i] = func->GetParameter(i);

    if (par[0] == 0) cout << "Failed: " << is1 << " " << is9 << endl;

    for (Int_t i = 0; i < Np/3; i++)
      if (par[i*3] != 0) {
	if (par[i*3+2] < 0) par[i*3+2] *= -1;
	if (par[i*3+2] < 1 || TMath::Abs(par[i*3+1]) > 200) {
	  par[i*3] = par[i*3+1] = 0;
	             par[i*3+2] = 1;
	}
      }

    Double_t err = 1e-3;
    Double_t csq = 0;
    for (Int_t i = 0; i < gb->GetN(); i++) {
      Double_t d = gb->GetY()[i]-func->Eval(gb->GetX()[i]);
      csq += d*d/err/err;
    }
    return csq;
  }

  TGraph grp; if (!gr) gr = &grp;
  TrProp trp(p1, p9-p1, rgt);

  AMSPoint pnt;
  AMSDir   dir;
  if (_Zref == 0) _Zref = TkDBc()->GetZlayerAJ(1);

  for (Int_t i = 0; i < 9; i++) {
    Double_t z = TkDBc()->GetZlayerAJ(i+1);
    trp.Propagate(z);
    gr->SetPoint(i, z, trp.GetP0y());
    if (i == 0) { pnt = trp.GetP0(); dir = trp.GetDir(); }
  }

  TF1 *func = GausBF::Head()->GetPr(is1, is9, rgt); 
  gr->Fit(func, "q0");

  for (Int_t i = 0; i < Np; i++)
    GausBF::Head()->SetPar(is1, is9, i, func->GetParameter(i+5));

  Double_t csq = 0;
  for (Int_t i = 0; i < gr->GetN(); i++) {
    Double_t res = (gr->GetY()[i]-func->Eval(gr->GetX()[i]))*1e4;
//  cout << Form("%6.1f %7.3f %7.3f  %6.1f", gr->GetX()[i], gr->GetY()[i],
//		 func->Eval(gr->GetX()[i]), res) << endl;
    gr->GetY()[i] = res;
    csq += res*res;
  }
  delete func;

  return csq/gr->GetN();
}

TString GausBF::FB(void)
{
  TString str = "gaus( 0)+gaus( 3)+gaus( 6)+gaus( 9)+gaus(12)+gaus(15)+"
                "gaus(18)+gaus(21)+gaus(24)+gaus(27)+gaus(30)+gaus(33)";
  return str;
}

TString GausBF::SE(Int_t i)
{
  TString str = Form("+[%d]*sqrt(pi/2)*"
		      "[%d]*(TMath::Erfc((x-[%d])/[%d]/sqrt(2))*(x-[%d])"
		     "-[%d]/sqrt(pi/2)*exp(-(x-[%d])^2/[%d]^2/2))",
		     i, i+2, i+1, i+2, i+1, i+2, i+1, i+2);
  return str;
}

TString GausBF::SV(Int_t i)
{
  TString str = Form("+[%d]*sqrt(pi/2)*"
		      "[%d]*TMath::Erfc((x-[%d])/[%d]/sqrt(2))",
		     i, i+2, i+1, i+2);
  return str;
}
