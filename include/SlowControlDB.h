#ifndef __SCDB__
#define __SCDB__
#include <map>
#include "TNamed.h"
#include "TFile.h"
#include "TTree.h"
#include <stdlib.h>
#include <climits>
#include <cstring>
#include <iostream>

class SubType{
 public:
  std::map<unsigned int,float> _table;
  TString tag;
  SubType(){};

  SubType(int i):number(i){};
  int number;
  
  /// destructor
  virtual ~SubType(){};

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
    // is no entries return error -1
    if(_table.size()==0)return -1;

    // try to find lower bound for given time
    std::map<unsigned int,float>::iterator it=_table.lower_bound(timestamp);
    
    // if no lower bound found (timestamp < lowest available time) return error 1
    if(it==_table.end()) return 1;

    // get time and value of lower bound entry
    unsigned int tmin=it->first;
    float min=it->second;

    // increment iterator to upper bound 
    it++;

    // get time and value of upper bound entry
    unsigned int tmax=it->first;
    float max=it->second;
    
    // get dt of timestamp to lower/upper bound
    float dtmin=(timestamp-tmin)+frac;
    float dtmax=(tmax-timestamp)-frac;

    if(flag==2){
      std::cerr<<"SubType::Find Polynomial fit not yet implemented, fall back to linear"<<std::endl;
      flag=1;
    }

    // return lower/upper value based on smaller dt to timestamp
    if(flag==0){
      if( dtmin < dtmax)val=min;
      else val=max;
      return 0;
    }

    // return linear interpolation between lower/upper bound
    if(flag==1){
      val=(dtmin*min+dtmax*max)/(dtmin+dtmax);
      return 0;
    }

    // return error - fit method not found
    std::cerr<<"SubType::Find illegal value of flag "<<flag<<std::endl;
    return 2;      
  }
  
  unsigned int GetTime(int order){
    if(order>=(int)_table.size()) std::cerr<<"Warning! SubType::GetTime requested entry "<<order<<" larger than map size "<<_table.size()<<std::endl;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->first;
  }

  float GetValue(int order){
    if(order>=(int)_table.size()) std::cerr<<"Warning! SubType::GetValue requested entry "<<order<<" larger than map size "<<_table.size()<<std::endl;
    std::map<unsigned int,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->second;
  }

  int getnelem(){return (int)_table.size();}

  ClassDef(SubType,1);
  friend class AMSSetupR;
};

class DataType{
 public:
  std::map<int,SubType> subtypes;
  DataType(){};
 DataType(int i):number(i){};
  int number;
  
  /// destructor
  virtual ~DataType(){};

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
  
  bool Add(SubType *st,unsigned int minT=0,unsigned int maxT=UINT_MAX){
    std::map<int,SubType>::iterator it;
    it=subtypes.find(st->number);
    if(it!=subtypes.end()){
      //      printf("SubType %i already found\n",st->number);
      return false;
    }

    if(minT>0||maxT<UINT_MAX){
      std::map<unsigned int,float>::iterator begin_it=st->_table.begin();
      std::map<unsigned int,float>::iterator end_it=st->_table.end();
      
      if(minT>0&&st->_table.lower_bound(minT)!=st->_table.end())begin_it=st->_table.lower_bound(minT); 
      if(maxT<UINT_MAX&&st->_table.upper_bound(maxT)!=st->_table.end())end_it=st->_table.upper_bound(maxT);
      
      st->_table.erase(end_it,st->_table.end());
      st->_table.erase(st->_table.begin(),begin_it);
    }

    subtypes.insert(std::pair<int,SubType>(st->number,*st));
    return true;
  }
  
  SubType* Append(SubType *st, unsigned int minT=0, unsigned int maxT=UINT_MAX){
    std::map<int,SubType>::iterator it;
    it=subtypes.find(st->number);
    if(it==subtypes.end()){
      subtypes.insert(std::pair<int,SubType>(st->number,*st));
      it=subtypes.find(st->number);
    }
    it->second._table.clear();
    
    std::map<unsigned int,float>::iterator begin_it=st->_table.begin();
    if(minT>0&&st->_table.lower_bound(minT)!=st->_table.end()){
      begin_it=st->_table.lower_bound(minT);
      if(begin_it!=st->_table.begin())begin_it--;
    }
    
    std::map<unsigned int,float>::iterator end_it=st->_table.end();
    if(maxT!=UINT_MAX&&st->_table.upper_bound(maxT)!=st->_table.end()){
      end_it=st->_table.upper_bound(maxT);
      if(end_it!=st->_table.end())end_it++;
    }
    
    for(std::map<unsigned int,float>::iterator iter=begin_it;iter!=end_it;iter++)
      it->second.Add(iter->first,iter->second);

    return &it->second;
  }

  int getnelem(){return (int)subtypes.size();}
  
  ClassDef(DataType,1);
  friend class AMSSetupR;
};

class Node:public TNamed{
 public:
  std::map<int,DataType> datatypes;
 Node():TNamed("default","default"){};
 Node(char *name):TNamed(name,name){};
 Node(char *name1,char *name2):TNamed(name1,name2){};
  int number;

  //destructor
  virtual ~Node(){};

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

  bool Add(DataType *dt,unsigned int minT=0,unsigned int maxT=UINT_MAX){
    std::map<int,DataType>::iterator it;
    it=datatypes.find(dt->number);
    if(it!=datatypes.end()){
      return false;
    }
    datatypes.insert(std::pair<int,DataType>(dt->number,*dt));

    for(int i=0;i<dt->getnelem();i++)
      dt->Add(dt->GetSubTypeN(i),minT,maxT);
    
    return true;
  }

  DataType* Append(DataType *dt,unsigned int minT=0,unsigned int maxT=UINT_MAX){
    std::map<int,DataType>::iterator it;
    it=datatypes.find(dt->number);
    if(it==datatypes.end()){
      datatypes.insert(std::pair<int,DataType>(dt->number,*dt));
      it=datatypes.find(dt->number);
    }

    for(int i=0;i<dt->getnelem();i++)
      it->second.Append(dt->GetSubTypeN(i),minT,maxT);

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
 public:
  unsigned int begin;
  unsigned int end;
  unsigned int uncompleted;
  std::map<std::string,Node> nodemap;
  std::map<std::string,unsigned int> searchmap;

  SlowControlDB(){
    nodemap.clear();
    searchmap.clear();
  };//:TTree("SlowControlDB","SlowControlDB"){SetDirectory(0);};

  int GetEntries(){return (int)nodemap.size();}

  /// Get the pointer to the DB
  static SlowControlDB* GetPointer(){
    if(!head)head=new SlowControlDB();
    return head;
  }

  /// Get the pointer to the DB
  static void  KillPointer(){
    delete head;
    head=0;  //VC
  }

  /// destructor
  virtual ~SlowControlDB(){}

  /// Load the DB from a root file with name
  bool Load(const char* fname,unsigned int minT=0,unsigned int maxT=UINT_MAX, int debug=0);

  /// Load the file from an already opened ROOT File 
  bool Load(TFile* f,unsigned int minT=0,unsigned int maxT=UINT_MAX,int debug=0);

  bool SaveToFile(const char* fname,int debug=0);
  
  int AppendNode(Node* copynode,unsigned int minT=0,unsigned int maxT=UINT_MAX);
  
  bool BuildSearchIndex(int debug=0);

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
	  std::cout <<it->second.GetDataTypeN(i)->GetSubTypeN(j)->getnelem()<<" values for entry "<<j<<" (dt "<<it->second.GetDataTypeN(i)->number<<") "<<it->second.GetDataTypeN(i)->GetSubType(j)->tag<<std::endl;
	  if(abs(level)<4)continue;
	  for(int k=0;k<it->second.GetDataTypeN(i)->GetSubTypeN(j)->getnelem();k++){
	    std::cout<<"time "<<it->second.GetDataTypeN(i)->GetSubTypeN(j)->GetTime(k)<<" val "<<it->second.GetDataTypeN(i)->GetSubTypeN(j)->GetValue(k)<<std::endl;
	  }
	}
      }
      return;
    }
  }


  ClassDef(SlowControlDB,3);
  friend class AMSSetupR;
};

#endif
