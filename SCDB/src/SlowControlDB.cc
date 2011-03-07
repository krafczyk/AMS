#include "SlowControlDB.h"

SlowControlDB* SlowControlDB::head=0;

ClassImp(SlowControlDB);

bool SlowControlDB::Load(const char* fname,unsigned int minT,unsigned int maxT, int debug){
  TFile *f=TFile::Open(fname);
  if(debug)printf("opening file %s ptr\n",fname);
  if(debug)printf("requesting time min %i max %i\n",minT,maxT);
  return Load(f,minT,maxT,debug);
}

bool SlowControlDB::Load(TFile* f,unsigned int minT,unsigned int maxT,int debug){
  if(!f) return false;
  //    if(head) delete head;
  if(debug)printf("ptr %d %d\n",this,GetPointer());
  
  TTree* _tree=(TTree*)f->Get("SlowControlDB");
  if(!_tree){
    printf("SlowControlDB tree not found\n");
    return false;
  }
  
  _tree->SetBranchAddress("begin",&begin);
  _tree->SetBranchAddress("end",&end);
  Branch("begin", &begin,"begin/i");
  Branch("end", &end,"end/i");

  TObjArray *branchlist=_tree->GetListOfBranches();
  for(int i=0;i<(int)branchlist->GetEntries();i++){
    TObject* obj=(TObject*)branchlist->At(i);
    if(strcmp(obj->ClassName(),"TBranchElement")==0){
      Node *node=0;
      _tree->SetBranchAddress(branchlist->At(i)->GetName(),&node);
      Branch(branchlist->At(i)->GetName(),"Node",&node);
    }
  }
  _tree->SetBranchStatus("*",1);
  
  for(int i=0;i<_tree->GetEntries();i++){
    _tree->GetEntry(i);
    if(debug)printf("entry %i begin %i end %i - add? %i\n",i,begin,end,(begin<maxT&&end>minT));
    if(begin>maxT||end<minT)continue;
    Fill();
  }
  f->Close();

  if(debug)printf("files merged to SlowControlDB %i ptr %d %d\n",(int)GetEntries(),this,GetPointer());
  return GetEntries();
}
