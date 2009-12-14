
#ifndef  __VCon_gb__
#define  __VCon_gb__
#include "VCon.h"
#include "cont.h"

class VCon_gb :public VCon{

  public:
    AMSContainer * con;
    VCon_gb(AMSContainer * cc=0);
    ~VCon_gb(){}


    VCon*     GetCont(const char * name);
    int      getnelem();
    void      eraseC();
    int      getindex(TrElem* aa);
    TrElem* getelem(int ii);
    void     addnext(TrElem* aa);
    void      removeEl(TrElem* prev, integer restore=1);

};


#endif
