
#include "root.h"
#include "EcalH.h"

ClassImp(EcalHR)

double EcalHR::fZref = -143.2;
double EcalHR::fXmin =  -32.270;
double EcalHR::fXmax =   32.530;
double EcalHR::fYmin =  -32.470;
double EcalHR::fYmax =   32.330;
double EcalHR::fZtop = -142.732;
double EcalHR::fZbot = -159.382;

bool EcalHR::IsInside(void) const
{
  return (fXmin < TrkX(fZtop) && TrkX(fZtop) < fXmax &&
	  fYmin < TrkY(fZtop) && TrkY(fZtop) < fXmax &&
	  fXmin < TrkX(fZbot) && TrkX(fZbot) < fXmax &&
	  fYmin < TrkY(fZbot) && TrkY(fZbot) < fXmax);
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

  _nhit = 0;
  for (int i = 0; i < 5*NL; i++) _hidx[i/NL][i%NL] = 0;
  for (int i = 0; i < 3; i++)
    _apex[i] = _ecen[i] = _elast[i] = _lmax[i] = 0;
}

int EcalHR::Process(void)
{
  Clear();

  

  return 0;
}

int EcalHR::Build(void)
{
  VCon *cont = GetVCon()->GetCont("AMSEcalH");
  if (!cont) return -1;

#ifndef __ROOTSHAREDLIBRARY__
  AMSEcalH *ech = new AMSEcalH;
#else
  EcalHR   *ech = new EcalHR;
#endif

  int ret = ech->Process();
  if (ret) {
    delete ech;
    return ret;
  }
  cont->addnext(ech);

  return 0;
}

TF1 *EcalHR::Lfun(double norm, double apex, double ldep)
{
  TF1 *func = new TF1("func", "(sqrt((x-[1])^2)+x-[1])/2"
		              "*[0]/[2]/1200*((x-[1])*10.4/[2])^6"
		              "*exp(-0.67*((x-[1])*10.4/[2]))", 0, 17);
  func->SetParameters(norm, apex, ldep);

  return func;
}

void EcalHR::_PrepareOutput(int opt)
{
  sout.clear();
  sout.append("EcalHR info");
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
