#ifndef AMSParticleReader_H
#define AMSParticleReader_H

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

class AMSParticle;

class AMSParticleReader : public AMSMaker {

protected:
   Int_t          m_NParticles;           //Number of tracks

//     Particles histograms
   TH1F          *m_Mult;              //tracks multiplicity

public:
                  AMSParticleReader();
                  AMSParticleReader(const char *name, const char *title);
   virtual       ~AMSParticleReader();
   AMSParticle   *AddParticle(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
//         void   HelixParameters(Float_t charge, Float_t *vert1, Float_t *pvert1, Float_t *b);
   virtual void   Init(TTree * h1=0);
   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//    Getters

//     Getters Tracks histograms
   TH1F          *Mult() {return m_Mult;}


//    Setters for tracks

   ClassDef(AMSParticleReader, 1)   //AMSRoot Particle Reader
};

#endif
