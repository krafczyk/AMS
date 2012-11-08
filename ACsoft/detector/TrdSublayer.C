#include "TrdSublayer.hh"

#include "TrdLayer.hh"
#include "TrdModule.hh"

#include "AMSGeometry.h"

#include "dumpstreamers.hh"

#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdSublayer> "
#include <debugging.hh>

Detector::TrdSublayer::TrdSublayer( int layerNumber, int sublayerNumber, TrdLayer* mother ) :
  fLayerNumber(layerNumber),
  fSublayerNumber(sublayerNumber),
  fMother(mother)
{

  assert( layerNumber >= 0 && layerNumber < int(AC::AMSGeometry::TRDLayers) );
  assert( sublayerNumber >= 0 && sublayerNumber < 2 );

  fGlobalSublayerNumber = 2*fLayerNumber + fSublayerNumber;

  float zrel = sublayerNumber ? +0.25*AC::AMSGeometry::TrdLayerThickness : -0.25*AC::AMSGeometry::TrdLayerThickness;
  fNominalRelativePosition = TVector3(0.0,0.0,zrel);
  fOffsetRelativePosition  = TVector3(0.0,0.0,0.0);
  fNominalRelativeRotation.SetToIdentity();
  fExtraRelativeRotation.SetToIdentity();

  // set global values to dummy values, will be set by layer anyway
  fGlobalPosition = TVector3();
  fGlobalRotation.SetToIdentity();

  ConstructModules();
}

Detector::TrdSublayer::~TrdSublayer() { }


void Detector::TrdSublayer::ConstructModules() {

  std::vector<Short_t> modules = AC::AMSGeometry::Self()->TrdModulesOnSublayer( fLayerNumber, fSublayerNumber );

  for( unsigned int i=0 ; i<modules.size() ; ++i )
    fTrdModules.push_back( new Detector::TrdModule(modules[i],this) );
}


TVector3 Detector::TrdSublayer::GlobalPosition() const {

  return fGlobalPosition;
}

TRotation Detector::TrdSublayer::GlobalRotation() const {

  return fGlobalRotation;
}



/** Change the alignment parameters for this sublayer and trigger
  * a recalculation of the global coordinates of the sublayer and all its modules.
  *
  * The nominal position and orientation will remain unchanged.
  */
void Detector::TrdSublayer::ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot ) {

  fOffsetRelativePosition = offsetPos;
  fExtraRelativeRotation  = extraRot;

  UpdateGlobalPositionAndDirection();
}


/** Recalculate global position and rotation for this sublayer and all its modules.
  *
  */
void Detector::TrdSublayer::UpdateGlobalPositionAndDirection() {

  SetGlobalPosition( fMother->GlobalPosition() + fMother->GlobalRotation()*RelativePosition() );
  SetGlobalRotation( fMother->GlobalRotation() * RelativeRotation() );

  for( unsigned int i = 0 ; i < fTrdModules.size() ; ++i ){

    Detector::TrdModule* module = fTrdModules[i];
    module->UpdateGlobalPositionAndDirection();
  }
}


void Detector::TrdSublayer::Dump() const {

  INFO_OUT << "TRD Sublayer " << fLayerNumber << "," << fSublayerNumber
           << " relative position " << fNominalRelativePosition << " + " << fOffsetRelativePosition << " rotation " << fExtraRelativeRotation << " * " << fNominalRelativeRotation
           << " global position " << GlobalPosition() << " rotation " << GlobalRotation() << std::endl;

  for( unsigned int i = 0 ; i < fTrdModules.size() ; ++i ){

    Detector::TrdModule* module = fTrdModules[i];
    module->Dump();
  }
}
