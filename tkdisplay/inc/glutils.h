// $Id: glutils.h,v 1.1 2009/06/13 21:40:47 shaino Exp $
//
// glutils : a set of classes to manage GLCamera
//           imported from ROOT:TGLUtils and arranged by SH
//
#ifndef GLUTILS_H
#define GLUTILS_H

#include <cmath>
#include <vector>

class GLVector3;

class GLVertex3
{
protected:
   // Fields
   bool ValidIndex(unsigned int index) const { return (index < 3); }
   double fVals[3];

public:
   GLVertex3();
   GLVertex3(double x, double y, double z);
   GLVertex3(double* v);
   GLVertex3(const GLVertex3 & other);
   virtual ~GLVertex3();

   bool       operator == (const GLVertex3 & rhs) const;
   GLVertex3 & operator =  (const GLVertex3 & rhs);
   GLVertex3 & operator *= (double f);
   GLVertex3   operator -  () const;
   const GLVertex3 & operator -= (const GLVector3 & val);
   const GLVertex3 & operator += (const GLVector3 & val);

   // Manipulators
   void Fill(double val);
   void Set(double x, double y, double z);
   void Set(const GLVertex3 & other);
   void Shift(GLVector3 & shift);
   void Shift(double xDelta, double yDelta, double zDelta);
   void Negate();

   void Minimum(const GLVertex3 & other);
   void Maximum(const GLVertex3 & other);

   // Accessors
         double & operator [] (int index);
   const double & operator [] (int index) const;
   double   X() const { return fVals[0]; }
   double & X()       { return fVals[0]; }
   double   Y() const { return fVals[1]; }
   double & Y()       { return fVals[1]; }
   double   Z() const { return fVals[2]; }
   double & Z()       { return fVals[2]; }

   const double * CArr() const { return fVals; }
   double *       Arr()        { return fVals; }

   void Dump() const;
};


inline GLVertex3 operator*(double f, const GLVertex3& v)
{
   return GLVertex3(f*v.X(), f*v.Y(), f*v.Z());
}

inline void GLVertex3::Negate()
{
   fVals[0] = -fVals[0];
   fVals[1] = -fVals[1];
   fVals[2] = -fVals[2];
}

inline bool GLVertex3::operator == (const GLVertex3 & rhs) const
{
   return (fVals[0] == rhs.fVals[0] && fVals[1] == rhs.fVals[1] && fVals[2] == rhs.fVals[2]);
}

inline GLVertex3 & GLVertex3::operator = (const GLVertex3 & rhs)
{
   // Check for self-assignment
   if (this != &rhs) {
      Set(rhs);
   }
   return *this;
}

inline GLVertex3 GLVertex3::operator - () const
{
   return GLVertex3(-fVals[0], -fVals[1], -fVals[2]);
}

inline GLVertex3& GLVertex3::operator *= (double f)
{
   fVals[0] *= f;
   fVals[1] *= f;
   fVals[2] *= f;
   return *this;
}

inline double & GLVertex3::operator [] (int index)
{
  return fVals[index];
}

inline const double& GLVertex3::operator [] (int index) const
{
  return fVals[index];
}

inline void GLVertex3::Fill(double val)
{
   Set(val,val,val);
}

inline void GLVertex3::Set(double x, double y, double z)
{
   fVals[0]=x;
   fVals[1]=y;
   fVals[2]=z;
}

inline void GLVertex3::Set(const GLVertex3 & other)
{
   fVals[0]=other.fVals[0];
   fVals[1]=other.fVals[1];
   fVals[2]=other.fVals[2];
}


class GLVector3 : public GLVertex3
{
public:
   GLVector3();
   GLVector3(double x, double y, double z);
   GLVector3(const double *src);
   GLVector3(const GLVector3 & other);
   virtual ~GLVector3();

   GLVector3& operator = (const GLVertex3& v)
   { fVals[0] = v[0]; fVals[1] = v[1]; fVals[2] = v[2]; return *this; }

   GLVector3 & operator /= (double val);
   GLVector3   operator -  () const;

   double Mag() const;
   void     Normalise();
};


inline const GLVertex3 & GLVertex3::operator -= (const GLVector3 & vec)
{
   fVals[0] -= vec[0]; fVals[1] -= vec[1]; fVals[2] -= vec[2];
   return *this;
}

inline const GLVertex3 & GLVertex3::operator += (const GLVector3 & vec)
{
   fVals[0] += vec[0]; fVals[1] += vec[1]; fVals[2] += vec[2];
   return *this;
}

inline GLVector3 & GLVector3::operator /= (double val)
{
   fVals[0] /= val;
   fVals[1] /= val;
   fVals[2] /= val;
   return *this;
}

inline GLVector3 GLVector3::operator - () const
{
   return GLVector3(-fVals[0], -fVals[1], -fVals[2]);
}

inline double GLVector3::Mag() const
{
   return sqrt(fVals[0]*fVals[0] + fVals[1]*fVals[1] + fVals[2]*fVals[2]);
}

inline void GLVector3::Normalise()
{
   double mag = Mag();
   if ( mag == 0.0 ) return;
   fVals[0] /= mag;
   fVals[1] /= mag;
   fVals[2] /= mag;
}

inline double Dot(const GLVector3 & v1, const GLVector3 & v2)
{
   return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

inline GLVector3 Cross(const GLVector3 & v1, const GLVector3 & v2)
{
    return GLVector3(v1[1]*v2[2] - v2[1]*v1[2],
                      v1[2]*v2[0] - v2[2]*v1[0],
                      v1[0]*v2[1] - v2[0]*v1[1]);
}

inline const GLVector3 operator / (const GLVector3 & vec, double val)
{
   return GLVector3(vec[0] / val, vec[1] / val, vec[2] / val);
}

inline const GLVector3 operator * (const GLVector3 & vec, double val)
{
   return GLVector3(vec[0] * val, vec[1] * val, vec[2] * val);
}

inline GLVertex3 operator + (const GLVertex3 & vertex1, const GLVector3 & vertex2)
{
   return GLVertex3(vertex1[0] + vertex2[0], vertex1[1] + vertex2[1], vertex1[2] + vertex2[2]);
}

inline GLVector3 operator - (const GLVertex3 & vertex1, const GLVertex3 & vertex2)
{
   return GLVector3(vertex1[0] - vertex2[0], vertex1[1] - vertex2[1], vertex1[2] - vertex2[2]);
}

inline GLVector3 operator + (const GLVector3 & vector1, const GLVector3 & vector2)
{
   return GLVector3(vector1[0] + vector2[0], vector1[1] + vector2[1], vector1[2] + vector2[2]);
}

inline GLVector3 operator - (const GLVector3 & vector1, const GLVector3 & vector2)
{
   return GLVector3(vector1[0] - vector2[0], vector1[1] - vector2[1], vector1[2] - vector2[2]);
}

inline double operator * (const GLVector3 & a, const GLVector3 & b)
{
   return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}


class GLLine3
{
private:
   // Fields
   GLVertex3 fVertex; //! Start vertex of line
   GLVector3 fVector; //! Vector of line from fVertex

public:
   GLLine3(const GLVertex3 & start, const GLVertex3 & end);
   GLLine3(const GLVertex3 & start, const GLVector3 & vector);
   virtual ~GLLine3();

   void Set(const GLVertex3 & start, const GLVertex3 & end);
   void Set(const GLVertex3 & start, const GLVector3 & vector);

   // Bitwise copy constructor and = operator are fine

   // Accessors
   const GLVertex3 & Start()  const { return fVertex; }
   const GLVertex3   End()    const { return fVertex + fVector; }
   const GLVector3 & Vector() const { return fVector; }

   // Debug
   void Draw() const;
};


class GLPlane
{
private:
   // Fields
   double fVals[4];

   // Methods
   void Normalise();

public:
   GLPlane();
   GLPlane(const GLPlane & other);
   GLPlane(double a, double b, double c, double d);
   GLPlane(double eq[4]);
   GLPlane(const GLVector3 & norm, const GLVertex3 & point);
   GLPlane(const GLVertex3 & p1, const GLVertex3 & p2, const GLVertex3 & p3);
   virtual ~GLPlane(); // ClassDef introduces virtual fns

   // Manipulators
   void Set(const GLPlane & other);
   void Set(double a, double b, double c, double d);
   void Set(double eq[4]);
   void Set(const GLVector3 & norm, const GLVertex3 & point);
   void Set(const GLVertex3 & p1, const GLVertex3 & p2, const GLVertex3 & p3);
   void Negate();

   // Accessors
   double A() const { return fVals[0]; }
   double B() const { return fVals[1]; }
   double C() const { return fVals[2]; }
   double D() const { return fVals[3]; }

   GLVector3 Norm() const { return GLVector3( fVals[0], fVals[1], fVals[2]); }
   double DistanceTo(const GLVertex3 & vertex) const;
   GLVertex3 NearestOn(const GLVertex3 & point) const;

   // Internal data accessors - for GL API
   const double * CArr() const { return fVals; }
   double * Arr() { return fVals; }

   void Dump() const;
};


inline void GLPlane::Set(const GLPlane & other)
{
   fVals[0] = other.fVals[0];
   fVals[1] = other.fVals[1];
   fVals[2] = other.fVals[2];
   fVals[3] = other.fVals[3];
}

inline void GLPlane::Set(double a, double b, double c, double d)
{
   fVals[0] = a;
   fVals[1] = b;
   fVals[2] = c;
   fVals[3] = d;
   Normalise();
}

inline void GLPlane::Set(double eq[4])
{
   fVals[0] = eq[0];
   fVals[1] = eq[1];
   fVals[2] = eq[2];
   fVals[3] = eq[3];
   Normalise();
}

inline void GLPlane::Set(const GLVector3 & norm, const GLVertex3 & point)
{
   // Set plane from a normal vector and in-plane point pair
   fVals[0] = norm[0];
   fVals[1] = norm[1];
   fVals[2] = norm[2];
   fVals[3] = -(fVals[0]*point[0] + fVals[1]*point[1] + fVals[2]*point[2]);
   Normalise();
}

inline void GLPlane::Set(const GLVertex3 & p1, const GLVertex3 & p2, const GLVertex3 & p3)
{
   GLVector3 norm = Cross(p2 - p1, p3 - p1);
   Set(norm, p2);
}

inline void GLPlane::Negate()
{
   fVals[0] = -fVals[0];
   fVals[1] = -fVals[1];
   fVals[2] = -fVals[2];
   fVals[3] = -fVals[3];
}

inline void GLPlane::Normalise()
{
   double mag = sqrt( fVals[0]*fVals[0] + fVals[1]*fVals[1] + fVals[2]*fVals[2] );

   if ( mag == 0.0 ) return;

   fVals[0] /= mag;
   fVals[1] /= mag;
   fVals[2] /= mag;
   fVals[3] /= mag;
}

inline double GLPlane::DistanceTo(const GLVertex3 & vertex) const
{
   return (fVals[0]*vertex[0] + fVals[1]*vertex[1] + fVals[2]*vertex[2] + fVals[3]);
}

inline GLVertex3 GLPlane::NearestOn(const GLVertex3 & point) const
{
   GLVector3 o = Norm() * (Dot(Norm(), GLVector3(point[0], point[1], point[2])) + D() / Dot(Norm(), Norm()));
   GLVertex3 v = point - o;
   return v;
}

// Some free functions for planes
std::pair<bool, GLLine3>   Intersection(const GLPlane & p1, const GLPlane & p2);
std::pair<bool, GLVertex3> Intersection(const GLPlane & p1, const GLPlane & p2, const GLPlane & p3);
std::pair<bool, GLVertex3> Intersection(const GLPlane & plane, const GLLine3 & line, bool extend);


class GLMatrix
{
private:
   // Fields
   double fVals[16]; // Column MAJOR as per OGL

   // Methods
   bool ValidIndex(unsigned int index) const { return (index < 16); }

public:
   GLMatrix();
   GLMatrix(double x, double y, double z);
   GLMatrix(const GLVertex3 & translation);
   GLMatrix(const GLVertex3 & origin, const GLVector3 & zAxis, const GLVector3 & xAxis);
   GLMatrix(const GLVertex3 & origin, const GLVector3 & zAxis);
   GLMatrix(const double vals[16]);
   GLMatrix(const GLMatrix & other);
   virtual ~GLMatrix(); // ClassDef introduces virtual fns

   // Operators
   GLMatrix & operator =(const GLMatrix & rhs);
   double  & operator [] (int index);
   double    operator [] (int index) const;

   void MultRight(const GLMatrix & rhs);
   void MultLeft (const GLMatrix & lhs);
   GLMatrix & operator*=(const GLMatrix & rhs) { MultRight(rhs); return *this; }

   // Manipulators
   void Set(const GLVertex3 & origin, const GLVector3 & zAxis, const GLVector3 & xAxis = 0);
   void Set(const double vals[16]);
   void SetIdentity();

   void SetTranslation(double x, double y, double z);
   void SetTranslation(const GLVertex3 & translation);

   void Translate(const GLVector3 & vect);
   void MoveLF(int ai, double amount);
   void Scale(const GLVector3 & scale);
   void Rotate(const GLVertex3 & pivot, const GLVector3 & axis, double angle);
   void RotateLF(int i1, int i2, double amount);
   void RotatePF(int i1, int i2, double amount);
   void TransformVertex(GLVertex3 & vertex) const;
   void Transpose3x3();
   double Invert();

   // Accesors
   GLVector3  GetTranslation() const;
   GLVector3  GetScale() const;

   void SetBaseVec(int b, double x, double y, double z);
   void SetBaseVec(int b, const GLVector3& v);
   void SetBaseVec(int b, double* x);

   GLVector3 GetBaseVec(int b) const;
   void       GetBaseVec(int b, GLVector3& v) const;
   void       GetBaseVec(int b, double* x) const;

   GLVector3 Multiply(const GLVector3& v, double w=1) const;
   GLVector3 Rotate(const GLVector3& v) const;
   void       MultiplyIP(GLVector3& v, double w=1) const;
   void       RotateIP(GLVector3& v) const;

   // Internal data accessors - for GL API
   const double * CArr() const { return fVals; }
   double * Arr() { return fVals; }

   void Dump() const;
};


inline GLMatrix & GLMatrix::operator =(const GLMatrix & rhs)
{
   // Check for self-assignment
   if (this != &rhs) {
      Set(rhs.fVals);
   }
   return *this;
}

inline double & GLMatrix::operator [] (int index)
{
   /*if (!ValidIndex(index)) {
      assert(kFALSE);
      return fVals[0];
   } else {*/
      return fVals[index];
   //}
}

inline double GLMatrix::operator [] (int index) const
{
   /*if (!ValidIndex(index)) {
      assert(kFALSE);
      return fVals[0];
   } else {*/
      return fVals[index];
   //}
}

inline GLMatrix operator * (const GLMatrix & lhs, const GLMatrix & rhs)
{
   GLMatrix res;

   res[ 0] = rhs[ 0] * lhs[ 0] + rhs[ 1] * lhs[ 4] + rhs[ 2] * lhs[ 8] + rhs[ 3] * lhs[12];
   res[ 1] = rhs[ 0] * lhs[ 1] + rhs[ 1] * lhs[ 5] + rhs[ 2] * lhs[ 9] + rhs[ 3] * lhs[13];
   res[ 2] = rhs[ 0] * lhs[ 2] + rhs[ 1] * lhs[ 6] + rhs[ 2] * lhs[10] + rhs[ 3] * lhs[14];
   res[ 3] = rhs[ 0] * lhs[ 3] + rhs[ 1] * lhs[ 7] + rhs[ 2] * lhs[11] + rhs[ 3] * lhs[15];

   res[ 4] = rhs[ 4] * lhs[ 0] + rhs[ 5] * lhs[ 4] + rhs[ 6] * lhs[ 8] + rhs[ 7] * lhs[12];
   res[ 5] = rhs[ 4] * lhs[ 1] + rhs[ 5] * lhs[ 5] + rhs[ 6] * lhs[ 9] + rhs[ 7] * lhs[13];
   res[ 6] = rhs[ 4] * lhs[ 2] + rhs[ 5] * lhs[ 6] + rhs[ 6] * lhs[10] + rhs[ 7] * lhs[14];
   res[ 7] = rhs[ 4] * lhs[ 3] + rhs[ 5] * lhs[ 7] + rhs[ 6] * lhs[11] + rhs[ 7] * lhs[15];

   res[ 8] = rhs[ 8] * lhs[ 0] + rhs[ 9] * lhs[ 4] + rhs[10] * lhs[ 8] + rhs[11] * lhs[12];
   res[ 9] = rhs[ 8] * lhs[ 1] + rhs[ 9] * lhs[ 5] + rhs[10] * lhs[ 9] + rhs[11] * lhs[13];
   res[10] = rhs[ 8] * lhs[ 2] + rhs[ 9] * lhs[ 6] + rhs[10] * lhs[10] + rhs[11] * lhs[14];
   res[11] = rhs[ 8] * lhs[ 3] + rhs[ 9] * lhs[ 7] + rhs[10] * lhs[11] + rhs[11] * lhs[15];

   res[12] = rhs[12] * lhs[ 0] + rhs[13] * lhs[ 4] + rhs[14] * lhs[ 8] + rhs[15] * lhs[12];
   res[13] = rhs[12] * lhs[ 1] + rhs[13] * lhs[ 5] + rhs[14] * lhs[ 9] + rhs[15] * lhs[13];
   res[14] = rhs[12] * lhs[ 2] + rhs[13] * lhs[ 6] + rhs[14] * lhs[10] + rhs[15] * lhs[14];
   res[15] = rhs[12] * lhs[ 3] + rhs[13] * lhs[ 7] + rhs[14] * lhs[11] + rhs[15] * lhs[15];

   return res;
}

inline void GLMatrix::SetBaseVec(int b, double x, double y, double z)
{
   double* C = fVals + 4*--b;
   C[0] = x; C[1] = y; C[2] = z;
}

inline void GLMatrix::SetBaseVec(int b, const GLVector3& v)
{
   double* C = fVals + 4*--b;
   C[0] = v[0]; C[1] = v[1]; C[2] = v[2];
}

inline void GLMatrix::SetBaseVec(int b, double* x)
{
   double* C = fVals + 4*--b;
   C[0] = x[0]; C[1] = x[1]; C[2] = x[2];
}

inline GLVector3 GLMatrix::GetBaseVec(int b) const
{
   return GLVector3(&fVals[4*--b]);
}

inline void GLMatrix::GetBaseVec(int b, GLVector3& v) const
{
   const double* C = fVals + 4*--b;
   v[0] = C[0]; v[1] = C[1]; v[2] = C[2];
}

inline void GLMatrix::GetBaseVec(int b, double* x) const
{
   const double* C = fVals + 4*--b;
   x[0] = C[0], x[1] = C[1], x[2] = C[2];
}

#endif
