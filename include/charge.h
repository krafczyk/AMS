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
const int ncharge=10; // e,p,He...C+3
const int nbins=100;
const int TOFTypes=2, TrackerTypes=3;
const int TOFMaxHits=4, TrackerMaxHits=6;
const number powmx=50.;
class AMSCharge: public AMSlink{
protected:
  AMSBeta * _pbeta;      // pointer to beta --> then to track & tof

 integer _ChargeTOF;
 integer _ChargeTracker;
 number _TrMeanTracker;
 number _TrMeanTOF;
 number _TrMeanTOFD;
 number  _ProbTOF[ncharge];       //prob  to be e,p,He,...,F
 number  _LkhdTOF[ncharge];       //loglikhd value for e,p,...
 integer _IndxTOF[ncharge];       //index 0,...,9 from most to least prob charge
 integer _iTOF;                   //index of most probable charge
 number  _ProbTracker[ncharge];   //prob  to be e,p,He,...,F
 number  _LkhdTracker[ncharge];   //loglikhd value for e,p,...
 integer _IndxTracker[ncharge];   //index 0,...,9 from most to least prob charge
 integer _iTracker;               //index of most probable charge
 number  _ProbAllTracker;         //prob of maximum charge using all tracker clusters

 integer _sortlkhd(integer sort);
 number _probcalc(int mode, int fit, int nhittyp[],number lkhd[]);
 void _copyEl();
 void _printEl(ostream & stream){ stream << " ChargeTOF "<<_ChargeTOF<<" ChargeTracker "<<_ChargeTracker<<" ProbTOF "<<_ProbTOF[0]<<" "<<_ProbTOF[1]<<" "<<_ProbTOF[2]<<" "<<_ProbTOF[3]<<" "<<_ProbTOF[4]<<" "<<_ProbTOF[5]<<" "<<_ProbTOF[6]<<" "<<" ProbTracker "<<_ProbTracker[0]<<" "<<_ProbTracker[1]<<" "<<_ProbTracker[2]<<" "<<_ProbTracker[3]<<" "<<_ProbTracker[4]<<" "<<_ProbTracker[5]<<" "<<_ProbTracker[6]<<" "<<endl;}
 void _writeEl();
 static char _fnam[128];
   static integer _sec[2];
   static integer _min[2];
   static integer _hour[2];
   static integer _day[2];
   static integer _mon[2];
   static integer _year[2];
   static geant _lkhdTOF[TOFTypes][ncharge][nbins];
   static geant _lkhdTracker[TrackerTypes][ncharge][nbins];
   static geant _lkhdStepTOF[TOFTypes][ncharge];
   static geant _lkhdStepTracker[TrackerTypes][ncharge];
   static geant _lkhdNormTOF[TOFTypes][ncharge];
   static geant _lkhdNormTracker[TrackerTypes][ncharge];
   static geant _lkhdSlopTOF[TOFTypes][ncharge];
   static geant _lkhdSlopTracker[TrackerTypes][ncharge];
   static integer _chargeTracker[ncharge];
   static integer _chargeTOF[ncharge];
public:
  AMSCharge *  next(){return (AMSCharge*)_next;}
  AMSCharge(): AMSlink(),  _pbeta(0){}
  AMSCharge(AMSBeta * pbeta, number trtr, number trtof, number trtofd): AMSlink(), _pbeta(pbeta), _TrMeanTracker(trtr),_TrMeanTOF(trtof),_TrMeanTOFD(trtofd){}
  integer getvotedcharge(int & index);
  inline number getrmeantrk(){return _TrMeanTracker/2;}
  inline number getrmeantof(){return _TrMeanTOF;}
  inline number getrmeantofd(){return _TrMeanTOFD;}
  number getprobcharge(integer charge);
  integer getchargeTOF()const{return _ChargeTOF;}
  integer getchargeTracker()const{return _ChargeTracker;}
  AMSBeta * getpbeta()const{return _pbeta;}
  static void addnext(AMSBeta* pbeta, integer nhitTOF, integer nhitTracker, AMSTOFCluster *pTOFc[], number EdepTOF[TOFTypes][TOFMaxHits], AMSTrCluster *pTrackerc[TrackerTypes-1][TrackerMaxHits], number EdepTracker[TrackerTypes-1][TrackerMaxHits], number trtr, number trtof, number trtofd);
  int FitTOF(int toffit, number beta, int bstatus, int nhitTOF, AMSTOFCluster *pTOFc[], number etof[TOFTypes][TOFMaxHits]);
  int FitTracker(int trkfit, number beta, int bstatus, int nhitTracker, AMSTrCluster *pTrackerc[TrackerTypes-1][TrackerMaxHits], number etrk[TrackerTypes-1][TrackerMaxHits]);
  static number resmax(number x[],int ntot,int refit,number rescut,int &imax,number &mean,number &trres,number &trmax);
  static void lkhcalc(int mode, number beta, int nhit, number ehit[], int typeh[], number lkh[]);
  static void init();
  static integer build(integer refit=0);
  static void print();
//+
#ifdef __DB__
   friend class AMSChargeD;
#endif
void setBetaP (AMSBeta* p) { _pbeta = p;}
//-
friend class AMSJob;
};


class AMSChargeI{
public:
AMSChargeI();
private:
static integer _Count;
};
static AMSChargeI cI;


#endif
