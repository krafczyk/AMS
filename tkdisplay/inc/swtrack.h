// $Id: swtrack.h,v 1.2 2010/05/10 21:55:46 shaino Exp $
//
// SWTrack : a class to manage track information sub window by SH
//
#ifndef SWTRACK_H
#define SWTRACK_H

#include "subwindow.h"
#include "TkDBc.h"

class SWTrack : public SubWindow {

public:
  SWTrack(AMSEventR *event, int tid);
 ~SWTrack();

  virtual int getID() const { return tID; }
  virtual int getDobjID  () const;
  virtual int getDobjType() const;

  int getFocusHid();

  virtual bool checkFocus(int x, int y);
  virtual void procMpress();

protected:
  virtual void drawInfobar(QPainter *pnt);
  virtual void drawObjects(QPainter *pnt);

  void drawButtons (QPainter *pnt);
  void drawHits    (QPainter *pnt);
  bool checkButtons(int x, int y);

  enum { LSTA_HITT = 1, LSTA_HITY = 2 };

  enum { WIN_W = 460, WIN_H = 260, TBAR_H = 110, 
	 BFP_X = 140, BFP_W =  55, BLY_X  = 30, BLY_W  = 15,
	 LYR_H = 120, LYR_D =  10, LYR_PX = 20, LYR_PY =  5, RES_H = 20 };

  enum { NPAR = 4, NLAY = trconst::maxlay };

  int tID;
  int lyrSta[NLAY];
  int fitPar[NPAR];

  static int fID;
  static int lyrConf;

  static int zLayer1[NLAY];    // Original Tracker conf. (S.C. magnet)
  static int zLayer2[NLAY*4];  // New Tracker conf. (AKA plan-B)
};

#endif
