// 1.0 version 16.11.2006 by E.Choumilov

#ifndef __AMSDAQECBLOCK__
#define __AMSDAQECBLOCK__
#include "typedefs.h"
#include "link.h"
#include "ecaldbc.h"
// 
//
// EC-block class:
//
class DAQECBlock {
private:
  static int16u nodeids[ecalconst::ECRT];// valid JINF-node_id's (1x2crates)
  static int16u format;// currently processing DATA-format: 0-raw, 1-compressed, 2-mixed
  static integer totbll;//total length of all(MAX 4) blocks
//specific for pedcalib-jobs:
  static uinteger _PrevRunNum;
  static bool _FirstPedBlk;
  static integer _NReqEdrs;
  static integer _FoundPedBlks;
  static integer _GoodPedBlks;
  static integer _PedBlkCrat[ecalconst::ECRT][ecalconst::ECEDRS];
  static bool _CalFirstSeq;
public:
  static void clrtbll(){totbll=0;};
  static integer gettbll(){return totbll;};
//
  static void setrawf(){format=0;}
  static void setcomf(){format=1;}
  static void setmixf(){format=2;}
  static int16u getformat(){return format;}
  static void node2crs(int16u nodeid, int16u &cr);
  static integer checkblockid(int16u id);
  static integer checkblockidJ(int16u id);
  static integer checkblockidP(int16u id);
  static integer getportid(int16u crat);
  static void buildraw(integer len, int16u *p);
  static void buildrawJ(integer len, int16u *p);
  static void buildonbP(integer len, int16u *p);
  static void buildonbP2(integer len, int16u *p);
  static void buildonbP1(integer len, int16u *p);
  static integer getmaxblocks();
  static integer calcblocklength(integer ibl);
  static void buildblock(integer ibl, integer len, int16u *p);
//  static void frbdump(int16u *p, int16u len, char name);//fragment bits-dump 
  static void EventBitDump(integer leng, int16u *p, char * message);
//Calib-jobs specific:
  static uinteger & PrevRunNum(){return _PrevRunNum;}
  static integer & NReqEdrs(){return _NReqEdrs;}
  static bool & FirstPedBlk(){return _FirstPedBlk;}
  static integer & FoundPedBlks(){return _FoundPedBlks;}
  static integer & GoodPedBlks(){return _GoodPedBlks;}
  static integer & PedBlkCrat(int ic,int edr){return _PedBlkCrat[ic][edr];}
  static bool & CalFirstSeq(){return _CalFirstSeq;}
};

#endif
