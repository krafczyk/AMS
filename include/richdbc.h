//  $Id: richdbc.h,v 1.60 2013/09/04 11:44:18 mdelgado Exp $
// Author C.J. Delgado (1999) 
// Updated October 2000

#ifndef __RICHDB__
#define __RICHDB__ 


// Undefine next one to use solid guides with opaque walls separating each section 
#undef __HOLLOW__


// Next define switches to the lg table taking into account the
// lg absorption implicitly
#define __RICH_SINGLE_LG_TABLE__


#ifndef __ROOTSHAREDLIBRARY__
#include"job.h"
#include "mceventg.h"
#endif
#include"commons.h" 


class RichAlignment;

// Constants
namespace richconst{
const integer RICnrot=30001;   // Rot. matrix no.

// Geometry parameters
//const geant      RICradpos=-71.87;     // Top of the radiator position
//const geant      RICradposs=-73.5;   //  changed by VC as not allow change the ricxh geo on the fly 
const geant      RICradposs=-71.87;   //  changed by VC as not allow change the ricxh geo on the fly 

const geant      RICaethk=0.1;         // Radiator support thickness
#define RIClgthk (2.5*RIClgthk_top)    // A simple fix in the meanwhile
const geant      RIClgthk_top=0.02;    // LG    top gap
const geant      RIClgthk_bot=0.07;    // LG bottom gap
const geant      RICmithk=0.2;         // Mirror thickness
//const geant      RICradmirgap=0.1;
//const geant      RIClgdmirgap=0.4;

/*********** updated numbers */
const geant      RICradmirgap=0.6-0.5;
const geant      RIClgdmirgap=0.6-0.1;
/*****************************/

const geant      RICotherthk=0.08;     // PMT window thickness
const geant      RICcatolength=1.81;   // cathode window length 
const geant      RICcatogap=0.03;      // Gap btwn PMT pixels 
const geant      RICpmtlength=2.0;     // phototube length including PMT window 
const geant      RICeleclength=2.75;   // electronics length below PMT           
const geant      RICpmtsupport=0.6;    // support structure thickness
const geant      RICshiheight=6.5;     // This for the new LG with 3.4
const geant      RICpmtshield=0.1;     // PMT shield thickness 
const geant      RICepsln=0.002;       // Epsilon
const geant      RICpmtfoil=0.1;       // thickness of the goil over the LG
const geant      RICpmtsupportheight=11.1-2.0;  // support structure height
const geant      PMT_electronics=3.0;    // PMT side size
const geant      cato_inner_pixel=0.42;  // Inner pixel side size in the photocathode
const geant      cathode_length=RICcatolength+RICcatogap;
const geant      pitch=3.7;
const integer    RICradiator_box_sides=12;
const geant      RICradiator_box_radius=118.5/2;
const geant      RICradiator_box_thickness=5;
const geant      RICmireff=0.85;     // Mirror reflectivity sectors 2 and 3
const geant      RICmireffs1=0.8;    // Mirror reflectivity Sector 1 

// Entries in the tables of materials
const integer RICmaxentries=44;
const integer RICmaxrows=100;

// Threshold, in % of the total beta range, to avoid  the reconstruction
// of secondaries
const geant RICthreshold=0.05; //16% is the maximum, 5% is good enough

const integer RICentries=44;   // no. of entries in wave_length,eff...   
const integer RICmaxpmts=680; // max no. of pmts
const integer RICnwindows=16;  // Number of windows for pmt
const integer RIChistos=30000; // Starting point for RICH histos 
const integer RICmaxphotons=500; //Max mean number of photons generated
const integer RICmaxtiles=1000;

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

// Status word for good channels
const integer Status_good_channel=1;


// Dword for the hits status 
const uinteger ok_status_bit=28;
const uinteger gain_mode_bit=29;
const uinteger crossed_pmt_bit=30;
const uinteger ok_status=uinteger(1)<<ok_status_bit;
const uinteger gain_mode=uinteger(1)<<gain_mode_bit;
const uinteger crossed_pmt=uinteger(1)<<crossed_pmt_bit;
const geant npe_crossed_signature=6.;  // Minimum number of p.e. to consider  
                                       // a pixel crossed by a charged particle

// Dword for ring status
const uinteger dirty_ring_bit=0;
const uinteger dirty_ring=uinteger(1)<<dirty_ring_bit;
const uinteger naf_ring_bit=1;
const uinteger naf_ring=uinteger(1)<<naf_ring_bit; 

// Radiator kinds
const integer radiator_kinds=2; 
const integer empty_kind=0; // Used by default
const integer agl_kind=1;
const integer naf_kind=2;

// Lg eff tables size
const integer RIC_NTH=20;  // binning in theta
const integer RIC_NPHI=12; // binning in phi
const integer RIC_NWND=3;  // Number of light guides

const integer RIC_prob_bins=1000;


// DAQ related constants
const int RICH_JINFs=2;
const int RICH_CDPperJINF=12;
const int RICH_LinksperJINF=24; 
const int RICH_PMTperCDP=31;
}

using namespace richconst;
				   
class RICHDB { 
public: // To be migrated in the future

  //---------- GEOMETRY PARAMETERS: unused by now
  
  // RICH detector features
  static geant top_radius;
  static geant bottom_radius;
  static geant rich_height;
  static geant hole_radius[2];  // Now it is rectangular


  // Inner mirror features
  static geant inner_mirror_height;
  static geant  RICmirrors3_s2; // Sectors position
  static geant  RICmirrors2_s1; // Sectors position
  static geant  RICmirrors1_s3; // Sectors position

  // Radiator features
  static geant rad_clarity;
  static geant rad_radius;
  static geant rad_agl_height;
  static geant rad_height;
  static geant rad_length;
  static geant rad_index;   // Mean index for the main radiator
  static geant scatprob;    // Probability of surface scattering
  static geant scatang;     // Angle of surface scattering  
  static geant scatloss;    // Probability of losing a photon when crossing the aerogel surface
  static geant eff_rad_clarity; // Effective radiator clarity for charge reconstruction 

  // Central NaF radiator features
  static geant naf_index;
  static geant naf_height;
  static geant naf_length;


  // Foil features
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
  static geant support_foil_abs[RICmaxentries];  // Hesaglass absorption length
  static geant support_foil_index[RICmaxentries]; // Hesaglass refractice index (the suppor foi index)

  static float lg_dist_tbl[RIC_NWND][RIC_NPHI][RIC_NTH];  // distance in LG table
  static float lg_eff_tbl[RIC_NWND][RIC_NPHI][RIC_NTH]; // efficiency
  //---------- RADIATOR PARAMETERS
  static integer entries;
  
  static geant wave_length[RICmaxentries];
  static geant index[RICmaxentries];
  static geant abs_length[RICmaxentries];

  //---------- NaF Radiator Parameters
  static geant naf_index_table[RICmaxentries];  
  static geant naf_abs_length[RICmaxentries];


  //---------- Tracking media numbers
  static integer agl_media;
  static integer naf_media;


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
  static geant prob_dark;  // Probability of dark current 


  //--------- PMT array parameters
  static integer total; // Total number of pmts in the actual setup
  static geant pmt_p[RICmaxpmts][2];			     
 
  //--------- Some simple counters
  static integer nphgen;  // Nb. of generated photons
  static integer nphbas;  // Nb. of photons at base
  static integer nphrad;
  static integer numrefm; // Nb. of reflections in the mirror
  static integer numrayl; // Nb. of rayleigh scatterings
  static geant _RICradpos;
#pragma omp threadprivate(_RICradpos)
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
  static geant max_step(double index=0);
  static geant mean_height();
  static integer _Nph;
#pragma omp threadprivate(_Nph)
  static geant & RICradpos(){return _RICradpos;}
  static integer & Nph(){return _Nph;}  
  static integer get_wavelength_bin(geant wavelength);


  static void dump();
  
};


#include "point.h"

// This class stores all the information about coordinates system:
//    - A transformation from AMS to RICH (radiator related) coordinates
//    - A transformation from RICH to AMS
class RichAlignment{
 private:
  static bool      _IgnoreDB;       // Set to true to avoid reading the DB
  static AMSPoint  _a2rShift;
  static AMSRotMat _a2rRot;
  static AMSPoint  _r2aShift;
  static AMSRotMat _r2aRot;
  static AMSPoint  _mirrorShift;                // Internal alignment 
  static void LoadFile(char *filename,int ver=0);
  static double _align_parameters[12];  // 6 for RICH+6 internal for mirror

 public:
  static void Init();
  static AMSPoint AMSToRich(AMSPoint &p){return (_a2rRot*p)+_a2rShift;}
  static AMSPoint RichToAMS(AMSPoint &p){return (_r2aRot*p)+_r2aShift;}
  static AMSDir   AMSToRich(AMSDir &p){return (_a2rRot*p);}
  static AMSDir   RichToAMS(AMSDir &p){return (_r2aRot*p);}
  static AMSPoint RichToMirror(AMSPoint &p){return p+_mirrorShift;}
  static AMSPoint MirrorToRich(AMSPoint &p){return p-_mirrorShift;}
  static void Set(double Dx,double Dy,double Dz,double alpha,double beta,double gamma);
  static void SetMirrorShift(double Dx,double Dy,double Dz);
  static void GetFromTDV();
  static void Finish();
  static bool IgnoreDB(){return _IgnoreDB;}

  friend class AMSJob;
};


#endif










