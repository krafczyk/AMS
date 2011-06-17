// $Id: pmalign.C,v 1.6 2011/06/17 00:30:49 shaino Exp $
#include "TStopwatch.h"
#include "TSystem.h"
#include "TMath.h"
#include "TH3.h"

#include "root.h"
#include "amschain.h"
#include "TrTrackSelection.h"
#include "TrCharge.h"

#include "Predict.h"
#include "geo-cgmC.h"

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

  Int_t mfc = TrTrackR::DefaultFitID;
  Int_t mfit[4] = { mfc,
		    mfc | TrTrackR::kFitLayer8,
		    mfc | TrTrackR::kFitLayer9,
		    mfc | TrTrackR::kFitLayer8
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
	       "p0x/F:p0y/F:beta/F:chrg/F:th/F:ph/F:glat/F:"   //  0- 7
	       "trkx[9]/F:trky[9]/F:"                          //  7-25
	       "csqx[4]/F:csqy[4]/F:"                          // 25-33
	       "rgt[4]/F:erinv[4]/F:"                          // 33-41
	       "xcog[9]/F:ycog[9]/F");                         // 41-59

  ofile.cd();
  Int_t ne = 2000000;//(nent/1000+1)*1000;

  Double_t bn[101];
  for (Int_t i = 0; i <= 100; i++) bn[i] = TMath::Power(10, 0.10*i-4);

  new TH2F("hist01", "nTrRawCluster", 100, 0, ne, 100, 0, 200);
  new TH2F("hist02", "nTrCluster",    100, 0, ne, 100, 0, 200);
  new TH2F("hist03", "nTrRecHit",     100, 0, ne, 100, 0, 200);
  new TH2F("hist04", "nTrTrack",      100, 0, ne,  10, 0,  10);
  new TH2F("hist05", "ResYe VS 1/R",  100, bn, 201, -100.5, 100.5);

  new TH3F("hist11", "ResX", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -0.1, 0.1);
  new TH3F("hist12", "ResY", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -0.1, 0.1);

  Int_t tref = 1305756000; // Thu May 19 00:00:00 2011

  Double_t bin[31];
  for (Int_t i = 0; i <=  30; i++) bin[i] = TMath::Power(10, i*0.10-1);
  new TH2F("hist20", "glat VS time", 1000, tref+5e4, tref+2.5e5, 160, -80, 80);
  new TH2F("hist21", "Z=1  Down", 30, bin, 160, -80, 80);
  new TH2F("hist22", "Z=2  Down", 30, bin, 160, -80, 80);
  new TH2F("hist23", "Z>2  Down", 30, bin, 160, -80, 80);
  new TH2F("hist24", "Z=-1 Down", 30, bin, 160, -80, 80);
  new TH2F("hist25", "Z=1  Up",   30, bin, 160, -80, 80);
  new TH2F("hist26", "Z=-1 Up",   30, bin, 160, -80, 80);

  new TH3F("hist31", "|Z|=1 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist32", "|Z|=2 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);
  new TH3F("hist33", "|Z|>2 Dw", 9, 0.5, 9.5, 33, -16.5, 16.5, 200, -.5, .5);

  new TH3F("hist41", "|Z|=1 Dw", 9, 0.5, 9.5, 120, -0.6, 0.6, 200, -.5, .5);
  new TH3F("hist42", "|Z|=2 Dw", 9, 0.5, 9.5, 120, -0.6, 0.6, 200, -.5, .5);
  new TH3F("hist43", "|Z|>2 Dw", 9, 0.5, 9.5, 120, -0.6, 0.6, 200, -.5, .5);

  Int_t malg  = 1; // Choutko fit //2; // Alcaraz fit
  Int_t mpat  = 3; // Inner only  //7; // Full span
  Int_t mspan = TrTrackSelection::kMaxSpan;
  Int_t refit = 0;//2;

  TStopwatch timer;
  timer.Start();

  cout << nproc << " events to process" << endl;

  Int_t intv  = 10000;
  Int_t nevt  = 0;
  Int_t nfill = 0;
  Int_t ntk[3] = { 0, 0, 0 };

  if (nproc <= 10000) intv = 10;

  ISSCoo iss;
  cgmcoo cgm;
  Double_t glat = 0;
  Bool_t first = kTRUE;

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

    mf0 = trk->iTrTrackPar(malg, mpat, refit);
    if (mf0 < 0 || !trk->ParExists(mf0)) continue;

    Int_t span = TrTrackSelection::GetSpanFlags(trk) & 0xff;
    if (!(span & TrTrackSelection::kAllPlane)) continue;

    if (evt->nTrdTrack() != 1) continue;
    TrdTrackR *trd = evt->pTrdTrack(0);
    if (!trd) continue;

    AMSPoint pnt;
    AMSDir   dir;
    trk->Interpolate(trd->Coo[2], pnt, dir);
    Double_t dtx = pnt[0]-trd->Coo[0];
    Double_t dty = pnt[1]-trd->Coo[1];

    if (TMath::Abs(dtx) > 1 || TMath::Abs(dty) > 1) continue;

    if (evt->nBeta() != 1) continue;
    BetaR *bet = evt->pBeta(0);
    if (!bet || bet->Status > 0 || bet->Beta == -1) continue;

    Double_t chgp = TrCharge::GetQ(trk, 1);
    Double_t chgn = TrCharge::GetQ(trk, 0);
    if (chgp <= 0 || chgn <= 0) continue;

/*
    Double_t rgt  = trk->GetRigidity(mf0);
    Double_t eres = trk->GetNormChisqY(mf0);
    if (mf > 0) {
      rgt  = trk->GetRigidity(mf);
      eres = TrTrackSelection::GetHalfRessq(mfit[3], trk, refit);
      ((TH2F *)ofile.Get("hist05"))->Fill(eres, 1e3/rgt);
      if (eres > 20) continue;
    }
*/

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

    Double_t  rgt = trk->GetRigidity(mf0);
    Double_t argt = TMath::Abs(rgt);
    Double_t csqx = trk->GetNormChisqX(mf0);
    Double_t csqy = trk->GetNormChisqY(mf0);
    Double_t eric = trk->GetErrRinv   (mf0);
    eric /= std::sqrt(0.004*0.004+0.03*0.03/argt/argt
		                 +0.02*0.02/argt/argt/argt/argt);

    if (csqx <  0 || csqy <  0 || trk->GetRigidity(mf0) == 0) continue;
    if (csqx > 20 || csqy > 20 || eric > 20) continue;

    for (Int_t i = 0; i < NID; i++) idata[i] = 0;
    for (Int_t i = 0; i < NFD; i++) fdata[i] = 0;

    Int_t time = evt->fHeader.Time[0]-tref;

    Int_t *iptr = idata;
    *(iptr++) = evt->Run();
    *(iptr++) = evt->Event();
    *(iptr++) = ient;
    *(iptr++) = time;

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

    Double_t tant = TMath::Tan(trk->GetTheta());
  //Double_t dzdx = tant*TMath::Cos(trk->GetPhi());
  //Double_t dzdy = tant*TMath::Sin(trk->GetPhi());

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

    iss.TrackISS(evt->fHeader.Time[0], 0);
    cgm.GetCGM(iss.Lat, iss.Lon, iss.Alt);
    glat = cgm.clar;

    ((TH2F *)ofile.Get("hist20"))->Fill(evt->fHeader.Time[0], glat);

    Int_t hid = -1;
    Double_t beta = bet->Beta;
    Double_t chg  = (chgp+chgn)/2;
    if (beta > 0 && rgt > 0) {
      if      (chg > 3.0*std::sqrt(1.2*1.2/rgt/rgt+1)) hid = 23;
      else if (chg > 1.6*std::sqrt(1.5*1.5/rgt/rgt+1)) hid = 22;
      else                                             hid = 21;
    }
    else if (beta > 0 && rgt < 0) hid = 24;
    else if (beta < 0 && rgt < 0) hid = 25;
    else if (beta < 0 && rgt > 0) hid = 26;
    if (hid > 0)
      ((TH2F *)ofile.Get(Form("hist%d", hid)))->Fill(argt, glat);

    Int_t hidr = -1;
    if (beta > 0) {
      if      (chg > 3.0*std::sqrt(1.2*1.2/rgt/rgt+1)) hidr = 33;
      else if (chg > 1.6*std::sqrt(1.5*1.5/rgt/rgt+1)) hidr = 32;
      else                                             hidr = 31;
    }
    TH3F *hrv1 = (hidr > 0) ? (TH3F *)ofile.Get(Form("hist%d", hidr))    : 0;
    TH3F *hrv2 = (hidr > 0) ? (TH3F *)ofile.Get(Form("hist%d", hidr+10)) : 0;

    AMSDir tdir(trk->GetTheta(), trk->GetPhi());
    Double_t dydz = tdir.y()/tdir.z();

    if (first) {
      first = kFALSE;
      cout << "glat0= " << glat << endl;
    }

    for (Int_t i = 0; i < trk->GetNhits(); i++) {
      TrRecHitR *hit = trk->GetHit(i);
      if (hit) {
	Int_t ily = hit->GetLayer()-1;
	tptr[ily] = hit->GetTkId();
	mptr[ily] = hit->GetResolvedMultiplicity();

	Int_t    slt = hit->GetTkId()%100;
	AMSPoint res = trk->GetResidual(ily, mf0);
	((TH3F *)ofile.Get("hist11"))->Fill(ily+1, slt, res.x());
	((TH3F *)ofile.Get("hist12"))->Fill(ily+1, slt, res.y());
	if (hrv1) hrv1->Fill(ily+1,  slt, 1/rgt);
	if (hrv2) hrv2->Fill(ily+1, dydz, 1/rgt);
      }
    }

    Float_t *fptr = fdata;
    *(fptr++) = trk->GetP0x(mf0);
    *(fptr++) = trk->GetP0y(mf0);
    *(fptr++) = beta;
    *(fptr++) = chg;
    *(fptr++) = trk->GetTheta(mf0);
    *(fptr++) = trk->GetPhi  (mf0);
    *(fptr++) = glat;

    for (Int_t i = 0; i < 9; i++) *(fptr++) = plx[i];
    for (Int_t i = 0; i < 9; i++) *(fptr++) = ply[i];

    for (Int_t i = 0; i < 4; i++) {
      if (trk->ParExists(mfit[i])) {
	fptr[i]    = trk->GetNormChisqX(mfit[i]);
	fptr[i+ 4] = trk->GetNormChisqY(mfit[i]);
	fptr[i+ 8] = trk->GetRigidity  (mfit[i]);
      //fptr[i+12] = trk->GetErrRinv   (mfit[i]);
	fptr[i+12] = (i < 3) ? TrTrackSelection::GetHalfRdiff(mfit[i], trk)
	                     : TrTrackSelection::GetHalfRessq(mfit[3], trk);

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

  cout << "glat1= " << glat << endl;

  ofile.Write();
}
