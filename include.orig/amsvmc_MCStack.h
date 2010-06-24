#ifndef AMSVMC_STACK_H
#define AMSVMC_STACK_H

#include <TVirtualMCStack.h>

#include <stack>

class TParticle;
class TClonesArray;

/// \ingroup E03
/// \brief Implementation of the TVirtualMCStack interface
///
/// \date 06/03/2003
/// \author I. Hrivnacova; IPN, Orsay

class amsvmc_MCStack : public TVirtualMCStack
{
  public:
    amsvmc_MCStack(Int_t size);
    amsvmc_MCStack();
    virtual ~amsvmc_MCStack();     

    // methods
    virtual void  PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
  	              Double_t px, Double_t py, Double_t pz, Double_t e,
  		      Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		      Double_t polx, Double_t poly, Double_t polz,
		      TMCProcess mech, Int_t& ntr, Double_t weight,
		      Int_t is) ;
    virtual TParticle* PopNextTrack(Int_t& track);
    virtual TParticle* PopPrimaryForTracking(Int_t i); 
    virtual void Print(Option_t* option = "") const;   
    void Reset();   
   
    // set methods
    virtual void  SetCurrentTrack(Int_t track);                           

    // get methods
    virtual Int_t  GetNtrack() const;
    virtual Int_t  GetNprimary() const;
    virtual TParticle* GetCurrentTrack() const;   
    virtual Int_t  GetCurrentTrackNumber() const;
    virtual Int_t  GetCurrentParentTrackNumber() const;
    TParticle*     GetParticle(Int_t id) const;
    
  private:
    // data members
    std::stack<TParticle*>  fStack;       //!< The stack of particles (transient)
    TClonesArray*           fParticles;   ///< The array of particle (persistent)
    Int_t                   fCurrentTrack;///< The current track number
    Int_t                   fNPrimary;    ///< The number of primaries
    
    ClassDef(amsvmc_MCStack,1) // Ex03MCStack
};

#endif //EX03_STACK_H   
   

