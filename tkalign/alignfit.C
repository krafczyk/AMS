
#include "TROOT.h"
#include "TH2.h"
#include "TSystem.h"
#include "TMath.h"
#include "TRandom.h"
#include "TStopwatch.h"

#include "root.h"
#include "amschain.h"

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrTrack.h"
#include "TrRecHit.h"

#include "TrParDB.h"
#include "TrRecon.h"
#include "MagField.h"
#include "TrAlignFit.h"

#include <iostream>

using namespace std;

enum { PREINT = 1, FLIGHT = 2, TBEAM = 12, AMS02P = 100, MCSIM = 1000 };

enum { NLAY = 9 };

void alignfit(const char *fname, const char *oname, const char *dbc,
	      Double_t csqmax = 100, Int_t setup = FLIGHT + AMS02P, 
	      Int_t recalc = 0, Int_t nevt = 0)
{
  AMSChain ch;
  if (ch.Add(fname) <= 0) return;

  ofstream fout(oname);
  if (!fout) {
    cout << "Output file open error: " << oname << endl;
    return;
  }

  if (recalc && dbc) {
    Int_t tksetup = ((setup/100)%10 == AMS02P/100) ? 3 : 2;
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(tksetup, (char*)dbc);
  }
  ch.GetEvent(0);

  if (!TkDBc::Head) {
    std::cerr << "Error: TkDBc not found: " << dbc << std::endl;
    return;
  }

  if (!recalc && dbc) TkDBc::Head->write(dbc);

  if (!TrParDB::Head) TrParDB::Load("pardb.root");
  if (!TrParDB::Head) {
    std::cerr << "TrParDB not found" << std::endl;
    return;
  }
  TrClusterR::UsingTrParDB(TrParDB::Head);

  TrTrackR::DefaultFitID = TrTrackR::kLinear;

  Double_t bin[121];
  for (Int_t i = 0; i <= 120; i++) bin[i] = TMath::Power(10, -2+0.05*i);
  TH2F *hist0 = new TH2F("hist0", "Chisq", 120, bin, 120, bin);

  Int_t nb = NLAY*32;
  Double_t bx = NLAY+0.5;

  TH2F *hist1 = new TH2F("hist1", "dX(um)", nb, 0.5, bx, 500, -250, 250);
  TH2F *hist2 = new TH2F("hist2", "dY(um)", nb, 0.5, bx, 500, -250, 250);
  TH2F *hist3 = new TH2F("hist3", "dX(um)", 100, -1,  1, 500, -250, 250);
  TH2F *hist4 = new TH2F("hist4", "dY(um)", 100, -1,  1, 500, -250, 250);

  TH2F *hist5 = 0;
  TH2F *hist6 = 0;
  TH2F *hist7 = 0;
  TH2F *hist8 = 0;
  if ((setup/100)%10 == AMS02P/100) {
    hist5 = new TH2F("hist5", "ChisqY VS Rigidity(+)", 80, &bin[40], 120, bin);
    hist6 = new TH2F("hist6", "ChisqY VS Rigidity(-)", 80, &bin[40], 120, bin);
    hist7 = new TH2F("hist7", "ChisqY VS Rigidity(+)", 80, &bin[40], 120, bin);
    hist8 = new TH2F("hist8", "ChisqY VS Rigidity(-)", 80, &bin[40], 120, bin);
  }

  Int_t nent = ch.GetEntries();
  if (nevt <= 0 || nevt > nent) nevt = nent;

  TStopwatch timer;
  timer.Start();

  cout << nevt << " events to process" << endl;

  Int_t nfill = 0;
  Int_t intv  = 10000;
  Int_t nfmax = 1500000;

  Double_t cfx = 1;
  Double_t cfy = 1;
  if (setup%100 == FLIGHT || setup%100 == TBEAM) {

    cfx = 34.*34./300/300;
    cfy = 20.*20./300/300;
  }

  Int_t nhitmin = 6;

  for (Int_t i = 0; i < nevt; i++) {
    if ((i > 0 && i%intv == 0) || nfill == nfmax) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%7d %7d (%4.1f%%) %4.0f sec (%4.2f kHz)",
		   nfill, i, 100.*i/nevt, tm, 1e-3*i/tm) << endl;
    }
    if (nfill >= nfmax) break;

    AMSEventR *evt = ch.GetEvent(i);
    if (evt->nTrTrack() != 1) continue;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;
    if (trk->GetChisqX() <= 0 || trk->GetChisqY() <= 0) continue;

    hist0->Fill(trk->GetChisqX()*cfx, trk->GetChisqY()*cfy);

    Double_t rgt = trk->GetRigidity();
    if (hist5 && rgt > 0) hist5->Fill( rgt, trk->GetChisqY()*cfy);
    if (hist6 && rgt < 0) hist6->Fill(-rgt, trk->GetChisqY()*cfy);
    if ((setup/100)%10 == AMS02P/100 && TMath::Abs(rgt) < 1) continue;

    if (trk->GetChisqX()*cfx > csqmax || 
	trk->GetChisqY()*cfy > csqmax) continue;

    Float_t arr[7][NLAY];
    for (Int_t j = 0; j < NLAY*7; j++) arr[j/NLAY][j%NLAY] = 0;

    Int_t nhit = trk->GetNhits();
    if (nhit < nhitmin) continue;

    if (hist7 && rgt > 0) hist7->Fill( rgt, trk->GetChisqY()*cfy);
    if (hist8 && rgt < 0) hist8->Fill(-rgt, trk->GetChisqY()*cfy);

    if (recalc) {
      evt->NTrRecHit();
      evt->NTrCluster();
    }

    Int_t nxy = 0;
    for (Int_t j = 0; j < nhit; j++) {
      TrRecHitR *hit = trk->GetHit(j);
      if (!hit) continue;

    //if ((hit->getstatus() & XONLY) ||
    //	  (hit->getstatus() & YONLY)) continue;
      if (hit->OnlyX()) continue;

      Int_t tkid = hit->GetTkId();
      Int_t ilyr = hit->GetLayer()-1;
      Int_t slot = TMath::Abs(tkid)%100;
      Int_t half = (tkid < 0) ? 1 : 2;

      AMSPoint co0 = hit->GetCoord();
      if (recalc) hit->BuildCoordinates();

      AMSPoint coo = hit->GetCoord();

      if (recalc) {
	Double_t xtk = hit->GetLayer()+(tkid%100+0.5)/32;
	Double_t xdf = (coo.x()-co0.x())*1e4;
	Double_t ydf = (coo.y()-co0.y())*1e4;
	Double_t xsg = tkid/TMath::Abs(tkid);
	Double_t ysg = xsg*((hit->GetLayer()%2 == 1) ? -1 : 1);
	hist1->Fill(xtk, xdf);
	hist2->Fill(xtk, ydf);

	TrClusterR *xcls = hit->GetXCluster();
	TrClusterR *ycls = hit->GetYCluster();
	if (xcls) hist3->Fill(xcls->GetCofG(), xdf*xsg);
	if (ycls) hist4->Fill(ycls->GetCofG(), ydf*ysg);
      }

      arr[0][ilyr] = coo[0];
      arr[1][ilyr] = coo[1];
      arr[2][ilyr] = coo[2];
      arr[3][ilyr] = slot;
      arr[4][ilyr] = half;
      arr[5][ilyr] = 300e-4;
      arr[6][ilyr] = 300e-4;

      nxy++;
    }

    if (nxy < 6) continue;

    if (setup/1000 == MCSIM/1000) {
      MCEventgR *mcg = evt->pMCEventg(0);
      if (!mcg) {
	cerr << "Error: MCEventg not found for MCSIM mode at ient= "
	     << i << endl;
	return ;
      }
      Float_t pref = mcg->Momentum;
      fout.write((char*)&pref, sizeof(pref));
    }

    fout.write((char*)arr, sizeof(arr));
    nfill++;
  }

  fout.close();

  TFile of("alignfit0.root", "recreate");
  gROOT->GetList()->Write();
}

void alignfit(const char *fname, const char *dbc,
	      Int_t setup = FLIGHT, Int_t nevt = 0, Float_t skip = 0)
{
  ifstream fin(fname);
  if (!fin) {
    cout << "File not found: " << fname << endl;
    return;
  }

  TrAlignFit::fNameOut = fname;
  TrAlignFit::fNameOut.ReplaceAll(".dat", ".root");
  if (TrAlignFit::fNameOut == fname) TrAlignFit::fNameOut = "alignfit.root";

  // Initialize TKDBc
  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    if (dbc && dbc[0]) {
      Int_t tksetup = setup%10;
      if ((setup/100)%10 == AMS02P/100) tksetup = 3;
      TkDBc::Head->init(tksetup, dbc);
    }
    else TkDBc::Head->init();
  }

  TrTrackR::DefaultFitID = TrTrackR::kLinear;

  Int_t fix[7][NLAY];
  for (Int_t i = 0; i < NLAY; i++) {
    for (Int_t j = 0; j < 6; j++) 
      fix[j][i] = (j == 5) ? 0 : 1;
  }

  Long_t id, size, flags, modtime;
  gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);

  TrAlignFit::fMaxIter  = 20;
  TrAlignFit::fMaxChisq = 10;
  if (setup%100 == TBEAM) {
    TrAlignFit   ::fMaxIter = 10;
    TrAlignFit   ::fMode    = TrAlignFit::kTestBeam;
    TrAlignObject::fRange   = 0.1;

    if ((setup/100)%10 == 1) {
      TrAlignFit::fFitM = TrAlignFit::kCurved;
      TrRecon::ReadMagField(
	gSystem->ExpandPathName("$AMSDataDir/v5.00/MagneticFieldMap07.bin"),
				400./460, 1);
    }
    else if ((setup/100)%10 == 2) {
      TrAlignFit::fFitM = TrAlignFit::kRgtFix;
      TrAlignFit::fFixR = 0;
    }
    else if ((setup/100)%10 == 4) {
      TrAlignFit::fFitM = TrAlignFit::kRgtFix;
      TrAlignFit::fFixR = 400;
      TrRecon::ReadMagField(
	gSystem->ExpandPathName("$AMSDataDir/v5.00/MagneticFieldMap07.bin"),
				400./460, 1);
    }
/*  /// TO BE FIXED
    else
      TrAlignFit::fMaxIter = 6;
    /// TO BE FIXED
*/
  }
  else if (setup%100 == FLIGHT) {
    if ((setup/100)%10 == AMS02P/100) {
      TrAlignFit::fFitM = TrAlignFit::kCurved;
      TrRecon::ReadMagField(
	gSystem->ExpandPathName("$AMSDataDir/v5.00/"
				"MagneticFieldMapPermanent_NEW.bin"), 1, 1);
      MagField::GetPtr()->SetScale(1);
    }
  }

  cout<<"setup= "<<setup<<" "<<(setup/100)%10<<" "
      <<TrAlignFit::fFitM<<" "<<TrAlignFit::fFixR<<endl;


  Int_t usize = NLAY*7*4;
  if (setup/1000 == MCSIM/1000) {
    usize += 4;
    TrAlignFit::fMode += TrAlignFit::kMCSim;
  }

  Int_t maxcases = size/usize;
  TrAlignFit afit(maxcases);

  if (skip > 0) {
    Long_t time = (Long_t)gSystem->Now();
    UInt_t seed = (UInt_t)time;
    cout << "Random seed is: " << seed << endl;
    gRandom->SetSeed(seed);
  }

  // Set hits
  Float_t tmp[7][NLAY];
  Float_t arr[11][NLAY];
  Float_t pref = 0;
  for (Int_t i = 0; nevt == 0 || i < nevt; i++) {
    if (setup/1000 == MCSIM/1000)
      fin.read((char*)&pref, sizeof(pref));

    fin.read((char*)tmp, sizeof(tmp));
    if (fin.eof()) break;

    if (skip > 0 && gRandom->Rndm() < skip) continue;

    Int_t nlay = 0;
    for (Int_t j = 0; j < NLAY; j++) {
      tmp[5][j] = 500e-4;
      tmp[6][j] = 200e-4;
      for (Int_t k = 0; k < 7; k++) arr[k][j] = tmp[k][j];

      if (arr[3][j] != 0) {
	Int_t sign = (arr[4][j] == 1) ? -1 : 1;
	Int_t tkid = sign*((j+1)*100+(Int_t)arr[3][j]);
	TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	AMSPoint pos;
	if (lad) pos = TkCoo::GetGlobalN(tkid, 0, 0);

	arr[7][j] = pos[0];
	arr[8][j] = pos[1];
	arr[9][j] = pos[2];
	nlay++;

	// Mask missing ladders
	if (setup%100 == PREINT) {
	  if (tkid == 410 || tkid == -807) {
	    for (Int_t k = 0; k < 9; k++) arr[k][j] = 0;
	  }
	}
	else if (setup%100 == FLIGHT) {
	  if (tkid == 405) {
	    for (Int_t k = 0; k < 9; k++) arr[k][j] = 0;
	  }
	}
      }
    }
    if (nlay < 6) continue;

    afit.Set(arr, fix, pref);
  }

  afit.Fit();

  TString ofn = fname;
  ofn.ReplaceAll("alignfit.", "align.");
  if (ofn == fname) ofn = "align.dat";

  ofstream fout(ofn);

  for (Int_t j = 0; j < 15; j++)
    for (Int_t k = 0; k < 2; k++) {
      afit.Get(arr, (j+1)*2+k);

      for (Int_t i = 0; i < 8; i++) {
	if (arr[10][i] == 0) continue;

	Int_t sign = (k == 0) ? -1 : 1;
	Int_t tkid = sign*((i+1)*100+j+1);
	TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	if (!lad) continue;

	fout << Form("%4d %7.4f %7.4f %7.4f %7.4f %7.4f %7.4f", tkid, 
		     arr[0][i]*10,  arr[1][i]*10,  arr[2][i]*10,
		     arr[4][i]*600, arr[3][i]*600, arr[5][i]*600) << endl;

	AMSRotMat nrm;
	nrm.SetRotAngles(-arr[4][i], -arr[3][i], arr[5][i]);

	lad->posA = lad->posA + AMSPoint(arr[0][i], arr[1][i], arr[2][i]);
	lad->rotA = lad->rotA * nrm;
      }
    }

  TString dname = fname;
  dname.ReplaceAll("alignfit.", "tkdbc.");
  if (dname == "tkdbc.dat" || dname == fname) dname = "tkdbc.dat.new";

  TkDBc::Head->write(dname.Data());
}

void alignfit(const char *fname, 
	      Int_t setup = FLIGHT + AMS02P, Int_t recalc = 0)
{
  TString dbc = "tkdbc.dat";
  TString alf = "alignfit.dat";

  Double_t csqmax = 100;
  if (setup%100 == TBEAM) csqmax = 10;

  alignfit(fname, alf, dbc, csqmax, setup, recalc);
  alignfit(alf, dbc, setup);
}

