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

template<typename T, size_t inlineCapacity>
static Moments CalculateMoments(const Vector<T, inlineCapacity>& input, MomentsMode mode = AllEntries) {

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

}

}

#endif // STATISTICS_HH
