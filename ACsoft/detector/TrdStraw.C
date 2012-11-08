#include "TrdStraw.hh"

#include "TrdModule.hh"
#include "dumpstreamers.hh"
#include "AMSGeometry.h"

#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdStraw> "
#include <debugging.hh>
Detector::TrdStraw::TrdStraw( int layerNumber, int sublayerNumber, int ladderNumber, int moduleNumber, int strawNumber,
                              TrdModule* mother ) :
  fLayerNumber(layerNumber),
  fSublayerNumber(sublayerNumber),
  fLadderNumber(ladderNumber),
  fModuleNumber(moduleNumber),
  fStrawNumber(strawNumber),
  fMother(mother)
{

  assert( strawNumber >= 0 && strawNumber < int(AC::AMSGeometry::TRDStrawsPerModule) );

  fGlobalStrawNumber = fModuleNumber*16 + fStrawNumber;

  Double_t x = -0.5*AC::AMSGeometry::TrdModuleWidth;
  x += 0.31 + 0.62 * float(strawNumber);

  // Account TRD longitudinal stringers.
  if(strawNumber >=  1) x += 0.03;
  if(strawNumber >=  4) x += 0.03;
  if(strawNumber >=  7) x += 0.03;
  if(strawNumber >=  9) x += 0.03;
  if(strawNumber >= 12) x += 0.03;
  if(strawNumber >= 15) x += 0.03;

  fNominalRelativePosition = TVector3(x,0.0,0.0);
  fNominalWireDirection    = TVector3(0.0,1.0,0.0);

  // set global values to dummy values, will be set by TrdModule anyway
  fGlobalPosition = TVector3();
  fGlobalWireDirection = NominalWireDirection();
}

/** Call this function when the position and/or orientation of the mother module have been changed. */
void Detector::TrdStraw::UpdateGlobalPositionAndDirection() {

  SetGlobalPosition( fMother->GlobalPosition() + fMother->GlobalRotation()*fNominalRelativePosition );
  SetGlobalWireDirection( fMother->GlobalRotation() * fNominalWireDirection );
}

void Detector::TrdStraw::Dump() const {

  DEBUG_OUT << "TrdStraw " << fStrawNumber << " (global " << fGlobalStrawNumber << ")"
            << " rel pos " << RelativePosition() << " nom wire dir " << NominalWireDirection()
            << " global pos " << GlobalPosition() << " dir " << GlobalWireDirection()
            << std::endl;

  if(DEBUG>=2){
    int direction;
    float xy, z;
    AC::TRDStrawToCoordinates(fGlobalStrawNumber, direction, xy, z);
    TVector3 amsgeoPos(0.,0.,z);
    if( direction == 1 ) amsgeoPos.SetY(xy);
    else amsgeoPos.SetX(xy);
    if( (amsgeoPos-GlobalPosition()).Mag() > 0.01 ) WARN_OUT << "Straw position mismatch in straw " << fGlobalStrawNumber << std::endl;

    DEBUG_OUT_L(3) << "amsgeo: " << amsgeoPos << " my: " << GlobalPosition() << std::endl;
   }

}
