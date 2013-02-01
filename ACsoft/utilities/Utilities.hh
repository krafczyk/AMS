#ifndef UTILITIES_HH
#define UTILITIES_HH

#include <vector>
#include <math.h>
#include <string>
#include <sstream>

#include <TH1.h>
#include <TH2.h>
#include <TVector3.h>

namespace ACsoft {

namespace Utilities {

/** Returns a vector with the lower edges for a logarithmic equidistant binning */
std::vector<double> GenerateLogBinning(int nBinLog, double Tmin, double Tmax);

/** Performs a line fit to a given set of points (x,y) with errors ey */
double FastLineFit(std::vector<double> &x, std::vector<double> &y, std::vector<double> &ey, double &offset, double &e_offset, double &slope, double &e_slope, double &rho);

/** Normalize each of the X-slices of a 2D-Histogram to 1 */
void NormalizeHistogramXSlices(TH2* hist);

/** Transform from AMS02 to MAG coordinate system.
  * \todo Add documentation! The inline comments inside the function aren't visible to doxygen!
  */
TVector3 TransformAMS02toMAG(const double &yaw, const double &pitch, const double &roll, const double &altitude, const double &phi, const double &theta, const double &velPhi, const double &velTheta, const TVector3 &inputVector);

template<typename T>
std::string AnyToString( T anything ) { 

  std::stringstream sstr;
  sstr << anything;
  return sstr.str();
}

}

}

#endif // UTILITIES_HH
