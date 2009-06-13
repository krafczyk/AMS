// $Id: qwchist.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>

#include "qwchist.h"

#include "root.h"
#include "TrCluster.h"
#include "THLimitsFinder.h"

QWCHist::QWCHist(QWidget *parent) : QWidget(parent)
{
  wWid = wHei = 0;

  xBin = 0;
  cSig = cNoi = cSta = cShr = 0;
  clearHist();

  rEvent = 0;
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
  sMin = sMax = 0;
}

void QWCHist::drawCluster(int icls)
{
  clearHist();

  TrClusterR *cls = rEvent->pTrCluster(icls);
  if (!cls) {
    update();
    return;
  }

  int bofs = 6;
  nBin = cls->GetNelem()+bofs*2;

  xBin = new int  [nBin];
  cSig = new float[nBin];
  cNoi = new float[nBin];
  cSta = new float[nBin];
  cShr = new float[nBin];

  sMax = 0;

  for (int i = 0; i < nBin; i++) {
    xBin [i] = cls->GetAddress(0)+i-bofs;
    if (0 <= i-bofs && i-bofs < cls->GetNelem()) {
      cSig[i] = cls->GetSignal(i-bofs);
      cNoi[i] = cls->GetSigma (i-bofs);
      if (cSig[i] > sMax) sMax = cSig[i];
    }
    else {
      cSig[i] = cNoi[i] = 0;
      cSta[i] = 0;
    }
    cShr[i] = 0;
  }

  for (int i = 0; i < rEvent->NTrCluster(); i++) {
    if (i == icls) continue;
    TrClusterR *cc = rEvent->pTrCluster(i);
    if (cc->GetTkId() != cls->GetTkId() || 
	cc->GetAddress(0)+cc->GetNelem() < xBin[0] ||
	cc->GetAddress(0) > xBin[nBin-1]) continue;

    for (int j = 0; j < nBin; j++) {
      if (cShr[j] != 0) continue;
      int k = xBin[j]-cc->GetAddress(0);
      if (0 <= k && k < cc->GetNelem()) {
	cShr[j] = cc->GetSignal(k);
	if (cShr[j] > sMax) sMax = cShr[j];
      }
    }
  }

  float ssta = -sMax*0.1;
  for (int i = 0; i < nBin; i++) {
    if (0 <= i-bofs && i-bofs < cls->GetNelem())
      cSta[i] = (cls->GetStatus(i-bofs) != 0) ? ssta : 0;
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

  double hmax =  sMax*1.10;
  double hmin = -sMax*0.15;

  pnt.setPen((dStyle) ? Qt::white : Qt::black);
  pnt.drawRect(lmag, tmag, hwid, hhei);

  int nbx;
  double xmin, xmax, xstp;
  THLimitsFinder::Optimize(xBin[0], xBin[nBin-1], ndvx, xmin, xmax, nbx, xstp);

  // X-axis
  for (int i = 0; i < nBin; i++) {
    int px = lmag+hwid*(xBin[i]-xBin[0])/nBin;
    int py = tmag+hhei;
    pnt.drawLine(px, py, px, py-5);
  }
  for (int i = 0; i <= nbx; i++) {
    double xval = xmin+i*xstp;
    int px = lmag+hwid*(xval-xBin[0])/nBin;
    pnt.drawText(px-twid/2, tmag+hhei+lofs, twid, thei, 
		 Qt::AlignTop | Qt::AlignHCenter, Form("%.0f", xval));
  }
  pnt.drawText(lmag, tmag+hhei+lofs+thei, hwid, thei,
	       Qt::AlignTop | Qt::AlignRight, "Channel");

  int nby;
  double ymin, ymax, ystp;
  THLimitsFinder::Optimize(hmin, hmax, ndvy, ymin, ymax, nby, ystp);

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
    for (int j = 0; j < nBin; j++) {
      if (i == 2 && bcont == 0 && cont[j] == 0) {
	bcont = cont[j];
	continue;
      }
      int px1 = lmag+hwid* j   /nBin;
      int px2 = lmag+hwid*(j+1)/nBin;
      int py1 = tmag+hhei-hhei*(bcont  -hmin)/(hmax-hmin);
      int py2 = tmag+hhei-hhei*(cont[j]-hmin)/(hmax-hmin);
      pnt.setPen(col1);
      pnt.drawLine(px1, py1, px1, py2);
      pnt.drawLine(px1, py2, px2, py2);
      pnt.setPen(col2);
      pnt.drawLine(px1+1, py1+1, px1+1, py2+1);
      pnt.drawLine(px1+1, py2+1, px2+1, py2+1);
      bcont = cont[j];
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
