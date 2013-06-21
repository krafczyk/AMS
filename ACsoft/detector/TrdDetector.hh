#ifndef TRDDETECTOR_HH
#define TRDDETECTOR_HH

// This file is named TrdDetector.hh to avoid clashes with Trd.h from AMS/include.
// When building ACsoft within AMS both Trd.C files would produce Trd.o
// objects - and linking would fail.

#include <vector>
#include <string>

#include <TVector3.h>
#include <TRotation.h>

class TTimeStamp;
class TH3F;
class TCanvas;

namespace ACsoft {

namespace AC {
class TRDRawHit;
}

/// Classes that model detector structure.
namespace Detector {

class TrdLayer;
class TrdSublayer;
class TrdModule;
class TrdStraw;

/** Access to %TRD structure.
  *
  * The %TRD is built as a hierarchical structure of layers, sublayers, modules, and straws.
  * Each component, including the entire %TRD, can be moved and rotated with respect to its mother
  * volume. These translations and rotations are specified in relative coordinate frames for simplicity.
  * Whenever an element is moved, the position and orientattion of all sub-elements is recalculated automatically.
  *
  * The position and orientation of all sub-elements in the global (AMS tracker) frame can be accessed via the
  * corresponding functions (see Detector::TrdLayer, Detector::TrdSublayer, Detector::TrdModule, and Detector::TrdStraw).
  *
  * Global and module-wise shimming information is automatically read and applied when a %TRD object is constructed.
  */
class Trd
{

public:

  explicit Trd( bool applyShimmings );
  ~Trd();

  /** Global position of %TRD (nominal + alignment) in AMS frame. */
  TVector3 Position() const { return fNominalPosition + fOffsetPosition; }

  /** Global rotation of %TRD (nominal and alignment) with respect to AMS frame. */
  TRotation Rotation() const { return fExtraRotation*fNominalRotation; }

  void ChangePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot );

  /** Get TrdModule with the given global number (0..327). */
  Detector::TrdModule* GetTrdModule( int number ) { return fModules.at(number); }
  /** Get TrdModule with the given global number (0..327). */
  const Detector::TrdModule* GetTrdModule( int number ) const { return fModules.at(number); }

  /** Get TrdLayer with the given number (0..19). */
  Detector::TrdLayer* GetTrdLayer( int number ) { return fLayers.at(number); }
  /** Get TrdLayer with the given number (0..19). */
  const Detector::TrdLayer* GetTrdLayer( int number ) const { return fLayers.at(number); }

  /** Get TrdSublayer with the given global number (0..39). */
  Detector::TrdSublayer* GetTrdSublayer( int number ) { return fSublayers.at(number); }
  /** Get TrdSublayer with the given number (0..39). */
  const Detector::TrdSublayer* GetTrdSublayer( int number ) const { return fSublayers.at(number); }

  /** Get TrdStraw with the given global number (0..5247). */
  Detector::TrdStraw* GetTrdStraw ( int number ) { return fStraws.at(number); }
  /** Get TrdStraw   with the given global number (0..5247). */
  const Detector::TrdStraw* GetTrdStraw ( int number ) const { return fStraws.at(number); }

  void Dump() const;

  TCanvas* DrawProjections( float xcenter, float ycenter, float xywidth, bool rotated = false, std::string canvasname = "" );

  /** Helper function: Get the global position of a given TRDRawHit.*/
  TVector3 HitPosition( const AC::TRDRawHit* hit ) const;

  /** Helper function: Get the global tube direction of a given TRDRawHit.*/
  TVector3 HitDirection( const AC::TRDRawHit* hit ) const;

private:
  void ConstructTrdLayers();
  void FillConvenienceVectors();

  void ReadTrdShimmingGlobalFile();
  void ReadTrdShimmingModuleFile();


private:

  std::vector<Detector::TrdLayer*> fLayers;
  std::vector<Detector::TrdSublayer*> fSublayers;
  std::vector<Detector::TrdModule*> fModules;
  std::vector<Detector::TrdStraw*> fStraws;

  TVector3 fNominalPosition;  ///< nominal position of %TRD centre in AMS tracker frame (fixed with time)
  TVector3 fOffsetPosition;   ///< additional offset to \c fNominalPosition from alignment (may change with time)
  TRotation fNominalRotation; ///< nominal rotation of %TRD in AMS tracker frame (fixed with time)
  TRotation fExtraRotation;   ///< additional rotation of %TRD from alignment (may change with time)


};

}

}

#endif // TRDDETECTOR_HH
