// Author V. Choutko 24-may-1996
 
#ifndef __AMSCONTAINER__
#define __AMSCONTAINER__
#include <typedefs.h>
#include <link.h>
#include <node.h>
#include <upool.h>

typedef integer  (*pBuilder)(integer refit=0);
class AMSContainer: public AMSNode{
private:
pBuilder _pb;
integer _BuildSuccess;
AMSlink *_Head;
AMSlink *_Last;
integer _nelem;   // number of elements in containers
integer _sorted;   // For Nevsky 
void _init(){};
void _printEl(ostream & stream){stream <<_name<<" "<<_id<<" Elements: "<<_nelem<<endl;}
public:
integer buildOK(){ return _BuildSuccess ;}
void setbuilder(pBuilder pb){_pb=pb;}
 void runbuilder(integer par);
void * operator new(size_t t, void *p) {return p;}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
  {UPool.udelete(p);p=0;}

AMSContainer(AMSID id,integer nelem=0):
AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(0),
_Last(0),_Head(0),_sorted(0){}
AMSContainer(AMSID id,pBuilder pb,integer nelem=0):
AMSNode(id),_nelem(nelem),_BuildSuccess(0),_pb(pb),
_Last(0),_Head(0),_sorted(0){}
void eraseC(){if(_Head)_Head->_erase();delete _Head;_BuildSuccess=0;_Head=_Last=0;_nelem=0;}
void printC(ostream & stream){_printEl(stream);if(_Head)_Head->_print(stream);}
void writeC(){if(_Head)_Head->_write();}
void copyC(){if(_Head)_Head->_copy();}
void addnext(AMSlink* ptr);
void replaceEl(AMSlink* ptr, AMSlink* prev);
void removeEl(AMSlink* prev, integer restore=1);
void exchangeEl(AMSlink* pel1, AMSlink *pel2);
AMSlink* gethead()const {return _Head;}
AMSlink* getlast()const {return _Last;}
void sethead(AMSlink *head){ 
 _Head=head;
 _Last=head;
 if(_Head){
  _nelem=1;
  while(_Last->next()){
   _nelem++;
   _Last=_Last->next();
  }
 }
 else _nelem=0;
}

integer getnelem() const{return _nelem;}
void sort();
integer& setnelem() {return _nelem;}
~AMSContainer(){eraseC();}
};
#endif
