#ifndef __ROOTSETUP__
#define __ROOTSETUP__
#include <typedefs.h>
#include <map>
#include "TObject.h"
#include "TTree.h"
class AMSEventR;
class AMSSetupR{
public:
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
  const char * BuildTime(){time_t tm=fHeader.BuildTime;return ctime(&tm);};
protected:
static AMSSetupR * _Head;
public:
static    AMSSetupR * gethead(){return _Head;}
 void CreateBranch(TTree *tree, int brs);
 bool UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime);
 void UpdateHeader(AMSEventR* ev);
 AMSSetupR();
 void Init(TTree *tree);
ClassDef(AMSSetupR,2)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
};
#endif
