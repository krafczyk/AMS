#include "TrdPerformancePlots.hh"
#include<TrdHit.hh>
#include<math.h>

#include<TList.h>
#include<TF1.h>
#include<TH1.h>
#include<TH2.h>
#include<TGraphErrors.h>
#include<TProfile.h>
#include<TFile.h>
#include<TCanvas.h>
#include<TPad.h>
#include<TStyle.h>
#include<TColor.h>
#include<TVector3.h>
#include<TMath.h>
#include<TLine.h>
#include<TLegend.h>
#include<TLegendEntry.h>

#include "Event.h"
#include "AnalysisParticle.hh"

#include<ConfigHandler.hh>
#include <FileManager.hh>
#include <SplineTrack.hh>
#include<TRDVTrack.h>
#include<TRDHTrack.h>
#include<TRDRawHit.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdPerformancePlots> "
#include<debugging.hh>

/** Default constructor
 *  Calls CreateHistograms()
 */
Analysis::TrdPerformancePlots::TrdPerformancePlots( Utilities::ConfigHandler* cfghandler,
                                                    std::string resultDirectory,
                                                    std::string resultFileSuffix ) :
  fResultDirectory(resultDirectory),
  fResultFileSuffix(resultFileSuffix)
{

  // read options from cfghandler here and save them in the class
  fOutputFilePrefix = "trdperformanceplots";
  if(cfghandler)
    cfghandler->GetValue( "TrdPerformancePlots", "OutputFilePrefix", fOutputFilePrefix,
                          "Prefix for output file. Directory and suffix are set via global option." );

  fHistogramList= new TList();
  CreateHistograms();
}

/** Default destructor
 *  Calls DeleteHistos()
 */
Analysis::TrdPerformancePlots::~TrdPerformancePlots() {

  DeleteHistos();
  delete fHistogramList;
}

/** Delete Histograms.
  * Called by default destructor
  */
void Analysis::TrdPerformancePlots::DeleteHistos(){

  delete fNumberOfTrdHitsHisto;
  delete fNumberOfTrdHitsHistoafterPreselection;
  delete fNumberOfTrdHitsOnHTrack;
  delete fNumberOfTrdHitsOnVTrack;
  delete fNumberOfTrdHitsOnTTrack;
  delete fNumberOfTrdHitsAroundHTrack;
  delete fNumberOfTrdHitsAroundVTrack;
  delete fNumberOfTrdHitsAroundTTrack;
  delete fNumberOfTrdHTracksHistoafterPreselection;
  delete fNumberOfTrdVTracksHistoafterPreselection;
  delete fEventsvsLatLon;
  delete fEventsvsLatLonafterPreselection;
  delete fNumberOfTrdHTracksHisto;
  delete fNumberOfTrdVTracksHisto;
  delete fHPreCalibrationPhsHisto;
  delete fHPreCalibrationMpvPerStraw;
  delete fHPreCalibrationMpvHisto;
  delete fHPreCalibrationWidthHisto;
  delete fVPreCalibrationPhsHisto;
  delete fVPreCalibrationMpvPerStraw;
  delete fVPreCalibrationMpvHisto;
  delete fVPreCalibrationWidthHisto;
  delete fTPreCalibrationPhsHisto;
  delete fTPreCalibrationMpvPerStraw;
  delete fTPreCalibrationMpvHisto;
  delete fTPreCalibrationWidthHisto;
  delete fHOccupancyOnTrack;
  delete fHOccupancyOffTrack;
  delete fVOccupancyOnTrack;
  delete fVOccupancyOffTrack;
  delete fTOccupancyOnTrack;
  delete fTOccupancyOffTrack;
  delete fHTrackingQuality;
  delete fVTrackingQuality;
  delete fHTrackingQualityAngle;
  delete fVTrackingQualityAngle;
  delete fHCalibFactors;

  delete fTest;
  delete fTest2;

  return;
}


/** Create new histograms here.
  * Called by default constructor
  * To create new Histograms: Add Histogram to header file, Create it here and add it to fHistogramList, add delete operator in DeleteHistos()
  */
void Analysis::TrdPerformancePlots::CreateHistograms() {

  fNumberOfTrdHitsHisto = new TH1I( "NumberOfTrdHitsHisto", "number of TRD hits;N_{hits};entries", 151, -0.5, 150.5 );
  fNumberOfTrdHitsHistoafterPreselection = new TH1I( "NumberOfTrdHitsHistoafterPreselection", "number of TRD hits;N_{hits};entries", 151, -0.5, 150.5 );

  fNumberOfTrdHitsOnHTrack = new TH1I( "NumberOfTrdHitsOnHTrack", "number of TRD hits on Track;N_{hits};entries", 35, -0.5, 34.5 );
  fNumberOfTrdHitsOnVTrack = new TH1I( "NumberOfTrdHitsOnVTrack", "number of TRD hits on Track;N_{hits};entries", 35, -0.5, 34.5 );
  fNumberOfTrdHitsOnTTrack = new TH1I( "NumberOfTrdHitsOnTTrack", "number of TRD hits on Track;N_{hits};entries", 35, -0.5, 34.5 );

  fNumberOfTrdHitsAroundHTrack = new TH1I( "NumberOfTrdHitsAroundHTrack", "number of TRD hits around Track;N_{hits};entries", 23, -0.5, 34.5 );
  fNumberOfTrdHitsAroundVTrack = new TH1I( "NumberOfTrdHitsAroundVTrack", "number of TRD hits around Track;N_{hits};entries", 23, -0.5, 34.5 );
  fNumberOfTrdHitsAroundTTrack = new TH1I( "NumberOfTrdHitsAroundTTrack", "number of TRD hits around Track;N_{hits};entries", 23, -0.5, 34.5 );

  fNumberOfTrdHTracksHisto = new TH1I("NumberOfTrdHTracksHisto","Number of TRD Tracks;N_{tracks};entries",6,-0.5,5.5);
  fNumberOfTrdVTracksHisto = new TH1I("NumberOfTrdVTracksHisto","Number of TRD Tracks;N_{tracks};entries",6,-0.5,5.5);
  fNumberOfTrdHTracksHistoafterPreselection = new TH1I("NumberOfTrdHTracksHistoafterPreselection","Number of TRD Tracks;N_{tracks};entries",6,-0.5,5.5);
  fNumberOfTrdVTracksHistoafterPreselection = new TH1I("NumberOfTrdVTracksHistoafterPreselection","Number of TRD Tracks;N_{tracks};entries",6,-0.5,5.5);

  fEventsvsLatLon = new TH2I("EventsvsLatLon","Events before Preselection;Longitude [deg]; Latitude [deg]",720,-180,180,360,-90,90);
  fEventsvsLatLonafterPreselection = new TH2I("EventsvsLatLonafterPreselection","Events after Preselection;Longitude [deg]; Latitude [deg]",720,-180,180,360,-90,90);

  //There are 5248 Tubes in 20 Layers
  fHPreCalibrationMpvPerStraw = new TGraphErrors();
  fHPreCalibrationMpvPerStraw->SetName("HPreCalibrationMpvPerStraw");
  fHPreCalibrationMpvPerStraw->SetTitle("H-based PreCalib MPV per Straw;Straw;MPV [ADC]");

  fHPreCalibrationPhsHisto = new TH2I("HPreCalibrationPhsHisto","H-based PreCalib Pulse-height Spectra;Straw;deposited energy [ADC]",5248,-0.5,5247.5,400,0.,400.);
  fHPreCalibrationMpvHisto = new TH1I("HPreCalibrationMpvHisto","H-based PreCalib Most Probable Value (Straw Level);MPV [ADC];entries",240,0.,120.);
  fHPreCalibrationWidthHisto = new TH1I("HPreCalibrationWidthHisto","H-based PreCalib Width of PHS (Landau, Straw Level);Width [ADC];entries",120,0.,60.);

  fVPreCalibrationMpvPerStraw = new TGraphErrors();
  fVPreCalibrationMpvPerStraw->SetName("VPreCalibrationMpvPerStraw");
  fVPreCalibrationMpvPerStraw->SetTitle("V-based PreCalib MPV per Straw;Straw;MPV [ADC]");

  fVPreCalibrationPhsHisto = new TH2I("VPreCalibrationPhsHisto","V-based PreCalib Pulse-height Spectra;Straw;deposited energy [ADC]",5248,-0.5,5247.5,400,0.,400.);
  fVPreCalibrationMpvHisto = new TH1I("VPreCalibrationMpvHisto","V-based PreCalib Most Probable Value (Straw Level);MPV [ADC];entries",240,0.,120.);
  fVPreCalibrationWidthHisto = new TH1I("VPreCalibrationWidthHisto","V-based PreCalib Width of PHS (Landau, Straw Level);Width [ADC];entries",120,0.,60.);

  fTPreCalibrationMpvPerStraw = new TGraphErrors();
  fTPreCalibrationMpvPerStraw->SetName("TPreCalibrationMpvPerStraw");
  fTPreCalibrationMpvPerStraw->SetTitle("T-based PreCalib MPV per Straw;Straw;MPV [ADC]");

  fTPreCalibrationPhsHisto = new TH2I("TPreCalibrationPhsHisto","T-based PreCalib Pulse-height Spectra;Straw;deposited energy [ADC]",5248,-0.5,5247.5,400,0.,400.);
  fTPreCalibrationMpvHisto = new TH1I("TPreCalibrationMpvHisto","T-based PreCalib Most Probable Value (Straw Level);MPV [ADC];entries",240,0.,120.);
  fTPreCalibrationWidthHisto = new TH1I("TPreCalibrationWidthHisto","T-based PreCalib Width of PHS (Landau, Straw Level);Width [ADC];entries",120,0.,60.);

  fHOccupancyOnTrack = new TH2I("HOccupancyOnTrack","Occupancy Hits on H Track (d<2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);
  fHOccupancyOffTrack = new TH2I("HOccupancyOffTrack","Occupancy Hits off H Track (d>2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);
  fVOccupancyOnTrack = new TH2I("VOccupancyOnTrack","Occupancy Hits on V Track (d<2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);
  fVOccupancyOffTrack = new TH2I("VOccupancyOffTrack","Occupancy Hits off V Track (d>2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);
  fTOccupancyOnTrack = new TH2I("TOccupancyOnTrack","Occupancy Hits on T Track (d<2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);
  fTOccupancyOffTrack = new TH2I("TOccupancyOffTrack","Occupancy Hits on T Track (d<2cm);16*Ladder+Straw;Layer",288,-0.5,287.5,20,-0.5,19.5);

  fHTrackingQuality = new TH2I("HTrackingQuality","H Track Matching in UTof;difference in X [cm];difference in Y [cm]",80,-8.,8.,80,-8.,8.);
  fVTrackingQuality = new TH2I("VTrackingQuality","V Track Matching in UTof;difference in X [cm];difference in Y [cm]",80,-8.,8.,80,-8.,8.);
  fHTrackingQualityAngle = new TH1I("HTrackingQualityAngle","Angle between H Track and T track at UToF;angle [deg];entries",100,0.,10.);
  fVTrackingQualityAngle = new TH1I("VTrackingQualityAngle","Angle between V Track and T track at UToF;angle [deg];entries",100,0.,10.);

  fHCalibFactors = new TProfile("HCalibFactors","HCalib Factors by Straw;Straw;calibration factor",5248,-0.5,5247.5);

  fTest = new TH1I("Test","HDistance Track-Hit;Distance [mm];entries",48,0.,12.);
  fTest2 = new TH1I("Test2","HDistance Track-Hit;Distance [mm];entries",48,0.,12.);

  // store pointers in the list of histograms, to simplify operations on all histograms of the class
  fHistogramList->Add(fNumberOfTrdHitsHisto);
  fHistogramList->Add(fNumberOfTrdHitsHistoafterPreselection);
//   fHistogramList->Add(fNumberOfTrdHitsOnHTrack);
  fHistogramList->Add(fNumberOfTrdHitsOnVTrack);
  fHistogramList->Add(fNumberOfTrdHitsOnTTrack);
//   fHistogramList->Add(fNumberOfTrdHitsAroundHTrack);
  fHistogramList->Add(fNumberOfTrdHitsAroundVTrack);
  fHistogramList->Add(fNumberOfTrdHitsAroundTTrack);
//   fHistogramList->Add(fNumberOfTrdHTracksHisto);
  fHistogramList->Add(fNumberOfTrdVTracksHisto);
//   fHistogramList->Add(fNumberOfTrdHTracksHistoafterPreselection);
  fHistogramList->Add(fNumberOfTrdVTracksHistoafterPreselection);
  fHistogramList->Add(fEventsvsLatLon);
  fHistogramList->Add(fEventsvsLatLonafterPreselection);
//   fHistogramList->Add(fHPreCalibrationPhsHisto);
//   fHistogramList->Add(fHPreCalibrationMpvPerStraw);
//   fHistogramList->Add(fHPreCalibrationMpvHisto);
//   fHistogramList->Add(fHPreCalibrationWidthHisto);
  fHistogramList->Add(fVPreCalibrationPhsHisto);
  fHistogramList->Add(fVPreCalibrationMpvPerStraw);
  fHistogramList->Add(fVPreCalibrationMpvHisto);
  fHistogramList->Add(fVPreCalibrationWidthHisto);
  fHistogramList->Add(fTPreCalibrationPhsHisto);
  fHistogramList->Add(fTPreCalibrationMpvPerStraw);
  fHistogramList->Add(fTPreCalibrationMpvHisto);
  fHistogramList->Add(fTPreCalibrationWidthHisto);
//   fHistogramList->Add(fHOccupancyOnTrack);
//   fHistogramList->Add(fHOccupancyOffTrack);
  fHistogramList->Add(fVOccupancyOnTrack);
  fHistogramList->Add(fVOccupancyOffTrack);
  fHistogramList->Add(fTOccupancyOnTrack);
  fHistogramList->Add(fTOccupancyOffTrack);
//   fHistogramList->Add(fHTrackingQuality);
  fHistogramList->Add(fVTrackingQuality);
//   fHistogramList->Add(fHTrackingQualityAngle);
  fHistogramList->Add(fVTrackingQualityAngle);
  //fHistogramList->Add(fHCalibFactors);

  //fHistogramList->Add(fTest);
  //fHistogramList->Add(fTest2);

  return;
}


/** Fill histogram from the current event.
  *
  * \param[in] event pointer to the current event
  */

void Analysis::TrdPerformancePlots::FillHistosRaw( const Analysis::Particle& particle ) {

  const AC::Event* rawEvent = particle.RawEvent();

  fNumberOfTrdHitsHisto->Fill( rawEvent->TRD().RawHits().size() );
  //fNumberOfTrdHTracksHisto->Fill( event.TRD().HTracks().size() );
  fNumberOfTrdVTracksHisto->Fill( rawEvent->TRD().VTracks().size() );

  const AC::EventHeader& head = rawEvent->EventHeader();

  fEventsvsLatLonafterPreselection->Fill(((head.ISSLongitude()>180. && head.ISSLongitude()<360.) ? head.ISSLongitude()-360.:head.ISSLongitude()), head.ISSLatitude());

  return;
}


/** Fill Histograms from Event after preselection.
 * \param[in] event pointer to the current event
 * \todo HTrack does not exist
 */
void Analysis::TrdPerformancePlots::FillHistosPreselected( const Analysis::Particle& particle ){

  const AC::Event* rawEvent = particle.RawEvent();
  const AC::EventHeader& head = rawEvent->EventHeader();

  fEventsvsLatLonafterPreselection->Fill(((head.ISSLongitude()>180. && head.ISSLongitude()<360.) ? head.ISSLongitude()-360.:head.ISSLongitude()), head.ISSLatitude());

  fNumberOfTrdHitsHistoafterPreselection->Fill( rawEvent->TRD().RawHits().size() );
//   fNumberOfTrdHTracksHistoafterPreselection->Fill( event.TRD().HTracks().size() );
  fNumberOfTrdVTracksHistoafterPreselection->Fill( rawEvent->TRD().VTracks().size() );

  //Check if Hit is on Track to fill certain histos
//   int NhitsOnH=0,NhitsAroundH;
  int NhitsOnV=0,NhitsAroundV=0, NhitsOnT=0,NhitsAroundT=0;

  //Generate SplineTrack from TrackerTrack
  const Analysis::SplineTrack* TrkTrackSpline = particle.GetSplineTrack();

  // Get TRD Track Objects from event
  // FIXME pass objects by reference!!!
  // FIXME take correct objects from high-level event class instead of duplicating selection here!
  AC::TRDVTrack Vtrack;
  if (!rawEvent->TRD().VTracks().isEmpty())
    Vtrack = rawEvent->TRD().VTracks()[0];
  AC::TRDHTrack Htrack;
  if (!rawEvent->TRD().HTracks().isEmpty())
    Htrack = rawEvent->TRD().HTracks()[0];

  for(int h=0;h<(int)rawEvent->TRD().RawHits().size();h++){ // loop over hits

    TrdHit* myHit = new TrdHit(rawEvent->TRD().RawHits()[h]);
    int StrawNumber = myHit->GetGlobalStrawNumber();
    int LadderForOccupancy=myHit->Ladder();
    if(myHit->Layer()<12) LadderForOccupancy++;
    if(myHit->Layer()<4) LadderForOccupancy++;

    // HTRACK
    // HTrack does not work!
    /*
    if(event.TRD().HTracks().size()!=0){
      double dist=myHit->DistanceToTrack(event.TRD().HTracks()[0]); //in mm

      if(fabs(dist)<2.0){ //cm
        NhitsAroundH++;
        if(fabs(dist)<0.3) NhitsOnH++;
        //Fill Histos
        fHPreCalibrationPhsHisto->Fill( StrawNumber,myHit->GetAmplitude() );
        fHOccupancyOnTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
      else{
        //Fill other Histos
        fHOccupancyOffTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
    }
    */

    //V TRACK
    if(rawEvent->TRD().VTracks().size()!=0){
      double dist=myHit->DistanceToTrack(Vtrack);
      if(fabs(dist)<2.0){ //in cm
        NhitsAroundV++;
        if(fabs(dist)<0.3) NhitsOnV++;
        //Fill Histos
        fVPreCalibrationPhsHisto->Fill( StrawNumber,myHit->GetAmplitude() );
        fVOccupancyOnTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
      else{
        //Fill other Histos
        fVOccupancyOffTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
    }

    // TRK Track
    if(rawEvent->Tracker().Tracks().size()!=0){
      double dist=myHit->DistanceToTrack(*TrkTrackSpline);
      if(fabs(dist)<2.0){ //in cm
        NhitsAroundT++;
        if(fabs(dist)<0.3) NhitsOnT++;
        //Fill Histos
        fTPreCalibrationPhsHisto->Fill( StrawNumber,myHit->GetAmplitude() );
        fTOccupancyOnTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
      else{
        //Fill other Histos
        fTOccupancyOffTrack->Fill(16*LadderForOccupancy+myHit->Tube(),myHit->Layer());
      }
    }

    delete myHit;
  } //end loop over hits

//   fNumberOfTrdHitsOnHTrack->Fill( NhitsOnH );
  fNumberOfTrdHitsOnVTrack->Fill( NhitsOnV );
  fNumberOfTrdHitsOnTTrack->Fill( NhitsOnT );
//   fNumberOfTrdHitsAroundHTrack->Fill( NhitsAroundH );
  fNumberOfTrdHitsAroundVTrack->Fill( NhitsAroundV );
  fNumberOfTrdHitsAroundTTrack->Fill( NhitsAroundT );

  //Tracking Quality
//  TVector3* Hvector1 = new TVector3(,,63.65);
//  TVector3* Hvector2 = new TVector3(,,66.6);
  TVector3 Vvector1(Vtrack.Extrapolate_to_z(63.65,AC::XZMeasurement),Vtrack.Extrapolate_to_z(63.65,AC::YZMeasurement),63.65);
  TVector3 Vvector2(Vtrack.Extrapolate_to_z(66.6, AC::XZMeasurement),Vtrack.Extrapolate_to_z(66.6, AC::YZMeasurement),66.6);
  TVector3 Tvector1 = TrkTrackSpline->InterpolateToZ(63.65);
  TVector3 Tvector2 = TrkTrackSpline->InterpolateToZ(66.6);

//  TVector3* Hdirection = Hvector2 - Hvector1;
  TVector3 Vdirection = Vvector2 - Vvector1;
  TVector3 Tdirection = Tvector2 - Tvector1;

  //  fVTrackingQuality->Fill(Vvector1.X()-Tvector1.X(), Vvector1.Y()-Tvector1.Y());
  //fVTrackingQualityAngle->Fill(Tdirection->Angle(Vdirection)*TMath::RadtoDeg);
  fVTrackingQuality->Fill(Vvector1.X()-Tvector1.X(), Vvector1.Y()-Tvector1.Y());
  fVTrackingQualityAngle->Fill(Tdirection.Angle(Vdirection)*TMath::RadToDeg());

  return;
}

/** Fill Histos that need event-by-event filled histos as input.
  *
  */
void Analysis::TrdPerformancePlots::FillIntegralHistos() {

  INFO_OUT << "Filling integral histograms..." << std::endl;

  TH1D* projY = new TH1D("projY","Y Projection;ADC;entries",400,0.,400.);
  TF1* Lfunc = new TF1("Lfunc","landau(0)",0.,400.); //1 mpv, 2 sigma

  //Lfunc->SetParameters(60,20);
/* FUNKTIONIERT IM MOMENT NICHT WEGEN H Track
  for(int i=0;i<5248;i++){
    fHPreCalibrationPhsHisto->ProjectionY("projY",i+1,i+1);
    Lfunc->SetParameters(60,20);
    projY->Fit("Lfunc","R0Q");

    fHPreCalibrationMpvPerStraw->SetPoint(i,i,Lfunc->GetParameter(0));
    fHPreCalibrationMpvPerStraw->SetPointError(i,0,Lfunc->GetParError(0));

    fHPreCalibrationMpvHisto->Fill(Lfunc->GetParameter(0));
    fHPreCalibrationWidthHisto->Fill(Lfunc->GetParameter(1));
  }
*/

  for(int i=0;i<5248;i++){
    fVPreCalibrationPhsHisto->ProjectionY("projY",i+1,i+1);
    Lfunc->SetParameters(projY->GetEntries()/150,60,20);
    projY->Fit("Lfunc","0Q");

    fVPreCalibrationMpvPerStraw->SetPoint(i,i,Lfunc->GetParameter(1));
    fVPreCalibrationMpvPerStraw->SetPointError(i,0,Lfunc->GetParError(1));

    fVPreCalibrationMpvHisto->Fill(Lfunc->GetParameter(1));
    fVPreCalibrationWidthHisto->Fill(Lfunc->GetParameter(2));
  }

//5248
  for(int i=0;i<5248;i++){
    fTPreCalibrationPhsHisto->ProjectionY("projY",i+1,i+1);
    Lfunc->SetParameters(projY->GetEntries()/150,60,20);
    projY->Fit("Lfunc","Q0");

    fTPreCalibrationMpvPerStraw->SetPoint(i,i,Lfunc->GetParameter(1));
    fTPreCalibrationMpvPerStraw->SetPointError(i,0,Lfunc->GetParError(1));

    fTPreCalibrationMpvHisto->Fill(Lfunc->GetParameter(1));
    fTPreCalibrationWidthHisto->Fill(Lfunc->GetParameter(2));
  }

  delete projY;
  delete Lfunc;
  return;
}

/** Set drawing style.
  *
  * Ticks on all sides, gStyle is "plain", Color Palette with 512 shades, prepared for b/w coloring.
  */
void Analysis::TrdPerformancePlots::SetTrdPerformanceStyle(){
  gStyle->Reset("Plain");

  Double_t stops[5] = {0.00, 0.34, 0.61, 0.84, 1.00};

  //===============
  // "white to black"
  //===============
  //Double_t red[5]   = {1.00, 0.84, 0.61, 0.34, 0.00};
  //Double_t green[5] = {1.00, 0.84, 0.61, 0.34, 0.00};
  //Double_t blue[5]  = {1.00, 0.84, 0.61, 0.34, 0.00};

  //starting a little darker
  //Double_t red[5]   = {.9, 0.68, 0.45, 0.22, 0.00};
  //Double_t green[5] = {.9, 0.68, 0.45, 0.22, 0.00};
  //Double_t blue[5]  = {.9, 0.68, 0.45, 0.22, 0.00};

  //===============
  // "rainbow"
  //===============
  Double_t red[5]   = {0.00, 0.00, 0.87, 1.00, 0.51};
  Double_t green[5] = {0.00, 0.81, 1.00, 0.20, 0.00};
  Double_t blue[5]  = {0.51, 1.00, 0.12, 0.00, 0.00};

  TColor::CreateGradientColorTable(5, stops, red, green, blue, 512);
  gStyle->SetNumberContours(512);

  gStyle->SetMarkerStyle(20);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  return;
}

/** Create canvases and draw and save histograms.
  * \todo Legends
  */
void Analysis::TrdPerformancePlots::DrawHistos() {

  std::string PdfFileName = Analysis::FileManager::MakeStandardPdfFileName(fResultDirectory,fOutputFilePrefix,fResultFileSuffix);
  INFO_OUT << "Drawing histograms in " << PdfFileName.c_str() << std::endl;

  SetTrdPerformanceStyle();

  //Earth
  TGraph* Gearth = new TGraph("$ROOTSYS/tutorials/graphics/earth.dat");
  Gearth->SetMarkerStyle(6);

  TLine* line = new TLine();
  line->SetLineWidth(2);
  line->SetLineStyle(9);

  //////////////////////////////////////
  // Drawing convention
  //Cuts by colors, items by linestyle
  ////////////////////////////////////////

  //Initialize Canvases and Draw into them

  //////Canvas0 --- Number of events
  TCanvas* Canvas0 = new TCanvas("Canvas0", "Canvas0", 800, 600);
  Canvas0->SetLogy();
  fNumberOfTrdHitsHisto->SetStats(false);
  fNumberOfTrdHitsHistoafterPreselection->SetStats(false);
  fNumberOfTrdHitsHisto->SetLineColor(1);
  fNumberOfTrdHitsHisto->Draw();
  fNumberOfTrdHitsHistoafterPreselection->SetLineColor(2);
  fNumberOfTrdHitsHistoafterPreselection->Draw("same");

  TLegend* leg0 = new TLegend(0.5,0.9,0.9,0.75);
  leg0->AddEntry(fNumberOfTrdHitsHisto,"Number of TRD Hits","l");
  TLegendEntry* leg0e1 = leg0->AddEntry(fNumberOfTrdHitsHistoafterPreselection,"Number of TRD Hits after Preselection","l");
  leg0e1->SetTextColor(2);
  leg0->Draw("same");


  //////Canvas1 --- Number of tracks
  TCanvas* Canvas1 = new TCanvas("Canvas1", "Canvas1", 800, 600);
  Canvas1->SetLogy();
  fNumberOfTrdVTracksHisto->SetStats(false);
  fNumberOfTrdVTracksHistoafterPreselection->SetStats(false);
  fNumberOfTrdVTracksHisto->Draw();
  fNumberOfTrdVTracksHistoafterPreselection->SetLineColor(2);
  fNumberOfTrdVTracksHistoafterPreselection->Draw("same");

  TLegend* leg1 = new TLegend(0.5,0.9,0.9,0.75);
  TLegendEntry* leg1e0 = leg1->AddEntry(fNumberOfTrdVTracksHisto,"Number of V tracks","l");
  leg1e0->SetTextColor(4);
  TLegendEntry* leg1e1 = leg1->AddEntry(fNumberOfTrdVTracksHistoafterPreselection,"Number of V tracks after Preselection","l");
  leg1e1->SetTextColor(2);
  leg1->Draw("same");


  //////Canvas2 --- HIts on/around Track
  TCanvas* Canvas2 = new TCanvas("Canvas2", "Canvas2", 800, 600);
  Canvas2->SetLogy();
  fNumberOfTrdHitsOnVTrack->SetStats(false);
  fNumberOfTrdHitsOnTTrack->SetStats(false);
  fNumberOfTrdHitsAroundVTrack->SetStats(false);
  fNumberOfTrdHitsAroundTTrack->SetStats(false);

  fNumberOfTrdHitsOnVTrack->SetLineColor(2);
  fNumberOfTrdHitsOnTTrack->SetLineColor(2);
  fNumberOfTrdHitsOnVTrack->Draw();
  fNumberOfTrdHitsOnTTrack->SetLineStyle(8);
  fNumberOfTrdHitsOnTTrack->Draw("same");

  fNumberOfTrdHitsAroundVTrack->SetLineColor(1);
  fNumberOfTrdHitsAroundTTrack->SetLineColor(1);
  fNumberOfTrdHitsAroundVTrack->Draw("same");
  fNumberOfTrdHitsAroundTTrack->SetLineStyle(8);
  fNumberOfTrdHitsAroundTTrack->Draw("same");

  TLegend* leg2 = new TLegend(0.13,0.13,0.6,0.35);
  leg2->SetNColumns(2);
  TLegendEntry* leg2e0 = leg2->AddEntry(fNumberOfTrdHitsOnVTrack,"Hits on V Track (d<0.3cm)","l");
  leg2e0->SetTextColor(2);
  leg2->AddEntry(fNumberOfTrdHitsAroundVTrack,"Hits around V Track (d<2cm)","l");
  TLegendEntry* leg2e2 = leg2->AddEntry(fNumberOfTrdHitsOnTTrack,"Hits on T Track (d<0.3cm)","l");
  leg2e2->SetTextColor(2);
  leg2->AddEntry(fNumberOfTrdHitsAroundTTrack,"Hits around T Track (d<2cm)","l");
  leg2->Draw("same");


  //////Canvas10 --- Tracking Quality
  TCanvas* Canvas10 = new TCanvas("Canvas10", "Canvas10", 800, 600);
  Canvas10->Divide(2,1);
  fVTrackingQuality->SetStats(false);
  fVTrackingQualityAngle->SetStats(false);
  Canvas10->cd(1);
  gPad->SetLogz();
  fVTrackingQuality->Draw("COLZ");
  Canvas10->cd(2);
  gPad->SetLogy();
  fVTrackingQualityAngle->Draw();


  //////Canvas8 --- Occupancy V Track
  TCanvas* Canvas8 = new TCanvas("Canvas8", "Canvas8", 800, 600);
  Canvas8->Divide(2,1);
  fVOccupancyOffTrack->SetStats(false);
  fVOccupancyOnTrack->SetStats(false);
  Canvas8->cd(1);
  fVOccupancyOffTrack->Draw("COLZ");
  line->DrawLine(0,15.5,288,15.5);
  line->DrawLine(0,3.5,288,3.5);
  Canvas8->cd(2);
  fVOccupancyOnTrack->Draw("COLZ");
  line->DrawLine(0,15.5,288,15.5);
  line->DrawLine(0,3.5,288,3.5);

  //////Canvas8 --- Occupancy T Track
  TCanvas* Canvas9 = new TCanvas("Canvas9", "Canvas9", 800, 600);
  Canvas9->Divide(2,1);
  fTOccupancyOffTrack->SetStats(false);
  fTOccupancyOnTrack->SetStats(false);
  Canvas9->cd(1);
  fTOccupancyOffTrack->Draw("COLZ");
  line->DrawLine(0,15.5,288,15.5);
  line->DrawLine(0,3.5,288,3.5);
  Canvas9->cd(2);
  fTOccupancyOnTrack->Draw("COLZ");
  line->DrawLine(0,15.5,288,15.5);
  line->DrawLine(0,3.5,288,3.5);

  //////Canvas3 --- Geometric distribution of events
  TCanvas* Canvas3 = new TCanvas("Canvas3", "Canvas3", 800, 600);
  Canvas3->Divide(2,1);
  fEventsvsLatLon->SetStats(false);
  fEventsvsLatLonafterPreselection->SetStats(false);
  Canvas3->cd(1);
  fEventsvsLatLon->Draw("COLZ");
  Gearth->Draw("Psame");
  Canvas3->cd(2);
  fEventsvsLatLonafterPreselection->Draw("COLZ");
  Gearth->Draw("Psame");

  //////Canvas4 --- Pre-calib PHS V Track based
  TCanvas* Canvas4 = new TCanvas("Canvas4", "Canvas4", 800, 600);
  fVPreCalibrationPhsHisto->SetStats(false);
  fVPreCalibrationPhsHisto->Draw("COLZ");

  //////Canvas5 --- Pre-calib PHS properties V Track based
  TCanvas* Canvas5 = new TCanvas("Canvas5", "Canvas5", 800, 600);
  Canvas5->Divide(1,2);
  Canvas5->cd(1);
  fVPreCalibrationMpvPerStraw->Draw("AP");
  Canvas5->cd(2);
  gPad->Divide(2,1);
  gPad->cd(1);
  fVPreCalibrationMpvHisto->Draw();
  Canvas5->cd(2);
  gPad->cd(2);
  fVPreCalibrationWidthHisto->Draw();

  //////Canvas6 --- Pre-calib PHS T Track based
  TCanvas* Canvas6 = new TCanvas("Canvas6", "Canvas6", 800, 600);
  fTPreCalibrationPhsHisto->SetStats(false);
  fTPreCalibrationPhsHisto->Draw("COLZ");

  //////Canvas7 --- Pre-calib PHS properties T Track based
  TCanvas* Canvas7 = new TCanvas("Canvas7", "Canvas7", 800, 600);
  fTPreCalibrationMpvHisto->SetLineStyle(8);;
  fTPreCalibrationWidthHisto->SetLineStyle(8);;
  Canvas7->Divide(1,2);
  Canvas7->cd(1);
  fTPreCalibrationMpvPerStraw->Draw("AP");
  Canvas7->cd(2);
  gPad->Divide(2,1);
  gPad->cd(1);
  fTPreCalibrationMpvHisto->Draw();
  Canvas7->cd(2);
  gPad->cd(2);
  fTPreCalibrationWidthHisto->Draw();


  //Save Canvases
  Canvas0->Print((PdfFileName+std::string("(")).c_str(),"pdf");
  Canvas1->Print(PdfFileName.c_str(),"pdf");
  Canvas2->Print(PdfFileName.c_str(),"pdf");
  Canvas10->Print(PdfFileName.c_str(),"pdf");
  Canvas8->Print(PdfFileName.c_str(),"pdf");
  Canvas9->Print(PdfFileName.c_str(),"pdf");
  Canvas3->Print(PdfFileName.c_str(),"pdf");
  Canvas4->Print(PdfFileName.c_str(),"pdf");
  Canvas5->Print(PdfFileName.c_str(),"pdf");
  Canvas6->Print(PdfFileName.c_str(),"pdf");
  Canvas7->Print((PdfFileName+std::string(")")).c_str(),"pdf");

  delete Canvas0;
//   delete leg0e0;
//   delete leg0e1;
  delete leg0;
  delete Canvas1;
/*  delete leg1e0;
  delete leg1e1;*/
  delete leg1;
  delete Canvas2;
/*  delete leg2e0;
  delete leg2e1;
  delete leg2e2;
  delete leg2e3;*/
  delete leg2;
  delete Canvas3;
  delete Canvas4;
  delete Canvas5;
  delete Canvas6;
  delete Canvas7;
  delete Canvas8;
  delete Canvas9;
  delete Canvas10;
  delete Gearth;
  delete line;

 return;
}


/** Writes all histograms to the currently opened ROOT file.
  *
  */
void Analysis::TrdPerformancePlots::WriteHistos() {

  INFO_OUT << "Write Histograms to current root file..." << std::endl;

  for( int i=0 ; i<fHistogramList->GetSize() ; ++i ) {
    fHistogramList->At(i)->Write();
  }
}

/** Creates a new ROOT file and writes all histograms to the file.
  *
  * The filename is set via the configuration file.
  * \sa Analysis::FileManager::MakeStandardRootFileName
  */
void Analysis::TrdPerformancePlots::WriteHistosToNewFile() {

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
