
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
#include "TrAlignFit.h"

#include <iostream>

using namespace std;

enum { PREINT = 1, FLIGHT = 2, TBEAM = 12 };

void alignfit(const char *fname, const char *oname, const char *dbc,
	      Double_t csqmax = 100, Int_t setup = FLIGHT, 
	      Int_t recalc = 0, Int_t nevt = 0)
{
  AMSChain ch;
  if (ch.Add(fname) <= 0) return;

  ofstream fout(oname);
  if (!fout) {
    cout << "Output file open error: " << oname << endl;
    return;
  }

  TString tks = "tksens.dat";

  ch.GetEvent(0);
  if (dbc) {
    std::ifstream ftmp(dbc);
    if (!ftmp) {
      if (!TkDBc::Head) {
	std::cerr << "Error: TkDBc not found: " << dbc << std::endl;
	return;
      }
      TkDBc::Head->write(dbc);
      dbc = 0;
    }
  }

  if (!TrParDB::Head) TrParDB::Load("pardb.root");
  if (!TrParDB::Head) {
    std::cerr << "TrParDB not found" << std::endl;
    return;
  }
  TrClusterR::UsingTrParDB(TrParDB::Head);

  Int_t initdbc = (dbc) ? 1 : 0;

  TrTrackR::DefaultFitID = TrTrackR::kLinear;

  Double_t bin[121];
  for (Int_t i = 0; i <= 120; i++) bin[i] = TMath::Power(10, -2+0.05*i);
  TH2F *hist0 = new TH2F("hist0", "Chisq", 120, bin, 120, bin);

  TH2F *hist1 = new TH2F("hist1", "X shift", 256, 0.5, 8.5, 500, -50, 50);
  TH2F *hist2 = new TH2F("hist2", "Y shift", 256, 0.5, 8.5, 500, -50, 50);
  TH2F *hist3 = new TH2F("hist3", "X shift", 100, -1, 1, 500, -50, 50);
  TH2F *hist4 = new TH2F("hist4", "Y shift", 100, -1, 1, 500, -50, 50);


  Int_t nent = ch.GetEntries();
  if (nevt <= 0 || nevt > nent) nevt = nent;

  TStopwatch timer;
  timer.Start();

  cout << nevt << " events to process" << endl;

  Int_t nfill = 0;
  Int_t intv  = 10000;

  Double_t cfx = 1;
  Double_t cfy = 1;
  if (setup%100 == FLIGHT || setup%100 == TBEAM) {
    cfx = 34.*34./300/300;
    cfy = 20.*20./300/300;
  }

  for (Int_t i = 0; i < nevt; i++) {
    if (i > 0 && i%intv == 0) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%7d %7d (%4.1f%%) %4.0f sec (%4.2f kHz)",
		   nfill, i, 100.*i/nevt, tm, 1e-3*i/tm) << endl;
    }

    AMSEventR *evt = ch.GetEvent();
    if (evt->nTrTrack() != 1) continue;

    if (initdbc) {
      TkDBc::Head->init(setup%10, (char*)dbc);
      initdbc = 0;

      // Sensor alignment
      if (recalc && tks != "") {
	ifstream ftmp(tks);
	if (ftmp.good())
	  TkDBc::Head->readAlignmentSensor(tks);
      }
    }

//  TrParDB::Head->_asymmetry[0] = 0.027;
//  TrParDB::Head->_asymmetry[1] = 0.034;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;
    if (trk->GetChisqX() <= 0 || trk->GetChisqY() <= 0) continue;

    hist0->Fill(trk->GetChisqX()*cfx, trk->GetChisqY()*cfy);
    if (trk->GetChisqX()*cfx > csqmax || 
	trk->GetChisqY()*cfy > csqmax) continue;

    Float_t arr[7][8];
    for (Int_t j = 0; j < 8*7; j++) arr[j/8][j%8] = 0;

    Int_t nhit = trk->GetNhits();
    if (nhit < 6) continue;

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
      AMSPoint coo = (recalc) 
	? hit->GetGlobalCoordinate(hit->GetResolvedMultiplicity())
	: hit->GetCoord();

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
    if (dbc && dbc[0]) TkDBc::Head->init(setup%10, dbc);
    else TkDBc::Head->init();
  }

  TrTrackR::DefaultFitID = TrTrackR::kLinear;

  Int_t fix[7][8];
  for (Int_t i = 0; i < 8; i++) {
    for (Int_t j = 0; j < 6; j++) 
      fix[j][i] = (j == 5) ? 0 : 1;
  }

  Long_t id, size, flags, modtime;
  gSystem->GetPathInfo(fname, &id, &size, &flags, &modtime);

  Int_t maxcases = size/8/7/4;
  TrAlignFit afit(maxcases);

  if (skip > 0) {
    Long_t time = (Long_t)gSystem->Now();
    UInt_t seed = (UInt_t)time;
    cout << "Random seed is: " << seed << endl;
    gRandom->SetSeed(seed);
  }

  // Set hits
  Float_t tmp[7][8];
  Float_t arr[11][8];
  for (Int_t i = 0; nevt == 0 || i < nevt; i++) {
    fin.read((char*)tmp, sizeof(tmp));
    if (fin.eof()) break;

    if (skip > 0 && gRandom->Rndm() < skip) continue;

    Int_t nlay = 0;
    for (Int_t j = 0; j < 8; j++) {
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

    afit.Set(arr, fix);
  }

  TrAlignFit::fMaxIter  = 20;
  TrAlignFit::fMaxChisq = 10;
  if (setup%100 == TBEAM) {
    TrAlignFit   ::fMaxIter = 10;
    TrAlignFit   ::fMode    = TrAlignFit::kTestBeam;
    TrAlignObject::fRange   = 0.1;

    if (setup/100 == 1) {
      TrAlignFit::fFitM = TrAlignFit::kCurved;
      TrRecon::ReadMagField(
	gSystem->ExpandPathName("$AMSDataDir/v5.00/MagneticFieldMap07.bin"),
				400./460, 1);
    }
    else if (setup/100 == 2) {
      TrAlignFit::fFitM = TrAlignFit::kRgtFix;
      TrAlignFit::fFixR = 0;
    }
    else if (setup/100 == 4) {
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

void alignfit(const char *fname, Int_t setup = FLIGHT, Int_t recalc = 0)
{
  TString dbc = "tkdbc.dat";
  TString alf = "alignfit.dat";

  Double_t csqmax = 100;
  if (setup%100 == TBEAM) csqmax = 10;

  alignfit(fname, alf, dbc, csqmax, setup, recalc);
  alignfit(alf, dbc, setup);
}

