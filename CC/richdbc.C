#include<richdbc.h>
#include<cern.h>
#include<math.h>
#include<iostream.h>

// defaults

geant RICHDB::wave_length[RICmaxentries]={608.696, 605.797, 602.899, 600.000, 595.652, 591.304,
					  585.507, 578.261, 573.913, 568.116, 563.768, 556.522,
					  550.725, 543.478, 536.232, 528.986, 520.739, 511.594,
					  502.899, 494.203, 482.609, 471.014, 459.42,  447.826,
					  431.884, 426.087, 404.348, 391.304, 376.812, 369.565,
					  355.012, 340.58,  328.986, 314.493, 304.348, 295.304, 
					  288.406, 284.058, 279.71,  275.812, 272.464, 270.014,
					  268.116, 266.667};

geant RICHDB::rad_index=1.14;

geant RICHDB::index[RICmaxentries]={1.136,   1.13602, 1.13605, 1.13608, 1.13612, 1.13617,
				    1.13623, 1.13631, 1.13635, 1.13642, 1.13647, 1.13656,
				    1.13663, 1.13672, 1.13681, 1.13691, 1.13703, 1.13717,
				    1.13731, 1.13745, 1.13766, 1.13787, 1.13811, 1.13837,
				    1.13875, 1.1389,  1.13953, 1.13996, 1.1405,  1.14079,
				    1.14144, 1.14217, 1.14284, 1.1438,  1.14456, 1.14532,
				    1.14596, 1.14639, 1.14684, 1.14727, 1.14766, 1.14795,
				    1.14818, 1.14837};





geant RICHDB::abs_length[RICmaxentries]={1372.78, 1346.82, 1321.23, 1296, 1258.84, 1222.49, 1175.25,
					 1118.14, 1084.89, 1041.71, 1010.19,  959.24,   919.9,  872.43,
					 826.82,  783.02,  735.33,  685.02,  639.62,  596.52,  542.48,
					 492.2,  445.49,   402.2,  347.91,   329.6,  267.31,  234.46,
					 201.6,  186.54,  158.84,  134.55,  117.14,   97.82,    85.8,
					 76.05,   69.19,   65.11,   61.21,   57.87,   55.11,   53.16,
					 51.68,   50.57};





geant RICHDB::eff[RICmaxentries]={1.296, 1.476, 1.717, 1.853, 2.041, 2.324, 2.646, 3.214, 3.504,
				  3.904, 4.350, 5.171, 5.518, 6.420, 7.153, 8.143, 9.271,10.330,
				  11.509,12.280,13.981,15.244,16.984,18.122,19.337,20.191,20.633,
				  20.633,20.633,20.633,20.633,20.010,18.923,17.355,16.266,14.918, 
				  13.682,11.509,10.555, 8.321, 7.153, 6.282, 6.148, 4.953};


integer RICHDB::n_rows[2]={10,8};

integer RICHDB::n_pmts[15][2]={{11,7},{11,8},{11,7},{11,6},{11,5},{11,4},
			       {10,3},{10,2},{10,0},{8,0}};

integer RICHDB::offset[15][2]={{0,1},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},
                               {0,0},{0,0},{1,0}};

geant RICHDB::pmt_p[RICmaxpmts][2];


integer RICHDB::entries=RICmaxentries;
geant RICHDB::top_radius=60.0;
geant RICHDB::bottom_radius=67.;
geant RICHDB::height=45.4;
geant RICHDB::hole_radius=31.5;
geant RICHDB::inner_mirror_height=50;
geant RICHDB::rad_clarity=0.011;
geant RICHDB::rad_radius=60.0;
geant RICHDB::rad_height=2;
geant RICHDB::rad_tile_size=15;
geant RICHDB::lg_height=3.31;
geant RICHDB::lg_tile_size=3.1;  // NEW! Called lg_length in the standalone version
integer RICHDB::total=0;

geant RICHDB::ped=0.;           // Values from A. Contin talk
geant RICHDB::sigma_ped=0.5335; // January 11 1999
geant RICHDB::peak=23.04;
geant RICHDB::sigma_peak=12.10;
geant RICHDB::c_ped=2.;           // N od ADC counts for detection threshold
geant RICHDB::prob_noisy=1-FREQ(RICHDB::c_ped/RICHDB::sigma_ped);

integer RICHDB::nphgen=0;
integer RICHDB::nphbas=0;
integer RICHDB::numrefm=0;
integer RICHDB::numrayl=0;


void RICHDB::bookhist(){
}

/// Now Some functions for the rich geometry

geant RICHDB::total_height()
{
  return RICGEOM.height+ // Expanxion length
    RICGEOM.radiator_height+
    RICGEOM.light_guides_height+
    RICshiheight; // NEW!!
}

geant RICHDB::pmtb_height() // NEW!
{
  return RICpmtlength+RICeleclength+RICGEOM.light_guides_height;
}

geant RICHDB::mirror_pos()
{
  return total_height()/2-RICGEOM.height/2-RICGEOM.radiator_height;
}

geant RICHDB::rad_pos()
{
  return total_height()/2-RICGEOM.radiator_height/2;
}

geant RICHDB::pmt_pos() // In RICH
{
  return total_height()/2-RICGEOM.radiator_height-RICGEOM.height-
    (RICpmtlength+RICeleclength)/2-RICGEOM.light_guides_height/2;
}

geant RICHDB::elec_pos() // In PMT box
{
  return (RICpmtlength+RICeleclength)/2 // 7(=electronics+phototube length)/2
    -RICGEOM.light_guides_height/2-RICotherthk/2-
    RICpmtlength/2; 
}

geant RICHDB::cato_pos() // In PMT box
{
  return (RICpmtlength+RICeleclength)/2
    -RICGEOM.light_guides_height/2-RICotherthk/2;
}

geant RICHDB::lg_pos()
{
  return (RICpmtlength+RICeleclength)/2;
}

geant RICHDB::lg_mirror_angle(integer i)
{
  if(i==1) // NEW!
    return atan2(RICGEOM.light_guides_length/2-RICcatolength/2,
			 RICGEOM.light_guides_height)*180/3.1415926;

  if(i==2) // NEW!
    return atan2((RICGEOM.light_guides_length/2-RICcatolength/2)/2,
		 RICGEOM.light_guides_height)*180/3.1415926;

  return 0;
}

geant RICHDB::lg_mirror_pos(integer i)
{
  if(i==1)
    return RICcatolength/2-RIClgthk/2+RICGEOM.light_guides_height/2
      *tan(lg_mirror_angle(1)*3.1415926/180);
  
  if(i==2)
   return (RICcatolength/2-RIClgthk/2)/2+RICGEOM.light_guides_height/2
     *tan(lg_mirror_angle(2)*3.1415926/180);

  return 0;
}

geant RICHDB::x(integer channel)
{
  integer pmt=channel/RICnwindows;
  integer window=channel%RICnwindows;

  //  geant x=pmt_p[pmt][0]+(window%integer(sqrt(RICnwindows))
  //			 -integer(sqrt(RICnwindows))/2)*RICcatolength/sqrt(RICnwindows);

  geant x=(2*(window%integer(sqrt(RICnwindows)))-3)*RICHDB::lg_tile_size/8.+pmt_p[pmt][0];

  return x;
}
  

geant RICHDB::y(integer channel)
{
  integer pmt=channel/RICnwindows;
  integer window=channel%RICnwindows;

  //  geant y=pmt_p[pmt][1]+(window/integer(sqrt(RICnwindows))
  //			 -integer(sqrt(RICnwindows))/2)*RICcatolength/sqrt(RICnwindows);

  geant y=(2*(window/integer(sqrt(RICnwindows)))-3)*RICHDB::lg_tile_size/8.+pmt_p[pmt][1];

  return y;
}




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

