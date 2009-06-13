// $Id: gltdisp.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// GLTDisp : a class to manage 3D Tracker Display on OpenGL by SH
//
#ifndef GLTDISP_H
#define GLTDISP_H

#include "glwidget.h"
#include "gllight.h"

#include <QTimer>
#include <QString>
#include <QPixmap>

class AMSEventR;
class SubWindow;
class SWEvent;
class TEWidget;

class GLTDisp : public GLWidget
{
  Q_OBJECT

public:
  GLTDisp(QWidget *parent = 0);
 ~GLTDisp();

  bool testOpt(int option) { return (drawOpt & option); }
  void addOpt (int option) { drawOpt |=  option; }
  void delOpt (int option) { drawOpt &= ~option; }
  void setOpt (int option, bool sw)
    { (sw) ? addOpt(option) : delOpt(option); updateGL(); }

  void drawOneEvent(AMSEventR *event);
  void setCurrEvent(AMSEventR *event) { currEvent = event; }

public Q_SLOTS:
  void updateGL() { update(); }
  void moreInfo();
  void closeAll();
  void closeSubWin();
  void setEvtTewType (const QString &);
  void setAllTewType (const QString &);
  void changeAllStyle(const QString &);
  void paintDark(bool sw);
  void paintLock(bool sw);

signals:
  void setEnabled(bool);
  void closeTew();
  void changeStyle(const QString &);
  void drawCluster(int);

protected:
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent (QMouseEvent *event);
  virtual void leaveEvent     (QEvent      *event);
  virtual void wheelEvent     (QWheelEvent *event);
  virtual void keyPressEvent  (QKeyEvent   *event);
  virtual void paintEvent     (QPaintEvent *event);

protected:
  void drawObject (GLenum mode);
  void drawTracker(GLenum mode);
  void drawTracks (GLenum mode);
  void processPick();

  void drawClusters();
  void drawHits();
  void drawAxes();

  TEWidget *openTEWidget(TEWidget *tew, int wtype = -1);

  void buildLines();
  void fillHitVec(int *vecx, int *vecy);

protected:
  int    drawOpt;
  int    nLadLine[8];
  double xLadLine[512];
  double yLadLine[512];
  double zLadLine[512];

  bool darkOpt;
  bool glLock;

  QPixmap glPixmap;

  int *ladVec;

  AMSEventR *currEvent;

  QString    styleSheet;
  int        wtEvt;
  int        wtObj;
  TEWidget  *twEvt;
  TEWidget  *twLad;
  TEWidget  *twCls;
  TEWidget  *twHit;
  TEWidget  *twTrk;

  SWEvent   *swEvent;
  SubWindow *subWin;
  QTimer     aTimer;

  enum { SID_OFFS_TRACK = 1001 };

  enum { LS_NONE, LS_AOPEN, LS_OPEN, LS_ACLOSE, 
	 LS_NSTEP = 10, LS_WAIT = 20 };
  int lsNstep;
  int lsWait;
  int lsStatus;
  int aStep;

public:
  enum { AN_NORMAL, AN_LIGHT, AN_NONE };
  void setAnimeMode(int mode = AN_NORMAL);

private slots:
  void animate();
};


namespace TkDisp {
  enum { 
    ALLTRK = 0x0001,    ///< Draw all the Tracker ladders
    TRKCLS = 0x0002,    ///< Draw Tracker clusters
    TRKHIT = 0x0004,    ///< Draw Tracker hits
    TRACK  = 0x0008,    ///< Draw Tracker tracks
    MCTRK  = 0x0010,    ///< Draw Tracker MC tracks
    MCCLS  = 0x0020,    ///< Draw Tracker MC clusters
    
    ANYTOF = 0x0100,    ///< Draw TOF or not
    ALLTOF = 0x0200,    ///< Draw all the TOF pads
    TOFHIT = 0x0400,    ///< Draw TOF hit position

    LFRT   = GLLight::kLightFront,
    LTOP   = GLLight::kLightTop,
    LBTM   = GLLight::kLightBottom,
    LLFT   = GLLight::kLightLeft,
    LRGT   = GLLight::kLightRight,
    LSPC   = GLLight::kLightSpecular
  };
};

using namespace TkDisp;

#endif
