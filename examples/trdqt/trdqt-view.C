#include <TCanvas.h>
#include <TFile.h>
#include <TH1F.h>
#include <TLegend.h>

// First run, eg. ./trdqt_example root://castorpublic.cern.ch///castor/cern.ch/ams/Data/AMS02/2011B/ISS.B584/pass3/1323073267.00000001.root.
// Then use: root -l trdqt_view.C+, to examine the results of the demo application.
int trdqt_view() {

  TFile* file = new TFile("TrdQtExample.root", "READ");
  // file->ls();

  TCanvas* canvas1 = new TCanvas("canvas1", "TrdQt - Number of active layers example", 1024, 768);
  canvas1->SetLogy();

  TH1F* nTrdActiveLayers_all = (TH1F*)file->Get("nTrdActiveLayers_all");
  TH1F* nTrdActiveLayers_in_acceptance = (TH1F*)file->Get("nTrdActiveLayers_in_acceptance");
  TH1F* nTrdActiveLayers_passes_preselection_cuts = (TH1F*)file->Get("nTrdActiveLayers_passes_preselection_cuts");
  TH1F* nTrdActiveLayers_passes_all_cuts = (TH1F*)file->Get("nTrdActiveLayers_passes_all_cuts");

  nTrdActiveLayers_all->SetLineColor(kBlack);
  nTrdActiveLayers_all->Draw();
  nTrdActiveLayers_in_acceptance->SetLineColor(kBlue);
  nTrdActiveLayers_in_acceptance->Draw("same");
  nTrdActiveLayers_passes_preselection_cuts->SetLineColor(kGreen);
  nTrdActiveLayers_passes_preselection_cuts->Draw("same");
  nTrdActiveLayers_passes_all_cuts->SetLineColor(kRed);
  nTrdActiveLayers_passes_all_cuts->Draw("same");

  TLegend* legend = new TLegend(0.3470588,0.5822581,0.677451,0.8112903);
  legend->AddEntry(nTrdActiveLayers_all, "All events");
  legend->AddEntry(nTrdActiveLayers_in_acceptance, "In TRD acceptance");
  legend->AddEntry(nTrdActiveLayers_passes_preselection_cuts, "and passes preselection cuts");
  legend->AddEntry(nTrdActiveLayers_passes_all_cuts, "and passes candidate matching");
  legend->Draw();

  TCanvas* canvas2 = new TCanvas("canvas2", "TrdQt - e/p Likelihood example", 1024, 768);
  canvas2->SetLogy();

  TH1F* logLikelihoodRatioElectronProton = (TH1F*)file->Get("logLikelihoodRatioElectronProton");
  logLikelihoodRatioElectronProton->Draw();

  return 1;
}
