// $Id: gvconst.cpp,v 1.1 2010/05/10 21:55:47 shaino Exp $
#include "gvconst.h"

#include "TString.h"

namespace gvconst {

int geomOrder[] = { Geom_TRK,  Geom_LTRK, Geom_ACC, Geom_TOF, Geom_LTOF, 
		    Geom_ECAL, Geom_RICH, Geom_TRD, Geom_MAG, Geom_USS };

// Color vector (RGBA) for geometries
double rgbaDef [4] = { 0.8, 0.8, 0.8, 0.2 };  // Default
double rgbaUSS [4] = { 0.8, 0.8, 0.8, 0.2 };  // USS
double rgbaMag [4] = { 0.9, 0.3, 0.3, 0.2 };  // Magnet
double rgbaTRD [4] = { 0.0, 0.0, 0.8, 0.3 };  // TRD
double rgbaTRK [4] = { 0.0, 0.9, 0.0, 0.6 };  // TRK
double rgbaTOF [4] = { 0.9, 0.9, 0.0, 0.4 };  // TOF
double rgbaACC [4] = { 0.0, 0.8, 0.8, 0.3 };  // ACC
double rgbaRICH[4] = { 0.8, 0.0, 0.8, 0.4 };  // RICH
double rgbaECAL[4] = { 0.8, 0.0, 0.0, 0.4 };  // Ecal

EGeomType getType(const char *nodename)
{
  TString str(nodename);
  if (str.BeginsWith("US")) return Geom_USS;
  if (str.BeginsWith("RA")) return Geom_USS;
  if (str.BeginsWith("CR")) return Geom_USS;
  if (str.BeginsWith("MM")) return Geom_MAG;
  if (str.BeginsWith("TR")) return Geom_TRD;
  if (str.BeginsWith("TK")) return Geom_TRK;
  if (str.BeginsWith("ST")) return Geom_TRK;
  if (str.BeginsWith("TF")) return Geom_TOF;
  if (str.BeginsWith("TL")) return Geom_LTOF;
  if (str.BeginsWith("AS")) return Geom_ACC;
  if (str.BeginsWith("RI")) return Geom_RICH;
  if (str.BeginsWith("EC")) return Geom_USS;
  if (str.BeginsWith("BE")) return Geom_ECAL;
  return Geom_NONE;
}

int typeToGLID(int type, int id)
{
  return type*1000+id;
}

int glIDToType(int glid)
{
  return glid/1000;
}

int glIDToSelID(int glid)
{
  return glid%1000;
}

int getGeomOrder(EGeomType type)
{
  int nord = sizeof(geomOrder)/sizeof(int);

  for (int i = 0; i < nord; i++)
    if (type == geomOrder[i]) return i;

  return 99;
}

double *getRgba(EGeomType code)
{
  if (code == Geom_USS ) return rgbaUSS;
  if (code == Geom_MAG ) return rgbaMag;
  if (code == Geom_TRD ) return rgbaTRD;
  if (code == Geom_TRK ) return rgbaTRK;
  if (code == Geom_TOF ) return rgbaTOF;
  if (code == Geom_LTRK) return rgbaTRK;
  if (code == Geom_LTOF) return rgbaTOF;
  if (code == Geom_ACC ) return rgbaACC;
  if (code == Geom_RICH) return rgbaRICH;
  if (code == Geom_ECAL) return rgbaECAL;
  return rgbaDef;
}

};   // namespace gvconst
