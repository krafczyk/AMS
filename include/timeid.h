#ifndef __AMSTimeID__
#define __AMSTimeID__
#include <time.h>
#include <node.h>
#include <dirent.h>
#include <astring.h>
class AMSTimeID: public AMSNode {
protected:

integer _UpdateMe;
time_t _Insert;    // insert time
time_t _Begin;     // validity starts
time_t _End;       //  validity ends
uinteger _CRC;       // Control Sum
integer _Nbytes;   // Number of bytes in _pData
uinteger * _pData;      // pointer to data
integer _DataBaseSize;
uinteger * _pDataBaseEntries[4];  // Run Insert Begin End
uinteger _CalcCRC();
void _init(){};
static void _InitTable();
static uinteger * _Table;
void _convert(uinteger *pdata, integer nw);
void _fillDB(const char * dir);
void _fillfromDB();
integer _getDBRecord(uinteger time);
static integer _select(dirent * entry);
static AString *_selectEntry;
public:
AMSTimeID():AMSNode(),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),
_CRC(0),_UpdateMe(0),_DataBaseSize(0){for(int i=0;i<4;i++)_pDataBaseEntries[i]=0;}
AMSTimeID(AMSID  id, integer nbytes=0, void* pdata=0):
AMSNode(id),_Insert(0),_Begin(0),_End(0),_Nbytes(nbytes),_pData((uinteger*)pdata),
_UpdateMe(0),_DataBaseSize(0){for(int i=0;i<4;i++)_pDataBaseEntries[i]=0;_CalcCRC();}
AMSTimeID( AMSID  id, tm  begin, tm end, integer nbytes,  void *pdata);
~AMSTimeID(){for(int i=0;i<4;i++)delete[] _pDataBaseEntries[i];}
integer GetNbytes() const { return _Nbytes;}
integer CopyOut(void *pdataNew);
integer CopyIn(void *pdataNew);
uinteger getCRC()const {return _CRC;}
void UpdCRC();
integer & UpdateMe() {return _UpdateMe;}
static const uinteger CRC32;
void gettime(time_t & insert, time_t & begin, time_t & end) const;
void SetTime (time_t insert, time_t begin, time_t end) ;
integer validate(time_t & Time,integer reenter=0);
integer write(char * dir );
integer read(char * dir, integer reenter=0);
integer readDB(integer reenter=0);
};

#endif
