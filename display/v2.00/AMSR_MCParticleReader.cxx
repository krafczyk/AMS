//  $Id: AMSR_MCParticleReader.cxx,v 1.3 2001/01/22 17:32:39 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_MCParticleReader.h"
#include "AMSR_MCParticle.h"
#include "AMSR_Ntuple.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSR_MCParticleReader)


//_____________________________________________________________________________
AMSR_MCParticleReader::AMSR_MCParticleReader()
{
   m_NParticles = 0;
}

//_____________________________________________________________________________
AMSR_MCParticleReader::AMSR_MCParticleReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSR_MCParticle", 3, kFALSE);
   m_BranchName = "MCParticles";
   m_NParticles    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_MCParticleReader::~AMSR_MCParticleReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_MCParticle *AMSR_MCParticleReader::AddParticle(Int_t code, Int_t mcparticle)
{
//            Add a new particle to the list of particles

 //Note the use of the "new with placement" to create a new track object.
 //This complex "new" works in the following way:
 //   tracks[i] is the value of the pointer for track number i in the TClonesArray
 //   if it is zero, then a new track must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new track parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   return new(particles[m_NParticles++]) AMSR_MCParticle();
}

//_____________________________________________________________________________
void AMSR_MCParticleReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NParticles = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_MCParticleReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_MCParticleReader::Make()
{

   Int_t k, i;
   MCEVENTG_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkMceventg;

   m_NParticles = _ntuple->nmcg;
   //   m_NParticles=0;
   debugger.Print("AMSR_MCParticleReader::Make(): making %d particles.\n", m_NParticles);
   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NParticles; k++) {
      debugger.Print("Making particle #%d:\n");

      new(particles[k]) AMSR_MCParticle();
      AMSR_MCParticle * t = (AMSR_MCParticle *) particles[k];
      t->m_GeantID          = _ntuple->Particle[k];
      t->m_Mass             = _ntuple->mass[k];
      t->m_Momentum         = _ntuple->momentum[k];
      t->m_Charge           = _ntuple->charge[k];
      for (i=0; i<3; i++){
          t->m_Position[i]  = _ntuple->coo[k][i];
          t->m_Dir[i]  = _ntuple->dir[k][i];
      }
      t->SetHelix();
   }

}

//_____________________________________________________________________________
void AMSR_MCParticleReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

