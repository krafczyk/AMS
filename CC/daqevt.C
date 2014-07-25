//  $Id$
#ifdef __CORBA__
#include <producer.h>
#endif
#include <stdio.h>
#include "daqevt.h"
#include "trigger102.h"
#include "event.h"
#ifdef __WRITEROOT__
#include "ntuple.h"
#endif
#include "commons.h"
#include <sys/stat.h>
#include <sys/file.h>
#include <ctype.h>
#include "astring.h"
#include <fstream>
#include <unistd.h>
#include <strstream>
#include "daqs2block.h"
#include "tofsim02.h"
#include "richdaq.h"
#ifdef _PGTRACK_
#include "tkdcards.h"
#include "MagField.h"
#include "TrCalDB.h"
#endif
#ifdef __LVL3ONLY__
ofstream fbin1("/f2users/choutko/AMS/examples/zip.txt",ios::out);
#endif



#ifndef __ALPHA__
using std::ostrstream;
using std::istrstream;
#endif
#if !defined( __IBMAIX__) && !defined(sun) 
#else

//#define _D_NAME_MAX 255

//struct  dirent {
//         ulong_t         d_offset;       /* real off after this entry */
//         ino_t           d_ino;          /* inode number of entry */
/* make ino_t when it's ulong */
//         ushort_t        d_reclen;       /* length of this record */
//         ushort_t        d_namlen;       /* length of string in d_name */
//         char            d_name[_D_NAME_MAX+1];  /* name must be no longer than t
// his */
//                                         /* redefine w/#define when name decided
// */
// };






extern "C" int scandir64(		const char *, struct dirent64 ***, 
					int (*)(struct dirent64 *),  
					int (*)(struct dirent64 **, struct dirent64 **));
#endif



extern "C" size_t _compressable(Bytef* istream,size_t length);
extern "C" size_t _decompressable(Bytef* istream,size_t length);
extern "C" bool _compress(Bytef* istream,size_t ilength,Bytef* ostream,size_t olength);
extern "C" bool _decompress(Bytef* istream,size_t ilength,Bytef* ostream,size_t olength);



char * DAQEvent::_RootDir=0;
bool DAQEvent::_Waiting=false;
DAQEvent* DAQEvent::_DAQEvent=0;
uinteger DAQEvent::_PRun=0;
uinteger DAQEvent::_PEvent=0;
integer DAQEvent::_Buffer[50000];
int16u DAQEvent::_Buffer2[100000];
integer DAQEvent::_BufferLock=0;
size_t DAQEvent::_FileSize=0;
integer DAQEvent::_Buffer2Lock=0;
integer DAQEvent::_Length2=0;
const integer lover=2;
uinteger      Time_1;

DAQEvent::~DAQEvent(){
  shrink();
  _DAQEvent=0;
}

int16u DAQEvent::calculate_CRC16(int16u *dat, int16u len) {

  int i;
  int16u CRC = 0xFFFF;
  int16u g = 0x1021;

  for (i=0; i<len; i++) {
    int j;
    //    int16u d = (((dat[i])&255)<<8) | (dat[i]>>8); 
    int16u d = dat[i];
    for (j=0; j<16; j++) {
      if ((CRC ^ d) & 0x8000) CRC = (CRC << 1) ^ g;
      else                    CRC = (CRC << 1);
      d <<= 1;
    }
  }
  return CRC;
}


void DAQEvent::shrink(){

  if(_pData && !_BufferOwner){
       UPool.udelete(_pData);
//       if (UPool.size()>10000000)UPool.erase(1);
  }
  if(_BufferOwner)_BufferLock=0;
  _pData=0;
  //_Length=0;
  _pcur=0;
  _Event=0;
  _Run=0;
  _RunType=0;
  _Time=0;
  _Checked=0;
  _BufferOwner=0;
}

DAQSubDet * DAQEvent::_pSD[nbtps]={0,0,0,0,0,0,0,0};
DAQBlockType * DAQEvent::_pBT[nbtps]={0,0,0,0,0,0,0,0};

void DAQEvent::addsubdetector(pid pgetid, pputdata pput, uinteger btype){
  DAQSubDet * p= new DAQSubDet(pgetid, pput);
  if(_pSD[btype]){
    DAQSubDet *paux=_pSD[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pSD[btype]=p;
}
void DAQEvent::addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget,uinteger btype){
  DAQBlockType * p= new DAQBlockType(pgmb,pgl,pget);
  if(_pBT[btype]){
    DAQBlockType *paux=_pBT[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pBT[btype]=p;
  p->_maxbl=(p->_pgmb)();
  p->_plength=new integer[p->_maxbl];
  if(p->_plength == NULL){
    cerr<<"DAQEvent::addblock-F-Memory exhausted "<<p->_maxbl<<endl;  
    p->_maxbl=0;
    throw amsglobalerror();     
  }
}

const char* DAQEvent::_PortNamesJ[32] = {
  "JINFT2", "JINFT3", "JINFU1", "JINFT0",
  "SDR-1A", "SDR-1B", "SDR-0A", "SDR-0B",
  "JINFU0", "JINFT1", "JINFR0", "JINFR1",
  "JINFE0", "JINFE1", "JLV1-A", "JLV1-B",
  "JINFT4", "JINFT5", "SDR-2A", "SDR-2B",
  "SDR-3A", "SDR-3B", "JINFT6", "JINFT7",
  "MCPART",  "MCISSA", "MCTRCL", "      ",
  "     ", "     ", "     ", "      "
};

const char* DAQEvent::_NodeNames[512]={
  "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3", "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3",     // 000 - 007
  "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3", "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3",     // 008 - 015
  "JMDC-H", "JMDC-L", "JMDC-D", "JMDC-M", "N-X014", "N-X015", "ACOP_N", "JMDC-A",     // 016 - 023
  "CAB-A",  "CAB-B",  "CAB-P",  "CAB-S",  "CCEB-A", "CCEB-B", "CCEB-P", "CCEB-S",     // 024 - 031
  "EPD-0A", "EPD-0B", "EPD-0P", "EPD-0S", "EPD-1A", "EPD-1B", "EPD-1P", "EPD-1S",     // 032 - 039
  "JPD-A",  "JPD-B",  "JPD-P",  "JPD-S",  "MPD-A",  "MPD-B",  "MPD-P",  "MPD-S",      // 040 - 047
  "PDS-A",  "PDS-B",  "PDS-P",  "PDS-S",  "RPD-0A", "RPD-0B", "RPD-0P", "RPD-0S",     // 048 - 055
  "RPD-1A", "RPD-1B", "RPD-1P", "RPD-1S", "SPD-0A", "SPD-0B", "SPD-0P", "SPD-0S",     // 056 - 063
  "SPD-1A", "SPD-1B", "SPD-1P", "SPD-1S", "SPD-2A", "SPD-2B", "SPD-2P", "SPD-2S",     // 064 - 071
  "SPD-3A", "SPD-3B", "SPD-3P", "SPD-3S", "TPD-0A", "TPD-0B", "TPD-0P", "TPD-0S",     // 072 - 079
  "TPD-1A", "TPD-1B", "TPD-1P", "TPD-1S", "TPD-2A", "TPD-2B", "TPD-2P", "TPD-2S",     // 080 - 087
  "TPD-3A", "TPD-3B", "TPD-3P", "TPD-3S", "TPD-4A", "TPD-4B", "TPD-4P", "TPD-4S",     // 088 - 095
  "TPD-5A", "TPD-5B", "TPD-5P", "TPD-5S", "TPD-6A", "TPD-6B", "TPD-6P", "TPD-6S",     // 096 - 103
  "TPD-7A", "TPD-7B", "TPD-7P", "TPD-7S", "TTCE-A", "TTCE-B", "TTCE-P", "TTCE-S",     // 104 - 111
  "UPD-0A", "UPD-0B", "UPD-0P", "UPD-0S", "UPD-1A", "UPD-1B", "UPD-1P", "UPD-1S",     // 112 - 119
  "UGPD-A", "UGPD-B", "UGPD-P", "UGPD-S", "CAB-A",  "CAB-B",  "CAB-P",  "CAB-S",      // 120 - 127
  "JINJ-0", "JINJ-1", "JINJ-2", "JINJ-3", "JINJ-P", "JINJ-S", "JINJ-T", "JINJ-Q",     // 128 - 135
  "JLV1-A", "JLV1-B", "JLV1-P", "JLV1-S", "ETRG0A", "ETRG0B", "ETRG0P", "ETRG0S",     // 136 - 143
  "ETRG1A", "ETRG1B", "ETRG1P", "ETRG1S", "N-X094", "N-X095", "JF-E0A", "JF-E0B",     // 144 - 151
  "JF-E0P", "JF-E0S", "JF-E1A", "JF-E1B", "JF-E1P", "JF-E1S", "JF-R0A", "JF-R0B",     // 152 - 159
  "JF-R0P", "JF-R0S", "JF-R1A", "JF-R1B", "JF-R1P", "JF-R1S", "JF-T0A", "JF-T0B",     // 160 - 167
  "JF-T0P", "JF-T0S", "JF-T1A", "JF-T1B", "JF-T1P", "JF-T1S", "JF-T2A", "JF-T2B",     // 168 - 175
  "JF-T2P", "JF-T2S", "JF-T3A", "JF-T3B", "JF-T3P", "JF-T3S", "JF-T4A", "JF-T4B",     // 176 - 183
  "JF-T4P", "JF-T4S", "JF-T5A", "JF-T5B", "JF-T5P", "JF-T5S", "JF-T6A", "JF-T6B",     // 184 - 191
  "JF-T6P", "JF-T6S", "JF-T7A", "JF-T7B", "JF-T7P", "JF-T7S", "JF-U0A", "JF-U0B",     // 192 - 199
  "JF-U0P", "JF-U0S", "JF-U1A", "JF-U1B", "JF-U1P", "JF-U1S", "EDR00A", "EDR00B",     // 200 - 207
  "EDR00P", "EDR01A", "EDR01B", "EDR01P", "EDR02A", "EDR02B", "EDR02P", "EDR03A",     // 208 - 215
  "EDR03B", "EDR03P", "EDR04A", "EDR04B", "EDR04P", "EDR05A", "EDR05B", "EDR05P",     // 216 - 223
  "EDR10A", "EDR10B", "EDR10P", "EDR11A", "EDR11B", "EDR11P", "EDR12A", "EDR12B",     // 224 - 231
  "EDR12P", "EDR13A", "EDR13B", "EDR13P", "EDR14A", "EDR14B", "EDR14P", "EDR15A",     // 232 - 239
  "EDR15B", "EDR15P", "RDR00A", "RDR00B", "RDR01A", "RDR01B", "RDR02A", "RDR02B",     // 240 - 247
  "RDR03A", "RDR03B", "RDR04A", "RDR04B", "RDR05A", "RDR05B", "RDR10A", "RDR10B",     // 248 - 255
  "RDR11A", "RDR11B", "RDR12A", "RDR12B", "RDR13A", "RDR13B", "RDR14A", "RDR14B",     // 256 - 263
  "RDR15A", "RDR15B", "SDR-0A", "SDR-0B", "SDR-0P", "SDR-0S", "SDR-1A", "SDR-1B",     // 264 - 271
  "SDR-1P", "SDR-1S", "SDR-2A", "SDR-2B", "SDR-2P", "SDR-2S", "SDR-3A", "SDR-3B",     // 272 - 279
  "SDR-3P", "SDR-3S", "TDR00A", "TDR00B", "TDR01A", "TDR01B", "TDR02A", "TDR02B",     // 280 - 287
  "TDR03A", "TDR03B", "TDR04A", "TDR04B", "TDR05A", "TDR05B", "TDR06A", "TDR06B",     // 288 - 295
  "TDR07A", "TDR07B", "TDR08A", "TDR08B", "TDR09A", "TDR09B", "TDR0AA", "TDR0AB",     // 296 - 303
  "TDR0BA", "TDR0BB", "TDR10A", "TDR10B", "TDR11A", "TDR11B", "TDR12A", "TDR12B",     // 304 - 311
  "TDR13A", "TDR13B", "TDR14A", "TDR14B", "TDR15A", "TDR15B", "TDR16A", "TDR16B",     // 312 - 319
  "TDR17A", "TDR17B", "TDR18A", "TDR18B", "TDR19A", "TDR19B", "TDR1AA", "TDR1AB",     // 320 - 327
  "TDR1BA", "TDR1BB", "TDR20A", "TDR20B", "TDR21A", "TDR21B", "TDR22A", "TDR22B",     // 328 - 335
  "TDR23A", "TDR23B", "TDR24A", "TDR24B", "TDR25A", "TDR25B", "TDR26A", "TDR26B",     // 336 - 343
  "TDR27A", "TDR27B", "TDR28A", "TDR28B", "TDR29A", "TDR29B", "TDR2AA", "TDR2AB",     // 344 - 351
  "TDR2BA", "TDR2BB", "TDR30A", "TDR30B", "TDR31A", "TDR31B", "TDR32A", "TDR32B",     // 352 - 359
  "TDR33A", "TDR33B", "TDR34A", "TDR34B", "TDR35A", "TDR35B", "TDR36A", "TDR36B",     // 360 - 367
  "TDR37A", "TDR37B", "TDR38A", "TDR38B", "TDR39A", "TDR39B", "TDR3AA", "TDR3AB",     // 368 - 375
  "TDR3BA", "TDR3BB", "TDR40A", "TDR40B", "TDR41A", "TDR41B", "TDR42A", "TDR42B",     // 376 - 383
  "TDR43A", "TDR43B", "TDR44A", "TDR44B", "TDR45A", "TDR45B", "TDR46A", "TDR46B",     // 384 - 391
  "TDR47A", "TDR47B", "TDR48A", "TDR48B", "TDR49A", "TDR49B", "TDR4AA", "TDR4AB",     // 392 - 399
  "TDR4BA", "TDR4BB", "TDR50A", "TDR50B", "TDR51A", "TDR51B", "TDR52A", "TDR52B",     // 400 - 407
  "TDR53A", "TDR53B", "TDR54A", "TDR54B", "TDR55A", "TDR55B", "TDR56A", "TDR56B",     // 408 - 415
  "TDR57A", "TDR57B", "TDR58A", "TDR58B", "TDR59A", "TDR59B", "TDR5AA", "TDR5AB",     // 416 - 423
  "TDR5BA", "TDR5BB", "TDR60A", "TDR60B", "TDR61A", "TDR61B", "TDR62A", "TDR62B",     // 424 - 431
  "TDR63A", "TDR63B", "TDR64A", "TDR64B", "TDR65A", "TDR65B", "TDR66A", "TDR66B",     // 432 - 439
  "TDR67A", "TDR67B", "TDR68A", "TDR68B", "TDR69A", "TDR69B", "TDR6AA", "TDR6AB",     // 440 - 447
  "TDR6BA", "TDR6BB", "TDR70A", "TDR70B", "TDR71A", "TDR71B", "TDR72A", "TDR72B",     // 448 - 455
  "TDR73A", "TDR73B", "TDR74A", "TDR74B", "TDR75A", "TDR75B", "TDR76A", "TDR76B",     // 456 - 463
  "TDR77A", "TDR77B", "TDR78A", "TDR78B", "TDR79A", "TDR79B", "TDR7AA", "TDR7AB",     // 464 - 471
  "TDR7BA", "TDR7BB", "UDR00A", "UDR00B", "UDR00P", "UDR01A", "UDR01B", "UDR01P",     // 472 - 479
  "UDR02A", "UDR02B", "UDR02P", "UDR03A", "UDR03B", "UDR03P", "UDR04A", "UDR04B",     // 480 - 487
  "UDR04P", "UDR05A", "UDR05B", "UDR05P", "UDR10A", "UDR10B", "UDR10P", "UDR11A",     // 488 - 495
  "UDR11B", "UDR11P", "UDR12A", "UDR12B", "UDR12P", "UDR13A", "UDR13B", "UDR13P",     // 496 - 503
  "UDR14A", "UDR14B", "UDR14P", "UDR15A", "UDR15B", "UDR15P", "N-X1FE", "N-X1FF",     // 504 - 511
};

const integer DAQEvent::_OffsetL=1;
char *  DAQEvent::_DirName=0;
char ** DAQEvent::ifnam=0;
integer DAQEvent::InputFiles=0;
integer DAQEvent::KIFiles=0;
char * DAQEvent::ofnam=0;
fstream DAQEvent::fbin;
fstream DAQEvent::fbout;
uinteger DAQEvent::_NeventsO=0;
uinteger DAQEvent::_NeventsPerRun=0;

void DAQEvent::setfile(const char *ifile){
  if(ifnam[0])delete [] ifnam[0];
  ifnam[0]=new char[strlen(ifile)+1];
  strcpy(ifnam[0],ifile);
  InputFiles=1;
  KIFiles=0;
}

const char * DAQEvent::getfile(){
  return ifnam[0];
}

void DAQEvent::setfiles(char *ifile, char *ofile){
  if(ifile){
    if(!_DirName){
      _DirName=new char[strlen(ifile)+1];
      strcpy(_DirName,ifile);
    }
    InputFiles=parser(ifile,ifnam);
    if(InputFiles)
      cout <<"DAQEvent::setfiles-I-"<<InputFiles<<" input files parsed"<<endl;
    else if(DAQCFFKEY.mode%10){
      ifnam=new char*[1];
      ifnam[0]=0;
      InputFiles=0;
      KIFiles=0;
      //cerr <<"DAQEvent::setfiles-F-No input files parsed"<<endl; 
    }
  }
  if(ofile){
    ofnam=new char[strlen(ofile)+1];
    strcpy(ofnam,ofile);
  }
}


void DAQEvent::buildDAQ(uinteger btype){
  DAQBlockType *fpl=_pBT[btype];
  if(fpl == NULL && btype){
    static int init=0;
    if(init++==0)cerr << "DAQEvent::build-S-NoSubdetectors in DAQ"<<endl;
    return;
  }


  int16u* pjinj=0;
  integer ntot=0;
  integer ntotm=0;
  while(fpl){
    for(int i=0;i<fpl->_maxbl;i++){
      int len=(fpl->_pgetlength)(i);
      if(len>=16384 || len<=-32768){
	cerr<<" DAQEvent::buildDaq-E-SubDLTooBigIgnoring "<<len<<endl;
	len=0;
      }
      *(fpl->_plength+i)=len>0?len+_OffsetL:len-_OffsetL;
      if(abs(*(fpl->_plength+i))>1)ntot+=abs(*(fpl->_plength+i));
      if((*(fpl->_plength+i))<-1)ntotm-=(*(fpl->_plength+i));
    }
    fpl=fpl->_next;
  }


  // Make array

  if(!ntot )return;
  int preset=ntotm?4+5:4;
  _Length=preset+ntot+_OffsetL;
  const int thr=32767;
  if((_Length-_OffsetL)*sizeof(*_pcur) > thr){
    cout<<"DAQEvent::buildDAQ-W-lengthToobig "<<_Length<<" "<<ntotm<<endl;
    _Length++;
    if((ntotm+5-_OffsetL)*sizeof(*_pcur)>32767)_Length++;
  }
#ifdef __AMSDEBUG__
  assert(sizeof(time_t) == sizeof(integer));
#endif

  int btype5=btype;
  if(btype==0)btype5=5;
  else if(btype==5)btype5=0;
  if(_create(btype5) ){

    int16u * _pbeg=_pcur;


    fpl=_pBT[btype];
    while(fpl){
      for(int i=0;i<fpl->_maxbl;i++){
	if(*(fpl->_plength+i)>1){
	  *_pcur=(*(fpl->_plength+i)-_OffsetL)*2;
	  int16u *psafe=_pcur+1;
	  fpl->_pgetdata(i,*(fpl->_plength+i)-1,psafe);
	  _pcur=_pcur+*_pcur/2+_OffsetL;
	}
      }
      fpl=fpl->_next;
    }
    if((ntotm+4)*2 >32767 && 0){
      cerr<<"DAQEvent::buildDAQ-E-jinjBlockLengthTooBigWillNotBeEncoded "<<ntotm<<endl; 
    }
    else if(ntotm){
      fpl=_pBT[btype];
      uint lntotm=(ntotm+5-_OffsetL)*2;
      while(fpl){
	for(int i=0;i<fpl->_maxbl;i++){
	  if(*(fpl->_plength+i)<-1){
	    if( ntotm){
	      if(lntotm <=32767){
		*_pcur=lntotm;
		pjinj=_pcur;
	      }
	      else{
		*_pcur=(lntotm>>16)&32767;
		*(_pcur+1)=(lntotm)&65535;
		*_pcur=*_pcur | (1<<15);
		pjinj=++_pcur;
	      }
	      ntotm=0;
	      if(*(fpl->_plength+i)<=-32768){
		cerr<<" DAQEvent::buildDaq-E-SubDLTooBigIgnoring "<<-*(fpl->_plength+i)<<endl;
		continue;
	      }
	      *(_pcur+1)=1 | (128<<5) | (1<<15);
	      *(_pcur+2)=AMSEvent::gethead()->getid()&65535;
	      _pcur+=3; 
	    }  
	    *_pcur=-*(fpl->_plength+i)-_OffsetL;
	    int16u *psafe=_pcur+1;
	    fpl->_pgetdata(i,-*(fpl->_plength+i)-1,psafe);
	    _pcur=_pcur+*_pcur+_OffsetL;
	  }
	}
	fpl=fpl->_next;
      }
      if(pjinj) {
	*(_pcur)=0;
	*(_pcur+1)=calculate_CRC16((pjinj+2),lntotm/2-2);
	if(DAQCFFKEY.mode/100==9) {
	  //
	  // special daqcdffkey.mode/100==9;
	  // hoffman compression (assumed to be same as  in jmdc) compressin
	  //
	  //  only jmdc block is compressed
	  //  Mark  jmdc data type as 4 instead of 1
	  // 
	  const int HDRMAX=4;

	  AMSgObj::BookTimer.start("SIZIP");
	  static int ziperr=0;
	  uint l2c=*(pjinj)-2;
	  size_t outl=DAQCompress::compressable((unsigned short *)(pjinj+2), l2c, _Run);
	  if(outl) {
	    Bytef* ostream=(Bytef*)calloc(outl+2,sizeof(Bytef));
	    if(DAQCompress::compress((unsigned short *)(pjinj+2), l2c, (unsigned short *)ostream, outl)){ 
	      *(pjinj)=*(pjinj)+(outl-l2c);
	      *(pjinj+1)+=3;
	      char *src=(char*)pjinj;
	      int hdrsize=sizeof(_pData[0])*2;
	      int inputl=DAQCompress::decompressable((unsigned short *)ostream, outl, _Run);
	      //    cout <<"  *** imput/output "<<inputl<<" "<<l2c<<" "<<outl;
	      memcpy(src+hdrsize,ostream,outl);
	      if(outl%2){
		src[hdrsize+outl]=0;
	      }

	      //
	      // Change length
	      //  
#ifdef __LVL3ONLY__
	      uint lold=_Length;
#endif    
	      uint len=_clb(_pData)+outl-l2c;
	      int16u offset=_pbeg-_pData; 
	      if(offset == 5){    // short length format
		_pData[0]=len;
		if(len>32767){
		  cerr<<"DAQEvent::builddaq-S-LogicErrorLengthTooBig "<<len<<" "<<_Length<<endl;
		}
		_Length=(len+1)/2+_OffsetL;
	      }
	      else{
		_pData[0]=(len>>16)&32767;
		_pData[1]=(len)&65535;
		_pData[0]=_pData[0] | (1<<15);
		_Length=(len+1)/2+_OffsetL+1;
	      } 
	      AMSgObj::BookTimer.stop("SIZIP");
#ifdef __LVL3ONLY__
	      fbin1<<AMSEvent::gethead()->getid()<<" "<<lold*2<<" "<<_Length*2<<endl;
#endif


	    }
	    else{
	      int static ierr=0;
	      if(ierr++<10)cerr<<"DAQEvent::biuldDAQ-E-CompressionError "<<l2c<<endl;
	    }
	    free(ostream);
	  }
	  else{
	    int static ierr=0;
	    if(ierr++<10)cerr<<"DAQEvent::biuldDAQ-W-EventNotCompressable "<<l2c<<endl;
	  }

	}
      }
    }


    // add zlib compression
    // adopted from root package bits.h
    //
    // format as ams block
    // add bit 13 in FBI/STATUS in AMSB 2ndary Header (status ==10)
    //
    // add compressed block length
    //  if length%2 !=0 add 0 to the last byte.  
    //  do not change length 
    //
    const int HDRMAX=4;

    if(DAQCFFKEY.mode/100==9){
      //  return ;
      return;
    }
    else if(DAQCFFKEY.mode/100==8){
      //
      // special daqcdffkey.mode/100=8;
      // zero compressin
      AMSgObj::BookTimer.start("SIZIP");
      static int ziperr=0;
      int16u offset=_pbeg-_pData; 
      uint l2c=(_Length-offset)*sizeof(_pData[0]);
      if(l2c>0xfffff ){
	if(ziperr++<10)cerr<<"DAQEvent::builddaq-W-LengthTooBigWillNotBe Compressed "<<l2c<<endl;
        AMSgObj::BookTimer.stop("SIZIP");
	return;
      }
      size_t outl=_compressable((Bytef*)(_pbeg),l2c);
      //cout <<"  outl "<<outl<<" "<<l2c<<endl;
      if(outl){
	Bytef* ostream=(Bytef*)calloc(outl+2,sizeof(Bytef));
	if(_compress((Bytef*)(_pbeg),l2c,ostream,outl)){ 
	  char *src=(char*)_pbeg;
	  int hdrsize=outl<=32767?sizeof(_pData[0]):2*sizeof(_pData[0]);
	  memcpy(src+hdrsize,ostream,outl);
	  if(outl%2){
	    src[hdrsize+outl]=0;
	  }
	  //printf("byte %d %d %d %d \n",src[hdrsize],ostream[0],ostream[1],ostream[outl-1]);

	  //
	  //  add compressed bit
	  //
	  *(_pbeg-3)= (*(_pbeg-3)) | (1<<14);

	  //




	  //
	  //  add subblock length
	  //
	  if(outl<=32767){
	    *_pbeg=outl;
	  }
	  else{
	    *_pbeg=(outl>>16)&32767;
	    *(_pbeg+1)=(outl)&65535;
	    *_pbeg=(*_pbeg) | (1<<15);
	  }

	  /*
	  // 
	  // calculate crc
	  //
	  *(_pbeg+(suboffset+lc+1)/sizeof(_pbeg[0]))=calculate_CRC16(_pbeg+suboffset/sizeof(_pbeg[0]),(lc+1)/sizeof(_pbeg[0]));  
	  */

	  //
	  // Change length
	  //  
#ifdef __LVL3ONLY__
	  uint lold=_Length;
#endif    
	  uint len=outl+sizeof(_pData[0])*(5+_cll(_pbeg)-_OffsetL);
	  if(offset == 5){    // short length format
	    _pData[0]=len;
	    if(len>32767){
	      cerr<<"DAQEvent::builddaq-S-LogicErrorLengthTooBig "<<len<<" "<<_Length<<endl;
	    }
	    _Length=(len+1)/2+_OffsetL;
	  }
	  else{
	    _pData[0]=(len>>16)&32767;
	    _pData[1]=(len)&65535;
	    _pData[0]=_pData[0] | (1<<15);
	    _Length=(len+1)/2+_OffsetL+1;
	  } 
	  AMSgObj::BookTimer.stop("SIZIP");
#ifdef __LVL3ONLY__
	  fbin1<<AMSEvent::gethead()->getid()<<" "<<lold*2<<" "<<_Length*2<<endl;
#endif


	}
	free(ostream);
      }

    }
    else if (DAQCFFKEY.mode/100){
      AMSgObj::BookTimer.start("SIZIP");
      static int ziperr=0;
      int16u offset=_pbeg-_pData; 
      uint l2c=(_Length-offset)*sizeof(_pData[0]);
      if(l2c>0xfffff ){
	if(ziperr++<10)cerr<<"DAQEvent::builddaq-W-LengthTooBigWillNotBe Compressed "<<l2c<<endl;
        AMSgObj::BookTimer.stop("SIZIP");
	return;
      }
      int err=0;
      z_stream stream;
      
      stream.next_in   = (Bytef*)(_pbeg);
      stream.avail_in  = l2c;
      int outl=l2c+HDRMAX;
      char tgt[outl];
      stream.next_out  = (Bytef*)(tgt);
      stream.avail_out = sizeof(tgt);

      stream.zalloc    = (alloc_func)0;
      stream.zfree     = (free_func)0;
      stream.opaque    = (voidpf)0;
      int cxlevel=DAQCFFKEY.mode/100;
      //    err = deflateInit(&stream, cxlevel);
      int method=Z_DEFLATED;
      //    cout <<" event "<<AMSEvent::gethead()->getid()<<" "<<_Length*2<<endl;
      err = deflateInit2(&stream, cxlevel,method,12,8,Z_FILTERED);
      if (err != Z_OK) {
	printf("error %d in deflateInit (zlib)\n",err);
        AMSgObj::BookTimer.stop("SIZIP");
	return;
      }

      err = deflate(&stream, Z_FINISH);
      if (err != Z_STREAM_END) {
	deflateEnd(&stream);
	//          the buffer cannot be compressed or compressed buffer would be larger than original buffer
	printf("error %d in deflate (zlib) is not = %d\n",err,Z_STREAM_END);
        AMSgObj::BookTimer.stop("SIZIP");
	return;
      }

      err = deflateEnd(&stream);
      if(stream.total_out+HDRMAX>=l2c){
	if(ziperr++<10)cerr<<"DAQEvent::builddaq-E-CanNotCompress "<<l2c<<" "<<stream.total_out<<endl;
        AMSgObj::BookTimer.stop("SIZIP");
	return;
      }
      int lc=stream.total_out;
      char *src=(char*)_pbeg;
      int hdrsize=lc<=32767?sizeof(_pData[0]):2*sizeof(_pData[0]);
      memcpy(src+hdrsize,tgt,lc);
      if(lc%2){
	src[hdrsize+lc]=0;
      }
      //  
      //  add compressed bit
      //
      *(_pbeg-3)= (*(_pbeg-3)) | (1<<13);

      //
      //  add subblock length
      //
      if(lc<=32767){
	*_pbeg=lc;
      }
      else{
	*_pbeg=(lc>>16)&32767;
	*(_pbeg+1)=(lc)&65535;
	*_pbeg=(*_pbeg) | (1<<15);
      }

      /*
      // 
      // calculate crc
      //
      *(_pbeg+(suboffset+lc+1)/sizeof(_pbeg[0]))=calculate_CRC16(_pbeg+suboffset/sizeof(_pbeg[0]),(lc+1)/sizeof(_pbeg[0]));  
      */

      //
      // Change length
      //  
#ifdef __LVL3ONLY__
      uint lold=_Length;
#endif    
      uint len=lc+sizeof(_pData[0])*(5+_cll(_pbeg)-_OffsetL);
      if(offset == 5){    // short length format
	_pData[0]=len;
	if(len>32767){
	  cerr<<"DAQEvent::builddaq-S-LogicErrorLengthTooBig "<<len<<" "<<_Length<<endl;
	}
	_Length=(len+1)/2+_OffsetL;
      }
      else{
	_pData[0]=(len>>16)&32767;
	_pData[1]=(len)&65535;
	_pData[0]=_pData[0] | (1<<15);
	_Length=(len+1)/2+_OffsetL+1;
      } 
      AMSgObj::BookTimer.stop("SIZIP");
#ifdef __LVL3ONLY__
      fbin1<<AMSEvent::gethead()->getid()<<" "<<lold*2<<" "<<_Length*2<<endl;   
#endif    
    }

  }
}

uinteger DAQEvent::_cl(int16u *pdata){
  const  int16u lmask=0x8000; 
  const int16u hmask=lmask-1;
  uinteger len=(*pdata)&lmask?(*(pdata+1)|(((*pdata)&hmask)<<16))+sizeof(pdata[0]): *(pdata);
  len=(len+sizeof(pdata[0])/2)/sizeof(pdata[0]);
  return len+_OffsetL;

}




uinteger DAQEvent::_clb(int16u *pdata){
  const  int16u lmask=0x8000; 
  const int16u hmask=lmask-1;
  uinteger len=(*pdata)&lmask?(*(pdata+1)|(((*pdata)&hmask)<<16)): *(pdata);
  return len;
}



bool    DAQEvent::_isddg(int16u id){
#ifdef __AMSDEBUG__
  cout <<"DAQEvent::_isddg-I-"<<id<<" "<<(id&31)<<" "<<((id>>5)&((1<<9)-1))<<" "<<(id>>14)<<endl;
#endif
  // accordig x.cai
  if(((id&31) ==1 || (id&31)==4)&& ((id>>5)&((1<<9)-1))>=128 && (id>>14)==2)return true;
  else return false;
}


bool    DAQEvent::_iscompressed(int16u id){
  if((id&31) ==4)return true;
  else return false;
}


int    DAQEvent::_isjinj(int16u id){
  //if(((id&31) ==1 || (id&31)==4)&& ((id>>5)&((1<<9)-1))>=128 && ((id>>5)&((1<<9)-1))<=135 && (id>>14)==2)return true;
  if( ((id>>5)&((1<<9)-1))>=128 && ((id>>5)&((1<<9)-1))<=135 && (id>>14)==2)return ((id>>5)&((1<<9)-1));
  else return 0;
}

bool    DAQEvent::_isjinf(int16u id){
  //if((id&31) ==1 && ((id>>5)&((1<<9)-1))>=150 && ((id>>5)&((1<<9)-1))<=205 && (id>>14)==2)return true;
  if( ((id>>5)&((1<<9)-1))>=150 && ((id>>5)&((1<<9)-1))<=205 && (id>>14)==2)return true;
  else return false;
}

bool    DAQEvent::_issdr(int16u id){
  if( ((id>>5)&((1<<9)-1))>=266 && ((id>>5)&((1<<9)-1))<=281 )return true;
  else return false;
}
bool    DAQEvent::_istdr(int16u id){
  if( ((id>>5)&((1<<9)-1))>=282 && ((id>>5)&((1<<9)-1))<=473 )return true;
  else return false;
}
bool    DAQEvent::_isedr(int16u id){
  if( ((id>>5)&((1<<9)-1))>=206 && ((id>>5)&((1<<9)-1))<=241 )return true;
  else return false;
}
bool    DAQEvent::_isudr(int16u id){
  if( ((id>>5)&((1<<9)-1))>=474 && ((id>>5)&((1<<9)-1))<=509 )return true;
  else return false;
}

bool   DAQEvent::_isrdr(int16u id){  // cpied from udr
  if( ((id>>5)&((1<<9)-1))>=242 && ((id>>5)&((1<<9)-1))<=265 )return true;
  else return false;
}

bool   DAQEvent::_iscceb(int16u id){  // cpied from udr
  if( ((id>>5)&((1<<9)-1))>=28 && ((id>>5)&((1<<9)-1))<=31 ){
    return true;
  }
  else return false;
}

bool   DAQEvent::_istofst(int16u id){  // cpied from udr
  if( ((id>>5)&((1<<9)-1))>=40 && ((id>>5)&((1<<9)-1))<=47 && ((id&0x1F)==24 || (id&0x1F)==25) ){
    //cout<<"<<--- In istofst, id="<<hex<<id<<" nadr(dec)="<<dec<<((id>>5)&((1<<9)-1))<<" dtyp(h/d)="<<hex<<(id&0x1F)<<" "<<dec<<(id&0x1F)<<endl;
    return true;
  }
  else return false;
}


bool    DAQEvent::_isjlvl1(int16u id){
  if(  ((id>>5)&((1<<9)-1))>=136 && ((id>>5)&((1<<9)-1))<=139 && (id>>14)==2)return true;
  else return false;
}

uinteger DAQEvent::_GetBlType(){
  uinteger type=(*(_pData+_cll(_pData)))&31;
  if(type==31){
    type=(*(_pData+_cll(_pData)+1));
  }
  //cout <<" type "<<*(_pData+_cl(_pData))<<" "<<_cll(_pData)<<" "<<type<<endl;
  if(type==5)return 0;
  else if(type==0)return 5;
  else return type;
}

integer DAQEvent::_cll(int16u *pdata){
  const  int16u lmask=0x8000; 
  if(pdata){
    return (*pdata) & lmask?2:1;
  }
  return 0;
}

integer DAQEvent::_cltype(int16u *pdata){
  const int16u rtype=0x1f;
  return  ((*pdata)&rtype)==rtype?2:1;
}

uinteger DAQEvent::_clll(int16u *pdata){
  //  return if event seq is present in jinj block
  //
  const  int16u lmask=0x8000;
  const int16u rtype=0x1f;
  const int16u rtypec[3]={0x13,0x14,0x1b};
  integer nodetype=1;
  if(pdata && _cl(pdata)>4){
    if (*pdata & lmask){
      nodetype=2;
    }
    if((pdata[nodetype]&rtype) == rtypec[0]  ){
      return 0; 
    }
    else return 1; 
  }
  else return 0;
}


integer DAQEvent::getpreset(int16u *pdata){
  const  int16u lmask=0x8000; 
  const int16u rtype=0x1f;
  const int16u rtypec[3]={0x13,0x14,0x1b};
  integer offset=5;
  integer nodetype=1;
  if(pdata && _cl(pdata)>4){
    if (*pdata & lmask){
      offset++;
      nodetype=2;
    }
    if((pdata[nodetype]&rtype) == rtype ){
      offset++;
    }
    else if((pdata[nodetype]&rtype) == rtypec[0] || (pdata[nodetype]&rtype) == rtypec[1]|| (pdata[nodetype]&rtype) == rtypec[2]  ){
      offset=0;
    } 
    return offset;
  }
  else return 0;

}

bool DAQEvent::_ComposedBlock(){
  return _GetBlType()==0 || _GetBlType()==6 || _GetBlType()==7 || _GetBlType()==8 ; 
}
integer DAQEvent::_EventOK(){
#ifdef __AMS02DAQ__
  if(_GetBlType()==0 && AMSJob::gethead()->isRealData())_HeaderOK();
  if((!_ComposedBlock() && _GetBlType()!= 0x14  && _GetBlType()!= 0x13 &&_GetBlType()!= 0x1b && _GetBlType()!= 896) || !(DAQCFFKEY.BTypeInDAQ[0]<=_GetBlType() && DAQCFFKEY.BTypeInDAQ[1]>=_GetBlType()))return 0;
  int preset=getpreset(_pData); 
  int ntot=0;
  //  if(!_ComposedBlock())return 1;
  if(_Length >1 && _pData ){
    integer ntot=preset;
    for(_pcur=_pData+preset;_pcur<_pData+_Length && _pcur>=_pData;_pcur+=_cl(_pcur)) {
      ntot+=_cl(_pcur);
    }
    if(ntot != _Length)goto wrong;
 

    ntot=preset;
    for(_pcur=_pData+preset;_pcur<_pData+_Length && _pcur>=_pData;_pcur+=_cl(_pcur)) {
      ntot+=_cl(_pcur);
      if(_isddg(*(_pcur+_cll(_pcur))) && _iscompressed(*(_pcur+_cll(_pcur)))){
	//
	//    Decompress
	// 
	//         cout <<"**** compressed structure found ***"<<endl;
	uint l2u=_clb(_pcur)-2;
	uint l2c=DAQCompress::decompressable((unsigned short *)(_pcur+_cll(_pcur)+1), l2u, _Run);
	if(l2c){
          uinteger bl=(_pcur-_pData-_cll(_pData))*sizeof(_pData[0]);
          uinteger cl=(_pcur+_cl(_pcur)-_pData-_cll(_pData));
          integer al=(_pData+_Length-_pcur-_cl(_pcur))*sizeof(_pData[0]);
          if(al<0){
            cerr<<" DAQEvent::_EVentOK-LogicError "<<al<<endl;
          }
          int add=2*sizeof(_pData[0]);
          if(l2c+sizeof(_pData[0])>32766){
	    add+=sizeof(_pData[0]);
          }
          Bytef ostream[l2c+al+add+bl];
          memcpy(ostream,(char*)(_pData+_cll(_pData)),bl);
	  if(DAQCompress::decompress((unsigned short *)(_pcur+_cll(_pcur)+1), l2u, (unsigned short *)(ostream+bl+add), l2c)){
            memcpy(ostream+bl+add+l2c,(char*)(_pcur+_cl(_pcur)),al);
            uint16 smb;
            smb=*(_pcur+_cll(_pcur));
            smb-=3;

            memcpy(ostream+bl+add-sizeof(_pData[0]),(char*)(&smb),sizeof(smb));

            if(l2c+sizeof(_pData[0])<32767){
              uint16 slb=l2c+sizeof(_pData[0]);
              memcpy(ostream+bl,(char*)(&slb),sizeof(slb));
            }
            else{
              uint16 slb[2]={0,0};
              slb[0]=((l2c+sizeof(_pData[0]))>>16)&32767;
              slb[0]|=(1<<15);
              slb[1]= (l2c+sizeof(_pData[0]))&65535;
              memcpy(ostream+bl+add,(char*)(slb),sizeof(slb));
            }
            
	    if(l2c+al+add+bl<32767)_Length=1;
	    else _Length=2;
	    _Length+=(l2c+al+add+bl+1)/sizeof(_pData[0]);
	    _create(_GetBlType());
	    memcpy((char*)(_pData+((l2c+al+add+bl<32767)?1:2)),ostream,l2c+al+add+bl);
	    if(l2c+al+add+bl>=32767){
	      _pData[0]=((l2c+al+add+bl)>>16)&32767;
	      _pData[0]|=(1<<15);
	      _pData[1]=(l2c+al+add+bl)&65535;
	    }
	    else{
	      _pData[0]=(l2c+al+add+bl);
	    }  
	    if(_EventOK()){
	      return 1;
	    }
	    else {
	      return 0;
	    }             
        
	  }
       
	}
      }
    }
  wrong:
    if(ntot != _Length){            
      cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
	_Length<<" Blocks say length is "<<ntot<<" "<<_GetBlType()<<endl;
      cerr <<" SubBlock dump follows"<<endl;
      _pcur=_pData+preset;
      for(_pcur=_pData+preset;_pcur<_pData+_Length&& _pcur>=_pData;_pcur+=_cl(_pcur))
	cerr <<" ID " <<*(_pcur+1)<<" Length "<< _cl(_pcur)<<endl;
      if(!fbin.good()){
        cerr<<" DAQEvent::_Eventok-S-HardError,CouldNotReadFile"<<endl;
      }
      if(fbin.eof()){
        cerr<<"DAQEvent::_Eventok-W-UnexpectedEOF"<<endl;
      }
      return 0;
    }
    else{
      if( (*(_pData+preset-3)) & (1<<14) && !AMSJob::gethead()->isRealData()){
	uint l2u=_clb(_pData+preset);
	uint l2c=_decompressable((Bytef*)(_pData+preset+_cll(_pData+preset)),l2u);
	//cout<<"  decompress "<<l2u<<" "<<l2c<<endl;
	if(l2c){
          Bytef ostream[l2c+preset*sizeof(_pData[0])];
          memcpy(ostream,(char*)_pData,preset*sizeof(_pData[0])); 
          if(_decompress((Bytef*)(_pData+preset+_cll(_pData+preset)),l2u,ostream+preset*sizeof(_pData[0]),l2c)){
	    //cout <<"  decompressed!!! "<<endl;
	    uint lhc=_cll(_pData+preset);
	    _Length+=(l2c-l2u)/sizeof(_pData[0])-lhc;
	    _create(_GetBlType());
	    memcpy((char*)(_pData),ostream,l2c+preset*sizeof(_pData[0]));
	    if(_cll(_pData)>1){
	      _pData[0]=(((_Length-_cll(_pData))*sizeof(_pData[0]))>>16)&32767;
	      _pData[0]|=(1<<15);
	      _pData[1]=((_Length-_cll(_pData))*sizeof(_pData[0]))&65535;
	    }
	    else{
	      _pData[0]=(_Length-_OffsetL)*sizeof(_pData[0]);
	    }  
	    *(_pData+preset-3)= (*(_pData+preset-3)) & (~(1<<14));
	    if(_EventOK()){
	      return 1;
	    }
	    else {
	      return 0;
	    }             
          }
	}
      }
      else  if( (*(_pData+preset-3)) & (1<<13) &&!AMSJob::gethead()->isRealData()){
	//
	//       zip bit found
	//



	// Guess event length;
	uint l2u=_clb(_pData+preset);
	uint l2c=2*l2u;
      again:
	l2c+=l2u;
	if(l2c>0xfffff){
	  static int ziperr=0;
	  if(ziperr++<10)cerr<<"DAQEvent::EventOK-W-LengthTooBigWillNotBeUnCompressed "<<l2c<<" "<<_Length<<endl;
	  return 0;
	}
	char tgt[l2c+preset*sizeof(_pData[0])];
	memcpy(tgt,(char*)_pData,preset*sizeof(_pData[0])); 
	z_stream stream; /* decompression stream */
	int err = 0;

	stream.next_in   = (Bytef*)((char*)(_pData+preset+_cll(_pData+preset)));
	stream.avail_in  = l2u;
	stream.next_out  = (Bytef*)(tgt+preset*sizeof(_pData[0]));
	stream.avail_out = sizeof(tgt)-preset*sizeof(_pData[0]);
	stream.zalloc    = (alloc_func)0;
	stream.zfree     = (free_func)0;
	stream.opaque    = (voidpf)0;

	err = inflateInit(&stream);
	if (err != Z_OK) {
	  fprintf(stderr,"R__unzip: error %d in inflateInit (zlib)\n",err);
	  return 0;
	}

	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
	  inflateEnd(&stream);
	  fprintf(stderr,"R__unzip: error %d in inflate (zlib)\n",err);
	  goto again;
	}

	inflateEnd(&stream);

	l2c = stream.total_out;
	uint lhc=_cll(_pData+preset);
	_Length+=(l2c-l2u)/sizeof(_pData[0])-lhc;
	_create(_GetBlType());
	memcpy((char*)(_pData),tgt,l2c+preset*sizeof(_pData[0]));
	if(_cll(_pData)>1){
	  _pData[0]=(((_Length-_cll(_pData))*sizeof(_pData[0]))>>16)&32767;
	  _pData[0]|=(1<<15);
	  _pData[1]=((_Length-_cll(_pData))*sizeof(_pData[0]))&65535;
	}
	else{
	  _pData[0]=(_Length-_OffsetL)*sizeof(_pData[0]);
	}  
	*(_pData+preset-3)= (*(_pData+preset-3)) & (~(1<<13));
	if(_EventOK())return 1;
	else return 0;
      }
      return 1;    
    }
  }
  return 0;

#else  
  const int tofidL = 0x1400;
  const int tofidR = 0x15C1;

  const int trkidL = 0x1680;
  const int trkidR = 0x174C;

  integer tofL = 0;
  integer trkL = 0;

  if(_Length >1 && _pData ){
    if( (_pData[1] & ~63)<<3 ==0){
      // envelop header
      integer ntot=0;
      _pcur=_pData+2;
      for(_pcur=_pData+2;_pcur<_pData+_Length && _pcur>=_pData;_pcur+=*(_pcur)+_OffsetL) {
	ntot+=*(_pcur)+_OffsetL;
	if (AMSJob::gethead() && AMSJob::gethead() -> isMonitoring()) {
	  int l   = *(_pcur) + _OffsetL;
	  int bid = *(_pcur+1);
	  int aid = bid;
	  int tofid = bid & 0x1400;
	  if (tofid == 0x1400) aid = aid &~60;
	  int hid    = 300000 + aid; 
	  if (aid == 0x200 ||  aid == 0x0440){
#pragma omp critical (hf1)
	    {
	      HF1(hid,l,1.);
	    }
	  }
	  if (aid >= tofidL && aid<=tofidR) {
#pragma omp critical (hf1)
	    {
	      HF1(hid,l,1.);
	    }
	    tofL = tofL + l;
	  }
	  if (aid == 0x1680 || aid == 0x1740 ||
	      aid == 0x1681 || aid == 0x1741 ||
	      aid == 0x168C || aid == 0x174C) {
	    trkL = trkL + l;
#pragma omp critical (hf1)
	    {
              HF1(hid,l,1.);
	    }
	  }
	}
      }
      if (AMSJob::gethead() && AMSJob::gethead()->isMonitoring()) {
#pragma omp critical (hf1)
	{
	  HF1(300000,_Length,1.);
	  if (tofL) HF1(300001,tofL,1.);
	  if (trkL) HF1(300002,trkL,1.);
	}
      }
      if(ntot != _Length-2){
	cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
	  _Length<<" Blocks say length is "<<ntot+2<<endl;
	cerr <<" SubBlock dump follows"<<endl;
	_pcur=_pData+2;
	for(_pcur=_pData+2;_pcur<_pData+_Length && _pcur>=_pData;_pcur+=*(_pcur)+_OffsetL)
	  cerr <<" ID " <<*(_pcur+1)<<" Length "<< *(_pcur)+_OffsetL<<endl;
	//     int ic=0;
	//     for(_pcur=_pData;_pcur<_pData+_Length;_pcur++)
	//       cout <<ic++ <<" "<<*(_pcur)<<endl;
	if(!fbin.good()){
	  cerr<<" DAQEvent::_Eventok-S-HardError,CouldNotReadFile"<<endl;
	}
	if(fbin.eof()){
	  cerr<<"DAQEvent::_Eventok-W-UnexpectedEOF"<<endl;
	}
	return 0;
      }
      else return 1;    
    }
    else {
      cout <<"DAQEvent::_EventOK-I-NonEventBlockFound-ID "<<_pData[1]<<" Length "<<
        _Length<<endl;
      return -1;
    }
  }
  else return 0;
#endif
}
bool DAQEvent::CalibRequested(unsigned int crate,unsigned int xdr){
  if(crate<sizeof(_CalibData)/sizeof(_CalibData[0])){
    return (_CalibData[crate] & (1<<xdr))!=0;
  }
  else return false;
}
integer DAQEvent::_HeaderOK(){
  const integer Laser=204;
  static int lr=-1;
  if(!_ComposedBlock() && _GetBlType()!= 0x1b  &&_GetBlType()!=896)return 0;
  for(_pcur=_pData+getpreset(_pData);_pcur < _pData+_Length;_pcur+=_cl(_pcur)){
    _Time=(*(_pcur-1)) |  (*(_pcur-2))<<16;
    if(!_ComposedBlock()){
      _Time=(*(_pcur+4)) |  (*(_pcur+3))<<16;
      static int event=0;
      static int run=0;
      if(event==0)run=_Time;
      _Event=++event;
      _Run=run;
      _RunType=_GetBlType();
      const uinteger _OffsetT=0x12d53d80;
      //_Time+=_OffsetT;
    }
    if(AMSEvent::checkdaqid(*(_pcur+_cll(_pcur)))){
      AMSEvent::buildraw(_cl(_pcur)-1,_pcur+1, _Run,_Event,_RunType,_Time,_usec);
      if(_Run>=1208965123 || !AMSJob::gethead()->isRealData())TRCALIB.Version=1;
      else TRCALIB.Version=0;
      DAQCFFKEY.DAQVersion=0;
      if(_Run>=1240000000 && AMSJob::gethead()->isRealData())DAQCFFKEY.DAQVersion=1;  
      if(_RunType%256==Laser && TRCALIB.LaserRun==0){
	TRCALIB.LaserRun=22;
	cout<<"DAQEvent::_HeaderOK-I-LaserRunDetected "<<endl;
      }
      else if(_RunType&256!=Laser && TRCALIB.LaserRun==22){
	cout<<"DAQEvent::_HeaderOK-I-NormalRunDetected "<<endl;
	TRCALIB.LaserRun=0;
      }
      if(TRCALIB.LaserRun){
        MAGSFFKEY.magstat=0;
      }
      static int nmsg=0;
      int shift=0;
      if((*(_pcur+12)) & (1<<15)){
	if(nmsg++<100){
          cout<< "DAQEVent::_HeadrOK-I-GPSInfoFound "<<endl;
	}
	shift=5;
      }
      int mask= ((*(_pcur+12)>>6) & 15);
      _setcalibdata(mask);//reset(to 0, if mask>0)/preset(to FFFFFFFF) _CalibData[i]-array
      if(mask){
	//exrtact JINJ slaves mask:
	int16u w1=*(_pcur+12+shift+1);//JINJ slave mask label+msb
	int16u w2=*(_pcur+12+shift+2);//................lsb
	uinteger _JinjSlaveMask(0);
	if(((w1>>8)&255)==0)_JinjSlaveMask=(((w1&255)<<16) | w2);//include SDRs as ports !!!
	//fill _CalibData[portj] for SDRs only(has no slaves):
	for(int i=0;i<sizeof(_CalibData)/sizeof(_CalibData[0]);i++){
	  if((_JinjSlaveMask&(1<<i))>0){
	    if(strstr(_PortNamesJ[i],"SDR-")!=0)_CalibData[i]=1;
	  }
	}
	//fill the rest(slaves of JINJ-slaves):
	for(int16u* pmask=_pcur+shift+12+3;pmask<_pcur+_cl(_pcur);pmask+=2){//SDRs are not set here!!!
	  int16u portj=_getportj(*pmask>>8);
	  _CalibData[portj]=(((*pmask)&255)<<16) | *(pmask+1);        
	}
      }
      _Checked=1;
#ifdef __AMSDEBUG__
      cout << "Run "<<_Run<<" Event "<<_Event<<" RunType "<<_RunType<<endl;
      cout <<ctime(&_Time)<<" usec "<<_usec<<endl;
#endif
      time_t tmin=1180000000;
      time_t tmax=1980000000;
      static int rep=0;
      static int time=_Time;
      if(_Time<tmin ){

	if(rep++<100)cerr <<"DAQEvent::_HeaderOK-E-TimeProblems-Resetting "<<_Time<<" "<<ctime(&_Time)<<endl;
	if(TRMFFKEY.TIME==0){
	  _Time=tmin;
	}
	else _Time+=TRMFFKEY.TIME-time;
      }
      if(_Time>tmax){
	if(rep++<100)cerr <<"DAQEvent::_HeaderOK-E-TimeProblems-Resetting "<<ctime(&_Time)<<endl;
	const uinteger _OffsetT=0x12d53d80;
	_Time-=_OffsetT;
      }
 
      // fix against event 0
      if(_Event==0 && _GetBlType()==0)return 0;


      //  fix against broken sequence
      if(_PRun==_Run && _PEvent && _Event<_PEvent){
	cerr<<"DAQEvent::Headerok-E-EventSeqBroken for Run "<<_PRun<<" "<<_PEvent<<" "<<_Event<<endl;
	_PRun=_Run;
	_PEvent=_Event;
	return 0;
      }
      if(_Event-_PEvent>1 && AMSJob::gethead() && AMSJob::gethead()->isRealData()){
	static int icerr=0;
	if(icerr++<100)cerr<<"DAQEvent::_HeaderOK-W-EventSequenceGap "<<_Run<<" "<<_Event<<" "<<_PEvent<<endl;
      }
      _PRun=_Run;
      _PEvent=_Event;
      return 1;
    }


    else if(int stype=AMSEvent::checkdaqid2009(*(_pcur+_cll(_pcur)))){
      //      AMSEvent::buildraw2009(_cl(_pcur)-1,_pcur+1, _Run,_Event,_RunType,_Time,_usec);
      AMSEvent::buildraw2009(stype,_pcur+1, _Run,_Event,_RunType,_Time,_usec,_lvl3,_gps,_gps_count,_gpsl);
      TRCALIB.Version=1;
      DAQCFFKEY.DAQVersion=0;
      if(AMSJob::gethead()->isRealData())DAQCFFKEY.DAQVersion=1;  
      //
      /*

      laser don;t know yet

      if(_RunType%256==Laser && TRCALIB.LaserRun==0){
      TRCALIB.LaserRun=22;
      cout<<"DAQEvent::_HeaderOK-I-LaserRunDetected "<<endl;
      }
      else if(_RunType%256!=Laser && TRCALIB.LaserRun==22){
      cout<<"DAQEvent::_HeaderOK-I-NormalRunDetected "<<endl;
      TRCALIB.LaserRun=0;
      }
      */
      if(_GetBlType()>=6 && _GetBlType()<=8){
#pragma omp critical (calibdata)
	{      

	  //     set commands here
	  int mask=(_Event & 65535);
	  int seq=(_Event>>16);
	  _Event=seq;
	  if(seq==1)_setcalibdataS(_Run);
	  // try to be smart
	  int16u id=*(_pcur+_cl(_pcur)+_cll(_pcur+_cl(_pcur)));
	  int pos=-1;
	  if(_istdr(id)){
	    pos=0;
	  }
	  else if(_isedr(id)){
	    pos=4;
	  }
	  else if(_isudr(id)){
	    pos=1;
	  }
	  else if(_isrdr(id)){
	    pos=3;
	  }
	  else if(_issdr(id)){
	    pos=2;
	  }
	  if(pos>=0){
	    if(_CalibDataS[pos*3+2]++==0)_CalibDataS[pos*3]=seq;
	    _CalibDataS[pos*3+1]=seq;          
	    if(seq==mask && _Run==_CalibDataS[sizeof(_CalibDataS)/sizeof(_CalibDataS[0])-1]){
	      _CalibDataS[pos*3+2]++;
	    }
	    else{
	      // must read next event, thanks to developers
	      {
		if(fbin.good() && !fbin.eof()){
		  int16u l16[2];
		  uint64 of=getsoffset();
		  fbin.read(( char*)(l16),sizeof(l16));
		  _convertl(l16[0]);
		  _convertl(l16[1]);
		  int length=_cl(l16);
		  if(length>255)length=256;
		  int16u buf[256];
		  setoffset(of);
		  fbin.read((char*)buf,length);
		  setoffset(of);
		  int16u *pp=buf;
		  if((*(pp+_cll(pp)) &31) >=6 && (*(pp+_cll(pp))&31)<=8){
		    int shift=4+_cll(pp)+_cl(pp+4+_cll(pp))+_cll(pp+4+_cll(pp)+_cl(pp+4+_cll(pp)));
		    if(shift<length){
		      id=*(pp+shift);
		      int pos2=-1;
		      if(_istdr(id)){
			pos2=0;
		      }
		      else if(_isedr(id)){
			pos2=4;
		      }
		      else if(_isudr(id)){
			pos2=1;
		      }
		      else if(_isrdr(id)){
			pos2=3;
		      }
		      else if(_issdr(id)){
			pos2=2;
		      }
		      if(pos2>=0 && pos2!=pos &&_Run==_CalibDataS[sizeof(_CalibDataS)/sizeof(_CalibDataS[0])-1])_CalibDataS[pos*3+2]++;
		    }
		  }
		}
	      }
	    }
	  }
	} 
      }

      // level3 now inside  (should add smth here)


      // gps now inside  (should add smth here)


      _Checked=1;
#ifdef __DEBUGFLAG__
      cout << "Run "<<_Run<<" Event "<<_Event<<" RunType "<<_RunType<<endl;
      cout <<ctime(&_Time)<<" usec "<<_usec<<endl;
      if(_Event==246){
	cout << " priehali "<<endl;
      }
#endif
      // fix against wrong time
      if(_Time<1180000000){
	static int  bbte=0;
	if(bbte++<100)cerr<<"DAQEvent::_HeaderOK-E-BadTime "<<_Time<<" "<<_Run<<" "<<_Event<<endl;
	return 0;
      }
     
      // fix against event 0

      if(_Event==0 && _GetBlType()==0)return 0;

      //  fix against broken sequence
      if(_PRun==_Run && _PEvent && _Event<_PEvent){
	if(_GetBlType()==5){
	  cerr<<"DAQEvent::Headerok-E-EventSeqBroken for Run "<<_PRun<<" "<<_PEvent<<" "<<_Event<<endl;
	  _PRun=_Run;
	  _PEvent=_Event;
	  return 0;
        }
        else  if(_GetBlType()>=6 && _GetBlType()<=8){
	  _Event=_PEvent+1;
	}
      }
      if(_Event-_PEvent>1){
	static int icerr=0;
	if(icerr++<100)cerr<<"DAQEvent::_HeaderOK-W-EventSequenceGap "<<_Run<<" "<<_Event<<" "<<_PEvent<<endl;
      }

      _PRun=_Run;
      _PEvent=_Event;
      //return 0;
            
      return 1;
    }



    // new checkdaqid
    


  }
  cerr<<"DAQEvent::_HeaderOK-W-NoHeaderinEvent Type "<<_pData[1]<<" "<<((_pData[1]>>5)&511)<<" "<<_GetBlType()<<" "<<_Time<<endl;
  _Checked=1;
  if(_GetBlType()==0x1b || _GetBlType()==896)return 1;
  else return 0;
}


integer DAQEvent::_DDGSBOK(){
  const integer Laser=204;
  for(_pcur=_pData+getpreset(_pData);_pcur < _pData+_Length;_pcur+=_cl(_pcur)){

#ifdef __TRDOFFLINE__
    if(*(_pcur+_cll(_pcur)) == 0x8001) *(_pcur+_cll(_pcur))=0x98C1; // U0-EPP-id 0x8001 -> JINF-U0A-id 0x98C1

    if(*(_pcur+_cll(_pcur)) == 0x8021) *(_pcur+_cll(_pcur))=0x9941; // U1-EPP-id 0x8021 -> JINF-U1A-id 0x9941
#endif

    if(_isddg(*(_pcur+_cll(_pcur)))){
      if( calculate_CRC16(_pcur+_cll(_pcur)+1,_cl(_pcur)-1-_cll(_pcur))){
	cerr<<"DAQEvent::_DDGSBOK-E-CRCError "<<endl;
	return 0;
      }
      static int nprint=0;
      int ok=lv3.lv3_unpack(_pcur,_Event,AMSJob::gethead()->isRealData()?false:true);
      if(_isjinj(*(_pcur+_cll(_pcur)))){
	int16u event=*(_pcur+_cll(_pcur)+1);
	if(event !=  (_Event&((1<<16)-1)) && nprint++<100 ){
	  int ee= (_Event&((1<<16)-1)); 
	  cerr<<"DAQEvent::_DDGSBOK-E-EventNoMismatch  Header says event 16 lsb is "<<ee<<" DDGSB says it is  "<<event<<endl;
	  //return 0;
	}
	int ntot=0;
	for(int16u *p=_pcur+_cll(_pcur)+1+_clll(_pcur);p<_pcur+_cl(_pcur)-2;p+=*p+1){
	  ntot+=*p+1;
	  int16u port=_getportj(*(p+*p));
#ifdef __AMSDEBUG__
	  int16u status=(*(p+*p))>>5;
	  cout <<"  JINJ Port "<<port <<" "<<_PortNamesJ[port]<<"  Length "<<*p<<" Status "<<status<<endl;
#endif
	  switch(port){
	  case 0:
	  case 1:
	  case 3:
	  case 9:
	  case 16:
	  case 17:
	  case 22:
	  case 23:
	    _SubLength[0]+=*p;
	    _SubCount[0]++;
	    break;
	  case 2:
	  case 8:
	    _SubLength[1]+=*p;
	    _SubCount[1]++;
	    break;
	  case 4:
	  case 5:
	  case 6:
	  case 7:
	  case 18:
	  case 19:
	  case 20:
	  case 21:
	    _SubLength[2]+=*p;
	    _SubCount[2]++;
	    break;
	  case 10:
	  case 11:
	    _SubLength[3]+=*p;
	    _SubCount[3]++;
	    break;
	  case 12:
	  case 13:
	    _SubLength[4]+=*p;
	    _SubCount[4]++;
	    break;
	  case 14:
	  case 15:
	    _SubLength[5]+=*p;
	    _SubCount[5]++;
	    break;
	  }

       
	}
	if(ntot !=_cl(_pcur)-2-1-1-_cll(_pcur)){
	  cerr<<"DAQEvent::_DDGSBOK-E-LengthMismatch Event says block length is "<<_cl(_pcur)-2-1-1-_cll(_pcur)<<" Block says it is "<<ntot<<endl;
	  return 0;
	}
	int maxl[8]={8,2,8,2,2,2,2,2};
	//  tracker trd tof rich ecal lvl1 lvl3;
	static int nerr=0;
	for(int i=0;i<6;i++){
	  if(getsubcount(i)>maxl[i]){
	    if(nerr++<100)cerr<<"DAQEvent::_DDGSBOK-W-TooManyBlocksFor "<<i<<" "<<getsubcount(i)<<endl;
	  }
	}
      }
      else if(_isjlvl1(*(_pcur+_cll(_pcur)))){
	int16u event=*(_pcur+_cll(_pcur)+1);
	if(event !=  (_Event&((1<<16)-1)) && nprint++<100 ){
	  cerr<<"DAQEvent::_DDGSBOK-E-EventNoMismatch  Header says event 16 lsb is "<<(_Event&((1<<16)-1))<<" DDGSB says it is  "<<event<<endl;;
	  //return 0;
	}
#ifdef __AMSDEBUG__
	int16u status=(*(_pcur+_cl(_pcur))-1)>>5;
	cout <<"  JLV1 Port "<<_getnodename(*(_pcur+_cll(_pcur))) <<" "<<"  Length "<<_cl(_pcur)<<" Status "<<status<<endl;
#endif
      }
      else if(_isjinf(*(_pcur+_cll(_pcur)))){
	int16u event=*(_pcur+_cll(_pcur)+1);
	if(event !=  (_Event&((1<<16)-1)) && nprint++<100){
	  cerr<<"DAQEvent::_DDGSBOK-E-EventNoMismatch  Header says event 16 lsb is "<<(_Event&((1<<16)-1))<<" DDGSB says it is  "<<event<<endl;;
	  //return 0;
	}
#ifdef __AMSDEBUG__
	int16u status=(*(_pcur+_cl(_pcur))-1)>>5;
	cout <<"  JINF Port "<<_getnodename(*(_pcur+_cll(_pcur))) <<" "<<"  Length "<<_cl(_pcur)<<" Status "<<status<<endl;
#endif
	int add2=0;
	if(DAQCFFKEY.DAQVersion==1)add2=2;
	int ntot=add2;
	for(int16u *p=_pcur+_cll(_pcur)+2;p<_pcur+_cl(_pcur)-2-add2;p+=*p+1){
	  ntot+=*p+1;
	  int16u port=_getportj(*(p+*p));
#ifdef __AMSDEBUG__
	  int16u status=(*(p+*p))>>5;
	  cout <<"  JINF Port "<<port <<"  Length "<<*p<<" Status "<<status<<endl;
#endif
	}
	if(ntot !=_cl(_pcur)-2-1-1-_cll(_pcur)){
	  cerr<<"DAQEvent::_DDGSBOK-E-LengthMismatch Event says block length is "<<_cl(_pcur)-2-1-1-_cll(_pcur)<<" Block says it is "<<ntot<<endl;
	  return 0;
	}


      }
      else if(!_iscceb(*(_pcur+_cll(_pcur))) || !_istofst(*(_pcur+_cll(_pcur)))){
        cerr<<"DAQEvent::_DDGSBOK-E-XDRModeNotYetImplemented "<<_getnodename(*(_pcur+_cll(_pcur)))<<endl;
        return 0;
      }
    }
  }
  return 1;
}


  


void DAQEvent::buildRawStructures(){
#ifdef __AMS02DAQ__


  if((_Checked ||(_EventOK()==1 && _HeaderOK())) && _DDGSBOK() ){


    if(_Buffer2Lock ){
      int16u *pd=_Buffer2;
      if(((*(pd+_cll(pd)))&31)==5){
	int16u* pc;
	for(pc=pd+getpreset(pd);pc < pd+_Length2;pc=pc+_cl(pc)){
	  int16u id=*(pc+_cll(pc));
	  if(_isjinj(id)){
	    for(int16u * pdown=pc+_cll(pc)+1+_clll(pc);pdown<pc+_cl(pc)-2 && pdown>=pc;pdown+=*pdown+1){
	      int ic=DAQS2Block::checkblockid(_getportj(*(pdown+*pdown)))-1;
	      if(ic>=0){
		int16u *psafe=pdown+1;
		integer n=((ic)<<16) | (*pdown) | (1<<30);
		DAQS2Block::buildraw(n,psafe);
	      }
	    }
	  }
	}
	int sta;
	TOF2RawSide::validate(sta,1);
      }
    }



    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      for(_pcur=_pData+getpreset(_pData);_pcur < _pData+_Length && _pcur>=_pData;_pcur=_pcur+_cl(_pcur)){
	int16u id=*(_pcur+_cll(_pcur));
	int jinj=_isjinj(id);
	if(jinj){
 	  uinteger size=((*_pcur)&0x8000)?(*(_pcur+1)|(((*_pcur)&0x7FFF)<<16))+sizeof(_pcur[0]): *_pcur;
	  if(size%2==1) size++;
	  ushort JinjStatus=(*(_pcur+size/2-1));
	  if(jinj== 0x080)      _JStatus[0]=JinjStatus;
	  else if(jinj== 0x081) _JStatus[1]=JinjStatus;
	  else if(jinj== 0x082) _JStatus[2]=JinjStatus;
	  else if(jinj== 0x083) _JStatus[3]=JinjStatus;
//
// 	  if(jinj== 0x81 | jinj== 0x82)
// 	    _JStatus|=(JinjStatus>>8);
// 	  else 
// 	    _JStatus|=(JinjStatus&0xFF00);

	  for(int16u * pdown=_pcur+_cll(_pcur)+1+_clll(_pcur);pdown<_pcur+_cl(_pcur)-2&& pdown>=_pcur &&pdown<_pData+_Length;pdown+=*pdown+1){
	    int ic=fpl->_pgetid(_getportj(*(pdown+*pdown)))-1;

	    if(ic>=0){
#ifdef __AMSDEBUG__
	      cout <<" getportj "<<_getportj(*(pdown+*pdown))<<" "<<_getportnamej(*(pdown+*pdown))<<" "<<*pdown<<"  Error "<<isError(*(pdown+*pdown))<<endl;
#endif
	      if(_getportj(*(pdown+*pdown))<sizeof(_JError)/sizeof(_JError[0])){
		_JError[_getportj(*(pdown+*pdown))]=(*(pdown+*pdown))>>8;
	      }
	      
	      int16u *psafe=pdown+1;
	      
	      integer n=(ic<<16) | (*pdown);
	      if(DAQRichBlock::checkdaqid(_getportj(*(pdown+*pdown)))){
		n|=(jinj-128)<<24;
	      }
	      fpl->_pputdata(n,psafe);
	    }
	  }
	}
	else if(_isjlvl1(id) ){
	  if(fpl->_pgetid(id)){
	    int16u *pdown=_pcur+_cll(_pcur)+2;
	    int16u *psafe=pdown;
	    fpl->_pputdata(_cl(_pcur)-_cll(_pcur)-2-1,psafe);
	  }
	}
	else if(_isjinf(id)){
	  int ic=fpl->_pgetid(id)-1;
	  if(ic>=0){
	    int16u *pdown=_pcur+_cll(_pcur)+_cltype(_pcur+_cll(_pcur));
	    int16u *psafe=pdown;
	    int n=(ic<<16) | (_cl(_pcur)-_cll(_pcur)-_cltype(_pcur+_cll(_pcur))-1);
	    fpl->_pputdata(n,psafe);
	  }
	}
	else if(_istdr(id)){
	  int ic=fpl->_pgetid(id)-1;
	  if(ic>=0){
	    int16u *pdown=_pcur+_cll(_pcur)+2;
	    int16u *psafe=pdown;
	    int n=(ic<<16) | (_cl(_pcur)-_cll(_pcur));
	    unsigned  run=*(_pData+9)<<16 | *(_pData+10); 
#ifdef _PGTRACK_
	    if(TrCalDB::Head) TrCalDB::Head->run=run;
#endif
	    fpl->_pputdata(n,psafe);
	  }
	}
	else if(_isudr(id) || _isrdr(id) || _isedr(id) || _issdr(id)|| _iscceb(id) || _istofst(id) ){
	  int ic=fpl->_pgetid(id)-1;
	  if(ic>=0){
	    int16u *pdown=_pcur+_cll(_pcur)+2;
	    int16u *psafe=pdown;
	    int n=(ic<<16) | (_cl(_pcur)-_cll(_pcur));
	    fpl->_pputdata(n,psafe);
	  }
	}  
	else if(_isddg(id)){    // normal data if any...
	  cerr <<"   not supported !!!! "<<endl;
	  if(fpl->_pgetid(*(_pcur+_cll(_pcur)))){
	    int16u *psafe=_pcur+_cll(_pcur);
	    fpl->_pputdata(_cl(_pcur)-1,psafe);
	  }
	}
      }
      fpl=fpl->_next; 
    }
  }
#else
  if(_Checked ||(_EventOK()==1 && (_HeaderOK()))){
    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
	if(fpl->_pgetid(*(_pcur+1))){
	  int16u *psafe=_pcur+1;
	  fpl->_pputdata(*_pcur+_OffsetL-1,psafe);
	}
      }
      fpl=fpl->_next; 
    }
  }
  else if(_EventOK()==-1){
    // without envelop only one block
    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      _pcur=_pData;
      if(fpl->_pgetid(*(_pcur+1))){
	fpl->_pputdata(*_pcur+_OffsetL-1,_pcur+1);
      }
      fpl=fpl->_next; 
    }
  }
#endif
}








void DAQEvent::buildRawStructuresEarly(){
  if((_Checked ||(_EventOK()==1 && _HeaderOK()))  ){
    for(_pcur=_pData+getpreset(_pData);_pcur < _pData+_Length;_pcur=_pcur+_cl(_pcur)){
      int16u id=*(_pcur+_cll(_pcur));
      if(_isjinj(id)){
	for(int16u * pdown=_pcur+_cll(_pcur)+1+_clll(_pcur);pdown<_pcur+_cl(_pcur)-2;pdown+=*pdown+1){
	  if(_getportj(*(pdown+*pdown))==14 || _getportj(*(pdown+*pdown))==15){
	    int16u *psafe=pdown+1;
	    int ic=_getportj(*(pdown+*pdown))-14;
	    integer n=((ic)<<16) | (*pdown);
	    _TrigTime=Trigger2LVL1::buildrawearly(n,psafe);
	    break;
	  }
	}
      }
      else if(0 && _iscceb(id)){
	int ic=AMSEvent::checkccebid(id)-1; 
	if(ic>=0){
	  int16u *pdown=_pcur+_cll(_pcur)+2;
	  int16u *psafe=pdown;
	  int n=(ic<<16) | (_cl(_pcur)-_cll(_pcur));
	  AMSEvent::buildcceb(n,psafe);
	}
      }
      else if(0 && _istofst(id)){
	int ic=AMSEvent::checktofstid(id)-1; 
	if(ic>=0){
	  int16u *pdown=_pcur+_cll(_pcur)+2;
	  int16u *psafe=pdown;
	  int n=(ic<<16) | (_cl(_pcur)-_cll(_pcur));
	  AMSEvent::buildtofst(n,psafe);
	}
      }
    }
  }

  // must read next event, thanks to developers
  _Buffer2Lock=0;
  if(DAQCFFKEY.ReadAhead){
    
    if(fbin.good() && !fbin.eof()){
      int16u l16[2];
      uint64 of=getsoffset();
      fbin.read(( char*)(l16),sizeof(l16));
      _convertl(l16[0]);
      _convertl(l16[1]);
      int length=_cl(l16);
      if(length>65535)length=65535;
      int16u buf[256];
      setoffset(of);
      fbin.read((char*)_Buffer2,length);
      setoffset(of);
      if(fbin.good() && !fbin.eof())_Buffer2Lock=1;
      else fbin.clear();
      _Length2=length;

    }
  }

}




void DAQEvent::write(){
  if(_Length && fbout.is_open()){
    _convert();
    fbout.write((char*)_pData,sizeof(_pData[0])*_Length);
    _NeventsO++;
    //  should flush output for each event
    //
    fbout.flush();
  }

}


void DAQEvent::setoffset(uint64 offset){
  if(fbin)fbin.seekg(offset);
}

uint64 DAQEvent::getsoffset(){
  if(fbin)return fbin.tellg();
  else return 0;
}

integer DAQEvent::read(){
  enum open_mode{binary=0x80};
  do{
    if(fbin.eof()){
      fbin.clear();
#ifdef __ALPHA__ 
      fbin.close();
#else
      if(fbin.is_open())fbin.close();
#endif
      integer Run,Event;
      char * fnam=_getNextFile(Run, Event);
      if(fnam){
	for(;;){
	  fbin.open(fnam,ios::in);
          _FileSize=0;
	  if(fbin){ 
            SetFileSize(fnam);
	    cout <<"DAQEvent::read-I-opened file "<<fnam<<endl;
	    GCFLAG.IEORUN=0;
	    break;
	  }    
	  else{
	    cerr<<"DAQEvent::read-F-cannot open file "<<fnam<<endl;
	    fbin.clear();
	    if((fnam=_getNextFile(Run, Event))==0)return 0;
	  }
	}
      }
    }
    if(fbin.good() && !fbin.eof()){
      int16u l16[2];
      int16u l16c[2];
      fbin.read(( char*)(l16),sizeof(l16));
      _convertl(l16[0]);
      _convertl(l16[1]);
      _LengthR=_Length=_cl(l16);
    unexpected:
      if(fbin.eof()){
	integer Run,Event;
	char * fnam=_getNextFile(Run, Event);
	if(fnam){
#ifdef __ALPHA__ 
	  fbin.close();
#else
	  fbin.close();
	  fbin.clear();       
	  if(fbin.is_open())fbin.close();
#endif
	  for(;;){
	    fbin.open(fnam,ios::in);
          _FileSize=0;
	    if(fbin){ 
              SetFileSize(fnam);
	      cout <<"DAQEvent::read-I-opened file "<<fnam<<endl;
	      GCFLAG.IEORUN=0;
	      fbin.read(( char*)(l16),sizeof(l16));
	      _convertl(l16[0]);
	      _convertl(l16[1]);
	      _LengthR=_Length=_cl(l16);
	      break;
	    }    
	    else{
	      fbin.clear();
	      cerr<<"DAQEvent::read-F-cannot open file "<<fnam<<endl;
	      if((fnam=_getNextFile(Run, Event))==0)return 0;
     
	    }
	  }
	}
	else break;
      }
      if(fbin.good() && !fbin.eof()){
	if(_create()){
	  //       fbin.seekg(integer(fbin.tellg())-2*sizeof(_pData[0]));
	  int off=-2*int(sizeof(_pData[0]));
	  fbin.seekg(off,ios::cur);
	  fbin.read(( char*)(_pData),sizeof(_pData[0])*(_Length));
	  _convert();
	}
	else{
	  //       fbin.seekg(integer(fbin.tellg())+sizeof(_pData[0])*(_Length-1));
	  unsigned int off=sizeof(_pData[0])*(_Length-1);
	  fbin.seekg(off,ios::cur);
	  _LengthR=_Length=0;
	}
      }
      else {
	goto unexpected;  
      }
    }
    else {
      break;
    }
  }while(_Length==0 || _EventOK()==0 || (_HeaderOK()==0 ));
  //
  // here we must put triggerlvl1 buildraw
  //   
  

        _NeventsPerRun++;
    if(fbin){
        _Offset=fbin.tellg();
//     cout <<" offset "<<_Offset<<" "<<getlengthR()<<" "<<_Offset-getlengthR()<<endl;
     _Offset-=getlengthR();
    }
    else {
    //cerr<<"DAQEvent::getoffset-E-fbinNotOPened"<<endl;
    _Offset=0;
  }
  if(fbin.good() && !fbin.eof())buildRawStructuresEarly();
  return fbin.good() && !fbin.eof();
}



void DAQEvent::select(){
  DAQEvent::_BufferLock=0;
  DAQEvent daq;
  int ok;
  while((ok=daq.read())){
    if(daq.runno() == SELECTFFKEY.Run &&
       daq.eventno() >= SELECTFFKEY.Event)break;
    daq.shrink();
  }
  // pos back if fbin.good
  if(ok){
    int off=-daq.getlengthR();
    fbin.seekg(off,ios::cur);
    static int nmsg=0;
    if(nmsg++<100)cout<<"DAQEvent::select-I-Selected Run = "<<daq.runno()<<
      " Event = "<<daq.eventno()<<" Time "<<ctime(&daq.time())<<endl;

  }
}

DAQEvent::InitResult DAQEvent::init(){
  enum open_mode{binary=0x80};
  integer Run,Event;
  int ntryscp=0;
  char * fnam=_getNextFile(Run, Event);
  if(!fnam)return UnableToFindRunEvent;

        string fnamei=fnam; 
  if(getenv("TMPRawFile") && getenv("NtupleDir") && strstr(getenv("TMPRawFile"),getenv("NtupleDir"))){
    unlink(getenv("TMPRawFile"));
    unsetenv("TMPRawFile");
  }
 castor:
  if(fnam){
#ifdef __ALPHA__ 
    fbin.close();
#else
    fbin.clear();
    if(fbin.is_open())fbin.close();
#endif
    fbin.open(fnam,ios::in);
          _FileSize=0;
    if(fbin){ 
      cout <<"DAQEvent::init-I-opened file "<<fnam<<endl;
      SetFileSize(fnam);
      GCFLAG.IEORUN=0;
      if(Run){
	DAQEvent daq;
	integer run=-1;
	integer ok=1;
	integer iposr=0;
	uinteger tfevent=0;
	uinteger tlevent=0;
	integer fevent=0;
	integer levent=0;
	while(ok){
	  if(GCFLAG.IEOTRI){
	    cerr<<"DAQEvent::init-F-Interrupt"<<endl;
	    return Interrupt;
	  }
	  fbin.clear();
	  ok=daq.read();
	  if(ok){
	    if(Run==-1){
	      SELECTFFKEY.Event+=1;
	    }

	    iposr++;
	    if (daq.runno() != run){
	      if(run>0){
		cout <<" DAQEvent::init-I-Run "<<run<<" events "<<SELECTFFKEY.Event-1<<" "<<fevent<<" "<<levent<<endl;
	      }
	      cout <<" DAQEvent::init-I-New Run "<<daq.runno()<<endl;
	      run=daq.runno();
	      Time_1 = 0;
	      fevent=daq.eventno();
	      tfevent=daq.time();
	      if(Run==-1){
		SELECTFFKEY.Event=1;
	      }
	    } 
	    levent=daq.eventno();
	    tlevent=daq.time();
	    if(Event >=0 && daq.runno() == Run &&
	       daq.eventno() >= Event)break;
	    if(daq.runno() == Run && iposr ==-Event)break;
	    run=daq.runno();
	    daq.shrink();
	  }
	}
	// pos back if fbin.good
	if(ok){
	  int off=-daq.getlengthR();
	  fbin.seekg(off,ios::cur);
	  cout<<"DAQEvent::init-I-Selected Run = "<<Run<<
	    " Event = "<<daq.eventno()<< " Position = "<<iposr<<" Time "<<ctime(&daq.time())<<endl;

	}
	else {
	  if(run>0){
	    cout <<" DAQEvent::init-I-Run "<<run<<" events "<<SELECTFFKEY.Event<<" "<<fevent<<" "<<levent<<endl;
	  }

	  cerr <<"DAQEvent::init-F-Failed to select Run = "<<
	    Run<<" Event >= "<<Event<<endl;
	  return UnableToFindRunEvent;
	}

      }
    }
    else{ 
    cerr<<"DAQEvent::init-E-cannot open file "<<fnam<<" in mode input"<<endl;
againcp:
      //    try castor
      if(getenv("NtupleDir") && !strstr(fnam,getenv("NtupleDir"))){
        setenv("NtupleDir0",getenv("NtupleDir"),1);
	setenv("STAGE_HOST","castorpublic",1);
	setenv("RFIO_USE_CASTOR_V2","YES",1);
	setenv("STAGE_SVCCLASS","amscdr",1);
	setenv("CASTOR_INSTANCE","castorpublic",1);
	char ln[1024];
	char utmp[80];
	sprintf(utmp,"/tmp/raw.%d",getpid());
	sprintf(ln,"ls -l %s 1>%s 2>&1",fnam,utmp);
	system(ln);
	ifstream ftxt;                                          
	ftxt.open(utmp);
	if(ftxt){
	  string txt;
	  getline(ftxt,txt);
	  if(txt.find("->")!=string::npos){
	    string castor("/castor/cern.ch/ams");
	    string file(txt.c_str()+txt.rfind("/")+1);
	    castor+=txt.c_str()+txt.find("/",txt.find("/",txt.find("->"))+1);
	  again:
	    if(getenv("NtupleDir")){
	      string local(getenv("NtupleDir"));
	      setenv("LD_LIBRARY_PATH",getenv("NtupleDir"),1);
	      if(getenv("TransferSharedLib")){
		setenv("LD_LIBRARY_PATH",getenv("TransferSharedLib"),1);
	      }
	      string cp(getenv("TransferRawBy")?getenv("TransferRawBy"):"rfcp ");
	      cp+=castor;
	      cp+=" ";
	      cp+=local;
	      int i=system(cp.c_str());
	      if(i){
		cerr <<"DAQEvent::init-E-Unableto "<<cp.c_str()<<endl;
		if(getenv("NtupleDir2") ){
		  char *nt2=getenv("NtupleDir2");
		  setenv("NtupleDir02",getenv("NtupleDir2"),1);
		  if(strlen(nt2)){
		    string nt2_new=nt2;
		    char * whoami=getlogin();
		    int pos=nt2_new.find("whoami");  
		    if(pos>=0 && whoami)nt2_new.replace(pos,6,whoami);
		    setenv("NtupleDir",nt2_new.c_str(),1);
		    cout <<"daqevt-I-RedefinedNtupleDir "<<getenv("NtupleDir")<<endl;
		  } 
		  else setenv("NtupleDir",getenv("NtupleDir2"),1);
		  unsetenv("NtupleDir2");
		  goto again;
		}    
		else if(getenv("NtupleDir3")){
		  setenv("NtupleDir03",getenv("NtupleDir3"),1);
		  setenv("NtupleDir",getenv("NtupleDir3"),1);
		  unsetenv("NtupleDir3");
		  goto again;
		}    
		local+="/";
		local+=file;
		setenv("TMPRawFile",local.c_str(),1);
		strcpy(fnam,local.c_str());

	      }
              else{
		local+="/";
		local+=file;
		cout<<"DAQEvent::init-I-CopiedTo "<<local<<endl;
		setenv("TMPRawFile",local.c_str(),1);
		strcpy(fnam,local.c_str());
              }
	    }
	    ftxt.close();
	    unlink(utmp);
	    goto castor;

	  }
	  ftxt.close();
	  unlink(utmp);
	}
      }
//
//  try scp once more
//
  if(getenv("TransferRawByB") and strlen(getenv("TransferRawByB"))){
    setenv("TransferBy",getenv("TransferRawByB"),1);
    unsetenv("TransferRawByB");
    goto againcp;
  }
  
strcpy(fnam,fnamei.c_str());
      if(ntryscp++>3)return UnableToOpenFile; 
      if(getenv("NtupleDir0"))setenv("NtupleDir",getenv("NtupleDir0"),1);
      if(getenv("NtupleDir02"))setenv("NtupleDir2",getenv("NtupleDir02"),1);
      if(getenv("NtupleDir03"))setenv("NtupleDir3",getenv("NtupleDir03"),1);
      if(getenv("NtupleDir") && !strstr(fnam,getenv("NtupleDir"))){
		  unsetenv("NtupleDir0");
	char ln[1024];
	char utmp[80];
	sprintf(utmp,"/tmp/raw.%d",getpid());
	sprintf(ln,"ls -l %s 1>%s 2>&1",fnam,utmp);
	system(ln);
	ifstream ftxt;                                          
	ftxt.open(utmp);
	if(ftxt){
	  string txt;
	  getline(ftxt,txt);
	  if(txt.find("->")!=string::npos){
	    string castor("/castor/cern.ch/ams");
	    string file(txt.c_str()+txt.rfind("/")+1);
	    castor+=txt.c_str()+txt.find("/",txt.find("/",txt.find("->"))+1);
	  againscp:
	    if(getenv("NtupleDir")){
	      string local(getenv("NtupleDir"));
	      setenv("LD_LIBRARY_PATH",getenv("NtupleDir"),1);
	      if(getenv("TransferSharedLib")){
		setenv("LD_LIBRARY_PATH",getenv("TransferSharedLib"),1);
	      }
	      string cp(getenv("TransferRawBy2")?getenv("TransferRawBy2"):"scp  ams.cern.ch:");
             setenv("TransferRawBy2",cp.c_str(),0);
              char hostdefault[]="ams.cern.ch";
              char *host=getenv("AMSRescueHost")?getenv("AMSRescueHost"):hostdefault;
              cp+=host;
              cp+=":";
	      cp+=fnam;
	      cp+=" ";
	      cp+=local;
	      int i=system(cp.c_str());
	      if(i){
		cerr <<"DAQEvent::init-E-Unableto "<<cp.c_str()<<endl;
		if(getenv("NtupleDir2") ){
		  char *nt2=getenv("NtupleDir2");
		  unsetenv("NtupleDir02");
		  if(strlen(nt2)){
		    string nt2_new=nt2;
		    char * whoami=getlogin();
		    int pos=nt2_new.find("whoami");  
		    if(pos>=0 && whoami)nt2_new.replace(pos,6,whoami);
		    setenv("NtupleDir",nt2_new.c_str(),1);
		    cout <<"daqevt-I-RedefinedNtupleDir "<<getenv("NtupleDir")<<endl;
		  } 
		  else setenv("NtupleDir",getenv("NtupleDir2"),1);
		  unsetenv("NtupleDir2");
		  goto againscp;
		}    
		else if(getenv("NtupleDir3")){
		  unsetenv("NtupleDir03");
		  setenv("NtupleDir",getenv("NtupleDir3"),1);
		  unsetenv("NtupleDir3");
		  goto againscp;
		}    
		local+="/";
		local+=file;
		setenv("TMPRawFile",local.c_str(),1);
		strcpy(fnam,local.c_str());

	      }
              else{
		local+="/";
		local+=file;
		cout<<"DAQEvent::init-I-CopiedTo "<<local<<endl;
		setenv("TMPRawFile",local.c_str(),1);
		strcpy(fnam,local.c_str());
              }
	    }
	    ftxt.close();
	    unlink(utmp);
	    goto castor;

	  }
	  ftxt.close();
	  unlink(utmp);
	}
      }




      return UnableToOpenFile;
    }
  }
  else {
    cout <<"DAQEvent::init-I-NoMoreInputFiles"<<endl;
    return NoInputFile;
  }
  return OK;
}


void DAQEvent::initO(integer run,integer eventno,time_t tt){
  enum open_mode{binary=0x80};
  integer mode=DAQCFFKEY.mode;
 again:
  if(mode/10 ){
    if(ofnam){
      char name[255];
      ostrstream ost(name,sizeof(name));
      ost.clear();
      if(ofnam[strlen(ofnam)-1]=='/')ost << ofnam<<run<<".raw"<<ends;
      //next line gives compiler error on gcc
      // else ost << ofnam<<ends;
      if(fbout)fbout.close();
      fbout.clear();
      if(ofnam[strlen(ofnam)-1]!='/')ost << ofnam<<ends;
      fbout.open(name,ios::out);
      if(fbout){ 
#ifdef __CORBA__
	AMSProducer::gethead()->sendNtupleStart(DPS::Producer::RawFile,name,run,eventno,tt);
#endif
	static char buffer[12048+1];
	// Associate buffer
#if defined(__USE_STD_IOSTREAM) || defined(__STDC_HOSTED__)  || defined(sun) || defined(__ICC__)
	(fbout.rdbuf())->pubsetbuf(buffer,12048);
#else
	//      (fbout.rdbuf())->pubsetbuf(buffer,12048);
	(fbout.rdbuf())->pubsetbuf(buffer,12048);
#endif
	cout<<"DAQEvent::initO-I- opened file "<<name<<" in mode "<<mode<<endl;

      }
      else{
	cerr<<"DAQEvent::initO-F-cannot open file "<<name<<" in mode "<<mode<<endl;
#ifdef __CORBA__
	bool writeable=false;
      ndir:  
	if(char *ntd=getenv("NtupleDir")){
	  AString cmd=" mkdir -p -v  ";
	  cmd+=ntd;
	  system(cmd);
	  cmd=" touch ";
	  cmd+=ntd;
	  cmd+="/qq";
	  int i=system(cmd);
	  if(i==0){
	    writeable=true;
	    cmd=ntd;
	    cmd+="/qq";
	    unlink((const char*)cmd);
	  }
	  else{
	    if(getenv("NtupleDir2")){
	      char *nt2=getenv("NtupleDir2");
	      if(strlen(nt2)){
		string nt2_new=nt2;
		char * whoami=getlogin();
		int pos=nt2_new.find("whoami");  
		if(pos>=0 && whoami)nt2_new.replace(pos,6,whoami);
		setenv("NtupleDir",nt2_new.c_str(),1);
		cout <<"daqevt-I-RedefinedNtupleDir "<<getenv("NtupleDir")<<endl;
	      } 
	      else setenv("NtupleDir",getenv("NtupleDir2"),1);
	      unsetenv("NtupleDir2");
	      goto ndir;
	    }    
	    else if(getenv("NtupleDir3")){
	      setenv("NtupleDir",getenv("NtupleDir3"),1);
	      unsetenv("NtupleDir3");
	      goto ndir;
	    }    
	  }
	} 

	if(writeable){
	  strcpy(ofnam,getenv("NtupleDir"));
	  if(ofnam[strlen(ofnam)-1]!='/')strcat(ofnam,"/");
	  goto again;
	}
#endif
	throw amsglobalerror("UnableToOpenOutputRawFile",3);
      }
    }
    else {
      cerr <<"DAQEvent::init-F-no output file to init "<<endl;
      exit(1);
    }
  }




}



void DAQEvent::_convert(){
  if(AMSDBc::BigEndian){
    unsigned char tmp;
    unsigned char *pc = (unsigned char*)(_pData);
    int i;
    for(i=0;i<_Length;i++){
      tmp=*pc;
      *pc=*(pc+1);
      *(pc+1)=tmp;
      pc=pc+sizeof(int16u);
    }
  }
}




void DAQEvent::_convertl(int16u & l16){
  if(AMSDBc::BigEndian){
    unsigned char tmp;
    unsigned char *pc = (unsigned char*)(&l16);
    tmp=*pc;
    *pc=*(pc+1);
    *(pc+1)=tmp;
  }
}


void DAQEvent::_copyEl(){


}

void DAQEvent::_writeEl(){
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this); 
#endif

}




integer DAQEventI::_Count=0;
DAQEventI::DAQEventI(){
  _Count++;
}
DAQEventI::~DAQEventI(){
  if(--_Count==0){
#ifdef __ALPHA__ 
    DAQEvent::fbin.close();
#else
    if(DAQEvent::fbin.is_open())DAQEvent::fbin.close();
#endif
#ifdef __ALPHA__ 
    DAQEvent::fbout.close();
#else
    if(DAQEvent::fbout.is_open())DAQEvent::fbout.close();
#endif
  }
}

void DAQEvent::CloseO(integer run,integer eventno,time_t tt){
  if(DAQEvent::fbout.is_open()){
    DAQEvent::fbout.close();
#ifdef __CORBA__
    AMSProducer::gethead()->sendNtupleEnd(DPS::Producer::RawFile,_NeventsO,eventno,tt,true);
#endif


  }
}

integer DAQEvent::_create(uinteger btype){
  if(_pData)shrink();
  if(_Length <=0 )return 0;
  if(_Length*sizeof(_pData[0])> sizeof(_Buffer) || _BufferLock){
    if(_Length>100000000){
      cerr<<" DAQEvent::_create-E-LengthIsTooBig "<<_Length<<endl;
      return 0;
    }
    _pData= (int16u*)UPool.insert(sizeof(_pData[0])*_Length);
  }
  else {
    _pData=(int16u*)_Buffer;
    _BufferOwner=1;
    _BufferLock=1;
  }
  if(_pData && AMSJob::gethead()->isSimulation()){
    //   This is for daq creation only
    //   

    const int thr=32767;
    if((_Length-_OffsetL)*2<=thr){
      _pData[0]=(_Length-_OffsetL)*2;
      _pData[1]=(btype) | (1<<15) ;
      _pData[2]=0;
      time_t timeu=AMSEvent::gethead()->gettime();
      _pData[4]=timeu&65535;
      _pData[3]=(timeu>>16)&65535;
      _pcur=_pData+5;
    }
    else{
      _pData[0]=(((_Length-2*_OffsetL)*2)>>16)&32767;
      _pData[1]=(((_Length-2*_OffsetL)*2))&65535;
      _pData[0]=_pData[0] | (1<<15);
      _pData[2]=(btype)  | (1<<15);
      _pData[3]=0;
      time_t timeu=AMSEvent::gethead()->gettime();
      _pData[5]=timeu&65535;
      _pData[4]=(timeu>>16)&65535;
      _pcur=_pData+6;
    }

  }



  return _pData != NULL ;


}


//+

uint16  DAQEvent::sdetlength(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
    id = _pData[offset];
    l  = _pData[offset - 1];
    if (id == sdetid) return l;
    offset = offset  + l + 1;
    if (offset > getlength()/sizeof(_pData[0])) break;
  }
  return 1;
}  

integer DAQEvent::sdet(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
    id = _pData[offset];
    l  = _pData[offset-1];
    if (id == sdetid) return offset;
    offset = offset  + l + 1;
    if (offset > getlength()/sizeof(_pData[0])) break;
  }
  return -1;
}  

void DAQEvent::dump(uint16 sdetid) {
  // dump event
  // if sdetid == -1 dump whole event
  //
  cout<<"run, event, length "<<runno()<<", "<<eventno()<<", "<<getlength()
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
      if (offset > getlength()/sizeof(_pData[0])) break;
    }
  }
}  
//-  




int DAQEvent::parser(char a[], char **& fname){


  int ntot=0;
  int kl;
  for(kl=strlen(a);kl>=0;kl--){
    if(a[kl]=='/'){
      kl++;
      break;
    }
  }
  if(kl<0)kl=0;
  // cout << " kl "<<kl<<endl;

  if(kl==0 || kl==strlen(a)){
    // Whole directory  wanted
    AString fdir(a);

#if defined(__DARWIN__)
    dirent ** namelist;
    ntot=scandir((const char *)fdir,&namelist,&_select,reinterpret_cast<int(*)(const dirent **, const dirent **)>(&_sort));
#elif defined(__LINUXSLC6__)
    dirent64 ** namelist;
    ntot=scandir64((const char *)fdir,&namelist,&_select,reinterpret_cast<int(*)(const dirent64 **, const dirent64 **)>(&_sort));
#elif defined(__LINUXNEW__)
    dirent64 ** namelist;
    ntot=scandir64((const char *)fdir,&namelist,&_select,&_sort);
#elif defined(__LINUXGNU__)
    dirent64 ** namelist;
    ntot=scandir64((const char *)fdir,&namelist,&_select,reinterpret_cast<int(*)(const void*, const void*)>(&_sort));
#else
    dirent64 ** namelist;
    ntot=scandir64((const char *)fdir,&namelist,&_select,&_sort);
#endif
    int ngood=0;
    if(ntot==-1 && !(DAQCFFKEY.mode/10)){
      cerr<<"DAQEvent::parser-F-ProblemToScan "<<fdir<<endl;
      exit(1);
    }
    if(ntot>0){
      fname =new char*[ntot];
      char ftemp[1025];
      for(int i=0;i<ntot;i++){
	strcpy(ftemp,a);
	strcat(ftemp,namelist[i]->d_name);
	struct stat64 statbuf;
	if(!stat64 (ftemp,&statbuf) && statbuf.st_size>0){
	  fname[ngood]=new char[strlen(a)+strlen(namelist[i]->d_name)+1];
	  strcpy(fname[ngood],a);
	  strcat(fname[ngood++],namelist[i]->d_name);
	  //             cout << i<< "  "<< fname[ngood-1]<<endl;
	}
	free(namelist[i]);
      }
      free(namelist);
    }
    return ngood>0?ngood:0;      
  }


  {
    int coma=kl-1;
    for(int i=kl;i<strlen(a)+1;i++){
      if(a[i]==',' || i==strlen(a)){
	if(i-coma > 1){
	  // find -
	  int tire=0;
	  for(int j=coma+1;j<i;j++){
	    if(a[j]=='-' && j != coma+1 && j != i-1){
	      istrstream osta(a+coma+1,j-coma-1);
	      int ia= 1000000000;
	      int ib=-1000000000;
	      osta >>ia;   
	      istrstream ostb(a+j+1,i-j-1);
	      ostb >>ib;   
	      if(ib >=ia)ntot+=ib-ia+1;
	      tire=1;  
	    }
	  }
	  if(tire==0)ntot++; 
	  coma=i;
	}
      }
    }
  }
  //   cout << " ntot "<<ntot<<endl;
  fname =new char*[ntot];
  ntot=0;
  {
    int coma=kl-1;
    for(int i=kl;i<strlen(a)+1;i++){
      if(a[i]==',' || i==strlen(a)){
	if(i-coma > 1){
	  // find -
	  int tire=0;
	  for(int j=coma+1;j<i;j++){
	    if(a[j]=='-' && j!=coma+1 && j!=i-1){
	      istrstream osta(a+coma+1,j-coma-1);
	      int ia= 1000000000;
	      int ib=-1000000000;
	      osta >>ia;   
	      istrstream ostb(a+j+1,i-j-1);
	      ostb >>ib;   
	      if(ib >=ia){
		//add leading zero(s)
		int lz=0;
		int l;
		for(l=coma+1;l<j;l++){
		  if(a[l]=='0')lz++;
		  else break;
		}
		for(int k=ia;k<=ib;k++){
		  fname[ntot++]=new char[255];
		  for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
		  for(l=kl;l<kl+lz;l++)fname[ntot-1][l]='0';
		  for(l=kl+lz;l<255;l++)fname[ntot-1][l]='\0';
		  ostrstream ost(fname[ntot-1]+kl+lz,255-kl-lz);
		  ost <<k;
		}
	      }
	      tire=1;  
	    }
	  }
	  if(tire==0){
	    fname[ntot++]=new char[255];
	    int l;
	    for(l=0;l<255;l++)fname[ntot-1][l]='\0';
	    for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
	    for(l=kl;l<i-coma-1+kl;l++)fname[ntot-1][l]=a[l-kl+coma+1];
	  }
	  coma=i;
	}
      }
    }
  }
  return ntot;  


}


#if defined( __ALPHA__) || defined(sun)

integer DAQEvent::_select(dirent64 *entry)
#endif
#if defined(__LINUXNEW__) || defined(__LINUXGNU__)
  integer DAQEvent::_select(const dirent64 *entry)
#endif
#ifdef __DARWIN__
  integer DAQEvent::_select( const dirent *entry)
#endif
{
  for(int i=0;i<strlen(entry->d_name);i++){
    if(!isdigit((entry->d_name)[i]))return 0;
  }

  int e1;
  sscanf(entry->d_name,"%d",&e1);
  // if(e1<SELECTFFKEY.Run)return 0;
  if(e1<SELECTFFKEY.Run || (SELECTFFKEY.RunE>0 && e1>SELECTFFKEY.RunE))return 0;

  else {
    if(AMSEvent::getselrun(0)>0){
      for(int i=0;;i++){
	if(e1==AMSEvent::getselrun(i))return 1;
	else if(!AMSEvent::getselrun(i))break;
      }
      return 0;
    }
    else return 1;
  }
}

void DAQEvent::SetEOFIn(){
  if(fbin){
    fbin.clear(fbin.rdstate() | ios::eofbit);
    if(fbin.eof())cout <<"DAQEvent::SetEOFIn-I-EOFSetForRun "<<_Run<<" "<<endl;
    else cerr <<"DAQEvent::SetEOFIn-E-EOFSetFailedForRun "<<_Run<<" "<<endl;
    GCFLAG.IEORUN=0;
  }
}

char * DAQEvent::_getNextFile(integer & run, integer &event){
  if(KIFiles==0){
    run=SELECTFFKEY.Run;
    event=SELECTFFKEY.Event;
  }
  else{
    run=0;
    event=0;
  }

  if (AMSJob::gethead()!=0 && AMSJob::gethead()->isMonitoring()) {
    _Waiting=true;
  again:
    while(KIFiles>=InputFiles){
      // Check if up directory isdigital, exists, and has files
      char dir[255];
      char newdir[1025];
      char delims[]="/";
      strcpy(newdir,_DirName);
      char *result=strtok(newdir,delims);
      while(result){
	strcpy(dir,result);
	result = strtok( NULL, delims );
      }
      bool digit=true;
      for(int j=0;j<strlen(dir);j++){
	if(!isdigit(dir[j])){
	  digit=false;
	  break;
	}
      }
      if(digit){
	int q=atoi(dir)+1;
	if(q<10000){
	  strcpy(newdir,_DirName);
	  result=strstr(newdir,dir);
	  sprintf(dir,"%04d",q);
	  for(int i=0;i<4;i++){
	    *(result+i)=dir[i];
	  }
#ifdef __DARWIN__
	  dirent ** namelist;
	  int ntot=scandir((const char *)newdir,&namelist,&_select,&_sort);
#elif defined(__LINUXSLC6__)
    dirent64 ** namelist;
    int ntot=scandir64((const char *)newdir,&namelist,&_select,reinterpret_cast<int(*)(const dirent64 **, const dirent64 **)>(&_sort));
#elif defined( __LINUXNEW__)
	  dirent64 ** namelist;
	  int ntot=scandir64((const char *)newdir,&namelist,&_select,(&_sort));
#elif defined( __LINUXGNU__)
	  dirent64 ** namelist;
	  int ntot=scandir64((const char *)newdir,&namelist,&_select,reinterpret_cast<int(*)(const void*, const void*)>(&_sort));
#else
	  dirent64 ** namelist;
	  int ntot=scandir64((const char *)newdir,&namelist,&_select,&_sort);
#endif
	  if(ntot>0){
	    for(int i=0;i<ntot;i++){
	      free(namelist[i]);
	    }            
	    free(namelist);
       
	    strcpy(_DirName,newdir); 
	    KIFiles=0;
	    cout <<"DAQEvent-I-SwitchingToNewDirFound "<<_DirName<<endl;
	  }
	  else sleep(60);
	}
	else sleep(60);
      }
      else sleep(60);
      if(ifnam){
	for(int i=0;i<InputFiles;i++){
	  delete[] ifnam[i];
	}
	delete[] ifnam;
	ifnam=0;
      }
      InputFiles=parser(_DirName,ifnam); 
    }
         
    char rootdir[10250];
    if(strlen(ofnam)){
      strcpy(rootdir,ofnam);
      strcat(rootdir,ifnam[KIFiles]);
    }
    else{
      strcpy(rootdir,ifnam[KIFiles]);
    }
    const int cl=161;
    static char bd[cl]="";
    static unsigned int initbd=0;
    #pragma omp threadprivate(bd,initbd)
    if(!initbd++){
    UHTOC(DAQCFFKEY.BlocksDir,(cl-1)/sizeof(integer),bd,cl-1);
    for (int i=cl-2; i>=0; i--) {
      if(bd[i] == ' ') bd[i]='\0';
      else break;
    }

    for(int i=1;i<cl-1;i++){
      if(bd[i]==' '){
	bd[i]='\0';
	break;
      }
    }    
   }
    cout <<"  BlocksDir "<<bd<<endl;
    char *last=strstr(rootdir,bd);
    if(last && strlen(bd)>0){
      //*last='R';
      //*(last+1)='O';
      //*(last+2)='O';
      //*(last+3)='T';
      char cmd[10250];
      strcpy(cmd,"mkdir -p ");
      last=strrchr(rootdir,'/');
      if(last){
	strncat(cmd,rootdir,(last-rootdir));
      }
      if(system(cmd)){
	cerr<<"DAQEvent-E-Parser-Unableto "<< cmd<<endl;
      }
    }
    else{
      last=strstr(rootdir,bd);
      if(last && strlen(bd)>0){
	*last='R';
	*(last+1)='O';
	*(last+2)='O';
	*(last+3)='T';
	char cmd[1025];
	strcpy(cmd,"mkdir -p ");
	last=strrchr(rootdir,'/');
	if(last){
	  strncat(cmd,rootdir,(last-rootdir));
	}
	if(system(cmd)){
	  cerr<<"DAQEvent-E-Parser-Unableto "<< cmd<<endl;
	}
      }
      else{
	cerr<<"DAQEvent-F-Parser-InvalidDirectoryStructure "<< rootdir<<" "<<bd<<" "<<strstr(rootdir,bd)<<endl;
               
	abort();
      }  
    }
    /*             
		   struct stat f_stat;
		   char lockfile[1024];
		   strcpy(lockfile,rootdir);
		   strcat(lockfile,".lock");
		   if(!stat(lockfile,&f_stat)){
		   KIFiles++;
		   goto again;
		   }
		   else{
		   char cmd[1024];
		   strcpy(cmd,"touch ");
		   strcat(cmd,lockfile);
		   if(system(cmd))cerr<<"DAQEvent-E-ParserUnableTo "<<cmd<<endl;
		   } 
    */
    struct stat64 f_stat;
    //              sleep(1);
    char rootfile[1024];
    strcpy(rootfile,rootdir);
    strcat(rootfile,".root");
    if(!stat64(rootfile,&f_stat)){
      cout <<"DAQEvent::_getNextFile-I-"<<rootfile<<" skipped"<<endl;  
      KIFiles++;
      goto again;   
    } 
    try{
      char cmd[1024];
      strcpy(cmd,"touch ");
      strcat(cmd,rootfile);
      if(system(cmd))cerr<<"DAQEvent-E-ParserUnableTo "<<cmd<<endl;
      setRootDir(rootdir);
      cout << " trying "<<RootDir()<<endl; 
      AMSEvent::getSRun()=0;  
      //AMSJob::gethead()->urinit(rootdir);
    }
    catch (amsglobalerror e){
      cerr << "Catached "<<e.getmessage()<<endl;
      KIFiles++;
      goto again;   
    }
    _Waiting=false;
    int kb=0;
    for(int k=0;k<strlen(ifnam[KIFiles]);k++){
      if( *(ifnam[KIFiles]+k)==' ')kb++;
      else break;
    }
    return ifnam[KIFiles++]+kb;
  }
  else{
    if(!_NeventsPerRun || _NeventsPerRun>AMSEvent::get_num_threads()){
      if(KIFiles<InputFiles){
	_NeventsPerRun=0;
	int kb=0;
	for(int k=0;k<strlen(ifnam[KIFiles]);k++){
	  if( *(ifnam[KIFiles]+k)==' ')kb++;
	  else break;
	}
	for(int k=strlen(ifnam[KIFiles])-1;k>kb;k--){
	  if( *(ifnam[KIFiles]+k)==' ')*(ifnam[KIFiles]+k)='\0';
	}

	return ifnam[KIFiles++]+kb;
      }
      else return 0;
    }
    else{
      cerr<<"DAQEvent::_getNextFile-E-FileTooShort "<<_NeventsPerRun<<" "<<ifnam[KIFiles-1]<<endl;
    }
    int kb=0;
    for(int k=0;k<strlen(ifnam[KIFiles-1]);k++){
      if( *(ifnam[KIFiles-1]+k)==' ')kb++;
      else break;
    }
    return ifnam[KIFiles-1]+kb;
  }

}

void DAQEvent::setRootDir(char * rootdir){
  if(_RootDir){
    delete[]_RootDir;
    _RootDir=0;
  }
  if(rootdir && strlen(rootdir)>0){
    _RootDir=new char[strlen(rootdir)+1];
    strcpy(_RootDir,rootdir);
  }
}



extern "C" size_t _compressable(Bytef * istream, size_t inputl){
  // 
  //estimate compressability
  //
  //input:   istream, inputl(Bytef)
  //
  //returns outputl(Bytef) if success ; 0 otherwise
  //


  size_t outputlb=0;
  for(int i=0;i<inputl;i++){
    if(istream[i]==0)outputlb++;
    else outputlb+=CHAR_BIT+1;
  }

  size_t outputl=(outputlb+CHAR_BIT-1)/CHAR_BIT;
  const int HDR=3;
  return (outputl+HDR<inputl?outputl:0);

}


extern "C" size_t _decompressable(Bytef * istream, size_t inputl){
  //
  //input:   istream, inputl(Bytef)
  //
  //returns outputl(Bytef) if success ; 0 otherwise
  //


  size_t pos=0;
  size_t outl=0;
  while(1){
    if(pos>=inputl*CHAR_BIT)break;
    size_t id=pos/CHAR_BIT;
    size_t off=pos%CHAR_BIT;
    if(istream[id]& (1<<off)){
      pos+=CHAR_BIT+1;
      if(pos>inputl*CHAR_BIT)break;
    }
    else pos++;
    outl++;

  }
  return outl;
}



extern "C" bool _decompress(Bytef * istream, size_t inputl,Bytef * ostream, size_t outputl){


  size_t pos=0;
  size_t outl=0;
  while(1){
    if(pos>=inputl*CHAR_BIT)break;
    size_t id=pos/CHAR_BIT;
    size_t off=pos%CHAR_BIT;
    if(istream[id]& (1<<off)){
      if(pos+CHAR_BIT>inputl*CHAR_BIT)break;
      if(outl>outputl){
	cerr<<"  _decompress error !!! "<<outl<<" "<<pos<<" "<<inputl<<" "<<outputl<<endl;
	return false;
      }
      pos++;
      for(int k=0;k<CHAR_BIT;k++){
	id=pos/CHAR_BIT;
	off=pos%CHAR_BIT;
	if(istream[id]& (1<<off))ostream[outl]|=(1<<k);
	else  ostream[outl]&=~(1<<k); 
	pos++;
      }
      outl++;
      if(pos>=inputl*CHAR_BIT)break;
    }
    else {
      pos++;
      ostream[outl++]=0;
    }
  }
  return true;
}




extern "C" bool _compress(Bytef * istream, size_t inputl,Bytef * ostream, size_t outputl){
  /*
  // do zero suppression 
  //
  //
  //input:   istream, inputl,outputl
  //
  //output   ostream
  //
  // NB ostream[outputl+2] ZERO array should be provided by caller
  //  

  // returns true if success
  */
  size_t id=0;
  size_t off=0;
  for(int i=0;i<inputl;i++){
    if(id>=outputl){
      return false;
    }
    if(istream[i]){
      ostream[id]+=(1<<off);
      off=(++off)%CHAR_BIT;
      if(off){
        ostream[id++]+=(istream[i]<<off);
        ostream[id]=(istream[i]>>(CHAR_BIT-off));     
      }
      else ostream[(++id)++]=istream[i];
    }
    else{
      off=(++off)%CHAR_BIT;
      if(!off)id++;
    }
  }
  if(off)ostream[id++]+=(1<<off);
  return id == outputl;
}



bool DAQCompress::_huffman_init = false;
bool DAQCompress::_huffman_swap = false;

unsigned int   DAQCompress::huffman_code[256] = { 256*0 };
unsigned short DAQCompress::_huffman_version = 0;
unsigned short DAQCompress::huffman_first_code[16] = {16*0};
unsigned short DAQCompress::huffman_decode_table[16][256] = {4096*0};
unsigned short DAQCompress::huffman_array[0xC000] = {0xC000*0};
unsigned int   DAQCompress::huffman_length = 0;

void DAQCompress::huffman_init(unsigned int run_id) {

  unsigned char  *ptr;
  unsigned int   v;
  unsigned short i, n, m, pos, vcode, lcode;
  unsigned int huffman_code_B606[256] = {
    0x00030002, 0x00140005, 0x00150005, 0x00240006, 0x00410007, 0x00420007, 0x00250006, 0x00260006,
    0x00270006, 0x00430007, 0x005e0008, 0x005f0008, 0x00600008, 0x00610008, 0x00620008, 0x00630008,
    0x00640008, 0x00440007, 0x00650008, 0x00660008, 0x00670008, 0x00680008, 0x001a0009, 0x001b0009,
    0x00690008, 0x006a0008, 0x001c0009, 0x001d0009, 0x001e0009, 0x001f0009, 0x00200009, 0x00210009,
    0x006b0008, 0x006c0008, 0x00220009, 0x00230009, 0x00240009, 0x00250009, 0x00260009, 0x00270009,
    0x00280009, 0x00290009, 0x002a0009, 0x002b0009, 0x002c0009, 0x002d0009, 0x002e0009, 0x002f0009,
    0x00300009, 0x00450007, 0x006d0008, 0x006e0008, 0x00310009, 0x00320009, 0x00330009, 0x00340009,
    0x00350009, 0x00360009, 0x00370009, 0x00380009, 0x00390009, 0x003a0009, 0x003b0009, 0x003c0009,
    0x006f0008, 0x003d0009, 0x003e0009, 0x003f0009, 0x00400009, 0x00410009, 0x00420009, 0x00430009,
    0x00440009, 0x00450009, 0x00460009, 0x00470009, 0x00480009, 0x00490009, 0x004a0009, 0x004b0009,
    0x004c0009, 0x004d0009, 0x004e0009, 0x004f0009, 0x00500009, 0x00700008, 0x00710008, 0x00720008,
    0x00510009, 0x00730008, 0x00520009, 0x00460007, 0x00740008, 0x00530009, 0x00540009, 0x00550009,
    0x00560009, 0x00750008, 0x00570009, 0x00580009, 0x00590009, 0x005a0009, 0x005b0009, 0x005c0009,
    0x005d0009, 0x005e0009, 0x005f0009, 0x00600009, 0x000a000a, 0x000b000a, 0x000c000a, 0x00610009,
    0x00620009, 0x00630009, 0x00640009, 0x000d000a, 0x000e000a, 0x00650009, 0x000f000a, 0x0010000a,
    0x0011000a, 0x00660009, 0x0012000a, 0x0013000a, 0x0014000a, 0x0015000a, 0x00670009, 0x00760008,
    0x00160005, 0x00680009, 0x00770008, 0x00780008, 0x00790008, 0x00690009, 0x006a0009, 0x006b0009,
    0x006c0009, 0x006d0009, 0x006e0009, 0x006f0009, 0x00700009, 0x00710009, 0x0016000a, 0x0017000a,
    0x00720009, 0x00730009, 0x0018000a, 0x0019000a, 0x001a000a, 0x00740009, 0x00750009, 0x00760009,
    0x001b000a, 0x001c000a, 0x001d000a, 0x001e000a, 0x001f000a, 0x0020000a, 0x0021000a, 0x00770009,
    0x007a0008, 0x007b0008, 0x00780009, 0x00790009, 0x007c0008, 0x007a0009, 0x007b0009, 0x007c0009,
    0x007d0009, 0x007d0008, 0x007e0009, 0x007f0009, 0x00800009, 0x00810009, 0x00820009, 0x00830009,
    0x00840009, 0x007e0008, 0x00850009, 0x00860009, 0x00870009, 0x007f0008, 0x00880009, 0x00890009,
    0x0022000a, 0x00800008, 0x0023000a, 0x0024000a, 0x0025000a, 0x0026000a, 0x008a0009, 0x008b0009,
    0x008c0009, 0x008d0009, 0x0027000a, 0x0028000a, 0x0029000a, 0x008e0009, 0x002a000a, 0x002b000a,
    0x002c000a, 0x008f0009, 0x002d000a, 0x002e000a, 0x002f000a, 0x0030000a, 0x0031000a, 0x0032000a,
    0x00900009, 0x00910009, 0x00920009, 0x0033000a, 0x00930009, 0x00940009, 0x00950009, 0x00960009,
    0x00970009, 0x00980009, 0x00990009, 0x009a0009, 0x009b0009, 0x009c0009, 0x009d0009, 0x009e0009,
    0x00470007, 0x009f0009, 0x00a00009, 0x00a10009, 0x00a20009, 0x00a30009, 0x00a40009, 0x00a50009,
    0x00a60009, 0x00a70009, 0x00a80009, 0x00a90009, 0x00aa0009, 0x00ab0009, 0x00ac0009, 0x00ad0009,
    0x00ae0009, 0x00af0009, 0x00b00009, 0x00b10009, 0x00b20009, 0x00b30009, 0x00b40009, 0x00b50009,
    0x00b60009, 0x00b70009, 0x00b80009, 0x00b90009, 0x00ba0009, 0x00bb0009, 0x00810008, 0x00170005
  };

  unsigned int huffman_code_B50A[256] = {
    0x00010001, 0x00160006, 0x000d0005, 0x000e0005, 0x000f0005, 0x003f0008, 0x00250007, 0x00400008,
    0x00170006, 0x00260007, 0x00410008, 0x003b0009, 0x003c0009, 0x002e000a, 0x003d0009, 0x003e0009,
    0x00420008, 0x00430008, 0x003f0009, 0x00400009, 0x00410009, 0x002f000a, 0x0030000a, 0x00420009,
    0x0031000a, 0x00430009, 0x00440009, 0x0032000a, 0x0033000a, 0x0034000a, 0x0035000a, 0x00450009,
    0x00460009, 0x0036000a, 0x00470009, 0x0037000a, 0x0038000a, 0x0039000a, 0x003a000a, 0x00480009,
    0x00490009, 0x003b000a, 0x004a0009, 0x003c000a, 0x003d000a, 0x003e000a, 0x003f000a, 0x0040000a,
    0x004b0009, 0x004c0009, 0x004d0009, 0x004e0009, 0x004f0009, 0x00500009, 0x00510009, 0x00520009,
    0x00530009, 0x00540009, 0x00550009, 0x00560009, 0x00570009, 0x00580009, 0x00590009, 0x005a0009,
    0x00270007, 0x005b0009, 0x005c0009, 0x005d0009, 0x005e0009, 0x0041000a, 0x0042000a, 0x005f0009,
    0x00280007, 0x0043000a, 0x0044000a, 0x0045000a, 0x0046000a, 0x0047000a, 0x0048000a, 0x0049000a,
    0x00440008, 0x00600009, 0x004a000a, 0x004b000a, 0x004c000a, 0x004d000a, 0x004e000a, 0x0001000b,
    0x0002000b, 0x004f000a, 0x0003000b, 0x0004000b, 0x0050000a, 0x0005000b, 0x0051000a, 0x0052000a,
    0x0053000a, 0x0054000a, 0x0006000b, 0x0007000b, 0x0055000a, 0x0008000b, 0x0009000b, 0x000a000b,
    0x000b000b, 0x0056000a, 0x000c000b, 0x000d000b, 0x0057000a, 0x000e000b, 0x0058000a, 0x000f000b,
    0x0059000a, 0x005a000a, 0x0010000b, 0x0011000b, 0x0012000b, 0x005b000a, 0x0013000b, 0x0014000b,
    0x0015000b, 0x005c000a, 0x0016000b, 0x0017000b, 0x005d000a, 0x005e000a, 0x005f000a, 0x00610009,
    0x00180006, 0x00620009, 0x00630009, 0x00640009, 0x00450008, 0x00650009, 0x00660009, 0x00670009,
    0x00680009, 0x00690009, 0x00460008, 0x00290007, 0x00470008, 0x006a0009, 0x006b0009, 0x006c0009,
    0x00190006, 0x006d0009, 0x006e0009, 0x006f0009, 0x00700009, 0x00710009, 0x00720009, 0x00730009,
    0x0018000b, 0x0019000b, 0x001a000b, 0x001b000b, 0x0060000a, 0x001c000b, 0x001d000b, 0x001e000b,
    0x0061000a, 0x00480008, 0x001f000b, 0x0020000b, 0x00740009, 0x0021000b, 0x0062000a, 0x0022000b,
    0x0063000a, 0x00750009, 0x0023000b, 0x0024000b, 0x0064000a, 0x0025000b, 0x0026000b, 0x0027000b,
    0x0065000a, 0x00490008, 0x0066000a, 0x0028000b, 0x0067000a, 0x0029000b, 0x0068000a, 0x002a000b,
    0x002b000b, 0x00760009, 0x002c000b, 0x002d000b, 0x002e000b, 0x002f000b, 0x0069000a, 0x006a000a,
    0x006b000a, 0x00770009, 0x0030000b, 0x0031000b, 0x0032000b, 0x0033000b, 0x0034000b, 0x0035000b,
    0x0036000b, 0x00780009, 0x0037000b, 0x0038000b, 0x0039000b, 0x003a000b, 0x006c000a, 0x003b000b,
    0x006d000a, 0x00790009, 0x003c000b, 0x003d000b, 0x003e000b, 0x003f000b, 0x0040000b, 0x006e000a,
    0x0041000b, 0x007a0009, 0x0042000b, 0x0043000b, 0x0044000b, 0x0045000b, 0x006f000a, 0x0070000a,
    0x002a0007, 0x0046000b, 0x0047000b, 0x0048000b, 0x0049000b, 0x004a000b, 0x004b000b, 0x004c000b,
    0x007b0009, 0x004d000b, 0x004e000b, 0x004f000b, 0x0071000a, 0x0050000b, 0x0051000b, 0x0072000a,
    0x007c0009, 0x0052000b, 0x0053000b, 0x0073000a, 0x0054000b, 0x0055000b, 0x0074000a, 0x0056000b,
    0x0057000b, 0x0058000b, 0x0075000a, 0x0059000b, 0x005a000b, 0x005b000b, 0x007d0009, 0x002b0007
  };

  if ( run_id > 0x4DE80000 ) {
    if ( (_huffman_version == 0xB606) && (_huffman_init == true) ) return; 
    for (i=0; i<256; i++) huffman_code[i] = huffman_code_B606[i];
    _huffman_version = 0xB606;
    _huffman_init    = true;
  } else {
    if ( (_huffman_version == 0xB50A) && (_huffman_init == true) ) return;
    for (i=0; i<256; i++) huffman_code[i] = huffman_code_B50A[i];
    _huffman_version = 0xB50A;
    _huffman_init    = true;
  }        


  for(i=0; i<16; i++){                                  // code length
    for (n=0;  n<256; n++) {                            // position for a given length
      huffman_decode_table[i][n] = 0xFFFF;              // undefine code
    }
  }

  m = 0;
  for(i=0; i<16; i++){

    if ( i < 9 ) huffman_first_code[i] = 1<<(i+1);
    else         huffman_first_code[i] = 0;

    pos = 0;
    for (n=0;  n<256; n++) {

      v = huffman_code[n];                              // Huffman codes
      vcode = v >> 16;                                  // encoded bit pattern
      lcode = v & 0xFFFF;                               // number of bits in the pattern
      if ( m < lcode ) m = lcode;

      if( lcode == (i+1) ) {
        if ( pos == 0 ) huffman_first_code[i] = vcode;
        huffman_decode_table[i][pos++] = n;             // fill decoding table
      }
    }

  }

  // insert end symbol
  huffman_decode_table[m][0] = 256;                     // end symbol code


  // define swap rules
  ptr = (unsigned char *) &vcode;
  vcode = 0x55AA;
  lcode = ptr[1]<<8 | ptr[0];

  if ( lcode == vcode ) {
    _huffman_swap = true;
  } else {
    _huffman_swap = false;
  }

}


size_t DAQCompress::compressable(unsigned short *istream, size_t len, unsigned int run_id)
{

  huffman_init(run_id);

  unsigned int   i, v;
  unsigned short vcode, lcode, nbits, *dest;

  nbits  = 0;
  huffman_length = 0;

  len = len >> 1;                                        // input length in bytes
  dest = huffman_array;

  *dest = 0;

  for(i=0; i<len; i++) {

    v = (*istream & 0xFF00) >> 8;
    v = huffman_code[v];                                 // Huffman code for MSB

    vcode = v >> 16;                                     // encoded bit pattern
    lcode = v & 0xFFFF;                                  // number of bits in the pattern

    nbits += lcode;
    if ( nbits > 16 ) {

      // fill the current word and start the new one
      nbits &= 0xF;                                       // economic way of doing nbits -= 16;
      huffman_length++;

      if ( huffman_length < 0xC000 ) {
	*dest++ |= vcode >> nbits;
	*dest    = vcode << (16-nbits);
      }

    } else {

      // new code fits the current word
      *dest |= vcode << (16-nbits);

    }

    v = *istream++ & 0xFF;
    v = huffman_code[v];                                 // Huffman code for LSB

    vcode = v >> 16;                                     // encoded bit pattern
    lcode = v & 0xFFFF;                                  // number of bits in the pattern

    nbits += lcode;
    if ( nbits > 16 ) {

      // fill the current word and start the new one
      nbits &= 0xF;                                       // economic way of doing nbits -= 16;
      huffman_length++;

      if ( huffman_length < 0xC000 ) {
	*dest++ |= vcode >> nbits;
	*dest    = vcode << (16-nbits);
      }

    } else {

      // new code fits the current word
      *dest |= vcode << (16-nbits);

    }

  }

  // END of BLOCK symbol - 12 bits, all 0 - already encoded by construction

  huffman_length++;

  // output length in bytes
  return huffman_length<<1;
        
}


bool DAQCompress::compress(unsigned short *istream, size_t inputl, unsigned short *ostream, size_t outputl)
{
  if(!_huffman_init)                    return false;

  if ( outputl != (huffman_length<<1) ) return false;

  if ( huffman_length > 0xC000 )        return false;

  memcpy(ostream, huffman_array, outputl);

  return true;
}


size_t DAQCompress::decompressable(unsigned short *istream, size_t length, unsigned int run_id)
{

  huffman_init(run_id);

  unsigned char  *ostream;
  unsigned short l, v, dv, nb = 0;
  unsigned int   base = 0;

  //
  // decoding ends when all input words are processed
  // or END symbol - (12 zero bits) is identified
  //

  length  = length >> 1;
  ostream = (unsigned char *) huffman_array;

  while( 1 ) {

    // get encoded value
    for(l=0, v=huffman_bit(istream, l+base); v<huffman_first_code[l]; v=(v<<1) | huffman_bit(istream, ++l + base));

    base += l + 1;

    // BASE exceeding length means remaing bits are all ZEROes
    if ( base > (length<<4) ) break;                    // total decoded length <= input length

    dv = huffman_decode_table[l][v -huffman_first_code[l]];
    if( dv < 256 ) {
      ostream[nb++] = dv;                               // valid byte - continue decoding
    } else if (dv == 256) {
      break;                                            // end symbol - normal termination
    } else {
      nb = 0;                                           // invalid byte - abnormal termination
      break;
    }

  }

  // if the encoded stream is only partially processed
  if ( base < ((length<<4)-12) ) nb = 0;

  huffman_length = (nb+1)>>1;

  return nb;

}


bool DAQCompress::decompress(unsigned short *istream, size_t inputl, unsigned short *ostream, size_t outputl)
{

  int i;

  if(!_huffman_init)                    return false;

  if ( outputl != (huffman_length<<1) ) return false;

  if ( huffman_length > 0xC000 )        return false;

  // swap bytes if needed
  if ( _huffman_swap ) {
    for (i=0; i<huffman_length; i++) ostream[i] = ((huffman_array[i]&0xFF00)>>8) | ((huffman_array[i]&0xFF)<<8);
  } else {
    for (i=0; i<huffman_length; i++) ostream[i] = huffman_array[i];
  }

  return true;
}


#include "timeid.h"

bool DAQEvent::CalibDone(int id){
  if(id>=0 && id<7){
    if(_CalibDataS[id*3+2] && _CalibDataS[id*3+2] ==_CalibDataS[id*3+1]-_CalibDataS[id*3]+2 )return true;
    else return false; 
  }
  else return false;
}

bool DAQEvent::CalibInit(int id){
  if(id>=00 && id<7){
    if(_CalibDataS[id*3+1]-_CalibDataS[id*3]==0)return true;
    else return false; 
  }
  else return false;
}


unsigned int DAQEvent::_CalibDataS[7*3+1];

bool DAQEvent::SetFileSize(const char *fnam){
	struct stat64 statbuf;
	if(fnam && !stat64 (fnam,&statbuf) && statbuf.st_size>0){
          _FileSize=statbuf.st_size;
          return true;
        }
        else{
          _FileSize=0;
          return false;
        }
}


unsigned short int DAQlv3::lv3_node_addr[384] = {
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,    // JINJs
  0x6000, 0x6001, 0x6000, 0x6001, 0x7016, 0x7017, 0x7016, 0x7017,    // JLV1s   and ETRGs
  0x7116, 0x7117, 0x7116, 0x7117, 0x0000, 0x0000, 0xC000, 0xC000,    // ETRGs   and JINF-Es
  0xC000, 0xC000, 0xC100, 0xC100, 0xC100, 0xC100, 0xB000, 0xB000,    // JINJ-Es and JINF-Rs
  0xB000, 0xB000, 0xB100, 0xB100, 0xB100, 0xB100, 0x9000, 0x9000,    // JINJ-Es and JINF-Ts
  0x9000, 0x9000, 0x9100, 0x9100, 0x9100, 0x9100, 0x9200, 0x9200,    // JINJ-Ts
  0x9200, 0x9200, 0x9300, 0x9300, 0x9300, 0x9300, 0x9400, 0x9400,    // JINJ-Ts
  0x9400, 0x9400, 0x9500, 0x9500, 0x9500, 0x9500, 0x9600, 0x9600,    // JINJ-Ts
  0x9600, 0x9600, 0x9700, 0x9700, 0x9700, 0x9700, 0xA000, 0xA000,    // JINJ-Ts and JINF-Us
  0xA000, 0xA000, 0xA100, 0xA100, 0xA100, 0xA100, 0x4000, 0x4001,    // JINJ-Us and EDRs
  0x4000, 0x4004, 0x4005, 0x4004, 0x4008, 0x4009, 0x4008, 0x400C,    // EDRs
  0x400D, 0x400C, 0x4010, 0x4011, 0x4010, 0x4014, 0x4015, 0x4014,    // EDRs
  0x4100, 0x4101, 0x4100, 0x4104, 0x4105, 0x4104, 0x4108, 0x4109,    // EDRs
  0x4108, 0x410C, 0x410D, 0x410C, 0x4110, 0x4111, 0x4110, 0x4114,    // EDRs
  0x4115, 0x4114, 0x3000, 0x3001, 0x3004, 0x3005, 0x3008, 0x3009,    // EDRs    and RDRs
  0x300C, 0x300D, 0x3010, 0x3011, 0x3014, 0x3015, 0x3100, 0x3101,    // RDRs
  0x3104, 0x3105, 0x3108, 0x3109, 0x310C, 0x310D, 0x3110, 0x3111,    // RDRs
  0x3114, 0x3115, 0x5000, 0x5000, 0x5000, 0x5000, 0x5001, 0x5001,    // RDRs    and SDRs
  0x5001, 0x5001, 0x5002, 0x5002, 0x5002, 0x5002, 0x5003, 0x5003,    // SDRs
  0x5003, 0x5003, 0x1000, 0x1001, 0x1002, 0x1003, 0x1004, 0x1005,    // SDRs    and TDRs
  0x1006, 0x1007, 0x1008, 0x1009, 0x100A, 0x100B, 0x100C, 0x100D,    // TDRs
  0x100E, 0x100F, 0x1010, 0x1011, 0x1012, 0x1013, 0x1014, 0x1015,    // TDRs
  0x1016, 0x1017, 0x1100, 0x1101, 0x1102, 0x1103, 0x1104, 0x1105,    // TDRs
  0x1106, 0x1107, 0x1108, 0x1109, 0x110A, 0x110B, 0x110C, 0x110D,    // TDRs
  0x110E, 0x110F, 0x1110, 0x1111, 0x1112, 0x1113, 0x1114, 0x1115,    // TDRs
  0x1116, 0x1117, 0x1200, 0x1201, 0x1202, 0x1203, 0x1204, 0x1205,    // TDRs
  0x1206, 0x1207, 0x1208, 0x1209, 0x120A, 0x120B, 0x120C, 0x120D,    // TDRs
  0x120E, 0x120F, 0x1210, 0x1211, 0x1212, 0x1213, 0x1214, 0x1215,    // TDRs
  0x1216, 0x1217, 0x1300, 0x1301, 0x1302, 0x1303, 0x1304, 0x1305,    // TDRs
  0x1306, 0x1307, 0x1308, 0x1309, 0x130A, 0x130B, 0x130C, 0x130D,    // TDRs
  0x130E, 0x130F, 0x1310, 0x1311, 0x1312, 0x1313, 0x1314, 0x1315,    // TDRs
  0x1316, 0x1317, 0x1400, 0x1401, 0x1402, 0x1403, 0x1404, 0x1405,    // TDRs
  0x1406, 0x1407, 0x1408, 0x1409, 0x140A, 0x140B, 0x140C, 0x140D,    // TDRs
  0x140E, 0x140F, 0x1410, 0x1411, 0x1412, 0x1413, 0x1414, 0x1415,    // TDRs
  0x1416, 0x1417, 0x1500, 0x1501, 0x1502, 0x1503, 0x1504, 0x1505,    // TDRs
  0x1506, 0x1507, 0x1508, 0x1509, 0x150A, 0x150B, 0x150C, 0x150D,    // TDRs
  0x150E, 0x150F, 0x1510, 0x1511, 0x1512, 0x1513, 0x1514, 0x1515,    // TDRs
  0x1516, 0x1517, 0x1600, 0x1601, 0x1602, 0x1603, 0x1604, 0x1605,    // TDRs
  0x1606, 0x1607, 0x1608, 0x1609, 0x160A, 0x160B, 0x160C, 0x160D,    // TDRs
  0x160E, 0x160F, 0x1610, 0x1611, 0x1612, 0x1613, 0x1614, 0x1615,    // TDRs
  0x1616, 0x1617, 0x1700, 0x1701, 0x1702, 0x1703, 0x1704, 0x1705,    // TDRs
  0x1706, 0x1707, 0x1708, 0x1709, 0x170A, 0x170B, 0x170C, 0x170D,    // TDRs
  0x170E, 0x170F, 0x1710, 0x1711, 0x1712, 0x1713, 0x1714, 0x1715,    // TDRs
  0x1716, 0x1717, 0x2000, 0x2001, 0x2000, 0x2004, 0x2005, 0x2004,    // TDRs    and UDRs
  0x2008, 0x2009, 0x2008, 0x200C, 0x200D, 0x200C, 0x2010, 0x2011,    // UDRs
  0x2010, 0x2014, 0x2015, 0x2014, 0x2100, 0x2101, 0x2100, 0x2104,    // UDRs
  0x2105, 0x2104, 0x2108, 0x2109, 0x2108, 0x210C, 0x210D, 0x210C,    // UDRs
  0x2110, 0x2111, 0x2110, 0x2114, 0x2115, 0x2114, 0x0000, 0x0000     // UDRs
};
//
// HW ID - JINJ slave ID correspondence
// (JINF-XA connected to JINJ 1,2; JINF-XB - to JINJ 0,3)
//
unsigned short int  DAQlv3::lv3_jinj_slave[32] = {
  0x9200,                                                            // 00 - JINF-T2
  0x9300,                                                            // 01 - JINF-T3
  0xA100,                                                            // 02 - JINF-U1
  0x9000,                                                            // 03 - JINF-T0
  0x5001,                                                            // 04 - SDR2-1A
  0x5001,                                                            // 05 - SDR2-1B
  0x5000,                                                            // 06 - SDR2-0A
  0x5000,                                                            // 07 - SDR2-0B
  0xA000,                                                            // 08 - JINF-U0
  0x9100,                                                            // 09 - JINF-T1
  0xB000,                                                            // 10 - JINF-R0
  0xB100,                                                            // 11 - JINF-R1
  0xC000,                                                            // 12 - JINF-E0
  0xC100,                                                            // 13 - JINF-E1
  0x6000,                                                            // 14 - JLV1-A
  0x6000,                                                            // 15 - JLV1-B
  0x9400,                                                            // 16 - JINF-T4
  0x9500,                                                            // 17 - JINF-T5
  0x5002,                                                            // 18 - SDR2-2A
  0x5002,                                                            // 19 - SDR2-2B
  0x5003,                                                            // 20 - SDR2-3A
  0x5003,                                                            // 21 - SDR2-3B
  0x9600,                                                            // 22 - JINF-T6
  0x9700,                                                            // 23 - JINF-T7
  0x0000,                                                            // 24 - MC
  0x0001,                                                            // 25 - MC
  0x0002,                                                            // 26 - MC
  0x0003,                                                            // 27 - MC
  0x0004,                                                            // 28 - MC
  0x0005,                                                            // 29 - MC
  0x0006,                                                            // 30 - MC
  0x0007,                                                            // 31 - MC
};


void DAQlv3::lv3_xdr_proc(unsigned short detector, unsigned short len, unsigned short *ptr, bool mc) {


  unsigned short register reg = 0, var, nht, i, j;
  unsigned short k, stat;

  unsigned short *p, l;

  unsigned long register reg0, reg1, reg2 = 0;

  stat = ptr[len-1];

if(mc){
  stat |= 0x0020;
  if ( (detector&0x7000) == 0 ) {
    // process MC information
    return;
  }
}

  // check XDR fragment status
  if ( stat & 0x7800 ) {                                     // Fragment Error

    // check significance of the error
    if ( (stat & 0xF800) == 0xA000 ) err_run = 1;

    // check how many nodes affected
    switch ( (detector>>12) & 0x7 ) {
    case 1:
      err_tdr += 1;
      break;
    case 2:
      err_udr += 1;
      break;
    case 3:
      err_rdr += 1;
      break;
    case 4:
      if ( (stat&0x1F) < 22 ) err_edr += 1;
      else                    err_etrg+= 1;
      break;
    case 5:
      err_sdr += 1;
      break;
    case 6:
      err_lv1 += 1;
      break;
    case 7:
      err_etrg+= 1;
    }

    /*
#ifdef OFFLINE
    printf("\n");
    printf("Detector %04x, Fragment Length %d, XDR Fragment:\n", detector, len);
    for (i=0; i<len; i++) {
      if ( i%16 == 0 ) {
	if ( i == 0 )  printf(" %04x:", i);
	else           printf("\n %04x:", i);
      }
      printf(" %04x", ptr[i]);
    }
    printf("\n");
#endif
    */

    return;

  } else if ( stat & 0x0600 ) {

    // account for permanent error in T0, TDR12 and T3, TDR0c/0d/10/11/14/15
    //    if ( (detector != 0x1012) || (stat != 0x84b2) ) {
    if ( ((detector != 0x1012) || (stat != 0x84b2)) && 
	 ((detector != 0x130c) || (stat != 0x84ac)) &&
	 ((detector != 0x130d) || (stat != 0x84ad)) &&
	 ((detector != 0x1310) || (stat != 0x84b0)) &&
	 ((detector != 0x1311) || (stat != 0x84b1)) &&
	 ((detector != 0x1314) || (stat != 0x84b4)) &&
	 ((detector != 0x1315) || (stat != 0x84b5))  ) {

      // check how many nodes affected
      switch ( (detector>>12) & 0x7 ) {
      case 1:
	err_tdr += 1;
	break;
      case 2:
	err_udr += 1;
	break;
      case 3:
	err_rdr += 1;
	break;
      case 4:
	if ( (stat&0x1F) < 22 ) err_edr += 1;
	else                    err_etrg+= 1;
	break;
      case 5:
	err_sdr += 1;
	break;
      case 6:
	err_lv1 += 1;
	break;
      case 7:
	err_etrg+= 1;
      }

      /*
#ifdef OFFLINE
      printf("\n");
      printf("Detector %04x, Fragment Length %d, XDR Fragment:\n", detector, len);
      for (i=0; i<len; i++) {
	if ( i%16 == 0 ) {
	  if ( i == 0 )  printf(" %04x:", i);
	  else           printf("\n %04x:", i);
	}
	printf(" %04x", ptr[i]);
      }
      printf("\n");
#endif
      */

      return;

    }

  }

}

int DAQlv3::lv3_unpack(unsigned short *ev_sblock,
	       unsigned int    event_id,
	       bool mc) {


  unsigned long  register i, l, len, length;
  unsigned short register reg, det, stat, node_id, *node_data, *ptr;



  //
  //  analyse data - create input data to LV3 algorithms
  //
  ptr    = ev_sblock;
  length = *ptr++;                               // Normal Size in bytes

if(mc){
  if ( length & 0x8000 ) {
    length = (length & 0x7FFF) << 16;
    length = length + *ptr++;                    // Large Size in bytes
  }
}

  length    = (length>>1) - 1;                   // Length of the DSP data part - short words

  node_id   = (*ptr++ >> 5) & 0x1FF;
  node_data = ptr;                               // Pointer to event number


  // fragment status as defined by JINJ
  stat = node_data[length-2];                    // the very last word is CRC


  if ( (node_id > 0x7F) && (node_id < 0x1FE) ) { // DSP nodes

    det = lv3_node_addr[node_id - 0x80];
    if ( det == 0x8000 ) {                       // this is JINJ

      // pointers to JINJ slaves
      len = 3;                                   // size without events
      ptr = &node_data[1];                       // skip event number

      // check event number
      if ( (event_id&0xFFFF) != node_data[0] ) {

	// Event mismatch between JINJ and JMDC
        err_event    = 1;
        err_run      = 1;

	/*
#ifdef OFFLINE
	printf("\n");
	printf("Detector %04x, Fragment Length %d, JINJ Fragment Status %04x, Event %04x\n", det, length, stat, node_data[0]);
#endif
	*/
	
	goto lv3_compress;

      }

      while ( len < length ) {

	l    = ptr[0];                           //  slave event fragment length
	stat = ptr[l];                           //  status of event fragment
	det  = lv3_jinj_slave[stat & 0x1f];      //  find detector according to slave id

	if ( det & 0x8000 ) {                    // JINF - detector except for SDR and LV1

	  // check JINF fragment status
	  /*
if(mc){
	  stat |= 0x8000;                        // MC fix
}
	  */
	  if ( (stat & 0xF800) == 0x8000 ) {     // process only Data reply without errors

	    i = 1;
	    while ( i < l-3 ) {
	      reg  = ptr[i];
	      stat = ptr[i + reg];
	      // processing event fragment
	      lv3_xdr_proc( (0x7FFF&det) + (stat&0x1f), ptr[i], &ptr[i+1], mc);
	      i += ptr[i] + 1;
	    }

	  } else {

	    // Error set by JINF - set corresponding error bit
	    err_event    = 1;
            // check significance of the error
	    if ( (stat & 0xF800) == 0xA000 ) err_run = 1;

            // check how many nodes affected
	    switch ( (det>>12) & 0x7 ) {
	    case 1:
	      err_tdr += 24;
	      break;
	    case 2:
	      err_udr += 6;
	      break;
	    case 3:
	      err_rdr += 12;
	      break;
	    case 4:
	      err_edr += 6;
	      err_etrg+= 1;
	      break;
	    }

	    /*
#ifdef OFFLINE
	    printf("\n");
	    printf("Detector %04x, Fragment Length %d, JINF Fragment Status %04x\n", det, l, stat);
	    for (i=0; i<l; i++) {
	      if ( i%16 == 0 ) {
		if ( i == 0 )  printf(" %04x:", i);
		else           printf("\n %04x:", i);
	      }
	      printf(" %04x", ptr[i+1]);
	    }
	    printf("\n");
#endif
	    */
	    //	    goto lv3_compress;

	  }

	} else {
	  lv3_xdr_proc( det, ptr[0], &ptr[1], mc);
	}

	len += l + 1;
	ptr += l + 1;

      }

    } else if ( det & 0x8000 ) {               // this is JINF

      ptr = &node_data[1];                     // skip event number

      // check status errors
      if ( (stat & 0x7800) != 0x0000 ) {

	// Error set by JINF - set corresponding error bit
        err_event    = 1;

	// check how many nodes affected
	switch ( (det>>12) & 0x7 ) {
	case 1:
	  err_tdr += 24;
	  break;
	case 2:
	  err_udr += 6;
	  break;
	case 3:
	  err_rdr += 12;
	  break;
	case 4:
	  err_edr += 6;
	  err_etrg+= 1;
	  break;
	}
	/*
#ifdef OFFLINE
	printf("\n");
	printf("Detector %04x, Fragment Length %d, JINF Fragment Status %04x, Event %04x\n", det, length, stat, node_data[0]);
	for (i=0; i<length-1; i++) {
	  if ( i%16 == 0 ) {
	    if ( i == 0 )  printf(" %04x:", i);
	    else           printf("\n %04x:", i);
	  }
	  printf(" %04x", node_data[i]);
	}
	printf("\n");
#endif
	*/
	goto lv3_compress;

      }

      // check event mismatch
      if ( (event_id&0xFFFF) != node_data[0] ) {

	// Error set by JINF - set corresponding error bit
        err_event    = 1;
        err_run      = 1;

	/*
#ifdef OFFLINE
	printf("\n");
	printf("Detector %04x, Fragment Length %d, JINF Fragment Status %04x, Event %04x\n", det, length, stat, node_data[0]);
	for (i=0; i<length-1; i++) {
	  if ( i%16 == 0 ) {
	    if ( i == 0 )  printf(" %04x:", i);
	    else           printf("\n %04x:", i);
	  }
	  printf(" %04x", node_data[i]);
	}
	printf("\n");
#endif
	*/

	goto lv3_compress;

      }

      i = 0;
      while ( i < length-5 ) {

	reg  = ptr[i];
	stat = ptr[i + reg];

	lv3_xdr_proc( (0x7FFF&det) + (stat&0x1f), ptr[i], &ptr[i+1], mc);
	i += ptr[i] + 1;

      }

    } else {                                   // this is xDR

      ptr = &node_data[1];                     // skip event number

      // check event number
      if ( (event_id&0xFFFF) != node_data[0] ) {

	// Event mismatch between JINJ and JMDC
        err_event    = 1;
        err_run      = 1;

	/*
#ifdef OFFLINE
	printf("\n");
	printf("Detector %04x, Fragment Length %d, CDP Fragment Status %04x, Event %04x\n", det, length, stat, node_data[0]);
	for (i=0; i<length-1; i++) {
	  if ( i%16 == 0 ) {
	    if ( i == 0 )  printf(" %04x:", i);
	    else           printf("\n %04x:", i);
	  }
	  printf(" %04x", node_data[i]);
	}
	printf("\n");
#endif
	*/

	goto lv3_compress;

      }

      lv3_xdr_proc( det, length-2, ptr, mc);       // this is CDP
    }

  } else {

    err_proc    = 1;

  }


 lv3_compress:

  return 0;

}


unsigned long long DAQlv3::pack(){

unsigned long long out=0;


out|= err_proc?(1ULL<<63):0;
out|= err_run?(1ULL<<62):0;
out|= err_event?(1ULL<<61):0;
out|= err_tdr?(((unsigned long long)err_tdr&255)<<48):0;
out|= err_udr?((unsigned long long)(err_udr&255)<<40):0;
out|= err_rdr?((unsigned long long)(err_rdr&255)<<32):0;
out|= err_edr?((err_edr&255)<<24):0;
out|= err_sdr?((err_sdr&255)<<16):0;
out|= err_lv1?((err_lv1&255)<<8):0;
out|= (err_etrg&255);
if(out){
//cerr<<" "<<out<<endl;
}
return out;

}


