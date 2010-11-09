// $Id: pmrinv.C,v 1.1 2010/11/09 08:38:54 shaino Exp $
#include "MagField.h"
#include "TkCoo.h"
#include "TrFit.h"

#include "TMath.h"
#include "TH3.h"
#include "TH2.h"
#include "TROOT.h"

#include "TStopwatch.h"
#include "TSystem.h"

#include <fstream>


enum { NL = 9 };

void    init(void);
void    fill(Int_t hid, Double_t x, Double_t y = 0, Double_t z = 0);
Double_t fit(Int_t *tkid, Int_t *imlt, Float_t *xcog, Float_t *ycog);

void pmrinv(const char *fname, const char *tkdbc)
{
  ifstream fin(fname);
  if (!fin) {
    cerr << "File not found: " << fname << endl;
    return;
  }

  TkDBc::CreateTkDBc();
  TkDBc::Head->init(3, tkdbc);

  TString ssn = "tksens.dat";
  ifstream ftmp(ssn);
  if (ftmp.good())
    TkDBc::Head->readAlignmentSensor(ssn);

  TString smf = gSystem->ExpandPathName("$AMSDataDir/v5.00"
					"/MagneticFieldMapPermanent_NEW.bin");
  MagField::GetPtr()->Read(smf);
  MagField::GetPtr()->SetScale(1);
  MagField::GetPtr()->SetMagstat(1);

  gROOT->cd();
  init();

  Int_t   tkid[NL], imlt[NL], ibuf[NL], run, ient;
  Float_t xcog[NL], ycog[NL];

  Int_t nr = 0, nf = 0, intv = 100000;

  TStopwatch timer;
  timer.Start();

  for (;;) {
    fin.read((char*)&run,  sizeof(run));
    fin.read((char*)&ient, sizeof(ient));
    fin.read((char*) ibuf, sizeof(ibuf));
    fin.read((char*) xcog, sizeof(xcog));
    fin.read((char*) ycog, sizeof(ycog));
    if (fin.eof()) break;

    if (nr > 0 && nr%intv == 0) {
      Double_t rt = timer.RealTime();
      Double_t ct = timer.CpuTime();
      timer.Continue();
      cout << Form("%7d %7d %6.1f %6.1f", nf, nr, rt, ct) << endl;
    }
    nr++;

    for (Int_t i = 0; i < NL; i++) {
      tkid[i] = TMath::Sign(TMath::Abs(ibuf[i])/100, ibuf[i]);
      imlt[i] =             TMath::Abs(ibuf[i])%100;
    }

    if (tkid[7] == 0 || tkid[8] == 0) continue;

    Double_t argt = fit(tkid, imlt, xcog, ycog);

    if (argt > 0) {
      nf++;
      if (argt > 2e3) cout << Form("Event check: %d %6d %6.2f",
				   run, ient, argt*1e-3) << endl;
    }
  }

  TString sfn = fname;
  sfn.ReplaceAll("dat", "root");
  sfn.ReplaceAll("pmafit", "pmrinv");
  if (sfn == fname) sfn = "pmrinv.root";

  TFile of(sfn, "recreate");
  gROOT->GetList()->Write();
}

void init(void)
{
  Double_t binc[121], binr[81];
  for (Int_t i = 0; i <=  120; i++) binc[i] = TMath::Power(10, -3+i*0.05);
  for (Int_t i = 0; i <=   80; i++) binr[i] = TMath::Power(10, -1+i*0.05);

  new TH2F("hist11", "csqx", 80, binr, 120, binc);
  new TH2F("hist12", "csqy", 80, binr, 120, binc);
  new TH2F("hist13", "csqx", 80, binr, 120, binc);
  new TH2F("hist14", "csqy", 80, binr, 120, binc);
  new TH2F("hist15", "csqx", 80, binr, 120, binc);
  new TH2F("hist16", "csqy", 80, binr, 120, binc);

  new TH2F("hist10", "argt", 80, binr,  80, binr);

  new TH2F("hist21", "res8", 80, binr, 200, -1000, 1000);
  new TH2F("hist22", "res9", 80, binr, 200, -1000, 1000);
  new TH2F("hist23", "res8", 80, binr, 300,    -3, 3);
  new TH2F("hist24", "res9", 80, binr, 300,    -3, 3);
  new TH2F("hist25", "res8", 80, binr, 200, -1000, 1000);
  new TH2F("hist26", "res9", 80, binr, 200, -1000, 1000);
  new TH2F("hist27", "res8", 80, binr, 300,    -3, 3);
  new TH2F("hist28", "res9", 80, binr, 300,    -3, 3);

  new TH2F("hist31", "csqx", 201, -100.5, 100.5, 120, binc);
  new TH2F("hist32", "csqy", 201, -100.5, 100.5, 120, binc);
  new TH2F("hist35", "csqx", 201, -100.5, 100.5, 120, binc);
  new TH2F("hist36", "csqy", 201, -100.5, 100.5, 120, binc);

  new TH2F("hist41", "res8", 201, -100.5, 100.5, 300, -3, 3);
  new TH2F("hist42", "res9", 201, -100.5, 100.5, 300, -3, 3);
  new TH2F("hist45", "res8", 201, -100.5, 100.5, 300, -3, 3);
  new TH2F("hist46", "res9", 201, -100.5, 100.5, 300, -3, 3);

  new TH1F("hist51", "1/R", 201, -100.5, 100.5);
  new TH1F("hist52", "1/R", 201, -100.5, 100.5);
  new TH1F("hist53", "1/R", 201, -100.5, 100.5);
  new TH3F("hist54", "1/R", 9, 0.5, 9.5, 100, -50, 50, 201, -100.5, 100.5);
}

void fill(Int_t hid, Double_t x, Double_t y, Double_t z)
{
  TObject *obj = gROOT->FindObject(Form("hist%d", hid));
  if (!obj) return;

  TString scl = obj->ClassName();
  if (scl == "TH1F") ((TH1F *)obj)->Fill(x);
  if (scl == "TH2F") ((TH2F *)obj)->Fill(x, y);
  if (scl == "TH3F") ((TH3F *)obj)->Fill(x, y, z);
}

Double_t fit(Int_t *tkid, Int_t *imlt, Float_t *xcog, Float_t *ycog)
{
  Double_t zhit[NL];
  Int_t nhit = 0;
  Int_t nmin = 6;

  Double_t errx =  40e-4;
  Double_t erry =  15e-4;
  Double_t errz = 300e-4;

  for (Int_t i = 0; i < NL; i++) {
    Int_t ily = TMath::Abs(tkid[i])/100-1;
    if (0 <= ily && ily < NL) {
      zhit[i] = TkDBc::Head->GetZlayer(ily+1);
      nhit++;
    }
    else zhit[i] = -999;
  }
  if (nhit < nmin) return -1;

  Int_t idx[NL];
  TMath::Sort<Double_t,Int_t>(NL, zhit, idx);

  TrFit::_mscat = 0;

  TrFit trfit;

  TrFit ttmp;
  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0 || xcog[j] < 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], xcog[j], imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j], ycog[j], imlt[j]), 0);
    AMSPoint err(errx, erry, errz);
    ttmp.Add(TkCoo::GetGlobalA(tkid[j], loc), err);
  }
  if (ttmp.LinearFit() <  0) return -1;

  ttmp.SetRigidity(0);
  for (Int_t i = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0) continue;
    Int_t ily = TMath::Abs(tkid[j])/100-1;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid[j], TMath::Abs(xcog[j]), imlt[j]),
		 TkCoo::GetLocalCoo(tkid[j],            ycog[j],  imlt[j]), 0);
    AMSPoint err(((xcog[j] > 0) ? errx : -1), erry, errz);

    AMSPoint coo = TkCoo::GetGlobalA(tkid[j], loc);
    if (xcog[j] < 0) {
      ttmp.Propagate(coo.z());
      coo[0] = ttmp.GetP0x();
    }

    trfit.Add(coo, err);
  }

  TrFit::_mscat = 0;
  if (trfit.AlcarazFit ()  < 0) return -1;
  if (trfit.GetRigidity() == 0) return -1;

  Double_t rgtn = trfit.GetRigidity();

  TrFit::_mscat = 1;
  if (trfit.AlcarazFit ()  < 0) return -1;
  if (trfit.GetRigidity() == 0) return -1;

  Double_t rgt  = trfit.GetRigidity();
  Double_t argt = TMath::Abs(rgt);
  Double_t csqx = trfit.GetChisqX()/trfit.GetNdofX();
  Double_t csqy = trfit.GetChisqY()/trfit.GetNdofY();
  Double_t lr   = TMath::Log10(argt);
  Double_t csrx = 1;
  Double_t csry = 1;
  Double_t rsr8 = 1.3*TMath::Power(argt, -1.1);
  Double_t rsr9 = 0.7*TMath::Power(argt, -1.1);

  fill(10, TMath::Abs(rgtn), argt);
  fill(11, argt, csqx);
  fill(12, argt, csqy);
  fill(13, argt, csqx/csrx);
  fill(14, argt, csqy/csry);
  fill(31, 1e3/rgt, csqx/csrx);
  fill(32, 1e3/rgt, csqy/csry);
  fill(51, 1e3/rgt);

  Double_t cthd = 5-5*(lr-2);
  if (cthd < 3) cthd = 3;
  if (csqx/csrx > cthd || csqy/csry > cthd) return -1;

  fill(15, argt, csqx);
  fill(16, argt, csqy);
  fill(35, 1e3/rgt, csqx/csrx);
  fill(36, 1e3/rgt, csqy/csry);
  fill(52, 1e3/rgt);

  Int_t i8 = -1, i9 = -1;

  for (Int_t i = 0, k = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0) continue;
    Int_t layer = TMath::Abs(tkid[j])/100;
    if (layer == 8) i8 = k;
    if (layer == 9) i9 = k;
    k++;
  }

  Double_t ry8 = 0;
  Double_t ry9 = 0;
  trfit.SetErr(i8,    0,    0, errz);
  trfit.SetErr(i9, errx, erry, errz);
  if (trfit.AlcarazFit() > 0) ry8 = trfit.GetYr(i8);

  trfit.SetErr(i8, errx, erry, errz);
  trfit.SetErr(i9,    0,    0, errz);
  if (trfit.AlcarazFit() > 0) ry9 = trfit.GetYr(i9);

  if (ry8 == 0 || ry9 == 0) return -1;

  fill(21, argt, ry8*1e4);
  fill(22, argt, ry9*1e4);
  fill(23, argt, ry8/rsr8);
  fill(24, argt, ry9/rsr9);
  fill(41, 1e3/rgt, ry8/rsr8);
  fill(42, 1e3/rgt, ry9/rsr9);

  Double_t rthd = 1-(lr-2);
  if (rthd < 0.5) rthd = 0.5;

  if (TMath::Abs(ry8/rsr8) > rthd || TMath::Abs(ry9/rsr9) > rthd) return -1;

  if (argt > 2e3) cout << Form("Check: %6.2f %5.2f %5.2f %6.3f %6.3f",
			       rgt*1e-3, csqx/csrx, csqy/csry,
			       ry8/rsr8, ry9/rsr9) << endl;

  fill(25, argt, ry8*1e4);
  fill(26, argt, ry9*1e4);
  fill(27, argt, ry8/rsr8);
  fill(28, argt, ry9/rsr9);
  fill(45, 1e3/rgt, ry8/rsr8);
  fill(46, 1e3/rgt, ry9/rsr9);
  fill(53, 1e3/rgt);

  for (Int_t i = 0, k = 0; i < nhit; i++) {
    Int_t j = idx[i];
    if (tkid[j] == 0) continue;
    Int_t layer = TMath::Abs(tkid[j])/100;
    fill(54, layer, trfit.GetYh(k++), 1e3/rgt);
  }

  return argt;
}
