#ifndef AMSVMC_RUNCONFIGURATION_H
#define AMSVMC_RUNCONFIGURATION_H

#include "TG4RunConfiguration.h"


class amsvmc_RunConfiguration : public TG4RunConfiguration
{
  public:
    amsvmc_RunConfiguration(const TString& userGeometry,
			  const TString& specialProcess = "stepLimiter");
    virtual ~amsvmc_RunConfiguration();

    // methods
    virtual G4VUserPhysicsList*  CreatePhysicsList();
    //    virtual TG4VUserRegionConstruction*  CreateUserRegionConstruction();

    ClassDef(amsvmc_RunConfiguration,1) // Ex03MCStack
};


#endif // AMSVMC_RUNCONFIGURATION_H
