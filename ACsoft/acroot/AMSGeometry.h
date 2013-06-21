#ifndef AMSGeometry_h
#define AMSGeometry_h

#include <string>
#include <vector>
#include <ostream>

#include <Rtypes.h>

class TCutG;

namespace ACsoft {

namespace AC {

/** AMS geometry data
  */
class AMSGeometry {
public:
  /** Returns a singleton of the AMSGeometry class */
  static AMSGeometry* Self();

public:
  /** Number of TRD modules */
  static const unsigned int TRDModules = 328;
 
  /** Number of TRD layers */
  static const unsigned int TRDLayers = 20;

  /** Number of TRD sub-layers.
    * Each TRD layer is divded into an upper and lower layer forming two sub-layers.
    */
  static const unsigned int TRDSubLayers = 40;

  /** Number of TRD towers */
  static const unsigned int TRDTowers = 18;

  /** Number of TRD straw tubes */
  static const unsigned int TRDStraws = 5248;
 
  /** Number of TRD straw tubes per module */
  static const unsigned int TRDStrawsPerModule = 16;
 
  /** Maximum ADC value reportable by electronics [#] */
  static const unsigned int TRDMaxADCCount = 4096;

  /** Radius of TRD tubes [cm] */
  static const float TRDTubeRadius;

  /** Diameter of TRD tubes [cm] */
  static const float TRDTubeDiameter;

  /** Maximum length of TRD straw tube [cm] */
  static const float TRDMaximumStrawLength;

  /** Maximum length of TRD straw tube [cm] */
  static const unsigned int TRDMaximumStrawLengthAsInteger = 210;

  /** Z coordinate [cm] of upper TOF */
  static const float ZTOFUpper;

  /** Z coordinate [cm] of upper ECAL */
  static const float ZECALUpper;

  /** Z coordinate [cm] of lower ECAL */
  static const float ZECALLower;

  /** Z coordinate [cm] of RICH */
  static const float ZRICH;

  /** Z coordinate [cm] of Tracker Layer 1 */
  static const float ZTrackerLayer1;

  /** Z coordinate [cm] of Tracker Layer 56 */
  static const float ZTrackerLayer56;

  /** Z coordinate [cm] of Tracker Layer 9 */
  static const float ZTrackerLayer9;
  
  /** Z coordinate [cm] of TRD center */
  static const float ZTRDCenter;
  
  /** Z coordinate [cm] of TRD upper */
  static const float ZTRDUpper;

  /** Z coordinate [cm] of TRD lower */
  static const float ZTRDLower;

  /** Overall thickness of a TRD layer (cm). */
  static const float TrdLayerThickness;

  /** Overall width of a TRD module (cm). */
  static const float TrdModuleWidth;



  /** Map from crate, nudr, nufe, nute to layer, ladder, direction (0, 1)
    * \todo Add better documentation. Resolve acronyms
    */
  Int_t GetGeo[2][6][7][4][3]; // crate,nudr,nufe,nute -> layer,ladder,direction(0,1) 

  /** Map from sublayers/x/y coordinates to a pair of numbers, representing which
    * straws are located at a certain set of coordinates.
    */
  std::vector<unsigned short int> TRDStrawLookupTable[TRDSubLayers][TRDMaximumStrawLengthAsInteger][TRDMaximumStrawLengthAsInteger];

  struct TRDModuleGeometry { // FIXME move to TrdModule, keep only moduleLength and contour, align!
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

  TCutG* TRDFirstLayerContour; // FIXME move to TrdDetector, align!
  TCutG* TRDLastLayerContour;

  TRDModuleGeometry TRDModuleGeometries[TRDModules]; // FIXME Move to TrdModule class
  short TRDLadderLayerToModuleLookupTable[TRDTowers][TRDLayers];
  unsigned short TRDModulesPerLayer[TRDLayers];

public:

  std::vector<Short_t> TrdModulesOnLayer( Short_t layer );
  std::vector<Short_t> TrdModulesOnSublayer( Short_t layer, Short_t sublayer );

  friend unsigned short TRDStrawToLayer(unsigned short);
  friend unsigned short TRDStrawToLadder(unsigned short);
  void TRDStrawToLadderAndLayer(unsigned short straw, unsigned short& ladder, unsigned short& layer);
  void TRDModuleToLadderAndLayer(unsigned short Module, unsigned short &Lad, unsigned short &Lay);

  bool CollectStrawCandidates( unsigned short sublayer, double x, double y, std::vector<unsigned short>& strawNumbers );

private:
  AMSGeometry();
  void Init();
  void InitTRDLadderLayerToModuleLookupTable();
  void InitTRDStrawLookupTable();
  void InitTRDGeometryFile(const std::string& path);
  void FillGeoArray();
  void BuildTRDLayerContours(unsigned int layer, std::vector<std::pair<float, float> >& positiveContours, std::vector<std::pair<float, float> >& negativeContours) const;
  TCutG* BuildTRDLayerPolygon(unsigned int layer) const;
};

/** Helper function to return the module associated with a straw 0-5247 */
unsigned short TRDStrawToModule(unsigned short straw); // FIXME remove

/** Helper function to return the layer associated with a straw 0-5247 */
unsigned short TRDStrawToLayer(unsigned short straw); // FIXME remove

/** Helper function to return the ladder associated with a straw 0-5247 */
unsigned short TRDStrawToLadder(unsigned short straw); // FIXME remove

/** Helper function to return the tube associated with a straw 0-5247 */
unsigned short TRDStrawToTube(unsigned short straw); // FIXME remove

/** Helper function to return the coordinates associated with a straw 0-5247 */
void TrdStrawToRawCoordinates(unsigned short straw, int& direction, float& xy, float& z); // FIXME Rename to TrdStrawToRawCoordinates, then remove

inline std::ostream& operator<<(std::ostream& out, const AC::AMSGeometry::TRDModuleGeometry& geo) {

  out << "TRDModuleGeometry z=" << geo.z << " lay " << geo.layer << " sublayer " << geo.sublayer << " len " << geo.moduleLength;
  return out;
}

}

}

#endif
