// V. Choutko 15/6/97
//
// A.Klimentov June 21, 1997. eventlength(),    ! add functions
//                            sdetlength(id)    !
//                            data(buff)        !
//                            sdet(id)          !
//                            dump(id)          !
//
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

//+
integer eventlength() {return _Length;}
void    data(uinteger *buff) {memcpy(buff,_pData,eventlength()*2);}
uint16  sdetlength(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset - 1];
   if (id == sdetid) return l;
   offset = offset  + l + 1;
   if (offset > eventlength()) break;
  }
  return -1;
}  
integer sdet(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset-1];
   if (id == sdetid) return offset;
   offset = offset  + l + 1;
   if (offset > eventlength()) break;
  }
  return -1;
}  
void dump(uint16 sdetid) {
// dump event
// if sdetid == -1 dump whole event
//
  cout<<"run, event, length "<<runno()<<", "<<eventno()<<", "<<eventlength()
      <<endl;
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16  id;
  integer l;
  for (;;) {
   id = _pData[offset];
   l  = _pData[offset - 1];
   if (id == sdetid || sdetid == 0) {
    printf("sub.detector id... %#x, length... %d\n ",id,l);
    if (l > 0) {
     for (int i = 1; i < l-1; i++) {
      cout<<*((uint16*)(_pData + offset + i))<<" ";
     }
     cout<<endl;
    } else {
      cout<<"Error : invalid length. Quit"<<endl;
      return;
    }
    offset = offset  + l + 1;
    if (offset > eventlength()) break;
   }
  }
}  
//-  
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
