#include "DetectorManager.hh"

#include "TrdDetector.hh"
#include "TrdModule.hh"
#include "TrdAlignment.hh"
#include "AMSGeometry.h"

#define DEBUG 0
#define INFO_OUT_TAG "DetectorManager> "
#include <debugging.hh>

ACsoft::Detector::DetectorManager::DetectorManager() :
  fTrdNoAlignment(0),
  fTrdAligned(0),
  fAlignmentMethod(OnlyShimming),
  fRefreshInterval(1.0),
  fLastUpdateTime(TTimeStamp(0,0))
{
}

void ACsoft::Detector::DetectorManager::UpdateIfNeeded( const TTimeStamp& time ) {

  double deltaT = fabs( double(time) - double(fLastUpdateTime) );
  if( deltaT > fRefreshInterval ){

    DEBUG_OUT << "Updating alignment at time " << time << " with method " << fAlignmentMethod << std::endl;

    switch(fAlignmentMethod){

    case OnlyShimming:
      // do nothing
      break;

    case EffectiveModuleShifts:
      UpdateFromEffectiveModuleShifts(time);
      break;

    default:
      assert(false);
      break;
    }

    fLastUpdateTime = time;
  }
}


void ACsoft::Detector::DetectorManager::UpdateFromEffectiveModuleShifts( const TTimeStamp& time ) {

  TRotation identity;
  identity.SetToIdentity();

  for( unsigned int Mod = 0 ; Mod < AC::AMSGeometry::TRDModules ; ++Mod ) {

    Double_t shift = Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(Mod,double(time));
    DEBUG_OUT << "Mod " << Mod << ": applying shift " << shift << std::endl;

    GetAlignedTrd()->GetTrdModule(Mod)->ChangeRelativePositionAndRotation(TVector3(-shift,0.0,0.0),identity);
  }
}


ACsoft::Detector::Trd* ACsoft::Detector::DetectorManager::GetAlignedTrd() {

  if( !fTrdAligned ) fTrdAligned = new ACsoft::Detector::Trd;

  return fTrdAligned;
}


ACsoft::Detector::Trd* ACsoft::Detector::DetectorManager::GetUnalignedTrd() {

  if( !fTrdNoAlignment ) fTrdNoAlignment = new ACsoft::Detector::Trd;

  return fTrdNoAlignment;
}
