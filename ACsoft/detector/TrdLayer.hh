#ifndef TRDLAYER_HH
#define TRDLAYER_HH

#include <TVector3.h>
#include <TRotation.h>

#include <vector>

namespace ACsoft {

namespace Detector {

class TrdSublayer;
class TrdModule;
class Trd;

/** %TRD layer structure.
  *
  * \sa Detector::Trd
  */
class TrdLayer
{
public:

  TrdLayer( int layerNumber, Trd* mother );
  ~TrdLayer();

  /** Relative position of layer (nominal + alignment) in %TRD frame. */
  TVector3 RelativePosition() const { return ( fNominalRelativePosition + fOffsetRelativePosition ); }
  /** Global rotation of layer (nominal and alignment) with respect to %TRD frame. */
  TRotation RelativeRotation() const { return ( fExtraRelativeRotation*fNominalRelativeRotation ); } // FIXME: Is this correct?

  /** Global position of layer in AMS tracker frame. */
  TVector3 GlobalPosition() const;
  /** Global rotation of layer in AMS tracker frame. */
  TRotation GlobalRotation() const;

  int LayerNumber() const { return fLayerNumber; }
  unsigned int NumberOfSublayers() const { return fSublayers.size(); }

  /** Get TrdSublayer with the given global number (0..1). */
  Detector::TrdSublayer* GetSublayer( int number ) { return fSublayers.at(number); }
  /** Get TrdSublayer with the given number (0..1). */
  const Detector::TrdSublayer* GetSublayer( int number ) const { return fSublayers.at(number); }

  /** Get a list of TrdModule objects in this layer, in no particular order! */
  const std::vector<TrdModule*>& GetListOfModules() const { return fTrdModules; }

  void ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot );
  void UpdateGlobalPositionAndDirection();

  void Dump() const;

private:

  void SetGlobalPosition( const TVector3& pos ) { fGlobalPosition = pos; }
  void SetGlobalRotation( const TRotation& matrix ) { fGlobalRotation = matrix; }

  void ConstructSublayers();
  void FillConvenienceVectors();

private:

  std::vector<TrdSublayer*> fSublayers;
  std::vector<TrdModule*> fTrdModules;

  TVector3 fNominalRelativePosition;  ///< nominal position of layer in %TRD frame (fixed with time)
  TVector3 fOffsetRelativePosition;   ///< additional offset to fNominalRelativePosition from alignment (may change with time)
  TRotation fNominalRelativeRotation; ///< nominal rotation of layer in %TRD frame (fixed with time)
  TRotation fExtraRelativeRotation;   ///< additional rotation from alignment (may change with time)

  TVector3 fGlobalPosition; ///< current position in AMS tracker frame
  TRotation fGlobalRotation; ///< current orientation in AMS tracker frame

  int fLayerNumber;     ///< global layer number (0..19)

  Trd* fMother;         ///< pointer to mother volume
};
}
}

#endif // TRDLAYER_HH
