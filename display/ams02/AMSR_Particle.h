#ifndef AMSR_Particle_H
#define AMSR_Particle_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#include <THelix.h>

class AMSR_ParticleReader;


//class AMSR_Particle : public AMSR_Track {
class AMSR_Particle : public THelix {

private:

   Int_t        m_PCerenkov[2];		//Pointers to matched Cerenkov clusters
   Int_t	m_PBeta;		//Pointer to its Beta properties
   Int_t	m_PCharge;		//Pointer to Charge properties
   Int_t	m_PTrack;		//Pointer to Track properties
   Int_t	m_GeantID;		//Particle ID a la Geant
   Float_t	m_Mass;			//Mass
   Float_t	m_ErrMass;		//Error of mass
   Float_t	m_Momentum;		//Momentum
   Float_t	m_ErrMomentum;		//Error of momentum
   Float_t	m_Charge;		//Charge
   Float_t	m_Theta;		//Track Theta (on entry)
   Float_t	m_Phi;			//Track Phi (on entry)
   Float_t	m_Position[3];		//Track Position (on entry)
   Float_t	m_SignalCTC[2];		//Signal in Cerenkov
   Float_t	m_BetaCTC[2];		//Beta from Cerenkov
   Float_t	m_ErrBetaCTC[2];	//Error of the above
   Float_t	m_PositionCTC[2][3];	//Track extrapolated into Cerenkov
   Float_t      m_beta;
   Float_t      m_errbeta;
   Int_t        m_betapattern;
   Int_t        m_pattern;
   Int_t        m_trstatus;

public:
                  AMSR_Particle() {;}
//                AMSR_Particle(Int_t status, Int_t pattern);
   virtual       ~AMSR_Particle() {;}
   virtual char  *GetObjectInfo(Int_t px, Int_t py);
           void   SetHelix();
   virtual void   Paint(Option_t *option="");

   //
   // Getters and Setters...
   //
   friend class AMSR_ParticleReader;

   ClassDef(AMSR_Particle, 1)   //AMSR_Root track class
};

#endif
