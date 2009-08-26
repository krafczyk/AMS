#ifndef  __VCon__
#define  __VCon__
#include "TrElem.h"

class VCon{

public:


  VCon();
  virtual ~VCon(){}

  virtual  VCon*  GetCont(char* name){}

  virtual int      getnelem(){}
  virtual void     eraseC(){}
  virtual TrElem* getelem(int ii){}
  virtual int      getindex(TrElem*){}
  virtual void     addnext(TrElem* aa){}
  // remove next element !!!
  virtual void     removeEl(TrElem* prev, int restore=1) {}
};




VCon* GetVCon();


#endif


