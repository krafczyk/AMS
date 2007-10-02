#ifndef __RICHID__
#define __RICHID__


// NEXT DEFINE CONTROL THE USE OF THE OLD OR THE NEW MANAGEMENT OF THE
// RICH PMT INFORMATION
#define __USERICHPMTMANAGER__  // Activate the new code

#include "richdbc.h"
#include "cfortran.h"

// Define the calls to richpmtlib
PROTOCCALLSFFUN3(FLOAT,PDENS,pdens,FLOAT,FLOAT,FLOAT)
#define PDENS(A1,A2,A3) CCALLSFFUN3(PDENS,pdens,FLOAT,FLOAT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB4(GETRMURSG,getrmursg,FLOAT,FLOAT,PFLOAT,PFLOAT)
#define GETRMURSG(A1,A2,A3,A4) CCALLSFSUB4(GETRMURSG,getrmursg,FLOAT,FLOAT,PFLOAT,PFLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB4(GETRLRS,getrlrs,FLOAT,FLOAT,PFLOAT,PFLOAT)
#define GETRLRS(A1,A2,A3,A4) CCALLSFSUB4(GETRLRS,getrlrs,FLOAT,FLOAT,PFLOAT,PFLOAT,A1,A2,A3,A4)


#ifndef __USERICHPMTMANAGER__

// This code is obsolete

class AMSRICHIdGeom{
private:
  integer _channel;    // RICHGeomId channel number
  integer _crate;      // Crate #
  integer _pmt_in_crate;  // PMT# in crate
  integer _pmt;        // PMT#
  integer _pixel;      // pixel #

  static geant *_pmt_p[3];      // PMTs position of PMT center
  static integer *_pmt_orientation; // PMTs orientation for future use
  static integer *_pmt_crate;  // PMT crate
  static integer *_npmts_crate;// number of PMT in crate
  static integer *_crate_pmt;  // index to first pmt in crate
  static integer _ncrates;     // number of crates  
  static integer _npmts;       // number of pmts

public:
  AMSRICHIdGeom(integer channel=0);
  AMSRICHIdGeom(integer pmtnb,geant x,geant y); // Given the points in the photocathode obtains everything=channel number
  //  AMSRICHIdGeom(AMSRICHIdSoft &){}; // convert soft->geom
  ~AMSRICHIdGeom(){}

  geant x();   // returns x coordinate of current channel
  geant y();   // returns y coordinate of current channel
  geant z();   // returns z coordinate at the LG entrance of current channel

  inline integer getchannel(){return _channel;}
  inline integer getcrate(){return _crate;}
  inline integer getpmt(){return _pmt;}
  inline integer getpixel(){return _pixel;}
  inline integer getpmtcrate(){return _pmt_in_crate;}

  static geant pmt_pos(int pmt,integer i){
    i=i<0?0:i%3;
    pmt=pmt<0?0:pmt%_npmts;
    return *(_pmt_p[i]+pmt);
  }

  static inline integer getpmtnb(){return _npmts;}
  static inline integer getcratesnb(){return _ncrates;}

  static inline integer getpmtsincrate(int n){return (n<_ncrates && n>-1)?_npmts_crate[n]:0;}
  static inline integer getfirstincrate(int n){return (n<_ncrates && n>-1)?_crate_pmt[n]:0;}

  static void Init(); // Init tables according to geometry

  static integer get_channel_from_top(geant,geant);

  static geant half_pixel(){return RICHDB::lg_length/8;}
  

};



class AMSRICHIdSoft{  // Soft Id (Readout)
private:
  //uinteger _crate;
  //  uinteger _pmt;
  //  uinteger _pixel;

  uinteger _mode;   // low/gain mode
  uinteger _address;   // address of present idsoft+pmt in geometry

  static uinteger _nchannels;

  // Pedestal simulation (gaussian)
  static geant *_ped;          // pedesta position
  static geant *_sig_ped;      // pedestal spread

  static geant *_lambda;       // lambda of single p.e.
  static geant *_scale;        // scale  of single p.e.
  static geant *_transparency; // transparency of photocathode
  static geant *_lambda_dyn;   // lambda of the first dynode 
  static geant *_scale_dyn;    // scale of the first dynode

  static geant *_threshold;    // Threshold measure in pedestals sigmas 
  static integer *_gain_mode_boundary;  // Value to swith to low gain
  static integer *_status;     // channel status 1=OK 0=Off

  static uinteger _nbins;
  static geant *_cumulative_prob; // Cumulative probability for a single
                                   // p.e. in order to get a better simulation 
  static geant *_step;
public:		 		     

  // constructors

  //  AMSRICHIdSoft():_crate(0),_pmt(0),_pixel(0),_address(0),_mode(0){};
  AMSRICHIdSoft(integer address):_address(address){};
  AMSRICHIdSoft(AMSRICHIdGeom &geom){
    _address=geom.getchannel();
  }


  //  AMSRICHIdSoft(integer guido_board,integer channel_id){
  //}  // Not yet defined. Here we need conversion tables because
       // the address is related to the geometry and not the hardware
       // This is done to simplify the different arrangement due to
       // the magnetic field. 


  static void Init();
  static void fill_probability();


  // Getting calibration values
  inline geant getped(int id){return _ped[(id<=0?0:1)+2*_address];}
  //  inline geant getgain(int id){return _scale[(id<=0?0:1)+2*_address]*
  //			  (_lambda[(id<=0?0:1)+2*_address]+1);}
  inline geant getgain(int id){
    geant gain,sgain;
    GETRMURSG(_lambda[(id<=0?0:1)+2*_address],_scale[(id<=0?0:1)+2*_address],gain,sgain);
    return gain;
  }

  inline geant getsped(int id){return _sig_ped[(id<=0?0:1)+2*_address];} 
  //  inline geant getsgain(int id){return _scale[(id<=0?0:1)+2*_address]*
  //			   sqrt(_lambda[(id<=0?0:1)+2*_address]);}
  inline geant getsgain(int id){
    geant gain,sgain;
    GETRMURSG(_lambda[(id<=0?0:1)+2*_address],_scale[(id<=0?0:1)+2*_address],gain,sgain);
    return sgain;
  }


  inline geant getthreshold(int id){return _threshold[(id<=0?0:1)+2*_address];}
  inline integer getboundary() {return _gain_mode_boundary[_address];}
  inline integer getstatus(){return _status[_address];}




  // Getting channel number
  inline integer getaddress(){return _address;}

  /*
  // Simulate the response for npe photoelectrons
  integer simulate_signal(integer npe){
    // Get the total charge as mesured in the high gain mode
    // If the charge is to high, switch to low gain mode 
  }
  integer simulate_dark_current(){}
  */

  geant simulate_single_pe(int mode);


  friend class AMSJob;

};


#endif

///////////////////////////////
/// NEW VERSION OF THE CODE ///
///////////////////////////////

// This class contains all the geometric information for all the channels, as well
// as the information for simulation
// 
#ifdef __USERICHPMTMANAGER__
class RichPMTChannel;


class RichPMT{
 private:
  int _geom_id;
  int _pos;
  int _orientation;
  int _pmtaddh;
  int _pmtaddc;
  int _pmtnumb;

  geant _global_position[3];                           // Position with respect to global system
  geant _local_position[3];                            // Position with respect to grid system (unused)
  geant _channel_position[RICnwindows][3];             // Channel global position as a function of the geometric channel id
  geant _cumulative_prob[RICnwindows][2][RIC_prob_bins];  // Cumulative probability for this channel
  geant _step[RICnwindows][2];                                      // Step size for the cumulative distribution   
  int _channel_id2geom_id[RICnwindows];       // Table to convert from channel id to geometric channel id

  void compute_tables();
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
  geant SimulateSinglePE(int mode){assert(_filled);return SimulateSinglePE(_current_channel,mode);}
  geant SimulateSinglePE(){assert(_filled);return SimulateSinglePE(_current_channel,_current_mode);}

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

  static int   _status[RICmaxpmts*RICnwindows];                  // Channel status word
  static geant _pedestal[2*RICmaxpmts*RICnwindows];              // Pedestal position (x2 gains) high,low
  static geant _pedestal_sigma[2*RICmaxpmts*RICnwindows];        // Pedestal width (x2 gains)
  static geant _pedestal_threshold[2*RICmaxpmts*RICnwindows];    // Pedestal threshold width (x2 gains)
  static geant _gain[2*RICmaxpmts*RICnwindows];                  // Gain (x2 gains) high,low
  static geant _gain_sigma[2*RICmaxpmts*RICnwindows];            // Gain width (x2 gains)
  static int _gain_threshold[RICmaxpmts*RICnwindows];           // Gain threshold 
  static geant _relative_efficiency[RICmaxpmts*RICnwindows];    // Efficiency with respect to an arbitrary PMT 

  static time_t _eff_begin,_eff_insert,_eff_end;  // Monitor changes eff tables in order to
                                                  // recompute the efficiency tables 


  // Look up tables
  static int _grid_nb_of_pmts[8];
  static int *_grid_pmts[8];    

  // Accessors

  static int& _Status(int Geom_id,int Geom_Channel);
  static geant& _Pedestal(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant& _PedestalSigma(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant& _PedestalThreshold(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant& _Gain(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant& _GainSigma(int Geom_id,int Geom_Channel,int low_gain=1);
  static int& _GainThreshold(int Geom_id,int Geom_Channel);
  static geant& _Eff(int Geom_id,int Geom_Channel);

  static void get_eff_quantities();
  static void ReadFromFile(const char *filename); // Read calibration from file: not implemented FIXME!
 public:
  static void Init();                    // Init geometry and kinds and so on
  //  static void Init_File(char *filename);
  static void Init_Default();            // Init geometry and kinds and so on
  static void Finish();
  //  static void Finish_File(char *filename);
  static void Finish_Default();


  static int Status(int Geom_id,int Geom_Channel);
  static geant Pedestal(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant PedestalSigma(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant PedestalThreshold(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant Gain(int Geom_id,int Geom_Channel,int low_gain=1);
  static geant GainSigma(int Geom_id,int Geom_Channel,int low_gain=1);
  static int GainThreshold(int Geom_id,int Geom_Channel);
  static geant Eff(int Geom_id,int Geom_Channel);

  static geant MeanEff(){return _mean_eff;}
  static geant MaxEff(){return _max_eff;}
  
  static int GetGeomPMTID(int pos);
  static int GetGeomChannelID(int pos,int pixel);
  static void GetGeomID(int pos,int pixel,int &geom_pos,int &geom_pix);

  static int PackGeom(int pmt,int channel){return pmt*RICnwindows+channel;}
  static void UnpackGeom(int packed,int &pmt,int &channel){
    pmt=packed/RICnwindows;
    channel=packed%RICnwindows;
  }

  static integer detcer(geant photen);
  static RichPMT& GetPMT(int pmt){assert(pmt>-1 && pmt<RICmaxpmts);return _pmts[pmt];}

  // Access to members
  static geant GetRichPMTPos(int geom_id,int p){
    assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
    return _pmts[geom_id]._global_position[p];
  }

  static geant GetAMSPMTPos(int geom_id,int p){
    assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
    if(p!=3)
    return _pmts[geom_id]._global_position[p]-RICHDB::total_height()/2+RICHDB::RICradpos();
    else return _pmts[geom_id]._global_position[p];
  }


  static geant GetChannelPos(int packed_geom,int p){
    int geom_id,window;
    UnpackGeom(packed_geom,geom_id,window);
    assert(geom_id>-1 && geom_id<RICmaxpmts && p>-1 && p<3);
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
#endif

#endif









