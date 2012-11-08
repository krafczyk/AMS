#include "TrdModule.hh"

#include "TrdStraw.hh"
#include "TrdSublayer.hh"
#include "TimeHistogramManager.hh"
#include "dumpstreamers.hh"

#include "AMSGeometry.h"

#include <sstream>
#include <TH1.h>
#include <TH2.h>
#include <TTimeStamp.h>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdModule> "
#include <debugging.hh>

Detector::TrdModule::TrdModule( int moduleNumber, TrdSublayer* mother ) :
  fModuleNumber( moduleNumber ),
  fMother(mother),
  fAlignmentShiftHisto(0),
  fDeDxHisto(0)
{

  assert(moduleNumber >= 0 && moduleNumber < int(AC::AMSGeometry::TRDModules));

  // get layer and ladder number from geometry lookup
  unsigned short layer, ladder;
  AC::AMSGeometry::Self()->TRDModuleToLadderAndLayer((unsigned short)(fModuleNumber),ladder,layer);
  fLayerNumber = layer;
  fLadderNumber = ladder;

  fSublayerNumber = mother ? mother->SublayerNumber() : 0;

  fTowerNumber = fLadderNumber;
  if (fLayerNumber < 12) ++fTowerNumber;
  if (fLayerNumber < 4) ++fTowerNumber;


  float deltaBulkhead = 0.0;
  if( ( layer >= 16 || layer <= 3 ) ){
    if( fTowerNumber >= 12 ) deltaBulkhead = +0.78;
    if( fTowerNumber <=  5 ) deltaBulkhead = -0.78;
  }
  float xrel = AC::AMSGeometry::TrdModuleWidth*(float(fTowerNumber)-8.5) + deltaBulkhead;

  fNominalRelativePosition = TVector3(xrel,0.,0.);
  fOffsetRelativePosition = TVector3(0.,0.,0.);

  fNominalRelativeRotation.SetToIdentity();
  fExtraRelativeRotation.SetToIdentity();

  // set global values to dummy values, will be set by sublayer anyway
  fGlobalPosition = TVector3();
  fGlobalRotation.SetToIdentity();

  ConstructStraws();
}

Detector::TrdModule::~TrdModule() {

  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){
    delete fStraws[i];
  }
}


void Detector::TrdModule::ConstructStraws() {

  for( unsigned int straw=0 ; straw<AC::AMSGeometry::TRDStrawsPerModule; ++straw ){

    fStraws.push_back( new Detector::TrdStraw( fLayerNumber, fSublayerNumber, fLadderNumber, fModuleNumber, straw, this ) );
  }

}


/** Change the alignment parameters for this module and trigger
  * a recalculation of the global coordinates of the module and all its straws.
  *
  * The nominal position and orientation will remain unchanged.
  */
void Detector::TrdModule::ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot ) {

  fOffsetRelativePosition = offsetPos;
  fExtraRelativeRotation  = extraRot;

  UpdateGlobalPositionAndDirection();
}

/** Recalculate global position and rotation for this module and all its straws.
  *
  */
void Detector::TrdModule::UpdateGlobalPositionAndDirection() {

  SetGlobalPosition( fMother->GlobalPosition() + fMother->GlobalRotation()*RelativePosition() );
  SetGlobalRotation( fMother->GlobalRotation() * RelativeRotation() );

  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){

    Detector::TrdStraw* straw = fStraws[i];
    straw->UpdateGlobalPositionAndDirection();
  }
}

TVector3 Detector::TrdModule::GlobalPosition() const {

  return fGlobalPosition;
}


TRotation Detector::TrdModule::GlobalRotation() const {

  return fGlobalRotation;
}

void Detector::TrdModule::Dump() const {

  INFO_OUT << "TRD Module " << fLayerNumber << "," << fSublayerNumber << "," << fModuleNumber
           << " (lad " << fLadderNumber << " tower " << fTowerNumber << ")"
           << " relative position " << fNominalRelativePosition << " + " << fOffsetRelativePosition << " rotation " << fExtraRelativeRotation << " * " << fNominalRelativeRotation
           << " global position " << GlobalPosition() << " rotation " << GlobalRotation() << std::endl;

  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){

    Detector::TrdStraw* straw = fStraws[i];
    straw->Dump();
  }
}


void Detector::TrdModule::CreateAlignmentShiftHisto() {

  std::stringstream name, title;
  name << "AlignmentShiftHisto_" << fModuleNumber;
  title << "Alignment shifts for module " << fModuleNumber;
  fAlignmentShiftHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( name.str(), title.str(), "#Delta R / cm", 100, -2.0, 2.0 );
}

void Detector::TrdModule::CreateDeDxHisto() {

  std::stringstream name, title;
  name << "DeDxHisto_" << fModuleNumber;
  title << "dE/dx for module " << fModuleNumber;
  fDeDxHisto = Utilities::TimeHistogramManager::MakeNewTimeHistogram2D<TH2I>( name.str(), title.str(), "dE/dx (ADC/cm)", 200, 0., 400. );
}


void Detector::TrdModule::StoreAlignmentShift( const TTimeStamp& time, Double_t shift ) {

  if(!fAlignmentShiftHisto) CreateAlignmentShiftHisto();
  fAlignmentShiftHisto->Fill(double(time),shift);
}

void Detector::TrdModule::StoreDeDx( const TTimeStamp& time, Double_t dedx ) {

  if(!fDeDxHisto) CreateDeDxHisto();
  fDeDxHisto->Fill(double(time),dedx);
}

void Detector::TrdModule::WriteHistogramsToCurrentFile() {

  if( fAlignmentShiftHisto && fAlignmentShiftHisto->GetEntries() > 0 )
    fAlignmentShiftHisto->Write();

  if( fDeDxHisto && fDeDxHisto->GetEntries() > 0 )
    fDeDxHisto->Write();
}
