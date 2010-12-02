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
 typedef map <uinteger,BValues> BValues_m;
 typedef map <uinteger,BValues>::iterator BValues_i;
  BValues_m   fBValues;
  map <uint64,GPSTime> fGPSTime;
  map <uinteger,AMSSetupR::ISSData> fISSData;
  map <uinteger,AMSSetupR::TTCS> fTTCS;
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
ClassDef(AMSSetupR,1)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
};
#endif
