//  $Id: ins_dummy.C,v 1.5 2001/12/10 20:03:04 choutko Exp $
#include <stdlib.h>
#ifdef  __IBMAIX__    
extern "C" void setkey(const char* a);
extern "C" void encrypt(char* a, int n);
extern "C" char* crypt(const char* a, const char* b);
extern "C"   void __setkey(const char* s){
              setkey(s);
}

extern "C" char* __crypt(const char* a, const char* b){
           return (crypt(a,b));
}


extern "C" void __encrypt(char* a, int n){
            encrypt(a,n);
}

#endif
#ifdef sun
extern "C" void dlopen(){
};
extern "C" void dlclose(){
};
extern "C" void dlsym(){
};
extern "C" void dlerror(){
};
#endif

