// $Id$

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasDB.C
///\brief Source file of TrTasDB class
///
///\date  2009/12/10 SH  First version
///\date  2009/12/17 SH  First Gbatch version
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrTasDB.h"
#include "TrTasPar.h"

#include "TFile.h"
#include "TString.h"

#include <fstream>
#include <iostream>

////////////////// New version for ISS //////////////////
TString TrTasDB::_sttl[TrTasDB::Ne];
int     TrTasDB::_beam[TrTasDB::Ne];
int     TrTasDB::_tkid[TrTasDB::Ne];
int     TrTasDB::_tfit[TrTasDB::Ne];
double  TrTasDB::_mean[TrTasDB::Ne];
double  TrTasDB::_ycoo[TrTasDB::Ne];
int     TrTasDB::_nfil;

int TrTasDB::Load(const char *fname)
{
  if (Filled()) return 0;

  std::ifstream fin(fname);
  if (!fin) {
    std::cout << "File not found: " << fname << std::endl;
    return -1;
  }
  std::cout << "Loading tasdb: " << fname << std::endl;

  _nfil = 0;

  for (int i = 0; i < Ne; i++) {
    if (fin.eof()) {
      std::cerr << "Unexpected EOF at " << i << std::endl;
      return -2;
    }
    int type, tkid;
    fin >> type >> tkid >> _beam[i] >> _mean[i] >> _ycoo[i] >> _tfit[i];
    _tkid[i] = tkid;
    _sttl[i] = Form("Type: %02dCC TkID: %4d", type, tkid);
    _nfil++;
  }
  return _nfil;
}

int TrTasDB::Find(const char *title, int ibeam)
{
  for (int i = 0; i < Ne; i++)
    if (_sttl[i] == title && _beam[i] == ibeam) return i;
  return -1;
}
////////////////// New version for ISS //////////////////

TrTasDB *TrTasDB::Head = 0;

ClassImp(TrTasDB);

TrTasPar *TrTasDB::FindPar(int ival, int lddr)
{
  _parIT it = _parmap.find(_Index(ival, lddr));
  return (it == _parmap.end()) ? 0 : (TrTasPar::Head = it->second);
}

TrTasDB *TrTasDB::Load_old(const char *fname)
{
  if (!fname) return 0;

  TFile f(fname);
  if (!f.IsOpen()) return 0;

  Head = (TrTasDB *)f.Get("TrTasDB");
  std::cout << "TrTasDB::Load " << fname << std::endl;
  return Head;
}

int TrTasDB::Init(const char *dname)
{
  int ival[3] = { 16, 20, 25 };
  int lddr[8] = { 0x0001, 0x0100, 0x0002, 0x0200,
		  0x0004, 0x0400, 0x0008, 0x0800 };

  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 8; j++) {
      TrTasPar *tasp = new TrTasPar;
      tasp->Init(ival[i], lddr[j], dname);
      _parmap[_Index(ival[i], lddr[j])] = tasp;
    }

  return 0;
}

void TrTasDB::Output(const char *dname)
{
  for (_parIT it = _parmap.begin(); it != _parmap.end(); it++)
    if (it->second) it->second->Output(dname);
}

void TrTasDB::Print(Option_t *option) const
{
  for (_parIT it = _parmap.begin(); it != _parmap.end(); it++)
    if (it->second) it->second->Print(option);
}


float *TrTasDB::linear = 0;

void TrTasDB::DB2Lin()
{
  if (!linear) {
    std::cout << "TrTasDB::DB2Lin()-INFO the linear space is created NOW"
	      << std::endl;
    linear = new float[GetLinearSize()/4];
  }
  for (int i = 0; i < GetLinearSize()/4; i++) linear[i] = 0;

  float *ptr = linear;
  for (_parIT it = _parmap.begin(); it != _parmap.end(); it++)
    if (it->second) ptr += it->second->Par2Lin(ptr);
}

void TrTasDB::Lin2DB()
{
  if (!linear) {
    std::cerr << "TrTasDB::Lin2DB()- Error! the linear space pointer is NULL!"
	 << std::endl;
    std::cerr << " Calibration is NOT updated!!!" << std::endl;
    return;
  }

  float *ptr = linear;
  for (_parIT it = _parmap.begin(); it != _parmap.end(); it++)
    if (it->second) ptr += it->second->Lin2Par(ptr);
}

void SLin2TasDB()
{
  if (TrTasDB::Head) TrTasDB::Head->Lin2DB();
}

#ifdef __ROOTSHAREDLIBRARY__
int TrTasDB::Save2DB(time_t, time_t) { return 0; }

#else
#include "timeid.h"
#include "commonsi.h"

int TrTasDB::Save2DB(time_t statime, time_t endtime)
{
  if (!TrTasDB::Head) {
    std::cerr << "TrTasDB::Save2DB---TrTasDB not exists" << std::endl;
    return -3;
  }

  TrTasDB::Head->CreateLinear();
  TrTasDB::Head->DB2Lin();

  tm beg, end;
  localtime_r(&statime, &beg);
  localtime_r(&endtime, &end);

  AMSTimeID* tt = new AMSTimeID(AMSID("TrackerTasPar",1),
				beg, end,
				TrTasDB::GetLinearSize(),
				TrTasDB::linear,
				AMSTimeID::Standalone, 1);
  tt->UpdateMe();
  tt->write(AMSDATADIR.amsdatabase);

  return 0;
}

#endif
