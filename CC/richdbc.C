//  $Id: richdbc.C,v 1.24 2002/03/15 15:36:26 mdelgado Exp $
#include<richdbc.h>
#include<cern.h>
#include<math.h>
#include<mceventg.h>
#include<iostream.h>
#include<richid.h>

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

geant RICHDB::rad_index=1.05;


// Fused SiO2 scaled to n=1.14

geant RICHDB::index[RICmaxentries]={1.136,   1.13602, 1.13605, 1.13608, 1.13612, 1.13617,
				    1.13623, 1.13631, 1.13635, 1.13642, 1.13647, 1.13656,
				    1.13663, 1.13672, 1.13681, 1.13691, 1.13703, 1.13717,
				    1.13731, 1.13745, 1.13766, 1.13787, 1.13811, 1.13837,
				    1.13875, 1.1389,  1.13953, 1.13996, 1.1405,  1.14079,
				    1.14144, 1.14217, 1.14284, 1.1438,  1.14456, 1.14532,
				    1.14596, 1.14639, 1.14684, 1.14727, 1.14766, 1.14795,
				    1.14818, 1.14837};



geant RICHDB::abs_length[RICmaxentries]={1372.78,1346.82,1321.23,1296.00,1258.84,
					 1222.49,1175.25,1118.14,1084.89,1041.71,
					 1010.19,959.240,919.900,872.430,826.820,
					 783.020,735.330,685.020,639.620,596.520,
					 542.480,492.200,445.490,402.200,347.910,
					 329.600,267.310,234.460,201.600,186.540,
					 158.840,134.550,117.140,97.8200,85.8000,
					 76.0500,69.1900,65.1100,61.2100,57.8700,
  	 				 55.1100,53.1600,51.6800,50.5700};

// PMT quantum eff. from Hamamatsu

geant RICHDB::eff[RICmaxentries]={1.296, 1.476, 1.717, 1.853, 2.041, 2.324, 2.646, 3.214, 3.504,
				  3.904, 4.350, 5.171, 5.518, 6.420, 7.153, 8.143, 9.271,10.330,
				  11.509,12.280,13.981,15.244,16.984,18.122,19.337,20.191,20.633,
				  20.633,20.633,20.633,20.633,20.010,18.923,17.355,16.266,14.918, 
				  13.682,11.509,10.555, 8.321, 7.153, 6.282, 6.148, 4.953};

/* Old version of code
integer RICHDB::n_rows[2]={10,8};

integer RICHDB::n_pmts[15][2]={{11,7},{11,8},{11,7},{11,6},{11,5},{11,4},
			       {10,3},{10,2},{10,0},{8,0}};

integer RICHDB::offset[15][2]={{0,1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                               {0,0},{0,0},{1,0}};
*/

geant RICHDB::pmt_p[RICmaxpmts][2];



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
geant RICHDB::top_radius=60.0;         // Top mirror radius
geant RICHDB::bottom_radius=67.;       // Bottom mirror radius
geant RICHDB::rich_height=46.8;             // Expansion height
geant RICHDB::hole_radius=31.5;        // half ECAL hole side length
geant RICHDB::inner_mirror_height=50;  // UNUSED
geant RICHDB::rad_clarity=0.0091;      // Radiator clarity
geant RICHDB::rad_radius=60.0;         // Radiator radius
geant RICHDB::rad_height=3;            // Radiator thickness
geant RICHDB::rad_length=11.3+0.1;        // Radiator tile side length
geant RICHDB::lg_height=3.0;           // Light guide height withou the fixing foil
geant RICHDB::lg_length=3.1;        // Side length of light guide top (Called lg_length in the standalone version)
geant RICHDB::lg_bottom_length=1.77;
geant RICHDB::inner_pixel=0.38;
geant RICHDB::foil_height=0.1;
geant RICHDB::rad_supthk=0.1;


integer RICHDB::total=0;

geant RICHDB::ped=0.;           // Values from A. Contin talk
geant RICHDB::sigma_ped=0.5335; // January 11 1999
geant RICHDB::peak=23.04;
geant RICHDB::sigma_peak=12.10;
geant RICHDB::c_ped=2.;           // N od ADC counts for detection threshold
geant RICHDB::prob_noisy=1-FREQ(RICHDB::c_ped/RICHDB::sigma_ped);


// Some counters

integer RICHDB::nphgen=0;
integer RICHDB::nphbas=0;
integer RICHDB::numrefm=0;
integer RICHDB::numrayl=0;


// Book some histograms

void RICHDB::bookhist(){
}

// Recompute some vars 
void RICHDB::mat_init(){
// Update chromatic dispersion if the radiator index is different from 1.14
// Scaled from fused SiO2

#ifdef __AMSDEBUG__
  if(RICHDB::rad_index!=1.14) 
    cout <<"Energia     Indice"<<endl
         <<"-------     ------"<<endl;
#endif

  if(RICHDB::rad_index!=1.14)
  for(integer i=0;i<RICmaxentries;i++){
    if(RICHDB::index[i]<1.) continue;
    RICHDB::index[i]=1.+(RICHDB::index[i]-1.)*(RICHDB::rad_index-1.)/0.14;

#ifdef __AMSDEBUG__
    cout <<2*3.1415926*197.327e-9/RICHDB::wave_length[i]<<"   "<<
           RICHDB::index[i]<<endl;
#endif

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
#ifdef __AMSDEBUG__
  cout<<"Total_height:"<<rad_height+foil_height+RICradmirgap+rich_height+
                  RIClgdmirgap+lg_height+RICpmtlength+RICeleclength<<endl;
#endif

  return rad_height+foil_height+RICradmirgap+rich_height+
                  RIClgdmirgap+lg_height+RICpmtlength+RICeleclength
                  +RICpmtfoil;
}

geant RICHDB::pmtb_height() // NEW!
{
#ifdef __AMSDEBUG__
  cout<<"pmtb_height:"<<RICpmtlength+RICeleclength+lg_height+RICpmtfoil<<endl;
#endif

  return RICpmtlength+RICeleclength+lg_height+RICpmtfoil;
}

geant RICHDB::mirror_pos()
{
#ifdef __AMSDEBUG__
  cout<<"mirror_pos:"<<rad_height+foil_height+RICradmirgap+rich_height/2.<<endl;
#endif
  return rad_height+foil_height+RICradmirgap+rich_height/2.;
}

geant RICHDB::rad_pos()
{
#ifdef __AMSDEBUG__
  cout <<"rad_pos:"<<rad_height/2.<<endl;
#endif
  return rad_height/2;
}

geant RICHDB::pmt_pos() // In RICH
{  
#ifdef __AMSDEBUG__
  cout <<"pmt(f)_pos:"<<rad_height+foil_height+RICradmirgap+rich_height+
             RIClgdmirgap+pmtb_height()/2.<<endl;
#endif

  return rad_height+foil_height+RICradmirgap+rich_height+
             RIClgdmirgap+pmtb_height()/2.;
}

geant RICHDB::elec_pos() // In PMT box
{
#ifdef __AMSDEBUG__
  cout <<"elec_pos:"<<cato_pos()+RICpmtlength/2.<<endl;
#endif

  return cato_pos()+RICpmtlength/2.;
}

geant RICHDB::cato_pos() // In PMT box
{
#ifdef __AMSDEBUG__
  cout <<"cato_pos:"<<lg_pos()+(lg_height+RICotherthk)/2<<endl;
#endif

  return lg_pos()+(lg_height+RICotherthk)/2;
}

geant RICHDB::lg_pos()
{
#ifdef __AMSDEBUG__
cout <<"lg_pos:"<<-(RICpmtlength+RICeleclength)/2+RICpmtfoil/2.<<endl;
#endif

  return -(RICpmtlength+RICeleclength)/2+RICpmtfoil/2.;
}

geant RICHDB::shield_pos(){
#ifdef __AMSDEBUG__
cout <<"shield_pos:"<<elec_pos()-RICotherthk/2.<<endl;
#endif

  return elec_pos()-RICotherthk/2.;
}

geant RICHDB::lg_mirror_angle(integer i)
{
#ifdef __AMSDEBUG__
cout <<"lg_mirror_angle(1):"<<atan2(lg_length/2.-
       (lg_bottom_length/2.+RIClgthk_bot/2.),
        lg_height)*180./3.1415926<<endl
     <<"lg_mirror_angle(2):"<<atan2(lg_length/4.-
        (RIClgthk_bot+inner_pixel),
         lg_height)*180./3.1415926<<endl;
#endif

  if(i==1) // NEW!
    return atan2(lg_length/2.-
       (lg_bottom_length/2.+RIClgthk_bot/2.),
        lg_height)*180./3.1415926;

  if(i==2) // NEW!
    return atan2(lg_length/4.-
        (RIClgthk_bot+inner_pixel),
         lg_height)*180./3.1415926;


  return 0;
}

geant RICHDB::lg_mirror_pos(integer i)
{
#ifdef __AMSDEBUG__
cout <<"lg_mirror_pos(1):"<<lg_bottom_length/2.+RIClgthk_bot/2.
           +lg_height/2.*tan(lg_mirror_angle(1)*3.1415926/180.)<<endl
     <<"lg_mirror_pos(2):"<<RIClgthk_bot+inner_pixel+  
            lg_height/2.*tan(lg_mirror_angle(2)*3.1415926/180.)<<endl;

#endif

  if(i==1)
    return lg_bottom_length/2.+RIClgthk_bot/2.
           +lg_height/2.*tan(lg_mirror_angle(1)*3.1415926/180.);

  if(i==2)
    return RIClgthk_bot+inner_pixel+
            lg_height/2.*tan(lg_mirror_angle(2)*3.1415926/180.);


  return 0;
}


geant RICHDB::x(integer id){AMSRICHIdGeom channel(id);return channel.x();};
geant RICHDB::y(integer id){AMSRICHIdGeom channel(id);return channel.y();};

// Old and unused version
//geant RICHDB::x(integer channel)
//{
//  integer pmt=channel/RICnwindows;
//  integer window=channel%RICnwindows;
//
//  //  geant x=pmt_p[pmt][0]+(window%integer(sqrt(RICnwindows))
//  //			 -integer(sqrt(RICnwindows))/2)*RICcatolength/sqrt(RICnwindows);
//
//  geant x=(2*(window%integer(sqrt(RICnwindows)))-3)*RICHDB::rad_length/8.+pmt_p[pmt][0];
//
//  return x;
//}
  

//geant RICHDB::y(integer channel)
//{
//  integer pmt=channel/RICnwindows;
//  integer window=channel%RICnwindows;
//
//  //  geant y=pmt_p[pmt][1]+(window/integer(sqrt(RICnwindows))
//  //			 -integer(sqrt(RICnwindows))/2)*RICcatolength/sqrt(RICnwindows);
//
//  geant y=(2*(window/integer(sqrt(RICnwindows)))-3)*RICHDB::rad_length/8.+pmt_p[pmt][1];
//
//  return y;
//}




integer RICHDB::detcer(geant photen)
{
   integer upper=-1,i;

   for(i=1;i<RICHDB::entries;i++) 
     if(2*3.1415926*197.327e-9/RICHDB::wave_length[i]>=photen)
        {upper=i;break;}

   if(upper==-1) return 0;
   i=upper;

   geant xufact=RICHDB::eff[i]-RICHDB::eff[i-1];
   xufact/=2*3.1415926*197.327e-9*(1/RICHDB::wave_length[i]-1/RICHDB::wave_length[i-1]);

   geant deteff=RICHDB::eff[i-1]+(photen-2*3.1415926*197.327e-9/RICHDB::wave_length[i-1])*xufact;

   geant dummy=0;
   geant ru=RNDM(dummy);

   if(100*ru<deteff) return 1;
   return 0;
}
   

// New function to apply q.eff a priori in photons produced in RICH

//extern "C" integer prod_cer_(geant *energy){
//  integer lvl=GCVOLU.nlevel-1;
//
//  if(
//    (GCVOLU.names[lvl][0]=='R' && GCVOLU.names[lvl][1]=='A' &&
//     GCVOLU.names[lvl][2]=='D' && GCVOLU.names[lvl][3]==' ') ||
//    (GCVOLU.names[lvl][0]=='C' && GCVOLU.names[lvl][1]=='A' &&
//     GCVOLU.names[lvl][2]=='T' && GCVOLU.names[lvl][3]=='O')){
//    if(RICHDB::detcer(*energy)) return 0;
//  }
//  return 1;
//}


geant RICHDB::max_step(){
  AMSmceventg dummy(GCKINE.ikine,0.,AMSPoint(),AMSDir());
  number charge=dummy.getcharge();

#ifdef __AMSDEBUG__
  cout<< "Particle Id "<<GCKINE.ikine << endl
      << "Charge "<< charge <<endl;
#endif

  if(charge==0) return 1000.;
  geant dndl=370*(1-1/RICHDB::rad_index/RICHDB::rad_index)*
        197.327*6.28*(1/RICHDB::wave_length[RICmaxentries-1]
	-1/RICHDB::wave_length[0])*charge*charge;
  geant max=RICmaxphotons/dndl;
//#ifdef __AMSDEBUG__
  cout << "Max step "<<max<<" cm "<<charge<<endl;
//#endif
  return max;
}


geant RICHDB::mean_height(){
  // Computes the mean emission point inside the radiator
  // of the detected photons.
  // The credits go to .... Elisa Lanciotti

  static geant value=0.0;
#ifdef __AMSDEBUG__
  cout<<"Present mean height value:"<<value<<endl;
#endif

  if(value) return value;

#ifdef __AMSDEBUG__
  cout<<"Computing mean height value"<<endl;  
#endif
  
  const integer steps=100;    // Number of steps for the approximation
  geant lambda,qeff,n,dl,l_scat=0,l_abs_rad,l_abs_lg;
  geant sum=0,densum=0;

  for(integer i=0;i<RICmaxentries-1;i++){ // Integration in wave length
    lambda=(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2.;
    qeff=(RICHDB::eff[i]+RICHDB::eff[i+1])/2.;
    n=(RICHDB::index[i]+RICHDB::index[i+1])/2.;
    dl=RICHDB::wave_length[i]-RICHDB::wave_length[i+1];
    if(rad_clarity==0. && l_scat!=1e6){
      cout <<"RICHDB::mean_height : radiador clarity is zero."<<endl;
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
    }
  }
  if(!densum){
    cout<<"RICHDB::mean_height : Error"<<endl;
  }else{
    value=rad_height-sum/densum;
    return value;
  }
  return -1;  // Codigo de error
}
