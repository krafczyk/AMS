#ifndef  __VCon__
#define  __VCon__
#include "TrElem.h"

class VCon{
 public:
  // c-tor
  VCon(){}
  // d-tor
  virtual ~VCon(){}
  // get container by name
  virtual VCon*    GetCont(const char* name)=0;
  // get number of elements in the container
  virtual int      getnelem()=0;
  // erase the container
  virtual void     eraseC()=0;
  // get element by index
  virtual TrElem*  getelem(int ii)=0;
  // get element index
  virtual int      getindex(TrElem*)=0;
  // add element
  virtual void     addnext(TrElem* aa)=0;
  // remove next element 
  virtual void     removeEl(TrElem* prev, int restore=1)=0;
  // exchange two elements
  virtual void     exchangeEl(TrElem* el1, TrElem* el2)=0;
  //Linked list like, access
  virtual TrElem* first()=0;
  //Linked list like, access
  virtual TrElem* next()=0;
};

VCon* GetVCon();

#endif


