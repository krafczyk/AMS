#ifndef __AMS2TGEO_H__
#define __AMS2TGEO_H__

/////////////////////////////////////////////////////////////// 
//   This routines are used to manage the conversion between AMSGeometry and the TGeoManager geometry
//   The AMSJob::gethead() method is used to retrieve the materials (AMSgmat), media (AMSgtmed) and volumes (AMSgvolume).
//   1. All the AMSgmat are converted into TGeoMaterial:
//      - Elements composing the AMSmat are firstly converted into TGeoElement (each element of each material, with repetition)
//      - The elements are assembled in TGeoMixture/TGeoMaterial (density, radlen, intlen)
//   2. All the AMSgtmed are converted into TGeoMedium (tracking properties)
//   3. AMSgvolume is converted into TGeoShape (solid)
//   4. AMSgvolume is converted into rotation matrix and position
//   5. The TGeoVolume is assembled and putted into the geometry
/////////////////////////////////////////////////////////////// 

#include "job.h"
#include "gmat.h"
#include "gvolume.h"
#include "typedefs.h"
#include "point.h"
 
#include "TGeometry.h"
#include "TGeoManager.h"
#include "TGeoElement.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoVolume.h"
#include "TGeoShape.h"
#include "TGeoBBox.h"
#include "TGeoTrd1.h"
#include "TGeoTrd2.h"
#include "TGeoArb8.h"
#include "TGeoTube.h"
#include "TGeoCone.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoPcon.h"
#include "TGeoEltu.h"
#include "TRotMatrix.h"
#include "TGeoMatrix.h"
#include "TGeoCompositeShape.h"
#include "TGeoBoolNode.h"

#include <vector>
#include <iostream>
#include <stdio.h>

class ams2tgeo{
 public:
  static bool debug_mat;
  static bool debug_med;
  static bool debug_geo;
  static bool debug_pos;
  static int  nsha;
  static int  nvol;
  static int  nrot;
  static void            ToUpper(char* name);
  static TGeoManager*    Build(char* name, char* title, char* filename);
  // static TGeoManager*    Load(); // future
  static TGeoVolume*     AMSGeometry2TGeometry(TGeoManager* tman, TGeoVolume* tvol_mot, AMSgvolume* amsvol);
  static TGeoVolume*     AMSgvolume2TGeoVolume(TGeoManager* tman, AMSgvolume* amsvol);
  static TGeoShape*      AMSgvolume2TGeoShape(AMSgvolume* amsvol);
  static TGeoCombiTrans* AMSgvolume2TGeoCombiTrans(AMSgvolume* amsvol);
};

#endif

