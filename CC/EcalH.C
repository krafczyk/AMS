//  $Id: EcalH.C,v 1.8.2.1 2013/12/24 11:41:36 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  EcalH.C
///\brief Source file of EcalHR class
///
///\date  2013/11/06 SH  Introduction of the class
///\date  2013/11/08 SH  Methods implemented
///\date  2013/11/10 SH  Parameters added
///
///$Date: 2013/12/24 11:41:36 $
///
///$Revision: 1.8.2.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "root.h"
#include "EcalH.h"

#include "TF1.h"
#include "TMath.h"
#include "TMinuit.h"

#ifndef __ROOTSHAREDLIBRARY__
#include "commons.h"
#include "ecalrec.h"
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

ClassImp(EcalHR)

float EcalHR::fZref = -151;
float EcalHR::fXmin =  -32.270;
float EcalHR::fXmax =   32.530;
float EcalHR::fYmin =  -32.470;
float EcalHR::fYmax =   32.330;
float EcalHR::fZtop = -142.732;
float EcalHR::fZbot = -159.382;
float EcalHR::fCell =    0.9;

float EcalHR::fEmin = 1;
float EcalHR::fEthd[3] = { 50, 100, 300 };

EcalHR::EcalHR()
{
  Clear();
}

EcalHR::EcalHR(const EcalHR *org)
{
  *this = *org;
}

EcalHR::~EcalHR()
{
}

void EcalHR::Clear(int mode)
{
  _hid.clear();
  _hxy.clear();
  _hz .clear();
  _de .clear();
  _sc .clear();
  _ac .clear();
  if (mode != 1) return;

  _tpnt.setp(0, 0, 0);
  _tdir.setp(0, 0, 0);
  _tcsq = 0;

  _nhit = 0;
  for (int i = 0; i < 5*NL; i++) _hidx[i/5][i%5] = -1;
  for (int i = 0; i < 3; i++)
    _apex [i] = _ecen[i] = _elast[i] = 
    _lapex[i] = _lmax[i] = _ltop [i] = _lsum[i] = 0;
  _etot = 0;
}


float EcalHR::GetChisq(float sigma, float emin, int norm) const
{
  float chisq = 0;
  int    nlay = 0;

  sigma *= fCell;
  if (sigma == 0) return -1;

  for (int i = 0; i < NL; i++) {
    if (Ecell(i, 2) > emin) {
      float dx = dXt(Hidx(i, 2));
      chisq += dx*dx/sigma/sigma;
      nlay++;
    }
  }

  if (norm && nlay > 0) chisq /= norm;
  return chisq;
}

namespace EcalH {
  static TMinuit *mnt = 0;
  static TF1    *func = 0;
  static float ecen[EcalHR::NL];

#ifdef _OPENMP
#pragma omp threadprivate(mnt)
#pragma omp threadprivate(func)
#pragma omp threadprivate(ecen)
#endif

  void FCN(int &n, double *grad, double &chisq, double *par, int iflag)
  {
    if (iflag == 2) {
      grad[0] = grad[1] = grad[2] = 0;
      return;
    }

    if (par[2] == 0) {
      chisq = 1e9;
      return;
    }

    chisq = 0;
    func->SetParameters(par[0], par[1], par[2]);

    for (int i = 0; i < EcalHR::NL; i++) {
      if (ecen[i] > 0 && i > par[1]) {
	double y = func->Eval(i);
	double e = ecen[i]/TMath::Sqrt(ecen[i]/10)+10;
	double d = ecen[i]-y;
	chisq += d*d/e/e;
      }
    }
  }
};


int EcalHR::Process(void)
{
  Clear();

#ifndef __ROOTSHAREDLIBRARY__
  for (int i = 0; i < NL; i++) {
    AMSEcalHit *hit
      = (AMSEcalHit *)AMSEvent::gethead()->getheadC("AMSEcalHit", i, 1);

    while (hit) {
      float edep = (hit->getedep() == hit->getedep()) ? hit->getedep() : 0;
      _hid.push_back(Hid(hit->getplane(), hit->getcell(), hit->getproj()));
      _hxy.push_back(hit->getcool());
      _hz .push_back(hit->getcooz());
      _de .push_back(edep);
      _sc .push_back(hit->getedepc());
      _ac .push_back(hit->getattcor());
      _nhit++;
      hit = hit->next();
    }
  }

#else
  AMSEventR *evt = AMSEventR::Head();
  for (int i = 0; evt && i < evt->nEcalHit(); i++) {
    EcalHitR *hit = evt->pEcalHit(i);
    if (!hit) continue;

    float edep = (hit->Edep == hit->Edep) ? hit->Edep : 0;
    AMSPoint coo(hit->Coo[0], hit->Coo[1], hit->Coo[2]);
    _hid.push_back(Hid(hit->Plane, hit->Cell, hit->Proj));
    _hxy.push_back((hit->Proj == 0) ? coo.x() : coo.y());
    _hz .push_back(coo.z());
    _de .push_back(edep);
    _sc .push_back(hit->EdCorr);
    _ac .push_back(hit->AttCor);
    _nhit++;
  }
#endif

  _etot = 0;
  for (int i = 0; i < _nhit; i++) _etot += dE(i);

  _apex[0] = _apex[1] = _apex[2] = -1;

  for (int i = 0; i < NL; i++) {
    float emax =  0;
    int   cmax = -1;
    for (int j = 0; j < _nhit; j++)
      if (Plane(j) == i && dE(j) > emax) { emax = dE(j); cmax = Cell(j); }

    bool ll = (i == NL-2 || i == NL-1);
    bool ap = (_apex[1] >= 0);
             _ecen[0] += emax;
    if (ap)  _ecap[0] += emax;
    if (ll) _elast[0] += emax;

    float es3 = 0;
    for (int j = 0; cmax >= 0 && j < _nhit; j++) {
      int jj = Cell(j)-cmax;
      if (Plane(j) == i && -2 <= jj && jj <= 2) {
	_hidx[i][jj+2] = j;

	if (-1 <= jj && jj <= 1) {
	            es3     += dE(j);
	           _ecen[1] += dE(j);
	  if (ap)  _ecap[1] += emax;
	  if (ll) _elast[1] += dE(j);
	}
	           _ecen[2] += dE(j);
	  if (ap)  _ecap[2] += emax;
	  if (ll) _elast[2] += dE(j);
      }
    }
    for (int j = 0; j < 3; j++)
      if (_apex[j] < 0 && es3 > fEthd[j]) _apex[j] = i;
  }

#ifdef _PGTRACK_
  VCon *cont = GetVCon()->GetCont("AMSTrTrack");
  if (!cont) return -1;

  float    cmin = 0;
  AMSPoint pmin;
  AMSDir   dmin;

  int ntrk = cont->getnelem();
  for (int i = 0; i < ntrk; i++) {
    TrTrackR *trk = dynamic_cast<TrTrackR *>(cont->getelem(i));
    if (!trk) continue;

    AMSPoint pnt;
    AMSDir   dir;
    trk->Interpolate(fZref, pnt, dir);
    if (IsInside(pnt, dir)) {
      _tpnt = pnt;
      _tdir = dir;

      float csq = GetChisq(1, 100, 1);
      if (cmin == 0 || csq < cmin) {
	cmin = csq;
	pmin = pnt;
	dmin = dir;
      }
    }
  }
  delete cont;

  _tpnt = pmin;
  _tdir = dmin;
  _tcsq = cmin;
#endif

  for (int s = 0; s < 3; s++) {
    if (_ecen[s] < fEmin*1e3) continue;

    float par[3], err[3];
    if (FitL(s, par, err) == 0 && EcalH::func) {
      EcalH::func->SetParameters(par[0], par[1], par[2]);
      _lapex[s] = par[1];
      _lmax [s] = par[1]+par[2];
      _ltop [s] = par[0];
      _lsum [s] = EcalH::func->Integral(par[1], NL*5);
    }
  }

  return _nhit;
}

#ifdef __ROOTSHAREDLIBRARY__
#include "TH1.h"
#endif

int EcalHR::FillS(int s, float es[EcalHR::NL])
{
  int   imax = -1;
  float emax =  0;
  for (int i = 0; i < NL; i++) {
    float ec = 0;
    for (int j = -s; j <= s; j++) ec += Ecell(i, j+2);

    if (ec > emax) { imax = i; emax = ec; }
    es[i] = ec;
  }
  return imax;
}

int EcalHR::FitL(int s, float par[3], float err[3], int method)
{
  float *ec = EcalH::ecen;
  int  imax = FillS(s, ec);
  if (imax < 0 || NL <= imax) return -1;

  int im = imax;
  if (im ==    0) im = 1;
  if (im == NL-1) im = NL-2;

  float emax = ec[imax];
  float xmax = Peak(im-1, ec[im-1], im, ec[im], im+1, ec[im+1]);

  if (xmax > 25) xmax = 25;
  if (xmax <  0) xmax =  0;

  if (!EcalH::func) EcalH::func = Lfun(1, 0, 1);

#ifdef __ROOTSHAREDLIBRARY__
 if (method == 3) {
  static TH1F *htmp = 0;
  if (!htmp) htmp = new TH1F("htmp", "Lfit", NL, -0.5, NL-0.5);

  for (int i = 0; i < NL; i++) {
    float e = ec[i];
    if (e > 0) {
      htmp->SetBinContent(i+1, e);
      htmp->SetBinError  (i+1, e/TMath::Sqrt(e/10)+10);
    }
  }
  EcalH::func->SetParameters(emax, xmax-10, 4);
  EcalH::func->SetParLimits(0,   0, 1e6);
  EcalH::func->SetParLimits(1, -10,  15);
  EcalH::func->SetParLimits(2,   1,  30);

  htmp->Fit(EcalH::func, "q0");

  for (int i = 0; i < 3; i++) {
    par[i] = EcalH::func->GetParameter(i);
    err[i] = EcalH::func->GetParError (i);
  }

  return 0;
 }
#endif

  if (!EcalH::mnt) {
    int thr = 0;
#ifdef _OPENMP
#pragma omp critical (tminuit)
    thr = omp_get_thread_num();
#endif
    EcalH::mnt = new TMinuit(3);
    cout << "EcalHR::Process-I-TMinuit object created for thread "
	 << thr << endl;
  }

  TMinuit *mnt = EcalH::mnt;
  mnt->SetFCN(EcalH::FCN);
  mnt->SetPrintLevel(-1);

  int ierr = 0;
  mnt->mnparm(0, "par0", emax, emax*0.1,   0, 1e6, ierr);
  mnt->mnparm(1, "par1", xmax-10,   0.1, -10,  15, ierr);
  mnt->mnparm(2, "par2", 10,        0.1,   1,  15, ierr);

  int ret = mnt->Migrad();
  if (ret != 0 && ret != 4) return -1;

  for (int i = 0; i < 3; i++) {
    double p = 0, e = 0;
    mnt->GetParameter(i, p, e);
    if (par) par[i] = p;
    if (err) err[i] = e;
  }

  return 0;
}

bool EcalHR::IsInside(AMSPoint pnt, AMSDir dir)
{
  AMSPoint pntt = pnt+dir/dir.z()*(fZtop-pnt.z());
  AMSPoint pntb = pnt+dir/dir.z()*(fZbot-pnt.z());

  return (fXmin < pntt.x() && pntt.x() < fXmax &&
	  fYmin < pntt.y() && pntt.y() < fYmax &&
	  fXmin < pntb.x() && pntb.x() < fXmax &&
	  fYmin < pntb.y() && pntb.y() < fYmax);
}

bool EcalHR::IsInside(TrTrackR *track)
{
  AMSPoint pnt;
  AMSDir   dir;
#ifdef _PGTRACK_
  track->Interpolate(fZref, pnt, dir);
#endif
  return IsInside(pnt, dir);
}

int EcalHR::GetN(void)
{
  VCon *cont = GetVCon()->GetCont("AMSEcalH");
  if (!cont) return -1;

  int n = cont->getnelem(); delete cont;
  return n;
}

EcalHR *EcalHR::Get(int i)
{
  VCon *cont = GetVCon()->GetCont("AMSEcalH");
  if (!cont) return 0;

  EcalHR *ech = dynamic_cast<EcalHR *>(cont->getelem(i)); delete cont;
  return ech;
}

#ifdef _PGTRACK_
extern double memchk(void);
#endif

int EcalHR::Build(int clear)
{
#ifndef __ROOTSHAREDLIBRARY__
#ifdef _PGTRACK_
  if (ECALHFFKEY.enable/10 == 1) {
    int evid = AMSEvent::gethead()->getEvent();
    if (evid%1000 == 0)
      cout << Form("EcalHR::Build-Memory check: %8d %6.1f",
		   evid, memchk()/1024) << endl;
  }
#endif
  if (ECALHFFKEY.enable%10 == 0) return 0;
#endif

  VCon *cont = GetVCon()->GetCont("AMSEcalH");
  if (!cont) return -1;

  if (clear) cont->eraseC();

  static bool first = true;
  if (first) {
#ifdef _OPENMP
#pragma omp critical (trrecsimple)
#endif
  {if (first) {
#ifndef __ROOTSHAREDLIBRARY__
    fEmin    = ECALHFFKEY.emin;
    fEthd[0] = ECALHFFKEY.ethd[0];
    fEthd[1] = ECALHFFKEY.ethd[1];
    fEthd[2] = ECALHFFKEY.ethd[2];
#endif
    cout << "EcalHR::Build-I-"
	 << "fEmin= " << fEmin << " "
	 << "fEthd= " << fEthd[0] << " " << fEthd[1] << " "
	                                 << fEthd[2] << " " << endl;
    first = false;
   }
  }}

#ifndef __ROOTSHAREDLIBRARY__
  AMSEcalH *ech = new AMSEcalH;
#else
  EcalHR   *ech = new EcalHR;
#endif

  if (ech->Process() < 0) {
    delete ech;
    return -1;
  }

#ifndef __ROOTSHAREDLIBRARY__
  if (ECALHFFKEY.enable%10 <  2) ech->EcalHR::Clear(0);
#endif

  cont->addnext(ech);

  return 1;
}

TF1 *EcalHR::Lfun(float norm, float apex, float lmax)
{
  TF1 *func = new TF1("func", "(sqrt((x-[1])^2)+x-[1])/2"
		              "*[0]/[2]/1200*((x-[1])*10.4/[2])^6"
		              "*exp(-0.67*((x-[1])*10.4/[2]))", 0, 17);
  func->SetParameters(norm, apex, lmax);

  return func;
}

double EcalHR::Peak(double x1, double y1, double x2, double y2,
		    double x3, double y3)
{
  double m[4] = { x1-x2, x1*x1-x2*x2,
		  x2-x3, x2*x2-x3*x3 };
  double d = m[0]*m[3]-m[1]*m[2];

  if (d == 0) return 0;

  double n[4] = { m[3]/d, -m[1]/d, -m[2]/d, m[0]/d };
  double v[2] = { y1-y2, y2-y3 };

  double p1 = n[0]*v[0]+n[1]*v[1];
  double p2 = n[2]*v[0]+n[3]*v[1];

  return (p2 != 0) ? -p1/p2/2 : 0;
}

void EcalHR::_PrepareOutput(int opt)
{
  sout.clear();
  sout.append(Form("EcalHR nhit= %d E(S1,3,5)= %.2f %.2f %.2f Chisq= %.2f",
		   Nhit(), Ecen(0)*1e-3, Ecen(1)*1e-3, Ecen(2)*1e-3, Tcsq()));

  if (opt) {
    sout.append("\n");
    for (int i = 0; i < _nhit; i++)
      sout.append(Form("Hit%02d %2d-%2d %5.1f %6.1f %6.1f\n", i,
		       Plane(i), Cell(i), Hxy(i), Hz(i), dE(i)));
		       
  }
}

void EcalHR::Print(int opt)
{
  _PrepareOutput(opt);
  cout << sout << endl;
}

const char *EcalHR::Info(int iref)
{
  string str;
  str.append(Form("EcalH%d ", iref));
  _PrepareOutput(0);
  str.append(sout);

  int len = MAXINFOSIZE;
  if (str.size() < len) len = str.size();
  strncpy(_Info, str.c_str(), len+1);

  return _Info;
}

std::ostream &EcalHR::putout(std::ostream &ostr)
{
  _PrepareOutput();
  return ostr << sout << std::endl; 
}

#ifndef __ROOTSHAREDLIBRARY__

#include "job.h"
#include "ntuple.h"

void AMSEcalH::_copyEl(void)
{
#ifdef __WRITEROOT__
#endif
}

void AMSEcalH::_writeEl(void)
{
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}

#endif
