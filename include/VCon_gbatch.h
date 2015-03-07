#ifndef  __VCon_gb__
#define  __VCon_gb__

#include "VCon.h"
#include "cont.h"

class VCon_gb : public VCon {
 private:
  AMSlink* _prevel;
 public:
  // container
  AMSContainer * con;
//  element hint
    int leli;
   AMSlink *lel; 
  // c-tor
  VCon_gb(AMSContainer * cc=0);
  // d-tor
  ~VCon_gb(){_prevel=0;}
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
  //Linked list like, access
  TrElem* first();
  //Linked list like, access
  TrElem* next();
};


#endif
