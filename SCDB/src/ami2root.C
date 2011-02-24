//  $Id: ami2root.C,v 1.1 2011/02/24 00:02:37 choutko Exp $
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

SlowControlDB* SlowControlDB::head=0;
ClassImp(SlowControlEl);

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

	    if(debug)printf("%s %i\n",tmp.c_str(),strtol( tmp.c_str(), & p, 16 ));
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
  if((int)select.size()==0)return true;
  map<int,vector<int> >::const_iterator it;
  it=select.find(node);
  if(it==select.end())return false;
  if(it->second.size()==0)return true;

  for(int j=0;j<(int)it->second.size();j++){
    if(debug)printf("dt %i %i\n",j,it->second[j]);
    if(datatype==it->second[j])return true;
  }
  return false;
}

int main(int argc, char *argv[]){
  if(argc<3||argc>5){
    printf("USAGE %s <begin> <end> <rootfile> (<debug>)\n",argv[0]);
    exit(1);
  }
  
  time_t start=atoi(argv[1]);
  time_t end=atoi(argv[2]);

  // set debub level
  int debug=0;
  if(argc==5)debug=atoi(argv[4]);
  
  // map to specify which node address and which datatype should be taken into account
  map<int,vector<int> > select;
    char * config=getenv("SLDBConfig");
     char local[1024]="";
    if(!(config && strlen (config))){
      char * amsdatadir=getenv("AMSDataDir");
      if(amsdatadir && strlen(amsdatadir)){
      strcpy(local,amsdatadir);
      strcat(local,"/v4.00/SLDBConfig.txt");
      config=local;
     }
    }
    if(config){
      select=ParseConfigFile(argv[3],debug);
       if(debug)PrintConfig(select);
    }  

  xmlrpcstr_t s;
  char amihost[80];
  char xmlurl[160];

  //  sprintf(amihost, "ams.cern.ch:8081");
  sprintf(amihost, "pcposk0:8081");
  sprintf(xmlurl, "http://%s/AMI/insert/call/xmlrpc", amihost);
  // prepare xmlrpc environment
  if (xmlrpc_init(&s, xmlurl, 0)) {
    /* xmlrpc already printd the problem */
    exit(3);
  }

  // initialize output file
  TFile *file=new TFile(argv[3],"update");
  
  // initialize output tree
  TTree *tree=0;
  tree=new TTree("SlowControlDB","SlowControlDB");


  // loop over node types available at AMI
  int nnodes=0;
  const char** nodetypes=get_node_types(&nnodes) ;
  for(int inode=0;inode<nnodes;inode++){
    
    // loop over node numbers 
    int nnum=0;
    node_numbers** node_numbers=get_node_numbers(nodetypes[inode],&nnum) ;
    for(int num=0;num<nnum;num++){

      // check if node is in selection
      if(!check_node(select,node_numbers[num]->node_number,debug))continue;
      
      // replace forbidden characters in nodename
      char nname[20];
      sprintf(nname,"%s",node_numbers[num]->name);
      replace( nname, nname+strlen(nname), '-','_');
      replace( nname, nname+strlen(nname), ' ','_');
      replace( nname, nname+strlen(nname), '/','_');
      
      // create Node object
      Node *node=NULL;
      node=new Node(node_numbers[num]->name,node_numbers[num]->node_type_name);
      node->number=node_numbers[num]->node_number;
      
      // loop over node datatypes (read from AMI)
      int ndata_types=0;
      data_types** datatypes=get_data_types(node_numbers[num]->node_type_name,&ndata_types);

      DataType *datatype=0;
      for(int data_type=0;data_type<ndata_types;data_type++){

	// check if datatype is in selection
	if(!check_selection(select,node_numbers[num]->node_number,datatypes[data_type]->data_type,debug))continue;
	printf("Processing %s %s ...\n",node_numbers[num]->name,datatypes[data_type]->name);

	// create DataType object and link to Node
	if(!datatype||datatype->number!=datatypes[data_type]->data_type){
	  datatype=new DataType(datatypes[data_type]->data_type);
	  datatype->number=datatypes[data_type]->data_type;
	  datatype=node->Append(datatype);
	}

	// create SubType object and link to DataType
	SubType *subtype=new SubType(datatypes[data_type]->subtype);
	subtype->number=datatypes[data_type]->subtype;
	subtype=datatype->Append(subtype);
	subtype->tag=datatypes[data_type]->name;
	  
	// read values for selected NA/DT/ST in given time range from AMI
 	int nval=0;
	data_vals** vals=get_real_valsN(node_numbers[num]->name,datatypes[data_type]->name,start,end, &nval);
	for(int ii=0;ii<nval;ii++)
	  subtype->Add(vals[ii]->timestamp,vals[ii]->val);

      }
      
      // create and fill node branch
      TBranch* branch=tree->Branch(nname,"Node",&node);
      branch->Fill();
    }
  }
  
  // fill tree
  tree->Branch("begin", &start,"start/i");
  tree->Branch("end", &end,"end/i");
  tree->Fill();
  
  // write file
  file->Write();
  file->Close();
  
  return 0;
}
