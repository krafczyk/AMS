#ifndef InterMVA_H
#define InterMVA_H

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


/** MVA Reader for MVA trained for
 * secondary interactions 
 *  see example/mvaexample/  mainintermva.cpp
 *  and twiki:  
 *  https://twiki.cern.ch/twiki/bin/view/AMS/InteractionsDiscriminant
 *  05.2013 -VZ
*/


class InterMVA{
public:
  InterMVA();
  InterMVA(string sconf);
   ~InterMVA();
  int Init();
  double GetMVAOut(AMSEventR* ev);
  int Debug;
 private:
 int filloutevent(AMSEventR* ev);
  OutTreeManager* ftree;
  ConfigParser* fconf;
  MVAmanager * fmva;
  //
  TrdQt* trdqt;
};

#endif

