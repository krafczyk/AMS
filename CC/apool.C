// Author V. Choutko 19-jul-1996
 
#include <apool.h>
#include <amsstl.h>
#include <amsgobj.h>
    AMSaPool APool(512000);
#ifndef __UPOOL__
    AMSaPool UPool(512000);
#endif
integer AMSaPool::_Mask=0xf0f0f0f0;
char * AMSNodeMapPool::get( integer o){
    integer i=0;
   if(_hash && _numo)i=AMSbia((integer**)_hash,o,_numo);
    if(i>0 && i<_numo){
      char *tmp=(char*)_hash[i];
      //UCOPY2(_hash+i+1,_hash+i,sizeof(_hash[0])*(--_numo-i)/4);
      for(int j=i;j<--_numo;j++)_hash[j]=_hash[j+1];
      return tmp;
    }
    else return 0;
}
void AMSNodeMapPool::put( integer * p ){
  //    AMSgObj::BookTimer.start("put");
    integer i=0;
    //    AMSgObj::BookTimer.start("putAMSBia");
    if(_hash && _numo)i=AMSbia((integer**)_hash,*p,_numo);
    //    AMSgObj::BookTimer.stop("putAMSBia");
      if(_hsize<=_numo){
       int size=++_numo;
       AMSNode ** tmp;
       do {
        _hsize=_numo+size;
        //    AMSgObj::BookTimer.start("putnew");
         tmp=new AMSNode*[_hsize];
         //    AMSgObj::BookTimer.stop("putnew");
         size=(size/2>1)?size/2:1;
       }while (!tmp && size==1);
       if(!tmp){
         if(AMSNodeMap::debug)cerr <<"AMSNodeMap::add-F-Can not allocate " <<size
        <<" words"<<endl;
        throw AMSNodeMapError("AMSNodeMap::add-F-CNNTALLMEM");
       }
       //UCOPY(_hash,tmp,sizeof(_hash[0])*i/4);
        int j;
        for ( j=0;j<i;j++)tmp[j]=_hash[j];
        tmp[i]= (AMSNode*)p;
        //        UCOPY(_hash+i,tmp+i+1,sizeof(_hash[0])*(_numo-i)/4);
                for (j=i+1;j<_numo;j++)tmp[j]=_hash[j-1];
        delete[] _hash;
        _hash=tmp;

      }
      else {
        //        UCOPY2(_hash+i,_hash+i+1,sizeof(_hash[0])*(_numo++-i)/4);
                        for (int j=_numo++;j>i;j--)_hash[j]=_hash[j-1];
        _hash[i]= (AMSNode*)p;
      }

      //    AMSgObj::BookTimer.stop("put");
}
void * AMSaPool::insert(size_t st){
    union ALIGN{void*p; long l; long double d;};
     // Align request
      st=st+sizeof(ALIGN)-1;
      st=st-st%sizeof(ALIGN)+sizeof(ALIGN);
      char * ptr=poolMap.get(st);
#ifdef __AMSDEBUG__
        _Count++;
#endif
      if(ptr) return ptr+sizeof(ALIGN);
      else{
        if(_free==0 || _lc+st > _head->_length  ){
        AMSaPool::_grow(st); 
        }
         if(_free){
        char * p=(char*)_free;
        *((integer*)p)=st;
        *((integer*)p+1)=_Mask; 
        _free=(void*)(p+st);
        _lc=_lc+st;
        //cout<< "insert "<<_lc<<" "<<st <<endl;
        return p+sizeof(ALIGN); }
        else{
        _Count--; 
        cerr <<" AMSaPool-F-Memory exhausted: Was "<<(_Nblocks-1)*_size<<
        " Requested "<<st<<" bytes"<<endl;
         throw AMSaPoolError("AMSaPool-F-Memory exhausted");
         return 0;
        }
      }
}
 void AMSaPool::_grow(size_t st){
  _Nblocks++;
#ifdef __AMSDEBUG__
#endif 
  if(_head==0) {
     _Nreq++;
     _head =new dlink();
     _head->_length=_size > st ? st*(_size/st): st; 
     _head->_address = new char[_head->_length];
     _free=(void*)(_head->_address);
     _lc=0;
   //cout <<" grow 0 "<<_head->_length<<endl;
  }
  else if(_head->_next==0){
     _Nreq++;
   _head->_next=new dlink;
   (_head->_next)->_prev=_head;
   _head=_head->_next;   
     _head->_length=_size > st ? st*(_size/st): st;
     _head->_address = new char[_head->_length];
     _free=(void*)(_head->_address);
     _lc=0;
   //cout <<" grow 1 "<<_head->_length<<endl;
  }
  else{
     _head=_head->_next;
     //cout <<" grow 2 "<<_head->_length<<endl;
     _free=(void*)(_head->_address);
     _lc=0; 
  }
 }
   void AMSaPool::erase(integer tol){
#ifdef __AMSDEBUG__
     _TotalNodes+=poolMap.getnum();
     if(_MinNodes>poolMap.getnum())_MinNodes=poolMap.getnum();
     if(_MaxNodes<poolMap.getnum())_MaxNodes=poolMap.getnum();
#endif
     poolMap.unmap();
     _Totalbl+=_Nreq;
     if(_Minbl>_Nreq)_Minbl=_Nreq;
     if(_Maxbl<_Nreq)_Maxbl=_Nreq;
     _Nreq=0;
 
     if(tol && _Count)cerr <<"AMSaPool::erase-S-Objects-Exist "<<_Count<<endl;
     if(_head){
       tol=(tol+_size/2)/_size;
       while(_head->_prev)_head=_head->_prev;       
       dlink *gl=_head;
       for(int i=0;i<tol;i++){
         if(_head){
           _head=_head->_next;
         }
       }              
       if(_head==gl){
         _head->_erase(_Nblocks);
         _head=0;
         _free=0;
#ifdef __AMSDEBUG__
       cout << "Apool Statistics: Min request # "<< _Minbl<<" Max "<<_Maxbl<<
       " Total "<<_Totalbl<<endl; 
       cout <<"ApMap Statistics: Min nodes # "<<_MinNodes<<" Max "<<_MaxNodes<<
       " Total "<<_TotalNodes<<endl; 
               
#endif         
       }
       else{
        if(_head)_head->_erase(_Nblocks);
        _head=gl;
        _free=(void*)(_head->_address);
       }
     }
     _lc=0;
   }

void AMSaPool::udelete(void *p){
#ifdef __AMSDEBUG__
 _Count--;
#endif
 union ALIGN{void*p; long l; long double d;};
 if(p){
   if (*((integer*)((char*)p-sizeof(ALIGN))+1) !=_Mask){
   cerr <<"AMSaPool::udelete-S-Identification failed"<<*((integer*)((char*)p-sizeof(ALIGN))+1)<<endl;
 }
 else {
   //   AMSgObj::BookTimer.start("udelete");
   poolMap.put((integer*)((char*)p-sizeof(ALIGN)));
   //   AMSgObj::BookTimer.stop("udelete");
 }
 }
}


AMSaPool::AMSaPool(integer blsize):_head(0),_free(0),_lc(0),
  _size(blsize),_Count(0),_Nblocks(0),_Minbl(10000000),_Maxbl(0),_Totalbl(0),
  _Nreq(0),_MinNodes(10000000),_MaxNodes(0),_TotalNodes(0){
#ifdef __AMSDEBUG__
    union ALIGN{void*p; long l; long double d;};
     assert(sizeof(ALIGN)>= 2*sizeof(integer));
#endif
     //   poolNode.setid(0);
     //   poolMap.map(poolNode);
  }



AMSaPoolError::AMSaPoolError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
}

char * AMSaPoolError::getmessage(){return msg;}

