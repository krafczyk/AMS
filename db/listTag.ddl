#ifndef listTag_h
#define listTag_h

// class List of Tag events
// Author: A.Klimentov
//
//
// last edit May 9, 1997, ak.
//

#include <typedefs.h>

#include <list_ref.h>
#include <list.h>

class AMSEventTagList : public EventList {

 private:

  integer   _listType;          // 0 == simulation
  
 public:

// Constructor
  AMSEventTagList();
  AMSEventTagList(char* listname, char* setup);
 ~AMSEventTagList() {};

// Set/Get Methods
           integer   ListType()          {return _listType;}
   void    setListType(integer listType) {_listType = listType;}

   void setlisttype(integer type) { _listType = type;}

};

#endif


