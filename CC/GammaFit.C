
#include "GammaFit.h"

#include "root.h"
#include "TMath.h"
#include "TrTrack.h"
#include "TrdKCluster.h"

Double_t GammaFit::_zme = 1/0.511e-3;   // Z/M for electron
Double_t GammaFit::_zmp = 1/0.9383;     // Z/M for proton
Double_t GammaFit::_zmh = 2/3.7274;     // Z/M for Helium

GammaFit::GammaFit(TrTrackR *track, int fitid)
{
  Init();
  if (!track) return;
  if (!track->ParExists(fitid)) fitid = 0;

  _rgt = track->GetRigidity(fitid);
  _drv = track->GetErrRinv (fitid);

  TrdKCluster trdk(AMSEventR::Head(), track,
		   (fitid > 0) ? fitid : track->Gettrdefaultfit());
  FillTrdHits(trdk);
}

GammaFit::GammaFit(TrdTrackR *track)
{
  Init();

  TrdKCluster trdk(AMSEventR::Head(), track, 0);
  FillTrdHits(trdk);
}

GammaFit::GammaFit(Int_t nhit, Double_t *len, Double_t *amp,
		   Int_t time, Double_t  rgt, Double_t  drv)
{
  Init();

  _nhit = 0;
  for (Int_t i = 0; i < nhit && i < Nmax; i++) {
    _len[i] = len[i];
    _amp[i] = amp[i];
    _nhit++;
  }
  _rgt = rgt;
  _drv = drv;

  AMSEventR *evt = AMSEventR::Head();
  if (!evt) {
    evt = AMSEventR::Head() = new AMSEventR;
    evt->SetCont();
  }
  if (time > 0) evt->fHeader.Time[0] = time;

  AMSPoint    pnt;
  AMSDir      dir;
  TrdKCluster trdk(evt, &pnt, &dir);
  _pxe = trdk.GetXePressure();
}

void GammaFit::Init(void)
{
  _nhit = 0;
  _rgt = _drv = _pxe = 0;

  for (Int_t i = 0; i < Nmax;    i++) _len  [i] = _amp  [i] = 0;
  for (Int_t i = 0; i < Npart*2; i++) _gamma[i] = _likel[i] = 0;
}

Double_t GammaFit::Fit(Double_t &likelihood, int particle,
	                                     int method)
{
  GammaPDF *pdf = GammaPDF::GetPtr(particle);
  if (!pdf) return -1;

  Double_t wt = (method   == kTRDOnly)  ? 0 : 4;
  Double_t zm = (particle == kElectron) ? _zme :
               ((particle == kProton)   ? _zmp : _zmh);
  Double_t rt = (particle == kElectron) ? TMath::Abs(_rgt) : _rgt;

  Int_t i = method*Npart+particle;
  _gamma[i] = pdf->GammaFit(_pxe, _len, _amp, _nhit, rt, _drv, zm, wt);
  _likel[i] = pdf->fLH[0];

  likelihood = _likel[i];
  return _gamma[i];
}

Int_t GammaFit::FitAll(void)
{
  Double_t lk;
  Int_t ret = 0;

  if (Fit(lk, kElectron, kTRDOnly) < 0) ret = -1;
  if (Fit(lk, kProton,   kTRDOnly) < 0) ret = -1;
  if (Fit(lk, kElectron, kTrTrack) < 0) ret = -1;
  if (Fit(lk, kProton,   kTrTrack) < 0) ret = -1;

  return ret;
}

Int_t GammaFit::FillTrdHits(TrdKCluster &trdk)
{
  _pxe = trdk.GetXePressure();

  AMSPoint pnt = trdk.GetPropogated_TrTrack_P0();
  AMSDir   dir = trdk.GetPropogated_TrTrack_Dir();
  Int_t     nh = 0;

  for (Int_t i = 0; i < trdk.NHits() && nh < Nmax; i++) {
    TrdKHit *hit = trdk.GetHit(i);
    if (!hit) continue;

    Double_t len = hit->Tube_Track_3DLength(&pnt, &dir);
    Double_t amp = hit->TRDHit_Amp;
    if (len > 0 && amp > 0) {
      _len[nh] = len;
      _amp[nh] = amp;
      nh++;
    }
  }

  return (_nhit = nh);
}

#include "TF1.h"

GammaPDF *GammaPDF::fPtr[3] = { 0, 0, 0 };
Int_t     GammaPDF::fDebug  = 0;

GammaPDF *GammaPDF::GetPtr(int part)
{
  if (part == 0 || TMath::Abs(part) > 3) return 0;

  Int_t ip = (part > 0) ? part-1 : -part-1;
  if   (!fPtr[ip]) new GammaPDF(part);
  return fPtr[ip];
}

GammaPDF::GammaPDF(int part)
{
  fPart  = part;
  fFPDF  = new TF1("fPDf",  GEGG, 10, 1e5, 8);
  fFPDFL = new TF1("fPDFL", LPDF,  1,   5, 8);
  fAr.Add(fFmn1  = new TF1("fmn1",  (fPart == 1) ? EE : BB, 1, 1e5, 
			            (fPart == 1) ? 4 : 5));
  fAr.Add(fFsig1 = new TF1("fsig1", EEEE, 1, 1e5, 8));
  fAr.Add(fFtail = new TF1("ftail", EE,   1, 1e5, 4));
  fAr.Add(fFfrac = new TF1("ffrac", EE,   1, 1e5, 4));
  fAr.Add(fFmn2  = new TF1("fmn2",  EE,   1, 1e5, 4));
  fAr.Add(fFsig2 = new TF1("fsig2", EE,   1, 1e5, 4));
  fAr.Add(fFsig3 = new TF1("fsig3", EE,   1, 1e5, 4));

  Double_t pini[7] = { 150, 1.9, 0.9, 0.0, 700, 1.4, 1.8 };
  for (Int_t i = 0; i < 7; i++) {
    TF1 *fn = new TF1(Form("f0%dp0", i+1), "pol0");
    fn->SetParameter(0, pini[i]);
    fAr.Add(fn);
  }

  Init(part);
  fPtr[part-1] = this;

  /*
    if (part == 1) {
      Load("tksel/tkpar/prfit01.root");
      Load("tksel/tkpar/prfit12.root"); EEscale(3, 0.5e-3);
   if(Load("tksel/tkpar/prfit11.root") < 0) {
      Load("tksel/tkpar/prfit22.root"); EEscale(2, 0.5e-3); }
      Load("tksel/tkpar/prfit21.root");
      Load("tksel/tkpar/prfit31.root");
      Load("tksel/tkpar/prfit41.root");
    }
    if (part == 2) {
      Load("tksel/tkpar/prfit02.root");  // 1
      Load("tksel/tkpar/prfit12.root");  // 3
      Load("tksel/tkpar/prfit22.root");  // 2
      Load("tksel/tkpar/prfit21.root");  // 6,7
      Load("tksel/tkpar/prfit31.root");  // 5
      Load("tksel/tkpar/prfit32.root");  // 4
    }
  */
}


#include "TFile.h"
#include "TList.h"
#include "TKey.h"

Int_t GammaPDF::Load(const char *fname)
{
  TFile f(fname);
  if (!f.IsOpen()) return -1;

  TIter it(f.GetListOfKeys());
  Int_t nf = 0;
  TKey *key;

  while (key = (TKey *)it()) {
    TString scn = key->GetClassName();
    if (scn == "TF1") {
      TObject *obj = fAr.FindObject(key->GetName());
      if (obj) fAr.Remove(obj);
      fAr.Add(f.Get(key->GetName()));
      nf++; 
    }
  }

  cout << "Functions loaded: " << nf << " from " << fname << endl;
  return nf;
}

TF1 *GammaPDF::GetFdE(void)
{
  TF1 *func = new TF1("fFdE", GE, 10, 1e5, 4);
  func->SetParameters(fFPDF->GetParameter(0), fFPDF->GetParameter(1),
		      fFPDF->GetParameter(2), fFPDF->GetParameter(3));
  return func;
}

TF1 *GammaPDF::GetFTR(void)
{
  TF1 *func = new TF1("fFdE", GG, 10, 1e5, 4);
  func->SetParameters(fFPDF->GetParameter(0) *fFPDF->GetParameter(4),
		      fFPDF->GetParameter(5), fFPDF->GetParameter(6),
		      fFPDF->GetParameter(7));
  return func;
}

Int_t GammaPDF::SetPar(Double_t px, Double_t pl, Double_t bgm, Int_t fix)
{
  for (Int_t i = 0; i < fFPDF->GetNpar()-1; i++) {
    TF1 *func = (TF1 *)fAr.At(i);
    for (Int_t j = 0; j < func->GetNpar(); j++) {
      Double_t par = Eval(Form("%dp%d", i+1, j), px, pl);

      if (par == par) func->SetParameter(j, par);
      else return -1;
    }
  }
  if (!(*this)(bgm, fix)) return -1;

  return 0;
}

TF1 *GammaPDF::operator()(Double_t bgm, Int_t fix)
{
  fFPDF->SetParameters(1, fFmn1 ->Eval((fPart == 2 && bgm > 1e3) ? 1e3 : bgm),
		          fFsig1->Eval(bgm), fFtail->Eval(bgm),
		          fFfrac->Eval(bgm), fFmn2 ->Eval(bgm),
		          fFsig2->Eval(bgm), fFsig3->Eval(bgm));
  for (Int_t i = 0; i < fFPDF->GetNpar(); i++) {
    if (fFPDF ->GetParameter(i) == fFPDF->GetParameter(i)) 
        fFPDFL->SetParameter(i,    fFPDF->GetParameter(i));
    else return 0;
  }

  Int_t dd = 1;
  for (Int_t i = fFPDF->GetNpar()-1; i >= 0; i--) {
    if ((fix/dd)%10) fFPDF->FixParameter(i, fFPDF->GetParameter(i));
    dd *= 10;
  }

  return fFPDF;
}

Double_t GammaPDF::Eval(TString fid, Double_t prxe, Double_t plen)
{
  Double_t bpxe[6] = { 725, 775, 825, 875, 925, 975 };

  Int_t ipxe = (prxe-bpxe[0])/50;
  if (ipxe < 0) ipxe = 0;
  if (ipxe > 4) ipxe = 4;

  TF1 *fn1 = (TF1 *)fAr.FindObject(Form("f%d", ipxe+1)+fid);
  TF1 *fn2 = (TF1 *)fAr.FindObject(Form("f%d", ipxe+2)+fid);

  if (!fn1 || !fn2) {
    TF1 *fn0 = (TF1 *)fAr.FindObject("f0"+fid);
    return (fn0) ? fn0->Eval(plen) : 0;
  }

  if (fDebug >= 2) cout << "Eval: " << fn1->GetName() << " "
			            << fn2->GetName() << endl;

  Double_t ev1 = fn1->Eval(plen);
  Double_t ev2 = fn2->Eval(plen);
  return ev1+(ev2-ev1)*(prxe-bpxe[ipxe])/50;
}

Double_t GammaPDF::Norm(void)
{
  Double_t ps = TMath::Sqrt(TMath::Pi()*2);
  Double_t p0 =              fFPDFL->GetParameter(0);
  Double_t p1 = TMath::Log10(fFPDFL->GetParameter(1));
  Double_t p2 = TMath::Log10(fFPDFL->GetParameter(2));
  Double_t p3 = TMath::Log10(fFPDFL->GetParameter(2)*1.6
			    -fFPDFL->GetParameter(3))+p1;
  Double_t p4 =              fFPDFL->GetParameter(4);
  Double_t p5 = TMath::Log10(fFPDFL->GetParameter(5));
  Double_t p6 = TMath::Log10(fFPDFL->GetParameter(6));
  Double_t p7 = TMath::Log10(fFPDFL->GetParameter(7));
  Double_t p8 = -(p3-p1)/p2/p2;
  Double_t p9 = -(p3-p1)*(p3-p1)/2/p2/p2+(p3-p1)*p3/p2/p2+TMath::Log(p0);
  Double_t u = 1.3;
  Double_t c = p5-p6*u;
  Double_t d = 4;
  Double_t b = TMath::Exp(-p6*p6*u*u/(2*p7*p7))/TMath::Exp(-u*u/2);
  Double_t pp[12] = { 0, ps*p2*p0,   p1, 0.000968482+1.1680*p2+0.01525*p2*p2,
		      0, ps*p6*p4*b, p5, 0.000782164+1.1612*p6+0.08095*p6*p6,
		      0, ps*p7*p4,   p5,   -0.025994+1.4660*p7-0.73832*p7*p7 };
  Double_t s1 = LEE(&p3, pp);
  Double_t s2 = (TMath::Exp(p9+p8*d)-TMath::Exp(p9+p8*p3))/p8;
  Double_t s3 = LEE(&c, &pp[4]);
  Double_t s4 = LEE(&d, &pp[8])-LEE(&c, &pp[8]);

  return s1+s2+s3+s4;
}

#include "TRandom.h"

Int_t GammaPDF::GenNh(void)
{
  Double_t pnh[8] = { 0.98501, 0.82510, 0.52969, 0.25338, 
		      0.09318, 0.03322, 0.01323, 0.00441 };
  Double_t rnd = gRandom->Rndm();
  for (Int_t j = 0; j < 8; j++) if (rnd > pnh[j]) return 21-j;
  return 0;
}

Double_t GammaPDF::GenPx(void)
{
  Double_t ppx[29] = { 0.99655, 0.99147, 0.98549, 0.97922, 0.96488,
		       0.94610, 0.92076, 0.88543, 0.85137, 0.81461,
		       0.77134, 0.71866, 0.66096, 0.60389, 0.54338,
		       0.48484, 0.43424, 0.38081, 0.32268, 0.27162,
		       0.22391, 0.18095, 0.13880, 0.09330, 0.06234,
		       0.03541, 0.01666, 0.00820, 0.00285 };

  Double_t rnd = gRandom->Rndm();
  for (Int_t j = 0; j < 29; j++) if (rnd > ppx[j]) return 985-j*10;
  return 700;
}

Int_t GammaPDF::Gen(Double_t  prxe, Double_t  bgm, Int_t nhit,
	       Double_t *plen, Double_t *amp)
{
  Double_t pls[30] = { 0.996753, 0.991975, 0.979274, 0.933666, 0.791487,
		       0.710487, 0.650179, 0.600176, 0.557006, 0.518682,
		       0.484382, 0.452833, 0.423610, 0.396660, 0.371600,
		       0.348009, 0.326034, 0.305375, 0.285855, 0.267627,
		       0.250383, 0.233971, 0.218452, 0.203742, 0.189872,
		       0.176843, 0.164553, 0.152967, 0.141783, 0.131309 };

  Int_t nh = 0;
  for (Int_t i = 0; i < nhit; i++) {
    Double_t rnd = gRandom->Rndm();
    plen[i] = 0;
    for (Int_t j = 0; j < 30; j++)
      if (rnd > pls[j]) { plen[i] = (64.5-j)*0.01; break; }

    if (plen[i] > 0.35) {
      if (SetPar(prxe, plen[i], bgm) < 0) {
	cout << "NaN detected for: "
	     << prxe << " " << plen[i] << " " << bgm << endl;
	return -1;
      }

      amp[i] = TMath::Power(10, fFPDFL->GetRandom());
      nh++;
    }
    else amp[i] = 0;
  }

  return nh;
}

Double_t GammaPDF::GammaL(Double_t prxe, Double_t *plen,
			                 Double_t *amp, Int_t nhit,
			  Double_t bgm)
{
  Double_t lks = 0;
  Int_t    lkn = 0;

  for (Int_t i = 0; i < nhit; i++) {
    if (plen[i] > 0.35 && amp[i] > 10) {
      if (SetPar(prxe, plen[i], bgm) < 0) continue;

      Double_t p = fFPDF->Eval(amp[i])/Norm();
      if (p > 0) {
	lks += TMath::Log10(p);
	lkn++;
      }
    }
  }
  return (lkn > 5) ? lks : -20;
}

Double_t GammaPDF::GammaFit(Double_t prxe, Double_t *plen, 
		                           Double_t *amp, Int_t nhit,
			    Double_t rtrk, Double_t  drv, Double_t zpm,
			    Double_t wtk)
{
  if (rtrk == 0) return 0;

  Double_t nsr = 2;

  Double_t lrf[5];
  Double_t lkp[5];

  Int_t j1 = -2;
  Int_t j2 =  2;
  Int_t nm = -1;
  Int_t np =  0;

  Double_t rfit = TMath::Abs(rtrk);
  if (rfit > 1e6) rfit = 1e6;

  if (fDebug) cout << "GammaPDFFit " << rtrk << " " << drv << endl;

  while (nsr > 0.2) {
    if (fDebug) cout << "rfit= " << rfit << " " << TMath::Log10(rfit) << endl;

    for (Int_t j = j1; j <= j2; j++) {
      Double_t x = j*nsr/2;
      Double_t y = TMath::Log10(rfit)+x*0.5;
      Double_t r = TMath::Power(10, y);
      Double_t k = GammaL(prxe, plen, amp, nhit, r*zpm);
      Double_t d = (1/r-1/rtrk)/drv;
      Double_t g = (TMath::Abs(d) < 10) 
	          ? TMath::Log10(TMath::Gaus(d, 0, 1, 1)) : -22;
      if (k != k) k = -22;

      lrf[np] = y;
      lkp[np] = -k-g*wtk;

      if (nm < 0 || lkp[np] < lkp[nm]) nm = np;
      np++;
    }

    if (fDebug) {
      for (Int_t j = 0; j < 5; j++) {
	cout << Form("%d %5.2f %5.2f %6.3f", j, (j-2)*nsr/2, lrf[j], lkp[j]);
	if (j == nm) cout << " <<<";
	cout << endl;
      }
    }

    if (0 < nm && nm < np-1) {
      rfit = TMath::Power(10, Peak(&lrf[nm-1], &lkp[nm-1]));
      nsr /= 2;
      j1 = -2; j2 = 2;
      nm = -1; np = 0;
    }
    else {
      if (nm == 0) {
	rfit = TMath::Power(10, TMath::Log10(rfit)-3*nsr/2*0.5);
	lrf[3] = lrf[0]; lkp[3] = lkp[0];
	lrf[4] = lrf[1]; lkp[4] = lkp[1];
	j1 = -2; j2 = 0; nm = 3; np = 0;
      }
      else {
	rfit = TMath::Power(10, TMath::Log10(rfit)+3*nsr/2*0.5);
	lrf[0] = lrf[3]; lkp[0] = lkp[3];
	lrf[1] = lrf[4]; lkp[1] = lkp[4];
	j1 = 0; j2 = 2; nm = 1; np = 2;
      }
    }

    if (rfit > 1e6) break;
  }

  if (fDebug) cout << "rfit= " << rfit << " " << TMath::Log10(rfit) << endl;

  Double_t d = (1/rfit-1/rtrk)/drv;
  fLH[0] = GammaL(prxe, plen, amp, nhit, rfit*zpm);
  fLH[1] = (TMath::Abs(d) < 10) ? TMath::Log10(TMath::Gaus(d, 0, 1, 1)) : -22;

  return rfit;
}

void GammaPDF::EEscale(Int_t ifn, Double_t scl)
{
  Double_t lsc = TMath::Log10(scl);
  for (Int_t i = 0; i < 6; i++)
    for (Int_t j = 0; j < 2; j++) {
      TF1 *fn = (TF1 *)fAr.FindObject(Form("f%d%dp%d", i+1, ifn, j*4+2));
      if (fn) fn->SetParameter(0, fn->GetParameter(0)-lsc);
    }
}

Double_t GammaPDF::Peak(Double_t *x,  Double_t *y)
{
  Double_t m[4] = { x[0]-x[1], x[0]*x[0]-x[1]*x[1],
		    x[1]-x[2], x[1]*x[1]-x[2]*x[2] };
  Double_t d = m[0]*m[3]-m[1]*m[2];

  if (d == 0) return 0;

  Double_t n[4] = { m[3]/d, -m[1]/d, -m[2]/d, m[0]/d };
  Double_t v[2] = { y[0]-y[1], y[1]-y[2] };

  Double_t p1 = n[0]*v[0]+n[1]*v[1];
  Double_t p2 = n[2]*v[0]+n[3]*v[1];

  return (p2 != 0) ? -p1/p2/2 : 0;
}

Double_t GammaPDF::LPDF(Double_t *x, Double_t *p)
{
  Double_t px = TMath::Power(10, x[0]);
  return GEGG(&px, p);
}

Double_t GammaPDF::GEGGS(Double_t *x, Double_t *p)
{
  Double_t p2[4] = { p[0]*p[4], p[5], p[6], p[7] };
  Double_t y = (GE(x, p)+GG(x, p2))/p[0];
  return p[0]*y/(1+y*y*p[8]);
}

Double_t GammaPDF::GEGG(Double_t *x, Double_t *p)
{
  Double_t p2[4] = { p[0]*p[4], p[5], p[6], p[7] };
  return GE(x, p)+GG(x, p2);
}

Double_t GammaPDF::EEEE(Double_t *xx, Double_t *pp)
{
  if (pp[3]*pp[7] == 0) return pp[0];

  return EE(xx, pp)*EE(xx, &pp[4]);
}

Double_t GammaPDF::EE(Double_t *xx, Double_t *pp)
{
  if (pp[3] == 0) return pp[0];
  Double_t x = TMath::Log10(xx[0]);
  return LEE(&x, pp);
}

Double_t GammaPDF::BB(Double_t *xx, Double_t *p)
{
  Double_t x = xx[0];
  if (x > 800) x = 800;

  Double_t l = TMath::Log10(x);
  return p[0]*(1+p[1]/x/x+p[2]*l+p[3]*l*l+p[4]*l*l*l);
}

Double_t GammaPDF::G(Double_t *xx, Double_t *pp)
{
  Double_t a = pp[0];
  Double_t x = TMath::Log10(xx[0]);
  Double_t m = TMath::Log10(pp[1]);
  Double_t s = TMath::Log10(pp[2]);
  return a*TMath::Exp(-(x-m)*(x-m)/(2*s*s));
}

Double_t GammaPDF::GE(Double_t *xx, Double_t *pp)
{
  Double_t x = TMath::Log10(xx[0]);
  return LGE(&x, pp);
}

Double_t GammaPDF::GG(Double_t *xx, Double_t *pp)
{
  Double_t x = TMath::Log10(xx[0]);
  return LGG(&x, pp);
}

Double_t GammaPDF::GES(Double_t *x, Double_t *p)
{
  Double_t y = GE(x, p)/p[0];
  return p[0]*y/(1+y*y*p[4]);
}

Double_t GammaPDF::GGS(Double_t *x, Double_t *p)
{
  Double_t y = GG(x, p)/p[0];
  return p[0]*y/(1+y*y*p[4]);
}

Double_t GammaPDF::LEE(Double_t *xx, Double_t *pp)
{
  if (pp[3] == 0) return pp[0];

  Double_t x  = (xx[0]-pp[2])/pp[3];
  Double_t e  = TMath::Exp(x);
  Double_t p0 = (pp[1]+pp[0])/2;
  Double_t p1 = (pp[1]-pp[0])/2;
  return p0+p1*(e-1/e)/(e+1/e);
}

Double_t GammaPDF::LGE(Double_t *xx, Double_t *pp)
{
  if (pp[1] <= 0 || pp[2] <= 0) return pp[0];

  Double_t a = pp[0];
  Double_t x = xx[0];
  Double_t m = TMath::Log10(pp[1]);
  Double_t s = TMath::Log10(pp[2]);
  Double_t c = TMath::Log10(pp[2]*1.6-pp[3])+m;
  if (x < c)
    return a*TMath::Exp(-(x-m)*(x-m)/(2*s*s));

  else {
    Double_t q = -(c-m)/s/s;
    Double_t p = -(c-m)*(c-m)/2/s/s+(c-m)*c/s/s+TMath::Log(a);
    return TMath::Exp(p+q*x);
  }
}

Double_t GammaPDF::LGG(Double_t *xx, Double_t *pp)
{
  if (pp[1] <= 0 || pp[2] <= 0 || pp[3] <= 0) return pp[0];

  Double_t a = pp[0];
  Double_t x = xx[0];
  Double_t m = TMath::Log10(pp[1]);
  Double_t s = TMath::Log10(pp[2]);
  Double_t t = TMath::Log10(pp[3]);
  Double_t u = 1.3;
  if (x < m-s*u) {
    Double_t b = TMath::Exp(-s*s*u*u/(2*t*t));
    Double_t c = TMath::Exp(-u*u/2);
    return a*b/c*TMath::Exp(-(x-m)*(x-m)/(2*s*s));
  }
  else
    return a*TMath::Exp(-(x-m)*(x-m)/(2*t*t));
}

Double_t GammaPDF::P11(Double_t *xx, Double_t *pp)
{
  Double_t p0 = pp[0];
  Double_t p1 = pp[1];
  Double_t p2 = pp[2];
  Double_t p3 = pp[3];
  Double_t x  = xx[0]-p1;
  return (x < 0) ? p0+p2*x : p0+p3*x;
}

Double_t GammaPDF::P22(Double_t *xx, Double_t *pp)
{
  Double_t p0 = pp[0];
  Double_t p1 = pp[1];
  Double_t p2 = pp[2];
  Double_t p3 = pp[3];
  Double_t p4 = pp[4];
  Double_t p5 = pp[5];
  Double_t x  = xx[0]-p1;
  return (x < 0) ? p0+p2*x+p3*x*x : p0+p4*x+p5*x*x;
}

Double_t GammaPDF::P31(Double_t *xx, Double_t *pp)
{
  Double_t p0 = pp[0];
  Double_t p1 = pp[1];
  Double_t p2 = pp[2];
  Double_t p3 = pp[3];
  Double_t p4 = pp[4];
  Double_t p5 = pp[5];
  Double_t x  = xx[0]-p1;
  return (x < 0) ? p0+p2*x+p3*x*x+p4*x*x*x : p0+p5*x;
}

void GammaPDF::Dump(void) const
{
  for (Int_t i = 7; i < fAr.GetEntries(); i++) {
    TF1 *func = (TF1 *)fAr.At(i);
    if (!func) continue;

    Int_t    np = func->GetNpar();
    TString str = func->GetTitle();
    TString srg;
    if (str.Contains("PDF")) 
      srg += Form(", 0, 0.7, %d", np);
    else
      str = Form("\"%s\"", str.Data());

    cout << Form("  fn = new TF1(\"%s\",%s%s);", func->GetName(), str.Data(),
		                                                  srg.Data());
    if (np == 1)
      cout << Form(" fn->SetParameter(0, %+11.4e); ", func->GetParameter(0));
    else {
      cout << endl << "  fn->SetParameters(";
      for (Int_t j = 0; j < np; j++) {
	cout << Form("%+11.4e", func->GetParameter(j));
	if (j < np-1) cout << ", ";
	if (j%4 == 3) cout << endl << "                    ";
      }
      cout << "); ";
    }
    cout << "fAr.Add(fn);" << endl;
  }
}

void GammaPDF::Init(int part)
{
 TF1 *fn;
 if (part == GammaFit::kElectron) {
  fn = new TF1("f01p0","pol0"); fn->SetParameter(0, +1.5000e+02); fAr.Add(fn);
  fn = new TF1("f02p0","pol0"); fn->SetParameter(0, +1.9000e+00); fAr.Add(fn);
  fn = new TF1("f03p0","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f04p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f05p0","pol0"); fn->SetParameter(0, +7.0000e+02); fAr.Add(fn);
  fn = new TF1("f06p0","pol0"); fn->SetParameter(0, +1.4000e+00); fAr.Add(fn);
  fn = new TF1("f07p0","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f11p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5020e+02, +6.0000e-01, +5.5936e+01, +9.9524e+01, 
                    -5.4168e+02, +2.7311e+02); fAr.Add(fn);
  fn = new TF1("f11p1","pol2");
  fn->SetParameters(-5.7974e+00, +2.0173e+02, +1.5969e+02); fAr.Add(fn);
  fn = new TF1("f11p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f11p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f21p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.4996e+02, +6.0000e-01, +3.7440e+01, +7.8919e+01, 
                    -4.9722e+02, +2.3093e+02); fAr.Add(fn);
  fn = new TF1("f21p1","pol2");
  fn->SetParameters(-1.2662e+01, +2.4128e+02, +1.1232e+02); fAr.Add(fn);
  fn = new TF1("f21p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f21p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f31p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5037e+02, +6.0000e-01, +4.6864e+01, +5.6932e+01, 
                    -6.7306e+02, +2.8309e+02); fAr.Add(fn);
  fn = new TF1("f31p1","pol2");
  fn->SetParameters(-1.0078e+01, +2.6275e+02, +6.5895e+01); fAr.Add(fn);
  fn = new TF1("f31p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f31p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f41p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5108e+02, +6.0000e-01, +4.9895e+01, +1.7053e+02, 
                    -3.1497e+02, +2.0065e+02); fAr.Add(fn);
  fn = new TF1("f41p1","pol2");
  fn->SetParameters(-1.2883e+01, +2.8525e+02, +3.6149e+01); fAr.Add(fn);
  fn = new TF1("f41p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f41p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f51p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5283e+02, +6.0000e-01, +1.1136e+02, +7.6725e+02, 
                    +1.1551e+03, +1.9738e+02); fAr.Add(fn);
  fn = new TF1("f51p1","pol2");
  fn->SetParameters(-1.9585e+01, +3.2321e+02, -9.3692e+00); fAr.Add(fn);
  fn = new TF1("f51p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f51p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f61p0",P31, 0, 0.7, 6);
  fn->SetParameters(+1.6168e+02, +6.0000e-01, +1.9369e+01, -1.5960e+02, 
                    -1.6077e+03, +2.1044e+02); fAr.Add(fn);
  fn = new TF1("f61p1","pol2");
  fn->SetParameters(-4.9697e+01, +4.4104e+02, -1.0633e+02); fAr.Add(fn);
  fn = new TF1("f61p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f61p3","pol0"); fn->SetParameter(0, +4.0000e-01); fAr.Add(fn);
  fn = new TF1("f13p0","pol0"); fn->SetParameter(0, +1.3083e+00); fAr.Add(fn);
  fn = new TF1("f13p1","pol0"); fn->SetParameter(0, +1.0164e+00); fAr.Add(fn);
  fn = new TF1("f13p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f13p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f23p0","pol0"); fn->SetParameter(0, +1.3106e+00); fAr.Add(fn);
  fn = new TF1("f23p1","pol0"); fn->SetParameter(0, +9.8840e-01); fAr.Add(fn);
  fn = new TF1("f23p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f23p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f33p0","pol0"); fn->SetParameter(0, +1.3093e+00); fAr.Add(fn);
  fn = new TF1("f33p1","pol0"); fn->SetParameter(0, +9.7351e-01); fAr.Add(fn);
  fn = new TF1("f33p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f33p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f43p0","pol0"); fn->SetParameter(0, +1.2786e+00); fAr.Add(fn);
  fn = new TF1("f43p1","pol0"); fn->SetParameter(0, +9.8329e-01); fAr.Add(fn);
  fn = new TF1("f43p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f43p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f53p0","pol0"); fn->SetParameter(0, +1.2395e+00); fAr.Add(fn);
  fn = new TF1("f53p1","pol0"); fn->SetParameter(0, +9.8874e-01); fAr.Add(fn);
  fn = new TF1("f53p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f53p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f63p0","pol0"); fn->SetParameter(0, +1.1673e+00); fAr.Add(fn);
  fn = new TF1("f63p1","pol0"); fn->SetParameter(0, +9.9796e-01); fAr.Add(fn);
  fn = new TF1("f63p2","pol0"); fn->SetParameter(0, +4.2010e+00); fAr.Add(fn);
  fn = new TF1("f63p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.1160e+00, +6.0000e-01, -2.3119e+00, -1.2780e+01, 
                    -2.2727e+01, -9.5994e-02); fAr.Add(fn);
  fn = new TF1("f12p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7882e+00, +6.0000e-01, -1.9144e+00, +1.0237e+01, 
                    +2.0954e+01, -6.6779e-01); fAr.Add(fn);
  fn = new TF1("f12p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f12p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f12p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.1050e+00, +6.0000e-01, -2.6763e+00, -1.4927e+01, 
                    -2.5079e+01, -1.4879e-03); fAr.Add(fn);
  fn = new TF1("f22p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7874e+00, +6.0000e-01, -1.9867e+00, +6.8355e+00, 
                    +4.3019e+00, -8.4885e-01); fAr.Add(fn);
  fn = new TF1("f22p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f22p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f22p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.0945e+00, +6.0000e-01, -2.3592e+00, -1.3081e+01, 
                    -2.1907e+01, +3.0045e-01); fAr.Add(fn);
  fn = new TF1("f32p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7872e+00, +6.0000e-01, -1.9716e+00, +6.7312e+00, 
                    +7.7423e+00, -8.4471e-01); fAr.Add(fn);
  fn = new TF1("f32p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f32p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f32p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.0895e+00, +6.0000e-01, -2.1035e+00, -9.4177e+00, 
                    -1.1800e+01, -6.7194e-01); fAr.Add(fn);
  fn = new TF1("f42p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7820e+00, +6.0000e-01, -1.8658e+00, +7.5440e+00, 
                    +6.8864e+00, -9.6280e-01); fAr.Add(fn);
  fn = new TF1("f42p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f42p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f42p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.0805e+00, +6.0000e-01, -1.7897e+00, -3.1968e+00, 
                    +1.0408e+01, -5.2462e-01); fAr.Add(fn);
  fn = new TF1("f52p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7635e+00, +6.0000e-01, -2.0454e+00, +5.6833e+00, 
                    -2.8910e+00, -1.0781e+00); fAr.Add(fn);
  fn = new TF1("f52p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f52p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f52p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.1316e+00, +6.0000e-01, -4.5573e+00, -3.7455e+01, 
                    -9.5213e+01, -7.9872e-01); fAr.Add(fn);
  fn = new TF1("f62p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.7327e+00, +6.0000e-01, -2.0938e+00, +1.9402e+01, 
                    +1.8402e+01, -2.5245e+00); fAr.Add(fn);
  fn = new TF1("f62p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f62p3","pol0"); fn->SetParameter(0, +3.5000e-01); fAr.Add(fn);
  fn = new TF1("f62p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p5","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p6","pol0"); fn->SetParameter(0, +4.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p7","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f16p0","pol0"); fn->SetParameter(0, +1.3504e+00); fAr.Add(fn);
  fn = new TF1("f16p1","pol0"); fn->SetParameter(0, +1.4349e+00); fAr.Add(fn);
  fn = new TF1("f16p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f16p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f17p0","pol0"); fn->SetParameter(0, +1.7014e+00); fAr.Add(fn);
  fn = new TF1("f17p1","pol0"); fn->SetParameter(0, +1.7409e+00); fAr.Add(fn);
  fn = new TF1("f17p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f17p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f26p0","pol0"); fn->SetParameter(0, +1.3558e+00); fAr.Add(fn);
  fn = new TF1("f26p1","pol0"); fn->SetParameter(0, +1.4269e+00); fAr.Add(fn);
  fn = new TF1("f26p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f26p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f27p0","pol0"); fn->SetParameter(0, +1.6931e+00); fAr.Add(fn);
  fn = new TF1("f27p1","pol0"); fn->SetParameter(0, +1.7435e+00); fAr.Add(fn);
  fn = new TF1("f27p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f27p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f36p0","pol0"); fn->SetParameter(0, +1.3548e+00); fAr.Add(fn);
  fn = new TF1("f36p1","pol0"); fn->SetParameter(0, +1.4206e+00); fAr.Add(fn);
  fn = new TF1("f36p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f36p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f37p0","pol0"); fn->SetParameter(0, +1.6788e+00); fAr.Add(fn);
  fn = new TF1("f37p1","pol0"); fn->SetParameter(0, +1.7424e+00); fAr.Add(fn);
  fn = new TF1("f37p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f37p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f46p0","pol0"); fn->SetParameter(0, +1.3551e+00); fAr.Add(fn);
  fn = new TF1("f46p1","pol0"); fn->SetParameter(0, +1.4177e+00); fAr.Add(fn);
  fn = new TF1("f46p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f46p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f47p0","pol0"); fn->SetParameter(0, +1.6701e+00); fAr.Add(fn);
  fn = new TF1("f47p1","pol0"); fn->SetParameter(0, +1.7336e+00); fAr.Add(fn);
  fn = new TF1("f47p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f47p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f56p0","pol0"); fn->SetParameter(0, +1.3553e+00); fAr.Add(fn);
  fn = new TF1("f56p1","pol0"); fn->SetParameter(0, +1.4146e+00); fAr.Add(fn);
  fn = new TF1("f56p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f56p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f57p0","pol0"); fn->SetParameter(0, +1.6663e+00); fAr.Add(fn);
  fn = new TF1("f57p1","pol0"); fn->SetParameter(0, +1.7275e+00); fAr.Add(fn);
  fn = new TF1("f57p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f57p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f66p0","pol0"); fn->SetParameter(0, +1.3411e+00); fAr.Add(fn);
  fn = new TF1("f66p1","pol0"); fn->SetParameter(0, +1.4156e+00); fAr.Add(fn);
  fn = new TF1("f66p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f66p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f67p0","pol0"); fn->SetParameter(0, +1.6336e+00); fAr.Add(fn);
  fn = new TF1("f67p1","pol0"); fn->SetParameter(0, +1.6936e+00); fAr.Add(fn);
  fn = new TF1("f67p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f67p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f15p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.0418e+02, +6.0000e-01, -1.8066e+02, +1.8046e+03, 
                    +3.1193e+03, +6.5055e+02); fAr.Add(fn);
  fn = new TF1("f15p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.9080e+02, +6.0000e-01, +7.0419e+02, -2.2652e+03, 
                    -6.5759e+03, +1.2407e+03); fAr.Add(fn);
  fn = new TF1("f15p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f15p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f25p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.8485e+02, +6.0000e-01, -4.4338e+02, -1.2407e+03, 
                    -5.5813e+03, +8.2657e+02); fAr.Add(fn);
  fn = new TF1("f25p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.6540e+02, +6.0000e-01, +7.6131e+02, -6.7130e+02, 
                    -1.5348e+03, +9.9250e+02); fAr.Add(fn);
  fn = new TF1("f25p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f25p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f35p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.7709e+02, +6.0000e-01, -1.4927e+02, +9.3445e+02, 
                    +2.1842e+02, +8.1513e+02); fAr.Add(fn);
  fn = new TF1("f35p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.3193e+02, +6.0000e-01, +5.4387e+02, -1.4850e+03, 
                    -3.5516e+03, +9.6279e+02); fAr.Add(fn);
  fn = new TF1("f35p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f35p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f45p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.5670e+02, +6.0000e-01, -1.7093e+02, +3.6322e+02, 
                    -1.4231e+03, +6.1182e+02); fAr.Add(fn);
  fn = new TF1("f45p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.0354e+02, +6.0000e-01, +4.9376e+02, -1.2542e+03, 
                    -2.6892e+03, +9.9216e+02); fAr.Add(fn);
  fn = new TF1("f45p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f45p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f55p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.3371e+02, +6.0000e-01, -1.6869e+02, +3.2715e+02, 
                    -1.4148e+03, +9.1701e+02); fAr.Add(fn);
  fn = new TF1("f55p1",P31, 0, 0.7, 6);
  fn->SetParameters(+6.7336e+02, +6.0000e-01, +4.7575e+02, -1.2183e+03, 
                    -2.3581e+03, +8.8451e+02); fAr.Add(fn);
  fn = new TF1("f55p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f55p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f65p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.0484e+02, +6.0000e-01, -5.0796e+02, -1.4396e+03, 
                    -4.7452e+03, +1.0268e+03); fAr.Add(fn);
  fn = new TF1("f65p1",P31, 0, 0.7, 6);
  fn->SetParameters(+6.5403e+02, +6.0000e-01, +6.8875e+02, -1.3300e+03, 
                    -2.1396e+03, +1.0801e+03); fAr.Add(fn);
  fn = new TF1("f65p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f65p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f14p0",P31, 0, 0.7, 6);
  fn->SetParameters(+4.6259e-01, +6.0000e-01, -8.3420e-02, +5.6702e+00, 
                    +9.3598e+00, +5.3669e-01); fAr.Add(fn);
  fn = new TF1("f14p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.8037e-01, +6.0000e-01, +7.6320e-01, -6.4528e+00, 
                    -1.3905e+01, +4.8572e-01); fAr.Add(fn);
  fn = new TF1("f14p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f14p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f24p0",P31, 0, 0.7, 6);
  fn->SetParameters(+4.9659e-01, +6.0000e-01, +4.3300e-01, +9.2045e+00, 
                    +1.8547e+01, +5.4248e-01); fAr.Add(fn);
  fn = new TF1("f24p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0138e+00, +6.0000e-01, +6.4958e-01, -8.1897e+00, 
                    -1.8983e+01, +4.3365e-01); fAr.Add(fn);
  fn = new TF1("f24p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f24p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f34p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.1181e-01, +6.0000e-01, +2.9535e-01, +9.8638e+00, 
                    +2.0722e+01, -4.2533e-01); fAr.Add(fn);
  fn = new TF1("f34p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0642e+00, +6.0000e-01, +7.0662e-01, -8.5499e+00, 
                    -2.0262e+01, +9.6464e-01); fAr.Add(fn);
  fn = new TF1("f34p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f34p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f44p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.3324e-01, +6.0000e-01, +1.2181e-01, +5.9866e+00, 
                    +9.5462e+00, +3.3410e-01); fAr.Add(fn);
  fn = new TF1("f44p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0965e+00, +6.0000e-01, +8.6296e-01, -6.8912e+00, 
                    -1.5442e+01, +7.8469e-01); fAr.Add(fn);
  fn = new TF1("f44p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f44p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f54p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.2814e-01, +6.0000e-01, -1.4781e-01, +3.4072e+00, 
                    +2.3158e+00, -4.3759e-02); fAr.Add(fn);
  fn = new TF1("f54p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1253e+00, +6.0000e-01, +1.0352e+00, -5.0374e+00, 
                    -9.2207e+00, +6.1773e-01); fAr.Add(fn);
  fn = new TF1("f54p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f54p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f64p0",P31, 0, 0.7, 6);
  fn->SetParameters(+4.6066e-01, +6.0000e-01, +2.3659e+00, +3.5037e+01, 
                    +9.2259e+01, -2.5021e+00); fAr.Add(fn);
  fn = new TF1("f64p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1430e+00, +6.0000e-01, +9.1988e-01, -1.6092e+01, 
                    -4.3873e+01, +2.5560e+00); fAr.Add(fn);
  fn = new TF1("f64p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f64p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
 }
 if (part == GammaFit::kProton) {
  fn = new TF1("f01p0","pol0"); fn->SetParameter(0, +1.5000e+02); fAr.Add(fn);
  fn = new TF1("f02p0","pol0"); fn->SetParameter(0, +1.9000e+00); fAr.Add(fn);
  fn = new TF1("f03p0","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f04p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f05p0","pol0"); fn->SetParameter(0, +7.0000e+02); fAr.Add(fn);
  fn = new TF1("f06p0","pol0"); fn->SetParameter(0, +1.4000e+00); fAr.Add(fn);
  fn = new TF1("f07p0","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f11p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.8796e+01, +6.0000e-01, +1.1800e+01, +1.0821e+03, 
                    +2.5364e+03, -1.3555e+02); fAr.Add(fn);
  fn = new TF1("f11p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.0308e-01, +6.0000e-01, -6.0533e-01, -2.7333e+01, 
                    -6.7829e+01, +8.2743e+00); fAr.Add(fn);
  fn = new TF1("f11p2",P31, 0, 0.7, 6);
  fn->SetParameters(+2.0563e-01, +6.0000e-01, +5.6584e+00, -1.4263e+01, 
                    -6.5747e+01, +1.2211e+01); fAr.Add(fn);
  fn = new TF1("f11p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.2136e-01, +6.0000e-01, -1.7725e+00, +8.8628e+00, 
                    +3.9593e+01, -8.2739e+00); fAr.Add(fn);
  fn = new TF1("f11p4",P31, 0, 0.7, 6);
  fn->SetParameters(-8.3948e-02, +6.0000e-01, +1.3020e-01, -2.5166e+00, 
                    -9.5422e+00, +2.1282e+00); fAr.Add(fn);
  fn = new TF1("f21p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.3667e+01, +6.0000e-01, -1.9411e+02, -1.0199e+03, 
                    -2.8947e+03, +3.3707e+02); fAr.Add(fn);
  fn = new TF1("f21p1",P31, 0, 0.7, 6);
  fn->SetParameters(+8.6162e-01, +6.0000e-01, +5.2895e+00, +2.8613e+01, 
                    +7.1508e+01, -5.7724e+00); fAr.Add(fn);
  fn = new TF1("f21p2",P31, 0, 0.7, 6);
  fn->SetParameters(+3.8756e-01, +6.0000e-01, +1.2017e+01, +4.4970e+01, 
                    +8.0231e+01, -7.7889e+00); fAr.Add(fn);
  fn = new TF1("f21p3",P31, 0, 0.7, 6);
  fn->SetParameters(+2.1747e-01, +6.0000e-01, -5.5555e+00, -2.6877e+01, 
                    -4.9344e+01, +5.1650e+00); fAr.Add(fn);
  fn = new TF1("f21p4",P31, 0, 0.7, 6);
  fn->SetParameters(-6.0333e-02, +6.0000e-01, +9.8772e-01, +5.6444e+00, 
                    +1.0831e+01, -1.2945e+00); fAr.Add(fn);
  fn = new TF1("f31p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.0376e+01, +6.0000e-01, -2.5696e+02, -1.5963e+03, 
                    -4.6011e+03, +4.5198e+01); fAr.Add(fn);
  fn = new TF1("f31p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.7686e-01, +6.0000e-01, +7.8166e+00, +4.8845e+01, 
                    +1.2327e+02, +3.2465e+00); fAr.Add(fn);
  fn = new TF1("f31p2",P31, 0, 0.7, 6);
  fn->SetParameters(+5.0255e-01, +6.0000e-01, +1.4710e+01, +6.7373e+01, 
                    +1.3536e+02, +4.3640e+00); fAr.Add(fn);
  fn = new TF1("f31p3",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5881e-01, +6.0000e-01, -7.4955e+00, -4.3699e+01, 
                    -9.1294e+01, -1.5488e+00); fAr.Add(fn);
  fn = new TF1("f31p4",P31, 0, 0.7, 6);
  fn->SetParameters(-4.8463e-02, +6.0000e-01, +1.4930e+00, +1.0181e+01, 
                    +2.2210e+01, +6.5163e-02); fAr.Add(fn);
  fn = new TF1("f41p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.7566e+01, +6.0000e-01, +2.0624e+00, +2.1385e+03, 
                    +6.3630e+03, +2.7497e+02); fAr.Add(fn);
  fn = new TF1("f41p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1163e+00, +6.0000e-01, +1.4848e+00, -4.6851e+01, 
                    -1.5462e+02, -6.5604e+00); fAr.Add(fn);
  fn = new TF1("f41p2",P31, 0, 0.7, 6);
  fn->SetParameters(+6.8701e-01, +6.0000e-01, +1.0422e+01, -7.9777e+00, 
                    -9.2265e+01, -8.4842e+00); fAr.Add(fn);
  fn = new TF1("f41p3",P31, 0, 0.7, 6);
  fn->SetParameters(+6.3966e-02, +6.0000e-01, -5.0753e+00, +1.3839e+00, 
                    +4.9869e+01, +4.4394e+00); fAr.Add(fn);
  fn = new TF1("f41p4",P31, 0, 0.7, 6);
  fn->SetParameters(-3.1785e-02, +6.0000e-01, +9.3504e-01, -2.6546e-01, 
                    -1.1229e+01, -8.8089e-01); fAr.Add(fn);
  fn = new TF1("f51p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.1377e+01, +6.0000e-01, +4.0714e+01, +2.3212e+03, 
                    +6.3686e+03, -3.5328e+01); fAr.Add(fn);
  fn = new TF1("f51p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.3924e-01, +6.0000e-01, +7.2329e-02, -4.8610e+01, 
                    -1.4434e+02, +5.8411e+00); fAr.Add(fn);
  fn = new TF1("f51p2",P31, 0, 0.7, 6);
  fn->SetParameters(+5.3327e-01, +6.0000e-01, +7.6323e+00, -2.7013e+01, 
                    -1.2840e+02, +5.2253e+00); fAr.Add(fn);
  fn = new TF1("f51p3",P31, 0, 0.7, 6);
  fn->SetParameters(+1.2168e-01, +6.0000e-01, -3.5523e+00, +1.4665e+01, 
                    +7.7447e+01, -2.0398e+00); fAr.Add(fn);
  fn = new TF1("f51p4",P31, 0, 0.7, 6);
  fn->SetParameters(-4.2402e-02, +6.0000e-01, +6.0739e-01, -3.3957e+00, 
                    -1.7707e+01, +1.6823e-01); fAr.Add(fn);
  fn = new TF1("f61p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.0394e+01, +6.0000e-01, -1.1117e+02, +7.2475e+02, 
                    +1.9938e+03, +1.2092e+02); fAr.Add(fn);
  fn = new TF1("f61p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0114e+00, +6.0000e-01, +3.8453e+00, -1.1781e+01, 
                    -4.9299e+01, -4.2086e-01); fAr.Add(fn);
  fn = new TF1("f61p2",P31, 0, 0.7, 6);
  fn->SetParameters(+6.9242e-01, +6.0000e-01, +1.3790e+01, +2.8130e+01, 
                    +1.0337e+01, -8.2541e-01); fAr.Add(fn);
  fn = new TF1("f61p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.3669e-02, +6.0000e-01, -6.7338e+00, -1.8510e+01, 
                    -1.3151e+01, +6.3780e-01); fAr.Add(fn);
  fn = new TF1("f61p4",P31, 0, 0.7, 6);
  fn->SetParameters(-2.8059e-02, +6.0000e-01, +1.2382e+00, +3.9065e+00, 
                    +3.1885e+00, +1.4720e-02); fAr.Add(fn);
  fn = new TF1("f13p0","pol0"); fn->SetParameter(0, +1.3083e+00); fAr.Add(fn);
  fn = new TF1("f13p1","pol0"); fn->SetParameter(0, +1.0164e+00); fAr.Add(fn);
  fn = new TF1("f13p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f13p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f23p0","pol0"); fn->SetParameter(0, +1.3106e+00); fAr.Add(fn);
  fn = new TF1("f23p1","pol0"); fn->SetParameter(0, +9.8840e-01); fAr.Add(fn);
  fn = new TF1("f23p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f23p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f33p0","pol0"); fn->SetParameter(0, +1.3093e+00); fAr.Add(fn);
  fn = new TF1("f33p1","pol0"); fn->SetParameter(0, +9.7351e-01); fAr.Add(fn);
  fn = new TF1("f33p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f33p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f43p0","pol0"); fn->SetParameter(0, +1.2786e+00); fAr.Add(fn);
  fn = new TF1("f43p1","pol0"); fn->SetParameter(0, +9.8329e-01); fAr.Add(fn);
  fn = new TF1("f43p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f43p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f53p0","pol0"); fn->SetParameter(0, +1.2395e+00); fAr.Add(fn);
  fn = new TF1("f53p1","pol0"); fn->SetParameter(0, +9.8874e-01); fAr.Add(fn);
  fn = new TF1("f53p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f53p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f63p0","pol0"); fn->SetParameter(0, +1.1673e+00); fAr.Add(fn);
  fn = new TF1("f63p1","pol0"); fn->SetParameter(0, +9.9796e-01); fAr.Add(fn);
  fn = new TF1("f63p2","pol0"); fn->SetParameter(0, +9.0000e-01); fAr.Add(fn);
  fn = new TF1("f63p3","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.5655e+00, +6.0000e-01, -2.8555e+00, +5.7569e+00, 
                    +1.2500e+02, -4.7113e-01); fAr.Add(fn);
  fn = new TF1("f12p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5411e+00, +6.0000e-01, -9.4667e-01, -5.2410e+00, 
                    -7.1647e+01, +8.3107e-02); fAr.Add(fn);
  fn = new TF1("f12p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f12p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f12p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1171e+00, +6.0000e-01, -2.7060e-01, -5.2234e+00, 
                    +7.1369e+00, +1.2753e-01); fAr.Add(fn);
  fn = new TF1("f12p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f12p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f22p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.5533e+00, +6.0000e-01, -2.8326e+00, +6.9315e+00, 
                    +1.3223e+02, -5.5561e-01); fAr.Add(fn);
  fn = new TF1("f22p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5367e+00, +6.0000e-01, -1.4278e+00, -1.2203e+01, 
                    -9.3580e+01, +5.3068e-01); fAr.Add(fn);
  fn = new TF1("f22p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f22p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f22p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1193e+00, +6.0000e-01, +2.6931e-02, -7.3015e-01, 
                    +2.0441e+01, -2.4017e-01); fAr.Add(fn);
  fn = new TF1("f22p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f22p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f32p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.5214e+00, +6.0000e-01, -2.4858e+00, +6.3766e+00, 
                    +1.1855e+02, -2.0673e+00); fAr.Add(fn);
  fn = new TF1("f32p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5439e+00, +6.0000e-01, -1.6473e+00, -1.2848e+01, 
                    -8.8298e+01, +1.1932e+00); fAr.Add(fn);
  fn = new TF1("f32p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f32p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f32p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.1129e+00, +6.0000e-01, +3.4214e-01, +1.9163e+00, 
                    +2.3756e+01, -1.1264e+00); fAr.Add(fn);
  fn = new TF1("f32p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f32p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f42p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.4859e+00, +6.0000e-01, -3.0202e+00, -1.0487e+01, 
                    +4.2501e+01, -1.1882e-01); fAr.Add(fn);
  fn = new TF1("f42p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5567e+00, +6.0000e-01, -4.2682e-01, +7.7593e+00, 
                    -1.1651e+01, -9.2760e-02); fAr.Add(fn);
  fn = new TF1("f42p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f42p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f42p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0954e+00, +6.0000e-01, -9.5898e-02, -5.4825e+00, 
                    -4.2858e+00, +1.3643e-01); fAr.Add(fn);
  fn = new TF1("f42p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f42p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f52p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.4848e+00, +6.0000e-01, -2.7715e+00, -1.2248e+01, 
                    +2.5083e+01, -1.3077e+00); fAr.Add(fn);
  fn = new TF1("f52p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.5393e+00, +6.0000e-01, -5.4742e-01, +8.3214e+00, 
                    -2.1616e+00, +4.4970e-01); fAr.Add(fn);
  fn = new TF1("f52p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f52p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f52p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0991e+00, +6.0000e-01, +2.5952e-02, -5.5083e+00, 
                    -7.9289e+00, -9.1390e-01); fAr.Add(fn);
  fn = new TF1("f52p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f52p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f62p0",P31, 0, 0.7, 6);
  fn->SetParameters(+2.5784e+00, +6.0000e-01, -3.4269e+00, -2.2391e+01, 
                    -1.2172e+00, -1.5739e+00); fAr.Add(fn);
  fn = new TF1("f62p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.4985e+00, +6.0000e-01, -1.2213e+00, +4.5732e+00, 
                    -9.5047e+00, -1.5408e-01); fAr.Add(fn);
  fn = new TF1("f62p2","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p3","pol0"); fn->SetParameter(0, +1.3000e+00); fAr.Add(fn);
  fn = new TF1("f62p4","pol0"); fn->SetParameter(0, +1.0000e+00); fAr.Add(fn);
  fn = new TF1("f62p5",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0993e+00, +6.0000e-01, +6.7852e-01, -6.9923e-01, 
                    +2.1976e+00, +4.9675e-01); fAr.Add(fn);
  fn = new TF1("f62p6","pol0"); fn->SetParameter(0, +1.8000e+00); fAr.Add(fn);
  fn = new TF1("f62p7","pol0"); fn->SetParameter(0, +8.0000e-01); fAr.Add(fn);
  fn = new TF1("f16p0","pol0"); fn->SetParameter(0, +1.3504e+00); fAr.Add(fn);
  fn = new TF1("f16p1","pol0"); fn->SetParameter(0, +1.4349e+00); fAr.Add(fn);
  fn = new TF1("f16p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f16p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f17p0","pol0"); fn->SetParameter(0, +1.7014e+00); fAr.Add(fn);
  fn = new TF1("f17p1","pol0"); fn->SetParameter(0, +1.7409e+00); fAr.Add(fn);
  fn = new TF1("f17p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f17p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f26p0","pol0"); fn->SetParameter(0, +1.3558e+00); fAr.Add(fn);
  fn = new TF1("f26p1","pol0"); fn->SetParameter(0, +1.4269e+00); fAr.Add(fn);
  fn = new TF1("f26p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f26p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f27p0","pol0"); fn->SetParameter(0, +1.6931e+00); fAr.Add(fn);
  fn = new TF1("f27p1","pol0"); fn->SetParameter(0, +1.7435e+00); fAr.Add(fn);
  fn = new TF1("f27p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f27p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f36p0","pol0"); fn->SetParameter(0, +1.3548e+00); fAr.Add(fn);
  fn = new TF1("f36p1","pol0"); fn->SetParameter(0, +1.4206e+00); fAr.Add(fn);
  fn = new TF1("f36p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f36p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f37p0","pol0"); fn->SetParameter(0, +1.6788e+00); fAr.Add(fn);
  fn = new TF1("f37p1","pol0"); fn->SetParameter(0, +1.7424e+00); fAr.Add(fn);
  fn = new TF1("f37p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f37p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f46p0","pol0"); fn->SetParameter(0, +1.3551e+00); fAr.Add(fn);
  fn = new TF1("f46p1","pol0"); fn->SetParameter(0, +1.4177e+00); fAr.Add(fn);
  fn = new TF1("f46p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f46p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f47p0","pol0"); fn->SetParameter(0, +1.6701e+00); fAr.Add(fn);
  fn = new TF1("f47p1","pol0"); fn->SetParameter(0, +1.7336e+00); fAr.Add(fn);
  fn = new TF1("f47p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f47p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f56p0","pol0"); fn->SetParameter(0, +1.3553e+00); fAr.Add(fn);
  fn = new TF1("f56p1","pol0"); fn->SetParameter(0, +1.4146e+00); fAr.Add(fn);
  fn = new TF1("f56p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f56p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f57p0","pol0"); fn->SetParameter(0, +1.6663e+00); fAr.Add(fn);
  fn = new TF1("f57p1","pol0"); fn->SetParameter(0, +1.7275e+00); fAr.Add(fn);
  fn = new TF1("f57p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f57p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f66p0","pol0"); fn->SetParameter(0, +1.3411e+00); fAr.Add(fn);
  fn = new TF1("f66p1","pol0"); fn->SetParameter(0, +1.4156e+00); fAr.Add(fn);
  fn = new TF1("f66p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f66p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f67p0","pol0"); fn->SetParameter(0, +1.6336e+00); fAr.Add(fn);
  fn = new TF1("f67p1","pol0"); fn->SetParameter(0, +1.6936e+00); fAr.Add(fn);
  fn = new TF1("f67p2","pol0"); fn->SetParameter(0, +3.7000e+00); fAr.Add(fn);
  fn = new TF1("f67p3","pol0"); fn->SetParameter(0, +2.0000e-01); fAr.Add(fn);
  fn = new TF1("f15p0",P31, 0, 0.7, 6);
  fn->SetParameters(+6.0418e+02, +6.0000e-01, -1.8066e+02, +1.8046e+03, 
                    +3.1193e+03, +6.5055e+02); fAr.Add(fn);
  fn = new TF1("f15p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.9080e+02, +6.0000e-01, +7.0419e+02, -2.2652e+03, 
                    -6.5759e+03, +1.2407e+03); fAr.Add(fn);
  fn = new TF1("f15p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f15p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f25p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.8485e+02, +6.0000e-01, -4.4338e+02, -1.2407e+03, 
                    -5.5813e+03, +8.2657e+02); fAr.Add(fn);
  fn = new TF1("f25p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.6540e+02, +6.0000e-01, +7.6131e+02, -6.7130e+02, 
                    -1.5348e+03, +9.9250e+02); fAr.Add(fn);
  fn = new TF1("f25p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f25p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f35p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.7709e+02, +6.0000e-01, -1.4927e+02, +9.3445e+02, 
                    +2.1842e+02, +8.1513e+02); fAr.Add(fn);
  fn = new TF1("f35p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.3193e+02, +6.0000e-01, +5.4387e+02, -1.4850e+03, 
                    -3.5516e+03, +9.6279e+02); fAr.Add(fn);
  fn = new TF1("f35p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f35p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f45p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.5670e+02, +6.0000e-01, -1.7093e+02, +3.6322e+02, 
                    -1.4231e+03, +6.1182e+02); fAr.Add(fn);
  fn = new TF1("f45p1",P31, 0, 0.7, 6);
  fn->SetParameters(+7.0354e+02, +6.0000e-01, +4.9376e+02, -1.2542e+03, 
                    -2.6892e+03, +9.9216e+02); fAr.Add(fn);
  fn = new TF1("f45p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f45p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f55p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.3371e+02, +6.0000e-01, -1.6869e+02, +3.2715e+02, 
                    -1.4148e+03, +9.1701e+02); fAr.Add(fn);
  fn = new TF1("f55p1",P31, 0, 0.7, 6);
  fn->SetParameters(+6.7336e+02, +6.0000e-01, +4.7575e+02, -1.2183e+03, 
                    -2.3581e+03, +8.8451e+02); fAr.Add(fn);
  fn = new TF1("f55p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f55p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f65p0",P31, 0, 0.7, 6);
  fn->SetParameters(+5.0484e+02, +6.0000e-01, -5.0796e+02, -1.4396e+03, 
                    -4.7452e+03, +1.0268e+03); fAr.Add(fn);
  fn = new TF1("f65p1",P31, 0, 0.7, 6);
  fn->SetParameters(+6.5403e+02, +6.0000e-01, +6.8875e+02, -1.3300e+03, 
                    -2.1396e+03, +1.0801e+03); fAr.Add(fn);
  fn = new TF1("f65p2","pol0"); fn->SetParameter(0, +3.5000e+00); fAr.Add(fn);
  fn = new TF1("f65p3","pol0"); fn->SetParameter(0, +7.0000e-01); fAr.Add(fn);
  fn = new TF1("f14p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f14p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.2236e-01, +6.0000e-01, +1.0760e+00, -1.8120e+00, 
                    -5.6732e+00, -2.5964e-01); fAr.Add(fn);
  fn = new TF1("f14p2","pol1");
  fn->SetParameters(+2.9361e+00, +6.2693e-01); fAr.Add(fn);
  fn = new TF1("f14p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.9939e-01, +6.0000e-01, +8.3424e-01, +1.7372e+00, 
                    +3.8075e+00, -2.8028e+00); fAr.Add(fn);
  fn = new TF1("f24p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f24p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.4609e-01, +6.0000e-01, +9.5743e-01, -2.7537e+00, 
                    -7.3959e+00, +3.9399e-01); fAr.Add(fn);
  fn = new TF1("f24p2","pol1");
  fn->SetParameters(+3.0760e+00, +3.1050e-01); fAr.Add(fn);
  fn = new TF1("f24p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.7370e-01, +6.0000e-01, +5.1349e-01, +7.0445e-01, 
                    +3.0957e+00, +2.4534e-01); fAr.Add(fn);
  fn = new TF1("f34p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f34p1",P31, 0, 0.7, 6);
  fn->SetParameters(+9.8987e-01, +6.0000e-01, +8.2108e-01, -4.6526e+00, 
                    -1.2468e+01, +9.4514e-01); fAr.Add(fn);
  fn = new TF1("f34p2","pol1");
  fn->SetParameters(+3.0080e+00, +4.2850e-01); fAr.Add(fn);
  fn = new TF1("f34p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.6199e-01, +6.0000e-01, -3.4987e-01, -7.4858e+00, 
                    -1.7399e+01, +8.3638e-01); fAr.Add(fn);
  fn = new TF1("f44p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f44p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0217e+00, +6.0000e-01, +9.8757e-01, -3.8249e+00, 
                    -1.0843e+01, +6.8282e-01); fAr.Add(fn);
  fn = new TF1("f44p2","pol1");
  fn->SetParameters(+2.8477e+00, +7.1857e-01); fAr.Add(fn);
  fn = new TF1("f44p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.6421e-01, +6.0000e-01, +1.3044e-01, -2.6946e+00, 
                    -1.6166e-01, -9.6132e-01); fAr.Add(fn);
  fn = new TF1("f54p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f54p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0532e+00, +6.0000e-01, +1.2598e+00, -1.3881e+00, 
                    -4.0438e+00, +2.6725e-01); fAr.Add(fn);
  fn = new TF1("f54p2","pol1");
  fn->SetParameters(+2.9115e+00, +6.7151e-01); fAr.Add(fn);
  fn = new TF1("f54p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.7701e-01, +6.0000e-01, +8.3465e-01, +5.8030e+00, 
                    +2.0655e+01, -9.5583e-01); fAr.Add(fn);
  fn = new TF1("f64p0","pol0"); fn->SetParameter(0, +0.0000e+00); fAr.Add(fn);
  fn = new TF1("f64p1",P31, 0, 0.7, 6);
  fn->SetParameters(+1.0445e+00, +6.0000e-01, +1.6798e+00, -4.0619e+00, 
                    -1.5155e+01, +1.2745e+00); fAr.Add(fn);
  fn = new TF1("f64p2","pol1");
  fn->SetParameters(+2.6106e+00, +1.1681e+00); fAr.Add(fn);
  fn = new TF1("f64p3",P31, 0, 0.7, 6);
  fn->SetParameters(+3.7440e-01, +6.0000e-01, -1.9590e-01, -9.8285e+00, 
                    -2.4287e+01, -1.7686e+00); fAr.Add(fn);
 }
}
