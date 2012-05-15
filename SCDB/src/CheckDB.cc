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
ClassImp(SlowControlDB);

using namespace std;

int reference_size(int node,int datatype){
  if(node==544&&datatype==32) return 27;
  if(node==545&&datatype==32) return 27;
  if(node==546&&datatype==32) return 27;
  if(node==547&&datatype==32) return 27;
  if(node==548&&datatype==32) return 27;
  if(node==549&&datatype==32) return 27;
  if(node==550&&datatype==32) return 27;
  if(node==551&&datatype==32) return 27;
  if(node==198&&datatype==20) return 84;
  if(node==199&&datatype==20) return 84;
  if(node==202&&datatype==20) return 84;
  if(node==203&&datatype==20) return 84;
  if(node==137&&datatype==15) return 1;
  if(node==137&&datatype==17) return 98;
  if(node==19&&datatype==2033426) return 20;
  if(node==40&&datatype==25) return 187;
  if(node==41&&datatype==25) return 187;
  if(node==108&&datatype==6) return 46;
  if(node==108&&datatype==7) return 24;
  if(node==108&&datatype==8) return 4;
  if(node==108&&datatype==9) return 6;
  if(node==108&&datatype==24) return 49;
  if(node==108&&datatype==25) return 22;
  if(node==108&&datatype==26) return 4;
  if(node==109&&datatype==6) return 46;
  if(node==109&&datatype==7) return 24;
  if(node==109&&datatype==8) return 4;
  if(node==109&&datatype==9) return 6;
  if(node==109&&datatype==24) return 49;
  if(node==109&&datatype==25) return 22;
  if(node==109&&datatype==26) return 4;
  if(node==28&&datatype==17) return 32;
  if(node==28&&datatype==24) return 9;
  if(node==28&&datatype==25) return 9;
  if(node==28&&datatype==27) return 40;
  if(node==28&&datatype==28) return 32;
  if(node==29&&datatype==17) return 32;
  if(node==29&&datatype==24) return 9;
  if(node==29&&datatype==25) return 9;
  if(node==29&&datatype==27) return 40;
  if(node==29&&datatype==28) return 32;
  if(node==44&&datatype==16) return 24;
  if(node==44&&datatype==25) return 98;
  if(node==45&&datatype==16) return 24;
  if(node==45&&datatype==25) return 98;
  if(node==120&&datatype==15) return 4;
  if(node==120&&datatype==20) return 9;
  if(node==120&&datatype==25) return 228;
  if(node==121&&datatype==15) return 4;
  if(node==121&&datatype==20) return 9;
  if(node==121&&datatype==25) return 228;

  return -1;
}

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

  if(debug)printf("select node - %i : %i\n",node,(it!=select.end()));
  return (it!=select.end());
}

bool check_selection(map<int,vector<int> > select,int node, int datatype, int debug){
  if((int)select.size()==0)return true;
  map<int,vector<int> >::const_iterator it;
  it=select.find(node);
  if(it==select.end())return false;
  if(it->second.size()==0)return true;

  for(int j=0;j<(int)it->second.size();j++){
    if(debug)printf("select datatypes - dt %i %i\n",j,it->second[j]);
    if(datatype==it->second[j])return true;
  }
  return false;
}

int main(int argc, char *argv[]){
  if(argc<2||argc>4){
    printf("USAGE %s <file> <configfile> (<debug>)\n",argv[0]);
    exit(1);
  }
  
  // set debub level
  int debug=0;
  if(argc==4)debug=atoi(argv[3]);
  
  // map to specify which node address and which datatype should be taken into account
  map<int,vector<int> > select;
  if(argc>2&&argv[2]!="0"){
    
    select=ParseConfigFile(argv[2],debug);
    if(debug)PrintConfig(select);
  }

  SlowControlDB* scdb=SlowControlDB::GetPointer();
  scdb->Load(argv[1]);
  
  FILE *fout=0;
  if(debug<0){
    string fnam(argv[1]);
    size_t found=fnam.find_last_of("/");
    if(found!=string::npos)fnam=fnam.substr(found+1);
    char outnam[100];
    sprintf(outnam,"%s_out",fnam.c_str());
    fout=fopen(outnam,"w");
  }

  // browse nodes
  for(std::map<std::string,Node>::iterator node=scdb->nodemap.begin();node!=scdb->nodemap.end();node++){
    
    // check if node is in selection
    if(!check_node(select,node->second.number,debug))continue;
    char nodename[10];sprintf(nodename,"%x",node->second.number);
	
    // browse datatypes
    for(std::map<int,DataType>::iterator dt=node->second.datatypes.begin();dt!=node->second.datatypes.end();dt++){
      if(!check_selection(select,node->second.number,dt->second.number,debug))continue;
      char dtname[10];sprintf(dtname,"%x",dt->second.number);
      
      int real_size=dt->second.subtypes.size();
      if(debug<0){
	if(debug<-1)fprintf(fout,"if(node==%i&&datatype==%i) return %i;\n",node->second.number,dt->second.number,real_size);
	else fprintf(fout,"%i %i %i\n",node->second.number,dt->second.number,real_size);
      }
      else{
	// subtypes size
	int expected_size=reference_size(node->second.number,dt->second.number);
	
	if(real_size!=expected_size||real_size<=0){
	  cerr<<"NA "<< nodename << " DT " << dtname << " - unexpected size "<< real_size << " instead of "<< expected_size<<endl;      
	  exit(1);
	}
	else if(debug>0)
	  cout<<"NA "<< nodename << " DT " << dtname << " - "<< real_size << " subtypes found"<<endl;      
      }

      int nempty=0;
      for(int i=0;i<real_size;i++)
	if(dt->second.GetSubTypeN(i)->_table.size()==0){
	  nempty++;
	  if(debug>1)
	    cout<<"NA "<< nodename << " DT " << dtname << " ST "<< dt->second.GetSubTypeN(i)->tag<< " - zero entries"<<endl;      
	}

      if(nempty==real_size&&debug>0)
	cerr<<"NA "<< nodename << " DT " << dtname << " contains only zero length subtypes - node disabled?"<<endl;      

    }
  }

  if(fout)
    fclose(fout);
  return 0;
}
