//  $Id: AMSTrackReader.cxx,v 1.2 2001/01/22 17:32:40 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSRoot.h"
//#include "ATLFMCMaker.h"
#include "AMSTrackReader.h"
#include "AMSTrack.h"

const Float_t kPI   = TMath::Pi();
const Float_t k2PI  = 2*kPI;
const Int_t kMAXTRA = 100;

ClassImp(AMSTrackReader)




//
// The structure for reading data
//
const Int_t maxtr=20;
static struct {
   Int_t           ntrtr;
   Int_t           trstatus[maxtr];
   Int_t           pattern[maxtr];
   Int_t           nhits[maxtr];
   Int_t           phits[maxtr][6];
   Int_t           Fastfitdone[maxtr];
   Int_t           Geanefitdone[maxtr];
   Int_t           Advancedfitdone[maxtr];
   Float_t         Chi2strline[maxtr];
   Float_t         Chi2circle[maxtr];
   Float_t         Circlerigidity[maxtr];
   Float_t         Chi2fastfit[maxtr];
   Float_t         Rigidity[maxtr];
   Float_t         Errrigidity[maxtr];
   Float_t         Theta[maxtr];
   Float_t         phi[maxtr];
   Float_t         p0[maxtr][3];
   Float_t         gchi2[maxtr];
   Float_t         grigidity[maxtr];
   Float_t         gerrrigidity[maxtr];
   Float_t         gtheta[maxtr];
   Float_t         gphi[maxtr];
   Float_t         gp0[maxtr][3];
   Float_t         hchi2[maxtr][2];
   Float_t         Hrigidity[maxtr][2];
   Float_t         Herrrigidity[maxtr][2];
   Float_t         htheta[maxtr][2];
   Float_t         hphi[maxtr][2];
   Float_t         hp0[maxtr][2][3];
   Float_t         fchi2ms[maxtr];
   Float_t         gchi2ms[maxtr];
   Float_t         rigidityms[maxtr];
   Float_t         grigidityms[maxtr];
} _ntuple;








//_____________________________________________________________________________
AMSTrackReader::AMSTrackReader()
{
   m_NTracks = 0;
}

//_____________________________________________________________________________
AMSTrackReader::AMSTrackReader(const char *name, const char *title)
                 :AMSMaker(name,title)
{
//    Default Setters for tracks

   m_Fruits     = new TClonesArray("AMSTrack",maxtr, kFALSE);
   m_BranchName = "Tracks";
   m_NTracks    = 0;
// Please, how to do this optionally ??!!!
   Save();
}

//_____________________________________________________________________________
AMSTrackReader::~AMSTrackReader()
{
   //dummy
}

//_____________________________________________________________________________
AMSTrack *AMSTrackReader::AddTrack(Int_t code, Int_t mcparticle)
{
//            Add a new track to the list of tracks

 //Note the use of the "new with placement" to create a new track object.
 //This complex "new" works in the following way:
 //   tracks[i] is the value of the pointer for track number i in the TClonesArray
 //   if it is zero, then a new track must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new track parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &tracks = *(TClonesArray*)m_Fruits;
   return new(tracks[m_NTracks++]) AMSTrack(code,mcparticle);
   //   return new(tracks[m_NTracks++]) AMSTrack(code,mcparticle);
}

//_____________________________________________________________________________
void AMSTrackReader::Clear(Option_t *option)
{
//    Reset Track Maker

   m_NTracks = 0;
   if (m_Fruits) m_Fruits->Clear();
}


//_____________________________________________________________________________
void AMSTrackReader::Init(TTree * h1)
{
//     Tracks histograms
   m_Mult        = new TH1F("TraMult","tracks multiplicity",100,0,100);


   if (h1)
   {
     h1->SetBranchAddress("ntrtr",               &_ntuple.ntrtr);
     h1->SetBranchAddress("trstatus",             _ntuple.trstatus);
     h1->SetBranchAddress("pattern",              _ntuple.pattern);
     h1->SetBranchAddress("nhits",                _ntuple.nhits);
     h1->SetBranchAddress("phits",                _ntuple.phits);
     h1->SetBranchAddress("Fastfitdone",          _ntuple.Fastfitdone);
     h1->SetBranchAddress("Geanefitdone",         _ntuple.Geanefitdone);
     h1->SetBranchAddress("Advancedfitdone",      _ntuple.Advancedfitdone);
     h1->SetBranchAddress("Chi2strline",          _ntuple.Chi2strline);
     h1->SetBranchAddress("Chi2circle",           _ntuple.Chi2circle);
     h1->SetBranchAddress("Circleridgidity",      _ntuple.Circlerigidity);
     h1->SetBranchAddress("Chi2fastfit",          _ntuple.Chi2fastfit);
     h1->SetBranchAddress("Ridgidity",            _ntuple.Rigidity);
     h1->SetBranchAddress("Errridgidity",         _ntuple.Errrigidity);
     h1->SetBranchAddress("Theta",                _ntuple.Theta);
     h1->SetBranchAddress("phi",                  _ntuple.phi);
     h1->SetBranchAddress("p0",                   _ntuple.p0);
     h1->SetBranchAddress("gchi2",                _ntuple.gchi2);
     h1->SetBranchAddress("gridgidity",           _ntuple.grigidity);
     h1->SetBranchAddress("gerrridgidity",        _ntuple.gerrrigidity);
     h1->SetBranchAddress("gtheta",               _ntuple.gtheta);
     h1->SetBranchAddress("gphi",                 _ntuple.gphi);
     h1->SetBranchAddress("gp0",                  _ntuple.gp0);
     h1->SetBranchAddress("hchi2",                _ntuple.hchi2);
     h1->SetBranchAddress("Hridgidity",           _ntuple.Hrigidity);
     h1->SetBranchAddress("Herrridgidity",        _ntuple.Herrrigidity);
     h1->SetBranchAddress("htheta",               _ntuple.htheta);
     h1->SetBranchAddress("hphi",                 _ntuple.hphi);
     h1->SetBranchAddress("hp0",                  _ntuple.hp0);
     h1->SetBranchAddress("fchi2ms",              _ntuple.fchi2ms);
     h1->SetBranchAddress("gchi2ms",              _ntuple.gchi2ms);
     h1->SetBranchAddress("ridgidityms",          _ntuple.rigidityms);
     h1->SetBranchAddress("gridgidityms",         _ntuple.grigidityms);

   }

}

//_____________________________________________________________________________
void AMSTrackReader::Finish()
{
// Function called when all makers for one event have been called

}

//_____________________________________________________________________________
void AMSTrackReader::Make()
{

   Int_t k, i;
   m_NTracks = _ntuple.ntrtr;
   if(m_NTracks >= maxtr)m_NTracks=maxtr;
   debugger.Print("AMSTrackReader::Make(): making %d tracks.\n", m_NTracks);
   TClonesArray &tracks = *(TClonesArray*)m_Fruits;
   for (k=0; k<m_NTracks; k++) {
      debugger.Print("Making track #%d: status %d, pattern %d, %d hits\n",
		k, _ntuple.trstatus[k], _ntuple.pattern[k], _ntuple.nhits[k]);

      new(tracks[k]) AMSTrack(_ntuple.trstatus[k], _ntuple.pattern[k]);
      AMSTrack * t = (AMSTrack *) tracks[k];
      t->m_ID              = k + 1;	    // in ntuple it starts at 1 not 0
      t->m_NHits           = _ntuple.nhits[k];
      for (i=0; i<6; i++)
	t->m_PHits[i]      = _ntuple.phits[k][i];
      t->m_FastFitDone     = _ntuple.Fastfitdone[k];
      t->m_GeaneFitDone    = _ntuple.Geanefitdone[k];
      t->m_AdvancedFitDone = _ntuple.Advancedfitdone[k];
      t->m_Chi2StrLine     = _ntuple.Chi2strline[k];
      t->m_Chi2Circle      = _ntuple.Chi2circle[k];
      t->m_CircleRigidity  = _ntuple.Circlerigidity[k];

      t->m_FChi2           = _ntuple.Chi2fastfit[k];
      t->m_FRigidity       = _ntuple.Rigidity[k];
      t->m_FErrRigidity    = _ntuple.Errrigidity[k];
      t->m_FTheta          = _ntuple.Theta[k];
      t->m_FPhi            = _ntuple.phi[k];
      for(i=0; i<3; i++)
        t->m_FP0[i]        = _ntuple.p0[k][i];

      t->m_GChi2           = _ntuple.gchi2[k];
      t->m_GRigidity       = _ntuple.grigidity[k];
      t->m_GErrRigidity    = _ntuple.gerrrigidity[k];
      t->m_GTheta          = _ntuple.gtheta[k];
      t->m_GPhi            = _ntuple.gphi[k];
      for(i=0; i<3; i++)
        t->m_GP0[i]        = _ntuple.gp0[k][i];

      for (i=0; i<2; i++) {
        t->m_HChi2[i]        = _ntuple.hchi2[k][i];
        t->m_HRigidity [i]   = _ntuple.Hrigidity[k][i];
        t->m_HErrRigidity[i] = _ntuple.Herrrigidity[k][i];
        t->m_HTheta[i]       = _ntuple.htheta[k][i];
        t->m_HPhi[i]         = _ntuple.hphi[k][i];
//	problem with 3d arrays
//      for(j=0; j<3; j++)
//        t->m_GP0[i][j]     = _ntuple.hp0[k][i][j];
      }

      t->m_FChi2MS         = _ntuple.fchi2ms[k];
      t->m_GChi2MS         = _ntuple.gchi2ms[k];
      t->m_RigidityMS      = _ntuple.rigidityms[k];
      t->m_GRigidityMS     = _ntuple.grigidityms[k];

      t->SetHelix();
   }
   debugger.Print("AMSTrackReader::Make(): %d tracks made.\n", m_NTracks);

   // m_Mult->Fill(m_NTracks+0.01);		// somehow this line is generating segmenation violations...
}

//_____________________________________________________________________________
void AMSTrackReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSTrackReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among particles
}
*/

