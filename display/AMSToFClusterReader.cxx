
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster Reader class.                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <iostream.h>
#include <TRandom.h>

#include <TClonesArray.h>
#include <TMCParticle.h>

#include "AMSRoot.h"
#include "AMSToFCluster.h"
#include "AMSToFClusterReader.h"

/*
#include "ATLFMCMaker.h"

const Float_t kPI  = TMath::Pi();
const Float_t k2PI = 2*kPI;
const Int_t kMAXCELLS = 2000;
const Int_t kMAXCLU   = 1000;
*/

ClassImp(AMSToFClusterReader)



//
// struct to read data from tree converted from ntuple
//
static struct {
  Int_t           NToF;
  Int_t           Status[60];
  Int_t           Plane[60];
  Int_t           Bar[60];
  Float_t         Edep[60];
  Float_t         Time[60];
  Float_t         ErTime[60];
  Float_t         Coo[60][3];
  Float_t         ErCoo[60][3];
} _ntuple;

//_____________________________________________________________________________
AMSToFClusterReader::AMSToFClusterReader(const char *name, const char *title)
                 : AMSMaker(name,title)
{
   // Construct an AMSToFClusterReader
   //

   m_Fruits     = new TClonesArray("AMSToFCluster",8,kFALSE);
   m_BranchName = "ToFClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSToFClusterReader::Init(TTree * h1)
{
//////////////////////////////////////////////////////////
//   This file is modified from automatically generated 
//   skeleton file for prmu.root (converted from prmu.new.hbk)
//////////////////////////////////////////////////////////

   debugger.Print("Init AMSToFClusterReader from TTree at %x\n", h1);

//
// Set branch addresses
//
   if ( h1 != 0 ) {
     debugger.Print("setting branch address\n");
     h1->SetBranchAddress("ntof",     &_ntuple.NToF);
     h1->SetBranchAddress("Tofstatus", _ntuple.Status);
     h1->SetBranchAddress("plane",     _ntuple.Plane);
     h1->SetBranchAddress("bar",       _ntuple.Bar);
     h1->SetBranchAddress("Tofedep",   _ntuple.Edep);
     h1->SetBranchAddress("Toftime",   _ntuple.Time);
     h1->SetBranchAddress("Tofetime",  _ntuple.ErTime);
     h1->SetBranchAddress("Tofcoo",    _ntuple.Coo);
     h1->SetBranchAddress("Tofercoo",  _ntuple.ErCoo);
     debugger.Print("finished\n");
   }

}

//_____________________________________________________________________________
void AMSToFClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSToFClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSToFClusterReader::Make(): making %d clusters.\n",
	  _ntuple.NToF);
   for (k=0; k<_ntuple.NToF; k++) {
      AddCluster(_ntuple.Status[k],
                 _ntuple.Plane[k],
                 _ntuple.Bar[k],
                 _ntuple.Edep[k],
                 _ntuple.Time[k],
                 _ntuple.ErTime[k],
                &_ntuple.Coo[k][0],
                &_ntuple.ErCoo[k][0]);
   }

   debugger.Print("AMSToFClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSToFClusterReader::PrintInfo()
{
   AMSMaker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSToFClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among ToF clusters
}
*/


//_____________________________________________________________________________
void  AMSToFClusterReader::AddCluster(Int_t status, Int_t plane, Int_t bar,
                                Float_t energy, Float_t time, Float_t ertime,
                                Float_t * coo, Float_t * ercoo,
                                Int_t ncells, Int_t npart,
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

  //cout << "ene "<<energy<<endl;
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   new(clusters[m_Nclusters++]) AMSToFCluster(status, plane, bar, 
	energy, time, ertime, coo, ercoo, ncells, npart, ntracks, tracks);
}

//_____________________________________________________________________________
void AMSToFClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSToFClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






