// Author V. Choutko 2-feb-1997
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
public:
AMSIO():_run(0),_event(0){};
AMSIO(integer run, integer event, time_t time,integer ipart, integer seed[], AMSPoint coo,
AMSDir dir, number mom);
~AMSIO(){};
integer getrun() const { return _run;}
integer getevent() const { return _event;}
time_t gettime() const {return _time;}
integer read();
static void setfile(char *f);
void write();
static char * fnam;
static fstream fbin;
static void init(integer mode);
friend class AMSmceventg;
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
