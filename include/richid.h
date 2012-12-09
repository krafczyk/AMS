#ifndef __RICHID__
#define __RICHID__


// NEXT DEFINE CONTROL THE USE OF THE OLD OR THE NEW MANAGEMENT OF THE
// RICH PMT INFORMATION
//#define __USERICHPMTMANAGER__  // Activate the new code

#include "richdbc.h"
#include "cfortran.h"

#define private public

// Define the calls to richpmtlib
PROTOCCALLSFFUN3(FLOAT,PDENS,pdens,FLOAT,FLOAT,FLOAT)
#define PDENS(A1,A2,A3) CCALLSFFUN3(PDENS,pdens,FLOAT,FLOAT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB4(GETRMURSG,getrmursg,FLOAT,FLOAT,PFLOAT,PFLOAT)
#define GETRMURSG(A1,A2,A3,A4) CCALLSFSUB4(GETRMURSG,getrmursg,FLOAT,FLOAT,PFLOAT,PFLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB4(GETRLRS,getrlrs,FLOAT,FLOAT,PFLOAT,PFLOAT)
#define GETRLRS(A1,A2,A3,A4) CCALLSFSUB4(GETRLRS,getrlrs,FLOAT,FLOAT,PFLOAT,PFLOAT,A1,A2,A3,A4)


#define _assert(x) {if(!(x))throw 1;}

///////////////////////////////
/// NEW VERSION OF THE CODE ///
///////////////////////////////

// This class contains all the geometric information for all the channels, as well
// as the information for simulation
// 

class RichPMTChannel;

class UnifiedRichChannelCalibration{
 public:
  geant pedx1;
  geant pedx5;
  geant sigmax1;
  geant sigmax5;
  geant thresholdx1;
  geant thresholdx5;
  geant status;
};

class UnifiedRichCalibration{
 public:
  static UnifiedRichChannelCalibration calibration[RICmaxpmts*RICnwindows];
  static const uint firstRun=1356998400; // 00:00:00 Jan 1st 2013 GMT
  //  static const uint firstRun=1300000000; // 00:00:00 Jan 1st 2013 GMT
  static void fillArrays();
  // A copy of the old data structure
  static int   _status[RICmaxpmts*RICnwindows];                  // Channel status word (this is (good?1:0)+10*other information) other information could include all the stuff related to the calibration process
  static geant _pedestal[2*RICmaxpmts*RICnwindows];              // Pedestal position (x2 gains) high,low
  static geant _pedestal_sigma[2*RICmaxpmts*RICnwindows];        // Pedestal width (x2 gains)
  static geant _pedestal_threshold[2*RICmaxpmts*RICnwindows];    // Pedestal threshold width in pedestal sigma units (x2 gains)
  static void fillArraysOld();
};


class RichPMT{
 private:
  int _geom_id;
  int _pos;
  int _orientation;
  int _pmtaddh;
  int _pmtaddc;
  int _pmtnumb;

  int BSearch(int channel,int mode,geant value);

  geant _global_position[3];                           // Position with respect to global system
  geant _local_position[3];                            // Position with respect to grid system (unused)
  geant _channel_position[RICnwindows][3];             // Channel global position as a function of the geometric channel id
  geant _cumulative_prob[RICnwindows][2][RIC_prob_bins];  // Cumulative probability for this channel
  geant _step[RICnwindows][2];                                      // Step size for the cumulative distribution   
  int _channel_id2geom_id[RICnwindows];       // Table to convert from channel id to geometric channel id


  int grid(){return _pos/1000;}

  //
  // Temporal members
  //
  int _current_channel;
  int _current_mode;
  int _filled;

 public:
  RichPMT():_current_channel(0),_current_mode(0),_filled(0){};
  static void Finish(){};
  geant SimulateSinglePE(int channel,int mode);
  geant SimulateSinglePE(int mode){_assert(_filled);return SimulateSinglePE(_current_channel,mode);}
  geant SimulateSinglePE(){_assert(_filled);return SimulateSinglePE(_current_channel,_current_mode);}
  void compute_tables(bool force=true);

  friend class RichPMTsManager;
  friend class RichPMTChannel;
};


//
// This class contains all the calibration information for all the channels
// and the pointers to the PMTs. It also contains further information for simulation
// and the TDV tables
// 

class RichPMTsManager{
 private:
  // All the necessary numbers                                                              
  static RichPMT _pmts[RICmaxpmts];           // The tiles themselves
  static geant _max_eff;   // Maximum relative efficiency
  static geant _mean_eff;

  //
  // TDV tables
  //
  // It is assumed that the TDV tables are sorted accroding to geometric ID
  // That is, pmts from 0 to 679 (look at Init) and channels
  // in the following order
  //
  //       12 13 14 15
  //        8  9 10 11
  //        4  5  6  7
  //        0  1  2  3
  //

  static int   _status[RICmaxpmts*RICnwindows];                  // Channel status word (this is (good?1:0)+10*other information) other information could include all the stuff related to the calibration process
  static int   _mask[RICmaxpmts*RICnwindows];                    // Channel offline mask,if==0 the channel is not used in  the reconstruction although it was not masked out in the calibration
  static geant _pedestal[2*RICmaxpmts*RICnwindows];              // Pedestal position (x2 gains) high,low
  static geant _pedestal_sigma[2*RICmaxpmts*RICnwindows];        // Pedestal width (x2 gains)
  static geant _pedestal_threshold[2*RICmaxpmts*RICnwindows];    // Pedestal threshold width in pedestal sigma units (x2 gains)
  static geant _sim_gain[2*RICmaxpmts*RICnwindows];                  // Gain (x2 gains) high,low
  static geant _gain[2*RICmaxpmts*RICnwindows];                  // Gain (x2 gains) high,low
  static geant _gain_sigma[2*RICmaxpmts*RICnwindows];            // Gain width (x2 gains)
  static int _gain_threshold[RICmaxpmts*RICnwindows];            // Gain threshold 
  static geant _relative_efficiency[RICmaxpmts*RICnwindows];     // Efficiency with respect to an arbitrary PMT 

  static short int _rdr_starts[RICH_JINFs*RICH_CDPperJINF];            // In which PMT (geom id) starts each RDR
  static short int _rdr_pmt_count[RICH_JINFs*RICH_CDPperJINF];         // How many pmts this RDR has 

  static time_t _eff_begin,_eff_insert,_eff_end;  // Monitor changes eff tables in order to
                                                  // recompute the efficiency tables 


  // Look up tables
  static int _grid_nb_of_pmts[8];
  static int *_grid_pmts[8];    

  // Accessors

  static int& _Status(int Geom_id,int Geom_Channel);
  static int& _Mask(int Geom_id,int Geom_Channel);
  static geant& _Pedestal(int Geom_id,int Geom_Channel,int high_gain=1);
  static geant& _PedestalSigma(int Geom_id,int Geom_Channel,int high_gain=1);
  static geant& _PedestalThreshold(int Geom_id,int Geom_Channel,int high_gain=1);
  static geant& _Gain(int Geom_id,int Geom_Channel,int high_gain=1);
  static geant& _GainSigma(int Geom_id,int Geom_Channel,int high_gain=1);
  static int& _GainThreshold(int Geom_id,int Geom_Channel);
  static geant& _Eff(int Geom_id,int Geom_Channel);

  static void get_eff_quantities();
  static void ReadFromFile(const char *filename); // Read calibration from file:  FIXME!
  static void SaveToFile(const char *filename); // Write calibration to file:  FIXME!
 public:
  static geant& _GainSim(int Geom_id,int Geom_Channel,int high_gain=1); // Allows to update the simulated gain, although it should not be done it that way

  static void Init();                    // Init geometry and kinds and so on
  static void Init_Default();            // Init geometry and kinds and so on
  static void Finish();
  static void Finish_Default();


  static int Status(int Geom_id,int Geom_Channel);                                 // Return the status 
  static int Mask(int Geom_id,int Geom_Channel);                                   // Return the offline mask
  static geant Pedestal(int Geom_id,int Geom_Channel,int high_gain=1);             // Return the pedestal
  static geant PedestalSigma(int Geom_id,int Geom_Channel,int high_gain=1);        // Return the pedestal sigma
  static geant PedestalThreshold(int Geom_id,int Geom_Channel,int high_gain=1);    // Return the pedestal threshold 
  static geant Gain(int Geom_id,int Geom_Channel,int high_gain=1);                 // Return the gain
  static geant GainSigma(int Geom_id,int Geom_Channel,int high_gain=1);            // Return the gain sigma
  static int GainThreshold(int Geom_id,int Geom_Channel);                          // Return the threshold to change the gain mode
  static geant Eff(int Geom_id,int Geom_Channel);                                  // Return the relative quantum efficiency

  static geant MeanEff(){return _mean_eff;}                                        // Return the mean channel efficiency
  static geant MaxEff(){return _max_eff;}                                          // Return the max efficiency
  
  static int GetGeomPMTID(int pos);                                                // Given the pos number, return the geometric pmt id
  static int GetPMTID(int geom_pos);
  static int GetGeomChannelID(int geom_pos,int pixel);                             // Given the pmt geom id and the pixel number return the geometric channel id.
  static int GetChannelID(int geom_pos,int pixel);                                 // Given the pmt geom id and the pixel geom id return the pixel id.
  static void GetGeomID(int pos,int pixel,int &geom_pos,int &geom_pix);            // The same for both numbers as references

  static int GetGeomPMTIdFromCDP(int CDP,int pmt);                                 // Given the RDR number (0-23) and the pmt within such RDR (0-30) return the geom id
  static int GetCDPFromGeomPMTId(int pmt,int &pmt_cdp);                            // Given the geom id, return the CDP number and the pmt within such cdp 

  static int PackGeom(int pmt,int channel){return pmt*RICnwindows+channel;}
  static void UnpackGeom(int packed,int &pmt,int &channel){
    pmt=packed/RICnwindows;
    channel=packed%RICnwindows;
  }

  static integer detcer(geant photen);
  static RichPMT& GetPMT(int pmt){_assert(pmt>-1 && pmt<RICmaxpmts);return _pmts[pmt];}

  // Access to members
  static geant GetRichPMTPos(int geom_id,int p){
    _assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
    return _pmts[geom_id]._global_position[p];
  }

  static geant GetAMSPMTPos(int geom_id,int p){
    _assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
    if(p==2)
    return _pmts[geom_id]._global_position[p]-RICHDB::total_height()/2+RICHDB::RICradpos();
    else return _pmts[geom_id]._global_position[p];
  }


  static geant GetChannelPos(int packed_geom,int p){
    int geom_id,window;
    UnpackGeom(packed_geom,geom_id,window);
    _assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
    return _pmts[geom_id]._channel_position[window][p];
  }
  
  static int FindPMT(geant x,geant y);
  static int FindChannel(geant x,geant y);
  static int FindWindow(geant x,geant y){int p,w;int c=FindChannel(x,y);if(c<0)return -1;
  UnpackGeom(c,p,w);return w;}

  RichPMTsManager(int channel_id); // Constructor given a track 
  RichPMTsManager(int grid,int kapton,int in_kapton); // Constructor given a track 
  ~RichPMTsManager(){};

  friend class AMSJob;  // To alow access to TDV
  friend class AMSRichCal;
  friend class DAQRichBlock;  // To allow reading calibration tables
};




class RichPMTChannel{
 private:
  void Init(int geom_id,int channel_id);


 public:
  int pmt_geom_id;
  int channel_geom_id;
  geant gain[2];
  geant gain_sigma[2];
  geant pedestal[2];
  geant pedestal_sigma[2];
  geant rel_eff;             // Efficiency with respect the maximum
  geant position[3];         // Channel position (not PMT position)
  geant gain_threshold;
  geant pedestal_threshold[2];
  int status;
  int mask;

  RichPMTChannel(int packed_id);
  RichPMTChannel(int geom_pmt,int geom_channel);
  RichPMTChannel(int _pmt_geom,geant x,geant y);
  RichPMTChannel(geant x,geant y);
  float SimulateSinglePE(int mode){
    return RichPMTsManager::GetPMT(pmt_geom_id).SimulateSinglePE(channel_geom_id,mode);
  }

  int GetPacked(){if(pmt_geom_id<0 || pmt_geom_id>RICmaxpmts || channel_geom_id<0 || channel_geom_id>RICnwindows) return -1;return RichPMTsManager::PackGeom(pmt_geom_id,channel_geom_id);}
  geant x(){return position[0];}
  geant y(){return position[1];}
  geant z(){return position[2];}
};


// TODO LIST
//
// - Verify that the the orientation of the PMT pixels is the right one
// - Use the physical pixel position in the calibration input and output instead
//   of the geometric ID (it is uninmportant for the PMT itself because the
//   pmtpos id is set)
// - Eliminate the pmtaddh and pmtaddc checks from loading the calibration. Use only
//   the pmtpos

void _Update_RICHPMT();

#undef _assert
#undef private
#endif









