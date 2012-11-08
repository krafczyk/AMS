#ifndef AMSGeometry_h
#define AMSGeometry_h

#include <string>

#include <Rtypes.h>

class TCutG;

namespace AC {

/** AMS geometry data
  */
class AMSGeometry {
public:
  AMSGeometry();

  /** Returns a singleton of the AMSGeometry class */
  static AMSGeometry* Self() {
    static AMSGeometry* gInstance = 0;
    if (!gInstance) {
      gInstance = new AMSGeometry;
      gInstance->Init();
    }
    return gInstance;
  }

public:
  /** Number of TRD modules */
  static const UInt_t TRDModules = 328;
 
  /** Number of TRD layers */
  static const UInt_t TRDLayers = 20;

  /** Number of TRD sub-layers.
    * Each TRD layer is divded into an upper and lower layer forming two sub-layers.
    */
  static const UInt_t TRDSubLayers = 40;

  /** Number of TRD ladders */
  static const UInt_t TRDLadders = 18;

  /** Number of TRD straw tubes */
  static const UInt_t TRDStraws = 5248;
 
  /** Number of TRD straw tubes per module */
  static const UInt_t TRDStrawsPerModule = 16;
 
  /** Maximum ADC value reportable by electronics [#] */
  static const UInt_t TRDMaxADCCount = 4096;

  /** Radius of TRD tubes [cm] */
  static const Float_t TRDTubeRadius = 0.3;

  /** Maximum length of TRD straw tube [cm] */
  static const Float_t TRDMaximumStrawLength = 210.0;

  /** Maximum length of TRD straw tube [cm] */
  static const UInt_t TRDMaximumStrawLengthAsInteger = 210;

  /** Minimal path length in TRD tubes [cm] */
  static const Float_t TRDTubeMinPathLength = 0.1;

  /** Z coordinate [cm] of upper TOF */
  static const Float_t ZTOFUpper = 63.65;

  /** Z coordinate [cm] of upper ECAL */
  static const Float_t ZECALUpper = -142.8;

  /** Z coordinate [cm] of RICH */
  static const Float_t ZRICH = -73.6;

  /** Z coordinate [cm] of Tracker Layer 1 */
  static const Float_t ZTrackerLayer1 = 159.067;

  /** Z coordinate [cm] of Tracker Layer 9 */
  static const Float_t ZTrackerLayer9 = -136.041; 
  
  /** Z coordinate [cm] of TRD center */
  static const Float_t ZTRDCenter = 113.55; 
  
  /** Z coordinate [cm] of TRD upper */
  static const Float_t ZTRDUpper = 136.75;

  /** Z coordinate [cm] of TRD lower */
  static const Float_t ZTRDLower = 90.35;

  /** Overall thickness of a TRD layer (cm). */
  static const Float_t TrdLayerThickness = 2.9;

  /** Overall width of a TRD module (cm). */
  static const Float_t TrdModuleWidth = 10.1;

  /** Global TRD X/Y/Z shifts [cm]
    * \todo Remove once TRD structure is in place.
    */
  float TRDShifts[3];

  /** Global TRD X/Y/Z rotations [deg]
    * \todo Remove once TRD structure is in place.
    */
  float TRDRotations[3];

  /** Map from crate, nudr, nufe, nute to layer, ladder, direction (0, 1)
    * \todo Add better documentation. Resolve acronyms
    */
  Int_t GetGeo[2][6][7][4][3]; // crate,nudr,nufe,nute -> layer,ladder,direction(0,1) 

  /** Map from sublayers/x/y coordinates to a pair of numbers, representing which
    * straws are located at a certain set of coordinates.
    */
  std::vector<unsigned short int> TRDStrawLookupTable[TRDSubLayers][TRDMaximumStrawLengthAsInteger][TRDMaximumStrawLengthAsInteger];

  struct TRDModuleGeometry {
    TRDModuleGeometry()
      : layer(0)
      , sublayer(0)
      , z(0)
      , moduleLength(0)
      , contour(0) {
  
    }
  
    unsigned short layer;
    unsigned short sublayer;
    float z;
    float moduleLength;
    TCutG* contour;
    std::vector<std::pair<float, float> > xyContour;
  };

  float TRDSubLayersZCoordinate[TRDSubLayers];

  TCutG* TRDFirstLayerContour;
  TCutG* TRDLastLayerContour;

  // TRD module displacement from shimming:     
  float Mod_Dz[TRDModules];         // Z-Offset/mu-m in Octagon from Shimming     
  float Mod_Arz[TRDModules];        // Tilt/mu-rad   in Octagom from Shimming
  
  TRDModuleGeometry TRDModuleGeometries[TRDModules];
  short TRDLadderLayerToModuleLookupTable[TRDLadders][TRDLayers];
  unsigned short TRDModulesPerLayer[TRDLayers];

  void ApplyShimmingCorrection(unsigned short straw, float secondCoordinate, float& dx, float& dy, float& dz);

public:

  std::vector<Short_t> TrdModulesOnLayer( Short_t layer );
  std::vector<Short_t> TrdModulesOnSublayer( Short_t layer, Short_t sublayer );

  friend unsigned short TRDStrawToLayer(unsigned short);
  friend unsigned short TRDStrawToLadder(unsigned short);
  friend void TRDStrawToCoordinates(unsigned short straw, int& direction, float& xy, float& z);
  void TRDStrawToLadderAndLayer(unsigned short straw, unsigned short& ladder, unsigned short& layer);
  void TRDModuleToLadderAndLayer(unsigned short Module, unsigned short &Lad, unsigned short &Lay);

private:
  void Init();
  void InitTRDLadderLayerToModuleLookupTable();
  void InitTRDStrawLookupTable();
  void InitTRDGeometryFile(const std::string& path);
  void InitTRDShimmingGlobalFile(const std::string& path);
  void InitTRDShimmingModuleFile(const std::string& path);
  void FillGeoArray();
  void BuildTRDLayerContours(unsigned int layer, std::vector<std::pair<float, float> >& positiveContours, std::vector<std::pair<float, float> >& negativeContours) const;
  TCutG* BuildTRDLayerPolygon(unsigned int layer) const;
};

/** Helper function to return the module associated with a straw 0-5247 */
unsigned short TRDStrawToModule(unsigned short straw);

/** Helper function to return the layer associated with a straw 0-5247 */
unsigned short TRDStrawToLayer(unsigned short straw);

/** Helper function to return the ladder associated with a straw 0-5247 */
unsigned short TRDStrawToLadder(unsigned short straw);

/** Helper function to return the tube associated with a straw 0-5247 */
unsigned short TRDStrawToTube(unsigned short straw);

/** Helper function to return the coordinates associated with a straw 0-5247 */
void TRDStrawToCoordinates(unsigned short straw, int& direction, float& xy, float& z);

}

#endif
