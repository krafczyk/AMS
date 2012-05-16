//  $Id: checka2r.C,v 1.4 2012/05/16 15:50:04 choutko Exp $
#include "TGraph.h"
#include "TH2F.h"
#include "TFile.h"
#include "TTree.h"
extern "C" {
#include "amilib.h"
}

#include <cstdlib>
#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include "xmlrpclib.h"
#include <algorithm>

#include "SlowControlDB.h"

//SlowControlDB* SlowControlDB::head=0;
ClassImp(SlowControlDB);

using namespace std;

map<int,vector<int> > ParseConfigFile(const char* fnam, int debug){
  ifstream input(fnam,ios::in);
  map<int,vector<int> > toReturn;
  
  
  char *p;
  string line;
  if (input.is_open())
    {
      while (getline(input, line))
        {
          istringstream istr (line);
          int n=0;
          string tmp("abcd");
	  int node=0;
	  vector<int> datatypes;

          while(istr.good()){
	    n++;
	    istr >> tmp;

	    if(debug)printf("%s %li\n",tmp.c_str(),strtol( tmp.c_str(), & p, 16 ));
	    if(n==1){
	      long n = strtol( tmp.c_str(), & p, 16 );
	      node=n;
	    }
	    else datatypes.push_back(strtol( tmp.c_str(), & p, 16 ));
	  }
	  
	  toReturn.insert(pair<int,vector<int> > (node,datatypes));
	}
    }
  return toReturn;
};

void PrintConfig(map<int,vector<int> > config){
  for(map<int,vector<int> >::const_iterator it=config.begin();it!=config.end();it++){
    printf("node address 0x%04x\n",it->first);
    if((int)it->second.size()==0)printf("all datatypes\n");
    else{
      printf("data types ");
      for(vector<int>::const_iterator jt=it->second.begin();jt!=it->second.end();jt++)
	printf("0x%02i ",*jt);
      printf("\n");
    }
  }
}

bool check_node(map<int,vector<int> > select,int node, int debug=0){
  if((int)select.size()==0)return true;
  map<int,vector<int> >::const_iterator it;
  it=select.find(node);

  if(debug)printf("check node %i : %i\n",node,(it!=select.end()));
  return (it!=select.end());
}

bool check_selection(map<int,vector<int> > select,int node, int datatype, int debug){
    cout <<" node "<<node<<" "<<select.size()<<endl;
  if((int)select.size()==0)return true;
  map<int,vector<int> >::const_iterator it;
  it=select.find(node);

  if(it==select.end())return true;
  cout <<" 2nd "<<it->second.size()<<endl;
  if(it->second.size()==0)return false;

  for(int j=0;j<(int)it->second.size();j++){
    if(debug)printf("dt %i %i\n",j,it->second[j]);
    printf("dt %i %i\n",j,it->second[j]);
    if(datatype==it->second[j])return false;
  }
  return true;
}

int main(int argc, char *argv[]){
  if(argc<7||argc>8){
    printf("USAGE %s <begin> <end> <rootfileref> <b2> <e2> <rootfiletocheck>  (<debug>)\n",argv[0]);
    return 1;
  }
  time_t timnow;
  time(&timnow);  
  unsigned int notcompleted=1;
  time_t start_real=INT_MAX;
  time_t end_real=0;
  time_t start=atoi(argv[1]);
  time_t end=atoi(argv[2]);
  time_t start2=atoi(argv[4]);
  time_t end2=atoi(argv[5]);
  int shift=0;
   if(getenv("AMI2ROOTSHIFT")){
     shift=atol(getenv("AMI2ROOTSHIFT"));
   }
    cout << "SHIFT "<<shift<<endl; 
 // set debug level
  int debug=0;
  if(argc==8)debug=atoi(argv[7]);
  // map to specify which node address and which datatype should be taken into account
  map<int,vector<int> > select;
    char * config=getenv("SCDBConfig");
     char local[1024]="";
    if(!(config && strlen (config))){
      char * amsdatadir=getenv("AMSDataDir");
      if(amsdatadir && strlen(amsdatadir)){
      strcpy(local,amsdatadir);
      strcat(local,"/v5.00/SCDBConfig.txt");
      config=local;
     }
    }
    cout <<" config debufg"<<config<<" "<<debug<<endl;
    if(config){
      select=ParseConfigFile(config,debug);
       if(debug)PrintConfig(select);
    }  





  SlowControlDB* scdb=SlowControlDB::GetPointer();
  if(scdb && scdb->Load(argv[3],start,end,1)){
     scdb->BuildSearchIndex(1);
     cout <<"  reference input file "<<argv[3]<<endl;
  }
  else{
   SlowControlDB::KillPointer();
    cerr<<"ERROR unable to load reference file scdb "<<argv[3]<<endl;  
    return 1;
}
  if(scdb->uncompleted){
     cerr <<"ERROR   reference file Uncompleted "<<scdb->uncompleted <<endl;
     return 1;
  } 
  SlowControlDB* scdb2=new SlowControlDB();
  if(scdb2 && scdb2->Load(argv[6],start2,end2,1)){
     scdb2->BuildSearchIndex(1);
     cout <<"   file to check"<<argv[6]<<endl;
  }
  else{
    cerr<<"ERROR unable to load scdb "<<argv[6]<<endl;  
    return 2;
   }

   int count=0;
   for(std::map<std::string,Node>::iterator it=scdb->nodemap.begin();it!=scdb->nodemap.end();it++){
      std::map<std::string,Node>::iterator it2=scdb2->nodemap.find(it->first);
     if(it2==scdb2->nodemap.end()){
      cerr<<"ERROR node "<<it->first<<" not found "<<endl;
      
      if(!strstr(it->first.c_str(),"CRATE_"))return 3;
     }
     else {
//        cout<<it2->first<<endl;
        for(std::map<int,DataType>::iterator itd=it->second.datatypes.begin();itd!=it->second.datatypes.end();itd++){
          std::map<int,DataType>::iterator itd2=it2->second.datatypes.find(itd->first);
          if(itd2==it2->second.datatypes.end()){
            cerr<<"ERROR datatype "<<itd->first <<" "<<it->first<<" not found "<<endl;
            if(!strstr(it->first.c_str(),"CRATE_"))return 4;
          }
          else {
//            cout<<itd2->first<<endl; 
             for(std::map<int,SubType>::iterator its=itd->second.subtypes.begin();its!=itd->second.subtypes.end();its++){
              std::map<int,SubType>::iterator its2=itd2->second.subtypes.find(its->first);
              if(its2==itd2->second.subtypes.end()){
                cerr<<"ERROR subtype "<<its->first<<" "<<itd->first <<" "<<it->first<<" not found "<<endl;
              if(!strstr(it->first.c_str(),"CRATE_"))return 5;
              }
              else{
               
//               cout<<its2->first<<" "<<its2->second.tag<<endl; 
                 if(its2->second._table.size()==0 && its->second._table.size() &&!strstr(it2->first.c_str(),"TTCE") &&!strstr(it2->first.c_str(),"JPD_B") &&!strstr(it2->first.c_str(),"JINF_U_0A") &&!strstr(it2->first.c_str(),"JINF_U_1A")   ){
                    cerr<<"  table size 0 " <<it2->first<<" "<<its2->second.tag<<" "<<its->second._table.size()<<endl; 
                    count++;
                 }
              }


          }
        }
     }
   }
}
 if(count){
   cerr<<"ERROR Total of "<<count<<" empty subtypes found "<<endl;
   return 6;
 }
 cout <<"ALLOK"<<endl;
 return 0;
}
