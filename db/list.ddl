#ifndef list_h
#define list_h

// class List of events
// May,    1997. ak. First try with Objectivity  
//
// last edit May 9, 1997, ak.
//

#include <typedefs.h>

class EventList : public ooContObj {

 private:

  integer   _nEvents;           // number of events

  ooVString _listName;          // name of list
  ooVString _setupName;         // setup assoc. with list

 public:

// Constructor
  EventList();
  EventList(char* listName);
  EventList(char* listName, char* setup);
 ~EventList() {};

// Set/Get Methods
   const   char*     ListName()          {return _listName;}
   const   char*     SetupName()         {return _setupName;}
   integer Events()                      {return _nEvents;}
   integer getNEvents()                  {return _nEvents;}
   void    incNEvents()                  {_nEvents++;     }
   void    decNEvents()                  {if (_nEvents > 0) _nEvents--;}

   void    resetCounters()               {_nEvents = 0;}
   void    setlistname(char* name);
   void    setsetupname(char* name);
 
   ooBoolean CheckListSstring(char* sstring);
   ooStatus  
        CheckContainer(char* name, ooMode mode, ooHandle(ooContObj)& contH);

   void     printListHeader();
   ooStatus PrintListStatistics(char* printMode);
};

#endif
