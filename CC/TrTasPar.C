// $Id: TrTasPar.C,v 1.2 2009/12/21 20:46:57 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasPar.C
///\brief Source file of TrTasPar class
///
///\date  2009/12/10 SH  First version
///\date  2009/12/17 SH  First Gbatch version
///$Date: 2009/12/21 20:46:57 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrTasPar.h"
#include "TkDBc.h"
#include "TkCoo.h"
#include "TString.h"

#include <iostream>
#include <fstream>

ClassImp(TrTasPar);

TrTasPar *TrTasPar::Head = 0;

int TrTasPar::GetIlad(int tkid) const
{
  for (int i = 0; i < NLAD; i++)
    if (GetTkId(i) == tkid) return i;

  return -1;
}

int TrTasPar::GetIpk(int ilad, int addr) const
{
  for (int i = 0; i < NADR; i++)
    if (GetAmin(ilad, i) <= addr && GetAmax(ilad, i) >= addr) return i;

  return -1;
}

AMSPoint TrTasPar::GetLadCoo(int ilad, int iadr) const
{
  int tkid =  GetTkId(ilad);
  int addr = (GetAmin(ilad, iadr)+GetAmax(ilad, iadr))/2;
  if (tkid == 0) return AMSPoint(0, 0, 0);

  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init();
  }

  int   iml = TkCoo::GetMaxMult(tkid, addr);
  double lx = TkCoo::GetLadderLength(tkid)/2;
  double ly = TkDBc::Head->_ssize_active[1]/2;
  if (addr >= 640) lx = TkCoo::GetLocalCoo(tkid, addr, iml);
  else             ly = TkCoo::GetLocalCoo(tkid, addr, iml);

  return TkCoo::GetGlobalA(tkid, lx, ly);
}

AMSPoint TrTasPar::GetLasCoo(int ilas, int ilay) const
{
  if (ilay < 0 || NLAY <= ilay ||
      ilas < 0 || NLAS <= ilas) return AMSPoint(0, 0, 0);

  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init();
  }

  double zlay = TkDBc::Head->GetZlayer(ilay+1);
  return AMSPoint(GetLasx(ilas)+GetLasdx(ilas)*zlay,
		  GetLasy(ilas)+GetLasdy(ilas)*zlay, zlay);
}

int TrTasPar::GetIlas(int ilad, int iadr) const
{
  AMSPoint gcoo = GetLadCoo(ilad, iadr);
  if (gcoo.norm() <= 0) return -1;

  int addr = (GetAmin(ilad, iadr)+GetAmax(ilad, iadr))/2;
  int side = (addr < 640) ? 1 : 0;

  for (int i = 0; i < NLAS; i++) {
    double z = gcoo.z();
    if (side == 0 && fabs(gcoo.x()-GetLasx(i)-GetLasdx(i)*z) < 0.1) return i;
    if (side == 1 && fabs(gcoo.y()-GetLasy(i)-GetLasdy(i)*z) < 0.1) return i;
  }
  return -1;
}

int TrTasPar::GetIlad(int ilas, int ilay) const
{
  AMSPoint pnt = GetLasCoo(ilas, ilay);
  for (int i = 0; i < 2; i++) {
    int ilad = ilay+i*NLAY;
    for (int j = 0; j < 2; j++) {
      AMSPoint gcoo = GetLadCoo(ilad, j);
      if (fabs(pnt.y()-gcoo.y()) < 0.1) return ilad;
    }
  }

  return -1;
}

int TrTasPar::GetIadr(int ilas, int ilay, int side) const
{
  AMSPoint pnt = GetLasCoo(ilas, ilay);
  int ilad = GetIlad(ilas, ilay);

  for (int j = 0; j < 2; j++) {
    int iadr = (side == 0) ? j+2 : j;
    AMSPoint gcoo = GetLadCoo(ilad, iadr);
    if (side == 0 && fabs(pnt.x()-gcoo.x()) < 0.1) return iadr;
    if (side == 1 && fabs(pnt.y()-gcoo.y()) < 0.1) return iadr;
  }

  return -1;
}

int TrTasPar::Init(int ival, int lddr, const char *dname)
{
  _ival = ival;
  _lddr = lddr;

  for (int i = 0; i < NLAD; i++) {
    _tkid[i] = 0;
    for (int j = 0; j < NADR; j++) {
      _amin[i*NADR+j] = _amax[i*NADR+j] = _mask[i*NADR+j] = 0;
      _resm[i*NADR+j] = 0;
      for (int k = 0; k < NPAR; k++) _fpar[(i*NADR+j)*NPAR+k] = 0;
    }
  }
  for (int i = 0; i < NLAS; i++)
    _lasx[i] = _lasy[i] = _lasdx[i] = _lasdy[i] = 0;
  if (!dname) return 0;

  TString sfn = Form("%s/taspar%2d%04x.dat", dname, _ival, _lddr);
  std::ifstream fin(sfn);

  if (!fin) {
    std::cerr << "File not found: " << sfn.Data() << std::endl;
    return -1;
  }

  int tkid, ilad, ipk, n = 0;
  while (n < NLAD*NADR && !(fin >> tkid >> ilad >> ipk).eof()) {
    _tkid[ilad] = tkid;
    fin >> _amin[ilad*NADR+ipk] >> _amax[ilad*NADR+ipk];
    for (int i = 0; i < NPAR; i++) fin >> _fpar[(ilad*NADR+ipk)*NPAR+i];
    fin >> _resm[ilad*NADR+ipk];
    _resm[ilad*NADR+ipk] *= 1e-4;
    n++;
  }

  for (int i = 0; i < NLAS && !fin.eof(); i++) fin >> _lasx [i] >> _lasy [i];
  for (int i = 0; i < NLAS && !fin.eof(); i++) fin >> _lasdx[i] >> _lasdy[i];
  for (int i = 0; i < NLAS; i++) { _lasdx[i] *= 1e-3; _lasdy[i] *= 1e-3; }

  std::cout << "Read: " << sfn.Data() << " " << n << std::endl;

  sfn = Form("%s/chmask.dat", dname);
  std::ifstream fin2(sfn);
  n=0;
  while (fin2.good()) {
    int adrs;
    fin2 >> tkid >> adrs;
    if (fin2.eof()) break;

    for (int i = 0; i < NLAD; i++) {
      if (GetTkId(i) != tkid) continue;
      for (int j = 0; j < NADR; j++) SetMask(i, j, adrs);
    }
    n++;
  }

  return 0;
}

void TrTasPar::Print(Option_t *option) const
{
  TString sopt = option;
  sopt.ToLower();

  std::cout << Form("TasPar I= %2dmA LDDR= 0x%04X", _ival, _lddr) << std::endl;

  if (sopt == "rpk") {
    std::cout << "Signal Ranges and saturation flags:" << std::endl;
    for (int i = 0; i < NLAD; i++) {
      std::cout << Form(" %4d :", GetTkId(i));
      for (int j = 0; j < NADR; j++)
	std::cout << Form(" %4d %4d ", 
			  GetAmin(i, j), GetAmax(i, j));
      std::cout << std::endl;
    }
  }

  else if (sopt == "tkp") {
    std::cout << "Fit parameters:" << std::endl;
    for (int l = 0; l < 2; l++)
      for (int j = 0; j < NADR; j++)
	for (int i = 0; i < NLAY; i++)
	  std::cout << Form(" %4d %2d %d %5.0f %6.1f %4.2f %4.0f %5.1f",
			    GetTkId(i+l*NLAY), i+l*NLAY+1, j+1,
			    GetFpar(i+l*NLAY, j, 0), GetFpar(i+l*NLAY, j, 1),
			    GetFpar(i+l*NLAY, j, 2), GetFpar(i+l*NLAY, j, 3),
			    GetFpar(i+l*NLAY, j, 4)) << std::endl;
  }

  else {
    std::cout << "TkIDs:";
    for (int i = 0; i < NLAY; i++) std::cout << Form(" %4d", GetTkId(i));
    std::cout << std::endl << "      ";
    for (int i = 0; i < NLAY; i++) std::cout << Form(" %4d", GetTkId(i+NLAY));
    std::cout << std::endl;
    std::cout << "Laser:";
    for (int i = 0; i < NLAS; i++) 
      std::cout << Form(" (%6.2f,%6.2f)", GetLasx(i), GetLasy(i));
    std::cout << std::endl;
  }
}

void TrTasPar::Output(const char *fname) const
{
  TString sfn;
  if (fname) sfn = fname;
  if (sfn == "" || sfn.EndsWith("/"))
    sfn += Form("taspar%02d%04x.dat", _ival, _lddr);

  std::ofstream fout(sfn);
  if (!fout) {
    std::cerr << "File open error: " << sfn.Data() << std::endl;
    return;
  }

  for (int i = 0; i < NLAY; i++)
    for (int j = 0; j < NLAY; j++) {
      int it = j+(i/4)*NLAY;
      int ip = i%4;
      fout << Form("%4d %2d %d %4d %4d %5.0f %6.1f %4.2f %4.0f %5.1f %6.1f",
		   GetTkId(it), it, ip, GetAmin(it, ip), GetAmax(it, ip),
		   GetFpar(it, ip, 0), GetFpar(it, ip, 1), GetFpar(it, ip, 2),
		   GetFpar(it, ip, 3), GetFpar(it, ip, 4), 
		   GetResm(it, ip)*1e4) << std::endl;
    }

  std::cout << "Output data: " << sfn.Data() << std::endl;
}

int TrTasPar::GetLinearSize(void)
{
  return 2 +NLAD +NLAD*NADR*3 +NLAD*NADR*NPAR +NLAD*NADR +NLAS*4;
}

int TrTasPar::Par2Lin(float *ptr)
{
  if (!ptr) return -1;

  float *psta = ptr;
  *(ptr++) = _ival;
  *(ptr++) = _lddr;
  for (int i = 0; i < NLAD;           i++) *(ptr++) = _tkid[i];
  for (int i = 0; i < NLAD*NADR;      i++) *(ptr++) = _amin[i];
  for (int i = 0; i < NLAD*NADR;      i++) *(ptr++) = _amax[i];
  for (int i = 0; i < NLAD*NADR;      i++) *(ptr++) = _mask[i];
  for (int i = 0; i < NLAD*NADR*NPAR; i++) *(ptr++) = _fpar[i];
  for (int i = 0; i < NLAD*NADR;      i++) *(ptr++) = _resm[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasx[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasy[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasdx[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasdy[i];

  return ptr-psta;
}

int TrTasPar::Lin2Par(float *ptr)
{
  if (!ptr) return -1;

  float *psta = ptr;
  _ival = (int)*(ptr++);
  _lddr = (int)*(ptr++);
  for (int i = 0; i < NLAD;           i++) _tkid [i] = (int)*(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _amin [i] = (int)*(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _amax [i] = (int)*(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _mask [i] = (int)*(ptr++);
  for (int i = 0; i < NLAD*NADR*NPAR; i++) _fpar [i] = *(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _resm [i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasx [i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasy [i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasdx[i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasdy[i] = *(ptr++);

  return ptr-psta;
}
