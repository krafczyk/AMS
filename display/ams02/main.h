//  $Id: main.h,v 1.2 2001/01/22 17:32:36 choutko Exp $
#include <TApplication.h>
class Myapp : public TApplication{

public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):
 TApplication(appClassName,argc, argv){};

};
