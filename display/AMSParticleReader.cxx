
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TClonesArray.h>
#include <TMCParticle.h>

#include "AMSRoot.h"
#include "AMSParticleReader.h"
#include "AMSParticle.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSParticleReader)




//
// The structure for reading data
//
static struct {
   Int_t           npart;
   Int_t           pctcp[30][2];
   Int_t           pbetap[60];
   Int_t           pchargep[60];
   Int_t           ptrackp[60];
   Int_t           pid[60];
   Float_t         pmass[60];
   Float_t         perrmass[60];
   Float_t         pmom[60];
   Float_t         perrmom[60];
   Float_t         pcharge[60];
   Float_t         ptheta[60];
   Float_t         pphi[60];
   Float_t         pcoo[20][3];
   Float_t         signalctc[30][2];
   Float_t         betactc[30][2];
   Float_t         errorbetactc[30][2];
   Float_t         cooctc[30][2];
} _ntuple;








//_____________________________________________________________________________
AMSParticleReader::AMSParticleReader()
{
   m_NParticles = 0;
}

//_____________________________________________________________________________
AMSParticleReader::AMSParticleReader(const char *name, const char *title)
                 :AMSMaker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSParticle", 3, kFALSE);
   m_BranchName = "Particles";
   m_NParticles    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSParticleReader::~AMSParticleReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSParticle *AMSParticleReader::AddParticle(Int_t code, Int_t mcparticle)
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
   return new(particles[m_NParticles++]) AMSParticle();
}

//_____________________________________________________________________________
void AMSParticleReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NParticles = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSParticleReader::Init(TTree * h1)
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

     debugger.Print("AMSParticleReader::Init(): branch address set\n");
   }

}

//_____________________________________________________________________________
void AMSParticleReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSParticleReader::Make()
{

   Int_t k, i;

   m_NParticles = _ntuple.npart;
   debugger.Print("AMSParticleReader::Make(): making %d particles.\n", m_NParticles);
   TClonesArray &particles = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NParticles; k++) {
      debugger.Print("Making particle #%d:\n");

      new(particles[k]) AMSParticle();
      AMSParticle * t = (AMSParticle *) particles[k];
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
      for (i=0; i<3; i++)
          t->m_Position[i]  = _ntuple.pcoo[k][i];

      t->SetHelix();
   }

//   m_Mult->Fill(m_NParticles+0.01);
}

//_____________________________________________________________________________
void AMSParticleReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSParticleReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

