
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Tracker Hit Reader class.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_SiHit.h"
#include "AMSR_SiHitReader.h"


ClassImp(AMSR_SiHitReader)



//
// struct to read data from tree converted from ntuple
//
static const Int_t MaxSiHits = 500;

static struct {
   Int_t           ntrrh;
   Int_t           px[MaxSiHits];
   Int_t           py[MaxSiHits];
   Int_t           statusr[MaxSiHits];
   Int_t           Layer[MaxSiHits];
   Float_t         hitr[MaxSiHits][3];
   Float_t         ehitr[MaxSiHits][3];
   Float_t         sumr[MaxSiHits];
   Float_t         difosum[MaxSiHits];
} _ntuple;


//_____________________________________________________________________________
AMSR_SiHitReader::AMSR_SiHitReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_SiHitReader
   //

   m_Fruits     = new TClonesArray("AMSR_SiHit",MaxSiHits,kFALSE);
   m_BranchName = "SiliconHits";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   DrawUsedHitsOnly = kFALSE;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSR_SiHitReader::Init(TTree * h1)
{
//////////////////////////////////////////////////////////
//   This file is modified from automatically generated 
//   skeleton file for prmu.root (converted from prmu.new.hbk)
//////////////////////////////////////////////////////////

//
// Set branch addresses
//
   if ( h1 != 0 ) {
     h1->SetBranchAddress("ntrrh",&_ntuple.ntrrh);
     h1->SetBranchAddress("px",_ntuple.px);
     h1->SetBranchAddress("py",_ntuple.py);
     h1->SetBranchAddress("statusr",_ntuple.statusr);
     h1->SetBranchAddress("Layer",_ntuple.Layer);
     h1->SetBranchAddress("hitr",_ntuple.hitr);
     h1->SetBranchAddress("ehitr",_ntuple.ehitr);
     h1->SetBranchAddress("sumr",_ntuple.sumr);
     h1->SetBranchAddress("difosum",_ntuple.difosum);
   }

}

//_____________________________________________________________________________
void AMSR_SiHitReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_SiHitReader::Make()
{
// Make the branch in result tree
//

   Int_t k;


//........................................................
//....Store clusters in Root ClonesArray
//........................................................
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_SiHitReader::Make(): making %d clusters.\n",
	  _ntuple.ntrrh);
   for (k=0; k<_ntuple.ntrrh; k++) {
      AddCluster(_ntuple.statusr[k],
                 _ntuple.Layer[k],
                 _ntuple.px[k],
                 _ntuple.py[k],
                &_ntuple.hitr[k][0],
                &_ntuple.ehitr[k][0],
                 _ntuple.sumr[k],
                 _ntuple.difosum[k]);
   }

   debugger.Print("AMSR_SiHitReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_SiHitReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

//_____________________________________________________________________________
Bool_t AMSR_SiHitReader::Enabled(TObject * obj)
{
  //
  // a hit with status 32 have a track passing through
  //
  AMSR_SiHit * hit = (AMSR_SiHit *) obj;
  Int_t status = hit->GetStatus();
  // debugger.Print("SiHit status = %d\n", status);
  if ( DrawUsedHitsOnly && !(status & 32) ) return kFALSE;
  else                                    return kTRUE;
}

//_____________________________________________________________________________
void  AMSR_SiHitReader::AddCluster(Int_t status, Int_t plane, Int_t px, Int_t py,
                             Float_t * hit, Float_t * errhit,
                             Float_t ampl, Float_t asym,
                             TObjArray * tracks)
{
Float_t errhitA[3];
Float_t hitA[3];
errhitA[0]=errhit[0];
errhitA[1]=errhit[1];
errhitA[2]=ampl/10000;
hitA[0]=hit[0];
hitA[1]=hit[1];
hitA[2]=hit[2]+ampl/30;
//            Add a new cluster to the list of clusters

 //Note the use of the "new with placement" to create a new cluster object.
 //This complex "new" works in the following way:
 //   clusters[i] is the value of the pointer for cluster number i in the TClonesArray
 //   if it is zero, then a new cluster must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new cluster parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   new(clusters[m_Nclusters++]) AMSR_SiHit(status, plane, px, py, 
	hitA, errhitA, ampl, asym, tracks);
}

//_____________________________________________________________________________
void AMSR_SiHitReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_SiHitReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






