#ifndef __SCDB__
#define __SCDB__
#include <map>
#include "TNamed.h"
#include "TFile.h"
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

    for(int i=0;i<dt->getnelem();i++)
      it->second.Add(dt->GetSubTypeN(i));

    return &it->second;
  }

  void AddNode(Node* node){
    for(int i=0;i<(int)node->datatypes.size();i++){
      DataType* dt=Append(node->GetDataTypeN(i));
      for(int j=0;j<(int)node->GetDataTypeN(i)->getnelem();j++){
	SubType *st=dt->Append(node->GetDataTypeN(i)->GetSubTypeN(j));
	for(int k=0;k<node->GetDataTypeN(i)->GetSubTypeN(j)->getnelem();k++)
	  st->Add(node->GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k),node->GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k));
      }
    }
  }

  int getnelem(){return (int)datatypes.size();}
  ClassDef(Node,1);
};


class SlowControlDB: public TTree
{  
 private:
  static SlowControlDB* head;
  unsigned int begin;
  unsigned int end;
 public:
  SlowControlDB():TTree("SlowControlDB","SlowControlDB"){SetDirectory(0);};
  
  /// Get the pointer to the DB
  static SlowControlDB* GetPointer(){
    if(!head)head=new SlowControlDB();
    return head;
  }

  /// Get the pointer to the DB
  static SlowControlDB* KillPointer(){
    delete head;
    head=0;  //VC
    head=new SlowControlDB();
    return head;
  }

  /// destructor
  virtual ~SlowControlDB(){}

  /// Load the DB from a root file with name
  bool Load(char* fname,unsigned int minT=0,unsigned int maxT=UINT_MAX, int debug=0);

  /// Load the file from an already opened ROOT File 
  bool Load(TFile* f,unsigned int minT=0,unsigned int maxT=UINT_MAX,int debug=0);
  
  Node GetNode(char* name){
    Node toReturn(name,name);
    Node *node=0;
    SetBranchAddress(name,&node);
    for(int i=0;i<GetEntries();i++){
      GetEntry(i);
      if(node)
	toReturn.AddNode(node);
    }
    delete node;
    return toReturn;
  }

  /// Returns the value of a quantity with a given name at a given timestamp
	/*! 
	 \param name       the name of the desired quantity
	 \param timestamp  Unix time (sec from 1970)
	 \param frac       second fraction after the timestamp
         \param val        return value  
	 \param flag   0 = the closer in time
	 1 = linear interplolation
	 2 = polynomial interpolation (to be implemented)
	 \return  0   success
                  1  no name found
                  2  outside of bounds
  
	 */
  int GetData(char* nname,int dt, int st, unsigned int timestamp,float frac,float &val,int flag=1){
    
    SubType *subtype=GetNode(nname).GetDataType(dt)->GetSubType(st);
    if(!st) return 1;

    val=subtype->Find(timestamp,frac,flag);

    if(val==-99999.)return 2;
    else return 0;
  }

  /// Print infos about all the members of the DB
  void Print(const Option_t* aa=0, int debug=0){
    if(debug)printf("looking for %s in %i entries\n",aa,(int)GetEntries());
    Node *node=0;
    SetBranchAddress(aa,&node);

    for(int i=0;i<(int)GetEntries();i++){
      GetEntry(i);
      printf("Name %s - data entries %i\n",node->GetName(),node->getnelem());
    }
    return;
  }
  ClassDef(SlowControlDB,2);
};

#endif
