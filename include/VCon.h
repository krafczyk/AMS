#ifndef  __VCon__
#define  __VCon__
#include "TrElem.h"

class VCon{

public:


  VCon(){}
  virtual ~VCon(){}

  virtual  VCon*  GetCont(char* name)=0;

  virtual int      getnelem()=0;
  virtual void     eraseC()=0;
  virtual TrElem* getelem(int ii)=0;
  virtual int      getindex(TrElem*)=0;
  virtual void     addnext(TrElem* aa)=0;
  // remove next element !!!
  virtual void     removeEl(TrElem* prev, int restore=1)=0;
};




VCon* GetVCon();


#endif


