#include "TrdMissingHit.hh"

#include "dumpstreamers.hh"
#include "AMSGeometry.h"

#include <TEllipse.h>

#include <iomanip>

#define DEBUG 0
#define INFO_OUT_TAG "TrdMissingHit> "
#include <debugging.hh>

ACsoft::Analysis::TrdMissingHit::TrdMissingHit( int globalSublayer, TVector3 pos, TVector3 dir, double gap, ACsoft::AC::MeasurementMode orientation ) :
  fGlobalSublayerNumber(globalSublayer),
  fPosition(pos),
  fDirection(dir),
  fGapWidth(gap),
  fOrientation(orientation),
  fEllipse(0)
{
}

/** Draw a small open circle at the position of the missing hit in the current canvas.
  *
  * \param rotatedSystem Draw all %TRD information in a system rotated by 90 deg for better screen occupation?
  */
void ACsoft::Analysis::TrdMissingHit::Draw( bool rotatedSystem ) const {

  if(fEllipse) delete fEllipse;

  float xy = fOrientation == ACsoft::AC::XZMeasurement ? Position().X() : Position().Y();

  float xPad = rotatedSystem ? -Position().Z() : xy;
  float yPad = rotatedSystem ? xy : Position().Z();
  fEllipse = new TEllipse(xPad,yPad,0.8*ACsoft::AC::AMSGeometry::TRDTubeRadius);
  fEllipse->SetLineColor(2);
  fEllipse->Draw();

}

/** Dump to console. */
void ACsoft::Analysis::TrdMissingHit::Dump() const {

  std::cout << "Missing hit sl " << std::setw(3) << fGlobalSublayerNumber << " pos " << fPosition << " dir " << fDirection
            << std::setw(12) << " gap " << fGapWidth << std::endl;

}

