// $Id: MagField.C,v 1.6 2010/01/15 10:46:29 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  MagField.C
///\brief Source file of MagField class
///
///\date  2007/12/12 SH  First test version
///\date  2007/12/16 SH  First import (as TkMfield)
///\date  2007/12/18 SH  First stable vertion after a refinement
///\date  2007/12/20 SH  All the parameters are defined in double
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/11/17 PZ  Many improvement and import to GBATCH
///$Date: 2010/01/15 10:46:29 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cmath>

#include "MagField.h"


MAGSFFKEY_DEF MAGSFFKEY;
bool MagFieldOn(){return MAGSFFKEY.magstat>0;}


#ifdef __ROOTSHAREDLIBRARY__
//PZMAG
void uctoh (char* MS,int* MT,int npw, int NCHP);


// void MAGSFFKEY_DEF::init(){

//   magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
//   fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
//   ecutge=0.001; //(3) e/g ener.cut for tracking in magnet materials(Gev) 

//   BTempCorrection=0;
//   BZCorr=1; 
//   rphi=0;
//   return;  

// }


void TKFIELD_DEF::init(){


  iniok=1;

  char mapfilename[160]="fld97int.txt";
  uctoh(mapfilename,mfile,4,160);
  //  memset(mfile,40,sizeof(mfile[0]));
  isec[0]=0;
  isec[1]=0;
  imin[0]=0;
  imin[1]=0;
  ihour[0]=0;
  ihour[1]=0;
  imon[0]=0;
  imon[1]=0;
  iyear[0]=0;
  iyear[1]=0;
  
  return;  

}

TKFIELD_DEF TKFIELD;


MagField *MagField::_ptr = 0;

MagField::MagField(void)
{
  iniok=0;
  _dx=_dy=_dz=0;
  for(int ii=0;ii<2;ii++)
    isec[ii]=imin[ii]=ihour[ii]=iday[ii]=imon[ii]=iyear[ii]=0;
  na[0]=na[1]=na[2]=0;
  for (int ix=0;ix<_nx;ix++)
    for (int iy=0;iy<_ny;iy++)
      for (int iz=0;iz<_nz;iz++){
	
	_x[ix]=0;
	_y[iy]=0;
	_z[iz]=0;
	  _bx[iz][iy][ix]=0;
	  _by[iz][iy][ix]=0;
	  _bz[iz][iy][ix]=0;
	  _xyz[ix+iy+iz]=0;
	  _bdx[0][iz][iy][ix]=0;
	  _bdy[0][iz][iy][ix]=0;
	  _bdz[0][iz][iy][ix]=0;
	  _bdx[1][iz][iy][ix]=0;
	  _bdy[1][iz][iy][ix]=0;
	  _bdz[1][iz][iy][ix]=0;
	  _bxc[iz][iy][ix]=0;
	  _byc[iz][iy][ix]=0;
	  _bzc[iz][iy][ix]=0;
	}
  _ptr = this;
}

MagField::~MagField()
{
  _ptr=0;
}

int MagField::Read(const char *fname)
{
  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "MagField::ReadAMS: File not found: " << fname << std::endl;
    return -1;
  }

  int stot = 0;

  char *cheader = (char *)isec;
  fin.read(cheader, 4*_header_size);
  stot += 4*_header_size;

  MAGSFFKEY.rphi = 0;
  if (na[0] == _nx && na[1] == _ny  && na[2] == _nz ) {
    MAGSFFKEY.rphi = 0;
    std::cout << "MagField::Read: X-Y-Z map" << std::endl;
  }
  else if (na[0] == _nr && na[1] == _nph && na[2] == _nzr) {
    MAGSFFKEY.rphi = 1;
    std::cout << "MagField::Read: R-Phi-Z map" << std::endl;
  }
  else {
    std::cerr << "Error in MagField::Read format error: "
	      << fname << " " 
	      << na[0] << " " << na[1] << " " << na[2] << std::endl;
    MAGSFFKEY.rphi = 0;
    MAGSFFKEY.magstat = 0;
    return -1;
  }

  int _nb = _nx*_ny*_nz;
  int  nn = _nx+_ny+_nz;

  std::cout << "MagField::Read: Reading field map... ";
  std::cout.flush();

  char *cdata = (char *)_x;
  int dsize = _nx+_ny+_nz+_nb*3+nn+_nb*2*3;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;
  std::cout << stot << std::endl;

  _dx = _x[1]-_x[0];
  _dy = _y[1]-_y[0];
  _dz = _z[1]-_z[0];

  geant *_rad = &_x[0];
  geant *_phi = &_x[_nr];

  bool check = true;
  if (MAGSFFKEY.rphi) {
    for (int i = 0; i < _nr -1; i++) if (_rad[i] >= _rad[i+1]) check = false;
    for (int i = 0; i < _nph-1; i++) if (_phi[i] >= _phi[i+1]) check = false;
    for (int i = 0; i < _nzr-1; i++) if (_z  [i] >= _z  [i+1]) check = false;
  }
  else {
    for (int i = 0; i < _nx-1; i++) if (_x[i] >= _x[i+1]) check = false;
    for (int i = 0; i < _ny-1; i++) if (_y[i] >= _y[i+1]) check = false;
    for (int i = 0; i < _nz-1; i++) if (_z[i] >= _z[i+1]) check = false;
    if (std::fabs(_x[_nx-1]-_x[0]-(_nx-1)*_dx) > 1e-3 ||
	std::fabs(_y[_ny-1]-_y[0]-(_ny-1)*_dy) > 1e-3 ||
	std::fabs(_z[_nz-1]-_z[0]-(_nz-1)*_dz) > 1e-3) check = false;
  }

  if (!check) {
    std::cerr << "Error in MagField::Read format check failed: "
	      << fname << " rphi= " << MAGSFFKEY.rphi << std::endl;
  }

  return _nb;
}

void MagField::GuFld(float *xx, float *b)
{
  b[0] = b[1] = b[2] = 0;
  if (MAGSFFKEY.magstat <= 0 ) return;

  if (MAGSFFKEY.rphi) {
    GuFldRphi(xx, b);
    return;
  }

  _dx = _x[1]-_x[0];
  _dy = _y[1]-_y[0];
  _dz = _z[1]-_z[0];

  double ax = std::fabs(xx[0]), sx = xx[0]/ax;
  double ay = std::fabs(xx[1]), sy = xx[1]/ay;
  double az = std::fabs(xx[2]), sz = xx[2]/az;
  //  az *= MAGSFFKEY.BZCorr;

  int idx[8];
  double ww[8];
  _Fint(ax, ay, az, idx, ww);

  float *mbx=&(_bx[0][0][0]);
  float *mby=&(_by[0][0][0]);
  float *mbz=&(_bz[0][0][0]);

  for (int i = 0; i < 8; i++) {
    b[0] += mbx[idx[i]]*ww[i];
    b[1] += mby[idx[i]]*ww[i];
    b[2] += mbz[idx[i]]*ww[i];
  }

  for (int i = 0; i < 3; i++) b[i] *= MAGSFFKEY.fscale;

//  printf ("X: %+7.3f %+7.3f %+7.3f B: %f  \n",xx[0],xx[1],xx[2],
//	  sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]));

}

void MagField::GuFldRphi(float *xx, float *b)
{
  b[0] = b[1] = b[2] = 0;
  if (MAGSFFKEY.magstat <= 0) return;

  double rr = std::sqrt(xx[0]*xx[0]+xx[1]*xx[1]);
  double ph = std::atan2(xx[1], xx[0]);
  double zz = xx[2];

  geant *_rad = &_x[0];
  geant *_phi = &_x[_nr];
  if (ph < _phi[0]) ph += 2*M_PI;
  if (rr > _rad[_nr-1] || std::fabs(ph) > _phi[_nph-1] || 
                          std::fabs(zz) > _z  [_nzr-1]) return;

  int   idx[8];
  double ww[8];
  _FintRphi(rr, ph, zz, idx, ww);

  float *mbx = &(_bx[0][0][0]);
  float *mby = &(_by[0][0][0]);
  float *mbz = &(_bz[0][0][0]);

  for (int i = 0; i < 8; i++) {
    b[0] += mbx[idx[i]]*ww[i];
    b[1] += mby[idx[i]]*ww[i];
    b[2] += mbz[idx[i]]*ww[i];
  }

  for (int i = 0; i < 3; i++) b[i] *= MAGSFFKEY.fscale;

//printf ("X: %+7.3f %+7.3f %+7.3f B: %+7.3f %+7.3f %+7.3f \n",
//        xx[0],xx[1],xx[2],b[0],b[1],b[2]);
}

void MagField::TkFld(float *xx, float hxy[][3])
{
  hxy[0][0] = hxy[0][1] = hxy[0][2] = 
  hxy[1][0] = hxy[1][1] = hxy[1][2] = 0;
  if (MAGSFFKEY.magstat <= 0 || !_bx || !_by || !_bz) return;
  if (MAGSFFKEY.rphi) return;

  double ax = std::fabs(xx[0]), sx = xx[0]/ax;
  double ay = std::fabs(xx[1]), sy = xx[1]/ay;
  double az = std::fabs(xx[2]), sz = xx[2]/az;
  //  az *= MAGSFFKEY.BZCorr;

  int idx[8];
  double ww[8];
  _Fint(ax, ay, az, idx, ww);

  int _nb = _nx*_ny*_nz;

  float *mbdx=&(_bdx[0][0][0][0]);
  float *mbdy=&(_bdy[0][0][0][0]);
  float *mbdz=&(_bdz[0][0][0][0]);

  for (int i = 0; i < 8; i++) {
    hxy[0][0] += mbdx[idx[i]]*ww[i]; hxy[1][0] += mbdx[idx[i]+_nb]*ww[i];
    hxy[0][1] += mbdy[idx[i]]*ww[i]; hxy[1][1] += mbdy[idx[i]+_nb]*ww[i];
    hxy[0][2] += mbdz[idx[i]]*ww[i]; hxy[1][2] += mbdz[idx[i]+_nb]*ww[i];
  }

}

void MagField::_Fint(double x, double y, double z, int *index, double *weight)
{
  for (int i = 0; i < 8; i++) {
    index [i] = 0;
    weight[i] = 0;
  }
  int _nb = _nx*_ny*_nz;

  int idx0 = _GetIndex(x, y, z);
  if (idx0 < 0 || _nb <= idx0) return;

  int ix = idx0    %_nx; if (ix >= _nx-1) ix = _nx-2;
  int iy = idx0/_nx%_ny; if (iy >= _ny-1) iy = _ny-2;
  int iz = idx0/_nx/_ny; if (iz >= _nz-1) iz = _nz-2;

  double dx[2], dy[2], dz[2];
  dx[1] = (x-_x[ix])/(_x[ix+1]-_x[ix]); dx[0] = 1-dx[1];
  dy[1] = (y-_y[iy])/(_y[iy+1]-_y[iy]); dy[0] = 1-dy[1];
  dz[1] = (z-_z[iz])/(_z[iz+1]-_z[iz]); dz[0] = 1-dz[1];

  int l = 0;
  for (int k = 0; k < 2; k++)
    for (int j = 0; j < 2; j++)
      for (int i = 0; i < 2; i++) {
	index [l] = _Index(ix+i, iy+j, iz+k);
	weight[l] = dx[i]*dy[j]*dz[k];
	l++;
      }
}

void MagField::_FintRphi(double r, double ph, double z, 
			 int *index, double *weight)
{
  for (int i = 0; i < 8; i++) {
    index [i] = 0;
    weight[i] = 0;
  }

  geant *_rad = &_x[0];
  geant *_phi = &_x[_nr];

  int ir, ip, iz;
  for (ir = 0; ir < _nr -1 && r  > _rad[ir+1]; ir++);
  for (ip = 0; ip < _nph-1 && ph > _phi[ip+1]; ip++);
  for (iz = 0; iz < _nz -1 && z  > _z  [iz+1]; iz++);

  int idx0 = (iz*_nph+ip)*_nr+ir;
  if (idx0 < 0 || _nr*_nph*_nzr <= idx0) return;

  double dr[2], dp[2], dz[2];
  dr[1] = (r -_rad[ir])/(_rad[ir+1]-_rad[ir]); dr[0] = 1-dr[1];
  dp[1] = (ph-_phi[ip])/(_phi[ip+1]-_phi[ip]); dp[0] = 1-dp[1];
  dz[1] = (z -_z  [iz])/(_z  [iz+1]-_z  [iz]); dz[0] = 1-dz[1];

  int l = 0;
  for (int k = 0; k < 2; k++)
    for (int j = 0; j < 2; j++)
      for (int i = 0; i < 2; i++) {
	index [l] = ((iz+k)*_nph+(ip+j))*_nr+ir+i;
	weight[l] = dr[i]*dp[j]*dz[k];
	l++;
      }
}

int MagField::_GetIndex(double x, double y, double z) const
{
  if (!_x || !_y || !_z) return -1;

  int ix = (int)((x-_x[0])/_dx);
  int iy = (int)((y-_y[0])/_dy);
  int iz = (int)((z-_z[0])/_dz);

  return _Index(ix, iy, iz);
}





inline int MagField::_Index(int i, int j, int k) const
{
    return (0 <= i && i < _nx && 0 <= j && j < _ny && 0 <= k && k < _nz) 
          ? (k*_ny+j)*_nx+i : -1; 
}




void MagField::Print(){

  printf("mfile  %s \n",mfile);
  printf("iniok  %d \n",iniok);
  for (int ix=0;ix<_nx;ix++)
    for (int iy=0;iy<_ny;iy++)
      for (int iz=0;iz<_nz;iz++)
	
	printf(" %2d %2d %2d X: %+6.1f Y: %+6.1f Z: %+6.1f BX: %+f BY: %+f BZ: %+f BdX0: %+f BdY0: %+f BdZ0: %+f BdX1: %+f BdY1: %+f BdZ1: %+f  BcX0: %+f BcY0: %+f BcZ0: %+f \n",
	       ix,iy,iz,_x[ix],_y[iy],_z[iz],_bx[iz][iy][ix],_by[iz][iy][ix],_bz[iz][iy][ix],
	       _bdx[0][iz][iy][ix],_bdy[0][iz][iy][ix],_bdz[0][iz][iy][ix],
	       _bdx[1][iz][iy][ix],_bdy[1][iz][iy][ix],_bdz[1][iz][iy][ix],
	       _bxc[iz][iy][ix],_byc[iz][iy][ix],_bzc[iz][iy][ix]);





}



void uctoh (char* MS,int* MT,int npw, int NCHP){
  /*
    C
    C CERN PROGLIB# M409    UCTOH           .VERSION KERNALT  1.00  880212
    C ORIG. 10/02/88  JZ
    C
  */

  int     MWDV[3];
  char*    CHWD= (char*) &MWDV[0];
	

  int    IBLAN1 = 0x00202020;
  int    IBLAN2 = 0x00002020;
  int    IBLAN3 = 0x00000020;
  int    MASK1  = 0xFF000000;
  int    MASK2  = 0xFFFF0000;
  int    MASK3  = 0xFFFFFF00;
  


  int  MASK[3]  ={0xFF000000, 0xFFFF0000, 0xFFFFFF00};
  int  IBLANK[3]={0x00202020,0x00002020,0x00000020};
  
  int NCH = NCHP;
  if   (!NCH) return;
  
  if(npw>=4) {
    
    //C--------          npw = 4
    
    int NWS=(NCH>>2);
    int NTRAIL = (NCH&3);
    if (NWS!=0){          
      for (int J=1;J<=NWS;J++)
	MT[J-1] = MS[J-1];
      
      if (NTRAIL==0) return;
    }
    MT[NWS] = IBLANK[NTRAIL-1]|(MS[NWS]&MASK[NTRAIL-1]);
    return;
  }
  else if(npw==1){
 
    //C--------          npw = 1
    //C--                equivalent to 'CALL UCTOH1(MS,MT,NCH)'
    
    int NWS    = (NCH>>2);
    int NTRAIL = (NCH&3);
    int JT     = 0;
    if (NWS!=0){
      //C--                Unpack the initial complete words
      for (int JS=1;JS<=NWS;JS++){
	int MWD  = MS[JS-1];
	MT[JT+0] = (IBLAN1|(MASK1&MWD));
	MT[JT+1] = (IBLAN1|(MASK1&(MWD<<8)));
	MT[JT+2] = (IBLAN1|(MASK1&(MWD<<16)));
	MT[JT+3] = (IBLAN1| (MWD<<24) );
	JT = JT + 4;
      }
      if (NTRAIL==0) return;
    }
    //C--                Unpack the trailing word
 
    int MWD = MS[NWS];
 
    for (int JS=1;JS<=NTRAIL;JS++){
      MT[JT] = (IBLAN1|(MASK1&MWD));
      MWD = MWD<<8;
      JT = JT + 1;
    }
    return;
  }
  else if(npw==2){
	
    //C--------          npw = 2
    
    int NWS    = (NCH>>2);
    int NTRAIL = (NCH&3);
    int JT     = 0;
    if (NWS!=0){      
      
      //C--                Unpack the initial complete words
      
      for( int JS=1;JS<=NWS;JS++){
	int MWD      = MS[JS-1];
	MT[JT+0] = (IBLAN2|(MASK2&MWD));
	MT[JT+1] = (IBLAN2|(MWD<<16));
	JT = JT + 2;
      }
      if (NTRAIL==0)       return;
    }
    
    //C--                Unpack the trailing word
    
    int MWD = MS[NWS];
    
    if (NTRAIL==1){ 
	MT[JT] = (IBLAN1|(MASK1&MWD));
      return;
    }
    else if (NTRAIL==2){
      MT[JT] = (IBLAN2|(MASK2&MWD));
      return;
    }else{
      MT[JT] = (IBLAN2|(MASK2&MWD));
      MT[JT+1] = (IBLAN1|(MASK1&(MWD<<16)));
    }
    return;
  }
  else if(npw==3){
    
    //C--------          npw = 3
    
    int NWS    = NCH/12;
    int NTRAIL = NCH - 12*NWS;
    int JS     = 0;
    int JT     = 0;
    if (NWS!=0)      {
      
      //C--                Unpack the initial complete words
      for (int JL=1;JL<=NWS;JL++){
	MWDV[0]  = MS[JS];
	MWDV[1]  = MS[JS+1];
	MWDV[2]  = MS[JS+2];
	MT[JT] =  (IBLAN3|(MASK3&MWDV[0]));
	MT[JT+1] = ((IBLAN3|(MWDV[0]<<24))|
		    ((MASK2&MWDV[1])>>8));
	MT[JT+2] = ((IBLAN3|(MWDV[1]<<16))
		    |((MASK1&MWDV[1])>>16));
	MT[JT+3] = (IBLAN3|(MWDV[2]<<8));
	JS = JS + 3;
	JT = JT + 4;
      }
      if (NTRAIL==0) return;
    }
    //C--                Unpack the trailing words
    
    MWDV[0] = MS[JS+0];
    MWDV[1] = MS[JS+1];
    MWDV[2] = MS[JS+2];
    
    CHWD[NTRAIL%12] = ' ';
    
    MT[JT] =  (IBLAN3|(MASK3&MWDV[1]));
    if (NTRAIL<=3) return;
 
    MT[JT+1] = ((IBLAN3|(MWDV[0]<<24))
		|((MASK2&MWDV[1])>>8));
    if (NTRAIL<=6)return;
    MT[JT+2] =  ((IBLAN3|(MWDV[1]<<16))
		 |((MASK1&MWDV[2])>>16));
    if (NTRAIL<=9) return;
    
    MT[JT+3] =  (IBLAN3|(MWDV[2]<<8));
    return;
  }
  return;
}


#endif
