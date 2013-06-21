#include "ResolutionModels.hh"

#include <cmath>
#include <assert.h>

double ACsoft::Utilities::ResolutionModels::EcalSigmaEperE(double energy) {

  // From http://arxiv.org/pdf/1210.0316.pdf, our ECAL paper: sigma_E / E  = 10.4% / sqrt(E) + 1.4%
  // However Incagli presented on 13.12.2012, that 10.2% / sqrt(E) + 2.0% describes
  // non-perpendicular tracks better. Stick with that suggestion for now.
  return (10.2 / sqrt(energy) + 2.0) / 100.0;
}

double ACsoft::Utilities::ResolutionModels::TrackerSigmaSperS(double sagitta, const double mdr, double mass) {

  assert(mdr > 0);
  double p = fabs(1. / sagitta);
  double beta = p / sqrt(p * p + mass * mass);
  double a = 1. / mdr;
  double b = 0.1;
  return sqrt((a / sagitta) * (a / sagitta) + (b / beta) * (b / beta));
}

double ACsoft::Utilities::ResolutionModels::TofSigmaInverseBetaPerInverseBeta(double inverseBeta, double charge) {
  // See: http://cds.cern.ch/record/962425/files/19041-ita-giovacchini-F-abs1-he21-oral.pdf
  const double p1 = 0.159;
  const double p2 = 0.079;
  const double sigmaT = sqrt((p1/charge) * (p1/charge) + p2 * p2); // ns
  const double tofDistance = 636.5 + 636.3; // mm
  const double speedOfLight = 299792458; // m/s
  const double sigmaInverseBeta = (sigmaT * speedOfLight / tofDistance) * 1e-6;
  return sigmaInverseBeta / inverseBeta;
}
