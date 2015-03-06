#include "G4Version.hh"
#if G4VERSION_NUMBER  > 945  && G4VERSION_NUMBER  <1000
#define __G4PROTONBUG962only__
#endif
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
// Hadronic Process: Nuclear De-excitations
// by V. Lara (May 1998)
//
//
// Modified:
// 30 June 1998 by V. Lara:
//      -Modified the Transform method for use G4ParticleTable and 
//       therefore G4IonTable. It makes possible to convert all kind 
//       of fragments (G4Fragment) produced in deexcitation to 
//       G4DynamicParticle
//      -It uses default algorithms for:
//              Evaporation: G4Evaporation
//              MultiFragmentation: G4StatMF 
//              Fermi Breakup model: G4FermiBreakUp
// 24 Jul 2008 by M. A. Cortes Giraldo:
//      -Max Z,A for Fermi Break-Up turns to 9,17 by default
//      -BreakItUp() reorganised and bug in Evaporation loop fixed
//      -Transform() optimised
// (September 2008) by J. M. Quesada. External choices have been added for :
//      -inverse cross section option (default OPTxs=3)
//      -superimposed Coulomb barrier (if useSICB is set true, by default it is false) 
// September 2009 by J. M. Quesada: 
//      -according to Igor Pshenichnov, SMM will be applied (just in case) only once.
// 27 Nov 2009 by V.Ivanchenko: 
//      -cleanup the logic, reduce number internal vectors, fixed memory leak.
// 11 May 2010 by V.Ivanchenko: 
//      -FermiBreakUp activated, used integer Z and A, used BreakUpFragment method for 
//       final photon deexcitation; used check on adundance of a fragment, decay 
//       unstable fragments with A <5
// 22 March 2011 by V.Ivanchenko: general cleanup and addition of a condition: 
//       products of Fermi Break Up cannot be further deexcited by this model 
// 30 March 2011 by V.Ivanchenko removed private inline methods, moved Set methods 
//       to the source
// 23 January 2012 by V.Ivanchenko general cleanup including destruction of 
//    objects, propagate G4PhotonEvaporation pointer to G4Evaporation class and 
//    not delete it here 
#ifdef __G4PROTONBUG__

#include <list>

#include "G4ExcitationHandler.hh"
#include "G4SystemOfUnits.hh"
#include "G4LorentzVector.hh"
#include "G4NistManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"

#include "G4VMultiFragmentation.hh"
#include "G4VFermiBreakUp.hh"
#include "G4VFermiFragment.hh"

#include "G4VEvaporation.hh"
#include "G4VEvaporationChannel.hh"
#include "G4VPhotonEvaporation.hh"
#include "G4Evaporation.hh"
#include "G4StatMF.hh"
#include "G4PhotonEvaporation.hh"
#include "G4FermiBreakUp.hh"
#include "G4FermiFragmentsPool.hh"
G4ExcitationHandler::G4ExcitationHandler():
  maxZForFermiBreakUp(9),maxAForFermiBreakUp(17),minEForMultiFrag(4*GeV),
  minExcitation(keV),OPTxs(3),useSICB(false),isEvapLocal(true)
{                                                                          
  theTableOfIons = G4ParticleTable::GetParticleTable()->GetIonTable();
  
  theMultiFragmentation = new G4StatMF;
  theFermiModel = new G4FermiBreakUp;
  thePhotonEvaporation = new G4PhotonEvaporation;
  theEvaporation = new G4Evaporation(thePhotonEvaporation);
  thePool = G4FermiFragmentsPool::Instance();
  SetParameters();
}

G4ExcitationHandler::~G4ExcitationHandler()
{
  if(isEvapLocal) { delete theEvaporation; }
  delete theMultiFragmentation;
  delete theFermiModel;
}

void G4ExcitationHandler::SetParameters()
{
  //for inverse cross section choice
  theEvaporation->SetOPTxs(OPTxs);
  //for the choice of superimposed Coulomb Barrier for inverse cross sections
  theEvaporation->UseSICB(useSICB);
  theEvaporation->Initialise();
}

G4ReactionProductVector * 
G4ExcitationHandler::BreakItUp(const G4Fragment & theInitialState) const
{	
  //G4cout << "@@@@@@@@@@ Start G4Excitation Handler @@@@@@@@@@@@@" << G4endl;
  
  // Variables existing until end of method
  G4Fragment * theInitialStatePtr = new G4Fragment(theInitialState);

  G4FragmentVector * theTempResult = 0;      // pointer which receives temporal results
  std::list<G4Fragment*> theEvapList;        // list to apply Evaporation or Fermi Break-Up
  std::list<G4Fragment*> thePhotoEvapList;   // list to apply PhotonEvaporation
  std::list<G4Fragment*> theResults;         // list to store final result
  //
  //G4cout << theInitialState << G4endl;  
  
  // Variables to describe the excited configuration
  G4double exEnergy = theInitialState.GetExcitationEnergy();
  G4int A = theInitialState.GetA_asInt();
  G4int Z = theInitialState.GetZ_asInt();

  G4NistManager* nist = G4NistManager::Instance();
  
  // In case A <= 1 the fragment will not perform any nucleon emission
  if (A <= 1)
    {
      theResults.push_back( theInitialStatePtr );
    }
  // check if a fragment is stable
  else if(exEnergy < minExcitation && nist->GetIsotopeAbundance(Z, A) > 0.0)
    {
      theResults.push_back( theInitialStatePtr );
    }  
  else  
    {      
      // JMQ 150909: first step in de-excitation is treated separately 
      // Fragments after the first step are stored in theEvapList 
      // Statistical Multifragmentation will take place only once
      //
      // move to evaporation loop
      if((A<maxAForFermiBreakUp && Z<maxZForFermiBreakUp) 
	 || exEnergy <= minEForMultiFrag*A) 
	{ 
	  theEvapList.push_back(theInitialStatePtr); 
	}
      else  
        {
          theTempResult = theMultiFragmentation->BreakItUp(theInitialState);
	  if(!theTempResult) { theEvapList.push_back(theInitialStatePtr); }
	  else {
	    size_t nsec = theTempResult->size();
	    if(0 == nsec) { theEvapList.push_back(theInitialStatePtr); }
	    else {
	      // secondary are produced
	      // Sort out secondary fragments
	      G4bool deletePrimary = true;
	      G4FragmentVector::iterator j;
	      for (j = theTempResult->begin(); j != theTempResult->end(); ++j) {  
		if((*j) == theInitialStatePtr) { deletePrimary = false; }
		A = (*j)->GetA_asInt();  

		// gamma, p, n
		if(A <= 1) { theResults.push_back(*j); }

		// Analyse fragment A > 1
		else {
		  G4double exEnergy1 = (*j)->GetExcitationEnergy();

		  // cold fragments
		  if(exEnergy1 < minExcitation) {
		    Z = (*j)->GetZ_asInt(); 
		    if(nist->GetIsotopeAbundance(Z, A) > 0.0) { 
		      theResults.push_back(*j); // stable fragment 

		    } else {

		      // check if the cold fragment is from FBU pool
		      const G4VFermiFragment* ffrag = thePool->GetFragment(Z, A);
		      if(ffrag) {
			if(ffrag->IsStable()) { theResults.push_back(*j); }
			else                  { theEvapList.push_back(*j); }

			// cold fragment may be unstable
		      } else {
			theEvapList.push_back(*j); 
		      }
		    }

		    // hot fragments are unstable
		  } else { theEvapList.push_back(*j); } 
		}
	      }
	      if( deletePrimary ) { delete theInitialStatePtr; }
	    }
	    delete theTempResult;
	  }
	}
    }
  /*
  G4cout << "## After first step " << theEvapList.size() << " for evap;  "
   << thePhotoEvapList.size() << " for photo-evap; " 
   << theResults.size() << " results. " << G4endl; 
  */
  // -----------------------------------
  // FermiBreakUp and De-excitation loop
  // -----------------------------------
      
  std::list<G4Fragment*>::iterator iList;
  for (iList = theEvapList.begin(); iList != theEvapList.end(); ++iList)
    {
      //G4cout << "Next evaporate: " << G4endl;  
      //G4cout << *iList << G4endl;  
      A = (*iList)->GetA_asInt(); 
      Z = (*iList)->GetZ_asInt();
	  
      // Fermi Break-Up 
      G4bool wasFBU = false;
      if (A < maxAForFermiBreakUp && Z < maxZForFermiBreakUp) 
	{
	  theTempResult = theFermiModel->BreakItUp(*(*iList));
	  wasFBU = true; 
	  // if initial fragment returned unchanged try to evaporate it
          if(1 == theTempResult->size()) {
            for(int k=0;k<theTempResult->size();k++)delete (*theTempResult)[k];
            delete theTempResult;
	    theTempResult = theEvaporation->BreakItUp(*(*iList)); 
	  }
	}
      else // apply Evaporation in another case
	{
	  theTempResult = theEvaporation->BreakItUp(*(*iList));
	}
      
      G4bool deletePrimary = true;
      size_t nsec = theTempResult->size();
      //G4cout << "Nproducts= " << nsec << G4endl;  
		  
      // Sort out secondary fragments
      if ( nsec > 0 ) {
	G4FragmentVector::iterator j;
	for (j = theTempResult->begin(); j != theTempResult->end(); ++j) {
	  if((*j) == (*iList)) { deletePrimary = false; }

	  //G4cout << *j << G4endl;  
	  A = (*j)->GetA_asInt();
	  exEnergy = (*j)->GetExcitationEnergy();

	  if(A <= 1) { theResults.push_back(*j); }    // gamma, p, n

	  // evaporation is not possible
	  else if(1 == nsec) { 
	    if(exEnergy < minExcitation) { theResults.push_back(*j); }
	    else                         { thePhotoEvapList.push_back(*j); }

	  } else { // Analyse fragment

	    // cold fragment
	    if(exEnergy < minExcitation) {
	      Z = (*j)->GetZ_asInt();

	      // natural isotope
	      if(nist->GetIsotopeAbundance(Z, A) > 0.0) { 
		theResults.push_back(*j); // stable fragment 

	      } else {
		const G4VFermiFragment* ffrag = thePool->GetFragment(Z, A);

		// isotope from FBU pool
		if(ffrag) {
		  if(ffrag->IsStable()) { theResults.push_back(*j); }
		  else                  { theEvapList.push_back(*j); }

		  // isotope may be unstable
		} else {
		  theEvapList.push_back(*j);
		}   
	      }

	      // hot fragment
	    } else if (wasFBU) { 
	      thePhotoEvapList.push_back(*j); // FBU applied only once 
	    } else {  
	      theEvapList.push_back(*j);        
	    }
	  }
	}
      }
      if( deletePrimary ) { delete (*iList); }
      delete theTempResult;
    } // end of the loop over theEvapList

  //G4cout << "## After 2nd step " << theEvapList.size() << " was evap;  "
  // << thePhotoEvapList.size() << " for photo-evap; " 
  // << theResults.size() << " results. " << G4endl; 
      
  // -----------------------
  // Photon-Evaporation loop
  // -----------------------
  
  // at this point only photon evaporation is possible
  for(iList = thePhotoEvapList.begin(); iList != thePhotoEvapList.end(); ++iList)
    {
      //G4cout << "Next photon evaporate: " << thePhotonEvaporation << G4endl;  
      //G4cout << *iList << G4endl;
      exEnergy = (*iList)->GetExcitationEnergy();

      // only hot fragments
      if(exEnergy >= minExcitation) {  
	theTempResult = thePhotonEvaporation->BreakUpFragment(*iList);	  
	size_t nsec = theTempResult->size();
	//G4cout << "Nproducts= " << nsec << G4endl;  
	  
	// if there is a gamma emission then
	if (nsec > 0)
	  {
	    G4FragmentVector::iterator j;
	    for (j = theTempResult->begin(); j != theTempResult->end(); ++j)
	      {
		theResults.push_back(*j); 
	      }
	  }
  	delete theTempResult;
      }

      // priamry fragment is kept
      theResults.push_back(*iList); 

    } // end of photon-evaporation loop

  //G4cout << "## After 3d step " << theEvapList.size() << " was evap;  "
  //	 << thePhotoEvapList.size() << " was photo-evap; " 
  //	 << theResults.size() << " results. " << G4endl; 
    
  G4ReactionProductVector * theReactionProductVector = new G4ReactionProductVector;

  // MAC (24/07/08)
  // To optimise the storing speed, we reserve space in memory for the vector
  theReactionProductVector->reserve( theResults.size() );

  G4int theFragmentA, theFragmentZ;

  std::list<G4Fragment*>::iterator i;
  for (i = theResults.begin(); i != theResults.end(); ++i) 
    {
      theFragmentA = (*i)->GetA_asInt();
      theFragmentZ = (*i)->GetZ_asInt();
      G4ParticleDefinition* theKindOfFragment = 0;
      if (theFragmentA == 0) {       // photon or e-
	theKindOfFragment = (*i)->GetParticleDefinition();   
      } else if (theFragmentA == 1 && theFragmentZ == 0) { // neutron
	theKindOfFragment = G4Neutron::NeutronDefinition();
      } else if (theFragmentA == 1 && theFragmentZ == 1) { // proton
	theKindOfFragment = G4Proton::ProtonDefinition();
      } else if (theFragmentA == 2 && theFragmentZ == 1) { // deuteron
	theKindOfFragment = G4Deuteron::DeuteronDefinition();
      } else if (theFragmentA == 3 && theFragmentZ == 1) { // triton
	theKindOfFragment = G4Triton::TritonDefinition();
      } else if (theFragmentA == 3 && theFragmentZ == 2) { // helium3
	theKindOfFragment = G4He3::He3Definition();
      } else if (theFragmentA == 4 && theFragmentZ == 2) { // alpha
	theKindOfFragment = G4Alpha::AlphaDefinition();;
      } else {
	theKindOfFragment = 
	  theTableOfIons->GetIon(theFragmentZ,theFragmentA,0.0);
      }
      if (theKindOfFragment != 0) 
	{
	  G4ReactionProduct * theNew = new G4ReactionProduct(theKindOfFragment);
	  theNew->SetMomentum((*i)->GetMomentum().vect());
	  theNew->SetTotalEnergy((*i)->GetMomentum().e());
	  theNew->SetFormationTime((*i)->GetCreationTime());
	  theReactionProductVector->push_back(theNew);
	}
      delete (*i);
    }

  return theReactionProductVector;
}

void G4ExcitationHandler::SetEvaporation(G4VEvaporation* ptr)
{
  if(ptr && ptr != theEvaporation) {
    delete theEvaporation; 
    theEvaporation = ptr;
    thePhotonEvaporation = ptr->GetPhotonEvaporation();
    SetParameters();
    isEvapLocal = false;
  }
}

void 
G4ExcitationHandler::SetMultiFragmentation(G4VMultiFragmentation* ptr)
{
  if(ptr && ptr != theMultiFragmentation) {
    delete theMultiFragmentation;
    theMultiFragmentation = ptr;
  }
}

void G4ExcitationHandler::SetFermiModel(G4VFermiBreakUp* ptr)
{
  if(ptr && ptr != theFermiModel) {
    delete theFermiModel;
    theFermiModel = ptr;
  }
}

void 
G4ExcitationHandler::SetPhotonEvaporation(G4VEvaporationChannel* ptr)
{
  if(ptr && ptr != thePhotonEvaporation) {
    thePhotonEvaporation = ptr;
    theEvaporation->SetPhotonEvaporation(ptr);
  }
}

void G4ExcitationHandler::SetMaxZForFermiBreakUp(G4int aZ)
{
  maxZForFermiBreakUp = aZ;
}

void G4ExcitationHandler::SetMaxAForFermiBreakUp(G4int anA)
{
  maxAForFermiBreakUp = std::min(5,anA);
}

void G4ExcitationHandler::SetMaxAandZForFermiBreakUp(G4int anA, G4int aZ)
{
  SetMaxAForFermiBreakUp(anA);
  SetMaxZForFermiBreakUp(aZ);
}

void G4ExcitationHandler::SetMinEForMultiFrag(G4double anE)
{
  minEForMultiFrag = anE;
}
#include "G4SubtractionSolid.hh"
G4double  G4SubtractionSolid::DistanceToIn( const G4ThreeVector& p ) const 
{
  G4double dist=0.0;

  if( Inside(p) == kInside )
  {
    G4cout << "WARNING - Invalid call in "
           << "G4SubtractionSolid::DistanceToIn(p)" << G4endl
           << "  Point p is inside !" << G4endl;
    G4cout << "          p = " << p << G4endl;
    G4cerr << "WARNING - Invalid call in "
           << "G4SubtractionSolid::DistanceToIn(p)" << G4endl
           << "  Point p is inside !" << G4endl;
    G4cerr << "          p = " << p << G4endl;
    G4cerr<< fPtrSolidA->GetName()<<" "<< fPtrSolidB->GetName()<<std::endl;
  }

  if( ( fPtrSolidA->Inside(p) != kOutside) &&   // case 1
      ( fPtrSolidB->Inside(p) != kOutside)    )
  {
      dist= fPtrSolidB->DistanceToOut(p)  ;
  }
  else
  {
      dist= fPtrSolidA->DistanceToIn(p) ; 
  }
  
  return dist; 
}


#endif
#if G4VERSION_NUMBER  < 946 

#include "G4EMDissociation.hh"
G4EMDissociation::~G4EMDissociation ()
{
  if (handlerDefinedInternally) delete theExcitationHandler;
  delete thePhotonSpectrum;
}
/*
  #include "G4IonProtonCrossSection.hh"

G4bool G4IonProtonCrossSection::IsIsoApplicable(const G4DynamicParticle* dp,
                                         G4int Z, G4int A)
{
  G4bool result = false;
  if(Z < 2 && A < 2 && dp->GetDefinition()->GetPDGCharge()/eplus > 1.5)
    { result = true;}
  return result;
}

*/
#endif
#if G4VERSION_NUMBER  > 945 && G4VERSION_NUMBER  <1000 
#include "G4HadronElasticPhysics.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronElastic.hh"
#include "G4CHIPSElastic.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4AntiNuclElastic.hh"

#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"
#include "G4NeutronElasticXS.hh"

#include "G4CrossSectionDataSetRegistry.hh"

#include "G4ChipsProtonElasticXS.hh"
#include "G4ChipsNeutronElasticXS.hh"

#include "G4ComponentAntiNuclNuclearXS.hh"  
#include "G4CrossSectionElastic.hh"
#include "G4ComponentGGNuclNuclXsc.hh"
#include "commons.h"
void G4HadronElasticPhysics::ConstructProcess()
{
  if(wasActivated) { return; }
  wasActivated = true;

  G4double elimitPi = 1.0*GeV;
  G4double elimitAntiNuc = 100*MeV;
  if(verbose > 1) {
    G4cout << "### HadronElasticPhysics Construct Processes with the limit for pi " 
	   << elimitPi/GeV << " GeV" 
	   << "                                                  for anti-neuclei " 
	   << elimitAntiNuc/GeV << " GeV"	   << G4endl;
  }

  G4AntiNuclElastic* anuc = new G4AntiNuclElastic();
  anuc->SetMinEnergy(elimitAntiNuc);
  G4CrossSectionElastic* anucxs = 
    new G4CrossSectionElastic(anuc->GetComponentCrossSection());

  G4HadronElastic* lhep0 = new G4HadronElastic();
  G4HadronElastic* lhep1 = new G4HadronElastic();
  G4HadronElastic* lhep2 = new G4HadronElastic();
  lhep1->SetMaxEnergy(elimitPi);
  lhep2->SetMaxEnergy(elimitAntiNuc);

  G4CHIPSElastic* chipsp = new G4CHIPSElastic();
  neutronModel = new G4CHIPSElastic();

  G4ElasticHadrNucleusHE* he = new G4ElasticHadrNucleusHE(); 
  he->SetMinEnergy(elimitPi);



//  G4HadronElastic* lhep3 = new G4HadronElastic();
//  G4CrossSectionElastic* nucxs = 
//    new G4CrossSectionElastic(new G4ComponentGGNuclNuclXsc());



  theParticleIterator->reset();
  while( (*theParticleIterator)() )
  {
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String pname = particle->GetParticleName();
    if(pname == "anti_lambda"  ||
       pname == "anti_neutron" ||
       pname == "anti_omega-"  || 
       pname == "anti_sigma-"  || 
       pname == "anti_sigma+"  || 
       pname == "anti_xi-"  || 
       pname == "anti_xi0"  || 
       pname == "lambda"    || 
       pname == "omega-"    || 
       pname == "sigma-"    || 
       pname == "sigma+"    || 
       pname == "xi-"        
       ) {
      
      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->RegisterMe(lhep0);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

     }else if(pname == "proton") {   

      G4HadronElasticProcess* hel = new G4HadronElasticProcess("protonelastic");
      //hel->AddDataSet(new G4BGGNucleonElasticXS(particle));
      if(G4FFKEY.HCrossSectionBias[1]!=1){
         hel->aScaleFactor=G4FFKEY.HCrossSectionBias[1];
         cout<<"G4HadronElasticProcess-I-ProtonElasticCrossSectionScaledBy "<< hel->aScaleFactor<<endl;
      }
      //      hel->AddDataSet(new G4ChipsProtonElasticXS());
      hel->AddDataSet(G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsProtonElasticXS::Default_Name()));
     
      hel->RegisterMe(chipsp);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(pname == "neutron") {   

      neutronProcess = new G4HadronElasticProcess();
      //neutronProcess->AddDataSet(new G4BGGNucleonElasticXS(particle));
      neutronProcess->AddDataSet(G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet(G4ChipsNeutronElasticXS::Default_Name()));
      neutronProcess->RegisterMe(neutronModel);
      pmanager->AddDiscreteProcess(neutronProcess);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " 
	       << neutronProcess->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if (pname == "pi+" || pname == "pi-") { 

      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->AddDataSet(new G4BGGPionElasticXS(particle));
      hel->RegisterMe(lhep1);
      hel->RegisterMe(he);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(pname == "kaon-"     || 
	      pname == "kaon+"     || 
	      pname == "kaon0S"    || 
	      pname == "kaon0L" 
	      ) {
      
      G4HadronElasticProcess* hel = new G4HadronElasticProcess();
      hel->RegisterMe(lhep0);
      pmanager->AddDiscreteProcess(hel);
      if(verbose > 1) {
	G4cout << "### HadronElasticPhysics: " << hel->GetProcessName()
	       << " added for " << particle->GetParticleName() << G4endl;
      }

    } else if(
       pname == "anti_proton"    || 
       pname == "anti_alpha"     ||
       pname == "anti_deuteron"  ||
       pname == "anti_triton"    ||
       pname == "anti_He3"       ) {

      G4HadronElasticProcess* hel = new G4HadronElasticProcess("antielastic");
      if(G4FFKEY.HCrossSectionBias[1]!=1){
         hel->aScaleFactor=G4FFKEY.HCrossSectionBias[1];
         cout<<"G4HadronElasticProcess-I-AntiProtonElasticCrossSectionScaledBy "<< hel->aScaleFactor<<endl;
      }
      hel->AddDataSet(anucxs);
      hel->RegisterMe(lhep2);
      hel->RegisterMe(anuc);
      pmanager->AddDiscreteProcess(hel);
//    } else if(
//       pname == "GenericIon"    || 
//       pname == "alpha"     ||
//       pname == "deuteron"  ||
//       pname == "triton"    ||
//       pname == "He3"       ) {
//
//      G4HadronElasticProcess* hel = new G4HadronElasticProcess("ionelastic");
//      hel->AddDataSet(nucxs);
//      hel->RegisterMe(lhep3);
//      pmanager->AddDiscreteProcess(hel);
    }

  }
}
#endif
#if G4VERSION_NUMBER  > 945 && G4VERSION_NUMBER  <2000 
#if G4VERSION_NUMBER>999
#include "G4HadronElasticPhysics.hh"

#include "G4SystemOfUnits.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"

#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

#include "G4HadronElasticProcess.hh"
#include "G4HadronElastic.hh"
#include "G4ElasticHadrNucleusHE.hh"
#include "G4AntiNuclElastic.hh"

#include "G4BGGNucleonElasticXS.hh"
#include "G4BGGPionElasticXS.hh"
#include "G4NeutronElasticXS.hh"

#include "G4CrossSectionDataSetRegistry.hh"

#include "G4ChipsProtonElasticXS.hh"
#include "G4ChipsNeutronElasticXS.hh"

#include "G4ComponentAntiNuclNuclearXS.hh"  
#include "G4CrossSectionElastic.hh"
#include "G4ComponentGGNuclNuclXsc.hh"
#include "commons.h"
#endif
#include "G4HadronElastic.hh"
extern "C" void     abcross_(int &icas,float &a_p,float &a_t,float &zp_p,float &z_t,float &p_p,float &sigma_t,float sigma_el[],float sigma_q[]);
        double deltam(double bg, double m1, double m2,double pt){
//      return delta_energy after elastic scattering
        double p1=m1*bg;
        double p2=m2*bg;
        double e1=sqrt(p1*p1+m1*m1);
        double e2=sqrt(p2*p2+m2*m2);
        double m=m1+m2;
        double pp=sqrt(p2*p2-pt*pt);
        double mp=m1*m1+m2*m2+2*(e1*e2-p1*pp);
        mp=sqrt(mp);
        return mp-m;
       }

G4double G4HadronElastic::SampleInvariantT(const G4ParticleDefinition* p,
                                  G4double plab,
                                  G4int Z, G4int A)
{
 
  static const G4double GeV2 = GeV*GeV;
  G4double momentumCMS = ComputeMomentumCMS(p,plab,Z,A);
  G4double tmax = 4.0*momentumCMS*momentumCMS/GeV2;
  G4double aa, bb, cc;
  G4double dd = 10.;
  G4Pow* g4pow = G4Pow::GetInstance();
    float a_p=p->GetBaryonNumber(); 
    float a_t=A;
    float z_p=int(p->GetPDGCharge()/eplus+0.5);
    float z_t=Z;
    float p_p=plab/GeV;
   const G4double protonMass = 938.272/1000.;
   const G4double neutronMass = 939.565/1000.;
   double  m_p=p->GetPDGMass()/GeV;
   double delta=protonMass*z_p+(a_p-z_p)*neutronMass-m_p;
  if (A <= 62) {
    bb = 14.5*g4pow->Z23(A);
    aa = g4pow->powZ(A, 1.63)/bb;
    cc = 1.4*g4pow->Z13(A)/dd;
  } else {
    bb = 60.*g4pow->Z13(A);
    aa = g4pow->powZ(A, 1.33)/bb;
    cc = 0.4*g4pow->powZ(A, 0.4)/dd;
  }
  if(strstr((const char*)(this->GetModelName()),"ionelasticVC")){
    float sigma_t=0;
    float sigma_el[3]={0,0,0};
    float sigma_q[5]={0,0,0,0,0};
    int icas=1;
    abcross_(icas,a_p,a_t,z_p,z_t,p_p,sigma_t,sigma_el,sigma_q);
    aa=sigma_el[0];
    bb=sigma_el[1];
    cc=sigma_q[0];
    dd=sigma_q[2]; 
  }
  G4double q1 = 1.0 - std::exp(-bb*tmax);
  G4double q2 = 1.0 - std::exp(-dd*tmax);
  double bias=G4FFKEY.HCrossSectionBias[2];
 // std::cout <<" aa  cc "<<aa<<" "<<cc<<" "<<bias<<" "<<plab/GeV<<" "<<bb<<" "<<dd<<std::endl;
  G4double s1 = q1*aa;
  G4double s2 = q2*cc*bias;
  if(s2<0)s2=0;
  if((s1 + s2)*G4UniformRand() < s2) {
    q1 = q2;
    bb = dd;
  }
    int mtry=100;
    if(z_p<3 || !G4FFKEY.HCrossSectionBias[3])mtry=0;
    
    static unsigned int nerr=0;
    double pt2=-GeV2*std::log(1.0 - G4UniformRand()*q1)/bb;
   for(int itry=0;itry<mtry;itry++){
//  neutron or proton
    if(itry>0)pt2=-GeV2*std::log(1.0 - G4UniformRand()*q1)/bb;
    double m2=protonMass;
    if(  G4UniformRand()>z_p/a_p)m2=neutronMass;
    double m1=m_p-m2;
    double bg=p_p/m_p;
    double pt=sqrt(pt2);
    pt=pt/GeV;
    double d2=deltam(bg,m1,m2,pt);
//    G4cout <<" Q2 "<<itry<<" "<<d2<<" "<<delta<<" "<<pt<<" "<<p_p<<" "<<m_p<<" " <<bg<<" "<<m1<<" "<<m2<<G4endl;
    if(d2<delta )return pt2;
    else if(nerr++<mtry)G4cerr<<" G4HadronElastic::SampleInvariantT-W-Q2Toobig "<<pt2<<" "<<delta<<" "<<d2<<" "<<p_p<<G4endl;
   }
  if(mtry)cerr<<"G4HadronElastic::SampleInvariantT-E-UnabletoSample "<<endl;
  return pt2;
}
#include "G4ComponentGGNuclNuclXsc.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4HadTmpUtil.hh"
#include "G4HadronNucleonXsc.hh"

G4double G4ComponentGGNuclNuclXsc::
GetZandACrossSection(const G4DynamicParticle* aParticle,
                     G4int tZ, G4int tA)
{
  static double fScale=fHadronNucleonXsc;
  static int init=0;
  if(!init++){
    G4cout<<"  G4ComponentGGNuclNuclXsc::GetZandACrossSection-I-CrossectionScaledBy "<<fScale<<G4endl;
  }
  G4double xsection;
  G4double sigma;
  G4double cofInelastic = 2.4;
  G4double cofTotal = 2.0;
  G4double nucleusSquare;
  G4double cB;
  G4double ratio;

  G4double pZ = aParticle->GetDefinition()->GetPDGCharge();
  G4double pA = aParticle->GetDefinition()->GetBaryonNumber();

  G4double pTkin = aParticle->GetKineticEnergy();  
  pTkin /= pA;

  G4double pN = pA - pZ;
  if( pN < 0. ) pN = 0.;

  G4double tN = tA - tZ;
  if( tN < 0. ) tN = 0.;

  G4double tR = GetNucleusRadius( G4double(tZ),G4double(tA) );  
  G4double pR = GetNucleusRadius(pZ,pA); 

  cB = GetCoulombBarier(aParticle, G4double(tZ), G4double(tA), pR, tR);

  if ( cB > 0. ) 
  {
    G4DynamicParticle* dProton = new G4DynamicParticle(theProton,
                                              G4ParticleMomentum(1.,0.,0.), 
                                              pTkin);

    G4DynamicParticle* dNeutron = new G4DynamicParticle(theNeutron,
                                              G4ParticleMomentum(1.,0.,0.), 
                                              pTkin);

    sigma = (pZ*tZ+pN*tN)*hnXsc->GetHadronNucleonXscNS(dProton, theProton);

    G4double ppInXsc = hnXsc->GetInelasticHadronNucleonXsc();

    sigma += (pZ*tN+pN*tZ)*hnXsc->GetHadronNucleonXscNS(dNeutron, theProton);

    G4double npInXsc = hnXsc->GetInelasticHadronNucleonXsc();

    // G4cout<<"ppInXsc = "<<ppInXsc/millibarn<<"; npInXsc = "<<npInXsc/millibarn<<G4endl;
    // G4cout<<"npTotXsc = "<<hnXsc->GetTotalHadronNucleonXsc()/millibarn<<"; npElXsc = "
    //                      <<hnXsc->GetElasticHadronNucleonXsc()/millibarn<<G4endl;

    nucleusSquare = cofTotal*pi*( pR*pR + tR*tR );   // basically 2piRR

    ratio      = sigma/nucleusSquare;
    xsection   = nucleusSquare*std::log( 1. + ratio );
    fTotalXsc  = xsection;
    fTotalXsc *= cB;

    fInelasticXsc = nucleusSquare*std::log( 1. + cofInelastic*ratio )/cofInelastic;

    fInelasticXsc *= cB;
    fElasticXsc   = fTotalXsc - fInelasticXsc;

    // if (fElasticXsc < DBL_MIN) fElasticXsc = DBL_MIN;
    /*    
    G4double difratio = ratio/(1.+ratio);

    fDiffractionXsc = 0.5*nucleusSquare*( difratio - std::log( 1. + difratio ) );
    */
    // production to be checked !!! edit MK xsc

    //sigma = (pZ*tZ+pN*tN)*GetHadronNucleonXscMK(theProton, pTkin, theProton) +
    //      (pZ*tN+pN*tZ)*GetHadronNucleonXscMK(theProton, pTkin, theNeutron);

    sigma = (pZ*tZ+pN*tN)*ppInXsc + (pZ*tN+pN*tZ)*npInXsc;
 
    ratio = sigma/nucleusSquare;
    fProductionXsc = nucleusSquare*std::log( 1. + cofInelastic*ratio )/cofInelastic;

    if (fElasticXsc < 0.) fElasticXsc = 0.;
    delete dProton;
    delete dNeutron;
  }
  else
  {
    fInelasticXsc  = 0.;
    fTotalXsc      = 0.;
    fElasticXsc    = 0.;
    fProductionXsc = 0.;
  }
// hack
    fInelasticXsc*=  fScale;
    fTotalXsc*= fScale;
    fElasticXsc*= fScale;
    fProductionXsc*= fScale;
    fDiffractionXsc*= fScale;
  
  return fInelasticXsc;   // xsection; 
}



#endif

#if G4VERSION_NUMBER  > 945  && G4VERSION_NUMBER  <1000
//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//  Calculation of the total, elastic and inelastic cross-sections
//  of anti-nucleon and anti-nucleus interactions with nuclei
//  based on Glauber approach and V. Grishine formulaes for
//  interpolations (ref. V.M.Grichine, Eur.Phys.J., C62(2009) 399;
//  NIM, B267 (2009) 2460) and our parametrization of hadron-nucleon
//  cross-sections
// 
// 
//   Created by A.Galoyan and V. Uzhinsky, 18.11.2010


#include "G4ComponentAntiNuclNuclearXS.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"

///////////////////////////////////////////////////////////////////////////////

G4ComponentAntiNuclNuclearXS::G4ComponentAntiNuclNuclearXS() 
: G4VComponentCrossSection("AntiAGlauber"), fUpperLimit(10000*GeV),
  fLowerLimit(10*MeV), fRadiusEff(0.0), fRadiusNN2(0.0),
  fTotalXsc(0.0), fElasticXsc(0.0), fInelasticXsc(0.0),
  fAntiHadronNucleonTotXsc(0.0), fAntiHadronNucleonElXsc(0.0),
  Elab(0.0), S(0.0), SqrtS(0) 
{
  theAProton       = G4AntiProton::AntiProton();
  theANeutron      = G4AntiNeutron::AntiNeutron();
  theADeuteron     = G4AntiDeuteron::AntiDeuteron();
  theATriton       = G4AntiTriton::AntiTriton();
  theAAlpha        = G4AntiAlpha::AntiAlpha();
  theAHe3          = G4AntiHe3::AntiHe3();

 Mn       = 0.93827231;           // GeV
 b0       = 11.92;                // GeV^(-2)
 b2       = 0.3036;               // GeV^(-2)
 SqrtS0   = 20.74;                // GeV
 S0       = 33.0625;              // GeV^2

}

///////////////////////////////////////////////////////////////////////////////////////
//
//

G4ComponentAntiNuclNuclearXS::~G4ComponentAntiNuclNuclearXS()
{
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// Calculation of total CrossSection of Anti-Nucleus - Nucleus 


G4double G4ComponentAntiNuclNuclearXS::GetTotalElementCrossSection
(const G4ParticleDefinition* aParticle, G4double kinEnergy, G4int Z, G4double A)
{
   G4double xsection,   sigmaTotal, sigmaElastic;

 const G4ParticleDefinition* theParticle = aParticle;

    sigmaTotal        = GetAntiHadronNucleonTotCrSc(theParticle,kinEnergy);
    sigmaElastic      = GetAntiHadronNucleonElCrSc(theParticle,kinEnergy);

// calculation of squared radius of  NN-collision
   fRadiusNN2=sigmaTotal*sigmaTotal*0.1/(8.*sigmaElastic*pi) ;  //fm^2   

// calculation of effective nuclear radius for Pbar and Nbar interactions (can be changed)

  //A.R. 29-Jan-2013 : use antiprotons/antineutrons as the default case,
  //                   to be used for instance, as first approximation
  //                   without validation, for anti-hyperons. 
  //if ( (theParticle == theAProton) || (theParticle == theANeutron) ) {   
     if(int(A+0.5)==1)
     { fTotalXsc = sigmaTotal * millibarn;
        return fTotalXsc;  }
 
   fRadiusEff = 1.34*std::pow(A,0.23)+1.35/std::pow(A,1./3.);   //fm
  
   if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.800;     //fm
   if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 3.300;  
   if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 3.300;  
   if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.376;     
 //}
      
//calculation of effective nuclear radius for AntiDeuteron interaction (can be changed)
  if (theParticle == theADeuteron) 
 { fRadiusEff = 1.46 * std::pow(A,0.21) + 1.45 / std::pow(A,1./3.);

    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.238;     //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 3.144;     
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 3.144;      
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.544;     
 }
// calculation of effective nuclear radius for AntiHe3 interaction (can be changed)

  if( (theParticle ==theAHe3) || (theParticle ==theATriton) )
 { fRadiusEff = 1.40* std::pow(A,0.21)+1.63/std::pow(A,1./3.);

    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.144;     //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 3.075;  
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 3.075;  
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.589;  
  }

//calculation of effective nuclear radius for AntiAlpha interaction (can be changed)

  if (theParticle == theAAlpha) 
 {
  fRadiusEff = 1.35* std::pow(A,0.21)+1.1/std::pow(A,1./3.);
  
    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 2.544;     //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 2.589;   
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 2.589;   
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.241;    
  
 }

   G4double R2 = fRadiusEff*fRadiusEff;
   G4double REf2  = R2+fRadiusNN2;
   G4double ApAt = std::abs(theParticle->GetBaryonNumber())  *  A;

 xsection = 2*pi*REf2*10.*std::log(1+(ApAt*sigmaTotal/(2*pi*REf2*10.)));  //mb
 xsection =xsection *millibarn; 
 fTotalXsc   = xsection;

  return fTotalXsc; 
}


////////////////////////////////////////////////////////////////
// Calculation of inelastic CrossSection of Anti-Nucleus - Nucleus
////////////////////////////////////////////////////////////////

G4double G4ComponentAntiNuclNuclearXS::GetInelasticElementCrossSection
(const G4ParticleDefinition* aParticle, G4double kinEnergy, G4int Z, G4double A)
{
  G4double  inelxsection,  sigmaTotal, sigmaElastic;

  const G4ParticleDefinition* theParticle = aParticle;

    sigmaTotal        = GetAntiHadronNucleonTotCrSc(theParticle,kinEnergy);
    sigmaElastic      = GetAntiHadronNucleonElCrSc(theParticle,kinEnergy);
  
// calculation of sqr of radius NN-collision
   fRadiusNN2=sigmaTotal*sigmaTotal*0.1/(8.*sigmaElastic*pi);   // fm^2   


// calculation of effective nuclear radius for Pbar and Nbar interaction (can be changed)

  //A.R. 29-Jan-2013 : use antiprotons/antineutrons as the default case,
  //                   to be used for instance, as first approximation
  //                   without validation, for anti-hyperons. 
  //if ( (theParticle == theAProton) || (theParticle == theANeutron) ) {   
  if (int(A+0.5)==1)
      { fInelasticXsc = (sigmaTotal - sigmaElastic) * millibarn;
        return fInelasticXsc;  
      } 
 fRadiusEff = 1.31*std::pow(A, 0.22)+0.9/std::pow(A, 1./3.);  //fm
    
    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.582;               //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 3.105;               
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 3.105;
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.209;
 //}

//calculation of effective nuclear radius for AntiDeuteron interaction (can be changed)

  if (theParticle ==theADeuteron) 
{ 
 fRadiusEff = 1.38*std::pow(A, 0.21)+1.55/std::pow(A, 1./3.);
  
    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.169;            //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 3.066;
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 3.066;
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.498;
 }

//calculation of effective nuclear radius for AntiHe3 interaction (can be changed)

  if( (theParticle ==theAHe3) || (theParticle ==theATriton) )
 {
  fRadiusEff = 1.34 * std::pow(A, 0.21)+1.51/std::pow(A, 1./3.);
  
    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 3.066;           //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 2.973;
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 2.973;
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.508;
  
 }

//calculation of effective nuclear radius for AntiAlpha interaction (can be changed)

  if (theParticle == theAAlpha) 
 {
  fRadiusEff = 1.3*std::pow(A, 0.21)+1.05/std::pow(A, 1./3.);
    
    if( (Z==1) && (int(A+0.5)==2) ) fRadiusEff = 2.498;            //fm
    if( (Z==1) && (int(A+0.5)==3) ) fRadiusEff = 2.508;
    if( (Z==2) && (int(A+0.5)==3) ) fRadiusEff = 2.508;
    if( (Z==2) && (int(A+0.5)==4) ) fRadiusEff = 2.158;
 }
  G4double R2 = fRadiusEff*fRadiusEff;
  G4double REf2  = R2+fRadiusNN2;
  G4double  ApAt= std::abs(theParticle->GetBaryonNumber())  *  A;

 inelxsection  = pi*REf2 *10* std::log(1+(ApAt*sigmaTotal/(pi*REf2*10.))); //mb
 inelxsection  = inelxsection * millibarn;  
   fInelasticXsc =  inelxsection; 
   return fInelasticXsc;
}

#endif
