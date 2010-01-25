//  $Id: MagField.h,v 1.8 2010/01/25 15:09:29 shaino Exp $
#ifndef __MagField__
#define __MagField__

#include "typedefs.h"



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
///$Date: 2010/01/25 15:09:29 $
///
///$Revision: 1.8 $
///
//////////////////////////////////////////////////////////////////////////

class MagField {
public:
  enum { _header_size = 15 };
  enum { _nx = 41, _ny  = 41, _nz  = 41 };
  enum { _nr =  9, _nph = 73, _nzr = 37 };

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

  geant  _x[_nx];
  geant  _y[_ny];
  geant  _z[_nz];

  geant _bx[_nz][_ny][_nx];
  geant _by[_nz][_ny][_nx];
  geant _bz[_nz][_ny][_nx];

  geant _xyz[_nx+_ny+_nz];

  geant _bdx[2][_nz][_ny][_nx];
  geant _bdy[2][_nz][_ny][_nx];
  geant _bdz[2][_nz][_ny][_nx];

  geant _bxc[_nz][_ny][_nx];
  geant _byc[_nz][_ny][_nx];
  geant _bzc[_nz][_ny][_nx];

  double  _dx;     ///< Element size in X (cm)
  double  _dy;     ///< Element size in Y (cm)
  double  _dz;     ///< Element size in Z (cm)

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
  void GuFldRphi(float *x, float *b);

  AMSPoint GuFld(AMSPoint x){
    float xx[3] = { 0, 0, 0 };
    float b [3] = { 0, 0, 0 };
    xx[0] = x[0]; xx[1] = x[1]; xx[2] = x[2];
    GuFld(xx, b);
    return AMSPoint(b[0], b[1], b[2]);
  }
  /// Get field derivative
  void TkFld(float *x, float hxy[][3]);

  void AddBcor(AMSPoint x, AMSPoint db);

  /// Get self pointer
  static MagField *GetPtr(void) {
    return (_ptr) ? _ptr : new MagField;
  }
  
  int* GetPointerForDB(){
    return  isec;
  }
  void SetInitOk(int pp){ iniok=pp;}

  void SetMfile(char* pp){ sprintf(mfile,"%s",pp);}
  int GetSizeForDB(){
    int siz= sizeof(isec[0])*15+sizeof(_x[0])*
      ((_nx*_ny*_nz)*12+2*(_nx+_ny+_nz));
    return siz;
  }

  void   SetMagstat(int  stat) { MAGSFFKEY.magstat = stat; }
  void   SetScale(double scal) { fscale = scal; }
  int    GetMagstat() const { return MAGSFFKEY.magstat; }
  double GetScale  () const { return fscale; }

  void Print();

protected:
  /// Interpolation (for xyz coordinate)
  void _Fint(double x, double y, double z, int *index, double *weight);

  /// Interpolation (for rphiz coordinate)
  void _FintRphi(double r, double ph, double z, int *index, double *weight);

  /// Get index of the array from each index number at x,y,z
  int _Index(int i, int j, int k) const;

  /// Get index of the array from position (x,y,z)
  int _GetIndex(double x, double y, double z) const;
};

// class TKFIELD_DEF{
// public:
//   integer mfile[40];
//   integer iniok;
//   integer isec[2];
//   integer imin[2];
//   integer ihour[2];
//   integer iday[2];
//   integer imon[2];
//   integer iyear[2];
//   void init();
// };

bool MagFieldOn();

#endif


#endif
