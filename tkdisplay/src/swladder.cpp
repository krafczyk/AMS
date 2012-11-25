// $Id: swladder.cpp,v 1.6 2012/11/25 15:10:06 shaino Exp $
#include <QtGui>

#include "swladder.h"
#include "root.h"
#include "amsdbc.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TkDBc.h"
#include "TkSens.h"

#include "TString.h"

SWLadder::SWLadder(AMSEventR *event, int tkid) 
  : SubWindow(event, SW_LADDER, WIN_W, WIN_H, TBAR_H), tkID(tkid)
{
  updateEvent(event);
}

SWLadder::~SWLadder()
{
}

int SWLadder::getDobjID() const
{
  if (focusStatus & FOCUS_DOBJ) {
    DrawObj dobj = drawObj.at(focusStatus/FOCUS_DOBJ/2-1);
    return dobj.idx;
  }

  return -1;
}

int SWLadder::getDobjType() const
{
  if (focusStatus & FOCUS_DOBJ) {
    DrawObj dobj = drawObj.at(focusStatus/FOCUS_DOBJ/2-1);
    if ((dobj.atrb & ATR_CLSX) || (dobj.atrb & ATR_CLSY)) return DOBJ_CLS;
    if ( dobj.atrb & ATR_TRK ) return (dobj.atrb/ATR_TRID)*DOBJ_TRK;
    if ( dobj.atrb & ATR_HIT ) return DOBJ_HIT;
  }

  return -1;
}

bool SWLadder::clearFocus()
{
  int fold = focusStatus;
  focusStatus = 0;

  for (int i = 0; i < drawObj.size(); i++)
    drawObj[i].atrb &= ~ATR_SEL;

  return (fold != focusStatus);
}

bool SWLadder::checkFocus(int x, int y)
{
  int fold = focusStatus;

  focusStatus = 0;
  checkClose(x, y);
  checkMore (x, y);

  x -= pX;
  y -= pY;

  int llen = nSen*SEN_DX;
  int px = (wWid-llen)/2;
  int py = (wHei-tHei-AXIS_H-SEN_SY)/2+tHei;

  int fobj = -1, dmin = 0, drng = 5;

  for (int i = 0; i < drawObj.size(); i++) {
    DrawObj dobj = drawObj.at(i);
    drawObj[i].atrb &= ~ATR_SEL;

    int d = -1;
    if (dobj.atrb & ATR_CLSX) {
      if (px+dobj.x-drng <= x && x <= px+dobj.x+drng && 
	  py <= y && y <= py+SEN_SY)
	d = abs(x-px-dobj.x);
    }
    else if (dobj.atrb & ATR_CLSY) {
      if (px <= x && x <= px+nSen*SEN_DX && 
	  py+dobj.y-drng <= y && y <= py+dobj.y+drng)
	d = abs(y-py-dobj.y);
    }
    else if ((dobj.atrb & ATR_HIT) || (dobj.atrb & ATR_TRK)) {
      if (px+dobj.x-drng <= x && x <= px+dobj.x+drng && 
	  py+dobj.y-drng <= y && y <= py+dobj.y+drng)
	d = qMin(abs(x-px-dobj.x), abs(y-py-dobj.y));
    }

    if (d >= 0 && (fobj < 0 || d <= dmin)) {
      fobj = i;
      dmin = d;
    }
  }

  if (fobj >= 0) {
    focusStatus |= FOCUS_DOBJ;
    focusStatus |= FOCUS_DOBJ*(fobj+1)*2;
    drawObj[fobj].atrb |= ATR_SEL;
  }

  return (fold != focusStatus);
}

void SWLadder::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(0, 0, wWid-2, tHei, QColor(0, 0, 0, 200));
  drawClose(pnt);
  drawMore (pnt);

  pnt->setPen(Qt::white);
  drawText(pnt,  28, 10, "Ladder");
  drawText(pnt,  80, 10, Form(" TkID: %d", tkID));
  drawText(pnt, 170, 10, Form("nClsX: %d", nClsX));
  drawText(pnt, 240, 10, Form("nClsY: %d", nClsY));
  drawText(pnt,  78, 30, Form(" nSen: %d", nSen ));
  drawText(pnt, 175, 30, Form(" nHit: %d", nHit ));
  drawText(pnt, 241, 30, Form("nHitT: %d", nHitT));
  drawText(pnt, 310, 30, Form("nHitG: %d", nHitG));

  if (focusStatus & FOCUS_DOBJ) {
    DrawObj dobj = drawObj.at(focusStatus/FOCUS_DOBJ/2-1);
    if ((dobj.atrb & ATR_CLSX) || (dobj.atrb & ATR_CLSY)) {
      TrClusterR *cls = rEvent->pTrCluster(dobj.idx);
      double sig = cls->GetTotSignal(TrClusterR::kAsym);
      drawText(pnt,  27, 50, Form("Cluster    Side: %d", cls->GetSide()));
      drawText(pnt, 140, 50, Form("Seed: %d", cls->GetAddress()
				             +cls->GetSeedIndex()));
      drawText(pnt, 220, 50, Form("Len: %d",  cls->GetLength()));
      drawText(pnt, 270, 50, Form("Signal: %.0f", sig));
      if (dobj.atrb & ATR_CLSX)
	drawText(pnt, 350, 50, Form("Mult: %d", dobj.mult));
    }
    else if (dobj.atrb & ATR_HIT) {
      TrRecHitR *hit = rEvent->pTrRecHit(dobj.idx);
      char cf1 = (hit->OnlyY()) ? 'G' : '_';
      char cf2 = (hit->checkstatus(AMSDBc::USED)) ? 'T' : '_';
      AMSPoint coo = hit->GetCoord(dobj.mult);
      drawText(pnt,  48, 50, Form("Hit     Flag: %c%c", cf1, cf2));
      drawText(pnt, 150, 50, Form("Coo: (%.2f, %.2f, %.2f)",
				  coo.x(), coo.y(), coo.z()));
      drawText(pnt, 350, 50, Form("Mult: %d",   dobj.mult));
    }
    else if (dobj.atrb & ATR_TRK) {
      TrTrackR *trk = rEvent->pTrTrack(dobj.idx);
      int fpat[8];
      for (int i = 0; i < 8; i++) fpat[i] = 0;

      TString hpat;
      for (int i = 0; i < trk->GetNhits(); i++) {
	TrRecHitR *hit = trk->GetHit(i);
	int ily = hit->GetLayer()-1;
	fpat[ily] = (hit->OnlyY()) ? 1 : 2;
      }
      for (int i = 0; i < 8; i++) {
	if      (fpat[i] == 1) hpat += "Y";
	else if (fpat[i] == 2) hpat += "O";
	else                   hpat += "_";
      }

      drawText(pnt,  48, 50, "Track");
      drawText(pnt, 100, 50, Form("nHit: %d",   trk->GetNhits()));
      drawText(pnt, 170, 50, Form("nHitXY: %d", trk->GetNhitsXY()));
      drawText(pnt, 270, 50, Form("fPat: %s",   hpat.Data()));
    }
  }
}

void SWLadder::drawObjects(QPainter *pnt)
{
  drawLadder(pnt);
  drawHits  (pnt);
}

void SWLadder::updateEvent(AMSEventR *event)
{
  rEvent = event;

  drawObj.clear();
  fillClsVec();
  fillHitVec();
  fillTrkVec();
}

void SWLadder::drawLadder(QPainter *pnt)
{
  int llen = nSen*SEN_DX;
  int px = (wWid-llen)/2;
  int py = (wHei-tHei-AXIS_H-SEN_SY)/2+tHei;

  for (int i = 0; i < nSen; i++) {
    pnt->setPen(Qt::darkCyan);
    pnt->drawRect(px+i*SEN_DX, py, SEN_SX, SEN_SY);
    pnt->fillRect(px+i*SEN_DX, py, SEN_SX, SEN_SY, QColor(0, 255, 255, 150));
    float xofs = (i < 10) ? 0.4 : 0.2;

    pnt->setPen(Qt::white);
    drawText(pnt, px+(i+xofs)*SEN_DX, py-17, Form("%d", i));
  }

  TkLadder *lad = TkDBc::Head->FindTkId(tkID);
  if (!lad) return;

  int x0, x1, y0, y1, dx, dy;
  if (lad->GetRotMat().GetEl(0, 0) > 0) { x0 =  0; x1 = 20; dx =  1; }
  else                                  { x0 = 20; x1 =  0; dx = -1; }
  if (lad->GetRotMat().GetEl(1, 1) < 0) { y0 =  0; y1 = 20; dy =  1; }
  else                                  { y0 = 20; y1 =  0; dy = -1; }

  int ax = 10, ay = wHei-27;
  pnt->setPen(Qt::gray);
  pnt->drawLine(ax+x0, ay+y0, ax+x1, ay+y0);
  pnt->drawLine(ax+x0, ay+y0, ax+x0, ay+y1);
  pnt->drawLine(ax+x1, ay+y0, ax+x1-2*dx, ay+y0+2);
  pnt->drawLine(ax+x1, ay+y0, ax+x1-2*dx, ay+y0-2);
  pnt->drawLine(ax+x0, ay+y1, ax+x0+2, ay+y1-2*dy);
  pnt->drawLine(ax+x0, ay+y1, ax+x0-2, ay+y1-2*dy);
}
 
void SWLadder::drawHits(QPainter *pnt)
{
  int llen = nSen*SEN_DX;
  int px = (wWid-llen)/2;
  int py = (wHei-tHei-AXIS_H-SEN_SY)/2+tHei;

  for (int i = 0; i < (int)drawObj.size(); i++) {
    DrawObj dobj = drawObj.at(i);

    Qt::GlobalColor qc1 = (Qt::GlobalColor)dobj.col1;
    Qt::GlobalColor qc2 = (Qt::GlobalColor)dobj.col2;

    if (dobj.atrb & ATR_SEL) {
      qc1 = Qt::white;
      qc2 = Qt::gray;
    }

    if (dobj.atrb & ATR_CLSX) {
      int x1 = dobj.x, y1 = 1;
      int x2 = dobj.x, y2 = SEN_SY-1;
      pnt->setPen(qc1); pnt->drawLine(px+x1, py+y1, px+x2, py+y2);
      pnt->setPen(qc2); pnt->drawLine(px+x1+1, py+y1, px+x2+1, py+y2);
    }

    else if (dobj.atrb & ATR_CLSY) {
      for (int j = 0; j < nSen; j++) {
	int y1 = dobj.y, x1 = SEN_DX*j+1;
	int y2 = dobj.y, x2 = x1+SEN_SX-2;
	pnt->setPen(qc1); pnt->drawLine(px+x1, py+y1, px+x2, py+y2);
	pnt->setPen(qc2); pnt->drawLine(px+x1, py+y1+1, px+x2, py+y2+1);
      }
    }

    else if ((dobj.atrb & ATR_HIT) && !(dobj.atrb & ATR_TRK)) {
      QColor col1(qc1);
      QColor col2(qc2);
      if (qc1 == Qt::yellow) {
	col1 = QColor(255, 120, 90, 255);
	col2 = QColor(127,  75, 50, 255);
      }
      pnt->setPen(Qt::white); 
      pnt->drawRect(px+dobj.x-3, py+dobj.y-3, 6, 6);
      pnt->setPen(col2); 
      pnt->drawRect(px+dobj.x-2, py+dobj.y-2, 6, 6);
      pnt->fillRect(px+dobj.x-2, py+dobj.y-2, 6, 6, QColor(col1));
    }
  }


  for (int i = 0; i < (int)drawObj.size(); i++) {
    DrawObj dobj = drawObj.at(i);

    Qt::GlobalColor qc1 = (Qt::GlobalColor)dobj.col1;
    Qt::GlobalColor qc2 = (Qt::GlobalColor)dobj.col2;

    if (dobj.atrb & ATR_SEL) {
      qc1 = Qt::white;
      qc2 = Qt::gray;
    }
    if (dobj.atrb & ATR_TRK) {
      QColor col1(qc1);
      QColor col2(qc2);
      if (qc1 == Qt::yellow) {
	col1 = QColor(255, 120, 90, 255);
	col2 = QColor(127,  75, 50, 255);
      }
      pnt->setPen(Qt::white); 
      pnt->drawRect(px+dobj.x-3, py+dobj.y-3, 6, 6);
      pnt->setPen(col2); 
      pnt->drawRect(px+dobj.x-2, py+dobj.y-2, 6, 6);
      pnt->fillRect(px+dobj.x-2, py+dobj.y-2, 6, 6, QColor(col1));
    }
  }
}

void SWLadder::fillClsVec()
{
  TkLadder *lad = TkDBc::Head->FindTkId(tkID);
  if (!lad) return;

  nSen = lad->GetNSensors();

  nClsX = nClsY = 0;

  for (int i = 0; rEvent && i < rEvent->nTrCluster(); i++) {
    TrClusterR *cls = rEvent->pTrCluster(i);
    if (!cls) continue;
    if (cls->GetTkId() != tkID) continue;

    int side = cls->GetSide();
    if (side == 0) nClsX++;
    else           nClsY++;

    int rhit = cls->GetAddress();
    int mult = (side == 0) ? cls->GetMultiplicity() : 1;

    for (int j = 0; j < mult; j++) {
      DrawObj dobj;
      dobj.idx  = i;
      dobj.atrb = (side == 0) ? ATR_CLSX    : ATR_CLSY;
      dobj.col1 = (side == 0) ? Qt::red     : Qt::blue;
      dobj.col2 = (side == 0) ? Qt::darkRed : Qt::darkBlue;
      dobj.mult = j;
      dobj.x = dobj.y = 0;

      if (side == 0)
	dobj.x = SEN_DX*TkCoo::GetLocalCoo(cls->GetTkId(), rhit, j)
	               /TkDBc::Head->_SensorPitchK;
      else
	dobj.y = SEN_SY-SEN_SY*TkCoo::GetLocalCoo(cls->GetTkId(), rhit, 0)
	                      /TkDBc::Head->_ssize_active[1];

      drawObj.push_back(dobj);
    }
  }
}

void SWLadder::fillHitVec()
{
  nHit = nHitT = nHitG = 0;

  for (int i = 0; rEvent && i < rEvent->nTrRecHit(); i++) {
    TrRecHitR *hit = rEvent->pTrRecHit(i);
    if (!hit) continue;
    if (hit->GetTkId() != tkID) continue;

    nHit++;
    DrawObj dobj;
    int im1 = 0, im2 = hit->GetMultiplicity()-1;
    dobj.idx  = i;
    dobj.col1 = Qt::magenta;
    dobj.col2 = Qt::darkMagenta;
    dobj.atrb = ATR_HIT;

    if (hit->checkstatus(AMSDBc::USED)) {
      nHitT++;
      im1 = im2 = hit->GetResolvedMultiplicity();
      dobj.col1 = Qt::yellow;
      dobj.col2 = Qt::darkYellow;
      dobj.atrb |= ATR_TRK;
    }
    else if (hit->OnlyY()) {
      nHitG++;
      continue;
    }

    double sax = TkDBc::Head->_ssize_active[0];
    double say = TkDBc::Head->_ssize_active[1];

    for (int j = im1; j <= im2; j++) {
      AMSPoint coo = hit->GetCoord(j);
      TkSens sens(coo, 0);
      dobj.mult = j;
      dobj.x = SEN_SX*sens.GetSensCoo().x()/sax+SEN_DX*sens.GetSensor();;
      dobj.y = SEN_SY-SEN_SY*sens.GetSensCoo().y()/say;
      drawObj.push_back(dobj);
    }
  }
}

void SWLadder::fillTrkVec()
{
  nTrk = 0;

  int layer = abs(tkID)/100;
  for (int i = 0; rEvent && i < rEvent->nTrTrack(); i++) {
    TrTrackR *trk = rEvent->pTrTrack(i);
    if (!trk) continue;

    int hid = -1;
    for (int j = 0; j < trk->GetNhits(); j++) {
      TrRecHitR *hit = trk->GetHit(j);
      if (!hit) continue;
      if (hit->GetLayer() == layer) {
	hid = j;
	if (hit->GetTkId() != tkID) continue;
	for (int k = 0; k < drawObj.size(); k++) {
	  if ((drawObj[k].atrb & ATR_TRK) &&
	      rEvent->pTrRecHit(drawObj[k].idx) == hit)
	    drawObj[k].atrb |= ATR_TRID*(i+1);
	}
      }
    }
    if (hid >= 0) continue;

    AMSPoint pnt;
    AMSDir dir;
    trk->Interpolate(TkDBc::Head->GetZlayer(layer), pnt, dir);
    TkSens sens(pnt, 0);
    if (sens.GetLadTkID() != tkID) continue;

    double sax = TkDBc::Head->_ssize_active[0];
    double say = TkDBc::Head->_ssize_active[1];

    DrawObj dobj;    
    dobj.idx  = i;
    dobj.mult = sens.GetMultIndex();
    dobj.col1 = Qt::yellow;
    dobj.col2 = Qt::darkYellow;
    dobj.atrb = ATR_TRK | ATR_TRID*(i+1);
    dobj.x = SEN_SX*sens.GetSensCoo().x()/sax+SEN_DX*sens.GetSensor();;
    dobj.y = SEN_SY-SEN_SY*sens.GetSensCoo().y()/say;
    drawObj.push_back(dobj);

    nTrk++;
  }
}
