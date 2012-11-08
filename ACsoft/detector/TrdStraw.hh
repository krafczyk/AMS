#ifndef TRDSTRAW_HH
#define TRDSTRAW_HH

#include <TVector3.h>

namespace Detector {

class TrdModule;

/** Container class for information regarding one TRD straw.
  *
  * \sa Detector::Trd
  */
class TrdStraw
{
public:

    TrdStraw( int layerNumber, int sublayerNumber, int ladderNumber, int moduleNumber, int strawNumber,
              TrdModule* mother );

    int LayerNumber() const { return fLayerNumber; }
    int SublayerNumber() const { return fSublayerNumber; }
    int LadderNumber() const { return fLadderNumber; }
    int ModuleNumber() const { return fModuleNumber; }
    int StrawNumber() const { return fStrawNumber; }
    int GlobalStrawNumber() const { return fGlobalStrawNumber; }

    TVector3 RelativePosition() const { return fNominalRelativePosition; }
    TVector3 NominalWireDirection() const { return fNominalWireDirection; }

    TVector3 GlobalPosition() const { return fGlobalPosition; }
    TVector3 GlobalWireDirection() const { return fGlobalWireDirection; }

    void UpdateGlobalPositionAndDirection();

    void Dump() const;

private:

    void SetGlobalPosition( const TVector3& pos ) { fGlobalPosition = pos; }
    void SetGlobalWireDirection( const TVector3& dir ) { fGlobalWireDirection = dir; }

private:

    TVector3 fNominalRelativePosition; ///< vector from nominal centre of module to nominal centre of straw (TRD module is assumed to be a rigid body!)
    TVector3 fNominalWireDirection; ///< vector in the nominal direction of the wire inside the straw

    TVector3 fGlobalPosition; ///< current position in AMS tracker frame
    TVector3 fGlobalWireDirection; ///< current direction of straw tube wire in AMS tracker frame

    int fLayerNumber;        ///< global layer number (0..19)
    int fSublayerNumber;     ///< sublayer in layer (0..1)
    int fLadderNumber;       ///< ladder position of module in layer (0..N-1)
    int fModuleNumber;       ///< global module number (0..327)
    int fStrawNumber;        ///< straw number in module (0..15)
    int fGlobalStrawNumber;  ///< global straw number (0..5247)

    TrdModule* fMother;      ///< pointer to mother volume
};
}

#endif // TRDSTRAW_HH
