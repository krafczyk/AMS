//  $Id: root_setup.h,v 1.3 2010/12/11 18:30:39 choutko Exp $
#ifndef __ROOTSETUP__
#define __ROOTSETUP__

#include <typedefs.h>
#include <map>
#include "TObject.h"
#include "TTree.h"
#include "trigger102_setup.h"
class AMSEventR;
class AMSSetupR{
public:
//! LVL1 Scalers
/*!

\sa AMSSetupR
\author vitali.choutko@cern.ch
*/
class Scalers { 
};
 
//! LVL1 Setup
/*!

\sa AMSSetupR
\author vitali.choutko@cern.ch
*/
class LVL1Setup { 
};


class BValues{
public:
float B[4][3];
float T[4];
ClassDef(BValues,1)
}; 
class TTCS{
public:
uinteger temp;
};

class GPSTime{
public:
uint64 time;
};

class ISSData{
public:
float Rad;
};
class Header{
public:
  uinteger Run;
  uinteger FEvent;
  uinteger LEvent;
  uinteger FEventTime;
  uinteger LEventTime;
  uinteger BuildTime;
  uinteger BuildNo;
  uinteger OS;
  Header():Run(0),FEvent(0),LEvent(0),FEventTime(0),LEventTime(0),
  BuildTime(0),BuildNo(0),OS(0){}
  ClassDef(Header,1)       //Header
//#pragma omp threadprivate(fgIsA)
};
public:
  Header fHeader;
 typedef map <unsigned int,BValues> BValues_m;
 typedef map <unsigned int,BValues>::iterator BValues_i;
 typedef map <unsigned int,GPSTime> GPSTime_m;
 typedef map <unsigned int,GPSTime>::iterator GPSTime_i;
 typedef map <unsigned int,ISSData> ISSData_m;
 typedef map <unsigned int,ISSData>::iterator ISSData_i;
 typedef map <unsigned int,TTCS> TTCS_m;
 typedef map <unsigned int,TTCS>::iterator TTCS_i;
  BValues_m   fBValues;
  GPSTime_m fGPSTime;
  ISSData_m fISSData;
  TTCS_m fTTCS;

   typedef map <unsigned long long ,ScalerMon> Scalers_m;
   typedef map <unsigned long long,ScalerMon>::iterator Scalers_i;
   Scalers_m fScalers; ///<  Scalers Map

   typedef multimap <unsigned int,Lvl1TrigConfig> LVL1Setup_m;
   typedef multimap <unsigned int,Lvl1TrigConfig>::iterator LVL1Setup_i;
   LVL1Setup_m fLVL1Setup; ///<  LVL1Setup Map

  const char * BuildTime(){time_t tm=fHeader.BuildTime;return ctime(&tm);};

protected:
static AMSSetupR * _Head;
public:
static    AMSSetupR * gethead(){return _Head;}
 void CreateBranch(TTree *tree, int brs);
 bool UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime);
 void UpdateHeader(AMSEventR* ev);
 void Reset();
 AMSSetupR();
 void Init(TTree *tree);
ClassDef(AMSSetupR,3)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
};
#endif
