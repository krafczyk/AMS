#ifndef TOFBeta_h
#define TOFBeta_h

#include "Tools.h"
#include "TOFBeta-Streamer.h"

namespace ACsoft {

namespace AC {

class Event;

/** Time-Of-Flight beta data
  */
class TOFBeta {
  WTF_MAKE_FAST_ALLOCATED;
public:
  /** A vector of char (8bit) numbers */
  typedef Vector<Char_t, 11> ChargesVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 11> ChargesProbabilityVector;

  /** A vector of float numbers */
  typedef Vector<Float_t, 4> ChargesNewVector;

  /** A vector of short numbers */
  typedef Vector<Short_t, 4> ClusterIndexVector;

  AC_TOFBeta_Variables

  /** Helper method dumping a TOFBeta object to the console.
    */
  void Dump() const;

  /** Helper method to return charge in TOFlayer = 1..4
    * sets 'charge' to sign*abs(Q), if there's a corresponding TOF hit for the TOFlayer and return true, otherwhise set charge to 0 and return false.
    * sign: + good charge measurement (BetaHR::IsGoodQPathL(TOFlayer)==true
    *       - weak charge measurement (BetaHR::IsGoodQPathL(TOFlayer)==false
    */ 
  bool ChargeInLayer(const AC::Event*, int TOFlayer, Float_t& charge) const;

  /** Enumeration describing the GetChargeAndError() operation mode */
  enum TOFChargeType {
    AllLayers,
    GoodLayers,
    AllUpperTOFLayers,
    AllLowerTOFLayers,
    GoodUpperTOFLayers,
    GoodLowerTOFLayers
  };

  /** Helper method to calculate charge & error for different entry selections:
    * type =  AllLayers :          use  all entries
    *         GoodLayers :         use only entries with BetaHR::IsGoodQPathL(ilay)==true   (DEFAULT)
    *         AllUpperTOFLayers :  use  all entries from ilay 1 or 2
    *         AllLowerTOFLayers :  use  all entries from ilay 3 or 4
    *         GoodUpperTOFLayers : use good entries from ilay 1 or 2
    *         GoodLowerTOFLayers : use good entries from ilay 3 or 4
    */
  ChargeAndError GetChargeAndErrorNew(const AC::Event*, TOFChargeType type = GoodLayers) const;

  /** Velocity: BetaHR::GetBeta()
    */
  Float_t Beta() const { return fBeta; }

  /** Error on Beta: BetaHR::GetEBetaV() * BetaHR::GetBeta()^2  
    */
  Float_t BetaError() const { return fBetaError; }

  /** Associated TrackerTrack index: BetaHR::iTrTrack()
   */
  Char_t TrackerTrackIndex() const { return fTrackerTrackIndex; }

  /** Measured charges in layers: BetaHR::GetQL(ilay)
    * sign: >0 for BetaHR::IsGoodQPathL(ilay)==true 
    *       <0 for BetaHR::IsGoodQPathL(ilay)==false
    *
    * vector index corresponds to TOFClusterIndex():
    *        ilay(ChargesNew(i)) = TOF().Clusters()[TOFClusterIndex()[i]].Layer()
    */
  const ChargesNewVector& ChargesNew() const { return fChargesNew; }

  /** Associated TOFCluster index: BetaHR::iTofClusterH(i(ilay))
    */
  const ClusterIndexVector& TOFClusterIndex() const { return fTOFClusterIndex; }

private:
  const AC::TOFBeta::ChargesNewVector& FilterChargesForType(const AC::Event*, TOFChargeType) const;
  mutable ChargesNewVector fSelectedCharges; // for Upper/Lower TOF charge mean

  REGISTER_CLASS_WITH_TABLE(TOFBeta)

};

}

}

#endif
