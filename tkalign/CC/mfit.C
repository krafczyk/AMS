
#include "TFile.h"
#include "TTree.h"

#include "TH3.h"
#include "TMath.h"
#include "TGraphErrors.h"

#include "MpFit.C"
#include "TrExtAlignDB.h"

#include <iostream>
using namespace std;

void update(Int_t layer, Int_t t1, Int_t t2, Double_t *par);

Double_t getf(TFile &f, const char *name);
Int_t    geti(TFile &f, const char *name);


void mfit(Int_t layer, 
	  const char *fname, const char *oname, const char *exalg,
	  Int_t mode, Double_t dt, Double_t range,
	  Double_t rmin = 0, Double_t rmax = -1)
{
  /// Open input file and get tree
  TFile f(fname);
  if (!f.IsOpen()) return;

  TTree *tree = (TTree *)f.Get(Form("tr%d", layer-7));
  if (!tree) return;

  Float_t fdata[20];
  Int_t  *idata = (Int_t *)fdata;
  tree->SetBranchAddress("fdata", fdata);


  /// Open output file
  TFile of(oname, "recreate");
  for (Int_t i = 0; i < 6; i++) {
    TGraphErrors *gr = new TGraphErrors;
    gr->SetName(Form("gr%d", i+1));
    gr->SetMarkerStyle(20);
    of.GetList()->Add(gr);
  }

  TTree *tr = new TTree("tree", "mfit");
  tr->Branch("fdata", fdata,
	     "time/F:iorb/I:rgt/F:x0/F:y0/F:dx/F:dy/F:rx/F:ry/F");

  Double_t binx[73], biny[21], binz[101];
  for (Int_t i = 0; i <=  72; i++) binx[i] = i*10;
  for (Int_t i = 0; i <=  20; i++) biny[i] = TMath::Power(10, 1+i*0.1);
  for (Int_t i = 0; i <= 100; i++) binz[i] = -0.2+i*40e-4;

  TH3F *hist1 = new TH3F("hist1", "resx", 72, binx, 20, biny, 100, binz);
  TH3F *hist2 = new TH3F("hist2", "resy", 72, binx, 20, biny, 100, binz);


  /// Get parameters
  Int_t toffs = geti(f, "toffs");
  Int_t tstep = geti(f, "tstep");
  Int_t norb  = geti(f,  "norb");

  TrExtAlignDB::Load(exalg);

  /// Initialization
  Int_t nent = tree->GetEntries();
  MpFit *mpf = new MpFit(nent);

  Float_t *fbuf = new Float_t[nent];
  Int_t   *ibuf = new Int_t  [nent];

  Int_t pfix = 0;
  if (mode/10 > 0) { pfix = 0x31; MpFit::fWx = 0; }
  mode = mode%10;

  Double_t trng = (mode == 2) ? tstep*norb : tstep;
  Int_t    nbm  = (Int_t)(TMath::Floor(trng/60/dt+0.5));

  if (mode == 2) dt = trng/nbm/60;
  cout << "dt= " << dt << " " << nbm << " " << trng << endl;

  if (mode == 1 && nbm >= 30 &&
      TrExtAlignDB::GetHead()->GetSize(layer) < 240)
    for (Int_t i = 0; i < nbm; i++) {
      Int_t tpar = (Int_t)(toffs+dt* i   *60);
      Int_t tbnd = (Int_t)(toffs+dt*(i+1)*60-1);

      for (Int_t j = 0; j < norb; j++) {
	Int_t tp = tpar+j*tstep;
	Int_t tb = tbnd+j*tstep;
	if (i == nbm-1) {
	  tb = toffs+tstep*(j+1)-1;
	  tp -= (Int_t)(dt*60);
	} 

	TrExtAlignPar apr = TrExtAlignDB::GetHead()->Get (layer, tp);
	TrExtAlignPar &ar = TrExtAlignDB::GetHead()->GetM(layer, tb);
	for (Int_t k = 0; k < 3; k++) {
	  ar.dpos  [k] = apr.dpos  [k];
	  ar.angles[k] = apr.angles[k];
	}
      }
    }

  if (nbm == 0) nbm = 1;

  for (Int_t i = 0; i < nbm; i++) {
    Double_t t1 = dt* i;
    Double_t t2 = dt*(i+1);

    Int_t tmin = (Int_t)(toffs+t1*60);
    Int_t tmax = (Int_t)(toffs+t2*60);

    mpf->Clear();
    Int_t n = 0;
    for (Int_t j = 0; j < nent; j++) {
      tree->GetEntry(j);
      if (rmin < rmax && (fdata[2] < rmin || rmax < fdata[2])) continue;

      Double_t time = (mode == 2) ?  fdata[0]+tstep*idata[1]/60. : fdata[0];
      if (time < t1 || t2 <= time) continue;

      Int_t tpar  = (Int_t)(toffs+time*60);
      Int_t tbnd  = tmax+tstep*idata[1]-1;
      Int_t tfind = TrExtAlignDB::GetHead()->Find(layer, tpar);

      if (mode == 1 && i == 30) tpar -= 2;

      TrExtAlignPar apar;
      if (mode == 2 || (tmin <= tfind && tfind < tmax))
	apar = TrExtAlignDB::GetHead()->Get (layer, tpar);
      else {
	cout << "GetM: " << tbnd << " | " << tmin << " " << tmax << " | "
	     << tpar << " " << tfind << " | "
	     << t1   << " " << time  << " " << t2 << " | "
	     << TrExtAlignDB::GetHead()->GetSize(layer) << endl;

        if (tfind-tmax < 180*3) {
          for (Int_t j = 0; j < norb; j++) {
            Int_t tp = tpar+j*tstep;
            Int_t tb = tbnd+j*tstep;
            TrExtAlignPar apr = TrExtAlignDB::GetHead()->Get (layer, tp);
            TrExtAlignPar &ar = TrExtAlignDB::GetHead()->GetM(layer, tb);
            for (Int_t k = 0; k < 3; k++) {
              ar.dpos  [k] = apr.dpos  [k];
              ar.angles[k] = apr.angles[k];
            }
          }
        }
        apar = TrExtAlignDB::GetHead()->GetM(layer, tbnd);
      }

      for (Int_t k = 0; k < 3; k++) {
	fdata[10+k] = apar.dpos  [k];
	fdata[13+k] = apar.angles[k];
      }

      mpf->Fill(&fdata[2], range);
      fbuf[n] = fdata[0];
      ibuf[n] = idata[1];
      n++;
    }
    cout << Form("Filled at %3d %5.1f %5.1f : ", i, t1, t2) << n << endl;

    if (mode > 0) {
      mpf->Minimize(pfix);
      cout << endl;
    }

    Double_t par[6];
    for (Int_t j = 0; j < 6; j++) {
      par[j] = mpf->GetParameter(j);

      Double_t fact = (j < 3) ? 1e4 : 30e4;
      TGraphErrors *gr = (TGraphErrors *)of.Get(Form("gr%d", j+1));
      gr->SetPoint(i, (t1+t2)/2, par[j]*fact);
      gr->SetPointError(i, 0, mpf->GetParError(j)*fact);
    }

    if (mode == 1) {
      for (Int_t j = 0; j < norb; j++)
	update(layer, tmin+j*tstep, tmax+j*tstep, par);
    }

    if (mode == 2)
      update(layer, tmin, tmax, par);

    for (Int_t j = 0; j < mpf->GetNent(); j++) {
      mpf->GetData(j, &fdata[2]);
      TVector3 vr = mpf->GetRes(j);
      fdata[0] = fbuf[j];
      idata[1] = ibuf[j];
      fdata[7] = vr.x();
      fdata[8] = vr.y();
      tr->Fill();

      Double_t time = fdata[0]+idata[1]*tstep/60.;
      if (vr.x() != 0) hist1->Fill(time, fdata[2], vr.x());
      if (vr.y() != 0) hist2->Fill(time, fdata[2], vr.y());
    }
  }

  of.Write();

  TString sfn = exalg;
  TFile ef(sfn+".new", "recreate");
  TrExtAlignDB::GetHead()->Write();
}

void update(Int_t layer, Int_t t1, Int_t t2, Double_t *par)
{
  vector<uint> vt = TrExtAlignDB::GetHead()->GetVt(layer, t1, t2);

  if (vt.size() == 0)
    TrExtAlignDB::GetHead()->AddM(layer, t2-1, par);

  else
    for (int i = 0; i < vt.size(); i++)
      TrExtAlignDB::GetHead()->AddM(layer, vt.at(i), par);
}

Double_t getf(TFile &f, const char *name)
{
  TObjString *str = (TObjString *)f.Get(name);
  if (str) return str->GetString().Atof();
  return -1;
}

Int_t geti(TFile &f, const char *name)
{
  TObjString *str = (TObjString *)f.Get(name);
  if (str) return str->GetString().Atoi();
  return 0;
}

