//  $Id: amsgobj.h,v 1.9 2008/12/08 15:15:19 choutko Exp $
// Author V. Choutko 24-may-1996

#ifndef __AMSGOBJ__
#define __AMSGOBJ__
#include "node.h"
#include "snode.h"
#include "uzstat.h"
class AMSgObj{
public:
static AMSNodeMap GTrMedMap; // Tracking med map
static AMSNodeMap GTrMatMap; // Tracking med map
//static AMSNodeMap GVolMap; // gvolume map
static AMSStat BookTimer;
static AMSStatErr Messages;
//#pragma omp threadprivate(BookTimer,Messages)
};
#endif
