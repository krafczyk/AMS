
#include "ParticleSelectionCuts.hh"

/** Classify particles according to tracker sign and charge measurements.
 *
 * Returns the particle id code for a given rigidity and tracker charge. Relies exclusively on tracker information.
 *
 * Possible results:
 *
 * - -1: unknown
 * - 0: electron
 * - 1: proton
 * - 2: Helium
 * - 3-6: Lithium .. Carbon
 *
 */
short Cuts::ParticleID(float trackerCharge, float rigidity) {

  int Q = rigidity < 0. ? -1 : 1;

  int IdPart = 1;
  if (Q < 0) IdPart = 0;

  int Z = 0;
  for (unsigned int i = 0; i < Cuts::TrackerCharges::gTrackerCharges; ++i) {
    if (fabs(trackerCharge - Cuts::TrackerCharges::GetMean(i)) < 3 * Cuts::TrackerCharges::GetSigma(i)) {
	  Z = i+1;
	  break;
	}
  }

  if (Z == 0) { return -1; }

  if (Z==1) {
    if (Q==+1) IdPart = 1;       // Proton
    else if (Q==-1) IdPart = 0;  // Electron
  }
  else IdPart = Z;

  return IdPart;
}
