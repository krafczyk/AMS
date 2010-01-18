// $Id: qwchist.h,v 1.2 2010/01/18 11:17:00 shaino Exp $
//
// QWCHist : a class to draw a cluster histogram on QWidget by SH
//
#ifndef QWCHIST_H
#define QWCHIST_H

#include "qwidget.h"

class TrCls;

class QWCHist : public QWidget
{
  Q_OBJECT

public:
  QWCHist(QWidget *parent = 0);
 ~QWCHist();

public Q_SLOTS:
  void fillCluster(TrCls *trcls, int icls);
  void darkStyle(bool ds) { dStyle  = ds; }
  void setNBinPnt(int nb) { nBinPnt = nb; update(); }
  int  getNBinPnt() const { return nBinPnt; }

protected:
  virtual void resizeEvent    (QResizeEvent *event);
  virtual void paintEvent     (QPaintEvent  *event);
  virtual void mousePressEvent(QMouseEvent  *event);
  virtual void mouseMoveEvent (QMouseEvent  *event);
  virtual void leaveEvent     (QEvent       *event);

  void clearHist();
  void initHist(int xmin, int xmax);

protected:
  bool dStyle;

  int wWid;
  int wHei;

  int binOffs;
  int nBinMin;
  int nBinPnt;

  int    nBin;
  int   *xBin;
  float *cSig;
  float *cNoi;
  float *cSta;
  float *cShr;

  float sMax;
};

#endif
