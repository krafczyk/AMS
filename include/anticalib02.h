#include <typedefs.h>
#include <antidbc02.h>  
//  Some classes for calibrations. E.Choumilov
//-----------------------------------------------------------------------
//  class to manipulate with ANTI-calibration data :
class AntiCalib {
private:
  static number ambinl[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in low_bin(event-by-event)
  static integer nevbl[2*ANTI2C::ANTISRS];//total events, accum. in low_bin
  static number xcol[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in low_bin(event-by-event)
  static number ambinh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.adcs in high_bin(event-by-event)
  static integer nevbh[2*ANTI2C::ANTISRS];//total events, accum. in high_bin
  static number xcoh[2*ANTI2C::ANTISRS][ANTI2C::ANACMX];//chan.xcoords in high_bin(event-by-event)
// for attlen calc. using r=A(is=2)/A(is=1)  
  static integer nebl[ANTI2C::ANTISRS];//low(-z) bin
  static integer nebh[ANTI2C::ANTISRS];//high(+z) bin
  static number attll[ANTI2C::ANTISRS];
  static number attlh[ANTI2C::ANTISRS];
  static number zcol[ANTI2C::ANTISRS];
  static number zcoh[ANTI2C::ANTISRS];
public:
  static void init();
  static void fill(integer secn, geant ama[2], geant coo);
//  static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
//  static void melfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
  static void select();
  static void fit();
};
//--------------------------
