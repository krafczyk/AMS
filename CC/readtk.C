#include <io.h>
#include <tkdbc.h>
#include <trid.h>
#include <commons.h>
AMSDATADIR_DEF AMSDATADIR;
int main(){
  int i,j,k;
  cout << " Please give lay lad sen ";  
  cin >> i >>j >>k;
  cout << " Number is " <<TKDBc::getnum(i-1,j-1,k-1)<<endl;
return 0;
}
