#ifndef AMSMCParticle_H
#define AMSMCParticle_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSMCParticleReader;


//class AMSMCParticle : public AMSTrack {
class AMSMCParticle : public THelix {

private:

   Int_t	m_GeantID;		//Particle ID a la Geant
   Float_t	m_Mass;			//Mass
   Float_t	m_Momentum;		//Momentum
   Float_t	m_Charge;		//Charge
   Float_t	m_Position[3];		//Track Position (on entry)
   Float_t	m_Dir[3];		//Track Position (on entry)

public:
                  AMSMCParticle() {;}
   virtual       ~AMSMCParticle() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSMCParticleReader;

   ClassDef(AMSMCParticle, 1)   //AMSRoot track class
};

#endif
