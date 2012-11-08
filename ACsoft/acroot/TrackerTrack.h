#ifndef TrackerTrack_h
#define TrackerTrack_h

#include "TrackerTrackFit.h"
#include "TrackerReconstructedHit.h"

#include <bitset>
#include <math.h>

namespace AC {

/** Helper structure for TrackerTrack
  */
struct PatternChargeAndError {
  WTF_MAKE_FAST_ALLOCATED;
public:

  /** Default constructor */
  PatternChargeAndError(UChar_t _pattern = 0, Float_t _charge = 0, Float_t _error = 0)
    : pattern(_pattern)
    , charge(_charge)
    , error(_error) {

  }

  /** Pattern */
  UChar_t pattern; ///< pattern
  
  /** Charge */
  Float_t charge;  ///< charge

  /** Error */
  Float_t error;   ///< error
};

#ifndef __CINT__
QDataStream& operator<<(QDataStream&, const PatternChargeAndError&);
QDataStream& operator>>(QDataStream&, PatternChargeAndError&);
#endif

/** %Tracker track data
  */
class TrackerTrack {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of char (8bit) numbers */
  typedef Vector<Char_t, 1> ChargesVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 1> ChargesProbabilityVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 3> SCalibProbabilityVector;

  /** A vector of PatternChargeAndError objects */
  typedef Vector<PatternChargeAndError, 3> ChargesNewVector;

  /** A vector of TrackerTrackFit objects */
  typedef Vector<TrackerTrackFit, 18> FitsVector;

  /** A vector of TrackerReconstructedHit objects */
  typedef Vector<TrackerReconstructedHit, 9>  ReconstructedHitsVector;

  /** A vector of unsigned short numbers */
  typedef Vector<UShort_t, 9> ClusterDistancesVector;

  TrackerTrack()
    : fStatus(0)
    , fNumberOfHitsX(0)
    , fNumberOfHitsY(0) {
 
  }

  /** Helper method dumping an TrackerTrack object to the console
    */
  void Dump() const;

  /** Returns track status information.
    * \todo Add documentation.
    */
  UInt_t Status() const { return fStatus; }
 
  /** Number of hits in X direction.
    */ 
  Int_t NumberOfHitsX() const { return fNumberOfHitsX; }

  /** Number of hits in Y direction.
    */ 
  Int_t NumberOfHitsY() const { return fNumberOfHitsY; }

  /** Charge indexes sorted in descending probability (0:e, 1:H, 2:He ...)
    * \todo Verify documentation.
    */
  const ChargesVector& Charges() const { return fCharges; }

  /** Probabilities for the entries in the Charges() list.
    */
  const ChargesProbabilityVector& ChargesProbability() const { return fChargesProbability; }

  /** Probabilities for this track extrapolted into TRD.
    * Note: the e/p/He logLikelihoodRatios are taken from the TrdSCalib
    * \todo Verify documentation.
    */
  const SCalibProbabilityVector& SCalibProbability() const { return fSCalibProbability; } // FIXME remove!

  /** Pattern, charge and error triplet.
    * \todo Add documentation.
    */
  const ChargesNewVector& ChargesNew() const { return fChargesNew; }

  /** Fitted tracks
    */
  const FitsVector& TrackFits() const { return fTrackFits; }

  /** Reconstructed hits
    */
  const ReconstructedHitsVector& ReconstructedHits() const { return fReconstructedHits; }

  /** Distance to next cluster in each layer in micrometer (for layer 0-8).
    */
  const ClusterDistancesVector& ClusterDistances() const { return fClusterDistances; }

  /** GetFit
    * \todo Add documentation.
    */
  int GetFit( int kAlgo, int kPatt, int kRefit ) const {
    int i;
    for( i=0; i<(int)fTrackFits.size(); i++){
      if(                 kAlgo  != fTrackFits[i].Algorithm()   ) continue;   // no match
      if(                 kPatt  != fTrackFits[i].Pattern()   ) continue;   // no match
      if( (kRefit>=0) && (kRefit != fTrackFits[i].Refit()) ) continue;   // no match
      break;                                                       // match
    }
    if(i<(int)fTrackFits.size()) return  i; // match within size()
    else                  return -1; // no match found
  }


  /** JLpatt
    * \todo Add documentation.
    */
  int JLpatt() const {    // Jlayer -attern bits 0-5:  JL2,34,56,78,1xy,9xy
    int JLpatt=0;
    for(int j=0; j<(int)fReconstructedHits.size(); j++){
      switch (fReconstructedHits[j].Layer()){
        case  2: JLpatt |= 1; break;
        case  3: JLpatt |= 2; break;
        case  4: JLpatt |= 2; break;
        case  5: JLpatt |= 4; break;
        case  6: JLpatt |= 4; break;
        case  7: JLpatt |= 8; break;
        case  8: JLpatt |= 8; break;
        case  1: if(fReconstructedHits[j].IsXAndYSignal()) JLpatt |= 16; break; // xy in Jlay-1
        case  9: if(fReconstructedHits[j].IsXAndYSignal()) JLpatt |= 32; break; // xy in Jlay-9
      }
    }
    return JLpatt;
  }

  /** NHxyInner
    * \todo Add documentation.
    */
  int NHxyInner() const {    // Number of XY hits in Inner Tracker Jlay 2-8
    int NHxy=0;
    for(unsigned int j=0; j<fReconstructedHits.size(); j++)
      if(    (fReconstructedHits[j].IsXAndYSignal())
          && (fReconstructedHits[j].Layer() >= 2  )
          && (fReconstructedHits[j].Layer() <= 8  )
        ) NHxy++;

    return NHxy;
  }

  /** Retrieve the charge & error for a given pattern.
    * Note: pattern=3 (inner tracker), 1 (layer 1), 9 (layer 9).
    * pattern=0 returns the max-span averaged charge over all available layers.
    * The returned error is -1, if there is no measurement available.
    * The returned error is 0 for single layer measurements.
    */
  // FIXME: Introduce an enum for pattern instead of relying on magic numbers.
  ChargeAndError GetChargeAndErrorNew(unsigned int pattern) const {

    Float_t charge = 0;
    Float_t error = -1;
    if (fChargesNew.size() == 0) return ChargeAndError(charge,error);

    if (pattern == 0) {
      Q_ASSERT(fChargesNew[0].pattern == 3); // The first entry, if present, always belongs to the inner tracker.
      charge = fChargesNew[0].charge;        // Inner.Q    as start
      error = fChargesNew[0].error;          // Inner.ERR  as start

      if (fChargesNew.size() == 1) return ChargeAndError(charge, error);

      // FIXME: Use Statistics::CalculateMoments() here - the error computation should not be duplicated, to avoid mistakes.
      // have extra single L1,9
      float Ninn = fChargesNew[1].error;                         // # of inner Layers
      float Nall =  Ninn + float(fChargesNew.size() - 1);        // add # of ext. Layers
      float V = (Ninn - 1.0) * error * error;                    // Variance of inner Layers
      float SQ2  =  Ninn* (V + charge * charge);                 // SUM Qi^2 for inner Layers
      for (int i = 1; i<(int)fChargesNew.size(); i++) {          // loop over Single L1,9
        charge += fChargesNew[i].charge/Ninn;                    // weighted average
        SQ2 += fChargesNew[i].charge * fChargesNew[i].charge;    // continue SUM Qi^2
      }
      charge *= Ninn / Nall;                                     // rescale
      error = sqrt((SQ2/Nall - charge * charge) / (Nall - 1.0)); // RMS_all/sqrt(Nall-1)
      return ChargeAndError(charge, error);
    }
 
    for (int i=0; i<(int)fChargesNew.size(); i++){
      if( fChargesNew[i].pattern != pattern ) continue;
      charge = fChargesNew[i].charge;
      error = 0;
      if( pattern == 3 ) error = fChargesNew[i].error;
      break;
    }

    return ChargeAndError(charge, error);
  }

  /** Test if all double-layers in inner tracker have at least one XY hit.
    *
    * To be precise, test if there is an XY hit on layers 2 and (3 or 4) and (5 or 6) and (7 or 8)
    */
  bool IsGoodInnerTrack() const {

    std::bitset<10> layers = LayerXYPatternBitset();
    return ( layers.test(2) && ( layers.test(3) || layers.test(4) ) && ( layers.test(5) || layers.test(6) ) && ( layers.test(7) || layers.test(8) ) );
  }

  /** Test if there is an XY hit on (3 or 4) and (5 or 6) and (7 or 8)
    */
  bool IsGoodCentralInnerTrack() const {

    std::bitset<10> layers = LayerXYPatternBitset();
    return ( ( layers.test(3) || layers.test(4) ) && ( layers.test(5) || layers.test(6) ) && ( layers.test(7) || layers.test(8) ) );
  }

  /** Test if layer 2 has at least one XY hit.
    */
  bool HasLayer2XYHit() const {

    std::bitset<10> layers = LayerXYPatternBitset();
    return layers.test(2);
  }

  /** Test if layer 2 has at least one Y hit.
    */
  bool HasLayer2YHit() const {

    std::bitset<10> layers = LayerYPatternBitset();
    return layers.test(2);
  }

  /** Returns a bitset with 9 bits, indiciating if there's XY hit in layer 1-9.
    */
  std::bitset<10> LayerXYPatternBitset() const {

    std::bitset<10> layers = 0;
    
    for( unsigned int i=0 ; i<fReconstructedHits.size() ; ++ i) {
      if( fReconstructedHits[i].IsXAndYSignal()) {
        int layer = int(fReconstructedHits[i].Layer());
        layers.set(layer);
      }
    }
  
    return layers;
  }

  /** Returns a bitset with 9 bits, indiciating if there's Y hit in layer 1-9, which doesn't neccessarily include a X hit.
    */
  std::bitset<10> LayerYPatternBitset() const {

    std::bitset<10> layers = 0;

    for( unsigned int i=0 ; i<fReconstructedHits.size() ; ++ i) {
      int layer = int(fReconstructedHits[i].Layer());
      layers.set(layer);
    }
    return layers;
  }

  /** Create a bit pattern of the hit distribution.
    *
    * Low bits: layers in track, high bits: layers with XY hit in track.
    */
  std::bitset<32> MakeBitPattern() const {

    std::bitset<32> TrkBits = 0;

    for (unsigned int i=0; i<ReconstructedHits().size(); i++) {
      const TrackerReconstructedHit& rhit = ReconstructedHits().at(i);
      Int_t iBit = rhit.Layer()-1;
      TrkBits.set(iBit);
      // Hit in X+Y ?
      if( rhit.IsXAndYSignal() ) TrkBits.set(16+iBit);
    }

    return TrkBits;
  }

private:
  UInt_t fStatus;                               // TrTrack->Status
  Int_t fNumberOfHitsX;                         // TrTrack->GetNhitsX()
  Int_t fNumberOfHitsY;                         // TrTrack->GetNhitsY()
  ChargesVector fCharges;                       // Particle.pCharge.ChargeSubD("AMSChargeTrackerInner")->ChargeI[0..1]
  ChargesProbabilityVector fChargesProbability; // Particle.pCharge.ChargeSubD("AMSChargeTrackerInner")->Prob[0..1]
  SCalibProbabilityVector fSCalibProbability;   // TrdSCalib.TrdCprobs[]  0:e  1:p  2:He
  ChargesNewVector fChargesNew;                 // TrTrack.GetQ/ERR for Patt 3:inner 1:L1  9:L9
  FitsVector fTrackFits;                        // TrTrack FitResults  [0]:default   [1]:inner-tracker only
  ReconstructedHitsVector fReconstructedHits;   // used TrRecHitR
  ClusterDistancesVector fClusterDistances;     // distances to next cluster

  REGISTER_CLASS_WITH_TABLE(TrackerTrack)
};

}

#endif
