//  $Id: richdbc.C,v 1.57 2008/12/08 15:15:17 choutko Exp $
#include"richdbc.h"
#include<math.h>
#include<iostream.h>
#include"richid.h"

geant RICHDB::_RICradpos=RICradposs;
integer RICHDB::_Nph=0;

// defaults
geant RICHDB::wave_length[RICmaxentries]={608.696, 605.797, 602.899, 600.000, 595.652, 591.304,
					  585.507, 578.261, 573.913, 568.116, 563.768, 556.522,
					  550.725, 543.478, 536.232, 528.986, 520.739, 511.594,
					  502.899, 494.203, 482.609, 471.014, 459.42,  447.826,
					  431.884, 426.087, 404.348, 391.304, 376.812, 369.565,
					  355.012, 340.58,  328.986, 314.493, 304.348, 295.304, 
					  288.406, 284.058, 279.71,  275.812, 272.464, 270.014,
					  268.116, 266.667};

geant RICHDB::rad_index=1.0529;
geant RICHDB::naf_index=1.33;

// Fused SiO2 scaled to n=1.14
geant RICHDB::index[RICmaxentries]={1.136,   1.13602, 1.13605, 1.13608, 1.13612, 1.13617,
				    1.13623, 1.13631, 1.13635, 1.13642, 1.13647, 1.13656,
				    1.13663, 1.13672, 1.13681, 1.13691, 1.13703, 1.13717,
				    1.13731, 1.13745, 1.13766, 1.13787, 1.13811, 1.13837,
				    1.13875, 1.1389,  1.13953, 1.13996, 1.1405,  1.14079,
				    1.14144, 1.14217, 1.14284, 1.1438,  1.14456, 1.14532,
				    1.14596, 1.14639, 1.14684, 1.14727, 1.14766, 1.14795,
				    1.14818, 1.14837};



geant RICHDB::naf_index_table[RICmaxentries]={1.32526,1.32531,1.32535,1.32540,1.32547,
					      1.32555,1.32565,1.32578,1.32587,1.32598,
					      1.32607,1.32622,1.32634,1.32650,1.32667,
					      1.32684,1.32705,1.32730,1.32754,1.32780,
					      1.32817,1.32857,1.32901,1.32948,1.33019,
					      1.33047,1.33165,1.33246,1.33348,1.33403,
					      1.33528,1.33669,1.33798,1.33984,1.34134,
					      1.34283,1.34408,1.34493,1.34583,1.34667,
					      1.34744,1.34802,1.34849,1.34885};


// Best fit to current measures: AGL abs length
geant RICHDB::abs_length[RICmaxentries]={100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.,100.,
					 100.,100.,100.,100.};



geant RICHDB::naf_abs_length[RICmaxentries]={36.5383,36.1793,35.8222,35.4667,34.9367,
					     34.4106,33.7151,32.8555,32.3448,31.6699,
					     31.1682,30.3407,29.6864,28.8781,28.0806,
					     27.2939,26.4115,25.4492,24.5502,23.6665,
					     22.5124,21.3856,20.2864,19.2146,17.7859,
					     17.2792,15.4405,14.3837,13.2504,12.6999,
					     11.6268,10.6057, 9.8163, 8.8685, 8.2308,
					     7.6803, 7.2717, 7.0193, 6.7707, 6.5513,
					     6.3653, 6.2307, 6.1272, 6.0488};


integer RICHDB::agl_media=0;
integer RICHDB::naf_media=0;

// PMT quantum eff. from Hamamatsu
geant RICHDB::eff[RICmaxentries]={1.296, 1.476, 1.717, 1.853, 2.041, 2.324, 2.646, 3.214, 3.504,
				  3.904, 4.350, 5.171, 5.518, 6.420, 7.153, 8.143, 9.271,10.330,
				  11.509,12.280,13.981,15.244,16.984,18.122,19.337,20.191,20.633,
				  20.633,20.633,20.633,20.633,20.010,18.923,17.355,16.266,14.918, 
				  13.682,11.509,10.555, 8.321, 7.153, 6.282, 6.148, 4.953};

geant RICHDB::pmt_p[RICmaxpmts][2]; // Table with PMT positions



// PMT photocathode window
geant RICHDB::pmtw_index=1.458;


// Measured abs. length for Bicron-BC800 plastic.
geant RICHDB::lg_abs[RICmaxentries]={100.000,100.000,100.000,100.000,100.000,
				     100.000,100.000,100.000,100.000,100.000,
				     100.000,100.000,100.000,100.000,100.000,
				     100.000,100.000,100.000,100.000,100.000,
				     100.000,100.000,100.000,100.000,100.000,
				     100.000,83.6568,61.6296,24.8616,14.1085,
				     7.0524, 5.3358, 5.2303, 5.0741, 2.8348,
				     0.7004, 0.2304, 0.0554, 0.0554, 0.0554,
				     0.0554, 0.0554, 0.0554, 0.0554};



geant RICHDB::lg_index[RICmaxentries]={1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,
				       1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,
				       1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,
				       1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,1.49,
				       1.49,1.49,1.49,1.49};
				       



integer RICHDB::entries=RICmaxentries;
//geant RICHDB::top_radius=60.0;                 // Top mirror radius
//geant RICHDB::bottom_radius=66.82;             // Bottom mirror radius
//geant RICHDB::rich_height=45.8;  // Mirror height (for historical reasons it is quoted rich height)

/*********** updated numbers */
geant RICHDB::top_radius=60.10;      // Top mirror radius
geant RICHDB::bottom_radius=67.00;   // Bottom mirror radius
geant RICHDB::rich_height=46.32;     // Mirror height (for historical reasons it is quoted rich height)
/****************************/

geant RICHDB::hole_radius[2]={63.8/2.,64.3/2}; // half ECAL hole side length (it is not symmetric)
geant RICHDB::inner_mirror_height=50;          // UNUSED
geant RICHDB::rad_clarity=0.0055;              // Radiator clarity: updated 10/28/04
geant RICHDB::scatprob=.19;                    // Probability of surface scattering accroding to C. Delgado model
geant RICHDB::scatang=14e-3;                   // Scattered angle (rad) according to C. Delgado model 
geant RICHDB::eff_rad_clarity=0.0055;          // clarity used in charge recosntruction

geant RICHDB::rad_radius=60.0;                 // Radiator radius
geant RICHDB::rad_agl_height=2.5;              // Radiator agl thickness
geant RICHDB::rad_height=3;                    // Radiator support structure height
geant RICHDB::naf_height=0.5;                  // NaF radiator thickness
geant RICHDB::rad_length=11.5;                 // AGL Radiator tile side length
geant RICHDB::naf_length=8.52;                 // NAF Radiator tile side length
geant RICHDB::lg_height=3.0;                   // Light guide height withou the fixing foil
geant RICHDB::lg_length=3.4;                   // Side length of light guide top (Called lg_length in the standalone version)
geant RICHDB::lg_bottom_length=1.77;           // Side length on the bottom
geant RICHDB::inner_pixel=0.38;                // Inner pixel size
geant RICHDB::foil_height=0.1;                 // Foil thickness
geant RICHDB::foil_index=1.49;                 // Foil refractive index


integer RICHDB::total=0;

geant RICHDB::ped=0.;                         // Default pedestal value (ADC counts)
geant RICHDB::sigma_ped=0.5335;               // Default pedestal width
geant RICHDB::peak=23.04;                     // Default gain
geant RICHDB::sigma_peak=12.10;               // Witdh of the single p.e.
geant RICHDB::c_ped=2.;                       // N od ADC counts for detection threshold

// Measure in the prototype
geant RICHDB::prob_noisy=8.e-5;
geant RICHDB::prob_dark=4.e-5;   

// Some counters
integer RICHDB::nphgen=0;
integer RICHDB::nphrad=0;
integer RICHDB::nphbas=0;
integer RICHDB::numrefm=0;
integer RICHDB::numrayl=0;


// Book some histograms
void RICHDB::bookhist(){
#ifdef __AMSDEBUG__
  dump();
  cout <<"RICH BOOKING"<<endl;
  HBOOK1(123456,"Error de los directos",50,-1.,1.,0.);
  HBOOK1(123457,"Error de los reflejados",50,-1.,1.,0.);
  HBOOK1(123000,"Generados",300,0.,100.,0.);
  HBOOK1(123001,"Radiador",300,0.,100.,0.);
  HBOOK1(123002,"Base",300,0.,100.,0.);
  HBOOK1(123003,"Detectados",300,0.,100.,0.);
  HBOOK1(123100,"LG TRAN",100,0.,1.,0.);
  HBOOK1(1231001,"f",100,0.,1.,0.);
  HBOOK1(1231002,"lglength",100,0.,6.,0.);
  HBOOK1(123100,"Numero de hits ",300,0.,50.,0.);
  HBOOK1(123100,"Numero de hits reflejados",300,0.,50.,0.);
  HBOOK1(123100,"Numero de hits sin reflejar",300,0.,50.,0.);
  HBOOK2(124000,"Posicion de los PMT",300,-70.,70.,300,-70.,70.,0.);
  HBOOK2(124010,"Posicion de los PMT",300,-60.,60.,300,-60.,60.,0.);
  HBOOK1(12345,"Probabilidad de scattering",2,0.,2.,0.);
  HBOOK1(12346,"Angulo ph",50,0.,3.141592,0.);
  HBOOK1(12347,"Angulo th",100,0.,2e-1,0.);
#endif 

}

// Recompute some vars 
void RICHDB::mat_init(){
// Update chromatic dispersion if the radiator index is different from 1.14
// Scaled from fused SiO2
  static int called=0;


  if(!called){
#pragma omp barrier
#pragma omp single
    {
#ifdef __AMSDEBUG__
    //    if(RICHDB::rad_index!=1.14) 
    //      cout <<"Energia     Indice"<<endl
    //	   <<"-------     ------"<<endl;
#endif

    if(RICHDB::rad_index!=1.14)
      for(integer i=0;i<RICmaxentries;i++){
	if(RICHDB::index[i]<1.) continue;
	RICHDB::index[i]=1.+(RICHDB::index[i]-1.)*(RICHDB::rad_index-1.)/0.14;
      }
    called=1;
    }
#pragma omp barrier
  }
}



// Aerogel density

geant RICHDB::aerogel_density(){
  geant water_conc=0.;
  geant methanol_conc=0.;

  // J.Phys D 27(1994)414: Unused
  //  return (RICHDB::rad_index-1.)/(0.19+0.31*water_conc+0.38*methanol_conc);

  // Matsushita
  return (RICHDB::rad_index-1.)/0.28;
}


/// Now Some functions for the rich geometry

geant RICHDB::total_height()
{

  return rad_height+foil_height+RICradmirgap+rich_height+
    RIClgdmirgap+RICpmtsupportheight;
}

geant RICHDB::pmtb_height()
{

  return RICpmtlength+RICeleclength+lg_height+RICpmtfoil;
}

geant RICHDB::mirror_pos()
{

  return rad_height+foil_height+RICradmirgap+rich_height/2.;
}

geant RICHDB::rad_pos()
{
  return rad_height/2;
}

geant RICHDB::pmt_pos() // In RICH
{  

  return rad_height+foil_height+RICradmirgap+rich_height+
             RIClgdmirgap+pmtb_height()/2.;
}

geant RICHDB::elec_pos() // In PMT box
{

  return cato_pos()+RICpmtlength/2.;
}

geant RICHDB::cato_pos() // In PMT box
{

  return lg_pos()+(lg_height+RICotherthk)/2;
}

geant RICHDB::lg_pos()
{
  return -(RICpmtlength+RICeleclength)/2+RICpmtfoil/2.;
}

geant RICHDB::shield_pos(){

  return elec_pos()-RICotherthk/2.;
}

geant RICHDB::lg_mirror_angle(integer i)
{

  if(i==1)
    return atan2(geant(lg_length/2.-RICepsln/2-
       (lg_bottom_length/2.+RIClgthk_bot/2.)),
		 lg_height)*180./3.14159265358979323846;


  if(i==2)
    return atan2(geant(lg_length/4.-
        (RIClgthk_bot+inner_pixel)),
		 lg_height)*180./3.14159265358979323846;


  return 0;
}

geant RICHDB::lg_mirror_pos(integer i)
{

  if(i==1)
    return lg_bottom_length/2.+RIClgthk_bot/2.
           +lg_height/2.*tan(lg_mirror_angle(1)*3.1415926/180.);

  if(i==2)
    return RIClgthk_bot+inner_pixel+
            lg_height/2.*tan(lg_mirror_angle(2)*3.1415926/180.);


  return 0;
}

geant RICHDB::x(integer id){return RichPMTsManager::GetChannelPos(id,0);};
geant RICHDB::y(integer id){return RichPMTsManager::GetChannelPos(id,1);};

integer RICHDB::detcer(geant photen)
{
  return RichPMTsManager::detcer(photen);
}
   

geant RICHDB::max_step(){
  AMSmceventg dummy(GCKINE.ikine,0.,AMSPoint(),AMSDir());
  number charge=dummy.getcharge();

  if(charge==0) return 1000.;
  geant dndl=370*(1-1/RICHDB::rad_index/RICHDB::rad_index)*
        197.327*6.28*(1/RICHDB::wave_length[RICmaxentries-1]
	-1/RICHDB::wave_length[0])*charge*charge;
  geant max=RICmaxphotons/dndl;
  return max;
}


geant RICHDB::mean_height(){
  // Computes the mean emission point inside the radiator
  // of the detected photons.
  // The credits go to .... Elisa Lanciotti

  static geant value=0.0;
#pragma omp threadprivate(value)

  if(value) return value;

#ifdef __AMSDEBUG__
  //  cout<<"Computing mean height value"<<endl;  
#endif
  
  const integer steps=100;    // Number of steps for the approximation
  geant lambda,qeff,n,dl,l_scat=0,l_abs_rad,l_abs_lg;
  geant sum=0,densum=0;
  geant sum_index=0;

  for(integer i=0;i<RICmaxentries-1;i++){ // Integration in wave length
    lambda=(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2.;
    qeff=(RICHDB::eff[i]+RICHDB::eff[i+1])/2.;
    n=(RICHDB::index[i]+RICHDB::index[i+1])/2.;
    dl=RICHDB::wave_length[i]-RICHDB::wave_length[i+1];
    if(rad_clarity==0. && l_scat!=1e6){
      cerr <<"RICHDB::mean_height : radiador clarity is zero."<<endl;
      l_scat=1e6;
    }else
      l_scat=(lambda/1000.)*(lambda/1000.)*(lambda/1000.)*(lambda/1000.)/
	rad_clarity;
    l_abs_rad=(RICHDB::abs_length[i]+RICHDB::abs_length[i+1])/2.;
    l_abs_lg=(RICHDB::lg_abs[i]+RICHDB::lg_abs[i+1])/2.;
    for(integer j=0;j<steps;j++){ // Integration in radiador thicknes
      geant x=rad_height*(geant(j)+0.5)/geant(steps);
      geant g=qeff/lambda/lambda/exp((rad_height-x)*
				     (1/l_scat+1/l_abs_rad))/
	exp(lg_height/l_abs_lg);
      sum+=dl*g*x;
      densum+=dl*g;
      sum_index+=dl*g*n;
    }
  }
  if(!densum){
    cerr<<"RICHDB::mean_height : Error"<<endl;
  }else{
    value=rad_height-sum/densum;

    RICHDB::rad_index=sum_index/densum;

    return value;
  }
  return -1;  // Codigo de error
}


void RICHDB::dump(){
  // DUMP constant values
  cout <<"Dimensions:"<<endl<<
    "  RICradpos:  "<<RICHDB::RICradpos()<<endl<<
    "  RICaethk:  "<<RICaethk<<endl<<      
    "  RIClgthk:  "<<RIClgthk<<endl<<
    "  RIClgthk_top:  "<<RIClgthk_top<<endl<<
    "  RIClgthk_bot:  "<< RIClgthk_bot<<endl<< 
    "  RICmithk:  "<<RICmithk<<endl<<      
    "  RICradmirgap:  "<<RICradmirgap<<endl<<   
    "  RIClgdmirgap:  "<<RIClgdmirgap<<endl<<
    "  RICotherthk:  "<<RICotherthk<<endl;

  // Dump not constant 
  cout<<
    "  rad_clarity:  "<<rad_clarity<<endl<<
    "  rad_radius:  "<<rad_radius<<endl<<
    "  rad_height:  "<<rad_height<<endl<<
    "  rad_length:  "<<rad_length<<endl<<
    "  rad_index:  "<<rad_index<<endl<<
    "  foil_height:  "<<foil_height<<endl<<
    "  foil_index:  "<<foil_index<<endl<<
    //    "  rad_supthk:  "<<rad_supthk<<endl<<
    "  lg_height:  "<<lg_height<<endl<<
    "  lg_length:  "<<lg_length<<endl<<
    "  lg_bottom_length:  "<<lg_bottom_length<<endl<<
    "  inner_pixel:  "<<inner_pixel<<endl;


  // Dump functions
  cout <<
    "  total_height:  "<<total_height()<<endl<<
    "  pmtb_height:  "<<pmtb_height()<<endl<<
    "  mirror_pos:  "<<mirror_pos()<<endl<<
    "  rad_pos:  "<<rad_pos()<<endl<<
    "  pmt_pos:  "<<pmt_pos()<<endl<<
    "  elec_pos:  "<<elec_pos()<<endl<<
    "  cato_pos:  "<<cato_pos()<<endl<<
    "  lg_pos:  "<<lg_pos()<<endl<<
    "  shield_pos:  "<<shield_pos()<<endl;

 
  // Other stuff
  /*
  cout <<
    " LG top at "<<RICHDB::total_height()/2+AMSRICHIdGeom::pmt_pos(1,2)
    +RICHDB::lg_height/2.+RICpmtfoil-lg_pos()<<endl<<
    " Radiator bottom at "<<RICHDB::total_height()/2-RICHDB::rad_pos()-
    RICHDB::rad_height/2<<endl;
  */

}




// Light guide efficiency tables
float RICHDB::lg_eff_tbl[RIC_NWND][RIC_NPHI][RIC_NTH]=
{0.924,0.912,0.893,0.877,0.846,0.822,0.781,0.720,0.669,0.521,0.331,0.126,0.028,0.011,0.013,0.000,0.000,0.000,0.000,0.000
,0.924,0.906,0.897,0.864,0.833,0.800,0.768,0.697,0.560,0.379,0.199,0.065,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000
,0.924,0.913,0.894,0.876,0.843,0.823,0.777,0.716,0.669,0.512,0.332,0.123,0.029,0.011,0.013,0.000,0.000,0.000,0.000,0.000
,0.923,0.920,0.905,0.889,0.863,0.844,0.806,0.743,0.736,0.646,0.462,0.227,0.074,0.025,0.024,0.000,0.000,0.000,0.000,0.000
,0.926,0.922,0.913,0.902,0.889,0.871,0.842,0.820,0.765,0.711,0.615,0.439,0.226,0.060,0.012,0.000,0.000,0.000,0.000,0.000
,0.928,0.926,0.922,0.920,0.918,0.900,0.869,0.855,0.818,0.786,0.723,0.627,0.422,0.261,0.149,0.035,0.000,0.015,0.000,0.000
,0.930,0.934,0.931,0.933,0.928,0.917,0.893,0.884,0.859,0.836,0.762,0.707,0.554,0.425,0.300,0.115,0.028,0.024,0.014,0.000
,0.930,0.937,0.936,0.928,0.921,0.925,0.917,0.890,0.868,0.836,0.808,0.760,0.674,0.483,0.260,0.101,0.021,0.000,0.000,0.000
,0.930,0.933,0.932,0.930,0.928,0.918,0.890,0.886,0.858,0.839,0.768,0.708,0.569,0.430,0.303,0.122,0.028,0.025,0.015,0.000
,0.929,0.929,0.925,0.917,0.919,0.899,0.869,0.855,0.817,0.782,0.723,0.624,0.424,0.261,0.148,0.037,0.000,0.014,0.000,0.000
,0.927,0.921,0.915,0.902,0.889,0.876,0.847,0.815,0.767,0.713,0.622,0.446,0.225,0.061,0.012,0.000,0.000,0.000,0.000,0.000
,0.921,0.919,0.900,0.887,0.862,0.845,0.801,0.741,0.737,0.656,0.463,0.221,0.069,0.022,0.023,0.000,0.000,0.000,0.000,0.000
,0.895,0.872,0.877,0.820,0.697,0.673,0.595,0.349,0.107,0.046,0.054,0.066,0.056,0.055,0.062,0.055,0.019,0.017,0.024,0.000
,0.893,0.875,0.854,0.838,0.753,0.617,0.475,0.308,0.157,0.061,0.032,0.031,0.031,0.028,0.021,0.016,0.000,0.000,0.000,0.000
,0.895,0.882,0.860,0.836,0.804,0.769,0.657,0.502,0.335,0.198,0.083,0.033,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000
,0.895,0.890,0.873,0.861,0.837,0.820,0.785,0.724,0.672,0.591,0.432,0.229,0.087,0.026,0.021,0.011,0.000,0.000,0.000,0.000
,0.897,0.899,0.896,0.884,0.874,0.868,0.855,0.840,0.807,0.769,0.735,0.672,0.540,0.351,0.169,0.048,0.000,0.000,0.000,0.000
,0.897,0.909,0.908,0.913,0.914,0.915,0.904,0.897,0.887,0.859,0.857,0.839,0.787,0.709,0.520,0.215,0.055,0.090,0.090,0.000
,0.903,0.914,0.919,0.921,0.930,0.928,0.925,0.920,0.916,0.898,0.891,0.880,0.856,0.797,0.649,0.419,0.157,0.154,0.161,0.000
,0.901,0.908,0.916,0.916,0.917,0.919,0.917,0.909,0.899,0.879,0.861,0.830,0.802,0.757,0.644,0.482,0.260,0.084,0.033,0.000
,0.900,0.908,0.908,0.909,0.905,0.894,0.873,0.862,0.839,0.810,0.760,0.676,0.553,0.402,0.307,0.161,0.055,0.024,0.018,0.000
,0.900,0.898,0.896,0.892,0.884,0.860,0.846,0.803,0.726,0.610,0.461,0.269,0.119,0.057,0.028,0.012,0.000,0.000,0.000,0.000
,0.894,0.887,0.877,0.889,0.856,0.776,0.727,0.653,0.526,0.323,0.145,0.058,0.039,0.043,0.042,0.042,0.034,0.017,0.000,0.000
,0.894,0.877,0.883,0.854,0.733,0.745,0.715,0.466,0.144,0.056,0.054,0.080,0.067,0.062,0.062,0.074,0.049,0.029,0.041,0.000
,0.858,0.846,0.817,0.704,0.522,0.378,0.254,0.121,0.049,0.037,0.049,0.047,0.046,0.044,0.036,0.023,0.010,0.000,0.000,0.000
,0.861,0.837,0.801,0.700,0.498,0.278,0.117,0.050,0.037,0.036,0.033,0.026,0.017,0.010,0.000,0.000,0.000,0.000,0.011,0.000
,0.862,0.848,0.820,0.697,0.518,0.377,0.248,0.117,0.048,0.039,0.049,0.045,0.043,0.045,0.034,0.024,0.000,0.000,0.000,0.000
,0.865,0.856,0.846,0.772,0.651,0.638,0.588,0.408,0.160,0.062,0.052,0.066,0.068,0.063,0.063,0.068,0.050,0.034,0.036,0.000
,0.869,0.864,0.863,0.867,0.840,0.781,0.741,0.708,0.641,0.507,0.348,0.214,0.110,0.064,0.053,0.055,0.057,0.056,0.056,0.000
,0.872,0.880,0.881,0.884,0.885,0.889,0.885,0.871,0.847,0.825,0.774,0.689,0.539,0.380,0.210,0.085,0.031,0.022,0.013,0.000
,0.880,0.886,0.892,0.900,0.908,0.910,0.910,0.906,0.898,0.881,0.875,0.870,0.844,0.803,0.673,0.424,0.191,0.137,0.141,0.000
,0.880,0.891,0.898,0.903,0.910,0.920,0.923,0.924,0.917,0.915,0.902,0.886,0.875,0.866,0.844,0.795,0.723,0.533,0.388,0.000
,0.880,0.892,0.896,0.898,0.908,0.908,0.907,0.905,0.897,0.879,0.876,0.869,0.840,0.799,0.673,0.429,0.195,0.137,0.135,0.000
,0.876,0.878,0.879,0.878,0.883,0.886,0.885,0.871,0.843,0.819,0.771,0.685,0.537,0.374,0.210,0.083,0.029,0.022,0.012,0.000
,0.870,0.863,0.862,0.866,0.840,0.784,0.754,0.729,0.666,0.516,0.344,0.210,0.111,0.064,0.052,0.054,0.056,0.056,0.054,0.000
,0.863,0.856,0.848,0.771,0.654,0.639,0.598,0.414,0.157,0.059,0.052,0.067,0.071,0.064,0.064,0.068,0.049,0.033,0.035,0.000};




float RICHDB::lg_dist_tbl[RIC_NWND][RIC_NPHI][RIC_NTH]=
{1.024,1.035,1.050,1.071,1.099,1.136,1.175,1.226,1.284,1.335,1.352,1.375,1.400,1.257,1.262,0.000,0.000,0.000,0.000,0.000
,1.025,1.037,1.053,1.075,1.104,1.144,1.197,1.253,1.312,1.367,1.417,1.463,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000
,1.025,1.035,1.050,1.072,1.099,1.136,1.176,1.227,1.284,1.334,1.353,1.376,1.398,1.257,1.263,0.000,0.000,0.000,0.000,0.000
,1.023,1.031,1.042,1.059,1.082,1.113,1.144,1.183,1.236,1.287,1.301,1.326,1.362,1.318,1.234,0.000,0.000,0.000,0.000,0.000
,1.021,1.024,1.031,1.042,1.059,1.081,1.110,1.146,1.190,1.246,1.308,1.368,1.417,1.457,1.485,0.000,0.000,0.000,0.000,0.000
,1.019,1.018,1.021,1.028,1.039,1.054,1.074,1.101,1.136,1.182,1.237,1.285,1.325,1.337,1.350,1.405,0.000,1.238,0.000,0.000
,1.017,1.014,1.014,1.017,1.025,1.036,1.052,1.074,1.101,1.136,1.172,1.213,1.256,1.288,1.315,1.360,1.403,1.225,1.249,0.000
,1.017,1.012,1.011,1.013,1.019,1.029,1.044,1.063,1.089,1.121,1.164,1.221,1.288,1.346,1.401,1.451,1.492,0.000,0.000,0.000
,1.017,1.014,1.014,1.017,1.025,1.036,1.052,1.074,1.100,1.136,1.173,1.215,1.260,1.293,1.321,1.365,1.401,1.228,1.251,0.000
,1.019,1.018,1.021,1.028,1.039,1.054,1.074,1.101,1.136,1.182,1.236,1.286,1.326,1.338,1.348,1.405,0.000,1.237,0.000,0.000
,1.021,1.024,1.032,1.042,1.059,1.081,1.111,1.146,1.190,1.245,1.307,1.368,1.416,1.459,1.489,0.000,0.000,0.000,0.000,0.000
,1.023,1.030,1.042,1.059,1.082,1.113,1.144,1.182,1.236,1.287,1.300,1.324,1.354,1.304,1.231,0.000,0.000,0.000,0.000,0.000
,1.086,1.114,1.149,1.179,1.215,1.289,1.356,1.392,1.354,1.177,1.144,1.169,1.171,1.217,1.277,1.302,1.379,1.397,1.420,0.000
,1.084,1.109,1.142,1.180,1.218,1.265,1.322,1.370,1.398,1.358,1.279,1.285,1.317,1.353,1.409,1.470,0.000,0.000,0.000,0.000
,1.081,1.097,1.119,1.150,1.190,1.234,1.280,1.325,1.370,1.413,1.437,1.427,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000
,1.075,1.080,1.089,1.106,1.127,1.157,1.187,1.224,1.267,1.307,1.326,1.332,1.359,1.378,1.271,1.275,0.000,0.000,0.000,0.000
,1.070,1.065,1.064,1.068,1.076,1.089,1.107,1.130,1.155,1.189,1.231,1.277,1.322,1.373,1.433,1.496,0.000,0.000,0.000,0.000
,1.066,1.054,1.047,1.042,1.042,1.045,1.052,1.062,1.078,1.098,1.126,1.160,1.204,1.259,1.313,1.373,1.477,1.376,1.431,0.000
,1.064,1.049,1.039,1.032,1.029,1.029,1.032,1.039,1.050,1.066,1.087,1.113,1.147,1.189,1.237,1.299,1.387,1.373,1.408,0.000
,1.065,1.052,1.043,1.038,1.036,1.038,1.044,1.054,1.068,1.087,1.112,1.144,1.181,1.230,1.290,1.358,1.424,1.486,1.543,0.000
,1.069,1.061,1.058,1.059,1.065,1.074,1.089,1.107,1.133,1.167,1.203,1.231,1.266,1.297,1.318,1.348,1.396,1.317,1.262,0.000
,1.075,1.078,1.085,1.096,1.114,1.137,1.166,1.201,1.241,1.289,1.340,1.372,1.398,1.381,1.402,1.438,0.000,0.000,0.000,0.000
,1.080,1.095,1.115,1.140,1.165,1.193,1.245,1.305,1.355,1.391,1.390,1.307,1.198,1.221,1.263,1.328,1.399,1.447,0.000,0.000
,1.085,1.108,1.139,1.166,1.198,1.265,1.332,1.365,1.339,1.206,1.115,1.148,1.139,1.156,1.203,1.249,1.291,1.349,1.378,0.000
,1.149,1.189,1.227,1.263,1.301,1.357,1.400,1.406,1.287,1.213,1.226,1.241,1.274,1.331,1.337,1.395,1.484,0.000,0.000,0.000
,1.153,1.198,1.242,1.289,1.335,1.378,1.400,1.381,1.352,1.386,1.407,1.408,1.419,1.427,0.000,0.000,0.000,0.000,1.307,0.000
,1.150,1.188,1.228,1.263,1.302,1.356,1.402,1.405,1.284,1.211,1.226,1.240,1.275,1.331,1.337,1.393,0.000,0.000,0.000,0.000
,1.142,1.166,1.192,1.209,1.239,1.300,1.357,1.384,1.367,1.249,1.150,1.150,1.166,1.172,1.212,1.248,1.273,1.328,1.380,0.000
,1.133,1.139,1.150,1.164,1.179,1.197,1.230,1.274,1.315,1.347,1.372,1.384,1.358,1.252,1.184,1.210,1.259,1.319,1.356,0.000
,1.125,1.115,1.111,1.111,1.114,1.122,1.133,1.148,1.167,1.194,1.228,1.265,1.303,1.345,1.393,1.455,1.449,1.460,1.494,0.000
,1.119,1.098,1.083,1.073,1.067,1.065,1.066,1.070,1.079,1.092,1.110,1.133,1.163,1.202,1.241,1.274,1.323,1.380,1.406,0.000
,1.117,1.092,1.075,1.062,1.053,1.047,1.045,1.046,1.050,1.059,1.072,1.090,1.113,1.142,1.178,1.227,1.292,1.369,1.416,0.000
,1.119,1.097,1.083,1.073,1.067,1.064,1.065,1.070,1.079,1.092,1.110,1.133,1.163,1.202,1.241,1.275,1.326,1.383,1.409,0.000
,1.125,1.114,1.110,1.110,1.114,1.121,1.133,1.148,1.168,1.196,1.228,1.265,1.304,1.344,1.394,1.453,1.443,1.460,1.498,0.000
,1.133,1.139,1.150,1.164,1.179,1.197,1.233,1.280,1.321,1.352,1.373,1.383,1.353,1.254,1.182,1.208,1.258,1.319,1.360,0.000
,1.142,1.166,1.192,1.209,1.238,1.300,1.358,1.386,1.372,1.236,1.142,1.149,1.166,1.173,1.212,1.247,1.272,1.322,1.371,0.000};






integer RICHDB::get_wavelength_bin(geant wl){
  // Use binary search to find the bin
  // It assumes the wave length array is sorted in descending order

  if(wl>=wave_length[0]) return 0;
  if(wl<wave_length[RICmaxentries-1]) return RICmaxentries-1;

  int bin_max=RICmaxentries-1;
  int bin_min=0;

  for(;;){
    int bin_med=(bin_max+bin_min)/2;
    if(wl<wave_length[bin_med]) bin_min=bin_med; else bin_max=bin_med;
    if(bin_max-bin_min<=1) break;
  }
  
  return bin_min;

}

//
// Test if the media is a radiator within fortran. To be used in gtckov
//

extern "C" integer testradiator_(integer *numed){
  if(*numed==RICHDB::agl_media) return 1;
  if(*numed==RICHDB::naf_media) return 2;
  return 0;
}


//
// Alignment class
//


AMSPoint  RichAlignment::_a2rShift;
AMSPoint  RichAlignment::_r2aShift;
AMSRotMat RichAlignment::_a2rRot;
AMSRotMat RichAlignment::_r2aRot;

void RichAlignment::LoadFile(char *filename){
  if(!filename || strlen(filename)==0){
    cout<<"RichAlignment::LoadFile -- problem with file name... Ignoring."<<endl;
    return;
  }

  fstream stream;
  stream.open(filename,fstream::in);
  if(!stream.is_open() || stream.fail()){
    cout<<"RichAlignment::LoadFile -- problem opening file "<<filename<<" ... Ignoring."<<endl;
    return;
  }

  cout<<"RichAlignment::LoadFile -- loading "<<filename<<endl;

  // Read the following components of the a2r matrixes
  double sx,sy,sz;
  double alpha,beta,gamma;

  stream>>sx>>sy>>sz;

  if(stream.eof()){
    cout<<"RichAlignment::LoadFile -- truncated... Ingnoring "<<filename<<endl;
  }

  stream>>alpha>>beta>>gamma;


  if(stream.eof()){
    cout<<"RichAlignment::LoadFile -- truncated... Ingnoring "<<filename<<endl;
  }

  stream.close();

#ifdef __AMSDEBUG__
  cout<<"RICH ALIGNMENT PARAMETERS --- "<<sx<<" "<<sy<<" "<<sz<<" --- "<<alpha<<" "<<beta<<" "<<gamma<<endl;
#endif

  // Fill the a2r shift and matrix
  _a2rShift.setp(sx,sy,sz);
  _a2rRot.SetRotAngles(alpha,beta,gamma);

#ifdef __AMSDEBUG__
  _a2rShift.getp(sx,sy,sz);
  _a2rRot.GetRotAngles(alpha,beta,gamma);

  cout<<" --- RECOVERED RICH ALIGNMENT PARAMETERS --- "<<sx<<" "<<sy<<" "<<sz<<" --- "<<alpha<<" "<<beta<<" "<<gamma<<endl;
#endif


  // Fill the r2a shift and matrix
  AMSRotMat t;
  _r2aRot.SetRotAngles(0,0,-gamma);
  t.SetRotAngles(0,-beta,0);
  _r2aRot=t*_r2aRot;
  t.SetRotAngles(-alpha,0,0);
  _r2aRot=t*_r2aRot;
  _r2aShift=(_r2aRot*_a2rShift)*-1.0;
  
  // Check that the result is acceptable
  AMSRotMat test_m=_r2aRot*_a2rRot;
  test_m.GetRotAngles(alpha,beta,gamma);

  AMSPoint test_p=_a2rShift;
  test_p=(_r2aRot*test_p)+_r2aShift;
  test_p.getp(sx,sy,sz);

#ifdef __AMSDEBUG__
  cout<<" -- RICH AFTER INVERTING --- "<<sx<<" "<<sy<<" "<<sz<<" --- "<<alpha<<" "<<beta<<" "<<gamma<<endl;
#endif

  if(!(fabs(alpha)<0.5e-4 && fabs(beta)<0.5e-4 && fabs(gamma)<0.5e-4)){
    cout<<"RichAlignment::LoadFile -- problem with rotation matrix "<<alpha<<" "<<beta<<" "<<gamma<<". Ignoring"<<endl;
    _r2aRot.Reset();
    _a2rRot.Reset();
  }
  if(!(fabs(sx)<0.5e-2 && fabs(sy)<0.5e-2 && fabs(sz)<0.5e-2)){
    cout<<"RichAlignment::LoadFile -- problem with shift vector "<<sx<<" "<<sy<<" "<<sz<<". Ignoring"<<endl;
    _r2aShift.setp(0,0,0);
    _a2rShift.setp(0,0,0);
  }
}

void RichAlignment::Init(){
  // If this is cosmics and real data, load the cosmics alignment.
  // This should be moved to database in some moment 
  char name[801];


  // Ensure that the initialization is done only once and is visible by all the threads 
  // Has the single an implicit barrier? Just in case we add them
  // TODO: check that this is PreAss
#pragma omp barrier
#pragma omp single
  if(AMSJob::gethead()->isRealData()  &&     // It is real data
     MISCFFKEY.BeamTest==1){                 // It is a cosmic run
    sprintf(name,"%s/%s/RichAlignmentCosmic.1207904521.2.dat",getenv("AMSDataDir"),AMSCommonsI::getversion());
    LoadFile(name);
  }
#pragma omp barrier

}
