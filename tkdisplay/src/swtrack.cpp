// $Id: swtrack.cpp,v 1.5 2012/11/25 15:10:06 shaino Exp $
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

int SWTrack::zLayer1[NLAY] 
= { 180, 100,  86,    6,   -8,  -86, -100, -180,    0 };

int SWTrack::zLayer2[NLAY*4] 
= { 180,  75,  58,  -46,  -63, -164, -180,    0,    0, 
    180, 136, 127,   84,   75,   32,   24,    0, -180,
    -28, -71, -80, -122, -131, -172, -180,  180,    0,
     44,  17,   9,  -16,  -24,  -48,  -57,  180, -180 };

int SWTrack::fID     = -1;
int SWTrack::lyrConf = 3;

SWTrack::SWTrack(AMSEventR *event, int tid) 
  : SubWindow(event, SW_TRACK, WIN_W, WIN_H, TBAR_H), tID(tid)
{
  updateEvent(event);
}

SWTrack::~SWTrack()
{
}

int SWTrack::getDobjID() const
{
  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if ((focusStatus & FOCUS_DOBJ) && ifc >= 0)
    return (abs(lyrSta[ifc]) < 10) ? ifc : lyrSta[ifc];

  return -1;
}

int SWTrack::getDobjType() const
{
  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if ((focusStatus & FOCUS_DOBJ) && ifc >= 0)
    return (abs(lyrSta[ifc]) < 10) ? DOBJ_HIT : DOBJ_LAD;

  return -1;
}

int SWTrack::getFocusHid()
{
  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if (!(focusStatus & FOCUS_DOBJ) || ifc < 0) return -1;

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
  if (!TkDBc::Head) return false;

  int fold = focusStatus;
  focusStatus = 0;

  if (checkButtons(x, y)) return true;

  x -= pX;
  y -= pY;

  int *zly = (TkDBc::Head->GetSetup() == 3) ? &zLayer2[NLAY*lyrConf] : zLayer1;

  int zl1 = -200, zl2 = 200;
  for (int i = 0; i < NLAY; i++) {
    if (zly[i] > zl1) zl1 = zly[i];
    if (zly[i] < zl2) zl2 = zly[i];
  }

  int zl   = zl1-zl2;
  int px   = (wWid-zl)/2+LYR_PX+zl1;
  int py   = (wHei-tHei-LYR_H)/2+tHei+LYR_PY;
  int drng = 5;

  for (int i = 0; i < NLAY; i++) {
    if (px-zly[i]-drng <= x && x <= px-zly[i]+drng &&
	py-drng <= y && y <= py+LYR_H+drng) {
      focusStatus |= FOCUS_DOBJ;
      focusStatus |= FOCUS_DOBJ*(i+1)*2;
    }
  }
  
  return (fold != focusStatus);
}

void SWTrack::drawInfobar(QPainter *pnt)
{
  pnt->fillRect(QRect(0, 0, wWid-2, tHei), QColor(0, 0, 0, 200));
  drawButtons(pnt);

  TrTrackR *trk = rEvent->pTrTrack(tID);
  if (!trk) return;

  int mfit = (0 <= fID && fID < NPAR) ? fitPar[fID] : 0;
  if (mfit <= 0) return;

  TString hpat;
  for (int i = 0; i < NLAY; i++) {
    if      (lyrSta[i] == LSTA_HITY) hpat += "Y";
    else if (lyrSta[i] == LSTA_HITT) hpat += "O";
    else                             hpat += "_";
  }

  pnt->setPen(Qt::white);
  drawText(pnt,  28, 10, Form("Track [%d]", tID));
  drawText(pnt, 100, 10, Form("nHit: %d",    trk->GetNhits()));
  drawText(pnt, 170, 10, Form("nHitXY: %d",  trk->GetNhitsXY()));
  drawText(pnt, 270, 10, Form("fPat: %s",  hpat.Data()));
  drawText(pnt,  28, 30, Form("Fit %d", fitPar[fID]));
  drawText(pnt, 100, 30, Form("chi2/Ndf (X): %.2f/%d", 
			      trk->GetChisqX(mfit), trk->GetNdofX(mfit)));
  drawText(pnt, 270, 30, Form("chi2/Ndf (Y): %.2f/%d", 
			      trk->GetChisqY(mfit), trk->GetNdofY(mfit)));
  drawText(pnt, 100, 50, Form("rigidity: %.2f", trk->GetRigidity(mfit)));
  drawText(pnt, 270, 50, Form("errRinv: %.2f",  trk->GetErrRinv(mfit)));

  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if ((focusStatus & FOCUS_DOBJ) && ifc >= 0) {
    int iht = getFocusHid();
    TrRecHitR *hit = trk->GetHit(iht);

    if (hit) {
      char cf1 = (hit->OnlyY()) ? 'G' : '_';
      char cf2 = (hit->checkstatus(AMSDBc::USED)) ? 'T' : '_';

      int lay = hit->GetLayer();
      drawText(pnt,  40, 70, Form("Hit [%d]", iht));
      drawText(pnt,  90, 70, Form("TkID: %d", hit->GetTkId()));
      drawText(pnt, 180, 70, Form("Flag: %c%c", cf1, cf2));
      drawText(pnt, 250, 70, Form("Residual: %.4f %.4f", 
				  trk->GetResidualO(lay, mfit).x(),
				  trk->GetResidualO(lay, mfit).y()));
    }
    else if (abs(lyrSta[ifc]) > 10) {
      int tkid = lyrSta[ifc];

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

void SWTrack::procMpress()
{
  if (focusStatus & FOCUS_OPT) {
    int ifc = focusStatus/FOCUS_OPT/2-1;
    if (0 <= ifc && ifc < NPAR && fitPar[ifc] > 0) fID = ifc;

    if (ifc == NPAR) {
           if (lyrConf == 0) lyrConf = 2;
      else if (lyrConf == 1) lyrConf = 3;
      else if (lyrConf == 2) lyrConf = 0;
      else if (lyrConf == 3) lyrConf = 1;
    }
    if (ifc == NPAR+1) {
           if (lyrConf == 0) lyrConf = 1;
      else if (lyrConf == 1) lyrConf = 0;
      else if (lyrConf == 2) lyrConf = 3;
      else if (lyrConf == 3) lyrConf = 2;
    }
  }
}


void SWTrack::updateEvent(AMSEventR *event)
{
  rEvent = event;

  int mf0 = TrTrackR::DefaultFitID;

  int mfit[NPAR] = { mf0, mf0 | TrTrackR::kFitLayer8,
		          mf0 | TrTrackR::kFitLayer9,
		          mf0 | TrTrackR::kFitLayer8 | TrTrackR::kFitLayer9 };

  for (int i = 0; i < NLAY; i++) lyrSta[i] = 0;
  for (int i = 0; i < NPAR; i++) fitPar[i] = 0;

  TrTrackR *trk;
  for (int t = tID; t >= 0; t--) {
    trk = rEvent->pTrTrack(t);
    if (trk) {
      tID = t;
      break;
    }
  }
  if (!trk) return;

  for (int i = 0; i < NPAR; i++)
    if (trk->ParExists(mfit[i])) fitPar[i] = mfit[i];

  if (fID < 0 || !trk->ParExists(mfit[fID])) {
    for (int i = 0; i < NPAR; i++)
      if (trk->ParExists(mfit[i])) {
	fID = i;
	break;
      }
  }
  if (fID < 0) return;

  for (int i = 0; i < trk->GetNhits(); i++) {
    TrRecHitR *hit = trk->GetHit(i);
    int ily = hit->GetLayer()-1;
    if (hit->OnlyY()) lyrSta[ily] = LSTA_HITY;
    else lyrSta[ily] = LSTA_HITT;
  }

  for (int i = 0; i < NLAY; i++) {
    if (lyrSta[i] == 0) {
      AMSPoint pnt;
      AMSDir dir;
      trk->Interpolate(TkDBc::Head->GetZlayer(i+1), pnt, dir);
      TkSens sens(pnt, 0);
      lyrSta[i] = sens.GetLadTkID();
    }
  }
}

void SWTrack::drawObjects(QPainter *pnt)
{
  drawHits(pnt);
}

bool SWTrack::checkButtons(int x, int y)
{
  int fold = focusStatus;
  checkClose(x, y);
  checkMore (x, y);
  if (fold != focusStatus) return true;

  x -= pX;
  y -= pY;

  for (int i = 0; i < NPAR; i++) {
    if (fitPar[i] && i != fID && (drawStatus & (FOCUS_OPT*(1 << i)))) {
      int mx = BFP_X+BFP_W*i;
      int my = tHei-MORE_H-MORE_Y;
      if (mx <= x && x <  mx+BFP_W && 
	  my <= y && y <= my+MORE_H) {
	focusStatus |= FOCUS_OPT;
	focusStatus |= FOCUS_OPT*(i+1)*2;
      }
    }
  }

  for (int i = 0; i < 2; i++) {
    int mx = BLY_X+BLY_W*i;
    int my = tHei-MORE_H-MORE_Y;
    if (mx <= x && x <  mx+BLY_W && my <= y && y <= my+MORE_H) {
      focusStatus |= FOCUS_OPT;
      focusStatus |= FOCUS_OPT*(i+NPAR+1)*2;
    }
  }
  
  return (fold != focusStatus);
}

void SWTrack::drawButtons(QPainter *pnt)
{
  drawClose(pnt);
  drawMore (pnt);

  TrTrackR *trk = rEvent->pTrTrack(tID);
  if (!trk) return;

  QString str[NPAR] = { "L1-7", "+L8", "+L9", "+L89" };
  int     bdw[NPAR] = { 5, 11, 11, 2 };

  int ifc = focusStatus/FOCUS_OPT/2-1;
  for (int i = 0; i < NPAR; i++) {
    int x = BFP_X+BFP_W*i;
    int y = tHei-MORE_H-MORE_Y;

    if (fitPar[i]) pnt->setPen(QColor(255,  50,  50, 200));
    else           pnt->setPen(QColor(150, 100, 100, 200));

    if (fitPar[i] && (focusStatus & FOCUS_OPT) && ifc == i && i != fID) {
      pnt->fillRect(x-3, y+1, BFP_W-bdw[i], MORE_H, QColor(255, 255, 255, 50));
      pnt->setPen(Qt::red);
    }
    drawText(pnt, x+15, y+1, str[i].toAscii().data());
    pnt->drawRect(x, y+3, 10, MORE_H-4);

    if (i == fID) pnt->fillRect(x+2, y+5, 6, MORE_H-8, Qt::red);

    drawStatus |= FOCUS_OPT*(1 << i);
  }

  for (int i = 0; i < 2; i++) {
    int x = BLY_X+BLY_W*i;
    int y = tHei-MORE_H-MORE_Y;

    pnt->setPen(QColor(255, 50, 50, 200));
    if ((focusStatus & FOCUS_OPT) && ifc == i+NPAR) {
      pnt->setPen(Qt::red);
      pnt->drawRect(x, y+1, BLY_W, MORE_H);
      pnt->fillRect(x, y+1, BLY_W, MORE_H, QColor(255, 255, 255, 50));
    }

    if (i == 0) {
      if (lyrConf == 0 || lyrConf == 1) drawText(pnt, x+2, y+1, "<");
      if (lyrConf == 2 || lyrConf == 3) drawText(pnt, x+2, y+1, ">");
    }
    if (i == 1) {
      if (lyrConf == 0 || lyrConf == 2) drawText(pnt, x+2, y+1, ">");
      if (lyrConf == 1 || lyrConf == 3) drawText(pnt, x+2, y+1, "<");
    }
  }
}

void SWTrack::drawHits(QPainter *pnt)
{
  int *zly = (TkDBc::Head->GetSetup() == 3) ? &zLayer2[NLAY*lyrConf] : zLayer1;

  int zl1 = -200, zl2 = 200;
  for (int i = 0; i < NLAY; i++) {
    if (zly[i] > zl1) zl1 = zly[i];
    if (zly[i] < zl2) zl2 = zly[i];
  }

  int zl  = zl1-zl2;
  int px  = (wWid-zl)/2+LYR_PX+zl1;
  int py  = (wHei-tHei-LYR_H)/2+tHei+LYR_PY;
  int py1 = py+RES_H+LYR_D;
  int py2 = py-RES_H-LYR_D+LYR_H;
  int px1 = px-zl1-20;
  int px2 = px-zl2+20;

  int ifc = focusStatus/FOCUS_DOBJ/2-1;
  if (!(focusStatus & FOCUS_DOBJ)) ifc = -1;

  for (int i = 0; i < NLAY; i++) {
    if (zly[i] == 0) continue;
    pnt->setPen(QColor(100, 255, 200));
    pnt->drawLine(px-zly[i], py, px-zly[i], py+LYR_H);
    pnt->setPen((ifc == i) ? Qt::white : Qt::green);
    pnt->drawLine(px-zly[i]+1, py, px-zly[i]+1, py+LYR_H);
    pnt->setPen((ifc == i) ? Qt::gray : Qt::darkGreen);
    pnt->drawLine(px-zly[i]+2, py, px-zly[i]+2, py+LYR_H);

    pnt->setPen(Qt::green);
    drawText(pnt, px-zly[i]-3, py-17, Form("%d", i+1));
  }

  TrTrackR *trk = rEvent->pTrTrack(tID);
  if (!trk) return;

  int mfit = (0 <= fID && fID < NPAR) ? fitPar[fID] : 0;
  if (mfit <= 0) return;

  double rscx = 0.001, rscy = 0.001;

  int nhit = trk->GetNhits();

  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = trk->GetHit(i);
    if (!hit) continue;

    int ily = hit->GetLayer()-1;
    if (zly[ily] == 0) continue;

    AMSPoint res = trk->GetResidualO(hit->GetLayer(), mfit);
    if (!hit->OnlyY() && abs(res.x()) > rscx) rscx = abs(res.x());
    if (!hit->OnlyX() && abs(res.y()) > rscy) rscy = abs(res.y());
  }

  rscx = (rscx < 0.005) ? (int)(rscx*2000)*0.0005 
       : (rscx < 1.000) ? (int)(rscx*200) *0.005
                        : (int)(rscx*20)  *0.05;
  rscy = (rscy < 0.005) ? (int)(rscy*2000)*0.0005 
       : (rscy < 1.000) ? (int)(rscy*200) *0.005
                        : (int)(rscy*20)  *0.05;

  int bx1, bx2, by1, by2;
  int n1 = 0, n2 = 0;

  int    idx[NLAY];
  double zht[NLAY];
  for (int i = 0; i < nhit; i++)
    zht[i] = trk->GetHit(i)->GetCoord().z();

  TMath::Sort(nhit, zht, idx);

  for (int i = 0; i < nhit; i++) {
    TrRecHitR *hit = trk->GetHit(idx[i]);
    int lay = hit->GetLayer();
    int ily = lay-1;
    if (zly[ily] == 0) continue;

    int x  = px-zly[ily];
    int y1 = py1-RES_H*trk->GetResidualO(lay, mfit).x()/rscx;
    int y2 = py2-RES_H*trk->GetResidualO(lay, mfit).y()/rscy;

    if (lyrSta[ily] == LSTA_HITT) {
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
	                       : (rscx < 1.000) ? Form("%.1fmm", rscx*2e1)
	                                        : Form("%.1fcm", rscx*2));
  drawText(pnt, -py2-20, px1-30, (rscy < 0.005) ? Form("%.0fum", rscy*2e4)
	                       : (rscy < 1.000) ? Form("%.1fmm", rscy*2e1)
	                                        : Form("%.1fcm", rscy*2));
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
