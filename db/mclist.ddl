#ifndef list_h
#define list_h
// class List of MC events
// May,    1997. ak. First try with Objectivity  
//
// last edit May 08, 1997, ak.
//

#include <typedefs.h>

#include <list_ref.h>
#include <list.h>

#include <mcevent_ref.h>
#include <mcevent.h>

class AMSMCEventList : public EventList {

 private:

 public:

// Constructor
  AMSMCEventList();
  AMSMCEventList(char* listName);
  AMSMCEventList(char* listName, char* setup);
 ~AMSMCEventList() {};

// Set/Get Methods

//
void     resetMCCounters();
void     SetContainersNames(char* name);

// add methods
ooStatus  Addantimccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Addctcmccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Addmceventg(ooHandle(AMSmcevent)& eventH);
ooStatus  Addtofmccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Addtrmccluster(ooHandle(AMSmcevent)& eventH);

// read methods
ooStatus  Readantimccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Readctcmccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Readmceventg(ooHandle(AMSmcevent)& eventH);
ooStatus  Readtofmccluster(ooHandle(AMSmcevent)& eventH);
ooStatus  Readtrmccluster(ooHandle(AMSmcevent)& eventH);

};

#endif
