#include <iostream.h>
class A{
int _i;
public:
  int geti(){return _i;}
  A(int i=0):_i(i){if(i<0)throw i;cout <<"Throw failed"<<endl;}
  void seti(int i=0){_i=i;}
  static void exc(int i)  ;
};

void A::exc(int i)  {
  try{
     A* pa=new A(i);
  }
catch (int i){
  cout <<" Throw ok"<<endl;
}
}

main(){
int i;
for(;;){
i=-5;
A::exc(i);
}
return 0;
}
