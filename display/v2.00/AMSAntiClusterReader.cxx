//  $Id: AMSAntiClusterReader.cxx,v 1.2 2001/01/22 17:32:37 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Anti Counter Cluster Reader class.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSRoot.h"
#include "AMSAntiCluster.h"
#include "AMSAntiClusterReader.h"


ClassImp(AMSAntiClusterReader)



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
AMSAntiClusterReader::AMSAntiClusterReader(const char *name, const char *title)
                 : AMSMaker(name,title)
{
   // Construct an AMSAntiClusterReader
   //

   m_Fruits     = new TClonesArray("AMSAntiCluster",8,kFALSE);
   m_BranchName = "AntiClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSAntiClusterReader::Init(TTree * h1)
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

     debugger.Print("AMSAntiClusterReader::Init(): branch address set\n");
   }
}

//_____________________________________________________________________________
void AMSAntiClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSAntiClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSAntiClusterReader::Make(): making %d clusters.\n",
	  _ntuple.nanti);
   for (k=0; k<_ntuple.nanti; k++) {
      AddCluster(_ntuple.AntiStatus[k],
                 _ntuple.AntiSector[k],
                 _ntuple.AntiSignal[k],
                &_ntuple.AntiCoo[k][0],
                &_ntuple.AntiErCoo[k][0]);
   }

   debugger.Print("AMSAntiClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSAntiClusterReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSAntiClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSAntiClusterReader::AddCluster(Int_t status, Int_t sector, 
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
   debugger.Print("--- AMSAntiClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSAntiCluster(status, sector, 
	signal,cooA, ercooA, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSAntiClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSAntiClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






