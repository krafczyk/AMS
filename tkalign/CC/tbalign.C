// $Id: tbalign.C,v 1.5 2011/03/31 10:10:05 haino Exp $
#include "TStopwatch.h"
#include "TSystem.h"
#include "TMath.h"

#include "root.h"
#include "amschain.h"
#include "TrRecon.h"
#include "MagField.h"

#include "tbpos.C"

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

void tbalign(const char *fname, const char *oname, const char *tkdbc,
	     Int_t posid, Int_t ipos, Int_t fpos, 
	     Int_t psel  = 1105,
	     Int_t nproc = 0, 
	     Int_t ievt  = 0)
{
  AMSChain ch;
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  TBpos tbpos(posid);
  if (posid == 7)
    cout << "ECAL"    << posid << " " << ipos << "-" << fpos << endl;
  else
    cout << "TRACKER" << posid << " " << ipos << "-" << fpos << endl;

  Int_t nstrk = (psel/1000)%10;
  Int_t nstrd = (psel/ 100)%10;
  Int_t nsant = (psel/  10)%10;
  Int_t nstof = (psel     )%10;
  cout << "Pre-Selection: "
       << nstrk << " " << nstrd << " " << nsant << " " << nstof << endl;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;

  if (tkdbc && tkdbc[0] == '0') tkdbc = 0;

  cout << "Ntr,Nent= " << ntr << " " << nent << endl;
  if (nproc <= 0) nproc = nent;

  for (Int_t i = 0; i < ntr; i++)
    cout << ch.GetListOfFiles()->At(i)->GetTitle() << endl;

  Int_t mfd = TrTrackR::DefaultFitID;
  Int_t mfit[4] = { mfd,
		    mfd | TrTrackR::kFitLayer8,
		    mfd | TrTrackR::kFitLayer9,
		    mfd | TrTrackR::kFitLayer8
		        | TrTrackR::kFitLayer9 };
  Int_t mf0 = mfit[0];
  Int_t mfc = mfit[0];
  if (posid == TBpos::T416) mfc = mfit[3]; // Layer 1N-9
  if (posid == TBpos:: T80) mfc = mfit[2]; // Layer  2-9
  if (posid == TBpos::T280) mfc = mfit[1]; // Layer 1N-7
  if (posid == TBpos::E7)   mfc = mfit[3]; // Layer 1N-9

  Double_t rref = 0;
  Double_t rrng = 50;
  Double_t crng = 10;
  Double_t brng = 10;
  Double_t nrng = 15000;

  if (posid == TBpos::E7) {
    rrng = 10;
    crng =  5;
    nrng = 50000;
  }

  TString smf = gSystem->ExpandPathName("$AMSDataDir/v5.00"
					"/MagneticFieldMapPermanent_NEW.bin");

  TrRecon::InitFFKEYs(1);
  MagField::GetPtr()->Read(smf);
  MagField::GetPtr()->SetScale(1);
  MagField::GetPtr()->SetMagstat(1);

  Int_t refit = 0;
  if (!TkDBc::Head && tkdbc) {
    Int_t setup = 3;
    TkDBc::CreateTkDBc();
    TkDBc::Head->init(setup, tkdbc);
    refit = 1;

    TString ssn = "tksens.dat";
    ifstream ftmp(ssn);
    if (ftmp.good())
      TkDBc::Head->readAlignmentSensor(ssn);
  }

  TFile ofile(oname, "recreate");

  enum { NID = 31, NFD = 67, NED = 7 };

  Int_t   idata[NID];
  Float_t fdata[NFD];
  Float_t edata[NED];

  TTree *ntpl = new TTree("tree", "Tracker tree");
  ntpl->Branch("idata", idata, "run/I:event/I:ient/I:time/I:"  //  0-4
	                       "ncls/I:nhit/I:ntrk/I:"         //  4-7
	                       "ntrd/I:nanti/I:tof/I:"         //  7-9
	                       "nhity/I:nhitc/I:tbpos/I:"      //  9-13
	                       "tkid[9]/I:imlt[9]/I");         // 13-31
  ntpl->Branch("track", fdata, 
	       "p0x/F:p0y/F:angx/F:angy/F:th/F:ph/F:plen/F:"   //  0- 7
	       "trkx[9]/F:trky[9]/F:"                          //  7-25
	       "dbx[3]/F:dby[3]/F:dth/F:dst/F:"                // 25-33
	       "csqx[4]/F:csqy[4]/F:"                          // 33-41
	       "rgt[4]/F:erinv[4]/F:"                          // 41-49
	       "xcog[9]/F:ycog[9]/F");                         // 49-67
  ntpl->Branch("ecal", edata,
	       "dir[3]/F:entry[3]/F:energy/F");

  ofile.cd();

  Int_t np = tbpos.fNpos+1;
  new TH1F("hist01", "nTrTrack",     10, 0, 10);
  new TH1F("hist02", "nTrdTrack",    10, 0, 10);
  new TH1F("hist03", "nAntiCluster", 10, 0, 10);
  new TH1F("hist04", "nTofCluster",  10, 0, 10);

  new TH2F("hist10", "dby0 VS dbx0", 200, -10, 10, 200, -10, 10);
  new TH2F("hist11", "dby1 VS dbx1", 200, -10, 10, 200, -10, 10);
  new TH2F("hist12", "dby2 VS dbx2", 200, -10, 10, 200, -10, 10);
  new TH2F("hist13",  "dth VS pos",  np, 0.5, 0.5+np, 100, 0, 20);
  new TH2F("hist14",   "dd VS pos",  np, 0.5, 0.5+np, 250, 0, 50);
  new TH2F("hist15",  "dd0 VS pos",  np, 0.5, 0.5+np, 250, 0, 50);
  new TH2F("hist16", "pos VS time",  400, 0, 100, np, 0.5, 0.5+np);
  new TH2F("hist17", "pos VS time",  400, 0, 100, np, 0.5, 0.5+np);
  new TH2F("hist18", "pos VS time",  400, 0, 100, np, 0.5, 0.5+np);

  new TH2F("hist21", "csqX VS pos",  np, 0.5, 0.5+np, 550, -10, 100);
  new TH2F("hist22", "csqY VS pos",  np, 0.5, 0.5+np, 550, -10, 100);
  new TH2F("hist23", "R0/R VS pos",  np, 0.5, 0.5+np, 800, -200, 200);
  new TH2F("hist24", "R3/R VS pos",  np, 0.5, 0.5+np, 800, -200, 200);
  new TH2F("hist25", "trY1 VS trX1", 280, -70, 70, 280, -70, 70);
  new TH2F("hist26", "trY4 VS trX4", 280, -70, 70, 280, -70, 70);
  new TH2F("hist27", "trY7 VS trX7", 280, -70, 70, 280, -70, 70);
  new TH2F("hist28", "trY8 VS trX8", 280, -70, 70, 280, -70, 70);
  new TH2F("hist29", "trY9 VS trX9", 280, -70, 70, 280, -70, 70);

  new TH2F("hist31", "Slot1N VS Slot9", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist32", "Slot1  VS Slot9", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist33", "Slot1N VS Slot7", 29, -14.5, 14.5, 29, -14.5, 14.5);
  new TH2F("hist34", "Slot1  VS Slot7", 29, -14.5, 14.5, 29, -14.5, 14.5);

  TStopwatch timer;
  timer.Start();

  cout << nproc << " events to process" << endl;

  Int_t intv  = 10000;
  Int_t nevt  = 0;
  Int_t npsel = 0;
  Int_t nfill = 0;
  Int_t ntk[3] = { 0, 0, 0 };

  if (nproc <= 10000) intv = 10;

  Int_t npos[1000];
  for (Int_t i = 0; i < 1000; i++) npos[i] = 0;

  Int_t tref = 1281308400; // Mon Aug  9 01:00:00 2010
  Int_t tsta = -1;

  for (Int_t ient = ievt; ient < ievt+nproc; ient++) {
    AMSEventR *evt = ch.GetEvent(ient);
    nevt++;

    if (rref == 0) {
      rref = 400;
      if (1281733073 <= evt->Run() && evt->Run() <= 1281755834) rref = 120;
      if (1281895937 <= evt->Run() && evt->Run() <= 1281915703) rref = 120;
      if (1281943338 <= evt->Run() && evt->Run() <= 1281956061) rref = 100;
      if (1281965748 <= evt->Run() && evt->Run() <= 1281988313) rref =  80;
      if (1282027109 <= evt->Run() && evt->Run() <= 1282051003) rref =  60;
      if (1282055201 <= evt->Run() && evt->Run() <= 1282101082) rref =  20;
      if (1282333047 <= evt->Run() && evt->Run() <= 1282351297) rref = 180;
      if (rref != 0) cout << "Rref= " << rref << endl;
    }

    if (rref == 20) brng = 20;

    if (nevt%intv == 0 || nevt == nproc) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%6d %6d %6d %6d %7d %7d (%5.1f%%) "
		   "%4.0f sec (%4.0f Hz) %3.0f MB",
		   nfill, ntk[0], ntk[1], ntk[2], npsel, nevt, 
		   100.*nevt/nproc, tm, nevt/tm, memchk()/1024)
	   << endl;

      if (nevt == nproc)
	cout << Form("%7.3f %7.3f %7.3f %7.3f",
		     1.*nfill/nevt, 1.*ntk[0]/nevt, 
		     1.*ntk[1]/nevt, 1.*ntk[2]/nevt) << endl;
    }

    ((TH1F *)ofile.Get("hist01"))->Fill(evt->nTrTrack());
    ((TH1F *)ofile.Get("hist02"))->Fill(evt->nTrdTrack());
    ((TH1F *)ofile.Get("hist03"))->Fill(evt->nAntiCluster());
    ((TH1F *)ofile.Get("hist04"))->Fill(evt->nTofCluster());

    // Pre-selection
    if (evt->nTrTrack    () == 0    ||
	evt->nTrdTrack   () == 0    ||
	evt->nTofCluster () <  3    ||
	evt->nTrTrack    () > nstrk || 
	evt->nTrdTrack   () > nstrd || 
	evt->nAntiCluster() > nsant || 
	evt->nTofCluster () > nstof) continue;
    npsel++;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;
    if (!trk->ParExists(mf0)) continue;

    if (!trk->ParExists(mfc)) continue;

    Int_t ibp = tbpos.Find(trk->GetP0(mf0), trk->GetDir(mf0), ipos, fpos);
    if (posid != TBpos::E7 && ibp < 0) continue;

    if (ibp < 0) ibp = tbpos.fNpos;
    if (tsta < 0) tsta = evt->fHeader.Time[0]-10;
    Int_t time = evt->fHeader.Time[0]-tsta;

    Double_t zpl8 = TkDBc::Head->GetZlayer(8);
    Double_t zpl9 = TkDBc::Head->GetZlayer(9);

    Float_t plx[9], ply[9];
    for (Int_t i = 0; i < 9; i++) plx[i] = ply[i] = 0;

    for (Int_t i = 0; i < 9; i++) {
      AMSPoint pl = trk->GetPlayer(i, mf0);

      if ((i == 7 && trk->ParExists(mfit[1])) ||
	  (i == 8 && trk->ParExists(mfit[2])))
	pl = trk->InterpolateLayer(i, mf0);

      Double_t max = 200;
      if (TMath::Abs(pl.x()) < max &&
	  TMath::Abs(pl.y()) < max) {
	plx[i] = pl.x();
	ply[i] = pl.y();
      }
    }

    AMSPoint pnt0 = tbpos.GetPos(   0, ibp);
    AMSPoint pnt1 = tbpos.GetPos(zpl8, ibp);
    AMSPoint pnt2 = tbpos.GetPos(zpl9, ibp);

    Double_t dbx0 = trk->GetP0x(mf0)-pnt0.x(), 
             dby0 = trk->GetP0y(mf0)-pnt0.y();
    Double_t dbx1 = plx[7]-pnt1.x(), dby1 = ply[7]-pnt1.y();
    Double_t dbx2 = plx[8]-pnt2.x(), dby2 = ply[8]-pnt2.y();
    Double_t dst  = tbpos.GetDst(ibp, trk->GetP0(mf0), trk->GetDir(mf0));
    Double_t dth  = tbpos.GetDth(ibp,                  trk->GetDir(mf0));
    Double_t dst0 = TMath::Sqrt(dbx0*dbx0+dby0*dby0);

    if (ibp < tbpos.fNpos) {
      ((TH2F *)ofile.Get("hist10"))->Fill(dbx0,  dby0);
      ((TH2F *)ofile.Get("hist11"))->Fill(dbx1,  dby1);
      ((TH2F *)ofile.Get("hist12"))->Fill(dbx2,  dby2);
      ((TH2F *)ofile.Get("hist13"))->Fill(ibp+1, dth);
      ((TH2F *)ofile.Get("hist14"))->Fill(ibp+1, dst);
      ((TH2F *)ofile.Get("hist15"))->Fill(ibp+1, dst0);
      ((TH2F *)ofile.Get("hist16"))->Fill(time/60., ibp+1);
    }
    if (dth > brng || dst > brng*2 || dst0 > brng) ibp = tbpos.fNpos;

    if (posid == TBpos::E7) {
      if (dth > brng/2 || ibp == tbpos.fNpos) continue;
    }


    ((TH2F *)ofile.Get("hist17"))->Fill(time/60., ibp+1);
    if (nrng > 0 && ibp < tbpos.fNpos && ++npos[ibp] > nrng) continue;

    ((TH2F *)ofile.Get("hist18"))->Fill(time/60., ibp+1);

    evt->NTrRecHit();

    if (refit) {
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

    Double_t csqx = trk->GetNormChisqX(mf0);
    Double_t csqy = trk->GetNormChisqY(mf0);
    ((TH2F *)ofile.Get("hist21"))->Fill(ibp+1, csqx);
    ((TH2F *)ofile.Get("hist22"))->Fill(ibp+1, csqy);

    if (csqx < 0 || csqx > crng || csqy < 0 || csqy > crng ||
	trk->GetRigidity(mf0) == 0) continue;

    Double_t drr = rref/trk->GetRigidity(mf0)-1;
    ((TH2F *)ofile.Get("hist23"))->Fill(ibp+1, drr);
    if (TMath::Abs(drr) > rrng) continue;

    if (trk->ParExists(mfit[3])) 
      ((TH2F *)ofile.Get("hist24"))->Fill(ibp+1,
					  rref/trk->GetRigidity(mfit[3])-1);

    ((TH2F *)ofile.Get("hist25"))->Fill(plx[0], ply[0]);
    ((TH2F *)ofile.Get("hist26"))->Fill(plx[3], ply[3]);
    ((TH2F *)ofile.Get("hist27"))->Fill(plx[6], ply[6]);
    ((TH2F *)ofile.Get("hist28"))->Fill(plx[7], ply[7]);
    ((TH2F *)ofile.Get("hist29"))->Fill(plx[8], ply[8]);

    for (Int_t i = 0; i < NID; i++) idata[i] = 0;
    for (Int_t i = 0; i < NFD; i++) fdata[i] = 0;
    for (Int_t i = 0; i < NED; i++) edata[i] = 0;

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
    *(iptr++) = ibp+1;

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

    EcalShowerR *ecal = evt->pEcalShower(0);
    if (ecal) {
      Float_t *eptr = edata;
      *(eptr++) = ecal->Dir[0];
      *(eptr++) = ecal->Dir[1];
      *(eptr++) = ecal->Dir[2];
      *(eptr++) = ecal->Entry[0];
      *(eptr++) = ecal->Entry[1];
      *(eptr++) = ecal->Entry[2];
      *(eptr++) = ecal->EnergyC;
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

    *(fptr++) = dbx0; *(fptr++) = dbx1; *(fptr++) = dbx2;
    *(fptr++) = dby0; *(fptr++) = dby1; *(fptr++) = dby2;
    *(fptr++) = dst;
    *(fptr++) = dth;

    for (Int_t i = 0; i < 4; i++) {
      if (trk->ParExists(mfit[i])) {
	fptr[i]    = trk->GetNormChisqX(mfit[i]);
	fptr[i+ 4] = trk->GetNormChisqY(mfit[i]);
	fptr[i+ 8] = trk->GetRigidity  (mfit[i]);
	fptr[i+12] = trk->GetErrRinv   (mfit[i]);

	if (i > 0) ntk[i-1]++;
      }
      else {
	fptr[i]   = fptr[i+ 4] = -1;  // csqx, csqy
	fptr[i+8] = fptr[i+12] =  0;  // rgt, erinv
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
      Int_t slot  = tkid%100;
      if (layer == 8) tslot[0] = slot;
      if (layer == 9) tslot[1] = slot;
      if (layer == 1) tslot[2] = slot;
      if (layer == 7) tslot[3] = slot;

      TrClusterR *xcls = hit->GetXCluster();
      TrClusterR *ycls = hit->GetYCluster();
      if (!ycls) continue;

      Int_t ily = layer-1;
      cgx[ily] = (!xcls) ? -(hit->GetDummyX()+640)
	       : xcls->GetCofG()+xcls->GetSeedAddress();
      cgy[ily] = ycls->GetCofG()+ycls->GetSeedAddress();
    }
    ((TH2F *)ofile.Get("hist31"))->Fill(tslot[1], tslot[0]);
    ((TH2F *)ofile.Get("hist32"))->Fill(tslot[1], tslot[2]);
    ((TH2F *)ofile.Get("hist33"))->Fill(tslot[3], tslot[0]);
    ((TH2F *)ofile.Get("hist34"))->Fill(tslot[3], tslot[2]);

    ntpl->Fill();
    nfill++;
  }

  ofile.Write();
}
