#include "TrCalDB.h"
#include "TrLadCal.h"
#include "TkDBc.h"

#include "GenericSlider.h"
#include "MonSlider.h"
#include "CalSlider.h"

#include "TFile.h"

int main(int argc, char **argv){
  MonSlider* monslider = new MonSlider("MonSlider","MonSlider");
  CalSlider* calslider = new CalSlider("CalSlider","CalSlider");
  return 0;
}

