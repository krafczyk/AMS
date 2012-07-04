//  $Id: root_setup.h,v 1.43.4.5 2012/07/04 12:41:31 mduranti Exp $
#ifndef __ROOTSETUP__
#define __ROOTSETUP__

#include <typedefs.h>
#include <map>
#include <math.h>
#include <vector>
#include "TObject.h"
#include "TString.h"
#include "TTree.h"
#include "trigger102_setup.h"
#include "RichConfig.h"


class SlowControlDB;
class DynAlFitContainer;
class RichConfigContainer;
class AMSEventR;
class AMSTimeID;
#ifdef __LINUXGNU__
struct dirent64;
#endif
#ifdef __DARWIN__
struct dirent;
#endif
class AMSSetupR{
public:

//! SlowControlDB reimplementation
/*!
\sa AMSSetupR
\author vitali.choutko@cern.ch
*/

class SlowControlR{
public:
static bool ReadFromExternalFile;
class Element{
public:
typedef map <unsigned int,float> table_m;
typedef map <unsigned int,float>::iterator table_i;
table_m fTable;
TString NodeName;  ///< Like TTCE
TString BranchName; ///< Like TTCE-A
int datatype; ///< DataType:
int subtype; ///< SubType
ClassDef (Element,1) //element
};
/*
class SubType{
public:
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
typedef map<string,unsigned long long> rtable_m;
typedef map<string,unsigned long long>::iterator rtable_i;
rtable_m fRTable; ///< Fast Search of Datatypes table value = (subtype<<32) | datatype
dtable_m fDTable; ///< Map of Datatypes by int id ?
ClassDef (Node,1) //Node
};
*/

public:
unsigned int fBegin; ///<Begin Validity
unsigned int fEnd;  ///<End Validity
unsigned int UnCompleted; ///< 0 if completed
typedef multimap <string,Element> etable_m;
typedef multimap <string,Element>::iterator etable_i;
etable_m fETable;   ///< Main Map of subtype name : elements
typedef map<unsigned long long,string> rtable_m;
typedef map<unsigned long long,string>::iterator rtable_i;
rtable_m fRTable;  ///< Conversion Map (To speed up the search)
void print(); ///< print element info for elements with non-zero entries
void printElementNames(const char*name=0); ///< print element names using in GetData
SlowControlR():fBegin(0),fEnd(0){}

         //! SlowControlElement Accessor
	/*! 
	 \param elementname       the name of the desired quantity or "" (any)
	\param nodename           nodfe name or "" (any)
         \param dt            datatype or -1  (any)
         \param st            subtype  or -1  (any)
	 \param timestamp  Unix time (sec from 1970)
	 \param frac       second fraction after the timestamp
         \param val        return array of values  
	 \param imethod   0 = the closer in time
	 1 = linear interplolation
	 \return  0   success

                  1  no elementname found

                  2  no nodename found

                  3  no dt and/or st found

                  4  outside of bound method 1

                  5  outside of bounds methods 0,1

                  6  element table empty

                  7  wrong method

                 -1  internal error
                 
	 */
int GetData(const char * elementname,unsigned int time, float frac, vector<float> &value , int imethod=1, const char *nodename="", int dt=-1, int st=-1);

ClassDef (SlowControlR,3)  //slowControl
};

//! AMSTimeID info used to create a given file
/*!
\sa AMSSetupR
\author vitali.choutko@cern.ch
*/
class TDVR{
public:
unsigned int Begin;  ///<Begin Validity
unsigned int End;  ///<End Validity
unsigned int Insert;  ///<Insert Time
unsigned int Size; ///<Size in 32bit integers
unsigned int CRC; ///<CRC
vector<unsigned int> Data; ///<Data  
int DataMC; ///<0 MC 2: Data
TString Name; ///< TDV Name; 
TString FilePath; ///<File Path
TDVR():Begin(0),End(0),Insert(0),CRC(0),Size(0),DataMC(0),Name(""),FilePath(""),Data(){}
TDVR(const TDVR &a):Begin(a.Begin),End(a.End),Insert(a.Insert),CRC(a.CRC),Size(a.Size),Name(a.Name),FilePath(a.FilePath),Data(a.Data){}
  friend ostream &operator << (ostream &o, const TDVR &b );
///  Copy out Data vector to Out Structure
///  \return true if success
bool CopyOut(void * Out); 
private:
  virtual ostream & print(ostream &) const;
 
ClassDef (TDVR,6) //TDVR
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



class GPS{
public:
unsigned int Run;  ///< Run
unsigned int Event;  ///<Event (Usually first event of Epoche)
unsigned int EventLast; ///<Last event with  the same Epoche
vector<unsigned int> Epoche; ///< GPS Time Epoche Format
GPS():Run(0),Event(0),EventLast(0){}
ClassDef(GPS,3)
};


class GPSWGS84{
public:
  float x; ///< x (km) in CTRS
  float y; ///< y
  float z; ///< z
  float vx; ///< v_x km/s
  float vy; ///< v_y km/s
  float vz; ///< v_z km/s
  unsigned int val; ///< validity
  float fom;    ///< figure of merit
  float fomv;   ///< figure of merit velocity
ClassDef(GPSWGS84,1)
};

class GPSWGS84R{
public:
  float r; ///< r (km) in CTRS
  float phi; ///< (rad)
  float theta; ///< (rad) 0 == equator
  float v; ///< velocity in rad/s
  float vphi; ///< (rad)
  float vtheta; ///< (rad)

  GPSWGS84R():r(0),phi(0),theta(0),v(0),vphi(0),vtheta(0){};

  GPSWGS84R(const GPSWGS84 &a){
    r=sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
    phi=atan2(a.y,a.x);
    theta=asin(a.z/r);
    v=sqrt(a.vx*a.vx+a.vy*a.vy+a.vz*a.vz);
    vphi=atan2(a.vy,a.vx);
    vtheta=asin(a.vz/v);
    v=v/r;
    r*=100000;
  }
ClassDef(GPSWGS84R,1)
};







class ISSData{
public:
TString Name;
TString TL1;
TString TL2;

};

class ISSAtt{
public:
float Roll; ///< Roll in LVLH, Rad
float Pitch; ///< Pitch in LVLH, Rad
float Yaw; ///< Yaw in LVLH, Rad
ClassDef(ISSAtt,1)       //ISS Attitude Data
};
class ISSSA{
public:
float alpha; ///< Solar Array alpha angle (degrees) alpha:0 means 3a and  1b is in ams field of view ;see also https://twiki.cern.ch/twiki/bin/view/AMS/ISSAuxData
float b1a; ///< beta 1a  degrees
float b3a; ///< beta 2a  --
float b1b; ///< beta 1b  --
float b3b; ///< beta 3b  --
ClassDef(ISSSA,1)       //ISS Solar Arrays Data
};
class ISSCTRS{
public:
float x; ///< x (km) in CTRS
float y; ///< y
float z; ///< z
float vx; ///< v_x km/s
float vy; ///< v_y km/s
float vz; ///< v_z km/s
ClassDef(ISSCTRS,1)       //ISS Solar Arrays Data
};
class ISSCTRSR{
public:
float r; ///< r (cm) in CTRS
float phi; ///< (rad)
float theta; ///< (rad) 0 == equator
float v; ///< velocity in rad/s
float vphi; ///< (rad)
float vtheta; ///< (rad)
ISSCTRSR():r(0),phi(0),theta(0),v(0),vphi(0),vtheta(0){};
ISSCTRSR(const ISSCTRS &a);
ClassDef(ISSCTRSR,1)       //ISS Solar Arrays Data
};

class ISSGTOD{
public:
float r; ///< r (cm) in GTOD
float phi; ///< (rad)
float theta; ///< (rad) 0 == equator
float v; ///< velocity in rad/s
float vphi; ///< (rad)
float vtheta; ///< (rad)
ISSGTOD():r(0),phi(0),theta(0),v(0),vphi(0),vtheta(0){};
ClassDef(ISSGTOD,1)       //ISS GTOD
};

//--------------DSP Errors-------------------------
 class DSPError{
 public:
   DSPError(){
     TimeStart=0;TimeEnd=0;
     Nodes_000_01F=0;Nodes_020_03F=0;Nodes_040_05F=0;Nodes_060_07F=0;
     Nodes_080_09F=0;Nodes_0A0_0BF=0;Nodes_0C0_0DF=0;Nodes_0E0_0FF=0;
     Nodes_100_11F=0;Nodes_120_13F=0;Nodes_140_15F=0;Nodes_160_17F=0;
     Nodes_180_19F=0;Nodes_1A0_1BF=0;Nodes_1C0_1DF=0;Nodes_1E0_1FF=0;
   };///< Standard constructor
   unsigned int TimeStart; ///< Start of NOT validity period due to DSP error (unixtime) (also key of the map AMSRootSetupR::DSPError_m). \li TimeStart<=t<TimeEnd is affected by DSP error
   unsigned int TimeEnd; ///< End of NOT validity period due to DSP error (unixtime). \li TimeStart<=t<TimeEnd is affected by DSP error
   unsigned int Nodes_000_01F; ///< Bitted map of affected nodes (from NA=0x000 [LSB] to NA=0x01F [MSB])
   unsigned int Nodes_020_03F; ///< Bitted map of affected nodes (from NA=0x020 [LSB] to NA=0x03F [MSB])
   unsigned int Nodes_040_05F; ///< Bitted map of affected nodes (from NA=0x040 [LSB] to NA=0x05F [MSB])
   unsigned int Nodes_060_07F; ///< Bitted map of affected nodes (from NA=0x060 [LSB] to NA=0x07F [MSB])
   unsigned int Nodes_080_09F; ///< Bitted map of affected nodes (from NA=0x080 [LSB] to NA=0x09F [MSB])
   unsigned int Nodes_0A0_0BF; ///< Bitted map of affected nodes (from NA=0x0A0 [LSB] to NA=0x0BF [MSB])
   unsigned int Nodes_0C0_0DF; ///< Bitted map of affected nodes (from NA=0x0C0 [LSB] to NA=0x0DF [MSB])
   unsigned int Nodes_0E0_0FF; ///< Bitted map of affected nodes (from NA=0x0E0 [LSB] to NA=0x0FF [MSB])
   unsigned int Nodes_100_11F; ///< Bitted map of affected nodes (from NA=0x100 [LSB] to NA=0x11F [MSB])
   unsigned int Nodes_120_13F; ///< Bitted map of affected nodes (from NA=0x120 [LSB] to NA=0x13F [MSB])
   unsigned int Nodes_140_15F; ///< Bitted map of affected nodes (from NA=0x140 [LSB] to NA=0x15F [MSB])
   unsigned int Nodes_160_17F; ///< Bitted map of affected nodes (from NA=0x160 [LSB] to NA=0x17F [MSB])
   unsigned int Nodes_180_19F; ///< Bitted map of affected nodes (from NA=0x180 [LSB] to NA=0x19F [MSB])
   unsigned int Nodes_1A0_1BF; ///< Bitted map of affected nodes (from NA=0x1A0 [LSB] to NA=0x1BF [MSB])
   unsigned int Nodes_1C0_1DF; ///< Bitted map of affected nodes (from NA=0x1C0 [LSB] to NA=0x1DF [MSB])
   unsigned int Nodes_1E0_1FF; ///< Bitted map of affected nodes (from NA=0x1E0 [LSB] to NA=0x1FF [MSB])
   bool SetNA(unsigned int NA); ///< Set the NA affected by DSP error (erase any previous set nodes!). \return true in case of positive inception.
   bool AddNA(unsigned int NA); ///< Add a NA affected by DSP error (keep previous nodes!). \return true in case of positive inception.
   bool AddNA(DSPError dsperr); ///< Add a NA affected by DSP error (keep previous nodes!) taking by a DSPError object (neglecting its timestart,timend). \return true in case of positive inception.
   int GetFirstNA(); ///< Gives NA affected by DSP error (in case of multiple nodes return the first [the one with lower NA] with negative sign)
   bool SearchNA(unsigned int NA); ///< Search if the passed NA is affected by the problem. \return true if that NA is affected
   ClassDef(DSPError,2)       //DSP Error
 };
//-------------------------------------------------

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
  typedef map <unsigned int,unsigned int> runtag_m;
  typedef map <unsigned int,unsigned int>::iterator runtag_i;
  runtag_m fRunTag;
  Header():Run(0),FEvent(0),LEvent(0),FEventTime(0),LEventTime(0),
  BuildTime(0),BuildNo(0),OS(0){}
        //! RunTagModifier 
        /*!


         \param unsigned int Time (unix time)
         \param unsigned int bit to be set/clear 
                bit 0  -> Bad MD ext alignment accuracy

           \return
               0   ok 
               1   bit>31
      */
  int SetRTB(unsigned int  Time,unsigned int bit);///< Set Bit and Add/Replace element 
  int ClearRTB(unsigned int  Time,unsigned int bit); ///< Clear Bit and Add/Replace element


       //! RunTagChecker
        /*!


         \param unsigned int Time (unix time)
         \param unsigned int bit to be checked
                bit 0  -> Bad MD ext alignment accuracy
                bit 32++ -> any bit
           \return
               false   bit not set
               true   bit  set
      */

  bool CheckRTB(unsigned int  Time,unsigned int bit); ///< Check bit (all word if bit>31) 

  ClassDef(Header,2)       //Header
//#pragma omp threadprivate(fgIsA)
};
public:
    Header fHeader;
    SlowControlR fSlowControl;
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
 int getTDVi(const string & name, unsigned int time, TDVR_i & tdvi); ///<Return TDVR_i terator  tdvi with name name for time time; return codes: 0 success; 1 no such name; 2: no valid record for time t
 typedef map <unsigned int,GPS> GPS_m;
 typedef map <unsigned int,GPS>::iterator GPS_i;
 typedef map <unsigned int,GPS>::reverse_iterator GPS_ri;
 GPS GetGPS(unsigned int run,unsigned int event);
 typedef map <unsigned int,ISSData> ISSData_m;
 typedef map <unsigned int,ISSSA> ISSSA_m;
 typedef map <unsigned int,ISSCTRS> ISSCTRS_m;
 typedef map <unsigned int,ISSGTOD> ISSGTOD_m;
 typedef map <double,ISSAtt> ISSAtt_m;
 //---------------DSP Errors-------------------------
 typedef map <unsigned int, DSPError> DSPError_m;
 //--------------------------------------------------
 typedef map <unsigned int,ISSData>::iterator ISSData_i;
 typedef map <unsigned int,ISSSA>::iterator ISSSA_i;
 typedef map <unsigned int,ISSCTRS>::iterator ISSCTRS_i;
 typedef map <unsigned int,ISSGTOD>::iterator ISSGTOD_i;
 typedef map <double,ISSAtt>::iterator ISSAtt_i;
 typedef map <unsigned int,GPSWGS84> GPSWGS84_m;
 typedef map <unsigned int,GPSWGS84>::iterator GPSWGS84_i;
 //---------------DSP Errors-------------------------
 typedef map <unsigned int, DSPError>::iterator DSPError_i;
 //--------------------------------------------------
 GPS_m fGPS;    ///< GPS Epoch Time
 GPSWGS84_m fGPSWGS84;  ///<  GPS Coo Data        
 ISSData_m fISSData;    ///< ISS Aux Data map
 ISSAtt_m fISSAtt;      ///< ISS Attitude angles map
 ISSSA_m fISSSA;      ///< ISS Solar Array angles map
 ISSCTRS_m fISSCTRS;      ///< ISS CTRS coordinates & velocity vector map
 ISSGTOD_m fISSGTOD;      ///< ISS GTOD coordinates & velocity vector map
//---------------DSP Errors-------------------------
 DSPError_m fDSPError; ///< DSP Error map (key is the start time of NOT validity period due to DSP Errors). Map 'second' contains DSPError object with start/end NOT validity and affected nodes
 //--------------------------------------------------
 typedef map <unsigned long long ,ScalerMon> Scalers_m;
 typedef map <unsigned long long,ScalerMon>::iterator Scalers_i;
 Scalers_m fScalers; ///<  Scalers Map
 vector<Scalers_i> fScalersReturn; ///< Return Vectors for getScalers function (thanks to rootcint bug)
 typedef multimap <unsigned int,Lvl1TrigConfig> LVL1Setup_m;
 typedef multimap <unsigned int,Lvl1TrigConfig>::iterator LVL1Setup_i;
 LVL1Setup_m fLVL1Setup; ///<  LVL1Setup Map
 typedef map <int,DynAlFitContainer> DynAlignment_m;
 DynAlignment_m fDynAlignment;
 typedef vector<RichConfigContainer> RichConfig_m; 
 RichConfig_m fRichConfig;
 
 const char * BuildTime(){time_t tm=fHeader.BuildTime;return ctime(&tm);};
 
void TDVRC_Purge(); ///< Purge TDVRC map
void TDVRC_Add(unsigned int time,AMSTimeID * tdv);
protected:
void Add(SlowControlDB *s);
friend class AMSTimeID;
friend class SlowControlDB;
static AMSSetupR * _Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (_Head)
#endif
#ifdef __DARWIN__
static int _select ( dirent * entry);
#endif
#ifdef __LINUXGNU__
static int _select (const dirent64 * entry);
#endif
public:
         //! ISS Att roll,pitch,yaw accessor
	/*! 
            

	 \param double xtime (unix time + fraction of second)
         \param roll,pitch,yaw interpolated values      
	   
             
           \return 
               0   ok (interpolation)
               1   ok  (extrapolation)
               2   no data                  
	 */
  int getISSAtt( float& roll,float&pitch,float&yaw, double xtime); 
         //! ISS Coo & Velocity CTRS accessor
	/*! 
            

	 \param double xtime (unix time + fraction of second)
         \param ISSSCTRSR a  interpolated values      
	   
             
           \return 
               0   ok (interpolation)
               1   ok  (extrapolation)
               2   no data                  
	 */
  int getISSCTRS(ISSCTRSR & a, double xtime); 

         //! ISS Coo & Velocity GPS accessor
	/*! 
            

	 \param double xtime (unix time + fraction of second)
         \param GPSWGS84R a  interpolated values      
	   
             
           \return 
               0   ok (interpolation)
               1   ok  (extrapolation)
               2   no data                  
	 */
  int getGPSWGS84(GPSWGS84R & a, double xtime); 


         //! ISS Coo & Velocity GTOD accessor
	/*! 
            

	 \param double xtime (unix time + fraction of second)
         \param ISSSGTOD a  interpolated values      
	   
             
           \return 
               0   ok (interpolation)
               1   ok  (extrapolation)
               2   no data                  
	 */
  int getISSGTOD(ISSGTOD & a, double xtime); 
         //! ISS Solar Angles Accessor
	/*! 
            

	 \param double xtime (unix time + fraction of second)
         \param ISSSSA a  interpolated values for the angles      
	   
             
           \return 
               0   ok (interpolation)
               1   ok  (extrapolation)
               2   no data                  
	 */
  int getISSSA(ISSSA & a, double xtime); 

   
  //---------------DSP Errors-------------------------
  //! DSP Errors accessor
  /*! 
    
    \param unsigned int time : unix time
    \param DSPError dsperror : object containing (if any DSP error affecting 'time') time interval of NOT validity and nodes affected
    
    \retval  0 -->  OK (NO nodes affected by DSP errors)
    \retval  1 -->  KO (SOME nodes affected by DSP errors)
  */
  int getDSPError(DSPError& dsperror, unsigned int time); 
  //--------------------------------------------------
  
         //! Scalers Accessor
	/*! 
            

	 \param timestamp (unix time)
	 \param usec       second fraction after in mksec
              
      NB return vector<Scalers_i> fScalersReturn  of Nearest Scalers iterators
           fScalers[0]->first < time < fScalers[1]->first
	
             
           \return Nearest Scalers iterators vector length                  
	 */

int getScalers(unsigned int time,unsigned int usec );
void Purge();
static    AMSSetupR * & gethead(){return _Head;}
 void CreateBranch(TTree *tree, int brs);
 bool UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime);
 bool FillHeader(uinteger run); //fillHeader at run start by database
 bool FillSlowcontrolDB( string & file);
 int LoadExt();
 void getSlowControlFilePath( string & file);
 void updateSlowControlFilePath( string & file);
 bool LoadSlowcontrolDB(const char *file);
 void UpdateHeader(AMSEventR* ev);
 void Reset();
 AMSSetupR();
 void LoadISS(unsigned int t1, unsigned int t2);
 int LoadISSAtt(unsigned int t1, unsigned int t2);
 int LoadISSSA(unsigned int t1, unsigned int t2);
 int LoadGPSWGS84(unsigned int t1, unsigned int t2);
 int LoadISSCTRS(unsigned int t1, unsigned int t2);
 int LoadISSGTOD(unsigned int t1, unsigned int t2);
 int LoadDynAlignment(unsigned int run);
 bool BuildRichConfig(unsigned int run);
 int LoadRichConfig(unsigned int run);
 //--------DSP Errors-----------------------
 //! DSP Errors loader from .csv files
 /*! 
   
   \param unsigned int t1 : start time to load
   \param unsigned int t2 : end time to load
   
   \retval -1 -->  KO (problems in retrieving the info from .csv files)
   \retval -2 -->  KO (passed time interval not valid [<=0])
   \retval +N -->  OK (number of periods affected by DSP problem in t1-t2 time intervals)
 */
 int LoadDSPErrors(unsigned int t1, unsigned int t2);
 //---------------------------------------
 void Init(TTree *tree);

 private:
 //--------DSP Errors-----------------------
 void RearrangeManyDSPErrors(vector<DSPError>& vec);
 void RearrangeTwoDSPErrors(DSPError dsperr, vector<DSPError>& vec, int index_vec);
 //---------------------------------------
 
 ClassDef(AMSSetupR,17)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
 };
#endif
