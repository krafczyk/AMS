#include <iostream.h>
#include <TBranch.h>
#include <root.h>
#include <stdio.h>

int rootread(char * fname, int nev, int iver,  int & lastevent){
 int LastEvent=lastevent;
int firstevent=-1;
 if(!iver){
  fclose(stderr);
  cout.setstate(ios_base::failbit);
  cerr.setstate(ios_base::failbit);
 }
 TFile * rfile= new TFile(fname,"READ");
 if(!rfile){
        if(iver>0)cout <<"problem to open file "<<fname<<" "<<endl;
	return -1;
 }
 TTree * tree=dynamic_cast<TTree*>(rfile->Get("AMSRoot"));
 if(!tree){
        if(iver>0)cout <<"problem to find tree AMSRoot "<<endl;
	return -1;
 }
 int nevents=tree->GetEntries();
 if(iver)cout <<"  entries "<<nevents<<" "<<nev<<endl;
 if(nevents<=0)return -2;
 else if(nev>nevents)return -3;
 else if(nev<nevents)return -4;
  AMSEventR *pev = new AMSEventR();
  tree->SetMakeClass(1);
  pev->SetBranchA(tree);
  pev->GetBranch(tree);
  int nbadev=0;
  int nevread=0;
  for (int i=0;i<nevents;i++){
   if(!pev->ReadHeader(i))break;
   if((pev->fHeader.Status[0]/1073741824)%2)nbadev++;
   if(firstevent<0)firstevent=pev->fHeader.Event;
   lastevent=pev->fHeader.Event;
   nevread++;
  }
  rfile->Close();
  if(nevread!=nev)return -2;
  if(nev>2){
   int rate=(lastevent+1-firstevent)/nevread;
   if(iver)cout <<"last event "<<LastEvent <<" " <<lastevent<<endl; 
   if(abs(LastEvent-lastevent)>5*rate){
     return -5;
   } 
  }
  float rrr=100*float(nbadev)/float(nevread);
  return int(rrr); 
}
