// $Id: gvgeom.h,v 1.1 2010/05/10 21:55:46 shaino Exp $
//
// GVGeom : a class to convert TGeometry object into GLRender  by SH
//
#ifndef GVGEOM_H
#define GVGEOM_H

#include "gvconst.h"
#include "glrender.h"

class TNode;

class GVGeom : public GLRender {

public:
  GVGeom(TNode *node, int id, EGeomType type);
 ~GVGeom();

  EGeomType getType() const { return geomType; }

  static int  buildGeom(TNode *node, bool first = true);
  static int  buildTracker();
  static void sortGeom();

  static void setEnabledAll(int type, bool enabled = true);
  static bool isGeomEnabled(int type) { 
    return (0 <= type && type <= Num_Geom) ? geomEnabled[type] : false;
  }

protected:
  void buildGL     (TNode *node);
  void updateMatrix(TNode *node);

  EGeomType geomType;

  static int tubeNdiv;
  static bool geomEnabled[Num_Geom];
};

#endif
