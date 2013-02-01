#ifndef RTICUTS_HH
#define RTICUTS_HH

#include "Cut.hh"
#include "RTIData.hh"
#include "RTIReader.hh"

#ifndef __CINT__
#include "Event.h"
#include "AnalysisParticle.hh"
#endif

namespace Cuts {

/** check if RTI data is available  (RunNumber > 0) */
class CutRTIdataAvailable : public Cut {
public:
  CutRTIdataAvailable() : Cut("check if rti info was available in this second.") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return data->GetRun() > 0;
  }

  ClassDef(Cuts::CutRTIdataAvailable, 1)
};

/** Cut to check if most events triggered (ntrigger/nevents > 0.98 ) */
class CutMostEventsTriggered : public Cut {
public:
  CutMostEventsTriggered() : Cut("test if most of the events have been triggered") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return (float)data->GetLevel1Trigger()/(float)data->GetEvents()  > 0.98;
  }

  ClassDef(Cuts::CutMostEventsTriggered, 1)
};

/** Cut to check if the unixsecond is within the Run time (time >= run) */
class CutSecondWithinRun : public Cut {
public:
  CutSecondWithinRun() : Cut("test if the unix second is within the runID") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return data->GetTime() >= data->GetRun();
  }

  ClassDef(Cuts::CutSecondWithinRun, 1)
};

 /** Cut out times where the reconstruction efficiency is poor ( nParticles/nTrigger > 0.07/1600 * nTrigger ) */
class CutBadReconstructionPeriod : public Cut {
public:
  CutBadReconstructionPeriod() : Cut("cut perdiods of bad reconstruction efficiency") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return (float)data->GetParticles()/(float)data->GetLevel1Trigger() > 0.07/1600. * data->GetLevel1Trigger();
  }

  ClassDef(Cuts::CutBadReconstructionPeriod, 1)
};

 /** Cut on the facing angle of AMS to reject periods in wich the ISS was rotated. ( zenith < 40 ) */
class CutBadFacingAngle : public Cut {
public:
  CutBadFacingAngle() : Cut("cut perdiods with rotated ISS") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return data->GetZenith() < 40;
  }

  ClassDef(Cuts::CutBadFacingAngle, 1)
};

/** standard livetime > 0.5 cut */
class CutBadLiveTime : public Cut {
public:
  CutBadLiveTime() : Cut("cut away periods with livetime < 0.5") { }

  virtual bool TestCondition(const ACsoft::Analysis::Particle& p) {

    const ACsoft::RTI::RTIData* data = ACsoft::RTI::RTIReader::Instance()->GetRTIData(p);
    if (!data)
      return true;
    return data->GetLiveTime() > 0.5;
  }

  ClassDef(Cuts::CutBadLiveTime, 1)
};

}

#endif // RTICUTS_HH
