#ifndef __AMSTimeID__
#define __AMSTimeID__
#include <time.h>
#include <node.h>
class AMSTimeID: public AMSNode {
protected:

integer _UpdateMe;
time_t _Insert;    // insert time
time_t _Begin;     // validity starts
time_t _End;       //  validity ends
uinteger _CRC;       // Control Sum
integer _Nbytes;   // Number of bytes in _pData
void * _pData;      // pointer to data
uinteger _CalcCRC();
void _init(){};
static void _InitTable();
public:
AMSTimeID():AMSNode(),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),
_CRC(0),_UpdateMe(0){}
AMSTimeID(AMSID  id, integer nbytes=0, void* pdata=0):
AMSNode(id),_Insert(0),_Begin(0),_End(0),_Nbytes(nbytes),_pData(pdata),
_UpdateMe(0){_CalcCRC();}
AMSTimeID( AMSID  id, tm  begin, tm end, integer nbytes,  void *pdata);
integer GetNbytes() const { return _Nbytes;}
void * GetpData() const { return _pData;}
integer CopyOut(void *pdataNew);
integer CopyIn(void *pdataNew);
uinteger getCRC()const {return _CRC;}
integer UpdateMe()const {return _UpdateMe;}
static uinteger * Table;
static const uinteger CRC32;
void gettime(time_t & insert, time_t & begin, time_t & end) const;
void SetTime (time_t insert, time_t begin, time_t end) ;
integer validate(time_t & Time);
};


#endif
