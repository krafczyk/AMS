#include <string>
#include <iostream>

#include "TList.h"
#include "TFile.h"
#include "TTree.h"
#include "SlowControlDB.h"

//SlowControlDB* SlowControlDB::head=0;


const char *TreeName = "SlowControlDB";

int main(int argc, char *argv[]){
  using namespace std;

  TList *tree_list=new TList();
  
  Int_t total_events = 0;

  //  string outfile="tmp.root";
  int i=0;
  unsigned int begin,end,minT=UINT_MAX,maxT=0;
  while(++i<argc) {
    TFile *f = new TFile(argv[i]);
    cout << "Adding file: " << argv[i] << endl;

    if(TTree *tree = (TTree *)f->Get(TreeName)) {
      tree_list->Add(tree);
      total_events += (Int_t )tree->GetEntries();
      cout << "tree found " << (Int_t )tree->GetEntries() << " entries" << endl;

      tree->SetBranchAddress("begin",&begin);
      tree->SetBranchAddress("end",&end);
      
      for(int i=0;i<(int)tree->GetEntries();i++){
	tree->GetEntry(i);
	printf("begin %i end %i minT %i max %i\n",begin,end,minT,maxT);
	if(begin<minT)minT=begin;
	if(end>maxT)maxT=end;
      }
      
    } else {
      cout << "File has no TTree named " << TreeName << endl;
    }
  }
  
  char outnam[100];
  sprintf(outnam,"SCDB.%i.%i.root",minT,maxT);
  TFile output(outnam, "RECREATE");

  cout << "Merging trees...patience..." << endl;
  TTree::MergeTrees(tree_list);
  
  output.Write();
  cout << "Total Events: " << total_events << endl;
}
