#ifndef TRDSUBLAYER_HH
#define TRDSUBLAYER_HH

#include <TVector3.h>
#include <TRotation.h>

#include <vector>

namespace ACsoft {

namespace Detector {

class TrdModule;
class TrdLayer;

/** %TRD sublayer structure.
  *
  * \sa Detector::Trd
  */
class TrdSublayer
{
public:

  TrdSublayer( int layerNumber, int sublayerNumber, TrdLayer* mother );
  ~TrdSublayer();

  /** Relative position of sublayer (nominal + alignment) in layer frame. */
  TVector3 RelativePosition() const { return ( fNominalRelativePosition + fOffsetRelativePosition ); }
  /** Global rotation of sublayer (nominal and alignment) with respect to layer frame. */
  TRotation RelativeRotation() const { return ( fExtraRelativeRotation*fNominalRelativeRotation ); } // FIXME: Is this correct?

  void ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot );
  void UpdateGlobalPositionAndDirection();

  /** Global position of sublayer in AMS tracker frame. */
  TVector3 GlobalPosition() const;
  /** Global rotation of sublayer in AMS tracker frame. */
  TRotation GlobalRotation() const;

  int LayerNumber() const { return fLayerNumber; }
  int SublayerNumber() const { return fSublayerNumber; }
  int GlobalSublayerNumber() const { return fGlobalSublayerNumber; }

  /** Get a list of TrdModule objects in this layer, sorted by ladder number. */
  const std::vector<TrdModule*>& Modules() const { return fTrdModules; }
  /** Get the number of modules in sublayer. */
  unsigned int NumberOfModules() const { return fTrdModules.size(); }
  /** Get a TrdModule by ladder number (0..N-1). */
  const TrdModule* GetModule( int ladderNumber ) const { return fTrdModules.at(ladderNumber); }
  /** Get a TrdModule by ladder number (0..N-1). */
  TrdModule* GetModule( int ladderNumber ) { return fTrdModules.at(ladderNumber); }

  void Dump() const;

private:

  void SetGlobalPosition( const TVector3& pos ) { fGlobalPosition = pos; }
  void SetGlobalRotation( const TRotation& matrix ) { fGlobalRotation = matrix; }

  void ConstructModules();

private:

  std::vector<TrdModule*> fTrdModules;

  TVector3 fNominalRelativePosition;  ///< nominal position of sublayer in layer frame (fixed with time)
  TVector3 fOffsetRelativePosition;   ///< additional offset to fNominalRelativePosition from alignment (may change with time)
  TRotation fNominalRelativeRotation; ///< nominal rotation of sublayer in layer frame (fixed with time)
  TRotation fExtraRelativeRotation;   ///< additional rotation from alignment (may change with time)

  TVector3 fGlobalPosition; ///< current position in AMS tracker frame
  TRotation fGlobalRotation; ///< current orientation in AMS tracker frame

  int fLayerNumber;     ///< global layer number (0..19)
  int fSublayerNumber;  ///< sublayer in layer (0..1)
  int fGlobalSublayerNumber; ///< global sublayer number (0..39)

  TrdLayer* fMother;

};
}


}

#endif // TRDSUBLAYER_HH
