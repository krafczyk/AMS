#ifndef MVAtest_H
#define MVAtest_H

#include "ConfigParser.h"
#include "MVAmanager.h"
#include "OutTreeManager.h"
#include "OutHistoManager.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <time.h>
#include <bitset>
#include <sys/resource.h>
#include <omp.h>
#include <dirent.h>


#include <root.h>
#include <amschain.h>
#include "TrdSCalib.h"
#include "TrdHCalib.h"
#include "TrTrackSelection.h"
#include "TofTrack.h"
#include "Tofrec02_ihep.h"
#include "TrdHRecon.h"
#include "TrdHCalib.h"
#include "TrdHCharge.h"
#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "DynAlignment.h"
#include "TrdKCluster.h"
#include "point.h"

#include "TrdQt.hh"
#include "TrkLH.h"

#include "TTree.h"
#include "TFrame.h"
#include "TString.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"


/** test class to do MVA optimization
 *  using MVAmanager. Have three steps:
 *  -store selected samples from AMSChain to custom tree
 *  -train with MVA using parameters defined in config
 *  -use the produced MVA templates for analysis while looping on AMSchain
 *   or custom tree
 *   config:
 *   bmva -use MVA for training or analysis
 *   bmvatrain -MVA training
 *   bmvaana - MVA analysis
 *   bouttree - use custom tree for trraining or analysis
 *   bamstree -use AMSchain
 *   binitouttree - create a new custom tree to be stored
                    or used onfly
 *   bloadouttree - load existing custom tree
 *   bstoreouttree  - store outtree
 *   bstoreouthisto - store out histos
 *
 *  04.2013 -VZ
*/

using namespace std;

class MVAtest{
public:
  MVAtest(ConfigParser* aconf);
  ~MVAtest();
virtual   OutTreeManager*   booktree();
virtual   OutHistoManager*  bookhisto();
virtual  int filloutevent(AMSEventR* ev, int ntot);
  int trainoutree();
  double getmva(AMSEventR* ev, int ntot);
  double getmva();
  int Debug;
OutTreeManager* fTreeMan;
OutHistoManager* fHistoMan;
 private:
  int bmc;
  ConfigParser* fConf;
  MVAmanager * fMVA;
};

#endif

