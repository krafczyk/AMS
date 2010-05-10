// $Id: glwidget.cpp,v 1.2 2010/05/10 21:55:47 shaino Exp $
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
}

GLWidget::~GLWidget()
{
  makeCurrent();
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

void GLWidget::cReset(int update)
{
  cFov = FOV_DEF;
  cDol = DOL_DEF;
  vRot = ROTV_DEF;
  hRot = ROTH_DEF;

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
  glView->setBCen(0, 0, val);
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
  if (event->modifiers() & Qt::ShiftModifier) {
    if (pickObject(event->x(), event->y()))
      updateGL();
    return;
  }
  else if (idSel >= 0) {
    idSel = -1;
    updateGL();
    return;
  }

  int dx =   event->x()-lastMpos.x();
  int dy = -(event->y()-lastMpos.y());

  if (event->buttons() & Qt::LeftButton) {
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
  else if (event->buttons() & Qt::RightButton) {
    glView->getCamera()->Truck(dx, dy, 0, 0);
    updateGL();
  }

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
  cFov -= event->delta()/100;
#else
  cFov -= event->delta()/10;
#endif
  if (cFov < FOV_MIN) cFov = FOV_MIN;
  if (cFov > FOV_MAX) cFov = FOV_MAX;

  updateGL();
  emit zChanged(cFov);
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

void GLWidget::drawObject(GLenum mode)
{
  static const GLfloat par[4*4] = { 0.8, 0.1, 0.0, 1.0,
				    0.0, 0.8, 0.2, 1.0,
				    0.2, 0.2, 1.0, 1.0,
				    0.8, 0.8, 0.8, 1.0 };

  double xc[3] = { -0.3, 0,   0   };
  double yc[3] = {  0,   0.3, 0   };
  double zc[3] = {  0,   0,   0.3 };

  for (int i = 0; i < 3; i++) {
    int id = i+1;

    if (mode == GL_RENDER) {
      if (idSel == id)
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &par[3*4]);
      else
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &par[i*4]);
    }
    else if (mode == GL_SELECT) glLoadName(id);

    GLRender::glBox(xc[i], yc[i], zc[i], 0.1, 0.1, 0.1);
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
