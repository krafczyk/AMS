// $Id: swevent.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// SWEvent : a class to manage event information sub window by SH
//
#ifndef SWEVENT_H
#define SWEVENT_H

#include "subwindow.h"

class SWEvent : public SubWindow {

public:
  SWEvent(AMSEventR *event = 0);
 ~SWEvent();

protected:
  virtual void drawInfobar(QPainter *pnt);

  enum { WIN_W = 480, WIN_H = 30, TBAR_H = 30 };
};

#endif
