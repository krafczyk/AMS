//  $Id: amsgobj.h,v 1.6 2005/05/17 09:56:33 pzuccon Exp $
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
};
#endif
