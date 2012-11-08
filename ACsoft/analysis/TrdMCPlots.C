#include "TrdMCPlots.hh"

#include <TList.h>
#include <TH1.h>
#include <THStack.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TLegend.h>

#include "Event.h"
#include "AnalysisParticle.hh"

#include <ConfigHandler.hh>
#include <Selector.hh>
#include <FileManager.hh>
#include <SplineTrack.hh>
#include <TrdHit.hh>

#define DEBUG 0
#define INFO_OUT_TAG "TrdMCPlots> "
#include <debugging.hh>

/** Default constructor
 *  Calls CreateHistograms()
 */
Analysis::TrdMCPlots::TrdMCPlots(std::string name,
                                 Utilities::ConfigHandler* cfghandler,
                                 std::string resultDirectory,
                                 std::string resultFileSuffix ) :
  fName(name),
  fResultDirectory(resultDirectory),
  fResultFileSuffix(resultFileSuffix)
{

  // read options from cfghandler here and save them in the class
  fOutputFilePrefix = "TrdMCPlots";
  if(cfghandler)
    cfghandler->GetValue( "TrdMCPlots", "OutputFilePrefix", fOutputFilePrefix,
                          "Prefix for output file. Directory and suffix are set via global option." );

  fHistogramList = new TList();
  CreateHistograms();
  
  fMCevents = 0;
  fDataevents = 0;
}

Analysis::TrdMCPlots::~TrdMCPlots() {
  delete fHNumberTRDhitsData;
  delete fHNumberTRDhitsMC;
  delete fHNumberTRDhitsStack;
  delete fHNumberTRDhitsResiduals;
  delete fHNumberTRDhitsOnTrackData;
  delete fHNumberTRDhitsOnTrackMC;
  delete fHNumberTRDhitsOnTrackResiduals;
  delete fHDepositedEnergyData;
  delete fHDepositedEnergyMC;
  delete fHDepositedEnergyStack;
  delete fHDepositedEnergyResiduals;
  delete fHDepositedEnergyOnTrackData;
  delete fHDepositedEnergyOnTrackMC;
  delete fHDepositedEnergyOnTrackResiduals;
}

/** Create new histograms here.
  *
  */
void Analysis::TrdMCPlots::CreateHistograms() {
  TH1::AddDirectory(kFALSE);
  fHNumberTRDhitsData = new TH1F(Form("fHNumberTRDhitsData_%s", fName.c_str()), Form("number of TRD hits from data files: %s;N_{hits};arbitrary units", fName.c_str()), 151, -0.5, 150.5);
  fHNumberTRDhitsMC = new TH1F(Form("fHNumberTRDhitsMC_%s", fName.c_str()), Form("number of TRD hits from MC files: %s;N_{hits};arbitrary units", fName.c_str()), 151, -0.5, 150.5);
  fHNumberTRDhitsResiduals = new TH1F(Form("fHNumberTRDhitsResiduals_%s", fName.c_str()), Form("residuals of TRD hits: %s;N_{hits};residuals", fName.c_str()), 151, -0.5, 150.5);
  fHNumberTRDhitsOnTrackData = new TH1F(Form("fHNumberTRDhitsOnTrackData_%s", fName.c_str()), Form("number of TRD hits on track from data files: %s;N_{hits};arbitrary units", fName.c_str()), 151, -0.5, 150.5);
  fHNumberTRDhitsOnTrackMC = new TH1F(Form("fHNumberTRDhitsOnTrackMC_%s", fName.c_str()), Form("number of TRD hits on track from MC files: %s;N_{hits};arbitrary units", fName.c_str()), 151, -0.5, 150.5);
  fHNumberTRDhitsOnTrackResiduals = new TH1F(Form("fHNumberTRDhitsOnTrackResiduals_%s", fName.c_str()), Form("residuals of TRD hits on track: %s;N_{hits};residuals", fName.c_str()), 151, -0.5, 150.5);
  fHDepositedEnergyData = new TH1F(Form("fHDepositedEnergyData_%s", fName.c_str()), Form("deposited energy in the TRD hits from data files: %s;dE/dX [ADC];arbitrary units", fName.c_str()), 401, -0.5, 4000.5);
  fHDepositedEnergyMC = new TH1F(Form("fHDepositedEnergyMC_%s", fName.c_str()), Form("deposited energy in the TRD hits from MC files: %s;dE [ADC];arbitrary units", fName.c_str()), 401, -0.5, 4000.5);
  fHDepositedEnergyResiduals = new TH1F(Form("fHDepositedEnergyResiduals_%s", fName.c_str()), Form("residuals of TRD deposited energy: %s;dE [ADC];residuals", fName.c_str()), 401, -0.5, 4000.5);
  fHDepositedEnergyOnTrackData = new TH1F(Form("fHDepositedEnergyOnTrackData_%s", fName.c_str()), Form("deposited energy in the TRD hits on track from data files: %s;dE/dX [ADC];arbitrary units", fName.c_str()), 401, -0.5, 4000.5);
  fHDepositedEnergyOnTrackMC = new TH1F(Form("fHDepositedEnergyOnTrackMC_%s", fName.c_str()), Form("deposited energy in the TRD hits on track from MC files: %s;dE [ADC];arbitrary units", fName.c_str()), 401, -0.5, 4000.5);
  fHDepositedEnergyOnTrackResiduals = new TH1F(Form("fHDepositedEnergyOnTrackResiduals_%s", fName.c_str()), Form("residuals of TRD deposited energy on track: %s;dE [ADC];residuals", fName.c_str()), 401, -0.5, 4000.5);
  
  fHNumberTRDhitsStack = new THStack(Form("fHNumberTRDhitsStack_%s", fName.c_str()), fName.c_str());
  fHDepositedEnergyStack = new THStack(Form("fHDepositedEnergyStack_%s", fName.c_str()), fName.c_str());
  
  // stack MC and data histograms
  fHNumberTRDhitsStack -> Add(fHNumberTRDhitsMC);
  fHNumberTRDhitsStack -> Add(fHNumberTRDhitsData);
  fHNumberTRDhitsStack -> Add(fHNumberTRDhitsOnTrackMC);
  fHNumberTRDhitsStack -> Add(fHNumberTRDhitsOnTrackData);
  
  fHDepositedEnergyStack -> Add(fHDepositedEnergyMC);
  fHDepositedEnergyStack -> Add(fHDepositedEnergyData);
  fHDepositedEnergyStack -> Add(fHDepositedEnergyOnTrackMC);
  fHDepositedEnergyStack -> Add(fHDepositedEnergyOnTrackData);
  
  // store pointers in the list of histograms, to simplify operations on all histograms of the class

  fHistogramList -> Add(fHNumberTRDhitsStack);
  fHistogramList -> Add(fHNumberTRDhitsResiduals);
  fHistogramList -> Add(fHNumberTRDhitsOnTrackResiduals);
  fHistogramList -> Add(fHDepositedEnergyStack);
  fHistogramList -> Add(fHDepositedEnergyResiduals);
  fHistogramList -> Add(fHDepositedEnergyOnTrackResiduals);
  
  return;
}


/** Fill histogram from the current event.
  *
  * \param event pointer to the current event
  */

void Analysis::TrdMCPlots::FillHistosFrom(const Analysis::Particle& particle) {

  const Analysis::SplineTrack* TrkTrackSpline = particle.GetSplineTrack();
  Analysis::TrdHit *RawHit;
  double distance = 0;
  unsigned short fnHitsOnTrackMC = 0;
  unsigned short fnHitsOnTrackData = 0;
  
  const AC::Event* rawEvent = particle.RawEvent();

  if(rawEvent->MC().EventGenerators().size() != 0)
  {
    ++fMCevents;
    fHNumberTRDhitsMC -> Fill(rawEvent->TRD().RawHits().size());
    for(unsigned int i = 0; i < rawEvent->TRD().RawHits().size(); i++)
    {
      RawHit = new TrdHit(rawEvent->TRD().RawHits().at(i));
      distance = RawHit -> DistanceToTrack(*TrkTrackSpline);
      if(fabs(distance < 1.5))
      {
        ++fnHitsOnTrackMC;
        fHDepositedEnergyOnTrackMC -> Fill(rawEvent->TRD().RawHits().at(i).DepositedEnergy());
      }
      fHDepositedEnergyMC -> Fill(rawEvent->TRD().RawHits().at(i).DepositedEnergy());
    }
    fHNumberTRDhitsOnTrackMC -> Fill(fnHitsOnTrackMC);
  }
  else
  {
    ++fDataevents;
    fHNumberTRDhitsData -> Fill(rawEvent->TRD().RawHits().size());
    for(unsigned int i = 0; i < rawEvent->TRD().RawHits().size(); i++)
    {
      RawHit = new TrdHit(rawEvent->TRD().RawHits().at(i));
      distance = RawHit -> DistanceToTrack(*TrkTrackSpline);
      if(fabs(distance < 1.5))
      {
        ++fnHitsOnTrackData;
        fHDepositedEnergyOnTrackData -> Fill(rawEvent->TRD().RawHits().at(i).DepositedEnergy());
      }
      fHDepositedEnergyData -> Fill(rawEvent->TRD().RawHits().at(i).DepositedEnergy());
    }
    fHNumberTRDhitsOnTrackData -> Fill(fnHitsOnTrackData);
  }
  return;
}

void Analysis::TrdMCPlots::NormaliseHistograms()
{
  if(fHNumberTRDhitsData -> GetEntries() > 0)
    fHNumberTRDhitsData -> Scale(1./fHNumberTRDhitsData -> GetEntries());
  if(fHNumberTRDhitsMC -> GetEntries() > 0)
    fHNumberTRDhitsMC -> Scale(1./fHNumberTRDhitsMC -> GetEntries());
  if(fHNumberTRDhitsOnTrackData -> GetEntries() > 0)
    fHNumberTRDhitsOnTrackData -> Scale(1./fHNumberTRDhitsOnTrackData -> GetEntries());
  if(fHNumberTRDhitsOnTrackMC -> GetEntries() > 0)
    fHNumberTRDhitsOnTrackMC -> Scale(1./fHNumberTRDhitsOnTrackMC -> GetEntries());
  if(fHDepositedEnergyData -> GetEntries() > 0)
    fHDepositedEnergyData -> Scale(1./fHDepositedEnergyData -> GetEntries());
  if(fHDepositedEnergyMC -> GetEntries() > 0)
    fHDepositedEnergyMC -> Scale(1./fHDepositedEnergyMC -> GetEntries());
  if(fHDepositedEnergyOnTrackData -> GetEntries() > 0)
    fHDepositedEnergyOnTrackData -> Scale(1./fHDepositedEnergyOnTrackData -> GetEntries());
  if(fHDepositedEnergyOnTrackMC -> GetEntries() > 0)
    fHDepositedEnergyOnTrackMC -> Scale(1./fHDepositedEnergyOnTrackMC -> GetEntries());
}

void Analysis::TrdMCPlots::CalculateResiduals()
{
  // Need normalised histograms. Check first that they are filled.
  NormaliseHistograms();

  // Calculate Residuals if MC and Data histograms are filled.
  if(fHNumberTRDhitsMC -> GetEntries() > 0 && fHNumberTRDhitsData -> GetEntries() > 0)
  {
    fHNumberTRDhitsResiduals -> Add(fHNumberTRDhitsData);
    fHNumberTRDhitsResiduals -> Add(fHNumberTRDhitsMC, -1);
    fHNumberTRDhitsResiduals -> Divide(fHNumberTRDhitsData);
  }
  if(fHNumberTRDhitsOnTrackMC -> GetEntries() > 0 && fHNumberTRDhitsOnTrackData -> GetEntries() > 0)
  {
    fHNumberTRDhitsOnTrackResiduals -> Add(fHNumberTRDhitsOnTrackData);
    fHNumberTRDhitsOnTrackResiduals -> Add(fHNumberTRDhitsOnTrackMC, -1);
    fHNumberTRDhitsOnTrackResiduals -> Divide(fHNumberTRDhitsOnTrackData);
  }
  if(fHDepositedEnergyMC -> GetEntries() > 0 && fHDepositedEnergyData -> GetEntries() > 0)
  {
    fHDepositedEnergyResiduals -> Add(fHDepositedEnergyData);
    fHDepositedEnergyResiduals -> Add(fHDepositedEnergyMC, -1);
    fHDepositedEnergyResiduals -> Divide(fHDepositedEnergyData);
  }
  if(fHDepositedEnergyOnTrackMC -> GetEntries() > 0 && fHDepositedEnergyOnTrackData -> GetEntries() > 0)
  {
    fHDepositedEnergyOnTrackResiduals -> Add(fHDepositedEnergyOnTrackMC);
    fHDepositedEnergyOnTrackResiduals -> Add(fHDepositedEnergyOnTrackData, -1);
    fHDepositedEnergyOnTrackResiduals -> Divide(fHDepositedEnergyOnTrackMC);
  }
}

/** Create canvases and draw histograms.
  *
  */
void Analysis::TrdMCPlots::DrawHistos() {

  INFO_OUT << "Drawing histograms..." << std::endl;

//  TCanvas **canvasTRDMCPlots = new TCanvas*[fHistogramList->GetSize()];
//  for( int i=0 ; i<fHistogramList->GetSize() ; ++i ) {
//    canvasTRDMCPlots[i] = new TCanvas(Form("%s", fHistogramList->At(i)->GetName()));
//    fHistogramList->At(i)->Draw();
//  }
  gStyle -> SetOptStat(0);
  gStyle -> SetStatFont(132);
  gStyle -> SetTitleFont(132, "XYZ");
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,1) 
  gStyle -> SetLegendFont(132);
#endif
  gStyle -> SetLabelFont(132, "XYZ");
  
  // If residuals are calculated, split the canvas into two and plot first the distributions and then the residuals
  TCanvas *canvasTRDhits = new TCanvas(Form("canvasTRDhits_%s", fName.c_str()), "number of TRD hits", 750, 1000);
  if(fHNumberTRDhitsResiduals -> GetEntries() > 0)
  {
    canvasTRDhits -> Divide(1, 2);
    canvasTRDhits -> cd(2);
    gPad -> SetPad(0., 0.0, 1., 0.33);
    fHNumberTRDhitsResiduals -> Draw("err");
    canvasTRDhits -> cd(1);
    gPad -> SetPad(0., 0.34, 1., 1.);
  }
  fHNumberTRDhitsMC -> SetLineColor(2);
  fHNumberTRDhitsOnTrackMC -> SetLineColor(2);
  fHNumberTRDhitsMC -> SetLineStyle(2);
  fHNumberTRDhitsData -> SetLineStyle(2);
  fHNumberTRDhitsStack -> Draw("nostack");
  fHNumberTRDhitsStack -> GetXaxis() -> SetTitle(fHNumberTRDhitsMC -> GetXaxis() -> GetTitle());
  fHNumberTRDhitsStack -> GetYaxis() -> SetTitle(fHNumberTRDhitsMC -> GetYaxis() -> GetTitle());
  TLegend* leg = new TLegend(0.65, 0.65, 0.85, 0.85);
  leg -> SetBorderSize(0);
  leg -> SetFillStyle(0);
  leg -> SetTextFont(132);
  leg -> SetTextSize(0.04);
  if(fDataevents > 0) leg -> AddEntry(fHNumberTRDhitsOnTrackData, "Data", "l");
  if(fMCevents > 0) leg -> AddEntry(fHNumberTRDhitsOnTrackMC, "MC", "l");
  leg -> AddEntry(fHNumberTRDhitsData, "Raw", "l");
  leg -> AddEntry(fHNumberTRDhitsOnTrackData, "On track", "l");
  leg -> Draw();
    
  TCanvas *canvasDepositedEnergy = new TCanvas(Form("canvasDepositedEnergy_%s", fName.c_str()), "Deposited Energy in TRD", 750, 1000);
  if(fHDepositedEnergyResiduals -> GetEntries() > 0)
  {
    canvasDepositedEnergy -> Divide(1, 2);
    canvasDepositedEnergy -> cd(2);
    gPad -> SetPad(0., 0.0, 1., 0.33);
    fHDepositedEnergyResiduals -> Draw("err");
    canvasDepositedEnergy -> cd(1);
    gPad -> SetPad(0., 0.34, 1., 1.);
  }
  gPad -> SetLogy();
  fHDepositedEnergyMC -> SetLineColor(2);
  fHDepositedEnergyOnTrackMC -> SetLineColor(2);
  fHDepositedEnergyMC -> SetLineStyle(2);
  fHDepositedEnergyData -> SetLineStyle(2);
  fHDepositedEnergyStack -> Draw("nostack");
  fHDepositedEnergyStack -> GetXaxis() -> SetTitle(fHDepositedEnergyMC -> GetXaxis() -> GetTitle());
  fHDepositedEnergyStack -> GetYaxis() -> SetTitle(fHDepositedEnergyMC -> GetYaxis() -> GetTitle());
  leg = new TLegend(0.65, 0.65, 0.85, 0.85);
  leg -> SetBorderSize(0);
  leg -> SetFillStyle(0);
  leg -> SetTextFont(132);
  leg -> SetTextSize(0.04);
  if(fDataevents > 0) leg -> AddEntry(fHDepositedEnergyOnTrackData, "Data", "l");
  if(fMCevents > 0) leg -> AddEntry(fHDepositedEnergyOnTrackMC, "MC", "l");
  leg -> AddEntry(fHDepositedEnergyData, "Raw", "l");
  leg -> AddEntry(fHDepositedEnergyOnTrackData, "On track", "l");
  leg -> Draw();
}

/** Writes all histograms to the currently opened ROOT file.
  *
  */
void Analysis::TrdMCPlots::WriteHistos() {

  INFO_OUT << "Write Histograms to current root file..." << std::endl;

  for( int i=0 ; i<fHistogramList->GetSize() ; ++i ) {
    fHistogramList->At(i)->Write();
  }
}

/** Creates a new ROOT file and writes all histograms to the file.
  *
  * The filename is set via the configuration file.
  */
void Analysis::TrdMCPlots::WriteHistosToNewFile() {

  std::string outputFileName = Analysis::FileManager::MakeStandardRootFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);

  TFile* f = new TFile( outputFileName.c_str(), "RECREATE");
  INFO_OUT << "Set current root file to " << f->GetName() << std::endl;
  if( !f->IsOpen() ){
    WARN_OUT << "ERROR opening output file \"" << outputFileName << "\"!" << std::endl;
    throw std::runtime_error("ERROR opening output file");
  }

  f->cd();

  WriteHistos();

  f->Close();
}
