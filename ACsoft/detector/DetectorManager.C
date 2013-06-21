#include "DetectorManager.hh"

#include "TrdDetector.hh"
#include "TrdModule.hh"
#include "TrdAlignment.hh"
#include "TrdGainCalibration.hh"
#include "TrdPdfLookup.hh"
#include "AMSGeometry.h"

#define DEBUG 0
#define INFO_OUT_TAG "DetectorManager> "
#include <debugging.hh>

ACsoft::Detector::DetectorManager::DetectorManager() :
  fTrdNoAlignment(0),
  fTrdOnlyShimming(0),
  fTrdAligned(0),
  fAlignmentMethod(EffectiveModuleShifts),
  fUpdateGain(true),
  fRefreshInterval(1.0),
  fCurrentTime(TTimeStamp(0,0)),
  fLastUpdateTime(TTimeStamp(0,0)),
  fCurrentRunType(ACsoft::AC::ISSRun),
  fLastRunType(ACsoft::AC::ISSRun),
  fEnableUpdateWarnings(true),
  fAlignmentOkForLastUpdate(false),
  fGainOkForLastUpdate(false)
{
}

void ACsoft::Detector::DetectorManager::UpdateIfNeeded( const TTimeStamp& time, ACsoft::AC::RunTypeIdentifier runtype ) {

  fCurrentTime = time;
  fCurrentRunType = runtype;

  double deltaT = fabs( double(time) - double(fLastUpdateTime) );
  if( deltaT > fRefreshInterval || runtype != fLastRunType )
    Update(time,runtype);

}

void ACsoft::Detector::DetectorManager::Update( const TTimeStamp& time, ACsoft::AC::RunTypeIdentifier runtype ) {

  DEBUG_OUT << "Updating at time " << time << " for runtype " << ACsoft::AC::RunHeader::RunTypeAsString(runtype) << " (" << runtype << ")" << std::endl;

  //
  // alignment
  //
  if( runtype != ACsoft::AC::MCRun ) {

    switch(fAlignmentMethod){

    case None:
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
  }


  //
  // gain
  //
  if( fUpdateGain )
    UpdateGainValues(time,runtype);

  // bookkeeping
  fLastUpdateTime = time;
  fLastRunType = runtype;
}

void ACsoft::Detector::DetectorManager::UpdateFromEffectiveModuleShifts( const TTimeStamp& time ) {

  if(!fTrdAligned)
    return;

  DEBUG_OUT << "Updating alignment at time " << time << " using effective module shifts" << std::endl;

  fEnableUpdateWarnings = false;

  TRotation identity;
  identity.SetToIdentity();

  fAlignmentOkForLastUpdate = true;

  for( unsigned int Mod = 0 ; Mod < ACsoft::AC::AMSGeometry::TRDModules ; ++Mod ) {

    Double_t shift = Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(Mod,double(time));
    bool shiftOk = Calibration::TrdAlignmentShiftLookup::Self()->LastQueryOk();
    fAlignmentOkForLastUpdate &= shiftOk;
    DEBUG_OUT_L(2) << "Mod " << Mod << ": applying shift " << shift << std::endl;

    fTrdAligned->GetTrdModule(Mod)->ChangeRelativePositionAndRotation(TVector3(-shift,0.0,0.0),identity,shiftOk);
  }

  fEnableUpdateWarnings = true;
}


void ACsoft::Detector::DetectorManager::UpdateGainValues( const TTimeStamp& time, ACsoft::AC::RunTypeIdentifier runtype ) {

  if(fCurrentTime==TTimeStamp(0,0)) return;

  DEBUG_OUT << "Updating gain values at time " << time << " for run type " << ACsoft::AC::RunHeader::RunTypeAsString(runtype) << std::endl;

  fGainOkForLastUpdate = true;

  for( unsigned int Mod = 0 ; Mod < ACsoft::AC::AMSGeometry::TRDModules ; ++Mod ) {

    bool gainOk = true;

    // FIXME move this hardcoded number to a dedicated config file
    // (which was, after all, the whole purpose of the current restructuring effort, but which will now be easy...)
    Double_t lookupGain = 83.5; // MC value
    if( runtype != ACsoft::AC::MCRun ){
      lookupGain = Calibration::TrdGainParametersLookup::Self()->GetGainValue(Mod,time.AsDouble());
      gainOk = Calibration::TrdGainParametersLookup::Self()->LastQueryOk();
      fGainOkForLastUpdate &= gainOk;
    }

    if( fTrdAligned )
      fTrdAligned->GetTrdModule(Mod)->SetCurrentGainValue(lookupGain,gainOk);

    if( fTrdOnlyShimming )
      fTrdOnlyShimming->GetTrdModule(Mod)->SetCurrentGainValue(lookupGain,gainOk);

    if( fTrdNoAlignment )
      fTrdNoAlignment->GetTrdModule(Mod)->SetCurrentGainValue(lookupGain,gainOk);
  }
}

ACsoft::Detector::Trd* ACsoft::Detector::DetectorManager::GetAlignedTrd() {

  if( !fTrdAligned ){
    bool applyShimming = ( fAlignmentMethod != None );
    fTrdAligned = new ACsoft::Detector::Trd(applyShimming);
    Update(fCurrentTime,fCurrentRunType);

    // return here to avoid false-positive warning
    return fTrdAligned;
  }

  if( fAlignmentMethod != None && fAlignmentMethod != OnlyShimming &&
      fEnableUpdateWarnings && fLastUpdateTime == TTimeStamp(0,0) ){
    WARN_OUT << "TRD geometry structure has never been updated!" << std::endl;
  }

  return fTrdAligned;
}


ACsoft::Detector::Trd* ACsoft::Detector::DetectorManager::GetShimmedTrd() {

  if( !fTrdOnlyShimming ){
    fTrdOnlyShimming = new ACsoft::Detector::Trd(true);
    Update(fCurrentTime,fCurrentRunType);
  }

  return fTrdOnlyShimming;
}


ACsoft::Detector::Trd* ACsoft::Detector::DetectorManager::GetUnalignedTrd() {

  if( !fTrdNoAlignment ){
    fTrdNoAlignment = new ACsoft::Detector::Trd(false);
    Update(fCurrentTime,fCurrentRunType);
  }

  return fTrdNoAlignment;
}


/** Check if TRD calibration data is available at the current internal time of the DetectorManager. */
bool ACsoft::Detector::DetectorManager::IsCompleteTrdCalibrationAvailable() const {
  return IsCompleteTrdCalibrationAvailable(fCurrentTime);
}

/** Check if TRD calibration data is available at the given \p time.
  *
  * This will check if:
  *  - slow control data is available
  *  - alignment parameters for the entire TRD hierarchy are available
  *  - gain calibration constants for all TRD modules are available
  *
  */
bool ACsoft::Detector::DetectorManager::IsCompleteTrdCalibrationAvailable( double time ) const {

  bool allOK = true;

  for( unsigned int Mod = 0 ; Mod < ACsoft::AC::AMSGeometry::TRDModules ; ++Mod ) {

    Calibration::TrdAlignmentShiftLookup::Self()->GetAlignmentShift(Mod,time);
    allOK &= Calibration::TrdAlignmentShiftLookup::Self()->LastQueryOk();
    if(!allOK) return false;

    Calibration::TrdGainParametersLookup::Self()->GetGainValue(Mod,time);
    allOK &= Calibration::TrdGainParametersLookup::Self()->LastQueryOk();
    if(!allOK) return false;
  }

  bool slowControlOk;
  ACsoft::Utilities::TrdPdfLookup::QueryXenonPressure(time,slowControlOk);

  allOK &= slowControlOk;

  return allOK;
}


/** Check if TRD calibration data is available at the given \p time. */
bool ACsoft::Detector::DetectorManager::IsCompleteTrdCalibrationAvailable( const TTimeStamp& time ) const {
  return IsCompleteTrdCalibrationAvailable(time.AsDouble());
}

