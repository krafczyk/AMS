#ifndef __AMSDBCOMMONS__
#define __AMSDBCOMMONS__

const int dbg_prtout  =        0; // debug printout flag
const int dbread_only =        1; // 0- read database, but not fill AMS classes

const int DBWriteGeom =        1; // write geometry to dbase
const int DBWriteMCEg =       10; // write mceventg
const int DBWriteMC   =      100; // write all MC banks
const int DBWriteRawE =     1000; // write raw event
const int DBWriteRecE =    10000; // write reconstructed event
const int DBWritePart =   100000; // write particle info.
const int DBWriteMCEv =  1000000; // write MC event
const int DBWriteDumm = 10000000; // write dummy event, debugging only

#endif
