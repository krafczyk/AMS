#ifndef BINNING_HH
#define BINNING_HH

#include <vector>

static const int nBinsPamela = 18;

double* PamelaBinning() {
  static double gPamelaBinning[nBinsPamela+1] = {0.5, 1.0, 1.5, 1.8, 2.2, 2.7, 3.3, 4.1, 5.0, 6.1, 7.4, 9.1, 11.2, 15.0, 20.0, 28.0, 42.0, 65.0, 100.0};
  return gPamelaBinning;
}

static const int nBinsEcal = 35;

double* EcalBinning() {
  static double gEcalBinning[nBinsEcal+1] = {0.86, 1.35, 1.95, 2.69, 3.57, 4.60, 5.78, 7.14, 8.68, 10.41, 12.36, 14.54, 16.97, 19.67, 22.67, 25.99, 29.66, 33.73, 38.22, 43.17, 48.64, 54.66, 61.30, 68.62, 76.68, 85.55, 96.59, 110.83, 130.07, 157.84, 202, 270, 370, 530, 750, 1100 };
  return gEcalBinning;
}

static const int nBinsAlpha = 33;

double* AlphaBinning() {
  static double gAlphaBinning[nBinsAlpha+1] = {0.75, 1.00, 1.39, 1.86, 2.41, 3.04, 3.75, 4.55, 5.43, 6.42, 7.50, 8.69, 10.00, 12.01, 14.26, 16.78, 19.59, 22.71, 26.18, 31.00, 38.36, 47.03, 57.22, 69.18, 83.20, 100.00, 127.90, 162.60, 206.00, 260.00, 350.0, 500, 750, 1000};
  return gAlphaBinning;
}

namespace ACsoft {

namespace Utilities {

void CollectEquidistantBinning(std::vector<double>& vec, int nbins, double fromValue, double toValue);

}

}

#endif // BINNING_HH
