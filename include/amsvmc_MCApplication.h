#ifndef AMSVMC_H
#define AMSVMC_H


#include <TFile.h>
#include <TVirtualMCApplication.h>
#include <TGeoUniformMagField.h> 
#include <TGeoManager.h>
#include <TH1F.h>
#include "TMCVerbose.h"

#include "amsvmc_DetectorConstruction.h"
#include "amsvmc_PrimaryGenerator.h"
#include "amsvmc_RootManager.h"
#include "amsvmc_MagField.h"


class amsvmc_MCStack;
class amsvmc_PrimaryGenerator;

class amsvmc_MCApplication : public TVirtualMCApplication
{
  public:
  amsvmc_MCApplication(const char* name,  const char *title, int fileMode = 2);
  amsvmc_MCApplication();
  virtual ~amsvmc_MCApplication();
  
    // static access method
    static amsvmc_MCApplication* Instance(); 

    // methods
    void InitMC();
    void RunMC(Int_t nofEvents);
    void FinishRun();
    void ReadEvent(Int_t i);
 
    virtual void ConstructGeometry();
    virtual void ConstructOpGeometry();
    virtual void InitGeometry();
    virtual void PreInit();
    virtual void AddParticles();
    virtual void AddIons();
    //    virtual void DefineParticles();
    virtual void GeneratePrimaries();
    virtual void BeginEvent();
    virtual void BeginPrimary();
    virtual void PreTrack();
    virtual void Stepping();
    virtual void PostTrack();
    virtual void FinishPrimary();
    virtual void FinishEvent();
    
    // set methods
    void  SetPrintModulo(Int_t value);
    void  SetVerboseLevel(Int_t verboseLevel);
    void  SetField(Double_t field);



    // get methods
    amsvmc_DetectorConstruction* GetDetectorConstruction() const;

    amsvmc_PrimaryGenerator*     GetPrimaryGenerator() const;

 
  private:
    // methods
    void RegisterStack();
  
    // data members
    Int_t                     fPrintModulo;     ///< The event modulus number to be printed 
    Int_t                     fEventNo;         ///< Event counter
    Int_t                     fOptPhotonNo;   //Optical photon number
    float                     fECALedep;
    Int_t                       CurrentLevel;
    TGeoMedium*               CurrentMed;
    TGeoMaterial*             CurrentMat;
    Int_t                       vmc_isvol;
    Int_t                     vmc_nstep;
    Int_t                     vmc_istop;
    Int_t                     vmc_version;
   
    TGeoVolume*               CurrentVol;

    TGeoNode*             MotherVol;
    char*                 MotherVolName;
    int                   mothercopyNo;
    int                   copyNo;


    TGeoNode*             GrandMotherVol;
    char*                 GrandMotherVolName;
	   

    TH1F *			h_edep_ionization;
    TH1F *			h_edep_ionization2;
    TH1F *			h_edep_absorption;
    TH1F *			h_edep_absorption2;




    TMCVerbose                fVerbose;         ///< VMC verbose helper
    amsvmc_MCStack*              fStack;           ///< VMC stack */
    amsvmc_DetectorConstruction* fDetConstruction; ///< Dector construction */
    amsvmc_PrimaryGenerator*     fPrimaryGenerator;///< Primary generator */
    amsvmc_RootManager           fRootManager;     ///< Root manager
    amsvmc_MagField*      fMagField;        ///< Magnetic field 


  ClassDef(amsvmc_MCApplication,1)  //Interface to MonteCarlo application
};

// inline functions

/// \return The singleton instance 
inline amsvmc_MCApplication* amsvmc_MCApplication::Instance()
{ return (amsvmc_MCApplication*)(TVirtualMCApplication::Instance()); }

/// Set the event modulus number to be printed 
/// \param value  The new event modulus number value
inline void  amsvmc_MCApplication::SetPrintModulo(Int_t value)  
{ fPrintModulo = value; }

/// Set verbosity 
/// \param verboseLevel  The new verbose level value
inline void  amsvmc_MCApplication::SetVerboseLevel(Int_t verboseLevel)
{ fVerbose.SetLevel(verboseLevel); }

// Set magnetic field
/// \param bz  The new field value in z
inline void  amsvmc_MCApplication::SetField(Double_t bz)
{// fMagField->SetFieldValue(0., 0., bz); 
}
/// \return The detector construction
inline amsvmc_DetectorConstruction* amsvmc_MCApplication::GetDetectorConstruction() const
{ return fDetConstruction; }

/// \return The calorimeter sensitive detector

/// \return The primary generator
inline amsvmc_PrimaryGenerator* amsvmc_MCApplication::GetPrimaryGenerator() const
{ 
  cout<<"DEBUG:GetPrimaryGenerator()"<<endl;
return fPrimaryGenerator; }


#endif 

