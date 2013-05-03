
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// GEANT4 tag $Name:  $
//
// 

#include "G4ios.hh"
#include <fstream>
#include <iomanip>

#include "g4strangelet.h"
#include "geant4.h"
#include "mceventg.h"

#include "G4ParticleTable.hh"

// ######################################################################
// ###                            STRANGELET                          ###
// ######################################################################
G4Strangelet* G4Strangelet::theInstance = 0;


G4Strangelet*  G4Strangelet::Definition(G4double mass, G4double charge)
{
    if (theInstance !=0) return theInstance;

    const G4String name = "strangelet";
    // search in particle table]
    G4ParticleTable* pTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* anInstance = pTable->FindParticle(name);
    if (anInstance ==0)
    {
        // create particle
        //
        //    Arguments for constructor are as follows
        //               name             mass          width         charge
        //             2*spin           parity  C-conjugation
        //          2*Isospin       2*Isospin3       G-parity
        //               type    lepton number  baryon number   PDG encoding
        //             stable         lifetime    decay table
        //             shortlived      subType    anti_encoding
        //    mass is recorded in CCFFKEY.StrMass (MCGEN 32)
        //    charge is recorded in CCFFKEY.StrCharge (MCGEN 33)
        anInstance = new G4ParticleDefinition(
                name,            mass*MeV,    0.0*MeV,      charge*eplus,
                1,              +1,             0,
                1,              +1,             0,
                "strangelet",          0,            +1,          0,
                true,               -1.0,          NULL,
                false,              "static",         0,
                0.0
                );
    }
    theInstance = reinterpret_cast<G4Strangelet*>(anInstance);
    return theInstance;
}

G4Strangelet*  G4Strangelet::StrangeletDefinition(G4double mass, G4double charge)
{
    return Definition(mass, charge);
}

G4Strangelet*  G4Strangelet::Strangelet(G4double mass, G4double charge)
{
    return Definition(mass, charge);
}






#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
extern "C" void gentrd_(float &, float &, float&, float &,int &, float[],float[]);
extern "C" void gentrdi_(float &, float &, float &,int &, float[],float[]);


G4double G4StrangeletTRDP::PostStepGetPhysicalInteractionLength( const G4Track& Track,
        G4double previousStepSize,
        G4ForceCondition* condition ){
    *condition=Forced;
    return DBL_MAX;

}

G4VParticleChange* G4StrangeletTRDP::PostStepDoIt( const G4Track& Track, const G4Step& Step ){
    pParticleChange->Initialize(Track);
    G4StepPoint * PostPoint = Step.GetPostStepPoint();
    G4StepPoint * PrePoint = Step.GetPreStepPoint();
    G4VPhysicalVolume * PostPV = PostPoint->GetPhysicalVolume();
    G4VPhysicalVolume * PrePV = PrePoint->GetPhysicalVolume();
    if(PostPV && PrePV){
        if( PostPV->GetLogicalVolume()->GetSensitiveDetector() ==AMSG4DummySD::pSD(2) && (PostPV != PrePV)){
            G4ParticleDefinition* particle =Track.GetDefinition();
            geant gamma=Track.GetTotalEnergy()/particle->GetPDGMass();
            /*
            //      Check Gamma
            if(gamma>100){
                //       Run
                integer ntr=0;
                float etr[1000],str[1000];
                float vect[7];
                geant step=Step.GetStepLength()/cm;
                geant charge=fabs(particle->GetPDGCharge());
                //       Set RNDM
                AMSmceventg::SaveSeeds(); 
                GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
                gentrdi_(charge,gamma,step,ntr,etr,str);
                // Restore RNDM;
                GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
                AMSmceventg::RestoreSeeds();
                //         cout << " invoked poststep "<<endl; 
                if (ntr){                  
                    //        Change Primary & Add Xrays
                    int i;
                    number ekin=aParticleChange.GetEnergy();
                    for(i=0;i<ntr;i++)ekin=ekin-etr[i]*GeV;
                    if(ekin<0){
                        ekin=0;
                        pParticleChange->ProposeTrackStatus(fStopButAlive);
                    }
                    aParticleChange.ProposeEnergy(ekin);
                    pParticleChange->SetNumberOfSecondaries(ntr);
                    G4StepPoint * PostPoint = Step.GetPostStepPoint();
                    G4ThreeVector dir(PostPoint->GetMomentumDirection());
                    for(i=0;i<ntr;i++){
                        G4DynamicParticle* ap =
                            new G4DynamicParticle(G4Strangelet::Strangelet(),dir);
                        ap->SetKineticEnergy(etr[i]*GeV);
                        G4Track* at =new G4Track(ap,PostPoint->GetGlobalTime(),PostPoint->GetPosition());
                        at->SetParentID(Track.GetTrackID());
                        pParticleChange->AddSecondary(at);
                    }

                }
            }*/
        }
    }
    return pParticleChange;
}

