//  $Id$
#ifndef __ROOTSETUP__
#define __ROOTSETUP__

#include <typedefs.h>
#include <map>
#include <math.h>
#include <vector>
#include "TObject.h"
#include "TString.h"
#include "TGraph.h"
#include "TTree.h"
#include "TMatrixD.h"
#include "trigger102_setup.h"
#include "RichConfig.h"
#include "DynAlignment.h"

class SlowControlDB;
class RichConfigContainer;
class AMSEventR;
class AMSTimeID;
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
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
virtual ~Element() { }
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
virtual ~SlowControlR() {}

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
TDVR():Begin(0),End(0),Insert(0),Size(0),CRC(0),Data(),DataMC(0),Name(""),FilePath(""){}
TDVR(const TDVR &a):Begin(a.Begin),End(a.End),Insert(a.Insert),Size(a.Size),CRC(a.CRC),Data(a.Data),DataMC(a.DataMC),Name(a.Name),FilePath(a.FilePath){}
  friend ostream &operator << (ostream &o, const TDVR &b );
virtual ~TDVR() {}
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
virtual ~GPS() { }
ClassDef(GPS,3)
};

//! AMS Exposure Time information for every second(JMDC Time[0])
/*!
\sa AMSSetupR
\author vitali.choutko@cern.ch qyan@cern.ch
*/
class RTI{
protected:
	static TGraph *SAAedge;	///< outer boundaries of the SAA region
        static AMSSetupR *setuphead;///< AMSSetup
public:

	static TGraph *GetSAAedge(); ///< Construct the boundaries of the SAA region
	//--------------------------------------------------------------------------------------------------
	///
	//!   Says if AMS was inside SAA boundaries
	/*!
	 returns: true if inside SAA, false if outside.
	 */
	bool IsInSAA();				///< True if ISS in in SAA, false otherwize
        static AMSSetupR *getsetup();///< AMSSetup
         ///  BadRuns accessor
         /*!
           returns: 0 GoodRun, 1 BadRun, 2 UnableToLoadBadRunList         
         */
        int IsBadRun(string &reason);
         
unsigned int run;  ///< run
unsigned int evno;///< first event no in one second
unsigned int evnol;///< last event no in one second
float lf;  ///< life time
float cf[4][2];  ///< max cutoff for 25,30,35,40 degrees (gv) for neg+pos
float cfi[4][2]; ///< max IGRF cutoff for 25,30,35,40 degrees (gv) for neg+pos
float mphe;   ///< most probable He rigidity;
float theta;  ///< theta gtod (rad)
float phi;    ///< phi gtod (rad)
float r;      ///< rad gtod (cm)
float zenith; ///< ams zenith angle (degrees)
float glat;   ///< ams pointing galatic latitude (degrees) -1 faild
float glong;  ///< ams pointing galatic longitude (degrees) -1 faild
float nev;    ///< exist events  nev+nerr=sumev
float nerr;   ///< absent events
float ntrig;  ///< events with trigger;
float nhwerr; ///< events with has HW error(JINJStatus)
float npart;  ///< events with tof+trd+tracker+ecal
float nl1l9[2][2]; ///<events with track L1 L9 XY hit 
float dl1l9[2][3]; ///< mean difference(um) bewteen PG ad CIEMAT alignment of L1 and L9(XYZ)
float mtrdh; ///< average trdrawhit number for one event  
int good;    ///< 0 if good
/*!<
  bit0: duplicate events                          \n
  bit1: event number flip                         \n
  bit2: event missing at the beginging of second  \n
  bit3: event missing at the end of second        \n
  bit4: second at the begining of run             \n
  bit5: second at the end of run                  \n
*/
unsigned int utime;  ///< JMDC unix time(second)
unsigned int usec[2];///< JMDC unix time microsecond(us) for first and last event
double utctime[2];   ///< UTC time for first and last event
/// get unix time
/*!
 * @param[in] itm  0: JMDC  time,  1: UTC  time,  2: JMDC time-UTC time
 * @param[in] iev  0: first event, 1: last event
 */
double gettime(int itm=0, int iev=0); 
float  getthetam(); ///< PhiM (rad)
float  getphim();   ///< ThetaM(rad)
int    getissatt(float &roll,float &pitch,float &yaw);///<roll, pitch, yaw(rad)
int    getisstle(float &velocitys,float &veltheta,float &velphi);///< velocitys(rad/sec),velphi(rad),veltheta(rad)
double getbetasun();///< solar beta angle(degree)
static int Version; ///< RTI Version id
/*!< 
  0:  2013-01 B620 default \n
  1:  2013-08 B620         \n
  2:  2013-12 B700         \n
  3:  2014-03 B620 latest  \n 
  503:2014-03 B800(p5)  \n
  513:2014-12 B800(p5) (TTCS study) \n
  604:2015-03 B950(p6) latest \n
*/
static int Loadopt;//< load option
/// use latest RTI Version
/*!
 * @param[in] pass  4: B620-p4, 6: B950-p6
 * @return Version id
*/
static int UseLatest(int pass=4);
//---
 RTI():run(0),evno(0),evnol(0),lf(0),mphe(0),theta(0),phi(0),r(0),zenith(0),glat(-2),glong(-2),nev(0),nerr(0),ntrig(0),nhwerr(0),npart(0),mtrdh(0),good(-1),utime(0){
        for(int ifv=0;ifv<4;ifv++){
          for(int ipn=0;ipn<2;ipn++){cf[ifv][ipn]=0;cfi[ifv][ipn]=0;}
        }
        for(int iexl=0;iexl<2;iexl++){
          for(int ico=0;ico<3;ico++){if(ico<2)nl1l9[iexl][ico]=0;dl1l9[iexl][ico]=0;}
        }
        usec[0]=usec[1]=0;
        utctime[0]=utctime[1]=0;
   }
 virtual ~RTI() { }
ClassDef(RTI,10)
};


//! AMS RTI Process RootFile Run information
/*!
\sa AMSSetupR
\author qyan@cern.ch
*/

class RunI{
 public: 
  unsigned int run;///< run
  unsigned int bt; ///< begin time of this run, JMDC Time
  unsigned int et; ///< end   time of this run, JMDC Time
  unsigned int begev; ///< begin event id of this run
  unsigned int endev; ///< end   event id of this run
  vector<string>fname;///< root file name
  virtual ~RunI() {}
  RunI():run(0),bt(0),et(0),begev(0),endev(0){}
  RunI(unsigned int _run,unsigned int _bt, unsigned int _et,unsigned int _begev, unsigned int _endev, string _fn):run(_run),bt(_bt),et(_et),begev(_begev),endev(_endev){
    fname.push_back(_fn);
  }
  ClassDef(RunI,2)
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
  virtual ~GPSWGS84() {}

ClassDef(GPSWGS84,1)
};

class GPSWGS84R{
public:
  float r; ///< r (cm) in CTRS
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
  virtual ~GPSWGS84R() {}
ClassDef(GPSWGS84R,1)
};

class JGCR{
//  JMDC- GPS Correction
public:
virtual ~JGCR() { }
unsigned int Validity[2];  ///< validity range unix time
double A[2]; ///< A[0]+A[1]*x correction
double Par[3]; ///<Par[0]*sin(Par[1]*x+Par[2]) add correction
double Err[2]; ///< residial error estimation (rms, gaussian (sec)
ClassDef(JGCR,1)
};

class JGC{
//  JMDC- GPS Correction
// seprecated
public:
unsigned int Validity[2];  ///< validity range unix time
double A[2]; ///< A[0]+A[1]*x correction
double Par[3]; ///<Par[0]*sin(Par[1]*x+Par[2]) add correction
double Err[2]; ///< residial error estimation (rms, gaussian (sec)
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
virtual ~ISSAtt() {}
ClassDef(ISSAtt,1)       //ISS Attitude Data
};

class ISSSA{
public:
float alpha; ///< Solar Array alpha angle (degrees) alpha:0 means 3a and  1b is in ams field of view ;see also https://twiki.cern.ch/twiki/bin/view/AMS/ISSAuxData
float b1a; ///< beta 1a  degrees
float b3a; ///< beta 2a  --
float b1b; ///< beta 1b  --
float b3b; ///< beta 3b  --
virtual ~ISSSA() {}
ClassDef(ISSSA,1)       //ISS Solar Arrays Data
};

class AMSSTK{
public:
float ams_ra;    ///< ams right ascension (degrees)
float ams_dec;    ///< ams declination (degrees)
int cam_id;       ///< stk camera is 0,1 
float cam_ra;     ///< stk camera right ascension degrees
float cam_dec;    ///< stk declination degrees       
float cam_or;     ///<  stk orientatoin
AMSSTK():ams_ra(0),ams_dec(0),cam_id(-1),cam_ra(0),cam_dec(0),cam_or(0){};
virtual ~AMSSTK() {}
ClassDef(AMSSTK,1)     // AMS Star Tracker Data
};


class ISSCTRS{
public:
float x; ///< x (km) in CTRS
float y; ///< y
float z; ///< z
float vx; ///< v_x km/s
float vy; ///< v_y km/s
float vz; ///< v_z km/s
virtual ~ISSCTRS() {}
ClassDef(ISSCTRS,1)       //ISS CTRS coordinates data
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
virtual ~ISSCTRSR() {}
ClassDef(ISSCTRSR,1)       //ISS Solar Arrays Data
};

class ISSINTL{
public:
float Roll;  ///< Roll  in INTL, Rad
float Pitch; ///< Pitch in INTL, Rad
float Yaw;   ///< Yaw   in INTL, Rad
ISSINTL():Roll(0),Pitch(0),Yaw(0){}
virtual ~ISSINTL() {}
ClassDef(ISSINTL,1)       //ISS Attitude data (INTL coordinate system)
};

class BadRun{
public:
 string reason;   // reason why it is bad
 unsigned int begin; //First Second (unix time)is bad 
 unsigned int end;   // Last second (unix time) is bad
 unsigned int run;   // Run number
BadRun(const char* str, unsigned int r, unsigned int b=0,unsigned int e=4294967295L):reason(str),begin(b),end(e),run(r){};
BadRun():reason(""),begin(0),end(0),run(0){};
virtual ~BadRun() {}
ClassDef(BadRun,1)       //BadRun
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
virtual ~ISSGTOD() {}
ClassDef(ISSGTOD,1)       //ISS GTOD
};

//--------------DSP Errors-------------------------
/*!
  Object containing the information about DSP errors.\n
  AMSSetupR::DSPError objects are created by AMSSetupR::LoadDSPErrors() starting from .csv files (stored in $AMSDataDir/altec/DSP/). \n
  The .csv files are created by a standalone program that scans the HKBPB stream. \n
  
  \sa http://ams.cern.ch/AMS/DAQ/eCos/jap/node_addresses.xls \n
  
  \note
  The default path could be customized defining the AMSDSP environment variable: this will overhide $AMSDataDir/altec/DSP/. \n
  
  \author matteo.duranti@cern.ch
*/
 class DSPError{
 public:
   DSPError(){
     TimeStart=0;TimeEnd=0;
     Nodes_000_01F=0;Nodes_020_03F=0;Nodes_040_05F=0;Nodes_060_07F=0;
     Nodes_080_09F=0;Nodes_0A0_0BF=0;Nodes_0C0_0DF=0;Nodes_0E0_0FF=0;
     Nodes_100_11F=0;Nodes_120_13F=0;Nodes_140_15F=0;Nodes_160_17F=0;
     Nodes_180_19F=0;Nodes_1A0_1BF=0;Nodes_1C0_1DF=0;Nodes_1E0_1FF=0;
   };///< Standard constructor
   virtual ~DSPError() {}
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
   bool SetNA(unsigned int NA); ///< Set the NA affected by DSP error (erase any previous set nodes!). \retval true in case of positive inception.
   bool AddNA(unsigned int NA); ///< Add a NA affected by DSP error (keep previous nodes!). \retval true in case of positive inception.
   bool AddNA(DSPError dsperr); ///< Add a NA affected by DSP error (keep previous nodes!) taking by a DSPError object (neglecting its timestart,timend). \retval true in case of positive inception.
   int GetFirstNA(); ///< Gives NA affected by DSP error (in case of multiple nodes return the first [the one with lower NA] with negative sign)
   bool SearchNA(unsigned int NA); ///< Search if the passed NA is affected by the problem. \retval true if that NA is affected
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
  virtual ~Header() {}
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
 typedef map<unsigned long long, unsigned int> htable_m;
 typedef map<unsigned long long, unsigned int>::iterator htable_i;
 typedef map <unsigned int,TDVR> TDVR_m;
 typedef map <unsigned int,TDVR>::iterator TDVR_i;
 typedef map <unsigned int,TDVR>::reverse_iterator TDVR_ri;
 typedef map <string,TDVR_m> TDVRC_m;
 typedef map <string,TDVR_m>::iterator TDVRC_i;
 htable_m fEntries;
 TDVRC_m fTDVRC;
 vector<TDVR> fTDV_Time; ///<Return of getAllTDV thanks to rootcint bug;
 vector<TDVR> fTDV_Name; ///<Return of getAllTDV thanks to rootcint bug;
 void printAllTDV_Time(){for( unsigned int i=0;i<fTDV_Time.size();i++){cout <<fTDV_Time[i]<<endl;}}
 void printAllTDV_Name(){for (unsigned int i=0;i<fTDV_Name.size();i++){cout <<fTDV_Name[i]<<endl;}} 
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
 typedef map <double,ISSINTL> ISSINTL_m;
 typedef map <double,AMSSTK> AMSSTK_m;
 typedef map <unsigned int,ISSGTOD> ISSGTOD_m;
 typedef map <double,ISSAtt> ISSAtt_m;
 typedef multimap <unsigned int,BadRun> BadRun_m;
 typedef multimap <unsigned int,BadRun>::iterator BadRun_i;
  
 //---------------DSP Errors-------------------------
 typedef map <unsigned int, DSPError> DSPError_m;
 //--------------------------------------------------
 typedef map <unsigned int,ISSData>::iterator ISSData_i;
 typedef map <unsigned int,ISSSA>::iterator ISSSA_i;
 typedef map <unsigned int,ISSCTRS>::iterator ISSCTRS_i;
 typedef map <unsigned int,ISSGTOD>::iterator ISSGTOD_i;
 typedef map <double,ISSINTL>::iterator ISSINTL_i;
 typedef map <double,ISSAtt>::iterator ISSAtt_i;
 typedef map <unsigned int, RTI>::iterator RTI_i;
 typedef map <double,AMSSTK>::iterator AMSSTK_i;
 //---------------DSP Errors-------------------------
 typedef map <unsigned int, DSPError>::iterator DSPError_i;
 //--------------------------------------------------
 typedef map <unsigned int,RTI> RTI_m;

 typedef map <unsigned int,GPSWGS84> GPSWGS84_m;
 typedef map <unsigned int,GPSWGS84>::iterator GPSWGS84_i;
 vector<JGC> fJGC;
 vector<JGCR> fJGCR;
 BadRun_m fBadRun; ///< BadRuns 
 GPS_m fGPS;    ///< GPS Epoch Time
 RTI_m fRTI;  ///<  runtimeInfo data
 GPSWGS84_m fGPSWGS84;  ///<  GPS Coo Data        
 ISSData_m fISSData;    ///< ISS Aux Data map
 ISSAtt_m fISSAtt;      ///< ISS Attitude angles map
 //---------------DSP Errors-------------------------
  DSPError_m fDSPError; ///< DSP Error map (key is the start time of NOT validity period due to DSP Errors). Map 'second' contains DSPError object with start/end NOT validity and affected nodes
 //--------------------------------------------------
  AMSSTK_m fAMSSTK;      ///< AMS STK pointing
  ISSSA_m fISSSA;      ///< ISS Solar Array angles map
  ISSINTL_m fISSINTL;      ///< ISS INTL attitude angles map
  ISSCTRS_m fISSCTRS;      ///< ISS CTRS coordinates & velocity vector map
  ISSGTOD_m fISSGTOD;      ///< ISS GTOD coordinates & velocity vector map
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
void Add(SlowControlDB *s, int verb=1);
friend class AMSTimeID;
friend class SlowControlDB;
static AMSSetupR * _Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (_Head)
#endif
#ifdef __DARWIN__
#if __OSXVER__ >= 1080
static int _select ( const dirent * entry);
#else
static int _select ( dirent * entry);
#endif
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
static int _select (const dirent64 * entry);
#endif
 public:
 bool LoadISSBadRun(); ///< Load badruns from $AMSDataDir/Badruns
 
 //! BadRuns accessor
 /*! 
   
   \param unsigned int utime=0 (unix time)
   \param unsigned int run=0  run to be checked; if 0 run from the setup header will be taken
   \param string  reason input ;  input=="" means any reason ; on output return all reasons matched        
   
   \retval 0   GoodRun
   \retval 1   BadRun
   \retval 2   UnableToLoadBadRunList                  
 */
 int IsBadRun( string &reason,unsigned int utime=0,unsigned int run=0);
 
 
 //! ISS Att roll,pitch,yaw accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param roll,pitch,yaw interpolated values      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 300 sec)
   
   \note
   The default path could be customized defining the AMSISS environment variable: this will overhide $AMSDataDir/altec/
 */
 int getISSAtt( float& roll,float&pitch,float&yaw, double xtime); 
 
 
 //! ISS Att roll,pitch,yaw (INTL coordinate system) accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param roll,pitch,yaw interpolated values      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 60 sec)
   
   \note
   The default path could be customized defining the AMSISS environment variable: this will overhide $AMSDataDir/altec/
 */
 int getISSINTL(float &roll, float &pitch, float &yaw, double xtime); 
 
 
 //! ISS Coo & Velocity CTRS accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param ISSSCTRSR a  interpolated values      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 60 sec)
   
   \note
   The default path could be customized defining the AMSISSSA environment variable: this will overhide $AMSDataDir/isssa/
 */
 int getISSCTRS(ISSCTRSR & a, double xtime); 


 //! Get Rotation Matrix from CTRS to inertial (ICRS)   
 /*! 
   
   \param double xtime (unix time + fraction of second)
   
   \retval Rotation Matrix from CTRS to inertial (ICRS)   

 */
 TMatrixD   getRotationMatrix(double xtime);

 
 //! AMS STK Orientation Accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param  AMSSTK  interpolated values      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   different cam ids for interpolation
   \retval 4   bad extrapolation ( gap > 60 sec)
   
   
  
 */
 int getAMSSTK(AMSSTK & a, double xtime); 



 //! RTI accessor
 /*! 
   
   \param unsigned int time (JMDC Time[0])
   \param RTI  value
   \retval 0   ok 
   \retval 1   no for this second
   \retval 2   no data
   

   \note
   The default path could be customized defining the AMSISS environment variable: this will overhide $AMSDataDir/altec/
 */
 int getRTI(RTI & a, unsigned int time); 

 
 //! ISS Coo & Velocity GPS accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param GPSWGS84R a  interpolated values        
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 600 sec)
   

   \note
   The default path could be customized defining the AMSISS environment variable: this will overhide $AMSDataDir/altec/
 */
 int getGPSWGS84(GPSWGS84R & a, double xtime); 


 
 //! JMDC GPS ad hoc correcton
 /*! 
   
   \param unsigned int time (JMDC time[0])
   \param double corr time correction (sec)        
   \param double error estimated error (sec)      
   \retval 0   ok 
   \retval 1   reserved
   \retval 2   no data
   \retval 3   outside of limits
   

 */
 int GetJMDCGPSCorr(double& corr,double& err,unsigned int time); 
 int LoadJMDCGPSCorr();  ///< Load Correction Vector into memory; 


 
 
 //! ISS Coo & Velocity GTOD accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param ISSSGTOD a  interpolated values      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 60 sec)
   
   \note
   The default path could be customized defining the AMSISS environment variable: this will overhide $AMSDataDir/altec/
 */
 int getISSGTOD(ISSGTOD & a, double xtime); 
 
 
 //! ISS TLE Prediction Accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param RTP[3] Rad (cm), Theta(rad), Phi (rad) (GTOD)
   \param VelTP[3] Velocity [rad/sec], Velocity_Theta(Rad), Velocity_Phi(Rad);
   
   \retval 0   ok 
   \retval 1   problem in TLE prediction
   \retval 2   TLE elements buffer empty
   \retval 3   No TLE element found for xtime
   
   \note
   The default path could be customized defining the AMSISSSA environment variable: this will overhide $AMSDataDir/isssa/
 */
 int getISSTLE(float RTP[3],float VelTP[3], double xtime); 
 
 //! ISS Solar Arrays Position accessor
 /*! 
   
   \param double xtime (unix time + fraction of second)
   \param ISSSSA a  interpolated values for the angles      
   
   \retval 0   ok (interpolation)
   \retval 1   ok  (extrapolation)
   \retval 2   no data
   \retval 3   bad extrapolation ( gap > 60 sec)
   
   \note
   The default path could be customized defining the AMSISSSA environment variable: this will overhide $AMSDataDir/isssa/
 */
 int getISSSA(ISSSA & a, double xtime); 
 
 
 //---------------DSP Errors-------------------------
 //! DSP Errors accessor
 /*! 
   
   \param unsigned int time : unix time
   \param DSPError dsperror : object containing (if any DSP error affecting 'time') time interval of NOT validity and nodes affected
   
   \retval  0 -->  OK (NO nodes affected by DSP errors)
   \retval  1 -->  KO (SOME nodes affected by DSP errors)
   
   \note
   The default path could be customized defining the AMSDSP environment variable: this will overhide $AMSDataDir/altec/DSP/.
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
 static    void sethead(AMSSetupR *setup) { _Head = setup; }
 void CreateBranch(TTree *tree, int brs);
 bool UpdateVersion(uinteger run,uinteger os,uinteger buildno,uinteger buildtime);
 bool FillHeader(uinteger run); //fillHeader at run start by database
 bool FillSlowcontrolDB( string & file);
 int LoadExt();
 time_t getSlowControlFilePath( string & file);
 void updateSlowControlFilePath( string & file);
 bool LoadSlowcontrolDB(const char *file, int verb=0);
 int UpdateHeader(AMSEventR* ev);
 void Reset();
 void ResetMC();
 int GetEntry(unsigned int run,unsigned int event); ///< GetEntry for run,event; return -1 if not found
 AMSSetupR();
 virtual ~AMSSetupR() {}
 static bool LoadISSMC;///< Load/Not LOAd ISS info for the MC runs
 void LoadISS(unsigned int t1, unsigned int t2);
 int LoadISSAtt(unsigned int t1, unsigned int t2);
 int LoadISSSA(unsigned int t1, unsigned int t2);
 int LoadRTI(unsigned int t1, unsigned int t2, const char *dir=0);
 int LoadGPSWGS84(unsigned int t1, unsigned int t2);
 int LoadISSINTL(unsigned int t1, unsigned int t2);
 int LoadISSCTRS(unsigned int t1, unsigned int t2);
 int LoadRotationMatrices(unsigned int t1, unsigned int t2);
 int LoadAMSSTK(unsigned int t1, unsigned int t2);
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


ClassDef(AMSSetupR,26)       //AMSSetupR
#pragma omp threadprivate(fgIsA)
};
#endif

