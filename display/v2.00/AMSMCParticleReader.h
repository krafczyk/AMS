#ifndef AMSMCParticleReader_H
#define AMSMCParticleReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot Particle Reader class.                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef AMSMaker_H
#include "AMSMaker.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif

class AMSMCParticle;

class AMSMCParticleReader : public AMSMaker {

protected:
   Int_t          m_NParticles;           //Number of tracks


public:
                  AMSMCParticleReader();
                  AMSMCParticleReader(const char *name, const char *title);
   virtual       ~AMSMCParticleReader();
   AMSMCParticle   *AddParticle(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
   virtual void   Init(TTree * h1=0);
   virtual void   Make();
   virtual void   PrintInfo();

//    Getters



//    Setters for tracks

   ClassDef(AMSMCParticleReader, 1)   //AMSRoot Particle Reader
};

#endif
