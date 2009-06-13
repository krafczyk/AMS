// $Id: qwchist.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// QWCHist : a class to draw a cluster histogram on QWidget by SH
//
#ifndef QWCHIST_H
#define QWCHIST_H

#include "qwidget.h"

class AMSEventR;

class QWCHist : public QWidget
{
  Q_OBJECT

public:
  QWCHist(QWidget *parent = 0);
 ~QWCHist();

public Q_SLOTS:
  void newEvent(AMSEventR *evt) { rEvent = evt; }
  void drawCluster(int icls);
  void darkStyle(bool ds) { dStyle = ds; }

protected:
  virtual void resizeEvent    (QResizeEvent *event);
  virtual void paintEvent     (QPaintEvent  *event);
  virtual void mousePressEvent(QMouseEvent  *event);
  virtual void mouseMoveEvent (QMouseEvent  *event);
  virtual void leaveEvent     (QEvent       *event);

  void clearHist();

protected:
  AMSEventR *rEvent;

  bool dStyle;

  int wWid;
  int wHei;

  int    nBin;
  int   *xBin;
  float *cSig;
  float *cNoi;
  float *cSta;
  float *cShr;

  float sMin;
  float sMax;
};

#endif
