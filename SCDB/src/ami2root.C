//  $Id: ami2root.C,v 1.17 2011/07/06 08:21:35 choutko Exp $
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
  if(argc<6||argc>7){
    printf("USAGE %s <begin> <end> <rootfilei> <rootfileo> <timeout> (<debug>)\n",argv[0]);
    return 1;
  }
  time_t timnow;
  time(&timnow);  
  unsigned int notcompleted=1;
  time_t start_real=INT_MAX;
  time_t end_real=0;
  time_t start=atoi(argv[1]);
  time_t end=atoi(argv[2]);
  time_t timeout =atol(argv[5]);
  int shift=0;
   if(getenv("AMI2ROOTSHIFT")){
     shift=atol(getenv("AMI2ROOTSHIFT"));
   }
    cout << "SHIFT "<<shift<<endl; 
 // set debug level
  int debug=0;
  if(argc==7)debug=atoi(argv[6]);
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
    cout <<" config debuf "<<config<<" "<<debug<<endl;
    if(config){
      select=ParseConfigFile(config,debug);
       if(debug)PrintConfig(select);
    }  

  xmlrpcstr_t s;

  char xmlurl[160];
  char *server=getenv("AMISERVER");
  char *host=getenv("SCDBHost");
  if(server && strlen(server)){
      setenv("SCDBHost",server,1);
   }
  else{
    if(host && strlen(host)){
      setenv("AMISERVER",host,1);
    }
   }

  host=getenv("SCDBHost");
  char hostlocal[]="http://ams.cern.ch:8081";

  if(! (host && strlen(host)))host=hostlocal;
  sprintf(xmlurl, "%s/AMI/insert/call/xmlrpc", host);

  // prepare xmlrpc environment
  if (xmlrpc_init(&s, xmlurl, 1)) {
    /* xmlrpc already printd the problem */
    cerr <<" unable to connect to "<<host<<endl;
    exit(2);
  }
  SlowControlDB* scdb=SlowControlDB::GetPointer();
  if(scdb && scdb->Load(argv[3],start,end,1)){
     scdb->BuildSearchIndex(1);
     cout <<"  input file "<<argv[3]<<endl;
  }
  else{
   SlowControlDB::KillPointer();
   scdb=0;
}
  if(scdb){
     cout <<"   Uncompleted "<<scdb->uncompleted <<endl;
  } 
  TFile *file=new TFile(argv[4],"recreate");
  if(!file || file->IsZombie()){
    cerr<<" unable to open output file "<<argv[4]<<endl;
    exit(3);
  }
  // initialize output tree



    

  TTree *tree=0;
  tree=new TTree("SlowControlDB","SlowControlDB");



  bool tm=false;    




  // loop over node types available at AMI
  int nnodes=0;
  const char** nodetypes=get_node_types(&nnodes) ;
  if(!nnodes){
    cerr<<" unable to get node_types "<<endl;
    exit(5);
  }

// get last scan block time

  unsigned int timelast = get_lsbf_time();
   cout <<"  last time "<<timelast<<endl;
if(timelast<86400+start){
  notcompleted=3;
  goto finish;
}
else if(timelast<end){
 end=timelast;
}


  for(int inode=0;inode<nnodes;inode++){
    
    // loop over node numbers 
    int nnum=0;
    node_numbers** node_numbers=get_node_numbers(nodetypes[inode],&nnum) ;
    for(int num=0;num<nnum;num++){

      // check if node is in selection
      //if(!check_node(select,node_numbers[num]->node_number,debug))continue;
      
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
         if(!datatypes){
           cerr<<"  Unable to get datatypes "<<endl;
           continue;    
           tm=true;
           goto finish;
         }      

      DataType *datatype=0;
        time_t t1=time(&t1);
        if(t1-timnow>timeout){
          cerr<< "  TimeOut "<<endl;
          tm=true;
          goto finish;
        }
       for(int data_type=0;data_type<ndata_types;data_type++){
	// check if datatype is in selection
	if(!check_selection(select,node_numbers[num]->node_number,datatypes[data_type]->data_type,debug))continue;

	printf("Processing %s | %s | %s | %x...\n",node_numbers[num]->name,node_numbers[num]->node_type_name,datatypes[data_type]->name,node_numbers[num]->node_number);
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
        //  only read valuesfrom ami if not found in scdb
        bool read=scdb==NULL;
        std::map<std::string,Node>::iterator it;
        if(!read){        
            it=scdb->nodemap.find(std::string(nname));
            if(it==scdb->nodemap.end())read=true;
        }
        int a;
        if(!read){
           std::map<int,DataType>::iterator itd;
            itd=it->second.datatypes.find(datatype->number);
            if(itd==it->second.datatypes.end())a=0;
            else{
              std::map<int,SubType>::iterator its;
              its=itd->second.subtypes.find(subtype->number);
              if(its==itd->second.subtypes.end())a=0;
              else{
    	if(its->second._table.size())cout <<" tag read from old file "<<subtype->tag<<" "<<its->second._table.size()<<endl;  
                for(std::map<unsigned int,float>::iterator itv=its->second._table.begin();itv!=its->second._table.end();itv++){
          	  subtype->Add(itv->first,itv->second);
                  if(itv->first<start_real)start_real=itv->first;
                  if(itv->first>end_real)end_real=itv->first;
                 }
              }
            }

          }
        if(read){        
	// read values for selected NA/DT/ST in given time range from AMI
 	int nval=0;

	data_vals** vals=0;
//         vals=get_real_valsN(node_numbers[num]->name,datatypes[data_type]->name,start,end, &nval);
             int retry=0;
retry:
         vals=get_real_valsN_fast(node_numbers[num]->name,datatypes[data_type]->name,start,end, &nval);
         if(!vals ){
           cerr<<"  Unable to get values "<<endl;
           
//           tm=true;
//           goto finish;
if(retry++>1){
              continue;
}
else{
             sleep(3);
             goto retry;
}
         }      
	if(nval)cout <<" tag "<<subtype->tag<<" "<<nval<<endl;  
	for(int ii=0;ii<nval;ii++){
	  subtype->Add(vals[ii]->timestamp,vals[ii]->val);
                  if(vals[ii]->timestamp<start_real)start_real=vals[ii]->timestamp;
                  if(vals[ii]->timestamp>end_real)end_real=vals[ii]->timestamp;
         }
      }
      }
      // create and fill node branch
      TBranch* branch=tree->Branch(nname,"Node",&node);
      branch->Fill();
    }
  }
finish:
  // fill tree
  cout <<" start,end real "<<start_real<<" "<<end_real<<endl;
// change start_real end_real only if end>timnow-shift;

if(!tm)notcompleted=0;
if(end>timnow-shift){
if(start_real<end_real ){
  if(start_real>start)start=start_real;
  if(end_real<end)end=end_real;
}
else{
 notcompleted=2;
}
}
  tree->Branch("begin", &start,"start/i");
  tree->Branch("end", &end,"end/i");
  tree->Branch("uncompleted", &notcompleted,"uncompleted/i");
  tree->Fill();
  
  // write file
  file->Write();
  file->Close();
  if(!tm){
     cout <<"ALLOK"<<endl;
      return 0;
   }
   else{
     cout <<"TMOUT"<<endl;
      return 4;
   }
}
