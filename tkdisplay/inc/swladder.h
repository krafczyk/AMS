// $Id: swladder.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// SWLadder : a class to manage ladder information sub window by SH
//
#ifndef SWLADDER_H
#define SWLADDER_H

#include "subwindow.h"

#include <vector>

class SWLadder : public SubWindow {

public:
  SWLadder(AMSEventR *event, int tkid);
 ~SWLadder();

  virtual int getID() const { return tkID; }
  virtual int getDobjID  () const;
  virtual int getDobjType() const;

  virtual bool clearFocus();
  virtual bool checkFocus(int x, int y);

protected:
  virtual void drawInfobar(QPainter *pnt);
  virtual void drawObjects(QPainter *pnt);

  void drawLadder(QPainter *pnt);
  void drawHits  (QPainter *pnt);
  void fillClsVec();
  void fillHitVec();
  void fillTrkVec();

private:
  int tkID;
  int nSen;

  int nClsX;
  int nClsY;
  int nHit;
  int nHitT;
  int nHitG;
  int nTrk;

  enum { ATR_CLSX = 0x01,  ATR_CLSY = 0x02, 
	 ATR_HIT  = 0x10,  ATR_TRK  = 0x20, 
	 ATR_SEL  = 0x100, ATR_TRID = 0x200};

  struct DrawObj {
    int idx;
    int atrb;
    int x, y;
    int mult;
    int col1, col2;
  };
  std::vector<DrawObj> drawObj;

  enum { WIN_W  = 460, WIN_H  = 190, TBAR_H = 75, AXIS_H = 10,
	 SEN_DX =  30, SEN_SX =  29, SEN_SY = 51 };
};

#endif
