#include "OutHistoManager.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <math.h>






OutHistoManager::OutHistoManager() {
  Debug=0;
}
//
OutHistoManager::~OutHistoManager() {
  ClearAll();
}


void OutHistoManager::ClearAll() {
 for(unsigned int i=0; i<thevh1.size();i++) {
   if(thevh1[i])    delete thevh1[i];
}
 thevh1.clear();
 for(unsigned int i=0; i<thevh2.size();i++) {
  if(thevh2[i]) delete thevh2[i];
}
 thevh2.clear();

}

//
int OutHistoManager::WriteAll(TFile* f,string dirname) {
 f->cd(); 
 if(dirname!="none") {
 TDirectory* dir=f->mkdir(dirname.c_str());
 dir->cd(); 
 }
for(unsigned int i=0; i<thevh1.size();i++) {
  if(thevh1[i])  thevh1[i]->Write();
 }
 for(unsigned int i=0; i<thevh2.size();i++) {
   if(thevh1[2])  thevh2[i]->Write();
 }
  return 1;  
} 
//
int OutHistoManager::WriteAll(string fname,string dirname) {
 TFile*  ff  = new TFile(fname.c_str(), "RECREATE" ); 
 return WriteAll(ff,dirname);
} 
//
int OutHistoManager::AddH1(TH1D* h) {
  if(h) thevh1.push_back(h); else return -1;
  return thevh1.size();
} 
//
int OutHistoManager::AddH2(TH2D* h) {
  if(h) thevh2.push_back(h); else return -1;
  return thevh2.size();
} 


//
int OutHistoManager::AddH1(string sname, string stitle, int nb, double xmin, double xmax) {
  thevh1.push_back(new TH1D(sname.c_str(),stitle.c_str(), nb,xmin, xmax));
  return thevh1.size();
} 
//
int OutHistoManager::AddH2(string sname, string stitle, int nb1, double xmin1, double xmax1, int nb2, double xmin2,double xmax2) {
thevh2.push_back(new TH2D(sname.c_str(),stitle.c_str(), nb1,xmin1, xmax1,nb2,xmin2,xmax2));
 return thevh2.size();
} 
//
int OutHistoManager::FillH2(string name, double x, double y) {
for(unsigned int i=0; i<thevh2.size();i++) {
   string hname=string(thevh2[i]->GetName());
   if(name==hname) { thevh2[i]->Fill(x,y); return 1;}
 }
  return 1;  
}
//
int OutHistoManager::FillH1(string name, double x) {
for(unsigned int i=0; i<thevh1.size();i++) {
   string hname=string(thevh1[i]->GetName());
   if(name==hname) { thevh1[i]->Fill(x); return 1;}
 }
  return 1;  
}


//
TH1D* OutHistoManager::GetH1byName(string aname) {
for(unsigned int i=0; i<thevh1.size();i++) {
   string hname=string(thevh1[i]->GetName());
   if(aname==hname) {return thevh1[i];}
 }
 return 0;
}

TH2D* OutHistoManager::GetH2byName(string aname) {
for(unsigned int i=0; i<thevh2.size();i++) {
   string hname=string(thevh2[i]->GetName());
   if(aname==hname) {return thevh2[i];}
 }
 return 0;

}

