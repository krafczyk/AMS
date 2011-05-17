// $Id: TrAlignFit.C,v 1.9 2011/05/17 09:19:40 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrAlignFit.C
///\brief Source file of TrAlignFit
///
///\date  2007/04/02 SH  First test version
///\date  2011/05/13 SH  Dynamic ext.planes alignment
///$Date: 2011/05/17 09:19:40 $
///
///$Revision: 1.9 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrAlignFit.h"

ClassImp(TrExtAlignFit)


#ifdef _MINUIT2_

#include "HistoMan.h"
#include "TH3.h"
#include "TH2.h"
#include "TF1.h"
#include "TROOT.h"
#include "TMath.h"
#include "TDirectoryFile.h"

Double_t TrExtAlignFit::fRange = 1;

TDirectory *TrExtAlignFit::fWorkDir = 0;

TFitterMinuit *TrExtAlignFit::fMinfit = 0;

Int_t TrExtAlignFit::Fit(Int_t layer, uint time, 
			 TDirectory *dir, const char *name,
			 Int_t prof_mode,
			 Int_t fix, Int_t verb, Int_t nfcn)
{
  TDirectory *dsave = gDirectory;

  if (!fWorkDir) {
    gROOT->cd();
    fWorkDir = new TDirectoryFile("TrExtAlignFit",
				   "External plane alingment");
  }
  fWorkDir->cd();

  TString sdn = Form("%d", time);
  
  TDirectory *wdir = (TDirectory *)fWorkDir->Get(sdn);
  if (!wdir) wdir = new TDirectoryFile(sdn, "Time: "+sdn);
  wdir->cd();

  // afit will be deleted by the next fMinfit->SetMinuitFCN(this);
  TrExtAlignFit *afit = new TrExtAlignFit(layer);

  if (afit->CopyHists(dir, name) < 4) return -1;
  if (afit->ProfHists(prof_mode) < 4) return -2;

  Int_t ret = afit->Minimize(fix, verb, nfcn);
  if (ret < 0) return -3;

  TrExtAlignDB::GetHead()->Fill(layer, time, afit->fPar);

  if (dsave) dsave->cd();

  return 0;
}

Int_t TrExtAlignFit::WriteWorkDir(const char *name)
{
  if (!fWorkDir) {
    std::cout << "TrExtAlignFit::WriteWorkDir-E-WorkDir not found"
	      << std::endl;
    return -1;
  }

  TDirectory *dsave = gDirectory;

  TList *objs = fWorkDir->GetList();
  if(!objs) {
    std::cout << "TrExtAlignFit::WriteWorkDir-E-objs not found"
	      << std::endl;
    return -1;
  }

  Int_t ndir = 0;
  TDirectoryFile *dir = new TDirectoryFile(name, fWorkDir->GetTitle());

  for (Int_t i = 0; i < objs->GetSize(); i++) {
    TDirectory *sdir = dynamic_cast<TDirectory*>(objs->At(i));
    if (!sdir) continue;
    dir->cd();

    TDirectoryFile *wdir = new TDirectoryFile(sdir->GetName(),
					      sdir->GetTitle());
    wdir->cd();

    TList *hobjs = sdir->GetList();
    for (Int_t j = 0; j < hobjs->GetSize(); j++)
      if (hobjs->At(j)) hobjs->At(j)->Write();

    ndir++;
  }

  if (dsave) dsave->cd();

  return ndir;
}

Double_t TrExtAlignFit::Chisq(const PVec &par, Bool_t pset) const
{
  Double_t dx = par[0]*1e-4;
  Double_t dy = par[1]*1e-4;
  Double_t dz = par[2]*1e-4;
  Double_t da = par[3]*1e-4/40;
  Double_t db = par[4]*1e-4/40;
  Double_t dc = par[5]*1e-4/40;

  if (pset) fPar.SetPar(-dx, -dy, -dz, -da, -db, -dc);

  Double_t chisq = 0;
  Int_t    ndf   = 0;

  for (Int_t i = 0; i < 4; i++) {
    TString shn = Form("prof%d%d", fLayer, i+1);
    TH2 *prof = dynamic_cast<TH2*>(fHarray.FindObject(shn));
    if (!prof) {
      static int nerr = 0;
      if (nerr++ < 8)
	std::cout << "TrExtAlignFit::Chisq-E-Histogram not found: "
		  << shn.Data() << std::endl;
      return -1;
    }

    for (Int_t j = 0; j < prof->GetNbinsX(); j++) {
      for (Int_t k = 0; k < prof->GetNbinsY(); k++) {
	if (prof->GetBinError(j+1, k+1) <= 0) continue;

	Double_t pos = prof->GetXaxis()->GetBinCenter(j+1);
	Double_t dtr = prof->GetYaxis()->GetBinCenter(k+1);
	Double_t res = prof->GetBinContent(j+1, k+1);
	Double_t rer = prof->GetBinError  (j+1, k+1);
	Double_t ddz = (i%2 == 0) ? db*pos : -dc*pos;
	Double_t drs = (i/2 == 0) ? dx : dy;
      //Double_t drt = (i == 1) ?  da*pos 
      //            : ((i == 2) ? -da*pos : 0);
	Double_t drt = (i == 2) ? -da*pos : 0;

	Double_t ddr = res -drs-drt +(dz+ddz)*dtr;
	chisq += ddr*ddr/rer/rer;
	ndf++;
      }
    }
  }

  if (pset) {
    fPar.chisq = chisq;
    fPar.NDF   = ndf;
  }

  return chisq;
}

Double_t TrExtAlignFit::Chisq(Bool_t pset) const
{
  if (!fMinfit) return -1;

  PVec pvec;
  for (Int_t i = 0; i < fMinfit->GetNumberTotalParameters(); i++)
    pvec.push_back(fMinfit->GetParameter(i));

  return Chisq(pvec, pset);
}

Int_t TrExtAlignFit::Minimize(Int_t fix, Int_t verb, Int_t nfcn)
{
  if (!fMinfit) fMinfit = new TFitterMinuit;

  fMinfit->CreateMinimizer();
  fMinfit->SetMinuitFCN(this);

  enum { NPAR = 6 };
  TString  stn[NPAR] = { "dx", "dy", "dz", "dth", "dph", "dps" };
  Double_t err[NPAR] = {   10,   10,  100,    50,   100,   100 };

  for (Int_t i = 0; i < NPAR; i++)
    fMinfit->SetParameter(i, stn[i], 0, err[i], 0, 0);

  std::cout << "TrExtAlignFit::Minimize-I-Minimization for L" << fLayer
	    << std::endl;

  Int_t nfix = 0;
  for (Int_t i = 0; i < NPAR; i++) {
    if (fix & (1 << i)) {
      fMinfit->FixParameter(i);
      if (nfix++ == 0)
	std::cout << "TrExtAlignFit::Minimize-I-Parameter";
      std::cout << " " << stn[i].Data();
    }
  }
  if (nfix > 0) std::cout << " fixed" << std::endl;

  Double_t csq0 = Chisq(kTRUE);
  if (csq0 < 0 || fPar.NDF <= 0) return -1;

  Int_t ret = fMinfit->Minimize(nfcn, 0.001);
  std::cout << "TrExtAlignFit::Minimize-I-ret = " << ret << std::endl;
  if (verb > 0) fMinfit->PrintResults(verb, 0);

  Double_t csq1 = Chisq(kTRUE);
  if (csq1 < 0 || fPar.NDF <= 0) return -2;
  std::cout << "TrExtAlignFit::Minimize-I-csq= "
	    << csq0/fPar.NDF << " => " << csq1/fPar.NDF
	    << std::endl << std::endl;

  Double_t per[NPAR];
  for (Int_t i = 0; i < NPAR; i++) per[i] = fMinfit->GetParError(i);
  fPar.SetErr(per[0], per[1], per[2], per[3], per[4], per[5]);

  return ret;
}

Int_t TrExtAlignFit::CopyHists(TDirectory *dir, const char *name)
{
  fHarray.Clear();

  Int_t nadd = 0;
  for (Int_t i = 0; i < 4; i++) {
    TString shn = Form(name, fLayer, i+1);
    TObject *obj = (dir) ? dir->Get(shn) : hman.Get(shn);
    if (!obj) {
      static int nerr = 0;
      if (nerr++ < 8)
	std::cout << "TrExtAlignFit::CopyHists-E-Histogram not found: "
		  << shn.Data() << std::endl;
      continue;
    }
    fHarray.Add(obj->Clone(Form("hist%d%d", fLayer, i+1)));
    nadd++;
  }

  return nadd;
}

Int_t TrExtAlignFit::ProfHists(Int_t mode)
{
  Int_t wthd = 500;

  if (mode == 0) {
    mode = (fRange > 0.05) ?   1 :   2;
    wthd = (fRange > 0.05) ? 500 : 100;
  }

  Double_t pmax =  500e-4;
  Double_t emax =  100e-4;
  Double_t emin =  0.5e-4;
  Double_t sig1 =  300e-4;
  Double_t sig2 = 1300e-4;
  Double_t rpk  =  1.0;

  std::cout << "TrExtAlignFit::ProfHists-I-Profiling histograms for L"
	    << fLayer << std::endl;

  Int_t nadd = 0;
  for (Int_t i = 0; i < 4; i++) {
    TString shn = Form("hist%d%d", fLayer, i+1);
    TH3 *hist = dynamic_cast<TH3*>(fHarray.FindObject(shn));
    if (!hist) {
      static int nerr = 0;
      if (nerr++ < 8)
	std::cout << "TrExtAlignFit::ProfHists-E-Histogram not found: "
		  << shn.Data() << std::endl;
      continue;
    }

    Double_t wsum = hist->GetSumOfWeights();
    if (wsum < 1e5) { emax = 100e-4; wthd = 200; }
    if (wsum > 1e6)   emax =  20e-4;

    shn.ReplaceAll("hist", "prof");
    fHarray.Add(Prof(hist, shn, mode, wthd,
		     pmax, emax, emin, sig1, sig2, rpk));
    nadd++;
  }
  std::cout << std::endl;

  return nadd;
}

TH2 *TrExtAlignFit::Prof(TH3 *hist, const char *name,
			 Int_t mode, Int_t wthd,
			 Double_t pmax, Double_t emax, Double_t emin,
			 Double_t sig1, Double_t sig2, Double_t rpk)
{
  if (!hist) return 0;

  Int_t nbx = hist->GetNbinsX();
  Int_t nby = hist->GetNbinsY();
  Int_t nbz = hist->GetNbinsZ();
  TAxis *ax = hist->GetXaxis();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  Int_t wcut = (Int_t)(hist->GetSumOfWeights()/nbx/nby);
  if (wcut > wthd) wthd = wcut;

  TH2F *prof = new TH2F(name, hist->GetName(), 
			nbx, ax->GetXmin(), ax->GetXmax(),
			nby, ay->GetXmin(), ay->GetXmax());

  TString shn = name;
  shn.ReplaceAll("prof", "hprj");

  TH1F *hprj = new TH1F(shn, hist->GetTitle(),
			nbz, az->GetXmin(), az->GetXmax());
  for (Int_t k = 0; k < nbz; k++)
    hprj->SetBinContent(k+1, hist->Integral(1, nbx, 1, nby, k+1, k+1));

  shn.ReplaceAll("hprj", "");
  Int_t hid = shn.Atoi()%10;
  Int_t lyr = shn.Atoi()/10;

  TString scn = Form("hchk%d", lyr);
  TH1F *hchk = (TH1F *)gDirectory->Get(scn);
  if (!hchk) hchk = new TH1F(scn, Form("Prof.check L%d", lyr),
			     4*nbx*nby, 0, 4*nbx*nby);

  TF1 *func;
  Double_t pfix[6];

  if (mode == 1) {
    func = new TF1("func",
		   "[0]/(exp((x-[1])/[2])+exp(-(x-[1])/[2]))+"
		   "[3]*[0]/(exp((x-[4])/[5])+exp(-(x-[4])/[5]))");
    if (rpk > 0)
      func->SetParameters(hprj->GetMaximum(), 0, sig1, rpk, 0, sig2);
    else {
      func->FixParameter(3, 0);
      func->FixParameter(4, 0);
      func->FixParameter(5, 1);
      func->SetParameters(hprj->GetMaximum(), 0, sig1, 0, 0, 1);
    }
    hprj->Fit(func, "q0");

    pfix[1] =            func->GetParameter(1);
    pfix[2] = TMath::Abs(func->GetParameter(2));
    pfix[3] =            func->GetParameter(3);
    pfix[4] =            func->GetParameter(4);
    pfix[5] = TMath::Abs(func->GetParameter(5));

    Double_t perr[6];
    for (Int_t i = 0; i < 6; i++) perr[i] = func->GetParError(i);

    if (pfix[2] > pfix[5]) {
      pfix[1] =            func->GetParameter(4);
      pfix[2] = TMath::Abs(func->GetParameter(5));
      pfix[3] =          1/func->GetParameter(3);
      pfix[4] =            func->GetParameter(1);
      pfix[5] = TMath::Abs(func->GetParameter(2));
      perr[1] = func->GetParError(4);
      perr[2] = func->GetParError(5);
      perr[4] = func->GetParError(1);
      perr[5] = func->GetParError(2);
    }

    func->FixParameter(2, pfix[2]); func->FixParameter(3, pfix[3]);
    func->FixParameter(5, pfix[4]); func->FixParameter(4, pfix[5]);

    std::cout << Form(" s: %5.1f(%4.1f) %6.1f(%5.1f) "
		       "m: %6.1f(%4.1f) %6.1f(%4.1f) r: %4.2f(%4.2f)",
		      pfix[2]*1e4, perr[2]*1e4,
		      pfix[5]*1e4, perr[5]*1e4,
		      pfix[1]*1e4, perr[1]*1e4,
		      pfix[4]*1e4, perr[4]*1e4,
		      pfix[3],     perr[3]) << std::endl;
  }

  TH1F *htmp = new TH1F("htmp", "htmp", nbz, az->GetXmin(), az->GetXmax());

  for (Int_t i = 0; i < nbx; i++) {
    for (Int_t j = 0; j < nby; j++) {

      htmp->Reset();
      for (Int_t k = 0; k < nbz; k++)
	htmp->SetBinContent(k+1, hist->GetBinContent(i+1, j+1, k+1));

      if (htmp->GetSumOfWeights() < wthd) continue;

      if (mode == 1) {
	Double_t sig1 = 0.05;
	Double_t sig2 = 0.25;

	if (htmp->GetRMS() < 0.02) {
	  sig1 = 0.002;
	  sig2 = 0.010;
	}

	func->SetParameters(htmp->GetMaximum(), pfix[1], 
			    pfix[2], pfix[3], pfix[4], pfix[5]);
	htmp->Fit(func, "q0");
      }
      else {
	htmp->Fit("gaus", "q0");
	func = htmp->GetFunction("gaus");
      }

      Double_t par = func->GetParameter(1);
      Double_t per = func->GetParError (1);

      if (per < emax && TMath::Abs(par) < pmax) {
	if (per < emin) per = emin;

	prof->SetBinContent(i+1, j+1, par);
	prof->SetBinError  (i+1, j+1, per);

	Int_t ib = (hid-1)*nbx*nby+j*nbx+i+1;
	hchk->SetBinContent(ib, par);
	hchk->SetBinError  (ib, per);
      }
    }
  }
  delete htmp;

  return prof;
}

#endif
