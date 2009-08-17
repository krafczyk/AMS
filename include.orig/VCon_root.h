#ifndef  __VCon_gb__
#define  __VCon_gb__
#include "VCon.h"

#include "root_RVS.h"

class VCon_root :public VCon{

public:
  AMSEventR* ev;
  char contname[50];
  VCon_root(char *name=0);
  ~VCon_root(){ev=0;}


  VCon*     GetCont(char * name);
  int       getnelem();
  void      eraseC();
  TrElem*  getelem(int ii);
  int       getindex(TrElem* aa);
  void      addnext(TrElem* aa);
  // remove next element !!!
  void      removeEl(TrElem* prev, integer restore=1);
};


#endif
