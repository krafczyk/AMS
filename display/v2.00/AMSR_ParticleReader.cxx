
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_ParticleReader.h"
#include "AMSR_Particle.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSR_ParticleReader)




//
// The structure for reading data
//
const Int_t maxpart=20;
static struct {
   Int_t           npart;
   Int_t           pctcp[maxpart][2];
   Int_t           pbetap[maxpart];
   Int_t           pchargep[maxpart];
   Int_t           ptrackp[maxpart];
   Int_t           pid[maxpart];
   Float_t         pmass[maxpart];
   Float_t         perrmass[maxpart];
   Float_t         pmom[maxpart];
   Float_t         perrmom[maxpart];
   Float_t         pcharge[maxpart];
   Float_t         ptheta[maxpart];
   Float_t         pphi[maxpart];
   Float_t         pcoo[maxpart][3];
   Float_t         signalctc[maxpart][2];
   Float_t         betactc[maxpart][2];
   Float_t         errorbetactc[maxpart][2];
   Float_t         cooctc[maxpart][2][3];
   Float_t         beta[maxpart];
   Float_t         errbeta[maxpart];
   Int_t           pattern[maxpart];
   Int_t           betapattern[maxpart];
   Int_t           trstatus[maxpart];
} _ntuple;








//_____________________________________________________________________________
AMSR_ParticleReader::AMSR_ParticleReader()
{
   m_NParticles = 0;
}

//_____________________________________________________________________________
AMSR_ParticleReader::AMSR_ParticleReader(const char *name, const char *title)
                 :AMSR_Maker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSR_Particle", 3, kFALSE);
   m_BranchName = "Particles";
   m_NParticles    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSR_ParticleReader::~AMSR_ParticleReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSR_Particle *AMSR_ParticleReader::AddParticle(Int_t code, Int_t mcparticle)
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
   return new(particles[m_NParticles++]) AMSR_Particle();
}

//_____________________________________________________________________________
void AMSR_ParticleReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NParticles = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSR_ParticleReader::Init(TTree * h1)
{
//     Tracks histograms
//   m_Mult        = new TH1F("TraMult","particle multiplicity",100,0,100);


   if (h1)
   {
     h1->SetBranchAddress("npart",        &_ntuple.npart);
     h1->SetBranchAddress("pctcp",         _ntuple.pctcp);
     h1->SetBranchAddress("pbetap",        _ntuple.pbetap);
     h1->SetBranchAddress("pchargep",      _ntuple.pchargep);
     h1->SetBranchAddress("ptrackp",       _ntuple.ptrackp);
     h1->SetBranchAddress("pid",           _ntuple.pid);
     h1->SetBranchAddress("pmass",         _ntuple.pmass);
     h1->SetBranchAddress("perrmass",      _ntuple.perrmass);
     h1->SetBranchAddress("pmom",          _ntuple.pmom);
     h1->SetBranchAddress("perrmom",       _ntuple.perrmom);
     h1->SetBranchAddress("pcharge",       _ntuple.pcharge);
     h1->SetBranchAddress("ptheta",        _ntuple.ptheta);
     h1->SetBranchAddress("pphi",          _ntuple.pphi);
     h1->SetBranchAddress("pcoo",          _ntuple.pcoo);
     h1->SetBranchAddress("signalctc",     _ntuple.signalctc);
     h1->SetBranchAddress("betactc",       _ntuple.betactc);
     h1->SetBranchAddress("errorbetactc",  _ntuple.errorbetactc);
//   h1->SetBranchAddress("cooctc",        _ntuple.cooctc);
     h1->SetBranchAddress("beta",       _ntuple.beta);
     h1->SetBranchAddress("betaerror",       _ntuple.errbeta);
     h1->SetBranchAddress("betapattern",       _ntuple.betapattern);
     h1->SetBranchAddress("pattern",       _ntuple.pattern);
     h1->SetBranchAddress("trstatus",       _ntuple.trstatus);

     debugger.Print("AMSR_ParticleReader::Init(): branch address set\n");
   }

}

//_____________________________________________________________________________
void AMSR_ParticleReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSR_ParticleReader::Make()
{

   Int_t k, i;

   m_NParticles = _ntuple.npart;
   //   m_NParticles=0;
   debugger.Print("AMSR_ParticleReader::Make(): making %d particles.\n", m_NParticles);
   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NParticles; k++) {
      debugger.Print("Making particle #%d:\n");

      new(particles[k]) AMSR_Particle();
      AMSR_Particle * t = (AMSR_Particle *) particles[k];
      for (i=0; i<2; i++) {
        t->m_PCerenkov[i]   = _ntuple.pctcp[k][i];
        t->m_SignalCTC[i]   = _ntuple.signalctc[k][i];
        t->m_BetaCTC[i]     = _ntuple.betactc[k][i];
        t->m_ErrBetaCTC[i]  = _ntuple.errorbetactc[k][i];
//      t->m_PositionCTC[i] = _ntuple.cooctc[k][i];	// trouble here
      }
      t->m_PBeta            = _ntuple.pbetap[k];
      t->m_PCharge          = _ntuple.pchargep[k];
      t->m_PTrack           = _ntuple.ptrackp[k];
      t->m_GeantID          = _ntuple.pid[k];
      t->m_Mass             = _ntuple.pmass[k];
      t->m_ErrMass          = _ntuple.perrmass[k];
      t->m_Momentum         = _ntuple.pmom[k];
      t->m_ErrMomentum      = _ntuple.perrmom[k];
      t->m_Charge           = _ntuple.pcharge[k];
      t->m_Theta            = _ntuple.ptheta[k];
      t->m_Phi              = _ntuple.pphi[k];
      t->m_beta             = _ntuple.beta[t->m_PBeta-1];
      t->m_errbeta          = _ntuple.errbeta[t->m_PBeta-1];
      t->m_pattern          = _ntuple.pattern[t->m_PTrack-1];
      t->m_betapattern      = _ntuple.betapattern[t->m_PBeta-1];
      t->m_trstatus         = _ntuple.trstatus[t->m_PTrack-1];
      for (i=0; i<3; i++)
          t->m_Position[i]  = _ntuple.pcoo[k][i];

      t->SetHelix();
   }

//   m_Mult->Fill(m_NParticles+0.01);
}

//_____________________________________________________________________________
void AMSR_ParticleReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_ParticleReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

