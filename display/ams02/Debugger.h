//  $Id: Debugger.h,v 1.3 2001/06/25 20:18:45 kscholbe Exp $
#ifndef DEBUGGER_H
#define DEBUGGER_H

//#include <stdarg.h>

#ifndef ROOT_TObject
#include <TObject.h>
#endif

class Debugger : public TObject
{
protected:
  Bool_t isOn;			//Is the debugger on?
//static int nInstance;

public:
  Debugger();
  ~Debugger() {}
  void On() { isOn = kTRUE;}	// Turn on the debugger
  void Off() { isOn = kFALSE;}	// Turn off the debugger
  void Print(char * fmt, ... );	// Print a debug message

  ClassDef(Debugger, 1)		// Debug message printing class
};

#ifndef __CINT__
extern Debugger debugger;
#endif

#endif
