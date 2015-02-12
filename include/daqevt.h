//  $Id$
// V. Choutko 15/6/97
//
// A.Klimentov June 21, 1997.                   ! add functions
//                            sdetlength(id)    !
//                            data(buff)        !
//                            sdet(id)          !
//                            dump(id)          !
//
#ifndef __AMSDAQEVT__
#define __AMSDAQEVT__
#include "id.h"
#include "typedefs.h"
#include "link.h"
#include <fstream>
#include <time.h>
#include <dirent.h>
#include "zlib.h"
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
  DAQSubDet():_pgetid(0),_pputdata(0),_next(0){}
  DAQSubDet(pid pgetid,  pputdata pput):
    _pgetid(pgetid), _pputdata(pput),_next(0){}

  friend class DAQEvent;
};

class DAQCompress {
protected:
  static unsigned int   huffman_code[256];
  static unsigned short huffman_first_code[16];
  static unsigned short huffman_decode_table[16][256];
  static unsigned short huffman_array[0xC000];
  static unsigned int   huffman_length;
  static bool           _huffman_init;
  static bool           _huffman_swap;
  static unsigned short _huffman_version;
  static void           huffman_init(unsigned int run_id);
  static unsigned short huffman_bit(unsigned short *data, unsigned int i) {
    return (data[i>>4] & (1 << (0xF - (i&0xF)))) >> (0xF - (i&0xF));
  };
public:
  static size_t         compressable(unsigned short *istream, size_t inputl, unsigned int run_id);
  static bool           compress(unsigned short *istream, size_t inputl, unsigned short *ostream, size_t outputl); 
  static size_t         decompressable(unsigned short *istream, size_t inputl, unsigned int run_id);
  static bool           decompress(unsigned short *istream, size_t inputl, unsigned short *ostream, size_t outputl); 
};

class DAQlv3{
public:
  unsigned short err_proc;                       // proc error
  unsigned short err_run;                       // Desynch error
  unsigned short err_event;                     // Event error

  unsigned short err_tdr;                       // number of TDR  with fatal errors
  unsigned short err_udr;                       // number of UDR  with fatal errors
  unsigned short err_rdr;                       // number of RDR  with fatal errors
  unsigned short err_edr;                       // number of EDR  with fatal errors
  unsigned short err_sdr;                       // number of SDR  with fatal errors
  unsigned short err_lv1;                       // number of LV1  with fatal errors
  unsigned short err_etrg;                      // number of ETRG with fatal errors

static unsigned short int lv3_node_addr[384];
static unsigned short int lv3_jinj_slave[32];
void lv3_xdr_proc(unsigned short int detector, unsigned short int len, unsigned short int *ptr, bool mc);

/******************************************************************
 *                                                                *
 *            Subroutine  lv3_unpack                              *
 *                                                                *
 *         input: *ev_sblock - pointer to DSP subblock            *
 *                 event_id  - JMDC event ID                      *
 *                                                                *
 *        output:  OK status                                      *
 *                 structure lv3_event                            *
 *                                                                *
 ******************************************************************/
 unsigned long long pack();  ///<  pack founction to 64 bit word
 int lv3_unpack(unsigned short int *ev_sblock,unsigned int    event_id, bool mc);
DAQlv3():err_proc(0), err_run(0),err_event(0),err_tdr(0),err_udr(0),err_rdr(0),err_edr(0),err_sdr(0),err_lv1(0),err_etrg(0){}

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
  DAQBlockType():_maxbl(0),_plength(0),_pgmb(0),_pgetdata(0),_pgetlength(0),_next(){}
  DAQBlockType(pgetmaxblocks pgmb, pgetl pgl, pgetdata pget):_maxbl(0),_plength(0),_pgmb(pgmb),_pgetdata(pget),_pgetlength(pgl),_next(){}
  DAQBlockType(const DAQBlockType &o);

  friend class DAQEvent;
};


//const uinteger UINT_MAX=4294967295;
const integer nbtps=24;    // blocks num 


class DAQEvent : public AMSlink{
protected:
  static DAQEvent* _DAQEvent;
  static bool _Waiting;
  static char * _RootDir;
  static uinteger _PRun,_PEvent;
#pragma omp threadprivate(_DAQEvent)
  integer _TrigTime;
  integer _BufferOwner;
  integer _Checked;
  uinteger _Length;
  uinteger _LengthR;
  uinteger _Event;
  uinteger _Run;
  uinteger _RunType;
  uint64 _Offset;
  uinteger _CalibData[32];
  static uinteger _CalibDataS[7*3+1]; //  tracker trd tof rich ecal lvl1 lvl3;  // start stop total
  uinteger _JinjSlaveMask;
  int16u calculate_CRC16(int16u * dat, int16u len);
  time_t _Time;
  uinteger _usec;
public:
  int16u _lvl3[5];
  static const unsigned int _gps_count = 7;
  unsigned int _gps[_gps_count];
  unsigned int _gpsl;
  DAQlv3 lv3;
protected:
  int16u *  _pcur;
  int16u * _pData;
  static const char *_NodeNames[512];
  static const char *_PortNamesJ[32];
  unsigned int _SubLength[7];   //  tracker trd tof rich ecal lvl1 lvl3
  unsigned int _SubCount[7];   //  tracker trd tof rich ecal lvl1 lvl3
  unsigned char _JError[24];
  int16u _JStatus[4];
  static DAQSubDet * _pSD[nbtps];
  static DAQBlockType * _pBT[nbtps];
  static DAQBlockType * _pBTshared[nbtps];
#ifdef _OPENMP
  #pragma omp threadprivate(_pBT)
  #pragma omp threadprivate(_pBTshared)
#endif
  static const integer _OffsetL;
  integer _DDGSBOK();
  integer _EventOK();
  integer getpreset(int16u *pdata);
  uinteger _cl(int16u *pdata);
  uinteger _clb(int16u *pdata);
  uinteger _clll(int16u *pdata);
  integer _cll(int16u *pdata);  // calculate length of length !!!
  integer _cltype(int16u *pdata);  // calculate length of length !!!
  /*
    size_t _compressable(Bytef* istream,size_t length);
    size_t _decompressable(Bytef* istream,size_t length);
    bool _compress(Bytef* istream,size_t ilength,Bytef* ostream,size_t olength);
    bool _decompress(Bytef* istream,size_t ilength,Bytef* ostream,size_t olength);
  */
  static integer _getnode(int16u id){
    return ((id>>5)&((1<<9)-1));
  }
  static const char *  _getnodename(int16u id){
    return _NodeNames[((id>>5)&((1<<9)-1))];
  }
  static  integer _getportj(int16u id) {
    return (id&0x1F);
  }
  static const char *  _getportnamej(int16u id) {
    return _PortNamesJ[(id&0x1F)];
  }
  bool    _isddg(int16u id);       //  identify the detector data group sub block
  int    _isjinj(int16u id);       //  identify the detector data group sub block
  bool    _iscompressed(int16u id);       //  identify the compressed detector data group sub block
  bool    _isjinf(int16u id);       //  identify the detector data group sub block
  bool    _isjlvl1(int16u id);       //  identify the detector data group sub block
  bool    _issdr(int16u id);       //  identify the detector data group sub block
  bool    _istdr(int16u id);       //  identify the detector data group sub block
  bool    _isedr(int16u id);       //  identify the detector data group sub block
  bool    _isudr(int16u id);       //  identify the detector data group sub block
  bool    _isrdr(int16u id);       //  identify the detector data group sub block
  bool    _iscceb(int16u id);       //  identify the detector data group sub block
  bool    _istofst(int16u id);       //  identify the detector data group sub block
  integer _HeaderOK();
  uinteger _GetBlType();
  bool _ComposedBlock();
  void _convert();
  void _convertl(int16u & l16);
  integer _create(uinteger btype=0);
  void _copyEl();
  void _writeEl();
  void _printEl(ostream& o){}
  static int16u _Buffer2[100000];
  static integer _Buffer2Lock;
  static integer _Length2;
#pragma omp threadprivate(_Buffer2,_Buffer2Lock,_Length2)
  static integer _Buffer[50000];
  static size_t  _FileSize;
  static integer _BufferLock;
#pragma omp threadprivate(_Buffer,_BufferLock)
  void _setcalibdata(int mask){
    for (unsigned int i=0;i<sizeof(_CalibData)/sizeof(_CalibData[0]);i++)_CalibData[i]=mask!=0?0:0xFFFFFFFF;
  }
  static void _setcalibdataS(unsigned int run){
    for (unsigned int i=0;i<sizeof(_CalibDataS)/sizeof(_CalibDataS[0]);i++)_CalibDataS[i]=0;
    _CalibDataS[sizeof(_CalibDataS)/sizeof(_CalibDataS[0])-1]=run;
  }
#if (defined  __LINUXNEW__ || defined  __LINUXGNU__)
  static integer _select(const dirent64 * entry=0);
  static int _sort(const dirent64 **e1, const dirent64 ** e2){return strcmp((*e1)->d_name,(*e2)->d_name);}
#endif
#ifdef __DARWIN__
  static integer _select(const dirent * entry=0);
  static int _sort(const dirent **e1, const dirent ** e2){return strcmp((*e1)->d_name,(*e2)->d_name);}
#endif
public:
  static void setRootDir(char *rootdir=0);
  static bool Waiting(){return _Waiting;}
  static char* RootDir(){return _RootDir;}
  static DAQEvent* gethead(){return _DAQEvent;}
  uinteger GetBlType(){return _GetBlType();}
  ~DAQEvent();
  DAQEvent(): AMSlink(),_BufferOwner(0),_Checked(0),_Length(0),_Event(0),_Run(0),
	      _RunType(0),_Offset(0),_Time(0),_usec(0),_pcur(0),_pData(0){
     _JStatus[0]=_JStatus[1]=_JStatus[2]=_JStatus[3]=0;
   for (unsigned  int i=0;i<sizeof(_JError)/sizeof(_JError[0]);i++)_JError[i]=0;
    for (unsigned int i=0;i<sizeof(_SubLength)/sizeof(_SubLength[0]);i++)_SubLength[i]=0;
    for (unsigned int i=0;i<sizeof(_SubCount)/sizeof(_SubCount[0]);i++)_SubCount[i]=0;
    _setcalibdata(0);_DAQEvent=this;
    for(unsigned int k=0;k<_gps_count;k++)_gps[k]=0;
    _gpsl=0;
  }
  static size_t FileSize(){return _FileSize;}
  static bool SetFileSize(const char *fnam);
  static bool ismynode(int16u id,char * sstr){return id<32?strstr(_getportnamej(id),sstr)!=0:(strstr(_getnodename(id),sstr)!=0);}
  static bool isRawMode(int16u id){return (id&64)>0;}
  static bool isCompMode(int16u id){return (id&128)>0;}
  static bool isError(int16u id){return (id&512)>0;}
  static const char *  getnodename(int16u idn){return _NodeNames[idn];}
  static const char *  getportname(int16u idn){return _PortNamesJ[idn];}
  uinteger & eventno(){return _Event;}
  integer trigtime()  {return _TrigTime;}
  uinteger & runno(){return _Run;}
  time_t   & time(){return _Time;}
  uinteger & runtype(){return _RunType;}
  uinteger & usec(){return _usec;}
  void buildDAQ(uinteger btype=0);
  void buildRawStructures();
  void buildRawStructuresEarly();
  void write();
  integer read();
  uint64 getoffset()const {return _Offset;}
  uint64 getsoffset();
  void setoffset(uint64 offset);
  void shrink();
  integer getlengthR() const {return _LengthR*sizeof(_pData[0]);}
  integer getlength() const {return _Length*sizeof(_pData[0]);}
  integer getsublength(unsigned int i) const {return i<sizeof(_SubLength)/sizeof(_SubLength[0])?_SubLength[i]*sizeof(_pData[0]):0;}
  unsigned char getjerror(unsigned int i)const {return i<sizeof(_JError)/sizeof(_JError[0])?_JError[i]:0;}
  int16u getjstatus(int ii) const {return _JStatus[ii];}
  integer getsubcount(unsigned int i) const {return i<sizeof(_SubCount)/sizeof(_SubCount[0])?_SubCount[i]:0;}
  uinteger getcalibdata(unsigned int i) const {return i<sizeof(_CalibData)/sizeof(_CalibData[0])?_CalibData[i]:0;}
  bool  CalibRequested(unsigned int crate, unsigned int xdr);
  static bool  CalibDone(int id);
  static bool  CalibInit(int id);
  void close(){ fbin.close();fbout.close();}
  static char * _DirName;
  static char ** ifnam;
  static integer InputFiles;
  static integer KIFiles;
  static fstream fbin;
  static char * ofnam;
  static fstream fbout;
  static uinteger _NeventsO;
  static uinteger _NeventsPerRun;
  static void CloseO(integer run,integer event,time_t tt);
  void SetEOFIn();
  enum InitResult{Interrupt,NoInputFile,UnableToOpenFile,UnableToFindRunEvent,OK};
  static InitResult init();
  static void select();
  static char * _getNextFile(integer & run, integer & event);
  static void initO(integer run,integer event,time_t tt);
  static void addsubdetector(pid pgetid, pputdata pput, uinteger btype=0);
  static void addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget, uinteger btype=0);

  //+
  void    data(uint16 *buff) {memcpy(buff,_pData,getlength());}
  uint16  sdetlength(uint16 sdetid);
  integer sdet(uint16 sdetid);
  void    dump(uint16 sdetid); 
  //-  
  static void setfiles(char *ifile, char *ofile);
  static void setfile(const char *ifile);
  static const char * getfile();
  static integer parser(char *ifile, char ** & ifnam); 
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
