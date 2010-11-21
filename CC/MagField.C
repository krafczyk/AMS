// $Id: MagField.C,v 1.13 2010/11/21 16:28:04 shaino Exp $

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
///$Date: 2010/11/21 16:28:04 $
///
///$Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "MagField.h"
#ifdef _PGTRACK_

#define SIGN(A) ((A>=0)?1:-1)


MAGSFFKEY_DEF MAGSFFKEY;
bool MagFieldOn(){return MAGSFFKEY.magstat>0;}


//PZMAG
void uctoh (char* MS,int* MT,int npw, int NCHP);


void MAGSFFKEY_DEF::init(){

   magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
   fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
}


// void TKFIELD_DEF::init(){


//   iniok=1;

//   char mapfilename[160]="fld97int.txt";
//   uctoh(mapfilename,mfile,4,160);
//   //  memset(mfile,40,sizeof(mfile[0]));
//   isec[0]=0;
//   isec[1]=0;
//   imin[0]=0;
//   imin[1]=0;
//   ihour[0]=0;
//   ihour[1]=0;
//   imon[0]=0;
//   imon[1]=0;
//   iyear[0]=0;
//   iyear[1]=0;
  
//   return;  

// }

// TKFIELD_DEF TKFIELD;


MagField *MagField::_ptr = 0;

MagField::MagField(void)
{
  iniok=0;
  fscale=1;
  for(int ii=0;ii<2;ii++)
    isec[ii]=imin[ii]=ihour[ii]=iday[ii]=imon[ii]=iyear[ii]=0;
  na[0]=na[1]=na[2]=0;
  _ptr = this;
  mm=0;
}

MagField::~MagField()
{
  if(mm) delete mm;
  _ptr=0;
}

int MagField::Read(const char *fname)
{
  
  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "MagField::ReadAMS: FATAL  File not found: " << fname << std::endl;
    exit(-1);
  }

  char *cheader = (char *)isec;
  fin.read(cheader, 4*_header_size);
  fin.close();

  int type=0;
  printf("MagField::Read-I- found a map of type "); 
  if(na[0]/10000){ na[0]=na[0]%10000; type=1;printf(" 1st Octant ");}
  else printf(" Full ");
  printf(" and Size %d %d %d\n",na[0],na[1],na[2]);
  mm= new magserv(na[0],na[1],na[2],type);

  return   mm->Read(fname,_header_size);
  
}

void MagField::GuFld(float *xx, float *b)
{
  b[0] = b[1] = b[2] = 0;
  if (MAGSFFKEY.magstat <= 0 ) {
    //  printf("No magfield\n");
    return;
  }
  if(!mm) return;

// PZ FORCE RECTANGULAR  if (MAGSFFKEY.rphi) {
//     GuFldRphi(xx, b);
//     return;
//   }

  
  double ax = xx[0];
  double ay = xx[1];
  double az = xx[2];
  //az *= MISCFFKEY.BZCorr;

  int idx[8][3];
  double ww[8];
  // Get the point for the interpolation
  // if outside the grid return 0 field
  if(!_Fint(ax, ay, az, idx, ww)) return;;

  for (int i = 0; i < 8; i++) {
    b[0] += mm->_bx(idx[i][0],idx[i][1],idx[i][2]) * ww[i];  
    b[1] += mm->_by(idx[i][0],idx[i][1],idx[i][2]) * ww[i];  
    b[2] += mm->_bz(idx[i][0],idx[i][1],idx[i][2]) * ww[i];  
  }

  //  for (int i = 0; i < 3; i++) b[i] *= MAGSFFKEY.fscale;
  for (int i = 0; i < 3; i++) b[i] *= fscale;

  //printf ("X: %+7.3f %+7.3f %+7.3f B: %f  \n",xx[0],xx[1],xx[2],
  //	  sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]));
  
}

// void MagField::GuFldRphi(float *xx, float *b)
// {
//   b[0] = b[1] = b[2] = 0;
//   if (MAGSFFKEY.magstat <= 0) return;

//   double rr = std::sqrt(xx[0]*xx[0]+xx[1]*xx[1]);
//   double ph = std::atan2(xx[1], xx[0]);
//   double zz = xx[2];

//   geant *_rad = &mm->_x[0];
//   geant *_phi = &mm->_x[_nr];
//   if (ph < _phi[0]) ph += 2*M_PI;
//   if (rr > _rad[_nr-1] || std::fabs(ph) > _phi[_nph-1] || 
//                           std::fabs(zz) > mm->_z  [_nzr-1]) return;

//   int   idx[8];
//   double ww[8];
//   _FintRphi(rr, ph, zz, idx, ww);

//   float *mbx = &(mm->_bx[0][0][0]);
//   float *mby = &(mm->_by[0][0][0]);
//   float *mbz = &(mm->_bz[0][0][0]);

//   for (int i = 0; i < 8; i++) {
//     b[0] += mbx[idx[i]]*ww[i];
//     b[1] += mby[idx[i]]*ww[i];
//     b[2] += mbz[idx[i]]*ww[i];
//   }

//   //  for (int i = 0; i < 3; i++) b[i] *= MAGSFFKEY.fscale;
//   for (int i = 0; i < 3; i++) b[i] *= fscale;

// //printf ("X: %+7.3f %+7.3f %+7.3f B: %+7.3f %+7.3f %+7.3f \n",
// //        xx[0],xx[1],xx[2],b[0],b[1],b[2]);
// }

void MagField::TkFld(float *xx, float hxy[][3])
{
  hxy[0][0] = hxy[0][1] = hxy[0][2] = 
  hxy[1][0] = hxy[1][1] = hxy[1][2] = 0;
  if (MAGSFFKEY.magstat <= 0 || !mm ) return;
  //  PZ FORCE RECTANGULAR if (MAGSFFKEY.rphi) return;

  double ax = xx[0];
  double ay = xx[1];
  double az = xx[2];
  //az *= MISCFFKEY.BZCorr;

  int idx[8][3];
  double ww[8];
  // Get the point for the interpolation
  // if outside the grid return 0 field
  if(!_Fint(ax, ay, az, idx, ww)) return;

  for (int i = 0; i < 8; i++) {
    hxy[0][0] += mm->_bdx(0,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 
    hxy[1][0] += mm->_bdx(1,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 

    hxy[0][1] += mm->_bdy(0,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 
    hxy[1][1] += mm->_bdy(1,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 

    hxy[0][2] += mm->_bdz(0,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 
    hxy[1][2] += mm->_bdz(1,idx[i][0],idx[i][1],idx[i][2])*ww[i]; 
  }

}

int  MagField::_Fint(double x, double y, double z, int index[][3], double *weight)
{
  for (int i = 0; i < 8; i++) {
    index [i][0] = 0;    index [i][1] = 0;    index [i][2] = 0;
    weight[i] = 0;
  }
  if(!mm) return 0;

  //Get the grid point near to the hit 
  int ix = (int)((x-mm->_x(0))/mm->_dx);
  int iy = (int)((y-mm->_y(0))/mm->_dy);
  int iz = (int)((z-mm->_z(0))/mm->_dz);
  //  printf(" Point %f %f %f grid %d %d %d index %d\n",x,y,z,ix,iy,iz,mm->getindex(ix,iy,iz));
  // check if we are inside the map

  if(
     ix<0 || ix>=(mm->nx()-1) ||
     iy<0 || iy>=(mm->ny()-1) ||
     iz<0 || iz>=(mm->nz()-1) 
     ) return 0;

  double dx[2], dy[2], dz[2];
  dx[1] = (x-mm->_x(ix))/(mm->_x(ix+1)-mm->_x(ix)); dx[0] = 1-dx[1];
  dy[1] = (y-mm->_y(iy))/(mm->_y(iy+1)-mm->_y(iy)); dy[0] = 1-dy[1];
  dz[1] = (z-mm->_z(iz))/(mm->_z(iz+1)-mm->_z(iz)); dz[0] = 1-dz[1];

  int l = 0;
  for (int k = 0; k < 2; k++)
    for (int j = 0; j < 2; j++)
      for (int i = 0; i < 2; i++) {
	index [l][0] = ix+i;
	index [l][1] =iy+j;
	index [l][2] =iz+k;
	weight[l] = dx[i]*dy[j]*dz[k];
	l++;
      }
  return 1;
}

// void MagField::_FintRphi(double r, double ph, double z, 
// 			 int *index, double *weight)
// {
//   for (int i = 0; i < 8; i++) {
//     index [i] = 0;
//     weight[i] = 0;
//   }

//   geant *_rad = &_x[0];
//   geant *_phi = &mm->_x[_nr];

//   int ir, ip, iz;
//   for (ir = 0; ir < _nr -1 && r  > _rad[ir+1]; ir++);
//   for (ip = 0; ip < _nph-1 && ph > _phi[ip+1]; ip++);
//   for (iz = 0; iz < mm->_nz -1 && z  > mm->_z  [iz+1]; iz++);

//   int idx0 = (iz*_nph+ip)*_nr+ir;
//   if (idx0 < 0 || _nr*_nph*_nzr <= idx0) return;

//   double dr[2], dp[2], dz[2];
//   dr[1] = (r -_rad[ir])/(_rad[ir+1]-_rad[ir]); dr[0] = 1-dr[1];
//   dp[1] = (ph-_phi[ip])/(_phi[ip+1]-_phi[ip]); dp[0] = 1-dp[1];
//   dz[1] = (z -_z  [iz])/(mm->_z  [iz+1]-mm->_z  [iz]); dz[0] = 1-dz[1];

//   int l = 0;
//   for (int k = 0; k < 2; k++)
//     for (int j = 0; j < 2; j++)
//       for (int i = 0; i < 2; i++) {
// 	index [l] = ((iz+k)*_nph+(ip+j))*_nr+ir+i;
// 	weight[l] = dr[i]*dp[j]*dz[k];
// 	l++;
//       }
//  }







void MagField::Print(){
  
  printf("mfile  %s \n",mfile);
  printf("iniok  %d \n",iniok);
  if(!mm) return;
  for (int ix=0;ix<mm->nx();ix++)
    for (int iy=0;iy<mm->ny();iy++)
      for (int iz=0;iz<mm->nz();iz++)
	
	printf(" %2d %2d %2d X: %+6.1f Y: %+6.1f Z: %+6.1f BX: %+f BY: %+f BZ: %+f BdX0: %+f BdY0: %+f BdZ0: %+f BdX1: %+f BdY1: %+f BdZ1: %+f  \n",
	       ix,iy,iz,mm->_x(ix),mm->_y(iy),mm->_z(iz),mm->_bx(iz,iy,ix),mm->_by(iz,iy,ix),mm->_bz(iz,iy,ix),
	       mm->_bdx(0,iz,iy,ix),mm->_bdy(0,iz,iy,ix),mm->_bdz(0,iz,iy,ix),
	       mm->_bdx(1,iz,iy,ix),mm->_bdy(1,iz,iy,ix),mm->_bdz(1,iz,iy,ix));
  //	       mm->_bxc(iz,iy,ix),mm->_byc(iz,iy,ix),mm->_bzc(iz,iy,ix));
}


//================================================================================
//================================================================================
//================================================================================


int magserv::getindex(int i,int j,int k){
  if (_type==1){
    int i2 = 1 - _nx + i;
    int j2 = 1 - _ny + j;
    int k2 = 1 - _nz + k;
    return int(fabs(k2))*_nx*_ny+ int(fabs(j2))*_nx + int(fabs(i2));
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

int magserv::Read(const char *fname, int skip){
  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "MagField::ReadAMS: File not found: " << fname << std::endl;
    return -1;
  }

  if(skip){
    char *cheader = new char[4*skip];
    fin.read(cheader, 4*skip);
   //  int *pp=(int*) cheader;
//     for (int ii=0;ii<skip;ii++){
//       cout <<" header "<<ii<<"  "<<pp[ii]<<endl;
//     }
    delete[] cheader;
  }
  int stot=0;
  int _nb = _nx*_ny*_nz;
  int _nn = _nx+_ny+_nz;
  std::cout << "MagField::Read: Reading field map... ";
  std::cout.flush();
  char *cdata = (char *)x;
  int dsize = _nx;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)y;
  dsize = _ny;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)z;
  dsize = _nz;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  _dx = x[1]-x[0];
  _dy = y[1]-y[0];
  _dz = z[1]-z[0];

 

  cdata = (char *)bx;
  dsize = _nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)by;
  dsize = _nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)bz;
  dsize = _nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  if(_type!=1){
    cdata = new char[4*_nn];
    dsize = _nn;
    fin.read(cdata, 4*dsize);
    stot += 4*dsize;
    if(cdata) delete[] cdata;
  }
  cdata = (char *)bdx;
  dsize = 2*_nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)bdy;
  dsize = 2*_nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  cdata = (char *)bdz;
  dsize = 2*_nb;
  fin.read(cdata, 4*dsize);
  stot += 4*dsize;

  bool check = true;
  for (int i = 0; i < _nx-1; i++) if (x[i] >= x[i+1]) check = false;
  for (int i = 0; i < _ny-1; i++) if (y[i] >= y[i+1]) check = false;
  for (int i = 0; i < _nz-1; i++) if (z[i] >= z[i+1]) check = false;
  if (std::fabs(x[_nx-1]-x[0]-(_nx-1)*_dx) > 1e-3 ||
      std::fabs(y[_ny-1]-y[0]-(_ny-1)*_dy) > 1e-3 ||
      std::fabs(z[_nz-1]-z[0]-(_nz-1)*_dz) > 1e-3) check = false;
  

  if (!check) {
    std::cerr << "Error in MagField::Read format check failed: "
	      << fname <<  std::endl;
  }else
    std::cout << "MagField::Read format check Success: "
	      << fname <<  std::endl;
  return _nb;
  
  
}



magserv::magserv(int inx, int iny, int inz,int type)
  :_type(type),_nx(inx),_ny(iny),_nz(inz){
  
  x   = new float[_nx];
  y   = new float[_ny];
  z   = new float[_nz];
  
  bx  = new float[_nx*_ny*_nz];
  by  = new float[_nx*_ny*_nz] ;
  bz  = new float[_nx*_ny*_nz] ;
  
  
  bdx  = new float[2*_nx*_ny*_nz] ;
  bdy  = new float[2*_nx*_ny*_nz] ;
  bdz  = new float[2*_nx*_ny*_nz] ;
  
//   bxc  = new float[_nx*_ny*_nz] ;
//   byc  = new float[_nx*_ny*_nz] ;
//   bzc  = new float[_nx*_ny*_nz] ;
  _dx=1;  _dy=1;  _dz=1;
  
}


magserv::~magserv(){
  if(x) delete[] x;
  if(y) delete[] y;
  if(z) delete[] z;
  
  
  if(bx) delete[] bx;
  if(by) delete[] by;
  if(bz) delete[] bz;
  
  
  
  if(bdx) delete[] bdx;
  if(bdy) delete[] bdy;
  if(bdz) delete[] bdz;
  
  
//   if(bxc) delete[] bxc;
//   if(byc) delete[] byc;
//   if(bzc) delete[] bzc;
  
}

// float magserv::_x(int i){
//   float aa;
//   if(_type==1){
//     int sigx=((1-_nx+i)>=0)?1:-1;
//     aa=x[abs(1-_nx+i)]*sigx;
//   }
//   else aa=x[i];
  
//   return aa;
// }



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
