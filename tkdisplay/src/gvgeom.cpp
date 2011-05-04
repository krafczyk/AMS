// $Id: gvgeom.cpp,v 1.3 2011/05/04 16:36:54 pzuccon Exp $
#include <algorithm>
#include "TFile.h"
#include "TGeometry.h"
#include "TNode.h"
#include "TShape.h"
#include "TBuffer3D.h"
#include "TTUBE.h"
#include "TCONE.h"
#include "TPCON.h"

#include "gvgeom.h"
#include "gvconst.h"

int  GVGeom::tubeNdiv = 60;
bool GVGeom::geomEnabled[Num_Geom];

GVGeom::GVGeom(TNode *node, int id, EGeomType type) 
  : GLRender(id), geomType(type)
{
  setMatCol(getRgba(geomType));

  double scol[4];
  for (int i = 0; i < 4; i++)
    scol[i] = (matCol[i] > 0) ? std::min(matCol[i]+0.1, 1.0) : 0.2;
  setSelCol(scol);

  if (node) buildGL(node);
}

GVGeom::~GVGeom()
{
}

int GVGeom::buildGeom(TNode *node, bool first)
{
  static int glID[Num_Geom], nTot = 0;
  if (first) {
    for (int i = 0; i < Num_Geom; i++) glID[i] = 0;
    for (int i = 0; i < Num_Geom; i++) geomEnabled[i] = true;
    nTot = 0;
  }

  if (!node) return 0;

  TList *nlist = node->GetListOfNodes();
  if (!nlist) return 0;

  TNode *cnode = (TNode*)nlist->First();

  while (cnode) {
    if (cnode->GetVisibility()) {
      EGeomType type = ::getType(cnode->GetName());
      addObject(new GVGeom(cnode, typeToGLID(type, ++glID[type]), type));
      nTot++;
    }

    if (cnode->GetListOfNodes() && 
	cnode->GetListOfNodes()->GetSize() > 0)
      buildGeom(cnode, false);

    cnode = (TNode *)nlist->After(cnode);
  }

  return nTot;
}

class compGeomOrder {
public:
  bool operator() (const GLRender *obj1, const GLRender *obj2) const {
    int ord1 = getGeomOrder(((GVGeom *)obj1)->getType());
    int ord2 = getGeomOrder(((GVGeom *)obj2)->getType());
    return ( ord1 <  ord2 || 
	    (ord1 == ord2 && obj1->getGLID() < obj2->getGLID()));
  }
};

void GVGeom::sortGeom()
{
  std::sort(svRender.begin(), svRender.end(), compGeomOrder());
}

void GVGeom::setEnabledAll(int type, bool enabled)
{
  for (int i = 0; i < svRender.size(); i++) {
    GLRender *glr = svRender.at(i);
    if (glr && glIDToType(glr->getGLID()) == type) glr->setEnabled(enabled);
  }
  geomEnabled[type] = enabled;
}

void GVGeom::buildGL(TNode *node)
{
  TShape *shape = node->GetShape();
  if (!shape) return;

  TString str = shape->ClassName();
  if (str == "TTUBE")((TTUBE *)shape)->SetNumberOfDivisions(tubeNdiv);
  if (str == "TCONE")((TCONE *)shape)->SetNumberOfDivisions(tubeNdiv);
  if (str == "TPCON")((TPCON *)shape)->SetNumberOfDivisions(tubeNdiv);
  if (str == "TXTRU") invNormal = true;

  updateMatrix(node);
  const TBuffer3D &buf = shape->GetBuffer3D(TBuffer3D::kAll);

  GLRender::buildGL(buf.NbPnts(), buf.NbSegs(), buf.NbPols(), 
		    buf.fPnts, buf.fSegs, buf.fPols);
}

void GVGeom::updateMatrix(TNode *node)
{
  enum { NMAX = 20 };
  TNode *vnode[NMAX];

  int glv = 0;
  while (node) {
    vnode[glv] = node;
    node = node->GetParent();
    glv++;
  }
  glv--;

  double trav[NMAX][3];
  double rotm[NMAX][9];
  for (int i = 0; i < 3; i++) trav[0][i] = 0;
  for (int i = 0; i < 9; i++) rotm[0][i] = 0;
  rotm[0][0] = rotm[0][4] = rotm[0][8] = 1;

  for (int i = 1; i <= glv; i++) {
    node = vnode[glv-i];
    node->UpdateTempMatrix((double*)&trav[i-1], (double*)&rotm[i-1],
			   node->GetX(), node->GetY(), node->GetZ(),
			   node->GetMatrix()->GetMatrix(), 
			   (double*)&trav[i], (double*)&rotm[i]);
  }

  gGeometry->SetGeomLevel(0);
  gGeometry->UpdateTempMatrix(0, 0, 0, 0, false);
  gGeometry->SetGeomLevel(1);
  gGeometry->UpdateTempMatrix(trav[glv][0], trav[glv][1], trav[glv][2],
			      rotm[glv], false);
}
