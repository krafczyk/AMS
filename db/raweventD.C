// Author A. Klimentov  9-Apr-1997
// methods for AMSraweventD class
//
// use event format from A.Lebedev (version Jan '97) to check overhead
//
// Last Edit: Apr 9, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <raweventD.h>

AMSraweventD::AMSraweventD(
             uinteger runUni, uinteger runAux, uinteger eventNumber, 
             uinteger status, time_t time, integer ltrig, integer trig[], 
             integer ltracker, integer tracker[],
             integer ltof, integer tof[], 
             integer lanti, integer anti[], 
             integer lctc, integer ctc[],
             integer lslow, integer slow[])
{
 integer i;

 setrun(runUni);
 _runAux = runAux;
 setevent(eventNumber);
 _status = status;
 settime(time);

 if (ltrig > 0) {
   Trig.resize(ltrig);
   for (i=0; i<ltrig; i++) Trig.elem(i) = trig[i];
 }

 if (ltof > 0) {
   TOF.resize(ltof);
   for (i=0; i<ltof; i++) TOF.elem(i) = tof[i];
 }

 if (lanti > 0) {
   Anti.resize(lanti);
   for (i=0; i<lanti; i++) Anti.elem(i) = anti[i];
 }

 if (lctc > 0) {
   CTC.resize(lctc);
   for (i=0; i<lctc; i++) CTC.elem(i) = ctc[i];
 }


 if (lslow > 0) {
   Slow.resize(lslow);
   for (i=0; i<lslow; i++) Slow.elem(i) = slow[i];
 }

 if (ltracker > 0) {
   Tracker.resize(ltracker);
   for (i=0; i<ltracker; i++) Tracker.elem(i) = tracker[i];
 }
}

void AMSraweventD::readEvent (uinteger& run, uinteger & runAux, 
                              uinteger & eventNumber, uinteger & status, 
                              time_t & time,
                              integer & ltrig, integer* trig, 
                              integer & ltof,  integer* tof,
                              integer & ltracker, integer* tracker,
                              integer & lanti,  integer* anti,
                              integer & lctc, integer* ctc,
                              integer& lslow, integer* slow)
{
 integer i;

 run         = Run();
 runAux      = _runAux;
 eventNumber = Event();
 status      = _status;
 time        = Time();

 ltrig = Trig.size();
 if (ltrig > 0) {
   for (i=0; i<ltrig; i++) trig[i] = Trig[i];
 }

 ltof = TOF.size();
 if (ltof > 0) {
   for (i=0; i<ltof; i++) tof[i] = TOF[i];
 }

 lanti = Anti.size();
 if (lanti > 0) {
   for (i=0; i<lanti; i++) anti[i] = Anti[i];
 }

 lctc = CTC.size();
 if (lctc > 0) {
   for (i=0; i<lctc; i++) ctc[i] = CTC[i];
 }


 lslow = Slow.size();
 if (lslow > 0) {
   for (i=0; i<lslow; i++) slow[i] = Slow[i];
 }

 ltracker = Tracker.size();
 if (ltracker > 0) {
   for (i=0; i<ltracker; i++) tracker[i] = Tracker[i];
 }
}
