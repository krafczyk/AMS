#include "TrdDetector.hh"

#include "AMSGeometry.h"
#include "TrdModule.hh"
#include "TrdLayer.hh"
#include "TrdSublayer.hh"
#include "TrdStraw.hh"
#include "FileManager.hh"
#include "TimeHistogramManager.hh"
#include "Settings.h"

#include "dumpstreamers.hh"

#include <TH3F.h>
#include <TFile.h>
#include <TTimeStamp.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TArrow.h>
#include <TText.h>

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <math.h>
#include <assert.h>

#define DEBUG 0
#define INFO_OUT_TAG "Trd> "
#include <debugging.hh>

ACsoft::Detector::Trd::Trd( bool applyShimmings )
{

  fNominalPosition = TVector3(0.0,0.0,ACsoft::AC::AMSGeometry::ZTRDCenter);
  fOffsetPosition  = TVector3(0.0,0.0,0.0);
  fNominalRotation.SetToIdentity();
  fExtraRotation.SetToIdentity();

  if( applyShimmings )
    INFO_OUT << "Constructing TRD with shimming corrections..." << std::endl;
  else
    INFO_OUT << "Constructing TRD without shimming corrections..." << std::endl;

  ConstructTrdLayers();
  FillConvenienceVectors();

  if( applyShimmings ){
    ReadTrdShimmingGlobalFile();
    ReadTrdShimmingModuleFile();
  }


  // trigger recalculation of geometry
  ChangePositionAndRotation(TVector3(),TRotation());
}

ACsoft::Detector::Trd::~Trd()
{
  for( unsigned int i = 0 ; i < fLayers.size() ; ++i )
    delete fLayers[i];
}


void ACsoft::Detector::Trd::ConstructTrdLayers() {

  // create layers, which will in turn create the additional substructure (sublayers, modules, straws)
  for( unsigned int lay=0 ; lay < ACsoft::AC::AMSGeometry::TRDLayers ; ++lay ){

    fLayers.push_back( new ACsoft::Detector::TrdLayer(lay,this) );
  }

  // trigger calculation of global positions and rotations for all substructure
  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    ACsoft::Detector::TrdLayer* layer = fLayers[i];
    layer->UpdateGlobalPositionAndDirection();
  }
}


void ACsoft::Detector::Trd::FillConvenienceVectors() {

  fSublayers.clear();
  fModules.clear();
  fStraws.clear();

  fSublayers.assign(ACsoft::AC::AMSGeometry::TRDSubLayers,0);
  fModules.assign(ACsoft::AC::AMSGeometry::TRDModules,0);
  fStraws.assign(ACsoft::AC::AMSGeometry::TRDStraws,0);

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    ACsoft::Detector::TrdLayer* layer = fLayers[i];

    for( unsigned int j=0 ; j<layer->NumberOfSublayers() ; ++j ){

      ACsoft::Detector::TrdSublayer* sublayer = layer->GetSublayer(j);
      int globalSublayerNumber = sublayer->GlobalSublayerNumber();
      fSublayers.at(globalSublayerNumber) = sublayer;

      for( unsigned int m = 0 ; m<sublayer->NumberOfModules() ; ++m ){
        ACsoft::Detector::TrdModule* module = sublayer->GetModule(m);
        int moduleNumber = module->ModuleNumber();
        fModules.at(moduleNumber) = module;

        for( unsigned int st = 0 ; st < module->NumberOfStraws() ; ++st ){
          ACsoft::Detector::TrdStraw* straw = module->GetTrdStraw(st);
          int globalStrawNumber = straw->GlobalStrawNumber();
          fStraws.at(globalStrawNumber) = straw;
        }
      }
    }
  }
}




static inline void VerifySettingsInFile(const std::string& fileIdentifier, std::ifstream& file, const std::string& expectedGitSHA, unsigned short expectedVersion) {

  std::string gitSHA;
  unsigned short version = 0;
  file >> gitSHA >> version;

  if (gitSHA != expectedGitSHA) {
    WARN_OUT << "ERROR validating " << fileIdentifier << ". File mismatch! Expected Git SHA: \"" << expectedGitSHA << "\". Actual Git SHA: \"" << gitSHA << "\"" << std::endl;
    throw std::runtime_error("ERROR validating " + fileIdentifier);
  }

  if (version != expectedVersion) {
    WARN_OUT << "ERROR validating " << fileIdentifier << ". File mismatch! Expected version: \"" << expectedVersion << "\". Actual version: \"" << version << "\"" << std::endl;
    throw std::runtime_error("ERROR validating " + fileIdentifier);
  }
}

#define VALIDATE_LOOKUP_FILE(FileIdentifier, VariableName) \
  VerifySettingsInFile(FileIdentifier, file, ::AC::Settings::VariableName##ExpectedGitSHA, ::AC::Settings::VariableName##ExpectedVersion)


void ACsoft::Detector::Trd::ReadTrdShimmingGlobalFile() {

  char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    WARN_OUT << "ACROOTSOFTWARE variable not set." << std::endl;
    throw std::runtime_error("ACROOTSOFTWARE variable not set.");
    return;
  }

  std::string filePath(acrootsoftware);
  filePath += "/acroot/data/";

  std::string fname = filePath + ::AC::Settings::gTrdQtShimmingGlobalFileName;
  INFO_OUT << "Read global TRD shimming from file " << fname << std::endl;

  std::ifstream file(fname.c_str());
  if (!file.good()){
    WARN_OUT << "ERROR opening file " << fname << std::endl;
    throw std::runtime_error("ERROR opening TRD global shimming file.");
  }

  VALIDATE_LOOKUP_FILE("TRDShimmingGlobalFile", gTrdQtShimmingGlobalFileName);


  // Global TRD X/Y/Z shifts [cm]
  float TRDShifts[3];

 // Global TRD phi/theta/psi rotations (Euler angles in Landau/Lifshitz definition, see TRotation documentation) [rad]
  float TRDRotations[3];

  file >> TRDShifts[0] >> TRDShifts[1] >> TRDShifts[2]
       >> TRDRotations[0] >> TRDRotations[1] >> TRDRotations[2];

  file.close();

  DEBUG_OUT << "TRDShifts,    x=  " << TRDShifts[0]    << " y=    " << TRDShifts[1]    << " z=  " << TRDShifts[2]    << " cm" << std::endl;
  DEBUG_OUT << "TRDRotations, phi=" << TRDRotations[0] << " theta=" << TRDRotations[1] << " psi=" << TRDRotations[2] << " rad" << std::endl;


  fNominalPosition += TVector3(TRDShifts[0],TRDShifts[1],TRDShifts[2]);
  fNominalRotation.SetXEulerAngles(TRDRotations[0],TRDRotations[1],TRDRotations[2]);

}

void ACsoft::Detector::Trd::ReadTrdShimmingModuleFile() {

  char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    WARN_OUT << "ACROOTSOFTWARE variable not set." << std::endl;
    throw std::runtime_error("ACROOTSOFTWARE variable not set.");
    return;
  }

  std::string filePath(acrootsoftware);
  filePath += "/acroot/data/";

  std::string fname = filePath + ::AC::Settings::gTrdQtShimmingModuleFileName;
  INFO_OUT << "Read TRD module shimming from file " << fname << std::endl;

  std::ifstream file(fname.c_str());
  if (!file.good()){
    WARN_OUT << "ERROR opening file " << fname << std::endl;
    throw std::runtime_error("ERROR opening TRD module shimming file.");
  }

  VALIDATE_LOOKUP_FILE("TRDShimmingModuleFile", gTrdQtShimmingModuleFileName);

  int Imod;
  float Imod_Dz, Imod_Arz;

  while (file.good()) {
    file >> Imod >> Imod_Dz >> Imod_Arz;
    if (file.eof())
      break;

    ACsoft::Detector::TrdModule* module = GetTrdModule(Imod-1); // shimming file still uses 1..328 convention

    DEBUG_OUT << "module number: " << Imod << " dz: " << std::setw(10) << Imod_Dz << " um, rot-z: " << std::setw(10) << Imod_Arz << " urad" << std::endl;

    module->SetShimmingOffset(TVector3(0.0,0.0,1.e-4*Imod_Dz));
    TRotation shimrot;
    if(module->Direction()==ACsoft::AC::YZMeasurement)
      Imod = -Imod; // FIXME sign convention in shimming file is inconsistent!
    shimrot.RotateX(1.e-6*Imod_Arz);
    module->SetShimmingRotation(shimrot);
  }

  file.close();
}



/** Change the alignment parameters for the TRD and trigger
  * a recalculation of the global coordinates of all the layers.
  *
  * The nominal position and orientation of the TRD will remain unchanged.
  * This function sets the additional offset and rotation due to alignment.
  *
  * The recalculation of the parameters for the layers will automatically trigger a recalculation
  * also for all other components further down the logical detector hierarchy.
  *
  */
void ACsoft::Detector::Trd::ChangePositionAndRotation( const TVector3& offsetPos, const TRotation& extraRot ) {

  fOffsetPosition = offsetPos;
  fExtraRotation  = extraRot;

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    ACsoft::Detector::TrdLayer* layer = fLayers[i];
    layer->UpdateGlobalPositionAndDirection();
  }
}


TCanvas* ACsoft::Detector::Trd::DrawProjections( float xcenter, float ycenter, float xywidth, bool rotated, std::string canvasname ) {

  DEBUG_OUT << "x " << xcenter << " y " << ycenter << " xywidth " << xywidth << std::endl;

  const float zoffsetTop = 2.0; // cm
  const float zoffsetLow = 6.0; // cm
  float zmin = GetTrdLayer( 0)->GlobalPosition().Z() - zoffsetLow;
  float zmax = GetTrdLayer(19)->GlobalPosition().Z() + zoffsetTop;

  float xmin = xcenter - 0.5*xywidth;
  float xmax = xcenter + 0.5*xywidth;
  float ymin = ycenter - 0.5*xywidth;
  float ymax = ycenter + 0.5*xywidth;

  float xminstraw = xmin + ACsoft::AC::AMSGeometry::TRDTubeRadius;
  float xmaxstraw = xmax - ACsoft::AC::AMSGeometry::TRDTubeRadius;
  float yminstraw = ymin + ACsoft::AC::AMSGeometry::TRDTubeRadius;
  float ymaxstraw = ymax - ACsoft::AC::AMSGeometry::TRDTubeRadius;

  float canvasHeight = gStyle->GetCanvasDefH(); // fix this, adjust width so that aspect ratio is roughly in the two pads
  float canvasWidth  = 2.0 * canvasHeight * xywidth / (zmax-zmin);

  if(rotated){
    canvasWidth  = gStyle->GetCanvasDefW();
    canvasHeight = 2.0 * canvasWidth * xywidth / (zmax-zmin);
  }

  DEBUG_OUT << "canvas pixel size: (" << canvasWidth << " x " << canvasHeight << ")" << std::endl;

  if( canvasname == "" ) canvasname = "trdCanvas";
  TCanvas* c = new TCanvas( canvasname.c_str(), "TRD", canvasWidth, canvasHeight );
  if(!rotated)
    c->Divide(2,1);
  else
    c->Divide(1,2);

  c->cd(1);
  if(!rotated)
    gPad->Range(xmin,zmin,xmax,zmax);
  else
    gPad->Range(-zmax,xmin,-zmin,xmax);

  c->cd(2);
  if(!rotated)
    gPad->Range(ymin,zmin,ymax,zmax);
  else
    gPad->Range(-zmax,ymin,-zmin,ymax);


  for( unsigned int i = 0 ; i<ACsoft::AC::AMSGeometry::TRDStraws ; ++i ){
    ACsoft::Detector::TrdStraw* trdstraw = GetTrdStraw(i);
    TVector3 strawPos = trdstraw->GlobalPosition();
    if( trdstraw->Direction() == ACsoft::AC::XZMeasurement ){
      if( strawPos.X() > xminstraw && strawPos.X() < xmaxstraw ){
        c->cd(1);
        trdstraw->Draw(rotated);
      }
    }
    else if( trdstraw->Direction() == ACsoft::AC::YZMeasurement ){
      if( strawPos.Y() > yminstraw && strawPos.Y() < ymaxstraw ){
        c->cd(2);
        trdstraw->Draw(rotated);
      }
    }
  }


  // draw coordinate system
  float arrowOffset = 0.5;
  float arrowLength = 2.0;
  float arrowSize   = 0.02;
  float textOffset  = 0.3;
  float xaxpad1 = rotated ? -(zmin+arrowOffset) : xmin + arrowOffset;
  float xaxpad2 = rotated ? -(zmin+arrowOffset) : xmin + arrowOffset + arrowLength;
  float xaypad1 = rotated ?  xmin + arrowOffset : zmin + arrowOffset;
  float xaypad2 = rotated ?  xmin + arrowOffset + arrowLength : zmin + arrowOffset;
  float yaxpad1 = rotated ? -(zmin+arrowOffset) : ymin + arrowOffset;
  float yaxpad2 = rotated ? -(zmin+arrowOffset) : ymin + arrowOffset + arrowLength;
  float yaypad1 = rotated ?  ymin + arrowOffset : zmin + arrowOffset;
  float yaypad2 = rotated ?  ymin + arrowOffset + arrowLength : zmin + arrowOffset;
  float zxaxpad1 = rotated ? -(zmin+arrowOffset): xmin + arrowOffset;
  float zxaxpad2 = rotated ? -(zmin+arrowOffset+arrowLength): xmin + arrowOffset;
  float zxaypad1 = rotated ? xmin + arrowOffset: zmin + arrowOffset;
  float zxaypad2 = rotated ? xmin + arrowOffset: zmin + arrowOffset + arrowLength;
  float zyaxpad1 = rotated ? -(zmin+arrowOffset): ymin + arrowOffset;
  float zyaxpad2 = rotated ? -(zmin+arrowOffset+arrowLength): ymin + arrowOffset;
  float zyaypad1 = rotated ? ymin + arrowOffset: zmin + arrowOffset;
  float zyaypad2 = rotated ? ymin + arrowOffset: zmin + arrowOffset + arrowLength;

  TArrow* xArrow = new TArrow(xaxpad1,xaypad1,xaxpad2,xaypad2,arrowSize);
  TArrow* yArrow = new TArrow(yaxpad1,yaypad1,yaxpad2,yaypad2,arrowSize);
  TArrow* zxArrow = new TArrow(zxaxpad1,zxaypad1,zxaxpad2,zxaypad2,arrowSize);
  TArrow* zyArrow = new TArrow(zyaxpad1,zyaypad1,zyaxpad2,zyaypad2,arrowSize);

  float xtxpad = rotated ? xaxpad2 : xaxpad2 + textOffset;
  float xtypad = rotated ? xaypad2 + textOffset : xaypad2;
  float ytxpad = rotated ? yaxpad2 : yaxpad2 + textOffset;
  float ytypad = rotated ? yaypad2 + textOffset : yaypad2;
  float zxtxpad = rotated ? zxaxpad2 : zxaxpad2 + textOffset;
  float zxtypad = rotated ? zxaypad2 + textOffset : zxaypad2;
  float zytxpad = rotated ? zyaxpad2 : zyaxpad2 + textOffset;
  float zytypad = rotated ? zyaypad2 + textOffset : zyaypad2;

  TText* xLabel = new TText(xtxpad,xtypad,"x");
  TText* yLabel = new TText(ytxpad,ytypad,"y");
  TText* zxLabel = new TText(zxtxpad,zxtypad,"z");
  TText* zyLabel = new TText(zytxpad,zytypad,"z");

  c->cd(1);
  xArrow->Draw();
  zxArrow->Draw();
  xLabel->Draw();
  zxLabel->Draw();
  c->cd(2);
  yArrow->Draw();
  zyArrow->Draw();
  yLabel->Draw();
  zyLabel->Draw();

  // FIXME still to do: add a nice TPaletteAxis (color scale for hit amplitude)

  return c;
}

TVector3 ACsoft::Detector::Trd::HitPosition( const ACsoft::AC::TRDRawHit* hit ) const {

  Short_t strawNumber = hit->Straw();
  const ACsoft::Detector::TrdStraw* straw = GetTrdStraw(strawNumber);
  return straw->GlobalPosition();
}

TVector3 ACsoft::Detector::Trd::HitDirection( const ACsoft::AC::TRDRawHit* hit ) const {

  Short_t strawNumber = hit->Straw();
  const ACsoft::Detector::TrdStraw* straw = GetTrdStraw(strawNumber);
  return straw->GlobalWireDirection();
}


void ACsoft::Detector::Trd::Dump() const {

  INFO_OUT << "TRD position: " << fNominalPosition << " + " << fOffsetPosition << " rotation: " << fExtraRotation << " * " << fNominalRotation << std::endl;

  for( unsigned int i = 0 ; i < fLayers.size() ; ++i ){

    ACsoft::Detector::TrdLayer* layer = fLayers[i];
    layer->Dump();
  }
}

