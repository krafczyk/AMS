// $Id: swtrack.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// SWTrack : a class to manage track information sub window by SH
//
#ifndef SWTRACK_H
#define SWTRACK_H

#include "subwindow.h"

class SWTrack : public SubWindow {

public:
  SWTrack(AMSEventR *event, int tid);
 ~SWTrack();

  virtual int getID() const { return tID; }
  virtual int getDobjID  () const;
  virtual int getDobjType() const;

  int getFocusHid();

  virtual bool checkFocus(int x, int y);

protected:
  virtual void drawInfobar(QPainter *pnt);
  virtual void drawObjects(QPainter *pnt);

  void drawHits(QPainter *pnt);

  int tID;
  int fitPat[8];

  enum { FPAT_HITT = 1, FPAT_HITY = 2 };

  enum { WIN_W = 460, WIN_H = 240, TBAR_H = 100, 
	 LYR_H = 110, LYR_D =  10, LYR_PX =  20, RES_H = 20 };

  static int zLayer[8];
};

#endif
