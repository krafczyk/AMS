// 1.0 version 2.07.97 E.Choumilov

#ifndef __AMSDAQS2BLOCK__
#define __AMSDAQS2BLOCK__
#include <typedefs.h>
#include <iostream.h>
#include <link.h>
#include <fstream.h>
#include <tofdbc02.h>
//
// 
//
// S-block class:
//
class DAQS2Block {
private:
  static int16u subdmsk[TOF2GC::DAQSBLK];//subd.bit_mask, lsbits= ...111 -> ... ctc/anti/tof
  static int16u slotadr[TOF2GC::DAQSSLT];// h/w address of slot
  static int16u tofmtyp[TOF2GC::SCTOFC][4];// TOF meas.assignment inside each TOFchannel in SFET
  static int16u tempch[TOF2GC::SCCRAT][TOF2GC::SCSFET];// TOF-channel(1-4) with temperature(=0 if true TOF)   
  static int16u sblids[TOF2GC::DAQSFMX][TOF2GC::DAQSBLK];// valid block_id's for each format
  static integer format;// current TDC-format: 0-raw, 1-reduced
  static number rwtemp[TOF2GC::DAQSTMX];// raw temp. vs global temp-channel number
  static integer totbll;//total length of all 8 blocks
public:
  static void clrtbll(){totbll=0;};
  static integer gettbll(){return totbll;};
  static integer isSFET(int16u sla);// for slot-address "sla"(h/w !!!) answer "SFET/no" (1/0)
  static integer isSFEC(int16u sla);// for slot-address "sla" answer "SFEC/no" (1/0)
  static integer isSFEA(int16u sla);// for slot-address "sla" answer "SFEA/no" (1/0)
  static int16u gettempch(int16u cra, int16u sfet);// gives TOFch(0,1-4!!!) with temper-info
  static int16u isSFETT(int16u cra, int16u sla);//sequential number of "temperature"-slot
//                                     or 0 (if not temp-SFET) (2/1/0) for crate/slad 
  static int16u isSFETT2(int16u cra, int16u sln);//sequential number of "temperature"-slot
//                                     or 0 (if not temp-SFET) (2/1/0) for crate/slnum 
  static int16u sladdr(int16u sln){// return h/w slotaddr for sequential slot number "sln"
    return slotadr[sln];
  };
  static int16u slnumb(int16u sla){// return sequential slot number for h/w slotaddr "sla"
    for(int i=0;i<TOF2GC::DAQSSLT;i++){
      if(slotadr[i]==sla)return(i);
    }
    return(TOF2GC::DAQSSLT);//=TOF2GC::DAQSSLT if sla was invalid
  };
  static void puttemp(int16u chan, integer val){
    rwtemp[chan]=number(val);
  };
  static number gettemp(int16u chan){
    number val;
    val=rwtemp[chan];
    return val;
  };
  static int16u getdmsk(int16u blk){
    return subdmsk[blk];
  };
// function to get meas.type inside(tdcc=0-3) of each 4 TOF-channel(0-3) in SFET 
  static int16u mtyptof(int16u tofc, int16u tdcc){// 
    return tofmtyp[tofc][tdcc];
  }
//
  static integer checkblockid(int16u id);
  static void buildraw(integer len, int16u *p);
  static integer getmaxblocks();
  static integer calcblocklength(integer ibl);
  static void buildblock(integer ibl, integer len, int16u *p);
};

#endif
