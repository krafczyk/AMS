// Author V. Choutko 24-may-1996
 
#include <link.h>
void AMSlink::_erase(){
  //if(_next){
  //_next->_erase();
  //_next=0;
  //}
  //if(!_next) delete this;
  AMSlink * next=_next;
  AMSlink * prev;
  do {
   if(next){
      prev=next;
      next=next->_next;
      delete prev;
   }
  }while(next);
  _next=0;
}
void AMSlink::_print(ostream & stream){
   _printEl(stream);
   if(_next)_next->_print(stream);
}
void AMSlink::_write(){
   _writeEl();
   if(_next)_next->_write();
}
void AMSlink::_copy(){
   _copyEl();
   if(_next)_next->_copy();
}

integer AMSlink::testlast( ){
 if ( _next==0 || (*this < *_next) || (*_next < *this))return 1;
 else return 0;
}

integer AMSlink::operator < ( AMSlink & o) const{
  return 1;     // Dummy operator
}
