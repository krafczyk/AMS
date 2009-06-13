// $Id: swtrack.cpp,v 1.1 2009/06/13 21:40:48 shaino Exp $
#include <QtGui>

#include "swtrack.h"
#include "root.h"
#include "amsdbc.h"
#include "TrTrack.h"
#include "TrCluster.h"
#include "TrRecHit.h"
#include "TkDBc.h"
#include "TkSens.h"

#include "TString.h"
#include "TMath.h"

int SWTrack::zLayer[8] = { 180, 100, 86, 6, -8, -86, -100, -180 };

SWTrack::SWTrack(AMSEventR *event, int tid) 
  : SubWindow(event, SW_TRACK, WIN_W, WIN_H, TBAR_H), tID(tid)
{
  for (int i = 0; i < 8; i++) fitPat[i] = 0;

  TrTrackR *trk = rEvent->pTrTrack(tID);
  if (!trk) return;

  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    int ily = hit->GetLayer()-1;
    if (hit->OnlyY()) fitPat[ily] = FPAT_HITY;
    else fitPat[ily] = FPAT_HITT;
  }

  for (int i = 0; i < 8; i++) {
    if (fitPat[i] == 0) {
      AMSPoint pnt;
      AMSDir dir;
      trk->Interpolate(TkDBc::Head->GetZlayer(i+1), pnt, dir);
      TkSens sens(pnt);
      fitPat[i] = sens.GetLadTkID();
    }
  }
}

SWTrack::~SWTrack()
{
}

int SWTrack::getDobjID() const
{
  if (focusStatus & FOCUS_DOBJ) {
    int ifc = focusStatus/FOCUS_DOBJ/2-1;
    return (abs(fitPat[ifc]) < 10) ? ifc : fitPat[ifc];
  }
  return -1;
}

int SWTrack::getDobjType() const
{
  if (focusStatus & FOCUS_DOBJ) {
    int ifc = focusStatus/FOCUS_DOBJ/2-1;
    return (abs(fitPat[ifc]) < 10) ? DOBJ_HIT : DOBJ_LAD;
  }
  return -1;
}

int SWTrack::getFocusHid()
{
  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if (ifc < 0) return -1;

  TrTrackR *trk = rEvent->pTrTrack(tID);
  if (!trk) return -1;

  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    if (hit && hit->GetLayer()-1 == ifc) return i;
  }

  return -1;
}

bool SWTrack::checkFocus(int x, int y)
{
  int fold = focusStatus;

  focusStatus = 0;
  checkClose(x, y);
  checkMore (x, y);

  x -= pX;
  y -= pY;

  int zl   = zLayer[0]-zLayer[7];
  int px   = (wWid-zl)/2+LYR_PX+zLayer[0];
  int py   = (wHei-tHei-LYR_H)/2+tHei;
  int drng = 5;

  for (int i = 0; i < 8; i++) {
    if (px-zLayer[i]-drng <= x && x <= px-zLayer[i]+drng &&
	py-drng <= y && y <= py+LYR_H+drng) {
      focusStatus |= FOCUS_DOBJ;
      focusStatus |= FOCUS_DOBJ*(i+1)*2;
    }
  }
  
  return (fold != focusStatus);
}

void SWTrack::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(QRect(0, 0, wWid, tHei), QColor(0, 0, 0, 200));
  drawClose(pnt);
  drawMore (pnt);

  TrTrackR *trk = rEvent->pTrTrack(tID);

  TString hpat;
  for (int i = 0; i < 8; i++) {
    if      (fitPat[i] == FPAT_HITY) hpat += "Y";
    else if (fitPat[i] == FPAT_HITT) hpat += "O";
    else                             hpat += "_";
  }

  pnt->setPen(Qt::white);
  drawText(pnt,  28, 10, Form("Track [%d]", tID));
  drawText(pnt, 100, 10, Form("nHit: %d",    trk->GetNhits()));
  drawText(pnt, 170, 10, Form("nHitXY: %d",  trk->GetNhitsXY()));
  drawText(pnt, 270, 10, Form("fPat: %s",  hpat.Data()));
  drawText(pnt, 100, 30, Form("chi2/Ndf (X): %.2f/%d", 
			      trk->GetChisqX(), trk->GetNdofX()));
  drawText(pnt, 270, 30, Form("chi2/Ndf (Y): %.2f/%d", 
			      trk->GetChisqY(), trk->GetNdofY()));
  drawText(pnt, 100, 50, Form("rigidity: %.2f", trk->GetRigidity()));
  drawText(pnt, 270, 50, Form("errRinv: %.2f",  trk->GetErrRinv()));

  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if (ifc >= 0) {
    int iht = getFocusHid();
    TrRecHitR *hit = trk->GetHit(iht);

    if (hit) {
      char cf1 = (hit->OnlyY()) ? 'G' : '_';
      char cf2 = (hit->checkstatus(AMSDBc::USED)) ? 'T' : '_';

      int ily = hit->GetLayer()-1;
      drawText(pnt,  40, 70, Form("Hit [%d]", iht));
      drawText(pnt,  90, 70, Form("TkID: %d", hit->GetTkId()));
      drawText(pnt, 180, 70, Form("Flag: %c%c", cf1, cf2));
      drawText(pnt, 250, 70, Form("Residual: %.4f %.4f", 
				  trk->GetResidual(ily).x(),
				  trk->GetResidual(ily).y()));
    }
    else if (abs(fitPat[ifc]) > 10) {
      int tkid = fitPat[ifc];

      int nclx = 0, ncly = 0, nhit = 0, nhtt = 0, nhtg = 0;
      for (int i = 0; rEvent && i < rEvent->nTrCluster(); i++) {
	TrClusterR *cls = rEvent->pTrCluster(i);
	if (!cls || cls->GetTkId() != tkid) continue;
	if (cls->GetSide() == 0) nclx++; 
	else ncly++;
      }
      for (int i = 0; rEvent && i < rEvent->nTrRecHit(); i++) {
	TrRecHitR *hit = rEvent->pTrRecHit(i);
	if (!hit || hit->GetTkId() != tkid) continue;
	nhit++;
	if (hit->checkstatus(AMSDBc::USED)) nhtt++;
	else if (hit->OnlyY()) nhtg++;
      }

      drawText(pnt,  40, 70, "Ladder");
      drawText(pnt,  90, 70, Form("TkID: %d", tkid));
      drawText(pnt, 170, 70, Form("nClsX/Y: %d/%d", nclx, ncly));
      drawText(pnt, 270, 70, Form("nHit/T/G: %d/%d/%d", nhit, nhtt, nhtg));
    }
  }
}

void SWTrack::drawObjects(QPainter *pnt)
{
  drawHits(pnt);
}

void SWTrack::drawHits(QPainter *pnt)
{
  int zl  = zLayer[0]-zLayer[7];
  int px  = (wWid-zl)/2+LYR_PX+zLayer[0];
  int py  = (wHei-tHei-LYR_H)/2+tHei;
  int py1 = py+RES_H+LYR_D;
  int py2 = py-RES_H-LYR_D+LYR_H;
  int px1 = px-zLayer[0]-20;
  int px2 = px-zLayer[7]+20;

  int ifc = focusStatus/FOCUS_DOBJ/2-1;

  for (int i = 0; i < 8; i++) {
    pnt->setPen(QColor(100, 255, 200));
    pnt->drawLine(px-zLayer[i], py, px-zLayer[i], py+LYR_H);
    pnt->setPen((ifc == i) ? Qt::white : Qt::green);
    pnt->drawLine(px-zLayer[i]+1, py, px-zLayer[i]+1, py+LYR_H);
    pnt->setPen((ifc == i) ? Qt::gray : Qt::darkGreen);
    pnt->drawLine(px-zLayer[i]+2, py, px-zLayer[i]+2, py+LYR_H);

    pnt->setPen(Qt::green);
    drawText(pnt, px-zLayer[i]-3, py-17, Form("%d", i+1));
  }

  TrTrackR *trk = rEvent->pTrTrack(tID);
  double rscx = 0.001, rscy = 0.001;

  for (int i = 0; i < trk->GetNhits(); i++) {
    AMSPoint res = trk->GetResidual(trk->GetHit(i)->GetLayer()-1);
    if (abs(res.x()) > rscx) rscx = abs(res.x());
    if (abs(res.y()) > rscy) rscy = abs(res.y());
  }

  rscx = (rscx < 0.005) ? (int)(rscx*2000)*0.0005 : (int)(rscx*200)*0.005;
  rscy = (rscy < 0.005) ? (int)(rscy*2000)*0.0005 : (int)(rscy*200)*0.005;

  int bx1, bx2, by1, by2;
  int n1 = 0, n2 = 0;

  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    int ily = hit->GetLayer()-1;

    int x  = px-zLayer[ily];
    int y1 = py1-RES_H*trk->GetResidual(ily).x()/rscx;
    int y2 = py2-RES_H*trk->GetResidual(ily).y()/rscy;

    if (fitPat[ily] == FPAT_HITT) {
      pnt->setPen(Qt::white);
      pnt->drawRect(x-3, y1-3, 6, 6);
      pnt->setPen((ifc == ily) ? Qt::darkGray : Qt::darkRed);
      pnt->drawRect(x-2, y1-2, 6, 6);
      pnt->fillRect(x-2, y1-2, 6, 6, 
		    (ifc == ily) ? QColor(Qt::gray) : QColor(Qt::red));

      if (n1++ > 0) {
	pnt->setPen(Qt::red);
	pnt->drawLine(bx1, by1, x, y1);
	pnt->setPen(Qt::darkRed);
	if (abs(x-bx1) > abs(y1-by1))
	  pnt->drawLine(bx1, by1+1, x, y1+1);
	else
	  pnt->drawLine(bx1+1, by1, x+1, y1);
      }
      bx1 = x;
      by1 = y1;
    }

    pnt->setPen(Qt::white);
    pnt->drawRect(x-3, y2-3, 6, 6);
    pnt->setPen((ifc == ily) ? Qt::darkGray : Qt::darkRed);
    pnt->drawRect(x-2, y2-2, 6, 6);
    pnt->fillRect(x-2, y2-2, 6, 6, 
		  (ifc == ily) ? QColor(Qt::gray) : QColor(Qt::red));

    if (n2++ > 0) {
      pnt->setPen(Qt::red);
      pnt->drawLine(bx2, by2, x, y2);
      pnt->setPen(Qt::darkRed);
      if (abs(x-bx2) > abs(y2-by2))
	pnt->drawLine(bx2, by2+1, x, y2+1);
      else
	pnt->drawLine(bx2+1, by2, x+1, y2);
    }
    bx2 = x;
    by2 = y2;
  }

  pnt->save();
  pnt->rotate(-90);
  pnt->setPen(QColor(255, 50, 50));
  drawText(pnt, -py1-14, px1-47, "ResX");
  drawText(pnt, -py2-14, px1-47, "ResY");
  drawText(pnt, -py1-20, px1-30, (rscx < 0.005) ? Form("%.0fum", rscx*2e4)
	                                        : Form("%.1fmm", rscx*2e1));
  drawText(pnt, -py2-20, px1-30, (rscy < 0.005) ? Form("%.0fum", rscy*2e4)
	                                        : Form("%.1fmm", rscy*2e1));
  pnt->restore();

  pnt->setPen(QColor(255, 30, 30));
  pnt->drawLine(px1, py1, px2, py1);
  pnt->drawLine(px1, py2, px2, py2);

  pnt->drawLine(px1-7, py1-RES_H, px1-7, py1+RES_H);
  pnt->drawLine(px1-9, py1-RES_H, px1-5, py1-RES_H);
  pnt->drawLine(px1-9, py1+RES_H, px1-5, py1+RES_H);

  pnt->drawLine(px1-7, py2-RES_H, px1-7, py2+RES_H);
  pnt->drawLine(px1-9, py2-RES_H, px1-5, py2-RES_H);
  pnt->drawLine(px1-9, py2+RES_H, px1-5, py2+RES_H);
}
