// $Id: pmafit.C,v 1.5 2011/06/17 00:30:49 shaino Exp $
#include "tkalign.C"
#include "MagField.h"

#include "TChain.h"
#include "TMath.h"
#include "TH3.h"
#include "TF1.h"

#include "TStopwatch.h"

#include <fstream>
#include <signal.h>

Int_t atoixb(TString str)
{
  Int_t sign = 1;
  if (str.BeginsWith("-")) {
    sign = -1;
    str.ReplaceAll("-", "");
  }

  str.ToLower();

  if (str.BeginsWith("0b")) {
    Int_t bits = 0;
    for (Int_t i = 2; i < str.Length(); i++) {
      bits = bits*2;
      if (str[i] == '1') bits |= 1;
    }

    cout << str.Data() << " => " << bits << endl;
    return sign*bits;
  }

  if (str.BeginsWith("0x")) {
    Int_t bits = 0;
    for (Int_t i = 2; i < str.Length(); i++) {
      bits = bits*16;
      if ('0' <= str[i] && str[i] <= '9') bits |= str[i]-'0';
      if ('a' <= str[i] && str[i] <= 'a') bits |= str[i]-'a'+0xa;
    }
    return sign*bits;
  }

  return sign*str.Atoi();
}

static Int_t SigTERM = 0;

void handler(int sig)
{
  switch(sig){
  case SIGTERM: case SIGINT: 
    cerr << " SIGTERM intercepted" << endl;
    SigTERM = 1;
    break;
  }
}

Int_t init(const char *tkdbc, Double_t rng, Int_t &bits)
{
  if (tkdbc && tkdbc[0] == '0') tkdbc = 0;

  TkDBc::CreateTkDBc();
  TkDBc::Head->init(3, tkdbc);

  if (!TkDBc::Head) {
    cerr << "Error: TkDBc not found" << endl;
    return -1;
  }

  TString sfn = "tksens.dat";
  ifstream ftmp(sfn);
  if (ftmp.good())
    TkDBc::Head->readAlignmentSensor(sfn);

  TkAlign::fRfix = 0;
  TkAlign::fRref = 1;

  Int_t bsign = (bits > 0) ? 1 : -1;
  bits = TMath::Abs(bits);

  if (bits == 64 || bits == 65 || bits == 66 || bits == 67) {
    if (bits == 64) bits = 0x1030; // 0b01 0000 0011 0000
    if (bits == 65) bits = 0x1c30; // 0b01 1100 0011 0000
    if (bits == 66) bits = 0x1e30; // 0b01 1110 0011 0000
    if (bits == 67) bits = 0x1f30; // 0b01 1111 0011 0000
    PlAlign::fMode = 3;
    cout << "bits= " << bits << " mode= " << PlAlign::fMode << endl;
  }

  if ((bits & TkAlign::kLyFit) && 
      (bits & TkAlign::kPlFit))
    PlAlign::fMode = 2;

  if (bits & TkAlign::kPlFit) {
    TkAlign::fMsc  = 0;
    TkAlign::fNmin = 4;
    TkAlign::InitPar();

    PlAlign::fRange = rng;
    TkAlign::InitVec();
    TkAlign::InitShists(rng);

    if ((bits & 0x3f) ==    0 ||
        (bits & 0x30) == 0x30 || PlAlign::fMode == 2) {
      TkAlign::fErrX[7] = TkAlign::fErrX[8] = 0;
      TkAlign::fErrY[7] = TkAlign::fErrY[8] = 0;
      cout << "External planes set free"  << endl;
    }
    else
      cout << "External planes used for the fitting"  << endl;
  }
  else {
    TkAlign::fMsc  = (bits != TkAlign::kSensY) ? 2 : 0;
    TkAlign::fNmin = 6;
    TkAlign::InitPar();

    TkAlign::InitShists(rng);
    TkAlign::InitMap(rng);
  }

  TkAlign::fProfMode = 2;//3;

  if (bits & TkAlign::kPlFit) {
    for (Int_t i = 0; i < TkAlign::NLAY; i++) {
      if ((1 << (TkDBc::Head->_plane_layer[i]-1)) & bits) {
	TkAlign::fErrX[i] = TkAlign::fErrY[i] = 0;
	cout << "Layer " << i+1 << " excluded from fitting" << endl;
      }
    }
  }

  bits *= bsign;

  return 0;
}

void salig(TH3 *hist);

void copyproc(Int_t bits, const char *tkdbc,
	      Float_t rng = 0.01,
	      const char *fname = "pmafit.root",
	      Int_t check = -1)
{
  TFile f(fname);
  if (!f.IsOpen()) return;

  TFile of("pmafit.root.cp", "recreate");
  if (init(tkdbc, rng, bits) < 0) return;

  if (bits & TkAlign::kPlFit) TkAlign::CopyVec(f);

  TkAlign::CopyShists(f);

  if (check >= 0) {
    TkAlign::fCheckID = check;
    TkAlign::fFile    = &of;
  }
  if (bits == TkAlign::kSensY) {
    of.cd();
    salig((TH3 *)of.Get("hist4"));
  }

  else if (bits > 0)
    TkAlign::Proc(bits);

  of.Write();
  TkDBc::Head->write("tkdbc.dat.new");
}

Int_t hfill(TDirectory *dir, Int_t hid, Double_t x, Double_t y)
{
  TH2F *hist = (TH2F *)dir->Get(Form("hist%d", hid));
  if (hist) return hist->Fill(x, y);
  return 0;
}

Int_t hfill(TDirectory *dir, Int_t hid, Double_t x, Double_t y, Double_t z)
{
  TH3F *hist = (TH3F *)dir->Get(Form("hist%d", hid));
  if (hist) return hist->Fill(x, y, z);
  return 0;
}

void pmafit(Int_t bits, const char *tkdbc, 
	    Float_t rng = 0.01,
	    const char *fname = "pmafit.dat")
{
  ifstream fin(fname);
  if (!fin) {
    cerr << "File not found: " << fname << endl;
    return;
  }

  TString sfn = fname;
  sfn.ReplaceAll("dat", "root");
  if (sfn == fname) sfn = "pmafit.root";

  TFile of(sfn, "recreate");
  if (init(tkdbc, rng, bits) < 0) return;

  enum { NBX = 80, NBY = 100, NBR = 32 };
  Double_t bx[NBX+1], by[NBY+1], br[NBR+1];
  for (Int_t i =  0; i <= NBX; i++) bx[i] = TMath::Power(10, -1+i*0.05);
  for (Int_t i =  0; i <= NBY; i++) by[i] = TMath::Power(10, -3+i*0.05);
  for (Int_t i =  0; i <   20; i++) br[i] = TMath::Power(10, 0.10*i-1);
  for (Int_t i = 20; i <   30; i++) br[i] = TMath::Power(10, 0.10*i-1);
  for (Int_t i = 30; i <=  32; i++) br[i] = TMath::Power(10, 0.50*i-13);

  of.cd();
  new TH2F("hist31", "Rgt+", NBR, br, NBY, by);
  new TH2F("hist32", "Rgt-", NBR, br, NBY, by);

  enum { NL = TkAlign::NLAY };

  Int_t   tkid[NL], imlt[NL], ibuf[NL], run, ient;
  Float_t xcog[NL], ycog[NL];

  Int_t nr = 0, nf = 0, intv = 100000;
  Double_t csum = 0;

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
      Double_t cc = (nf > 0) ? csum/nf : 0;
      Double_t rt = timer.RealTime();
      Double_t ct = timer.CpuTime();
      timer.Continue();
      cout << Form("%7d %7d %7.3f %6.1f %6.1f", nf, nr, cc, rt, ct) << endl;
    }
    nr++;

    for (Int_t i = 0; i < NL; i++) {
      tkid[i] = TMath::Sign(TMath::Abs(ibuf[i])/100, ibuf[i]);
      imlt[i] =             TMath::Abs(ibuf[i])%100;

      if (tkid[i] == 405 || tkid[i] == -202) {
	if (TMath::Abs(bits) & TkAlign::kPlFit) {
	  tkid[i] = imlt[i] = 0;
	  xcog[i] = ycog[i] = 0;
	}
	else
	  ycog[i] = -ycog[i];
      }

      //if (TMath::Abs(tkid[i])/100 == 8 ||
      //    TMath::Abs(tkid[i])/100 == 9) ycog[i] = -ycog[i];
    }

    if (bits == 0 && (tkid[7] == 0 || tkid[8] == 0)) continue;

    Double_t csq = TkAlign::Fit(tkid, imlt, xcog, ycog);
    if (csq > 0) {
      nf++;
      csum += csq;

      Double_t rfit = TkAlign::fRfit;
      if (rfit > 0) hfill(&of, 31,  rfit, csq);
      if (rfit < 0) hfill(&of, 32, -rfit, csq);
    }
  }

  if (nf > 0) {
    cout << Form("%7d %7d %7.3f", nf, nr, csum/nf) << endl;

    if (bits > 0) {
      if (bits == TkAlign::kSensY) {
	of.cd();
	salig((TH3 *)of.Get("hist4"));
      }
      else
	TkAlign::Proc(bits);
    }
  }

  of.Write();

  if (bits > 0)
    TkDBc::Head->write("tkdbc.dat.new");
}

void pmafit(const char *fname, const char *tkdbc, Int_t bits = -1)
{
  TChain ch("tree");
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;
  cout << "Ntr,Nent= " << ntr << " " << nent << endl;

  if (ntr <= 10) {
    for (Int_t i = 0; i < ntr; i++)
      cout << ch.GetListOfFiles()->At(i)->GetTitle() << endl;
  }

  Int_t fsel = (bits < 0) ? -bits : 0;
  Int_t qsel = fsel & 0xff;
  cout << "bits/fsel/qsel= " << bits << " " << fsel << " " << qsel << endl;

  sfn.ReplaceAll("pmalign_", "pmafit");
  sfn.ReplaceAll("*", "");
  if (sfn == fname) sfn = "pmafit.root";

  signal(SIGTERM, handler);
  signal(SIGINT,  handler);

  TFile of(sfn, "recreate");
  Int_t bb = 0;
  if (init(tkdbc, 0.02, bb) < 0) return;

  of.cd();

  enum { NBX = 80, NBY = 100, NBR = 32 };
  Double_t bx[NBX+1], by[NBY+1], br[NBR+1];
  for (Int_t i =  0; i <= NBX; i++) bx[i] = TMath::Power(10, -1+i*0.05);
  for (Int_t i =  0; i <= NBY; i++) by[i] = TMath::Power(10, -2+i*0.05);
  for (Int_t i =  0; i <   20; i++) br[i] = TMath::Power(10, 0.10*i-1);
  for (Int_t i = 20; i <   30; i++) br[i] = TMath::Power(10, 0.10*i-1);
  for (Int_t i = 30; i <=  32; i++) br[i] = TMath::Power(10, 0.50*i-13);

  new TH2F("hist11", "csqX VS Rgt", NBX, bx, NBY, by);
  new TH2F("hist12", "csqY VS Rgt", NBX, bx, NBY, by);
  new TH2F("hist13", "csqX VS Rgt", NBX, bx, NBY, by);
  new TH2F("hist14", "csqY VS Rgt", NBX, bx, NBY, by);

  new TH2F("hist21", "Y1 VS X1", 280, -70, 70, 280, -70, 70);
  new TH2F("hist22", "Y4 VS X4", 280, -70, 70, 280, -70, 70);
  new TH2F("hist23", "Y7 VS X7", 280, -70, 70, 280, -70, 70);
  new TH2F("hist24", "Y8 VS X8", 280, -70, 70, 280, -70, 70);
  new TH2F("hist25", "Y9 VS X9", 280, -70, 70, 280, -70, 70);

  new TH2F("hist31", "Rgt+", NBR, br, NBY, by);
  new TH2F("hist32", "Rgt-", NBR, br, NBY, by);

  new TH3F("hist41", "Z= 1 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist42", "Z= 2 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist43", "Z> 2 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist44", "Z=-1 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist45", "Z= 1 Up", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);

  Double_t bin[31];
  for (Int_t i = 0; i <= 30; i++) bin[i] = TMath::Power(10, i*0.10-1);
  new TH2F("hist51", "Z= 1 Down", 30, bin, 160, -80, 80);
  new TH2F("hist52", "Z= 2 Down", 30, bin, 160, -80, 80);
  new TH2F("hist53", "Z> 2 Down", 30, bin, 160, -80, 80);
  new TH2F("hist54", "Z=-1 Down", 30, bin, 160, -80, 80);
  new TH2F("hist55", "Z= 1 Up",   30, bin, 160, -80, 80);

  enum { NID = 31, NFD = 67 };
  Int_t   idata[NID];
  Float_t fdata[NFD];

  Int_t brun  = -1;
  Int_t nsel  = 0;
  Int_t nfill = 0;

  sfn.ReplaceAll("root", "dat");
  ofstream fout(sfn);
  if (!fout) return;

  TStopwatch timer;
  timer.Start();

  Double_t csum = 0;
  for (Int_t ient = 0; !SigTERM && ient < nent; ient++) {
    Int_t ent = ch.LoadTree(ient);

    TBranch *br1 = ch.GetBranch("idata");
    br1->SetAddress(&idata);
    br1->GetEntry(ent);

    Int_t run = idata[0];
    if ((run != brun && brun > 0) || ient+1 == nent) {
      Double_t rt = timer.RealTime();
      Double_t ct = timer.CpuTime();
      Double_t cs = (nfill > 0) ? csum/nfill : 0;
      timer.Continue();

      if (bits > 0) 
	cout << Form("%8d %8d %8d (%5.1f %%) %d %7.3f %6.1f %6.1f",
		     nfill, nsel, ient, 100.*ient/nent, 
		     brun, cs, rt, ct) << endl;
      else
	cout << Form("%8d %8d %8d (%5.1f %%) %d %6.1f %6.1f",
		     nfill, nsel, ient, 100.*ient/nent, brun, rt, ct) << endl;
    }
    brun = run;

    Int_t fpat = idata[12];
    if (fpat < 1) continue;

    Int_t *tkid = &idata[13];
    if ((fsel & 0x100) && tkid[7] == 0 && tkid[8] == 0) continue;

    TBranch *br2 = ch.GetBranch("track");
    br2->SetAddress(&fdata);
    br2->GetEntry(ent);

    Double_t  beta = fdata[2];
    Double_t  chrg = fdata[3];
    Double_t  tht  = fdata[4];
    Double_t  phi  = fdata[5];
    Double_t  glat = fdata[6];
    Double_t  rgt  = fdata[33];
    Double_t  argt = TMath::Abs(rgt);
    Double_t  csqx =  fdata[25];
    Double_t  csqy =  fdata[29];
    Float_t  *trkx = &fdata[ 7];
    Float_t  *trky = &fdata[16];

    AMSDir dir(tht, phi);
    Double_t dydz = dir.y()/dir.z();

    if (beta == 0) continue;
    if (csqx < 0 || csqy < 0 || argt ==0) continue;

    hfill(&of, 11, argt, csqx);
    hfill(&of, 12, argt, csqy);
    if (argt < 0.3 || csqx > 20 || csqy > 20) continue;

    Int_t qid = 1;
    if      (chrg > 3.0*std::sqrt(1.2*1.2/rgt/rgt+1)) qid = 3;
    else if (chrg > 1.6*std::sqrt(1.5*1.5/rgt/rgt+1)) qid = 2;

    if (qid == 1) {
      if (beta > 0 && rgt < 0) qid = 4;
      if (beta < 0)            qid = 5;

      if (qsel == 11 || qsel == 21) {
	if (TMath::Abs(glat) > 50) continue;

	Int_t rsel = (argt > 8 ||
		      TMath::Abs(glat) > 50-10*TMath::Log(argt)-argt) ? 1 : 2;
	if (qsel/10 != rsel) continue;
      }
    }

    if (qsel > 0 && qsel%10 != qid) continue;
    nsel++;

    Double_t csq = TkAlign::Fit(&idata[13], &idata[22],
				&fdata[41], &fdata[50]);

    if (csq > 0) {
      csum += csq;

      hfill(&of, 13, argt, csqx);
      hfill(&of, 14, argt, csqy);

      Double_t rfit = TkAlign::fRfit;
      if (rfit > 0) hfill(&of, 31,  rfit, csq);
      if (rfit < 0) hfill(&of, 32, -rfit, csq);

      for (Int_t i = 0; i < TkAlign::NLAY; i++) {
	Int_t tkid = idata[13+i];
	if (tkid == 0) continue;

	Int_t layr = TMath::Abs(tkid)/100;
	Int_t slot = tkid%100;
	hfill(&of, 40+qid, layr, slot, 1/rfit);
      }
      hfill(&of, 50+qid, argt, glat);

      Int_t run  = idata[0];
      Int_t ient = idata[2];

      Int_t ibuf[TkAlign::NLAY];
      for (Int_t i = 0; i < TkAlign::NLAY; i++) {
	Int_t tkid = idata[13+i];
	Int_t imlt = idata[22+i];
	ibuf[i] = TMath::Sign(TMath::Abs(tkid)*100+imlt, tkid);
      }

      if (TMath::Abs(trkx[8]) > 33 || TMath::Abs(trky[8]) > 33) {
	ibuf[8] = 0;
	fdata[41+8] = fdata[50+8] = 0;
      }

      fout.write((char *)&run,  sizeof(run));
      fout.write((char *)&ient, sizeof(ient));
      fout.write((char *) ibuf, sizeof(ibuf));
      fout.write((char *)&fdata[41], 4*TkAlign::NLAY);
      fout.write((char *)&fdata[50], 4*TkAlign::NLAY);

      Int_t ilid[5] = { 0, 3, 6, 7, 8 };
      for (Int_t i = 0; i < 5; i++)
	hfill(&of, 21+i, fdata[7+ilid[i]], fdata[16+ilid[i]]);

      nfill++;
    }
  }
  if (nfill > 0 && bits > 0) {
    cout << Form("%8d %7.3f", nfill, csum/nfill) << endl;
    if (bits > 0) TkAlign::Proc(bits);
  }

  of.Write();
  if (bits > 0)
    TkDBc::Head->write("tkdbc.dat.new");

  return;
}

void writes(const char *fname = "tksens.dat.new")
{
  ofstream fsen(fname);
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

void salig(TH3 *hist)
{
  if (!TkDBc::Head) {
    cout << "TkDBc not found" << endl;
    return;
  }

  TH2F *prof = TkAlign::Profile(hist);
  TH1F *hprj = new TH1F("hprj", "salig", 200, -50, 50);
  TH1F *hfit = new TH1F("hfit", "salig", 32*9*15, 0.5, 9.5);

  ofstream fout("salig.dat");
  if (!fout) return;

  Int_t nbx = prof->GetNbinsX();
  Int_t nby = prof->GetNbinsY();

  for (Int_t i = 0; i < nbx; i++)
    for (Int_t j = 0; j < nby; j++) {
      if (prof->GetBinError(i+1, j+1) <= 0) continue;

      Double_t mean = prof->GetBinContent(i+1, j+1);

      Int_t layer = (i+1)/32+1;
      Int_t slot  =  i+1-(layer-1)*32-16;
      Int_t side  = (slot >= 0) ? 1 : 0;
      Int_t tkid  =  TMath::Sign(layer*100+TMath::Abs(slot+side), slot+side);

      if (layer == 8 || layer == 9) continue;

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
		   hprj->GetSumOfWeights())
	   << endl;
      lad->_sensy[j] += mean;

      hprj->Fill(mean*1e4);

      hfit->SetBinContent(i*nby+j+1, mean);
      hfit->SetBinError  (i*nby+j+1, prof->GetBinError(i+1, j+1));
    }

  writes();
}

/*
  ntpl->Branch("idata", idata, "run/I:event/I:ient/I:time/I:"  //  0-4
	                       "ncls/I:nhit/I:ntrk/I:"         //  4-7
	                       "ntrd/I:nanti/I:tof/I:"         //  7-9
	                       "nhity/I:nhitc/I:fpat/I:"       //  9-13
	                       "tkid[9]/I:imlt[9]/I");         // 13-31
  ntpl->Branch("track", fdata, 
	       "p0x/F:p0y/F:angx/F:angy/F:th/F:ph/F:plen/F:"   //  0- 7
	       "trkx[9]/F:trky[9]/F:"                          //  7-25
	       "csqx[4]/F:csqy[4]/F:"                          // 25-33
	       "rgt[4]/F:erinv[4]/F:"                          // 33-41
	       "xcog[9]/F:ycog[9]/F");                         // 41-59
*/

