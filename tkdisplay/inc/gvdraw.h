// $Id: gvdraw.h,v 1.1 2010/05/10 21:55:46 shaino Exp $
//
// GVDraw : a class to manage drawing of AMS geometry and hits on OpenGL
//          by SH
//
#ifndef GVDRAW_H
#define GVDRAW_H

#include "glwidget.h"
#include "gllight.h"
#include "TkDBc.h"

class AMSEventR;
class TkLadder;
class TNode;

class GVDraw {

public:
  GVDraw();
 ~GVDraw();

  bool testOpt(int opt) { return (drawOpt & opt); }
  void addOpt (int opt) { drawOpt |=  opt; }
  void delOpt (int opt) { drawOpt &= ~opt; }
  void setOpt (int opt, bool sw) { (sw) ? addOpt(opt) : delOpt(opt); }

  TkLadder *getLadder(int sid) const;

  void setEnable(int type, bool sw);

  void setCurrEvent(AMSEventR *event) { currEvent = event; }

  void drawObject(bool render, bool select, int idsel);

  void initGeom(const char *fname = "geom/geom.root",
		const char *key   = "ams02",
		const char *nname = "FMOT1");

protected:
  void drawAMSgeom  (bool render, bool select, int idsel);
  void drawTracker  (bool render, bool select, int idsel);
  void drawTrTracks (bool render, bool select, int idsel);
  void drawTrdTracks(bool render, bool select, int idsel);
  void drawMCTracks (bool render, bool select, int idsel);

  void drawTrClusters();
  void drawTrRecHits ();
  void drawTrdHits ();
  void drawTofHits ();
  void drawRichHits();
  void drawEcalHits();
  void drawAxes();

  void buildTracker();

  void buildLines();
  void fillHitVec(int *vecx, int *vecy);

public:
  static int tkSetup;

protected:
  enum { NLAY = trconst::maxlay };

  int  drawOpt;
  int *ladVec;

  AMSEventR *currEvent;
};

#endif
