#ifndef ResolutionModels_HH
#define ResolutionModels_HH

namespace ACsoft {

namespace Utilities {
  class ResolutionModels {
  public:
    static double EcalSigmaEperE(double energy);
    static double TrackerSigmaSperS(double sagitta, const double mdr, double mass);
    static double TofSigmaInverseBetaPerInverseBeta(double inverseBeta, double charge);
  };
};

}

#endif
