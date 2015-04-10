//Author P. Zuccon -- MIT
//
#ifndef hashtable_h
#define hashtable_h
#include <map>

template <typename T> class hashtable{
  
 public:
  typedef  unsigned long ulong;
  typedef typename std::map<ulong,T>::pointer         pointer;
  typedef typename std::map<ulong,T>::const_pointer   const_pointer;
  typedef typename std::map<ulong,T>::reference       reference;
  typedef typename std::map<ulong,T>::const_reference const_reference;
  typedef typename std::map<ulong,T>::iterator               iterator;
  typedef typename std::map<ulong,T>::const_iterator         const_iterator;
  typedef typename std::map<ulong,T>::size_type              size_type;
  typedef typename std::map<ulong,T>::difference_type        difference_type;
  typedef typename std::map<ulong,T>::reverse_iterator       reverse_iterator;
  typedef typename std::map<ulong,T>::const_reverse_iterator const_reverse_iterator;

 private:
  std::map<ulong, T> hmap;
  ulong hmaphash(const char *str)
  {
    ulong hash = 5381;
    int c;
    
    while ((c = *str++))
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
  }

  
 public:
  hashtable(){}
  virtual ~hashtable(){hmap.clear();}
  iterator begin () {return hmap.begin();}
  const_iterator begin () const {return hmap.begin();}
  iterator end (){return hmap.end();}
  const_iterator end () const {return hmap.end();}
  reverse_iterator rbegin() {return hmap.rbegin();}
  const_reverse_iterator rbegin() const {return hmap.rbegin();}
  reverse_iterator rend() {return hmap.rend();}
  const_reverse_iterator rend() const {return hmap.rend();}
  bool empty ( ) const {return hmap.size()==0;}
  size_type size() const {return hmap.size();}
  size_type max_size() const {return hmap.max_size();}

  T& operator[] ( const char * str ){
    ulong ll=hmaphash(str);
    return hmap[ll];
  }
  void Add ( const char * str,const T& aa ){
    ulong ll=hmaphash(str);
    hmap[ll]=aa;
    return;
  }

  size_type erase ( const char* str ){
    return hmap.erase( hmaphash(str));
  }
  void clear ( ){hmap.clear();}

  int find ( const char* str, const_iterator& aa){
    ulong ll=hmaphash(str);
    aa=hmap.find(ll);
    if(aa==hmap.end()) return 0;
    return 1;
  }

};

#endif
