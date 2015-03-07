#ifndef  __VCon_root__
#define  __VCon_root__

#include "VCon.h"
#include "root_RVS.h"

#include <vector>
#include <algorithm>

class VCon_root : public VCon {
 private:
  int _prevel;
 public:
  // event
  AMSEventR* ev;
  // container name
  char contname[50];
  // c-tor
  VCon_root(const char *name=0);
  // d-tor
  ~VCon_root() { ev=0; }
  // get container by name
  VCon*     GetCont(const char * name);
  // get number of elements in the container  
  int       getnelem();
  // erase the container
  void      eraseC();
  // get element by index
  TrElem*  getelem(int ii);
  // get element index
  int       getindex(TrElem* aa);
  // add element
  void      addnext(TrElem* aa);
  // remove next element
  void      removeEl(TrElem* prev, integer restore=1);
  // exchange two elements
  void      exchangeEl(TrElem* el1, TrElem* el2);
  //Linked list like, access
  TrElem* first();
  //Linked list like, access
  TrElem* next();
};

#endif
