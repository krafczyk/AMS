// $Id: qwchist.cpp,v 1.3 2010/01/18 11:17:00 shaino Exp $
#include <QtGui>

#include "qwchist.h"
#include "trcls.h"

#include "TString.h"
#include "THLimitsFinder.h"

QWCHist::QWCHist(QWidget *parent) : QWidget(parent)
{
  wWid = wHei = 0;

  binOffs = 6;
  nBinMin = 1024;
  nBinPnt = 16;

  xBin = 0;
  cSig = cNoi = cSta = cShr = 0;
  clearHist();

  dStyle = true;
}

QWCHist::~QWCHist()
{
  clearHist();
}

void QWCHist::clearHist()
{
  delete [] xBin; xBin = 0;
  delete [] cSig; cSig = 0;
  delete [] cNoi; cNoi = 0;
  delete [] cSta; cSta = 0;
  delete [] cShr; cShr = 0;

  nBin = 0;
  sMax = 0;
}

void QWCHist::initHist(int xmin, int xmax)
{
  nBin = xmax-xmin;
  xBin = new int  [nBin];
  cSig = new float[nBin];
  cNoi = new float[nBin];
  cSta = new float[nBin];
  cShr = new float[nBin];

  for (int i = 0; i < nBin; i++) {
    xBin[i] = xmin+i;
    cSig[i] = cNoi[i] = cSta[i] = cShr[i] = 0;
  }
  sMax = 0;
}

void QWCHist::fillCluster(TrCls *trcls, int icls)
{
  clearHist();

  if (!trcls->pCluster(icls)) {
    update();
    return;
  }

  int nbin = binOffs*2+trcls->getNelem(icls);
  if (nbin < nBinMin) nbin = nBinMin;

  int bofs = (nbin-trcls->getNelem(icls))/2;
  int bmin = trcls->getAddress(icls, 0)-bofs;
  int bmax = bmin+nbin;

  if (bmin < 0) {
    bmin = 0;
    bmax = bmin+nbin;
  }
  if (bmax >= 1024) {
    bmax = 1023;
    bmin = bmax-nbin;
  }

  initHist(bmin, bmax);

  int tkid = trcls->getTkId(icls);

  for (int i = 0; i < trcls->nCluster(); i++) {
    if (trcls->getTkId(i) != tkid) continue;

    for (int j = 0; j < trcls->getNelem(i); j++) {
      int jj = trcls->getAddress(i, j)-xBin[0];
      if (0 <= jj && jj < nBin) {
	cShr[jj] = trcls->getSignal(i, j);
	cNoi[jj] = trcls->getSigma (i, j);
	cSta[jj] = trcls->getStatus(i, j);
	if (cShr[jj] > sMax) sMax = cShr[jj];
	if (i == icls) cSig[jj] = cShr[jj];
      }
    }
  }

  update();
}

void QWCHist::resizeEvent(QResizeEvent *event)
{
  wWid = event->size().width();
  wHei = event->size().height();
}

void QWCHist::paintEvent(QPaintEvent *)
{
  QPainter pnt(this);
  pnt.setRenderHint(QPainter::Antialiasing);  

  if (!dStyle)
    pnt.fillRect(0, 0, wWid, wHei, Qt::white);

  if (nBin <= 0 || !xBin || !cSig || !cNoi || !cSta || !cShr) return;

  int lmag = 55;
  int rmag = 15;
  int tmag = 15;
  int bmag = 50;
  int hwid = wWid-lmag-rmag;
  int hhei = wHei-tmag-bmag;
  int lofs = 5;
  int tofs = 30;

  int twid = 50;
  int thei = 20;

  int ndvx = 10;
  int ndvy = 10;

  pnt.setPen((dStyle) ? Qt::white : Qt::black);
  pnt.drawRect(lmag, tmag, hwid, hhei);

  int sb1 = 1024;
  int sb2 =    0;
  for (int i = 0; i < nBin; i++) {
    if (cSig[i] > 0) {
      if (i < sb1) sb1 = i;
      if (i > sb2) sb2 = i;
    }
  }

  int nbp = nBinPnt;
  if (sb2-sb1 > nbp) nbp = sb2-sb1;
  int ib1 = (sb1+sb2)/2-nBinPnt/2;
  int ib2 = (sb1+sb2)/2+nBinPnt/2+nBinPnt%2;
  int nbi = ib2-ib1;

  if (ib1 <     0) { ib1 = 0;      ib2 = ib1+nBinPnt-1; }
  if (ib2 >= nBin) { ib2 = nBin-1; ib1 = ib2-nBinPnt+1; }

  sMax = 0;
  for (int i = ib1; i <= ib2; i++)
    if (cShr[i] > sMax) sMax = cShr[i];

  double hmax =  sMax*1.10;
  double hmin = -sMax*0.15;
  if (hmax >=  100) tofs = 35;
  if (hmax >= 1000) tofs = 40;

  int nbx;
  double xmin, xmax, xstp;
  THLimitsFinder::Optimize(xBin[ib1], xBin[ib2], ndvx, xmin, xmax, nbx, xstp);
  if (xstp < 1) xstp = 1;

  // X-axis
  if (xstp < 10) {
    for (int i = ib1; i <= ib2; i++) {
      int px = lmag+hwid*(xBin[i]-xBin[ib1])/nbi;
      int py = tmag+hhei;
      pnt.drawLine(px, py, px, py-5);
    }
  }
  for (int i = 0; i <= nbx; i++) {
    double xval = xmin+i*xstp;
    int px = lmag+hwid*(xval-xBin[ib1])/nbi;
    int py = tmag+hhei;
    pnt.drawText(px-twid/2, tmag+hhei+lofs, twid, thei, 
		 Qt::AlignTop | Qt::AlignHCenter, Form("%.0f", xval));

    if (xstp >= 10) {
      pnt.drawLine(px, py, px, py-10);
      if (i < nbx) {
	px = lmag+hwid*(xval-xBin[ib1]+xstp*0.5)/nbi;
	pnt.drawLine(px, py, px, py-5);
      }
    }
  }
  pnt.drawText(lmag, tmag+hhei+lofs+thei, hwid, thei,
	       Qt::AlignTop | Qt::AlignRight, "Channel");

  int nby;
  double ymin, ymax, ystp;
  THLimitsFinder::Optimize(hmin, hmax, ndvy, ymin, ymax, nby, ystp);
  if (ystp < 1) ystp = 1;

  // Y-axis
  for (int i = 0; i <= nby; i++) {
    float yval = ymin+i*ystp;
    if (yval < 0) continue;

    int px = lmag;
    int py = tmag+hhei-hhei*(yval-hmin)/(hmax-hmin);
    pnt.drawLine(px, py, px+10, py);
    pnt.drawText(px-twid-lofs, py-thei/2, twid, thei, 
		 Qt::AlignRight | Qt::AlignVCenter, Form("%.0f", yval));

    if (i < nby) {
      py = tmag+hhei-hhei*(yval+ystp*0.5-hmin)/(hmax-hmin);
      pnt.drawLine(px, py, px+5, py);
    }
  }
  pnt.save();
  pnt.rotate(-90);
  pnt.drawText(-tmag-hhei, lmag-tofs-thei, hhei, thei, 
	       Qt::AlignRight | Qt::AlignBottom, "ADC count");
  pnt.restore();

  int py0 = tmag+hhei-hhei*(-hmin)/(hmax-hmin);
  pnt.setPen((dStyle) ? QColor(255, 255, 255, 100)
	              : QColor( 30,  30,  30, 255));
  pnt.drawLine(lmag, py0, lmag+hwid, py0);

  // Contents
  for (int i = 0; i < 4; i++) {
    float *cont;
    QColor col1, col2;
    if (i == 0) { 
      cont = cShr;
      col1 = (dStyle) ? QColor( 20, 100,  20, 230) 
	              : QColor(127, 255, 255, 255);
      col2 = (dStyle) ? QColor(  0,  50,   0, 230) 
	              : QColor( 63, 127, 127, 255);
    }
    else if (i == 1) { 
      cont = cSta; 
      col1 = (dStyle) ? QColor(255, 100, 100, 230) : QColor(200, 0, 0, 255);
      col2 = (dStyle) ? QColor(200,   0,   0, 230) : QColor(100, 0, 0, 255);
      
    }
    else if (i == 2) { 
      cont = cNoi; 
      col1 = (dStyle) ? QColor( 50, 100, 255, 230) : QColor(0, 0, 200, 255);
      col2 = (dStyle) ? QColor(  0,  50, 255, 230) : QColor(0, 0, 100, 255);
    }
    else if (i == 3) {
      cont = cSig;
      col1 = (dStyle) ? QColor(240, 255, 240, 230) : QColor(0, 150, 0, 255);
      col2 = (dStyle) ? QColor(  0, 255,   0, 230) : QColor(0,  75, 0, 255);
    }

    float bcont = cont[0];
    for (int j = ib1; j <= ib2; j++) {

      float ccont = cont[j];
      if (i == 1) ccont = (cont[j] != 0) ? -sMax*0.1 : 0;

      if (ccont < hmin) ccont = hmin;

      if (i == 2 && bcont == 0 && ccont == 0) {
	bcont = ccont;
	continue;
      }

      int px1 = lmag+hwid*(j-ib1  )/nbi;
      int px2 = lmag+hwid*(j-ib1+1)/nbi;
      int py1 = tmag+hhei-hhei*(bcont-hmin)/(hmax-hmin);
      int py2 = tmag+hhei-hhei*(ccont-hmin)/(hmax-hmin);
      pnt.setPen(col1);
      pnt.drawLine(px1, py1, px1, py2);
      pnt.drawLine(px1, py2, px2, py2);
      pnt.setPen(col2);
      pnt.drawLine(px1+1, py1+1, px1+1, py2+1);
      pnt.drawLine(px1+1, py2+1, px2+1, py2+1);
      bcont = ccont;
    }
  }
}

void QWCHist::mousePressEvent(QMouseEvent *event)
{
}

void QWCHist::mouseMoveEvent(QMouseEvent *event)
{
}

void QWCHist::leaveEvent(QEvent *event)
{
}
