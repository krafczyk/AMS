#ifndef AMSVMC_MFIELD_H
#define AMSVMC_MFIELD_H


#include <TVirtualMCApplication.h>
#include <TGeoManager.h>
#include <TVirtualMagField.h>


class amsvmc_MagField : public TVirtualMagField
{
  public:
    amsvmc_MagField();
    virtual ~amsvmc_MagField();     

    // methods
    virtual void  Field(const double* xdouble, double* bdouble);

  ClassDef(amsvmc_MagField,1)  //amsvmc_MagField
};



#endif
