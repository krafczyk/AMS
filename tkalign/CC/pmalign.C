// $Id: pmalign.C,v 1.5 2011/03/31 10:10:04 haino Exp $
#include "TStopwatch.h"
#include "TSystem.h"
#include "TMath.h"

#include "root.h"
#include "amschain.h"
#include "TrTrackSelection.h"

#include <iostream>

using namespace std;

#include <fstream>
#include <string>

float memchk(void)
{
  char path[256];
  sprintf(path, "/proc/%d/status", getpid());
  std::ifstream fin(path);
  if (!fin) return 0;

  char buff[256];
  while (!fin.eof()) {
    fin >> buff;
    if (buff == std::string("VmSize:")) {
      float vmsize;
      fin >> vmsize;
      return vmsize;
    }
  }
  return 0;
}

void pmalign(const char *fname, const char *oname, 
	     const char *tkdbc = 0, Int_t nproc = 0)
{
  AMSChain ch;
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;

  if (tkdbc && tkdbc[0] == '0') tkdbc = 0;

  cout << "Ntr,Nent= " << ntr << " " << nent << endl;
  if (nproc <= 0) nproc = nent;

  for (Int_t i = 0; i < ntr; i++)
    cout << ch.GetListOfFiles()->At(i)->GetTitle() << endl;

  Int_t mfit[4] = { TrTrackR::kChoutko,
		    TrTrackR::kChoutko | TrTrackR::kFitLayer8,
		    TrTrackR::kChoutko | TrTrackR::kFitLayer9,
		    TrTrackR::kChoutko | TrTrackR::kFitLayer8
		                       | TrTrackR::kFitLayer9 };
  Int_t mf0 = mfit[0];

  Int_t recal = 0;
  if (!TkDBc::Head && tkdbc) {
    Int_t setup = 3;
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(setup, tkdbc);
    recal = 1;

    TString ssn = "tksens.dat";
    ifstream ftmp(ssn);
    if (ftmp.good())
      TkDBc::Head->readAlignmentSensor(ssn);
  }

  TFile ofile(oname, "recreate");

  enum { NID = 31, NFD = 67, NED = 7 };

  Int_t   idata[NID];
  Float_t fdata[NFD];

  TTree *ntpl = new TTree("tree", "Tracker tree");
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

  ofile.cd();
  Int_t ne = (nent/1000+1)*1000;

  Double_t bn[101];
  for (Int_t i = 0; i <= 100; i++) bn[i] = TMath::Power(10, 0.10*i-4);

  new TH2F("hist01", "nTrRawCluster", 100, 0, ne, 100, 0, 200);
  new TH2F("hist02", "nTrCluster",    100, 0, ne, 100, 0, 200);
  new TH2F("hist03", "nTrRecHit",     100, 0, ne, 100, 0, 200);
  new TH2F("hist04", "nTrTrack",      100, 0, ne,  10, 0,  10);
  new TH2F("hist05", "ResYe VS 1/R",  100, bn, 201, -100.5, 100.5);

  Int_t malg  = 2; // Alcaraz fit
  Int_t mpat  = 7; // Full span
  Int_t mspan = TrTrackSelection::kMaxSpan;
  Int_t refit = 2;

  TStopwatch timer;
  timer.Start();

  cout << nproc << " events to process" << endl;

  Int_t intv  = 10000;
  Int_t nevt  = 0;
  Int_t nfill = 0;
  Int_t ntk[3] = { 0, 0, 0 };

  if (nproc <= 10000) intv = 10;

  Int_t tref = 1282946400; // Sat Aug 28 00:00:00 2010

  for (Int_t ient = 0; ient < nproc; ient++) {
    AMSEventR *evt = ch.GetEvent(ient);
    nevt++;

    if (nevt%intv == 0 || nevt == nproc) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%6d %6d %6d %6d %7d (%5.1f%%) "
		   "%4.0f sec (%4.0f Hz) %3.0f MB",
		   nfill, ntk[0], ntk[1], ntk[2], nevt, 
		   100.*nevt/nproc, tm, nevt/tm, memchk()/1024)
	   << endl;

      if (nevt == nproc)
	cout << Form("%7.3f %7.3f %7.3f %7.3f",
		     1.*nfill/nevt, 1.*ntk[0]/nevt, 
		     1.*ntk[1]/nevt, 1.*ntk[2]/nevt) << endl;
    }

    ((TH2F *)ofile.Get("hist01"))->Fill(ient, evt->nTrRawCluster());
    ((TH2F *)ofile.Get("hist02"))->Fill(ient, evt->nTrCluster());
    ((TH2F *)ofile.Get("hist03"))->Fill(ient, evt->nTrRecHit());
    ((TH2F *)ofile.Get("hist04"))->Fill(ient, evt->nTrTrack());

    // Pre-selection
    if (evt->nTrTrack () != 1) continue;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;
    if (!trk->ParExists(mf0)) continue;

  //Int_t span = TrTrackSelection::GetSpanFlags(trk) & 0xff;
  //if (span != 0x1f) continue;

    Int_t mf = trk->iTrTrackPar(malg, mpat, refit);
  //if (mf < 0) continue;

    if (evt->nTrdTrack() != 1) continue;
    TrdTrackR *trd = evt->pTrdTrack(0);
    if (!trd) continue;

    AMSPoint pnt;
    AMSDir   dir;
    trk->Interpolate(trd->Coo[2], pnt, dir);
    Double_t dtx = pnt[0]-trd->Coo[0];
    Double_t dty = pnt[1]-trd->Coo[1];

    if (TMath::Abs(dtx) > 1 || TMath::Abs(dty) > 1) continue;

    Double_t rgt  = trk->GetRigidity();
    Double_t eres = trk->GetNormChisqY();
    if (mf > 0) {
      rgt  = trk->GetRigidity(mf);
      eres = TrTrackSelection::GetHalfRessq(mfit[3], trk, refit);
      ((TH2F *)ofile.Get("hist05"))->Fill(eres, 1e3/rgt);
      if (eres > 20) continue;
    }

    Float_t plx[9], ply[9];
    for (Int_t i = 0; i < 9; i++) plx[i] = ply[i] = 0;

    for (Int_t i = 0; i < 9; i++) {
      AMSPoint pl = trk->InterpolateLayer(i, mf0);

      Double_t max = 200;
      if (TMath::Abs(pl.x()) < max &&
	  TMath::Abs(pl.y()) < max) {
	plx[i] = pl.x();
	ply[i] = pl.y();
      }
    }

    Double_t csqx0 = trk->GetNormChisqX(mf0);
    Double_t csqy0 = trk->GetNormChisqY(mf0);

    evt->NTrRecHit();

    if (recal) {
      evt->NTrCluster();
      for (Int_t i = 0; i < trk->GetNhits(); i++) {
	TrRecHitR *hit = trk->GetHit(i);
	if (!hit) continue;
	hit->BuildCoordinate();
      }
      if (trk->FitT(mf0) < 0) continue;
      if (trk->ParExists(mfit[1])) trk->FitT(mfit[1]);
      if (trk->ParExists(mfit[2])) trk->FitT(mfit[2]);
      if (trk->ParExists(mfit[3])) trk->FitT(mfit[3]);
    }

    Int_t fpat = 0;
    if (trk->ParExists(mfit[0])) fpat += 1;
    if (trk->ParExists(mfit[1])) fpat += 2;
    if (trk->ParExists(mfit[2])) fpat += 4;
    if (trk->ParExists(mfit[3])) fpat += 8;

    Double_t csqx = trk->GetNormChisqX(mf0);
    Double_t csqy = trk->GetNormChisqY(mf0);

    if (csqx < 0 || csqy < 0 || trk->GetRigidity(mf0) == 0) continue;

    for (Int_t i = 0; i < NID; i++) idata[i] = 0;
    for (Int_t i = 0; i < NFD; i++) fdata[i] = 0;

    Int_t *iptr = idata;
    *(iptr++) = evt->Run();
    *(iptr++) = evt->Event();
    *(iptr++) = ient;
    *(iptr++) = evt->fHeader.Time[0]-tref;

    *(iptr++) = evt->nTrCluster();
    *(iptr++) = evt->nTrRecHit();
    *(iptr++) = evt->nTrTrack();

    *(iptr++) = evt->nTrdTrack();
    *(iptr++) = evt->nAntiCluster();
    *(iptr++) = evt->nTofCluster();

    *(iptr++) = trk->GetNhits();
    *(iptr++) = trk->GetNhitsXY();
    *(iptr++) = fpat;

    Int_t *tptr = &iptr[0];
    Int_t *mptr = &iptr[9];

    for (Int_t i = 0; i < trk->GetNhits(); i++) {
      TrRecHitR *hit = trk->GetHit(i);
      if (hit) {
	Int_t ily = hit->GetLayer()-1;
	tptr[ily] = hit->GetTkId();
	mptr[ily] = hit->GetResolvedMultiplicity();
      }
    }


    Double_t tant = TMath::Tan(trk->GetTheta());
    Double_t dzdx = tant*TMath::Cos(trk->GetPhi());
    Double_t dzdy = tant*TMath::Sin(trk->GetPhi());

    Int_t lmin = 10, lmax = -1;
    for (Int_t i = 0; i < 9; i++) {
      Int_t layer = i+1;
      if (layer == 8) layer = 9;
      if (trk->TestHitBits(layer)) {
	if (layer < lmin) lmin = layer;
	if (layer > lmax) lmax = layer;
      }
    }
    if (lmin == 0 && lmax > 0) lmin = 8;
    Double_t plen = TkDBc::Head->GetZlayer(lmin)-TkDBc::Head->GetZlayer(lmax);

    Float_t *fptr = fdata;
    *(fptr++) = trk->GetP0x(mf0);
    *(fptr++) = trk->GetP0y(mf0);
    *(fptr++) = TMath::ATan(dzdx)*TMath::RadToDeg();
    *(fptr++) = TMath::ATan(dzdy)*TMath::RadToDeg();
    *(fptr++) = trk->GetTheta(mf0);
    *(fptr++) = trk->GetPhi  (mf0);
    *(fptr++) = plen;

    for (Int_t i = 0; i < 9; i++) *(fptr++) = plx[i];
    for (Int_t i = 0; i < 9; i++) *(fptr++) = ply[i];

    for (Int_t i = 0; i < 4; i++) {
      if (trk->ParExists(mfit[i])) {
	fptr[i]    = trk->GetNormChisqX(mfit[i]);
	fptr[i+ 4] = trk->GetNormChisqY(mfit[i]);
	fptr[i+ 8] = trk->GetRigidity  (mfit[i]);
      //fptr[i+12] = trk->GetErrRinv   (mfit[i]);
	fptr[i+12] = (i < 3) ? TrTrackSelection::GetHalfRdiff(mfit[i], trk)
	                     : eres;

	if (i > 0) ntk[i-1]++;
      }
      else {
	fptr[i]   = fptr[i+ 4] = -1;  // csqx, csqy
	fptr[i+8] = fptr[i+12] =  0;  // rgt, halfR
      }
    }
    fptr += 16;

    Int_t tslot[4] = { 0, 0, 0, 0 };

    Float_t *cgx = &fptr[0];
    Float_t *cgy = &fptr[9];

    for (Int_t i = 0; i < trk->GetNhits(); i++) {
      TrRecHitR *hit = trk->GetHit(i);
      if (!hit) continue;
      Int_t tkid  = hit->GetTkId();
      Int_t layer = hit->GetLayer();

      TrClusterR *xcls = hit->GetXCluster();
      TrClusterR *ycls = hit->GetYCluster();
      if (!ycls) continue;

      Int_t ily = layer-1;
      cgx[ily] = (!xcls) ? -(hit->GetDummyX()+640)
	       : xcls->GetCofG()+xcls->GetSeedAddress();
      cgy[ily] = ycls->GetCofG()+ycls->GetSeedAddress();
    }

    ntpl->Fill();
    nfill++;
  }

  ofile.Write();
}
