// Original code by Choutko and Choumilov
//  Last mod. 19.09.97 AH.
#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <trigger1.h>
// #include <tofdbc.h>
// struct int_array {int size; int *array;};
//
class Triggerlvl1D: public ooObj{
private:
 integer _TrigMode;
 integer _TofFlag;   // =1/2/3 -> "z>=1"/"z>1"/"z>2" 
//  struct int_array _tofpatt;
 integer _TofPatt[SCLRS];// TOF:  triggered paddles/layer pattern
 integer _AntiPatt;  // ANTI: triggered sectors pattern 
 
public:
 Triggerlvl1D() {};
 Triggerlvl1D(TriggerLVL1* p);

  integer getTrigMode () const {return _TrigMode;}
  integer getTofFlag() {return _TofFlag;}
  void getTofPatt(integer* TofPatt);
  integer getAntiPatt() {return _AntiPatt;}
};
