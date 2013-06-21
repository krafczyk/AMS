#include "Utilities.hh"

#include <iostream>
#include <math.h>
#include <TMath.h>
#include <TMatrixDEigen.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TClass.h>
#include <TCanvas.h>


#define DEBUG 0
#define INFO_OUT_TAG "Utilities> "
#include <debugging.hh>

namespace ACsoft {

namespace Utilities {


TH1* GetHisto( TFile* file, const char* histoName ) {

  TH1* histo = (TH1*)file->Get(histoName);
  if( !histo ){
    WARN_OUT << "ERROR: Histogram " << histoName << " not found!" << std::endl;
    return 0;
  }

  DEBUG_OUT << "Found histo " << histo->GetName() << " of type " << histo->IsA()->GetName() << " with " << histo->GetEntries() << " entries" << std::endl;

  return histo;
}

void SaveCanvas(TCanvas* canvas) {

  if(!canvas) return;

  std::string fileName(canvas->GetName());

  INFO_OUT << "Saving canvas " << fileName << ".png/root..." << std::endl;
  canvas->Update();
  canvas->SaveAs((fileName + std::string(".root")).c_str());
  canvas->SaveAs((fileName + std::string(".png")).c_str());
}

double FastLineFit(std::vector<double> &x, std::vector<double> &y, std::vector<double> &ey, double &offset, double &e_offset, double &slope, double &e_slope, double &correlation) {

  double s    = 0.0;
  double sx   = 0.0;
  double sy   = 0.0;
  double sxx  = 0.0;
  double sxy  = 0.0;

  static int nWarnings    =  1;
  static int maxWarnings  =  1;

#ifdef AMS_ACQT_SILENCE_COMMON_WARNINGS
  maxWarnings = 0;
#endif

  for (unsigned int i=0; i<x.size(); i++) {
    if (ey.at(i) <= 0.0) {
      nWarnings++;
      if (nWarnings<maxWarnings) {
        WARN_OUT << "Error for point i=" << i << " ey=" << ey.at(i) << "<=0.0" << std::endl;
      } else if (nWarnings==maxWarnings) {
        WARN_OUT << "Maximum number of warnings reached !!!" << std::endl;
      }
      return -1.0;
    }
    double InvError2 = 1.0/pow(ey.at(i),2);
    s   += InvError2;
    sx  += x.at(i)*InvError2;
    sy  += y.at(i)*InvError2;;
    sxx += pow(x.at(i),2)*InvError2;
    sxy += (x.at(i)*y.at(i))*InvError2;
  }

  double delta = s*sxx - sx*sx;
  if (delta == 0.0) {
    nWarnings++;
    if (nWarnings<maxWarnings) {
      WARN_OUT << "Error delta==0, no solution for line fit!" <<  std::endl;
      for (unsigned int i=0; i<x.size(); i++) {
        printf("i=%3d x=%12.4E y=%12.4E ey=%12.4E \n",i,x.at(i),y.at(i),ey.at(i));
      }
    } else if (nWarnings==maxWarnings) {
      WARN_OUT << "Maximum number of warnings reached !!!" << std::endl;
    }
    return -2.0;
  }

  offset         = (sxx*sy - sx*sxy)/delta;
  slope         = (s*sxy - sx*sy)/delta;
  e_offset      = sqrt(sxx/delta);
  e_slope        = sqrt(s/delta);
  correlation    = -sx/sqrt(s*sxx);

  double chiq = 0.0;
  for (unsigned int i=0; i<x.size(); i++) chiq += pow((y.at(i)-(offset+slope*x.at(i)))/ey.at(i),2);

  return chiq;
}

/** Test if the binning of two histogram axes is identical. */
bool TestBinning( TAxis* axis1, TAxis* axis2, double epsilon ) {

  if( axis1->GetNbins() != axis2->GetNbins() ) return false;

  for( int i=1 ; i<=axis1->GetNbins() ; ++i ){
    if(fabs(axis1->GetBinLowEdge(i)-axis2->GetBinLowEdge(i)) > epsilon ) return false;
    if(fabs(axis1->GetBinUpEdge(i)-axis2->GetBinUpEdge(i)) > epsilon ) return false;
  }

  return true;
}

/** Test if the binning of two histograms is identical. */
bool TestBinning( TH1* h1, TH1* h2, double epsilon ) {

  return TestBinning(h1->GetXaxis(),h2->GetXaxis(),epsilon);
}


void NormalizeHistogramXSlices( TH2* histo ) {

  int nBinsX = histo->GetNbinsX();
  int nBinsY = histo->GetNbinsY();

  for( int iBinX=1; iBinX<=nBinsX; ++iBinX ) {

    double xc = histo->GetXaxis()->GetBinCenter(iBinX);
    std::stringstream name;
    name << histo->GetName() << "_" << iBinX << "_" << xc;
    TH1D* hp = histo->ProjectionY(name.str().c_str(),iBinX,iBinX);
    double sum = hp->Integral();
    if (sum<=0.0){
      WARN_OUT << "Non-positive integral in projection " << hp->GetName() << std::endl;
      continue;
    }
    if (iBinX==1) hp->Sumw2();

    hp->Scale(1.0/sum);

    for (int iBinY=1; iBinY<=nBinsY; iBinY++) {
      double Value = hp->GetBinContent(iBinY);
      double eValue = hp->GetBinError(iBinY);
      histo->SetBinContent(iBinX,iBinY,Value);
      histo->SetBinError(iBinX,iBinY,eValue);
    }
    delete hp; hp = 0;
  }
}


/** Create a XZ projection of a 3D histogram.
  *
  * \param[in] histo 3D-histogram to project
  * \param[in] projectionType Specify which projection to calculate, valid values are "XZ", "YZ", and "XY"
  * \param[in] firstBin first bin on projected axis to be used for projection (use firstBin=lastBin=0 for all bins)
  * \param[in] lastBin  last bin on projected axis to be used for projection (use firstBin=lastBin=0 for all bins)
  * \param[in] title title for projected histogram, will be determined automatically if none given
  * \param[in] name name for projected histogram, will be determined automatically if none given
  *
  */
TH2* Projection2D(TH3* histo, ProjectionType projectionType, int firstBin, int lastBin, std::string title, std::string name ) {

  if(!histo) {
    WARN_OUT << "Received null-pointer!" << std::endl;
    return 0;
  }

  const TAxis* axis1 = 0;
  const TAxis* axis2 = 0;
  const TAxis* paxis = 0;
  std::string tag;

  if( projectionType == Utilities::XZ ){
    axis1 = histo->GetXaxis();
    axis2 = histo->GetZaxis();
    paxis = histo->GetYaxis();
    tag = "XZ";
  }
  else if( projectionType == Utilities::YZ ){
    axis1 = histo->GetYaxis();
    axis2 = histo->GetZaxis();
    paxis = histo->GetXaxis();
    tag = "YZ";
  }
  else{
    axis1 = histo->GetXaxis();
    axis2 = histo->GetYaxis();
    paxis = histo->GetZaxis();
    tag = "XY";
  }


  const Double_t* binArray1 = axis1->GetXbins()->GetArray();
  const Double_t* binArray2 = axis2->GetXbins()->GetArray();
  if( !binArray1 || !binArray2 ) {
    WARN_OUT << "Bin array not available for 3D histo " << histo->GetName() << std::endl;
    return 0;
  }

  int nBinsAxis1 = axis1->GetNbins();
  int nBinsAxis2 = axis2->GetNbins();

  int StartBin = 1;
  int EndBin   = paxis->GetNbins();
  if( firstBin>0 ) StartBin = firstBin;
  if( lastBin >0 ) EndBin   = lastBin;

  std::stringstream name2d, title2d;
  if( name == "" ){
    name2d << histo->GetName() << "_" << tag;
    if( firstBin>0 || lastBin>0 )
      name2d << "_" << StartBin << "_" << EndBin;
  }
  else
    name2d << name;
  if( title == "" ){
    title2d << histo->GetTitle() << " (" << tag << " projection)";
  }
  else
    title2d << title;

  TH2* h2d = 0;
  if( histo->IsA()->GetName() == std::string("TH3F") )
    h2d = new TH2F(name2d.str().c_str(),title2d.str().c_str(),nBinsAxis1,&(binArray1[0]),nBinsAxis2,&(binArray2[0]));
  else if( histo->IsA()->GetName() == std::string("TH3D") )
    h2d = new TH2D(name2d.str().c_str(),title2d.str().c_str(),nBinsAxis1,&(binArray1[0]),nBinsAxis2,&(binArray2[0]));
  else if( histo->IsA()->GetName() == std::string("TH3C") )
    h2d = new TH2C(name2d.str().c_str(),title2d.str().c_str(),nBinsAxis1,&(binArray1[0]),nBinsAxis2,&(binArray2[0]));
  else if( histo->IsA()->GetName() == std::string("TH3S") )
    h2d = new TH2S(name2d.str().c_str(),title2d.str().c_str(),nBinsAxis1,&(binArray1[0]),nBinsAxis2,&(binArray2[0]));
  else if( histo->IsA()->GetName() == std::string("TH3I") )
    h2d = new TH2I(name2d.str().c_str(),title2d.str().c_str(),nBinsAxis1,&(binArray1[0]),nBinsAxis2,&(binArray2[0]));
  else return 0;

  h2d->GetXaxis()->SetTitle(axis1->GetTitle());
  h2d->GetYaxis()->SetTitle(axis2->GetTitle());

  for( int iBin1=1; iBin1<=nBinsAxis1; ++iBin1 ) {

    double p1 = axis1->GetBinCenter(iBin1);

    for( int iBin2=1; iBin2<=nBinsAxis2; ++iBin2 ) {

      double p2 = axis2->GetBinCenter(iBin2);

      for( int iBinP=StartBin; iBinP<=EndBin; ++iBinP ) {

        double value = 0.0;

        if( projectionType == Utilities::XZ )
          value = histo->GetBinContent(iBin1,iBinP,iBin2);
        else if( projectionType == Utilities::YZ )
          value = histo->GetBinContent(iBinP,iBin1,iBin2);
        else
          value = histo->GetBinContent(iBin1,iBin2,iBinP);

        h2d->Fill(p1,p2,value);
      }
    }
  }

  h2d->SetStats(0);

  return h2d;
}


TVector3 TransformAMS02toMAG(const double &yaw, const double &pitch, const double &roll, const double &altitude, const double &phi, const double &theta, const double &velPhi, const double &velTheta, const TVector3 &inputVector)
{
  TVector3 event = inputVector;
  // Particle direction is given as an input: inputVector
  // needed ISS information: position in LVLH (yaw, pitch, roll), position in GTOD (altitude, theta, phi), and velocity direction in GTOD (velocity theta, velocity phi)
  // all vectors are/should be normalised

  // The Space Station Coordinate system is a body-fixed right-handed Catesian system.
  // The X_ISS axis is parallel to the longitudinal axis of the module cluster. The forward direction (+X_ISS) is called Ram side. The backward direction (-X_ISS) is called Wake side.
  // The Y_ISS axis is parallel to the axis of the module cluster. The positive direction (+Y_ISS) is called Starboard, the negative direction(-Y_ISS) is called Port.
  // The Z_ISS axis points to the Nadir direction (+Z_ISS). The zenith is the negative side (-Z_ISS).

  // AMS-02 is located on the Starboard side of the ISS.
  // AMS-02 has its own Cartesian right-handed coordinate system.
  // The origin is located at the geometric center of the tracker detector.
  // The X_AMS axis points towards the port direction (=-Y_ISS).
  // The Y_AMS axis points towards the wake side (=-X_ISS).
  // The Z_AMS axis points towards the zenith (=-Z_ISS).
  // Initialise the AMS02 reference frame
  TMatrix AMS02toISS(3, 3);
  AMS02toISS(0, 0) = 0.;
  AMS02toISS(0, 1) = -1.;
  AMS02toISS(0, 2) = 0.;
  AMS02toISS(1, 0) = -1.;
  AMS02toISS(1, 1) = 0.;
  AMS02toISS(1, 2) = 0.;
  AMS02toISS(2, 0) = 0.;
  AMS02toISS(2, 1) = 0.;
  AMS02toISS(2, 2) = -1.;

  // Transform event direction from AMS to ISS system
  // AMS is tilted of about 12 degrees with respect to the axis perpendicular to the ISS structure
  event = AMS02toISS*inputVector;
  event.RotateX(-12.*TMath::DegToRad());

  // Local Vertical Local Horizontal (LVLH)
  // The LVLH frame is a quasi-inertial, right-handed Cartesian system with the origin located in the center of mass of the object (i.e., the ISS).
  // To define a relationship between the ISS reference frame and the flight orientation is is necessaR_Y to give three attitude angles called Yaw, Pich, and Roll, around the Z_ISS, Y_ISS, and X_ISS axes respectively.
  // To pass from the LVLH reference system to the ISS one, one has to apply the following sequence: Yaw, Pitch, Roll.

  // Apply ISS to LVLH transformation
  event.RotateX(roll);
  event.RotateY(pitch);
  event.RotateZ(yaw);

  // Greenwich True Of Date (GTOD)
  // The GTOD system is a Geocentric, Earth-fixed frame.
  // The X_GTOD axis points towards the GTOD meridian and it lies on the Earth equatorial plane.
  // The Y_GTOD axis lies on the equatorial plane.
  // The Z_GTOD axis overlaps with the Earth rotational axis pointing to the North pole.

  // The LVLH is defined by means of the ISS radial position vector R and its velocity vector V given in GTOD.
  // The Z_GTOD lies along the geocentric radius vector to the vehicle R and is positive towards the centre of Earth.
  // The X_GTOD lies on the vertical orbital plane to the Z_GTOD axis and is positive in the of the vehicle motion.
  // The Y_GTOD is the cross product of the X_GTOD and Z_GTOD axes (Y_GTOD = Z_GTOD x X_GTOD)

  // First construct GTOD to LVLH matrix and then invert it
  TMatrix LVLHtoGTOD(3, 3);
  // Calculate the components of the R and V vectors
  TVector3 R, V;
  R.SetMagThetaPhi(1., TMath::Pi()*.5 + theta, phi + TMath::Pi());
  V.SetMagThetaPhi(1., TMath::Pi()*.5 - velTheta, velPhi);

  // Calculate each axis
  // new Z-axis: R pointing to the centre of Eearth
  LVLHtoGTOD(2,0) = R(0);
  LVLHtoGTOD(2,1) = R(1);
  LVLHtoGTOD(2,2) = R(2);

  // new X-axis: V
  LVLHtoGTOD(0,0) = V(0);
  LVLHtoGTOD(0,1) = V(1);
  LVLHtoGTOD(0,2) = V(2);

  // new Y-axis: cross product of R and V
  TVector3 ZxX = R.Cross(V);
  LVLHtoGTOD(1, 0) = ZxX(0);
  LVLHtoGTOD(1, 1) = ZxX(1);
  LVLHtoGTOD(1, 2) = ZxX(2);

  // now invert
  LVLHtoGTOD.Invert();

  // LVLH to GTOD
  event = LVLHtoGTOD*event;

  // GTOD to MAG
  // The MAG frame is shifted (~570 km) and rotated (~11 degrees) with respect to the GTOD frame
  // We apply first the shift and then rotation

  // Needed information about the geomagnetic field, approximated by a dipole field
  const double magneticNpole_theta = -1.399310;       // magnetic centred north pole latitude 2012 (rad)
  const double magneticNpole_phi = 1.877269;          // magnetic centred north pole longitude 2012 (rad)
  const double magneticDpolecenter_theta = 0.395086;    // dipole centre latitude 2012 (rad)
  const double magneticDpolecenter_phi = 2.639650;         // dipole centre longitude 2012 (rad)
  const double magneticDshift = 569.779321;           // dipole centre shift respect earth's center (km) 2012

  // Calculate magnetic Dipole center in GTOD
  TVector3 Dipole;
  Dipole.SetMagThetaPhi(1., TMath::Pi()*0.5 - magneticDpolecenter_theta, magneticDpolecenter_phi);

  // shift the event position from Earth centre to Dipole centre in GTOD
  // take ISS altitude. The error introduced with the altitude difference between ISS and AMS should be negligeable.
  double relative_X = altitude*event(0) - magneticDshift*Dipole(0);
  double relative_Y = altitude*event(1) - magneticDshift*Dipole(1);
  double relative_Z = altitude*event(2) - magneticDshift*Dipole(2);

  // normalise vector
  double length = sqrt(relative_X*relative_X + relative_Y*relative_Y + relative_Z*relative_Z);
  TVector3 EventFromDipoleCentre(relative_X/length, relative_Y/length, relative_Z/length);

  // Calculate magnetic North Pole in GTOD (We have to take into account that the geographic latitude is pi/2 - the cylindrical theta)
  TVector3 Npole;
  Npole.SetMagThetaPhi(1., TMath::Pi()*0.5 - magneticNpole_theta, magneticNpole_phi);

  // The geomagnetic coordinate system (MAG) defined so that its Z axis is parallel to the magnetic dipole axis.
  // The Y-axis of this system is perpendicular to the geographic poles such that if D is the dipole position and S is the south pole Y = D x S.
  // The X-axis completes a right-handed orthogonal set.

  TMatrix GTODtoMAG(3, 3);
  // new Z axis: parallel to dipole axis
  GTODtoMAG(2, 0) = Npole(0);
  GTODtoMAG(2, 1) = Npole(1);
  GTODtoMAG(2, 2) = Npole(2);

  // new Y axis: cross product of new Z axis and position of the South Pole (0, 0, -1)
  TVector3 geographicSpole;
  geographicSpole.SetMagThetaPhi(1., 180., 0.);
  TVector3 DxS = Npole.Cross(geographicSpole);
  DxS.SetMag(1.);

  GTODtoMAG(1, 0) = DxS(0);
  GTODtoMAG(1, 1) = DxS(1);
  GTODtoMAG(1, 2) = DxS(2);

  // X axis: cross product of new axis Y and Z
  GTODtoMAG(0,0) = GTODtoMAG(1,1)*GTODtoMAG(2,2) - GTODtoMAG(1,2)*GTODtoMAG(2,1);
  GTODtoMAG(0,1) = GTODtoMAG(1,2)*GTODtoMAG(2,0) - GTODtoMAG(1,0)*GTODtoMAG(2,2);
  GTODtoMAG(0,2) = GTODtoMAG(1,0)*GTODtoMAG(2,1) - GTODtoMAG(1,1)*GTODtoMAG(2,0);

  // The event direction in the MAG frame is the scalar product of MAG unit vectors and the shifted event direction
  TVector3 eventInMAG = GTODtoMAG*EventFromDipoleCentre;

  return eventInMAG;
}

/** function that follows the 1/beta curve for a given particle species,
  * defined by mass and charge.
  *
  * use the shift parameter to define a 1/beta band.
  */
double OneOverBetaCurves(double mass, double rigidity, double charge, double shift, double scale) {
  const double fabsRigidity = fabs(rigidity);
  const double fabsCharge = fabs(charge);
  return sqrt(1.0 + mass * mass / (fabsRigidity * fabsRigidity * fabsCharge * fabsCharge)) + scale * shift / (fabsRigidity * fabsCharge) + shift;
}

int RootColor(int i)
{
  if (0 <= i%16 && i%16 <= 7)
    return i+2;
  switch (i%16) {
    case  8: return 28;
    case  9: return 30;
    case 10: return 33;
    case 11: return 38;
  }
  if (12 <= i%16 && i%16 <= 15)
    return i+28;
  return 1;
}

// Work around missing GetPaintedGraph() accessor in AMS ROOT 5.2.7.
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,34,1)
TGraphAsymmErrors* GraphFromTEfficiency(TEfficiency* efficiency) { return efficiency->GetPaintedGraph(); }
#else
TGraphAsymmErrors* GraphFromTEfficiency(TEfficiency* efficiency) { return efficiency->fPaintGraph; }
#endif

}

}
