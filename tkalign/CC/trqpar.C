// $Id: trqpar.C,v 1.3 2010/12/08 16:06:32 shaino Exp $
#include "TStopwatch.h"
#include "TMath.h"
#include "TH2.h"
#include "TROOT.h"

#include "root.h"
#include "amschain.h"

void trqpar(const char *fname,
	    const char *oname = "trqpar.root",
	    Int_t mfs = TrTrackR::kChoutko | TrTrackR::kMultScat, 
	    Int_t nproc = 0)
{
  AMSChain ch;
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;

  cout << "Ntr,Nent= " << ntr << " " << nent << endl;
  if (nproc <= 0) nproc = nent;

  Double_t bn1[51], bn2[61], bn3[81];
  for (Int_t i = 0; i <= 50; i++) bn1[i] = TMath::Power(10, 0.10*i-1);
  for (Int_t i = 0; i <= 60; i++) bn2[i] = TMath::Power(10, 0.10*i-3);
  for (Int_t i = 0; i <= 80; i++) bn3[i] = TMath::Power(10, 0.05*i-2);

  TString stt[4] = { "inner", "half8", "half9", "full" };

  gROOT->cd();

  for (Int_t i = 0; i < 4; i++) {
    new TH2F(Form("hist%d0", i+1), "dRinv-"+stt[i], 50, bn1, 80, bn3);
    new TH2F(Form("hist%d1", i+1), "Chisq-"+stt[i], 50, bn1, 60, bn2);
    new TH2F(Form("hist%d2", i+1), "HalfR-"+stt[i], 50, bn1, 80, bn3);
    new TH2F(Form("hist%d3", i+1), "Exres-"+stt[i], 50, bn1, 80, bn3);
    new TH2F(Form("hist%d4", i+1), "d1/R0-"+stt[i], 50, bn1, 400, -100, 100);
    new TH2F(Form("hist%d5", i+1), "d1/R1-"+stt[i], 50, bn1, 400, -100, 100);
    new TH2F(Form("hist%d6", i+1), "d1/R2-"+stt[i], 50, bn1, 400, -100, 100);
  }

  int trq[TrTrackR::Nclass] = { 0, TrTrackR::kBaseQ, 
				   TrTrackR::kBaseQ | TrTrackR::kHighQ };

  TStopwatch timer;
  timer.Start();

  Int_t intv  = 10000;
  Int_t nevt  = 0;
  Int_t nfill = 0;

  for (Int_t ient = 0; ient < nproc; ient++) {
    AMSEventR *evt = ch.GetEvent(ient);
    nevt++;

    if (nevt%intv == 0 || nevt == nproc) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%7d %7d (%5.1f%%) %4.0f sec (%4.1f kHz)",
		   nfill, nevt, 100.*nevt/nproc, tm, 1e-3*ient/tm) << endl;
    }
    if (evt->nTrTrack() == 0) continue;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;

    MCEventgR *mcg = (evt->nMCEventg() > 0) ? evt->pMCEventg(0) : 0;
    if (mcg && mcg->Charge == 0) mcg = 0;

    double rgt = TMath::Abs(trk->GetRigidity());
    if (mcg) rgt = TMath::Abs(mcg->Momentum/mcg->Charge);

    double hpar[TrTrackR::Nconf*TrTrackR::Nqpar];
    int    tcls[TrTrackR::Nconf+1];
    TrTrackR::DoTrackClass(mfs, hpar, tcls);

    for (Int_t i = 0; i < TrTrackR::Nconf; i++) {
      for (Int_t j = 0; j < TrTrackR::Nqpar-1; j++) {
	Double_t hp = hpar[i*TrTrackR::Nqpar+j];
	if (0 && j == 0) {
	  Double_t ecr = TrTrackR::GetErrRinvNorm(i, hpar[i*5+4]);
	  Double_t rrf = hpar[i*TrTrackR::Nqpar+4];
	  hp = TMath::Sqrt(hp*ecr*ecr)*1e2;
	}

	if (hp > 0) {
	  TH2F *hist = (TH2F *)gROOT->Get(Form("hist%d%d", i+1, j));
	  hist->Fill(rgt, hp);
	}
      }

      if (mcg) {
	Double_t ref = mcg->Momentum/mcg->Charge;
	Double_t rgt = hpar[i*TrTrackR::Nqpar+4];
	Double_t ecr = TrTrackR::GetErrRinvNorm(i, ref);

	if (rgt != 0 && ref != 0 && ecr > 0) {
	  Double_t dr  = 1/rgt-1/ref;

	  for (Int_t j = 0; j < TrTrackR::Nclass; j++) {
	    if ((tcls[i] & trq[j]) == trq[j]) {
	      TH2F *hist = (TH2F *)gROOT->Get(Form("hist%d%d", i+1, j+4));
	      hist->Fill(ref, dr/ecr);
	    }
	  }
	}

      }
    }
    nfill++;
  }

  TrTrackR::ShowTrackClass();

  TFile of(oname, "recreate");
  gROOT->GetList()->Write();
}
