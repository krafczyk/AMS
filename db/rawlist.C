//  $Id: rawlist.C,v 1.2 2001/01/22 17:32:30 choutko Exp $
// method source file for the object AMSEventList
// First try with Objectivity May 02, 1996
// rawlist separated from dbA_Add.C 
//
// last edit Oct. 25 1997 ah.
//
//
#include <iostream.h>
#include <string.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>

#include <rd45.h>

#include <db_comm.h>

#include <rawlist_ref.h>
#include <raweventD_ref.h>


#include <rawlist.h>
#include <raweventD.h>

#include <job.h>
#include <event.h>

// container  names
  static char* rawCont;


// ooContObj classes
ooHandle(ooContObj)  contrawH;      // Raw  

// ooObj classes
ooHandle(AMSraweventD) raweventH;

// Iterators
ooItr(AMSraweventD) raweventItr;   // Raw

AMSRawEventList::AMSRawEventList(char* listname, char* prefix)
{
  setlistname(listname);
  resetCounters();
// raw specific methods
  resetRawCounters();
  SetContainersNames(prefix);
}

AMSRawEventList::AMSRawEventList(char* listname, char* setup, char* prefix)
{
  setlistname(listname);
  setsetupname(setup);
  resetCounters();
// raw specific methods
  resetRawCounters();
//  SetContainersNames(prefix);
}

void AMSRawEventList::resetRawCounters()
{
}

void AMSRawEventList::SetContainersNames(char* prefix)
{
  if (!rawCont) rawCont =  StrCat("RawEvents_",prefix);

}

