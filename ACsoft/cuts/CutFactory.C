#include "CutFactory.hh"

#include "ECALCuts.hh"
#include "ParticleSelectionCuts.hh"
#include "PreselectionCuts.hh"
#include "TOFCuts.hh"
#include "TRDCuts.hh"
#include "TrackerCuts.hh"
#include "TriggerCuts.hh"
#include "RICHCuts.hh"
#include "GroupACuts.hh"
#include "BadRunCuts.hh"
#include "RTICuts.hh"

#include "BadRunManager.hh"

#include <string>
#include <stdexcept>

#include <QStringList>
#include <QString>
#include <QRegExp>

#define DEBUG 0
#include <debugging.hh>

static double convertStringToDouble( QString string ) {
  DEBUG_OUT << "\"" << qPrintable(string) << "\"" << std::endl;
  bool ok = false;
  double value = string.toDouble(&ok);
  assert(ok);
  return value;
}

Cuts::Cut* Cuts::CutFactory::CreateCut( std::string stringToParse ) const {

  DEBUG_OUT << stringToParse << std::endl;

  QString cutString = QString::fromStdString(stringToParse);
  cutString.replace(QRegExp(" +"), "");
  QStringList cutStringList = cutString.split(QRegExp("\\(|\\||\\)"), QString::SkipEmptyParts);

  assert(cutStringList.size() != 0);
  DEBUG_OUT << cutStringList.size() << std::endl;
  DEBUG_OUT << qPrintable(cutStringList.join(QString("-"))) << std::endl;
  QString cut = cutStringList.at(0);
  QString arg1 = cutStringList.size() >= 2 ? cutStringList.at(1) : "";
  QString arg2 = cutStringList.size() >= 3 ? cutStringList.at(2) : "";

  if (cut == "TrackerTrackInEcalAcceptance") {
    return new Cuts::CutTrackerTrackInEcalAcceptance;
  }
  else if (cut == "EcalShowerInFiducialVolume") {
    return new Cuts::CutEcalShowerInFiducialVolume;
  }
  else if (cut == "EcalEnergy") {
    return new Cuts::CutEcalEnergy(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "EcalBdtEstimator") {
    return new Cuts::CutEcalBdtEstimator(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "EcalTrackerTrackMatch") {
    return new Cuts::CutEcalTrackerTrackMatch(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "RemoveAntiNuclei") {
    return new Cuts::CutRemoveAntiNuclei;
  }
  else if (cut == "RemoveHeavyNuclei") {
    return new Cuts::CutRemoveHeavyNuclei;
  }
  else if (cut == "RemovePositrons") {
    return new Cuts::CutRemovePositrons;
  }
  else if (cut == "RemoveAntiProtons") {
    return new Cuts::CutRemoveAntiProtons;
  }
  else if (cut == "ReduceMisidentifiedHelium") {
    return new Cuts::CutReduceMisidentifiedHelium;
  }
  else if (cut == "EnergyOverRigidity") {
    return new Cuts::CutEnergyOverRigidity(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "ElectronCandidate") {
    return new Cuts::CutElectronCandidate;
  }
  else if (cut == "ProtonCandidate") {
    return new Cuts::CutProtonCandidate;
  }
  else if (cut == "SingleParticle") {
    return new Cuts::CutSingleParticle;
  }
  else if (cut == "AtLeastOneParticle") {
    return new Cuts::CutAtLeastOneParticle;
  }
  else if (cut == "SingleTrackerTrack") {
    return new Cuts::CutSingleTrackerTrack;
  }
  else if (cut == "HasEcalShower") {
    return new Cuts::CutHasEcalShower;
  }
  else if (cut == "HasTrackerTrack") {
    return new Cuts::CutHasTrackerTrack;
  }
  else if (cut == "HasTrackerTrackFit") {
    return new Cuts::CutHasTrackerTrackFit;
  }
  else if (cut == "HasTofBeta") {
    return new Cuts::CutHasTofBeta;
  }
  else if (cut == "HasTRDVTrack") {
    return new Cuts::CutHasTRDVTrack;
  }
  else if (cut == "IsDowngoing") {
    return new Cuts::CutIsDowngoing;
  }
  else if (cut == "EventConsistency") {
    return new Cuts::CutEventConsistency;
  }
  else if (cut == "TofCharge") {
    return new Cuts::CutTofCharge(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TofChargeAvailable") {
    return new Cuts::CutTofChargeAvailable;
  }
  else if (cut == "TofMaxEnergy") {
    return new Cuts::CutTofMaxEnergy(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TofMeanEnergy") {
    return new Cuts::CutTofMeanEnergy(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TofTotalMaxEnergy") {
    return new Cuts::CutTofTotalMaxEnergy(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrdNumberOfRawHits") {
    return new Cuts::CutTrdNumberOfRawHits(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrackerChargeAvailable") {
    return new Cuts::CutTrackerChargeAvailable;
  }
  else if (cut == "TrackerCharge") {
    return new Cuts::CutTrackerCharge(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrackerLayerOneOrNine") {
    return new Cuts::CutTrackerLayerOneOrNine;
  }
  else if (cut == "IsGoodInnerTrackerTrack") {
    return new Cuts::CutIsGoodInnerTrackerTrack;
  }
  else if (cut == "IsGoodCentralInnerTrackerTrack") {
    return new Cuts::CutIsGoodCentralInnerTrackerTrack;
  }
  else if (cut == "TrackerTrackGoodnessOfFit") {
    return new Cuts::CutTrackerTrackGoodnessOfFit(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrackerTrackHasAtLeastThreeXHits") {
    return new Cuts::CutTrackerTrackHasAtLeastThreeXHits;
  }
  else if (cut == "TrackerTrackFitRigidity") {
    return new Cuts::CutTrackerTrackFitRigidity(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrackerTrackFitAbsoluteRigidity") {
    return new Cuts::CutTrackerTrackFitAbsoluteRigidity(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "TrackerTrackReliableRigidity") {
    return new Cuts::CutTrackerTrackReliableRigidity(convertStringToDouble(arg1));
  }
  else if (cut == "TriggerInformationAvailable") {
    return new Cuts::CutTriggerInformationAvailable;
  }
  else if (cut == "TriggerLiveTime") {
    return new Cuts::CutTriggerLiveTime;
  }
  else if (cut == "TriggerRate") {
    return new Cuts::CutTriggerRate;
  }
  else if (cut == "RichRingHits") {
    return new Cuts::CutRichRingHits(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "RichRingBeta") {
    return new Cuts::CutRichRingBeta(convertStringToDouble(arg1), convertStringToDouble(arg2));
  }
  else if (cut == "BadRun") {
    ACsoft::Analysis::BadRunManager::SubD type = ACsoft::Analysis::BadRunManager::Self()->StringToSubd(arg1.toStdString());
    return new Cuts::CutBadRun(type);
  }
  else if (cut == "EventErrors") {
    return new Cuts::CutEventErrors;
  }
  else if (cut == "ScienceRunTag") {
    return new Cuts::CutScienceRunTag;
  }
  else if (cut == "GoodHW") {
    return new Cuts::CutGoodHW;
  }
  else if (cut == "IsNotInSolarArrayShadow") {
    return new Cuts::CutIsNotInSolarArrayShadow;
  }
  else if (cut == "InEcalAcceptance") {
    return new Cuts::CutInEcalAcceptance;
  }
  else if (cut == "InTrdAcceptance") {
    return new Cuts::CutInTrdAcceptance;
  }
  else if (cut == "TrackerEcalMatching") {
    return new Cuts::CutTrackerEcalMatching;
  }
  else if (cut == "TofTrackerMatching") {
    return new Cuts::CutTofTrackerMatching;
  }
  else if (cut == "RigidityAboveGeomagneticCutoff") {
    return new Cuts::CutRigidityAboveGeomagneticCutoff;
  }
  else if (cut == "RTIdataAvailable" ) {
    return new Cuts::CutRTIdataAvailable;
  }
  else if (cut == "MostEventsTriggered") {
    return new Cuts::CutMostEventsTriggered;
  }
  else if (cut == "SecondWithinRun") {
    return new Cuts::CutSecondWithinRun;
  }
  else if (cut == "BadReconstructionPeriod"){
    return new Cuts::CutBadReconstructionPeriod;
  }
  else if (cut == "BadFacingAngle"){
    return new Cuts::CutBadFacingAngle;
  }
  else if (cut == "BadLiveTime"){
    return new Cuts::CutBadLiveTime;
  }
  else {
    WARN_OUT << "Cut \"" << qPrintable(cut) << "\" unknown." << std::endl;
    throw std::runtime_error("Unknown cut.");
  }

  return 0;
}
