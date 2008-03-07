//  $Id: trdcalib.h,v 1.2 2008/03/07 16:19:51 pzuccon Exp $
// Author V. Choutko 4-mar-1997

#ifndef __AMSTRDCALIB__
#define __AMSTRDCALIB__
#include "typedefs.h"
#include  "commons.h"
#include "link.h"
#include "point.h"
//PZ #include "particle.h"
//PZ 
#include "trdrec.h"
#include "trdid.h"
class AMSmceventg;
using namespace amsprotected;
class TRDCalib_def{
public:
integer Layer;
integer Ladder;
integer Tube;
geant Ped;
geant Sigma;
geant BadCh;
};

class AMSTRDIdCalib : public AMSTRDIdSoft{
protected:
static integer * _Count;
static geant * _BadCh;
static number  * _ADC;
static number *_ADCMax;
static number * _ADC2;
static number * _ADC2Raw;
static number * _ADCRaw;
static void _calc();
static void _hist();
static void _update();
static void _clear();
static time_t _BeginTime;
static time_t _CurTime;
static uinteger _CurRun;
static void _add();
public:
static void printbadchanlist();
static integer CalcBadCh(integer half, integer side);
static uinteger getrun(){return _CurRun;}
AMSTRDIdCalib():AMSTRDIdSoft(){};
AMSTRDIdCalib(const AMSTRDIdSoft & o):AMSTRDIdSoft(o){};
static void initcalib();
static void check(integer forcedw=0);
static void ntuple(integer s);
inline integer getcount() const {return _Count[getchannel()];}
void updADC(geant ped){(_ADC[getchannel()])+=ped;}
number getADC(){return _ADC[getchannel()];}
number getADCRaw(){return _ADCRaw[getchannel()];}
geant getBadCh(){return _BadCh[getchannel()];}
void updBadCh(){_BadCh[getchannel()]++;}
void updADC2(geant ped){(_ADC2[getchannel()])+=ped*ped;}
void updADCRaw(geant ped){(_ADCRaw[getchannel()])+=ped;}
void updADC2Raw(geant ped){(_ADC2Raw[getchannel()])+=ped*ped;}
void updADCMax(geant ped){if(_ADCMax[getchannel()]<ped)
                           _ADCMax[getchannel()]=ped;}
void updcounter(){(_Count[getchannel()])++;}
};



#endif
