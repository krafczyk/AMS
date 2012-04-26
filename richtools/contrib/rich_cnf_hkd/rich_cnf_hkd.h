#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef __cplusplus
extern "C" {
#endif


/* JMDC names */
const char* _JMDC[]= {      "",       "",       "",       "",
                            "",       "",       "",       "",
                            "",       "",       "",       "",
                      "JMDC-0", "JMDC-1", "JMDC-2", "JMDC-3",
                            "",       "",       "",       "",
                            "",       "",       "",       ""
                    }; 

/* JINJ names */
const char* _JINJ[]= { "JINJ-0  ",   "JINJ-1",   "JINJ-2",   "JINJ-3",
                       "JINJ-P-A", "JINJ-P-B", "JINJ-S-A", "JINJ-S-B"};

/* JINF names */
const char* _JINF[]= {
                      "JINF-E0-A",  "JINF-E0-B", "JINF-E0-P", "JINF-E0-S", 
                      "JINF-E1-A",  "JINF-E1-B", "JINF-E1-P", "JINF-E1-S", 

                      "JINF-R0-A",  "JINF-R0-B", "JINF-R0-P", "JINF-R0-S",
	              "JINF-R1-A",  "JINF-R1-B", "JINF-R1-P", "JINF-R1-S",

                      "JINF-T0-A",  "JINF-T0-B", "JINF-T0-P", "JINF-T0-S",  
                      "JINF-T1-A",  "JINF-T1-B", "JINF-T1-P", "JINF-T1-S",  
                      "JINF-T2-A",  "JINF-T2-B", "JINF-T2-P", "JINF-T2-S",  
                      "JINF-T3-A",  "JINF-T3-B", "JINF-T3-P", "JINF-T3-S",  
                      "JINF-T4-A",  "JINF-T4-B", "JINF-T4-P", "JINF-T4-S",  
                      "JINF-T5-A",  "JINF-T5-B", "JINF-T5-P", "JINF-T5-S",  
                      "JINF-T6-A",  "JINF-T6-B", "JINF-T6-P", "JINF-T6-S",
                      "JINF-T7-A",  "JINF-T7-B", "JINF-T7-P", "JINF-T7-S",  

                      "JINF-U0-A",  "JINF-U0-B", "JINF-U0-P", "JINF-U0-S",  
                      "JINF-U1-A",  "JINF-U1-B", "JINF-U1-P", "JINF-U1-S"  
                  };


/* USCM JPD names */
const char* _JPD[]= {
                     "JPD-A",  "JPD-B",  "JPD-P", "JPD-S"
                  };
 
/* JINJ Links */

const char* _JINJLNK[]={"JINFT2",   "JINFT3", "JINFU1", "JINFT0", 
                        "SDR-1A",   "SDR-1B", "SDR-0A", "SDR-0B",
                        "JINFU0",   "JINFT1", "JINFR0", "JINFR1",
                        "JINFE0",   "JINFE1", "JLV1-A", "JLV1-B",
                        "JINFT4",   "JINFT5", "SDR-2A", "SDR-2B",
                        "SDR-3A",   "SDR-3B", "JINFT6", "JINFT7"};


/* JINJ Reduced Node Addr. -> A/B side (A=1, B=0) and aJINR (0-3) maps (default AMSnode Table assumed) */
int _JLINK_SMAP[] = {   0,        1,        1,       0,
                        1,        1,        0,       0};
int _JLINK_JINR[2][2] = { { 1, 0 }, {2, 3} };

/* JINR Reduced Node Addr. -> A/B side map (A=1, B=0) (dflt. AMSnode Table assumed for logical addr.) */
int _NADDR_SMAP[] = {      1,           0,           1,           0,
                           1,           0,           1,           0};
/* JINR Reduced Node Addr. -> aJINR map (dflt. AMSnode Table assumed for logical addr.) */
int _NADDR_JINR[] = {      0,           1,           0,           1,
                           3,           2,           3,           2};
/* aJINR -> JINR Reduced Node Addr. map (physical node Addresses)*/
int _JINR_INDEX[] = {  0,  1,  5,  4};

/* RDR Links in JINR-A/B sides */
int _RLINKA[2][24]={
                    7,  5, -1, -1,  6,  9, -1, -1, 10, 11, -1, -1,  8,  2, -1, -1,  1,  0, -1, -1,  3,  4, -1, -1,
                    7, 11, -1, -1,  6,  9, -1, -1, 10,  5, -1, -1,  8,  2, -1, -1,  1,  0, -1, -1,  3,  4, -1, -1
                   };

int _RLINKB[2][24]={ 
	            7, 11, -1, -1,  6,  9, -1, -1, 10,  5, -1, -1,  8,  2, -1, -1,  1,  0, -1, -1,  3,  4, -1, -1,
                    7,  5, -1, -1,  6,  9, -1, -1, 10, 11, -1, -1,  8,  2, -1, -1,  1,  0, -1, -1,  3,  4, -1, -1 
                   };

/* FLIGHT SPARE NODES */
/*               T T U T  S S S S  U T R R  E E J J  T T S S  S S T T */
/*               | | | |  1 1 0 0  | | | |  | | | |  | | 2 2  3 3 | | */
/*               2 3 1 0  A B A B  0 1 0 1  0 1 A B  4 5 A B  A B 6 7 */
int FSJINF[] = { 1,1,1,1, 1,1,1,1, 1,1,1,0, 1,1,1,1, 1,1,1,1, 1,1,1,1 };

/*              JINF-R0-A JINF-R0-B JINF-R1-B JINF-R1-A */
int FSJINR[] = {        1,        0,        0,        0 };

/*               0-00 0-01 0-02 0-03 0-04 0-05 0-06 0-07 0-08 0-09 0-10 0-11 */
int FSRDR[]  = {    0,   0,   0,   1,   1,   1,   0,   0,   0,   0,   0,   0,
/*               1-00 1-01 1-02 1-03 1-04 1-05 1-06 1-07 1-08 1-09 1-10 1-11 */
                    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 };

/* JINR Node Names */
const char* _JINR[]= { "JINF-R0-A", "JINF-R0-B", "JINF-R0-P", "JINF-R0-S",
      	               "JINF-R1-A", "JINF-R1-B", "JINF-R1-P", "JINF-R1-S"};

/* RDR Node Names  */
const char* _RDR[2][12]={ "RDR-0-00", "RDR-0-01", "RDR-0-02", "RDR-0-03", "RDR-0-04", "RDR-0-05",
  	 	          "RDR-0-06", "RDR-0-07", "RDR-0-08", "RDR-0-09", "RDR-0-10", "RDR-0-11",
                          "RDR-1-00", "RDR-1-01", "RDR-1-02", "RDR-1-03", "RDR-1-04", "RDR-1-05",
		          "RDR-1-06", "RDR-1-07", "RDR-1-08", "RDR-1-09", "RDR-1-10", "RDR-1-11"
                        };

/* RDR PMTs */
int _PMTS[2][12]={  31, 27, 27, 27, 31, 27, 31, 27, 27, 27, 31, 27,
                    31, 27, 27, 27, 31, 27, 31, 27, 27, 27, 31, 27
                };

const char* _DATA[4] = { "N/A", "RAW", "COMPRESSED", "MIXED" };

const char* _RICH_CALIB_TABLE[5] = {"PEDESTAL Gx1", "PEDESTAL Gx5", "THRESHOLD",
                              "SIGMA Gx5 (x1024)", "STATUS" };

const char* _GAIN[2] = { "Gx5", "Gx1" };

/* RDR TYPE FOR FE LV MONITOR */
int _RDR_FE_LV[2][24] = {
                        -1, 0, 2, 4,-1,  5, -1, 6, 8,10,-1, 11, -1,12,14,16,-1, 17, -1,18,20,22,-1, 23,
                        -1, 1,-1, 3,-1, -1, -1, 7,-1, 9,-1, -1, -1,13,-1,15,-1, -1, -1,19,-1,21,-1, -1 
                        };

int _RDR_TYPE[24] = { 0, 2, 1, 2, 0,  3,  0, 2, 1, 2, 0,  3,  0, 2, 1, 2, 0,  3,  0, 2, 1, 2, 0,  3 };

#ifdef __cplusplus
}
#endif

#endif


#ifndef READ_RAW_H
#define READ_RAW_H

#include <string.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAXV(a,b) ((a)>(b)?(a):(b))
#define MINV(a,b) ((a)<(b)?(a):(b))

#define FELV_CODE_TO_VOLT(x) (5750./4750*(3.*(x)/4096+2.))

/* AMSBlock Header Data Types */

/* This is a bad feature of the current AMSBlock assembly organisation ... */

/* unassembled blocks : may not follow any prescription!! */
#define SCIE_DATA 0x05
#define CALI_DATA 0x13       /* calibration data type (JMDC tests Aug 2009) */
#define CONF_DATA  0x14      /* configuration data type (JTCrate tests May 2009) */
#define DAQHK_DATA 0x03      /* DAQ HouseKeeping data type (JTBX tests Sep 2009) */

/* CAN Data Types */
#define DTS_S_DATA 0x18
#define DTS_L_DATA 0x19

/* JMDC assembled blcoks : AMS Group Command (Evelop) Reply */
#define JMDC_ENVELOP_DATA 0x1F0380

/* JMDC assembled blocks : AMS Block Header (Envelop) */
#define JMDC_SCIE_DATA  0x05
#define JMDC_CALI_DATA  0x06
#define JMDC_CONF_DATA  0x07
#define JMDC_DAQHK_DATA 0x08

/* JMDC assembled blocks : JMDC Subblock */
#define JMDC_BLOCK        0x05 /* JMDC block data type after May 7th 2009 (A. Kunin dixit)*/
#define JMDC_BLOCK_BASIC  0x10 /* JMDC (sub)block data type : basic info (after Oct. 2009) */
#define JMDC_BLOCK_LV3    0x11 /* JMDC (sub)block data type : basic info + LV3 (after Oct. 2009) */
#define JMDC_BLOCK_GPS    0x12 /* JMDC (sub)block data type : basic info + GPS (after Oct. 2009) */
#define JMDC_BLOCK_LV3GPS 0x13 /* JMDC (sub)block data type : basic info + LV3 + GPS (after Oct. 2009) */

/* JMDC assembled blocks : DSP node subblock (aka Subdetector Data Group) */
#define JMDC_BLOCK_SDDG   0x01 /* JMDC Subdetector Data Group block data type */
#define JMDC_BLOCK_HCODED 0x04 /* JMDC Subdetector Data Group block (Huffman Encoded) */

/* THIS IS OLD STUFF sometimes its hard to be backward compatible ... */
#define CALI_BLOCK  0x13      /* Calibration block JINJ Link Address */
#define CONF_BLOCK  0x14      /* Configuration block JINJ level after May 7th 2009 (A. Kunin dixit)*/
#define DAQHK_BLOCK 0x03      /* HouseKeeping block JINJ level (JTBX tests Sep 2009) */

/******************************   Node Addresses ************************/

/* Node Address Range */
#define AMSaddrFirst 0x000
#define AMSaddrLast  0x1FF

/* JMDC node address Range */
#define JMDCaddrFirst 0x00
#define JMDCaddrLast  0x17

/* JINJ node address Range */
#define NJINJ 4
#define JINJaddrFirst 0x80
#define JINJaddrLast  0x87

/* JLV1 node address Range */
#define JLV1addrFirst 0x88
#define JLV1addrLast  0x8B

/* ETRG node address Range */
#define ETRGaddrFirst 0x8C
#define ETRGaddrLast  0x93

/* JINF node address Range */
#define JINFaddrFirst 0x96
#define JINFaddrLast  0xCD

/* xDR node address Range */
#define xDRaddrFirst  0x0CE
#define xDRaddrLast   0x1FD

/* JPD node address range */
#define JPDaddrFirst 0x028
#define JPDaddrLast  0x02B

/* Node Properties */
int is_AMSW[AMSaddrLast+1];
int is_CAN[AMSaddrLast+1];
int is_JMDC[AMSaddrLast+1];

/* JMDC names */
char JMDC[24][15];

/* JINJ names */
char JINJ[8][15];

/* JINF names */
char JINF[56][15];

/* JPD names */
char JPD[4][15];

/* JINJ Links */
#define NJINJLNK 24
enum JINJlnk{ JINFT2=0x00, JINFT3=0x01, JINFU1=0x02, JINFT0=0x03, 
              SDR_1A=0x04, SDR_1B=0x05, SDR_0A=0x06, SDR_0B=0x07,
              JINFU0=0x08, JINFT1=0x09, JINFR0=0x0a, JINFR1=0x0b,
              JINFE0=0x0c, JINFE1=0x0d, JLV1_A=0x0e, JLV1_B=0x0f,
              JINFT4=0x10, JINFT5=0x11, SDR_2A=0x12, SDR_2B=0x13,
              SDR_3A=0x14, SDR_3B=0x15, JINFT6=0x16, JINFT7=0x17
            };

char JINJLNK[24][15];

/******************************   RICH Addresses ************************/

#define NJINR 4
#define NRDR 24

#define NJINJSlaves 24
#define NJINRSlaves 12

/* JINR node address Range */
#define JINRaddrFirst 0x9E
#define JINRaddrLast  0xA5

/* RDR node address Range */
#define RDRaddrFirst 0x0F2
#define RDRaddrLast  0x109

/* JINJ Reduced Node Addr. -> A/B side (A=1, B=0) and aJINR (0-3) maps (default AMSnode Table assumed) */
int JLINK_SMAP[8];
int JLINK_JINR[2][2];

/* JINR Reduced Node Addr. -> A/B side map (A=1, B=0) (dflt. AMSnode Table assumed for logical addr.) */
int NADDR_SMAP[8];

/* JINR Reduced Node Addr. -> aJINR map (dflt. AMSnode Table assumed for logical addr.) */
int NADDR_JINR[8];

/* aJINR -> JINR Reduced Node Addr. map (physical node Addresses)*/
int JINR_INDEX[4];

/* RDR Links in JINFR-A/B sides */
int RLINKA[2][24], RLINKB[2][24];

/* JINR Node Names */
char JINR[8][15];

/* RDR Node Names  */
char RDR[2][12][15];

/* RDR PMTs */
#define NPMT_RDR 31
#define NPIX_PMT 16
#define NCHN_RDR (NPMT_RDR*NPIX_PMT)
#define NPMT 680
#define NCHN (NPIX_PMT*NPMT)

/* GRIDs */
#define NGRD 8

/* PMT DTSs */ 
#define NDTS 48
#define SIDES 2

int PMTS[2][12];

int PMTB[2][12];

char DATA[4][15];

char RICH_CALIB_TABLE[5][30];

char GAIN[2][10];

/* RDR TYPE FOR FE LV MONITOR */
int RDR_TYPE[24];
int RDR_FE_LV[2][24];

/* CONVERSION FACTORS */
#define K_DCDC 0.975
#define Q_DCDC 3.
#define K_LR   0.950
#define Q_LR   3.

/* JLV1 */
#define TJLV1toSeconds 640e-9 // 640ns/unit

/* TDV params */
#define MAXTDV 1000
#define NSPAN 4 
#define HRDLTDVSample 10000

#define RAW 1
#define COMPRESSED 2
#define MIXED 3

#define RAWBLKSIZE 993
#define CTABLESIZE 2480

#define TRUE 1
#define FALSE 0

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

int DEBUG;

int decode_block(u32 type, u16 *event, u32 size, u32 length);
int decode_AMSblock(u16 *event, u32 size, u32 length);
int decode_calib_block(u16 *event, u32 size, u32 length);
int decode_jmdc(u32 header_type, char rr, char rw, u16 node, u32 type, u16 *event, u32 size, u32 length);
int decode_sddg(char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int decode_calib(u32 header_type, char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int decode_config(u32 header_type, char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int decode_hkinfo(u32 header_type, char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int decode_hkcnf(char rr, char rw, u16 node, u32 type, u16 *event, u32 size, u32 length);
int decode_trd_block(u16 node, u16 *event, u32 length);
int decode_jlv1_block(u16 node, u16 *event, u32 length);
int decode_rich_block(u16 jnode, u16 node, u16 *event, u32 length);
int decode_rich_calib(u16 node, u16 *event, u32 length);
int decode_jinr_config(u16 jnode, u16 node, u16 *event, u32 length);
int decode_rdr_config(u16 jnode, u16 knode, u16 node, u16 *event, u32 length);
int decode_jinr_hkinfo(u16 jnode, u16 node, u16 *event, u32 length);
int decode_rdr_hkinfo(u16 jnode, u16 knode, u16 node, u16 *event, u32 length);
int decode_dts_short(char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int decode_dts_long(char rr, char rw, u16 node, u16 *event, u32 size, u32 length);
int PrintError(u16 status);
int dump_block(u16 *event, u32 length);
int decode_RXstatus(u16 RXstatus);
int decode_RXaddress(u16 RXaddress);
int decode_status(u16 status);
void fill_reply(char *cReply, u16 ReplyCode, u16 DAT);
int fill_status(char *cStatus, u16 SlaveStatus);
int fill_reply_from_statusword(char *cReply, u16 stat);
int fill_status_from_statusword(char *cStatus, u16 stat);
int is_amsw(u16 NodeAddress);
int is_can(u16 NodeAddress);
int is_jmdc(u16 NodeAddress);
int is_tas();

void PrepareFCSTable();
unsigned short DoFCS(unsigned short *data, int size);

// Huffman decoding arrays
u16 huffman_swap;
u16 huffman_first_code[16];
u16 huffman_decode_table[16][256];
u16 huffman_restore[0x8000];

u16 huffman_bit(u16 *data, u32 i);
u16 huffman_decode(u16 *istream, u16 length, u16 **dest);
void huffman_decode_init(void);

// Event Blocks
struct PrimaryHeaderBlock{
  u32 Length;
  u32 Size;
  char RR;
  char RW;
  u16 Node;
  u32 Type;
}; typedef struct PrimaryHeaderBlock PrimaryHeaderBlock;

struct SecondaryHeaderBlock{
  u16 Status;
  u16 Tag;
  u32 Time;
}; typedef struct SecondaryHeaderBlock SecondaryHeaderBlock;

struct JMDCblock{
  u32 Length;
  u32 Size;
  char RR;
  char RW;
  u16 Node;
  u32 Type;
  u16 RunTag;
  u32 RunID;
  u16 Time;
  u32 EventNo;
  u16 CmdNb;
  u16 NbCmds;
  u16 LV3[4];
  u16 GPS[5];
}; typedef struct JMDCblock JMDCblock;


struct JLV1block{
  u16 FT_pattern;
  u16 LV1_pattern;
  u16 ACC_pattern;
  u16 TOF_pattern;
  u64 Time;
  float LiveTime[2];
  u16 FTscaler;
  u16 LV1scaler;
  u16 SubLV1scaler[8];
}; typedef struct JLV1block JLV1block;

struct AMSblock{
  u16 Event;
  u32 Time;
}; typedef struct AMSblock AMSblock;

AMSblock AMSblockLast;
int AMSblockInit(AMSblock *);

struct BlockStat{
   u32 Length;
   u16 Status;
   u16 rx_sta;
   u16 rx_len;
   u16 event;
   char cstat[100];
   char estat[100];
   char xstat[100];
}; typedef struct BlockStat BlockStat;

struct RICHblock{
  u16 RichHitsGx1; 
  u16 RichHitsGx5; 
}; typedef struct RICHblock RICHblock;

struct Event{
  u32 JINJSlaves;
  u16 DataBlocks;
  u16 JMDCBlocks;
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  JMDCblock JMDC;
  JLV1block JLV1;
  float JLV1DeltaTime;
  float JMDCDeltaTime;
  BlockStat JINJ[NJINJ];
  BlockStat JINF[NJINJ][NJINJSlaves];
  BlockStat RDR[2][12];
  char cstat[1000];
  RICHblock RICH;
}; typedef struct Event Event;

Event EventFirst, EventLast, EventPrev;

void EventInit(Event *);
int EventStat(Event *);
void EventPrint(Event *);
int EventWrite(Event *);
int EventCheck(int event_jok);
int WriteError(int, Event *);

// DTS block
#define MAXDTS 64
#define NNOD 2
enum { k0x28 = 0, k0x29 = 1};
#define NBUS 4
enum { kBus3 = 0, kBus4 = 1, kBus5 = 2, kBus6 = 3 };

struct DTS {
  u16 Node;
  u32 Time;
  u16 Bus;
  u16 Ndts;
  u16 Ok[MAXDTS];
  float Temp[MAXDTS];
  u32 Age[MAXDTS];
}; typedef struct DTS DTS;

int DTSPrint(DTS *);
int DTSWrite(DTS *);

struct DallasTempSensor{
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  DTS Dallas;
}; typedef struct DallasTempSensor DallasTempSensor;

DallasTempSensor DTSinfo[NNOD][NBUS];

// Run counters
struct ShortEvent{
  u32 RunTag;
  u32 RunID;
  u32 EventNo;
  u32 Time;
  u32 msec;
}; typedef struct ShortEvent ShortEvent;

struct Run{
  u32 ReadEvents;
  u32 JINJSlaves;
  u16 FT_pattern;
  u16 LV1_pattern;
  u16 ACC_pattern;
  u16 TOF_pattern;
  double TDiffSum;
  u32 TDiffMin;
  u32 TDiffMax;
  double TJLV1DiffSum;
  u32 TJLV1DiffMin;
  u32 TJLV1DiffMax;
  ShortEvent FirstEvent;
  ShortEvent LastEvent;
  double AveEventSize;
  double MaxEventSize;
  double AveRichHitsGx5;
  u16 MaxRichHitsGx5;
  double AveRichHitsGx1;
  u16 MaxRichHitsGx1;
  u32 ReadLiveTimes;
  double AveLiveTime[2];
  double MinLiveTime[2];
  u32 ReadScalers;
  double AveFTscaler;
  double AveLV1scaler;
  double AveSubLV1scaler[8];
}; typedef struct Run Run;

Run RunFirst, RunLast;

int RunClear();
int RunReport();

// General Counters & Flags
int events_read;
int events_good;
int events_nerr;
int events_njerr;
int events_neerr;
int events_nrerr;
int events_nserr;
int events_nterr;
int events_nuerr;
int events_cerr;
int StatusErrorCounter[2][16];
int crc_error;
int event_jok;
int event_eok;
int event_rok;
int event_sok;
int event_tok;
int event_uok;

int is_Physics;

int FakeFS;

int utype_events;
int calib_events;
int daqhk_events;
int config_events;
int sci_events;
int sci_calib_events;
int sci_daqhk_events;
int sci_config_events;

char rdr_proc_mode[2];
int rdr_calib_blocks;
int rdr_config_blocks;
int rdr_daqhk_blocks;
int jinr_config_blocks;
int jinr_daqhk_blocks;
int jinr_sci_blocks;
double jinr_sci_length;

int rich_hits_Gx5;
int rich_hits_Gx1;
int rich_hits_bad;

int FT_ExtGate0;
int FT_ExtGate1;
int FT_ExtGate0x1;

int FT_pattern_cnt[16];

int ErrorClear();
int ErrorReport();

/* Ouput Handling */
FILE *pout, *perr, *pcnt;

char LogsDir[200];

char FileOutName[200], FileErrName[200], FileCntName[200];

char BlockFilePath[200], BlockFileName[200];
char BlockFileServerDir[200],BlockFileServerPath[200];
char BlockFileParentPath[200], BlockFileGrandParentPath[200];

char fdate[40];
char BlockFileDate[40], EventFirstDate[40], EventDate[40];

FILE *pjconf, *pjdaqhk, *prcalib, *prconf, *prdaqhk;
FILE *pmon;
FILE *ptagcalib, *ptagdata, *ptagrdaqhk, *ptagrconf, *ptagjdaqhk, *ptagjconf;
FILE *pfile;

char FileLogJINRConfigName[100], FileLogJINRHKinfoName[100];
char FileLogRDRConfigName[100], FileLogRDRHKinfoName[100], FileLogRDRCalibName[100];
char FileLogDTSmonName[100];

char FileMonRDRHKinfoName[100];

char FileTagRDRData[100], FileTagRDRCalib[100];
char FileTagRDRConf[100], FileTagRDRHK[100];
char FileTagJINRConf[100], FileTagJINRHK[100];
char FileTagDTSData[100];

char FileMsg[100], FileNok[100];

char FileMsgRDRData[100], FileMsgRDRCalib[100];
char FileMsgRDRConf[100], FileMsgRDRHK[100];
char FileMsgJINRConf[100], FileMsgJINRHK[100];
char FileMsgDTSData[100];
char FileMsgRMON[100];

char FileNCRDRData[100], FileNCRDRCalib[100];
char FileNCRDRConf[100], FileNCRDRHK[100];
char FileNCJINRConf[100], FileNCJINRHK[100];
char FileNCDTSData[100];
char FileNCRMON[100];

char FileOKRDRData[100], FileOKRDRCalib[100];
char FileOKRDRConf[100], FileOKRDRHK[100];
char FileOKJINRConf[100], FileOKJINRHK[100];
char FileOKDTSData[100];
char FileOKRMON[100];

char FileKORDRData[100], FileKORDRCalib[100];
char FileKORDRConf[100], FileKORDRHK[100];
char FileKOJINRConf[100], FileKOJINRHK[100];
char FileKODTSData[100];
char FileKORMON[100];

char FileTagNC[100], FileLogNC[100];
char FileTagOK[100], FileLogOK[100];
char FileTagKO[100], FileLogKO[100];

char FileSubELOG[100];
char FileTxtELOG[100];
char FileTxtPOPW[100];

char cmdELOG[100], cmdPOPW[100];

#define MAXLP 400
char lout[MAXLP][500];


/********** Configuration/HouseKeeping **********/

// RDR Calibration 
struct RDRCalTables{
  u16 PedGx1[NCHN_RDR];
  u16 PedGx5[NCHN_RDR];
  u16 Thrsld[NCHN_RDR];
  u16 SigGx5[NCHN_RDR];
  u16 Status[NCHN_RDR];
}; typedef struct RDRCalTables RDRCalTables;

struct RDRCalibration{
  u16 CalibStatus;
  RDRCalTables Tables;
  u16 Status;
}; typedef struct RDRCalibration RDRCalibration;


// Configuration Blocks
struct cpar{
  u16 Parameter;
  u16 Readback;
}; typedef struct cpar cpar;

struct RDRVerBlock{
  u16 pro_vers;
  u16 sub_vers;
}; typedef struct RDRVerBlock RDRVerBlock;

struct RDRSeqBlock{
  cpar peak_time[4];   /* 0001 - 0004 */
  cpar fepow_ctrl;     /* 0005 */
  cpar busy_ctrl;      /* 0006 */
  cpar ft_ctrl;        /* 0007 */
  cpar lv1_ctrl;       /* 0008 */
}; typedef struct RDRSeqBlock RDRSeqBlock;

struct RDRProBlock{
  cpar sig_prev;                 /* 1000 */
  cpar sig_cut;                  /* 1001 */
  cpar low_cut;                  /* 1002 */
  cpar sat;                      /* 1003 */
  cpar rich_mode;                /* 1004 */
  cpar max_len_hgh_thr;          /* 1005 */
  cpar max_len_low_thr;          /* 1006 */
  cpar low_thr_evt_len;          /* 1007 */
  cpar low_thr_evt_len_min_evts; /* 1008 */
  cpar hgh_thr_evt_len;          /* 1009 */
  cpar hgh_thr_evt_len_max_evts; /* 100A */
  cpar max_evts;                 /* 100B */
  cpar hgh_thr_mean_len;         /* 100C */
  cpar low_thr_mean_len;         /* 100D */
  cpar lv_type;                  /* 100E */
  cpar max_lv;                   /* 100F */
  cpar min_lv;                   /* 1010 */
  cpar max_chn_masked_user;      /* 1011 */
  cpar max_chn_masked;           /* 1012 */
  cpar max_chn_masked_itera;     /* 1013 */
  cpar max_chn_masked_over;      /* 1014 */
  cpar max_nb_hits_pmt;          /* 1015 */
  cpar hgh_ampl_threshold;       /* 1016 */
  cpar bad_hit_thrs;             /* 1017 */
  cpar max_nb_bad_pmts;          /* 1018 */
  cpar max_nb_bad_hits;          /* 1019 */
  cpar cal_dis_mask;             /* 101A */
  cpar lv_read_ctrl;             /* 101B */
  cpar auto_power_off;           /* 101C */
}; typedef struct RDRProBlock RDRProBlock;

struct RDRUsrBlock{
  cpar mask[31];
}; typedef struct RDRUsrBlock RDRUsrBlock;

struct RDRCalBlock{
  cpar mask[31];
}; typedef struct RDRCalBlock RDRCalBlock;

struct RDRMisBlock{
  u16 mismatches;
}; typedef struct RDRMisBlock RDRMisBlock;

struct RDRConfiguration{
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  u16 Event; /* what is this? */
  u32 Time;
  RDRVerBlock RDRVer;
  RDRSeqBlock RDRSeq;
  RDRProBlock RDRPro;
  RDRUsrBlock RDRUsr;
  RDRCalBlock RDRCal;
  RDRMisBlock RDRMis;
  u16 Status;
  u16 CRC16;
}; typedef struct RDRConfiguration RDRConfiguration;


struct JINRhvBlock{
  u16 Value[42][2];
  u16 Status[2];
  u16 Error[12];
  u16 ErrorCode[2];
  u16 Reserved[4];
}; typedef struct JINRhvBlock JINRhvBlock;

struct JINRx2Block{
  u16 SlaveMask[32][2];
  u16 SlaveMaskErrorCode[2];
  u16 Register[4][2];
  u16 RegisterErrorCode[2];
  u16 Reserved[4];
  JINRhvBlock JINRhv[4];
  u16 JINRstatus;
}; typedef struct JINRx2Block JINRx2Block;

struct JINRConfiguration{
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  u16 Event; /* what is this? */
  u32 Time;
  JINRx2Block JINRx2;
  u16 Status;
  u16 CRC16;
}; typedef struct JINRConfiguration JINRConfiguration;


// RICH Housekeeping Block
struct HKComBlock{
  u16 sw_vers_id;
  u16 sd_vers_id;
  u16 node_status;
  u16 last_evt_nb;
  u16 ave_proc_time;
}; typedef struct HKComBlock HKComBlock;

struct hkrdr{
  u16 status;
  u16 chn_masked_user;
  u16 chn_masked_iter;
  u16 chn_masked_over;
  u16 chn_masked;
  u16 min_lv_1;
  u16 max_lv_1;
  u16 min_lv_2;
  u16 max_lv_2;
  u16 proc_evts;
  u16 mean_len;
  u16 max_len;
  u16 evts_above_low_thr_evt_len;
  u16 evts_above_hgh_thr_evt_len;
  u16 pmt_status_w1;
  u16 pmt_status_w2;
  u16 bad_hits;
}; typedef struct hkrdr hkrdr;

struct hkjinr{
  u16 status;
  u16 slave_mask_error_code;
  u16 internal_reg_error_code;
  u16 rhv0_error_code;
  u16 rhv1_error_code;
  u16 rhv2_error_code;
  u16 rhv3_error_code;
  u16 rhv0_status_error5;
  u16 rhv1_status_error5;
  u16 rhv2_status_error5;
  u16 rhv3_status_error5;
}; typedef struct hkjinr hkjinr;

struct HKRdrBlock{
  u16 calib_type;
  u16 calib_status;
  hkrdr rdr;
  u16 status;
  u16 crc16;
}; typedef struct HKRdrBlock HKRdrBlock;

struct HKJinrBlock{
  u16 busy_error_h;
  u16 busy_error_l;
  hkjinr jinr;
  u16 status;
  u16 crc16;
}; typedef struct HKJinrBlock HKJinrBlock;

struct RDRHouseKeeping{
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  u16 Event; /* what is this? */
  u32 Time;
  HKComBlock HKCom;
  HKRdrBlock HKRdr;
}; typedef struct RDRHouseKeeping RDRHouseKeeping;

struct JINRHouseKeeping{
  PrimaryHeaderBlock PrimaryHeader;
  SecondaryHeaderBlock SecondaryHeader;
  u16 Event; /* what is this? */
  u32 Time;
  HKComBlock  HKCom;
  HKJinrBlock HKJinr;
}; typedef struct JINRHouseKeeping JINRHouseKeeping;

// Declare functions
int RDRCalibStatus(u32 RunID, int aCDP);

int RDRConfigInit(RDRConfiguration *);
void RDRConfigPrint(int aCDP, RDRConfiguration *, u16, int);
int RDRConfigStatus(int ID, int aCDP, RDRConfiguration *);

void RDRVerPrint(RDRVerBlock *, u16, int );
void RDRSeqPrint(RDRSeqBlock *, u16, int );
void RDRProPrint(RDRProBlock *, u16, int );
void RDRUsrPrint(RDRUsrBlock *, u16, int );
void RDRCalPrint(RDRCalBlock *, u16, int );
void RDRMisPrint(RDRMisBlock *, u16, int );

int JINRConfigInit(JINRConfiguration *);
void JINRConfigPrint(int aJINR, JINRConfiguration *);
int JINRConfigStatus(int ID, int aJINR, JINRConfiguration *);

void JINRx2Print(JINRx2Block *);

int RDRHKmonWrite(int aCDP, RDRHouseKeeping *);

int RDRHKinfoInit(RDRHouseKeeping *);
void RDRHKinfoPrint(int aCDP, RDRHouseKeeping *);
int RDRHKinfoStatus(int ID, int aCDP, RDRHouseKeeping *);

int JINRHKinfoInit(JINRHouseKeeping *);
void JINRHKinfoPrint(int aJINR, JINRHouseKeeping *);
int JINRHKinfoStatus(int ID, int aJINR, JINRHouseKeeping *);

void HKComPrint(int isCDP, HKComBlock *);
void HKRdrPrint(HKRdrBlock *, u16 sd_vers_id);
void HKJinrPrint(HKJinrBlock *);

int DTSinfoInit(DallasTempSensor *);
int DTSinfoStatus(int ID, DallasTempSensor *);

// Declare objects
JINRConfiguration JINRConfig[NJINR];
JINRHouseKeeping JINRHKinfo[NJINR];

RDRConfiguration RDRConfig[NRDR];
RDRHouseKeeping RDRHKinfo[NRDR];

RDRCalibration RDRCalibTables[NRDR];

#ifdef FELV_DEBUG
unsigned short lv_val[NRDR];
double sum_time = 0;
#endif

/* Common Data Structures */
struct DataHeader {
  u32 RunID;
  u16 RunTag;
  u32 FirstEventNo;
  u32 LastEventNo;
  u32 FirstEventTime;
  u32 LastEventTime;
  char FirstBlockFile[100];
  char LastBlockFile[100];
}; typedef struct DataHeader DataHeader;

/* STATUS CONTROL */

void set_status( u32 time, u16 node, char* name, int DT, int ERR, int npar, int* par);

#define T_PMT_LOW  (-30+5)
#define T_PMT_HGH  ( 50-5)
#define T_HVB_LOW  (-20+5)
#define T_HVB_HGH  ( 65-5)
#define T_ERPD_LOW (-20+5)
#define T_ERPD_HGH ( 50-5)

enum DATA_TYPES {
  kSCI    = 0,
  kRCALIB = 1, 
  kRCONF  = 2,
  kRDAQHK = 3,
  kJCONF  = 4,
  kJDAQHK = 5,
  kDTS    = 6
};

enum MSG_TYPES {

  /* NotChkd/OK */
  kNC           = 0,
  kOK           = 1,
  kKO           = 2,
  kALL          = 3,

  /* SCI */
  kCRC_ERR      = 100,
  kRICH_ERR     = 101,
  kJINR_CNT     = 102,

  /* JINFR */
  kSLV_MSK      = 200,
  kINT_REG      = 201,
  kERR_RHV      = 202,
  kERR5_RHV     = 203,

  /* RDR */
  kBAD_HIT      = 300,
  kPMT_STATUS   = 301,
  kCALIB_INTEG  = 302,
  kCONFIG_INTEG = 303,
  kLV_STATUS    = 304,

  /* DTS */
  kT_PMT_LOW    = 400,
  kT_PMT_HGH    = 401,
  kT_HVB_LOW    = 402,
  kT_HVB_HGH    = 403,
  kT_ERPDR_LOW  = 404,
  kT_ERPDR_HGH  = 405,
  kT_ERPDW_LOW  = 406,
  kT_ERPDW_HGH  = 407
//  kT_PMT_OK   = 400, kT_PMT_ALOW  = 401, kT_PMT_WLOW  = 402, kT_PMT_WHGH  = 403, kT_PMT_AHGH = 404,
//  kT_HVB_OK   = 410, kT_HVB_ALOW  = 411, kT_HVB_WHGH  = 412, kT_HVB_WHGH  = 413, kT_HVB_AHGH = 414,
//  kT_ERPRDR_OK= 420, kT_ERPDR_ALOW= 421, kT_ERPDR_WLOW= 422, kT_ERPDR_WHGH= 423, kT_ERPDR_AHGH=424,
//  kT_ERPRDW_OK= 420, kT_ERPDW_ALOW= 421, kT_ERPDW_WLOW= 422, kT_ERPDW_WHGH= 423, kT_ERPDW_AHGH=424
};

enum ACTIONS {
  kNONE  = 0x000,  /* do nothing */

  /* Level 0: record the alarm & notify RICH shifter */
  kALOG  = 0x001,  /* append alarm logfile   */
  kRMON  = 0x002,  /* write file for monitor */
  kELOG  = 0x004,  /* send elog entry */
  kPOPW  = 0x008,  /* notify shifter with popup window */

  /* Level 1: broadcast */
  kREXP  = 0x010,  /* notify to RICH Expert  */
  kLEAD  = 0x020,  /* notify to Shift Leader */
  
  /* Level 2: automatic actions */
  kCMD   = 0x100   /* emergency command */
};

enum ACTIONS_ON_ERR {

  kSCI_OK             = kALOG | kRMON,
  kSCI_NC             = kALOG | kRMON,
  kSCI_CRC_ERR        = kALOG | kRMON,
  kSCI_RICH_ERR       = kALOG | kRMON,
  kSCI_JINR_CNT       = kALOG | kRMON,
  kSCI_BAD_HIT        = kALOG | kRMON,

  kRCALIB_OK          = kALOG | kRMON,
  kRCALIB_NC          = kALOG | kRMON,

  kJCONF_OK           = kALOG | kRMON,
  kJCONF_NC           = kALOG | kRMON,
  kJCONF_ERR_RHV      = kALOG | kRMON,
  kJCONF_SLV_MSK      = kALOG | kRMON,
  kJCONF_INT_REG      = kALOG | kRMON,

  kJDAQHK_OK          = kALOG | kRMON,
  kJDAQHK_NC          = kALOG | kRMON,
  kJDAQHK_ERR_RHV     = kALOG | kRMON,
  kJDAQHK_ERR5_RHV    = kALOG | kRMON,
  kJDAQHK_SLV_MSK     = kALOG | kRMON,
  kJDAQHK_INT_REG     = kALOG | kRMON,

  kRCONF_OK           = kALOG | kRMON,
  kRCONF_NC           = kALOG | kRMON,
  kRCONF_CONFIG_INTEG = kALOG | kRMON,

  kRDAQHK_OK          = kALOG | kRMON,
  kRDAQHK_NC          = kALOG | kRMON,
  kRDAQHK_BAD_HIT     = kALOG | kRMON,
  kRDAQHK_PMT_STATUS  = kALOG | kRMON,
  kRDAQHK_CALIB_INTEG = kALOG | kRMON,
  kRDAQHK_CONFIG_INTEG= kALOG | kRMON,
  kRDAQHK_LV_STATUS   = kALOG | kRMON,

  kDTS_T_OK           = kALOG | kRMON,
  kDTS_T_NC           = kALOG | kRMON,
  kDTS_T_PMT_LOW      = kALOG | kRMON,
  kDTS_T_PMT_HGH      = kALOG | kRMON,
  kDTS_T_HVB_LOW      = kALOG | kRMON,
  kDTS_T_HVB_HGH      = kALOG | kRMON,
  kDTS_T_ERPDR_LOW    = kALOG | kRMON,
  kDTS_T_ERPDR_HGH    = kALOG | kRMON,
  kDTS_T_ERPDW_LOW    = kALOG | kRMON,
  kDTS_T_ERPDW_HGH    = kALOG | kRMON
};

#define MAX_ERR_WRT 1000
#define MAX_EXE_TIME 50
time_t ExeInitTime, ExeCurrTime;
double dtime;

#ifdef __cplusplus
}
#endif

#endif
