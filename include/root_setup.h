//  $Id: root_setup.h,v 1.10 2011/03/08 23:21:58 mmilling Exp $
#ifndef __ROOTSETUP__
#define __ROOTSETUP__

#include <typedefs.h>
#include <map>
#include <vector>
#include "TObject.h"
#include "TString.h"
#include "TTree.h"
#include "trigger102_setup.h"
class SlowControlDB;
class AMSEventR;
class AMSTimeID;
class AMSSetupR{
public:

//! SlowControlDB reimplementation
/*!
\sa AMSSetupR
\author vitali.choutko@cern.ch
*/

/*class SlowControlR{
public:
class SubType{
public:
typedef map <unsigned int,float> table_m;
typedef map <unsigned int,float>::iterator table_i;
table_m fTable;
ClassDef (SubType,1) //SubType
};

class DataType{
typedef map <int,SubType> stable_m;
typedef map <unsigned int,float>::iterator stable_i;
public:
stable_m fSTable;
ClassDef (DataType,1) //DataType
};

class Node{
public:
typedef map <int,DataType> dtable_m;
typedef map <int,DataType>::iterator dtable_i;
int Number; ///<  ?
dtable_m fDTable; ///< Map of Datatypes by int id ?
ClassDef (Node,1) //Node
};

public:
unsigned int fBegin; ///<Begin Validity
unsigned int fEnd;  /// <End Validity
typedef map <string,Node> ntable_m;
typedef map <string,Node>::iterator ntable_i;
ntable_m fNTable;
SlowControlR():fBegin(0),fEnd(0){}
ClassDef (SlowControlR,1) //SlowControlR
  /// Returns the value of a quantity with a given name at a given timestamp
	/*! 
	 \param name       the name of the desired quantity
        \param dt
         \param st
	 \param timestamp  Unix time (sec from 1970)
	 \param frac       second fraction after the timestamp
         \param val        return value  
	 \param imethod   0 = the closer in time
	 1 = linear interplolation
	 \return  0   success
                  1  no name found
                  2  no dt found
                  3  no st found
                  4  outside of bounds
  
	 *
int GetData(const char * name, int dt, int st, unsigned int time, float frac, int imethod, float &value);

};*/

//! AMSTimeID info used to create a given file
/*!
\sa AMSSetupR
\author vitali.choutko@cern.ch
*/
class TDVR{
public:
unsigned int Begin;  ///<Begin Validity
unsigned int End;  ///<Begin Validity
unsigned int Insert;  ///<Begin Validity
unsigned int Size; ///<Size in 32bit integers
unsigned int CRC; ///<CRC
vector<unsigned int> Data; ///<Data  (not filling by default)
int DataMC; ///<0 MC 2: Data
TString Name; ///< TDV Name; TString because of root can't properly handle string
TString FilePath; ///<File Path
TDVR():Begin(0),End(0),Insert(0),CRC(0),Size(0),DataMC(0),Name(""),FilePath(""),Data(){}
TDVR(const TDVR &a):Begin(a.Begin),End(a.End),Insert(a.Insert),CRC(a.CRC),Size(a.Size),Name(a.Name),FilePath(a.FilePath),Data(a.Data){}
  friend ostream &operator << (ostream &o, const TDVR &b );
private:
  virtual ostream & print(ostream &) const;
ClassDef (TDVR,4) //TDVR
};



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
  //  SlowControlR fSlowControl;
  typedef map <unsigned int,TDVR> TDVR_m;
 typedef map <unsigned int,TDVR>::iterator TDVR_i;
 typedef map <unsigned int,TDVR>::reverse_iterator TDVR_ri;
 typedef map <string,TDVR_m> TDVRC_m;
 typedef map <string,TDVR_m>::iterator TDVRC_i;
 TDVRC_m fTDVRC;
 vector<TDVR> fTDV_Time; ///<Return of getAllTDV thanks to rootcint bug;
 vector<TDVR> fTDV_Name; ///<Return of getAllTDV thanks to rootcint bug;
 void printAllTDV_Time(){for( int i=0;i<fTDV_Time.size();i++){cout <<fTDV_Time[i]<<endl;}}
 void printAllTDV_Name(){for (int i=0;i<fTDV_Name.size();i++){cout <<fTDV_Name[i]<<endl;}} 
int  getAllTDV(unsigned int time); ///< Get All TDV for the Current Time Returns fTDV_Time
 int getAllTDV(const string & name);  ///<Get All TDV for the current TDV name; Returns fTDV_Name 
 int getTDV(const string & name, unsigned int time, TDVR & tdv); ///<Return TDV tdv with name name for time time; return codes: 0 success; 1 no such name; 2: no valid record for time t
 typedef map <unsigned int,BValues> BValues_m;
 typedef map <unsigned int,BValues>::iterator BValues_i;
 typedef map <unsigned int,GPSTime> GPSTime_m;
 typedef map <unsigned int,GPSTime>::iterator GPSTime_i;
 typedef map <unsigned int,ISSData> ISSData_m;
 typedef map <unsigned int,ISSData>::iterator ISSData_i;
  BValues_m   fBValues; ///< Magnetic Field  from CCEB
  GPSTime_m fGPSTime;    ///< GPS Time
  ISSData_m fISSData;    ///< ISS Aux Data

   typedef map <unsigned long long ,ScalerMon> Scalers_m;
   typedef map <unsigned long long,ScalerMon>::iterator Scalers_i;
   Scalers_m fScalers; ///<  Scalers Map

   typedef multimap <unsigned int,Lvl1TrigConfig> LVL1Setup_m;
   typedef multimap <unsigned int,Lvl1TrigConfig>::iterator LVL1Setup_i;
   LVL1Setup_m fLVL1Setup; ///<  LVL1Setup Map

  const char * BuildTime(){time_t tm=fHeader.BuildTime;return ctime(&tm);};

protected:
void TDVRC_Purge(); ///< Purge TDVRC map
void TDVRC_Add(unsigned int time,AMSTimeID * tdv);
void Add(SlowControlDB *s);
friend class AMSTimeID;
friend class SlowControlDB;
static AMSSetupR * _Head;
public:
void Purge();
static    AMSSetupR * gethead(){return _Head;}
 void CreateBranch(TTree *tree, int brs);
 bool UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime);
 bool FillHeader(uinteger run); //fillHeader at run start by database
 bool FillSlowcontrolDB(const char * file);
 bool LoadSlowcontrolDB(const char * file, unsigned int t1, unsigned int t2);
 void UpdateHeader(AMSEventR* ev);
 void Reset();
 AMSSetupR();
 void Init(TTree *tree);
ClassDef(AMSSetupR,5)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
};
#endif
