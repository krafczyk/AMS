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

  inline integer getpmtnb(){return _npmts;}
  inline integer getcratesnb(){return _ncrates;}

  static void Init(); // Init tables according to geometry
  

};

/*

// Future class prototype

class AMSRICHIdSoft{  // Soft Id (Readout)
private:
  uinteger _crate;
  uinteger _pmt;
  uinteger _pixel;

  uinteger _address;  // _address of present idsoft+pmt

  static geant *_ped;
  static geant *_sig_peg;
  static geant *_gain[2];
  static geant *_sig_gain[2];
  static geant *_status;

  static geant *_pmt_crate;

public:				     
  AMSRICHIdSoft():_crate(0),_pmt(0),_pixel(0){};
  AMSRICHIdSoft(AMSRICHIdGeom &);  // geom->soft

  void Init();

  friend class AMSJob;

};

*/

#endif
