// Author C.J. Delgado (1999)


#ifndef __RICHDB__
#define __RICHDB__ 

#include<job.h>
#include<commons.h> 

// Constants

const integer RICnrot=30001;   // Rot. matrix no.
const geant   RICmithk=0.5;    // Mirror and 2*aerogel support thickness
const geant   RIClgthk=0.05;   // Light guides thickness
const geant   RICotherthk=0.1; // Shielding and PMT window

const integer RICmaxentries=44;
const integer RICmaxrows=100;

const integer RICentries=44;   // no. of entries in wave_length,eff...   
const integer RICmaxpmts=2000; // max no. of pmts
const integer RICnwindows=16;  // Number of windows for pmt
const integer RIChistos=30000; // Starting point for RICH histos 


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


  //--------- PMT array parameters
  static geant first[RICmaxrows];
  static integer n_rows;
  static integer total; // Total number of pmts in the actual setup
  static integer n_pmts[RICmaxrows];
  
  static integer c_ped;


public:

  //Initialisation
//  static void standard(); // Fill everything with the standards
  // Add news pmts: called from amsgeom
  static void add_row(geant);
  static void add_pmt();
  // Get coordinates given PMT number and window number
  static geant x(integer pmt,integer window);
  static geant y(integer pmt,integer window);
  static geant pmt_response(integer n_photons);
  static void bookhist();
  static char *name(char beg,int copy);
};

#endif

