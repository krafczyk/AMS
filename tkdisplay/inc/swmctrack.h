// $Id: swmctrack.h,v 1.1 2010/05/10 21:55:46 shaino Exp $
//
// SWMCTrack : a class to manage MC track information sub window by SH
//
#ifndef SWMCTRACK_H
#define SWMCTRACK_H

#include "subwindow.h"

class SWMCTrack : public SubWindow {

public:
  SWMCTrack(AMSEventR *event, int tid);
 ~SWMCTrack();

  virtual int getID() const { return tID; }

protected:
  virtual void drawInfobar(QPainter *pnt);

protected:
  int tID;

  enum { WIN_W = 460, WIN_H = 82, TBAR_H = 80 };
};

#endif
