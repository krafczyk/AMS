//  $Id: ins_dummy.C,v 1.4 2001/01/22 17:32:20 choutko Exp $
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

