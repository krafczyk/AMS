// $Id: glrender.cpp,v 1.3 2011/05/04 16:36:54 pzuccon Exp $

#include "glrender.h"
#include <cmath>
#include <cstdlib>

#include <GL/gl.h>
#include <GL/glu.h>

std::vector<GLRender *> GLRender::svRender;

GLRender::GLRender(int id, double *mcol, double *scol)
  : glID(id), invNormal(false), glEnabled(true)
{
  nbPol = 0;
  setMatCol(mcol);
  setSelCol(scol);
}

GLRender::GLRender(int id, int n, double *x, double *y, double *z,
		   double *mcol, double *scol)
  : glID(id), invNormal(false), glEnabled(true)
{
  nbPol = 0;
  setMatCol(mcol);
  setSelCol(scol);
  buildGLPLines(n, x, y, z);
}

GLRender::GLRender(int id, int npnt, int nseg, int npol, const double *vpnt, 
		   const int *vseg, const int *vpol, 
		   double *mcol, double *scol)
  : glID(id), invNormal(false), glEnabled(true)
{
  setMatCol(mcol);
  setSelCol(scol);
  buildGL(npnt, nseg, npol, vpnt, vseg, vpol);
}

GLRender::~GLRender()
{
}

void GLRender::setMatCol(const double *mcol)
{
  for (int i = 0; i < 4; i++) matCol[i] = (mcol) ? mcol[i] : 0;
}

void GLRender::setSelCol(const double *scol)
{
  for (int i = 0; i < 4; i++) selCol[i] = (scol) ? scol[i] : matCol[i];
}

void GLRender::drawObject(bool render, int idsel)
{
  if (!glEnabled || matCol[3] == 0) return;

  if (render) {
    if (idsel > 0 && idsel == glID)
      glMatCol(selCol);
    else
      glMatCol(matCol);
  }

  if (glNormal.empty() && polyDesc.empty()) {
    glBegin(GL_LINES);

    int n = glVertex.size()/3;
    for (int i = 0; i < n-1; i++) {
      glVertex3d(glVertex[i*3],   glVertex[i*3+1], glVertex[i*3+2]);
      glVertex3d(glVertex[i*3+3], glVertex[i*3+4], glVertex[i*3+5]);
    }

    glEnd();
  }
  else {
    for (int i = 0, j = 0; i < nbPol; ++i) {
      int npoints = polyDesc[j++];

      glBegin(GL_POLYGON);
      glNormal3dv(&glNormal[i*3]);

      for (int k = 0; k < npoints; ++k, ++j)
	glVertex3dv(&glVertex[polyDesc[j]*3]);

      glEnd();
    }
  }
}

void GLRender::buildGL(int npnt, int nseg, int npol, const double *vpnt, 
		       const int *vseg, const int *vpol)
{
  nbPol = npol;
  glNormal.assign(nbPol*3, 0);

  for (int i = 0; i < npnt*3; i++)
    glVertex.push_back(vpnt[i]);

  int dsiz = 0;

  for (int i = 0, j = 1; i < nbPol; ++i, ++j) {
    dsiz += vpol[j]+1;
    j += vpol[j]+1;
  }

  polyDesc.resize(dsiz);

  for (int numPol = 0, currInd = 0, j = 1; numPol < nbPol; ++numPol) {
    int segmentInd = vpol[j]+j;
    int segmentCol = vpol[j];
    int s1 = vpol[segmentInd];
    segmentInd--;
    int s2 = vpol[segmentInd];
    segmentInd--;

    int segEnds[] = {vseg[s1*3+1], vseg[s1*3+2], vseg[s2*3+1], vseg[s2*3+2]};
    int numPnts[3];

    if (segEnds[0] == segEnds[2]) {
      numPnts[0] = segEnds[1]; 
      numPnts[1] = segEnds[0]; 
      numPnts[2] = segEnds[3];
    }
    else if (segEnds[0] == segEnds[3]) {
      numPnts[0] = segEnds[1];
      numPnts[1] = segEnds[0];
      numPnts[2] = segEnds[2];
    }
    else if (segEnds[1] == segEnds[2]) {
      numPnts[0] = segEnds[0];
      numPnts[1] = segEnds[1];
      numPnts[2] = segEnds[3];
    }
    else {
      numPnts[0] = segEnds[0];
      numPnts[1] = segEnds[1];
      numPnts[2] = segEnds[2];
    }

    polyDesc[currInd] = 3;
    int sizeInd = currInd++;
    polyDesc[currInd++] = numPnts[0];
    polyDesc[currInd++] = numPnts[1];
    polyDesc[currInd++] = numPnts[2];
    int lastAdded = numPnts[2];

    int end = j+1;
    for (; segmentInd != end; segmentInd--) {
      segEnds[0] = vseg[vpol[segmentInd]*3+1];
      segEnds[1] = vseg[vpol[segmentInd]*3+2];
      if (segEnds[0] == lastAdded) {
	polyDesc[currInd++] = segEnds[1];
	lastAdded = segEnds[1];
      } else {
	polyDesc[currInd++] = segEnds[0];
	lastAdded = segEnds[0];
      }
      ++polyDesc[sizeInd];
    }
    j += segmentCol+2;
  }

  fillNormals();
}

void GLRender::buildGLBox(double  x, double  y, double  z, 
			  double dx, double dy, double dz)
{
  double vpnt[24];
  vpnt[ 0] = -dx+x; vpnt[ 1] = -dy+y; vpnt[ 2] = -dz+z;
  vpnt[ 3] = -dx+x; vpnt[ 4] =  dy+y; vpnt[ 5] = -dz+z;
  vpnt[ 6] =  dx+x; vpnt[ 7] =  dy+y; vpnt[ 8] = -dz+z;
  vpnt[ 9] =  dx+x; vpnt[10] = -dy+y; vpnt[11] = -dz+z;
  vpnt[12] = -dx+x; vpnt[13] = -dy+y; vpnt[14] =  dz+z;
  vpnt[15] = -dx+x; vpnt[16] =  dy+y; vpnt[17] =  dz+z;
  vpnt[18] =  dx+x; vpnt[19] =  dy+y; vpnt[20] =  dz+z;
  vpnt[21] =  dx+x; vpnt[22] = -dy+y; vpnt[23] =  dz+z;

  int vseg[36], c = 0;
  vseg[ 0] = c;   vseg[ 1] = 0; vseg[ 2] = 1;
  vseg[ 3] = c+1; vseg[ 4] = 1; vseg[ 5] = 2;
  vseg[ 6] = c+1; vseg[ 7] = 2; vseg[ 8] = 3;
  vseg[ 9] = c;   vseg[10] = 3; vseg[11] = 0;
  vseg[12] = c+2; vseg[13] = 4; vseg[14] = 5;
  vseg[15] = c+2; vseg[16] = 5; vseg[17] = 6;
  vseg[18] = c+3; vseg[19] = 6; vseg[20] = 7;
  vseg[21] = c+3; vseg[22] = 7; vseg[23] = 4;
  vseg[24] = c;   vseg[25] = 0; vseg[26] = 4;
  vseg[27] = c+2; vseg[28] = 1; vseg[29] = 5;
  vseg[30] = c+1; vseg[31] = 2; vseg[32] = 6;
  vseg[33] = c+3; vseg[34] = 3; vseg[35] = 7;

  int vpol[36];
  vpol[ 0] = c;   vpol[ 1] = 4; vpol[ 2] = 0;
  vpol[ 3] = 9;   vpol[ 4] = 4; vpol[ 5] = 8;
  vpol[ 6] = c+1; vpol[ 7] = 4; vpol[ 8] = 1;
  vpol[ 9] = 10;  vpol[10] = 5; vpol[11] = 9;
  vpol[12] = c;   vpol[13] = 4; vpol[14] = 2;
  vpol[15] = 11;  vpol[16] = 6; vpol[17] = 10;
  vpol[18] = c+1; vpol[19] = 4; vpol[20] = 3;
  vpol[21] = 8;   vpol[22] = 7; vpol[23] = 11;
  vpol[24] = c+2; vpol[25] = 4; vpol[26] = 0;
  vpol[27] = 3;   vpol[28] = 2; vpol[29] = 1;
  vpol[30] = c+3; vpol[31] = 4; vpol[32] = 4;
  vpol[33] = 5;   vpol[34] = 6; vpol[35] = 7;

  buildGL(8*3, 12*3, 6*6, vpnt, vseg, vpol);
}

void GLRender::buildGLPLines(int n, double *x, double *y, double *z)
{
  for (int i = 0; i < n; i++) {
    glVertex.push_back(x[i]);
    glVertex.push_back(y[i]);
    glVertex.push_back(z[i]);
  }
}

void GLRender::drawAll(bool render, bool select, int idsel, int order)
{
  int i1 = 0, i2 = svRender.size()-1, di = 1;
  if (order < 0) {
    i1 = svRender.size()-1;
    i2 = 0;
    di = -1;
  }
  for (int i = i1; i != i2; i += di) {
    GLRender *gobj = svRender.at(i);
    if (!gobj) continue;

    if (select) glLoadName(gobj->getGLID());
    gobj->drawObject(render, idsel);
  }
}

GLRender *GLRender::findObject(int id)
{
  for (int i = 0; i < svRender.size(); i++) {
    GLRender *gobj = svRender.at(i);
    if (gobj && gobj->getGLID() == id) return gobj;
  }
  return 0;
}

void GLRender::glMatCol(const double *cpar)
{
  glMatCol(cpar[0], cpar[1], cpar[2], cpar[3]);
}

void GLRender::glMatCol(double cr, double cg, double cb, double al)
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

void GLRender::glBox(double  x, double  y, double  z, 
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

void GLRender::glLine(double x1, double y1, double z1,
		      double x2, double y2, double z2)
{
  glBegin(GL_LINES);

  glVertex3d(x1, y1, z1);
  glVertex3d(x2, y2, z2);

  glEnd();
}

void GLRender::glSphere(double x, double y, double z, double r, int ndiv)
{
  glPushMatrix();

  glTranslated(x, y, z);
  GLUquadricObj *sphere = gluNewQuadric();
  gluQuadricDrawStyle(sphere, GLU_FILL);
  gluSphere(sphere, r, ndiv, ndiv);

  glPopMatrix();
}

void GLRender::glPLines(int n, double *x, double *y, double *z)
{
  glBegin(GL_LINES);

  for (int i = 0; i < n-1; i++) {
    glVertex3d(x[i],   y[i],   z[i]);
    glVertex3d(x[i+1], y[i+1], z[i+1]);
  }

  glEnd();
}

void GLRender::glPoints(int n, double *x, double *y, double *z)
{
  glBegin(GL_POINTS);

  for (int i = 0; i < n-1; i++) {
    glVertex3d(x[i],   y[i],   z[i]);
    glVertex3d(x[i+1], y[i+1], z[i+1]);
  }

  glEnd();
}

void GLRender::glDigit(double x0, double y0, double z0, int digit, 
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

void GLRender::glNum(double x0, double y0, double z0, int num, 
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

void GLRender::fillNormals()
{
  double *pnts = &glVertex[0];
   for (int i = 0, j = 0; i < nbPol; ++i) {
      int polEnd = polyDesc[j]+j+1;
      int norm[] = {polyDesc[j+1], polyDesc[j+2], polyDesc[j+3]};
      j += 4;
      int check = checkPoints(norm, norm), ngood = check;
      if (check == 3) {
         normPlane(pnts+norm[0]*3, pnts+norm[1]*3,
		   pnts+norm[2]*3, &glNormal[i*3]);
         j = polEnd;
         continue;
      }
      while (j < polEnd) {
         norm[ngood++] = polyDesc[j++];
         if (ngood == 3) {
            ngood = checkPoints(norm, norm);
            if (ngood == 3) {
	       normPlane(pnts+norm[0]*3, pnts+norm[1]*3,
			 pnts+norm[2]*3, &glNormal[i*3]);
               j = polEnd;
               break;
            }
         }
      }
   }
}

int GLRender::checkPoints(const int *source, int *dest)
{
   const double*p1 = &glVertex[source[0]*3];
   const double*p2 = &glVertex[source[1]*3];
   const double*p3 = &glVertex[source[2]*3];
   int retVal = 1;

   if (eqTest(p1, p2)) {
      dest[0] = source[0];
      if (!eqTest(p1, p3) ) {
         dest[1] = source[2];
         retVal = 2;
      }
   } else if (eqTest(p1, p3)) {
      dest[0] = source[0];
      dest[1] = source[1];
      retVal = 2;
   } else {
      dest[0] = source[0];
      dest[1] = source[1];
      retVal = 2;
      if (!eqTest(p2, p3)) {
         dest[2] = source[2];
         retVal = 3;
      }
   }

   return retVal;
}

bool GLRender::eqTest(const double *p1, const double *p2)
{
   double dx = fabs(p1[0]-p2[0]);
   double dy = fabs(p1[1]-p2[1]);
   double dz = fabs(p1[2]-p2[2]);
   return (dx < 1e-10 && dy < 1e-10 && dz < 1e-10);
}

double *GLRender::normPlane(const double p1[3], const double p2[3],
			    const double p3[3], double norm[3])
{
  double v1[3], v2[3];

  v1[0] = p2[0]-p1[0];
  v1[1] = p2[1]-p1[1];
  v1[2] = p2[2]-p1[2];

  v2[0] = p3[0]-p1[0];
  v2[1] = p3[1]-p1[1];
  v2[2] = p3[2]-p1[2];

  norm[0] = v1[1]*v2[2]-v1[2]*v2[1];
  norm[1] = v1[2]*v2[0]-v1[0]*v2[2];
  norm[2] = v1[0]*v2[1]-v1[1]*v2[0];

  double vv = std::sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
  if (invNormal) vv = -vv;
  norm[0] /= vv;
  norm[1] /= vv;
  norm[2] /= vv;

  return norm;
}


int GLRender::numDataN[GLRender::NNUM] 
 = { 2, 17, 4, 14, 15, 4, 17, 23, 4, 29, 23 };

int GLRender::numDataI[GLRender::NNUM] 
 = { 0, 2, 19, 23, 37, 52, 56, 73, 96, 100, 129 };

int GLRender::numDataX[GLRender::NNPT] 
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

int GLRender::numDataY[GLRender::NNPT] 
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

