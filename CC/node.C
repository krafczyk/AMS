//  $Id: node.C,v 1.8 2008/12/08 15:15:17 choutko Exp $
// Author V. Choutko 24-may-1996
 
#include "typedefs.h"
#include "node.h"
#ifdef _OPENMP
#include <omp.h>
#endif
AMSNode * AMSNode::add(  AMSNode *ptr, bool init){
  //
  // Add new node
  //
  AMSNode *cur;
  cur=_down;
  if(cur){
    while(cur->_next)cur=cur->_next;
    cur->_next=ptr;
    ptr->_prev=cur;
  }
  else{
    _down=ptr;
    ptr->_prev=0;
  }
  ptr->_up=this;
  if(init)ptr->_init();
  return ptr;
} 
AMSNode * AMSNode::addup(  AMSNode *ptr){
  //
  // Add new node on virtual top
  //
  AMSNode *cur;
  cur=_down;
  if(cur){
    while(cur->_next)cur=cur->_next;
    cur->_next=ptr;
    ptr->_prev=cur;
  }
  else{
    _down=ptr;
    ptr->_prev=0;
  }
  ptr->_up=0;
  //   ptr->_init();
  return ptr;
} 
void  AMSNode::remove(){
  //
  // Remove itself + all daughters
  // if not at the top
  //
  while (_down){
    _down->remove();
  } 
  if(_down==0 && _up){ 
    if(_prev){
      _prev->_next=_next;
      if(_next)_next->_prev=_prev;
    }
    else {
      _up->_down=_next;
      if(_next)_next->_prev=0;
    }
    delete this;
  }
}

/*
void  AMSNode::remove(){
  //
  // Remove itself + all daughters
  // if not at the top
  //
bool bproc=true;
AMSNode *cur=this;
while (cur && cur->down()!=this){
 if(bproc && cur->down()){
  cur=cur->down();  
 }
 else if(cur->next()){
  bproc=true;
  cur=cur->next();  
 }
 else{
  bproc=false; 
   AMSNode *d=cur;
   cur=cur->up();
  if(d->_down==0 && d->_up){ 
    if(d->_prev){
      d->_prev->_next=d->_next;
      if(d->_next)d->_next->_prev=d->_prev;
    }
    else {
      d->_up->_down=_next;
      if(d->_next)d->_next->_prev=0;
    }
    delete d;
  }
 }
}



}

*/

void AMSNode::printN(ostream & stream, int i){
  stream<<*this;
  if(down())down()->printN(stream,1);
  if(i && next())next()->printN(stream,1);
}


void AMSNode::setMessage(const char * message){
  if(message){
     if(_message)delete[] _message;
    _message=new char[strlen(message)+1];
    strcpy(_message,message);
  }
}
