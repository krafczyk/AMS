#ifndef __AMSTimeID__
#define __AMSTimeID__
#include <time.h>
#include <node.h>
class AMSTimeID: public AMSNode {
protected:

time_t _Insert;    // insert time
time_t _Begin;     // validity starts
time_t _End;       //  validity ends
integer _CRC;       // Control Sum
integer _Nbytes;   // Number of bytes in _pData
void * _pData;      // pointer to data
integer _CalcCRC();
public:
AMSTimeID():AMSNode(),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),_CRC(0){}
AMSTimeID(char * id):
AMSNode(id),_Insert(0),_Begin(0),_End(0),_Nbytes(0),_pData(0),_CRC(0){}
AMSTimeID(char *id, tm & begin, tm &end, integer nbytes, 
          void *pdata);
integer GetNbytes() const { return _Nbytes;}
void * GetpData() const { return _pData;}
integer CopyOut(void *pdataNew);
integer CopyIn(void *pdataNew);
integer getCRC()const {return _CRC;}
void gettime(time_t & insert, time_t & begin, time_t & end) const;
void SetTime (time_t insert, time_t begin, time_t end) ;
void _init(){}
integer validate(time_t & Time);
};


#endif
