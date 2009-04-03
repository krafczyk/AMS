#ifndef  __VCon_gb__
#define  __VCon_gb__
#include "VCon.h"
#include "link.h"
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
  AMSlink*  getelem(int ii);
  int       getindex(AMSlink* aa);
  void      addnext(AMSlink* aa);
  // remove next element !!!
  void      removeEl(AMSlink* prev, integer restore=1);
};


#endif
