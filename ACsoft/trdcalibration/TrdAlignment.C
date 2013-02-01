
#include "TrdAlignment.hh"

#include <ConfigHandler.hh>
#include <TrdHit.hh>
#include <Event.h>
#include <AnalysisParticle.hh>
#include <StringTools.hh>

#include <SplineTrack.hh>
#include <TimeHistogramManager.hh>
#include <FileManager.hh>
#include "dumpstreamers.hh"

#include <assert.h>

#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include <TCanvas.h>
#include <TMath.h>
#include <TArrayD.h>
#include <TF1.h>

#include <iostream>
#include <vector>
#include <sstream>

#define DEBUG 0
#define INFO_OUT_TAG "TrdAlignment> "
#include <debugging.hh>

ACsoft::Calibration::TrdAlignment::TrdAlignment( Utilities::ConfigHandler* ) :
  fIsInitialized(false),
  fNumberOfTrdHitsWithinCut(0),
  fNumberOfTrdLayersWithHitWithinCut(0),
  fNumberOfHitsOnLayersWithHitWithinCut(0),
  fPathlengthHisto(0),
  fNumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength(0),
  fNumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut(0),
  fEventTimeHisto(0),
  fIssPositionHisto(0) {

  // read options from cfghandler here and save them in the class if needed
  // ...

  // TRD and histograms are not initialized here because we need the information from the TimeHistogramManager, only available after FileManager has been initialized
}


void ACsoft::Calibration::TrdAlignment::Initialize() {

  assert(!fIsInitialized);

  CreateAlignmentShiftHistos();

  // construct histograms
  fNumberOfTrdHitsWithinCut = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "NumberOfTrdHitsWithinCut", "number of TRD hits within distance cut", "number", 50, 0., 50. );
  fNumberOfTrdLayersWithHitWithinCut = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "NumberOfTrdLayersWithHitWithinCut",
                                                                                                      "number of TRD layers with at least one hit within distance cut", "number", 21, 0., 21. );
  fNumberOfHitsOnLayersWithHitWithinCut = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "NumberOfHitsOnLayersWithHitWithinCut",
                                                                                                         "number of TRD hits on layers participating in track", "number", 10, 0., 10. );
  fPathlengthHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "Pathlength", "3D pathlength of hits within cut distance", "pathlength / cm", 100, 0., 1. );
  fNumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength =
      Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "NumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength",
                                                                     "number of TRD layers with at least 1 hit with non-zero 3D pathlength (from track interpolation)", "number", 21, 0., 21. );
  fNumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut =
      Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "NumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut",
                                                                     "number of TRD layers with at least 1 hit with 3D pathlength above cut", "number", 21, 0., 21. );
  fEventTimeHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( "EventTimeHisto", "event times", "seconds since bin start",
                                                                                   120, 0., Utilities::TimeHistogramManager::instance()->GetBinWidthSeconds() );
  fIssPositionHisto = new TH2I( "IssPositionHisto", "ISS position;lon (deg);lat (deg)", 720, 0., 360., 720, -90., 90. );

  fIsInitialized = true;
}

void ACsoft::Calibration::TrdAlignment::CreateAlignmentShiftHistos() {

  assert(fAlignmentShiftHistos.empty());
  fAlignmentShiftHistos.reserve(AC::AMSGeometry::TRDModules);

  for( unsigned int moduleNumber = 0 ; moduleNumber < AC::AMSGeometry::TRDModules ; ++moduleNumber ) {

    std::stringstream name, title;
    name << "AlignmentShiftHisto_" << moduleNumber;
    title << "Alignment shifts for module " << moduleNumber;
    fAlignmentShiftHistos.push_back( Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( name.str(), title.str(), "#Delta R / cm", 100, -2.0, 2.0 ) );
  }
}

void ACsoft::Calibration::TrdAlignment::StoreAlignmentShift( unsigned int module, const TTimeStamp& time, Double_t shift ) {

  fAlignmentShiftHistos.at(module)->Fill(double(time),shift);
}


/**
  * \todo what about using the external alignment for tracker layer 1?
  */
void ACsoft::Calibration::TrdAlignment::Process( const Analysis::Particle& particle ) {

  if(!fIsInitialized)
    Initialize();
 
  const AC::EventHeader& evthdr = particle.RawEvent()->EventHeader();
  TTimeStamp evttime = evthdr.TimeStamp();

  static const float CutTrdTrkD = 3.0;

  const Analysis::SplineTrack* splineTrack = particle.GetSplineTrack();
  if( !splineTrack ){
    WARN_OUT << "No valid SplineTrack present!" << std::endl;
    return;
  }

  int numberOfTrdHitsWithinCut = 0;
  std::map<int,int> layerNumberToNhitsMap;
  std::map<int,int> layerNumberToNhitsWithNonzeroPathlength;
  std::map<int,int> layerNumberToNhitsWithPathlengthAboveCut;
  std::map<int,int> moduleNumberToNhitsMap;
  std::map<int,Double_t> moduleNumberToLastTrackResidual;

  for( unsigned int i=0 ; i<particle.TrdHits().size(); ++i ){

    const Analysis::TrdHit& hit = particle.TrdHits().at(i);

    // check distance to track
//    Double_t trackResid = hit.DistanceToTrack();
    Double_t deltaXY = hit.Orientation() == AC::XZMeasurement ? hit.R() - splineTrack->InterpolateToZ(hit.Z()).X() :
                                                                hit.R() - splineTrack->InterpolateToZ(hit.Z()).Y();
    Double_t trackResid = deltaXY;
    DEBUG_OUT << hit << std::endl;
    DEBUG_OUT << "track pos: " << splineTrack->InterpolateToZ(hit.Z())
              << " deltaXY: " << deltaXY
              << std::endl;

    if( fabs(trackResid) < CutTrdTrkD ){

      ++moduleNumberToNhitsMap[hit.Module()];
      moduleNumberToLastTrackResidual[hit.Module()] = trackResid;

      Float_t pathlength3d = hit.Pathlength3D();

      if( pathlength3d > 0.0 )
        layerNumberToNhitsWithNonzeroPathlength[hit.Layer()]++;
      if( pathlength3d > ::AC::Settings::TrdTubeDefaultMinPathLength )
        layerNumberToNhitsWithPathlengthAboveCut[hit.Layer()]++;

      fPathlengthHisto->Fill(evttime,pathlength3d);

      ++layerNumberToNhitsMap[hit.Layer()];
      ++numberOfTrdHitsWithinCut;
    }
  }

  // fill track residuals only for modules with exactly one hit
  for( std::map<int,int>::iterator it = moduleNumberToNhitsMap.begin(); it != moduleNumberToNhitsMap.end() ; ++it ){

    int moduleNumber = it->first;
    int nHitsInModule = it->second;

    DEBUG_OUT << "module " << moduleNumber << " hits " << nHitsInModule << std::endl;

    if( nHitsInModule == 1 )
      StoreAlignmentShift(moduleNumber, evttime, moduleNumberToLastTrackResidual[moduleNumber]);
  }

  // fill histograms
  int numberOfTrdLayersWithHitWithinCut = 0;
  for( std::map<int,int>::iterator it = layerNumberToNhitsMap.begin(); it != layerNumberToNhitsMap.end() ; ++it ){
    int nHitsOnLayer = it->second;
    if(nHitsOnLayer > 0){
      ++numberOfTrdLayersWithHitWithinCut;
      fNumberOfHitsOnLayersWithHitWithinCut->Fill(evttime,Double_t(nHitsOnLayer));
    }
  }

  // bookkeeping for histograms
  int numberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength = 0;
  for( std::map<int,int>::iterator it = layerNumberToNhitsWithNonzeroPathlength.begin(); it != layerNumberToNhitsWithNonzeroPathlength.end() ; ++it ){
    int nh = it->second;
    if( nh > 0 ) ++numberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength;
  }
  int numberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut = 0;
  for( std::map<int,int>::iterator it = layerNumberToNhitsWithPathlengthAboveCut.begin(); it != layerNumberToNhitsWithPathlengthAboveCut.end() ; ++it ){
    int nh = it->second;
    if( nh > 0 ) ++numberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut;
  }

  // fill more histograms
  fNumberOfTrdHitsWithinCut->Fill(evttime,Double_t(numberOfTrdHitsWithinCut));
  fNumberOfTrdLayersWithHitWithinCut->Fill(evttime,Double_t(numberOfTrdLayersWithHitWithinCut));
  int binEvtTime = fEventTimeHisto->GetXaxis()->FindBin(Double_t(evttime));
  fEventTimeHisto->Fill(Double_t(evttime),Double_t(evttime)-fEventTimeHisto->GetXaxis()->GetBinLowEdge(binEvtTime));
  fIssPositionHisto->Fill(evthdr.ISSLongitude(),evthdr.ISSLatitude());
  fNumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength->Fill(Double_t(evttime),numberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength);
  fNumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut->Fill(Double_t(evttime),numberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut);

}



void ACsoft::Calibration::TrdAlignment::WriteResultsToCurrentFile() {

  if (!fIsInitialized)
    return;
  for( unsigned int i=0 ; i<fAlignmentShiftHistos.size() ; ++i )
    fAlignmentShiftHistos.at(i)->Write();

  fNumberOfTrdHitsWithinCut->Write();
  fNumberOfTrdLayersWithHitWithinCut->Write();
  fNumberOfHitsOnLayersWithHitWithinCut->Write();
  fPathlengthHisto->Write();
  fNumberOfTrdLayersWithAtLeastOneHitWithNonzeroPathlength->Write();
  fNumberOfTrdLayersWithAtLeastOneHitWithPathlengthAboveCut->Write();
  fEventTimeHisto->Write();
  fIssPositionHisto->Write();

}



static inline void SaveCanvas(TCanvas* canvas, const std::string& prefix, const std::string& name, const std::string& postfix) {

  assert(canvas);
  std::stringstream fileName;
  fileName << prefix << name << postfix;
  canvas->Update();
  canvas->SaveAs((fileName.str() + std::string(".root")).c_str());
  canvas->SaveAs((fileName.str() + std::string(".png")).c_str());
}

/** Fit alignment shift time-histograms.
 *
 * Algorithm:
 *  - Accumulate statistics from adjacent time bins.
 *  - Fit a top-hat function with smoothed edges to the y-projection ( = distribution of alignment shifts), using meaningful start values.
 *  - Apply cuts on fit results (hardcoded for the moment, see code), to identify poor fits and exclude those from the final graph used for alignment.
 *  - Also calculate simple mean for comparison.
 *  - Save top-hat center ( = alignment shift) to graph, use time weighted by number of events on x-axis.
 *
 * \param inputfile Name of file containing the input histogram.
 * \param histogram Name of histogram to be analysed.
 * \param requiredNumberOfEntries Number of events required for fit and calculation of mean. Time bins are merged together until this number is exceeded. The value is ignored and a fit is done regardless
 * if a given maximum number of time bins is reached before.
 * \param interactive If \c true, draw canvases with results at the end, and do not write output file.
 * \param snapshot If \c true, draw canvases with results at the end, write an output file and exit.
 * \param resultfile Name of output file.
 * \param testbin If different from zero, fit only the projection starting at that bin and show the histogram and fit function for debugging purposes.
 *
 * \return 0 if no errors, \>0 in case of errors
 */
int ACsoft::Calibration::TrdAlignment::AnalyzeAlignmentShiftHistograms(
  std::string inputfile,
  std::string histogram,
  int requiredNumberOfEntries,
  bool interactive,
  bool snapshot,
  std::string resultfile,
  int testbin ) {

  int maxNbins = 25; // if this number of time bins is exceeded, force a fit even if the number of entries in the histogram is below requiredNumberOfEntries
  int absoluteMinimumNumberOfEntries = 100; // ... but require at least some entries...

  // smooth-edged top-hat function, see maple/alignment_shift_function.mw
  // the same functional form can be obtained from a convolution of a top-hat function (-> tube geometry) with a Gaussian (-> resolution function), see maple/convolve_tophat_gaussian.mw
  TF1* fitFunc = new TF1( "fitFunc", "[0]+0.5*([1]-[0])*(TMath::Erf((x-([2]-1/2*[3]))/[4])-TMath::Erf((x-([2]+1/2*[3]))/[4]))", -10., 10. );
  fitFunc->SetParName(0,"y_{0}");
  fitFunc->SetParName(1,"y_{1}");
  fitFunc->SetParName(2,"center");
  fitFunc->SetParName(3,"width");
  fitFunc->SetParName(4,"sharpness");

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

  // create result graphs
  TGraph* simpleMeanGraph = new TGraph;
  simpleMeanGraph->SetName((std::string("simpleMeanGraph_")+(moduleId)).c_str());
  TGraph* histoMeanGraph = new TGraph;
  histoMeanGraph->SetName((std::string("histoMeanGraph_")+(moduleId)).c_str());
  TGraph* fitMeanGraph = new TGraph;
  fitMeanGraph->SetName((std::string("fitMeanGraph_")+(moduleId)).c_str());
  TGraph* fitMeanGraphGoodFit = new TGraph;
  fitMeanGraphGoodFit->SetName((std::string("fitMeanGraphGoodFit_")+(moduleId)).c_str());
  TGraph* fitMeanGraphBadFit = new TGraph;
  fitMeanGraphBadFit->SetName((std::string("fitMeanGraphBadFit_")+(moduleId)).c_str());
  TGraph* fitMeanErrorGraph = new TGraph;
  fitMeanErrorGraph->SetName((std::string("fitMeanErrorGraph_")+(moduleId)).c_str());
  TGraph* fitMeanErrorGraphGoodFit = new TGraph;
  fitMeanErrorGraphGoodFit->SetName((std::string("fitMeanErrorGraphGoodFit_")+(moduleId)).c_str());
  TGraph* fitBaselineGraph = new TGraph;
  fitBaselineGraph->SetName((std::string("fitBaselineGraph_")+(moduleId)).c_str());
  TGraph* fitPlateauGraph = new TGraph;
  fitPlateauGraph->SetName((std::string("fitPlateauGraph_")+(moduleId)).c_str());
  TGraph* fitWidthGraph = new TGraph;
  fitWidthGraph->SetName((std::string("fitWidthGraph_")+(moduleId)).c_str());
  TGraph* fitSharpnessGraph = new TGraph;
  fitSharpnessGraph->SetName((std::string("fitSharpnessGraph_")+(moduleId)).c_str());
  TGraph* fitChisquareGraph = new TGraph;
  fitChisquareGraph->SetName((std::string("fitChisquareGraph_")+(moduleId)).c_str());
  TGraph* fitNbinsGraph = new TGraph;
  fitNbinsGraph->SetName((std::string("fitNbinsGraph_")+(moduleId)).c_str());
  TGraph* fitNentriesGraph = new TGraph;
  fitNentriesGraph->SetName((std::string("fitNentriesGraph_")+(moduleId)).c_str());
  TGraph* replacementMeanGraph = new TGraph;
  replacementMeanGraph->SetName((std::string("replacementMeanGraph_")+(moduleId)).c_str());

  int nbinsy = histo->GetNbinsY();

  TH1D* fitHisto = new TH1D( "fitHisto", "projection", nbinsy, histo->GetYaxis()->GetXmin(), histo->GetYaxis()->GetXmax() );

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

  double posMin = histo->GetYaxis()->GetXmax(); // for optimal drawing range
  double posMax = histo->GetYaxis()->GetXmin(); // for optimal drawing range

  bool test = false;
  int currentStartBin = 0; // remember first x-bin for projection
  int startbin = 1;
  if(testbin) startbin=testbin;

  int fitPosErrorCtr = 0;
  int fitWidthTooLargeCtr = 0;
  int fitWidthTooSmallCtr = 0;
  int fitNbinsCtr = 0;
  int fitChi2Ctr = 0;
  int fitEdgeSharpnessHighCtr = 0;
  int fitEdgeSharpnessLowCtr = 0;

  // loop over x (time)
  for( int i=startbin ; i<=histo->GetNbinsX() ; ++i ){

    bool printOut = ( i==1 || !(i%500) );
    if( printOut )
      INFO_OUT << "Bin " << i << std::endl;

    double x = histo->GetXaxis()->GetBinCenter(i);
    if(!currentStartBin) currentStartBin = i;

    double xentries = 0.0;    // for mean time
    double baselineSum = 0.0; // for y0 start value
    int baselineEntries = 0;  // for y0 start value
    for( int j=1 ; j<=histo->GetNbinsY() ; ++j ){

      double val = histo->GetBinContent(i,j);
      yweights[j-1] += val;
      entries += val;
      xentries += val;

      // get baseline from histogram edges
      if( j<6 || j>(nbinsy-5) ){
        baselineSum += val;
        baselineEntries++;
      }
    }

    xpos.push_back(x);            // for mean time
    xweights.push_back(xentries); // for mean time

    int fitNbins = xpos.size();

    if( entries >= requiredNumberOfEntries || ( entries >= absoluteMinimumNumberOfEntries && fitNbins >= maxNbins ) ) {
      // we have enough entries for a meaningful mean and a fit!

      double ymean = TMath::Mean(nbinsy,yvals.GetArray(),yweights.GetArray());
      double xmean = TMath::Mean(xpos.begin(),xpos.end(),xweights.begin());     // weighted average of time
      simpleMeanGraph->SetPoint(pointCtr,xmean,ymean);

      DEBUG_OUT << "simple mean: " << pointCtr << " x " << xmean << " y " << ymean << std::endl;

      // prepare for fit
      for( int k=0 ; k<nbinsy ; ++k )
        fitHisto->SetBinContent(k+1,yweights[k]);

      histoMeanGraph->SetPoint(pointCtr,xmean,fitHisto->GetMean());
      // find start parameters
      double baseline = baselineSum / baselineEntries;
      double maximum  = fitHisto->GetBinContent(fitHisto->GetMaximumBin());
      double plateau  = maximum - TMath::Sqrt(maximum);
      double center   = ymean;
      double width    = 0.6;
      double sharpness = 0.01;

      fitFunc->SetParameters(baseline,plateau,center,width,sharpness);
      fitFunc->SetParError(0,0.);
      fitFunc->SetParError(1,0.);
      fitFunc->SetParError(2,0.);
      fitFunc->SetParError(3,0.);
      fitFunc->SetParError(4,0.);
      DEBUG_OUT << "Par errors: " << fitFunc->GetParError(0) << " " << fitFunc->GetParError(1) << " " << fitFunc->GetParError(2) << " " << fitFunc->GetParError(3) << " " << fitFunc->GetParError(4) << std::endl;

      std::string fitOpt = "LN"; // LNI";
      if( !testbin ) fitOpt += "Q";
      fitHisto->Fit(fitFunc,fitOpt.c_str());
      // retrieve fit results
      double fitPos = fitFunc->GetParameter(2);
      double fitWidth = fitFunc->GetParameter(3);
      double fitChi2 = fitFunc->GetChisquare();
      double fitPosError = fitFunc->GetParError(2);
      double fitEdgeSharpness = fitFunc->GetParameter(4);
      fitMeanErrorGraph->SetPoint(pointCtr,xmean,fitPosError);
      fitBaselineGraph->SetPoint(pointCtr,xmean,fitFunc->GetParameter(0));
      fitPlateauGraph->SetPoint(pointCtr,xmean,fitFunc->GetParameter(1));
      fitWidthGraph->SetPoint(pointCtr,xmean,fitWidth);
      fitSharpnessGraph->SetPoint(pointCtr,xmean,fitEdgeSharpness);
      fitChisquareGraph->SetPoint(pointCtr,xmean,fitChi2);
      fitMeanGraph->SetPoint(pointCtr,xmean,fitPos);

      DEBUG_OUT << "fit pos:     " << pointCtr << " x " << xmean << " y " << fitPos << std::endl;

      fitNbinsGraph->SetPoint(pointCtr,xmean,fitNbins);
      fitNentriesGraph->SetPoint(pointCtr,xmean,entries);

      ++pointCtr;

      if(fitPos>posMax) posMax = fitPos;
      if(fitPos<posMin) posMin = fitPos;

      // cut on good fit, fill two separate graphs for supposedly good and bad fits, respectively
      bool goodFit = true;

      if( fitPosError < 0.0005 ){
        ++fitPosErrorCtr;
        goodFit = false;
      }
      if( fitWidth > 0.62 ){
        ++fitWidthTooLargeCtr;
        goodFit = false;
      }
      if( fitWidth < 0.4 ){
        ++fitWidthTooSmallCtr;
        goodFit = false;
      }
      if( fitNbins > maxNbins ){
        ++fitNbinsCtr;
        goodFit = false;
      }
      if( fitChi2/fitFunc->GetNDF() > 20. ){
        ++fitChi2Ctr;
        goodFit = false;
      }
      if( fitEdgeSharpness > 0.2 ){
        ++fitEdgeSharpnessHighCtr;
        goodFit = false;
      }
      if( fitEdgeSharpness < 0.04 ){
        ++fitEdgeSharpnessLowCtr;
        goodFit = false;
      }


      if( goodFit ){
        fitMeanGraphGoodFit->SetPoint(pointCtrGoodFit,xmean,fitPos);
        fitMeanErrorGraphGoodFit->SetPoint(pointCtrGoodFit,xmean,fitPosError);
        ++pointCtrGoodFit;


//        if( fitPos < 0.004 ){ // TEST
//          WARN_OUT << "x=" << xmean << " : " << TTimeStamp(time_t(xmean),xmean-time_t(xmean)).AsString() << " fitPos: " << fitPos << "+-" << fitPosError
//                    << " currentStartBin: " << currentStartBin << " entries: " << entries << std::endl;
//          WARN_OUT << "Start values: " << baseline << " " << plateau << " " << center << " " << width << " " << sharpness << std::endl;
//          WARN_OUT << "Results:      " << fitFunc->GetParameter(0) << " " << fitFunc->GetParameter(1) << " " << fitPos << " " << fitWidth << " " << fitEdgeSharpness << std::endl;
//        }

      }
      else {
        fitMeanGraphBadFit->SetPoint(pointCtrBadFit,xmean,fitPos);
        ++pointCtrBadFit;
        replacementMeanGraph->SetPoint(pointCtrBadFit,xmean,ymean);
      }

      if(testbin>0) {
        INFO_OUT << "Breaking at bin " << i << " (current start bin: " << currentStartBin << ") entries: " << entries << std::endl;
        INFO_OUT << "x=" << xmean << " : " << TTimeStamp(time_t(xmean),xmean-time_t(xmean)).AsString() << std::endl;
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

  INFO_OUT << "Cut on fit position uncertainty: " << fitPosErrorCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on large tube width:         " << fitWidthTooLargeCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on small tube width:         " << fitWidthTooSmallCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on nbins:                    " << fitNbinsCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on Chi2:                     " << fitChi2Ctr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on high edge sharpness:      " << fitEdgeSharpnessHighCtr << " / " << pointCtr << " failed." << std::endl;
  INFO_OUT << "Cut on low edge sharpness:       " << fitEdgeSharpnessLowCtr << " / " << pointCtr << " failed." << std::endl;



  if(test){
    // draw a single histogram and corresponding fit function for debugging purposes
    TCanvas* t = new TCanvas( "t", "test", 1400, 700 );
    t->Divide(2,1);
    t->cd(1);
    histo->ProjectionY("_py",currentStartBin,currentStartBin+xpos.size()-1,"d");
    gPad->SetLogy();
    t->cd(2);
    fitHisto->GetXaxis()->SetTitle(histo->GetYaxis()->GetTitle());
    fitHisto->SetTitle(histo->GetTitle());
    fitHisto->SetStats(0);
    fitHisto->Draw();
    fitFunc->SetNpx(1000);
    fitFunc->Draw("L SAME");

    std::stringstream stream;
    stream << moduleId << "_timebin_" << testbin;
    if (snapshot)
      SaveCanvas(t, "fit_module_", stream.str(), "_results");
  }

  TCanvas* ac = new TCanvas( "auxCanvas", "aux canvas", 1400, 1000 );
  ac->Divide(4,2);
  ac->cd(1);
  fitNbinsGraph->Draw("AP");
  fitNbinsGraph->SetTitle("number of time bins for fit");
  fitNbinsGraph->GetXaxis()->SetTimeDisplay(1);
  fitNbinsGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  ac->cd(2);
  fitNentriesGraph->Draw("AP");
  fitNentriesGraph->SetTitle("number of entries for fit");
  fitNentriesGraph->GetXaxis()->SetTimeDisplay(1);
  fitNentriesGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  TCanvas* c = new TCanvas( "canvas", "canvas", 1400, 1000 );
  c->Divide(4,2);
  c->cd(1);
  simpleMeanGraph->Draw( "AP" );
  simpleMeanGraph->SetTitle("simple mean");
  simpleMeanGraph->GetXaxis()->SetTimeDisplay(1);
  simpleMeanGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  simpleMeanGraph->GetYaxis()->SetTitle("#Delta R / cm");
  c->cd(2);
  fitMeanGraph->Draw( "AP" );
  fitMeanGraph->SetTitle((std::string("position from fit for module ")+(moduleId)).c_str());
  fitMeanGraph->GetXaxis()->SetTimeDisplay(1);
  fitMeanGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  fitMeanGraph->GetYaxis()->SetTitle("#Delta R / cm");
  c->cd(3);
  fitMeanErrorGraph->Draw( "AP" );
  fitMeanErrorGraph->SetTitle((std::string("position uncertainty from fit for module ")+(moduleId)).c_str());
  fitMeanErrorGraph->GetXaxis()->SetTimeDisplay(1);
  fitMeanErrorGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  fitMeanErrorGraph->GetYaxis()->SetTitle("#sigma(#Delta R) / cm");
  fitMeanErrorGraph->GetHistogram()->SetMaximum(0.005);
  c->cd(4);
  fitBaselineGraph->Draw( "AP" );
  fitBaselineGraph->SetTitle("baseline from fit");
  fitBaselineGraph->GetXaxis()->SetTimeDisplay(1);
  fitBaselineGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  c->cd(5);
  fitPlateauGraph->Draw( "AP" );
  fitPlateauGraph->SetTitle("plateau from fit");
  fitPlateauGraph->GetXaxis()->SetTimeDisplay(1);
  fitPlateauGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  c->cd(6);
  fitWidthGraph->Draw( "AP" );
  fitWidthGraph->SetTitle("tube width from fit");
  fitWidthGraph->GetXaxis()->SetTimeDisplay(1);
  fitWidthGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  fitWidthGraph->GetYaxis()->SetTitle("d / cm");
  c->cd(7);
  fitSharpnessGraph->Draw( "AP" );
  fitSharpnessGraph->SetTitle("edge sharpness from fit");
  fitSharpnessGraph->GetXaxis()->SetTimeDisplay(1);
  fitSharpnessGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
  fitSharpnessGraph->GetYaxis()->SetTitle("sharpness / cm");
  c->cd(8);
  fitChisquareGraph->Draw("AP");
  fitChisquareGraph->SetTitle("#chi^{2} of ML fit function");
  fitChisquareGraph->GetXaxis()->SetTimeDisplay(1);
  fitChisquareGraph->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");

  if( !testbin ){
    TCanvas* fc = new TCanvas( "finalCanvas", "final", 1400, 1000 );
    fc->cd();
    if(fitMeanGraphGoodFit->GetN()>0){
      fitMeanGraphGoodFit->Draw( "AP" );
      fitMeanGraphGoodFit->SetMarkerStyle(7);
      fitMeanGraphGoodFit->SetMarkerColor(kBlue);
      fitMeanGraphGoodFit->SetTitle((std::string("position from good fits for module ")+(moduleId)).c_str());
      fitMeanGraphGoodFit->GetXaxis()->SetTimeDisplay(1);
      fitMeanGraphGoodFit->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
      fitMeanGraphGoodFit->GetYaxis()->SetTitle("#Delta R / cm");
      fitMeanGraphGoodFit->GetHistogram()->SetMinimum(posMin-0.01);
      fitMeanGraphGoodFit->GetHistogram()->SetMaximum(posMax+0.01);
      if(fitMeanGraphBadFit->GetN()>0)
        fitMeanGraphBadFit->Draw( "P" );
      //      if(replacementMeanGraph->GetN()>0)
      //        replacementMeanGraph->Draw( "P" );
    }
    else{
      fitMeanGraphBadFit->Draw( "AP" );
      fitMeanGraphBadFit->GetXaxis()->SetTimeDisplay(1);
      fitMeanGraphBadFit->GetXaxis()->SetTimeFormat("%y-%m-%d%F1970-01-01 00:00:00");
      fitMeanGraphBadFit->GetHistogram()->SetMinimum(posMin-0.01);
      fitMeanGraphBadFit->GetHistogram()->SetMaximum(posMax+0.01);
      if(replacementMeanGraph->GetN()>0)
        replacementMeanGraph->Draw( "P" );
    }
    fitMeanGraphBadFit->SetMarkerStyle(5);
    fitMeanGraphBadFit->SetMarkerColor(kRed);
    replacementMeanGraph->SetMarkerStyle(2);
    replacementMeanGraph->SetMarkerColor(kGreen);
  
    if (snapshot && !test)
      SaveCanvas(fc, "fit_module_", moduleId, "_shifts");
  }

  if( !interactive && !snapshot ) {

    TFile* outfile = new TFile( resultfile.c_str(), "RECREATE" );
    if( !outfile->IsOpen() ){
      WARN_OUT << "ERROR opening output file " << resultfile << std::endl;
      return 3;
    }

    fitMeanGraphBadFit->SetTitle("position from fit (bad fits only)");
    fitMeanErrorGraphGoodFit->SetTitle("position uncertainty from fit (good fits only)");

    INFO_OUT << "Writing result graphs to file " << outfile->GetName() << std::endl;
    outfile->cd();

    simpleMeanGraph->Write();
    fitMeanGraph->Write();
    fitMeanGraphGoodFit->Write();
    fitMeanErrorGraphGoodFit->Write();
    fitMeanGraphBadFit->Write();
    fitMeanErrorGraph->Write();
    fitBaselineGraph->Write();
    fitPlateauGraph->Write();
    fitWidthGraph->Write();
    fitSharpnessGraph->Write();
    fitChisquareGraph->Write();
    fitNbinsGraph->Write();
    fitNentriesGraph->Write();

    outfile->Close();
    return 0;
  }

  if (snapshot && test) {
    SaveCanvas(ac, "fit_module_", moduleId, "_parameters");
    SaveCanvas(c, "fit_module_", moduleId, "_results");
  }
  return 0;
}
