
void tkafit(const char *fname, const char *dname = "HistoMan", 
	                       const char *pname = 0)
{
  TFile f(fname);
  if (!f.IsOpen()) return;

  TDirectory *dir = (dname) ? (TDirectory *)f.Get(dname) : &f;
  if (!dir) return;

  ginit();
  gROOT->cd();

  Float_t tsiz = 0.08;
  gStyle->SetTitleFontSize(tsiz);
  gStyle->SetLabelSize(tsiz, "X");
  gStyle->SetLabelSize(tsiz, "Y");
  gStyle->SetTitleSize(tsiz, "X");
  gStyle->SetTitleSize(tsiz, "Y");

  TCanvas *c1 = new TCanvas("c1", "tkafit", 0, 0, 1000, 700);

  TString spn = (pname) ? pname : "tkafit.ps";
  c1->SaveAs(spn+"[");

  for (Int_t i = 0; i < 3; i++) {
    c1->Clear();
    c1->Divide(5, 2, 0.001, 0.001);

    Int_t hid  = (i  < 2) ? i*2+1 : 1;
    Int_t axis = (i != 2) ? 1 : 2;

    TH1F *pfit1 = 0;
    TH1F *pfit2 = 0;
    if (axis == 2) {
      pfit1 = new TH1F("pfit1", "par(1)", 9, 0.5, 9.5);
      pfit2 = new TH1F("pfit2", "par(1)", 9, 0.5, 9.5);
    }

    for (Int_t j = 0; j < 9; j++) {
      c1->cd(j+1);

      Double_t rng = 10;

      TH1F *prof1 = 0;
      TH1F *prof2 = 0;

      for (Int_t k = 0; k < 2; k++) {
	TString shn = Form("TrRes%d%d", j+1, hid+k);
	TH3F *hist = (TH3F *)dir->Get(shn);
	if (!hist) {
	  cout << "Hist not found: " << shn.Data() << endl;
	  continue;
	}

	Int_t   rebin = (j == 0 || j == 8) ? 2 : 1;
	Double_t xrng = (j == 8) ? 30 : 0;

	shn = Form("prof%d%d%d", i+1, j+1, k+1);
	Int_t   col = (k == 0) ? 2 : 4;
	TH1F  *prof = fitp(hist, shn, axis, xrng, rebin, col);
	if (!prof) continue;

	while (prof->GetMinimum() < -rng) rng *= 2;
	while (prof->GetMaximum() >  rng) rng *= 2;

	if (k == 0) prof1 = prof;
	if (k == 1) prof2 = prof;
      }

      prof1->SetMinimum(-rng);
      prof1->SetMaximum( rng);

      if (i == 2) prof1->SetXTitle("Inclination");
      else        prof1->SetXTitle(Form("%c coo", 'X'+i%3));
      prof1->SetYTitle("Residual (#mum)");

      prof1->GetXaxis()->SetNdivisions(506);
      prof1->GetYaxis()->SetNdivisions(506);

      prof1->Draw();
      prof2->Draw("same");

      if (axis == 2) {
	TF1 *func1 = prof1->GetFunction("pol1");
	TF1 *func2 = prof2->GetFunction("pol1");
	if (func1) {
	  pfit1->SetBinContent(j+1, func1->GetParameter(1));
	  pfit1->SetBinError  (j+1, func1->GetParError (1));
	  func1->Draw("same");
	}
	if (func2) {
	  pfit2->SetBinContent(j+1, func2->GetParameter(1));
	  pfit2->SetBinError  (j+1, func2->GetParError (1));
	  func2->Draw("same");
	}
      }
    }

    if (axis == 2) {
      c1->cd(10);

      Double_t hmin = -10;
      Double_t hmax =  10;
      while (pfit1->GetMinimum() < hmin) hmin *= 2;
      while (pfit1->GetMaximum() > hmax) hmax *= 2;
      while (pfit2->GetMinimum() < hmin) hmin *= 2;
      while (pfit2->GetMaximum() > hmax) hmax *= 2;

      pfit1->SetXTitle("Layer");
      pfit1->SetYTitle("par(1) = Z-shift (#mum)");

      pfit1->SetMinimum(hmin);
      pfit1->SetMaximum(hmax);
      pfit1->SetMarkerStyle(21); pfit1->SetMarkerColor(2);
      pfit2->SetMarkerStyle(21); pfit2->SetMarkerColor(4);
      pfit1->SetLineColor(2);
      pfit2->SetLineColor(4);
      pfit1->Draw();
      pfit2->Draw("same");
    }

    c1->SaveAs(spn);
  }

  c1->SaveAs(spn+"]");
}

TH1F *fitp(TH3F *hist, const char *hname, Int_t axis, Double_t xrng = 0,
	   Int_t rebin = 1, Int_t col = 1, Int_t sty = 21)
{
  Int_t nbx = hist->GetNbinsX();
  Int_t nby = hist->GetNbinsY();
  Int_t nbz = hist->GetNbinsZ();
  TAxis *ax = hist->GetXaxis();
  TAxis *ay = hist->GetYaxis();
  TAxis *az = hist->GetZaxis();

  Int_t     nbn = (axis == 1) ? nbx : nby;
  Double_t xmin = (axis == 1) ? ax->GetXmin() : ay->GetXmin();
  Double_t xmax = (axis == 1) ? ax->GetXmax() : ay->GetXmax();

  Double_t frng = 50;
  if (az->GetXmax() > 200) frng = 500;

  if (rebin > 1) nbn /= rebin;

  TH1F *prof = new TH1F(hname, hist->GetTitle(), nbn, xmin, xmax);
  TH1D *htmp = 0;

  Double_t wthd = hist->GetSumOfWeights()/nby*0.1;
  if (wthd < 200) wthd = 200;
  if (wthd > 1e3) wthd = 1e3;

  Int_t jb1 = 1;
  Int_t jb2 = (axis == 1) ? nby : nbx;
  if (axis == 2 && xrng > 0) {
    jb1 = ax->FindBin(-xrng);
    jb2 = ax->FindBin( xrng)-1;
  }

  for (Int_t i = 0; i < nbn; i++) {
    Int_t ib1 = i*rebin+1;
    Int_t ib2 = (i+1)*rebin;

    htmp = (axis == 1) ? hist->ProjectionZ("htmp", ib1, ib2, jb1, jb2)
                       : hist->ProjectionZ("htmp", jb1, jb2, ib1, ib2);
    if (htmp->GetSumOfWeights() < wthd) continue;
    htmp->Fit("gaus", "q0");

    TF1 *func = htmp->GetFunction("gaus");
    if (!func) continue;

    if (frng > 0) {
      Double_t mn = func->GetParameter(1);
      if (htmp->GetXaxis()->GetXmin() < mn-frng &&
	  htmp->GetXaxis()->GetXmax() > mn+frng) {
	htmp->Fit("gaus", "q0", "", mn-frng, mn+frng);
	func = htmp->GetFunction("gaus");
      }
      if (!func) continue;
    }

    if (func->GetParError(1) < 10) {
      prof->SetBinContent(i+1, func->GetParameter(1));
      prof->SetBinError  (i+1, func->GetParError (1));
    }
  }
  delete htmp;

  prof->SetMarkerStyle(sty);
  prof->SetMarkerColor(col);
  prof->SetLineColor(col);

  if (axis == 2) {
    prof->Fit("pol1", "q0", "", -0.5, 0.5);
    TF1 *func = prof->GetFunction("pol1");
    if (func) {
      func->SetLineColor(col);
      func->SetLineWidth(2);
    }
  }

  return prof;
}

void ginit(void)
{
  gStyle->Reset();
  gStyle->SetOptStat(0);

  gStyle->SetCanvasColor   (10);
  gStyle->SetPadColor      (10);
  gStyle->SetFrameFillColor(10);
  gStyle->SetTitleFillColor(10);

  gStyle->SetPadTopMargin(0.12);
  gStyle->SetPadLeftMargin(0.17);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);

  gROOT->GetColor( 3)->SetRGB(0.0, 0.5, 0.0);
  gROOT->GetColor( 5)->SetRGB(0.7, 0.5, 0.0);
  gROOT->GetColor( 6)->SetRGB(0.7, 0.0, 0.7);
  gROOT->GetColor( 7)->SetRGB(0.0, 0.6, 0.7);
  gROOT->GetColor(12)->SetRGB(0.5, 0.0, 0.0);
  gROOT->GetColor(13)->SetRGB(0.0, 0.5, 0.0);
  gROOT->GetColor(14)->SetRGB(0.0, 0.0, 0.5);
  gROOT->GetColor(15)->SetRGB(0.6, 0.3, 0.0);
  gROOT->GetColor(16)->SetRGB(0.4, 0.0, 0.4);
  gROOT->GetColor(17)->SetRGB(0.0, 0.3, 0.3);
}
