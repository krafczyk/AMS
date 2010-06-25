#include "amsvmc_RootManager.h"

#include <TParticle.h>
#include <TObjArray.h>
#include <iostream>
/// \cond CLASSIMP
ClassImp(amsvmc_RootManager)
/// \endcond

amsvmc_RootManager* amsvmc_RootManager::fgInstance = 0;

//_____________________________________________________________________________
amsvmc_RootManager::amsvmc_RootManager(const char* projectName, int fileMode)
  : TObject()
{
/// Standard constructor
/// \param projectName  The project name (passed as the Root tree name)
/// \param fileMode     Option for opening Root file (read or write mode)

  std::cout<<"DEBUG: in amsvmc_RootManager::amsvmc_RootManager(const char* projectName, int fileMode)"<<std::endl;

  if (fgInstance) {
    Fatal("amsvmc_RootManager", "Singleton instance already exists.");
    return;
  }  

  TString fileName(projectName);
  fileName += ".root";

  TString treeTitle(projectName);
  treeTitle += " tree";

  switch (fileMode) {                                  //1=read, 2-write
    case 1:
      fFile = new TFile(fileName);
      fTree = (TTree*) fFile->Get(projectName);
      break;
      
    case 2:  
      std::cout<<"DEBUG: Selected in Write mode"<<std::endl;
      fFile = new TFile(fileName, "recreate");
      std::cout<<"DEBUG: File Created"<<std::endl;
      fTree = new TTree(projectName, treeTitle);
              std::cout<<"DEBUG: tree Created"<<std::endl;
  }
  
  fPath = gDirectory->GetPath();
  fgInstance = this;
              std::cout<<"DEBUG: Finish RootManager"<<std::endl;
}

//_____________________________________________________________________________
amsvmc_RootManager::amsvmc_RootManager()
  : TObject(),
    fFile(0),
    fTree(0) 
{
/// Default constructor

  if (fgInstance) {
    Fatal("amsvmc_RootManager", "Singleton instance already exists.");
    return;
  }  

  fgInstance = this;
}

//_____________________________________________________________________________
amsvmc_RootManager::~amsvmc_RootManager() 
{
/// Destructor

  delete  fTree->GetCurrentFile();
  fgInstance = 0;
}

//
// static methods
//

//_____________________________________________________________________________
amsvmc_RootManager* amsvmc_RootManager::Instance()
{
/// \return The singleton instance.

  return fgInstance;
}  

//
// public methods
//

//_____________________________________________________________________________
void  amsvmc_RootManager::Register(const char* name, const char* className, 
                                void* objAddress)
{
/// Create a branch and associates it with the given address.
/// \param name       The branch name
/// \param className  The class name of the object
/// \param objAddress The object address

  if (!fTree->GetBranch(name)) 
    fTree->Branch(name, className, objAddress, 32000, 99);
  else  
    fTree->GetBranch(name)->SetAddress(objAddress);
}

//_____________________________________________________________________________
void  amsvmc_RootManager::Fill()
{
/// Fill the Root tree.

  fTree->Fill();
}  

//_____________________________________________________________________________
void amsvmc_RootManager:: WriteAll()
{
/// Write the Root tree in the file.

  TFile* file =  fTree->GetCurrentFile();
  file->Write();
  file->Close();
}  

//_____________________________________________________________________________
void  amsvmc_RootManager::ReadEvent(Int_t i)
{
/// Read the event data for \em i -th event for all connected branches.
/// \param i  The event to be read

  fTree->GetEntry(i);
}
