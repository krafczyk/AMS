// Author V. Choutko 24-may-1996
 
#ifndef __AMSCONTAINER__
#define __AMSCONTAINER__
#include <typedefs.h>
#include <link.h>
#include <node.h>
#include <upool.h>
class AMSContainer: public AMSNode{
private:
AMSlink *_Head;
AMSlink *_Last;
integer _nelem;   // number of elements in containers
integer _sorted;   // For Nevsky 
void _init(){};
void _printEl(ostream & stream){stream <<_name<<" "<<_id<<" Elements: "<<_nelem<<endl;}
public:
void * operator new(size_t t, void *p) {return p;}
void * operator new(size_t t) {return UPool.insert(t);}
void operator delete(void *p)
  {if(p){((AMSContainer*)p)->~AMSContainer();p=0;UPool.udelete(p);}}

AMSContainer(AMSID id,integer nelem=0):
AMSNode(id),_nelem(nelem),_Last(0),_Head(0),_sorted(0){}
void eraseC(){if(_Head)_Head->_erase();}
void printC(ostream & stream){_printEl(stream);if(_Head)_Head->_print(stream);}
void writeC(){if(_Head)_Head->_write();}
void copyC(){if(_Head)_Head->_copy();}
void addnext(AMSlink* ptr);
AMSlink* gethead()const {return _Head;}
void sethead(AMSlink *head){ 
 _Head=head;
 _Last=head;
 _nelem=1;
 while(_Last->next()){
  _nelem++;
  _Last=_Last->next();
 }
}

integer getnelem() const{return _nelem;}
void sort();
integer& setnelem() {return _nelem;}
~AMSContainer(){eraseC();}
};
#endif
