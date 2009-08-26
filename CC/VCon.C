#include "VCon.h"

#ifdef __ROOTSHAREDLIBRARY__
#ifdef _STANDALONE_
#include "VCon_root2.h"
#else
#include "VCon_root.h"
#endif
#else  
#include "VCon_gbatch.h"
#endif

VCon::VCon(){


}


VCon* GetVCon(){
  
#ifdef __ROOTSHAREDLIBRARY__
 return new VCon_root();
#else  
 return new VCon_gb();
#endif

}
