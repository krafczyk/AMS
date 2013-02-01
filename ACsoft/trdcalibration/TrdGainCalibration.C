
#include "TrdGainCalibration.hh"

#include <gain_functions.hh>
#include <ConfigHandler.hh>
#include <TrdHit.hh>
#include <SplineTrack.hh>
#include <TimeHistogramManager.hh>
#include <FileManager.hh>
#include <AnalysisParticle.hh>
#include <StringTools.hh>

#include "Event.h"

#include <assert.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TArrayD.h>
#include <TF1.h>
#include <TStyle.h>

#include <iostream>
#include <vector>
#include <sstream>

#define DEBUG 0
#define INFO_OUT_TAG "TrdGainCalibration> "
#include <debugging.hh>

ACsoft::Calibration::TrdGainCalibration::TrdGainCalibration( Utilities::ConfigHandler* ) :
  fIsInitialized(false) {

  // read options from cfghandler here and save them in the class if needed
  // ...

}


void ACsoft::Calibration::TrdGainCalibration::Initialize() {

  assert(!fIsInitialized);

  // construct histograms
  // (...)

  CreateDeDxHistos();

  fIsInitialized = true;
}


void ACsoft::Calibration::TrdGainCalibration::CreateDeDxHistos() {

  assert(fDeDxHistos.empty());
  fDeDxHistos.reserve(AC::AMSGeometry::TRDModules);

  for( unsigned int moduleNumber = 0 ; moduleNumber < AC::AMSGeometry::TRDModules ; ++moduleNumber ) {

    std::stringstream name, title;
    name << "DeDxHisto_" << moduleNumber;
    title << "dE/dx for module " << moduleNumber;
    fDeDxHistos.push_back( Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( name.str(), title.str(), "dE/dx (ADC/cm)", 200, 0., 400. ) );
  }
}


void ACsoft::Calibration::TrdGainCalibration::StoreDeDx( unsigned int moduleNumber, const TTimeStamp& time, Double_t dedx ) {

  fDeDxHistos.at(moduleNumber)->Fill(double(time),dedx);
}


/**
  * \todo fill interesting histograms: number of TRD hits on track, pathlengths, bethe-bloch corr factors, dE/dx vs pathlength ...
  */
void ACsoft::Calibration::TrdGainCalibration::Process( const Analysis::Particle& particle ) {

  if(!fIsInitialized)
    Initialize();

  static const float CutTrdTrkD = 3.0;

  const AC::Event* evt = particle.RawEvent();
  TTimeStamp evttime = evt->EventHeader().TimeStamp();

  const Analysis::SplineTrack* splineTrack = particle.GetSplineTrack();
  if( !splineTrack ){
    WARN_OUT << "No valid SplineTrack present!" << std::endl;
    return;
  }


  // find particle momentum
  const AC::TrackerTrackFit* trackFit = particle.MainTrackerTrackFit();
  assert(trackFit);
  Float_t p_evt = trackFit->Rigidity();

  for( unsigned int i=0 ; i<particle.TrdHits().size(); ++i ){

    const Analysis::TrdHit& hit = particle.TrdHits().at(i);

    // calculate dE/dx based on tube amplitude and pathlength
    // GetDeDx function will return dE/dx based on raw amplitude if no gain correction has been applied in hit factory
    float dEdX = hit.GetDeDx();

    // calculate dE/dx correction to correct for rigidity dependence
    Double_t betheBlochValue = fdedx(p_evt);
    static const Double_t p_ref = 10.0;
    static const Double_t betheBlochValue_ref = fdedx(p_ref);
    float dEdX_corr = dEdX * betheBlochValue_ref / betheBlochValue;

    // make sure we only use protons
    // -> done by selection in main event loop

    // check distance to track
//    Double_t trackResid = hit.DistanceToTrack();
    Double_t deltaXY = hit.Orientation() == AC::XZMeasurement ? hit.R() - splineTrack->InterpolateToZ(hit.Z()).X() :
                                                                hit.R() - splineTrack->InterpolateToZ(hit.Z()).Y();
    Double_t trackResid = deltaXY;

    // cut on minimum pathlength, only fill histogram for first half of event sample
    if( fabs(trackResid) < CutTrdTrkD && hit.Pathlength3D() > ::AC::Settings::TrdTubeDefaultMinPathLength ){
      StoreDeDx(hit.Module(), evttime, dEdX_corr);
    }
  }
}



void ACsoft::Calibration::TrdGainCalibration::WriteResultsToCurrentFile() {

  for( unsigned int i=0 ; i<fDeDxHistos.size() ; ++i )
    fDeDxHistos.at(i)->Write();
  }



static inline void SaveCanvas(TCanvas* canvas, const std::string& prefix, const std::string& name, const std::string& postfix) {

  assert(canvas);
  std::stringstream fileName;
  fileName << prefix << name << postfix;
  canvas->Update();
  canvas->SaveAs((fileName.str() + std::string(".root")).c_str());
  canvas->SaveAs((fileName.str() + std::string(".png")).c_str());
}


/** Fit gain parameter time-histograms.
 *
 * Algorithm:
 *  - Accumulate statistics from adjacent time bins.
 *  - Fit a Landau distribution to the dE/dx histogram, using meaningful start values.
 *  - Apply cuts on fit results (hardcoded for the moment, see code), to identify poor fits and exclude those from the final graph used for gain calibration.
 *  - Save gain to graph, use time weighted by number of events on x-axis.
 *
 * \param inputfile Name of file containing the input histogram.
 * \param histogram Name of histogram to be analysed.
 * \param requiredNumberOfEntries Number of events required for fit and calculation of Mpv. Time bins are merged together until this number is exceeded.
 * \param interactive If \c true, draw canvases with results at the end, and do not write output file.
 * \param snapshot If \c true, draw canvases with results at the end, write an output file and exit.
 * \param resultfile Name of output file.
 * \param testbin If different from zero, fit only the projection starting at that bin and show the histogram and fit function for debugging purposes.
 *
 * \return 0 if no errors, \>0 in case of errors
 */
int ACsoft::Calibration::TrdGainCalibration::AnalyzeGainHistograms(
  std::string inputfile,
  std::string histogram,
  int requiredNumberOfEntries,
  bool interactive,
  bool snapshot,
  std::string resultfile,
  int testbin ) {

  // simple Landau function a la ROOT
  // cut off lowest energy depositions
  TF1* fitFunc = new TF1( "fitFunc", "landau", 20., 1000. );

  TFile* file = new TFile( inputfile.c_str(), "READ" );
  if(!file->IsOpen()) return 1;
  TH2* histo = (TH2*)file->Get(histogram.c_str());
  if(!histo) return 2;

  std::string moduleId = "0";
  // try to get module number from histogram name
  std::vector<std::string> tokens = split(histogram,std::string("_"));
  if( tokens.size() >= 2 ){
    moduleId = tokens.back();
  }

  INFO_OUT << "Using module identifier \"" << moduleId << "\"" << std::endl;

  std::string mpvtitle("proton dE/dx MPV from fit");
  if( moduleId != "0" ) mpvtitle += " for module " + moduleId;

  TGraph* fitConstantGraph = new TGraph;
  fitConstantGraph->SetName((std::string("fitConstantGraph_")+(moduleId)).c_str());
  TGraph* fitMpvGraph = new TGraph;
  fitMpvGraph->SetName((std::string("fitMpvGraph_")+(moduleId)).c_str());
  TGraph* fitSigmaGraph = new TGraph;
  fitSigmaGraph->SetName((std::string("fitSigmaGraph_")+(moduleId)).c_str());
  TGraph* fitMpvGraphGoodFit = new TGraph;
  fitMpvGraphGoodFit->SetName((std::string("fitMpvGraphGoodFit_")+(moduleId)).c_str());
  TGraph* fitMpvGraphBadFit = new TGraph;
  fitMpvGraphBadFit->SetName((std::string("fitMpvGraphBadFit_")+(moduleId)).c_str());
  TGraph* fitMpvErrorGraph = new TGraph;
  fitMpvErrorGraph->SetName((std::string("fitMpvErrorGraph_")+(moduleId)).c_str());
  TGraph* fitChisquareGraph = new TGraph;
  fitChisquareGraph->SetName((std::string("fitChisquareGraph_")+(moduleId)).c_str());
  TGraph* fitNbinsGraph = new TGraph;
  fitNbinsGraph->SetName((std::string("fitNbinsGraph_")+(moduleId)).c_str());
  TGraph* fitNentriesGraph = new TGraph;
  fitNentriesGraph->SetName((std::string("fitNentriesGraph_")+(moduleId)).c_str());

  int nbinsy = histo->GetNbinsY();

  TH1D* fitHisto = new TH1D( "fitHisto", "projection", nbinsy, histo->GetYaxis()->GetXmin(), histo->GetYaxis()->GetXmax() );
  double fitHistoBinWidth = fitHisto->GetXaxis()->GetBinWidth(1);

  INFO_OUT << "Read histo with " << histo->GetNbinsX() << " x-bins." << std::endl;

  TArrayD yvals(nbinsy); // store y bin centers for fast access
  for( int j=1 ; j<=histo->GetNbinsY() ; ++j ){
    double y   = histo->GetYaxis()->GetBinCenter(j);
    yvals[j-1] = y;
  }

  TArrayD yweights(nbinsy); // weights = histogram bin contents
  yweights.Reset();

  int pointCtr = 0;
  int pointCtrGoodFit = 0;
  int pointCtrBadFit = 0;
  double entries = 0.;

  std::vector<double> xpos;
  std::vector<double> xweights;

  double mpvMin = histo->GetYaxis()->GetXmax(); // for optimal drawing range
  double mpvMax = histo->GetYaxis()->GetXmin(); // for optimal drawing range

  bool test = false;
  int currentStartBin = 0; // remember first x-bin for projection
  int startbin = 1;
  if(testbin) startbin=testbin;

//  int cutNbinsCtr = 0;
  int cutChi2Ctr = 0;
  int cutUncertaintyCtr = 0;
  int cutSigmaCtr = 0;

  // loop over x (time)
  for( int i=startbin ; i<=histo->GetNbinsX() ; ++i ){

    bool printOut = ( i==1 || !(i%500) );
    if( printOut )
      INFO_OUT << "Bin " << i << std::endl;

    double x = histo->GetXaxis()->GetBinCenter(i);
    if(!currentStartBin) currentStartBin = i;

    double xentries = 0.0;    // for mean time

    for( int j=1 ; j<=histo->GetNbinsY() ; ++j ){

      double val = histo->GetBinContent(i,j);
      yweights[j-1] += val;
      entries += val;
      xentries += val;
    }

    xpos.push_back(x);            // for mean time
    xweights.push_back(xentries); // for mean time

    if( entries >= requiredNumberOfEntries ) {
      DEBUG_OUT << entries << " >= " << requiredNumberOfEntries << std::endl;
      // we have enough entries for a meaningful mean and a fit!
      double ymean = TMath::Mean(nbinsy,yvals.GetArray(),yweights.GetArray());
      double xmean = TMath::Mean(xpos.begin(),xpos.end(),xweights.begin());     // weighted average of time

      // prepare for fit
      for( int k=0 ; k<nbinsy ; ++k )
        fitHisto->SetBinContent(k+1,yweights[k]);

      // find start parameters
      double mpv = 0.5 * ymean;
      double sigma = 0.5*fitHisto->GetRMS();
      double constant = ( sigma > 0 ? fitHistoBinWidth*yweights.GetSum()/sigma : 1.0 );

      DEBUG_OUT << "Start values for fit starting at bin " << currentStartBin << ", histogram " << histo->GetName() << ": "
                << constant << " " << mpv << " " << sigma << std::endl;

      fitFunc->SetParameters(constant,mpv,sigma);
      if((interactive || snapshot) && testbin)
        fitHisto->Fit(fitFunc,"BL0");
      else
        fitHisto->Fit(fitFunc,"BQLN");
      // retrieve fit results
      double fitConstant = fitFunc->GetParameter(0);
      double fitMpv = fitFunc->GetParameter(1);
      double fitMpvUncertainty = fitFunc->GetParError(1);
      double fitSigma = fitFunc->GetParameter(2);
      double fitChi2 = fitFunc->GetChisquare();
      int fitNbins = xpos.size();

      fitConstantGraph->SetPoint(pointCtr,xmean,fitConstant);
      fitMpvGraph->SetPoint(pointCtr,xmean,fitMpv);
      fitSigmaGraph->SetPoint(pointCtr,xmean,fitSigma);
      fitMpvErrorGraph->SetPoint(pointCtr,xmean,fitMpvUncertainty);
      fitChisquareGraph->SetPoint(pointCtr,xmean,fitChi2);
      fitNbinsGraph->SetPoint(pointCtr,xmean,fitNbins);
      fitNentriesGraph->SetPoint(pointCtr,xmean,entries);

      ++pointCtr;

      if(fitMpv>mpvMax) mpvMax = fitMpv;
      if(fitMpv<mpvMin) mpvMin = fitMpv;

//      if( fitSigma > 65. && fitSigma < 75. )
//        WARN_OUT << "Bin " << currentStartBin << " fit sigma: " << fitSigma << std::endl;

      // cut on good fit, fill two separate graphs for supposedly good and bad fits, respectively
      bool goodFit = true;

//      if( fitNbins > 20 ){
//        ++cutNbinsCtr;
//        goodFit = false;
//      }
      if( fitChi2 > 600. ){
        ++cutChi2Ctr;
        goodFit = false;
      }
      if( fitMpvUncertainty > 0.02*fitMpv ){
        ++cutUncertaintyCtr;
        goodFit = false;
      }
      if( fitSigma > 0.5*fitMpv ){
        ++cutSigmaCtr;
        goodFit = false;
      }

      if( goodFit ){
        fitMpvGraphGoodFit->SetPoint(pointCtrGoodFit,xmean,fitMpv);
        ++pointCtrGoodFit;
      }
      else {
        fitMpvGraphBadFit->SetPoint(pointCtrBadFit,xmean,fitMpv);
        ++pointCtrBadFit;
      }

      if(testbin>0) {
        INFO_OUT << "Breaking at bin " << i << " (current start bin: " << currentStartBin << ")" << std::endl;
        test = true;
        break;
      }

      // reset everything for next fit
      entries = 0.0;
      yweights.Reset();

      xpos.clear();
      xweights.clear();

      fitHisto->Reset();
      currentStartBin = 0;
    }
  }

//  INFO_OUT << "Cut on Nbins:           " << cutNbinsCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on Chi2:            " << cutChi2Ctr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on MPV uncertainty: " << cutUncertaintyCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on sigma:           " << cutSigmaCtr << " / " << pointCtr << " failed." << std::endl;

  if(test){
    gStyle->SetOptFit(1111);
    // draw a single histogram and corresponding fit function for debugging purposes
    TCanvas* t = new TCanvas( "t", "test", 1400, 700 );
    t->Divide(2,1);
    t->cd(1);
    histo->ProjectionY("_py",currentStartBin,currentStartBin+xpos.size()-1,"d");
    t->cd(2);
    fitHisto->GetXaxis()->SetTitle(histo->GetYaxis()->GetTitle());
    fitHisto->SetTitle(histo->GetTitle());
    fitHisto->Draw();
    fitFunc->SetNpx(1000);
    fitFunc->Draw("L SAME");

    std::stringstream stream;
    stream << moduleId << "_timebin_" << testbin;
    if (snapshot)
      SaveCanvas(t, "fit_module_", stream.str(), "_results");
  }

  TCanvas* c = new TCanvas( "gainFitCanvas", "gain fits", 1400, 1000 );
  c->Divide(4,2);
  c->cd(1);
  fitMpvGraph->Draw( "AP" );
  fitMpvGraph->SetTitle("Landau MPV from fit");
  fitMpvGraph->GetXaxis()->SetTimeDisplay(1);
  fitMpvGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(2);
  fitMpvErrorGraph->Draw( "AP" );
  fitMpvErrorGraph->SetTitle("Uncertainty on Landau MPV from fit");
  fitMpvErrorGraph->GetXaxis()->SetTimeDisplay(1);
  fitMpvErrorGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(3);
  fitConstantGraph->Draw( "AP" );
  fitConstantGraph->SetTitle("Landau constant from fit");
  fitConstantGraph->GetXaxis()->SetTimeDisplay(1);
  fitConstantGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(4);
  fitSigmaGraph->Draw( "AP" );
  fitSigmaGraph->SetTitle("Landau sigma from fit");
  fitSigmaGraph->GetXaxis()->SetTimeDisplay(1);
  fitSigmaGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(5);
  fitChisquareGraph->Draw( "AP" );
  fitChisquareGraph->SetTitle("#chi^{2} of ML fit function");
  fitChisquareGraph->GetXaxis()->SetTimeDisplay(1);
  fitChisquareGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(6);
  fitNbinsGraph->Draw( "AP ");
  fitNbinsGraph->SetTitle("number of time bins for fit");
  fitNbinsGraph->GetXaxis()->SetTimeDisplay(1);
  fitNbinsGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  c->cd(7);
  fitNentriesGraph->Draw("AP");
  fitNentriesGraph->SetTitle("number of entries for fit");
  fitNentriesGraph->GetXaxis()->SetTimeDisplay(1);
  fitNentriesGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  if (snapshot && !testbin)
    SaveCanvas(c, "fit_module_", moduleId, "_parameters");

  if( !testbin ){
    TCanvas* fc = new TCanvas( "gainFinalCanvas", "final gain parameters", 1400, 1000 );
    fc->cd();
    if(fitMpvGraphGoodFit->GetN()>0){
      fitMpvGraphGoodFit->Draw( "AP" );
      fitMpvGraphGoodFit->SetMarkerStyle(7);
      fitMpvGraphGoodFit->SetMarkerColor(kBlue);
      fitMpvGraphGoodFit->SetTitle(mpvtitle.c_str());
      fitMpvGraphGoodFit->GetXaxis()->SetTimeDisplay(1);
      fitMpvGraphGoodFit->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
      fitMpvGraphGoodFit->GetYaxis()->SetTitle("ADC / cm");
      fitMpvGraphGoodFit->GetHistogram()->SetMinimum(mpvMin-10.0);
      fitMpvGraphGoodFit->GetHistogram()->SetMaximum(mpvMax+10.0);
      if(fitMpvGraphBadFit->GetN()>0)
        fitMpvGraphBadFit->Draw( "P" );
    }
    else{
      fitMpvGraphBadFit->Draw( "AP" );
      fitMpvGraphBadFit->SetTitle(mpvtitle.c_str());
      fitMpvGraphBadFit->GetXaxis()->SetTimeDisplay(1);
      fitMpvGraphBadFit->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
      fitMpvGraphBadFit->GetHistogram()->SetMinimum(mpvMin-10.0);
      fitMpvGraphBadFit->GetHistogram()->SetMaximum(mpvMax+10.0);
    }
    fitMpvGraphBadFit->SetMarkerStyle(5);
    fitMpvGraphBadFit->SetMarkerColor(kRed);
    if (snapshot)
      SaveCanvas(fc, "fit_module_", moduleId, "_results");
  }

  if( !interactive && !snapshot ){

    TFile* outfile = new TFile( resultfile.c_str(), "RECREATE" );
    if( !outfile->IsOpen() ){
      WARN_OUT << "ERROR opening output file " << resultfile << std::endl;
      return 3;
    }

    INFO_OUT << "Writing result graphs to file " << outfile->GetName() << std::endl;
    outfile->cd();

    fitConstantGraph->Write();
    fitMpvGraph->Write();
    fitSigmaGraph->Write();
    fitMpvGraphGoodFit->Write();
    fitMpvGraphBadFit->Write();
    fitMpvErrorGraph->Write();
    fitChisquareGraph->Write();
    fitNbinsGraph->Write();
    fitNentriesGraph->Write();

    outfile->Close();
  }



  return 0;
}
