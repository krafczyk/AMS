// Author V. Choutko 24-may-1996
 
#include <cont.h>
#include <amsstl.h>
void AMSContainer::addnext(AMSlink *ptr){
  if(_Last){
             _Last->_next=ptr;
             _Last=_Last->_next;
  }
  else _Head=_Last=ptr;
  _nelem++;
  _Last->setpos(_nelem);
}
void AMSContainer::sort(){
if(_sorted || (_nelem < 2) )return;
_sorted=1;
AMSlink ** arr=(AMSlink **)UPool.insert(sizeof(AMSlink*)*_nelem);
int i;
arr[0]=_Head;
for(i=1;i<_nelem;i++){
 arr[i]=arr[i-1]->_next;
}
AMSsortNAG(arr,_nelem);
_Head=arr[0];
_Head->setpos(1);
_Last=arr[_nelem-1];
_Last->_next=0;
for(i=1;i<_nelem;i++){
 arr[i-1]->_next=arr[i];
 arr[i]->setpos(i+1);
}
UPool.udelete(arr);
}


