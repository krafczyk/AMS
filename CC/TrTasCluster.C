#include "TrTasCluster.h"
#include "TrTasDB.h"
#include "TrTasPar.h"

#include "TF1.h"
#include "TH1.h"
#include "TMath.h"
#include "TROOT.h"

ClassImp(TrTasClusterR);

int TrTasClusterR::fitmode = TrTasClusterR::SGAUS;

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

  TH1F *hist = (TH1F *)gROOT->Get(hid);
  if (!hist) {
    gROOT->cd();
    hist = new TH1F(hid, Form("TkId %4d (%2d-%d)", _tkid, ilad, ipk),
		    41, ap1-0.5, ap2+0.5);
  }
  hist->Reset();

  for (int i = 0; i < _nelem; i++)
    if (ap1 <= _address+i && _address+i <= ap2) {
      int addr = _address+i;
      int ib   = addr-ap1+1;
      hist->SetBinContent(ib, _signal.at(i));
      hist->SetBinError  (ib, 1);
    }

  double pmax = hist->GetMaximum();
  for (int i = 0; i < _nelem; i++)
    if (ap1 <= _address+i && _address+i <= ap2) {
      int addr = _address+i;
      int ib   = addr-ap1+1;
      double bc = hist->GetBinContent(ib);

      //if (TrTasDB::Head->IsMasked(_tkid, addr) || bc < -100) {
      if (bc < -100) {
	if (bc < pmax/2) {
	  bc = pmax/2;
	  hist->SetBinContent(ib, bc);
	}
	hist->SetBinError(ib, pmax-bc);
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

  hist->Fit(func, "q0R");
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
