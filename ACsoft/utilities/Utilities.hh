#ifndef UTILITIES_HH
#define UTILITIES_HH

#include <vector>
#include <math.h>
#include <string>
#include <sstream>

class TH1;
class TH2;
class TH3;
class TFile;
class TCanvas;
class TAxis;

#include <TVector3.h>

// Work around missing GetPaintedGraph() accessor in AMS ROOT 5.2.7.
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,1)
#include <TEfficiency.h>
#else
#define protected public
#include <TEfficiency.h>
#undef protected
#endif

namespace ACsoft {

namespace Utilities {

enum ProjectionType {

  YZ = 0,
  XZ = 1,
  XY = 2
};

/** Performs a line fit to a given set of points (x,y) with errors ey */
double FastLineFit(std::vector<double> &x, std::vector<double> &y, std::vector<double> &ey, double &offset, double &e_offset, double &slope, double &e_slope, double &rho);

/** Normalize each of the X-slices of a 2D-Histogram to unity sum of entries.*/
void NormalizeHistogramXSlices(TH2* hist);

TH2* Projection2D(TH3* histo, ProjectionType projectionType, int firstBin, int lastBin, std::string title = "", std::string name = "" );

TH1* GetHisto( TFile* file, const char* histoName );

void SaveCanvas(TCanvas* canvas);

bool TestBinning( TAxis* axis1, TAxis* axis2, double epsilon = 1.e-4 );
bool TestBinning( TH1* h1, TH1* h2, double epsilon = 1.e-4 );

/** Transform from AMS02 to MAG coordinate system.
  * \todo Add documentation! The inline comments inside the function aren't visible to doxygen!
  */
TVector3 TransformAMS02toMAG(const double &yaw, const double &pitch, const double &roll, const double &altitude, const double &phi, const double &theta, const double &velPhi, const double &velTheta, const TVector3 &inputVector);

double OneOverBetaCurves(double mass, double rigidity, double charge, double shift = 0., double scale = 1.);

int RootColor(int i);

template<typename T>
std::string AnyToString( T anything ) {

  std::stringstream sstr;
  sstr << anything;
  return sstr.str();
}

TGraphAsymmErrors* GraphFromTEfficiency(TEfficiency*);

}

}

#endif // UTILITIES_HH
