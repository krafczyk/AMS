//  $Id: sizeof.C,v 1.2 2001/01/22 17:32:56 choutko Exp $
#include <iostream.h>
main(){
cout<<"char "<<sizeof(char)<<endl;
cout<<"int "<<sizeof(int)<<endl;
cout<<"long "<<sizeof(long)<<endl;
cout<<"float "<<sizeof(float)<<endl;
cout<<"double "<<sizeof(double)<<endl;
cout<<"int* "<<sizeof(int*)<<endl;
cout<<"char* "<<sizeof(char*)<<endl;
cout<<"long double  "<<sizeof(long double)<<endl;
union ALIGN{void *p; long double d; long l;};
cout<<"align  "<<sizeof(ALIGN)<<endl;
int i;
cin >> i;
i=i+sizeof(ALIGN)-1;
i-=i%sizeof(ALIGN);
cout <<i <<endl;
return 0;
}
