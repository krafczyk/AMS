
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster Reader class.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_CTCCluster.h"
#include "AMSR_CTCClusterReader.h"


ClassImp(AMSR_CTCClusterReader)



//
// struct to read data from tree converted from ntuple
//
static struct {
   Int_t           nctccl;
   Int_t           Ctcstatus[20];
   Int_t           Ctclayer[20];
   Float_t         ctccoo[20][3];
   Float_t         ctcercoo[20][3];
   Float_t         ctcrawsignal[20];
   Float_t         ctcsignal[20];
   Float_t         ctcesignal[20];
} _ntuple;


//_____________________________________________________________________________
AMSR_CTCClusterReader::AMSR_CTCClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_CTCClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_CTCCluster",8,kFALSE);
   m_BranchName = "CTCClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::Init(TTree * h1)
{
//////////////////////////////////////////////////////////
//   This file is modified from automatically generated 
//   skeleton file for prmu.root (converted from prmu.new.hbk)
//////////////////////////////////////////////////////////

//
// Set branch addresses
//
   if ( h1 != 0 ) {
     h1->SetBranchAddress("nctccl",&_ntuple.nctccl);
     h1->SetBranchAddress("Ctcstatus",_ntuple.Ctcstatus);
     h1->SetBranchAddress("Ctclayer",_ntuple.Ctclayer);
     h1->SetBranchAddress("ctccoo",_ntuple.ctccoo);
     h1->SetBranchAddress("ctcercoo",_ntuple.ctcercoo);
     h1->SetBranchAddress("ctcrawsignal",_ntuple.ctcrawsignal);
     h1->SetBranchAddress("ctcsignal",_ntuple.ctcsignal);
     h1->SetBranchAddress("ctcesignal",_ntuple.ctcesignal);

     debugger.Print("AMSR_CTCClusterReader::Init(): branch address set\n");
   }
}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_CTCClusterReader::Make(): making %d clusters.\n",
	  _ntuple.nctccl);
   for (k=0; k<_ntuple.nctccl; k++) {
      AddCluster(_ntuple.Ctcstatus[k],
                 _ntuple.Ctclayer[k],
                 _ntuple.ctcrawsignal[k],
                 _ntuple.ctcsignal[k],
                 _ntuple.ctcesignal[k],
                &_ntuple.ctccoo[k][0],
                &_ntuple.ctcercoo[k][0]);
   }

   debugger.Print("AMSR_CTCClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_CTCClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSR_CTCClusterReader::AddCluster(Int_t status, Int_t plane, 
                             Float_t rawsignal, Float_t signal, Float_t ersig,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ntracks, TObjArray * tracks)
{
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
   debugger.Print("--- AMSR_CTCClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSR_CTCCluster(status, plane, 
	rawsignal, signal, ersig, coo, ercoo, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_CTCClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






