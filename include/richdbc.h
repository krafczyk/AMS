// Author C.J. Delgado (1999)


#ifndef __RICHDB__
#define __RICHDB__ 

#include<job.h>
#include<commons.h> 

// Constants
namespace richconst{
const integer RICnrot=30001;   // Rot. matrix no.

const geant   RICmithk=0.5;         // Mirror and 2*aerogel support thickness
const geant   RIClgthk=0.05;        // Light guides thickness
const geant   RICotherthk=0.1;      // PMT window thickness
const geant   RICpmtshield=0.07/2;  // shield thickness
const geant   RICcatolength=2*.875; // cathode window length
const geant   RICpmtlength=4.5;     // phototube length
const geant   RICeleclength=0.5;    // electronics
const geant   RICpmtsupport=1.5;   // suppot structure thickness
const integer Cerenkov_Photon=50;   // may be different in geant4???? check

const geant   RICradpos=-76.66;// Top of the radiator position
const geant   sq2=1.4142135623;// useful constant:sqrt(2)

const integer RICmaxentries=44;
const integer RICmaxrows=100;

const integer RICentries=44;   // no. of entries in wave_length,eff...   
const integer RICmaxpmts=2000; // max no. of pmts
const integer RICnwindows=16;  // Number of windows for pmt
const integer RIChistos=30000; // Starting point for RICH histos 
}
using namespace richconst;
				   
class RICHDB { 
public: // To be migrated in the future

  //---------- GEOMETRY PARAMETERS: unused by now
  
  // RICH detector features
  static geant top_radius;
  static geant bottom_radius;
  static geant height;
  static geant hole_radius;


  // Inner mirror features
  static geant inner_mirror_height;

  // Radiator features
  static geant rad_radius;
  static geant rad_height;
  static geant rad_tile_size;

  // Light guide features
  static geant lg_height;
  static geant lg_tile_size;


  //---------- RADIATOR PARAMETERS
  static integer entries;
  
  static geant wave_length[RICmaxentries];
  static geant index[RICmaxentries];
  static geant abs_length[RICmaxentries];


  //---------- PMT PARAMETERS
  static geant eff[RICmaxentries];
  static geant ped;
  static geant sigma_ped;
  static geant peak;
  static geant sigma_peak;

  // For detection purposes

  static integer c_ped;


  //--------- PMT array parameters
  static integer n_rows[2];
  static integer n_pmts[15][2];
  static integer total; // Total number of pmts in the actual setup
  static geant pmt_p[RICmaxpmts][2];			     
 


public:

  //Initialisation

  // Add news pmts: called from amsgeom... to be eliminated
  static void add_row(geant);
  static void add_pmt();

  // Get coordinates given PMT number and window number
  static geant x(integer pmt,integer window);
  static geant y(integer pmt,integer window);

  static geant pmt_response(integer n_photons);
  static void bookhist();
  static geant total_height();
  static geant mirror_pos();
  static geant rad_pos();
  static geant pmt_pos();
  static geant elec_pos();
  static geant cato_pos();
  static geant lg_pos();
  static geant lg_mirror_angle(integer);
  static geant lg_mirror_pos(integer);
};

#endif










