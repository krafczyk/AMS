// V. Choutko 15/6/97

#ifndef __AMSDAQEVT__
#define __AMSDAQEVT__
#include <id.h>
#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <time.h>
typedef integer (*pid)(int16u id);
typedef void  (*pputdata)(integer n, int16u* data);

typedef integer (*pgetmaxblocks)();
typedef integer (*pgetl)(integer i);
typedef void  (*pgetdata)(integer i, integer n, int16u* data);

class DAQSubDet  {
protected:
pid _pgetid;
pputdata _pputdata;
DAQSubDet * _next;
public:
DAQSubDet():_next(0),_pgetid(0),_pputdata(0){}
DAQSubDet(pid pgetid,  pputdata pput):
  _next(0),_pgetid(pgetid), _pputdata(pput){}

friend class DAQEvent;
};

class DAQBlockType  {
protected:
integer _maxbl;
integer *  _plength;
pgetmaxblocks _pgmb;
pgetdata _pgetdata;
pgetl   _pgetlength;
DAQBlockType * _next;
public:
DAQBlockType():_next(0),_pgetdata(0),
_pgetlength(0),_pgmb(0),_maxbl(0),_plength(0){}
DAQBlockType(pgetmaxblocks pgmb, pgetl pgl, pgetdata pget):
  _next(0),_pgetlength(pgl),_pgetdata(pget), _pgmb(pgmb),
_maxbl(0),_plength(0){}

friend class DAQEvent;
};


class DAQEvent : public AMSlink{
protected:
integer _Checked;
uinteger _Length;
uinteger _Event;
uinteger _Run;
uinteger _RunType;
time_t _Time;
int16u *  _pcur;
int16u * _pData;
static DAQSubDet * _pSD;
static DAQBlockType * _pBT;
static const integer _OffsetL;
integer _EventOK();
integer _HeaderOK();
void _convert();
void _convertl(int16u & l16);
integer _create();
void _copyEl();
void _writeEl(){}
void _printEl(ostream& o){}
public:
~DAQEvent();
DAQEvent(): AMSlink(),_Length(0),_Event(0),_Run(0),_pcur(0),_pData(0),_Checked(0),
_Time(0),_RunType(0){}
uinteger & eventno(){return _Event;}
uinteger & runno(){return _Run;}
time_t   & time(){return _Time;}
uinteger & runtype(){return _RunType;}
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
static void addsubdetector(pid pgetid, pputdata pput);
static void addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget);


static void setfiles(char *ifile, char *ofile);
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
