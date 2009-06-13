// $Id: glwidget.cpp,v 1.1 2009/06/13 21:40:47 shaino Exp $
#include <QtGui>
#include <QtOpenGL>

#include "glcamera.h";
#include "gllight.h";
#include "glviewer.h";
#include "glwidget.h"

#include "TMath.h"

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
  double fov = bFov*TMath::Power(sFov, (double)cFov/FOV_SCALE);
  glView->getCamera()->Reset();
  glView->getCamera()->Configure(fov, (double)cDol/DOL_SCALE, cen,
				 (double)hRot/ROT_SCALE*TMath::DegToRad(),
				 (double)vRot/ROT_SCALE*TMath::DegToRad()); 
				    
  emit zChanged(cFov);
  emit dChanged(cDol);
  emit vRotated(vRot);
  emit hRotated(hRot);

  if (update) updateGL();
}

void GLWidget::setLSet(int light, int sw, int update)
{
  glView->getLight()->setLight((GLLight::ELight)light, sw);
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
  double fov = bFov*TMath::Power(sFov, (double)cFov/FOV_SCALE);

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
  cFov -= event->delta()/10;
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

    glBox(xc[i], yc[i], zc[i], 0.1, 0.1, 0.1);
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

void GLWidget::glMatCol(const double *cpar)
{
  glMatCol(cpar[0], cpar[1], cpar[2], cpar[3]);
}

void GLWidget::glMatCol(double cr, double cg, double cb, double al)
{
  GLfloat col[17] = { 0.0, 0.0, 0.0, 1.0, 
		      0.2, 0.2, 0.2, 1.0,
		      0.7, 0.7, 0.7, 1.0,
		      0.0, 0.0, 0.0, 1.0, 60. };

  col[0] = cr; col[1] = cg; col[2] = cb; col[3] = al;

  glMaterialfv(GL_FRONT, GL_DIFFUSE,  &col[0]);
  glMaterialfv(GL_FRONT, GL_AMBIENT,  &col[4]);
  glMaterialfv(GL_BACK,  GL_AMBIENT,  &col[4]);
  glMaterialfv(GL_FRONT, GL_SPECULAR, &col[8]);
  glMaterialfv(GL_FRONT, GL_EMISSION, &col[12]);
  glMaterialf (GL_FRONT, GL_SHININESS, col[16]);
  glColor4fv(col);
}

void GLWidget::glBox(double  x, double  y, double  z, 
		     double dx, double dy, double dz)
{
  glBegin(GL_QUADS);

  glNormal3d(0, 0, -1);
  glVertex3d(x-dx, y-dy, z-dz);
  glVertex3d(x+dx, y-dy, z-dz);
  glVertex3d(x+dx, y+dy, z-dz);
  glVertex3d(x-dx, y+dy, z-dz);

  glNormal3d(0, 0, 1);
  glVertex3d(x-dx, y-dy, z+dz);
  glVertex3d(x+dx, y-dy, z+dz);
  glVertex3d(x+dx, y+dy, z+dz);
  glVertex3d(x-dx, y+dy, z+dz);

  glNormal3d(-1, 0, 0);
  glVertex3d(x-dx, y-dy, z-dz);
  glVertex3d(x-dx, y+dy, z-dz);
  glVertex3d(x-dx, y+dy, z+dz);
  glVertex3d(x-dx, y-dy, z+dz);

  glNormal3d(1, 0, 0);
  glVertex3d(x+dx, y-dy, z-dz);
  glVertex3d(x+dx, y+dy, z-dz);
  glVertex3d(x+dx, y+dy, z+dz);
  glVertex3d(x+dx, y-dy, z+dz);

  glNormal3d(0, -1, 0);
  glVertex3d(x-dx, y-dy, z-dz);
  glVertex3d(x-dx, y-dy, z+dz);
  glVertex3d(x+dx, y-dy, z+dz);
  glVertex3d(x+dx, y-dy, z-dz);

  glNormal3d(0, 1, 0);
  glVertex3d(x-dx, y+dy, z-dz);
  glVertex3d(x-dx, y+dy, z+dz);
  glVertex3d(x+dx, y+dy, z+dz);
  glVertex3d(x+dx, y+dy, z-dz);

  glEnd();
}

void GLWidget::glLine(double x1, double y1, double z1,
		      double x2, double y2, double z2)
{
  glBegin(GL_LINES);

  glVertex3d(x1, y1, z1);
  glVertex3d(x2, y2, z2);

  glEnd();
}

void GLWidget::glSphere(double x, double y, double z, double r, int ndiv)
{
  glPushMatrix();

  glTranslated(x, y, z);
  GLUquadricObj *sphere = gluNewQuadric();
  gluQuadricDrawStyle(sphere, GLU_FILL);
  gluSphere(sphere, r, ndiv, ndiv);

  glPopMatrix();
}

void GLWidget::glPLines(int n, double *x, double *y, double *z)
{
  glBegin(GL_LINES);

  for (int i = 0; i < n-1; i++) {
    glVertex3d(x[i],   y[i],   z[i]);
    glVertex3d(x[i+1], y[i+1], z[i+1]);
  }

  glEnd();
}

void GLWidget::glPoints(int n, double *x, double *y, double *z)
{
  glBegin(GL_POINTS);

  for (int i = 0; i < n-1; i++) {
    glVertex3d(x[i],   y[i],   z[i]);
    glVertex3d(x[i+1], y[i+1], z[i+1]);
  }

  glEnd();
}

void GLWidget::glDigit(double x0, double y0, double z0, int digit, 
		       int mode, double size)
{
  if (digit < -999 || 999 < digit) return;

  double step = -size;
  if (digit < 0) {
    glNum(x0, y0, z0, -1, mode, size);
    digit = -digit;
  }
  double sx = (abs(mode) == 2) ? -step : 0;
  double sy = (abs(mode) == 1) ?  step : 0;
  x0 += sx; y0 += sy;
  glNum(x0, y0, z0, (digit/100)%10, mode, size); x0+= sx; y0+= sy;
  glNum(x0, y0, z0, (digit/ 10)%10, mode, size); x0+= sx; y0+= sy;
  glNum(x0, y0, z0, (digit    )%10, mode, size);
}

void GLWidget::glNum(double x0, double y0, double z0, int num, 
		     int mode, double size)
{
  if (num < -1 || 9 < num) return;

  double scl = size*0.01;

  double x[30], y[30], z[30];
  for (int j = 0; j < numDataN[num+1]; j++) {
    int jj = numDataI[num+1]+j;
    x[j] = (abs(mode) == 2) ? x0+numDataX[jj]*scl 
                            : x0-numDataY[jj]*scl*mode;
    y[j] = (abs(mode) == 2) ? y0-numDataY[jj]*scl*mode/2
                            : y0-numDataX[jj]*scl;
    z[j] = z0;
  }
  glPLines(numDataN[num+1], x, y, z);
}

int GLWidget::numDataN[GLWidget::NNUM] 
 = { 2, 17, 4, 14, 15, 4, 17, 23, 4, 29, 23 };

int GLWidget::numDataI[GLWidget::NNUM] 
 = { 0, 2, 19, 23, 37, 52, 56, 73, 96, 100, 129 };

int GLWidget::numDataX[GLWidget::NNPT] 
= { -50,  50,  -5, -21, -31, -37, -37, -31, -21,  -5,   6,  21,  32,  37,  37,
     32,  21,   6,  -5, -13,  -2,  13,  13, -32, -32, -26, -21, -11,  11,  21,
     26,  32,  32,  26,  16, -37,  37, -27,  32,   0,  16,  26,  32,  37,  37,
     32,  21,   5, -11, -27, -32, -37,  14,  14, -40,  40,  26, -26, -32, -26,
    -10,   6,  22,  32,  38,  38,  32,  22,   6, -10, -26, -32, -38,  29,  24,
      8,  -3, -19, -29, -34, -34, -29, -19,  -3,   3,  19,  29,  34,  34,  29,
     19,   3,  -3, -19, -29, -34, -16,  37, -37,  37, -11, -26, -32, -32, -26,
    -16,   5,  21,  32,  37,  37,  32,  27,  11, -11, -26, -32, -37, -37, -32,
    -21,  -5,  16,  27,  32,  32,  27,  11, -11,  34,  28,  18,   2,  -2, -18,
    -30, -34, -34, -30, -18,  -2,   2,  18,  28,  34,  34,  28,  18,   2,  -8,
    -24, -30 };

int GLWidget::numDataY[GLWidget::NNPT] 
= {   0,   0,  56,  50,  34,   8,  -8, -34, -50, -56, -56, -50, -34,  -8,   8,
     34,  50,  56,  56,  34,  40,  56, -56,  29,  34,  45,  50,  56,  56,  50,
     45,  34,  24,  13,  -3, -56, -56,  56,  56,  13,  13,   8,   3, -13, -24,
    -40, -50, -56, -56, -50, -45, -34, -56,  56, -19, -19,  56,  56,   8,  13,
     19,  19,  13,   3, -13, -24, -40, -50, -56, -56, -50, -45, -34,  40,  50,
     56,  56,  50,  34,   8, -19, -40, -50, -56, -56, -50, -40, -24, -19,  -3,
      8,  13,  13,   8,  -3, -19, -56,  56,  56,  56,  56,  50,  40,  29,  19,
     13,   8,   3,  -8, -19, -34, -45, -50, -56, -56, -50, -45, -34, -19,  -8,
      3,   8,  13,  19,  29,  40,  50,  56,  56,  19,   3,  -8, -13, -13,  -8,
      3,  19,  24,  40,  50,  56,  56,  50,  40,  19,  -8, -34, -50, -56, -56,
    -50, -40 };

