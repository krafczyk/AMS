// Author V. Choutko 4-mar-1997

#ifndef __AMSTRCALIB__
#define __AMSTRCALIB__
#include <typedefs.h>
#include  <commons.h>
#include <link.h>
#include <point.h>
#include <particle.h>
class AMSTrCalibPar{
protected:
AMSPoint _Coo;
AMSDir _Dir[3];
public:
AMSTrCalibPar(){};
AMSTrCalibPar(AMSPoint coo, number nrm[3][3]);
AMSTrCalibPar(AMSPoint coo, AMSDir dir[3]);
AMSPoint  getcoo()const {return _Coo;}
AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
};

const integer tkcalpat=4;
class AMSTrCalibration: public AMSlink{
protected:
 integer _Pattern;
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " Pattern " << _Pattern<<endl;}
 void _writeEl(){}
public:
 AMSTrCalibration(integer pattern=0):AMSlink(0),_Pattern(pattern){}
};


class AMSTrCalibData{
protected:
  AMSPoint _Hits[3];
  AMSPoint _EHits[3];
  geant _Beta;
  geant _InvMomentum;
  geant _ErrInvMomentum;
public:
AMSTrCalibData():_Beta(0){};
void Init(AMSBeta * pbeta, AMSTrTrack *ptrack, integer pattern);
integer Select(integer alg, integer pattern);
integer PatternMatch(integer patcal,integer pattr);
friend class AMSTrCalibFit;
};

class AMSTrCalibFit{
protected:
integer  _Pattern; // 123, 234, 345, 456 : 1,2,3,4
integer _PositionData;
integer _PositionIter;
integer _NData;
integer _NIter;
AMSTrCalibData * _pData;
integer _Algorithm;
integer _NSen;    // Sensor number
AMSTrCalibPar * _pParC[3];       // pointer to fitted current par
AMSTrCalibPar * _pParM;          // Pointer to fitted mean parameters
AMSTrCalibPar * _pParS;          // Pointer to fitted sigmas parameters
static  AMSTrCalibFit  * _pCalFit[2][tkcalpat];
public:
AMSTrCalibFit();
AMSTrCalibFit(integer pattern, integer data, integer iter, integer alg);
integer Test();
void Fit();
void Anal();
static AMSTrCalibFit * getHead(integer alg, integer pat)
  {return pat>=0 && pat<tkcalpat && alg>=0 && alg<2 ? _pCalFit[alg][pat]:0;}
static void setHead(integer alg, integer pat, AMSTrCalibFit * ptr);

~AMSTrCalibFit();
AMSTrCalibPar * getparM(integer layer, integer ladder, integer sensor);
AMSTrCalibPar * getparS(integer layer, integer ladder, integer sensor);

};


#endif
