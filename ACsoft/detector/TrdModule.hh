#ifndef TRDMODULE_HH
#define TRDMODULE_HH

#include <Rtypes.h>

#include <vector>

#include <TVector3.h>
#include <TRotation.h>

#include <TRDRawHit.h>

class TH1;
class TTimeStamp;

namespace ACsoft {

namespace Detector {

class TrdStraw;
class TrdSublayer;

/** Container class for information regarding one TRD module.
  *
  * \sa Detector::Trd
  */
class TrdModule
{

public:

  TrdModule( int moduleNumber, TrdSublayer* mother );
  ~TrdModule();

  int ModuleNumber() const { return fModuleNumber; }
  int LayerNumber() const { return fLayerNumber; }
  int SublayerNumber() const { return fSublayerNumber; }
  int GlobalSublayerNumber() const { return fGlobalSublayerNumber; }
  int LadderNumber() const { return fLadderNumber; }
  int TowerNumber() const { return fTowerNumber; }
  AC::MeasurementMode Direction() const { return fDirection; }

  TVector3 RelativePosition() const { return ( fNominalRelativePosition + fShimmingOffset + fOffsetRelativePosition ); }
  TRotation RelativeRotation() const { return ( fExtraRelativeRotation*fShimmingRotation*fNominalRelativeRotation ); } // FIXME: Is this correct? Extra rot a good idea?

  TVector3 GlobalPosition() const;
  TRotation GlobalRotation() const;

  void ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot, bool alignmentOk );
  void UpdateGlobalPositionAndDirection();

  /** Get TrdStraw with the given number (0..15).
    *
    */
  Detector::TrdStraw* GetTrdStraw( int number ) { return fStraws.at(number); }
  /** Get TrdStraw with the given number (0..15).
    *
    */
  const Detector::TrdStraw* GetTrdStraw( int number ) const { return fStraws.at(number); }
  /** Get the number of straws in module. */
  unsigned int NumberOfStraws() const { return fStraws.size(); }

  void Dump() const;

  void SetShimmingRotation( const TRotation& matrix );
  void SetShimmingOffset( const TVector3& offset );

  void SetCurrentGainValue( const double val, const bool isGainOk ) { fCurrentGainValue = val; fCurrentGainValueOk = isGainOk; }
  double CurrentGainValue() const { return fCurrentGainValue; }
  bool IsCurrentGainValueOk() const { return fCurrentGainValueOk; }

  bool IsAlignmentOk() const { return fAlignmentParametersOk; }
  // FIXME for the general case of aligning different levels in the TRD hierarchy at the same time, there must be a top-level function in
  // FIXME TRD detector, which checks that the alignment at all levels is ok

private:

  void SetGlobalPosition( const TVector3& pos ) { fGlobalPosition = pos; }
  void SetGlobalRotation( const TRotation& matrix ) { fGlobalRotation = matrix; }

  void ConstructStraws();

private:

  TVector3 fNominalRelativePosition;  ///< nominal position of module in sublayer frame (fixed with time)
  TVector3 fShimmingOffset;           ///< shimming offset of module (fixed with time)
  TVector3 fOffsetRelativePosition;   ///< additional offset to fNominalRelativePosition from alignment (may change with time)
  TRotation fNominalRelativeRotation; ///< nominal rotation of module in sublayer frame (fixed with time)
  TRotation fShimmingRotation;        ///< shimming rotation (fixed with time)
  TRotation fExtraRelativeRotation;   ///< additional rotation from alignment (may change with time)
  bool fAlignmentParametersOk;        ///< errors when getting alignment parameters from lookup?

  TVector3 fGlobalPosition; ///< current position in AMS tracker frame
  TRotation fGlobalRotation; ///< current orientation in AMS tracker frame

  double fCurrentGainValue;   ///< current gain (ADC/cm)
  bool   fCurrentGainValueOk; ///< errors when getting gain value from lookup?

  int fModuleNumber;    ///< global module number (0..327)
  int fLayerNumber;     ///< global layer number (0..19)
  int fSublayerNumber;  ///< sublayer in layer (0..1)
  int fGlobalSublayerNumber; ///< global sublayer number (0..39)
  int fLadderNumber;    ///< ladder position of module in layer (0..N-1)
  int fTowerNumber;     ///< tower number (0..17 for top row ,..., 2..15 for bottom row)
  AC::MeasurementMode fDirection; ///< describes how module is oriented (before alignment)

  TrdSublayer* fMother; ///< pointer to mother volume

  std::vector<Detector::TrdStraw*> fStraws;

};

}

}

#endif // TRDMODULE_HH
