// Author V. Choutko 4-mar-1997

#ifndef __AMSTRCALIB__
#define __AMSTRCALIB__
#include <typedefs.h>
#include  <commons.h>
#include <link.h>
#include <point.h>
#include <particle.h>
#include <trid.h>
const integer nrho=64;
class PSStr_def{
public:
integer Layer;
integer Ladder;
integer Half;
integer Side;
integer Strip;
geant Ped;
geant Sigma;
geant BadCh;
geant SigmaRaw;
geant Rho[nrho];
};
class AMSTrCalibPar{
protected:
AMSPoint _Coo;
AMSDir _Dir[3];
public:
AMSTrCalibPar(){};
AMSTrCalibPar(AMSPoint coo, number nrm[3][3]);
AMSTrCalibPar(AMSPoint coo, AMSDir dir[3]);
AMSTrCalibPar(const AMSPoint & coo, const AMSDir & dir1, 
const  AMSDir & dir2, const  AMSDir & dir3);
AMSPoint  getcoo()const {return _Coo;}
void setcoo(const AMSPoint & o) {_Coo=o;}
AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
AMSDir &  setmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
AMSTrCalibPar  operator +(const AMSTrCalibPar &o){
return AMSTrCalibPar(_Coo+o._Coo,_Dir[0]+o._Dir[0],
_Dir[1]+o._Dir[1],_Dir[2]+o._Dir[2]);
}         
AMSTrCalibPar  operator -(const AMSTrCalibPar &o){
return AMSTrCalibPar(_Coo-o._Coo,_Dir[0]-o._Dir[0],
_Dir[1]-o._Dir[1],_Dir[2]-o._Dir[2]);
}         
AMSTrCalibPar  operator *(const AMSTrCalibPar &o){
return AMSTrCalibPar(_Coo*o._Coo,_Dir[0]*o._Dir[0],
_Dir[1]*o._Dir[1],_Dir[2]*o._Dir[2]);
}         
AMSTrCalibPar  operator /(number o){
return AMSTrCalibPar(_Coo/o,_Dir[0]/o,_Dir[1]/o,_Dir[2]/o);
}
void sqr();

friend ostream &operator << (ostream &o, const  AMSTrCalibPar &b )
  {return o<<" "<<b._Coo<<" "<<b._Dir[0]<<" "<<b._Dir[1]<<" "<<b._Dir[2];}

};

const integer tkcalpat=5;
class AMSTrCalibration: public AMSlink{
protected:
 integer _Pattern;
 void _copyEl(){}
 void _printEl(ostream & stream){ stream << " Pattern " << _Pattern<<endl;}
 void _writeEl(){}
public:
 AMSTrCalibration(integer pattern=0):_Pattern(pattern){}
};


class AMSTrCalibData{
protected:
  integer _NHits;
  AMSPoint * _Hits;
  AMSPoint * _EHits;
  geant _InvBeta;
  geant _ErrInvBeta;
  geant _InvMomentum;
  geant _ErrInvMomentum;
static integer patpoints[2][tkcalpat];
static integer patconf[2][tkcalpat][6];
public:
AMSTrCalibData():_NHits(0),_Hits(0),_EHits(0),_InvBeta(0),_ErrInvBeta(0),
_InvMomentum(0),_ErrInvMomentum(0){};
integer Init(AMSBeta * pbeta, AMSTrTrack *ptrack, integer pattern, integer alg);
integer Select(integer alg, integer pattern);
integer PatternMatch(integer patcal,integer pattr);
friend class AMSTrCalibFit;
~AMSTrCalibData(){ delete [] _Hits; delete[] _EHits;}
};

class AMSTrCalibFit{
protected:
integer  _Pattern; 
integer _PositionData;
integer _PositionIter;
integer _NData;
integer _NIter;
AMSTrCalibData * _pData;
integer _Algorithm;
integer _NSen;    // Sensor number
AMSTrCalibPar * _pParC[6];       // pointer to fitted current par
AMSTrCalibPar * _pParM;          // Pointer to fitted mean parameters
AMSTrCalibPar * _pParS;          // Pointer to fitted sigmas parameters
static  AMSTrCalibFit  * _pCalFit[2][tkcalpat];
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
{};
static void alfun(integer & n, number xc[], number & fc, AMSTrCalibFit * ptr);
public:
AMSTrCalibFit();
AMSTrCalibFit(integer pattern, integer data, integer iter, integer alg);
integer Test();
void Fit();
void Anal();

static AMSTrCalibFit * getHead(integer alg, integer pat)
  {return pat>=0 && pat<tkcalpat && alg>=0 && alg<2 ? _pCalFit[alg][pat]:0;}
static void setHead(integer alg, integer pat, AMSTrCalibFit * ptr);
integer getlayer(integer c);
~AMSTrCalibFit();
AMSTrCalibPar * getparM(integer layer, integer ladder, integer sensor);
AMSTrCalibPar * getparS(integer layer, integer ladder, integer sensor);

};

class AMSTrIdCalib : public AMSTrIdSoft{
protected:

static integer * _Count;
static geant * _BadCh;
static number  * _ADC;
static number *_ADCMax;
static number * _ADC2;
static number * _ADCRho[nrho];
static number * _ADC2Raw;
static number * _ADCRaw;
static integer  _CmnNoiseC[10][ms];
static geant  _CmnNoise[10][ms];
static void _calc();
static void _hist();
static void _update();
static void _clear();
static time_t _BeginTime;
static time_t _CurTime;
static uinteger _CurRun;

public:
static void printbadchanlist();
static integer CalcBadCh(integer half, integer side);
static uinteger getrun(){return _CurRun;}
AMSTrIdCalib():AMSTrIdSoft(){};
AMSTrIdCalib(const AMSTrIdSoft & o):AMSTrIdSoft(o){};
static void initcalib();
static void check(integer forcedw=0);
 static void buildpreclusters( AMSTrIdSoft & idd, integer len, geant id[]);
static void buildSigmaPed(integer n, int16u* p);
static void buildSigmaPedA(integer n, int16u* p);
static void buildSigmaPedB(integer n, int16u* p);
inline getcount() const {return _Count[getchannel()];}
geant getcmnnoise() const {return _CmnNoise[_VANumber][_addr];}
void updcmnnoise(geant cmn){(_CmnNoise[_VANumber][_addr])+=cmn*cmn;}
void updcmnnoiseC(){(_CmnNoiseC[_VANumber][_addr])++;}
void updADC(geant ped){(_ADC[getchannel()])+=ped;}
void updBadCh(){_BadCh[getchannel()]++;}
void updADC2(geant ped){(_ADC2[getchannel()])+=ped*ped;}
void updADCRaw(geant ped){(_ADCRaw[getchannel()])+=ped;}
void updADC2Raw(geant ped){(_ADC2Raw[getchannel()])+=ped*ped;}
void updADCRho(geant ped1, geant ped2, integer k){(_ADCRho[k][getchannel()])+=ped1*ped2;}
void updADCMax(geant ped){if(_ADCMax[getchannel()]<ped)
                           _ADCMax[getchannel()]=ped;}
void updcounter(){(_Count[getchannel()])++;}
};



#endif
