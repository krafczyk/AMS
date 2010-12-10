// $Id: glutils.cpp,v 1.2 2010/12/10 21:38:01 shaino Exp $
#include "glutils.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <algorithm>
#include <string>
#include <map>
#include <iostream>

#include "TMath.h"

GLVertex3::GLVertex3()
{
   // Construct a default (0.0, 0.0, 0.0) vertex
   Fill(0.0);
}

GLVertex3::GLVertex3(double x, double y, double z)
{
   // Construct a vertex with components (x,y,z)
   Set(x,y,z);
}

GLVertex3::GLVertex3(double* v)
{
   // Construct a vertex with components (v[0], v[1], v[2])
   Set(v[0], v[1], v[2]);
}

GLVertex3::GLVertex3(const GLVertex3 & other)
{
   // Construct a vertex from 'other'
   Set(other);
}

GLVertex3::~GLVertex3()
{
   // Destroy vertex object
}

void GLVertex3::Shift(GLVector3 & shift)
{
   // Offset a vertex by vector 'shift'
   fVals[0] += shift[0];
   fVals[1] += shift[1];
   fVals[2] += shift[2];
}

void GLVertex3::Shift(double xDelta, double yDelta, double zDelta)
{
   // Offset a vertex by components (xDelta, yDelta, zDelta)
   fVals[0] += xDelta;
   fVals[1] += yDelta;
   fVals[2] += zDelta;
}

void GLVertex3::Minimum(const GLVertex3 & other)
{
   fVals[0] = TMath::Min(fVals[0], other.fVals[0]);
   fVals[1] = TMath::Min(fVals[1], other.fVals[1]);
   fVals[2] = TMath::Min(fVals[2], other.fVals[2]);
}

void GLVertex3::Maximum(const GLVertex3 & other)
{
   fVals[0] = TMath::Max(fVals[0], other.fVals[0]);
   fVals[1] = TMath::Max(fVals[1], other.fVals[1]);
   fVals[2] = TMath::Max(fVals[2], other.fVals[2]);
}

void GLVertex3::Dump() const
{
   // Output vertex component values to std::cout
   std::cout << "(" << fVals[0] << "," << fVals[1] << "," << fVals[2] << ")" << std::endl;
}


GLVector3::GLVector3() :
   GLVertex3()
{
   // Construct a default (0.0, 0.0, 0.0) vector
}

GLVector3::GLVector3(double x, double y, double z) :
   GLVertex3(x, y, z)
{
   // Construct a vector with components (x,y,z)
}

GLVector3::GLVector3(const GLVector3 & other) :
   GLVertex3(other.fVals[0], other.fVals[1], other.fVals[2])
{
   // Construct a vector from components of 'other'
}

GLVector3::GLVector3(const double *src) :
   GLVertex3(src[0], src[1], src[2])
{
   // Construct a vector with components (src[0], src[1], src[2])
}

GLVector3::~GLVector3()
{
   // Destroy vector object
}


GLLine3::GLLine3(const GLVertex3 & start, const GLVertex3 & end) :
   fVertex(start), fVector(end - start)
{
   // Construct 3D line running from 'start' to 'end'
}

GLLine3::GLLine3(const GLVertex3 & start, const GLVector3 & vect) :
   fVertex(start), fVector(vect)
{
   // Construct 3D line running from 'start', magnitude 'vect'
}

GLLine3::~GLLine3()
{
   // Destroy 3D line object
}

void GLLine3::Set(const GLVertex3 & start, const GLVertex3 & end)
{
   // Set 3D line running from 'start' to 'end'

   fVertex = start;
   fVector = end - start;
}

void GLLine3::Set(const GLVertex3 & start, const GLVector3 & vect)
{
   // Set 3D line running from start, magnitude 'vect'
   fVertex = start;
   fVector = vect;
}

void GLLine3::Draw() const
{
   // Draw line in current basic GL color. Assume we are in the correct reference
   // frame
   glBegin(GL_LINE_LOOP);
   glVertex3dv(fVertex.CArr());
   glVertex3dv(End().CArr());
   glEnd();
}



GLPlane::GLPlane()
{
   // Construct a default plane of x + y + z = 0
   Set(1.0, 1.0, 1.0, 0.0);
}

GLPlane::GLPlane(const GLPlane & other)
{
   // Construct plane from 'other'
   Set(other);
}

GLPlane::GLPlane(double a, double b, double c, double d)
{
   // Construct plane with equation a.x + b.y + c.z + d = 0
   // with optional normalisation
   Set(a, b, c, d);
}

GLPlane::GLPlane(double eq[4])
{
   // Construct plane with equation eq[0].x + eq[1].y + eq[2].z + eq[3] = 0
   // with optional normalisation
   Set(eq);
}

GLPlane::GLPlane(const GLVertex3 & p1, const GLVertex3 & p2,
                   const GLVertex3 & p3)
{
   // Construct plane passing through 3 supplied points
   // with optional normalisation
   Set(p1, p2, p3);
}

GLPlane::GLPlane(const GLVector3 & v, const GLVertex3 & p)
{
   // Construct plane with supplied normal vector, passing through point
   // with optional normalisation
   Set(v, p);
}

GLPlane::~GLPlane()
{
   // Destroy plane object
}

void GLPlane::Dump() const
{
   // Output plane equation to std::out
   std::cout.precision(6);
   std::cout << "Plane : " << fVals[0] << "x + " << fVals[1] << "y + " << fVals[2] << "z + " << fVals[3] << std::endl;
}

// Some free functions for plane intersections

std::pair<bool, GLLine3> Intersection(const GLPlane & p1, const GLPlane & p2)
{
   // Find 3D line interestion of this plane with 'other'. Returns a std::pair
   //
   // first (bool)                   second (GLLine3)
   // kTRUE - planes intersect         intersection line between planes
   // kFALSE - no intersect (parallel) undefined
   GLVector3 lineDir = Cross(p1.Norm(), p2.Norm());

   if (lineDir.Mag() == 0.0) {
      return std::make_pair(kFALSE, GLLine3(GLVertex3(0.0, 0.0, 0.0),
                                             GLVector3(0.0, 0.0, 0.0)));
   }
   GLVertex3 linePoint = Cross((p1.Norm()*p2.D() - p2.Norm()*p1.D()), lineDir) /
                           Dot(lineDir, lineDir);
   return std::make_pair(kTRUE, GLLine3(linePoint, lineDir));
}

std::pair<bool, GLVertex3> Intersection(const GLPlane & p1, const GLPlane & p2, const GLPlane & p3)
{
   double denom = Dot(p1.Norm(), Cross(p2.Norm(), p3.Norm()));
   if (denom == 0.0) {
      return std::make_pair(kFALSE, GLVertex3(0.0, 0.0, 0.0));
   }
   GLVector3 vect = ((Cross(p2.Norm(),p3.Norm())* -p1.D()) -
                      (Cross(p3.Norm(),p1.Norm())*p2.D()) -
                      (Cross(p1.Norm(),p2.Norm())*p3.D())) / denom;
   GLVertex3 interVert(vect.X(), vect.Y(), vect.Z());
   return std::make_pair(kTRUE, interVert);
}

std::pair<bool, GLVertex3> Intersection(const GLPlane & plane, const GLLine3 & line, bool extend)
{
   // Find intersection of 3D space 'line' with this plane. If 'extend' is kTRUE
   // then line extents can be extended (infinite length) to find intersection.
   // If 'extend' is kFALSE the fixed extents of line is respected.
   //
   // The return a std::pair
   //
   // first (bool)                   second (GLVertex3)
   // kTRUE - line/plane intersect     intersection vertex on plane
   // kFALSE - no line/plane intersect undefined
   //
   // If intersection is not found (first == kFALSE) & 'extend' was kTRUE (infinite line)
   // this implies line and plane are parallel. If 'extend' was kFALSE, then
   // either line parallel or insuffient length.
   double denom = -(plane.A()*line.Vector().X() +
                      plane.B()*line.Vector().Y() +
                      plane.C()*line.Vector().Z());

   if (denom == 0.0) {
      return std::make_pair(kFALSE, GLVertex3(0.0, 0.0, 0.0));
   }

   double num = plane.A()*line.Start().X() + plane.B()*line.Start().Y() +
                  plane.C()*line.Start().Z() + plane.D();
   double factor = num/denom;

   // If not extending (projecting) line is length from start enough to reach plane?
   if (!extend && (factor < 0.0 || factor > 1.0)) {
      return std::make_pair(kFALSE, GLVertex3(0.0, 0.0, 0.0));
   }

   GLVector3 toPlane = line.Vector() * factor;
   return std::make_pair(kTRUE, line.Start() + toPlane);
}


GLMatrix::GLMatrix()
{
   // Construct default identity matrix:
   //
   // 1 0 0 0
   // 0 1 0 0
   // 0 0 1 0
   // 0 0 0 1
   SetIdentity();
}

GLMatrix::GLMatrix(double x, double y, double z)
{
   // Construct matrix with translation components x,y,z:
   //
   // 1 0 0 x
   // 0 1 0 y
   // 0 0 1 z
   // 0 0 0 1
   SetIdentity();
   SetTranslation(x, y, z);
}

GLMatrix::GLMatrix(const GLVertex3 & translation)
{
   // Construct matrix with translation components x,y,z:
   //
   // 1 0 0 translation.X()
   // 0 1 0 translation.Y()
   // 0 0 1 translation.Z()
   // 0 0 0 1
   SetIdentity();
   SetTranslation(translation);
}

GLMatrix::GLMatrix(const GLVertex3 & origin, const GLVector3 & zAxis)
{
   // Construct matrix which when applied puts local origin at
   // 'origin' and the local Z axis in direction 'z'. Both
   // 'origin' and 'zAxisVec' are expressed in the parent frame
   SetIdentity();

   GLVector3 zAxisInt(zAxis);
   zAxisInt.Normalise();
   GLVector3 arbAxis;

   if (TMath::Abs(zAxisInt.X()) <= TMath::Abs(zAxisInt.Y()) && TMath::Abs(zAxisInt.X()) <= TMath::Abs(zAxisInt.Z())) {
      arbAxis.Set(1.0, 0.0, 0.0);
   } else if (TMath::Abs(zAxisInt.Y()) <= TMath::Abs(zAxisInt.X()) && TMath::Abs(zAxisInt.Y()) <= TMath::Abs(zAxisInt.Z())) {
      arbAxis.Set(0.0, 1.0, 0.0);
   } else {
      arbAxis.Set(0.0, 0.0, 1.0);
   }

   Set(origin, zAxis, Cross(zAxisInt, arbAxis));
}

GLMatrix::GLMatrix(const GLVertex3 & origin, const GLVector3 & zAxis, const GLVector3 & xAxis)
{
   // Construct matrix which when applied puts local origin at
   // 'origin' and the local Z axis in direction 'z'. Both
   // 'origin' and 'zAxisVec' are expressed in the parent frame
   SetIdentity();
   Set(origin, zAxis, xAxis);
}

GLMatrix::GLMatrix(const double vals[16])
{
   // Construct matrix using the 16 double 'vals' passed,
   // ordering is maintained - i.e. should be column major
   // as we are
   Set(vals);
}

GLMatrix::GLMatrix(const GLMatrix & other)
{
   // Construct matrix from 'other'
   *this = other;
}

GLMatrix::~GLMatrix()
{
   // Destroy matirx object
}

void GLMatrix::MultRight(const GLMatrix & rhs)
{
   // Multiply with matrix rhs on right.

  double  B[4];
  double* C = fVals;
  for(int r=0; r<4; ++r, ++C)
  {
    const double* T = rhs.fVals;
    for(int c=0; c<4; ++c, T+=4)
      B[c] = C[0]*T[0] + C[4]*T[1] + C[8]*T[2] + C[12]*T[3];
    C[0] = B[0]; C[4] = B[1]; C[8] = B[2]; C[12] = B[3];
  }
}

void GLMatrix::MultLeft (const GLMatrix & lhs)
{
   // Multiply with matrix lhs on left.

   double  B[4];
   double* C = fVals;
   for (int c=0; c<4; ++c, C+=4)
   {
      const double* T = lhs.fVals;
      for(int r=0; r<4; ++r, ++T)
         B[r] = T[0]*C[0] + T[4]*C[1] + T[8]*C[2] + T[12]*C[3];
      C[0] = B[0]; C[1] = B[1]; C[2] = B[2]; C[3] = B[3];
   }
}

void GLMatrix::Set(const GLVertex3 & origin, const GLVector3 & zAxis, const GLVector3 & xAxis)
{
   // Set matrix which when applied puts local origin at
   // 'origin' and the local Z axis in direction 'z'. Both
   // 'origin' and 'z' are expressed in the parent frame
   GLVector3 zAxisInt(zAxis);
   zAxisInt.Normalise();

   GLVector3 xAxisInt(xAxis);
   xAxisInt.Normalise();
   GLVector3 yAxisInt = Cross(zAxisInt, xAxisInt);

   fVals[0] = xAxisInt.X(); fVals[4] = yAxisInt.X(); fVals[8 ] = zAxisInt.X(); fVals[12] = origin.X();
   fVals[1] = xAxisInt.Y(); fVals[5] = yAxisInt.Y(); fVals[9 ] = zAxisInt.Y(); fVals[13] = origin.Y();
   fVals[2] = xAxisInt.Z(); fVals[6] = yAxisInt.Z(); fVals[10] = zAxisInt.Z(); fVals[14] = origin.Z();
   fVals[3] = 0.0;          fVals[7] = 0.0;          fVals[11] = 0.0;          fVals[15] = 1.0;
}

void GLMatrix::Set(const double vals[16])
{
   // Set matrix using the 16 double 'vals' passed,
   // ordering is maintained - i.e. should be column major
   // as we are
   for (unsigned int i=0; i < 16; i++) {
      fVals[i] = vals[i];
   }
}

void GLMatrix::SetIdentity()
{
   // Set matrix to identity:
   //
   // 1 0 0 0
   // 0 1 0 0
   // 0 0 1 0
   // 0 0 0 1
   fVals[0] = 1.0; fVals[4] = 0.0; fVals[8 ] = 0.0; fVals[12] = 0.0;
   fVals[1] = 0.0; fVals[5] = 1.0; fVals[9 ] = 0.0; fVals[13] = 0.0;
   fVals[2] = 0.0; fVals[6] = 0.0; fVals[10] = 1.0; fVals[14] = 0.0;
   fVals[3] = 0.0; fVals[7] = 0.0; fVals[11] = 0.0; fVals[15] = 1.0;
}

void GLMatrix::SetTranslation(double x, double y, double z)
{
   // Set matrix translation components x,y,z:
   //
   // . . . x
   // . . . y
   // . . . z
   // . . . .
   //
   // The other components are NOT modified
   SetTranslation(GLVertex3(x,y,z));
}

void GLMatrix::SetTranslation(const GLVertex3 & translation)
{
   // Set matrix translation components x,y,z:
   //
   // . . . translation.X()
   // . . . translation.Y()
   // . . . translation.Z()
   // . . . .
   //
   // . = Exisiting component value - NOT modified
   fVals[12] = translation[0];
   fVals[13] = translation[1];
   fVals[14] = translation[2];
}

GLVector3 GLMatrix::GetTranslation() const
{
   // Return the translation component of matrix
   //
   // . . . X()
   // . . . Y()
   // . . . Z()
   // . . . .

   return GLVector3(fVals[12], fVals[13], fVals[14]);
}

void GLMatrix::Translate(const GLVector3 & vect)
{
   // Offset (shift) matrix translation components by 'vect'
   //
   // . . . . + vect.X()
   // . . . . + vect.Y()
   // . . . . + vect.Z()
   // . . . .
   //
   // . = Exisiting component value - NOT modified
   fVals[12] += vect[0];
   fVals[13] += vect[1];
   fVals[14] += vect[2];
}

void GLMatrix::MoveLF(int ai, double amount)
{
   // Translate in local frame.
   // i1, i2 are axes indices: 1 ~ x, 2 ~ y, 3 ~ z.

   const double *C = fVals + 4*--ai;
   fVals[12] += amount*C[0]; fVals[13] += amount*C[1]; fVals[14] += amount*C[2];
}

void GLMatrix::Scale(const GLVector3 & scale)
{
   // Set matrix axis scales to 'scale'. Note - this really sets
   // the overall (total) scaling for each axis - it does NOT
   // apply compounded scale on top of existing one
   GLVector3 currentScale = GetScale();

   // x
   if (currentScale[0] != 0.0) {
      fVals[0] *= scale[0]/currentScale[0];
      fVals[1] *= scale[0]/currentScale[0];
      fVals[2] *= scale[0]/currentScale[0];
   }
   // y
   if (currentScale[1] != 0.0) {
      fVals[4] *= scale[1]/currentScale[1];
      fVals[5] *= scale[1]/currentScale[1];
      fVals[6] *= scale[1]/currentScale[1];
   }
   // z
   if (currentScale[2] != 0.0) {
      fVals[8] *= scale[2]/currentScale[2];
      fVals[9] *= scale[2]/currentScale[2];
      fVals[10] *= scale[2]/currentScale[2];
   }
}

void GLMatrix::Rotate(const GLVertex3 & pivot, const GLVector3 & axis, double angle)
{
   // Update martix so resulting transform has been rotated about 'pivot'
   // (in parent frame), round vector 'axis', through 'angle' (radians)
   // Equivalent to glRotate function, but with addition of translation
   // and compounded on top of existing.
   GLVector3 nAxis = axis;
   nAxis.Normalise();
   double x = nAxis.X();
   double y = nAxis.Y();
   double z = nAxis.Z();
   double c = TMath::Cos(angle);
   double s = TMath::Sin(angle);

   // Calculate local rotation, with pre-translation to local pivot origin
   GLMatrix rotMat;
   rotMat[ 0] = x*x*(1-c) + c;   rotMat[ 4] = x*y*(1-c) - z*s; rotMat[ 8] = x*z*(1-c) + y*s; rotMat[12] = pivot[0];
   rotMat[ 1] = y*x*(1-c) + z*s; rotMat[ 5] = y*y*(1-c) + c;   rotMat[ 9] = y*z*(1-c) - x*s; rotMat[13] = pivot[1];
   rotMat[ 2] = x*z*(1-c) - y*s; rotMat[ 6] = y*z*(1-c) + x*s; rotMat[10] = z*z*(1-c) + c;   rotMat[14] = pivot[2];
   rotMat[ 3] = 0.0;             rotMat[ 7] = 0.0;             rotMat[11] = 0.0;             rotMat[15] = 1.0;
   GLMatrix localToWorld(-pivot);

   // TODO: Ugly - should use quaternions to avoid compound rounding errors and
   // triple multiplication
   *this = rotMat * localToWorld * (*this);
}

void GLMatrix::RotateLF(int i1, int i2, double amount)
{
   // Rotate in local frame. Does optimised version of MultRight.
   // i1, i2 are axes indices: 1 ~ x, 2 ~ y, 3 ~ z.

   if(i1 == i2) return;
   const double cos = TMath::Cos(amount), sin = TMath::Sin(amount);
   double  b1, b2;
   double* c = fVals;
   --i1 <<= 2; --i2 <<= 2; // column major
   for(int r=0; r<4; ++r, ++c) {
      b1 = cos*c[i1] + sin*c[i2];
      b2 = cos*c[i2] - sin*c[i1];
      c[i1] = b1; c[i2] = b2;
   }
}

void GLMatrix::RotatePF(int i1, int i2, double amount)
{
   // Rotate in parent frame. Does optimised version of MultLeft.

   if(i1 == i2) return;

   // Optimized version:
   const double cos = TMath::Cos(amount), sin = TMath::Sin(amount);
   double  b1, b2;
   double* C = fVals;
   --i1; --i2;
   for(int c=0; c<4; ++c, C+=4) {
      b1 = cos*C[i1] - sin*C[i2];
      b2 = cos*C[i2] + sin*C[i1];
      C[i1] = b1; C[i2] = b2;
   }
}

void GLMatrix::TransformVertex(GLVertex3 & vertex) const
{
   // Transform passed 'vertex' by this matrix - converts local frame to parent
   GLVertex3 orig = vertex;
   for (unsigned int i = 0; i < 3; i++) {
      vertex[i] = orig[0] * fVals[0+i] + orig[1] * fVals[4+i] +
                  orig[2] * fVals[8+i] + fVals[12+i];
   }
}

void GLMatrix::Transpose3x3()
{
   // Transpose the top left 3x3 matrix component along major diagonal
   // Supported as currently incompatability between TGeo and GL matrix
   // layouts for this 3x3 only. To be resolved.

   // TODO: Move this fix to the TBuffer3D filling side and remove
   //
   // 0  4  8 12
   // 1  5  9 13
   // 2  6 10 14
   // 3  7 11 15

   double temp = fVals[4];
   fVals[4] = fVals[1];
   fVals[1] = temp;
   temp = fVals[8];
   fVals[8] = fVals[2];
   fVals[2] = temp;
   temp = fVals[9];
   fVals[9] = fVals[6];
   fVals[6] = temp;
}

double GLMatrix::Invert()
{
   // Invert the matrix, returns determinant.
   // Copied from TMatrixFCramerInv.

   double* M = fVals;

   const double det2_12_01 = M[1]*M[6]  - M[5]*M[2];
   const double det2_12_02 = M[1]*M[10] - M[9]*M[2];
   const double det2_12_03 = M[1]*M[14] - M[13]*M[2];
   const double det2_12_13 = M[5]*M[14] - M[13]*M[6];
   const double det2_12_23 = M[9]*M[14] - M[13]*M[10];
   const double det2_12_12 = M[5]*M[10] - M[9]*M[6];
   const double det2_13_01 = M[1]*M[7]  - M[5]*M[3];
   const double det2_13_02 = M[1]*M[11] - M[9]*M[3];
   const double det2_13_03 = M[1]*M[15] - M[13]*M[3];
   const double det2_13_12 = M[5]*M[11] - M[9]*M[7];
   const double det2_13_13 = M[5]*M[15] - M[13]*M[7];
   const double det2_13_23 = M[9]*M[15] - M[13]*M[11];
   const double det2_23_01 = M[2]*M[7]  - M[6]*M[3];
   const double det2_23_02 = M[2]*M[11] - M[10]*M[3];
   const double det2_23_03 = M[2]*M[15] - M[14]*M[3];
   const double det2_23_12 = M[6]*M[11] - M[10]*M[7];
   const double det2_23_13 = M[6]*M[15] - M[14]*M[7];
   const double det2_23_23 = M[10]*M[15] - M[14]*M[11];


   const double det3_012_012 = M[0]*det2_12_12 - M[4]*det2_12_02 + M[8]*det2_12_01;
   const double det3_012_013 = M[0]*det2_12_13 - M[4]*det2_12_03 + M[12]*det2_12_01;
   const double det3_012_023 = M[0]*det2_12_23 - M[8]*det2_12_03 + M[12]*det2_12_02;
   const double det3_012_123 = M[4]*det2_12_23 - M[8]*det2_12_13 + M[12]*det2_12_12;
   const double det3_013_012 = M[0]*det2_13_12 - M[4]*det2_13_02 + M[8]*det2_13_01;
   const double det3_013_013 = M[0]*det2_13_13 - M[4]*det2_13_03 + M[12]*det2_13_01;
   const double det3_013_023 = M[0]*det2_13_23 - M[8]*det2_13_03 + M[12]*det2_13_02;
   const double det3_013_123 = M[4]*det2_13_23 - M[8]*det2_13_13 + M[12]*det2_13_12;
   const double det3_023_012 = M[0]*det2_23_12 - M[4]*det2_23_02 + M[8]*det2_23_01;
   const double det3_023_013 = M[0]*det2_23_13 - M[4]*det2_23_03 + M[12]*det2_23_01;
   const double det3_023_023 = M[0]*det2_23_23 - M[8]*det2_23_03 + M[12]*det2_23_02;
   const double det3_023_123 = M[4]*det2_23_23 - M[8]*det2_23_13 + M[12]*det2_23_12;
   const double det3_123_012 = M[1]*det2_23_12 - M[5]*det2_23_02 + M[9]*det2_23_01;
   const double det3_123_013 = M[1]*det2_23_13 - M[5]*det2_23_03 + M[13]*det2_23_01;
   const double det3_123_023 = M[1]*det2_23_23 - M[9]*det2_23_03 + M[13]*det2_23_02;
   const double det3_123_123 = M[5]*det2_23_23 - M[9]*det2_23_13 + M[13]*det2_23_12;

   const double det = M[0]*det3_123_123 - M[4]*det3_123_023 +
      M[8]*det3_123_013 - M[12]*det3_123_012;

   if(det == 0) {
    //Warning("GLMatrix::Invert", "matrix is singular.");
      return 0;
   }

   const double oneOverDet = 1.0/det;
   const double mn1OverDet = - oneOverDet;

   M[0]  = det3_123_123 * oneOverDet;
   M[4]  = det3_023_123 * mn1OverDet;
   M[8]  = det3_013_123 * oneOverDet;
   M[12] = det3_012_123 * mn1OverDet;

   M[1]  = det3_123_023 * mn1OverDet;
   M[5]  = det3_023_023 * oneOverDet;
   M[9]  = det3_013_023 * mn1OverDet;
   M[13] = det3_012_023 * oneOverDet;

   M[2]  = det3_123_013 * oneOverDet;
   M[6]  = det3_023_013 * mn1OverDet;
   M[10] = det3_013_013 * oneOverDet;
   M[14] = det3_012_013 * mn1OverDet;

   M[3]  = det3_123_012 * mn1OverDet;
   M[7]  = det3_023_012 * oneOverDet;
   M[11] = det3_013_012 * mn1OverDet;
   M[15] = det3_012_012 * oneOverDet;

   return det;
}

GLVector3 GLMatrix::Multiply(const GLVector3& v, double w) const
{
   // Multiply vector.
   const double* M = fVals;
   GLVector3 r;
   r.X() = M[0]*v[0] + M[4]*v[1] +  M[8]*v[2] + M[12]*w;
   r.Y() = M[1]*v[0] + M[5]*v[1] +  M[9]*v[2] + M[13]*w;
   r.Z() = M[2]*v[0] + M[6]*v[1] + M[10]*v[2] + M[14]*w;
   return r;
}

GLVector3 GLMatrix::Rotate(const GLVector3& v) const
{
   // Rotate vector. Translation is not applied.
   const double* M = fVals;
   GLVector3 r;
   r.X() = M[0]*v[0] + M[4]*v[1] +  M[8]*v[2];
   r.Y() = M[1]*v[0] + M[5]*v[1] +  M[9]*v[2];
   r.Z() = M[2]*v[0] + M[6]*v[1] + M[10]*v[2];
   return r;
}

void GLMatrix::MultiplyIP(GLVector3& v, double w) const
{
   // Multiply vector in-place.
   const double* M = fVals;
   double r[3] = { v[0], v[1], v[2] };
   v.X() = M[0]*r[0] + M[4]*r[1] +  M[8]*r[2] + M[12]*w;
   v.Y() = M[1]*r[0] + M[5]*r[1] +  M[9]*r[2] + M[13]*w;
   v.Z() = M[2]*r[0] + M[6]*r[1] + M[10]*r[2] + M[14]*w;
}

void GLMatrix::RotateIP(GLVector3& v) const
{
   // Rotate vector in-place. Translation is not applied.
   const double* M = fVals;
   double r[3] = { v[0], v[1], v[2] };
   v.X() = M[0]*r[0] + M[4]*r[1] +  M[8]*r[2];
   v.Y() = M[1]*r[0] + M[5]*r[1] +  M[9]*r[2];
   v.Z() = M[2]*r[0] + M[6]*r[1] + M[10]*r[2];
}

GLVector3 GLMatrix::GetScale() const
{
   // Get local axis scaling factors
   GLVector3 x(fVals[0], fVals[1], fVals[2]);
   GLVector3 y(fVals[4], fVals[5], fVals[6]);
   GLVector3 z(fVals[8], fVals[9], fVals[10]);
   return GLVector3(x.Mag(), y.Mag(), z.Mag());
}

void GLMatrix::Dump() const
{
   // Output 16 matrix components to std::cout
   //
   // 0  4   8  12
   // 1  5   9  13
   // 2  6  10  14
   // 3  7  11  15
   //
   std::cout.precision(6);
   for (int x = 0; x < 4; x++) {
      std::cout << "[ ";
      for (int y = 0; y < 4; y++) {
         std::cout << fVals[y*4 + x] << " ";
      }
      std::cout << "]" << std::endl;
   }
}
