
#include "Binning.hh"

#include <assert.h>

void ACsoft::Utilities::CollectEquidistantBinning(std::vector<double>& vec, int nbins, double fromValue, double toValue) {

  assert(nbins>0);
  assert(toValue>fromValue);

  vec.clear();
  vec.assign(nbins+1,0.0);

  double binwidth = (toValue-fromValue)/nbins;

  for( int i=0 ; i<=nbins ; ++i )
    vec[i] = fromValue + i*binwidth;
}
