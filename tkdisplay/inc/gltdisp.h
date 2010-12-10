// $Id: gltdisp.h,v 1.5 2010/12/10 21:38:01 shaino Exp $
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
class GVDraw;
class SubWindow;
class SWEvent;
class TEWidget;

class GLTDisp : public GLWidget {

  Q_OBJECT

public:
  GLTDisp(QWidget *parent = 0);
 ~GLTDisp();

  void setOpt (int option, bool sw);
  void setGeom(int type,   bool sw);
  void setUpdateLock(bool sw = true) { updateLock = sw; }

  void drawOneEvent(AMSEventR *event);

  GVDraw *getDraw() { return glDraw; }

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
  void swipeEvent(int);

protected:
  virtual void mousePressEvent(QMouseEvent   *event);
  virtual void mouseMoveEvent (QMouseEvent   *event);
  virtual void leaveEvent     (QEvent        *event);
  virtual void wheelEvent     (QWheelEvent   *event);
  virtual void keyPressEvent  (QKeyEvent     *event);
  virtual void paintEvent     (QPaintEvent   *event);

#ifdef Q_WS_MAC
  virtual void swipeGesture(QSwipeGesture *swipe);
#endif

protected:
  virtual void drawObject (GLenum mode);
  virtual void processPick();

protected:
  virtual void resetButton(QPainter *pnt, int x = -1, int y = -1);

  TEWidget *openTEWidget(TEWidget *tew, int wtype = -1);

protected:
  GVDraw *glDraw;

  bool darkOpt;
  bool glLock;
  bool updateLock;

  QPixmap glPixmap;

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

  enum { SID_OFFS_TRACK = 501 };

  enum { LS_NONE, LS_AOPEN, LS_OPEN, LS_ACLOSE, 
	 LS_NSTEP = 10, LS_WAIT = 20 };
  int lsNstep;
  int lsWait;
  int lsStatus;
  int aStep;

  bool focusReset;

public:
  enum { AN_NORMAL, AN_LIGHT, AN_NONE };
  void setAnimeMode(int mode = AN_NORMAL);
  void initAMSgeom(const char *fname = "geom/geom.root",
		   const char *key   = "ams02",
		   const char *nname = "FMOT1");

private slots:
  void animate();
};

#endif
