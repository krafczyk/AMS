
#include "TkDBc.h"
#include "TChain.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TMath.h"

Double_t fit(Int_t  *tkml, Float_t *xcog, Float_t *ycog, Float_t *zref,
	     Float_t *fd1, Float_t *fd2);
void wstr(const char *name, const char *txt);

void refit(const char *fname, const char *oname, const char *tkdbc,
	   Int_t toffs, Int_t tstep, Int_t norb)
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

  TkDBc::CreateTkDBc();
  if (tkdbc && tkdbc[0] && tkdbc[0] != '0')
    TkDBc::Head->init(3, tkdbc);
  else
    TkDBc::Head->init(3);

  TFile of(oname, "recreate");
  TTree *tr1 = new TTree("tr1", "refit L8");
  TTree *tr2 = new TTree("tr2", "refit L9");

  Float_t fd1[10]; Int_t *id1 = (Int_t *)fd1;
  Float_t fd2[10]; Int_t *id2 = (Int_t *)fd2;
  tr1->Branch("fdata", fd1, "time/F:iorb/I:"
	                    "rgt/F:x0/F:y0/F:dx/F:dy/F:hx/F:hy/F:hz/F");
  tr2->Branch("fdata", fd2, "time/F:iorb/I:"
	                    "rgt/F:x0/F:y0/F:dx/F:dy/F:hx/F:hy/F:hz/F");

  Int_t   idata[20];
  Float_t fdata[50];

  TBranch *bri = 0;
  TBranch *brf = 0;

  Int_t bent = nent;
  Int_t nfil = 0;

  Float_t zref[2] = { 162.00, -137.58 };

  TStopwatch timer;
  timer.Start();

  cout << "Tstep,Toffs,Norb= " << tstep << " " << toffs << " " << norb << endl;
  cout << "nent= " << nent << endl;

  for (Int_t i = 0; i < nent; i++) {
    if ((i > 0 && i%50000 == 0) || i == nent-1) {
      Double_t tm = timer.RealTime();
      timer.Continue();
      cout << Form("%7d %7d (%5.1f%%) %4.0f sec (%4.1f kHz)",
		   nfil, i, 100.*i/nent, tm, i/tm*1e-3) << endl;
    }

    Int_t ient = ch.LoadTree(i);
    if (!bri || !brf || ient < bent) {
      bri = ch.GetBranch("idata");
      brf = ch.GetBranch("fdata");
      bri->SetAddress(idata);
      brf->SetAddress(fdata);
    }
    bent = ient;

    bri->GetEntry(ient);
    Int_t time = idata[3];
    Int_t rime = time-toffs;
    Int_t iorb = (tstep > 0) ? rime/tstep : 0;
    Int_t tsec = (tstep > 0) ? rime%tstep : rime;
    if (rime < 0 || iorb >= norb) continue;

    brf->GetEntry(ient);
    Float_t csqx = fdata[8];
    Float_t csqy = fdata[9];
    Double_t rgt = fdata[2];
    if (csqx > 1e3 || csqy > 1e3 || TMath::Abs(rgt) < 10) continue;

    for (Int_t j = 0; j < 8; j++) fd1[j] = fd2[j] = 0;

    Int_t   *tkml = &idata[ 4];
    Float_t *xcog = &fdata[10];
    Float_t *ycog = &fdata[19];
    if (fit(tkml, xcog, ycog, zref, &fd1[2], &fd2[2]) < 0) continue;

    Float_t tmin = tsec/60.;
    fd1[0] = tmin; id1[1] = iorb;
    fd2[0] = tmin; id2[1] = iorb;

    if (fd1[2] != 0) tr1->Fill();
    if (fd2[2] != 0) tr2->Fill();
    nfil++;
  }

  of.Write();
  of.cd();
  wstr("toffs", Form("%d", toffs));
  wstr("tstep", Form("%d", tstep));
  wstr("norb",  Form("%d", norb));
  wstr("zref1", Form("%f", zref[0]));
  wstr("zref2", Form("%f", zref[1]));
}

void wstr(const char *name, const char *txt)
{
  TObjString str(txt);
  str.Write(name);
}

#include "TkCoo.h"
#include "TrFit.h"

Double_t fit(Int_t  *tkml, Float_t *xcog, Float_t *ycog, Float_t *zref, 
	     Float_t *fd1, Float_t *fd2)
{
  Double_t ErrX =  25e-4;
  Double_t ErrY =  13e-4;
  Double_t ErrZ = 300e-4;
  Double_t ErrT = 300e-4;

  TrFit ttmp;
  for (Int_t i = 0; i < 7; i++) {
    if (tkml[i] == 0 || xcog[i] < 0 || ycog[i] < 0) continue;
    Int_t imlt = TMath::Abs(tkml[i])/1000;
    Int_t tkid = tkml[i]%1000;
    AMSPoint loc(TkCoo::GetLocalCoo(tkid, xcog[i], imlt),
		 TkCoo::GetLocalCoo(tkid, ycog[i], imlt), 0);
    AMSPoint err(ErrT, ErrT, ErrZ);
    ttmp.Add(TkCoo::GetGlobalA(tkid, loc), err);
  }
  if (ttmp.LinearFit() <  0) return -1;
  if (ttmp.GetChisqX() > 50) return -2;

  TrFit trfit;
  for (Int_t i = 0; i < 7; i++) {
    if (tkml[i] == 0) continue;
    Int_t imlt = TMath::Abs(tkml[i])/1000;
    Int_t tkid = tkml[i]%1000;
    Int_t layr = TMath::Abs(tkid)/100;

    AMSPoint loc(TkCoo::GetLocalCoo(tkid, TMath::Abs(xcog[i]), imlt),
		 TkCoo::GetLocalCoo(tkid, TMath::Abs(ycog[i]), imlt), 0);
    AMSPoint err(ErrX, ErrY, ErrZ);
    if (xcog[i] < 0) err[0] = -1;
    if (ycog[i] < 0) err[1] = -1;

    AMSPoint coo = TkCoo::GetGlobalA(tkid, loc);
    if (xcog[i] < 0) {
      ttmp.Propagate(coo.z());
      coo[0] = ttmp.GetP0x();
    }
    trfit.Add(coo, err);
  }

  trfit._mscat = 0;
  if (trfit.AlcarazFit() < 0) return -3;

  if (trfit.GetNdofX() <= 0 || trfit.GetChisqX()/trfit.GetNdofX() > 1e3 ||
      trfit.GetNdofY() <= 0 || trfit.GetChisqY()/trfit.GetNdofY() > 1e3)
    return -4;

  for (Int_t i = 0; i < 2; i++) {
    Int_t il = i+7;
    if (tkml[il] == 0) continue;

    Int_t imlt = TMath::Abs(tkml[il])/1000;
    Int_t tkid = tkml[il]%1000;

    AMSPoint loc(TkCoo::GetLocalCoo(tkid, TMath::Abs(xcog[il]), imlt),
		 TkCoo::GetLocalCoo(tkid, TMath::Abs(ycog[il]), imlt), 0);

    TkLadder *lad = TkDBc::Head->FindTkId(tkid);
    TkPlane  *pln = lad->GetPlane();
    AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
    AMSPoint  pos = lad->GetPosA()   +lad->GetPos();
    AMSPoint hcoo = pln->GetRotMat()*(rot*loc+pos);
    if (xcog[il] < 0) hcoo[0] += TMath::Sign(1000., hcoo[0]);
    if (ycog[il] < 0) hcoo[1] += TMath::Sign(1000., hcoo[1]);

    Float_t *fdata = (i == 0) ? fd1 : fd2;

    TrProp trp = trfit;
    trp.Propagate(zref[i]);

    fdata[0] = trp.GetRigidity();
    fdata[1] = trp.GetP0x();
    fdata[2] = trp.GetP0y();
    fdata[3] = trp.GetDxDz();
    fdata[4] = trp.GetDyDz();
    fdata[5] = hcoo.x();
    fdata[6] = hcoo.y();
    fdata[7] = hcoo.z();
  }

  return trfit.GetChisq();
}
