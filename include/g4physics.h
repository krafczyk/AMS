#ifndef __G4PHYSICS__
#define __G4PHYSICS__

#include "G4VUserPhysicsList.hh"
#include "globals.hh"
#include <node.h>

class AMSG4Physics: public AMSNode, public G4VUserPhysicsList
{
public:
  AMSG4Physics();
  virtual ~AMSG4Physics();
  public:
  char* G3toG4(G4int pid);
  G4int G4toG3(const G4String&  particle);
  void _init();
  // Construct particle and physics
  virtual void ConstructParticle();
  virtual void ConstructProcess();
  
  // 
  virtual void SetCuts();
  
protected:
  static G4int _G3DummyParticle;
  AMSIDi* _pg3tog4;
  AMSIDs* _pg4tog3;
  integer _Ng3tog4; 
 // these methods Construct physics processes and register them
  virtual void ConstructGeneral();
  virtual void ConstructEM();
  virtual void ConstructHad();

  // these methods Construct all particles in each category
  virtual void ConstructAllBosons();
  virtual void ConstructAllLeptons();
  virtual void ConstructAllMesons();
  virtual void ConstructAllBarions();
  virtual void ConstructAllIons();
  virtual void ConstructAllShortLiveds();
};


#endif
