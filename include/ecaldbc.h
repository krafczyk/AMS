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
const integer ECROTN=10000; // geant numbering of ECAL rot. matr.(starting from...)
const integer ECJSTA=10; // max size of caunter-array for job statistics
const integer ECHIST=2000;// Ecal histogram number(starting from...) 
//
//geometry :
//
//===========================================================================
// 
// --------------> Class for "time-stable("geom")"  parameters :
class ECALDBc {  
//
private:
// geom. constants:
  static geant _gendim[10]; // ECAL general dimensions
//                            1-3-> X,Y,Z-sizes of EC-radiator;
//                            4-> eff. X(Y)-thickness of PMT+electronics volume
//                            5->X-pos; 6->Y-pos; 7->Z-pos(front face);
  static geant _fpitch[3]; // fiber pitch:
//                           1-> in X(Y); 2/3->Z(inside of x(y) cell and between x-y)
  static geant _rdcell[10];// readout-cell parameters
//                          1-4->fiber att.length,%,diam; 5->size(dx=dz) of "1/4" of PMT-cathode;
//                          6->abs(x(z)-position) of "1/4" in PMT coord.syst.;
//                          7/8->X(Y)/Z-pitch of PMT's; 
//                          9/10-> direction of readout in X/Y(= +-1.)
// 
  static integer _nfibpl[2];// num.of fibers per odd/even(1st/2nd) elementary layer in s-layer
  static integer _slstruc[5];//1st super-layer projection(0->X, 1->Y);
//                             num. of fib.layers per super-layer; num.of super-layers (X+Y);
//                             num. of PMT's per super-layer; spare
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
  static geant fpitch(integer i);
  static geant rdcell(integer i);
  static integer nfibpl(integer i);
  static integer slstruc(integer i);
//  
  static integer debug;
//
};
//===========================================================================
//
// ---------------> Class for JobControlStatistics  :
//
class EcalJobStat{
//
private:
  static integer mccount[ECJSTA];// event passed MC-cut "i"
//          i=0 -> entries
//          i=1 => MCHits->RawEvent OK
  static integer recount[ECJSTA];// event passed RECO-cut "i"
//          i=0 -> entries
//          i=1 -> 
public:
  static void clear();
  inline static void addmc(int i){
    assert(i>=0 && i< ECJSTA);
    mccount[i]+=1;
  }
  inline static void addre(int i){
    assert(i>=0 && i< ECJSTA);
    recount[i]+=1;
  }
  static void printstat();
  static void bookhist();
  static void bookhistmc();
  static void outp();
  static void outpmc();
};
//
#endif

