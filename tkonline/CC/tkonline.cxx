#include "MonitorUI.h"

#include <TApplication.h>

int main(int argc, char **argv){
  char filename[200] = "";
  if (argc>=2) strcpy(filename,argv[1]);
  TApplication app("App",&argc,argv);
  new MonitorUI(gClient->GetRoot(),200,200,filename);
  app.Run();
  return 0;
}

