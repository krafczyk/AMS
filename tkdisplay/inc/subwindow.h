// $Id: subwindow.h,v 1.3 2010/12/13 21:43:08 shaino Exp $
//
// SubWindow : a class to manage sub window on the main display by SH
//
#ifndef SUBWINDOW_H
#define SUBWINDOW_H


class AMSEventR;

QT_BEGIN_NAMESPACE
class QPainter;
QT_END_NAMESPACE

class SubWindow {

public:
  SubWindow(AMSEventR *event, int type, int wid, int hei, int thei, 
	    int pos = POS_CENTER, int brd = 1);
 ~SubWindow();

  enum { FOCUS_CLOSE = 1, FOCUS_MORE = 2, FOCUS_DOBJ = 4, FOCUS_OPT = 0x100 };
  enum { SW_EVENT = 1, SW_TRACK = 2, SW_LADDER = 3 };
  enum { DOBJ_LAD = 1, DOBJ_CLS = 2, DOBJ_HIT  = 3, DOBJ_TRK = 4 };

  void setEvent(AMSEventR *evt = 0) { rEvent = evt; }
  int getType() const { return swType; }

  unsigned int getFocus() const { return focusStatus; }
  bool  isFocus(int stat) const { return focusStatus & stat; }

  virtual int getID() const { return 0; }
  virtual int getDobjID  () const { return -1; }
  virtual int getDobjType() const { return -1; }

  virtual bool clearFocus();
  virtual bool checkFocus(int x, int y);
  virtual void drawWindow(QPainter *pnt, int wid, int hei, float scale = 1);
  virtual void procMpress() {}
  virtual void updateEvent(AMSEventR *event) { rEvent = event; }

protected:
  virtual void drawInfobar(QPainter *pnt);
  virtual void drawObjects(QPainter *pnt);

  void drawClose (QPainter *pnt);
  void checkClose(int x, int y);

  void drawMore (QPainter *pnt);
  void checkMore(int x, int y);

  void drawText(QPainter *pnt, int x, int y, const char *text,
		int opt = Qt::AlignLeft | Qt::TextWordWrap);

protected:
  AMSEventR *rEvent;

  int swType;

  unsigned int focusStatus;
  unsigned int drawStatus;

  int pX, pY;
  int wWid, wHei, tHei;
  int wPos, fBorder;

  enum { POS_CENTER, POS_TOP };
  enum { CLOSE_X =  5, CLOSE_Y = 5, CLOSE_W = 15, CLOSE_H = 15,
	 MORE_X  = 10, MORE_Y  = 5, MORE_W  = 35, MORE_H  = 15 };
};

#endif
