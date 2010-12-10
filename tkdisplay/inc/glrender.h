// $Id: glrender.h,v 1.2 2010/12/10 21:38:01 shaino Exp $
//
// GLRender : a class to manage 3D object defined as TBuffer3D format 
//            (but independent of TBuffer3D)
//            imported from Root:TGLFaceSet and arranged by SH
//
//
#ifndef GLRENDER_H
#define GLRENDER_H

#include <vector>

class GLRender {

public:
  GLRender(int id, double *mcol = 0, double *scol = 0);
  GLRender(int id, int n, double *x, double *y, double *z,
	   double *mcol = 0, double *scol = 0);
  GLRender(int id, int npnt, int nseg, int npol, 
	   const double *vpnt, const int *vseg, const int *vpol,
	   double *mcol = 0, double *scol = 0);
 ~GLRender();

  int getGLID() const { return glID; }

  void setEnabled (bool enabled = true)  { glEnabled = enabled; }
  void setDisabled(bool enabled = false) { glEnabled = enabled; }

  void setMatCol(const double *col);
  void setSelCol(const double *col);

  void invertNormal(bool inv = true) { invNormal = inv; }

  void drawObject(bool render, int idsel = -1);

  void buildGL(int npnt, int nseg, int npol, 
	       const double *vpnt, const int *vseg, const int *vpol);

  void buildGLBox(double  x, double  y, double  z, 
		  double dx, double dy, double dz);

  void buildGLPLines(int n, double *x, double *y, double *z);

public:
  static void addObject(GLRender *buf) { svRender.push_back(buf); }
  static void drawAll(bool render, bool select, int idsel = -1, int order = 1);
  static GLRender *findObject(int id);

  static void glMatCol(const double *cpar);
  static void glMatCol(double cr, double cg, double cb, double al);

  static void glBox(double  x, double  y, double  z, 
		    double dx, double dy, double dz);

  static void glLine(double x1, double y1, double z1,
		     double x2, double y2, double z2);

  static void glSphere(double x, double y, double z, double r, int ndiv = 6);

  static void glPLines(int n, double *x, double *y, double *z);
  static void glPoints(int n, double *x, double *y, double *z);

  static void glDigit(double x0, double y0, double z0, int digit, 
		      int mode = 1, double size = 1.5);
  static void glNum(double x0, double y0, double z0, int num, 
		    int mode = 1, double size = 1.5);

protected:
  void fillNormals();
  int  checkPoints(const int *source, int *dest);

  bool    eqTest   (const double *p1,    const double *p2);
  double *normPlane(const double  p1[3], const double  p2[3],
		    const double  p3[3], double norm[3]);

protected:
  int glID;

  int nbPol;
  std::vector<double> glVertex;
  std::vector<double> glNormal;
  std::vector<int>    polyDesc;

  bool invNormal;
  bool glEnabled;

  double matCol[4];
  double selCol[4];

  static std::vector<GLRender *> svRender;

private:
  enum { NNUM = 11, NNPT = 152 };
  static int numDataN[NNUM];
  static int numDataI[NNUM];
  static int numDataX[NNPT];
  static int numDataY[NNPT];
};

#endif
