#include "TGraph.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
#include <cstdlib>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "SlowControlDB.h"
SlowControlDB* SlowControlDB::head=0;

ClassImp(SlowControlEl);
ClassImp(SlowControlDB);

using namespace std;

int main(int argc, char *argv[]){
  if(argc<3||argc>4){
    printf("USAGE %s <file> <nodename> (<level>)\n",argv[0]);
    exit(1);
  }
  
  SlowControlDB* scdb=SlowControlDB::GetPointer();
  scdb->Load(argv[1],1,1301290000,1);
  
  printf("SCDB built\n");

  int level=0;
  if(argc==4)level=atoi(argv[3]);

  Node *node=scdb->GetNode(argv[2]);

  printf("node pointer %d got - datatypes %i\n",node,(int)node->getnelem());
  for(int i=0;i<(int)node->getnelem();i++){
    printf("node %i %s entries %i\n",i,node->GetName(),node->GetDataTypeN(i)->getnelem());
    if(level==0)continue;
    for(int j=0;j<node->GetDataTypeN(i)->getnelem();j++){
      printf("%i values for entry %i %s\n",node->GetDataTypeN(i)->GetSubTypeN(j)->getnelem(),j,node->GetDataTypeN(i)->GetSubType(j)->tag.c_str());
      if(level==1)continue;
      for(int k=0;k<node->GetDataTypeN(i)->GetSubTypeN(j)->getnelem();k++){
	printf("time %i val %.2f\n",node->GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k),node->GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k));
      }
    }
  }
  
  delete node;
  delete scdb;
  return 0;
}
