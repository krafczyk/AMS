#include "ACQtProducer-include.C"


bool ACsoft::ACQtProducer::ProduceEventHeader() {

  static TRandom3* fRandomNumberGenerator = 0;
  if (!fRandomNumberGenerator)
    fRandomNumberGenerator = new TRandom3(0);

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  AC::EventHeader eventHeader;
  eventHeader.fRandom = int(256.0 * fRandomNumberGenerator->Uniform()); // RandomNr from 0..255
  eventHeader.fAMSStatus = fAMSEvent->fStatus;
  eventHeader.fEvent = fAMSEvent->Event();

  eventHeader.fTimeStamp = AC::ConstructTimeStamp(fAMSEvent->UTime(), fAMSEvent->Frac());
  eventHeader.fFlags = AC::EventHeaderNoFlags;

  double correctedUTCTime = 0;
  double correctedUTCTimeError = 0;
  if (!fAMSEvent->UTCTime(correctedUTCTime, correctedUTCTimeError))
    eventHeader.fFlags = AC::EventHeaderUTCTimeFromGPS;

  int nonFracUTCTime = static_cast<int>(correctedUTCTime);
  eventHeader.fUTCTimeStamp = AC::ConstructTimeStamp(nonFracUTCTime, correctedUTCTime - nonFracUTCTime);

  eventHeader.fMagneticLatitude = fAMSEvent->fHeader.ThetaM;
  eventHeader.fMagneticLongitude = fAMSEvent->fHeader.PhiM;
  eventHeader.fISSLatitude = fAMSEvent->fHeader.ThetaS;
  eventHeader.fISSLongitude = fAMSEvent->fHeader.PhiS;
  eventHeader.fISSDistanceFromEarthCenter = fAMSEvent->fHeader.RadS;
  eventHeader.fISSRoll = fAMSEvent->fHeader.Roll;
  eventHeader.fISSPitch = fAMSEvent->fHeader.Pitch;
  eventHeader.fISSYaw = fAMSEvent->fHeader.Yaw;
  eventHeader.fISSVelocityLatitude = fAMSEvent->fHeader.VelTheta;
  eventHeader.fISSVelocityLongitude = fAMSEvent->fHeader.VelPhi;

  fEvent->fEventHeader = eventHeader;
  return true;
}
