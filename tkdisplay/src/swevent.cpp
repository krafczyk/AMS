// $Id: swevent.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>

#include "swevent.h"
#include "root.h"

#include "TString.h"

SWEvent::SWEvent(AMSEventR *event) 
  : SubWindow(event, SW_EVENT, WIN_W, WIN_H, TBAR_H, POS_TOP, 0)
{
}

SWEvent::~SWEvent()
{
}

void SWEvent::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(QRect(0, 0, wWid, tHei), QColor(0, 0, 0, 130));

  if (!rEvent) return;

  pnt->setPen(Qt::white);
  drawText(pnt,   5, 5, Form("Run: %d",    rEvent->Run()));
  drawText(pnt, 125, 5, Form("Event: %d",  rEvent->Event()));
  drawText(pnt, 225, 5, Form("nTrack: %d", rEvent->nTrTrack()));
  drawText(pnt, 300, 5, Form("nHit: %d",   rEvent->nTrRecHit()));
  drawText(pnt, 365, 5, Form("nCls: %d",   rEvent->nTrCluster()));

  drawMore(pnt);
}
