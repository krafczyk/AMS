#include "amschain.h"
#include "DynAlignment.h"

int main(int argc,char *argv[]){
  AMSChain chain;
  if(argc<3){
    printf("Usage %s input_group_file output\n",argv[0]);
    return 0;
  }

  FILE *fp=fopen(argv[1],"r");
  
  if(!fp){
    printf("Problem with file %s\n",argv[1]);
    return 0;
  }
  
  char buffer[4096];
  while(fscanf(fp,"%s",buffer)==1){
    printf("Adding %s\n",buffer);
    chain.Add(Form("root://castorpublic.cern.ch//%s",buffer));
  }

  // If required, use a different inner alignment
  if(argc==4){
    chain.GetEvent(0);
    cout<<"READING "<<argv[3]<<endl;
    TkDBc::Head->init(3,argv[3]);
    chain.Rewind();
  }
  

  //  DynAlContinuity::BetaCut=0.990; // Unused for v2
  cout<<"CREATING IDX FOR LAYER 1"<<endl;
  DynAlContinuity::CreateIdx(chain,1,argv[2],"layer_1_");
  cout<<"CREATING IDX FOR LAYER 9"<<endl;
  DynAlContinuity::CreateIdx(chain,9,argv[2],"layer_9_");
  
}


