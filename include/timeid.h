#ifndef __AMSTimeID__
#define __AMSTimeID__
#include <time.h>
#include <node.h>
#include <astring.h>
#include <list>
struct dirent;
class AMSTimeID: public AMSNode {
private:
AMSTimeID(const AMSTimeID & o){};
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
IBE _ibe;
CType _Type;
mutable integer _UpdateMe;
time_t _Insert;    // insert time
time_t _Begin;     // validity starts
time_t _End;       //  validity ends
uinteger _CRC;       // Control Sum
integer _Nbytes;   // Number of bytes in _pData
uinteger * _pData;      // pointer to data
integer _DataBaseSize;
uinteger * _pDataBaseEntries[5];  // Run Insert Begin End SortedBeg
uinteger _CalcCRC();
void _init(){};
static void _InitTable();
static uinteger * _Table;
void _convert(uinteger *pdata, integer nw);
void _fillDB(const char * dir,int reenter);
void _fillDBaux();
integer _getDBRecord(time_t & time, int & index);
void _getDefaultEnd(uinteger time, time_t & endt);
void _checkcompatibility (const char* dir);
void _rewrite(const char * dir, AString & ffile);
char* _getsubdirname(time_t time);
static integer _select(
#ifndef __ALPHA__
const
#endif
dirent * entry=0);
static integer _selectsdir(
#ifndef __ALPHA__
const
#endif
dirent * entry=0);
static AString *_selectEntry;
public:
AMSTimeID():AMSNode(),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),
_CRC(0),_UpdateMe(0),_DataBaseSize(0),_Type(Standalone){for(int i=0;i<5;i++)_pDataBaseEntries[i]=0;}
AMSTimeID(AMSID  id,integer nbytes=0, void* pdata=0,CType server=Standalone):
AMSNode(id),_Insert(0),_Begin(0),_End(0),_Type(server),_Nbytes(nbytes),_pData((uinteger*)pdata),
_UpdateMe(0),_DataBaseSize(0){for(int i=0;i<5;i++)_pDataBaseEntries[i]=0;_CalcCRC();}
AMSTimeID( AMSID  id, tm  begin, tm end, integer nbytes,  void *pdata, CType server);
~AMSTimeID(){for(int i=0;i<5;i++)delete[] _pDataBaseEntries[i];}
integer GetNbytes() const { return _Nbytes;}
integer CopyOut (void *pdataNew) const;
integer CopyIn( const void *pdataNew);
uinteger getCRC()const {return _CRC;}
void UpdCRC();
IBE &  findsubtable(time_t begin, time_t end);
void checkupdate(const char * name);
integer & UpdateMe() {return _UpdateMe;}
static const uinteger CRC32;
void gettime(time_t & insert, time_t & begin, time_t & end) const;
void SetTime (time_t insert, time_t begin, time_t end) ;
integer validate(time_t & Time,integer reenter=0);
bool updatedb();
bool updatemap(const char *dir);
bool write(const char * dir, int sleep=1);
bool read(const char * dir,int run, time_t begin, int index=0);
integer readDB(const char * dir, time_t time,integer reenter=0);
#ifdef __CORBA__
void fillDB(int length, uinteger * ibe[5]);
#endif
#ifdef __DB__
void _fillfromDB();
integer readDB(integer reenter=0);
#endif
};

#endif
