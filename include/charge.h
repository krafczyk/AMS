// V. Choutko 5-june-96
//
// July 12, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. implementation of friend class
//
// Last Edit Oct  10, 1996.  ak. 
//
#ifndef __AMSCHARGE__
#define __AMSCHARGE__
#include <link.h>
#include <trrec.h>
#include <tofrec.h>
#include <beta.h>
const integer ncharge=9; // e,p,He...C+2
class AMSCharge: public AMSlink{
protected:
  AMSBeta * _pbeta;      // pointer to beta --> then to track & tof

 integer _ChargeTOF;
 integer _ChargeTracker;

 number _ProbTOF[ncharge];       //(prob) to be e, p, He, C
 number _ProbTracker[ncharge];    // (prob) to be e, p, He, C

 void _copyEl();
  void _printEl(ostream & stream){ stream << " ChargeTOF "<<_ChargeTOF<<" ChargeTracker "<<_ChargeTracker<<" ProbTOF "<<_ProbTOF[0]<<" "<<
_ProbTOF[1]<<" "<<_ProbTOF[2]<<" "<<_ProbTOF[3]<<" "<<_ProbTOF[4]<<" "<<_ProbTOF[5]<<" "<<_ProbTOF[6]<<" "<<" ProbTracker "<<_ProbTracker[0]<<" "<<_ProbTracker[1]<<" "<<_ProbTracker[2]<<" "<<_ProbTracker[3]<<" "<<_ProbTracker[4]<<" "<<_ProbTracker[5]<<" "<<_ProbTracker[6]<<" "<<endl;}
 void _writeEl();
 static number _lkhdTracker[ncharge][100];
 static number _lkhdTOF[ncharge][100];
 static number _lkhdStepTracker[ncharge];
 static number _lkhdStepTOF[ncharge];
 static integer _chargeTracker[ncharge];
 static integer _chargeTOF[ncharge];
 static char _fnam[128];
public:
  AMSCharge *  next(){return (AMSCharge*)_next;}
  AMSCharge(): AMSlink(0),  _pbeta(0){}
  AMSCharge(AMSBeta * pbeta): AMSlink(0), _pbeta(pbeta){}
  integer getchargeTOF()const{return _ChargeTOF;}
  integer getchargeTracker()const{return _ChargeTracker;}
  AMSBeta * getpbeta()const{return _pbeta;}
  static void addnext(number rid, AMSBeta* pbeta, integer nhitTOF, integer nhitTracker, number
   EdepTOF[4], number EdepTracker[6]);
   void Fit(number rid,integer nhitTOF, integer nhitTracker, number
   EdepTOF[4], number EdepTracker[6]);
  static void init();
  static void build();
  static void print();
//+
#ifdef __DB__
   friend class AMSChargeD;
#endif
void setBetaP (AMSBeta* p) { _pbeta = p;}
//-

};


class AMSChargeI{
public:
AMSChargeI();
private:
static integer _Count;
};
static AMSChargeI cI;


#endif
