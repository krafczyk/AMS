//  $Id: Debugger.cxx,v 1.3 2003/06/17 07:39:54 choutko Exp $

#include <stdio.h>
#include <stdarg.h>

#include "Debugger.h"

//
// prints out debug information
//




Debugger debugger;


ClassImp(Debugger)

//_____________________________________________
Debugger::Debugger(void)
{
  isOn = kTRUE;
}


//_____________________________________________
void Debugger::Print(char * fmt, ... )
{
  if (! isOn) return;

  va_list ap;
  va_start(ap, fmt);

  vfprintf(stderr, fmt, ap);
  va_end(ap);
  return;
}
