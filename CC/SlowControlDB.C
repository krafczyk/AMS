#include "SlowControlDB.h"

SlowControlDB* SlowControlDB::head=0;

ClassImp(SlowControlDB);

bool SlowControlDB::Load(const char* fname,unsigned int minT,unsigned int maxT, int debug){
  TFile *f=TFile::Open(fname);
  if(debug)std::cerr <<"opening file "<<fname<<" ptr "<<f<<std::endl;
  if(debug)std::cout<<"requesting time min "<<minT<<" max "<<maxT<< std::endl;
  return Load(f,minT,maxT,debug);
}

bool SlowControlDB::Load(TFile* f,unsigned int minT,unsigned int maxT,int debug){
  if(!f){
    std::cerr<<"file not found"<<std::endl;
    return false;
  }
  //    if(head) delete head;
  
  TTree* _tree=(TTree*)f->Get("SlowControlDB");
  if(!_tree){
    std::cerr<<"SlowControlDB tree not found"<<std::endl;
    return false;
  }
  if(_tree->GetEntries()<1){
    std::cerr<<"tree entries: "<<_tree->GetEntries()<<std::endl;
    return false;
  }

  unsigned int tree_begin,tree_end; 
  _tree->SetBranchAddress("begin",&tree_begin);
  _tree->SetBranchAddress("end",&tree_end);
  
  TObjArray *branchlist=_tree->GetListOfBranches();
  Node* nodearr[branchlist->GetEntries()];
  
  for(int i=0;i<(int)branchlist->GetEntries();i++){
    nodearr[i]=0;
    TObject* obj=(TObject*)branchlist->At(i);
    std::cout << "name: "<< branchlist->At(i)->GetName() << std::endl;
    if(strcmp(obj->ClassName(),"TBranchElement")==0)
      _tree->SetBranchAddress(branchlist->At(i)->GetName(),&nodearr[i]);
  }
  
  int n_add=0;
  for(int i=0;i<(int)_tree->GetEntries();i++){
    _tree->GetEntry(i);
    if(debug)std::cout<<"entry "<<i<<" begin "<<tree_begin<<" end "<<tree_end <<" - add? "<<(tree_begin<maxT&&tree_end>minT)<<std::endl;
    if(tree_begin>maxT||tree_end<minT)continue;
    
    n_add++;
    if(tree_end>end)end=tree_end;
    if(begin=0||tree_begin<begin)begin=tree_begin;
    
    for(int j=0;j<(int)branchlist->GetEntries();j++){
      TObject* obj=(TObject*)branchlist->At(j);
      if(strcmp(obj->ClassName(),"TBranchElement")==0){
	nodearr[j]->SetName(obj->GetName());
	AppendNode((Node*)nodearr[j]);
      }
    }
  }
  f->Close();

  if(debug)std::cout<<"files merged to SlowControlDB "<<n_add<<std::endl;
  return n_add;
}

int SlowControlDB::AppendNode(Node* copynode)
{
  if(!copynode){
    std::cerr<<"AppendNode ptr "<<copynode<<" not found"<<std::endl;
    return 1;
  }
  if(copynode->getnelem()<=0){
    std::cerr<<"AppendNode node "<<copynode->GetName()<<" has "<<copynode->getnelem()<<" entries" << std::endl;
    return 2;
  }
  std::cout<<"node "<<copynode->GetName()<<std::endl;

  std::map<std::string,Node>::iterator it = nodemap.find(std::string(copynode->GetName())); 
  if(it==nodemap.end()){
    nodemap.insert(std::pair<std::string,Node>(std::string(copynode->GetName()),*copynode));
    it = nodemap.find(std::string(copynode->GetName())); 
  }
    
  for(int i=0;i<(int)copynode->getnelem();i++)
    DataType* dt=it->second.Append(copynode->GetDataTypeN(i));
  return 0;
}
