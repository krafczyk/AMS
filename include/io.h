// Author V. Choutko 2-feb-1997
//
// Feb  10, 1997.  ak.  implement friend class

// Last edit : Feb 10, 1997. ak.
//

#ifndef __AMSIO__
#define __AMSIO__
#include <point.h>
#include <cern.h>
#include <link.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
class AMSIO {
private:
integer _run;
integer _event;
time_t _time;
integer _ipart;
integer _seed[2];
geant _coo[3];
geant _dir[3];
geant _mom;
geant _polephi;
geant _stationtheta;
geant _stationphi;
integer _nskip;
public:
AMSIO():_run(0),_event(0){};
AMSIO(integer run, integer event, time_t time,integer ipart, 
integer seed[], AMSPoint coo,
AMSDir dir, number mom, number pole, number stheta, number sphi, integer nskip);
~AMSIO(){};
 friend ostream &operator << (ostream &o, const AMSIO &b );
void convert();   
integer getrun() const { return _run;}
integer getevent() const { return _event;}
integer getpid() const {return _ipart;}
integer getskip() const{return _nskip;}
void setpid(integer pid)  {_ipart=pid;}
time_t gettime() const {return _time;}
geant getpolephi() const{return _polephi;}
geant getstheta() const{return _stationtheta;}
geant getsphi() const{return _stationphi;}
integer read();
void close(){ fbin.close();}
integer getseed(integer i){return i>=0 && i<2 ? _seed[i] : 0;} 
static void setfile(char *f);
void write();
static char * fnam;
static fstream fbin;
static void init(integer mode, integer format=0);
friend class AMSmceventg;

#ifdef __DB__
   friend class AMSIOD;
#endif
};

class AMSIOI{
public:
AMSIOI();
~AMSIOI();
private:
static integer _Count;
};
static AMSIOI ioI;
 

#endif
