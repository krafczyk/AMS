#include "TrdStraw.hh"

#include "dumpstreamers.hh"
#include "AMSGeometry.h"
#include "TrdModule.hh"

#include <TEllipse.h>

#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "TrdStraw> "
#include <debugging.hh>

ACsoft::Detector::TrdStraw::TrdStraw( int layerNumber, int sublayerNumber, int globalSublayerNumber, int ladderNumber, int towerNumber, int moduleNumber, int strawNumber,
                              TrdModule* mother ) :
  fLayerNumber(layerNumber),
  fSublayerNumber(sublayerNumber),
  fGlobalSublayerNumber(globalSublayerNumber),
  fLadderNumber(ladderNumber),
  fTowerNumber(towerNumber),
  fModuleNumber(moduleNumber),
  fStrawNumber(strawNumber),
  fMother(mother),
  fEllipse(0)
{

  assert( strawNumber >= 0 && strawNumber < int(ACsoft::AC::AMSGeometry::TRDStrawsPerModule) );

  fDirection = mother->Direction();

  fGlobalStrawNumber = fModuleNumber*16 + fStrawNumber;

  Double_t x = -0.5*ACsoft::AC::AMSGeometry::TrdModuleWidth;
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
void ACsoft::Detector::TrdStraw::UpdateGlobalPositionAndDirection() {

  SetGlobalPosition( fMother->GlobalPosition() + fMother->GlobalRotation()*fNominalRelativePosition );
  SetGlobalWireDirection( fMother->GlobalRotation() * fNominalWireDirection );
}

/** Draw straw into current canvas.
 *
 * \param rotatedSystem Draw all %TRD information in a system rotated by 90 deg for better screen occupation?
 */
void ACsoft::Detector::TrdStraw::Draw( bool rotatedSystem ) {

  DEBUG_OUT << std::endl;

  if(fEllipse) delete fEllipse;

  float xy = fDirection == ACsoft::AC::XZMeasurement ? GlobalPosition().X() : GlobalPosition().Y();

  float xPad = rotatedSystem ? -GlobalPosition().Z() : xy;
  float yPad = rotatedSystem ? xy : GlobalPosition().Z();
  fEllipse = new TEllipse(xPad,yPad,ACsoft::AC::AMSGeometry::TRDTubeRadius);
  fEllipse->Draw();
}

void ACsoft::Detector::TrdStraw::Dump() const {

  INFO_OUT << "TrdStraw " << fStrawNumber << " (global " << fGlobalStrawNumber << ")"
           << " sl " << fSublayerNumber << " gsl " << fGlobalSublayerNumber
           << " rel pos " << RelativePosition() << " nom wire dir " << NominalWireDirection()
           << " global pos " << GlobalPosition() << " dir " << GlobalWireDirection()
           << std::endl;

  if(DEBUG>=2){
    int direction;
    float xy, z;

    ACsoft::AC::TrdStrawToRawCoordinates(fGlobalStrawNumber, direction, xy, z);
    TVector3 amsgeoPos(0.,0.,z);
    if( direction == 1 ) amsgeoPos.SetY(xy);
    else amsgeoPos.SetX(xy);
    if( (amsgeoPos-GlobalPosition()).Mag() > 0.01 ) WARN_OUT << "Straw position mismatch in straw " << fGlobalStrawNumber << std::endl;

    DEBUG_OUT_L(3) << "amsgeo: " << amsgeoPos << " my: " << GlobalPosition() << std::endl;
   }

}

