#ifndef TRDMODULE_HH
#define TRDMODULE_HH

#include <Rtypes.h>

#include <vector>

#include <TVector3.h>
#include <TRotation.h>

class TH1;
class TTimeStamp;

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
  int LadderNumber() const { return fLadderNumber; }
  int TowerNumber() const { return fTowerNumber; }

  TVector3 RelativePosition() const { return ( fNominalRelativePosition + fOffsetRelativePosition ); }
  TRotation RelativeRotation() const { return ( fExtraRelativeRotation*fNominalRelativeRotation ); } // FIXME: Is this correct? Extra rot a good idea?

  TVector3 GlobalPosition() const;
  TRotation GlobalRotation() const;

  void ChangeRelativePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot );
  void UpdateGlobalPositionAndDirection();

  /** Get TrdStraw with the given number (0..15).
    *
    */
  Detector::TrdStraw* GetTrdStraw( int number ) { return fStraws.at(number); }
  /** Get TrdStraw with the given number (0..15).
    *
    */
  const Detector::TrdStraw* GetTrdStraw( int number ) const { return fStraws.at(number); }


  void StoreAlignmentShift( const TTimeStamp& time, Double_t shift );
  void StoreDeDx( const TTimeStamp& time, Double_t dedx );
  void WriteHistogramsToCurrentFile();

  void Dump() const;

private:

  void SetGlobalPosition( const TVector3& pos ) { fGlobalPosition = pos; }
  void SetGlobalRotation( const TRotation& matrix ) { fGlobalRotation = matrix; }

  void ConstructStraws();

private:

  TVector3 fNominalRelativePosition;  ///< nominal position of module in sublayer frame (fixed with time)
  TVector3 fOffsetRelativePosition;   ///< additional offset to fNominalRelativePosition from alignment (may change with time)
  TRotation fNominalRelativeRotation; ///< nominal rotation of module in sublayer frame (fixed with time)
  TRotation fExtraRelativeRotation;   ///< additional rotation from alignment (may change with time)

  TVector3 fGlobalPosition; ///< current position in AMS tracker frame
  TRotation fGlobalRotation; ///< current orientation in AMS tracker frame

  int fModuleNumber;    ///< global module number (0..327)
  int fLayerNumber;     ///< global layer number (0..19)
  int fSublayerNumber;  ///< sublayer in layer (0..1)
  int fLadderNumber;    ///< ladder position of module in layer (0..N-1)
  int fTowerNumber;     ///< tower number (0..17 for top row ,..., 2..15 for bottom row)

  TrdSublayer* fMother; ///< pointer to mother volume

  std::vector<Detector::TrdStraw*> fStraws;

  void CreateAlignmentShiftHisto();
  void CreateDeDxHisto();
  TH1* fAlignmentShiftHisto; // FIXME: Move this and related stuff to TrdAlignment?
  TH1* fDeDxHisto; // FIXME: Move this and related stuff to TrdGainCalibration?

};

}

#endif // TRDMODULE_HH
