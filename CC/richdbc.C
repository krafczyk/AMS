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

integer RICHDB::n_pmts[RICmaxrows]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

geant RICHDB::first[RICmaxrows]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
				    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

integer RICHDB::n_rows=0;
integer RICHDB::entries=RICmaxentries;
geant RICHDB::top_radius=63.6;
geant RICHDB::bottom_radius=80;
geant RICHDB::height=50;
geant RICHDB::hole_radius=40;
geant RICHDB::inner_mirror_height=50;
geant RICHDB::rad_radius=63.6;
geant RICHDB::rad_height=2;
geant RICHDB::rad_tile_size=15;
geant RICHDB::lg_height=3;
geant RICHDB::lg_tile_size=3;
integer RICHDB::total=0;

geant RICHDB::ped=-0.2888;      // Values extracted from A. Contin talk
geant RICHDB::sigma_ped=0.5335; // January 11 1999
geant RICHDB::peak=22.75;
geant RICHDB::sigma_peak=12.10;
integer RICHDB::c_ped=integer(-.2888+3*0.5335); 


// These function still are under test


geant RICHDB::x(integer pmt,integer window){
  integer w=(window-1)%4;
  geant offset=(lg_tile_size-2*RICotherthk)/4;  


  switch(w){
  case 0:
    offset*=-1.5;
    break;
  case 1:
    offset*=-0.5;
    break;
  case 2:
    offset*=0.5;
    break;
  case 3:
    offset*=1.5;
    break;
  }


  w=(pmt-1)%4;
  geant sx=1.;

  switch(w){
  case 1:
  case 3: 
    sx=-1.;
    break;
  }

  pmt=(pmt-1)/4;

  integer nr=0;

  while(pmt>n_pmts[nr]){pmt-=n_pmts[nr];nr++;};

  return (first[nr]+pmt*lg_tile_size)*sx+offset;
}


geant RICHDB::y(integer pmt,integer window){
  integer w=(window-1)/4;
  geant offset=(lg_tile_size-2*RICotherthk)/4;

  switch(w){
  case 0:
    offset*=1.5;
    break;
  case 1:
    offset*=0.5;
    break;
  case 2:
    offset*=-0.5;
    break;
  case 3:
    offset*=-1.5;
    break;
  }

  w=(pmt-1)%4;
  geant sx=1.;
  
  switch(w){
  case 2:
  case 3: 
    sx=-1.;
    break;
  }

  pmt=(pmt-1)/4;

  integer nr=0;
  
  while(pmt>n_pmts[nr]){pmt-=n_pmts[nr];nr++;};
  return (lg_tile_size/2+nr*lg_tile_size)*sx+offset;
}

void RICHDB::add_row(geant x){
  n_rows++;
  n_pmts[n_rows-1]=1;
  first[n_rows-1]=x;
}


void RICHDB::add_pmt(){
  n_pmts[n_rows-1]++;
}


geant RICHDB::pmt_response(integer n_photons){ // Quite unefficient 
  geant u1,u2;
  geant dummy=0;
  geant r;

  do{
    u1=2*RNDM(dummy)-1;
    u2=2*RNDM(dummy)-1;
    
    r=sqrt(u1*u1+u2*u2);
  }while(r>1);

  return u1*sqrt(-2*log(r*r)/r/r)*sqrt(sigma_ped*sigma_ped+
				       n_photons*sigma_peak*sigma_peak)+
    n_photons*peak+ped;  
}




void RICHDB::bookhist(){
#ifdef __AMSDEBUG__
  // Number of hits detected
  HBOOK1(RIChistos+0,"Number of hits",100.,0.,1000.,0);
  // Charge detected for good ones
  HBOOK1(RIChistos+1.,"ADC counts",100.,-10.,300.,0);
  //
  HBOOK1(RIChistos+2.,"ADC counts",50.,-10.,10.,0);
#endif
}


char *RICHDB::name(char beg,int copy){
  char code[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
  static char out[5]="P+++";
  

  if(copy>36*36*36){
    cerr << "RICHDB::name: copy number too long" <<endl;
    exit(1);
  }

  out[0]=beg;
  out[1]=code[copy/1296];
  copy%=1296;
  out[2]=code[copy/36];
  copy%=36;
  out[3]=code[copy];
  
  return out;
}
  
