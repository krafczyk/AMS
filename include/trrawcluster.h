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
AMSID _amsid;
struct rc{
integer nelem;
integer * array;
rc *next;
rc *prev;
};
integer _mode;
number _cmn;
rc * _first;
rc * _last;
void _init(){};
AMSTrRawCluster(const AMSTrRawCluster &){_next=0;};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();
public:

static int16 mkaddress(int16 strip, int16 va, int16 half, int16 icmpt)
{return icmpt | half<<5 | va<<6 | strip<<10;}

static void getaddress(int16u address, 
integer & strip, integer & va, integer & half, integer &drp){
  drp=address&31;half=(address>>5) & 1;va=(address>>6)&15;strip=(address>>10)&63;}

integer getid()const {return _amsid.getid();}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
{if(p){((AMSTrRawCluster*)p)->~AMSTrRawCluster();p=0;UPool.udelete(p);}}
integer _nelem;
~AMSTrRawCluster();
AMSTrRawCluster( AMSID id):_amsid(id),_nelem(0),_first(0){_next=0;};
AMSTrRawCluster(AMSID id, integer mode, integer nleft, integer nright,geant *p):_amsid(id),_mode(mode),_first(0),_last(0)
  {_cmn=AMSTrIdSoft(id.getid()).getcmnnoise();extend(nleft,nright,p);_next=0;}
void extend( integer nleft,integer nright, geant *p);
void expand( number *p) const ;
integer lvl3format(int16 * ptr, integer nmax);
static void sitkdigi();
  AMSTrRawCluster *  next(){return (AMSTrRawCluster*)_next;}

  static number ADC2KeV(){return 0.5e6/TRMCFFKEY.beta/TRMCFFKEY.dedx2nprel;}
};

#endif
