//  $Id: user.h,v 1.3 2001/01/22 17:32:45 choutko Exp $
class AMSUser{
public:
void static InitJob();
void static InitRun();
void static InitEvent();
void static Event();
void static EndJob();
};
