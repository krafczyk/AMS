// Feb 13, 1997. ak. First try with Objectivity. Method file for AMSTimeIDD
//                  
// Last Edit : Mar 6,1997. ak.
// 


#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <timeidD.h>


AMSTimeIDD::AMSTimeIDD(char* name, integer id, AMSTimeID* p) {

 _id     = id;
 if (name) strcpy (_name,name); 

 integer nb = p ->GetNbytes();
 if (nb != 0) {
  uinteger * tmp =new uinteger[nb/sizeof(uinteger)];
  integer ncp = p ->CopyOut((void*)tmp);
  if (ncp != 0) {
    _pData.resize(ncp/sizeof(integer));
    ncp = ncp/sizeof(integer);
    for (integer i=0; i<ncp; i++) { _pData.set(i,tmp[i]);}
  }
  delete[] tmp;
 }
 time_t insert;
 time_t begin;
 time_t end;
 p -> gettime(insert, begin, end);
 _Insert   = insert;
 _Begin    = begin;
 _End      = end;
 _CRC      = p -> getCRC();
 _UpdateMe = p -> UpdateMe();
}

AMSTimeIDD::AMSTimeIDD
        (char* name, integer id, time_t begin, time_t insert, time_t end) {
 
 _id     = id;
 if (name) strcpy (_name,name); 
 _Insert = insert;
 _Begin  = begin;
 _End    = end;
}

void AMSTimeIDD::GetTime(time_t & insert, time_t & begin, time_t & end) {

insert = _Insert;
begin  = _Begin;
end    = _End;
}

void AMSTimeIDD::SetTime(time_t insert, time_t begin, time_t end) {

_Insert = insert;
_Begin  = begin;
_End    = end;
}


void AMSTimeIDD::update(AMSTimeID* p) {

 integer nb = p ->GetNbytes();
 if (nb != 0) {
  uinteger * tmp =new uinteger[nb/sizeof(integer)];
  integer ncp = p ->CopyOut((void*)tmp);
  if (ncp != 0) {
    if (_pData.size() != ncp/sizeof(integer) ) 
                              _pData.resize(ncp/sizeof(integer));
    ncp = ncp/sizeof(integer);
    for (integer i=0; i<ncp; i++) { _pData.set(i,tmp[i]);}
  }
  delete[] tmp;
 }
 time_t insert;
 time_t begin;
 time_t end;
 p -> gettime(insert, begin, end);
 _Insert = insert;
 _Begin  = begin;
 _End    = end;
 _CRC    = p -> getCRC();
 _UpdateMe = p -> UpdateMe();
}

void AMSTimeIDD::copy(uinteger* tmp) {

  integer N = _pData.size();
  for (integer i=0; i<N; i++) { tmp[i] = _pData[i]; }
}

void AMSTimeIDD::PrintTime() {
  cout<<"Time (insert, begin, end) "<<endl;
  cout<<"                          "<<asctime(localtime(&_Insert))<<endl;
  cout<<"                          "<<asctime(localtime(&_Begin))<<endl;
  cout<<"                          "<<asctime(localtime(&_End))<<endl;
}
