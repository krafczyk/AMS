// Author V. Choutko 24-may-1996
 
#include <math.h>
#include <amsdbc.h>
#include <amsstl.h>
//

const integer AMSDBc::BAD=16;
const integer AMSDBc::USED=32;
const integer AMSDBc::DELETED=64;

integer AMSDBc::debug=1;
const number AMSDBc::raddeg=57.295779;
const number AMSDBc::pi=3.14159265358979;
const number AMSDBc::twopi=2*pi;
      geant AMSDBc::ams_size[3]={380.,380.,380.};
     const char AMSDBc::ams_name[]="AMSG version 1.00";
      geant AMSDBc::ams_coo[3]={0.,0.,0.};
// Tracker parameters
   const integer AMSDBc::_nlay=nl;
   const integer AMSDBc::_maxstrips=mxstr;

   const integer  AMSDBc::_nstripssen[_nlay][2]={224,640,192,640,192,640,192,
                                               640,192,640,224,640};
   const integer  AMSDBc::_nstripsdrp[_nlay][2]={384,640,384,640,384,640,
                                               384,640,384,640,384,640};
   
   const integer AMSDBc::_maxnlad=nld;
   const number AMSDBc::_layd[_nlay][5]={7.5,0.,75.,0.,54.3,
                               40.,0.,54.3,0.,54.3, 
                               40.,0.,54.3,0.,54.3, 
                               40.,0.,54.3,0.,54.3, 
                               40.,0.,54.3,0.,54.3, 
                               7.5,0.,54.3,0.,75.};
   const number  AMSDBc::_zposl[_nlay]={-47.5,0.,0.,0.,0.,47.5};
   const integer AMSDBc::_nlad[_nlay]={17,14,14,14,14,17};
   const integer AMSDBc::_nsen[_nlay][_maxnlad]={15,20,23,26,28,29,30,30,30,30,30,
                                 29,28,26,23,20,15,
                                 7,14,18,21,23,24,25,25,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,25,25,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,25,25,24,23,21,18,14,7,0,0,0,
                                 7,14,18,21,23,24,25,25,24,23,21,18,14,7,0,0,0,
                                 15,20,23,26,28,29,30,30,30,30,30,
                                 29,28,26,23,20,15};
   const integer AMSDBc::_nhalf[_nlay][_maxnlad]={8,10,12,13,14,15,15,15,15,15,15,
                                 15,14,13,12,10,8,
                                 4,7,9,11,12,12,13,13,12,12,11,9,7,4,0,0,0,
                                 4,7,9,11,12,12,13,13,12,12,11,9,7,4,0,0,0,
                                 4,7,9,11,12,12,13,13,12,12,11,9,7,4,0,0,0,
                                 4,7,9,11,12,12,13,13,12,12,11,9,7,4,0,0,0,
                                 8,10,12,13,14,15,15,15,15,15,15,15,14,13,12,10,8};
   const number  AMSDBc::_zpos[_nlay]={-50.95,-29.15,-7.75,7.75,29.15,50.95};
    
   const number  AMSDBc::_ssize_active[_nlay][2]={3.9884,7.04,3.9884,7.04,
                       3.9884,7.04,3.9884,7.04,3.9884,7.04,3.9884,7.04};
   const number  AMSDBc::_ssize_inactive[_nlay][2]={4.13600,7.2045,
                                                    4.13600,7.2045,
                                                    4.13600,7.2045,
                                                    4.13600,7.2045,
                                                    4.13600,7.2045,
                                                    4.13600,7.2045};
   const number  AMSDBc::_silicon_z[_nlay]={0.03,0.03,0.03,0.03,0.03,0.03};
   const number  AMSDBc::_zelec[_nlay][2]={6.,1.8,6.,1.8,6.,1.8,6.,1.8, 
                                         6.,1.8,6.,1.8 };
// center to center for ladders
const number  AMSDBc::_c2c[_nlay]={7.3039,7.3039,7.3039,7.3039,7.3039,7.3039};
// support half width;
const number  AMSDBc::_support_w[_nlay]={0.03,0.03,0.03,0.03,0.03,0.03};

   

integer AMSDBc::activeladdshuttle(int i,int j){
#ifdef __AMSDEBUG__
 assert(i>0 && i<=6);
 assert(j>0);
#endif

 // Shuttle ladders 

 if( i==1){
  if(j>=7 && j<=10) return 1;
  else return 0;
 }
 if( i>1 && i<6){
  if(j>=5 && j<=10) return 1;
  else return 0;
 }
 if( i==6){
  if(j>=8 && j<=10) return 1;
  else return 0;
 }
 else return 0;
}


