//  $Id: AMSCTCClusterReader.cxx,v 1.2 2001/01/22 17:32:37 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster Reader class.                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSRoot.h"
#include "AMSCTCCluster.h"
#include "AMSCTCClusterReader.h"


ClassImp(AMSCTCClusterReader)



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
AMSCTCClusterReader::AMSCTCClusterReader(const char *name, const char *title)
                 : AMSMaker(name,title)
{
   // Construct an AMSCTCClusterReader
   //

   m_Fruits     = new TClonesArray("AMSCTCCluster",8,kFALSE);
   m_BranchName = "CTCClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSCTCClusterReader::Init(TTree * h1)
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

     debugger.Print("AMSCTCClusterReader::Init(): branch address set\n");
   }
}

//_____________________________________________________________________________
void AMSCTCClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSCTCClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSCTCClusterReader::Make(): making %d clusters.\n",
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

   debugger.Print("AMSCTCClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSCTCClusterReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSCTCClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSCTCClusterReader::AddCluster(Int_t status, Int_t plane, 
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
   debugger.Print("--- AMSCTCClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSCTCCluster(status, plane, 
	rawsignal, signal, ersig, coo, ercoo, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSCTCClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSCTCClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






