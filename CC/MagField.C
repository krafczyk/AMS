// $Id: MagField.C,v 1.19 2014/01/04 16:00:13 pzuccon Exp $

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
///$Date: 2014/01/04 16:00:13 $
///
///$Revision: 1.19 $
///
//////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>

#include "MagField.h"
#include "bcorr.h"
#ifdef _PGTRACK_



MAGSFFKEY_DEF MAGSFFKEY;
bool MagFieldOn(){return MAGSFFKEY.magstat>0;}


//PZMAG
void uctoh (char* MS,int* MT,int npw, int NCHP);


void MAGSFFKEY_DEF::init(){

   magstat=1;    //(1) -1/0/1->warm/cold_OFF/cold_ON 
   fscale=1.;    //(2) rescale factor (wrt nominal field) (if any) 
}




MagField *MagField::_ptr = 0;

MagField::MagField(void)
{
  iniok=0;
  fscale=1;
  mag_temp=18.5;
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

MagField *MagField::GetPtr(void)
{
  if (_ptr) return _ptr;
#pragma omp critical (maggetptr)
  {
    if (!_ptr) new MagField;
  }
  return _ptr;
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
  if (MAGSFFKEY.magstat <= 0 || !mm) return;
  
  double ax = xx[0];
  double ay = xx[1];
  double az = xx[2];

  //Get the grid point near to the hit 
  int ix = (int)((ax-mm->_x0())/mm->_dx);
  int iy = (int)((ay-mm->_y0())/mm->_dy);
  int iz = (int)((az-mm->_z0())/mm->_dz);

  if (ix < 0 || ix >= (mm->nx()-1) ||
      iy < 0 || iy >= (mm->ny()-1) ||
      iz < 0 || iz >= (mm->nz()-1)) return;

  double dx[2], dy[2], dz[2];
  dx[1] = (ax-mm->_x(ix))/(mm->_x(ix+1)-mm->_x(ix)); dx[0] = 1-dx[1];
  dy[1] = (ay-mm->_y(iy))/(mm->_y(iy+1)-mm->_y(iy)); dy[0] = 1-dy[1];
  dz[1] = (az-mm->_z(iz))/(mm->_z(iz+1)-mm->_z(iz)); dz[0] = 1-dz[1];

  for (int k = 0; k < 2; k++)
    for (int j = 0; j < 2; j++)
      for (int i = 0; i < 2; i++) {

	float bx, by, bz;
	mm->_bb(ix+i, iy+j, iz+k, bx, by, bz);

	float ww = dx[i]*dy[j]*dz[k];
	b[0] += bx*ww;
	b[1] += by*ww;
	b[2] += bz*ww;
      }
  b[0]*=fscale;
  b[1]*=fscale;
  b[2]*=fscale;

}

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
//   for (int i = 0; i < 8; i++) {
//     index [i][0] = 0;    index [i][1] = 0;    index [i][2] = 0;
//     weight[i] = 0;
//   }
//   if(!mm) return 0;

  //Get the grid point near to the hit 
  int ix = (int)((x-mm->_x0())/mm->_dx);
  int iy = (int)((y-mm->_y0())/mm->_dy);
  int iz = (int)((z-mm->_z0())/mm->_dz);
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
  _nx2=_nx*2-1;  
  _ny2=_ny*2-1;
  _nz2=_nz*2-1;
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

float  MagField::BCorrFactor(float temp){
  float dBdT=8.37e-4+5.07e-6*temp;
  const float TETH=18.5;
  return 1.-dBdT*(temp-TETH);
}

int MagField::UpdateMagTemp(unsigned int time){

  float temp=18.5;
  if(time!=MagnetVarp::mgtt.time){
    MagnetVarp::mgtt.loadValues(time,true);
    if(time!=MagnetVarp::mgtt.time)return -1;
  }
  
  int ret=MagnetVarp::mgtt.getmeanmagnettemp(temp);
  if(ret)return -2;
  
  SetMagTemp(temp);
  return 0;
}

#endif
