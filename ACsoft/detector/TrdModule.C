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

ACsoft::Detector::TrdModule::TrdModule( int moduleNumber, TrdSublayer* mother ) :
  fAlignmentParametersOk(false),
  fCurrentGainValue(100.0),
  fCurrentGainValueOk(false),
  fModuleNumber( moduleNumber ),
  fMother(mother)
{

  assert(fMother);

  assert(moduleNumber >= 0 && moduleNumber < int(ACsoft::AC::AMSGeometry::TRDModules));

  // get layer and ladder number from geometry lookup
  unsigned short layer, ladder;
  ACsoft::AC::AMSGeometry::Self()->TRDModuleToLadderAndLayer((unsigned short)(fModuleNumber),ladder,layer);
  fLayerNumber = layer;
  fLadderNumber = ladder;

  fSublayerNumber = mother ? mother->SublayerNumber() : 0;
  fGlobalSublayerNumber = mother ? mother->GlobalSublayerNumber() : 0;

  fTowerNumber = fLadderNumber;
  if (fLayerNumber < 12) ++fTowerNumber;
  if (fLayerNumber < 4) ++fTowerNumber;

  fDirection = fMother->Direction();

  float deltaBulkhead = 0.0;
  if( ( layer >= 16 || layer <= 3 ) ){
    if( fTowerNumber >= 12 ) deltaBulkhead = +0.78;
    if( fTowerNumber <=  5 ) deltaBulkhead = -0.78;
  }
  float xrel = ACsoft::AC::AMSGeometry::TrdModuleWidth*(float(fTowerNumber)-8.5) + deltaBulkhead;

  fNominalRelativePosition = TVector3(xrel,0.,0.);
  fShimmingOffset = TVector3(0.,0.,0.);
  fOffsetRelativePosition = TVector3(0.,0.,0.);

  fNominalRelativeRotation.SetToIdentity();
  fShimmingRotation.SetToIdentity();
  fExtraRelativeRotation.SetToIdentity();

  // set global values to dummy values, will be set by sublayer anyway
  fGlobalPosition = TVector3();
  fGlobalRotation.SetToIdentity();

  ConstructStraws();
}

ACsoft::Detector::TrdModule::~TrdModule() {

  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){
    delete fStraws[i];
  }
}


void ACsoft::Detector::TrdModule::ConstructStraws() {

  for( unsigned int straw=0 ; straw<ACsoft::AC::AMSGeometry::TRDStrawsPerModule; ++straw ){

    fStraws.push_back( new ACsoft::Detector::TrdStraw( fLayerNumber, fSublayerNumber, fGlobalSublayerNumber, fLadderNumber, fTowerNumber, fModuleNumber, straw, this ) );
  }

}


/** Change the alignment parameters for this module and trigger
  * a recalculation of the global coordinates of the module and all its straws.
  *
  * The nominal position and orientation will remain unchanged.
  */
void ACsoft::Detector::TrdModule::ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot, bool alignmentOk ) {

  fOffsetRelativePosition = offsetPos;
  fExtraRelativeRotation  = extraRot;
  fAlignmentParametersOk  = alignmentOk;

  UpdateGlobalPositionAndDirection();
}

/** Recalculate global position and rotation for this module and all its straws.
  *
  */
void ACsoft::Detector::TrdModule::UpdateGlobalPositionAndDirection() {

  SetGlobalPosition( fMother->GlobalPosition() + fMother->GlobalRotation()*RelativePosition() );
  SetGlobalRotation( fMother->GlobalRotation() * RelativeRotation() );

  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){

    ACsoft::Detector::TrdStraw* straw = fStraws[i];
    straw->UpdateGlobalPositionAndDirection();
  }
}

TVector3 ACsoft::Detector::TrdModule::GlobalPosition() const {

  return fGlobalPosition;
}


TRotation ACsoft::Detector::TrdModule::GlobalRotation() const {

  return fGlobalRotation;
}


void ACsoft::Detector::TrdModule::SetShimmingRotation( const TRotation& matrix ) {

  if( fShimmingRotation != TRotation() )
    WARN_OUT << "Repeated call to SetShimmingRotation!" << std::endl;

  fShimmingRotation = matrix;
}

void ACsoft::Detector::TrdModule::SetShimmingOffset( const TVector3& offset ) {

  if( fShimmingOffset != TVector3(0.,0.,0.) )
    WARN_OUT << "Repeated call to SetShimmingOffset!" << std::endl;

  fShimmingOffset = offset;
}

void ACsoft::Detector::TrdModule::Dump() const {

  INFO_OUT << "TRD Module " << fLayerNumber << "," << fSublayerNumber << "," << fModuleNumber
           << " (lad " << fLadderNumber << " tower " << fTowerNumber << ")"
           << " relative position " << fNominalRelativePosition << " + " << fShimmingOffset << " + "  << fOffsetRelativePosition
           << " rotation " << fExtraRelativeRotation << " * " << fShimmingRotation << " * " << fNominalRelativeRotation
           << " global position " << GlobalPosition() << " rotation " << GlobalRotation() << std::endl;

//  for( unsigned int i = 0 ; i < fStraws.size() ; ++i ){

//    ACsoft::Detector::TrdStraw* straw = fStraws[i];
//    straw->Dump();
//  }
}







