//  $Id: TkDBc.C,v 1.47 2012/02/13 16:44:22 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkDBc.C
///\brief Source file of TkDBc class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/18 PZ  Update for the new TkSens class
///\date  2008/04/10 PZ  Update the Z coo according to the latest infos
///\date  2008/04/18 SH  Update for the alignment study
///$Date: 2012/02/13 16:44:22 $
///
///$Revision: 1.47 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include <cstring>
#include "tkpatt.h"
// Hard-coded sensor alignment parameters
#include "TkDBcSalig.C"


TkDBc* TkDBc::Head=0;
float* TkDBc::linear=0;
char   TkDBc::_setupname[4][30]={"Unknown","AMS02-PreIntegration","AMS02-Ass1","AMS02P"};
int    TkDBc::_setup=3;
 
TkDBc::TkDBc(){
 for (int ii=0;ii<maxplanes;ii++)
      planes[ii]=0; 
}

void TkDBc::CreateTkDBc(int force_delete){
  
  if(force_delete&&TkDBc::Head){
    delete TkDBc::Head;
    TkDBc::Head=0;
  }
  
  if(!TkDBc::Head){
    TkDBc::Head=new TkDBc();

  }
  else{
    printf("Error a TkDBc instance already exists and \nonly one TkDBc object is allowed (singleton)!! \n" );
  }
}


TkDBc* TkDBc::GetHead(int setup,const char* inputfilename, int pri){
  if(TkDBc::Head) return TkDBc::Head;
  CreateTkDBc();
  TkDBc::Head->init(setup,inputfilename,pri);
  return TkDBc::Head;
}



TkDBc::~TkDBc(){
  for (int ii=0;ii<maxplanes;ii++)
    if(planes[ii]) delete planes[ii];
  
  for ( tkidIT pp=tkidmap.begin();pp!=tkidmap.end();++pp)
    if(pp->second) delete pp->second;

}


void TkDBc::init(int setup,const char *inputfilename, int pri){
  // get setup
    char name[20];
    _setup=setup;
    if(_setup<1 || _setup>3){
      fprintf(stderr,"TkDBc::init -E-  FATAL Unknown Setup number %d\n",_setup);
      exit(-3);
    }
    
    printf("TkDBc::init -I- Selected Setup %d  %s\n",_setup,_setupname[_setup]);

    printf("initializing track patterns\n");
     if(patt) delete patt;
    int nn=8;
    if(setup==3)nn=7;
    patt= new tkpatt(nn);
    patt->Init(nn);

// J-layer numbering  
    const integer J2lay_SC[10]={-1, 1, 2, 3, 4, 5, 6, 7, 8, -1};
    const integer J2lay_PM[10]={-1, 8, 1, 2, 3, 4, 5, 6, 7,  9};
    
    const integer lay2J_SC[10]={-1, 1, 2, 3, 4, 5, 6, 7, 8, -1};
    const integer lay2J_PM[10]={-1, 2, 3, 4, 5, 6, 7, 8, 1,  9};

    if(setup==3){
      memcpy(_J_to_lay, J2lay_PM, 10*sizeof(J2lay_PM[0]));
      memcpy(_lay_to_J, lay2J_PM, 10*sizeof(lay2J_PM[0]));
    }else{
      memcpy(_J_to_lay, J2lay_SC, 10*sizeof(J2lay_SC[0]));
      memcpy(_lay_to_J, lay2J_PM, 10*sizeof(lay2J_SC[0]));
    }
//----------------------------------------------------------------------------------
//       Ladder Electronics
//----------------------------------------------------------------------------------
    // Electronics dimension parameters
    const number  zelec[3]={ 1.5,6.8, 7.8};
    memcpy(_zelec,zelec,3*sizeof(zelec[0]));





//----------------------------------------------------------------------------------
//       Silicon Sensors
//----------------------------------------------------------------------------------

//    const number  ssize_active[2]={3.9728,7.062};
    const number  ssize_active[2]={383*0.0104,641.5*0.0110};
    memcpy(_ssize_active,ssize_active,sizeof(ssize_active));


    const number  ssize_inactive[2]={4.13600,7.2045};
    memcpy(_ssize_inactive,ssize_inactive,sizeof(ssize_inactive));

    
    _silicon_z=0.03;
    
    _NReadStripK7=224;
    _NReadStripK5=192;
    _NReadoutChanS=640;
    _NReadoutChanK=384;
    _SensorPitchK=4.140;
    _PitchS=0.0110;
    _PitchK5=0.0208;
    _PitchK7=0.0104;
    _ImplantPitchK=0.0104;
    

//----------------------------------------------------------------------------------
//         Planes
//----------------------------------------------------------------------------------
    if(_setup==1||_setup==2){
      nplanes=5;
      nlays=8;
    }else if(_setup==3){
      nplanes=6;
      nlays=9;
    }
    Plane6Size[0]=109.4;
    Plane6Size[1]=75;
    Plane6Size[2]=1.;
    
    Plane6EnvelopSize[0]=109.4;
    Plane6EnvelopSize[1]=75;
    Plane6EnvelopSize[2]=3.3;
    Plane6_skin_w[0]=0.07;
    Plane6_skin_w[1]=0.07;
  
    const number  xposl[maxplanes]={0,0,0,0,0,0.};
    memcpy(_xpos,xposl,maxplanes*sizeof(xposl[0]));
     // 30/11/2011  updated with info from Lubelsmeyer
    //                                      1/5N   2       3     4      1N     6    
    const number sup_hc_skin_w[maxplanes]={0.069, 0.015, 0.015, 0.015, 0.069, 0.06};
    memcpy(_sup_hc_skin_w,sup_hc_skin_w,maxplanes*sizeof(sup_hc_skin_w[0]));

    const number  yposl[maxplanes]={0,0,0,0,0,0};
    memcpy(_ypos,yposl,nplanes*sizeof(yposl[0]));

    const number  zposl[maxplanes]={56.14, 27.53, 0, -27.53, -56.14,900.};
    memcpy(_zpos,zposl,nplanes*sizeof(zposl[0]));
    if(_setup==3){
      // TRD TOP
      //_zpos[4]=170.;
      _zpos[4]=162.;    // SH: coarse alignment with CR data (Aug/2010)
      //       former ECAL FACE (PZ Aug 2010 adjusted from CAD)
      _zpos[5]=        -134.78     - (Plane6EnvelopSize[2] - Plane6Size[2]/2) ;
      
    }
    
    const int16  nslot[maxplanes][2]={{0,15},{15,15},{15,15},{15,15},{15,0},{8,0}};
    memcpy(_nslot,nslot,maxplanes*2*sizeof(nslot[0][0]));
    
    
    // Plane support thickness in cm
    const number sup_hc_w[maxplanes]={  4.0, 1.236, 1.236, 1.236,  4.0 ,1.};
    memcpy(_sup_hc_w, sup_hc_w, maxplanes*sizeof(sup_hc_w[0]));

    // Plane support radius in cm
    const number sup_hc_r[maxplanes]={ 71.5, 53.6, 53.6, 53.6, 71.5,0 };
    memcpy(_sup_hc_r, sup_hc_r, maxplanes*sizeof(sup_hc_r[0]));


    // Plane envelop radius in cm
    const  double plane_d1[maxplanes] = { 74.0, 54.0, 54.0, 54.0, 74.0,0. };
    memcpy(_plane_d1,plane_d1,maxplanes*sizeof(_plane_d1[0]));

    // Plane envelop half thickness in cm
    const double  plane_d2[maxplanes] = {  3., 11.0, 11.0, 11.0 ,3.,0.};
    memcpy(_plane_d2,plane_d2,maxplanes*sizeof(_plane_d2[0]));
    _plane_d2[5]= Plane6EnvelopSize[2]/2.;

    //! To account for the envelop assimmetry of external planes
    const  double dz[maxplanes]={0.,0.,0.,0.,0.,0.};
    memcpy(_dz,dz,maxplanes*sizeof(_dz[0]));
    _dz[0]=-1.*(_plane_d2[0]*2.-_sup_hc_w[0])/2.;
    _dz[4]=(_plane_d2[4]*2.-_sup_hc_w[4])/2.;

    //! To account for the envelop assimmetry of external planes
    if(_setup==3){
      _dz[0]=-1.*(_plane_d2[0]*2.-_sup_hc_w[0])/2.;
      
      _dz[4]=(_plane_d2[4]*2.-_sup_hc_w[4])/2.;
      
      _dz[5]=(Plane6EnvelopSize[2]-Plane6Size[2])/2.; 
    }

// // Ladder support whickness in cm
// double TrGeom::sup_foam_w   = 0.5;
// // Ladder support z-offset in cm
// double TrGeom::sup_foam_tol = 0.05;
//----------------------------------------------------------------------------------
//         Plane 1N Support
//----------------------------------------------------------------------------------

    P1NSupportRadius=222./2.;
    P1NSupportThickness=7.;
    P1NSupportSkinThickness=0.25;
    P1NSCRadius=94.5;
    P1NSCThickness=10.;

    P1NSupportCoo[0]=0;
    P1NSupportCoo[1]=0;
    P1NSupportCoo[2]= _zpos[4]+_sup_hc_w[4]/2.+
      //                                              space between 1N and 1NS
      P1NSupportSkinThickness+P1NSupportThickness/2.+0.12;

//----------------------------------------------------------------------------------
//            LAYERS
//----------------------------------------------------------------------------------
    const int plane_layer[maxlay]={1,2,2,3,3,4,4,5,6};
    memcpy(_plane_layer,plane_layer,maxlay*sizeof(plane_layer[0]));



 
    
    const number layer_deltaZ[maxlay]={-3.08, 1.698,-2.318, 1.698,-2.318, 2.318,-1.698, 3.08,1.698};
    memcpy(_layer_deltaZ,layer_deltaZ,maxlay*sizeof(layer_deltaZ[0]));


//----------------------------------------------------------------------------------
//            LADDERS
//----------------------------------------------------------------------------------
    
    _ladder_Ypitch      = 7.3  ; //cm
//  _ladder_Xseparation = 0.05 ; //cm
    _ladder_Xseparation = 0.085; //cm Estimated from alignment calibration (SH)


    const number ladder_offsetX_inner[maxlad]=
      //  1     2     3     4     5      6       7     8     9     10      11      12     13    14    15 //side 0
      {   0,    0,-0.025,2.045,-2.095, -0.025, -0.025, 0, -0.025, -0.025,-2.095, 2.045, -0.025,  0,    0};
    
    memcpy(_ladder_offsetX_inner,ladder_offsetX_inner,maxlad*sizeof(ladder_offsetX_inner[0]));
    
    const number ladder_offsetX_outer[maxlad]=
      //  1     2     3     4     5      6       7     8     9     10      11      12     13    14    15 //side 0
//      {   0, 2.045,   0,    0, -2.095,   0,      0,    0,    0,     0, -2.095,      0,     0, +2.045,  0};
// Estimated from alignment calibration (SH)
      {   0, 2.070,   0,    0, -2.070,   0,      0,    0,    0,     0, -2.070,      0,     0, +2.070,  0};
    memcpy(_ladder_offsetX_outer,ladder_offsetX_outer,maxlad*sizeof(ladder_offsetX_outer[0]));


    // Layer 9 Y positions
    float  lay9Ypos[2][8]={
      { +22.32555, +15.05385,  +7.74995,  +0.44685,  -7.56255, -14.86055, -22.15345, -29.45685},
      { +29.52105, +22.22935, +14.91995,  +7.63025,  -0.38485,  -7.68985, -14.97395, -22.27445}
    };


    // float lay9Ypos[2][8]={
    //   {22.3968, 15.1251, 7.8212, 0.5181, -7.4913, -14.7893, -22.0822, -29.3856},
    //   {29.4498, 22.1581, 14.8487, 7.5590, -0.4561, -7.7611, -15.0452, -22.3457}
    // };

    memcpy(_lay9Ypos,lay9Ypos,8*2*sizeof(lay9Ypos[0][0]));

    // Layer 9 Z rotations
    float lay9Zrot[2][8]={
      { 0      ,  0.00031,  0.00062,  0.00037,  0.00013,  0.00036,  0.00053,  0.00033},
      {M_PI + 0.00064, M_PI + 0.00017, M_PI + 0.00068, 
       M_PI + 0.00035, M_PI + 0.00029, M_PI + 0.00051, 
       M_PI + 0.00019, M_PI - 0.00006}
    };

    memcpy(_lay9Zrot,lay9Zrot,8*2*sizeof(lay9Zrot[0][0]));


    const integer nlad[2][maxlay]={{15,12,11,10,10,11,12,15,8},{15,12,11,10,10,11,12,15,8}};    
    memcpy(_nlad,nlad,2*maxlay*sizeof(nlad[0][0]));
    
    const short int nsen[2][maxlay][maxlad]={
      //  1     2     3     4     5      6    7     8     9     10    11    12    13    14     15 //side 0
      {{ 10  , 12  , 13  , 14  , 14  ,  15 , 15  , 15  , 15  ,  15 ,  14 ,  14 ,  13 ,  12 ,   10}, //layer1
       {  7  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer2
       {  0  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer3
       {  0  ,  0  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer4
       {  0  ,  0  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer5
       {  0  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer6
       {  7  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer7
       { 10  , 12  , 13  , 14  , 14  ,  15 , 15  , 15  , 15  ,  15 ,  14 ,  14 ,  13 ,  12 ,   10}, //layer8
       {  0  ,  0  ,  0  ,  0  ,  0  ,   0 ,  0  ,  0  ,  0  ,   0 ,   0 ,   0 ,   0 ,   0 ,    0}, //layer9
      },
      //  1     2     3     4     5      6    7     8     9     10    11    12    13    14     15 //side 1
      {{ 10  , 11  , 13  , 14  , 15  ,  15 , 15  , 15  , 15  ,  15 ,  15 ,  14 ,  13 ,  11 ,   10}, //layer1
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    7}, //layer2
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    0}, //layer3
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,   0 ,    0}, //layer4
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,   0 ,    0}, //layer5
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    0}, //layer6
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    7}, //layer7
       { 10  , 11  , 13  , 14  , 15  ,  15 , 15  , 15  , 15  ,  15 ,  15 ,  14 ,  13 ,  11 ,   10}, //layer8
       {  0  ,  0  ,  0  ,  0  ,  0  ,   0 ,  0  ,  0  ,  0  ,   0 ,   0 ,   0 ,   0 ,   0 ,    0}, //layer9
      }

    };

    const short int nsenP[2][maxlay][maxlad]={
      //  1     2     3     4     5      6    7     8     9     10    11    12    13    14     15 //side 0
      {{  0  ,  0  , 13  , 14  , 14  ,  15 , 15  , 15  , 15  ,  15 ,  14 ,  14 ,  13 ,   0 ,    0}, //layer1
       {  7  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer2
       {  0  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer3
       {  0  ,  0  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer4
       {  0  ,  0  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer5
       {  0  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer6
       {  7  , 14  ,  9  , 11  , 11  ,  12 , 12  ,  0  , 12  ,  12 ,  11 ,  11 ,   9 ,   0 ,    0}, //layer7
       {  0  , 12  , 13  , 14  , 14  ,  15 , 15  , 15  , 15  ,  15 ,  14 ,  14 ,  13 ,  12 ,    0}, //layer8
       { 11  , 12  , 12  , 12  , 12  ,  12 , 12  , 11 ,   0  ,   0 ,   0 ,   0 ,   0 ,   0 ,    0}, //layer9
      },
      //  1     2     3     4     5      6    7     8     9     10    11    12    13    14     15 //side 1
      {{  0  ,  0  , 13  , 14  , 15  ,  15 , 15  , 15  , 15  ,  15 ,  15 ,  14 ,  13 ,   0 ,    0}, //layer1
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    7}, //layer2
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    0}, //layer3
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,   0 ,    0}, //layer4
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,   0 ,    0}, //layer5
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    0}, //layer6
       {  0  ,  0  ,  9  , 10  , 12  ,  12 , 12  ,  0  , 12  ,  12 ,  12 ,  10 ,   9 ,  14 ,    7}, //layer7
       {  0  , 11  , 13  , 14  , 15  ,  15 , 15  , 15  , 15  ,  15 ,  15 ,  14 ,  13 ,  11 ,    0}, //layer8
       { 10  , 10  , 10  , 10  , 10  ,  10 , 10  , 10  ,  0  ,   0 ,   0 ,   0 ,   0 ,   0 ,    0}, //layer9
      }

    };


    if(setup==3){
      memcpy(_nsen,nsenP,2*maxlay*maxlad*sizeof(nsen[0][0][0]));
    }else{
      memcpy(_nsen,nsen,2*maxlay*maxlad*sizeof(nsen[0][0][0]));
    }
    /// OLD MAP CABLING PRE-INTEGRATION
    const short int octid_old[2][maxlay][maxlad]={
      //    1     2     3     4     5      6    7     8     9     10    11    12    13   14     15 //side 0
      {{   211,  206,  210,  213,  216,  212, -410,  406, -411,  412,  416,  413,  400,  404,  401, },
       {   202,  203,  214,  222,  220, -208,  200,    0, -402,  414,  422,  420,  408,    0,    0, },
       {     0,  207,  219,  223,  217, -205,  201,    0, -407,  419,  423,  417,  405,    0,    0, },
       {     0,    0,  215,  218,  221, -209,  204,    0, -403,  415,  418,  421,  409,    0,    0, },
       {     0,    0,  621,  616,  615, -603,  606,    0, -809,  821,  816,  815,  803,    0,    0, },
       {     0,  605,  617,  613,  619, -607,  611,    0, -805,  817,  813,  819,  807,    0,    0, },
       {   608,  609,  620,  612,  614, -602,  610,    0, -808,  820,  812,  814,  802,    0,    0, },
       {   601,  604,  600,  623,  618,  622, -800,  804, -801,  822,  818,  823,  810,  806,  811, },
       {     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, },
      },
      {{   101,  104,  100,  113,  116, -112,  111, -106,  110, -312,  316,  313,  310,  306,  311, },
       {     0,    0,  108,  120,  122, -114, -102,    0,  300, -308,  320,  322,  314,  303,  302, },
       {     0,    0,  105,  117,  123, -119, -107,    0,  301, -305,  317,  323,  319,  307,    0, },
       {     0,    0,  109,  121,  118, -115, -103,    0,  304, -309,  321,  318,  315,    0,    0, },
       {     0,    0,  503,  515,  516, -521, -509,    0,  706, -703,  715,  716,  721,    0,    0, },
       {     0,    0,  507,  519,  513, -517, -505,    0,  711, -707,  719,  713,  717,  701,    0, }, //WARNING 705 AND 701 EXCHANGED
       {     0,    0,  502,  514,  512, -520, -508,    0,  710, -702,  714,  712,  720,  709,  708, },
       {   511,  506,  510,  523,  518, -522, -501,  504,  500, -722,  718,  723,  700,  704,  705, },
       {     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, },
      },
    };


    /// NEW MAP CABLING FLIGHT
    const short int octid_new[2][maxlay][maxlad]={
      //    1     2     3     4     5      6    7     8     9     10    11    12    13   14     15 //side 0
      {{   211,  202,  210,  213,  216,  212, -410,  406, -411,  412,  416,  413,  400,  404,  401, },
       {   206,  203,  214,  222,  220, -208,  200,    0, -402,  414,  422,  420,  408,    0,    0, },
       {     0,  207,  219,  223,  217, -205,  201,    0, -407,  419,  423,  417,  405,    0,    0, },
       {     0,    0,  215,  218,  221, -209,  204,    0, -403,  415,  418,  421,  409,    0,    0, },
       {     0,    0,  621,  616,  615, -603,  606,    0, -809,  821,  816,  815,  803,    0,    0, },
       {     0,  605,  617,  613,  619, -607,  611,    0, -805,  817,  813,  819,  807,    0,    0, },
       {   604,  609,  620,  612,  614, -602,  610,    0, -808,  820,  812,  814,  802,    0,    0, },
       {   601,  608,  600,  623,  618,  622, -800,  804, -801,  822,  818,  823,  810,  806,  811, },
       {     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, },
      },
      {{   101,  104,  100,  113,  116, -112,  111, -106,  110, -312,  316,  313,  310,  302,  311, },
       {     0,    0,  108,  120,  123, -114, -102,    0,  300, -308,  320,  322,  314,  303,  306, },
       {     0,    0,  105,  117,  122, -119, -107,    0,  301, -305,  317,  323,  319,  307,    0, },
       {     0,    0,  109,  121,  118, -115, -103,    0,  304, -309,  321,  318,  315,    0,    0, },
       {     0,    0,  503,  515,  516, -521, -509,    0,  706, -703,  715,  716,  721,    0,    0, },
       {     0,    0,  507,  519,  513, -517, -505,    0,  711, -707,  719,  713,  717,  705,    0, },
       {     0,    0,  502,  514,  512, -520, -508,    0,  710, -702,  714,  712,  720,  709,  704, },
       {   511,  506,  510,  523,  518, -522,  501, -504,  500, -722,  718,  723,  700,  708,  701, },
       {     0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, },
      },
    };
    //// CABLING MAP AMS02P PZ FIXME RANDOM MAP for lay9
    const short int octid_newP[2][maxlay][maxlad]={
      //    1     2     3     4     5      6    7     8     9     10    11    12    13   14     15 //side 0
      {{     0,    0,  210,  213,  216,  212, -410,  406, -411,  412,  416,  413,  400,    0,    0, },
       {    -1,  207,  214,  222,  220, -208,  200,    0, -402,  414,  422,  420,  408,    0,    0, },
       {     0,  211,  219,  223,  217, -205,  201,    0, -407,  419,  423,  417,  405,    0,    0, },
       {     0,    0,  215,  218,  221, -209,  204,    0, -403,  415,  418,  421,  409,    0,    0, },
       {     0,    0,  621,  616,  615, -603,  606,    0, -809,  821,  816,  815,  803,    0,    0, },
       {     0,  601,  617,  613,  619, -607,  611,    0, -805,  817,  813,  819,  807,    0,    0, },
       {    -1,  605,  620,  612,  614, -602,  610,    0, -808,  820,  812,  814,  802,    0,    0, },
       {     0,  104,  101,  518,  510,  511,  506,  700,  709,  708,  704,  303,  306,  302,    0, },
       {   622,  618,  623,  801,  804,  800,  822,  823,    0,    0,    0,    0,    0,    0,    0, },
      },
      {{     0,    0,  100,  113,  116, -112,  111, -106,  110, -312,  316,  313,  310,    0,    0, },
       {     0,    0,  108,  120,  123, -114, -102,    0,  300, -308,  320,  322,  314,  307,   -1, },
       {     0,    0,  105,  117,  122, -119, -107,    0,  301, -305,  317,  323,  319,  311,    0, },
       {     0,    0,  109,  121,  118, -115, -103,    0,  304, -309,  321,  318,  315,    0,    0, },
       {     0,    0,  503,  515,  516, -521, -509,    0,  706, -703,  715,  716,  721,    0,    0, },
       {     0,    0,  507,  519,  513, -517, -505,    0,  711, -707,  719,  713,  717,  701,    0, },
       {     0,    0,  502,  514,  512, -520, -508,    0,  710, -702,  714,  712,  720,  705,   -1, },
       {     0,  202,  206,  203,  600,  604,  608,  609,  806,  811,  810,  818,  401,  404,    0, },
       {   522,  523,  504,  500,  501,  723,  722,  718,    0,    0,    0,    0,    0,    0,    0, },
      },
    };
    
    
    if(setup==3){
      memcpy(_octid,octid_newP,2*maxlay*maxlad*sizeof(octid_new[0][0][0]));
      printf("TkDBC--using the FLIGHT AMS02P Cabling %d!!!\n",setup);
    }else if(setup==2){
      memcpy(_octid,octid_new,2*maxlay*maxlad*sizeof(octid_new[0][0][0]));
      printf("TkDBC--using the FLIGHT Cabling %d!!!\n",setup);
    }else if(setup==1){
      memcpy(_octid,octid_old,2*maxlay*maxlad*sizeof(octid_old[0][0][0]));
      printf("TkDBC--using the PRE-INTEGRATION Cabling  %d!!!\n",setup);
    }else{
      printf("TkDBC--FATAL-- Unknown setup number %d!!\n I give up!!!\n",setup);
      exit(-2);
    }

    
    const float LadDeltaX[2][maxlay][maxlad]={{
	//1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
	{ -0.39,   20.31, -0.39, -0.39,	-21.09,	-0.39,	-0.39,	-0.39,	-0.39,	-0.39,	-21.09,	-0.39,	-0.39,	20.31,	-0.39}, //1
	{144.88,  289.55, -0.25, 20.45,	-20.95,	-0.25,	-0.25,	0.,	-0.25,	-0.25,	-20.95,	20.45,	-0.25,	0,	0},//2
	{  0.  ,  289.41, -0.39, 20.31,	-21.09,	-0.25,	-0.39,	0.,	-0.25,	-0.39,	-21.09,	20.31,	-0.39,	0,	0},//3
	{  0.  ,    0.,	  -0.25, 20.31,	-20.95,	-0.25,	-0.25,	0.,	-0.32,	-0.25,	-21.09,	20.38,	-0.32,	0,	0},//4
	{  0.  ,    0.,	  -0.39, 20.31,	-21.09,	-0.39,	-0.39,	0.,	-0.25,	-0.39,	-21.09,	20.31,	-0.39,	0,	0},//5
	{  0.  , 289.55,  -0.39, 20.31,	-21.09,	-0.32,	-0.39,	0.,	-0.25,	-0.39,	-21.09,	20.31,	-0.39,	0,	0},//6
	{144.88, 289.41,  -0.39, 20.31,	-21.09,	-0.25,	-0.39,	0.,	-0.25,	-0.39,	-21.09,	20.31,	-0.39,	0,	0},//7
	{ -0.39,  20.31,  -0.39, -0.39,	-21.09,	-0.39,	-0.39,	-0.39,	-0.39,	-0.39,	-21.09,	-0.39,	-0.39,	20.31,	-0.39},//8
	{+20.04, +40.630,+40.700, +40.600,   +40.540,+40.700,+40.500, +19.900 ,     0.,     0.,     0.,     0.,     0.,     0.,     0.},//9
      },{
	
														
	//1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
	{0.39,	21.09,	0.39,	0.39,	-20.31, 0.39,	0.39,	0.39,	0.39,	0.39,	-20.31,	0.39,	0.39,	  21.09,    0.39},
	{0.,	0,	0.25,	20.95,	-20.45,	0.25,	0.25,	0,	0.25,	0.25,	-20.45,	20.95,	0.25,	-289.55, -144.88},
	{0.,	0,	0.25,	20.95,	-20.31,	0.39,	0.39,	0,	0.39,	0.39,	-20.31,	21.09,	0.25,	-289.41,    0.  },
	{0.,	0,	0.32,	20.95,	-20.45,	0.25,	0.25,	0,	0.25,	0.25,	-20.45,	20.95,	0.32,	   0.  ,    0.  },
	{0.,	0,	0.35,	21.09,	-20.31,	0.25,	0.39,	0,	0.39,	0.39,	-20.45,	21.09,	0.39,	   0.  ,    0.  },
	{0.,	0,	0.39,	21.09,	-20.31,	0.39,	0.32,	0,	0.39,	0.39,	-20.31,	21.02,	0.39,	-289.41,    0.  },
	{0.,	0,	0.39,	21.09,	-20.31,	0.25,	0.32,	0,	0.25,	0.32,	-20.31,	21.02,	0.39,	-289.41, -144.74},
	{0.39,	21.09,	0.39,	0.39,	-20.31,	0.39,	0.39,	0.39,	0.39,	0.39,	-20.31,	0.39,	0.39,	  21.09,    0.39},
	{+20.79, +41.49, +41.46,+41.42, +41.42,+41.38, +41.32, +20.59,     0.,     0.,     0.,     0.,     0.,     0.,    0.},//9
      }};

    // memcpy(_LadDeltaX,LadDeltaX,2*nlays*maxlad*sizeof(LadDeltaX[0][0][0]));
    for (int s=0;s<2;s++)
      for (int la=0;la<maxlay;la++)
	for (int sl=0;sl<maxlad;sl++)
	  _LadDeltaX[s][la][sl]=LadDeltaX[s][la][sl];
    
    const char LadName[2][maxlay][maxlad][9]={
      {{  "L10AO151", "L12SO181", "L13AO073", "L14AO123", "L14AO125", "L15SO180", "L15AN101", "L15AO173", "L15AN100", "L15AO093", "L14AO159", "L14AO160", "L13AO114", "L12AO107", "L10AO145", },
       {  "L07PI002", "L14AI031", "L09GI003", "L11GI004", "L11GI008", "L12AH024", "L12AI004", "        ", "L12AH029", "L12AI005", "L11GI009", "L11GI016", "L09GI010", "        ", "        ", },
       {  "        ", "L14AI032", "L09AI139", "L11AI127", "L11AI140", "L12AJ055", "L12AJ038", "        ", "L12AJ054", "L12SI182", "L11SI176", "L11AI126", "L09SI164", "        ", "        ", },
       {  "        ", "        ", "L09GI005", "L11GI017", "L11AI082", "L12AH060", "L12AI048", "        ", "L12AH059", "L12AI064", "L11AI083", "L11AI081", "L09AI076", "        ", "        ", },
       {  "        ", "        ", "L09AI090", "L11AI087", "L11AI085", "L12AJ034", "L12AI049", "        ", "L12AJ053", "L12AI017", "L11AI086", "L11AI143", "L09AI092", "        ", "        ", },
       {  "        ", "L14AI041", "L09AI089", "L11AI084", "L11GI001", "L12AH030", "L12AI067", "        ", "L12AH028", "L12SI170", "L11AI141", "L11AI104", "L09GI012", "        ", "        ", },
       {  "L07PI003", "L14AI040", "L09AI103", "L11AI128", "L11AI088", "L12AJ020", "L12AI007", "        ", "L12AJ056", "L12AI130", "L11AI134", "L11AI142", "L09PI010", "        ", "        ", },
       {  "L10AO144", "L12AO105", "L13AO026", "L14AO119", "L14AO120", "L15AO175", "L15AP098", "L15AO097", "L15AP110", "L15AO095", "L14AO122", "L14AO124", "L13AO074", "L12AO106", "L10AO148", },
       {  "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", },
      },
      {{  "L10AO149", "L11AO154", "L13AO071", "L14AO156", "L15AO163", "L15AN113", "L15AO177", "L15AP109", "L15AO174", "L15AP099", "L15AO115", "L14AO158", "L13AO025", "L11AO153", "L10AO150", },
       {  "        ", "        ", "L09GI006", "L10PI005", "L12AI001", "L12AH057", "L12AJ021", "        ", "L12AI006", "L12AJ051", "L12AI003", "L10GI014", "L09GI013", "L14AI042", "L07PI004", },
       {  "        ", "        ", "L09GI011", "L10PI001", "L12AI012", "L12AJ033", "L12SH169", "        ", "L12SI167", "L12AH058", "L12AI016", "L10PI008", "L09GI002", "L14AI044", "        ", },
       {  "        ", "        ", "L09AI075", "L10AI079", "L12AI015", "L12AH061", "L12AJ018", "        ", "L12AI063", "L12AJ019", "L12AI065", "L10GI015", "L09AI077", "        ", "        ", },
       {  "        ", "        ", "L09AI138", "L10PI009", "L12AI132", "L12AJ022", "L12AH035", "        ", "L12AI131", "L12AH036", "L12AI066", "L10PI013", "L09AI136", "        ", "        ", },
       {  "        ", "        ", "L09AI135", "L10AI080", "L12AH062", "L12AH037", "L12AJ023", "        ", "L12SI171", "L12AJ052", "L12SI168", "L10AI078", "L09PI012", "L14AI043", "        ", },
       {  "        ", "        ", "L09AI091", "L10PI011", "L12AI133", "L12AJ047", "L12AH045", "        ", "L12AI068", "L12AH046", "L12AI050", "L10PI007", "L09AI102", "L14AI039", "L07PI006", },
       {  "L10AO147", "L11AO155", "L13AO072", "L14AO121", "L15AO116", "L15AP112", "L15AO162", "L15AN117", "L15AO096", "L15AN118", "L15SO178", "L14AO157", "L13AO111", "L11SO165", "L10AO146", },
       {  "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", "        ", },
      },
    };

    //PZ FIXME fix the ladder names when the map for plane 6 is ready
    const char LadNameP[2][maxlay][maxlad][9]={
      {{  "        ", "        ", "L13AO073", "L14AO123", "L14AO125", "L15SO180", "L15AN101", "L15AO173", "L15AN100", "L15AO093", "L14AO159", "L14AO160", "L13AO114", "        ", "        ", },
       {  "L07PI002", "L14AI031", "L09GI003", "L11GI004", "L11GI008", "L12AH024", "L12AI004", "        ", "L12AH029", "L12AI005", "L11GI009", "L11GI016", "L09GI010", "        ", "        ", },
       {  "        ", "L14AI032", "L09AI139", "L11AI127", "L11AI140", "L12AJ055", "L12AJ038", "        ", "L12AJ054", "L12SI182", "L11SI176", "L11AI126", "L09SI164", "        ", "        ", },
       {  "        ", "        ", "L09GI005", "L11GI017", "L11AI082", "L12AH060", "L12AI048", "        ", "L12AH059", "L12AI064", "L11AI083", "L11AI081", "L09AI076", "        ", "        ", },
       {  "        ", "        ", "L09AI090", "L11AI087", "L11AI085", "L12AJ034", "L12AI049", "        ", "L12AJ053", "L12AI017", "L11AI086", "L11AI143", "L09AI092", "        ", "        ", },
       {  "        ", "L14AI041", "L09AI089", "L11AI084", "L11GI001", "L12AH030", "L12AI067", "        ", "L12AH028", "L12SI170", "L11AI141", "L11AI104", "L09GI012", "        ", "        ", },
       {  "L07PI003", "L14AI040", "L09AI103", "L11AI128", "L11AI088", "L12AJ020", "L12AI007", "        ", "L12AJ056", "L12AI130", "L11AI134", "L11AI142", "L09PI010", "        ", "        ", },
       {  "        ", "L12AO105", "L13AO026", "L14AO119", "L14AO120", "L15AO175", "L15AP098", "L15AO097", "L15AP110", "L15AO095", "L14AO122", "L14AO124", "L13AO074", "L12AO106", "        ", },
       {  "L11AO153", "L12S0181", "L12AI070", "L12SO166", "L12AO107", "L12AI009", "L12AI002", "L11AO154", "        ", "        ", "        ", "        ", "        ", "        ", "        ", },
      },
      {{  "        ", "        ", "L13AO071", "L14AO156", "L15AO163", "L15AN113", "L15AO177", "L15AP109", "L15AO174", "L15AP099", "L15AO115", "L14AO158", "L13AO025", "        ", "        ", },
       {  "        ", "        ", "L09GI006", "L10PI005", "L12AI001", "L12AH057", "L12AJ021", "        ", "L12AI006", "L12AJ051", "L12AI003", "L10GI014", "L09GI013", "L14AI042", "L07PI004", },
       {  "        ", "        ", "L09GI011", "L10PI001", "L12AI012", "L12AJ033", "L12SH169", "        ", "L12SI167", "L12AH058", "L12AI016", "L10PI008", "L09GI002", "L14AI044", "        ", },
       {  "        ", "        ", "L09AI075", "L10AI079", "L12AI015", "L12AH061", "L12AJ018", "        ", "L12AI063", "L12AJ019", "L12AI065", "L10GI015", "L09AI077", "        ", "        ", },
       {  "        ", "        ", "L09AI138", "L10PI009", "L12AI132", "L12AJ022", "L12AH035", "        ", "L12AI131", "L12AH036", "L12AI066", "L10PI013", "L09AI136", "        ", "        ", },
       {  "        ", "        ", "L09AI135", "L10AI080", "L12AH062", "L12AH037", "L12AJ023", "        ", "L12SI171", "L12AJ052", "L12SI168", "L10AI078", "L09PI012", "L14AI043", "        ", },
       {  "        ", "        ", "L09AI091", "L10PI011", "L12AI133", "L12AJ047", "L12AH045", "        ", "L12AI068", "L12AH046", "L12AI050", "L10PI007", "L09AI102", "L14AI039", "L07PI006", },
       {  "        ", "L11AO155", "L13AO072", "L14AO121", "L15AO116", "L15AP112", "L15AO162", "L15AN117", "L15AO096", "L15AN118", "L15SO178", "L14AO157", "L13AO111", "L11SO165", "        ", },
       {  "L10AO144", "L10AO145", "L10AO146", "L10AO147", "L10AO148", "L10AO149", "L10AO150", "L10AO151", "        ", "        ", "        ", "        ", "        ", "        ", "        ", },
      },
    };
    
    if(_setup==3){
      memcpy(_LadName,LadNameP,2*maxlay*maxlad*9*sizeof(LadName[0][0][0][0]));
    }else{
      memcpy(_LadName,LadName,2*maxlay*maxlad*9*sizeof(LadName[0][0][0][0]));
    }
     

//---------------- Crate to Octant to JinJ cable connection --------------
    


    const short int octant_crate[8]={0,1,3,2,4,5,7,6};
    memcpy(_octant_crate,octant_crate,8*sizeof(octant_crate[0]));

    const short int octant_JinJ[8]={3,9,1,0,16,17,23,22};
    memcpy(_octant_JinJ,octant_JinJ,8*sizeof(octant_crate[0]));

    const short int octant_JMDC[8][4]=    
      {{166,  167, 168, 169},
       {170,  171, 172, 173},
       {178,  179, 180, 181},
       {174,  175, 176, 177},
       {182,  183, 184, 185},
       {186,  187, 188, 189},
       {194,  195, 196, 197},
       {190,  191, 192, 193}};
    memcpy(_octant_JMDC,octant_JMDC,8*4*sizeof(octant_JMDC[0][0]));

    


    //  Read or Generate the Geometry

    int ret = 0;
    if (inputfilename) {
      ret = read(inputfilename);
      if (ret == -1) { 
	cerr<<"TkDBc:: FATAL Cannot open the requested Geometry file "<< inputfilename<<endl;
	exit(2);
      }
    }
    else{ // generate the geometry

      cerr <<" TkDBc:: Generating the Tracker geometry from nominal positions!"<<endl;
      //create the planes objects
      for (int ii=0;ii<nplanes;ii++){
	sprintf(name,"Plane%d",ii+1);
	planes[ii]=new TkPlane(name,ii+1,_nslot[ii]);
	planes[ii]->setpos(_xpos[ii],_ypos[ii],_zpos[ii]);
	if(_setup==3 && ii==4) {
	  planes[ii]->rot.XParity();
	  planes[ii]->rot.ZParity();
	}
	//	 cout <<*(planes[ii])<<endl;
      }
      
      for (int lay=0;lay<nlays;lay++) //loop on layers
	for (int side=0;side<2;side++)
          for (int slot=0;slot<maxlad;slot++)
            if(filled_slot(side,lay,slot)){
              int oct=GetOctant(side,lay+1,slot+1);
	      if  (_setup==3 ){
		oct=abs(_octid[side][lay][slot])/100;
	      }
              int hwid=0;
	      if(_octid[side][lay][slot]) hwid=abs(_octid[side][lay][slot])-(100*(oct))+(_octant_crate[oct-1]*100);
	      if(_octid[side][lay][slot]==-1) hwid=-1;
	      int tkid=(lay+1)*100+(slot+1);
              if (side==0) tkid*=-1;
              sprintf(name,"%s",_LadName[side][lay][slot]);
              TkLadder* aa= new TkLadder(planes[_plane_layer[lay]-1],name,tkid,hwid,_nsen[side][lay][slot]);
              if(_octid[side][lay][slot]<0 && _octid[side][lay][slot]!=-1 ) aa->SetLaserFlag();
	      if((lay+1)==1 || (lay+1) ==8) aa->SetAsK7();
	      // Special for k5/k7 on layer 9/plane6
	      if((lay+1)==9 && side==1 ) aa->SetAsK7();
	      if((lay+1)==9 && side==0 ) 
		if((slot+1)!=3 &&(slot+1)!=6 && (slot+1)!=7)
		  aa->SetAsK7();
              number posz= _layer_deltaZ[lay];
              number posy= GetSlotY(lay+1,slot+1,side);
              number posx= GetSlotX(lay+1,slot+1,side);
	      aa->setpos(posx,posy,posz);
	      aa->rot.Reset();
	      
	      // Rotation along X-axis
	      if(lay%2==0 && (lay+1)!=9) {
		aa->rot.YParity(); 
		aa->rot.ZParity(); 
	      }
	      // Rotation along Z-axis
	      if(side && (lay+1)!=9   ) {
		aa->rot.XParity(); 
		aa->rot.YParity(); 
	      }
	      
	      if(lay+1==9){
		aa->SetRotAngles(_lay9Zrot[side][slot],0.,0.);
	      }

// 	      int pgid=aa->GetPgId();
// 	      _pgid[side][lay][slot]=pgid;
	      
	      if((lay+1)!=9){
		tkassemblymap[aa->GetAssemblyId()]=aa;
	      }
	      tkidmap[tkid]=aa;
	      hwidmap[hwid]=aa;
	      // SH FIXME pgid is not an unique ID for 192 ladders
	      //              pgidmap[pgid]=aa;
 	      string bb=name;
	      lnamemap[bb]=aa;
	      JMDCNumMap[aa->GetJMDCNum()]=aa;
//	      cout <<"layer "<<lay+1<<" slot  "<<slot+1<<"  side "<<side<<" tkid "<<tkid<<" hwid "<< aa->GetHwId()<<aa->GetPos()<<endl;
	    }
    }

  // SH FIXME size of pgidmap is only 24
  cout << "TkDBc::Maps have been filled: "
       << "tk= " << tkidmap      .size() << " "
       << "hw= " << hwidmap      .size() << " "
       << "ln= " << lnamemap     .size() << " "
       << "as= " << tkassemblymap.size() << " "
    //       << "pg= " << pgidmap      .size() << " "
       << "JN= " << JMDCNumMap   .size() << endl;

    // Set sensor alignment data
    for (tkidIT lad=tkidmap.begin(); lad!=tkidmap.end();++lad){
      //      if(lad->second->GetLayer()==9){ printf("TkDBC::init -W- PLANB No sensor alignement for layer 9! \n"); continue;}
      for (int i = 0; i < trconst::maxsen; i++) 
	lad->second->_sensx[i] = GetSensAlignX(lad->second->GetTkId(), i);

      for (int i = 0; i < trconst::maxsen; i++) 
	lad->second->_sensy[i] = GetSensAlignY(lad->second->GetTkId(), i);
    }
    
//   for (tkidIT pp=tkidmap.begin(); pp!=tkidmap.end();++pp){
//       cout<<*(pp->second)<<endl;
//}

    return;
}


number TkDBc::GetSlotY(int layer, int slot,int side){
  number ladpitch= _ladder_Ypitch;
  number ladder_Ygap= _ladder_Ypitch - _ssize_inactive[1];
  number central_gap= 0.8  ; // layer 9 central additional gap
  //Y coo of  the first S/p readout channel for the ladder on slot 9 X negative
  // 0.5 ladder_Ygap + distance from first readout channel to near ladder edge
  number distXN = (_ssize_inactive[1]-_ssize_active[1])/2 + 0.5 * ladder_Ygap;

  //Y coo of  the first S/p readout channel for the ladder on slot 9 X positive
  // 0.5 ladder_Ygap + distance from first readout channel to far ladder edge
  number distXF = _ssize_inactive[1]-(_ssize_inactive[1]-_ssize_active[1])/2 + 0.5 * ladder_Ygap;

  number distX2=_ssize_active[1]/2.;
  
//   printf("  _ladder_Ypitch %f\n", _ladder_Ypitch );
//   printf(" ladpitch %f ladder_Ygap %f distXN %f distXF  %f \n",_ladder_Ypitch,ladder_Ygap,distXN,distXF);
  number addspaces=0;
  if(abs(slot)>4) addspaces=ladder_Ygap-central_gap/2.;
  else            addspaces=central_gap/2.;
  if(layer==9){
    if(side==0)
      return _lay9Ypos[side][slot-1]+(_ssize_inactive[1]-_ssize_active[1])/2;
    else
      return _lay9Ypos[side][slot-1]-(_ssize_inactive[1]-_ssize_active[1])/2;
//     if(side==0) 
//       return  (distXF - ladder_Ygap/2 + ladpitch*(3-slot) + addspaces)
// 	     +0.3;  // SH: coarse alignment with CR data (Aug/2010)
//            //+ (slot<5)?central_gap/2.: (ladder_Ygap-central_gap)/2. ;
//     else
//       return  (distXN - ladder_Ygap/2 + ladpitch*(5-slot) + addspaces)
// 	     -0.1;  // SH: coarse alignment with CR data (Aug/2010)
           //+ (slot<5)?central_gap/2.: (ladder_Ygap-central_gap)/2.; 
  }
  if( layer==2||layer==4||layer==6){
    if(side==0){
      if(abs(slot)<8)      return -distXF+ ladpitch*(8-slot);
      else if(abs(slot)>8) return -distXF+ ladpitch*(9-slot);
      else return -8888;
    }else if (side==1){
      if(abs(slot)<8)      return -distXN+ ladpitch*(8-slot);
      else if(abs(slot)>8) return -distXN+ ladpitch*(9-slot);
      else return -8888;
    } else return -9999;
  }


  if( layer==3||layer==5||layer==7){
    if(side==0){
      if(abs(slot)<8)      return -distXN+ ladpitch*(8-slot);
      else if(abs(slot)>8) return -distXN+ ladpitch*(9-slot);
      else return -8888;
    }else if (side==1){
      if(abs(slot)<8)      return -distXF+ ladpitch*(8-slot);
      else if(abs(slot)>8) return -distXF+ ladpitch*(9-slot);
      else return -8888;
    } else return -9999;
  }
  
    
  if(layer==1&&side==0) 
    return distX2 + ladpitch*(8-slot);
  
  else if(layer==1&&side==1) 
    return -distX2 + ladpitch*(8-slot);
  
  else if(layer==8&&side==0) 
    return -distX2 + ladpitch*(8-slot);
  
  else if(layer==8&&side==1) 
    return distX2 + ladpitch*(8-slot);
  else return -9999;
      


}


number TkDBc::GetSlotX(int layer, int slot,int side){

  int sensors=_nsen[side][layer-1][slot-1];
  if(sensors==0) return 8888;

  // (N-1)*sensor pitch  X+ sensor_size -  the distance between the first readout channelX and the sensor edge
  number lenght=(sensors-1)*(_ssize_inactive[0]+0.004)+_ssize_inactive[0]- (_ssize_inactive[0] - _ssize_active[0])/2.;
  if(side==0) lenght*=-1;
  int otherside=1;
  if(side==1) otherside=0;

  // PZ Add the DIVIC infos on plane assembly
  return lenght+(_LadDeltaX[side][layer-1][slot-1]/10.);

  /*
  if(layer>1&& layer<8){
    // Estimated from alignment calibration (SH)
    number xtmp = (layer==2 || layer==3) ? -0.02 : 0;
    if (layer==3 && side==1 && slot>8) xtmp += 0.02;

    if(slot==1||slot==2||slot==14||slot==15)
    { // Estimated from alignment calibration (SH)
      number offs = (slot==1) ? 0.08 : (slot==2) ? 0.06 : (slot==14) ? 0.06 : 0;
      return (lenght-(_ssize_inactive[0]-_ssize_active[0])/2)/2+offs;
    }
    else if( slot>2&&slot<14){
      return  lenght + _ladder_offsetX_inner[slot-1]+side*_ladder_Xseparation+side*xtmp;

    }else return -9999;
  }else if( layer==1||layer==8){
    // Estimated from alignment calibration (SH)
    return lenght + _ladder_offsetX_outer[slot-1]+side*_ladder_Xseparation*1.1;
  }
  else  return -9999;
  */
}

#include <fstream>

int TkDBc::write(const char* filename){

  ofstream  fileout(filename);
  if(TkLadder::version>=2) fileout << "VVV"<<TkLadder::version<<endl;
  for (int ii=0;ii<nplanes;ii++)
    fileout<<*(planes[ii]);
  for (tkidIT pp=tkidmap.begin(); pp!=tkidmap.end(); ++pp)
    fileout << *(pp->second);

  fileout.close();
  return 0;

}

int TkDBc::read(const char* filename, int pri){
  TkLadder *aa;
  ifstream  fileout(filename);
  if (!fileout) {
    printf("Error: tkdbc file not found: %s\n", filename);
    return -1;
  }
  
  // Delete existing planes
  for (int ii=0;ii<nplanes;ii++)
    if(planes[ii]) delete planes[ii];
  
  // Delete existing ladders
  for ( tkidIT pp=tkidmap.begin();pp!=tkidmap.end();++pp)
    if(pp->second) delete pp->second;

  // Clear maps
  tkidmap.clear();
  hwidmap.clear();
  lnamemap.clear();
  tkassemblymap.clear();
  //  pgidmap.clear();
  JMDCNumMap.clear();

  string test;
  
  fileout>> test;
  if(test[0]=='V'&&test[1]=='V'&&test[2]=='V') {
    TkLadder::version=atoi(&(test[3]));
    printf("TkDBc::read-I- New Sensor Aligment Aware TkLadder format detected\n");
  }else{
    TkLadder::version=0;
    fileout.seekg(0,ios_base::beg);
  }


  for (int ii=0;ii<nplanes;ii++){
    planes[ii]= new TkPlane();
    
    fileout>>(*planes[ii]);
  }
  int count=0;
  for(;;){
    aa= new TkLadder();
    fileout >>(*aa);
    if(fileout.eof()){ delete aa; break;}
    if(!fileout.good()) cerr <<" Error in TkDBc::read the channel is not good"<<endl;
    tkidmap[aa->GetTkId()]=aa;
    hwidmap[aa->GetHwId()]=aa;
    string bb=aa->name;
    lnamemap[bb]=aa;

    int lay  = aa->GetLayer();
    int side = aa->GetSide();
    int slot = abs(aa->GetSlot());
//     int pgid = aa->GetPgId();
//     _pgid[side][lay][slot] = pgid;
    //    pgidmap[pgid]=aa;
    tkassemblymap[aa->GetAssemblyId()]=aa;
    JMDCNumMap[aa->GetJMDCNum()]=aa;

    aa->SetPlane(GetPlane(_plane_layer[aa->GetLayer()-1]));

    count++;
    if (pri) cout<<*aa<<endl;


  }

  cout << count <<" Ladders have been read from file "<<filename<<endl;


  fileout.close();
  return 0;

}

TkLadder* TkDBc::GetEntry(int ii) {
  int icrate = int(ii/24);
  int itdr   = ii%24;
  return FindHwId(icrate*100 + itdr);
}

int TkDBc::Entry2TkId(int ii) {
  int icrate = int(ii/24);
  int itdr   = ii%24;
  TkLadder* ladder = FindHwId(icrate*100 + itdr);
  if (ladder!=0) return ladder->GetTkId();
  return 0;
}

int TkDBc::TkId2Entry(int TkId) {
  TkLadder* ladder = FindTkId(TkId);
  if (ladder!=0) return ladder->GetCrate()*24 + ladder->GetTdr();
  return -1;
}

int TkDBc::GetOctant(int side,int layer,int slot){

  if(slot<1||slot>15||side>1||side<0) return -9999;

  if(layer==1){
    if(side==0){
      if(slot>=7) return 4;
      if(slot <7) return 2;
    } 
    else {
      if(slot<=9) return 1;
      if(slot >9) return 3;
    }
  }

  else if(layer==8){
    if(side==0){
      if(slot>=7) return 8;
      if(slot <7) return 6;
    } 
    else {
      if(slot<=9) return 5;
      if(slot >9) return 7;
    }
  }
  
  else if (layer==2 ||layer==3 ||layer==4){
    if(slot==8)  return -9999;

    if(slot<=7 && side==0) return 2;  
    if(slot>=9 && side==0) return 4;
    
    if(slot>=9 && side==1) return 3;
    if(slot<=7 && side==1) return 1; 
  }

  else if (layer==5 ||layer==6 ||layer==7){
    if(slot==8)  return -9999;
  
    if(slot>=9 && side==0) return 8; 
    if(slot<=7 && side==0) return 6; 
    if(slot>=9 && side==1) return 7;
    if(slot<=7 && side==1) return 5; 
  }
  
  return -9999;
  
  
}



TkLadder* TkDBc::Findmap(map<int,TkLadder*> & tkmap, int key){
  tkidIT iter=tkmap.find(key);
  return (iter== tkmap.end()?0:iter->second);
  
}

TkLadder* TkDBc::FindLadName( string& name){
  lnameIT iter=lnamemap.find(name);
  return (iter== lnamemap.end()?0:iter->second);
  
}


char * TkDBc::GetCratePos(int crate){
  for (int ii=0;ii<8;ii++){
    if(_octant_crate[ii]==crate)
      return GetOctName(ii+1);
  }
  return 0;
}
char * TkDBc::GetOctName(int oct){
  static char name[10];
#pragma omp threadprivate(name)
  if(oct==1) sprintf(name,"WAKE++");
  if(oct==2) sprintf(name,"WAKE-+");
  if(oct==3) sprintf(name,"RAM ++");
  if(oct==4) sprintf(name,"RAM -+");
  if(oct==5) sprintf(name,"WAKE+-");
  if(oct==6) sprintf(name,"WAKE--");
  if(oct==7) sprintf(name,"RAM +-");
  if(oct==8) sprintf(name,"RAM --");
  return name;
}



void TkDBc::GetLayerRot(int lay, number nrm[][3]){
  
  if( lay<1||lay>8) return;
  TkPlane* pp=GetPlane(_plane_layer[lay-1]);
  if(!pp) return ;
  for (int row=0;row<3;row++)
    for (int col=0;col<3;col++)
      nrm[row][col]=pp->rot.GetEl(row,col);
  return;
}



int TkDBc::readAlignmentAngles(const char* filename, int pri){
  FILE * ff= fopen(filename,"r");

  if(!ff) {
    fprintf(stderr,"TkDBc::readAlignmentAngles: WARNING Cannot open the alignemt file %s!!!\n",filename);
    return -1;
  }
  printf("TkDBc::readAlignmentAngles: open the alignemt file %s\n",filename);


  int tkid;
  float dx,dy,dz,theta,phi,xsi;
  while (!feof(ff)){
    fscanf(ff," %d %f %f %f %f %f %f",&tkid,&dx,&dy,&dz,&theta,&phi,&xsi);
    TkLadder* ll= FindTkId(tkid);
    if(pri) printf(" %+03d %f %f %f %f %f %f \n" ,tkid,dx,dy,dz,theta,phi,xsi);
    if(!ll) {
      fprintf(stderr,"TkDBc::readAlignmentAngles: WARNING Cannot find the ladder with tkid %+03d!!!\n",tkid);
      continue;
    }
    if(pri) printf("updating .... \n");
    ll->SetRotAnglesA(-theta/600.,-phi/600.,-xsi/600.);
    ll->setposA(dx/10.,dy/10.,dz/10.);
  }
  
  return 0;
}

int TkDBc::writeAlignmentLadFF(int tkid, const char* filename, int overwrite){
  
  ofstream *fileout;
  
  if(!overwrite) 
    fileout=new ofstream(filename,ios::app);
  else
    fileout=new ofstream(filename,ios::trunc);
  
  if(!fileout->is_open()){
    printf(" TkDBc::writeAlignmentLadFF -E- problems opeinf file %s\n",filename);
    if(fileout) delete fileout;
    return -1;
  }


  TkLadder *ll=FindTkId(tkid);
  if(!ll) {printf(" TkDBc::writeAlignmentLadFF -E- ladder %d not found ! \n",tkid);if(fileout)delete fileout;return -2;}

  if(TkLadder::version>=2) (*fileout) << "VVV"<<TkLadder::version<<endl; 
  
  (*fileout) << (*ll);
  //(*ll)>> (*fileout);
  
  if(!fileout->good()){
    printf(" TkDBc::writeAlignmentLadFF -E- problems opeinf file %s\n",filename);
    fileout->close();
    if(fileout) delete fileout;
    return -1;
  }
  
  fileout->close();
  if(fileout) delete fileout;
  return 0;
}

int TkDBc::readAlignmentLadFF(const char* filename, int pri){
  ifstream  fileout(filename);
  if (!fileout) {
    printf("Error: tkdbc alignemnt file not found: %s\n", filename);
    return -1;
  }
  string test;
  
  fileout>> test;
  if(test[0]=='V'&&test[1]=='V'&&test[2]=='V') {
    TkLadder::version=atoi(&(test[3]));
    printf("TkDBc::read-I- New Sensor Aligment Aware TkLadder format detected\n");
  }else{
    TkLadder::version=0;
    fileout.seekg(0,ios_base::beg);
  }

  int count=0;
  while(1){
    TkLadder  aa;
    fileout >> aa;
    if(fileout.eof()){  break;}
    if(!fileout.good()) cerr <<" Error in TkDBc::readAlignment the channel is not good"<<endl;
    int tkid=aa.GetTkId();
    TkLadder *bb=FindTkId(tkid);
    if(!bb) continue;
    bb->pos=aa.pos;
    bb->rot=aa.rot;
    bb->posA=aa.posA;
    bb->rotA=aa.rotA;
    count++;
    if (pri) aa.WriteA(cout);

  }
  cout << count <<" Ladders ALIGNMENTFree  have been read from file "<<filename<<endl;

  fileout.close();
  return 0;
}

int TkDBc::readAlignment(const char* filename, int pri){
  ifstream  fileout(filename);
  if (!fileout) {
    printf("Error: tkdbc alignemnt file not found: %s\n", filename);
    return -1;
  }
  string test;
  
  fileout>> test;
  if(test[0]=='V'&&test[1]=='V'&&test[2]=='V') {
    TkLadder::version=atoi(&(test[3]));
    printf("TkDBc::read-I- New Sensor Aligment Aware TkLadder format detected\n");
  }else{
    TkLadder::version=0;
    fileout.seekg(0,ios_base::beg);
  }

  for (int ii=0;ii<nplanes;ii++){
    planes[ii]->ReadA(fileout);
  }
  int count=0;
  for(tkidIT pp=tkidmap.begin(); pp!=tkidmap.end(); pp++) {
    TkLadder*  aa=pp->second;
    count++;
    aa->ReadA(fileout);
    if(fileout.eof()){  break;}
    if(!fileout.good()) cerr <<" Error in TkDBc::readAlignment the channel is not good"<<endl;
    if (pri) aa->WriteA(cout);

  }
  cout << count <<" Ladders ALIGNMENT  have been read from file "<<filename<<endl;

  fileout.close();
  return 0;

}

int TkDBc::readDisalignment(const char* filename, int pri){
  ifstream  fileout(filename);
  if (!fileout) {
    printf("Error: tkdbc alignemnt file not found: %s\n", filename);
    return -1;
  }
  

  for (int ii=0;ii<nplanes;ii++){
    planes[ii]->ReadT(fileout);
  }
  int count=0;
  for(tkidIT pp=tkidmap.begin(); pp!=tkidmap.end(); pp++) {
    TkLadder*  aa=pp->second;
    count++;
    aa->ReadT(fileout);
    if(fileout.eof()){  break;}
    if(!fileout.good()) cerr <<" Error in TkDBc::readDisalignment the channel is not good"<<endl;
    if (pri) aa->WriteA(cout);

  }
  cout << count <<" Ladders DIS-ALIGNMENT  have been read from file "<<filename<<endl;

  fileout.close();
  return 0;

}


int TkDBc::writeAlignment(const char* filename){

  ofstream  fileout(filename);
  if(TkLadder::version>=2) (*fileout) << "VVV"<<TkLadder::version<<endl; 

  for (int ii=0;ii<nplanes;ii++)
    planes[ii]->WriteA(fileout);
  for (tkidIT pp=tkidmap.begin(); pp!=tkidmap.end(); ++pp)
    pp->second->WriteA(fileout);

  fileout.close();
  return 0;

}

int TkDBc::writeDisalignment(const char* filename){

  ofstream  fileout(filename);
  if(TkLadder::version>=2) fileout << "VVV"<<TkLadder::version<<endl;
  for (int ii=0;ii<nplanes;ii++)
    planes[ii]->WriteT(fileout);
  for (tkidIT pp=tkidmap.begin(); pp!=tkidmap.end(); ++pp)
    pp->second->WriteT(fileout);

  fileout.close();
  return 0;

}

int TkDBc::readAlignmentSensor(const char* filename, int pri){
  ifstream  filein(filename);
  if (!filein) {
    printf("Error: sensor alignemnt file not found: %s\n", filename);
    return -1;
  }

  int count=0;  
  while (filein.good()) {
    int tkid;
    filein >> tkid;
    TkLadder* ll= FindTkId(tkid);
    if (!ll) {
      cerr << " Error in TkDBc::readAlignmentSensor Ladder not found: " << tkid << endl;
      return -1;
    }
    ll->ReadS(filein);
    if (filein.eof()) break;
    if(!filein.good()) cerr <<" Error in TkDBc::readAlignmentSensor the channel is not good"<<endl;
    count++;
    if (pri) ll->WriteS(cout);
  }

  cout << count <<" Sensor ALIGNMENT  have been read from file "<<filename<<endl;

  filein.close();
  return 0;

}

void TkDBc::Align2Lin(){

  if(! linear) {
    printf("TkDBc::Alingn2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
    memset(linear,0,GetLinearSize());
  }
  int off=0;
  for (int pla=1;pla<(nplanes+1);pla++){
    linear[off]=(float)pla;
    TkPlane* pl=GetPlane(pla);
    pl->Align2Lin(&linear[off+1]);
    off+=7;
  }

  for (tkidIT aa=tkidmap.begin(); aa!=tkidmap.end();aa++){
    if(!aa->second->IsActive()) continue;
    linear[off]=(float)(aa->first);
    aa->second->Align2Lin(&(linear[off+1]));
    off+=(TkLadder::GetSize()+1);
  }

}


void TkDBc::Lin2Align(){

  if(! linear) {
    fprintf(stderr,"TkDBc::Lin2Align()-ERROR the linear does not exist\n");
    return;
  }
  int off=0;
  for (int pla=1;pla<(nplanes+1);pla++){
    int pll=(int)linear[off];
    TkPlane* pl=GetPlane(pll);
    pl->Lin2Align(&linear[off+1]);
    off+=7;
  }

  for (int lad=0;lad<192;lad++){
    int tkid= (int)linear[off];
    TkLadder* ll =FindTkId(tkid);
    if(!ll) {
      fprintf(stderr,"TkDBc::Lin2Align()-W tkid %+03d not found!!!!\n",tkid);
    }
    else
      ll->Lin2Align(&(linear[off+1]));
    off+=(TkLadder::GetSize()+1);
  }

}


void SLin2Align(){
  if(TkDBc::Head)
    TkDBc::Head->Lin2Align();
  return;
}

TkDBc *TkDBc::Load(TFile *rfile)
{
  if (!rfile || !rfile->IsOpen()) return 0;
  TkDBc::Head = (TkDBc *)rfile->Get("TkDBc");

  if (TkDBc::Head) TkDBc::Head->RebuildMap();
  return TkDBc::Head;
}

void TkDBc::RebuildMap()
{
  // Clear all the maps except tkidmap
  tkassemblymap.clear();
  hwidmap      .clear();
  //  pgidmap      .clear();
  lnamemap     .clear();
  JMDCNumMap   .clear();

  // Rebuild all the maps
  for (tkidIT it = tkidmap.begin(); it != tkidmap.end(); it++) {
    TkLadder *lad = it->second;
    int lay  = lad->GetLayer();
    int side = lad->GetSide();
    int slot = abs(lad->GetSlot());
    int asid = lad->GetAssemblyId();
    int hwid = lad->GetHwId();
    //    int pgid = lad->GetPgId();
    int jmdc = lad->GetJMDCNum();
    string lnam = lad->name;
    tkassemblymap[asid] = lad;
    hwidmap      [hwid] = lad;
    //    pgidmap      [pgid] = lad;
    lnamemap     [lnam] = lad;
    JMDCNumMap   [jmdc] = lad;
  }

  // SH FIXME size of pgidmap is only 24
  cout << "TkDBc::Maps have been rebuilt: "
       << "tk= " << tkidmap      .size() << " "
       << "hw= " << hwidmap      .size() << " "
       << "ln= " << lnamemap     .size() << " "
       << "as= " << tkassemblymap.size() << " "
    //       << "pg= " << pgidmap      .size() << " "
       << "JN= " << JMDCNumMap   .size() << endl;
}

