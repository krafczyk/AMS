#ifndef AMSVMC_ROOT_MANAGER_H
#define AMSVMC_ROOT_MANAGER_H

#include <TObject.h>
#include <TTree.h>
#include <TFile.h>
#include <TDirectory.h>

class TParticle;

/// Root file mode


/// \ingroup E02
/// \ingroup E03
/// \brief Class that takes care of Root IO
///
/// \date 05/04/2002
/// \author I. Hrivnacova; IPN, Orsay

class amsvmc_RootManager : public TObject
{
  public:
    amsvmc_RootManager(const char* projectName, int fileMode);
    amsvmc_RootManager();
    virtual ~amsvmc_RootManager();     
  
    // static access method
    static amsvmc_RootManager* Instance(); 

    // methods
    void  Register(const char* name, const char* className, void* objAddress);
    void  Fill();
    void  WriteAll();
    void  ReadEvent(Int_t i);
    

    TFile* GetFile(){return fFile;}
  private:
    // data members
    static  amsvmc_RootManager* fgInstance; ///< Singleton instance

    // data members
    TFile*  fFile; ///< Root output file
    TTree*  fTree; ///< Root output tree 
    TString fPath; ///< The path to the root file
    
    ClassDef(amsvmc_RootManager,0) // Root IO manager
};

#endif //EX02_ROOT_MANAGER_H   
   

