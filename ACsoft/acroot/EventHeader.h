#ifndef EventHeader_h
#define EventHeader_h

#include "Tools.h"
#include "EventHeader-Streamer.h"

namespace ACsoft {

namespace AC {

/** %Event header data.
  *
  * \todo add units to documentation
  */
class EventHeader {
  WTF_MAKE_FAST_ALLOCATED;
public:
  AC_EventHeader_Variables

  /** Helper method dumping an EventHeader object to the console
    */
  void Dump() const;

  /** Clear event to the initial empty state, as it was just constructed.
    */
  void Clear();

  /** Returns %Event status information
    * Event Status
    * bits:
    *  - 0- 1 nParticle()
    *  -    2 ParticleR::iTrdTrack() !=-1
    *  -    3 ParticleR::iBeta() !=-1
    *  -    4 ParticleR::iTrTrack() !=-1
    *  -    5 ParticleR::iRichRing() !=-1
    *  -    6 ParticleR::iEcalShower() !=-1
    *  -    7 ParticleR::iVertex() !=-1
    *  - 8+ 9 nTrdTrack()
    *  - 10-12 nTofCluster()
    *  - 13+14 nTrTrack()
    *  - 15+16 nRichRing()
    *  - 17+18 nEcalShower()
    *  - 19+20 nVertex()
    *  - 21+22 nAntiCluster()
    *  - 23-25 ParticleR::Charge
    *  - 26+27 z=1 lvl1 number_of_tof_planes-1
    *  - 28+29 z>1 lvl1 number_of_tof_planes-1
    *  -  30 event has errors
    *  -  31 status not found
    *  -  32 not prescaled event, prescaled event
    *  -  33 velocity <0; >0
    *  -  34 momentum <0; >0
    *  - 35+36 rig                 <8; <32; <128; >128 GV
    *  - 37+38 ecalenergy:         <2;  <8;  <32; >32 gev
    *  - 39+40 external planes:  none;   1;    9;  1&9
    *  - 41+42 trigger rate <500; <1000 ; <2000; >2000 Hz
    *  - 43+44 trd hmult <1; <2; <4; >4
    *  - 45+46 geomagnetic latitude (not yet defined,only reserved)
    *    -     0: < 10 degrees (cos < 0.1736)
    *    -     1: < 30 degrees (cos < 0.5   )
    *    -     2: < 50 degrees (cos < 0.766 )
    *    -     3: > 50 degrees (cos > 0.766 )
    *  - 47-51 rich charge *4
    *  -    52 rich-radiator: agl; naf
    *  - 53+54 beta pattern   tof hits <2; <3; <4; 4
    *  - 55+56 mass/charge  <0.5; <1.5; <3.0; >3.0
    *  - 57-59 track hit    <4   <5  <6 <7 <8 <9     ??
    *  - 60-63 trd charge *2
    *
    * \todo Verify documentation.
    */
  qint64 Status() const { return fStatus; }

  /** Random number (0..255).
    *
    * This can be used to split the events into separate independent samples, e.g. for verification of the calibration.
    */
  UChar_t Random() const { return fRandom; }

  /** MC random seed 1.
    */
  Int_t MCRandomSeed1() const { return fMCRandomSeed1; }

  /** MC random seed 2.
    */
  Int_t MCRandomSeed2() const { return fMCRandomSeed2; }

  /** %Event number.
    */
  Int_t Event() const { return fEvent; }

  /** %Event time stamp
    */
  const TTimeStamp& TimeStamp() const { return fTimeStamp; }

  /** %Event UTC time stamp
    * \todo Add documentation.
    */
  Double_t UTCTime() const { return fUTCTime; }

  /** ISS Magnetic Latitude [deg].
    */
  Float_t MagneticLatitude() const { return fMagneticLatitude * TMath::RadToDeg(); }

  /** ISS Magnetic Longitude [deg].
    */
  Float_t MagneticLongitude() const { return fMagneticLongitude * TMath::RadToDeg(); }

  /** ISS Latitude [deg].
    */
  Float_t ISSLatitude() const { return fISSLatitude * TMath::RadToDeg(); }

  /** ISS Longitude [deg].
    */
  Float_t ISSLongitude() const { return fISSLongitude * TMath::RadToDeg(); }

  /** ISS Distance From Earth's Center [km].
    *
    * The Distence between the ISS and the Earth's Center under the assumption of an perfectly spherical Earth
    * with a radius of 6378.137 km.
    */
  Float_t ISSDistanceFromEarthCenter() const { return fISSDistanceFromEarthCenter / 100000; }

  /** ISS Altitude above the ground [km].
    *
    * Note: The ISS coordinate system is GTOD (Greenwhich True Of Date).
    * To determine the position above the ground, the equatorial radius of
    * 6378137m (semi-major axis length of the GRS80 GPS coordinate system)
    * is substracted from the original AMS data.
    *
    * For more details, see: http://www.asi.org/adb/04/02/00/iss-coordinate-systems.pdf
    */

  Float_t ISSAltitude() const { return ((fISSDistanceFromEarthCenter / 100) - 6378137) / 1000; }

  /** Distance ISS - center of earth's magnetic dipole [km].
    *
    * coordinates of the dipole's center are obtained from IGRF 2000.
    */
  float ISSDistanceFromDipoleCenter() const;

  /** ISS Roll [deg].
    */
  Float_t ISSRoll() const { return fISSRoll * TMath::RadToDeg(); }

  /** ISS Pitch [deg].
    */
  Float_t ISSPitch() const { return fISSPitch * TMath::RadToDeg(); }

  /** ISS Yaw [deg].
    */
  Float_t ISSYaw() const { return fISSYaw  * TMath::RadToDeg(); }

  /** ISS Velocity Latitude [deg].
    */
  Float_t ISSVelocityLatitude() const { return fISSVelocityLatitude * TMath::RadToDeg(); }

  /** ISS Velocity Longitude [deg].
    */
  Float_t ISSVelocityLongitude() const { return fISSVelocityLongitude * TMath::RadToDeg(); }

  /** Maximum Stoermer cutoff in AMS acceptance-cone (35deg) for negative particles [GV].
    */
  Float_t MaxCutOffConeNegative() const { return fMaxCutOffConeNegative; }

  /** Maximum Stoermer cutoff in AMS acceptance-cone (35deg) for positive particles [GV].
    */
  Float_t MaxCutOffConePositive() const { return fMaxCutOffConePositive; }

  /** Maximum Stoermer cutoff in AMS acceptance-cone (35deg) [GV].
    */
  Float_t MaxCutOffCone() const { return fabs(fMaxCutOffConeNegative) > fMaxCutOffConePositive ? fMaxCutOffConeNegative : fMaxCutOffConePositive;  }

private:
  // All these are private at the moment. If anyone needs those, please contact Niko first, so we can
  // properly refactor these methods. nPART() could be renamed to NumberOfParticles() for instance.
  int  bits(Long_t Word, int ifirst, int nbits) const { return (Word>>ifirst)&((1<<nbits)-1);}

  int  nPART() const {     return bits(fStatus, 0,2);}
  int  nTRDvtrk() const {  return bits(fStatus, 8,2);}
  int  nTOFclu() const {   return bits(fStatus,10,3);}
  int  nACCclu() const {   return bits(fStatus,21,2);}
  int  nTRKtrk() const {   return bits(fStatus,13,2);}
  int  nRICHring() const { return bits(fStatus,15,2);}
  int  nECALshwr() const { return bits(fStatus,17,2);}
  int  Charge() const {    return bits(fStatus,23,3);}

  int  PARTwithTRDvtrk() const {  return bits(fStatus, 2,1);}
  int  PARTwithTOFbeta() const {  return bits(fStatus, 3,1);}
  int  PARTwithTRKtrk() const {   return bits(fStatus, 4,1);}
  int  PARTwithRICHring() const { return bits(fStatus, 5,1);}
  int  PARTwithECALshwr() const { return bits(fStatus, 6,1);}

  int  PositiveVelocity() const { return bits(fStatus,33,1);}
  int  PositiveMomentum() const { return bits(fStatus,34,1);}
  int  RigidityBin() const {      return bits(fStatus,35,2);} // ?? always 0
  int  ECALenergyBin() const {    return bits(fStatus,37,2);}

  int  PrescaledEvt() const {     return bits(fStatus,32,1);} // ?? always 0

  bool not_SingleTrackEvt() const {return (fStatus & 0x0000000000007F1F) ^ 0x000000000000311D;}
  bool not_SingleECALshwr() const {return (fStatus & 0x0000000000060040) ^ 0x0000000000020040;}

private:
  REGISTER_CLASS_WITH_TABLE(EventHeader)
};

}

}

#endif
