//  $Id: amsgobj.h,v 1.5 2003/03/04 13:09:24 choutko Exp $
// Author V. Choutko 24-may-1996

#ifndef __AMSGOBJ__
#define __AMSGOBJ__
#include <node.h>
#include <snode.h>
#include <uzstat.h>
class AMSgObj{
public:
static AMSNodeMap GTrMedMap; // Tracking med map
static AMSNodeMap GTrMatMap; // Tracking med map
//static AMSNodeMap GVolMap; // gvolume map
static AMSStat BookTimer;
static AMSStatErr Messages;
};
#endif
