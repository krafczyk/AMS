//  $Id: daqs2block.h,v 1.11 2009/02/20 14:12:20 choutko Exp $
// 1.0 version 2.07.97 E.Choumilov

#ifndef __AMSDAQS2BLOCK__
#define __AMSDAQS2BLOCK__
#include "typedefs.h"
#include "link.h"
#include "tofdbc02.h"
//
// 
//
// S-block class:
//
class DAQS2Block {
private:
//  static int16u nodeids[2*TOF2GC::SCCRAT];// valid node_id's (incl. a/b-sides) for Data
//  static int16u nodeidsP[4*TOF2GC::SCCRAT];// valid node_id's (incl. a/b-sides,prim,sec) for OnBoard-Peds
  static int16u format;// currently processing DATA-format: 0-raw, 1-compressed, 2-mixed, 3-pedcal
  static integer totbll;//total length of all(MAX 4) scint. blocks in event
#pragma omp threadprivate (totbll)
//specific for calib-jobs:
  static uinteger _PrevRunNum;
  static bool _FirstPedBlk;
  static integer _NReqEdrs;
  static integer _FoundPedBlks;
  static integer _GoodPedBlks;
  static integer _PedBlkCrat[TOF2GC::SCCRAT];
  static bool _CalFirstSeq;
public:
  static void clrtbll(){totbll=0;};
  static integer gettbll(){return totbll;};
//
  static void setrawf(){format=0;}
  static void setcomf(){format=1;}
  static void setmixf(){format=2;}
  static void setpedf(){format=3;}
  static int16u getformat(){return format;}
  static void node2crs(int16u nodeid, int16u &cr, int16u &sd);
  static void node2crsP(int16u nodeid, int16u &cr, int16u &sd);
  static integer checkblockid(int16u id);
  static integer checkblockidP(int16u id);
  static void buildraw(integer len, int16u *p);
  static void buildonbP(integer len, int16u *p);
  static integer getportid(int16u crat, int16u sdrside);
  static integer getnodeid(int16u crat, int16u sdrside);
  static integer getmaxblocks();
  static integer calcblocklength(integer ibl);
  static void buildblock(integer ibl, integer len, int16u *p);
  static void EventBitDump(integer leng, int16u *p, char * message);
//Calib-jobs specific:
  static uinteger & PrevRunNum(){return _PrevRunNum;}
  static integer & NReqEdrs(){return _NReqEdrs;}
  static bool & FirstPedBlk(){return _FirstPedBlk;}
  static integer & FoundPedBlks(){return _FoundPedBlks;}
  static integer & GoodPedBlks(){return _GoodPedBlks;}
  static integer & PedBlkCrat(int ic){return _PedBlkCrat[ic];}
  static bool & CalFirstSeq(){return _CalFirstSeq;}
};

#endif
