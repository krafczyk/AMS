#include<Statistics.hh>
#include <math.h>
#include<iostream>

namespace ACsoft {

/**
 * This routine calculates the correct 1-sigma Poisson distributed uncertainty of the integer value val.
 * Above 20 the Square-root is calculated. Below twenty the values given in (Feldman & Cousins, Phys. Rev. D, 1998, Table 2, no background) are used.
 * So all uncertaintites, also for small numbers, are correct.
 */
void Utilities::PoissonUncertainty(int val, double& lowerError, double& upperError){

  if(val>20){ //sqrt
    lowerError=sqrt(val);
    upperError=sqrt(val);
  }
  else{
    switch(val){
    case 0: lowerError=0.0; upperError=1.29;
            break;
    case 1: lowerError=0.63; upperError=1.75;
            break;
    case 2: lowerError=1.26; upperError=2.25;
            break;
    case 3: lowerError=1.90; upperError=2.30;
            break;
    case 4: lowerError=1.66; upperError=2.78;
            break;
    case 5: lowerError=2.25; upperError=2.81;
            break;
    case 6: lowerError=2.18; upperError=3.28;
            break;
    case 7: lowerError=2.75; upperError=3.30;
            break;
    case 8: lowerError=2.70; upperError=3.32;
            break;
    case 9: lowerError=2.67; upperError=3.79;
            break;
    case 10: lowerError=3.22; upperError=3.81;
            break;
    case 11: lowerError=3.19; upperError=3.82;
            break;
    case 12: lowerError=3.17; upperError=4.29;
            break;
    case 13: lowerError=3.73; upperError=4.30;
            break;
    case 14: lowerError=3.70; upperError=4.32;
            break;
    case 15: lowerError=3.68; upperError=4.32;
            break;
    case 16: lowerError=3.67; upperError=4.80;
            break;
    case 17: lowerError=4.21; upperError=4.81;
            break;
    case 18: lowerError=4.19; upperError=4.82;
            break;
    case 19: lowerError=4.18; upperError=4.82;
            break;
    case 20: lowerError=4.17; upperError=5.30;
            break;
    default: lowerError=0; upperError=0;
             std::cout << "WARNING! No valid input for Utilities::PoissonError" << std::endl;
    } //end switch
  } // end else

  return;
}

}
