
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Anti Counter Cluster Reader class.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_AntiCluster.h"
#include "AMSR_AntiClusterReader.h"


ClassImp(AMSR_AntiClusterReader)



//
// struct to read data from tree converted from ntuple
//
static struct {
   Int_t           nanti;
   Int_t           AntiStatus[20];
   Int_t           AntiSector[20];
   Float_t         AntiCoo[20][3];
   Float_t         AntiErCoo[20][3];
   Float_t         AntiSignal[20];
} _ntuple;


//_____________________________________________________________________________
AMSR_AntiClusterReader::AMSR_AntiClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_AntiClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_AntiCluster",8,kFALSE);
   m_BranchName = "AntiClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Init(TTree * h1)
{
//////////////////////////////////////////////////////////
//   This file is modified from automatically generated 
//   skeleton file for prmu.root (converted from prmu.new.hbk)
//////////////////////////////////////////////////////////

//
// Set branch addresses
//
   if ( h1 != 0 ) {
     h1->SetBranchAddress("nanti",&_ntuple.nanti);
     h1->SetBranchAddress("Antistatus",_ntuple.AntiStatus);
     h1->SetBranchAddress("Antisector",_ntuple.AntiSector);
     h1->SetBranchAddress("Anticoo",_ntuple.AntiCoo);
     h1->SetBranchAddress("Antiercoo",_ntuple.AntiErCoo);
     h1->SetBranchAddress("Antiedep",_ntuple.AntiSignal);

     debugger.Print("AMSR_AntiClusterReader::Init(): branch address set\n");
   }
}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_AntiClusterReader::Make(): making %d clusters.\n",
	  _ntuple.nanti);
   for (k=0; k<_ntuple.nanti; k++) {
      AddCluster(_ntuple.AntiStatus[k],
                 _ntuple.AntiSector[k],
                 _ntuple.AntiSignal[k],
                &_ntuple.AntiCoo[k][0],
                &_ntuple.AntiErCoo[k][0]);
   }

   debugger.Print("AMSR_AntiClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_AntiClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSR_AntiClusterReader::AddCluster(Int_t status, Int_t sector, 
                             Float_t signal, 
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
  // Convert to other coo system
  Float_t cooA[3];
  Float_t ercooA[3];
  cooA[0]=coo[0]*cos(coo[1]/180.*3.14159);
  cooA[1]=coo[0]*sin(coo[1]/180.*3.14159);
  cooA[2]=coo[2];
  ercooA[0]=coo[0]*cos(coo[1]/180.*3.14159)*ercoo[1]/180.;
  ercooA[1]=coo[0]*sin(coo[1]/180.*3.14159)*ercoo[1]/180.;
  ercooA[2]=ercoo[2];
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   debugger.Print("--- AMSR_AntiClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSR_AntiCluster(status, sector, 
	signal,cooA, ercooA, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






