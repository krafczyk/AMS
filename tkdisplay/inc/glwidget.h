// $Id: glwidget.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// GLWidget : a class to manage OpenGL functions by SH
//
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

class GLViewer;

class GLWidget : public QGLWidget
{
  Q_OBJECT

public:
  GLWidget(QWidget *parent = 0);
 ~GLWidget();

  QSize minimumSizeHint() const { return QSize(480, 480); }
  QSize sizeHint()        const { return QSize(480, 480); }

public:
  enum { FOV_SCALE = 100, FOV_MIN  = -300, FOV_MAX =  50, FOV_DEF =  0,
	 DOL_SCALE =   1, DOL_MIN  = -100, DOL_MAX = 500, DOL_DEF = 10,
	 ROT_SCALE =  16, ROTV_DEF = 210*ROT_SCALE, 
	                  ROTH_DEF = -20*ROT_SCALE };

public slots:
  void cZoom  (int val, int update = 1);
  void cDolly (int val, int update = 1);
  void hRotate(int val, int update = 1);
  void vRotate(int val, int update = 1);
  void cReset (int update = 1);

  void setLSet(int light, int sw, int update = 1);

  void setLSize(int val, int update = 1);
  void setLZpos(int val, int update = 1);

signals:
  void zChanged(int val);
  void dChanged(int val);
  void hRotated(int val);
  void vRotated(int val);

protected:
  virtual void initializeGL();
  virtual void paintGL();
  virtual void resizeGL(int width, int height);

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent (QMouseEvent *event);
  virtual void leaveEvent     (QEvent      *event);
  virtual void wheelEvent     (QWheelEvent *event);
  virtual void keyPressEvent  (QKeyEvent   *event);

  virtual void drawObject(GLenum mode);
  virtual bool pickObject(int x, int y);
  virtual void processSel(GLint nhit, GLuint *sbuf);
  virtual void processPick() {}

protected:
  void glMatCol(const double *cpar);
  void glMatCol(double cr, double cg, double cb, double al);

  void glBox(double  x, double  y, double  z, 
	     double dx, double dy, double dz);

  void glLine(double x1, double y1, double z1,
	      double x2, double y2, double z2);

  void glSphere(double x, double y, double z, double r, int ndiv = 6);

  void glPLines(int n, double *x, double *y, double *z);
  void glPoints(int n, double *x, double *y, double *z);

  void glDigit(double x0, double y0, double z0, int digit, 
	       int mode = 1, double size = 1.5);
  void glNum(double x0, double y0, double z0, int num, 
	     int mode = 1, double size = 1.5);

protected:
  GLViewer *glView;

  int vpWidth;
  int vpHeight;

  double bFov;
  double sFov;

  int cFov;
  int cDol;
  int hRot;
  int vRot;

  QPoint lastMpos;

  int idSel;

private:
  enum { NNUM = 11, NNPT = 152 };
  static int numDataN[NNUM];
  static int numDataI[NNUM];
  static int numDataX[NNPT];
  static int numDataY[NNPT];
};

#endif
