// $Id$

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTasCluster.C
///\brief Source file of TrTasCluster class
///
///\date  2009/12/10 SH  First version
///\date  2009/12/17 SH  First Gbatch version
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "TrTasCluster.h"
#include "TrTasDB.h"
#include "TrTasPar.h"
#include "TkCoo.h"

#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"
#include "TROOT.h"
#include "TDirectory.h"

////////////////// New version for ISS //////////////////
double TrTasClusterR::Fit(TH1 *hist, int ibeam, int mode, double *par,
			                                  double *per)
{
  if (!par) return -1;

  if (!TkDBc::Head) TkDBc::GetFromTDV(1325000000, 5);
  if (!TrTasDB::Filled()) {
    TString sdb = getenv("AMSDataDir");
    sdb += "/v5.01/tasdb";

    if (getenv("TasDB")) sdb = getenv("TasDB");
    TrTasDB::Load(sdb);
  }

  for (int i = 0;        i < 5; i++) par[i] = 0;
  for (int i = 0; per && i < 5; i++) per[i] = 0;

  TString stt = hist->GetTitle();
  if (stt == "Type: 00CC TkID:  310") hist->SetBinContent(133, 0);
  if (stt == "Type: 30CC TkID: -606") hist->SetBinContent(102, 0);
  if (stt == "Type: 31CC TkID: -609") hist->SetBinContent(129, 0);
  if (stt == "Type: 31CC TkID: -609") hist->SetBinContent(131, 0);
  if (stt == "Type: 40CC TkID: -609") hist->SetBinContent( 79, 0);

  int id = TrTasDB::Find(hist->GetTitle(), ibeam);
  if (id < 0) return -1;

  static TF1 *func = 0;
  if (!func) func = new TF1("func", TrTasClusterR::SGaus, 0, 1, 5);

  double csq = -1;
  double mn = TrTasDB::Mean(id);
  int    tf = TrTasDB::Tfit(id);
  int  tkid = TrTasDB::TkID(id);

  if (mn > 0) {
    hist->GetXaxis()->SetRangeUser(mn-15, mn+15);

    Int_t    rng = tf%10;
    Int_t    ib1 = hist->FindBin(mn-rng);
    Int_t    ib2 = hist->FindBin(mn+rng);
    Double_t sum = hist->Integral(ib1, ib2);
    if (sum < 100) return -1;

    if (mode > 100) {
      Int_t nb1 = (mode/10)%10;
      Int_t nb2 =  mode    %10;
      Int_t ib  = hist->FindBin(mn);
      Double_t sx = 0;
      Double_t sw = 0;

      for (Int_t i = 0; i <= nb1; i++) {
	if (hist->GetBinContent(ib-i) > 0) {
	  sx += hist->GetBinContent(ib-i)*hist->GetBinCenter(ib-i);
	  sw += hist->GetBinContent(ib-i);
	}
      }
      for (Int_t i = 1; i <= nb2; i++) {
	if (hist->GetBinContent(ib+i) > 0) {
	  sx += hist->GetBinContent(ib+i)*hist->GetBinCenter(ib+i);
	  sw += hist->GetBinContent(ib+i);
	}
      }
      if (sw != 0) sx /= sw;
      par[0] = hist->GetBinContent(hist->FindBin(sx));
      par[1] = sx;
      par[2] = sw;
      csq = 1;
    }
    else if (mode == 2 || (mode == 0 && tf > 10)) {
      double hmax = hist->GetMaximum();
      func->SetParameters(hmax*1.5, mn, 3, hmax, 500);
      func->FixParameter(4, 25);
      func->ReleaseParameter(3);

      hist->Fit(func, "q0", "", mn-rng, mn+rng);
      for (int i = 0;        i < 4; i++) par[i] = func->GetParameter(i);
      for (int i = 0; per && i < 4; i++) per[i] = func->GetParError (i);

      if (func->GetNDF() > 0) csq = func->GetChisquare()/func->GetNDF();
    }
    else {
      hist->Fit("gaus", "q0", "", mn-rng, mn+rng);

      TF1 *fg = hist->GetFunction("gaus");
      for (int i = 0; fg &&        i < 3; i++) par[i] = fg->GetParameter(i);
      for (int i = 0; fg && per && i < 3; i++) per[i] = fg->GetParError (i);

      if (fg && fg->GetNDF() > 0) csq = fg->GetChisquare()/fg->GetNDF();
    }
    if (par[2] < 0) par[2] = TMath::Abs(par[2]);

    float    xch = 830;
    float    ych = par[1];
    int       ml = TkCoo::GetMaxMult (tkid, xch);
    float     lx = TkCoo::GetLocalCoo(tkid, xch, ml-1);
    float     ly = TkCoo::GetLocalCoo(tkid, ych, ml-1);
    AMSPoint coo = TkCoo::GetGlobalA (tkid, lx, ly);
    par[4] = coo.y();
    if (per) per[4] = per[1]*TkDBc::Head->_PitchS;
  }

  return csq;
}
////////////////// New version for ISS //////////////////


ClassImp(TrTasClusterR);

int TrTasClusterR::fitmode = TrTasClusterR::SGAUS;
TDirectory *TrTasClusterR::HistDir = 0;

TrTasClusterR::TrTasClusterR(int tkid, int side, int addr, 
			     int nelem, float *adc)
  : TrClusterR(tkid, side, addr, nelem, 0, adc, TASCLS)
{
}

float TrTasClusterR::GetXCofGTas()
{
  Status = 0;

  if (_nelem < 5) return 0;
  if (!TrTasPar::Head) return 0;

  int ilad = TrTasPar::Head->GetIlad(_tkid);
  if (ilad < 0) return 0;

  int ipk =  TrTasPar::Head->GetIpk(ilad, _address);
  if (ipk < 0) return 0;

  Int_t apk = (TrTasPar::Head->GetAmin(ilad, ipk)+
	       TrTasPar::Head->GetAmax(ilad, ipk))/2;
  Int_t ap1 = apk-20;
  Int_t ap2 = apk+20;

  TString hid = Form("hist%02d%d", ilad+1, ipk+1);

  TH1F *hist1 = (HistDir) ? (TH1F *)HistDir->Get(hid+"1") : 0;
  TH2F *hist2 = (HistDir) ? (TH2F *)HistDir->Get(hid+"2") : 0;

  if (!hist1 || !hist2) {
    if (!HistDir) {
      if (gFile) gFile->cd();
      else gROOT->cd();
      HistDir = new TDirectoryFile("tashist", "TAS histograms");
    }
    TDirectory *sdir = gDirectory;

    HistDir->cd();
    hist1 = new TH1F(hid+"1", Form("TkId %4d (%2d-%d)", _tkid, ilad, ipk),
		     41, ap1-0.5, ap2+0.5);
    hist2 = new TH2F(hid+"2", Form("TkId %4d (%2d-%d)", _tkid, ilad, ipk),
		     41, ap1-0.5, ap2+0.5, 500, 0, 5000);
    if (sdir) sdir->cd();
  }
  hist1->Reset();

  for (int i = 0; i < _nelem; i++)
    if (ap1 <= _address+i && _address+i <= ap2) {
      int addr = _address+i;
      int ib   = addr-ap1+1;
      hist1->SetBinContent(ib, _signal.at(i));
      hist1->SetBinError  (ib, 1);
      hist2->Fill(addr, _signal.at(i));
    }

  double pmax = hist1->GetMaximum();
  for (int i = 0; i < _nelem; i++)
    if (ap1 <= _address+i && _address+i <= ap2) {
      int addr = _address+i;
      int ib   = addr-ap1+1;
      double bc = hist1->GetBinContent(ib);

      if (TrTasPar::Head->IsMasked(ilad, ipk, addr) || bc < -100) {
	if (bc < pmax/2) {
	  bc = pmax/2;
	  hist1->SetBinContent(ib, bc);
	}
	hist1->SetBinError(ib, pmax-bc);
      }
    }

  Double_t xmin = _address-0.25;
  Double_t xmax = _address+_nelem-0.75;

  TF1 *func;
  if (fitmode == SGAUS) {
    func = SGaus(xmin, xmax);
    func->SetParameter(0, TrTasPar::Head->GetFpar(ilad, ipk, 0));
    func->SetParameter(1, TrTasPar::Head->GetFpar(ilad, ipk, 1));
    func->SetParameter(2, TrTasPar::Head->GetFpar(ilad, ipk, 2));
    func->SetParameter(3, TrTasPar::Head->GetFpar(ilad, ipk, 3));

    if (TrTasPar::Head->GetFpar(ilad, ipk, 3) == 0 && 
	TrTasPar::Head->GetFpar(ilad, ipk, 4) == 0) {
      func->FixParameter(3, 0);
      func->FixParameter(4, 0);
    }
    else {
      func->FixParameter(4, TrTasPar::Head->GetFpar(ilad, ipk, 4)); //50
      func->SetParLimits(0, 0, 25000);
      func->SetParLimits(3, 1000, 5000);
    }
  }
  else func = new TF1("func", "gaus", xmin, xmax);

  hist1->Fit(func, "q0R");
  float xmean = func->GetParameter(1);
  delete func;

  // The first sensor is TAS one
  int imult = TkCoo::GetMaxMult(_tkid, xmean);

  Status = TASCLS;
  return TkCoo::GetLocalCoo(_tkid, xmean, imult);
}


TF1 *TrTasClusterR::SGaus(double xmin, double xmax, const char *name)
{
  return new TF1(name, TrTasClusterR::SGaus, xmin, xmax, 5);
}

double TrTasClusterR::SGaus(double *xx, double *par)
{
  Double_t scale = par[0];
  Double_t mean  = par[1];
  Double_t sigma = par[2];
  Double_t sp0   = par[3];
  Double_t sp1   = par[4];
  Double_t x     = *xx;

  Double_t gaus = TMath::Gaus(x, mean, sigma)*scale;
  if (sp0 <= 0 || sp1 <= 0) return gaus;

  Double_t gx  = (gaus-sp0)/sp1;
  Double_t exp = TMath::Exp(gx);

  return sp0+(1-(exp+1/exp)/(exp-1/exp))*gx/2*sp1;
}


AMSPoint TrTasClusterR::Align(TrClusterR *xcls, TrClusterR *ycls)
{
  if (!xcls || !ycls) return AMSPoint(0, 0, 0);
  int tkid = xcls->GetTkId();
  int adrx = xcls->GetSeedAddress();
  int adry = ycls->GetSeedAddress();
  int ilad = TrTasPar::Head->GetIlad(tkid);
  int ipkx = TrTasPar::Head->GetIpk (ilad, adrx);
  int ipky = TrTasPar::Head->GetIpk (ilad, adry);

  return AMSPoint(TrTasPar::Head->GetResm(ilad, ipkx),
		  TrTasPar::Head->GetResm(ilad, ipky), 0);
}
