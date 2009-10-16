#ifndef  __TRDAQ__
#define __TRDAQ__

#include "TkDBc.h"
#include "typedefs.h"

/// TrackerInterface for communicating with DAQ binary formats
class TrDAQ{
public:
  //=========== DAQ Interface ===============
  
  /// Returns the number of DAQ blocks produce by the tracker
  static integer getmaxblocks(){return trconst::ncrt;}

  /// Returns the number of DAQ blocks produce by the tracker
  static integer getmaxblocksRaw(){return trconst::ncrt;}

  /// It Returns the JINJ slave port number for a 
  /// given TCrate (0-7) or produce an error code (-1)
  static integer getdaqid(int16u crt);

  /// Service method to getdaqid
  static integer checkdaqid(int16u id);
  
  /// It Calculate the (foreseen) Daq size for a crate from the TrRawCluster lists 
  static integer calcdaqlength(integer cratenum);
  /// It builds a DAQ like (binary) output from  the TrRawCluster lists 
  static void builddaq(integer crate, integer size, int16u *p);

  /// It decodes the DAQ (binary) data and produces the TrRawClusters
  static void buildraw(integer n, int16u *p);

private:
  /// The Actual TDR decoding routine
  static int ReadOneTDR(int16u* pp,int tsize,int cratenum,int pri);
  /// Checks the status word for errors
  static int TestBoardErrors(char *name,int16u status,int pri=1);

  

};

#endif
