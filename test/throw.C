#include <iostream.h>
class A{
int _i;
public:
  int geti(){return _i;}
  A(int i=0):_i(i){if(_i<0)throw i;}
  void seti(int i=0){_i=i;}
  static void exc(int i) throw(int) ;
};

void A::exc(int i) throw (int){
A* pa=new A(i);
catch (int i){
  cout <<" opa "<<i<<endl;
}
}

main(){
int i;
for(;;){
cin >> i;
A::exc(i);

}
return 0;
}
