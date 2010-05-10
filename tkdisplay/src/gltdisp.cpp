// $Id: gltdisp.cpp,v 1.6 2010/05/10 21:55:47 shaino Exp $
#include <QtGui>
#include <QtOpenGL>

#include "gltdisp.h"
#include "gvconst.h"
#include "gvdraw.h"
#include "gvgeom.h"

#include "tewidget.h"
#include "infotext.h"
#include "swevent.h"
#include "swtrack.h"
#include "swmctrack.h"
#include "swladder.h"

#include "TkDBc.h"
#include "TkCoo.h"
#include "TrCluster.h"
#include "root.h"
#include "amsdbc.h"

#include "TMath.h"

GLTDisp::GLTDisp(QWidget *parent) : GLWidget(parent)
{
  glDraw = new GVDraw;

  currEvent  = 0;
  darkOpt    = false;
  glLock     = false;
  updateLock = false;

  twEvt = twLad = twCls = twHit = twTrk = 0;

  swEvent  = new SWEvent;
  subWin   = 0;
  lsStatus = LS_NONE;

  aTimer.setSingleShot(false);
  connect(&aTimer, SIGNAL(timeout()), this, SLOT(animate()));
  aTimer.start(LS_WAIT);

  setAnimeMode();
}

GLTDisp::~GLTDisp()
{
}

void GLTDisp::setOpt(int option, bool sw)
{ 
  glDraw->setOpt(option, sw);
  if (!updateLock) updateGL(); 
}

void GLTDisp::setGeom(int type, bool sw)
{ 
  if (type < 0) return;

  GVGeom::setEnabledAll(type, sw);
  if (!updateLock) updateGL(); 
}

void GLTDisp::drawOneEvent(AMSEventR *event)
{
  currEvent = event;
  glDraw->setCurrEvent(event);
  if (currEvent) {
    updateGL();
    if (twEvt) twEvt->setText(InfoText::EventInfo(currEvent));
    if (twLad) twLad->clearText();
    if (twCls) twCls->clearText();
    if (twHit) twHit->clearText();
    if (twTrk) twTrk->clearText();
  }
}

void GLTDisp::moreInfo()
{
  if (twEvt && twEvt->isVisible()) 
    twEvt->close();
  else {
    twEvt = openTEWidget(twEvt, wtEvt);
    twEvt->setText(InfoText::EventInfo(currEvent));
    twEvt->show();
  }
}

void GLTDisp::closeAll()
{
  if (lsNstep > 0) lsStatus = LS_ACLOSE;
  else closeSubWin();
  emit closeTew();
}

void GLTDisp::closeSubWin()
{
  lsStatus = LS_NONE;
  aStep    = 0;

  if (subWin) {
    delete subWin;
    subWin = 0;
    emit setEnabled(true);
    update();
  }
}

void GLTDisp::setEvtTewType(const QString &type)
{
  wtEvt = Qt::Drawer;
  if (type == "Drawer" ) wtEvt = Qt::Drawer;
  if (type == "Sheet"  ) wtEvt = Qt::Sheet;
  if (type == "Popup"  ) wtEvt = Qt::Popup;
  if (type == "ToolTip") wtEvt = Qt::ToolTip;
  if (type == "Tool"   ) wtEvt = Qt::Tool;
  if (type == "Window" ) wtEvt = Qt::Window;

  if (twEvt) {
    int vis = twEvt->isVisible();
    twEvt->setWindowFlags((Qt::WindowFlags(wtEvt)));
    if (vis) {
      openTEWidget(twEvt);
      twEvt->show();
    }
  }
}

void GLTDisp::setAllTewType(const QString &type)
{
  wtObj = Qt::Drawer;
  if (type == "Drawer" ) wtObj = Qt::Drawer;
  if (type == "Sheet"  ) wtObj = Qt::Sheet;
  if (type == "Popup"  ) wtObj = Qt::Popup;
  if (type == "ToolTip") wtObj = Qt::ToolTip;
  if (type == "Tool"   ) wtObj = Qt::Tool;
  if (type == "Window" ) wtObj = Qt::Window;

  if (twLad) twLad->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twCls) twCls->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twHit) twHit->setWindowFlags((Qt::WindowFlags(wtObj)));
  if (twTrk) twTrk->setWindowFlags((Qt::WindowFlags(wtObj)));
}

void GLTDisp::changeAllStyle(const QString &sty)
{
  styleSheet = sty;
  emit changeStyle(styleSheet);
}

void GLTDisp::paintDark(bool sw)
{
  darkOpt = sw;
  update();
}

void GLTDisp::paintLock(bool sw)
{
  if (!glLock && sw) {
    QImage image = grabFrameBuffer();
    glPixmap = QPixmap::fromImage(image); 
    glLock = true;
  }
  if (glLock && !sw) glLock = false;
}

void GLTDisp::mousePressEvent(QMouseEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->isFocus(SubWindow::FOCUS_MORE)) {
      moreInfo();
      return;
    }
    GLWidget::mousePressEvent(event);
    return;
  }

  if (lsStatus != LS_OPEN || !subWin) return;

  subWin->checkFocus(event->x(), event->y());
  if (subWin->isFocus(SubWindow::FOCUS_CLOSE)) {
    closeAll();
    return;
  }

  if (subWin->isFocus(SubWindow::FOCUS_MORE)) {
    if (subWin->getType() == SubWindow::SW_TRACK) {
      if (twTrk && twTrk->isVisible()) twTrk->close();
      else {
	twTrk = openTEWidget(twTrk);
	twTrk->setText(InfoText::TrackInfo(currEvent, subWin->getID()));
	twTrk->show();
      }
    }
    else if (subWin->getType() == SubWindow::SW_LADDER) {
      if (twLad && twLad->isVisible()) twLad->close();
      else {
	twLad = openTEWidget(twLad);
	twLad->setText(InfoText::LadderInfo(currEvent, subWin->getID()));
	twLad->show();
      }
    }
    return;
  }

  if (subWin->isFocus(SubWindow::FOCUS_OPT)) {
    subWin->procMpress();
    update();
    return;
  }

  if (!subWin->isFocus(SubWindow::FOCUS_DOBJ)) return;

  if (subWin->getType() == SubWindow::SW_LADDER) {
    if (subWin->getDobjType() == SubWindow::DOBJ_CLS)
      emit drawCluster(subWin->getDobjID());

    else if (subWin->getDobjType() >= SubWindow::DOBJ_TRK) {
      int tid = subWin->getDobjType()/SubWindow::DOBJ_TRK-1;
      TrTrackR *trk = currEvent->pTrTrack(tid);	  
      if (trk) {
	delete subWin;
	subWin = new SWTrack(currEvent, tid);
	aStep  = 0;

	if (lsNstep > 0) lsStatus = LS_AOPEN;
	else {
	  lsStatus = LS_OPEN;
	  update();
	}
      }
    }
  }
  else if (subWin->getType() == SubWindow::SW_TRACK) {
    int tkid = 0;
    if (subWin->getDobjType() == SubWindow::DOBJ_HIT) {
      int hid = ((SWTrack *)subWin)->getFocusHid();
      TrTrackR  *trk = currEvent->pTrTrack(subWin->getID());
      TrRecHitR *hit = trk->GetHit(hid);
      if (hit) tkid = hit->GetTkId();
    }
    else if (subWin->getDobjType() == SubWindow::DOBJ_LAD) 
      tkid = subWin->getDobjID();

    if (tkid != 0) {
      delete subWin;
      subWin = new SWLadder(currEvent, tkid);
      aStep  = 0;

      if (lsNstep > 0) lsStatus = LS_AOPEN;
      else {
	lsStatus = LS_OPEN;
	update();
      }
    }
  }
}

void GLTDisp::mouseMoveEvent(QMouseEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->checkFocus(event->x(), event->y())) {
      update();
      return;
    }

    GLWidget::mouseMoveEvent(event);
    return;
  }

  if (subWin && subWin->checkFocus(event->x(), event->y())) update();
}

void GLTDisp::leaveEvent(QEvent *event)
{
  if (lsStatus == LS_NONE) {
    if (swEvent->clearFocus()) update();
    GLWidget::leaveEvent(event);
    return;
  }

  if (lsStatus == LS_OPEN && subWin && subWin->clearFocus()) update();
}

void GLTDisp::wheelEvent(QWheelEvent *event)
{
  if (lsStatus == LS_NONE) {
    GLWidget::wheelEvent(event);
    return;
  }
}

void GLTDisp::keyPressEvent(QKeyEvent *event)
{
  if (lsStatus == LS_NONE) {
    GLWidget::keyPressEvent(event);
    return;
  }

  if (lsStatus == LS_OPEN && event->key() == Qt::Key_Escape)
    closeAll();
}

void GLTDisp::paintEvent(QPaintEvent *)
{
  makeCurrent();

  if (!glLock)
    paintGL();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  if (glLock)
    painter.drawPixmap(0, 0, glPixmap);
  else {
    swEvent->setEvent(currEvent);
    swEvent->drawWindow(&painter, vpWidth, vpHeight);
  }

  if (subWin)
    subWin->drawWindow(&painter, vpWidth, vpHeight, 
		       (lsNstep > 0) ? (float)aStep/lsNstep : 1);

  if (darkOpt)
    painter.fillRect(0, 0, vpWidth, vpHeight, QColor(0, 0, 0, 150));

  painter.end();
}

void GLTDisp::drawObject(GLenum mode)
{
  glDraw->drawObject((mode == GL_RENDER), (mode == GL_SELECT), idSel);
}

void GLTDisp::processPick()
{
  if (lsStatus != LS_NONE) return;

  if (glIDToType(idSel) == Geom_NONE) {
    int sid = glIDToSelID(idSel);
    int tid = sid-SID_OFFS_TRACK;
    if (tid < 0) return;

    subWin = new SWMCTrack(currEvent, tid);
  }

  if (glIDToType(idSel) == Geom_TRK) {
    int sid = glIDToSelID(idSel);

    if (sid < SID_OFFS_TRACK) {
      TkLadder *lad = glDraw->getLadder(sid);
      if (!lad) return;

      subWin = new SWLadder(currEvent, lad->GetTkId());
    }
    else if (currEvent) {
      int tid = sid-SID_OFFS_TRACK;
      if (!currEvent->pTrTrack(tid)) return;

      subWin = new SWTrack(currEvent, tid);
    }
  }

  if (subWin) {
    emit setEnabled(false);
    aStep = 0;

    if (lsNstep > 0) lsStatus = LS_AOPEN;
    else {
      lsStatus = LS_OPEN;
      update();
    }
  }
}

TEWidget *GLTDisp::openTEWidget(TEWidget *tew, int wtype)
{
  if (wtype < 0) wtype = wtObj;

  if (!tew) {
    tew = new TEWidget(this);
    if (wtype != Qt::Drawer) {
      tew->setMinimumWidth (480);
      tew->setMinimumHeight(480);
    }
    tew->setStyleSheet(styleSheet);
    tew->setWindowFlags((Qt::WindowFlags(wtype)));

    connect(this, SIGNAL(closeTew()), tew, SLOT(close()));
    connect(this, SIGNAL(changeStyle(QString)), 
	       tew, SLOT(changeStyle(QString)));
  }

  QPoint pos = tew->pos();
  if (pos.x() < 0) pos.setX(0);
  if (pos.y() < 0) pos.setY(0);
  tew->move(pos);

  return tew;
}

void GLTDisp::setAnimeMode(int mode)
{
  if (mode == AN_NORMAL) {
    lsNstep = LS_NSTEP;
    lsWait  = LS_WAIT;
  }
  else if (mode == AN_LIGHT) {
    lsNstep = 3;
    lsWait  = 50;
  }
  else {
    lsNstep = 0;
    lsWait  = 1000;
  }

  aTimer.setInterval(lsWait);
}

void GLTDisp::animate()
{
  if (lsStatus != LS_AOPEN && lsStatus != LS_ACLOSE) return;

  if (lsStatus == LS_AOPEN  && ++aStep >= lsNstep) lsStatus = LS_OPEN;

  if (lsStatus == LS_ACLOSE && --aStep <= 0) closeSubWin();

  update();
}
