#include "amschain.h"
#include "DynAlignment.h"
#include "TSystem.h"

int main(int argc,char *argv[]){
  if(argc<4){
    printf("Usage %s Idx_dir run Output_dir [window] [betaCut]  [fitOrder] [rigidityCut]\n",
	   argv[0]);
     return 0;
  }
  
  // Parse the parameters
  int run;
  char inputDir[4096];
  char outputDir0[4096];
  float betaCut=0.995;
  float rigidityCut=5;
  int   fitOrder=0;
  int window=25;
  sscanf(argv[1],"%s",inputDir);
  sscanf(argv[2],"%i",&run);
  sscanf(argv[3],"%s",outputDir0);

  if(argc==8){
    sscanf(argv[4],"%i",&window);
    sscanf(argv[5],"%f",&betaCut);
    sscanf(argv[6],"%i",&fitOrder);
    sscanf(argv[7],"%f",&rigidityCut);
  }

  cout<<"Looking for runs in "<<inputDir<<endl
      <<"            BetaCut "<<betaCut<<endl
      <<"        RigidityCut "<<rigidityCut<<endl
      <<"           fitOrder "<<fitOrder<<endl
      <<"             window "<<window<<endl;
  
  TString outputDir=Form("%s/%4.3f_%4.3f_%i_%i",
			 outputDir0,
			 betaCut,
			 rigidityCut,
			 window,
			 fitOrder);


  gSystem->mkdir(outputDir);

  /////////////////////////////////////////////////////////////////
  map<int,DynAlFitContainer> final;
  final[1]=DynAlFitContainer();
  final[9]=DynAlFitContainer();
  final[1].ApplyLocalAlignment=false;
  final[9].ApplyLocalAlignment=false;

  // Now fit the run
  DynAlContinuity::FitOrder=fitOrder;
  DynAlContinuity::FitWindow=window;
  DynAlContinuity::BetaCut=betaCut;
  DynAlContinuity::RigidityCut=rigidityCut;



  final[1].FitParameters.clear();
  final[9].FitParameters.clear();
  final[1].BuildAlignment(inputDir,"layer_1_",run);
  final[9].BuildAlignment(inputDir,"layer_9_",run);

  cout<<"SIZE 1: "<<final[1].FitParameters.size()<<endl;
  cout<<"SIZE 9: "<<final[9].FitParameters.size()<<endl;

  TString target=Form("%s/%i",outputDir.Data(),DynAlContinuity::getBin(run));
  gSystem->mkdir(target);

  
  TFile f(Form("%s/%i.align.root",target.Data(),run),"RECREATE");
  final[1].Write("layer_1");
  final[9].Write("layer_9");
  f.Close();
}



