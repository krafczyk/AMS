// Author V. Choutko 24-may-1996
 
#ifndef __AMSTRRAWCLUSTER__
#define __AMSTRRAWCLUSTER__
#include <upool.h>
#include <apool.h>
#include <gsen.h>
#include <trid.h>
#include <event.h>
#include <cont.h>
#include <amsstl.h>
#include <commons.h>


// Tracker RAW bank

class AMSTrRawCluster : public AMSlink{
protected:
integer _address;   // as for idsoft
integer _strip;
integer _nelem;
integer * _array;
void _init(){};
AMSTrRawCluster(const AMSTrRawCluster &){_next=0;};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();
public:

//static int16 mkaddress(int16 strip, int16 va, int16 half, int16 icmpt, integer pedantic=0)
//{return pedantic==0?strip | va<<6 | icmpt << 10 | half<<15:strip | va<<6 | icmpt << 10;}


static void lvl3CompatibilityAddress(int16u address, 
integer & strip, integer & va, integer & side, integer & half, integer &drp){
  drp=(address>>10)&31;half=(address>>15) & 1;va=(address>>6)&15;
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
_array(0){}
AMSTrRawCluster(integer id,  integer nleft, integer nright,geant *p);
AMSTrRawCluster(integer id,  integer nleft, integer nright,int16 *p);
void expand( number *p) const ;
integer lvl3format(int16 * ptr, integer nmax, integer pedantic=0);
static void sitkdigi();
  AMSTrRawCluster *  next(){return (AMSTrRawCluster*)_next;}

  static number ADC2KeV(){return 0.5e6/TRMCFFKEY.beta/TRMCFFKEY.dedx2nprel;}


 // Interface with DAQ

 static int16u getdaqid(int i);
 static integer checkdaqid(int16u id);
 static integer calcdaqlength(integer i);
 static int16u getdaqidRaw(int i);
 static integer checkdaqidRaw(int16u id);
 static integer calcdaqlengthRaw(integer i);
 static integer getmaxblocks(){return 2;}
 static integer getmaxblocksRaw(){return 2;}
 static void builddaq(integer i, integer n, int16u *p);
 static void buildraw(integer n, int16u *p);
 static void builddaqRaw(integer i, integer n, int16u *p);
 static void buildrawRaw(integer n, int16u *p);
 integer TestRawMode();
  // H/K data read
  static integer checkstatusSid(int16u id) ;
  static integer checkpedSid(int16u id) ;
  static integer checksigmaSid(int16u id);
  static void updstatusS(integer length,int16u *p); 
  static void updpedS(integer length,int16u *p); 
  static void updsigmaS(integer length,int16u *p); 
  // H/K data simulate
  static int16u getstatusSid(int i) ;
  static int16u getpedSid(int i) ;
  static int16u getsigmaSid(int i);
  static integer getmaxblockS(){return 2;}
  static void writestatusS(integer i, integer length,int16u *p); 
  static void writepedS(integer i, integer length,int16u *p); 
  static void writesigmaS(integer i, integer length,int16u *p); 
  static integer calcstatusSl(int i);
  static integer calcpedSl(int i);
  static integer calcsigmaSl(int i);
  // H/K TDV correspondance
  static AMSID getTDVstatus(int i);
  static AMSID getTDVped(int i);
  static AMSID getTDVsigma(int i);
};

#endif
