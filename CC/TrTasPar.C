#include "TrTasPar.h"
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

int TrTasPar::Init(int ival, int lddr, const char *dname)
{
  _ival = ival;
  _lddr = lddr;

  for (int i = 0; i < NLAD; i++) {
    _tkid[i] = 0;
    for (int j = 0; j < NADR; j++) {
      _amin[i*NADR+j] = _amax[i*NADR+j] = 0;
      _resm[i*NADR+j] = 0;
      for (int k = 0; k < NPAR; k++) _fpar[(i*NADR+j)*NPAR+k] = 0;
    }
  }
  for (int i = 0; i < NLAS; i++) _lasx[i] = _lasy[i] = 0;
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
    n++;
  }

  for (int i = 0; i < NLAS && !fin.eof(); i++)
    fin >> _lasx[i] >> _lasy[i];

  std::cout << "Read: " << sfn.Data() << " " << n << std::endl;

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
	for (int i = 0; i < 8; i++)
	  std::cout << Form(" %4d %2d %d %5.0f %6.1f %4.2f %4.0f %5.1f",
			    GetTkId(i+l*8), i+l*8+1, j+1,
			    GetFpar(i+l*8, j, 0), GetFpar(i+l*8, j, 1),
			    GetFpar(i+l*8, j, 2), GetFpar(i+l*8, j, 3),
			    GetFpar(i+l*8, j, 4)) << std::endl;
  }

  else {
    std::cout << "TkIDs:";
    for (int i = 0; i < 8; i++) std::cout << Form(" %4d", GetTkId(i));
    std::cout << std::endl << "      ";
    for (int i = 0; i < 8; i++) std::cout << Form(" %4d", GetTkId(i+8));
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

  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++) {
      int it = j+(i/4)*8;
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
  return 2+NLAD+NLAD*NADR+NLAD*NADR+NLAD*NADR*NPAR+NLAD*NADR+NLAS+NLAS;
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
  for (int i = 0; i < NLAD*NADR*NPAR; i++) *(ptr++) = _fpar[i];
  for (int i = 0; i < NLAD*NADR;      i++) *(ptr++) = _resm[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasx[i];
  for (int i = 0; i < NLAS;           i++) *(ptr++) = _lasy[i];

  return ptr-psta;
}

int TrTasPar::Lin2Par(float *ptr)
{
  if (!ptr) return -1;

  float *psta = ptr;
  _ival = *(ptr++);
  _lddr = *(ptr++);
  for (int i = 0; i < NLAD;           i++) _tkid[i] = *(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _amin[i] = *(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _amax[i] = *(ptr++);
  for (int i = 0; i < NLAD*NADR*NPAR; i++) _fpar[i] = *(ptr++);
  for (int i = 0; i < NLAD*NADR;      i++) _resm[i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasx[i] = *(ptr++);
  for (int i = 0; i < NLAS;           i++) _lasy[i] = *(ptr++);

  return ptr-psta;
}
