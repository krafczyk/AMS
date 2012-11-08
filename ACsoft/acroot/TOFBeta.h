#ifndef TOFBeta_h
#define TOFBeta_h

#include "Tools.h"

namespace AC {

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

  TOFBeta()
    : fBeta(0)
    , fBetaError(0) {

  }

  /** Helper method dumping a TOFBeta object to the console.
    */
  void Dump() const;

  /** Helper method to calulate the charge & error.
    */
  ChargeAndError GetChargeAndErrorNew() const;

  /** Normalized velocity: beta.
    */
  Float_t Beta() const { return fBeta; }

  /** Beta error.
    */
  Float_t BetaError() const { return fBetaError; }

  /** Charge indexes sorted in descending probability (0:e, 1:H, 2:He ...)
    */
  const ChargesVector& Charges() const { return fCharges; }

  /** Probabilities for the entries in the Charges() list.
    */
  const ChargesProbabilityVector& ChargesProbability() const { return fChargesProbability; }

  /** Measured charges in layers.
    */
  const ChargesNewVector& ChargesNew() const { return fChargesNew; }

  /** Associated TOFCluster index.
    */
  const ClusterIndexVector& TOFClusterIndex() const { return fTOFClusterIndex; }

private:
  Float_t fBeta;                                // Beta(0).BetaC
  Float_t fBetaError;                           // Beta(0).ErrorC
  ChargesVector fCharges;                       // ChargeSubD("AMSChargeTOF")->ChargeI[0..1]
  ChargesProbabilityVector fChargesProbability; // ChargeSubD("AMSChargeTOF")->Prob[0..1]
  ChargesNewVector fChargesNew;                 // TOFtrack.GetQ
  ClusterIndexVector fTOFClusterIndex;          // Index to TOFclu for Q[]

  REGISTER_CLASS_WITH_TABLE(TOFBeta)
};

}

#endif
