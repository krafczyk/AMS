#include "SplineTrack.hh"
#include "AMSGeometry.h"
#include "pathlength_functions.hh"

#include <TMath.h>
#include <TGraph.h>

#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "SplineTrack> "
#include <debugging.hh>

bool ACsoft::Analysis::SplineTrack::WarnOutOfBounds = true;

ACsoft::Analysis::SplineTrack::SplineTrack() :
  fSource(ACsoft::Analysis::none),
  fRigidity(0.0),
  fRigidityUncertainty(0.0),
  fDrawGraphXZ(0),
  fDrawGraphYZ(0)
{
}

/** Interpolate track position at given z.
  *
  *  \param z z-coordinate (cm)
  */
TVector3 ACsoft::Analysis::SplineTrack::InterpolateToZ( Double_t z ) const {

  assert(fPoints.size()>2);

  // check z out of range!
  Double_t zmin = fPoints.front().z();
  Double_t zmax = fPoints.back().z();
  if( zmin > zmax ){
    Double_t ztmp = zmin;
    zmin = zmax;
    zmax = ztmp;
  }
  if(SplineTrack::WarnOutOfBounds && ( z > zmax || z < zmin )){
    WARN_OUT << "z value " << z << " out of range ("<<zmin<<".."<<zmax<<") !" << std::endl;
  }

  Double_t x = fSplineZX.Eval(z);
  Double_t y = fSplineZY.Eval(z);

  assert( !TMath::IsNaN(x) );
  assert( !TMath::IsNaN(y) );

  return TVector3(x,y,z);
}


/** Interpolate track position at given z.
  *
  *  \param[in] z z-coordinate (cm)
  *  \param[out] pos interpolated position (x,y,z) (cm)
  *  \param[out] dir interpolated direction (unit vector in tangential direction to track)
  */
void ACsoft::Analysis::SplineTrack::CalculateLocalPositionAndDirection( Double_t z, TVector3& pos, TVector3& dir ) const {

  /// \todo good value?
  static const Double_t dz = 0.1;

  pos = InterpolateToZ(z);

  TVector3 pos1 = InterpolateToZ(z-dz);
  TVector3 pos2 = InterpolateToZ(z+dz);

  TVector3 diff = pos2 - pos1;

  dir = diff.Unit();
}


/** Calculate distance of track and given point along a line perpendicular to the z-axis.
  *
  * \param D 0: \p XY is X-coordinate, 1: \p XY is Y-coordinate.
  * \param Z Z-coordinate of point.
  * \param XY X- or Y-coordinate of point (depending on value of D).
  */
Double_t ACsoft::Analysis::SplineTrack::DeltaXY( int D, Double_t Z, Double_t XY ) const {

  TVector3 trackPos = InterpolateToZ(Z);

  return XY - ( D==0 ? trackPos.X() : trackPos.Y() );
}


/** Print contents.
  *
  */
void ACsoft::Analysis::SplineTrack::Dump() const {

  INFO_OUT << "Source: " << fSource << " p=" << fRigidity << "+-" << fRigidityUncertainty << std::endl;
  std::cout << "     Points: " << std::endl;
  for( unsigned i=0 ; i<fPoints.size() ; ++i )
    std::cout << "       ["<<i<<"] " << "("<<fPoints[i].x()<<","<<fPoints[i].y()<<","<<fPoints[i].z()<<")" << std::endl;
  if(DEBUG>1){
    std::cout << "     x-z spline: \n";
    fSplineZX.Dump();
    std::cout << "     y-z spline: \n";
    fSplineZY.Dump();
  }
}


void ACsoft::Analysis::SplineTrack::DrawXZProjection( float zmin, float zmax, bool rotatedSystem, int nPoints ) {

  if( fDrawGraphXZ ) delete fDrawGraphXZ;
  fDrawGraphXZ = new TGraph(nPoints);

  for( int i=0 ; i<nPoints ; ++i ){
    float z    = zmin + i*(zmax-zmin)/(nPoints-1);
    float x    = InterpolateToZ(z).X();
    float xPad = rotatedSystem ? -z : x;
    float yPad = rotatedSystem ?  x : z;
    fDrawGraphXZ->SetPoint(i,xPad,yPad);
  }

  fDrawGraphXZ->Draw("P");
}


void ACsoft::Analysis::SplineTrack::DrawYZProjection( float zmin, float zmax, bool rotatedSystem, int nPoints ) {

  if( fDrawGraphYZ ) delete fDrawGraphYZ;
  fDrawGraphYZ = new TGraph(nPoints);

  for( int i=0 ; i<nPoints ; ++i ){
    float z    = zmin + i*(zmax-zmin)/(nPoints-1);
    float y    = InterpolateToZ(z).Y();
    float xPad = rotatedSystem ? -z : y;
    float yPad = rotatedSystem ?  y : z;
    fDrawGraphYZ->SetPoint(i,xPad,yPad);
  }

  fDrawGraphYZ->Draw("P");
}

/** Turn out-of-bounds warning on/off globally
 *
 * \param[in] value true to turn warnings on, false to turn warnings off
 */

void ACsoft::Analysis::SplineTrack::setWarnOutOfBounds(bool value) {
  ACsoft::Analysis::SplineTrack::WarnOutOfBounds = value;
}

