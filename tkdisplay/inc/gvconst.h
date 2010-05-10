// $Id: gvconst.h,v 1.1 2010/05/10 21:55:46 shaino Exp $
//
// Constant parameters for GL stuff by SH
//
#ifndef GVCONST
#define GVCONST

namespace gvconst {
  enum EGeomType {
    Geom_NONE =  0, Geom_USS  =  1, Geom_MAG  = 2, Geom_TRD  = 3, Geom_TRK = 4,
    Geom_TOF  =  6, Geom_ACC  =  7, Geom_RICH = 8, Geom_ECAL = 9,
    Geom_LTRK = 11, Geom_LTOF = 12,
    Num_Geom  = 13
  };

  enum EDrawOpt {
    Draw_TRK = 0x0001, Draw_HIT = 0x0002, Draw_CLS = 0x0004,
    Draw_MCP = 0x0008, Draw_MCA = 0x0010
  };

  enum { SID_OFFS_TRACK = 501 };

  extern int geomOrder[];

  // Color vector (RGBA) for geometries
  extern double rgbaDef [4];
  extern double rgbaUSS [4];
  extern double rgbaCrt [4];
  extern double rgbaMag [4];
  extern double rgbaTRD [4];
  extern double rgbaTRK [4];
  extern double rgbaTOF [4];
  extern double rgbaACC [4];
  extern double rgbaRICH[4];
  extern double rgbaECAL[4];

  EGeomType getType(const char *nodename);
  int    typeToGLID(int type, int id);
  int    glIDToType(int glid);
  int   glIDToSelID(int glid);
  int  getGeomOrder(EGeomType code);
  double   *getRgba(EGeomType code);
};

using namespace gvconst;

#endif
