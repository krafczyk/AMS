
#include "RejectionUtils.hh"

#include "Utilities.hh"

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
#include <TEfficiency.h>

#include <QVector>

#include <iostream>
#include <string>
#include <sstream>
#include <assert.h>

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


/** Analyze a pair of electron and proton TRD likelihood histograms.
  *
  * \param[in] hElec electron likelihood distribution
  * \param[in] hProt proton likelihood distribution
  * \param[in] RejValue fixed proton rejection value for which electron efficiency is to be calculated
  * \param[out] Results structure with relevant numbers, in particular proton rejection for chosen electron efficiency
  *
  * \returns graph of proton rejection as a function of electron efficiency
  */
TGraphAsymmErrors* GetEfficiency(TH1 *hElec, TH1* hProt, double RejValue, RejectionStats *Results) {

  assert(RejValue > 1.0);
  return GetRejectionOrEfficiency(hElec,hProt,RejValue,Results);
}

/** Analyze a pair of electron and proton TRD likelihood histograms.
  *
  * \param[in] hElec electron likelihood distribution
  * \param[in] hProt proton likelihood distribution
  * \param[in] EffValue fixed electron efficiency value for which proton rejection is to be calculated
  * \param[out] Results structure with relevant numbers, in particular proton rejection for chosen electron efficiency
  *
  * \returns graph of proton rejection as a function of electron efficiency
  */
TGraphAsymmErrors* GetRejection(TH1 *hElec, TH1* hProt, double EffValue, RejectionStats *Results) {

  assert(EffValue >= 0.0 && EffValue <= 1.0);
  return GetRejectionOrEfficiency(hElec,hProt,EffValue,Results);
}


/** Analyze a pair of electron and proton TRD likelihood histograms.
  *
  * \param[in] hElec electron likelihood distribution
  * \param[in] hProt proton likelihood distribution
  * \param[in] EffOrRejValue if between 0 and 1, interpreted as efficiency, if larger than one, interpreted as rejection,
  *            function will then calculate a proton rejection or electron efficiency, respectively. Resulting graph is identical in both cases.
  * \param[out] Results structure with relevant numbers, in particular proton rejection for chosen electron efficiency
  *
  * \returns graph of proton rejection as a function of electron efficiency
  */
TGraphAsymmErrors* GetRejectionOrEfficiency(TH1 *hElec, TH1* hProt, double EffOrRejValue, RejectionStats *Results) {

  int nBinX  = hElec->GetNbinsX();
  double nElecAll = hElec->Integral(0,nBinX+1);
  double nProtAll = hProt->Integral(0,nBinX+1);

  DEBUG_OUT << "Target value: " << EffOrRejValue << " nBinX: " << nBinX << " nElecAll: " << nElecAll << " nProtAll: " << nProtAll << std::endl;

  if (nElecAll<4 || nProtAll<4) return 0;

  std::vector<double> v_Cut, v_ElecEff, v_eElecEffUp, v_eElecEffDown, v_ProtEff, v_eProtEffUp, v_eProtEffDown;
  std::vector<double> v_ProtRej, v_eProtRejUp, v_eProtRejDown;
  std::vector<double> v_nElec, v_nProt;
  double pRejMin = 1.e99;
  double pRejMax = 0.;
  for (int iBin=2; iBin<=nBinX; iBin++) {
    double xc    = hElec->GetBinCenter(iBin);
    double nElec   = hElec->Integral(0,iBin);
    double nProt   = hProt->Integral(0,iBin);
    if( nElec <= 0. || nProt <= 0.0 ) continue;
    v_nElec.push_back(nElec);
    v_nProt.push_back(nProt);
    v_Cut.push_back(xc);

    double ElecEff=0.0, eElecEffUp=0.0, eElecEffDown=0.0;
    RootGetEfficiencyClopperPearson(nElec, nElecAll, ElecEff, eElecEffUp, eElecEffDown);
    v_ElecEff.push_back(ElecEff);
    v_eElecEffUp.push_back(eElecEffUp);
    v_eElecEffDown.push_back(eElecEffDown);

    double ProtEff=0.0, eProtEffUp=0.0, eProtEffDown=0.0;
    RootGetEfficiencyClopperPearson(nProt, nProtAll, ProtEff, eProtEffUp, eProtEffDown);
    v_ProtEff.push_back(ProtEff);
    v_eProtEffUp.push_back(eProtEffUp);
    v_eProtEffDown.push_back(eProtEffDown);

    double ProtRej      = 1.0/ProtEff;
    if(ProtRej<pRejMin) pRejMin = ProtRej;
    if(ProtRej>pRejMax) pRejMax = ProtRej;
    double eProtRejUp   = eProtEffUp/ProtEff/ProtEff;
    double eProtRejDown = eProtEffDown/ProtEff/ProtEff;
    v_ProtRej.push_back(ProtRej);
    v_eProtRejUp.push_back(eProtRejUp);
    v_eProtRejDown.push_back(eProtRejDown);
    DEBUG_OUT_L(2) << "iBin " << iBin << " eeff " << ElecEff << " prej " << ProtRej << " + " << eProtRejUp << " - " << eProtRejDown << std::endl;
  }

  TGraphAsymmErrors *grProt  = new TGraphAsymmErrors(v_ElecEff.size(),&v_ElecEff[0],&v_ProtRej[0],&v_eElecEffDown[0],&v_eElecEffUp[0],&v_eProtRejDown[0],&v_eProtRejUp[0]);

  bool targetIsEfficiency = (EffOrRejValue <= 1.0);

  if( targetIsEfficiency && ( v_ElecEff.at(0)>EffOrRejValue || v_ElecEff.at(v_ElecEff.size()-1)<EffOrRejValue ) ) {
    WARN_OUT << "Target efficiency " << EffOrRejValue << " outside range " << v_ElecEff.at(0) << " .. " << v_ElecEff.at(v_ElecEff.size()-1) << std::endl;
    return grProt;
  }
  if( !targetIsEfficiency && ( EffOrRejValue < pRejMin || EffOrRejValue > pRejMax ) ){
    WARN_OUT << "Target rejection " << EffOrRejValue << " outside range " << pRejMin << " .. " << pRejMax << std::endl;
    return grProt;
  }

  if(!Results) return grProt;

  TGraph  *grRejCut  = new TGraph(v_ProtRej.size(),&v_ProtRej[0],&v_Cut[0]);
  TGraph  *grEffCut  = new TGraph(v_ElecEff.size(),&v_ElecEff[0],&v_Cut[0]);
  TGraph  *grCutElec  = new TGraph(v_Cut.size(),&v_Cut[0],&v_nElec[0]);
  TGraph  *grCutProt  = new TGraph(v_Cut.size(),&v_Cut[0],&v_nProt[0]);
  // nasty little trick: to avoid code duplication, we treat the following two cases, depending on the value of the input
  // parameter:
  // - an efficiency (0<=eff<=1) is given, and we want to know the corresponding rejection
  // - a rejection (R>1) is given, and we want to know the corresponding efficiency
  double cut = targetIsEfficiency ? grEffCut->Eval(EffOrRejValue) : grRejCut->Eval(EffOrRejValue);
  Results->Cut      = cut;
  Results->grRejectionVsCut = grRejCut;
  Results->nElec    = grCutElec->Eval(cut);
  Results->nProt    = grCutProt->Eval(cut);

  RootGetEfficiencyClopperPearson(Results->nElec, nElecAll, Results->ElecEff, Results->eElecEffUp, Results->eElecEffDown);
  RootGetEfficiencyClopperPearson(Results->nProt, nProtAll, Results->ProtEff, Results->eProtEffUp, Results->eProtEffDown);

  DEBUG_OUT << "Cut: " << cut << std::endl;
  if(DEBUG) Results->Print();

  if( targetIsEfficiency ){
    if( Results->ProtEff>0.0 && fabs(Results->ElecEff-EffOrRejValue)<1.e-4 ) {
      Results->ProtRej       = 1.0/Results->ProtEff;
      Results->eProtRejUp    = Results->eProtEffUp/Results->ProtEff/Results->ProtEff;
      Results->eProtRejDown  = Results->eProtEffDown/Results->ProtEff/Results->ProtEff;
    }
    else WARN_OUT << "Could not find matching efficiency!" << std::endl;
  }
  else{
    if( Results->ProtEff>0.0 && Results->ProtEff<1.0 ) {
      Results->ProtRej       = 1.0/Results->ProtEff;
      Results->eProtRejUp    = Results->eProtEffUp/Results->ProtEff/Results->ProtEff;
      Results->eProtRejDown  = Results->eProtEffDown/Results->ProtEff/Results->ProtEff;
    }
    else WARN_OUT << "Could not find matching rejection: " << Results->ProtRej << " vs " << EffOrRejValue << "!" << std::endl;
  }

  delete grEffCut;
  delete grCutElec;
  delete grCutProt;

  return grProt;
}

/** Scale x-values of a graph.
  *
  * This function will multiply the x-values of all points in the given \p graphWithEfficiencyOnXaxis
  * by the given factor \p scale, typically the efficiency of a quality selection.
  */
void ScaleEfficiencyAxis( TGraph* graphWithEfficiencyOnXaxis, double scale ) {

  for( int i=0; i < graphWithEfficiencyOnXaxis->GetN(); ++i ) {
    double x,y;
    graphWithEfficiencyOnXaxis->GetPoint(i,x,y);
    graphWithEfficiencyOnXaxis->SetPoint(i,x*scale,y);
  }
}


/** Turn a list of rejection info ordered as a function of momentum into the graphs that we want to plot. */
RejectionGraphCollection MakeRejectionGraphs( const std::vector<RejectionStats*> input, Color_t color ) {

  RejectionGraphCollection graphs;

  std::vector<double> v_Rmean, v_ProtEff, v_eProtEffUp, v_eProtEffDown, v_ProtRej, v_eProtRejUp, v_eProtRejDown, v_ElecEff, v_eElecEffUp, v_eElecEffDown, v_Cut;

  for( unsigned int i=0 ; i<input.size() ; ++i ){

    const RejectionStats* Results = input[i];

    v_Rmean.push_back(Results->Rmean());
    v_ProtEff.push_back(Results->ProtEff);
    v_eProtEffUp.push_back(Results->eProtEffUp);
    v_eProtEffDown.push_back(Results->eProtEffDown);
    v_ProtRej.push_back(Results->ProtRej);
    v_eProtRejUp.push_back(Results->eProtRejUp);
    v_eProtRejDown.push_back(Results->eProtRejDown);
    v_ElecEff.push_back(Results->ElecEff);
    v_eElecEffUp.push_back(Results->eElecEffUp);
    v_eElecEffDown.push_back(Results->eElecEffDown);
    v_Cut.push_back(Results->Cut);
  }

  if(v_Rmean.empty()) return graphs;

  graphs.grCut     = new TGraph(v_Rmean.size(),&v_Rmean[0],&v_Cut[0]);
  graphs.grElecEff = new TGraphAsymmErrors(v_Rmean.size(),&v_Rmean[0],&v_ElecEff[0],0,0,&v_eElecEffDown[0],&v_eElecEffUp[0]);
  graphs.grProtEff = new TGraphAsymmErrors(v_Rmean.size(),&v_Rmean[0],&v_ProtEff[0],0,0,&v_eProtEffDown[0],&v_eProtEffUp[0]);
  graphs.grProtRej = new TGraphAsymmErrors(v_Rmean.size(),&v_Rmean[0],&v_ProtRej[0],0,0,&v_eProtRejDown[0],&v_eProtRejUp[0]);

  graphs.grCut->SetLineColor(color);
  graphs.grCut->SetMarkerColor(color);
  graphs.grCut->SetLineWidth(2);
  graphs.grCut->SetMarkerStyle(20);

  graphs.grElecEff->SetLineColor(color);
  graphs.grElecEff->SetMarkerColor(color);
  graphs.grElecEff->SetLineWidth(2);
  graphs.grElecEff->SetMarkerStyle(20);


  graphs.grProtEff->SetLineColor(color);
  graphs.grProtEff->SetMarkerColor(color);
  graphs.grProtEff->SetLineWidth(2);
  graphs.grProtEff->SetMarkerStyle(20);

  graphs.grProtRej->SetLineColor(color);
  graphs.grProtRej->SetMarkerColor(color);
  graphs.grProtRej->SetLineWidth(2);
  graphs.grProtRej->SetMarkerStyle(20);


  return graphs;
}


int RootGetEfficiencyBayesian(double m, double N, double &Eff, double &eUp, double &eDown) {

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

  delete gr;

  DEBUG_OUT << "m=" << m << " N=" << N << " -> Eff=" << Eff << " eUp=" << eUp << " eDown=" << eDown << std::endl;

  return 0;
}


int RootGetEfficiencyClopperPearson(double m, double N, double &Eff, double &eUp, double &eDown) {

  Eff = m/N;
  eUp = TEfficiency::ClopperPearson(N,m,0.683,true) - Eff;
  eDown = Eff - TEfficiency::ClopperPearson(N,m,0.683,false);

  DEBUG_OUT << "m=" << m << " N=" << N << " => Eff=" << Eff << " eUp=" << eUp << " eDown=" << eDown << std::endl;
  return 0;
}


TGraphErrors* RootGetGaussXslices(TH2 *hist, double X0, double X1) {

  int nBinX = hist->GetNbinsX();

  TGraphErrors *gr = new TGraphErrors();
  gr->SetMarkerStyle(20);
  gr->SetMarkerSize(1.5);

  for (int iBinX=1; iBinX<=nBinX; iBinX++) {
    std::stringstream hpName;
    hpName << "hpMX_" << hist->GetName() << "_" << nBinX;
    TH1D *hp = hist->ProjectionY(hpName.str().c_str(),iBinX,iBinX);
    if (hp->GetEntries()<20) continue;
    int rc = hp->Fit("gaus","Q0","",X0,X1);
    if (rc<0) continue;
    TF1 *FitFun  = hp->GetFunction("gaus");
    double Mean  = FitFun->GetParameter(1);
    double eMean  = FitFun->GetParError(1);
    int nPointGr = gr->GetN();
    gr->SetPoint(nPointGr,hist->GetXaxis()->GetBinCenter(iBinX),Mean);
    gr->SetPointError(nPointGr,0,eMean);
    delete hp;
  }
  return gr;
}


TGraph* CreateGraphForProtonRejectionFromPositronPaper2013() {

  QVector<double> qv_R, qv_Rejection;
  qv_R << 1.300 << 1.703 << 2.204 << 2.818 << 3.501 << 4.421 << 5.606 << 7.108 << 8.830 << 12.003 << 14.081 << 18.149 << 22.181 << 28.010 << 35.082 << 43.940
       << 55.944 << 70.936 << 87.763 << 120.282 << 139.952 << 179.649 << 219.552 << 279.529 << 351.547 << 440.313 << 558.308 << 710.826 << 883.053 << 1200.387;
  qv_Rejection << 198.365 << 716.958 << 1790.350 << 4229.559 << 8078.239 << 12021.098 << 15716.913 << 18908.584 << 18908.584 << 17080.424 << 16010.199
               << 16010.199 << 14066.706 << 10959.678 << 8003.906 << 6967.595 << 4388.874 << 3581.233 << 2790.220 << 2095.011 << 1501.969 << 1096.894
               << 903.355 << 659.723 << 481.799 << 249.937 << 139.608 << 60.198 << 26.198 << 5.969;

  TGraph *grProtonRejectionPositronPaper2013 = new TGraph(qv_R.toStdVector().size(),&qv_R.toStdVector()[0], &qv_Rejection.toStdVector()[0]);
  grProtonRejectionPositronPaper2013->SetLineColor(kBlack);
  grProtonRejectionPositronPaper2013->SetMarkerColor(kBlack);
  grProtonRejectionPositronPaper2013->SetMarkerStyle(20);
  grProtonRejectionPositronPaper2013->SetMarkerSize(1.5);
  grProtonRejectionPositronPaper2013->GetXaxis()->SetTitle("Rigidity (GV)");
  grProtonRejectionPositronPaper2013->GetYaxis()->SetTitle("proton rejection");
  grProtonRejectionPositronPaper2013->SetTitle("");
  return grProtonRejectionPositronPaper2013;
}


void RootPadStyle(double scale) {
  gPad->SetTicks();
  gPad->SetLeftMargin  ( 0.05*scale );
  gPad->SetRightMargin ( 0.05*scale );
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



void PerformAnalysisOfRejectionVsRigidity( TH2* h_Elec_Dat, TH2* h_Prot_Dat, TH2* h_Elec_MC, TH2* h_Prot_MC,
                                           TH1* preQualityInputSpectrumElectrons, TH1* postQualityInputSpectrumElectrons, std::string label, bool snapshot ) {

  const double ElecTargetEff = 0.90;

  if( !h_Elec_Dat || !h_Prot_Dat ) return;
  INFO_OUT << "Running analysis \"" << label << "\"..." << std::endl;
  double scale = 1.0;

  std::string labelName = "_" + label;
  std::string labelTitle = " (" + label + ")";

  // calculate efficiency of quality selection as a function of rigidity
  TH1* qualitySelectionEfficiency = 0;
  if( preQualityInputSpectrumElectrons && postQualityInputSpectrumElectrons ){
    std::stringstream qseName;
    qseName << "qualitySelectionEfficiency_" << label;
    qualitySelectionEfficiency = (TH1*)postQualityInputSpectrumElectrons->Clone(qseName.str().c_str());
    qualitySelectionEfficiency->SetTitle("Efficiency of quality selection");
    qualitySelectionEfficiency->Divide(preQualityInputSpectrumElectrons);

    if( !ACsoft::Utilities::TestBinning( h_Elec_Dat->GetXaxis(), qualitySelectionEfficiency->GetXaxis() ) )
      WARN_OUT << "Binning mis-match!" << std::endl;

    TCanvas* qseCanvas = new TCanvas(("qseCanvas"+labelName).c_str(),("Quality selection"+labelTitle).c_str());
    qseCanvas->Divide(2,1);
    qseCanvas->cd(1);
    postQualityInputSpectrumElectrons->SetLineColor(2);
    postQualityInputSpectrumElectrons->SetStats(0);
    postQualityInputSpectrumElectrons->Draw();
    preQualityInputSpectrumElectrons->Draw("SAME");
    gPad->SetLogx();
    gPad->SetLogy();
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,1)
    gPad->BuildLegend();
#endif

    qseCanvas->cd(2);
    qualitySelectionEfficiency->SetStats(0);
    qualitySelectionEfficiency->Draw();
    gPad->SetLogx();
  }

  TCanvas *scatterCanvas = new TCanvas(("rej_vs_rigidity_scatter_canvas"+labelName).c_str(),("Proton electron scatter plots"+labelTitle).c_str());
  scatterCanvas->Divide(1,2);

  h_Elec_Dat->SetMarkerStyle(1);
  h_Prot_Dat->SetMarkerStyle(1);
  h_Elec_Dat->SetMarkerColor(kRed);
  h_Prot_Dat->SetMarkerColor(kBlue);

  scatterCanvas->cd(1);
  RootPadStyle(scale);
  gPad->SetGrid();
  gPad->SetLogx();
  TGraphErrors *grMeanElec_Dat = RootGetGaussXslices(h_Elec_Dat,0.3,0.6);
  TGraphErrors *grMeanProt_Dat = RootGetGaussXslices(h_Prot_Dat,0.8,1.3);
  grMeanProt_Dat->SetMarkerStyle(21);
  grMeanElec_Dat->SetMarkerStyle(21);
  TGraphErrors *grMeanElec_MC=0, *grMeanProt_MC=0;
  if ( h_Elec_MC && h_Prot_MC ) {
    grMeanElec_MC = RootGetGaussXslices(h_Elec_MC,0.3,0.6);
    grMeanProt_MC = RootGetGaussXslices(h_Prot_MC,0.8,1.3);
    grMeanProt_MC->SetMarkerStyle(21);
    grMeanElec_MC->SetMarkerStyle(21);
  }
  h_Elec_Dat->GetYaxis()->SetRangeUser(0.0,2.5);
  h_Elec_Dat->GetXaxis()->SetMoreLogLabels();
  h_Elec_Dat->SetStats(0);
  h_Elec_Dat->Draw();
  h_Prot_Dat->Draw("same");
  grMeanElec_Dat->Draw("P.C.same");
  grMeanProt_Dat->Draw("P.C.same");
  if( grMeanElec_MC && grMeanProt_MC ) {
    grMeanElec_MC->SetLineColor(kMagenta);
    grMeanProt_MC->SetLineColor(kMagenta);
    grMeanElec_MC->Draw("C.same");
    grMeanProt_MC->Draw("C.same");
  }

  scatterCanvas->cd(2);
  RootPadStyle(scale);
  gPad->SetGrid();
  gPad->SetLogx();
  if (h_Elec_MC && h_Prot_MC) {
    h_Elec_MC->SetMarkerStyle(1);
    h_Prot_MC->SetMarkerStyle(1);
    h_Elec_MC->SetMarkerColor(kRed);
    h_Elec_MC->SetLineColor(kRed);
    h_Elec_MC->SetLineStyle(2);
    h_Prot_MC->SetMarkerColor(kBlue);
    h_Prot_MC->SetLineColor(kBlue);
    h_Prot_MC->SetLineStyle(2);
    h_Elec_MC->GetYaxis()->SetRangeUser(0.0,2.5);
    h_Elec_MC->GetXaxis()->SetMoreLogLabels();
    h_Elec_MC->SetStats(0);
    h_Elec_MC->Draw();
    h_Prot_MC->Draw("same");
    grMeanElec_MC->Draw("P.C.same");
    grMeanProt_MC->Draw("P.C.same");
  }

  if( snapshot )
    ACsoft::Utilities::SaveCanvas(scatterCanvas);

  TCanvas *logLCanvas = new TCanvas(("logL_rig_bins_canvas"+labelName).c_str(),("-ln(L)"+labelTitle).c_str());
  int nx=5;
  int ny=4;
  logLCanvas->Divide(nx,ny);

  int nBinX  = h_Elec_Dat->GetNbinsX();
  TH1D *hpElec_Dat[100]={0}, *hpProt_Dat[100]={0};
  TH1D *hpElec_MC[100]={0}, *hpProt_MC[100]={0};
  TGraphAsymmErrors *grProtEff_Dat[100];
  TGraphAsymmErrors *grProtEff_MC[100];
  std::vector<RejectionStats*> v_Results_Dat;
  std::vector<RejectionStats*> v_Results_MC;

  // store rejection and efficiency curves vs rigidity for certain fixed values of efficiency and rejection, respectively
  std::vector<double> fixedEfficiencies;
  std::vector<double> fixedRejections;
  fixedEfficiencies.push_back(0.7);
  fixedEfficiencies.push_back(0.8);
  fixedEfficiencies.push_back(0.9);
  fixedRejections.push_back(100.);
  fixedRejections.push_back(1000.);
  fixedRejections.push_back(10000.);
  // corresponding results
  std::vector<std::vector<RejectionStats*> > v_Results_fixedEffs;
  std::vector<std::vector<RejectionStats*> > v_Results_fixedRejs;
  for( unsigned int i=0 ; i<fixedEfficiencies.size() ; ++i )
    v_Results_fixedEffs.push_back(std::vector<RejectionStats*>());
  for( unsigned int i=0 ; i<fixedRejections.size() ; ++i )
    v_Results_fixedRejs.push_back(std::vector<RejectionStats*>());


  int iPad = 1;
  for (int iBin=1; iBin<=nBinX; iBin++) {
    char hName[1000], hTitle[1000];
    double R1   = h_Elec_Dat->GetXaxis()->GetBinLowEdge(iBin);
    double R2   = R1 + h_Elec_Dat->GetXaxis()->GetBinWidth(iBin);

    if (h_Elec_MC && h_Prot_MC) {
      sprintf(hName,"hpElecMC_%d_%s",iBin,label.c_str());
      hpElec_MC[iBin] = h_Elec_MC->ProjectionY(hName,iBin,iBin);
      sprintf(hName,"hpProtMC_%d_%s",iBin,label.c_str());
      hpProt_MC[iBin] = h_Prot_MC->ProjectionY(hName,iBin,iBin);
      sprintf(hTitle,"MC: Protons R=%6.2f - %6.2f GV; -ln(L) ; 1/N dn/dx",R1,R2);
      hpProt_MC[iBin]->SetTitle(hTitle);
    }

    sprintf(hName,"hpElecDat_%d_%s",iBin,label.c_str());
    hpElec_Dat[iBin]  = h_Elec_Dat->ProjectionY(hName,iBin,iBin);
    sprintf(hName,"hpProtDat_%d_%s",iBin,label.c_str());
    hpProt_Dat[iBin]  = h_Prot_Dat->ProjectionY(hName,iBin,iBin);
    sprintf(hTitle,"Data: Protons R=%6.2f - %6.2f GV; -ln(L) ; 1/N dn/dx",R1,R2);
    hpProt_Dat[iBin]->SetTitle(hTitle);

    hpProt_Dat[iBin]->Sumw2();
    hpElec_Dat[iBin]->Sumw2();
    hpProt_Dat[iBin]->SetLineColor(kBlue);
    hpElec_Dat[iBin]->SetLineColor(kRed);

    if( iPad<=nx*ny && !(iBin%2) ){ // draw only every other likelihood distribution plot
      logLCanvas->cd(iPad);
      gPad->SetGrid();
      gPad->SetLogy();
      if (hpProt_Dat[iBin]->GetEntries()>0) hpProt_Dat[iBin]->DrawNormalized("hist.e");
      if (hpElec_Dat[iBin]->GetEntries()>0) hpElec_Dat[iBin]->DrawNormalized("hist.e.same");

      if (hpProt_MC[iBin] && hpElec_MC[iBin]) {
        if (hpProt_MC[iBin]->GetEntries()>0) hpProt_MC[iBin]->DrawNormalized("hist.same");
        if (hpElec_MC[iBin]->GetEntries()>0) hpElec_MC[iBin]->DrawNormalized("hist.same");
      }
      ++iPad;
    }

    // target efficiency = selection efficiency x efficiency of likelihood cut
    double ElecSelectionEff = qualitySelectionEfficiency ? qualitySelectionEfficiency->GetBinContent(iBin) : 1.0;
    double ElecCutEff = ElecTargetEff / ElecSelectionEff;
    if( ElecCutEff > 1.0 ) ElecCutEff = 1.0;

    RejectionStats *Results_Dat = new RejectionStats(R1,R2);
    grProtEff_Dat[iBin] = GetRejection(hpElec_Dat[iBin], hpProt_Dat[iBin], ElecCutEff, Results_Dat);
    if( grProtEff_Dat[iBin] && Results_Dat->IsUsable() ) {
      Results_Dat->Print();
      v_Results_Dat.push_back(Results_Dat);
    }

    if( hpElec_MC[iBin] && hpProt_MC[iBin] ){
      RejectionStats *Results_MC = new RejectionStats(R1,R2);
      grProtEff_MC[iBin]  = GetRejection(hpElec_MC[iBin], hpProt_MC[iBin], ElecCutEff, Results_MC);
      if (grProtEff_MC[iBin] && Results_MC->IsUsable() ) {
        Results_MC->Print();
        v_Results_MC.push_back(Results_MC);
      }
    }

    for( unsigned int i=0 ; i<fixedEfficiencies.size() ; ++i ) {
      std::vector<RejectionStats*>& results = v_Results_fixedEffs[i];
      RejectionStats* res = new RejectionStats(R1,R2);
      double effSel = fixedEfficiencies[i]/ElecSelectionEff;
      if(effSel>1.0) effSel = 1.0;
      GetRejection(hpElec_Dat[iBin], hpProt_Dat[iBin], effSel, res);
      if(res->IsUsable()){
        results.push_back(res);
      }
    }

    for( unsigned int i=0 ; i<fixedRejections.size() ; ++i ) {
      std::vector<RejectionStats*>& results = v_Results_fixedRejs[i];
      RejectionStats* res = new RejectionStats(R1,R2);
      GetEfficiency(hpElec_Dat[iBin], hpProt_Dat[iBin], fixedRejections[i], res);
      if(res->IsUsable())
        results.push_back(res);
    }
  }

  if(snapshot)
    ACsoft::Utilities::SaveCanvas(logLCanvas);

  TCanvas* fixedEffRejCanvas = new TCanvas(("fixed_rej_eff_canvas"+labelName).c_str(),("Fixed eff, rej"+labelTitle).c_str());
  fixedEffRejCanvas->Divide(2,1);

  fixedEffRejCanvas->cd(1);
  TH1D *hProtRejFixedEff = new TH1D(("hRejFixedEff"+labelName).c_str(),";R (GV);proton rejection",100,3.0,1000.0);
  hProtRejFixedEff->SetMinimum(1.0);
  hProtRejFixedEff->SetMaximum(1.0E6);
  hProtRejFixedEff->GetXaxis()->SetMoreLogLabels();
  hProtRejFixedEff->SetStats(false);
  hProtRejFixedEff->Draw();
  gPad->SetGrid();
  gPad->SetLogx();
  gPad->SetLogy();

  TGraph* rejectionFromPosfracPaper = CreateGraphForProtonRejectionFromPositronPaper2013();

  TLegend* fixedEffLegend = new TLegend(0.7,0.7,0.89,0.89);
  fixedEffLegend->SetFillColor(10);

  for( unsigned int i=0 ; i<fixedEfficiencies.size() ; ++i ) {

    RejectionGraphCollection graphs = MakeRejectionGraphs(v_Results_fixedEffs.at(i), kBlack);
    if(!graphs.grProtRej) continue;
    graphs.grProtRej->SetLineColor(i+1);
    graphs.grProtRej->SetMarkerColor(i+1);
    graphs.grProtRej->Draw("PL");
    std::stringstream entry;
    entry << "#epsilon = " << fixedEfficiencies[i];
    fixedEffLegend->AddEntry(graphs.grProtRej,entry.str().c_str(),"PL");
  }

  rejectionFromPosfracPaper->SetMarkerColor(4);
  rejectionFromPosfracPaper->SetLineColor(4);
  rejectionFromPosfracPaper->Draw("PL");
  fixedEffLegend->AddEntry(rejectionFromPosfracPaper,"TrdK from paper","PL");

  fixedEffLegend->Draw();

  fixedEffRejCanvas->cd(2);
  TH1D *hElecEffFixedRej = new TH1D(("hEffFixedRej"+labelName).c_str(),";R (GV);electron efficiency",100,3.0,1000.0);
  hElecEffFixedRej->SetMinimum(0.1);
  hElecEffFixedRej->SetMaximum(1.2);
  hElecEffFixedRej->GetXaxis()->SetMoreLogLabels();
  hElecEffFixedRej->SetStats(false);
  hElecEffFixedRej->Draw();
  gPad->SetGrid();
  gPad->SetLogx();

  TLegend* fixedRejLegend = new TLegend(0.7,0.7,0.89,0.89);
  fixedRejLegend->SetFillColor(10);

  for( unsigned int i=0 ; i<fixedRejections.size() ; ++i ) {

    RejectionGraphCollection graphs = MakeRejectionGraphs(v_Results_fixedRejs.at(i), kBlack);
    if(!graphs.grElecEff) continue;
    graphs.grElecEff->SetLineColor(i+1);
    graphs.grElecEff->SetMarkerColor(i+1);
    graphs.grElecEff->Draw("PL");
    std::stringstream entry;
    entry << "R = " << fixedRejections[i];
    fixedRejLegend->AddEntry(graphs.grElecEff,entry.str().c_str(),"PL");
  }
  fixedRejLegend->Draw();

  if( snapshot )
    ACsoft::Utilities::SaveCanvas(fixedEffRejCanvas);


  TCanvas* rejVsRigCanvas = new TCanvas(("rejection_vs_rig_canvas"+labelName).c_str(),("TRD rejection"+labelTitle).c_str());
  rejVsRigCanvas->Divide(3,2);

  RejectionGraphCollection mainGraphsData = MakeRejectionGraphs(v_Results_Dat, kBlue);
  RejectionGraphCollection mainGraphsMc   = MakeRejectionGraphs(v_Results_MC, kMagenta);

  // background histos
  TH1D *hElecEff = new TH1D(("hElecEff"+labelName).c_str(),";R (GV);electron efficiency (particle-ID)",100,3.0,1000.0);
  hElecEff->SetMinimum(0.5);
  hElecEff->SetMaximum(1.2);
  hElecEff->GetXaxis()->SetMoreLogLabels();
  hElecEff->SetStats(false);

  TH1D *hProtEff = new TH1D(("hProtEff"+labelName).c_str(),";R (GV);proton efficiency (particle-ID)",100,3.0,1000.0);
  hProtEff->SetMinimum(1.E-8);
  hProtEff->SetMaximum(1.0);
  hProtEff->GetXaxis()->SetMoreLogLabels();
  hProtEff->SetStats(false);

  TH1D *hProtRej = new TH1D(("hRej"+labelName).c_str(),";R (GV);p rejection",100,3.0,1000.0);
  hProtRej->SetMinimum(1.0);
  hProtRej->SetMaximum(1.0E6);
  hProtRej->GetXaxis()->SetMoreLogLabels();
  hProtRej->SetStats(false);

  rejVsRigCanvas->cd(1);
  gPad->SetGrid();
  gPad->SetLogx();
  hElecEff->Draw();
  mainGraphsData.grElecEff->Draw("P.same");
  if(mainGraphsMc.grElecEff){
    mainGraphsMc.grElecEff->Draw("L.same");
  }

  rejVsRigCanvas->cd(2);
  gPad->SetGrid();
  gPad->SetLogy();
  gPad->SetLogx();
  hProtEff->Draw();
  mainGraphsData.grProtEff->Draw("P.same");
  if(mainGraphsMc.grProtEff){
    mainGraphsMc.grProtEff->Draw("L.same");
  }

  rejVsRigCanvas->cd(3);
  gPad->SetGrid();
  gPad->SetLogy();
  gPad->SetLogx();
  hProtRej->Draw();
  mainGraphsData.grProtRej->Draw("P.same");
  if(mainGraphsMc.grProtRej){
    mainGraphsMc.grProtRej->Draw("L.same");
  }

  TH1D *hCutX = new TH1D(("hCutX"+labelName).c_str(),";R (GV);Cut",100,3.0,1000.0);
  hCutX->SetMinimum(0.0);
  hCutX->SetMaximum(1.0);
  hCutX->GetXaxis()->SetMoreLogLabels();
  hCutX->SetStats(false);

  rejVsRigCanvas->cd(4);
  gPad->SetLogx();
  gPad->SetGrid();
  hCutX->Draw();
  mainGraphsData.grCut->Draw("P.same");
  if(mainGraphsMc.grCut){
    mainGraphsMc.grCut->Draw("L.same");
  }

  // plot rej vs eff curves for selected rigidities
  rejVsRigCanvas->cd(5);
  std::vector<double> selectedRigidities;
  selectedRigidities.push_back(10.0);
  selectedRigidities.push_back(30.0);
  selectedRigidities.push_back(100.0);

  TH1D* selRigBgHisto = new TH1D( ("selRigBgHisto"+labelName).c_str(), "rejection vs efficiency for selected rigidities", 100, 0.,1.);
  selRigBgHisto->GetXaxis()->SetTitle("electron efficiency");
  selRigBgHisto->GetYaxis()->SetTitle("proton rejection");
  selRigBgHisto->SetMinimum(2.0);
  selRigBgHisto->SetMaximum(1.e6);
  selRigBgHisto->SetStats(0);
  selRigBgHisto->Draw();

  TLegend* selRigLegend = new TLegend(0.1,0.1,0.5,0.4);
  selRigLegend->SetLineColor(10);

  for( unsigned int p=0 ; p<selectedRigidities.size() ; ++p ){
    double rigidity = selectedRigidities.at(p);
    int iBin = h_Elec_Dat->GetXaxis()->FindFixBin(rigidity);
    double ElecSelectionEff = qualitySelectionEfficiency ? qualitySelectionEfficiency->GetBinContent(qualitySelectionEfficiency->FindFixBin(rigidity)) : 1.0;

    TGraphAsymmErrors* rejVsEffGraph = GetRejection(hpElec_Dat[iBin], hpProt_Dat[iBin], ElecTargetEff, 0); // ElecTargetEff is dummy value here
    ScaleEfficiencyAxis(rejVsEffGraph,ElecSelectionEff);
    rejVsEffGraph->SetLineColor(p+1);
    rejVsEffGraph->SetMarkerColor(p+1);
    rejVsEffGraph->Draw("PL");

    std::stringstream text;
    text << "p = " << rigidity << " GeV";
    selRigLegend->AddEntry(rejVsEffGraph,text.str().c_str(),"PL");
  }

  selRigLegend->Draw();
  gPad->SetLogy();
  gPad->SetGrid();

  rejVsRigCanvas->Update();
  if(snapshot)
    ACsoft::Utilities::SaveCanvas(rejVsRigCanvas);
}
