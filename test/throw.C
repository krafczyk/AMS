//  $Id: throw.C,v 1.7 2001/01/22 17:32:56 choutko Exp $
#include <iostream.h>
#include <stdio.h>

#include <stdlib.h>
class A{
int _i;
public:
  int geti(){return _i;}
  A(int i=0):_i(i){
    if(i<0){
      throw i;
      cout <<"This version of g++ is BUGGY"<<endl;
      exit(1);
    }
  }
  void seti(int i=0){_i=i;}
  static void exc(int i)  ;
};

void A::exc(int i)  {
  try{
     A* pa=new A(i);
  }
catch (int i){
  cerr <<" This version of g++ is OK"<<endl;
}
}

main(){
int i;
for(;;){
i=-5;
A::exc(i);
exit(1);
}
return 0;
}
