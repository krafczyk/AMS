
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSRoot.h"
#include "AMSMCParticleReader.h"
#include "AMSMCParticle.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSMCParticleReader)




//
// The structure for reading data
//
const Int_t maxpart=20;
static struct {
   Int_t           npart;
   Int_t           pid[maxpart];
   Float_t         pmass[maxpart];
   Float_t         pmom[maxpart];
   Float_t         pcharge[maxpart];
   Float_t         pcoo[maxpart][3];
   Float_t         pdir[maxpart][3];
} _ntuple;








//_____________________________________________________________________________
AMSMCParticleReader::AMSMCParticleReader()
{
   m_NParticles = 0;
}

//_____________________________________________________________________________
AMSMCParticleReader::AMSMCParticleReader(const char *name, const char *title)
                 :AMSMaker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSMCParticle", 3, kFALSE);
   m_BranchName = "MCParticles";
   m_NParticles    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSMCParticleReader::~AMSMCParticleReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSMCParticle *AMSMCParticleReader::AddParticle(Int_t code, Int_t mcparticle)
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
   return new(particles[m_NParticles++]) AMSMCParticle();
}

//_____________________________________________________________________________
void AMSMCParticleReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NParticles = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSMCParticleReader::Init(TTree * h1)
{
//     Tracks histograms
//   m_Mult        = new TH1F("TraMult","particle multiplicity",100,0,100);


   if (h1)
   {
     h1->SetBranchAddress("nmcg",        &_ntuple.npart);
     h1->SetBranchAddress("Particle",           _ntuple.pid);
     h1->SetBranchAddress("mass",         _ntuple.pmass);
     h1->SetBranchAddress("momentum",          _ntuple.pmom);
     h1->SetBranchAddress("charge",       _ntuple.pcharge);
     h1->SetBranchAddress("dir",         _ntuple.pdir);
     h1->SetBranchAddress("coo",          _ntuple.pcoo);
 
     debugger.Print("AMSMCParticleReader::Init(): branch address set\n");
   }

}

//_____________________________________________________________________________
void AMSMCParticleReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSMCParticleReader::Make()
{

   Int_t k, i;

   m_NParticles = _ntuple.npart;
   //   m_NParticles=0;
   debugger.Print("AMSMCParticleReader::Make(): making %d particles.\n", m_NParticles);
   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NParticles; k++) {
      debugger.Print("Making particle #%d:\n");

      new(particles[k]) AMSMCParticle();
      AMSMCParticle * t = (AMSMCParticle *) particles[k];
      t->m_GeantID          = _ntuple.pid[k];
      t->m_Mass             = _ntuple.pmass[k];
      t->m_Momentum         = _ntuple.pmom[k];
      t->m_Charge           = _ntuple.pcharge[k];
      for (i=0; i<3; i++){
          t->m_Position[i]  = _ntuple.pcoo[k][i];
          t->m_Dir[i]  = _ntuple.pdir[k][i];
      }
      t->SetHelix();
   }

}

//_____________________________________________________________________________
void AMSMCParticleReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

