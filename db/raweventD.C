// Author A. Klimentov  9-Apr-1997
// methods for AMSraweventD class
//
// use event format from A.Lebedev (version Jan '97) to check overhead
// June 21, 1997. DAQEvent format is added.
//                use one VArray for all data blocks
// Last Edit: Jun 22, 1997 ak.
//
#include <typedefs.h>
#include <cern.h>
#include <raweventD.h>

enum {headerid = 0, lvl1id = 1, lvl3id = 3, tofid = 10, antiid =20, 
      trackerid = 30, ctcid = 40};


AMSraweventD::AMSraweventD(
             integer runUni, integer runAux, integer eventNumber, 
             time_t time, integer ldata, uint16 data[])
{
 integer i;

 setrun(runUni);
 _runAux = runAux;
 setevent(eventNumber);
 settime(time);

 if (ldata > 0) {
   Data.resize(ldata);
   for (i=0; i<ldata; i++) Data.elem(i) = data[i];
 }

}

AMSraweventD::AMSraweventD
        (integer runUni, integer eventNumber, time_t time, DAQEvent *pdaq)
{
 integer i;

 setrun(runUni);
 setevent(eventNumber);
 settime(time);
 
 int l = pdaq -> eventlength();
 if (l > 0) {
   Data.resize(l);
   uint16 *buff = new uint16[l];
   pdaq -> data(buff);
   for (i=0; i<l; i++) Data.elem(i) = buff[i];
   delete [] buff;
 }
}

void AMSraweventD::readEvent (integer& run, integer & runAux, 
                              integer & eventNumber, time_t & time,
                              integer & ldata, uint16* data)
{
 integer i;

 run         = Run();
 runAux      = _runAux;
 eventNumber = Event();
 time        = Time();

 ldata = Data.size();
 if (ldata > 0) {
   for (i=0; i<ldata; i++) data[i] = Data[i];
 }
}

void AMSraweventD::readEvent (integer& run, integer & eventNumber,
                              time_t & time, integer & runAux)
{
 integer i;

// return types are: uinteger Run(); uinteger Event();
 run         = Run();
 runAux      = _runAux;
 eventNumber = Event();
 time        = Time();

}


void AMSraweventD::dump(integer sdetid) {
// dump event
// if sdetid == -1 dump whole event
//
  cout<<"run, event, length "<<Run()<<", "<<Event()<<", "<<eventlength()
      <<endl;
  enum {header_offset = 1};
  integer offset = header_offset + 1;
  integer id;
  integer l;
  for (;;) {
   id = Data[offset];
   l  = Data[offset - 1];
   if (id == sdetid || sdetid == -1) {
    cout<<"sub.detector id "<<id<<", length "<<l<<endl;
    for (int i = 1; i < l-1; i++) {
      cout<<Data[offset + i]<<" ";
    }
    cout<<endl;
   }
   cout<<endl;
   offset = offset  + l;
   if (offset > eventlength()) break;
  }
}

integer AMSraweventD::sdetlength(integer sdetid) {
  enum {header_offset = 1};
  integer offset = header_offset + 1;
  integer id;
  integer l;
  for (;;) {
   id = Data[offset];
   l  = Data[offset - 1];
   if (id == sdetid) return l;
   offset = offset  + l;
   if (offset > eventlength()) break;
  }
  return -1;
}  

integer AMSraweventD::sdet(integer sdetid) {
  enum {header_offset = 1};
  integer offset = header_offset + 1;
  integer id;
  integer l;
  for (;;) {
   id = Data[offset];
   l  = Data[offset - 1];
   if (id == sdetid) return offset;
   offset = offset  + l;
   if (offset > eventlength()) break;
  }
  return -1;
}  
