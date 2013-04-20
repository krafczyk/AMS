#include "OutTreeManager.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>
#include <TClass.h>


using namespace std;

OutTreeManager::OutTreeManager() {
  Debug=0;
  thefilei=0;
  thefileo=0;
  thetreei=0;
  thetreeo=0;
  nentries=0;
  nev=0;
  npar=0;
  t_par=0;
  cpentry=0.;

}

OutTreeManager::~OutTreeManager() {
  CleanPars();
 if(thefilei) thefilei->Close();
 if(thefileo) thefileo->Close();
}

int   OutTreeManager::LoadTreeList(string alist, string trname) {
string sfile;
ifstream infile (alist.c_str());
 if(!infile)  {
    cout<<" *********cannot open  file "<<alist<<endl;;
  }
thetreei= new TChain(trname.c_str());

 while(infile>>sfile) {
   thetreei->AddFile(sfile.c_str());
   if(Debug>2) cout<<" added= "<<sfile<<endl;
 }
 GetBranches(thetreei);
 return 1;
}
//
int   OutTreeManager::LoadTree(string aname, string trname) {
thefilei   = new TFile(aname.c_str(), "UPDATE" );
 if(thefilei->IsZombie())  return 0;
 thetreei= new TChain(trname.c_str());
 thetreei->AddFile(aname.c_str());
 // thetreei=(TTree*)thefilei->Get(trname.c_str());
 if(thetreei==0) return 0;
 GetBranches(thetreei);
 return 1;
}
//
int   OutTreeManager::InitTree(string aname, string trname) {
 if(npar==0) return 0;
 thefileo  = new TFile(aname.c_str(), "RECREATE" ); 
 thefileo->cd();
 thetreeo=new TTree(trname.c_str(),trname.c_str());
 if(thetreeo==0) return 0;
 SetBranches(thetreeo);
 return 1;

}
// set all no filled parameters to -99
void OutTreeManager::ResetPars(){
for(vector<TreePar*>::iterator ip= theparams.begin(); ip!= theparams.end();ip++)  
    (*ip)->val=-99;
 for(int k=0;k<npar;k++) 
           t_par[k]=-99;

   return;
}
//
void OutTreeManager::CleanPars(){
  CleanPars(theparams);
}
//
int OutTreeManager::FillTree() {
  if(thetreeo==0) return 0;
 thetreeo->Fill();
  ResetPars();
 return 1;
}
// write output tree
int OutTreeManager::WriteTree() {
 if(thefileo==0) return 0;
 if(thetreeo==0) return 0;
  thefileo->cd();
  thetreeo->Write();
  thefileo->Close();
  thetreeo=0;
  thefileo=0;
 return 1;
}
//
int OutTreeManager::UpdateTreePars(vector<double>& aval) {
  if(aval.size()!=theparams.size()) return 0;
for(unsigned int i=0;i<aval.size();i++) {
  theparams[i]->val=aval[i];
  t_par[i]=(theparams[i])->val;
  }
  return 1;
}

// update theparams  from t_par assume number and order of params is the same
int OutTreeManager::UpdateTreePars() {
for(int i=0;i<npar;i++) {
  theparams[i]->val=t_par[i];
  }
  return 1;
}

//
int OutTreeManager::FillTreePar(string aname, double v) {
for(int i=0;i<npar;i++) {
  if(aname==theparams[i]->name) {
    theparams[i]->val=v;
     for(int k=0;k<npar;k++) {
       if(ppar[k]->name==aname)  {
           t_par[k]=v;
	   break;
       }
      }
     break; 
    }
  }
  return 1;
}


// clean up all current parameters
void OutTreeManager::CleanPars(vector<TreePar*> par){
   for(vector<TreePar*>::iterator ip= theparams.begin(); ip!= theparams.end();ip++)  delete *ip;   
   par.clear();
   npar=0;
   return;
}
// add parameter by name
int OutTreeManager::AddPar(string aname){
  theparams.push_back(new TreePar(aname,-99.));
  npar=int(theparams.size());
  return npar;
}

// new tree branches, set pointers
int  OutTreeManager::SetBranches(TTree* tree) {
  if(theparams.size()==0) return 0;
  nentries=0;
  t_entry=0;
tree->Branch("entry",&t_entry);
 if(!t_par) { 
t_par=new Float_t[npar];
ppar=new  TreePar* [npar];
 for(int i=0;i<npar;i++) {
   t_par[i]=-99;
   ppar[i]=0;
    }
 }
   int ii=0;
 for(vector<TreePar*>::iterator ip= theparams.begin(); ip!= theparams.end();ip++) {
  Float_t* val=&t_par[ii];
  ppar[ii]=(*ip);
  string snam=(*ip)->name;
   // string sformat=(*ip)->name+"/D";
   //  tree->Branch(snam.c_str(),val,sformat.c_str() );
 tree->Branch(snam.c_str(),val);
   ii++;
 }

return 1;
}

// get pointers to the existing  tree branches
int  OutTreeManager::GetBranches(TChain* tree) {
tree->SetBranchAddress("entry",&t_entry);
  if(t_par) delete [] t_par;
  t_par=new Float_t[npar];
  int ii=0;
   for(vector<TreePar*>::iterator ip= theparams.begin(); ip!= theparams.end();ip++) {
   tree->SetBranchAddress(((*ip)->name).c_str(),&t_par[ii]);
   ii++;
 }
   if(nentries==0) nentries=tree->GetEntries();
   return 0;
}
//read tree
int  OutTreeManager::NextEvent(TChain* tree,int iev) {
 if(tree==0) return 0;
 if(iev>nentries) return 0;
 if(!tree->GetEvent(iev)) return 0;
UpdateTreePars();
  return 1;
}
//
int  OutTreeManager::NextEvent() {
  if(NextEvent(thetreei,nev)) {
  nev++;
  return 1;  
  } else return 0;
}

//
double OutTreeManager::GetTreePar(string name) {
  double par=-999;
 for(int k=0;k<npar;k++) {
     if(name==theparams[k]->name) {
       return theparams[k]->val;
     }
 }
 return par;
}

