//  $Id: MagField.h,v 1.17 2011/08/31 18:47:33 shaino Exp $
#ifndef __MagField__
#define __MagField__
#include "typedefs.h"
#include <cstdlib>


#ifndef _PGTRACK_


// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

PROTOCCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV)
#define GUFLD(A1,A2)  CCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV,A1,A2)

PROTOCCALLSFSUB2(TKFLD,tkfld,FLOATV,FLOATVV)
#define TKFLD(A1,A2)  CCALLSFSUB2(TKFLD,tkfld,FLOATV,FLOATVV,A1,A2)

#define MAGSFFKEY COMMON_BLOCK(MAGSFFKEY,magsffkey)
COMMON_BLOCK_DEF(MAGSFFKEY_DEF,MAGSFFKEY);

#else

class MAGSFFKEY_DEF {
public:
integer magstat; // status: 0/1-> off/on
geant fscale;    // nom.field reduction
geant ecutge;    // e/g energy cut for tracking in magnet materials
geant r0[3];     // shift & rotation of mag field map w/r nom
geant pitch;
geant yaw;
geant roll;
integer rphi;    // use xyz (0) or rphiz (1) grid
integer begin;    // begin time
integer end;    //end time

void init();
MAGSFFKEY_DEF() { init(); }
};

extern MAGSFFKEY_DEF MAGSFFKEY;

#define GUFLD(A1,A2)  MagField::GetPtr()->GuFld(A1,A2)
#define TKFLD(A1,A2)  MagField::GetPtr()->TkFld(A1,A2)


#include "point.h"

//////////////////////////////////////////////////////////////////////////
///
///
///\class MagField
///\brief A class to manage magnetic field
///\ingroup tkrec
///
///\date  2007/12/12 SH  First test version
///\date  2007/12/16 SH  First import (as TkMfield)
///\date  2007/12/18 SH  First stable vertion after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2008/01/20 SH  Imported to tkdev
///\date  2008/11/17 PZ  Many improvement and import to GBATCH
///$Date: 2011/08/31 18:47:33 $
///
///$Revision: 1.17 $
///
//////////////////////////////////////////////////////////////////////////

class magserv {
public:
  // map type;  type=1 just one octant; type !=1 standard
  int _type;
  float*  x;
  float*  y;
  float*  z;

  float* bx;
  float* by;
  float* bz;


  float* bdx;
  float* bdy;
  float* bdz;


  // the real map size
  int _nx;
  int _ny;
  int _nz;
  int _nx2;
  int _ny2;
  int _nz2;
  inline int getsign(int i,int j,int k,int oo, int ll=-1);

public:
  inline int getindex(int i,int j,int k);

  int nx() { return (_type==1)?_nx2:_nx;}
  int ny() { return (_type==1)?_ny2:_ny;}
  int nz() { return (_type==1)?_nz2:_nz;}

  double  _dx;     ///< Element size in X (cm)
  double  _dy;     ///< Element size in Y (cm)
  double  _dz;     ///< Element size in Z (cm)


  magserv(int nx, int ny, int nz,int type);
  ~magserv();


  float _x(int i) {int sigx=((1-_nx+i)>=0)?1:-1;  return (_type==1)?sigx*x[abs(1-_nx+i)]:x[i];}
  float _y(int j) {int sigx=((1-_ny+j)>=0)?1:-1;  return (_type==1)?sigx*y[abs(1-_ny+j)]:y[j];}
  float _z(int k) {int sigx=((1-_nz+k)>=0)?1:-1;  return (_type==1)?sigx*z[abs(1-_nz+k)]:z[k];}

  float _x0() { return (_type==1)?-x[_nx-1]:x[0];}
  float _y0() { return (_type==1)?-y[_ny-1]:y[0];}
  float _z0() { return (_type==1)?-z[_nz-1]:z[0];}

  
  
  float _bx(int i, int j, int k){ return getsign(i,j,k,0)*bx[getindex(i,j,k)];}
  float _by(int i, int j, int k){ return getsign(i,j,k,1)*by[getindex(i,j,k)];}
  float _bz(int i, int j, int k){ return getsign(i,j,k,2)*bz[getindex(i,j,k)];}

  inline void _bb(int i, int j, int k, 
		  float &bbx, float &bby, float &bbz);

  float _bdx(int l,int i, int j, int k){ return getsign(i,j,k,0,l)*bdx[l*_nx*_ny*_nz+getindex(i,j,k)];}
  float _bdy(int l,int i, int j, int k){ return getsign(i,j,k,1,l)*bdy[l*_nx*_ny*_nz+getindex(i,j,k)];}
  float _bdz(int l,int i, int j, int k){ return getsign(i,j,k,2,l)*bdz[l*_nx*_ny*_nz+getindex(i,j,k)];}

   /// Read field map file
   int Read(const char* fname,int skip);
};

void magserv::_bb(int i, int j, int k, 
		  float &bbx, float &bby, float &bbz)
{
  int idx = getindex(i,j,k);
  bbx = getsign(i,j,k,0)*bx[idx];
  bby = getsign(i,j,k,1)*by[idx];
  bbz = getsign(i,j,k,2)*bz[idx];
}

#define SIGN(A) ((A>=0)?1:-1)

int magserv::getindex(int i,int j,int k){
  if (_type==1){
    int i2 = 1 - _nx + i;
    int j2 = 1 - _ny + j;
    int k2 = 1 - _nz + k;
    return abs(k2)*_nx*_ny+ abs(j2)*_nx + abs(i2);
  }
  else{
    return k*_nx*_ny+ j*_nx + i;
    
  }
}
int magserv::getsign(int i,int j,int k,int oo,int ll){
  if (_type==1){
    int ijk[3];
    ijk[0] = 1 - _nx + i;
    ijk[1] = 1 - _ny + j;
    ijk[2] = 1 - _nz + k;
    if(ll>-1)
      return SIGN(ijk[0])*SIGN(ijk[oo])*SIGN(ll);
    else
      return SIGN(ijk[0])*SIGN(ijk[oo]);
  }
  else
    return 1;
}

class MagField {
public:
  enum { _header_size = 15 };
protected:
  char mfile[120];
  integer iniok;
  integer isec [2];
  integer imin [2];
  integer ihour[2];
  integer iday [2];
  integer imon [2];
  integer iyear[2];
  integer na   [3];

  magserv* mm;

  static MagField* _ptr;  ///< Self pointer
  double fscale;
  MagField(void);

public:
  ~MagField();

  /// Read field map file
  int Read(const char *fname);

  /// Get field value (xyz coodinate)
  void GuFld(float *x, float *b);

  /// Get field value (Rphiz coodinate)
  //  void GuFldRphi(float *x, float *b);

  AMSPoint GuFld(AMSPoint x){
    float xx[3] = { 0, 0, 0 };
    float b [3] = { 0, 0, 0 };
    xx[0] = x[0]; xx[1] = x[1]; xx[2] = x[2];
    GuFld(xx, b);
    return AMSPoint(b[0], b[1], b[2]);
  }
  /// Get field derivative
  void TkFld(float *x, float hxy[][3]);

  /// Get self pointer
  static MagField *GetPtr(void);

  magserv* GetMap() {return mm;}
  int* GetPointerForDB(){
    return  isec;
  }
  void SetInitOk(int pp){ iniok=pp;}

  void SetMfile(char* pp){ sprintf(mfile,"%s",pp);}
  int GetSizeForDB(){
    if(!mm) return 0;
    int siz= sizeof(isec[0])*15+sizeof(mm->_x(0))*
      ((mm->nx()*mm->ny()*mm->nz())*12+2*(mm->nx()+mm->ny()+mm->nz()));
    return siz;
  }

  void   SetMagstat(int  stat) { MAGSFFKEY.magstat = stat; }
  void   SetScale(double scal) { fscale = scal; }
  int    GetMagstat() const { return MAGSFFKEY.magstat; }
  double GetScale  () const { return fscale; }

  void Print();

protected:
  /// Interpolation (for xyz coordinate)
  // Return 1 if ok, return 0 if outside the grid
  int _Fint(double x, double y, double z, int index[][3], double *weight);

  /// Interpolation (for rphiz coordinate)
  void _FintRphi(double r, double ph, double z, int *index, double *weight);

  /// Get index of the array from each index number at x,y,z
  int _Index(int i, int j, int k) const;

  /// Get index of the array from position (x,y,z)
  int _GetIndex(double x, double y, double z) const;
};

bool MagFieldOn();

#endif


#endif
