#ifndef STATISTICS_HH
#define STATISTICS_HH

#include <math.h>
#include <platform/Vector.h>
#include <TH2.h>

namespace ACsoft {

namespace Utilities {

struct Moments {
  Moments()
    : mean(0)
    , meanError(-1)
    , rms(0)
    , rmsError(-1) {

  }

  double mean;
  double meanError;
  double rms;
  double rmsError;
};

enum MomentsMode {
 AllEntries,
 AbsOfEntries,
 OnlyPositiveEntries
};

template<typename T>
static Moments CalculateMoments(const T& input, MomentsMode mode = AllEntries) {

  Moments moments;
  int inputSize = input.size();
  if( !inputSize) return moments;

  double n    = 0;
  double sum  = 0;
  double sum2 = 0;

  for( int i=0; i< inputSize; i++ ){
    if(    (mode==AllEntries) 
    || (mode==AbsOfEntries)
    || (mode==OnlyPositiveEntries && input[i]>0)
      ) {

      n    += 1.0;

      if (mode==AbsOfEntries) sum  += fabs(input[i]);
      else                    sum  += input[i];

      sum2 += input[i] * input[i];
    }
  }

  moments.mean = sum / n;
  if (n > 1) {
    moments.rms = sqrt((sum2/n - moments.mean * moments.mean));
    moments.meanError = moments.rms / sqrt(n-1.0);
    moments.rmsError  = moments.rms / sqrt(2.0 * n); // ??
  }

  return moments;
}

template<typename T, typename DataType>
static DataType FindMaximum(const T& input, MomentsMode mode = AllEntries) {

  DataType max = -std::numeric_limits<DataType>::max();
  for( unsigned int i = 0; i < input.size(); ++i) {
    if (mode == OnlyPositiveEntries && input.at(i) <= 0)
      continue;
    if( input.at(i) > max )
      max = input.at(i);
  }
  return max;
}

template<typename T, typename DataType>
static DataType FindMinimum(const T& input, MomentsMode mode = AllEntries) {

  DataType min = std::numeric_limits<DataType>::max();
  for( unsigned int i = 0; i < input.size(); ++i) {
    if (mode == OnlyPositiveEntries && input.at(i) <= 0)
      continue;
    if (input.at(i) < min)
      min = input.at(i);
  }
  return min;
}

/**
 * This routine calculates the correct 1-sigma Poisson distributed uncertainty of the integer value val.
 * Above 20 the Square-root is calculated. Below twenty the values given in (Feldman & Cousins, Phys. Rev. D, 1998, Table 2, no background) are used.
 * So all uncertaintites, also for small numbers, are correct.
 */
void PoissonUncertainty(int val, double& lowerError, double& upperError);

}

}

#endif // STATISTICS_HH
