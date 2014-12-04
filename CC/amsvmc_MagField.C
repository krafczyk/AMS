#include <iostream>
#include <TVirtualMagField.h>
#include "amsvmc_MagField.h"


/// \cond CLASSIMP
ClassImp(amsvmc_MagField)
/// \endcond


extern "C" void gufldvmc_(float [], float []);



amsvmc_MagField::amsvmc_MagField()
{
/// Default constructor
}

amsvmc_MagField::~amsvmc_MagField() 
{
/// Destructor  
}


void amsvmc_MagField::Field(const double* xdouble, double* bdouble)
{
  //   cout<<"in amsvmc_MagField::Field"<<endl;
  float temp_xfloat[3];
  float temp_bfloat[3]={0,0,0};
  for(int i=0;i<3;i++) temp_xfloat[i]= xdouble[i];
  //  cout<<"Position float:"<<temp_xfloat[0]<<", "<<temp_xfloat[1]<<", "<<temp_xfloat[2]<<endl;
  //  cout<<"Position double :"<<xdouble[0]<<", "<<xdouble[1]<<", "<<xdouble[2]<<endl;
  gufldvmc_(temp_xfloat,temp_bfloat);
  //  cout<<"B_Field:"<<temp_bfloat[0]<<", "<<temp_bfloat[1]<<", "<<temp_bfloat[2]<<endl;
  for(int i=0;i<3;i++) bdouble[i] =  temp_bfloat[i];
  //  cout<<""<<endl;
}
