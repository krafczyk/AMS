#ifndef  __VCon__
#define  __VCon__
#include "link.h"

class VCon{

public:


  VCon();
  virtual ~VCon(){}

  virtual  VCon*  GetCont(char* name){}

  virtual int      getnelem(){}
  virtual void     eraseC(){}
  virtual AMSlink* getelem(int ii){}
  virtual int      getindex(AMSlink*){}
  virtual void     addnext(AMSlink* aa){}
  // remove next element !!!
  virtual void     removeEl(AMSlink* prev, integer restore=1) {}
};

#endif


