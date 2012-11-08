#ifndef STATISTICS_HH
#define STATISTICS_HH

#include <math.h>
#include <platform/Vector.h>
#include <TH2.h>

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

template<typename T, size_t inlineCapacity>
static Moments CalculateMoments(const Vector<T, inlineCapacity>& input) {

  Moments moments;
  int inputSize = input.size();
  if( !inputSize) return moments;

  double sum = 0;
  double sum2 = 0;
  for( int i=0; i< inputSize; i++ ){
    sum  += input[i];
    sum2 += input[i] * input[i];
  }

  double dinputSize = inputSize;
  moments.mean = sum / dinputSize;
  if (inputSize > 1) {
    moments.rms = sqrt((sum2 - moments.mean * moments.mean / dinputSize) / (dinputSize - 1.0));
    moments.meanError = moments.rms / sqrt(dinputSize);
    moments.rmsError = moments.rms / sqrt(2.0 * dinputSize);
  }

  return moments;
}

}

#endif // STATISTICS_HH
