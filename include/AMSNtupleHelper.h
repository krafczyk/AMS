#ifndef __AMSNtupleHelper__
#define __AMSNtupleHelper__

#ifdef _PGTRACK_
#include "root_RVSP.h"
#else
#include "root_RVS.h"
#endif
class AMSNtupleHelper{
public:
 AMSNtupleHelper(){};
 virtual bool IsGolden(AMSEventR *o);
 static AMSNtupleHelper *fgHelper;
};
#endif
