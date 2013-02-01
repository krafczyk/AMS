#include "TrdLayer.hh"

#include "AMSGeometry.h"
#include "TrdDetector.hh"
#include "TrdSublayer.hh"

#include "dumpstreamers.hh"

#include <TMath.h>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdLayer> "
#include <debugging.hh>

ACsoft::Detector::TrdLayer::TrdLayer( int layerNumber, Trd* mother ) :
  fLayerNumber(layerNumber),
  fMother(mother)
{

  assert( layerNumber >= 0 && layerNumber < int(AC::AMSGeometry::TRDLayers) );

  double zrel = (double(layerNumber)-9.5) * AC::AMSGeometry::TrdLayerThickness;
  fNominalRelativePosition = TVector3(0.0,0.0,zrel);
  fOffsetRelativePosition  = TVector3(0.0,0.0,0.0);
  fNominalRelativeRotation.SetToIdentity();
  if( layerNumber <= 3 || layerNumber >= 16 ){
    fNominalRelativeRotation.RotateZ(90.0*TMath::DegToRad());
  }

  fExtraRelativeRotation.SetToIdentity();

  // set global values to dummy values, will be set by TRD anyway
  fGlobalPosition = TVector3();
  fGlobalRotation.SetToIdentity();

  ConstructSublayers();
}

ACsoft::Detector::TrdLayer::~TrdLayer() {

  for( unsigned int i = 0 ; i < fSublayers.size() ; ++i )
    delete fSublayers[i];
}


void ACsoft::Detector::TrdLayer::ConstructSublayers() {

  for( int i=0 ; i<2 ; ++i )
    fSublayers.push_back( new ACsoft::Detector::TrdSublayer(fLayerNumber,i,this) );
}

void ACsoft::Detector::TrdLayer::FillConvenienceVectors() {

  fTrdModules.clear();

  for( unsigned int j=0 ; j<NumberOfSublayers() ; ++j ){

    ACsoft::Detector::TrdSublayer* sublayer = GetSublayer(j);

    for( unsigned int m = 0 ; m<sublayer->NumberOfModules() ; ++m ){
      TrdModule* module = sublayer->GetModule(m);
//      int moduleNumber = module->ModuleNumber();
      fTrdModules.push_back( module );
    }
  }
}


TVector3 ACsoft::Detector::TrdLayer::GlobalPosition() const {

  return fGlobalPosition;
}

TRotation ACsoft::Detector::TrdLayer::GlobalRotation() const {

  return fGlobalRotation;
}

void ACsoft::Detector::TrdLayer::Dump() const {

  INFO_OUT << "TRD Layer " << fLayerNumber << " relative position " << fNominalRelativePosition << " + " << fOffsetRelativePosition << " rotation " << fExtraRelativeRotation << " * " << fNominalRelativeRotation
           << " global position " << GlobalPosition() << " rotation " << GlobalRotation() << std::endl;

  for( unsigned int i = 0 ; i < fSublayers.size() ; ++i ){

    ACsoft::Detector::TrdSublayer* sublayer = fSublayers[i];
    sublayer->Dump();
  }
}


/** Change the alignment parameters for this layer and trigger
  * a recalculation of the global coordinates of the layer and all its sublayers.
  *
  * The nominal position and orientation will remain unchanged.
  */
void ACsoft::Detector::TrdLayer::ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot ) {

  fOffsetRelativePosition = offsetPos;
  fExtraRelativeRotation  = extraRot;

  UpdateGlobalPositionAndDirection();
}

/** Recalculate global position and rotation for this layer and all its sublayers.
  *
  */
void ACsoft::Detector::TrdLayer::UpdateGlobalPositionAndDirection() {

  DEBUG_OUT << "mother pos: " << fMother->Position() << " my rel pos: " << RelativePosition() << std::endl;

  SetGlobalPosition( fMother->Position() + fMother->Rotation()*RelativePosition() );
  SetGlobalRotation( fMother->Rotation() * RelativeRotation() );

  for( unsigned int i = 0 ; i < fSublayers.size() ; ++i ){

    ACsoft::Detector::TrdSublayer* sublayer = fSublayers[i];
    sublayer->UpdateGlobalPositionAndDirection();
  }
}
