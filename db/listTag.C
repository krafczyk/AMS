// method source file for the object AMSEventTagList
//
// last edit Apr 14, 1997, ak.
//
//
#include <iostream.h>
#include <string.h>

#include <ooIndex.h>

#include <db_comm.h>

#include <listTag_ref.h>
#include <listTag.h>


AMSEventTagList::AMSEventTagList (char* listname, char* setup)
{
  if (listname) setlistname(listname);
  if (setup)    setsetupname(setup);
  resetCounters();
}
