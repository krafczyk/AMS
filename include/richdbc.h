//  $Id: richdbc.h,v 1.24 2002/05/21 16:35:13 delgadom Exp $
// Author C.J. Delgado (1999) 
// Updated October 2000

#ifndef __RICHDB__
#define __RICHDB__ 


// Undefine next one to use solid guides with opaque walls separating each section 
#undef __HOLLOW__

#include<job.h>
#include<commons.h> 
#include<trrec.h>

// Constants
namespace richconst{
const integer RICnrot=30001;   // Rot. matrix no.

// Geometry parameters
/*
const geant   RICmithk=0.2;         // Mirror and 2*aerogel support thickness
const geant   RICaethk=0.1;         // Light guides thickness
const geant   RIClgthk=0.05;        // NEW!
const geant   RICotherthk=0.1;      // PMT window thickness
const geant   RICpmtshield=0.05;    // shield thickness
const geant   RICcatolength=1.75;   // cathode window length
const geant   RICpmtlength=2.5;     // phototube length
const geant   RICeleclength=2.5;    // electronics
const geant   RICpmtsupport=0.4;    // suppot structure thickness
const geant   RICshiheight=8.0;     // NEW! Magnetic shielding height

const geant   RICradpos=-75.26;// Top of the radiator position
const geant   sq2=1.4142135623;// useful constant:sqrt(2)

const geant   RICepsln=0.002;  // Useful to simulate absence of optical contact
*/


const geant      RICradpos=-75.26;     // Top of the radiator position
const geant      RICaethk=0.1;         // Radiator support thickness
#define RIClgthk (2.5*RIClgthk_top)    // A simple fix in the meanwhile
const geant      RIClgthk_top=0.02;    // LG    top gap
const geant      RIClgthk_bot=0.07;    // LG bottom gap
const geant      RICmithk=0.2;         // Mirror thickness
const geant      RICradmirgap=0.;      // Mirror-Radiator Distance  (if !=0 Chk gustep)
const geant      RIClgdmirgap=0.;      // LG-Mirror Distance        (if !=0 Chk gustep)
const geant      RICotherthk=0.08;     // PMT window thickness
const geant      RICcatolength=1.81;   // cathode window length 
const geant      RICcatogap=0.03;      // Gap btwn PMT pixels 
const geant      RICpmtlength=2.0;     // phototube length including PMT window 
const geant      RICeleclength=2.75;   // electronics length below PMT           
const geant      RICpmtsupport=0.6;    // support structure thickness
const geant      RICshiheight=7.5;     // PMT shield height from the bottom 
const geant      RICpmtshield=0.1;     // PMT shield thickness 
const geant      RICepsln=0.002;       // Epsilon
const geant      RICpmtfoil=0.1;       // thickness of the goil over the LG
const geant      PMT_electronics=3.0;    // PMT side size
const geant      cato_inner_pixel=0.42;  // Inner pixel side size in the photocathode
const geant      cathode_length=RICcatolength+RICcatogap;
const geant      pitch=3.7;

// Entries in the tables of materials

const integer RICmaxentries=44;
const integer RICmaxrows=100;

// Threshold, in % of the total beta range, to avoid  the reconstruction
// of secondaries

const geant RICthreshold=0.05; //16% is the maximum, 5% is good enough

const integer RICentries=44;   // no. of entries in wave_length,eff...   
const integer RICmaxpmts=2000; // max no. of pmts
const integer RICnwindows=16;  // Number of windows for pmt
const integer RIChistos=30000; // Starting point for RICH histos 
const integer RICmaxphotons=500; //Max mean number of photons generated

// Particle types

const integer Cerenkov_photon=50; //Geant3 case
const integer Noise=-666;

// Status codes
const integer Status_Rayleigh=1;
const integer Status_Noise=-1;
const integer Status_Window=-2;
const integer Status_No_Cerenkov=-3;
const integer Status_Fake=-4;
const integer Status_LG_origin=-5;
const integer Status_primary_rad=-7;
const integer Status_primary_tracker=-8;
const integer Status_primary_tof=-8;
const integer Status_primary_radb=-9;


}

using namespace richconst;
				   
class RICHDB { 
public: // To be migrated in the future

  //---------- GEOMETRY PARAMETERS: unused by now
  
  // RICH detector features
  static geant top_radius;
  static geant bottom_radius;
  static geant rich_height;
  static geant hole_radius;


  // Inner mirror features
  static geant inner_mirror_height;

  // Radiator features
  static geant rad_clarity;
  static geant rad_radius;
  static geant rad_height;
  static geant rad_length;
  static geant rad_index;   // Mean index

  static geant foil_height; // Radiator foil support height
  static geant foil_index;	

  static geant rad_supthk;

  // Light guide features
  static geant lg_height;
  static geant lg_length;
  static geant lg_bottom_length;        // bottom LG length
  static geant inner_pixel;             // Inner pixel size
  static geant lg_abs[RICmaxentries];   // abs. length for solid LG
  static geant lg_index[RICmaxentries]; // refractive index for solid_LG

  //---------- RADIATOR PARAMETERS
  static integer entries;
  
  static geant wave_length[RICmaxentries];
  static geant index[RICmaxentries];
  static geant abs_length[RICmaxentries];


  //---------- PMT PARAMETERS
  static geant pmtw_index; // Photocathode window mean refractive index
  static geant eff[RICmaxentries];
  static geant ped;
  static geant sigma_ped;
  static geant peak;
  static geant sigma_peak;

  // For detection purposes

  static geant c_ped;      // Detection treshold
  static geant prob_noisy; // Probability of noise for one channel


  //--------- PMT array parameters
/* Old version
  static integer n_rows[2];
  static integer n_pmts[15][2];
  static integer offset[15][2];
*/
  static integer total; // Total number of pmts in the actual setup
  static geant pmt_p[RICmaxpmts][2];			     
 
  //--------- Some simple counters
  static integer nphgen;  // Nb. of generated photons
  static integer nphbas;  // Nb. of photons at base
  static integer numrefm; // Nb. of reflections in the mirror
  static integer numrayl; // Nb. of rayleigh scatterings

public:
  static inline geant x(integer);
  static inline geant y(integer);
  static void bookhist();
  static void mat_init();
  static geant aerogel_density();
  static geant total_height();
  static geant pmtb_height();
  static geant mirror_pos();
  static geant rad_pos();
  static geant pmt_pos();
  static geant elec_pos();
  static geant cato_pos();
  static geant lg_pos();
  static geant shield_pos();
  static geant lg_mirror_angle(integer);
  static geant lg_mirror_pos(integer);
  static integer detcer(geant);
  static geant max_step();
  static geant mean_height();
  static integer _Nph;
  static integer & Nph(){return _Nph;}  
  static geant ring_fraction(AMSTrTrack *ptrack ,geant &direct,geant &reflected,
			     geant &length);
  static void dump();
  
};

#endif










