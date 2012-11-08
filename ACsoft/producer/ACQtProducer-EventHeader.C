#include "ACQtProducer-include.C"

bool ACQtProducer::ProduceEventHeader() {

  static TRandom3* fRandomNumberGenerator = 0;
  if (!fRandomNumberGenerator)
    fRandomNumberGenerator = new TRandom3(0);

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  AC::EventHeader eventHeader;
  eventHeader.fRandom = int(256.0 * fRandomNumberGenerator->Uniform()); // RandomNr from 0..255
  eventHeader.fStatus = fAMSEvent->fStatus;
  eventHeader.fEvent = fAMSEvent->Event();
  eventHeader.fTimeStamp = AC::ConstructTimeStamp(fAMSEvent->UTime(), fAMSEvent->Frac());

  // FIXME: Enable star tracker data, once we switch to a newer AMS patch level for production!
#if 0
  int result = 0;
  double glong = 0, glat = 0;
  fAMSEvent->GetGalCoo(result, glong, glat);
  if (!result) {
    eventHeader.fGalacticLatitude = glat;
    eventHeader.fGalacticLongitude = glong;
  }
#endif

  eventHeader.fMagneticLatitude = fAMSEvent->fHeader.ThetaM;
  eventHeader.fMagneticLongitude = fAMSEvent->fHeader.PhiM;
  eventHeader.fISSLatitude = fAMSEvent->fHeader.ThetaS;
  eventHeader.fISSLongitude = fAMSEvent->fHeader.PhiS;
  eventHeader.fISSAltitude = fAMSEvent->fHeader.RadS;
  eventHeader.fISSRoll = fAMSEvent->fHeader.Roll;
  eventHeader.fISSPitch = fAMSEvent->fHeader.Pitch;
  eventHeader.fISSYaw = fAMSEvent->fHeader.Yaw;
  eventHeader.fISSVelocityLatitude = fAMSEvent->fHeader.VelTheta;
  eventHeader.fISSVelocityLongitude = fAMSEvent->fHeader.VelPhi;

  fEvent->fEventHeader = eventHeader;
  return true;
}
