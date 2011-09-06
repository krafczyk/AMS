
#include "amschain.h"
#include "root.h"
#include "TStopwatch.h"
#include "TrExtAlignDB.h"
#include "TrCharge.h"
#include "TrRecon.h"
#include "TrTrackSelection.h"

#include <signal.h>

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


void trcls(const char *fname, const char *oname, const char *tkdbc)
{
  AMSChain ch;
  TString sfn = fname;
  if (!sfn.Contains(".root")) sfn += "*.root";
  if (ch.Add(sfn) <= 0) return;

  Int_t ntr  = ch.GetNtrees();
  Int_t nent = ch.GetEntries();
  if (ntr <= 0 || nent <= 0) return;

  cout << "Ntr,Nent= " << ntr << " " << nent << endl;

  for (Int_t i = 0; i < ntr; i++)
    cout << ch.GetListOfFiles()->At(i)->GetTitle() << endl;

  Int_t   idata[13];
  Float_t fdata[28];

  TFile of(oname, "recreate");
  TTree *tree = new TTree("tree", "trcls");
  tree->Branch("idata", idata, "run/I:event/I:ient/I:time/I:tkml[9]/I");
  tree->Branch("fdata", fdata, "engc/F:enge/F:rgt/F:chrg/F:"
	                       "p0x/F:p0y/F:dzx/F:dzy/F:csqx/F:csqy/F:"
	                       "xcog[9]/F:ycog[9]/F");

  if (tkdbc && tkdbc[0] && tkdbc[0] != '0') {
    ch.GetEvent(0);
    TkDBc::Head->init(3, tkdbc);
  }
  TrExtAlignDB::OverLoadFlag = 0;

  Int_t malg = 2; // kAlcaraz | kMultScat
  Int_t pat0 = 3; // Inner only

  Int_t nevt = 0;
  Int_t nrsl = 0;
  Int_t npsl = 0;
  Int_t nfil = 0;
  Int_t intv = 10000;

  signal(SIGTERM, handler);
  signal(SIGINT,  handler);

  TStopwatch timer;
  timer.Start();

  for (Int_t ient = 0; ient < nent && !SigTERM; ient++) {
    AMSEventR *evt = ch.GetEvent(ient);
    nevt++;

    if (nevt%intv == 0 || nevt == nent) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%6d %6d %6d %7d (%5.1f%%) %4.0f sec (%4.1f kHz)",
		   nrsl, npsl, nfil, nevt,
		   100.*nevt/nent, tm, nevt/tm*1e-3)
	   << endl;
    }
    if (evt->nTrRecHit() >= 1600) continue;
    if (evt->nTrTrack () !=    1) continue;

    TrTrackR *trk = evt->pTrTrack(0);
    if (!trk) continue;

    Double_t chgp = TrCharge::GetQ(trk, 1);
    Double_t chgn = TrCharge::GetQ(trk, 0);
    if (chgp <= 0 || chgn <= 0) continue;

    Int_t mfp = trk->iTrTrackPar(malg, pat0, 0);
    if (mfp < 0) continue;

    Double_t rgtp = trk->GetRigidity(mfp);
    if (TMath::Abs(rgtp) < 5) continue;


    //////////////////// Recalc and refit ////////////////////
    trk->RecalcHitCoordinates();

    Int_t mf0 = trk->iTrTrackPar(malg, pat0, 2);
    if (mf0 < 0) continue;

    Double_t rgt0 = trk->GetRigidity(mf0);
    if (TMath::Abs(rgt0) < 10) continue;
    nrsl++;


    //////////////////// Pre-selection ////////////////////
    Bool_t psel = kTRUE;
    Int_t  span = (TrTrackSelection::GetSpanFlags(trk) & 0xff);
    if (!(span & TrTrackSelection::kMaxInt) ||
	!(span & TrTrackSelection::kAllPlane)) psel = kFALSE;

    if   (!(span & TrTrackSelection::kHalfL1N)) {
      if (!(span & TrTrackSelection::kHalfL9)) psel = kFALSE;

      AMSPoint pnt = trk->InterpolateLayerJ(9);
      if (TMath::Abs(pnt.x()) > 33) psel = kFALSE;
    }
    if (psel) npsl++;

    TrTrackR::AdvancedFitBits = 0x0f;

    TrRecon trec;
    Int_t nadd = trec.MergeExtHits(trk, mf0);
    if (!psel && nadd <= 0) continue;

    //////////////////// Remerge ////////////////////
    if (nadd > 0) {
      psel = kTRUE;
      span = (TrTrackSelection::GetSpanFlags(trk) & 0xff);
      if (!(span & TrTrackSelection::kMaxInt) ||
	  !(span & TrTrackSelection::kAllPlane)) psel = kFALSE;

      if   (!(span & TrTrackSelection::kHalfL1N)) {
	if (!(span & TrTrackSelection::kHalfL9)) psel = kFALSE;

	AMSPoint pnt = trk->InterpolateLayerJ(9);
	if (TMath::Abs(pnt.x()) > 33) psel = kFALSE;
      }
    }
    if (!psel) continue;

    idata[0] = evt->Run();
    idata[1] = evt->Event();
    idata[2] = ient;
    idata[3] = evt->fHeader.Time[0];

    EcalShowerR *ecal = evt->pEcalShower(0);
    fdata[0] = (ecal) ? ecal->EnergyC : 0;
    fdata[1] = (ecal) ? ecal->EnergyE : 0;
    fdata[2] = rgt0;
    fdata[3] = (chgp+chgn)/2;
    fdata[4] = trk->GetP0x(mf0);
    fdata[5] = trk->GetP0y(mf0);
    fdata[6] = trk->GetThetaXZ(mf0);
    fdata[7] = trk->GetThetaYZ(mf0);
    fdata[8] = trk->GetNormChisqX(mf0);
    fdata[9] = trk->GetNormChisqY(mf0);

    if (evt->nMCEventg() > 0) {
      MCEventgR *mcg = evt->pMCEventg(0);
      if (mcg) fdata[0] = mcg->Momentum;
    }

    Int_t   *tkml = &idata[ 4];
    Float_t *xcog = &fdata[10];
    Float_t *ycog = &fdata[19];
    for (Int_t i = 0; i < 9; i++) { tkml[i] = 0; xcog[i] = ycog[i] = 0; }

    for (Int_t i = 0; i < trk->GetNhits(); i++) {
      TrRecHitR *hit = trk->GetHit(i);
      if (!hit) continue;

      Int_t layer = hit->GetLayer();
      //if (layer != 8 && layer != 9) continue;

      Int_t il   = layer-1;//layer-8;
      Int_t tkid = hit->GetTkId();
      Int_t imlt = hit->GetResolvedMultiplicity();

      TrClusterR *clx = hit->GetXCluster();
      TrClusterR *cly = hit->GetYCluster();
      tkml[il] = TMath::Sign(imlt*1000+TMath::Abs(tkid), tkid);
      xcog[il] = (!clx) ? -(hit->GetDummyX()+640)
	       : clx->GetCofG()+clx->GetSeedAddress();
      ycog[il] = cly->GetCofG()+cly->GetSeedAddress();
    }

    tree->Fill();
    nfil++;
  }

  of.cd();
  tree->Write();
}
