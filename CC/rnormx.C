//  $Id: rnormx.C,v 1.14 2010/10/08 17:27:59 mmilling Exp $
// Author V. Choutko 24-may-1996
//                    8-jul-1996 

//            pi=3.1415926
//77           qq=rndm(dummy)
//           if(qq.eq.0.)then
//c            write(*,*)' rndm eq 0'
//            goto 77
//           endif
//           RGEN1=SQRT(-2.*ALOG(qq))
//           RGEN2=2.*PI*Rndm(dummy)
//           R1=RGEN1*COS(RGEN2)
//           end

#include <math.h>
#include "random.h"
#include "TRandom1.h"

#include "typedefs.h" 
static TRandom1 rr; 
double ROOTRndm(int dummy){
//  VC OMLY ONE RNDM ALLOWED
//return RNDM(dummy);
//PZ this is used only *Outside* gbatch 
//   into the shared lib to get rid of the 
//   fortran part (look at random.h)
  return rr.Rndm();
}

extern "C" void ROOTPoissn(float& mean, int& val, int& dummy) {
  val = rr.Poisson(mean);
}

#include "typedefs.h"
#include  "commons.h"
extern "C" number rnormx(){
  double const pi=3.1415926;
  number qq,qq2;
  geant dummy=0;
  do {
    qq=RNDM(dummy);
    qq2=2*pi*RNDM(dummy);
  }while (qq==0.);
  return cos(qq2)*sqrt(-2*log(qq));

}
