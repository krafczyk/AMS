//  $Id: db_comm.h,v 1.5 2001/01/22 17:32:29 choutko Exp $
#ifndef __AMSDBCOMMONS__
#define __AMSDBCOMMONS__

const int dbg_prtout  =        0; // debug printout flag
const int dbread_only =        0; // 0- read database, but not fill AMS classes

const int DBWriteGeom =        1; // write geometry to dbase
const int DBWriteMCEg =       10; // write mceventg
const int DBWriteMC   =      100; // write all MC banks
const int DBWriteRawE =     1000; // write raw event
const int DBWriteRecE =    10000; // write reconstructed event
const int DBWritePart =   100000; // write particle info.
const int DBWriteSlow =  1000000; // write slow control block

const int DBReadSetup =        1; // read setup from dbase


const int TagEvent    =        1;
const int RawEvent    =       10;
const int RecoEvent   =      100;

#endif
