#include "MonitorUI.h"

#include <TApplication.h>

int main(int argc, char **argv){
  printf("%s usage:\n",argv[0]);
  printf("   0 argouments    opening of the main interface.\n");
  printf("   1 argoument     tries to open monitoring and calibration information from that file.\n");
  printf("   2 argouments    tries to open monitoring info from tghe first file and calibration info from the second.\n");
  printf("   -W              opens the interface to the calibration database.\n");
  printf("   -W <calnumber>  tries to catch the calibration number from the database.\n");
  char filename1[200] = "";
  char filename2[200] = "";
  if (argc>1) strcpy(filename1,argv[1]);
  if (argc>2) strcpy(filename2,argv[2]);
  TApplication app("App",&argc,argv);
  new MonitorUI(gClient->GetRoot(),200,200,filename1,filename2);
  app.Run();
  return 0;
}

