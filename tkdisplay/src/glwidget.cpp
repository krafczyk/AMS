// $Id: glwidget.cpp,v 1.6 2012/11/25 15:10:06 shaino Exp $
#include <QtGui>
#include <QtOpenGL>

#include "glwidget.h"
#include "glcamera.h"
#include "gllight.h"
#include "glviewer.h"
#include "glrender.h"

GLWidget::GLWidget(QWidget *parent)
  : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
  vpWidth  = 480;
  vpHeight = 480;

  glView = new GLViewer(0, 0, vpWidth, vpHeight);

  bFov = 20;
  sFov =  2;

  idSel = -1;

  setFocusPolicy(Qt::StrongFocus);
  setAutoFillBackground(false);

#ifdef Q_WS_MAC
  grabGesture(Qt::TapGesture,        0);  // 1
  grabGesture(Qt::TapAndHoldGesture, 0);  // 2
  grabGesture(Qt::PanGesture,        0);  // 3
  grabGesture(Qt::PinchGesture,      0);  // 4
  grabGesture(Qt::SwipeGesture,      0);  // 5
#endif
}

GLWidget::~GLWidget()
{
}

void GLWidget::cZoom(int val, int update)
{
  if (val != cFov) {
    cFov = val;
    if (update) updateGL();
  }
}

void GLWidget::cDolly(int val, int update)
{
  if (val != cDol) {
    int dd = val-cDol;
    glView->getCamera()->Dolly(dd, 0, 0);

    if (update) updateGL();
    cDol = val;
  }
}

void GLWidget::hRotate(int val, int update)
{
  if (val != hRot) {
    int dy = (val-hRot)*vpHeight/ROT_SCALE/180;
    glView->getCamera()->Rotate(0, dy, 0, 0);

    if (update) updateGL();
    hRot = val;
  }
}

void GLWidget::vRotate(int val, int update)
{
  if (val != vRot) {
    int dx = (val-vRot)*vpWidth/ROT_SCALE/360;
    glView->getCamera()->Rotate(dx, 0, 0, 0);

    if (update) updateGL();
    vRot = val;
  }
}

void GLWidget::cReset(int update, int dfov,  int ddol, 
		                  int drotv, int droth)
{
  cFov = dfov;
  cDol = ddol;
  vRot = drotv;
  hRot = droth;

  double cen[3] = { 0, 0, 0 };
  double fov = bFov*std::pow(sFov, (double)cFov/FOV_SCALE);
  glView->getCamera()->Reset();
  glView->getCamera()->Configure(fov, (double)cDol/DOL_SCALE, cen,
				      (double)hRot/ROT_SCALE*M_PI/180,
				      (double)vRot/ROT_SCALE*M_PI/180);
				    
  emit zChanged(cFov);
  emit dChanged(cDol);
  emit vRotated(vRot);
  emit hRotated(hRot);

  if (update) updateGL();
}

void GLWidget::setLSet(ELight light, int sw, int update)
{
  glView->getLight()->setLight(light, sw);
  if (update) updateGL();
}

void GLWidget::setLSize(int val, int update)
{
  glView->setBSize(val);
  if (update) updateGL();
}

void GLWidget::setLZpos(int val, int update)
{
  glView->setBCen(0, 0, val, 3);
//glView->setBCen(0, val, 0, 2);
//glView->setBCen(val, 0, 0, 1);
  if (update) updateGL();
}

void GLWidget::initializeGL()
{
  glView->initGL();
}

void GLWidget::paintGL()
{
  double cen[3] = { 0, 0, 0 };
  double fov = bFov*std::pow(sFov, (double)cFov/FOV_SCALE);

  glView->getCamera()->Configure(fov, 0, cen, 0, 0);

  glView->preDraw();
  glView->preRender();

  drawObject(GL_RENDER);

  glView->postRender();
  glView->postDraw();
}

void GLWidget::resizeGL(int width, int height)
{
  if (width > 0 && height > 0) {
    vpWidth = vpHeight = qMin(width, height);
    glView->setViewport((width-vpWidth)/2, (height-vpHeight)/2, 
			vpWidth, vpHeight);
    glViewport((width-vpWidth)/2, (height-vpHeight)/2, 
	       vpWidth, vpHeight);
  }
}

bool GLWidget::event(QEvent *event)
{
#ifdef Q_WS_MAC
  if (event->type() == QEvent::Gesture)
    gestureEvent(static_cast<QGestureEvent*>(event));
#endif
  return QGLWidget::event(event);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
  if ((event->modifiers() & Qt::ShiftModifier) && 
      (event->buttons()   & Qt::LeftButton)) {
    pickObject(event->x(), event->y());
    processPick();
  }

  lastMpos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
#ifdef Q_WS_MAC
  bool modifire = true;
#else
  bool modifire = (event->modifiers() & Qt::ShiftModifier);
#endif

  if (modifire) {
    if (pickObject(event->x(), event->y()))
      updateGL();
    return;
  }
  else if (idSel >= 0) {
    idSel = -1;
    updateGL();
    return;
  }

#ifndef Q_WS_MAC
  int dx =   event->x()-lastMpos.x();
  int dy = -(event->y()-lastMpos.y());
  if      (event->buttons() & Qt::LeftButton)  processRot  (dx, dy);
  else if (event->buttons() & Qt::RightButton) processTruck(dx, dy);
#endif

  lastMpos = event->pos();
}

void GLWidget::leaveEvent(QEvent *event)
{
  if (idSel >= 0) {
    idSel = -1;
    updateGL();
  }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
#ifdef Q_WS_MAC 
  int dx = 0, dy = 0;
  if (event->orientation() == Qt::Horizontal) dx =  event->delta()/10;
  if (event->orientation() == Qt::Vertical  ) dy = -event->delta()/10;
  processTruck(dx, dy);
#else
  processZoom(event->delta());
#endif
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
  int redraw = 0;

  switch (event->key()) {
  case Qt::Key_Up:
    redraw = glView->getCamera()->Truck(0,  10, 0, 0);
    break;

  case Qt::Key_Down:
    redraw = glView->getCamera()->Truck(0, -10, 0, 0);
    break;

  case Qt::Key_Left:
    redraw = glView->getCamera()->Truck(-10, 0, 0, 0);
    break;

  case Qt::Key_Right:
    redraw = glView->getCamera()->Truck( 10, 0, 0, 0);
    break;

  case Qt::Key_Home:
    cReset();
    break;
  };

  if (redraw) updateGL();
}

#ifdef Q_WS_MAC 
#include <QGestureEvent>

void GLWidget::gestureEvent(QGestureEvent *event)
{
  if (QGesture *gst = event->gesture(Qt::SwipeGesture)) {
    QSwipeGesture *swipe = static_cast<QSwipeGesture *>(gst);
    swipeGesture(swipe);
  }

  else if (QGesture *gst = event->gesture(Qt::TapAndHoldGesture)) {
    QTapAndHoldGesture *taph = static_cast<QTapAndHoldGesture *>(gst);
    QPoint gp((int)(taph->position().x()+0.5),
	      (int)(taph->position().y()+0.5));
    QPoint lp = mapFromGlobal(gp);
    pickObject(lp.x(), lp.y());
    processPick();
  }

  else if (QGesture *gst = event->gesture(Qt::PinchGesture)) {
    QPinchGesture *pinch = static_cast<QPinchGesture *>(gst);
    if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged) {
      if (pinch->lastScaleFactor() > 0) {
	float dsc = pinch->scaleFactor()/pinch->lastScaleFactor();
	int delta = (int)(20000*(dsc-1));
	processZoom(delta);
      }
    }
    if (pinch->changeFlags() & QPinchGesture::RotationAngleChanged) {
      float nrot = pinch->rotationAngle();
      float brot = pinch->lastRotationAngle();
      processProt(nrot-brot);
    }
  }

  else if (QGesture *gst = event->gesture(Qt::PanGesture)) {
    QPanGesture *pan = static_cast<QPanGesture *>(gst);
    processRot(pan->delta().x(), -pan->delta().y());
  }

/*
  else {
    QList<QGesture *> glst = event->activeGestures();
    for (int i = 0; i < glst.size(); i++) {
      QGesture *gst = glst.at(i);
      qDebug() << "Unknown gesture " << gst->gestureType();
    }
  }
*/
}
#endif

void GLWidget::drawObject(GLenum mode)
{
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  static const double par[4*4] = { 0.8, 0.1, 0.0, 0.6,
				   0.0, 0.8, 0.2, 0.6,
				   0.0, 0.1, 0.8, 0.6,
				   0.8, 0.8, 0.8, 1.0 };

  double xc[3] = { -0, 0, 0 };
  double yc[3] = {  0, 3, 0 };
  double zc[3] = {  0, 0, 3 };

  double scl = 20;

  for (int i = 0; i < 3; i++) {
    int id = i+1;

    if (mode == GL_RENDER) {
      if (idSel == id)
	GLRender::glMatCol(&par[3*4]);
      else
	GLRender::glMatCol(&par[i*4]);
    }
    else if (mode == GL_SELECT) glLoadName(id);

    GLRender::glBox(xc[i]*scl, yc[i]*scl, zc[i]*scl, 
		          scl,       scl,       scl);
  }
}

bool GLWidget::pickObject(int x, int y)
{
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);

  enum { BUFSIZE = 512 };
  GLuint sbuf[BUFSIZE];
  glSelectBuffer(BUFSIZE, sbuf);
  glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glView->pickObject(x, y-vp[1]*2, 5, 5);
  glMatrixMode(GL_PROJECTION);
  drawObject(GL_SELECT);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();

  int ids = idSel;
  processSel(glRenderMode(GL_RENDER), sbuf);

  return (ids != idSel);
}

void GLWidget::processSel(GLint nhit, GLuint *sbuf)
{
  double zmin = 0;
  idSel = -1;

  for (int i = 0, j = 0; i < nhit; i++) {
    int nsel = sbuf[j++];
    double z1 = (float)sbuf[j++]/0x7fffffff;
    double z2 = (float)sbuf[j++]/0x7fffffff;

    if (idSel < 0 || (z1+z2)/2 < zmin) {
      zmin  = (z1+z2)/2;
      idSel = sbuf[j];
    }
    j += nsel;
  }
}

void GLWidget::processZoom(int delta)
{
#ifdef Q_WS_MAC
  cFov -= delta/100;
#else
  cFov -= delta/10;
#endif
  if (cFov < FOV_MIN) cFov = FOV_MIN;
  if (cFov > FOV_MAX) cFov = FOV_MAX;

  updateGL();
  emit zChanged(cFov);
}

void GLWidget::processRot(int dx, int dy)
{
  if (dx == 0 && dy == 0) return;

  glView->getCamera()->Rotate(dx, dy, 0, 0);

  vRot += ROT_SCALE*360*dx/vpWidth;
  hRot += ROT_SCALE*180*dy/vpHeight;
  while (vRot < 0)             vRot += 360*ROT_SCALE;
  while (vRot > 360*ROT_SCALE) vRot -= 360*ROT_SCALE;
  if    (hRot < -90*ROT_SCALE) hRot  = -90*ROT_SCALE;
  if    (hRot >  90*ROT_SCALE) hRot  =  90*ROT_SCALE;

  updateGL();
  emit vRotated(vRot);
  emit hRotated(hRot);
}

void GLWidget::processTruck(int dx, int dy)
{
  if (dx == 0 && dy == 0) return;

  int redraw = glView->getCamera()->Truck(dx, dy, 0, 0);  
  if (redraw) updateGL();
}

void GLWidget::processProt(int angle)
{
  if (angle == 0) return;

  int asign  = (angle > 0) ? 1 : -1;
  int redraw = glView->getCamera()->RotCamBase(asign*angle*angle*M_PI/180);
  if (redraw) updateGL();
}
