// $Id: subwindow.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>

#include "subwindow.h"

SubWindow::SubWindow(AMSEventR *event, int type, int wid, int hei, int thei, 
		     int pos, int brd)
  : rEvent(event), swType(type), wWid(wid), wHei(hei), tHei(thei), 
    wPos(pos), fBorder(brd)
{
  focusStatus = 0;

  pX = pY = 0;
}

SubWindow::~SubWindow()
{
}

bool SubWindow::clearFocus()
{
  int fold = focusStatus;
  focusStatus = 0;
  return (fold != focusStatus);
}

bool SubWindow::checkFocus(int x, int y)
{
  int fold = focusStatus;

  focusStatus = 0;
  checkClose(x, y);
  checkMore (x, y);

  return (fold != focusStatus);
}

void SubWindow::drawWindow(QPainter *pnt, int wid, int hei, float scale)
{
  if (scale <= 0) return;

  int ww = wWid*scale;
  int hh = wHei*scale;
  if (wPos == POS_CENTER) {
    pX = (wid-ww)/2;
    pY = (hei-hh)/2;
  }
  else if (wPos == POS_TOP) {
    pX = (wid-ww)/2;
    pY = 0;
  }

  pnt->save();
  pnt->translate(pX, pY);

  if (fBorder) {
    pnt->setPen(Qt::white);
    pnt->drawRect(-1, -1, ww, hh);
  }
  pnt->fillRect(0, 0, ww, hh, QColor(0, 50, 0, 200));

  drawStatus = 0;

  if (scale > 0.99) {
    drawInfobar(pnt);
    drawObjects(pnt);
  }

  pnt->restore();
}

void SubWindow::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(QRect(0, 0, wWid-1, tHei), QColor(0, 0, 0, 200));
  drawClose(pnt);
}

void SubWindow::drawObjects(QPainter *)
{
}

void SubWindow::drawClose(QPainter *pnt)
{
  pnt->setPen(QColor(255, 50, 50, 200));
  if (focusStatus & FOCUS_CLOSE) {
    pnt->setPen(Qt::red);
    pnt->drawRect(CLOSE_X, CLOSE_Y, CLOSE_W, CLOSE_H);
    pnt->fillRect(CLOSE_X, CLOSE_Y, CLOSE_W, CLOSE_H,
		  QColor(255, 255, 255, 50));
  }
  pnt->drawLine(CLOSE_X+4, CLOSE_Y+4, CLOSE_X+CLOSE_W-4, 
		                      CLOSE_Y+CLOSE_H-4);
  pnt->drawLine(CLOSE_X+4, CLOSE_Y+CLOSE_H-4, 
		           CLOSE_X+CLOSE_W-4, CLOSE_Y+4);

  drawStatus |= FOCUS_CLOSE;
}

void SubWindow::checkClose(int x, int y)
{
  if (drawStatus & FOCUS_CLOSE) {
    x -= pX;
    y -= pY;
    if (CLOSE_X <= x && x <= CLOSE_X+CLOSE_W && 
	CLOSE_Y <= y && y <= CLOSE_Y+CLOSE_H) focusStatus |= FOCUS_CLOSE;
  }
}

void SubWindow::drawMore(QPainter *pnt)
{
  int x = wWid-MORE_W-MORE_X;
  int y = tHei-MORE_H-MORE_Y;

  pnt->setPen(QColor(255, 50, 50, 200));
  if (focusStatus & FOCUS_MORE) {
    pnt->setPen(Qt::red);
    pnt->drawRect(x, y, MORE_W, MORE_H);
    pnt->fillRect(x, y, MORE_W, MORE_H, QColor(255, 255, 255, 50));
  }
  drawText(pnt, x+2, y-1, "more");

  drawStatus |= FOCUS_MORE;
}

void SubWindow::checkMore(int x, int y)
{
  if (drawStatus & FOCUS_MORE) {
    x -= pX;
    y -= pY;

    int mx = wWid-MORE_W-MORE_X;
    int my = tHei-MORE_H-MORE_Y;
    if (mx <= x && x <= mx+MORE_W && 
	my <= y && y <= my+MORE_H) focusStatus |= FOCUS_MORE;
  }
}

void SubWindow::drawText(QPainter *pnt, int x, int y, 
			 const char *text, int opt)
{
  pnt->drawText(x, y, wWid-x, tHei, opt, text);
}
