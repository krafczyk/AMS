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

static int16 mkaddress(int16 strip, int16 va, int16 half, int16 icmpt)
{return icmpt | half<<5 | va<<6 | strip<<10;}

static void getaddress(int16u address, 
integer & strip, integer & va, integer & side, integer & half, integer &drp){
  drp=address&31;half=(address>>5) & 1;va=(address>>6)&15;
  strip=((address>>10)&63)+(va%10)*64; side=va>9 ? 0 : 1;
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
integer lvl3format(int16 * ptr, integer nmax);
static void sitkdigi();
  AMSTrRawCluster *  next(){return (AMSTrRawCluster*)_next;}

  static number ADC2KeV(){return 0.5e6/TRMCFFKEY.beta/TRMCFFKEY.dedx2nprel;}


 // Interface with DAQ
      static AMSID getdaqid(){return AMSID("Tracker",30);}
      static integer calcdaqlength();
      static void builddaq(integer n, uinteger *p);
      static void buildraw(integer n, uinteger *p);



};

#endif
