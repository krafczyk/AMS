#ifndef __trelem__
#define __trelem__
#include "TObject.h"
#include <iostream>
#include <string>
#define MAXINFOSIZE 255

//! Abstract class used to define a  Generic Tracker Rec/Sim Object
class TrElem{
protected:
  //! C string of fixed sizer used for the event display interface
  static char _Info[MAXINFOSIZE];
#pragma omp threadprivate (_Info)
  //! Constructor (should no be used)
  TrElem(){}
  //! Destructor
  virtual ~TrElem(){}
  //! Function that returns meaningful info
  virtual std::string _PrepareOutput(int opt=0)=0;

public:
  //! it Prints Infos to std output
  virtual void   Print(int opt=0)=0;
  //! Returns a string of max size MAXINFOSIZE with some info
  virtual const char* Info(int iRef=0)=0;
  //! Send the sout infos to a given channel
  virtual std::ostream& putout(std::ostream &ostr = std::cout)=0;
  ClassDef(TrElem,1);
};
#endif
