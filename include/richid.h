#ifndef __RICHID__
#define __RICHID__

#include "richdbc.h"

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
  

};



// Future class prototype

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


  // Getting calibration values
  inline geant getped(int id){return _ped[(id<=0?0:1)+2*_address];}
  inline geant getgain(int id){return _scale[(id<=0?0:1)+2*_address]*
			  (_lambda[(id<=0?0:1)+2*_address]+1);}
  inline geant getsped(int id){return _sig_ped[(id<=0?0:1)+2*_address];} 
  inline geant getsgain(int id){return _scale[(id<=0?0:1)+2*_address]*
			   _lambda[(id<=0?0:1)+2*_address];}
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

  // Get the response for a single p.e. 
  geant simulate_single(){}
  

  integer simulate_dark_current(){}
  */

  friend class AMSJob;

};



#endif









