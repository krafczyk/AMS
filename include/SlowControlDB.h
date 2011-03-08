#ifndef __SCDB__
#define __SCDB__
#include <map>
#include "TNamed.h"
#include "TFile.h"
#include "TTree.h"
#include <climits>
#include <cstring>
#include <iostream>

class SubType{
  std::map<unsigned int,float> _table;
 public:
  std::string tag;
  SubType(){};

  SubType(int i):number(i){};
  int number;
  
  int Add(unsigned int time, float val){
    std::map<unsigned int,float>::iterator it=_table.find(time);
    
    // appending data
    if(it==_table.end()){
      _table.insert(std::pair<unsigned int,float>(time,val));
      return 0;
    }
    // replacing data
    else{
      it->second=val;
      return 2;
    }
    
    return 1;
  }
  
  //  float Find(unsigned int timestamp,float frac, int flag){
  int Find(unsigned int timestamp, float &val, float frac, int flag){
    std::map<unsigned int,float>::iterator it=_table.lower_bound(timestamp);
    if(it==_table.end()) return 1;
    unsigned int tmin=it->first;
    float min=it->second;
    it++;
    unsigned int tmax=it->first;
    float max=it->second;
    float dtmin=(timestamp-tmin)+frac;
    float dtmax=(tmax-timestamp)-frac;
    if(flag==2){
      std::cerr<<"SubType::Find Polynomial fit not yet implemented, fall back to linear"<<std::endl;
      flag=1;
    }
    else if(flag==0){
      if( dtmin < dtmax)val=min;//  return min;
      else val=max;
      return 0;
    }else if(flag==1){
      val=(dtmin*min+dtmax*max)/(dtmin+dtmax);
      return 0;
    }else{
      std::cerr<<"SubType::Find illegal value of flag "<<flag<<std::endl;
      return 2;      
    }
    return 3;      
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
  friend class AMSSetupR;
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
  friend class AMSSetupR;
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

  int getnelem(){return (int)datatypes.size();}
  ClassDef(Node,1);
  friend class AMSSetupR;
};


class SlowControlDB//: public TTree
{  
 private:
  static SlowControlDB* head;
  unsigned int begin;
  unsigned int end;
  std::map<std::string,Node> nodemap;
  
 public:
  SlowControlDB(){};//:TTree("SlowControlDB","SlowControlDB"){SetDirectory(0);};

  int GetEntries(){return (int)nodemap.size();}

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
  bool Load(const char* fname,unsigned int minT=0,unsigned int maxT=UINT_MAX, int debug=0);

  /// Load the file from an already opened ROOT File 
  bool Load(TFile* f,unsigned int minT=0,unsigned int maxT=UINT_MAX,int debug=0);
  
  int AppendNode(Node* copynode);
  
  Node GetNode(char* name){
    std::map<std::string,Node>::iterator it=nodemap.find(std::string(name));
    if(it==nodemap.end())
      std::cerr<<"node "<<name<<" not found"<<std::endl;

    return it->second;
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
    if(!st){
      std::cerr<<"SlowControlDB::GetData Node"<<nname<<" DT "<<dt<<" ST "<<st<<" not found"<<std::endl;
      return 1;
    }

    return subtype->Find(timestamp,val,frac,flag);
  }

  /// Print infos about all the members of the DB
  void Print(const Option_t* aa=0, int level=0){
    if(level<0)std::cout<<"looking for "<<aa<<std::endl;
    
    for(std::map<std::string,Node>::iterator it=nodemap.begin();it!=nodemap.end();it++){
      std::cout<<"Name "<<it->second.GetName()<<" - data types "<<it->second.getnelem()<<std::endl;
      
      if(abs(level)<2)continue;
      for(int i=0;i<(int)it->second.getnelem();i++){
	std::cout<<"node "<<i<<" " <<it->second.GetName()<< " dt "<<it->second.GetDataTypeN(i)->number<<" entries "<<it->second.GetDataTypeN(i)->getnelem()<<std::endl;
	if(abs(level)<3)continue;
	for(int j=0;j<it->second.GetDataTypeN(i)->getnelem();j++){
	  std::cout <<it->second.GetDataTypeN(i)->GetSubTypeN(j)->getnelem()<<" values for entry "<<j<<" (dt "<<it->second.GetDataTypeN(i)->number<<") "<<it->second.GetDataTypeN(i)->GetSubType(j)->tag.c_str()<<std::endl;
	  if(abs(level)<4)continue;
	  for(int k=0;k<it->second.GetDataTypeN(i)->GetSubTypeN(j)->getnelem();k++){
	    std::cout<<"time "<<it->second.GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k)<<" val "<<it->second.GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k)<<std::endl;
	  }
	}
      }
      return;
    }
  }

  ClassDef(SlowControlDB,2);
  friend class AMSSetupR;
};

#endif
