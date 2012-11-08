#include "AMSGeometry.h"
#include "Settings.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iomanip>
#include <math.h>
#include <TCutG.h>
#include <limits>
#include <algorithm>

#include <QFile>
#include <QStringList>
#include <QTextStream>

#define DEBUG 0
#define INFO_OUT_TAG "AMSGeom> "
#include <debugging.hh>

namespace AC {

AMSGeometry::AMSGeometry() {

 TRDFirstLayerContour = 0;
 TRDLastLayerContour = 0;

 TRDShifts[0]    =     0.e-4;  // global TRD X-Shift/cm
 TRDShifts[1]    =     0.e-4;  // global TRD Y-Shift/cm
 TRDShifts[2]    =  1000.e-4;  // global TRD Z-Shift/cm (1mm)
 TRDRotations[0] =     0.e-6;  // global TRD X-Rot/rad  (YZ-Tilt)
 TRDRotations[1] =     0.e-6;  // global TRD Y-Rot/rad  (XZ-Tilt)
 TRDRotations[2] =     0.e-6;  // global TRD Z-Rot/rad

 for (unsigned int ladder = 0; ladder < TRDLadders; ++ladder) {
   for (unsigned int layer = 0; layer < TRDLayers; ++layer)
     TRDLadderLayerToModuleLookupTable[ladder][layer] = -1;
 }

 for (unsigned int layer = 0; layer < TRDLayers; ++layer)
   TRDModulesPerLayer[layer] = 0;

 // for TRDrhit HADR -> Lay/Lad/Tube decoding:
 FillGeoArray();
}


/** Get a list of %TRD module numbers on given \p layer.
  *
  * \param[in] layer %TRD layer (0..19)
  *
  * \todo speed up this function or make it obsolete once a proper %TRD structure is in place
  */
std::vector<Short_t> AMSGeometry::TrdModulesOnLayer( Short_t layer ) {

  std::vector<Short_t> result;
  result.reserve(18);

  for( UShort_t mod = 0 ; mod < AC::AMSGeometry::TRDModules ; ++mod ){

    unsigned short modLayer, modLadder;
    AMSGeometry::Self()->TRDModuleToLadderAndLayer(mod, modLadder, modLayer);

    if( layer == modLayer )
      result.push_back(mod);
  }

  return result;
}

/** Get a list of %TRD module numbers on given \p layer and \p sublayer.
  *
  * \param[in] layer %TRD layer (0..19)
  * \param[in] sublayer %TRD sublayer (0..1)
  *
  * \todo speed up this function or make it obsolete once a proper %TRD structure is in place
  */
std::vector<Short_t> AMSGeometry::TrdModulesOnSublayer( Short_t layer, Short_t sublayer ) {

  std::vector<Short_t> modulesOnLayer = TrdModulesOnLayer( layer );
  std::vector<Short_t> modulesOnSublayer;
  for( unsigned int i=0 ; i<modulesOnLayer.size() ; ++i ){
    unsigned short ladder, layer;
    AMSGeometry::Self()->TRDModuleToLadderAndLayer(modulesOnLayer[i], ladder, layer);

    unsigned short tower = ladder;
    if (layer < 12) ++tower;
    if (layer < 4) ++tower;

    Short_t moduleSublayer = (tower % 2 == 0);
    if( moduleSublayer == sublayer )
      modulesOnSublayer.push_back(modulesOnLayer[i]);
  }

  return modulesOnSublayer;
}



void AMSGeometry::Init() {

  char* acrootsoftware = getenv("ACROOTSOFTWARE");
  if (!acrootsoftware) {
    WARN_OUT << "TRD Geometry information are NOT available. If this is a production build, everything is fine." << std::endl;
    return;
  }

  std::string filePath(acrootsoftware);
  filePath += "/acroot/data/";

  InitTRDGeometryFile(filePath);
  InitTRDShimmingModuleFile(filePath);
  InitTRDShimmingGlobalFile(filePath);

  InitTRDLadderLayerToModuleLookupTable();
  InitTRDStrawLookupTable();
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
  VerifySettingsInFile(FileIdentifier, file, AC::Settings::VariableName##ExpectedGitSHA, AC::Settings::VariableName##ExpectedVersion)

void AMSGeometry::InitTRDShimmingGlobalFile( const std::string& path) {

  std::string fname = path + Settings::gTrdQtShimmingGlobalFileName;
  INFO_OUT << "InitTRDShimmingGlobalFile from file " << fname << std::endl;

  std::ifstream file(fname.c_str());
  if (!file.good()){
    WARN_OUT << "ERROR opening TRDShimmingGlobalFile " << fname << std::endl;
    throw std::runtime_error("ERROR opening TRDShimmingGlobalFile.");
  }

  VALIDATE_LOOKUP_FILE("TRDShimmingGlobalFile", gTrdQtShimmingGlobalFileName);

  // FIXME: This file should be validated, if it's complete.
  file >> TRDShifts[0] >> TRDShifts[1] >> TRDShifts[2]
       >> TRDRotations[0] >> TRDRotations[1] >> TRDRotations[2];

  file.close();

  DEBUG_OUT << "TRDShifts,    x=" << 1e4*TRDShifts[0] << " y=" << 1e4*TRDShifts[1] << " z=" << 1e4*TRDShifts[2] << std::endl;
  DEBUG_OUT << "TRDRotations, x=" << 1e6*TRDRotations[0] << " y=" << 1e6*TRDRotations[1] << " z=" << 1e6*TRDRotations[2] << std::endl;
}

void AMSGeometry::InitTRDShimmingModuleFile( const std::string& path ) {

  std::string fname = path + Settings::gTrdQtShimmingModuleFileName;
  INFO_OUT << "InitTRDShimmingModuleFile from file " << fname << std::endl;

  std::ifstream file(fname.c_str());
  if (!file.good()){
    WARN_OUT << "ERROR opening TRDShimmingModuleFile " << fname << std::endl;
    throw std::runtime_error("ERROR opening TRDShimmingModuleFile.");
  }

  VALIDATE_LOOKUP_FILE("TRDShimmingModuleFile", gTrdQtShimmingModuleFileName);

  int Imod;
  float Imod_Dz, Imod_Arz;

  while (file.good()) {
    file >> Imod >> Imod_Dz >> Imod_Arz;
    if (file.eof())
      break;

    Mod_Dz[ Imod-1] = Imod_Dz;   // shimming file still uses 1..328 convention
    Mod_Arz[Imod-1] = Imod_Arz;

    // FIXME: Use better names instad of Mod_Dz, Mod_Arz, also print out units etc.
    DEBUG_OUT << "module number: " << Imod << " dz: " << std::setw(10) << Imod_Dz << std::setw(10) <<  " arz: " << Imod_Arz << std::endl;
  }

  file.close();
}

static inline unsigned short ParseTRDModuleContoursIdentifier(const QString& moduleIdentifier, unsigned int line, unsigned int sublayer) {

  bool ok = false;
  unsigned short iModuleNumber = moduleIdentifier.toUShort(&ok);
  if (!ok || iModuleNumber - 1u >= AMSGeometry::TRDModules) { // FIXME does this line make sense?
    WARN_OUT << "ERROR while parsing TRD Module Contours file. Module " << qPrintable(moduleIdentifier)
             << " in line " << line + 1 << " for sublayer " << sublayer + 1 << " is unknown." << std::endl;
    throw std::runtime_error("ERROR processing TRD Module Contours file.");
  }
  return iModuleNumber;
}

void AMSGeometry::InitTRDGeometryFile( const std::string& path ) {

  std::string fname = path + Settings::gTrdQtGeometryFileName;
  INFO_OUT << "InitTRDModuleContoursFile from file " << fname << std::endl;

  QFile file(QString::fromStdString(fname));
  if (!file.open(QIODevice::ReadOnly)) {
    WARN_OUT << "ERROR opening TRD Module Contours file " << fname << std::endl;
    throw std::runtime_error("ERROR opening TRD Module Contours file.");
  }

  std::vector<QStringList> perLayerContent;

  bool accumulatingLayerContent = false;
  QStringList layerContent;
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    QStringList tokens = line.split(";");
    if (tokens.size() != 10) continue;

    // If tokens[2] is not empty, and we've already seen the beginning of a layer "L<XXX> O/U"
    // then just record the content until the next layer is reached.
    QString layerIdentifier = tokens[0];
    if (!tokens[2].isEmpty() && accumulatingLayerContent) {
      layerContent.append(line);
      continue;;
    }

    // We've reached a new layer. If we already recorded data for a previous layer, clear
    // the temporary 'layerContent' and push the record into perLayerContent.
    if (!layerContent.isEmpty()) {
      perLayerContent.push_back(layerContent);
      layerContent.clear();
    }

    // A valid layer must look like "<XXX> O" or "<XXX> U".
    QStringList layerTokens = layerIdentifier.split(" ");
    if (layerTokens.size() != 2) continue;
    if (layerTokens[1] != "O" && layerTokens[1] != "U") continue;

    // Start accumulating data until the next layer is reached.
    if (!accumulatingLayerContent) {
      layerContent.append(line);
      accumulatingLayerContent = true;
    }
  }

  // Record the last layer content.
  if (!layerContent.isEmpty()) perLayerContent.push_back(layerContent);

  if (perLayerContent.size() != TRDSubLayers) {
    WARN_OUT << "ERROR while parsing TRD Module Contours file. Expecting "
             << TRDSubLayers  << " sublayer descriptions. Actual " << perLayerContent.size() << std::endl;
    throw std::runtime_error("ERROR processing TRD Module Contours file.");
  }

  // Crawl through the list of sublayer, we expect to see "L1 O", then "L1 U", followed by "L2 O", etc..
  for (unsigned int sublayer = 0; sublayer < perLayerContent.size(); ++sublayer) {
    TRDModuleGeometry geometry;
    geometry.layer = ceil((sublayer + 1) / 2.0) - 1;
    geometry.sublayer = sublayer;
    geometry.moduleLength = -999;

    if (perLayerContent[sublayer].size() < 36) {
        WARN_OUT << "ERROR while parsing TRD Module Contours file. Expecting at least 36 descriptors for sublayer " << sublayer + 1
                 << ", got " << perLayerContent[sublayer].size() << std::endl;
        throw std::runtime_error("ERROR processing TRD Module Contours file.");
    }

    QString sublayerIdentifier;
    unsigned short iModuleNumber = std::numeric_limits<unsigned short>::max() - 1;
    for (int i = 0; i < perLayerContent[sublayer].size(); ++i) {
      QStringList tokens = perLayerContent[sublayer][i].split(";");
      if (tokens.size() != 10) {
        WARN_OUT << "ERROR while parsing TRD Module Contours file. Expecting 10 tokens for sublayer " << sublayer + 1
                 << " in line " << i + 1 << ", got " << tokens.size() << std::endl;
        throw std::runtime_error("ERROR processing TRD Module Contours file.");
      }

      // Check that the first line of each perLayerContent describes the layer we expect it to.
      if (!i) {
        sublayerIdentifier = tokens[0];
        QString suffix = (sublayer % 2 == 0) ? " O" : " U";
        QString expectedIdentifier = "L" + QString::number(ceil((sublayer + 1) / 2.0)) + suffix;

        if (sublayerIdentifier != expectedIdentifier) {
          WARN_OUT << "ERROR while parsing TRD Module Contours file. Expecting sublayer " << qPrintable(expectedIdentifier)
                    << " in line " << i + 1 << " for sublayer " << sublayer + 1 << ", got " << qPrintable(sublayerIdentifier) << std::endl;
          throw std::runtime_error("ERROR processing TRD Module Contours file.");
        }

        iModuleNumber = ParseTRDModuleContoursIdentifier(tokens[1], i, sublayer);
        bool ok = false;
        geometry.z = tokens[5].toFloat(&ok) / 10.0;
        if (!ok) {
          WARN_OUT << "ERROR while parsing TRD Module Contours file. Couldn't parse z coordinate '" << qPrintable(tokens[5])
                   << "' in line " << i + 1 << " for sublayer " << sublayer + 1 << std::endl;
          throw std::runtime_error("ERROR processing TRD Module Contours file.");
        }
  
        geometry.moduleLength = tokens[6].toFloat(&ok) / 10.0;
        if (!ok) {
          WARN_OUT << "ERROR while parsing TRD Module Contours file. Couldn't parse module length '" << qPrintable(tokens[6])
                   << "' in line " << i + 1 << " for sublayer " << sublayer + 1 << std::endl;
          throw std::runtime_error("ERROR processing TRD Module Contours file.");
        }

        TRDSubLayersZCoordinate[sublayer] = geometry.z;
      } else {
        // Check that each line after the first corresponds to the right layer.
        QString expectedSubIdentifier = sublayerIdentifier + "." + QString::number(i + 1);
        expectedSubIdentifier.replace(" ", "_");
        if (tokens[2] != expectedSubIdentifier) {
          WARN_OUT << "ERROR while parsing TRD Module Contours file. Expecting sublayer " << qPrintable(expectedSubIdentifier)
                    << " in line " << i + 1 << " for sublayer " << sublayer + 1 << ", got " << qPrintable(tokens[2]) << std::endl;
          throw std::runtime_error("ERROR processing TRD Module Contours file.");
        }

        QString moduleIdentifier = tokens[1];
        if (!moduleIdentifier.isEmpty()) {
          TRDModuleGeometries[iModuleNumber - 1] = geometry; // FIXME is the -1 still correct?
          geometry.xyContour.clear();

          bool ok = false;
          geometry.moduleLength = tokens[6].toFloat(&ok) / 10.0;
          if (!ok) {
            WARN_OUT << "ERROR while parsing TRD Module Contours file. Couldn't parse module length '" << qPrintable(tokens[6])
                     << "' in line " << i + 1 << " for sublayer " << sublayer + 1 << std::endl;
            throw std::runtime_error("ERROR processing TRD Module Contours file.");
          }

          iModuleNumber = ParseTRDModuleContoursIdentifier(moduleIdentifier, i, sublayer);
        }
      }

      bool ok = false;
      float x = tokens[3].toFloat(&ok) / 10.0;
      if (!ok) {
        WARN_OUT << "ERROR while parsing TRD Module Contours file. Couldn't parse x coordinate '" << qPrintable(tokens[3])
                 << "' in line " << i + 1 << " for sublayer " << sublayer + 1 << std::endl;
        throw std::runtime_error("ERROR processing TRD Module Contours file.");
      }

      float y = tokens[4].toFloat(&ok) / 10.0;
      if (!ok) {
        WARN_OUT << "ERROR while parsing TRD Module Contours file. Couldn't parse y coordinate '" << qPrintable(tokens[4])
                 << "' in line " << i + 1 << " for sublayer " << sublayer + 1 << std::endl;
        throw std::runtime_error("ERROR processing TRD Module Contours file.");
      }

      geometry.xyContour.push_back(std::make_pair(x, y));
    }
    TRDModuleGeometries[iModuleNumber - 1] = geometry;  // FIXME is the -1 still correct?
  }

  for (unsigned short module = 0; module < TRDModules; ++module) {
    TRDModuleGeometry& geometry = TRDModuleGeometries[module];
    if (geometry.xyContour.size() != 4 && geometry.xyContour.size() != 6) {
      WARN_OUT << "ERROR while parsing TRD Module Contours file. File is broken." << std::endl;
      throw std::runtime_error("ERROR processing TRD Module Contours file.");
    }

    std::stringstream cutIdentifier;
    cutIdentifier << "cutg_" << module;

    geometry.contour = new TCutG(cutIdentifier.str().c_str(), geometry.xyContour.size() + 1);
    if (!(geometry.sublayer % 2)) geometry.contour->SetLineColor(kRed);

    for (unsigned int contour = 0; contour < geometry.xyContour.size(); ++contour)
      geometry.contour->SetPoint(contour, geometry.xyContour[contour].first, geometry.xyContour[contour].second);

    geometry.contour->SetPoint(geometry.xyContour.size(), geometry.xyContour[0].first, geometry.xyContour[0].second);
  }

  // Build outer contours for first and last layer
  TRDFirstLayerContour = BuildTRDLayerPolygon(0);
  TRDLastLayerContour = BuildTRDLayerPolygon(AC::AMSGeometry::TRDLayers - 1);
}

void AC::AMSGeometry::BuildTRDLayerContours(unsigned int layer, std::vector<std::pair<float, float> >& positiveContours, std::vector<std::pair<float, float> >& negativeContours) const {

  for (unsigned short module = 0; module < TRDModules; ++module) {
    const TRDModuleGeometry& geometry = TRDModuleGeometries[module];
    std::vector<std::pair<float, float> > reversedNegativeContours;
    for (unsigned int contour = 0; contour < geometry.xyContour.size(); ++contour) {
      if (geometry.layer != layer)
        continue;
      if (geometry.xyContour[contour].first > 0)
        positiveContours.push_back(geometry.xyContour[contour]);
      else
        reversedNegativeContours.push_back(geometry.xyContour[contour]);
    }

    std::reverse(reversedNegativeContours.begin(), reversedNegativeContours.end());
    for (unsigned int i = 0; i < reversedNegativeContours.size(); ++i)
      negativeContours.push_back(reversedNegativeContours[i]);
  }
}

TCutG* AC::AMSGeometry::BuildTRDLayerPolygon(unsigned int layer) const {

  std::vector<std::pair<float, float> > layerOuterContourPositive;
  std::vector<std::pair<float, float> > layerOuterContourNegative;
  BuildTRDLayerContours(layer, layerOuterContourPositive, layerOuterContourNegative);

  std::stringstream identifier;
  identifier << "outerLayerContour_" << layer;

  TCutG* polygon = new TCutG(identifier.str().c_str(), layerOuterContourPositive.size() + layerOuterContourNegative.size());

  // FIXME: Fix non-straight lines at the bulkheads (zoom into the plot where the bulkheads are to spot the diffs)
  // Build counter-clockwise polygon
  unsigned int point = 0;
  for (unsigned int contour = 0; contour < layerOuterContourPositive.size(); ++contour, ++point)
    polygon->SetPoint(point, layerOuterContourPositive[contour].first, layerOuterContourPositive[contour].second);

  for (int contour = layerOuterContourNegative.size() - 1; contour >= 0; --contour, ++point)
    polygon->SetPoint(point, layerOuterContourNegative[contour].first, layerOuterContourNegative[contour].second);

  polygon->SetPoint(point, layerOuterContourPositive[0].first, layerOuterContourPositive[0].second);
  return polygon;
}

/** Apply shimming corrections and global TRD shift and rotation.
  *
  * The internal bookkeeping is updated as well.
  */
void AMSGeometry::ApplyShimmingCorrection(unsigned short straw, float secondCoordinate, float& dx, float& dy, float& dz) {

  int direction;
  float xy, z;
  TRDStrawToCoordinates(straw, direction, xy, z);
  Q_ASSERT(direction != -1);

  int Mod = TRDStrawToModule(straw);
  float x = direction == 0 ? xy : secondCoordinate;
  float y = direction == 0 ? secondCoordinate : xy;

  // Global TRD Shifts:
  dx = TRDShifts[0];  // X
  dy = TRDShifts[1];  // Y
  dz = TRDShifts[2];  // Z

  // Octagon Shimming - needs coordinate along wire:
  dz +=                    Mod_Dz[ Mod]/1.e4;      // module Dz/mu-m     in shimmed octagon
  dz += secondCoordinate * Mod_Arz[Mod]/1.e6;      // module Tilt/mu-rad along wire

  // Global TRD Rotations:

  // XY corrections
  if(direction==0){ // XZ Measurement

    dx += -(z+dz - ZTRDCenter)*sin(TRDRotations[1])   // TRD Y-Rotation (XZ-Tilt)
           -(y+dy)            *sin(TRDRotations[2]);  // TRD Z-Rotation
  }else{ // YZ Measurement

    dy += -(z+dz - ZTRDCenter)*sin(TRDRotations[0])   // TRD X-Rotation (YZ-Tilt)
           +(x+dx)            *sin(TRDRotations[2]);  // TRD Z-Rotation
  }

  // Z corrections
  dz +=   (x+dx) * sin(TRDRotations[1])   // TRD Y-Rotation (XZ-Tilt)
        + (y+dy) * sin(TRDRotations[0]);  // TRD X-Rotation (YZ-Tilt)
}

void AMSGeometry::InitTRDLadderLayerToModuleLookupTable() {

#if DEBUG > 0
  std::cout << std::endl;
  std::cout << "Generate mStraw-Matrix." << std::endl;
  std::cout << std::endl;
#endif

  unsigned int iLay0 = 0;
  for (unsigned int iLay = iLay0; iLay<iLay0+4; iLay++) {
    TRDModulesPerLayer[iLay] = 14;
    int n = iLay0 + iLay;
    for (unsigned int iLadr=0; iLadr<TRDModulesPerLayer[iLay]; iLadr++) {
      TRDLadderLayerToModuleLookupTable[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 4;
  for (unsigned int iLay = iLay0; iLay<iLay0+4; iLay++) {
    TRDModulesPerLayer[iLay] = 16;
    int n = (56-iLay0) + iLay;
    for (unsigned int iLadr=0; iLadr<TRDModulesPerLayer[iLay]; iLadr++) {
      TRDLadderLayerToModuleLookupTable[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 8;
  for (unsigned int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (120-iLay0) + iLay;
    TRDModulesPerLayer[iLay] = 16;
    for (unsigned int iLadr=0; iLadr<TRDModulesPerLayer[iLay]; iLadr++) {
      TRDLadderLayerToModuleLookupTable[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 12;
  for (unsigned int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (184-iLay0) + iLay;
    TRDModulesPerLayer[iLay] = 18;
    for (unsigned int iLadr=0; iLadr<TRDModulesPerLayer[iLay]; iLadr++) {
      TRDLadderLayerToModuleLookupTable[iLadr][iLay] =   n;
      n += 4;
    }
  }

  iLay0 = 16;
  for (unsigned int iLay = iLay0; iLay<iLay0+4; iLay++) {
    int n = (256-iLay0) + iLay;
    TRDModulesPerLayer[iLay] = 18;
    for (unsigned int iLadr=0; iLadr<TRDModulesPerLayer[iLay]; iLadr++) {
      TRDLadderLayerToModuleLookupTable[iLadr][iLay] =   n;
      n += 4;
    }
  }

#if DEBUG > 0
  printf("    ");
  for (unsigned int lad=0; lad<TRDLadders; lad++) printf("%4d ",lad);
  printf("\n");
  printf("----");
  for (unsigned int lad=0; lad<TRDLadders; lad++) printf("-----");
  printf("\n");
  for (int lay=TRDLayers-1; lay>=0; lay--) {
    printf("%3d ",lay);
    for (unsigned int lad=0; lad<TRDLadders; lad++) {
      printf("%4d ",TRDLadderLayerToModuleLookupTable[lad][lay]);
    }
    printf("\n");
  }
#endif
}

void AMSGeometry::TRDStrawToLadderAndLayer(unsigned short Straw, unsigned short &Lad, unsigned short &Lay) {
  
  unsigned short Module = TRDStrawToModule(Straw);
  TRDModuleToLadderAndLayer(Module,Lad,Lay);
}

/** Convert module number (0..327) to ladder and layer. */
void AMSGeometry::TRDModuleToLadderAndLayer(unsigned short Module, unsigned short &Lad, unsigned short &Lay) {


  static std::vector<short>* gModuleLay = 0;
  static std::vector<short>* gModuleLad = 0;
  if (!gModuleLay) {
    gModuleLay = new std::vector<short>();
    gModuleLay->assign(TRDModules,-1);
    gModuleLad = new std::vector<short>();
    gModuleLad->assign(TRDModules,-1);
  }

  Lad = std::numeric_limits<unsigned short>::max() - 1;
  Lay = std::numeric_limits<unsigned short>::max() - 1;

  if (gModuleLay->at(Module)>=0) {
    Lay = gModuleLay->at(Module);
    Lad = gModuleLad->at(Module);
    DEBUG_OUT << "Module " << Module << " -> ladder " << Lad << " layer " << Lay << std::endl;
    return;
  }

  for (int lay=TRDLayers-1; lay>=0; lay--) {
    if (Module < TRDLadderLayerToModuleLookupTable[0][lay]) continue;
    for (int lad=0; lad<TRDModulesPerLayer[lay]; lad++) {
      if (Module != TRDLadderLayerToModuleLookupTable[lad][lay]) continue;
      Lay = lay;
      Lad = lad;
      gModuleLay->at(Module) = lay;
      gModuleLad->at(Module) = lad;
      DEBUG_OUT << "Module " << Module << " -> ladder " << Lad << " layer " << Lay << std::endl;
      return;
    }
  }
}

void TRDStrawToCoordinates(unsigned short straw, int& direction, float& xy, float& z) {
  
  static std::vector<int>* gStrawDirection = 0;
  static std::vector<float>* gStrawXY = 0;
  static std::vector<float>* gStrawZ = 0;

  Q_ASSERT(straw < AMSGeometry::TRDStraws);

  direction = -1;
  xy = std::numeric_limits<float>::max() - 1;
  z = std::numeric_limits<float>::max() - 1;

  if (!gStrawDirection) {
    gStrawDirection = new std::vector<int>();
    gStrawDirection->assign(AMSGeometry::TRDStraws,-1);
    
    gStrawXY = new std::vector<float>();
    gStrawXY->assign(AMSGeometry::TRDStraws,0.0);
    
    gStrawZ = new std::vector<float>();
    gStrawZ->assign(AMSGeometry::TRDStraws,0.0);
  }

  if (gStrawDirection->at(straw) >= 0) {
    direction = gStrawDirection->at(straw);
    xy = gStrawXY->at(straw);
    z = gStrawZ->at(straw);
    return;
  }

  unsigned short ladder, layer;
  AMSGeometry::Self()->TRDStrawToLadderAndLayer(straw, ladder, layer);

  unsigned short tower = ladder;
  if (layer < 12) ++tower;
  if (layer < 4) ++tower;
  direction = (layer >= 16 || layer <= 3);

  z = 85.275 + 2.9 * (float) layer;
  if (tower % 2 == 0) z += 1.45; // this means: even-numbered towers are on sublayer 1, odd-numbered layers are on sublayer 0

  xy = 10.1 * (float)(tower - 9);

  if ((direction==1) && (tower>=12)) xy += 0.78;
  if ((direction==1) && (tower<= 5)) xy -= 0.78;

  unsigned short tube = TRDStrawToTube(straw);
  xy += 0.31 + 0.62 * (float) tube;

  // Account TRD longitudinal stringers.
  if(tube >=  1) xy += 0.03;
  if(tube >=  4) xy += 0.03;
  if(tube >=  7) xy += 0.03;
  if(tube >=  9) xy += 0.03;
  if(tube >= 12) xy += 0.03;
  if(tube >= 15) xy += 0.03;

  gStrawDirection->at(straw) = direction;
  gStrawXY->at(straw) = xy;
  gStrawZ->at(straw) = z;
}

void PushStrawToLookupTableIfNeeded(unsigned short int straw, unsigned short int sublayer, int x, int y) {

  std::vector<unsigned short int>& myvector = AMSGeometry::Self()->TRDStrawLookupTable[sublayer][x][y];
  if (std::find(myvector.begin(), myvector.end(), straw) != myvector.end()) return;
  myvector.push_back(straw);
}

void AMSGeometry::InitTRDStrawLookupTable() {
  
  // Apply a coordinate transformation to map any negative x/y values to a positive range.
  float coordinateOffset = TRDMaximumStrawLength / 2.0;
  for (unsigned short straw = 0; straw < TRDStraws; ++straw) {
    unsigned short module = TRDStrawToModule(straw);
    unsigned short sublayer = TRDModuleGeometries[module].sublayer;
    int direction;
    float xy, z;
    TRDStrawToCoordinates(straw, direction, xy, z); 
    Q_ASSERT(direction != -1);

    float dx, dy, dz;
    ApplyShimmingCorrection(straw, 0, dx, dy, dz);
    if (direction == 0) xy += dx;
    else xy += dy;

    int xyLower = (int) floor(xy + coordinateOffset);
    int xyUpper = (int) ceil(xy + coordinateOffset);

    float moduleLength = TRDModuleGeometries[module].moduleLength;
    int positionStart = floor(coordinateOffset - moduleLength / 2.0);
    int positionEnd = ceil(coordinateOffset + moduleLength / 2.0);

    for (int position = positionStart; position <= positionEnd; ++position) {
      if (direction == 0) {
        PushStrawToLookupTableIfNeeded(straw, sublayer, xyLower, position);
        PushStrawToLookupTableIfNeeded(straw, sublayer, xyUpper, position);
      } else if (direction == 1) {
        PushStrawToLookupTableIfNeeded(straw, sublayer, position, xyLower);
        PushStrawToLookupTableIfNeeded(straw, sublayer, position, xyUpper);
      }
    }
  }
}

void AMSGeometry::FillGeoArray() {

 for(int k=0;k<4;k++){
  GetGeo[1][4][3][k][0]=0*4+k;
  GetGeo[1][4][3][k][1]=0;
  GetGeo[1][4][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][4][k][0]=0*4+k;
  GetGeo[1][4][4][k][1]=1;
  GetGeo[1][4][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][3][k][0]=0*4+k;
  GetGeo[1][1][3][k][1]=2;
  GetGeo[1][1][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][4][k][0]=0*4+k;
  GetGeo[1][1][4][k][1]=3;
  GetGeo[1][1][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][5][k][0]=0*4+k;
  GetGeo[1][4][5][k][1]=4;
  GetGeo[1][4][5][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][6][k][0]=0*4+k;
  GetGeo[1][4][6][k][1]=5;
  GetGeo[1][4][6][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][5][k][0]=0*4+k;
  GetGeo[1][1][5][k][1]=6;
  GetGeo[1][1][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][5][k][0]=0*4+k;
  GetGeo[0][4][5][k][1]=7;
  GetGeo[0][4][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][4][k][0]=0*4+k;
  GetGeo[0][1][4][k][1]=8;
  GetGeo[0][1][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][3][k][0]=0*4+k;
  GetGeo[0][1][3][k][1]=9;
  GetGeo[0][1][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][4][k][0]=0*4+k;
  GetGeo[0][4][4][k][1]=10;
  GetGeo[0][4][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][3][k][0]=0*4+k;
  GetGeo[0][4][3][k][1]=11;
  GetGeo[0][4][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][2][k][0]=0*4+k;
  GetGeo[0][1][2][k][1]=12;
  GetGeo[0][1][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][1][k][0]=0*4+k;
  GetGeo[0][1][1][k][1]=13;
  GetGeo[0][1][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][1][k][0]=1*4+k;
  GetGeo[1][1][1][k][1]=0;
  GetGeo[1][1][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][2][k][0]=1*4+k;
  GetGeo[1][1][2][k][1]=1;
  GetGeo[1][1][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][1][k][0]=1*4+k;
  GetGeo[0][4][1][k][1]=2;
  GetGeo[0][4][1][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][2][k][0]=1*4+k;
  GetGeo[0][4][2][k][1]=3;
  GetGeo[0][4][2][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][5][k][0]=1*4+k;
  GetGeo[1][3][5][k][1]=4;
  GetGeo[1][3][5][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][6][k][0]=1*4+k;
  GetGeo[1][3][6][k][1]=5;
  GetGeo[1][3][6][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][5][k][0]=1*4+k;
  GetGeo[0][3][5][k][1]=6;
  GetGeo[0][3][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][6][k][0]=1*4+k;
  GetGeo[0][3][6][k][1]=7;
  GetGeo[0][3][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][4][k][0]=1*4+k;
  GetGeo[1][3][4][k][1]=8;
  GetGeo[1][3][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][3][k][0]=1*4+k;
  GetGeo[1][3][3][k][1]=9;
  GetGeo[1][3][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][6][k][0]=1*4+k;
  GetGeo[0][2][6][k][1]=10;
  GetGeo[0][2][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][5][k][0]=1*4+k;
  GetGeo[0][2][5][k][1]=11;
  GetGeo[0][2][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][2][k][0]=1*4+k;
  GetGeo[1][4][2][k][1]=12;
  GetGeo[1][4][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][1][k][0]=1*4+k;
  GetGeo[1][4][1][k][1]=13;
  GetGeo[1][4][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][0][k][0]=1*4+k;
  GetGeo[0][1][0][k][1]=14;
  GetGeo[0][1][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][0][k][0]=1*4+k;
  GetGeo[0][0][0][k][1]=15;
  GetGeo[0][0][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][1][0][k][0]=2*4+k;
  GetGeo[1][1][0][k][1]=0;
  GetGeo[1][1][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][0][k][0]=2*4+k;
  GetGeo[1][0][0][k][1]=1;
  GetGeo[1][0][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][0][k][0]=2*4+k;
  GetGeo[0][5][0][k][1]=2;
  GetGeo[0][5][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][4][0][k][0]=2*4+k;
  GetGeo[0][4][0][k][1]=3;
  GetGeo[0][4][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][5][k][0]=2*4+k;
  GetGeo[1][2][5][k][1]=4;
  GetGeo[1][2][5][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][6][k][0]=2*4+k;
  GetGeo[1][2][6][k][1]=5;
  GetGeo[1][2][6][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][3][k][0]=2*4+k;
  GetGeo[0][3][3][k][1]=6;
  GetGeo[0][3][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][4][k][0]=2*4+k;
  GetGeo[0][3][4][k][1]=7;
  GetGeo[0][3][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][2][k][0]=2*4+k;
  GetGeo[1][3][2][k][1]=8;
  GetGeo[1][3][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][1][k][0]=2*4+k;
  GetGeo[1][3][1][k][1]=9;
  GetGeo[1][3][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][4][k][0]=2*4+k;
  GetGeo[0][2][4][k][1]=10;
  GetGeo[0][2][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][3][k][0]=2*4+k;
  GetGeo[0][2][3][k][1]=11;
  GetGeo[0][2][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][6][k][0]=2*4+k;
  GetGeo[1][5][6][k][1]=12;
  GetGeo[1][5][6][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][5][k][0]=2*4+k;
  GetGeo[1][5][5][k][1]=13;
  GetGeo[1][5][5][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][6][k][0]=2*4+k;
  GetGeo[0][1][6][k][1]=14;
  GetGeo[0][1][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][1][5][k][0]=2*4+k;
  GetGeo[0][1][5][k][1]=15;
  GetGeo[0][1][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][5][k][0]=3*4+k;
  GetGeo[0][5][5][k][1]=0;
  GetGeo[0][5][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][6][k][0]=3*4+k;
  GetGeo[0][5][6][k][1]=1;
  GetGeo[0][5][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][1][k][0]=3*4+k;
  GetGeo[1][2][1][k][1]=2;
  GetGeo[1][2][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][2][k][0]=3*4+k;
  GetGeo[1][2][2][k][1]=3;
  GetGeo[1][2][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][0][k][0]=3*4+k;
  GetGeo[0][2][0][k][1]=4;
  GetGeo[0][2][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][0][k][0]=3*4+k;
  GetGeo[0][3][0][k][1]=5;
  GetGeo[0][3][0][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][3][k][0]=3*4+k;
  GetGeo[1][2][3][k][1]=6;
  GetGeo[1][2][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][4][k][0]=3*4+k;
  GetGeo[1][2][4][k][1]=7;
  GetGeo[1][2][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][2][k][0]=3*4+k;
  GetGeo[0][3][2][k][1]=8;
  GetGeo[0][3][2][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][3][1][k][0]=3*4+k;
  GetGeo[0][3][1][k][1]=9;
  GetGeo[0][3][1][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][3][0][k][0]=3*4+k;
  GetGeo[1][3][0][k][1]=10;
  GetGeo[1][3][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][2][0][k][0]=3*4+k;
  GetGeo[1][2][0][k][1]=11;
  GetGeo[1][2][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][2][k][0]=3*4+k;
  GetGeo[0][2][2][k][1]=12;
  GetGeo[0][2][2][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][2][1][k][0]=3*4+k;
  GetGeo[0][2][1][k][1]=13;
  GetGeo[0][2][1][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][4][k][0]=3*4+k;
  GetGeo[1][5][4][k][1]=14;
  GetGeo[1][5][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][3][k][0]=3*4+k;
  GetGeo[1][5][3][k][1]=15;
  GetGeo[1][5][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][6][k][0]=3*4+k;
  GetGeo[0][0][6][k][1]=16;
  GetGeo[0][0][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][5][k][0]=3*4+k;
  GetGeo[0][0][5][k][1]=17;
  GetGeo[0][0][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][1][k][0]=4*4+k;
  GetGeo[1][0][1][k][1]=0;
  GetGeo[1][0][1][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][2][k][0]=4*4+k;
  GetGeo[1][0][2][k][1]=1;
  GetGeo[1][0][2][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][0][k][0]=4*4+k;
  GetGeo[1][5][0][k][1]=2;
  GetGeo[1][5][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][4][0][k][0]=4*4+k;
  GetGeo[1][4][0][k][1]=3;
  GetGeo[1][4][0][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][3][k][0]=4*4+k;
  GetGeo[1][0][3][k][1]=4;
  GetGeo[1][0][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][4][k][0]=4*4+k;
  GetGeo[1][0][4][k][1]=5;
  GetGeo[1][0][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][1][k][0]=4*4+k;
  GetGeo[1][5][1][k][1]=6;
  GetGeo[1][5][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][5][2][k][0]=4*4+k;
  GetGeo[1][5][2][k][1]=7;
  GetGeo[1][5][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][6][k][0]=4*4+k;
  GetGeo[1][0][6][k][1]=8;
  GetGeo[1][0][6][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[1][0][5][k][0]=4*4+k;
  GetGeo[1][0][5][k][1]=9;
  GetGeo[1][0][5][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][4][k][0]=4*4+k;
  GetGeo[0][0][4][k][1]=10;
  GetGeo[0][0][4][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][3][k][0]=4*4+k;
  GetGeo[0][0][3][k][1]=11;
  GetGeo[0][0][3][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][4][k][0]=4*4+k;
  GetGeo[0][5][4][k][1]=12;
  GetGeo[0][5][4][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][3][k][0]=4*4+k;
  GetGeo[0][5][3][k][1]=13;
  GetGeo[0][5][3][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][2][k][0]=4*4+k;
  GetGeo[0][0][2][k][1]=14;
  GetGeo[0][0][2][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][0][1][k][0]=4*4+k;
  GetGeo[0][0][1][k][1]=15;
  GetGeo[0][0][1][k][2]=0;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][2][k][0]=4*4+k;
  GetGeo[0][5][2][k][1]=16;
  GetGeo[0][5][2][k][2]=1;
 }
 for(int k=0;k<4;k++){
  GetGeo[0][5][1][k][0]=4*4+k;
  GetGeo[0][5][1][k][1]=17;
  GetGeo[0][5][1][k][2]=1;
 }

}

unsigned short TRDStrawToLayer(unsigned short straw) {

  Q_ASSERT(straw < AMSGeometry::TRDStraws);

  unsigned short ladder, layer;
  AMSGeometry::Self()->TRDStrawToLadderAndLayer(straw, ladder, layer);
  return layer;
}

unsigned short TRDStrawToLadder(unsigned short straw) {

  Q_ASSERT(straw < AMSGeometry::TRDStraws);

  unsigned short ladder, layer;
  AMSGeometry::Self()->TRDStrawToLadderAndLayer(straw, ladder, layer);
  return ladder;
}

unsigned short TRDStrawToTube(unsigned short straw) {

  Q_ASSERT(straw < AMSGeometry::TRDStraws);
  return straw % AMSGeometry::TRDStrawsPerModule;
}

unsigned short TRDStrawToModule(unsigned short straw) {

  Q_ASSERT(straw < AMSGeometry::TRDStraws);
  return straw / AMSGeometry::TRDStrawsPerModule;
}

}

