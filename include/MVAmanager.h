#ifndef MVAmanager_HH
#define MVAmanager_HH

#include<TFile.h>
#include<TTree.h>
#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/Config.h"
#include "TMVA/Tools.h"
#include "TMVA/MethodCuts.h"
#include "typedefs.h"


/**
 * TMVA  generic interface
 * take parameters by name from config and read from predefined tree 
 * use OutTreeManager to create one on fly or persistent.
 * train and read MVA output back
 *
 * 3.07.2012-  VZ     
*/




//using namespace TMVA;

class ConfigParser;

/**
 * MVA parameter descriptor
*/
class AParam {
 public:
  AParam() {name="no",title="no"; val=0;valmin=-1e-10;valmax=1.0e+10;type=0;}
  AParam(string stitle,string aname,float aval,float avmin, float avmax, int atype) 
  {name=aname; title=stitle; val=aval;valmin=avmin;valmax=avmax;type=atype;}
  AParam(string aname, double aval) {name=aname; val=aval;}
  ~AParam() {}
  bool select() {if(val<=valmin||val>valmax) return false; else return true;}
  void set(string aname, float aval) {name=aname; title="no";val=aval;valmin=-1e-10;valmax=
1.0e+10;type=0;}
  void fill(double v) {val=v;}
  string  name;
  string  title;
  float   val;
  float   valmin;
  float   valmax;
  int     type;
  TH1F*   histo;
};

/**
 * MVA manager
 * have two options:
 * 1. Training
 *    the training is done on custom tree(eg created with OutTreeManager)
 *    the parameters used in MVA are defined in config files
 *    and extracted form this tree branches
 *    The signal and background samples can be delivered separtly or
 *    extracted from the same file using cuts, stored in config files as well
 *    After training the weights are stored in location defined in configs
 *    and can be used to extract MVA out during read
 * 2. Read MVA
 *    This can be done with any tree, the onfly tree branches are created
 *    and passed to MVA which read the weight files and calculate output
 *    for each event.    
 *
*/
class MVAmanager {
 public:
  MVAmanager(ConfigParser* c);
 ~MVAmanager();
  int fillvariables(string fname);
  int fillvariables();
  void setmethods();
  int initfactory();
  int initreader();
  int initreadvariables();
 
  void setcuts();
  TCut*  readcuts(string fname);
  void bookfactory();
  void bookreader();
  int train();
  int train(vector<TTree*> ts,vector<TTree*> tb, vector<double> ws, vector<double> wb);
  
// get parameters for reader tree
  int setreadertree(TTree* ts);
  vector<AParam*> getparams() {return vvar;} 
  int setVariables(vector<float> mvainput);
  float getclassifier(string mname);
  float getclassifier();
  int Debug;

 private:
 void clearpar(vector<AParam*>* v);
 ConfigParser*  theconf;
 map<string,int> Use;
 vector<AParam*> vvar;
 vector<AParam*> vspecvar;
 vector<AParam*> vallvar;
 TCut*  mycuts;
 TCut*  mycutb;
 TMVA::Factory *factory;
 TMVA::Reader *reader;
 TFile* outputFile;
 vector<Float_t> mvapar;
 vector<Float_t> mvaspec;
 string sclass;
 };

#endif


