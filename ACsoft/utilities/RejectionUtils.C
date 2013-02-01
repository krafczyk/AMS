
#include "RejectionUtils.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TMath.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>

#include <string>

using namespace std;

#define DEBUG 0
#include <debugging.hh>

void CalculatePositronFraction(double ep, double eep, double em, double eem, double &PosFrac, double &PosFracError) {
// # Error Propagation Positron Fraction with MAPLE
// (simplify(diff(ep/(ep+em),ep))*eep)^2 + (simplify(diff(ep/(ep+em),em))*eem)^2;

  double denominator  = pow(ep+em,4);
  double nominator1   = pow(em,2)*pow(eep,2);
  double nominator2   = pow(ep,2)*pow(eem,2);
  PosFracError = sqrt(nominator1/denominator + nominator2/denominator);
  PosFrac      = ep/(ep+em);
  return;
}


TGraphAsymmErrors* GetRejection(TH1 *hElec, TH1* hProt, double EffValue, ClassResults *Results) {
  
  int   nBinX  = hElec->GetNbinsX();
  double nElecAll = hElec->Integral(0,nBinX+1);
  double nProtAll = hProt->Integral(0,nBinX+1);
  if (nElecAll<4 || nProtAll<4) return 0;

  vector<double> v_Cut, v_ElecEff, v_eElecEffUp, v_eElecEffDown, v_ProtEff, v_eProtEffUp, v_eProtEffDown;
  vector<double> v_ProtRej, v_eProtRejUp, v_eProtRejDown;
  vector<double> v_nElec, v_nProt;
  for (int iBin=2; iBin<=nBinX; iBin++) {
    double xc    = hElec->GetBinCenter(iBin);
    double nElec   = hElec->Integral(0,iBin);
    double nProt   = hProt->Integral(0,iBin);
    if( nElec <= 0. || nProt <= 0.0 ) continue;
    v_nElec.push_back(nElec);
    v_nProt.push_back(nProt);
    v_Cut.push_back(xc);

    double ElecEff=0.0, eElecEffUp=0.0, eElecEffDown=0.0;
    RootGetEffciencyBayesian(nElec, nElecAll, ElecEff, eElecEffUp, eElecEffDown);
    v_ElecEff.push_back(ElecEff);
    v_eElecEffUp.push_back(eElecEffUp);
    v_eElecEffDown.push_back(eElecEffDown);

    double ProtEff=0.0, eProtEffUp=0.0, eProtEffDown=0.0;
    RootGetEffciencyBayesian(nProt, nProtAll, ProtEff, eProtEffUp, eProtEffDown);
    v_ProtEff.push_back(ProtEff);
    v_eProtEffUp.push_back(eProtEffUp);
    v_eProtEffDown.push_back(eProtEffDown);

    double ProtRej      = 1.0/ProtEff;
    double eProtRejUp   = eProtEffUp/ProtEff/ProtEff;
    double eProtRejDown = eProtEffDown/ProtEff/ProtEff;
    v_ProtRej.push_back(ProtRej);
    v_eProtRejUp.push_back(eProtRejUp);
    v_eProtRejDown.push_back(eProtRejDown);
    DEBUG_OUT << "iBin " << iBin << " prej " << ProtRej << " + " << eProtRejUp << " - " << eProtRejDown << endl;
  }

  TGraphAsymmErrors *grProt  = new TGraphAsymmErrors(v_ElecEff.size(),&v_ElecEff[0],&v_ProtRej[0],&v_eElecEffDown[0],&v_eElecEffUp[0],&v_eProtRejDown[0],&v_eProtRejUp[0]);

  if (v_ElecEff.at(0)>EffValue || v_ElecEff.at(v_ElecEff.size()-1)<EffValue) return grProt;

  TGraph  *grEffCut  = new TGraph(v_ElecEff.size(),&v_ElecEff[0],&v_Cut[0]);
  TGraph  *grCutElec  = new TGraph(v_Cut.size(),&v_Cut[0],&v_nElec[0]);
  TGraph  *grCutProt  = new TGraph(v_Cut.size(),&v_Cut[0],&v_nProt[0]);
  Results->Cut     = grEffCut->Eval(EffValue);
  Results->nElec    = grCutElec->Eval(Results->Cut);
  Results->nProt    = grCutProt->Eval(Results->Cut);

  RootGetEffciencyBayesian(Results->nElec, nElecAll, Results->ElecEff, Results->eElecEffUp, Results->eElecEffDown);
  RootGetEffciencyBayesian(Results->nProt, nProtAll, Results->ProtEff, Results->eProtEffUp, Results->eProtEffDown);

  if (Results->ProtEff>0.0 && fabs(Results->ElecEff-EffValue)<1E-4) {
    Results->ProtRej       = 1.0/Results->ProtEff;
    Results->eProtRejUp    = Results->eProtEffUp/Results->ProtEff/Results->ProtEff;
    Results->eProtRejDown  = Results->eProtEffDown/Results->ProtEff/Results->ProtEff;
  }
  
  return grProt;
}

int RootGetEffciencyBayesian(double m, double N, double &Eff, double &eUp, double &eDown) {

  Eff  = 0.0;
  eUp  = 0.0;
  eDown = 0.0;
  if (N<=0.0) return -1;
  if (m< 0.0) return -1;

  TH1D *pass  = new TH1D("pass","m", 1,0.0,1.0);
  TH1D *total = new TH1D("total","N", 1,0.0,1.0);

  pass->Fill(0.5,m);
  total->Fill(0.5,N);
  pass->Sumw2();
  total->Sumw2();
  TGraphAsymmErrors *gr = new TGraphAsymmErrors();
  gr->Divide(pass,total,"cl=0.683 b(1,1) mode");

  double xx;
  gr->GetPoint(0,xx,Eff);
  eUp   = gr->GetErrorYhigh(0);
  eDown  = gr->GetErrorYlow(0);

  pass->Delete();
  total->Delete();

  return 0;
}


TH2F* GetXzProjection(TH3F *hist, string hName, string hTitle, int Bin0, int Bin1) {
  const double* xBin = hist->GetXaxis()->GetXbins()->GetArray();
  const double* zBin = hist->GetZaxis()->GetXbins()->GetArray();
  int nBinX = hist->GetNbinsX();
  int nBinZ = hist->GetNbinsZ();

  int StartBin = 1;
  int EndBin   = hist->GetNbinsY();
  if (Bin0>0) StartBin = Bin0;
  if (Bin1>0) EndBin   = Bin1;

  TH2F *h2d = new TH2F(hName.c_str(),hTitle.c_str(),nBinX,&xBin[0],nBinZ,&zBin[0]);
  for (int iBinX=1; iBinX<=nBinX; iBinX++) {
    double x = hist->GetXaxis()->GetBinCenter(iBinX);
    for (int iBinZ=1; iBinZ<=nBinZ; iBinZ++) {
      double z = hist->GetZaxis()->GetBinCenter(iBinZ);
      for (int iBinY=StartBin; iBinY<=EndBin; iBinY++) {
        double value = hist->GetBinContent(iBinX,iBinY,iBinZ);
        h2d->Fill(x,z,value);
      }
    }
  }
  return h2d;
}


TH2F* GetYzProjection(TH3F *hist, string hName, string hTitle, int Bin0, int Bin1) {
  const double* yBin = hist->GetYaxis()->GetXbins()->GetArray();
  const double* zBin = hist->GetZaxis()->GetXbins()->GetArray();
  int nBinY = hist->GetNbinsY();
  int nBinZ = hist->GetNbinsZ();

  int StartBin = 1;
  int EndBin   = hist->GetNbinsX();
  if (Bin0>0) StartBin = Bin0;
  if (Bin1>0) EndBin   = Bin1;

  TH2F *h2d = new TH2F(hName.c_str(),hTitle.c_str(),nBinY,&yBin[0],nBinZ,&zBin[0]);
  for (int iBinY=1; iBinY<=nBinY; iBinY++) {
    double y = hist->GetYaxis()->GetBinCenter(iBinY);
    for (int iBinZ=1; iBinZ<=nBinZ; iBinZ++) {
      double z = hist->GetZaxis()->GetBinCenter(iBinZ);
      for (int iBinX=StartBin; iBinX<=EndBin; iBinX++) {
        double value = hist->GetBinContent(iBinX,iBinY,iBinZ);
        h2d->Fill(y,z,value);
      }
    }
  }
  return h2d;
}

TGraphErrors* RootGetGaussXslices(TH2 *hist, double X0, double X1) {
  int nBinX   = hist->GetNbinsX();

  TGraphErrors *gr = new TGraphErrors();
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(1.5);

  const char *hname = hist->GetName();

  for (int iBinX=1; iBinX<=nBinX; iBinX++) {
    char  hpName[200];
    sprintf(hpName,"hpMX_%s_%d",hname,nBinX);
    TH1D   *hp   = hist->ProjectionY(hpName,iBinX,iBinX);
    if (hp->GetEntries()<20) continue;
    int   rc    = hp->Fit("gaus","Q0","",X0,X1);
    if (rc<0) continue;
    TF1 *FitFun  = hp->GetFunction("gaus");
    //double chi2  = FitFun->GetChisquare();
    double Mean  = FitFun->GetParameter(1);
    double eMean  = FitFun->GetParError(1);
    //printf("iBinX=%6d n=%9.1f chi2=%9.1f Mean=%9.4f eMean=%9.4f\n",iBinX,hp->GetEntries(),chi2,Mean,eMean);
    int nPointGr = gr->GetN();
    gr->SetPoint(nPointGr,hist->GetXaxis()->GetBinCenter(iBinX),Mean);
    gr->SetPointError(nPointGr,0,eMean);
    delete hp;
  }
  return gr;
}


void RootPadStyle(double scale) {
  gPad->SetTicks();
  gPad->SetLeftMargin  ( 0.05*scale );
  gPad->SetRightMargin ( 0.05*scale );
  //gPad->SetRightMargin ( 0.12*scale );
  gPad->SetBottomMargin( 0.12*scale  );
}

void RootHistStyle1D(TH1 *hist, double scale) {
  hist->GetXaxis()->SetTitleOffset( 1.25 );
  hist->GetYaxis()->SetTitleOffset( 1.0 );
  hist->GetXaxis()->SetTitleSize( 0.045*scale );
  hist->GetYaxis()->SetTitleSize( 0.045*scale );
  Float_t labelSize = 0.04*scale;
  hist->GetXaxis()->SetLabelSize( labelSize );
  hist->GetYaxis()->SetLabelSize( labelSize );
}

void RootHistStyle2D(TH2 *hist, double scale) {
  hist->GetXaxis()->SetTitleOffset( 1.25 );
  hist->GetYaxis()->SetTitleOffset( 0.5 );
  hist->GetXaxis()->SetTitleSize( 0.045*scale );
  hist->GetYaxis()->SetTitleSize( 0.045*scale );
  Float_t labelSize = 0.04*scale;
  hist->GetXaxis()->SetLabelSize( labelSize );
  hist->GetYaxis()->SetLabelSize( labelSize );
}
