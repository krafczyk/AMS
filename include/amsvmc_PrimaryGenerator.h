#ifndef AMSVMC_PRIMARY_GENERATOR_H
#define AMSVMC_PRIMARY_GENERATOR_H

#include <TVirtualMCApplication.h>
#include <iostream>
class TVirtualMCStack;
class TVector3;

class amsvmc_DetectorConstruction;

/// \ingroup E03
/// \brief The primary generator
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class amsvmc_PrimaryGenerator : public TObject
{
  public:
    amsvmc_PrimaryGenerator(TVirtualMCStack* stack); 
    amsvmc_PrimaryGenerator();
    virtual ~amsvmc_PrimaryGenerator();

    // methods
    virtual void GeneratePrimaries(int vmc_version);

    // set methods
    void  SetIsRandom(Bool_t isRandomGenerator);
    void  SetUserParticles(Bool_t userParticles);
    void  SetUserDecay(Bool_t userDecay);
    void  SetNofPrimaries(Int_t nofPrimaries);

    // get methods
    Bool_t GetUserDecay() const;
 
  private:
    // methods
    void GeneratePrimary1(int vmc_version);
    void GeneratePrimary2(const TVector3& origin);
    void GeneratePrimary3(const TVector3& origin);
 
    // data members
    TVirtualMCStack*  fStack;         ///< VMC stack
    Bool_t            fIsRandom;      ///< Switch to random generator
    Bool_t            fUserParticles; ///< Switch to user particles
    Bool_t            fUserDecay;     ///< Switch to particle with user decay
    Int_t             fNofPrimaries;  ///< Number of primary particles

  ClassDef(amsvmc_PrimaryGenerator,1)  //amsvmc_PrimaryGenerator
};

// inline functions

/// Switch on/off the random generator
/// \param isRandom  If true, the position is randomized
inline void  amsvmc_PrimaryGenerator::SetIsRandom(Bool_t isRandom)
{ fIsRandom = isRandom; }

/// Set the number of particles to be generated
/// \param nofPrimaries The number of particles to be generated
inline void  amsvmc_PrimaryGenerator::SetNofPrimaries(Int_t nofPrimaries)
{
  //  std::cout<<"DEBUG:SetNofPrimaries"<<std::endl;
  //  std::cout<<"DEBUG:before SetNofPrimaries, fNofPrimaries ="<<fNofPrimaries<<std::endl;

  //  fNofPrimaries = nofPrimaries; 

  //    std::cout<<"DEBUG:before SetNofPrimaries, nofPrimaries ="<< fNofPrimaries<<std::endl;

}

/// Return true if particle with user decay is activated
inline Bool_t amsvmc_PrimaryGenerator::GetUserDecay() const
{ return fUserDecay; }

#endif //EX03_PRIMARY_GENERATOR_H

