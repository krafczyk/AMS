#ifndef OUTTREEMANAGER_HH
#define OUTTREEMANAGER_HH

#include <iostream>
#include <string>
#include <TTimeStamp.h>
#include <TClass.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1.h>
#include <TH2.h>


using namespace std;

/** Generic out TreeManager
 * all branchecs are declared as double
 * only one variable per branch is allowed
 * Example of use:
  * OutTreeManager* fTreeMan=new OutTreeManager()
  * fTreeMan->AddPar("time");
  * etc...
  *   if(!fTreeMan->InitTree(outputFileName,"AMSA")) {
  *  cout<<"** failed to create outtree "<<endl;
  * return -1; 
  * }
  * while (fm.GetNextEvent()) {
  *  AC::Event* event = fm.Event()
  * fTreeMan->FillTreePar("event",event->EventHeader().Event() );
  *...
  *fTreeMan->FillTree();
  *  }
  * fTreeMan->WriteTree();
  * In analysis:
  * To read list of created .root, create all parameters as before
  * ....
  * string finalist="alllist.list";
  *  if(!fTreeMan->LoadTreeList(finalist,"AMSA")) {
  * cout<<"** failed to create outtree "<<endl;
  * return -1; 
  * }
  * while (fTreeMan->NextEvent()) {
  * int neve= fTreeMan->GetTreePar("event")
  *} 

  * 
  * 3.07.2012-  VZ 
  */
class TreePar;

class OutTreeManager{
public:
  OutTreeManager();
 ~OutTreeManager();
 
int   LoadTree(string aname, string trname="none");
int   LoadTreeList(string aname, string trname="none");
int   InitTree(string afname, string trname="none");
int   AddPar(string aname);
int   GetBranches(TChain* t);
int   SetBranches(TTree* t);
int   UpdateTreePars(vector<double>& val);
int   FillTreePar(string pname, double v);
int   UpdateTreePars();
int   FillTree();
int   WriteTree();
vector<TreePar*> GetTreePars() {return theparams;}
double GetTreePar(string name); // get number by 
double GetTreePar(string aname,int id, int ip); // get number by 
void  ResetPars();
void  CleanPars(); 
void  CleanPars(vector<TreePar*> par); 
int   NextEvent(TChain* tr, int iev);
int   NextEvent();
void  ResetEvent() {nev=0;}


vector<TreePar*> theparams;
TFile* thefilei;
TFile* thefileo;
TChain* thetreei;
TTree* thetreeo;
int    nentries;
int nev;
int nf;
int npar;
 //tree structure
UInt_t   t_entry;
Float_t* t_par;
TreePar**  ppar; 
double cpentry; // current entry
int Debug;
};


class TreePar {
public:
 TreePar(string aname,double aval) {name=aname;val=aval;}
 string name; 
 double val;
  };

#endif // OUTTREEMANAGER_HH
