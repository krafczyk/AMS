// V. Choutko 15/6/97

#ifndef __AMSDAQEVT__
#define __AMSDAQEVT__
#include <id.h>
#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <time.h>
typedef AMSID (*pid)();
typedef void  (*pgetput)(integer n, uinteger* data);
typedef integer (*pgetl)();

class DAQSubDet  {
protected:
integer _sdid;
integer _length;
pid _pgetid;
pgetput _pgetdata;
pgetput _pputdata;
pgetl   _pgetlength;
DAQSubDet * _next;
public:
DAQSubDet():_next(0),_pgetdata(0),_pgetid(0),_pputdata(0),
_pgetlength(0),_sdid(-1),_length(0){}
DAQSubDet(pid pgetid, pgetput pget, pgetput pput, pgetl pgl):
  _next(0),_pgetlength(pgl),_pgetid(pgetid),_pgetdata(pget), _pputdata(pput),
_sdid(-1),_length(0){}

friend class DAQEvent;
};


class DAQEvent : public AMSlink{
protected:
integer _Length;
uinteger *  _pcur;
uinteger * _pData;
static DAQSubDet * _pFirst;
integer _CRCok();
uinteger _makeCRC();
integer  _findid(AMSID id);
void _convert();
void _convertl();
integer _create();
void _copyEl();
void _writeEl(){}
void _printEl(ostream& o){}
public:
~DAQEvent();
DAQEvent(): AMSlink(),_Length(0),_pcur(0),_pData(0){}
integer *  getsdid(AMSID id);
void buildDAQ();
void buildRawStructures();
void write();
integer read();
void shrink();
integer getlength() const {return _Length*sizeof(_pData[0]);}
void close(){ fbin.close();fbout.close();}
static char * ifnam;
static fstream fbin;
static char * ofnam;
static fstream fbout;

static void init(integer mode, integer format=0);
static void addsubdetector(pid pgetid, pgetput pget, pgetput pput,
                            pgetl pgl);

integer & eventno() {return *((integer*)(_pData+5));}
integer & runno() {return *((integer*)(_pData+4));}
integer & runtype() {return *((integer*)(_pData+6));}
time_t  & time() {return *((time_t*)(_pData+7));}
uinteger & crc() { return *(_pData+3);}

static void setfiles(char *ifile, char *ofile);
static void _InitTable();
static uinteger * _Table;
static const uinteger CRC32;
};

class DAQEventI{
public:
DAQEventI();
~DAQEventI();
private:
static integer _Count;
};
static DAQEventI DAQEventI;




#endif
