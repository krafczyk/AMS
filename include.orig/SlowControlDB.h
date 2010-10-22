#include <map>
#include "TNamed.h"
#include "TFile.h"

/// Class holding the pair timestamp,value for a given quantity
class SlowControlEl: public TNamed{
 private:
  std::map<time_t,float> _table;
 public:
 
  ///Default Constructor
  SlowControlEl():TNamed("default","default"){};  

  ///Constructor
  SlowControlEl(char *name):TNamed(name,name){};

  ///Destructor
  virtual ~SlowControlEl(){}

  /// Clear all the data
  void Clear(const Option_t* aa=0){_table.clear();}

  /// Find the value at a given time
  /// \parameter timestamp  Unix time (sec from 1970)
  /// \parameter frac       second fraction after the timestamp  
  /// \parameter flag   0 = the closer in time
  ///                  1 = linear interplolation
  ///                  2 = polynomial interpolation (to be implemented)
  /// On Error returns -99999
  float Find(time_t timestamp,float frac, int flag){
    std::map<time_t,float>::iterator it=_table.lower_bound(timestamp);
    if(it==_table.end()) return -99999.;
    time_t tmin=it->first;
    float min=it->second;
    it++;
    time_t tmax=it->first;
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
  bool  Add(time_t timestamp, float value){_table[timestamp]=value; return true;}
  
  /// Returns the Number of data available
  int   getnelem(){return _table.size();}
  
  /// returns the  datum at postion order in the series
  float getElem(int order){
    if(order>=_table.size()) return -999999;
    std::map<time_t,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->second;
  }

  /// returns the timestamp of the datum at postion order in the series
  time_t getElemTime(int order){
    if(order>=_table.size()) return -999999;
    std::map<time_t,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    return it->first;
  }

  ///Remove the element at position order
  bool rmElem(int order){
    if(order>=_table.size()) return false;
    std::map<time_t,float>::iterator it=_table.begin();
    for(int ii=0;ii<order;ii++) it++;
    _table.erase(it);
    return true;
  }

  /// Remove the element with a givent timestamp
  bool rmElem(time_t timestamp){
    std::map<time_t,float>::iterator it=_table.find(timestamp);
    if(it!=_table.end()){
      _table.erase(it);
      return true;
    }    
    else
      return false;
  }
 
  /// Returns the begin timestamp of the serie
  time_t GetBeginTime(){return _table.begin()->first;}

  /// Returns the end timestamp of the serie
  time_t GetEndTime(){return _table.end()->first;}
  /// Print info about the quantity
  void Print(const Option_t* aa=0){
    TNamed::Print(aa);
    time_t bg=GetBeginTime();
    time_t en=GetEndTime();
    printf("Name: %s Num Meas: %d\nVal Start: %d %s  Val Stop %d %s\n",GetName(),getnelem(),(int)bg,ctime(&bg),(int)en,ctime(&en));
    std::map<time_t,float>::iterator it=_table.begin();
    for(;it!=_table.end();it++){
      time_t tt=it->first;
      printf(" Val: %f %s",it->second,ctime(&tt));
    }
    return;
  }

  ClassDef(SlowControlEl,1);
};

typedef  std::map<int,SlowControlEl>::iterator  tabit;

/// Class holding a series of slow control data
/// Class is a singleton and pointer to it can be obtained by the static funtion SlowControlDB::GetPointer()
/// Data are retrieved by name and timestamp value
/// The class can be written to a ROOT file by the inherited method Write() and can be loaded by its own methods Load(...)
class SlowControlDB: public TObject{
  
 private:
  std::map<int,SlowControlEl> _table;
  static SlowControlDB* head;
  SlowControlDB():TObject(){};
  unsigned long hash(const char *str)
    {
        unsigned long hash = 5381;
        int c;
        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        return hash;
    }

 public:
  /// Get the pointer to the DB
  static SlowControlDB* GetPointer(){
	return (head)? head :new SlowControlDB(); 
  }


  /// destructor
  virtual ~SlowControlDB(){}

  /// Clear all the contents
  void Clear(const Option_t* aa=0){ _table.clear();}

  /// Load the DB from a root file with name
  bool Load(char* fname){
    TFile *f=TFile::Open(fname);
    return Load(f);
  }

  /// Load the file from an already opened ROOT File 
  bool Load(TFile* f){
    if(!f) return false;
    if(head) delete head;
    head=(SlowControlDB*)f->Get("SlowControlDB");
    return head!=0;
  }

  /// Returns the number of Object (slow control data) in the DB
  int getnelem(){return _table.size();}

  /// Add a new Element to the DB (by copy) you remain the owner of the object pointed by el
  bool AddEl(SlowControlEl* el){
    int indx=hash(el->GetName());
    tabit it=_table.find(indx);
    if(it!= _table.end()) return false;
    _table[indx]=*el;
    return true;
  }

  /// returns the pointer to the Object in the DB with name
  SlowControlEl* GetEl(char* name){
    int indx=hash(name);
    tabit it=_table.find(indx);
    if(it== _table.end()) return 0;
    else
      return &(it->second);
  }
  /// Remove one object from the DB

  bool RemoveEl(char* name){
    int indx=hash(name);
    tabit it=_table.find(indx);
    if(it== _table.end()) return false;
    _table.erase(it);
    return true;
  }

  /// Returns the value of a quantity with a given name at a given timestamp
  /// \parameter name       the name of the desired quantity
  /// \parameter timestamp  Unix time (sec from 1970)
  /// \parameter frac       second fraction after the timestamp  
  /// \parameter flag   0 = the closer in time
  ///                  1 = linear interplolation
  ///                  2 = polynomial interpolation (to be implemented)
  /// On Error returns -99999
  float GetData(char* name,time_t timestamp,float frac,int flag){
    SlowControlEl* el=GetEl(name);
    return el->Find(timestamp,frac,flag);
  }

  /// Print infos about all the members of the DB
  void Print(const Option_t* aa=0){
    TObject::Print(aa);
    for(tabit it=_table.begin();it!=_table.end();it++) it->second.Print();
    return;
  }

  /// the hash map ( for experts only)
  std::map<int,SlowControlEl>* GetMap(){return  &_table;}
		
  ClassDef(SlowControlDB,1);
};
