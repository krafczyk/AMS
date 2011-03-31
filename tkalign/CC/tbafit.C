// $Id: tbafit.C,v 1.2 2011/03/31 10:10:04 haino Exp $
#include "tkalign.C"
#include "MagField.h"
#include "TkSens.h"

#include "amschain.h"
#include "root.h"

#include "TStopwatch.h"
#include "TSystem.h"

#include "TH3.h"
#include "TF1.h"

#include <fstream>

void salig(TH3 *hist);

Int_t init(const char *tkdbc, Double_t rng)
{
  if (tkdbc && tkdbc[0] != '0') {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(3, tkdbc);
  }
  if (!TkDBc::Head) {
    cerr << "Error: TkDBc not found" << endl;
    return -1;
  }

  TString sfn = "tksens.dat";
  ifstream ftmp(sfn);
  if (ftmp.good())
    TkDBc::Head->readAlignmentSensor(sfn);

  TString smf = gSystem->ExpandPathName("$AMSDataDir/v5.00"
					"/MagneticFieldMapPermanent_NEW.bin");
  MagField::GetPtr()->Read(smf);
  MagField::GetPtr()->SetScale(1);
  MagField::GetPtr()->SetMagstat(1);

  TkAlign::InitShists(rng);
  TkAlign::InitMap(rng);
  TkAlign::InitPar();

  return 0;
}

Bool_t tbchk(Int_t tbs, Int_t *tkid)
{
  for (Int_t i = 0; i < TkAlign::NLAY; i++)
    TkAlign::fErrX[i] = TkAlign::fErrY[i] = TkAlign::fErrZ[i];

  // TRACKER416 Layer 1N-9
  if (tbs == 1) {
    if (tkid[7] == 0 || tkid[8] == 0) return kFALSE;
  }
  // TRACKER80  Layer  2-9 (no 1N)
  else if (tbs == 2) {
    if (tkid[8] == 0) return kFALSE;
    TkAlign::fErrX[7] = TkAlign::fErrY[7] = 0;
  }
  // TRACKER280 Layer 1N-7 (no 9)
  else if (tbs == 3) {
    if (tkid[7] == 0) return kFALSE;
    TkAlign::fErrX[8] = TkAlign::fErrY[8] = 0;
  }
  // TRACKER60
  else if (tbs == 4 || tbs == 5 || tbs == 6) {
    TkAlign::fNmin = 5;
    TkAlign::fErrX[7] = TkAlign::fErrX[8] = 0;
    TkAlign::fErrY[7] = TkAlign::fErrY[8] = 0;
  }

  // TRACKER60B
  if (tbs == 6) {
    TkAlign::fRref = -400;
    TkAlign::fRfix = -400;
  }

  // ECAL7
  if (tbs == 7) {
    if (tkid[7] == 0 || tkid[8] == 0) return kFALSE;
    TkAlign::fRref = 120;
  }


  return kTRUE;
}

void copyproc(Int_t bits, const char *tkdbc,
	      Float_t rng = 0.01,
	      const char *fname = "tbafit.root",
	      Int_t check = -1)
{
  TFile f(fname);
  if (!f.IsOpen()) return;

  TFile of("tbafit.root.cp", "recreate");
  if (init(tkdbc, rng) < 0) return;

  TkAlign::CopyShists(f);

  if (check >= 0) {
    TkAlign::fCheckID = check;
    TkAlign::fFile    = &of;
  }
  if (bits == TkAlign::kSensY)
    salig((TH3 *)of.Get("hist4"));
  else if (bits > 0)
    TkAlign::Proc(bits);

  of.Write();
  TkDBc::Head->write("tkdbc.dat.new");
}

Int_t TBNpos[7] = { 416, 80, 280, 60, 60, 60, 7 };

void tbafit(Int_t bits, const char *tkdbc, 
	    Float_t rng = 0.01,
	    const char *fname = "tbafit.dat",
	    Int_t selpos = -1)
{
  ifstream fin(fname);
  if (!fin) {
    cerr << "File not found: " << fname << endl;
    return;
  }

  if (bits == 0) TkAlign::fRfix = 0;

  Int_t tbid;
  fin.read((char*)&tbid, sizeof(tbid));
  Int_t tbs = tbid/1000;
  TkAlign::fTbNpos = TBNpos[tbs-1];
  TkAlign::fTbPos  = tbid%1000;

  cout << "TbNpos= " << TkAlign::fTbNpos << endl;

  Bool_t first = kTRUE;

  TString sfn = fname;
  sfn.ReplaceAll("dat", "root");
  if (sfn == fname) sfn = "tbafit.root";

  TFile of(sfn, "recreate");
  if (init(tkdbc, rng) < 0) return;

  enum { NL = TkAlign::NLAY };

  Int_t   tkid[NL], imlt[NL], ibuf[NL];
  Float_t xcog[NL], ycog[NL];

  Int_t nr = 0, nf = 0, intv = 100000;
  Double_t csum = 0;

  TStopwatch timer;
  timer.Start();

  for (;;) {
    if (!first)
    fin.read((char*)&tbid, sizeof(tbid));
    fin.read((char*) ibuf, sizeof(ibuf));
    fin.read((char*) xcog, sizeof(xcog));
    fin.read((char*) ycog, sizeof(ycog));
    if (fin.eof()) break;

    first = kFALSE;

    if (nr > 0 && nr%intv == 0) {
      Double_t cc = (nf > 0) ? csum/nf : 0;
      Double_t rt = timer.RealTime();
      Double_t ct = timer.CpuTime();
      timer.Continue();
      cout << Form("%7d %7d %7.3f %6.1f %6.1f", nf, nr, cc, rt, ct) << endl;
    }
    nr++;

    Int_t tbs        = tbid/1000;
    TkAlign::fTbNpos = TBNpos[tbs-1];
    TkAlign::fTbPos  = tbid%1000;
    if (selpos >= 0 && TkAlign::fTbPos != selpos) continue;

    for (Int_t i = 0; i < NL; i++) {
      tkid[i] = TMath::Sign(TMath::Abs(ibuf[i])/100, ibuf[i]);
      imlt[i] =             TMath::Abs(ibuf[i])%100;
    }

    if (!tbchk(tbs, tkid)) continue;

    if (tbs == 4) TkAlign::fNmin = 5;      
    if (TkAlign::fRfix == 0 && TkAlign::fTbPos == 0) continue;

    Double_t csq = TkAlign::Fit(tkid, imlt, xcog, ycog);
    if (csq > 0) {
      nf++;
      csum += csq;
    }
  }

  if (nf > 0) {
    cout << Form("%7d %7d %7.3f", nf, nr, csum/nf) << endl;

    if (bits == TkAlign::kSensY)
      salig((TH3 *)of.Get("hist4"));
    else if (bits > 0)
      TkAlign::Proc(bits);
  }

  of.Write();

  if (bits > 0)
    TkDBc::Head->write("tkdbc.dat.new");
}

Int_t hcont(TDirectory *dir, Int_t hid, Double_t x, Double_t y)
{
  TH2F *hist = (TH2F *)dir->Get(Form("hist%d", hid));
  if (hist) {
    Int_t ibx = hist->GetXaxis()->FindBin(x);
    Int_t iby = hist->GetYaxis()->FindBin(y);
    return (Int_t)hist->GetBinContent(ibx, iby);
  }
  return 0;
}

Int_t hfill(TDirectory *dir, Int_t hid, Double_t x, Double_t y)
{
  TH2F *hist = (TH2F *)dir->Get(Form("hist%d", hid));
  if (hist) return hist->Fill(x, y);
  return 0;
}

void tbafit(const char *fname, const char *tkdbc, Int_t bits, 
	                                          Int_t nmax = 5000)
{
  TChain ch("tree");
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;
  cout << "Ntr,Nent= " << ntr << " " << nent << endl;

  for (Int_t i = 0; i < ntr; i++)
    cout << ch.GetListOfFiles()->At(i)->GetTitle() << endl;

  TFile of("tbafit.root", "recreate");
  if (init(tkdbc, 0.02) < 0) return;

  of.cd();
  Int_t tbs = 0;
  Int_t by1 = 0;
  Int_t by2 = 0;
  TH2F *hist1 = 0;
  new TH2F("hist21", "Y1 VS X1", 280, -70, 70, 280, -70, 70);
  new TH2F("hist22", "Y4 VS X4", 280, -70, 70, 280, -70, 70);
  new TH2F("hist23", "Y7 VS X7", 280, -70, 70, 280, -70, 70);
  new TH2F("hist24", "Y8 VS X8", 280, -70, 70, 280, -70, 70);
  new TH2F("hist25", "Y9 VS X9", 280, -70, 70, 280, -70, 70);
  new TH2F("hist31", "Slot1N VS Slot9", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist32", "Slot1  VS Slot9", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist33", "Slot1N VS Slot7", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist34", "Slot1  VS Slot7", 29, -14.5, 14.5, 29, -14.5, 14.5);

  enum { NID = 31, NFD = 67 };

  Int_t   idata[NID];
  Float_t fdata[NFD];

  Int_t brun = -1;
  Int_t nfill = 0;

  ofstream fout("tbafit.dat");
  if (!fout) return;

  TStopwatch timer;
  timer.Start();

  Double_t csum = 0;
  for (Int_t ient = 0; ient < nent; ient++) {
    Int_t ent = ch.LoadTree(ient);

    TBranch *br1 = ch.GetBranch("idata");
    br1->SetAddress(&idata);
    br1->GetEntry(ent);

    Int_t run = idata[0];
    if (run != brun) {
      Double_t rt = timer.RealTime();
      Double_t ct = timer.CpuTime();
      Double_t cs = (nfill > 0) ? csum/nfill : 0;
      timer.Continue();
      cout << Form("%8d %8d (%4.1f %%) %d %7.3f %6.1f %6.1f",
		   nfill, ient, 100.*ient/nent, run, cs, rt, ct) << endl;
    }
    brun = run;

    if (!hist1) {
      TString sfn = ch.GetFile()->GetName();
      if (sfn.Contains("TRACKER416")) tbs = 1;
      if (sfn.Contains("TRACKER80" )) tbs = 2;
      if (sfn.Contains("TRACKER280")) tbs = 3;
      if (sfn.Contains("TRACKER60/")) tbs = 4;
      if (sfn.Contains("TRACKER60A")) tbs = 5;
      if (sfn.Contains("TRACKER60B")) tbs = 6;
      if (sfn.Contains("ECAL7"))      tbs = 7;
      if (tbs == 0) {
	cerr << "File name doesn't contain TRACKER**" << endl;
	return;
      }

      if (tbs == 7) nmax = (Int_t)1e6;

      Int_t np = TBNpos[tbs-1];
      hist1 = new TH2F(Form("hist10%d", tbs), Form("TR%d", np),
		       np+1, 0.5, np+1.5, 300, -20, 10);
      by1 = hist1->GetYaxis()->FindBin(0.);
      by2 = hist1->GetNbinsY();
    }

    Int_t tbpos = idata[12];
    Int_t npos  = TBNpos[tbs-1];
    Int_t ibin  = hist1->GetXaxis()->FindBin(tbpos);
    if (tbpos <= npos && 
	hist1->Integral(ibin, ibin, by1, by2) > nmax) continue;

    Int_t *tkid = &idata[13];
    if (!tbchk(tbs, tkid)) continue;

    TBranch *br2 = ch.GetBranch("track");
    br2->SetAddress(&fdata);
    br2->GetEntry(ent);

    Bool_t sel = (bits == -405) ? kFALSE : kTRUE;

    Int_t tslot[7] = { 0, 0, 0, 0, 0, 0 };
    for (Int_t i = 0; i < TkAlign::NLAY; i++) {
      Int_t tkid  = idata[13+i];
      Int_t slot  = tkid%100;
      Int_t layer = TMath::Abs(tkid)/100;
      if (layer == 8) tslot[0] = slot;
      if (layer == 9) tslot[1] = slot;
      if (layer == 1) tslot[2] = slot;
      if (layer == 7) tslot[3] = slot;
      if (layer == 4) tslot[4] = slot;
      if (layer == 5) tslot[5] = slot;

      // For Ladder 405 alignment
      Float_t *ycog = &fdata[58];
      if (bits == -405 && tkid == 405 && ycog[i] > 0) {
	ycog[i] *= -1;
	sel = kTRUE;
      }
    }

    if (!sel) continue;

    Int_t npmax = nmax/2;
    Int_t nbchk = 0;

    Int_t ilid[5] = { 0, 3, 6, 7, 8 };
    for (Int_t i = 0; i < 5; i++)
      if (hcont(&of, 21+i, fdata[7+ilid[i]], fdata[16+ilid[i]]) > npmax)
	nbchk++;

    if (tbpos > npos && nbchk >= 2) continue;

    nbchk = 0;
    if (hcont(&of, 31, tslot[1], tslot[0]) > npmax*2) nbchk++;
    if (hcont(&of, 32, tslot[1], tslot[2]) > npmax*2) nbchk++;
    if (hcont(&of, 33, tslot[3], tslot[0]) > npmax*2) nbchk++;
    if (hcont(&of, 34, tslot[3], tslot[2]) > npmax*2) nbchk++;
    if (tbpos > npos && nbchk >= 2) continue;

    TkAlign::fTbNpos = TBNpos[tbs-1];
    TkAlign::fTbPos  = tbpos;

    Double_t csq = TkAlign::Fit(&idata[13], &idata[22], 
				&fdata[49], &fdata[58]);
    if (csq > -10) hist1->Fill(tbpos, csq);
    else hist1->Fill(tbpos, -TMath::Log10(-csq)-10);

    if (csq > 0) {
      csum += csq;

      Int_t tbid = tbs*1000+tbpos;
      fout.write((char *)&tbid, sizeof(tbid));

      Int_t ibuf[TkAlign::NLAY];
      for (Int_t i = 0; i < TkAlign::NLAY; i++) {
	Int_t tkid = idata[13+i];
	Int_t imlt = idata[22+i];
	ibuf[i] = TMath::Sign(TMath::Abs(tkid)*100+imlt, tkid);
      }
      fout.write((char *)ibuf, sizeof(ibuf));
      fout.write((char *)&fdata[49], 4*TkAlign::NLAY);
      fout.write((char *)&fdata[58], 4*TkAlign::NLAY);

      for (Int_t i = 0; i < 5; i++)
	hfill(&of, 21+i, fdata[7+ilid[i]], fdata[16+ilid[i]]);

      hfill(&of, 31, tslot[1], tslot[0]);
      hfill(&of, 32, tslot[1], tslot[2]);
      hfill(&of, 33, tslot[3], tslot[0]);
      hfill(&of, 34, tslot[3], tslot[2]);

      nfill++;
    }
  }
  if (nfill > 0) {
    cout << Form("%8d %7.3f", nfill, csum/nfill) << endl;
    if (bits > 0) TkAlign::Proc(bits);
  }

  of.Write();
  TkDBc::Head->write("tkdbc.dat.new");

  return;
}

void salig(TH3 *hist)
{
  if (!TkDBc::Head) {
    cout << "TkDBc not found" << endl;
    return;
  }

  gROOT->cd();

  Int_t nbx = hist->GetNbinsX();
  Int_t nby = hist->GetNbinsY();
  Int_t nbz = hist->GetNbinsZ();

  TAxis *ax = hist->GetXaxis();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  TH1F *hprj = new TH1F("hprj", "Res(Y)", 
			nbz, az->GetXmin(), az->GetXmax());
  for (Int_t i = 0; i < nbz; i++)
    hprj->SetBinContent(i+1, hist->Integral(1, nbx, 1, nby, i+1, i+1));

  hprj->Fit("gaus", "q0");

  TF1 *func = hprj->GetFunction("gaus");
  Double_t sigma = func->GetParameter(2);
  cout << "Sensor alignment: SigmaY= " << sigma*1e4 << endl;

  Double_t range = sigma*2.5;

  TH1F *htmp = new TH1F("htmp", "htmp", 
			nbz, az->GetXmin(), az->GetXmax());

  TH1F *hfit = new TH1F("hfit", "salig", 140, -35, 35);

  Double_t nthd = 500;

  ofstream fout("salig.dat");
  if (!fout) return;

  for (Int_t i = 0; i < nbx; i++)
    for (Int_t j = 0; j < nby; j++) {
      for (Int_t k = 0; k < nbz; k++)
	htmp->SetBinContent(k+1, hist->GetBinContent(i+1, j+1, k+1));

      htmp->SetEntries(htmp->GetSumOfWeights());

      if (htmp->GetSumOfWeights() < nthd) continue;

      Double_t rng;
      Double_t mean = 0, mtmp = 0;
      for (Int_t k = 0; k < 10; k++) {
	rng = range*k;
	htmp->Fit("gaus", "q0", "", -rng, rng);
	func = htmp->GetFunction("gaus");

	if (TMath::Abs(func->GetParameter(1)) < rng/2) {
	  mean = func->GetParameter(1);
	  break;
	}
      }
      if (rng > range*1.5) {
	htmp->Fit("gaus", "q0", "", mean-range, mean+range);
	func = htmp->GetFunction("gaus");

	mtmp = mean;
	mean = func->GetParameter(1);
	if (mean < mtmp-range || mtmp+range < mean) mean = mtmp;
      }

      Int_t layer = (i+1)/32+1;
      Int_t slot  =  i+1-(layer-1)*32-16;
      Int_t side  = (slot >= 0) ? 1 : 0;
      Int_t tkid  =  TMath::Sign(layer*100+TMath::Abs(slot+side), slot+side);

      TkLadder *lad = TkDBc::Head->FindTkId(tkid);
      if (!lad) {
	cout << "Error Ladder not exists: " << tkid << " "
	     << i+1 << " " << layer << " " << slot << " " << side << endl;
	continue;
      }
      if (j+1 > lad->GetNSensors()) {
	cout << "Wrong sensor: " << j+1 << " " << lad->GetNSensors() << endl;
	continue;
      }
      fout << Form("%3d %4d %2d %5.1f %5.1f %5.0f",
		   i, tkid, j+1, mean*1e4, lad->_sensy[j]*1e4, 
		   htmp->GetSumOfWeights())
	   << endl;
      lad->_sensy[j] += mean;

      hfit->Fill(mean*1e4);
    }

  ofstream fsen("tksens.dat.new");
  if (!fsen) return;

  enum { maxsen = 15 };

  Int_t nent = TkDBc::Head->GetEntries();
  for (Int_t i = 0; i < nent; i++) {
    TkLadder *lad = TkDBc::Head->GetEntry(i);
    if (!lad) continue;

    fsen << Form("%4d", lad->GetTkId());
    for (Int_t j = 0; j < maxsen; j++) fsen << Form(" %8.5f", lad->_sensx[j]);
    fsen << endl << "    ";
    for (Int_t j = 0; j < maxsen; j++) fsen << Form(" %8.5f", lad->_sensy[j]);
    fsen << endl;
  }
}

/*
  ntpl->Branch("idata", idata, "run/I:event/I:ient/I:time/I:"  //  0-4
	                       "ncls/I:nhit/I:ntrk/I:"         //  4-7
	                       "ntrd/I:nanti/I:tof/I:"         //  7-10
	                       "nhity/I:nhitc/I:tbpos/I:"      // 10-13
	                       "tkid[9]/I:imlt[9]/I");         // 13-31
  ntpl->Branch("track", fdata, 
	       "p0x/F:p0y/F:angx/F:angy/F:th/F:ph/F:plen/F:"   //  0- 7
	       "trkx[9]/F:trky[9]/F:"                          //  7-25
	       "dbx[3]/F:dby[3]/F:dth/F:dst/F:"                // 25-33
	       "csqx[4]/F:csqy[4]/F:"                          // 33-41
	       "rgt[4]/F:erinv[4]/F:"                          // 41-49
	       "xcog[9]/F:ycog[9]/F");                         // 49-67
*/

