#ifndef TRDDETECTOR_HH
#define TRDDETECTOR_HH

// This file is named TrdDetector.hh to avoid clashes with Trd.h from AMS/include.
// When building ACsoft within AMS both Trd.C files would produce Trd.o
// objects - and linking would fail.

#include <vector>
#include <string>

#include <TVector3.h>
#include <TRotation.h>

#include "TrdPdfLookup.hh"

class TTimeStamp;
class TH3F;

/// Classes that model detector structure.
namespace Detector {

class TrdLayer;
class TrdSublayer;
class TrdModule;
class TrdStraw;

/** Access to TRD structure.
  *
  */
class Trd
{

public:

  Trd();
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

  // FIXME: We need a GetTrdStraw( globalStrawNumber ) function!

  void Dump() const;


  void WriteHistosToNewFile( std::string resultdir, std::string prefix, std::string suffix );
  void WriteHistosToCurrentFile();

  void StoreADCSpectra( const TTimeStamp& time, int particleID, double dedx, double absoluteRigidity, short layer );

private:
  void ConstructTrdLayers();
  void FillConvenienceVectors();

  void CreateADCSpectraHistos();


private:

  std::vector<Detector::TrdLayer*> fLayers;
  std::vector<Detector::TrdSublayer*> fSublayers;
  std::vector<Detector::TrdModule*> fModules;

  TVector3 fNominalPosition;  ///< nominal position of %TRD centre in AMS tracker frame (fixed with time)
  TVector3 fOffsetPosition;   ///< additional offset to \c fNominalPosition from alignment (may change with time)
  TRotation fNominalRotation; ///< nominal rotation of %TRD in AMS tracker frame (fixed with time)
  TRotation fExtraRotation;   ///< additional rotation of %TRD from alignment (may change with time)

  TH3F* fADCSpectrum_vs_Rigidity[Utilities::TrdPdfLookup::fNumberOfParticles]; // FIXME: Move this and related stuff to TrdLikelihood?
  TH3F* fADCSpectrum_vs_Layer[Utilities::TrdPdfLookup::fNumberOfParticles];

};

}

#endif // TRDDETECTOR_HH
