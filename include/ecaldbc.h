// Author E.Choumilov 14.07.99.
//
//
#ifndef __ECALDBC__
#define __ECALDBC__
#include <cern.h>
#include <commons.h>
#include <amsdbc.h>
#include <extC.h>
#include <string.h>
#include <typedefs.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include <job.h>
//
// ECAL global constants definition
//
//geometry :
//
//const integer SCMXBR=14; // max nmb of bars/layer
//===========================================================================
// 
// --------------> Class for "time-stable"  parameters :
class ECALDBc {  
//
private:
// geom. constants:
  static geant _gendim[10]; // ECAL general dimensions
//
public:  
//
//  Member functions :
//
// ---> function to read geomconf-file
  static void readgconf();
//
//
  static geant gendim(integer i);
//  
  static integer debug;
//
};
#endif

