#ifndef  __VCon_gb__
#define  __VCon_gb__

#include "VCon.h"
#include "cont.h"

class VCon_gb : public VCon {
 public:
  // container
  AMSContainer * con;
  // c-tor
  VCon_gb(AMSContainer * cc=0);
  // d-tor
  ~VCon_gb(){}
  // get container by name
  VCon*    GetCont(const char * name);
  // get number of elements in the container
  int      getnelem();
  // erase the container
  void     eraseC();
  // get element index
  int      getindex(TrElem* aa);
  // get element by index
  TrElem*  getelem(int ii);
  // add element
  void     addnext(TrElem* aa);
  // remove next element 
  void     removeEl(TrElem* prev, integer restore=1);
  // exchange two elements
  void     exchangeEl(TrElem* el1, TrElem* el2);
};


#endif
