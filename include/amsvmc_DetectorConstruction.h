#ifndef AMS_DETECTOR_CONSTRUCTION_H
#define AMS_DETECTOR_CONSTRUCTION_H

#include <map>

#include <Riostream.h>
#include <TObject.h>
#include <TString.h>


class amsvmc_DetectorConstruction : public TObject
{
  /// The map of string names to integers
  typedef map<TString, Int_t>  NameMap; 
  /// The iterator for the map of string names to integers
  typedef NameMap::const_iterator   NameMapIterator; 

  public:  
    amsvmc_DetectorConstruction();
    virtual ~amsvmc_DetectorConstruction();

  public:
     void ConstructMaterials();
     void ConstructGeometry();
     void SetCuts();
     void PrintCalorParameters();
     //void UpdateGeometry();
     
     // set methods
     void SetNbOfLayers (Int_t value);
     void SetDefaultMaterial(const TString& materialName);
     void SetAbsorberMaterial(const TString& materialName);
     void SetGapMaterial(const TString& materialName );
     void SetCalorSizeYZ(Double_t value);
     void SetAbsorberThickness(Double_t value);
     void SetGapThickness(Double_t value);
     
     //
     // get methods

     /// \return The number of calorimeter layers
     Int_t    GetNbOfLayers() const    { return fNbOfLayers; }

     /// \return The world size x component
     Double_t GetWorldSizeX() const    { return fWorldSizeX; } 

     /// \return The world size y,z component
     Double_t GetWorldSizeYZ() const   { return fWorldSizeYZ; }

     /// \return The calorimeter size y,z component
     Double_t GetCalorSizeYZ() const   { return fCalorSizeYZ; }

     /// \return The calorimeter thickness
     Double_t GetCalorThickness()const { return fCalorThickness; } 

     /// \return The absorber thickness
     Double_t GetAbsorberThickness()const { return fAbsorberThickness; }      

     /// \return The gap thickness
     Double_t GetGapThickness()const   { return fGapThickness; }
          
  private:      
     // methods
     void  ComputeCalorParameters();

     // data members  
     Int_t     fNbOfLayers;       ///< The number of calorimeter layers
     Double_t  fWorldSizeX;       ///< The world size x component
     Double_t  fWorldSizeYZ;      ///< The world size y,z component
     Double_t  fCalorSizeYZ;      ///< The calorimeter size y,z component
     Double_t  fCalorThickness;   ///< The calorimeter thickness
     Double_t  fLayerThickness;   ///< The calorimeter layer thickness
     Double_t  fAbsorberThickness;///< The absorber thickness
     Double_t  fGapThickness;     ///< The gap thickness
     
     TString   fDefaultMaterial;  ///< The default material name
     TString   fAbsorberMaterial; ///< The absorber material name
     TString   fGapMaterial;      ///< The gap material name
     
  ClassDef(amsvmc_DetectorConstruction,1) //Ex03DetectorConstruction
};

#endif //EX03_DETECTOR_CONSTRUCTION_H
