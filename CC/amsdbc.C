// Author V. Choutko 24-may-1996
 
#include <math.h>
#include <amsdbc.h>
#include <amsstl.h>
//

const integer AMSDBc::REFITTED=1;
const integer AMSDBc::WIDE=2;
const integer AMSDBc::AMBIG=4;
const integer AMSDBc::RELEASED=8;
const integer AMSDBc::BAD=16;
const integer AMSDBc::USED=32;
const integer AMSDBc::DELETED=64;
const integer AMSDBc::BADHIS=128;
const integer AMSDBc::ONESIDE=256;
const integer AMSDBc::BADTIME=512;
const integer AMSDBc::NEAR=1024;
const integer AMSDBc::WEAK=2048;
const integer AMSDBc::AwayTOF=4096;
const integer AMSDBc::FalseX=8192;
const integer AMSDBc::FalseTOFX=16384;
const integer AMSDBc::RECOVERED=16384*2;

integer AMSDBc::debug=1;
integer AMSDBc::BigEndian=0;
const number AMSDBc::raddeg=57.295779;
const number AMSDBc::pi=3.14159265358979;
const number AMSDBc::twopi=2*pi;
geant AMSDBc::ams_size[3]={380.,380.,380.};
const char AMSDBc::ams_name[]="AMSG";
geant AMSDBc::ams_coo[3]={0.,0.,0.};
const integer AMSDBc::ams_rotmno=1;
number AMSDBc::ams_nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
//number AMSDBc::ams_nrm[3][3]={0.,0.,1.,0.,1.,0.,-1.,0.,0};
// Tracker parameters
   const integer AMSDBc::_nlay=nl;

   const integer  AMSDBc::_nstripssen[_nlay][2]={
                                               224,640,
                                               192,640,
                                               192,640,
                                               192,640,
                                               192,640,
                                               224,640};
   const integer  AMSDBc::_nstripssenR[_nlay][2]={
                                               767,1284,
                                               767,1284,
                                               767,1284,
                                               767,1284,
                                               767,1284,
                                               767,1284};
   const integer  AMSDBc::_nstripsdrp[_nlay][2]={384,640,384,640,384,640,
                                               384,640,384,640,384,640};
   integer AMSDBc::maxstripsdrp(){
     static int init=0;
     static int msstr=0;
     if(init++==0){
       for(int i=0;i<_nlay;i++){
         for(int j=0;j<2;j++){
           if(msstr< _nstripsdrp[i][j])msstr=_nstripsdrp[i][j];
         }
       } 
     }
     return msstr;
   }
   const integer AMSDBc::_maxnlad=nld;
   const number AMSDBc::_layd[_nlay][5]={
                               5.3,0.,75., 0.,75.,
                               5.,0.,54.0,0.,54.0, 
                               5.,0.,54.0,0.,54.0, 
                               5.,0.,54.0,0.,54.0, 
                               5.,0.,54.0,0.,54.0, 
                               5.3,0.,75. ,0.,75.};
   const number AMSDBc::_halfldist[_nlay]={0.047,0.047,0.047,0.047,0.047,0.047};
   const number  AMSDBc::_xposl[_nlay]={0,0,0,0,0,0};
   const number  AMSDBc::_yposl[_nlay]={0,0,0,0,0,0};
   const number  AMSDBc::_zposl[_nlay]={51.015,29.185,7.785,-7.785,-29.185,-51.015};
const number AMSDBc::_nrml[_nlay][3][3]={
                                          1,0,0,
                                          0,-1,0,
                                          0,0,-1,
                                          1,0,0,
                                          0,1,0,
                                          0,0,1,
                                          1,0,0,
                                          0,1,0,
                                          0,0,1,
                                          1,0,0,
                                          0,-1,0,
                                          0,0,-1,
                                          1,0,0,
                                          0,-1,0,
                                          0,0,-1,
                                          1,0,0,
                                          0,1,0,
                                          0,0,1};
   const integer AMSDBc::_nlad[_nlay]={17,14,14,14,14,17};
   const integer AMSDBc::_nsen[_nlay][_maxnlad]={15,20,23,26,28,29,30,30,30,30,30,
                                 29,28,26,23,20,15,
                                 7,14,18,21,23,24,24,24,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,24,24,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,24,24,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,24,24,24,23,21,18,14,7,0,0,0,
                                 15,20,23,26,28,29,30,30,30,30,30,
                                 29,28,26,23,20,15};
   const integer AMSDBc::_nhalf[_nlay][_maxnlad]={
                               7,10,11,13,14,14,15,15,15,15,15,14,14,13,11,10,7,
                               7,7,9,11,11,12,12,12,12,11,11,9,7,7,0,0,0,
                               7,7,9,11,11,12,12,12,12,11,11,9,7,7,0,0,0,
                               7,7,9,11,11,12,12,12,12,11,11,9,7,7,0,0,0,
                               7,7,9,11,11,12,12,12,12,11,11,9,7,7,0,0,0,
                               7,10,11,13,14,14,15,15,15,15,15,14,14,13,11,10,7};
const number  AMSDBc::_zpos[_nlay]={0,0,0,0,0,0};
    
   const number  AMSDBc::_ssize_active[_nlay][2]={3.9884,7.062,3.9884,7.062,
                       3.9884,7.062,3.9884,7.062,3.9884,7.062,3.9884,7.062};
   const number  AMSDBc::_ssize_inactive[_nlay][2]={4.14000,7.2045,
                                                    4.14000,7.2045,
                                                    4.14000,7.2045,
                                                    4.14000,7.2045,
                                                    4.14000,7.2045,
                                                    4.14000,7.2045};
   const number  AMSDBc::_silicon_z[_nlay]={0.03,0.03,0.03,0.03,0.03,0.03};
   const number  AMSDBc::_zelec[_nlay][3]={
                                           5.,.8,-0.015,
                                           5.,.8,-0.015,
                                           5.,.8,-0.015,
                                           5.,.8,-0.015,
                                           5.,.8,-0.015,
                                           5.,.8,-0.015 };
   const integer AMSDBc::_nladshuttle[_nlay]={4,6,6,6,6,3};
   const integer AMSDBc::_boundladshuttle[_nlay]={7,5,5,5,5,8};

// center to center for ladders
const number  AMSDBc::_c2c[_nlay]={7.30,7.30,7.30,7.30,7.30,7.30};
// support foam width;
const number  AMSDBc::_support_foam_w[_nlay]={0.5,0.5,0.5,0.5,0.5,0.5};
const number  AMSDBc::_support_foam_tol[_nlay]={0.05,0.05,0.05,0.05,0.05,0.05};
// support hc width;
const number  AMSDBc::_support_hc_w[_nlay]={4.,1.236,1.236,1.236,1.236,4.};
// support hc radius;
const number  AMSDBc::_support_hc_r[_nlay]={71.5,53.6,53.6,53.6,53.6,71.5};
// support hc z;
const number  AMSDBc::_support_hc_z[_nlay]={-3.052,-1.67,-1.67,-1.67,-1.67,-3.052};

const number AMSDBc::_PlMarkerPos[2][_nlay][4][3]={ -54.1,-45.35,-1.017,
                                                54.1,-45.35,-1.017,
                                                54.1, 45.35,-1.017,
                                                -54.1, 45.35,-1.017,
                                                -40.,  -33.5,-1.017,
                                                 40.,  -33.5,-1.017,
                                                 40.,   33.5,-1.017,
                                                -40.,   33.5,-1.017,
                                                -40.,  -33.5,-1.017,
                                                 40.,  -33.5,-1.017,
                                                 40.,   33.5,-1.017,
                                                -40.,   33.5,-1.017,
                                                -40.,  -33.5,-1.017,
                                                 40.,  -33.5,-1.017,
                                                 40.,   33.5,-1.017,
                                                -40.,   33.5,-1.017,
                                                -40.,  -33.5,-1.017,
                                                 40.,  -33.5,-1.017,
                                                 40.,   33.5,-1.017,
                                                -40.,   33.5,-1.017,
                                                 -54.1,-45.35,-1.017,
                                                  54.1,-45.35,-1.017,
                                                  54.1, 45.35,-1.017,
                                                 -54.1, 45.35,-1.017,
                                                 -54.1,-45.35,-3.052,
                                                 54.1,-45.35,-3.052,
                                                 54.1, 45.35,-3.052,
                                                 -54.1, 45.35,-3.052,
                                                -40.,  -33.5,-1.67,
                                                 40.,  -33.5,-1.67,
                                                 40.,   33.5,-1.67,
                                                -40.,   33.5,-1.67,
                                                -40.,  -33.5,-1.67,
                                                 40.,  -33.5,-1.67,
                                                 40.,   33.5,-1.67,
                                                -40.,   33.5,-1.67,
                                                -40.,  -33.5,-1.67,
                                                 40.,  -33.5,-1.67,
                                                 40.,   33.5,-1.67,
                                                -40.,   33.5,-1.67,
                                                -40.,  -33.5,-1.67,
                                                 40.,  -33.5,-1.67,
                                                 40.,   33.5,-1.67,
                                                -40.,   33.5,-1.67,
                                                -54.1,-45.35,-3.052,
                                                 54.1,-45.35,-3.052,
                                                 54.1, 45.35,-3.052,
                                                -54.1, 45.35,-3.052};


   


integer AMSDBc::activeladdshuttle(int i,int j, int s){
#ifdef __AMSDEBUG__
 assert(i>0 && i<=6);
 assert(j>0);
 assert (s==0 || s==1);
#endif

 // Shuttle ladders 

 if( i==1){
  if(j>=8 && j<=11) return 1;
  else return 0;
 }
 if( i==2){
  if(j>=5 && j<=10) return 1;
  else return 0;
 }
 if( i==3){
  if(j>=6 && j<=10){
   if(j==10 && s==0)   return 0;
   else return 1;
  }
  return 0;
 }
 if( i==4){
  if(j>=5 && j<=10){
    if((j==5 || j==10) && s==0)return 0;
    else return 1;
  }
  return 0;
 }
 if( i==5){
  if(j>=5 && j<=10) return 1;
  else return 0;
 }
 if( i==6){
  if(j>=8 && j<=10) return 1;
  else return 0;
 }
 else return 0;
}


