//  $Id: amsgobj.C,v 1.7 2008/08/27 18:17:36 pzuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "node.h"
#include "snode.h"
#include "uzstat.h"
#include "amsgobj.h"
AMSNodeMap AMSgObj::GTrMatMap;
AMSNodeMap AMSgObj::GTrMedMap;
//AMSNodeMap AMSgObj::GVolMap;
AMSStat     AMSgObj::BookTimer;
AMSStatErr  AMSgObj::Messages;
