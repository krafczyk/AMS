
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
// GEANT4 tag $Name$
//
// 

#include "G4ios.hh"
#include <fstream>
#include <iomanip>

#include "g4strangelet.h"
#include "geant4.h"
#include "mceventg.h"
#include "G4ParticleTable.hh"





#include "G4StepPoint.hh"
#include "G4TrackStatus.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4Poisson.hh"
#include "TF1.h"
#include "TLorentzVector.h"
#include "g4physics.h"
#include "job.h"
G4double G4StrangeletP::PostStepGetPhysicalInteractionLength( const G4Track& Track,
        G4double previousStepSize,
        G4ForceCondition* condition ){
    *condition=Forced;
    return DBL_MAX;

}

G4VParticleChange* G4StrangeletP::PostStepDoIt( const G4Track& Track, const G4Step& Step ){
    return NULL;
}
    G4double G4StrangeletP::GetMeanFreePath(const G4Track& track, G4double step, G4ForceCondition *){
return DBL_MAX;
}

G4bool G4StrangeletP::IsApplicable(const G4ParticleDefinition& p){
return strstr((const char*)(p.GetParticleName()),"strangelet")!=0;
}



#include "G4IonTable.hh"
#include "G4HadTmpUtil.hh"


StrCS::StrCS ()
 : G4VCrossSectionDataSet("StrCS")
{
  // Constructor only needs to instantiate the object which provides functions
  // to calculate the nuclear radius, and some other constants used to
  // calculate cross-sections.


  lowEnergyCheck = false;
}
///////////////////////////////////////////////////////////////////////////////
//
StrCS::~StrCS ()
{
//
//
// Destructor just needs to delete the pointer to the G4WilsonRadius object.
//
}
///////////////////////////////////////////////////////////////////////////////
//
G4bool StrCS::IsApplicable
  (const G4DynamicParticle* theProjectile, const G4Element* theTarget)
{
  G4int Z = G4lrint(theTarget->GetZ());
  G4int A = G4lrint(theTarget->GetN());
  return IsIsoApplicable(theProjectile, Z, A);
}


#if G4VERSION_NUMBER  > 945 
G4bool StrCS::IsElementApplicable(const G4DynamicParticle* theProjectile,G4int Z,const G4Material* mat){
  bool result=strstr((const char *)(theProjectile->GetParticleDefinition()->GetParticleName()),"strangelet")!=0;
  return result;
}
G4bool StrCS::IsIsoApplicable(const G4DynamicParticle* theProjectile,
                              G4int ZZ, G4int AA, const G4Element* elm, const G4Material* mat) {
#else
G4bool StrCS::IsIsoApplicable(const G4DynamicParticle* theProjectile,
                              G4int ZZ, G4int AA){
#endif
  bool result=strstr((const char *)(theProjectile->GetParticleDefinition()->GetParticleName()),"strangelet")!=0;
  return result;
}

#if G4VERSION_NUMBER  > 945 
G4double StrCS::GetIsoCrossSection(const G4DynamicParticle* theProjectile,
                                   G4int ZZ, G4int AA, const G4Isotope* iso, const G4Element* elm, const G4Material* mat) {
  return GetZandACrossSection(theProjectile,ZZ,AA,0);
}
G4double StrCS::GetElementCrossSection(const G4DynamicParticle* theProjectile,G4int Z,const G4Material*mat){
  const G4ElementVector*elv=mat->GetElementVector();
  for(int k=0;k<elv->size();k++){
    G4Element *el=(*elv)[k];
    if(Z==el->GetZ()){
      return GetCrossSection( theProjectile, el,293*kelvin);
    }
}
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
G4double
StrCS::GetZandACrossSection(const G4DynamicParticle* theProjectile,
                                                  G4int ZZ, G4int AA, G4double /*theTemperature*/)
{
  // Initialise the result.

  // Get details of the projectile and target (nucleon number, atomic number,
  // kinetic enery and energy/nucleon.

  const G4double AT = AA;
  const G4double ZT = ZZ;
  const G4double EA = theProjectile->GetKineticEnergy()/MeV;
  const G4double AP = theProjectile->GetDefinition()->GetBaryonNumber();
  const G4double ZP = theProjectile->GetDefinition()->GetPDGCharge();
  G4double E  = EA / AP;

  // Determine target mass and energy within the centre-of-mass frame.

  G4double mT = G4ParticleTable::GetParticleTable()
                ->GetIonTable()
                ->GetIonMass(static_cast<G4int>(ZT), static_cast<G4int>(AT));
  G4LorentzVector pT(0.0, 0.0, 0.0, mT);
  G4LorentzVector pP(theProjectile->Get4Momentum());
  pT = pT + pP;
  G4double E_cm = (pT.mag()-mT-pP.m())/MeV;
   
  double r0target = 1.22*fermi;
  double r0str = 1.10*fermi;

  double TotRad = r0target*pow(AT,0.3333333)+r0str*pow(AP,0.3333333);
  double result =  pi*TotRad*TotRad; // this is cross sec in fm**2

  return result;
}


G4double StrCS::GetCrossSection
  (const G4DynamicParticle* theProjectile, const G4Element* theTarget,
  G4double theTemperature)
{
  G4int nIso = theTarget->GetNumberOfIsotopes();
  G4double xsection = 0;

  if (nIso) {
    G4double sig;
    G4IsotopeVector* isoVector = theTarget->GetIsotopeVector();
    G4double* abundVector = theTarget->GetRelativeAbundanceVector();
    G4int ZZ;
    G4int AA;

    for (G4int i = 0; i < nIso; i++) {
      ZZ = (*isoVector)[i]->GetZ();
      AA = (*isoVector)[i]->GetN();
      sig = GetZandACrossSection(theProjectile, ZZ, AA, theTemperature);
      xsection += sig*abundVector[i];
    }

  } else {
    G4int ZZ = G4lrint(theTarget->GetZ());
    G4int AA = G4lrint(theTarget->GetN());
    xsection = GetZandACrossSection(theProjectile, ZZ, AA, theTemperature);
  }

  return xsection;
}


///////////////////////////////////////////////////////////////////////////////
//
void StrCS::SetLowEnergyCheck (G4bool aLowEnergyCheck)
{
  lowEnergyCheck = aLowEnergyCheck;
}

TF1* StrHP::f1=0;
TF1* StrHP::fE=0;

StrHP::StrHP():G4HadronicInteraction("StrangeletYaleHP"){
  verboseLevel=0;
  SetMinEnergy(0.001*GeV);
  SetMaxEnergy(1000.0*TeV);
  theParticleTable = G4ParticleTable::GetParticleTable();
  theIonTable      = const_cast <G4IonTable *> (theParticleTable->GetIonTable());
  if(!f1)f1 = new TF1("strangeletfunc1"," (x*x) / (exp((sqrt([0]*[0]+x*x)-[1]+[2]+[3])/[4]) +[5] )",0.,5000.);
  if(!fE)fE = new TF1("strangeletfuncE"," (x*x*(sqrt([0]*[0]+x*x))) / (exp((sqrt([0]*[0]+x*x)-[1]+[2]+[3])/[4]) +[5] )",0.,5000.);


  // from particle list remove (for now ) all antiparticles, delta resonances, kaons, alphas, 
  //   and heavier nuclei because they are so rarely produces
  // also remove H3-Lambda because, although a few are made at higher temps, the error 
  //from not including it is small and the effort to introduce it and its decays is large

  // proton
  for(int i=0;i<sizeof(IsStr)/sizeof(IsStr[0]);i++)IsStr[0]=0;
  particle_NUM=-1;
  particle_NUM++;
  name[particle_NUM]="proton";
  mass[particle_NUM]= 938.27;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= 0.5;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 14;
  
  // neturon
  particle_NUM++;
  name[particle_NUM]="neutron";
  mass[particle_NUM]= 939.56;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= -0.5;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 13;
  
  // deuteron
  particle_NUM++;
  name[particle_NUM]="deuteron";
  mass[particle_NUM]= 1875.6;
  g[particle_NUM] = 3;
  n_I3[particle_NUM]= 0.0;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 2.0;
  GEANT_ID[particle_NUM] = 45;

  // alpha 
  particle_NUM++;
  name[particle_NUM]="alpha";
  mass[particle_NUM]= 3726.66; // assumes 28MeV binding (7MeV/nucleon)
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= 0.0;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 4.0;
  GEANT_ID[particle_NUM] = 47;

  // Lambda
  particle_NUM++;
  name[particle_NUM]="lambda";
  mass[particle_NUM]= 1115.68;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= 0.0;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 18;

    
  // Sigma+ 
  particle_NUM++;
  name[particle_NUM]="SigmaP";
  mass[particle_NUM]= 1189.37;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= 1.0;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 19;

  
  // Sigma- - 16
  particle_NUM++;
  name[particle_NUM]="SigmaM";
  mass[particle_NUM]= 1197.45;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= -1.0;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]=  1.0;
  GEANT_ID[particle_NUM] = 21;
    
  // Sigma0 - 18
  particle_NUM++;
  name[particle_NUM]="SigmaZ";
  mass[particle_NUM]= 1192.64;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= 0.0;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]=  1.0;
  GEANT_ID[particle_NUM] = 20;
    
  // Cascade- - 19
  particle_NUM++;
  name[particle_NUM]="CascadeM";
  mass[particle_NUM]= 1321.31;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= -0.5;
  n_S[particle_NUM]= -2.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 23;

    
  // Cascade0 - 21
  particle_NUM++;
  name[particle_NUM]="CascadeZ";
  mass[particle_NUM]= 1314.8;
  g[particle_NUM] = 2;
  n_I3[particle_NUM]= 0.5;
  n_S[particle_NUM]= -2.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 22;
  
  // Omega - 24
  particle_NUM++;
  name[particle_NUM]="Omega";
  mass[particle_NUM]= 1672.5;
  g[particle_NUM] = 4;
  n_I3[particle_NUM]= 0.;
  n_S[particle_NUM]= -3.0;
  n_B[particle_NUM]= 1.0;
  GEANT_ID[particle_NUM] = 24;
  
  // piplus-6
  particle_NUM++;
  name[particle_NUM]="piP";
  mass[particle_NUM]= 139.57;
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= 1.0;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 0.0;
  GEANT_ID[particle_NUM] = 8;

  // piminus
  particle_NUM++;
  name[particle_NUM]="piM";
  mass[particle_NUM]= 139.57;
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= -1.0;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 0.0;
  GEANT_ID[particle_NUM] = 9;
  
  // pi0 - 8
  particle_NUM++;
  name[particle_NUM]="piZ";
  mass[particle_NUM]= 134.98;
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= 0.0;
  n_S[particle_NUM]= 0.0;
  n_B[particle_NUM]= 0.0;
  GEANT_ID[particle_NUM] = 7;
  
  // k- 
  particle_NUM++;
  name[particle_NUM]="kM";
  mass[particle_NUM]= 493.68;
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= -0.5;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]= 0.0;
  GEANT_ID[particle_NUM] = 12;
  
  // anti-k0
  particle_NUM++;
  name[particle_NUM]="kZbar";
  mass[particle_NUM]= 497.67;
  g[particle_NUM] = 1;
  n_I3[particle_NUM]= 0.5;
  n_S[particle_NUM]= -1.0;
  n_B[particle_NUM]= 0.0;
  GEANT_ID[particle_NUM] = 16;

   cout <<"StrHP::StrHP()-I-TotalOf "<<particle_NUM<<" particles initialized"<<endl;; 





}


G4bool StrHP::IsApplicable (const G4HadProjectile &theTrack, G4Nucleus &theTarget){
//
//
// Get relevant information about the projectile and target (A, Z)
//
  const G4ParticleDefinition *definitionP = theTrack.GetDefinition();
  G4int AP   = definitionP->GetBaryonNumber();
  G4int ZP   = G4int(definitionP->GetPDGCharge()/eplus + 0.5);
    G4int AT   = theTarget.GetA_asInt();
    G4int ZT   = theTarget.GetZ_asInt();

  bool ret= strstr((const char*)(definitionP->GetParticleName()),"strangelet")!=0;
  return ret;
}
G4HadFinalState *StrHP::ApplyYourself (const G4HadProjectile &theTrack, G4Nucleus &theTarget){
//
//
// The secondaries will be returned in G4HadFinalState &theParticleChange -
// initialise this.  The original track will always be discontinued and
// secondaries followed.
//
  theParticleChange.Clear();
  theParticleChange.SetStatusChange(stopAndKill);
//
//
// Get relevant information about the projectile and target (A, Z, energy/nuc,
// momentum, etc).
//
  const G4ParticleDefinition *definitionP = theTrack.GetDefinition();
  G4int AP   = definitionP->GetBaryonNumber();
  G4int ZP   = G4int(definitionP->GetPDGCharge()/eplus+0.5);
  G4double M          = definitionP->GetPDGMass();
  G4ThreeVector pP    = theTrack.Get4Momentum().vect();
  G4double T          = theTrack.GetKineticEnergy()/G4double(AP);   // Units are MeV/nuc 
  G4double E          = theTrack.GetTotalEnergy()/G4double(AP);	    // Units are MeV/nuc
    G4double AT         = theTarget.GetA_asInt();
    G4double ZT         = theTarget.GetZ_asInt();
  //G4double AT         = theTarget.GetN();
  //G4double ZT         = theTarget.GetZ();
  G4double mpnt  = theTarget.AtomicMass(AT, ZT);
  G4double TotalEPre  = theTrack.GetTotalEnergy() + mpnt;
    //    theTarget.AtomicMass(AT, ZT) + theTarget.GetEnergyDeposit();
//  G4LorentzRotation transformToLab =
//  	(const_cast <G4HadProjectile*> (&theTrack))->GetTrafoToLab();
//
//
// Output relevant information on initial conditions if verbose.  Note that
// most of the verbsse output is dealt with through private member function
// calls.
//
  if (verboseLevel >= 2)
  {
    G4cout <<"########################################"
           <<"########################################"
           <<G4endl;
    G4cout.precision(6);
    G4cout <<"IN G4DPMJET2_5Model::ApplyYourself" <<G4endl;
    G4cout <<"START OF EVENT" <<G4endl;
    G4cout <<"Initial projectile (A,Z) = (" <<AP <<", " <<ZP <<")" <<G4endl;
    G4cout <<"Initial target     (A,Z) = (" <<AT <<", " <<ZT <<")" <<G4endl;
    G4cout <<"Projectile momentum      = " <<pP/MeV <<" MeV/c" <<G4endl;
    G4cout <<"Total energy             = " <<TotalEPre/MeV <<" MeV" <<G4endl;
    G4cout <<"Kinetic energy/nuc       = " <<T/MeV <<" MeV" <<G4endl;
  }
double StrA_out;
double StrZ_out;
double StrMom_out[4];
int ID_out[1000];
int n_out=sizeof(ID_out)/sizeof(ID_out[0]);
double mom_out[1000][4];
double strBetaIn[3];
for(int i=0;i<3;i++)strBetaIn[i]=pP[i]/theTrack.GetTotalEnergy();
Thermal(M/MeV, mpnt/MeV, AT, ZP, AP, strBetaIn, StrA_out , StrZ_out, StrMom_out, n_out, ID_out, mom_out); 
 if(StrA_out>0){
  if(StrA_out==AP && StrZ_out==ZP){
    theParticleChange.SetStatusChange(isAlive);
    theParticleChange.SetEnergyChange(StrMom_out[0]-M);
    G4LorentzVector g4lv(StrMom_out[1],StrMom_out[2],StrMom_out[3],StrMom_out[0]);
    theParticleChange.SetMomentumChange(g4lv.vect().unit());
  }
  else{
//  Here need to create secondary strangelet;
    if(StrZ_out<=0)StrZ_out=1;
    G4ParticleDefinition*theParticle=AMSJob::gethead()->getg4physics()->ConstructStrangelet(0,StrZ_out,StrA_out);
    if(theParticle){
    G4LorentzVector g4lv(StrMom_out[1],StrMom_out[2],StrMom_out[3],StrMom_out[0]);
        G4DynamicParticle *theDynamicParticle = 
          new G4DynamicParticle(theParticle,g4lv);
        theParticleChange.AddSecondary (theDynamicParticle);
   }
   else{
         cerr<<"StrHP::ApplyYourSelf-E-UnableToCreat2ndaryStrangelet  "<<StrA_out<<StrZ_out<<endl;
   }  
  }
 }
 else if(n_out==0){
    theParticleChange.SetStatusChange(isAlive);
    theParticleChange.SetEnergyChange(theTrack.GetKineticEnergy());
    theParticleChange.SetMomentumChange(theTrack.Get4Momentum().vect().unit());
 }
 for(int i=0;i<n_out;i++){
   const char *name=AMSJob::gethead()->getg4physics()->G3toG4(ID_out[i]);
   if(name){
    G4ParticleDefinition* theParticle =
        theParticleTable->FindParticle(G4String(name));
      if (theParticle){
        G4LorentzVector lv = G4LorentzVector(mom_out[i][1],mom_out[i][2],mom_out[i][3],mom_out[i][0]);
        G4DynamicParticle *theDynamicParticle = 
          new G4DynamicParticle(theParticle,lv);
        theParticleChange.AddSecondary (theDynamicParticle);
      }
      else{
         cerr<<"StrHP::ApplyYourSelf-E-UnableTofindParticle  "<<name<<endl;
      }
}
else{
cerr<<"StrHP::ApplyYourself-E-G3toG4 Return Null "<<ID_out[i]<<endl;
}
}
return &theParticleChange;
}


// Input
// target nucleus A (targetA)
// strangelet Z (strZ) - 
// strangelet A (strA) - note that strangelet Z/A = 0.1 is assumed
// strangelet 3-velocity (strBetaIn[3])  :: Note betax is strBetaIn[0], betay is strBetaIn[1], betaz is strBetaIn[2]

// Output
// Final Strangelet A (&StrA_out) - if no final strangelet, StrA_out will be set to zero 
// Final Strangelet Z (&StrZ_out) - again, Z/A = 0.1
// Final Strangelet Momentum (StrMom_out[4])  ::  Note: [0] is energy, [1] is px, [2] is py, [3] is pz
// Number of particles (other than strangelet) created in collision (&n_out)
// Array of particle GEANT ID numbers of produced particles (ID_out[1000])
// Array of 4-momenta of produced particles ( mom_out[1000][4] ) ::  Note: [0] is energy, [1] is px, [2] is py, [3] is pz

// this routine assumes that the dominant effect comes from the fragmentation of the strangelet and
// ignores the target nucleus fragments after the interaction

void StrHP::Thermal(double M_S, double M_A,double targetA, double strZ, double strA, double strBetaIn[3], double &StrA_out , double &StrZ_out, double StrMom_out[4], int &n_out, int ID_out[1000], double mom_out[1000][4]) { 

  double strBetaInMag = sqrt(strBetaIn[2]*strBetaIn[2]+strBetaIn[1]*strBetaIn[1]+strBetaIn[0]*strBetaIn[0]);
  double strGammaIn = 1./sqrt(1.-strBetaInMag*strBetaInMag);
  int max_out=n_out;
  n_out = 0;

//  double M_S = strA*0.930*1000; // assumed mass of strangelet in MeV
//  double M_A = targetA*0.938*1000; // approximate mass of target nucleus in MeV

  double comtemp = (strGammaIn * M_S) / ( strGammaIn * M_S + M_A);
  double BetaCOMx = strBetaIn[0]*comtemp; // betaX of the COM frame in the lab fram
  double BetaCOMy = strBetaIn[1]*comtemp; // betaY of the COM frame in the lab fram
  double BetaCOMz = strBetaIn[2]*comtemp; // betaZ of the COM frame in the lab fram

  double s = M_A*M_A + M_S*M_S + 2*strGammaIn*M_A*M_S;
  double roots = sqrt(s);  //sqrt(s) - c.o.m. energy in MeV
  double KE = roots - M_S - M_A;  // energy available for particle production in MeV
  double ThermEoverA = (KE + M_S)/(1000*strA); // unfortunately parameterized in GeV for model use below 

  // we have T (temperature) and deltaB (loss of Baryon number) parameterized as functions of KE

  double KE_GeV = KE/1000.;
  double T = 23.66 + 73.89*(1-exp(-.09265*sqrt(KE_GeV))) ;

  double deltaB = -0.060553 + 3.84082*KE_GeV - 0.00340656*KE_GeV*KE_GeV;
  if (KE_GeV>563) deltaB = 1082.6;  // our parameterization would decrease after this point; this should be pretty unusual
  deltaB = floor(deltaB+.5);

  //first worry about output strangelet

  double strAout = strA - deltaB;
  bool StrangeletOut = 1;
  if (strAout<4) StrangeletOut = 0;
  double sr=strZ/strA;
  StrA_out = strAout;
  StrZ_out = strAout * sr;
  StrZ_out = floor(StrZ_out+0.5);
  if (strAout<4) StrA_out = 0;
  if (strAout<4) StrZ_out = 0;


  // now worry about particle production and momentum

  double mu_B = 799.0;
  double mu_S = 162.5;
  double mu_I3 = 1.698;

  double VolumeMod = 1.0;

  // for case where whole strangelet is destroyed (should be rare) need more complicated lookup (not parameterized very well)
  if (StrangeletOut==0) {
     VolumeMod = (strA/300.);
     T =   -6.61060e3 + 6.81570e3*(1-exp(-3.55094*sqrt(ThermEoverA))) ;
     mu_B = 1.67086e4 - 1.66702e4*(1-exp(-2.83433*sqrt(ThermEoverA))) ;
     mu_S = 3.66412e3 - 3.61937e3*(1-exp(-3.11877*sqrt(ThermEoverA))) ;
     if (ThermEoverA<1.20){  // should only reach this special case for small, slow strangelet
       T = 65;
       mu_B = 790;
       mu_S = 164;
     }
  }


  //  this part taken right from thermal model 

  double Volume = 1500.;  // volume in fm**3
  double NoStr_xmax = 2500.;
  double NoStrE_xmax = 2500.;
  double BigB_xmax = 10000.;
  double BigBE_xmax = 10000.;
  double Str_xmax = 50000.;
  double StrE_xmax = 50000.;


    // strangelet

  if (StrangeletOut){
    name[particle_NUM+1]="strangelet";
    n_B[particle_NUM+1]= strAout;
    double EoverA = 930.;
    if (strAout < 225) EoverA = 934. + (940. - 934.) * ((strAout-175.)/(225.-175.));
    if (strAout < 175) EoverA = 940. + (950. - 940.) * ((175.-strAout)/(175.));
    if (strAout > 224) EoverA = 930. + (934. - 930.) * ((300.-strAout)/(300.-224.));
    if (strAout > 300) EoverA = 930.;
    mass[particle_NUM+1]= n_B[particle_NUM+1] * EoverA;
    g[particle_NUM+1] = 1;   // this is an assumption but should not change qualitatively the results
    n_I3[particle_NUM+1]= 0;
    n_S[particle_NUM+1]= n_B[particle_NUM+1]*(-0.8);
    IsStr[particle_NUM+1] = 1;
    GEANT_ID[particle_NUM+1]=113;
  }

  // end of particle list


  double temperature = T;


  if (verboseLevel>1){
    cout << " KE  " << KE << endl;
    cout << " M_A  " << M_A << endl;
    cout << " ThermEoverA " << ThermEoverA << endl;
    cout << " Temp  " << temperature << endl;
    cout << " mu_B  " << mu_B  << endl;
    cout << " mu_S  " << mu_S  << endl;
    cout << " mu_I3 " << mu_I3 << endl;
  }

  int MAXSTATES = StrangeletOut?particle_NUM+2:particle_NUM+1;

  double SumS = 0;
  double SumB = 0;
  double SumI3 = 0;
  double SumE = 0;
  double SumM = 0;
  double SumK = 0;

  double REAL_SumE = 0;
  double REAL_SumB = 0;

  for (Int_t ii=0;ii<MAXSTATES;ii++) {
    f1->SetRange(0.,NoStr_xmax);
    fE->SetRange(0.,NoStrE_xmax);
    if (n_B[ii]>2.5) f1->SetRange(0.,BigB_xmax);
    if (n_B[ii]>2.5) fE->SetRange(0.,BigBE_xmax);
    if (IsStr[ii]) f1->SetRange(0.,Str_xmax);
    if (IsStr[ii]) fE->SetRange(0.,StrE_xmax);

    f1->SetParameter(0,mass[ii]);
    f1->SetParameter(1,n_B[ii]*mu_B);
    f1->SetParameter(2,n_S[ii]*mu_S);
    f1->SetParameter(3,n_I3[ii]*mu_I3);
    f1->SetParameter(4,temperature);
    f1->SetParameter(5,1.0);
    if (g[ii]==1) f1->SetParameter(5,-1.0);
    if (g[ii]==3) f1->SetParameter(5,-1.0);
    if (g[ii]==5) f1->SetParameter(5,-1.0);
    if (g[ii]==7) f1->SetParameter(5,-1.0);
    if (IsStr[ii]) f1->SetParameter(5,1.0); // we're making the strangelet a fermion

    fE->SetParameter(0,mass[ii]);
    fE->SetParameter(1,n_B[ii]*mu_B);
    fE->SetParameter(2,n_S[ii]*mu_S);
    fE->SetParameter(3,n_I3[ii]*mu_I3);
    fE->SetParameter(4,temperature);
    fE->SetParameter(5,1.0);

    if (g[ii]==1) fE->SetParameter(5,-1.0);
    if (g[ii]==3) fE->SetParameter(5,-1.0);
    if (g[ii]==5) fE->SetParameter(5,-1.0);
    if (g[ii]==7) fE->SetParameter(5,-1.0);
    if (IsStr[ii]) fE->SetParameter(5,1.0); // we're making the strangelet a fermion

    double thisInt;
    double EInt;

    thisInt = f1->Integral(0, NoStr_xmax);
    EInt = fE->Integral(0, NoStrE_xmax);
    if (n_B[ii]>2.5) thisInt = f1->Integral(0, BigB_xmax);
    if (n_B[ii]>2.5) EInt = fE->Integral(0, BigBE_xmax);
    if (IsStr[ii]) thisInt = f1->Integral(0, Str_xmax);
    if (IsStr[ii]) EInt = fE->Integral(0, StrE_xmax);

    if (verboseLevel>0){
      cout << endl;
      cout << ii << "--------------------------------" << name[ii] << endl;
      if (IsStr[ii]) cout << "---strangelet A = " << n_B[ii] << endl;
    }
    if (verboseLevel>1){
      cout << endl;
      cout << ii << "--------------------------------" << name[ii] << endl;
      cout << "Mass " << mass[ii] << endl;
      cout <<  "Baryon Number " << n_B[ii] << endl;
      cout << "g-factor " << g[ii] << endl ;
      cout << "I3 " <<  n_I3[ii] << endl;
      cout << "strangeness " << n_S[ii] << endl;
      cout << "IsStr " << IsStr[ii] << endl;
      cout << "n_B[ii]*mu_B " << n_B[ii]*mu_B << endl;
      cout << "n_S[ii]*mu_S " << n_S[ii]*mu_S << endl;
      cout << "n_I3[ii]*mu_I3 " << n_I3[ii]*mu_I3 << endl;
      cout << "function at p=4000 " << f1->Eval(4000.,0.,0.)<<endl;
      cout <<  "integral gives " << thisInt << endl; // this integral gives units of MeV**3/fm*3
      cout <<  "Eintegral gives " << EInt << endl; // this integral gives units of MeV*MeV**3/fm*3
      
      if (thisInt > 1e-10) cout << "average Energy is " << EInt/thisInt << endl;
      if (thisInt < 1e-10) cout << "average Energy is undefined" << endl;
    }

    double den = g[ii]*thisInt;
    //if (verboseLevel>1) cout << "check " << den << endl;
    den = den/(2*3.1415927*3.1415927*197.32*197.32*197.32) ;   // 197.32 is hbar*c in units of MeV*fm --> this is density in units of fm**3
    if (verboseLevel>1) cout << "density " << den << endl;

    number[ii] = den*Volume*VolumeMod;
    if (IsStr[ii]==1) number[ii] =1;
    total_S[ii] = n_S[ii]*number[ii];
    total_B[ii] = n_B[ii]*number[ii];
    total_I3[ii] = n_I3[ii]*number[ii];
    if (thisInt > 1e-10) total_E[ii] = (EInt/thisInt)*number[ii];
    if (thisInt < 1e-10) total_E[ii] = 0;
    SumS = SumS + total_S[ii];
    SumB = SumB + total_B[ii];
    SumI3 = SumI3 + total_I3[ii];
    SumE = SumE + total_E[ii];
    SumM = SumM + mass[ii]*number[ii];
    SumK = SumK + (total_E[ii]-mass[ii]*number[ii]);
    if (verboseLevel>1) {
      cout << "avg number -------------------------- " << number[ii] << endl; 
      cout << " E so far  -------------------------- " << SumE << endl; 
      cout << " KE so far -------------------------- " << SumK << endl; 
      cout << "  M so far -------------------------- " << SumM << endl; 
    }

    //int numToGenerate = floor(number[ii]+0.5); // always use the mean number
    Int_t numToGenerate = G4Poisson(number[ii]); // For now the number according to Poisson
    if (IsStr[ii]==1) numToGenerate = 1;
    REAL_SumB = REAL_SumB + n_B[ii]*numToGenerate;
    for (Int_t inum=0;inum<numToGenerate;inum++) {
      if (IsStr[ii]==0) n_out++;
      double P_com = f1->GetRandom();      
      double phi_com = TMath::Pi() * 2 * G4UniformRand();
      double cosTheta_com = 2 * ( G4UniformRand() - 0.50);
      double theta_com = acos(cosTheta_com);
      if (verboseLevel>1){
	cout << inum << " momentum is " << P_com << endl;
	cout << "  phi is " << phi_com << endl;  
	cout << "  theta is " << theta_com << endl;
      }
      double E_com = sqrt(P_com*P_com + mass[ii]*mass[ii]);
      REAL_SumE = REAL_SumE + E_com;

      TLorentzVector *FourP_com = new TLorentzVector(P_com*sin(theta_com)*cos(phi_com),P_com*sin(theta_com)*sin(phi_com) , P_com*cos(theta_com), E_com);
      TLorentzVector FourP_boosted = (*FourP_com);
      FourP_boosted.Boost(BetaCOMx,BetaCOMy,BetaCOMz);
      if (verboseLevel>1) cout << "    4mom is " << FourP_com->Px() << " " << FourP_com->Py() << " " <<  FourP_com->Pz() << " "  <<  FourP_com->E() << endl; 
      if (verboseLevel>0) cout << "lab 4mom is " << FourP_boosted.Px() << " " << FourP_boosted.Py() << " " <<  FourP_boosted.Pz() << " "  <<  FourP_boosted.E() << endl; 
      if (IsStr[ii]==1) {
	StrMom_out[0] = FourP_boosted.E();
	StrMom_out[1] = FourP_boosted.Px();
	StrMom_out[2] = FourP_boosted.Py();
	StrMom_out[3] = FourP_boosted.Pz();
      }

      if (IsStr[ii]==0) {
        if(n_out>=max_out){
         static int maxp=0;
         if(maxp++<100)cerr<<"StrHP::Thermal-E-TooManySecondaries "<<GEANT_ID[ii]<<" will be discarded "<<endl;
         n_out--;
        }
	mom_out[n_out-1][0] = FourP_boosted.E();
	mom_out[n_out-1][1] = FourP_boosted.Px();
	mom_out[n_out-1][2] = FourP_boosted.Py();
	mom_out[n_out-1][3] = FourP_boosted.Pz();
        ID_out[n_out-1]=GEANT_ID[ii];

      }
    }


  }

  if (verboseLevel>1){
    cout <<" AVERAGE " << endl;
    cout << "SumS " << SumS << " c.f." << strA * -0.8 <<  endl;
    cout << "SumB " << SumB << " c.f." << strA << endl;
    cout << "SumI3 " << SumI3 << " c.f " << 0 << endl;
    cout << "SumE " << SumE << " c.f " << M_S+KE << endl; // note we are not accounting for rest mass or B of target nucleus
    cout << "SumM " << SumM << endl;
    cout << "SumK " << SumK << endl;
    cout << "KE " << KE << endl;
    cout << "REAL_sumE " << REAL_SumE << endl;
    cout << "REAL_sumB " << REAL_SumB << endl;
    
    cout << " Temp  " << temperature << endl;
    cout << " mu_B  " << mu_B  << endl;
    cout << " mu_S  " << mu_S  << endl;
    cout << " mu_I3 " << mu_I3 << endl;
  }

}


/*
// use the following routine to test the thermal model output for various input choices
void callTest() {

  double targetA=12;
  double strA_in = 300;
  double strBeta_in[3] = {0.,0.,-0.894};
  double strA_out;
  double strZ_out;
  double strMom_out[4];
  int n_out;
  int ID_out[1000];
  double Mom_out[1000][4];
  Thermal(targetA, strA_in, strBeta_in, strA_out, strZ_out, strMom_out, n_out, ID_out, Mom_out);
  cout << "targetA       " << targetA << endl;
  cout << "strA_in       " << strA_in << endl;
  cout << "strBeta_in    " << strBeta_in[0] << " " << strBeta_in[1] << " " << strBeta_in[2] << endl;
  cout << "strA_out      " << strA_out << endl;
  cout << "strZ_out      " << strZ_out << endl;
  cout << "strMom_out    " << strMom_out[0] << " " << strMom_out[1] << " " << strMom_out[2] << " " << strMom_out[3] << endl;
  cout << "n_out         " << n_out << endl;
  for (Int_t ii=0;ii<n_out;ii++) {
     cout << ii << " ID_out " << ID_out[ii] << endl;
     cout << "  Mom_out " << Mom_out[ii][0] << " " << Mom_out[ii][1] << " " << Mom_out[ii][2] << " " << Mom_out[ii][3] << endl;
    
  }
}
*/
