#ifndef __RICHDAQ__
#define __RICHDAQ__

#include "typedefs.h"
#include "richdbc.h"
#include "job.h"

#define Do(x) ({if(Emit(x)) return;})

class DAQRichBlock{
  // Varibles
  static int JINFId[RICH_JINFs];
  static int Links[RICH_JINFs][RICH_LinksperJINF];                    // These are the link id to physical RDR

  class StatusParser{  // This class implements the status parser
  public:
    uint slaveId;
    uint isCDP;
    uint isRaw;
    uint isCompressed;
    uint errors;
    /*********** UNUSED **************
    uint cumulative;     
    uint sequenceError;  
    uint FEPowerFault;   
    uint timeoutError;   
    uint AMSWError;      
    uint assemblyError;  
    uint CRCError;       
    *********************************/
    uint isData;

  private:
    void assign(int16u status){
      isData=status&(0x8000);
      errors=status&(0x7F00);
      isCompressed=status&(0x0080);
      isRaw=status&(0x0040);
      isCDP=status&(0x0020);
      slaveId=(status&(0x001F));
    }

  public:
    StatusParser(int16u status){assign(status);}
    StatusParser(){assign(0);}
    int  operator=(int16u status){assign(status);return 1;}
  };                                      



  class FragmentParser{                 // This class implements the Fragment parser and navigator
  public:
    int length;                         // Fragment length
    int16u *data;                       // Pointer to the position of data
    int16u *next;                       // Position of next fragment (length)
    DAQRichBlock::StatusParser status;  // The parsed status 
    
    FragmentParser(int16u *pointer){
      length=*pointer;
      data=pointer+1;
      next=pointer+1+length;            //+1 is because length does not count itself
      status=*(pointer+length); 
    }
  };

  class DSPRawParser{
  private:
    int16u *_root;
    int _current_record;

    void parse();
  public:
    int pmt;
    int pixel;
    int gain;
    int counts;

    DSPRawParser(int16u *p):_root(p),_current_record(0){};
    int Next(){};                 // This fill t he variables pmt,pixel,gain and counts
  };


  class DSPCompressedParser{
  };


  enum {
    kOk,kLengthError,kDataError,kJINFIdError,              //  Possible error codes
    kTruncated,kRDRError,kCalibration,kMixedMode,
    kJINFError
  };                       

  static int Emit(int code);                              // Perform task accordingly to erro. Return 1 if should exit  
 public:
  // Declaration of functions (that can be seen from outside)
  // RAW -> DAQ

  static integer checkdaqid(int16u id);
  static void buildraw(integer length,int16u *p);

  // DAQ -> RAW: uninplemented
  static integer getmaxblocks(){return 0;}
  static integer calcdaqlength(int i){return 0;}
  static void builddaq(integer i,integer length,int16u *p){return;}

  static int Status;                                                     //  Exit status for debugging
};

#endif
