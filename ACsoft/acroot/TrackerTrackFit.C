#include "TrackerTrackFit.h"

namespace AC {

BEGIN_DEBUG_TABLE(TrackerTrackFit)
  COL( "Algo",             Int_t,   Algorithm)
  COL( "Patt",             Int_t,   Pattern)
  COL( "Refit",            Int_t,   Refit)
  COL( "Rigi [GV]",        Float_t, Rigidity)
  COL( "InvRigErr [1/GV]", Float_t, InverseRigidityError)
  COL( "Chi2NormX  ",      Float_t, ChiSquareNormalizedX)
  COL( "Chi2NormY  ",      Float_t, ChiSquareNormalizedY)
  COL( "L1Tht [deg]",      Float_t, ThetaLayer1)
  COL( "L1Phi [deg]",      Float_t, PhiLayer1)
  COL( "L1X [cm] ",        Float_t, XLayer1)
  COL( "L1Y [cm] ",        Float_t, YLayer1)
  NEWTABLE
  COL( "TRDUX [cm]",       Float_t, XTRDUpper)
  COL( "TRDUY [cm]",       Float_t, YTRDUpper)
  COL( "TRDCX [cm]",       Float_t, XTRDCenter)
  COL( "TRDCY [cm]",       Float_t, YTRDCenter)
  COL( "TRDLX [cm]",       Float_t, XTRDLower)
  COL( "TRDLY [cm]",       Float_t, YTRDLower)
  COL( "TOFUTht [deg]",    Float_t, ThetaTOFUpper)
  COL( "TOFUPhi [deg]",    Float_t, PhiTOFUpper)
  COL( "TOFUX [cm]",       Float_t, XTOFUpper)
  NEWTABLE
  COL( "TOFUY [cm]",       Float_t, YTOFUpper)
  COL( "RICHTht [deg]",    Float_t, ThetaRICH)
  COL( "RICHPhi [deg]",    Float_t, PhiRICH)
  COL( "RICHX [cm]",       Float_t, XRICH)
  COL( "RICHY [cm]",       Float_t, YRICH)
  COL( "L9Tht [deg]",      Float_t, ThetaLayer9)
  COL( "L9Phi [deg]",      Float_t, PhiLayer9)
  COL( "L9X [cm] ",        Float_t, XLayer9)
  COL( "L9Y [cm] ",        Float_t, YLayer9)
END_DEBUG_TABLE

/** Writes this object into a QDataStream, which is used to produce ACQt files */
QDataStream& operator<<(QDataStream& stream, const TrackerTrackFit& object) {

  stream << object.fParameters << object.fRigidity << object.fInverseRigidityError << object.fChiSquareNormalizedX << object.fChiSquareNormalizedY
         << object.fThetaLayer1 << object.fPhiLayer1 << object.fXLayer1 << object.fYLayer1 << object.fXTRDUpper
         << object.fYTRDUpper << object.fXTRDCenter << object.fYTRDCenter << object.fXTRDLower << object.fYTRDLower
         << object.fThetaTOFUpper << object.fPhiTOFUpper << object.fXTOFUpper << object.fYTOFUpper << object.fThetaRICH
         << object.fPhiRICH << object.fXRICH << object.fYRICH << object.fThetaLayer9 << object.fPhiLayer9
         << object.fXLayer9 << object.fYLayer9;
  return stream;
}

/** Reads this object from a QDataStream, which is used to construct AC objects from ACQt files */
QDataStream& operator>>(QDataStream& stream, TrackerTrackFit& object) {

  stream >> object.fParameters;

  FloatArrayStream<26> floatStream(stream);
  object.fRigidity = floatStream.read();
  object.fInverseRigidityError = floatStream.read();
  object.fChiSquareNormalizedX = floatStream.read();
  object.fChiSquareNormalizedY = floatStream.read();
  object.fThetaLayer1 = floatStream.read();
  object.fPhiLayer1 = floatStream.read();
  object.fXLayer1 = floatStream.read();
  object.fYLayer1 = floatStream.read();
  object.fXTRDUpper = floatStream.read();
  object.fYTRDUpper = floatStream.read();
  object.fXTRDCenter = floatStream.read();
  object.fYTRDCenter = floatStream.read();
  object.fXTRDLower = floatStream.read();
  object.fYTRDLower = floatStream.read();
  object.fThetaTOFUpper = floatStream.read();
  object.fPhiTOFUpper = floatStream.read();
  object.fXTOFUpper = floatStream.read();
  object.fYTOFUpper = floatStream.read();
  object.fThetaRICH = floatStream.read();
  object.fPhiRICH = floatStream.read();
  object.fXRICH = floatStream.read();
  object.fYRICH = floatStream.read();
  object.fThetaLayer9 = floatStream.read();
  object.fPhiLayer9 = floatStream.read();
  object.fXLayer9 = floatStream.read();
  object.fYLayer9 = floatStream.read();

  return stream;
}

}
