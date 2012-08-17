#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"

int readfile(const char *ifile, AMSChain &ch,int nroot=-1){

//----
   ifstream infile;
   string ifname;
   char rootfile[1000];
   infile.open(ifile);
   if( !infile){
      cerr<<"infile open cerr!"<<" "<<ifile<<endl;
      exit(1);
   }
//---
  int cont=0;
   while(infile.good()){
      infile.getline(rootfile,1000,'\n');
      if(strcmp(rootfile,"")==0)continue;
      ifname=rootfile;
      size_t fond=ifname.find(".root");
      if(fond==string::npos)continue;
      fond=ifname.find("castor");
      if(fond!=string::npos)ifname="root://castorpublic.cern.ch//"+ifname;
      cout<<"cont="<<cont<<" "<<ifname<<endl;
      ch.Add(ifname.c_str());
      cont++;
      if((nroot>=0)&&(cont>=nroot))break;
   }
   infile.close();
//--   
  return 0;

}

//---mode=0 normal file mode=1 svcClass
int readfile(const char *ifile, TChain &ch,int nroot=-1,int mode=0){

//----
   ifstream infile;
   string ifname;
   char rootfile[1000];
   infile.open(ifile);
   if( !infile){
      cerr<<"infile open cerr!"<<" "<<ifile<<endl;
      exit(1);
   }
//---
  int cont=0;
   while(infile.good()){
      infile.getline(rootfile,1000,'\n');
      if(strcmp(rootfile,"")==0)continue;
      ifname=rootfile;
      size_t fond=ifname.find(".root");
      if(fond==string::npos)continue;
      fond=ifname.find("castor");
      if(fond!=string::npos){
         ifname="root://castorpublic.cern.ch//"+ifname;
         if(mode!=0)ifname+="?svcClass=amsuser";
      }
      cout<<"cont="<<cont<<" "<<ifname<<endl;
      if(mode!=0)ch.AddFile(ifname.c_str());
      else       ch.Add(ifname.c_str());
      cont++;
      if((nroot>=0)&&(cont>=nroot))break;
   }
   infile.close();
//--   
  return 0;

}



