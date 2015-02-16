//  $Id$
// Author V. Choutko 24-may-1996
 
#ifndef __AMSTRRAWCLUSTER__
#define __AMSTRRAWCLUSTER__
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "event.h"
#include "cont.h"
#include "amsstl.h"
#include "commons.h"
#include "trid.h"
#include "daqevt.h"
// Tracker RAW bank

class AMSTrRawCluster : public AMSlink{
protected:
geant  _sub;
integer _address;   // as for idsoft
integer _strip;
integer _nelem;
geant _s2n;      // signal to noise for triggered strip
integer * _array;
void _init(){};
AMSTrRawCluster(const AMSTrRawCluster &){_next=0;};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();
integer _matched();
public:
    static const integer MATCHED;
geant getamp(int i);

static integer Out(integer);
static void lvl3CompatibilityAddress(int16u address, 
integer & strip, integer & va, integer & side,  integer &drp){
  drp=(address>>10)&31;va=(address>>6)&15;
  strip=((address)&63)+(va%10)*64; side=va>9 ? 0 : 1;
}

integer getid()const {return _address;}

integer operator < (AMSlink & o) const {
// No RTTI - take a "risk" here
  AMSTrRawCluster * p= (AMSTrRawCluster*)(&o);
  return getid() < p->getid();
}
~AMSTrRawCluster();
AMSTrRawCluster(integer id=0):AMSlink(),_address(id),_strip(0),_nelem(0),
_array(0),_s2n(0),_sub(0){}
AMSTrRawCluster(integer id,  integer nleft, integer nright,geant *p, geant s2n);
AMSTrRawCluster(integer id,  integer nleft, integer nright,int16 *p, geant s2n);
void expand( number *p)  ;
integer lvl3format(int16 * ptr, integer nmax,  integer matchedonly=0);
static void sitkdigi();
  AMSTrRawCluster *  next(){return (AMSTrRawCluster*)_next;}

  static number ADC2KeV(){return 0.5e6/TRMCFFKEY.beta/TRMCFFKEY.dedx2nprel;}


 // Interface with DAQ

 static integer getdaqid(int16u crt);
 static integer checkdaqid(int16u id);
 static integer checkdaqidS(int16u id);
 static integer calcdaqlength(integer i);
 static integer getmaxblocks(){return AMSTrIdSoft::ncrates();}
 static integer getmaxblocksRaw(){return AMSTrIdSoft::ncrates();}
 static void builddaq_new(integer i, integer n, int16u *p);
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
 static void matchKS();
 static void buildpreclusters( AMSTrIdSoft & idd, integer len, geant id[]);
 integer TestRawMode();
  // H/K data read
  static void updtrcalib2009S(integer length,int16u *p); 
  static void updtrcalibS(integer length,int16u *p); 
  static integer getmaxblockS(){return  AMSTrIdSoft::ncrates();}
  // H/K TDV correspondance
  static AMSID getTDVstatus(int i);
  static AMSID getTDVCmnNoise();
  static AMSID getTDVIndNoise();
  static AMSID getTDVped(int i);
  static AMSID getTDVgains(int i);
  static AMSID getTDVsigma(int i);
  static AMSID getTDVrawsigma(int i);
  static AMSID getTDVrhomatrix(int i);
#ifdef __WRITEROOT__
  friend class TrRawClusterR;
#endif
};

#endif
