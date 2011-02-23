#include <map>
#include "TNamed.h"
#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include <climits>
#include <cstring>

class SubType{
  std::map<unsigned int,float> _table;
 public:
  std::string tag;
  SubType(){};
 SubType(int i):number(i){};
  int number;

  void Add(unsigned int time, float val){
    _table.insert(std::pair<unsigned int,float>(time,val));
  }

  unsigned int GetTime(int order){
    if(order>=(int)_table.size()) return -999999;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->first;
  }

  float GetValue(int order){
    if(order>=(int)_table.size()) return -999999;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->second;

  }

  int getnelem(){return (int)_table.size();}
  ClassDef(SubType,1);
};

class DataType{
  std::map<int,SubType> subtypes;
 public:
  DataType(){};
 DataType(int i):number(i){};
  int number;
  
  SubType* GetSubType(int i){
    std::map<int,SubType>::iterator it;
    it=subtypes.find(i);
    if(it==subtypes.end())return NULL;
    else return &it->second;
  }
  
  
  SubType* GetSubTypeN(int order){
    if(order>=(int)subtypes.size()) return 0;
    std::map<int,SubType>::iterator it=subtypes.begin();
    for(int ii=0;ii<order;ii++) it++;
    return &it->second;
  }
  
  bool Add(SubType *st){
    std::map<int,SubType>::iterator it;
    it=subtypes.find(st->number);
    if(it!=subtypes.end()){
      //      printf("SubType %i already found\n",st->number);
      return false;
    }
    subtypes.insert(std::pair<int,SubType>(st->number,*st));
    return true;
  }
  
  SubType* Append(SubType *st){
    std::map<int,SubType>::iterator it;
    it=subtypes.find(st->number);
    if(it==subtypes.end()){
      subtypes.insert(std::pair<int,SubType>(st->number,*st));
      it=subtypes.find(st->number);
    }
    //    else delete st;
    for(int i=0;i<st->getnelem();i++)
      it->second.Add(st->GetTime(i),st->GetValue(i));
    //    delete st;
    //    }
    return &it->second;
  }

  int getnelem(){return (int)subtypes.size();}
  
  ClassDef(DataType,1);
};

class Node:public TNamed{
  std::map<int,DataType> datatypes;
 public:
 Node():TNamed("default","default"){};
 Node(char *name):TNamed(name,name){};
 Node(char *name1,char *name2):TNamed(name1,name2){};
  int number;
  
  DataType* GetDataType(int i){
    std::map<int,DataType>::iterator it;
    it=datatypes.find(i);
    if(it==datatypes.end())return NULL;
    else return &it->second;
  }

  DataType* GetDataTypeN(int order){
    if(order>=(int)datatypes.size()) return NULL;
    std::map<int,DataType>::iterator it=datatypes.begin();
    for(int ii=0;ii<order;ii++) it++;
    return &it->second;
  }

  bool Add(DataType *dt){
    std::map<int,DataType>::iterator it;
    it=datatypes.find(dt->number);
    if(it!=datatypes.end()){
      //      printf("DataType %i already found\n",dt->number);
      return false;
    }
    datatypes.insert(std::pair<int,DataType>(dt->number,*dt));

    for(int i=0;i<dt->getnelem();i++)
      dt->Add(dt->GetSubTypeN(i));
    
    return true;
  }

  DataType* Append(DataType *dt){
    std::map<int,DataType>::iterator it;
    it=datatypes.find(dt->number);
    if(it==datatypes.end()){
      datatypes.insert(std::pair<int,DataType>(dt->number,*dt));
      it=datatypes.find(dt->number);
    }
    //    else delete dt;
    //    else{
    for(int i=0;i<dt->getnelem();i++)
      it->second.Add(dt->GetSubTypeN(i));
    //    }
    return &it->second;
  }

  void AddNode(Node* node){
    for(int i=0;i<(int)node->datatypes.size();i++){
      DataType* dt=Append(node->GetDataTypeN(i));
      //      DataType* dt=node->GetDataTypeN(i);
      //      printf("adding dt %i\n",node->GetDataTypeN(i)->number);
      for(int j=0;j<(int)node->GetDataTypeN(i)->getnelem();j++){
	//	printf("subtype %i %i vals %i\n",j,node->GetDataTypeN(i)->GetSubTypeN(j)->number,node->GetDataTypeN(i)->GetSubTypeN(j)->getnelem());

	SubType *st=dt->Append(node->GetDataTypeN(i)->GetSubTypeN(j));
	//	printf(" adding subtype %i\n",node->GetDataTypeN(i)->GetSubTypeN(j)->number);

	for(int k=0;k<node->GetDataTypeN(i)->GetSubTypeN(j)->getnelem();k++){
	  //	  printf("  adding %i %i %.2f\n",k,node->GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k),node->GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k));
	  st->Add(node->GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k),node->GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k));
	}
      }
    }
  }

  int getnelem(){return (int)datatypes.size();}
  ClassDef(Node,1);
};


/// Class holding the pairs timestamp,value for a given quantity
class SlowControlEl:public TNamed{
 private:
  std::map<unsigned int,float> _table;
public:
 
  ///Default Constructor
 SlowControlEl():TNamed("default","default"){};

 SlowControlEl(char* name):TNamed(name,name){};

  ///Destructor
  virtual ~SlowControlEl(){}

  /// Clear all the data
  void Clear(const Option_t* aa=0){_table.clear();}

  /// Find the value at a given time
  /*! \param timestamp  Unix time (sec from 1970)
      \param frac       second fraction after the timestamp  
      \param flag   0 = the closer in time
					1 = linear interplolation
                    2 = polynomial interpolation (to be implemented)
    \return  The desired valu or On Error  -99999
   */
  float Find(unsigned int timestamp,float frac, int flag){
    std::map<unsigned int,float>::iterator it=_table.lower_bound(timestamp);
    if(it==_table.end()) return -99999.;
    unsigned int tmin=it->first;
    float min=it->second;
    it++;
    unsigned int tmax=it->first;
    float max=it->second;
    float dtmin=(timestamp-tmin)+frac;
    float dtmax=(tmax-timestamp)-frac;
    if(flag==2){
      printf("SlowControlEl::Find-W- Polynomial fit not yet implemented, fall back to linear\n");
      flag=1;
    }
    else if(flag==0){
      if( dtmin < dtmax) return min;
      else return max;
    }else if(flag==1){
      return (dtmin*min+dtmax*max)/(dtmin+dtmax);
    }else{
      printf("SlowControlEl::Find-W- Polynomial illegal value of flag %d\n",flag);
      return -99999;      
    }
    return -99999;      
  }

  /// Adds a value to the list. If a value with the same timestamp alredy exist it is overwritten
  bool  Add(unsigned int timestamp, float value){_table[timestamp]=value; return true;}
  
  /// Returns the Number of data available
  int   getnelem(){return _table.size();}
  
  /// returns the  datum at postion order in the series
  float getElem(int order){
    if(order>=(int)_table.size()) return -999999;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->second;
  }

  /// returns the timestamp of the datum at postion order in the series
  unsigned int getElemTime(int order){
    if(order>=(int)_table.size()) return -999999;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->first;
  }

  /// Returns the begin timestamp of the serie
  unsigned int GetBeginTime(){return _table.begin()->first;}

  /// Returns the end timestamp of the serie
  unsigned int GetEndTime(){return _table.end()->first;}

  /// Print info about the quantity
  void Print(const Option_t* aa=0){
    unsigned int bg=GetBeginTime();
    unsigned int en=GetEndTime();
    printf("Name: %s Num Meas: %d\nVal Start: %d %s  Val Stop %d %s\n",GetName(),getnelem(),(int)bg,ctime((time_t*)&bg),(int)en,ctime((time_t*)&en));
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(;it!=_table.end();it++){
      unsigned int tt=it->first;
      printf(" Val: %f %s",it->second,ctime((time_t*)&tt));
    }
    return;
  }

  ClassDef(SlowControlEl,1);
};

typedef  std::map<int,SlowControlEl>::iterator  tabit;
 
/// \brief Class holding a series of slow control data
/*! Class is a singleton and pointer to it can be obtained by the static funtion SlowControlDB::GetPointer().
  Data are retrieved by name and timestamp value.
  The class can be written to a ROOT file by the inherited method Write() and can be loaded by its own methods Load(...)
*/
class SlowControlDB: public TChain{
  
 private:
  static SlowControlDB* head;
  unsigned int begin;
  unsigned int end;
  //  int begin;
  //  int end;
 public:
 SlowControlDB():TChain("SlowControlDB","SlowControlDB"){};
  
  /// Get the pointer to the DB
  static SlowControlDB* GetPointer(){
    return (head)? head :new SlowControlDB(); 
  }

  /// Get the pointer to the DB
  static SlowControlDB* KillPointer(){
    delete head;
    return new SlowControlDB(); 
  }


  /// destructor
  virtual ~SlowControlDB(){}

  /// Load the DB from a root file with name
  bool Load(char* fname,unsigned int minT=0,unsigned int maxT=UINT_MAX, int debug=0){
    TFile *f=TFile::Open(fname);
    if(debug)printf("opening file %s ptr\n",fname);
    if(debug)printf("requesting time min %i max %i\n",minT,maxT);
    return Load(f,minT,maxT,debug);
  }

  /// Load the file from an already opened ROOT File 
  bool Load(TFile* f,unsigned int minT=0,unsigned int maxT=UINT_MAX,int debug=0){
    if(!f) return false;
    //    if(head) delete head;

    TTree* _tree=(TTree*)f->Get("SlowControlDB");
    if(!_tree){
      printf("SlowControlDB tree not found\n");
      return false;
    }
    
    _tree->SetBranchAddress("begin",&begin);
    _tree->SetBranchAddress("end",&end);

    bool add=false;
    for(int i=0;i<_tree->GetEntries();i++){
      _tree->GetEntry(i);
      if(debug)printf("entry %i begin %i end %i - add? %i\n",i,begin,end,(begin<maxT&&end>minT));
      if(begin>maxT||end<minT)continue;
      add=true;
    }

    if(add)Add(f->GetName());

    if(debug)printf("files merged to SlowControlDB %i\n",(int)GetEntries());
    return GetEntries();
  }

  /// returns the pointer to the Object in the DB with name
  SlowControlEl* GetElem(char* name){
    SlowControlEl *toReturn=new SlowControlEl(name);
    
    SlowControlEl *scel=0;
    SetBranchAddress(name,&scel);
    for(int i=0;i<GetEntries();i++){
      GetEntry(i);
      for(int n=0;n<(int)scel->getnelem();n++)
	toReturn->Add(scel->getElemTime(n),scel->getElem(n));
    }
    
    return toReturn;
  }
  
  Node* GetNode(char* name){
    Node *toReturn=new Node(name,name);
    Node *node=0;
    SetBranchAddress(name,&node);
    for(int i=0;i<GetEntries();i++){
      GetEntry(i);
      if(node)
	toReturn->AddNode(node);
    }
    return toReturn;
  }

  /// Returns the value of a quantity with a given name at a given timestamp
	/*! 
	 \param name       the name of the desired quantity
	 \param timestamp  Unix time (sec from 1970)
	 \param frac       second fraction after the timestamp  
	 \param flag   0 = the closer in time
	 1 = linear interplolation
	 2 = polynomial interpolation (to be implemented)
	 \return  The desired value or On Error  -99999
	 */
  //  float GetData(char* name,unsigned int timestamp,float frac,int flag){
  //    SlowControlEl* el=GetEl(name);
  //    return el->Find(timestamp,frac,flag);
  //  }
  
  /// Print infos about all the members of the DB
  void Print(const Option_t* aa=0){
    SlowControlEl *scel=0;
    SetBranchAddress(aa,&scel);
    
    for(int i=0;i<GetEntries();i++){
      GetEntry(i);
      printf("Name %s - data entries %i\n",scel->GetName(),scel->getnelem());
    }
    

    return;
  }

  ClassDef(SlowControlDB,1);
};

extern SlowControlDB* head;
