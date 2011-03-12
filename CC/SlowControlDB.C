#include "SlowControlDB.h"

SlowControlDB* SlowControlDB::head=0;

ClassImp(SlowControlDB);
ClassImp(Node);
ClassImp(DataType);
ClassImp(SubType);

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
    if(begin==0||tree_begin<begin)begin=tree_begin;
    
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

bool SlowControlDB::BuildSearchIndex(int debug){
  if(debug)std::cout<<"Enter SlowControlDB::BuildSearchIndex"<<std::endl;
  searchmap.clear();
  
  if(debug)std::cout<<"Nodemap size"<<nodemap.size()<<std::endl;
  for(std::map<std::string,Node>::iterator it=nodemap.begin();it!=nodemap.end();it++){
    for(int i=0;i<(int)it->second.getnelem();i++)
      for(int j=0;j<it->second.GetDataTypeN(i)->getnelem();j++){
	unsigned int idx=100000*it->second.number+1000*it->second.GetDataTypeN(i)->number+it->second.GetDataTypeN(i)->GetSubTypeN(j)->number;
	searchmap.insert(std::pair<std::string,unsigned int>((std::string)(const char*)it->second.GetDataTypeN(i)->GetSubTypeN(j)->tag,idx));
      }
  }
  if(debug)std::cout<<"SlowControlDB::BuildSearchIndex - map size"<<searchmap.size()<<std::endl;
  if(debug)
    for(std::map<std::string,unsigned int>::const_iterator it=searchmap.begin();it!=searchmap.end();it++){
      unsigned int node=it->second/100000;
      unsigned int dt=it->second%100000/1000;
      unsigned int st=it->second%1000;
      printf("subtype %s - NA 0x%03x DT 0x%02x ST %i \n",it->first.c_str(),node,dt,st);
      std::cout<<"subtype "<<it->first<<" - NA 0x"<<std::hex<<node<<" DT 0x"<<std::hex<<dt<<" ST "<<st<<std::endl; 
    }
  
  
  return true;
}

bool SlowControlDB::SaveToFile(const char* fname,int debug){
  if(debug)std::cout<<"Enter SlowControlDB::SaveToFile "<<fname<<std::endl;
  TFile *file=new TFile(fname,"update");
  if(debug)std::cout<<"File ptr "<<file<<std::endl;

  TTree *tree=new TTree("SlowControlDB","SlowControlDB");

  if(debug)std::cout<<"nodes "<<nodemap.size()<<std::endl;
  for(std::map<std::string,Node>::const_iterator nit=nodemap.begin();nit!=nodemap.end();nit++){
    if(debug)std::cout<<"node "<<nit->first<<std::endl;
    Node* node=(Node*)&nit->second;
    TBranch* branch=tree->Branch(nit->first.c_str(),"Node",&node);
    branch->Fill();
  }
  if(debug)std::cout<<"end of loop"<<std::endl;
  
  // fill tree
  tree->Branch("begin", &begin,"begin/i");
  tree->Branch("end", &end,"end/i");
  tree->Fill();
  if(debug)std::cout<<"end of fill tree"<<std::endl;
  
  // write file
  file->Write();
  file->Close();
  if(debug)std::cout<<"file closed"<<std::endl;
  
  //  delete tree;
  //  delete file;
  return true;
}
