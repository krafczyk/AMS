#ifndef list_h
#define list_h
// class List of Raw events
// May,    1997. ak. First try with Objectivity  
// Oct 1997 rawlist splitted from dbA_Add.C 
//
// Last edit Oct. 25, 1997 ah.
//

#include <typedefs.h>

#include <list_ref.h>
#include <list.h>
#include <daqevt.h>


class AMSraweventD;
#pragma ooclassref AMSraweventD <raweventD_ref.h>

class AMSRawEventList : public EventList {

 private:

 public:

// Constructor
  AMSRawEventList();
  AMSRawEventList(char* listName, char* prefix);
  AMSRawEventList(char* listName, char* setup, char* prefix);
  ~AMSRawEventList() {};

// Set/Get Methods

//
 void     resetRawCounters();
 void     SetContainersNames(char* name);

// add methods

// read methods
};

#endif
