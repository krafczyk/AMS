#include <TApplication.h>
class Myapp : public TApplication{

public:
 void HandleIdleTimer();
 Myapp(const char *appClassName,int *argc, char **argv):
 TApplication(appClassName,argc, argv){};

};
