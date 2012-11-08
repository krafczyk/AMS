#ifndef TrackerExternalAlignment_h
#define TrackerExternalAlignment_h

#include <TObject.h>
#include <TTimeStamp.h>

namespace AC {

/** Tracker external alignment data
  */
class TrackerExternalAlignment : public TObject {
public:
  TrackerExternalAlignment()
    : fID(0 ){

    for (unsigned int i = 0; i < 3; ++i) {
      fPositions[i] = 0;
      fRotations[i] = 0;
    }
  }

  /** ID
    * \todo Add documentation.
    */
  UChar_t ID() const { return fID; }

  /** Start-validity for data set.
    * \todo Verify documentation.
    */
  const TTimeStamp& StartTimeStamp() const { return fStartTimeStamp; }

  /** End-validity for data set.
    * \todo Verify documentation.
    */
  const TTimeStamp& EndTimeStamp() const { return fEndTimeStamp; }

  /** Returns the x-offset [cm].
    */
  Float_t XOffset() const { return fPositions[0]; }

  /** Returns the y-offset [cm].
    */
  Float_t YOffset() const { return fPositions[1]; }

  /** Returns the z-offset [cm].
    */
  Float_t ZOffset() const { return fPositions[2]; }

  /** Returns the x-rotation [deg].
    */
  Float_t XRotation() const { return fRotations[0]; }

  /** Returns the y-rotation [deg].
    */
  Float_t YRotation() const { return fRotations[1]; }

  /** Returns the z-rotation [deg].
    */
  Float_t ZRotation() const { return fRotations[2]; }

  /** Z coordinate [cm] of Layer 1, as measured in Perugia. */
  static const Float_t ZLayer1Perugia; //! Perugia Rotation-Z  for Layer-1  159.0673     check !!
  
  /** Z coordinate [cm] of Layer 9, as measured in Perugia. */
  static const Float_t ZLayer9Perugia; //! Perugia Rotation-Z  for Layer-9 -136.0406     check !!

  /** Z coordinate [cm] of Layer 1, as measured in Madrid. */
  static const Float_t ZLayer1Madrid;  //! Madrid  Rotation-Z  for Layer-1  159.0673

  /** Z coordinate [cm] of Layer 9, as measured in Madrid. */
  static const Float_t ZLayer9Madrid;  //! Madrid  Rotation-Z  for Layer-9 -136.0406

private:
  UChar_t fID;                // 10*(0:PG 1:Madrid) + Plane=1/9    20 for Kunin global TRD alignment 
  TTimeStamp fStartTimeStamp; // Unix-Time start-validity for DataSet 
  TTimeStamp fEndTimeStamp;   // Unix-Time   end-validity for DataSet 
  Float_t fPositions[3];      // Offset Dxyz/cm
  Float_t fRotations[3];      // Rotation Angles/Rad around X/Y/Z axis 

  ClassDef(TrackerExternalAlignment, 2);
};

}

#endif
