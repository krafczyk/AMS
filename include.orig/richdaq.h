#ifndef __RICHDAQ__
#define __RICHDAQ__

#include "typedefs.h"
#include "richdbc.h"
#include "job.h"
#include "root.h"

#define Do(x) ({if(Emit(x)) return;})

class DAQRichBlock{
  // Varibles
  static integer _Calib[2][24];
  static int JINFId[RICH_JINFs];
  static int Links[RICH_JINFs][RICH_LinksperJINF];                    // These are the link id to physical CDP
  static int JINFNodes[2][RICH_JINFs][2];                             // These are the link id to physical CDP
  static int FirstNode;   // Node number for CDP 0
  static int LastNode;   // Node number for CDP 23

  class StatusParser{  // This class implements the status parser
  public:
    uint slaveId;
    uint isCDP;
    uint isRaw;
    uint isCompressed;
    uint isCumulative;
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
      errors=status&(0x7E00);
      isCumulative=status&(0x0100);
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
      // Dump the fragment for testing
      //      for(int i=0;i<=(*pointer);i++){
	//printf("%i %x\n",i,*(pointer+i));
      //      }

      //      length=(*pointer)/sizeof(uint16); // Count length in words instead of bytes
      length=(*pointer);  // Count length in words instead of bytes (the CDP provided the length in words directly)
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

    DSPRawParser(int16u *p):_root(p),_current_record(0){parse();}
    int Next(){
      const int max_record=31*16*2; // Max Number of pmts per CDP * 16 channels * 2 gains
      _current_record++; 
      if(_current_record>=max_record) return 0;
      parse();
      return 1;
    };                 // This fill t he variables pmt,pixel,gain and counts
  };


  class DSPCompressedParser{
  private:
    int16u *_root;
    int _current_record;
    int _length;

    void parse();
  public:
    int pmt;
    int pixel;
    int gain;
    int counts;
    int inconsistent;

    DSPCompressedParser(int16u *p,int16u length):_root(p),_current_record(0),_length(length){parse();}
    int Next(){
      _current_record+=2; 
      if(_current_record>=_length) return 0;
      parse();
      return 1;
    };                 // This fill t he variables pmt,pixel,gain and counts
  };


  enum {
    kOk,kLengthError,kDataError,kJINFIdError,              //  Possible error codes
    kTruncated,kCDPError,kCalibration,kMixedMode,
    kJINFError,kCDPRawTruncated,kWrongCDPChannelNumber
  };                       

  static int Emit(int code);                              // Perform task accordingly to erro. Return 1 if should exit  
 public:
  // Declaration of functions (that can be seen from outside)
  // DAQ -> RAW
  static integer checkdaqid(int16u id);
  static void buildraw(integer length,int16u *p);
  static integer checkdaqidnode(int16u id);
  static void buildrawnode(integer length,int16u *p);
  static void DecodeRich(integer length,int16u *p,int side,int secondary);

  // CALIBRATION
  static integer checkcalid(int16u id);
  static void buildcal(integer length,int16u *p);;

  // RAW -> DAQ: uninplemented
  static integer getdaqid(int16u crate);
  static integer getmaxblocks(){return 2;}
  static integer calcdaqlength(int i);
  static void builddaq(integer i,integer length,int16u *p);

  // Histograms for raw mode
  static TH1F **daq_histograms;
  
  static int Status;                                                     //  Exit status for debugging
};

#endif
