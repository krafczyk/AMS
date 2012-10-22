//  $Id: timeid.h,v 1.51 2012/10/22 08:07:01 choutko Exp $
#ifndef __AMSTimeID__
#define __AMSTimeID__

#include <time.h>
#include "node.h"
#include "astring.h"
#include <list>
#include <map>

using namespace std;
#ifdef __DARWIN__
struct dirent;
#else
struct dirent64;
#endif
/*! \class AMSTimeID
 \brief Invented to manipulate time or "run" dependent variables : calibration const,
 pedestals etc etc etc.


 Initialization

 For each set of such variables (e.g. array or class instance) 
 the following instance of timeid class should be set up in 
 
 AMSJob::_timeinitjob()
 
 TID.add( new AMSTimeID(AMSID id, tm begin, tm end, int nbytes, void* pdata));
 
 where id is unique identifier (char * part is arbitary string with length < 256,
 integer part is equal to AMSJob::getjobtype()  )
 
 tm begin - start validity time
 
 tm end   - end   validity time
 
 nbytes   - sizeof data in bytes
 
 pdata     - pointer to first element of the data 
             data should consist of 32 bit words ( so no double please) 
             to be portable between bend/lend machines

 tm structure is defined in sys <time.h>

relevant elements:

structure tm{

...

int tm_sec;    // seconds 0-59

int tm_min;    // minutes 0-59

int tm_hour   //  hours   0-23

int tm_mday   //  day in month 1-31

int tm_mon    //  month   0-11

int tm_year   //  year from 1900 (96 for 1996)

...

}



The following methods exist to exchange dbase<->timeid
------------

access :

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();       

AMSTimeID * offspring=(AMSTimeID*)ptid->down();  // get first timeid instance

while(offspring){

.........

    offspring=(AMSTimeID*)offspring->next();   // get one by one

}


public methods:

integer UpdateMe()      // update(!=0) or not update dbase

integer GetNbytes()     // get number of bytes

integer CopyOut(void *pdataNew) // copy timeid-> pdatanew

integer CopyIn(void *pdataNew)  // copy pdatanew->timeid

void gettime(time_t & insert, time_t & begin, time_t & end) // get times

void SetTime (time_t insert, time_t begin, time_t end) // set times

methods from AMSNode & AMSID class (getname, getid etc)




DataBase Engine
-----------------

Provides search the relevant record by event time key, i.e.

Max(Insert){Begin < Time < End}



Implementation I 
-----------------

Every record correspond the file with name  getname().{0,1}.uid

     0 - MC

     1 - Data

     uid- unique integer identifier
   
File content:

uinteger array[GetNbytes] | CRC | InsertTime | BeginTime | EndTime

CRC is calculated by _CalcCRC() function

*/


typedef void  (*trigfun_type)(void);

class AMSTimeID: public AMSNode {

private:
  AMSTimeID(const AMSTimeID & o){};

  trigfun_type _trigfun;

public:
  struct iibe{
    uinteger id;
    time_t    insert;
    time_t begin;
    time_t end;
  };
  typedef list<iibe> IBE;
  typedef list<iibe>::iterator IBEI;
  enum CType{Standalone,Client,Server};

protected:
  static uinteger * _Table;
  static AString *  _selectEntry;
   typedef map <string,AMSTimeID> timeid_m;
    typedef map <string,AMSTimeID>::iterator timeid_i;
    static timeid_m timeid_f;
    int _updateable;
    string map_dir; //  Directory for store the map:  changed to separate one for the convineince of multiple users access/ read-only fs etc
  IBE _ibe;
  time_t _Insert;    //! insert time
  time_t _Begin;     //! validity starts
  time_t _End;       //!  validity ends
  integer _Nbytes;   //! Number of bytes in _pData
  uinteger _CRC;     //! Control Sum
  mutable integer _UpdateMe;
  bool _verify;
  AString _fname;
  uinteger * _pData; //! pointer to data
  integer _DataBaseSize;
  CType _Type;
  uinteger * _pDataBaseEntries[5];  //! Insert Insert Begin End SortedBeg
#ifdef _WEBACCESS_
  static int _WebAccess; //! enable the http, read-only web access.
  static char _WebDir[200];
#endif
  void      _init(){};
  uinteger  _CalcCRC();
  void      _convert(uinteger *pdata, integer nw);
  time_t    _stat_adv(const char * dir);
  void      _fillDB(const char * dir,int reenter, bool force=false);
  void      _fillDBServer(const char * dir,int reenter, bool force=false);
  void      _fillDBaux();
  integer   _getDBRecord(time_t & time, int & index);
  void      _getDefaultEnd(uinteger time, time_t & endt);
  void      _checkcompatibility (const char* dir);
  void      _rewrite(const char * dir, AString & ffile);
  char*     _getsubdirname(time_t time);




  static void   _InitTable();

#ifdef __DARWIN__
  static integer _select(   dirent * entry=0);
  static integer _selectsdir(  dirent * entry=0);
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
  static integer _select(  const dirent64 * entry=0);
  static integer _selectsdir(  const dirent64 * entry=0);
#endif



public:
  static const uinteger CRC32;
  void      setmapdir(const char *dir=0);
  bool      setupdateablemapdir(const char*dir);


  AMSTimeID():AMSNode(),_trigfun(0),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),
	      _CRC(0),_UpdateMe(0),_verify(true),_DataBaseSize(0),_Type(Standalone),_updateable(-1)
  {for(int i=0;i<5;i++)_pDataBaseEntries[i]=0;_fname="";setmapdir();}
  
  AMSTimeID(AMSID  id,integer nbytes=0, void* pdata=0,bool verify=true,CType server=Standalone,trigfun_type fun=0):
    AMSNode(id),_Insert(0),_Begin(0),_End(0),_Nbytes(nbytes),_pData((uinteger*)pdata),_UpdateMe(0),_verify(verify),
    _DataBaseSize(0),_Type(server),_updateable(-1)
  {for(int i=0;i<5;i++)_pDataBaseEntries[i]=0;_CalcCRC();_trigfun=fun;_fname="";setmapdir();}
  
  AMSTimeID( AMSID  id, tm  begin, tm end, integer nbytes,  void *pdata, CType server, bool verify=true,trigfun_type fun=0);
  
  ~AMSTimeID(){for(int i=0;i<5;i++)delete[] _pDataBaseEntries[i];_trigfun=0;}
  /// Returns the number of records in the database 
  integer  GetDataBaseSize(){return _DataBaseSize;}
  /// Returns the begin validity and the times of the record ii-th (insert, begin ,end)
  uinteger GetDBRecordTimes(int ii,uinteger* times)
  {if (ii>=0 && ii<_DataBaseSize){ 
      times[0]=_pDataBaseEntries[0][ii]; //insert 
      times[1]=_pDataBaseEntries[1][ii]; //insert 
      times[2]=_pDataBaseEntries[2][ii]; //insert 
      times[3]=_pDataBaseEntries[3][ii]; // begin
      times[4]=_pDataBaseEntries[4][ii]; //end
      return _pDataBaseEntries[2][ii];   //begin
    }
    else return 0;
  }
  
integer  GetNbytes() const { return _Nbytes;}
  integer  CopyOut (void *pdataNew) const;
  integer  CopyIn( const void *pdataNew);
  uinteger getCRC()const {return _CRC;}
  void     UpdCRC();
  const char * getfile(){return (const char*)_fname;}
  void setfile(const char * a){_fname=a;}
  bool &   Verify() {return _verify;}
  IBE &    findsubtable(time_t begin, time_t end);
  void     checkupdate(const char * name);
  integer& UpdateMe() {return _UpdateMe;}
  void     gettime(time_t & insert, time_t & begin, time_t & end) const;
  void     SetTime (time_t insert, time_t begin, time_t end) ;
  integer  validate(time_t & Time,integer reenter=0);
  bool     updatedb();
  void     rereaddb(bool force=false);
  bool     updatemap(const char *dir,bool slp=false);
  bool     write(const char * dir, int sleep=1);
  bool     read(const char * dir,int run, time_t begin, int index=0);
  integer  readDB(const char * dir, time_t time,integer reenter=0);
  void     fillDB(int length, uinteger * ibe[5]);
#ifdef _WEBACCESS_
  static void SetWebAccess() {_WebAccess=1;}
  static void UnSetWebAccess() {_WebAccess=0;}
  static void SetWebDir(const char* pp){strncpy(_WebDir,pp,200);}
#endif

#ifdef __CORBA__
  friend class AMSProducer;
#endif
friend class AMSSetupR;
#ifdef __DB__
  void     _fillfromDB();
  integer   readDB(integer reenter=0);
#endif

};


#endif
